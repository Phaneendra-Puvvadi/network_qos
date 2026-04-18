#ifndef PREDICT_H
#define PREDICT_H

#include "types.h"

void update_network_state(NetworkState *state, Packet *pkt);
double calculate_congestion_score(NetworkState *state);
bool predict_congestion(NetworkState *state);
void adjust_priority_for_congestion(Packet *pkt, NetworkState *state);

#endif
