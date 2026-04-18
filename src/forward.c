#include "../include/forward.h"
#include "../include/utils.h"
#include "../include/config.h"
#include <pcap.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>

static pcap_t *output_handle = NULL;

int forward_init(const char *interface) {
    char errbuf[PCAP_ERRBUF_SIZE];
    
    output_handle = pcap_open_live(interface, MAX_PACKET_SIZE, 1, 1000, errbuf);
    if (output_handle == NULL) {
        fprintf(stderr, "Error opening output device %s: %s\n", interface, errbuf);
        return -1;
    }
    
    log_message("Forward initialized on %s", interface);
    return 0;
}

void mark_packet_dscp(Packet *pkt) {
    if (pkt->data_len < sizeof(struct ether_header) + sizeof(struct ip)) {
        return;
    }
    
    struct ip *ip_hdr = (struct ip *)(pkt->data + sizeof(struct ether_header));
    
    uint8_t dscp_value;
    switch(pkt->priority) {
        case 3:
            dscp_value = 46 << 2;
            break;
        case 2:
            dscp_value = 34 << 2;
            break;
        case 1:
            dscp_value = 10 << 2;
            break;
        default:
            dscp_value = 0;
    }
    
    ip_hdr->ip_tos = dscp_value;
    
    ip_hdr->ip_sum = 0;
    ip_hdr->ip_sum = calculate_checksum((uint16_t *)ip_hdr, ip_hdr->ip_hl * 4);
}

int forward_packet(Packet *pkt) {
    if (output_handle == NULL) {
        return -1;
    }
    
    mark_packet_dscp(pkt);
    
    if (pcap_sendpacket(output_handle, pkt->data, pkt->data_len) != 0) {
        log_message("Error forwarding packet");
        return -1;
    }
    
    return 0;
}

void forward_cleanup() {
    if (output_handle) {
        pcap_close(output_handle);
        output_handle = NULL;
    }
}
