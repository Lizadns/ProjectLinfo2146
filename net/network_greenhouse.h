#ifndef NETWORK_GREENHOUSE_H_
#define NETWORK_GREENHOUSE_H_

#include "net/linkaddr.h"

#define CHANNEL 26

static const linkaddr_t multicast_addr = {{0xFF, 0xFF}};

typedef struct {
    linkaddr_t src_addr;
    linkaddr_t dst_addr;
    uint8_t src_type;
    uint8_t dst_type;
    uint8_t type; // 0 = Routing, 1 = Not Used, 2 = Data
    int8_t signal_strength;
    char payload[64];
} network_packet_t;

typedef struct {
    linkaddr_t node_addr;
    uint8_t type; // 0 = Gateway, 1 = Sub-Gateway, 2 = Light Sensor, 3 = Irrigation System, 4 = Light Bulb, 5= Mobile Terminal
    int8_t signal_strength;
} network_node_t;

/***
    Set the radio channel
*/
void set_radio_channel(void);

/***
    Assign a parent to the node
        @param packet The received packet
        @param parent The parent node
        @param has_parent A flag indicating if the node has a parent
        @param node_type The type of the node
*/
void assign_parent(network_packet_t packet, network_node_t* parent, uint8_t* has_parent, uint8_t node_type,uint8_t gateway_type);

/***
    Leave the parent node
        @param parent The parent node
*/
void leave_parent(network_node_t parent);

/***
    Assign children to the node
        @param packet The received packet
        @param children_nodes The list of children nodes
        @param children_nodes_count The number of children nodes
*/
void assign_child(network_packet_t packet, network_node_t* children_nodes, int* children_nodes_count);

/***
    Unassign a child from the node
        @param child_packet The received packet
        @param children_nodes The list of children nodes
        @param children_nodes_count The number of children nodes
*/
void unassign_child(network_packet_t child_packet, network_node_t* children_nodes, int* children_nodes_count);

/***
    Send a Node Hello packet to the network
        @param node_type The type of the node sending the packet
*/
void send_node_hello(uint8_t node_type);

/***
    Send a Node Hello Response to the source of the received packet
        @param packet The received packet
        @param node_type The type of the node sending the response
*/
void send_node_hello_response(network_packet_t packet, uint8_t node_type);

linkaddr_t convert_to_linkaddr(uint16_t number);

#endif /* NETWORK_GREENHOUSE_H_ */
