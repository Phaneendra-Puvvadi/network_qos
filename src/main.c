#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "../include/types.h"
#include "../include/config.h"
#include "../include/capture.h"
#include "../include/classify.h"
#include "../include/predict.h"
#include "../include/queue.h"
#include "../include/forward.h"
#include "../include/utils.h"

static volatile int keep_running = 1;
static PacketQueue queues[3];
static NetworkState state = {0};
static pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;

void signal_handler(int sig) {
    keep_running = 0;
}

void* forwarding_thread(void* arg) {
    int idle_iterations = 0;

    while (keep_running) {
        pthread_mutex_lock(&queue_mutex);

        int total_size = queues[0].size + queues[1].size + queues[2].size;

        if (total_size > 0) {

            // 🔥 Adaptive draining based on backlog
            int total = total_size;

            int high_limit   = (total > 30) ? 4 : (total > 10 ? 2 : 1);
            int medium_limit = (total > 30) ? 3 : (total > 10 ? 2 : 1);
            int low_limit    = (total > 30) ? 2 : 1;

            // 🔴 High priority
            for (int i = 0; i < high_limit && queues[0].size > 0; i++) {
                Packet *p = queue_dequeue(&queues[0]);
                if (p) {
                    forward_packet(p);
                    free(p->data);
                    free(p);
                }
            }

            // 🟡 Medium priority
            for (int i = 0; i < medium_limit && queues[1].size > 0; i++) {
                Packet *p = queue_dequeue(&queues[1]);
                if (p) {
                    forward_packet(p);
                    free(p->data);
                    free(p);
                }
            }

            // 🟢 Low priority
            for (int i = 0; i < low_limit && queues[2].size > 0; i++) {
                Packet *p = queue_dequeue(&queues[2]);
                if (p) {
                    forward_packet(p);
                    free(p->data);
                    free(p);
                }
            }

            idle_iterations = 0;

        } else {
            idle_iterations++;
        }

        // Update state
        state.queue_size[0] = queues[0].size;
        state.queue_size[1] = queues[1].size;
        state.queue_size[2] = queues[2].size;
        state.total_queue_size = queues[0].size + queues[1].size + queues[2].size;

        pthread_mutex_unlock(&queue_mutex);

        // Reset congestion when idle
        if (idle_iterations > 20) {
            state.congestion_score = 0.0;
            state.congestion_predicted = false;
        }

        // ⚡ Balanced speed (important)
        usleep(3500);
    }

    // ✅ Force drain EVERYTHING on shutdown
    printf("\nDraining queues on shutdown...\n");
    pthread_mutex_lock(&queue_mutex);
    for (int i = 0; i < 3; i++) {
        while (queues[i].size > 0) {
            Packet *p = queue_dequeue(&queues[i]);
            if (p) {
                forward_packet(p);
                free(p->data);
                free(p);
            }
        }
    }
    pthread_mutex_unlock(&queue_mutex);

    return NULL;
}

int main(int argc, char *argv[]) {
    if (getuid() != 0) {
        fprintf(stderr, "This program must be run as root\n");
        return 1;
    }
    
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    log_message("=== Network QoS Router Starting ===");
    
    if (capture_init(INPUT_INTERFACE) < 0) {
        fprintf(stderr, "Failed to initialize capture\n");
        return 1;
    }
    
    if (forward_init(OUTPUT_INTERFACE) < 0) {
        fprintf(stderr, "Failed to initialize forwarding\n");
        capture_cleanup();
        return 1;
    }
    
    queue_init(&queues[0], MAX_QUEUE_SIZE);
    queue_init(&queues[1], MAX_QUEUE_SIZE);
    queue_init(&queues[2], MAX_QUEUE_SIZE);
    
    uint64_t last_export_time = 0;
    uint64_t packet_count = 0;
    
    pthread_t fwd_thread;
    pthread_create(&fwd_thread, NULL, forwarding_thread, NULL);
    
    printf("Router running. Press Ctrl+C to stop.\n");
    log_message("Router initialized successfully");
    
    while (keep_running) {
        Packet *pkt = capture_packet();
        
        if (pkt != NULL) {
            packet_count++;
            
            assign_priority(pkt);
            update_network_state(&state, pkt);
            predict_congestion(&state);
            adjust_priority_for_congestion(pkt, &state);
            
            int queue_idx = pkt->priority - 1;
            if (queue_idx < 0) queue_idx = 0;
            if (queue_idx > 2) queue_idx = 2;
            
            pthread_mutex_lock(&queue_mutex);
            queue_enqueue(&queues[queue_idx], pkt);
            pthread_mutex_unlock(&queue_mutex);
        }
        
        uint64_t current_time = get_timestamp_us();
        if (current_time - last_export_time >= 100000) {
            export_stats(&state, queues);
            last_export_time = current_time;
        }
        
        if (pkt == NULL) {
            usleep(1000);
        }
    }
    
    log_message("Router shutting down");
    printf("\nShutting down...\n");
    
    pthread_join(fwd_thread, NULL);
    
    queue_cleanup(&queues[0]);
    queue_cleanup(&queues[1]);
    queue_cleanup(&queues[2]);
    
    capture_cleanup();
    forward_cleanup();
    
    log_message("=== Router Stopped ===");
    printf("Router stopped. Processed %lu packets.\n", packet_count);
    
    return 0;
}
