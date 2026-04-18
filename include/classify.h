#ifndef CLASSIFY_H
#define CLASSIFY_H

#include "types.h"

TrafficType classify_packet(Packet *pkt);
void assign_priority(Packet *pkt);

#endif
