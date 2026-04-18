#ifndef CAPTURE_H
#define CAPTURE_H

#include "types.h"

int capture_init(const char *interface);
Packet* capture_packet();
void capture_cleanup();

#endif
