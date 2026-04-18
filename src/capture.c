#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>
#include "../include/capture.h"
#include "../include/utils.h"

static pcap_t *handle = NULL;

int capture_init(const char *interface) {
    char errbuf[PCAP_ERRBUF_SIZE];
    
    handle = pcap_open_live(interface, MAX_PACKET_SIZE, 1, 1000, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "Error opening device %s: %s\n", interface, errbuf);
        return -1;
    }
    
    struct bpf_program fp;
    char filter_exp[] = "ip";
    
    if (pcap_compile(handle, &fp, filter_exp, 0, PCAP_NETMASK_UNKNOWN) == -1) {
        fprintf(stderr, "Error compiling filter\n");
        return -1;
    }
    
    if (pcap_setfilter(handle, &fp) == -1) {
        fprintf(stderr, "Error setting filter\n");
        return -1;
    }
    
    log_message("Packet capture initialized on %s", interface);
    return 0;
}

Packet* parse_packet(const u_char *packet_data, int packet_len) {
    Packet *pkt = (Packet *)malloc(sizeof(Packet));
    if (!pkt) return NULL;
    
    pkt->timestamp = get_timestamp_us();
    
    struct ether_header *eth = (struct ether_header *)packet_data;
    if (ntohs(eth->ether_type) != ETHERTYPE_IP) {
        free(pkt);
        return NULL;
    }
    
    struct ip *ip_hdr = (struct ip *)(packet_data + sizeof(struct ether_header));
    pkt->src_ip = ntohl(ip_hdr->ip_src.s_addr);
    pkt->dst_ip = ntohl(ip_hdr->ip_dst.s_addr);
    pkt->protocol = ip_hdr->ip_p;
    pkt->length = ntohs(ip_hdr->ip_len);
    
    int ip_header_len = ip_hdr->ip_hl * 4;
    
    if (pkt->protocol == IPPROTO_TCP) {
        struct tcphdr *tcp_hdr = (struct tcphdr *)((uint8_t *)ip_hdr + ip_header_len);
        pkt->src_port = ntohs(tcp_hdr->th_sport);
        pkt->dst_port = ntohs(tcp_hdr->th_dport);
    } 
    else if (pkt->protocol == IPPROTO_UDP) {
        struct udphdr *udp_hdr = (struct udphdr *)((uint8_t *)ip_hdr + ip_header_len);
        pkt->src_port = ntohs(udp_hdr->uh_sport);
        pkt->dst_port = ntohs(udp_hdr->uh_dport);
    }
    else {
        pkt->src_port = 0;
        pkt->dst_port = 0;
    }
    
    pkt->data = (uint8_t *)malloc(packet_len);
    memcpy(pkt->data, packet_data, packet_len);
    pkt->data_len = packet_len;
    pkt->priority = 0;
    
    return pkt;
}

Packet* capture_packet() {
    struct pcap_pkthdr header;
    const u_char *packet_data;
    
    packet_data = pcap_next(handle, &header);
    if (packet_data == NULL) {
        return NULL;
    }
    
    return parse_packet(packet_data, header.len);
}

void capture_cleanup() {
    if (handle) {
        pcap_close(handle);
        handle = NULL;
    }
}
