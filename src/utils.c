#include "../include/utils.h"
#include "../include/config.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <arpa/inet.h>

void log_message(const char *format, ...) {
    FILE *log_file = fopen(LOG_FILE, "a");
    if (!log_file) return;
    
    time_t now = time(NULL);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
    
    fprintf(log_file, "[%s] ", timestamp);
    
    va_list args;
    va_start(args, format);
    vfprintf(log_file, format, args);
    va_end(args);
    
    fprintf(log_file, "\n");
    fclose(log_file);
}

void export_stats(NetworkState *state, PacketQueue queues[3]) {
    FILE *f = fopen(STATS_FILE, "w");
    if (!f) return;
    
    fprintf(f, "{\n");
    fprintf(f, "  \"timestamp\": %ld,\n", time(NULL));
    fprintf(f, "  \"congestion_score\": %.3f,\n", state->congestion_score);
    fprintf(f, "  \"congestion_predicted\": %s,\n", 
            state->congestion_predicted ? "true" : "false");
    fprintf(f, "  \"queues\": {\n");
    fprintf(f, "    \"high\": %d,\n", queues[0].size);
    fprintf(f, "    \"medium\": %d,\n", queues[1].size);
    fprintf(f, "    \"low\": %d\n", queues[2].size);
    fprintf(f, "  },\n");
    fprintf(f, "  \"packets_per_sec\": %.2f,\n", state->packets_per_sec);
    fprintf(f, "  \"bytes_per_sec\": %.2f,\n", state->bytes_per_sec);
    fprintf(f, "  \"total_packets\": %lu,\n", state->total_packets_processed);
    fprintf(f, "  \"total_bytes\": %lu\n", state->total_bytes_processed);
    fprintf(f, "}\n");
    
    fclose(f);
}

uint64_t get_timestamp_us() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}

char* ip_to_string(uint32_t ip) {
    static char buffer[16];
    struct in_addr addr;
    addr.s_addr = htonl(ip);
    strcpy(buffer, inet_ntoa(addr));
    return buffer;
}

uint16_t calculate_checksum(uint16_t *buf, int len) {
    uint32_t sum = 0;
    while (len > 1) {
        sum += *buf++;
        len -= 2;
    }
    if (len == 1) {
        sum += *(uint8_t *)buf;
    }
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    return ~sum;
}
