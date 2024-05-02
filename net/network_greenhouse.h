#ifndef NETWORK_GREENHOUSE_H_
#define NETWORK_GREENHOUSE_H_

#include "net/linkaddr.h"

#define CHANNEL 26

typedef struct {
    linkaddr_t src_addr;
    linkaddr_t dst_addr;
    uint8_t type; // 0 = Routing, 1 = Not Used, 2 = Data 
    int8_t signal_strength;
    char payload[30];
} network_packet_t;

typedef struct {
    linkaddr_t node_addr;
    uint8_t type; // 0 = Gateway, 1 = Sub-Gateway, 2 = Node
    int8_t signal_strength;
} network_node_t;

void set_radio_channel(void);

#endif /* NETWORK_GREENHOUSE_H_ */
