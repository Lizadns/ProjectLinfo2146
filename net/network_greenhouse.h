#ifndef NETWORK_GREENHOUSE_H_
#define NETWORK_GREENHOUSE_H_

#include "net/linkaddr.h"

#define CHANNEL 26

typedef struct {
    linkaddr_t node_id;
    uint8_t type;
    int8_t signal_strength;
    char payload[30];
} network_packet_t;

void set_radio_channel(void);

#endif /* NETWORK_GREENHOUSE_H_ */
