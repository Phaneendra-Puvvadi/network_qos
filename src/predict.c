#include "../include/predict.h"
#include "../include/config.h"
#include "../include/utils.h"
#include <math.h>

void update_network_state(NetworkState *state, Packet *pkt) {
    if (state->last_update_time == 0) {
        state->last_update_time = pkt->timestamp;
        state->avg_packet_size = pkt->length;
        state->packets_per_sec = 0;
        state->bytes_per_sec = 0;
        state->avg_interarrival_time = 1.0;
        return;
    }
    
    uint64_t time_diff = pkt->timestamp - state->last_update_time;
    if (time_diff == 0) time_diff = 1;
    
    double inst_pps = 1000000.0 / time_diff;
    double inst_interarrival = time_diff / 1000.0;
    
    state->packets_per_sec = EWMA_ALPHA * inst_pps + 
                             (1 - EWMA_ALPHA) * state->packets_per_sec;
    
    state->avg_interarrival_time = EWMA_ALPHA * inst_interarrival +
                                   (1 - EWMA_ALPHA) * state->avg_interarrival_time;
    
    state->avg_packet_size = EWMA_ALPHA * pkt->length +
                             (1 - EWMA_ALPHA) * state->avg_packet_size;
    
    state->bytes_per_sec = state->packets_per_sec * state->avg_packet_size;
    
    state->last_update_time = pkt->timestamp;
    state->total_packets_processed++;
    state->total_bytes_processed += pkt->length;
}

double calculate_congestion_score(NetworkState *state) {
    double link_capacity_bps = LINK_CAPACITY_BPS;
    
    double utilization = (state->bytes_per_sec * 8.0) / link_capacity_bps;
    if (utilization > 1.0) utilization = 1.0;
    if (utilization < 0.0) utilization = 0.0;
    
    double queue_occupancy = state->total_queue_size / (double)(MAX_QUEUE_SIZE * 3);
    if (queue_occupancy > 1.0) queue_occupancy = 1.0;
    if (queue_occupancy < 0.0) queue_occupancy = 0.0;
    
    double interarrival_factor = 0.0;
    if (state->avg_interarrival_time > 0) {
        interarrival_factor = 10.0 / state->avg_interarrival_time;
        if (interarrival_factor > 1.0) interarrival_factor = 1.0;
    }
    
    double score = 0.5 * utilization +
                   0.3 * queue_occupancy +
                   0.2 * interarrival_factor;
    
    return score;
}

bool predict_congestion(NetworkState *state) {
    state->congestion_score = calculate_congestion_score(state);
    
    if (state->congestion_score > CONGESTION_THRESHOLD) {
        state->congestion_predicted = true;
        return true;
    }
    
    state->congestion_predicted = false;
    return false;
}

void adjust_priority_for_congestion(Packet *pkt, NetworkState *state) {
    if (!state->congestion_predicted) return;
    
    if (pkt->priority == TRAFFIC_INTERACTIVE) {
        // Slight demotion during congestion
    }
    else if (pkt->priority == TRAFFIC_BULK) {
        // Heavy demotion during congestion
    }
}
