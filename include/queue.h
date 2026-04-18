#ifndef QUEUE_H
#define QUEUE_H

#include "types.h"

void queue_init(PacketQueue *queue, int max_size);
int queue_enqueue(PacketQueue *queue, Packet *pkt);
Packet* queue_dequeue(PacketQueue *queue);
void queue_cleanup(PacketQueue *queue);

#endif
