#ifndef UTILS_H
#define UTILS_H

#include "types.h"
#include <stdio.h>

void log_message(const char *format, ...);
void export_stats(NetworkState *state, PacketQueue queues[3]);
uint64_t get_timestamp_us();
char* ip_to_string(uint32_t ip);
uint16_t calculate_checksum(uint16_t *buf, int len);

#endif
