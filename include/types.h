#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <sys/time.h>

#define MAX_PACKET_SIZE 65535
#define MAX_FLOWS 10000

typedef enum {
    TRAFFIC_REALTIME = 3,
    TRAFFIC_INTERACTIVE = 2,
    TRAFFIC_BULK = 1
} TrafficType;

typedef struct {
    uint32_t src_ip;
    uint32_t dst_ip;
    uint16_t src_port;
    uint16_t dst_port;
    uint8_t protocol;
    uint16_t length;
    uint64_t timestamp;
    uint8_t priority;
    uint8_t *data;
    int data_len;
} Packet;

typedef struct {
    uint32_t src_ip;
    uint32_t dst_ip;
    uint16_t src_port;
    uint16_t dst_port;
    uint8_t protocol;
    uint64_t packet_count;
    uint64_t byte_count;
    uint64_t first_seen;
    uint64_t last_seen;
    TrafficType type;
} Flow;

typedef struct {
    double packets_per_sec;
    double bytes_per_sec;
    double avg_packet_size;
    double avg_interarrival_time;
    int queue_size[3];
    int total_queue_size;
    double congestion_score;
    bool congestion_predicted;
    uint64_t last_update_time;
    uint64_t total_packets_processed;
    uint64_t total_bytes_processed;
} NetworkState;

typedef struct QueueNode {
    Packet *packet;
    struct QueueNode *next;
} QueueNode;

typedef struct {
    QueueNode *head;
    QueueNode *tail;
    int size;
    int max_size;
} PacketQueue;

#endif
