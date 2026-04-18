#ifndef FORWARD_H
#define FORWARD_H

#include "types.h"

int forward_init(const char *interface);
void mark_packet_dscp(Packet *pkt);
int forward_packet(Packet *pkt);
void forward_cleanup();

#endif
