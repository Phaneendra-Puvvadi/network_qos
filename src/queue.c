#include "../include/queue.h"
#include "../include/utils.h"
#include <stdlib.h>

void queue_init(PacketQueue *queue, int max_size) {
    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;
    queue->max_size = max_size;
}

int queue_enqueue(PacketQueue *queue, Packet *pkt) {
    if (queue->size >= queue->max_size) {
        log_message("Queue full, dropping packet");
        free(pkt->data);
        free(pkt);
        return -1;
    }
    
    QueueNode *node = (QueueNode *)malloc(sizeof(QueueNode));
    if (!node) {
        free(pkt->data);
        free(pkt);
        return -1;
    }
    
    node->packet = pkt;
    node->next = NULL;
    
    if (queue->tail == NULL) {
        queue->head = node;
        queue->tail = node;
    } else {
        queue->tail->next = node;
        queue->tail = node;
    }
    
    queue->size++;
    return 0;
}

Packet* queue_dequeue(PacketQueue *queue) {
    if (queue->head == NULL) {
        return NULL;
    }
    
    QueueNode *node = queue->head;
    Packet *pkt = node->packet;
    
    queue->head = node->next;
    if (queue->head == NULL) {
        queue->tail = NULL;
    }
    
    queue->size--;
    free(node);
    
    return pkt;
}

void queue_cleanup(PacketQueue *queue) {
    while (queue->head != NULL) {
        Packet *pkt = queue_dequeue(queue);
        if (pkt) {
            free(pkt->data);
            free(pkt);
        }
    }
}
