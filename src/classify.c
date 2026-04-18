#include "../include/classify.h"
#include "../include/utils.h"
#include <netinet/in.h>

TrafficType classify_packet(Packet *pkt) {
    if (pkt->protocol == IPPROTO_UDP) {
        if ((pkt->dst_port >= 16384 && pkt->dst_port <= 32767) ||
            (pkt->src_port >= 16384 && pkt->src_port <= 32767)) {
            return TRAFFIC_REALTIME;
        }
        
        if ((pkt->dst_port >= 8801 && pkt->dst_port <= 8810) ||
            (pkt->src_port >= 8801 && pkt->src_port <= 8810) ||
            pkt->dst_port == 3478 || pkt->src_port == 3478 ||
            pkt->dst_port == 5000 || pkt->src_port == 5000) {
            return TRAFFIC_REALTIME;
        }
        
        if (pkt->length < 500) {
            return TRAFFIC_REALTIME;
        }
    }
    
    if (pkt->protocol == IPPROTO_TCP) {
        if (pkt->dst_port == 80 || pkt->dst_port == 443 ||
            pkt->src_port == 80 || pkt->src_port == 443) {
            return (pkt->length < 1000) ? TRAFFIC_INTERACTIVE : TRAFFIC_BULK;
        }
        
        if (pkt->dst_port == 22 || pkt->src_port == 22) {
            return TRAFFIC_INTERACTIVE;
        }
        
        if (pkt->dst_port == 21 || pkt->src_port == 21 ||
            pkt->dst_port == 20 || pkt->src_port == 20) {
            return TRAFFIC_BULK;
        }
        
        if (pkt->length > 1400) {
            return TRAFFIC_BULK;
        }
    }
    
    return TRAFFIC_INTERACTIVE;
}

void assign_priority(Packet *pkt) {
    TrafficType type = classify_packet(pkt);
    pkt->priority = (uint8_t)type;
}
