#include "contiki.h"
#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#include "dev/radio.h"
#include <string.h>
#include <stdio.h>
#include "network_greenhouse.h"

#define BROADCAST_DELAY (CLOCK_SECOND * 30)

#define NODE_TYPE 1

static network_node_t parent;
static uint8_t has_parent = 0;

static network_node_t children_nodes[20];
static int children_nodes_count = 0;

static void input_callback(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest) {
    if (len == sizeof(network_packet_t)) {
        network_packet_t packet;
        memcpy(&packet, data, sizeof(packet));

        switch (packet.type)
        {
            case 0:
                if (strcmp(packet.payload, "Node Hello") == 0) {
                    if (packet.src_type == 0) {
                        assign_parent(packet, &parent, &has_parent, NODE_TYPE, 0);
                    } else {
                        send_node_hello_response(packet, 1);
                    }
                } else if (strcmp(packet.payload, "Parent Join") == 0) {
                    assign_child(packet, children_nodes, &children_nodes_count);
                } else if (strcmp(packet.payload, "Parent Leave") == 0) {
                    unassign_child(packet, children_nodes, &children_nodes_count);
                }
                break;
            case 1:
                printf("Received packet from %02x:%02x with signal strength %d \n", packet.src_addr.u8[0], packet.src_addr.u8[1], packet.signal_strength);
                break;
            case 2:
                if (linkaddr_cmp(&packet.dst_addr, &linkaddr_node_addr)) {
                    printf("Received packet from %02x:%02x with payload: %s\n", packet.src_addr.u8[0], packet.src_addr.u8[1], packet.payload);
                } else if(linkaddr_cmp(&packet.dst_addr, &linkaddr_null)){
                    nullnet_buf = (uint8_t *)&packet;
                    nullnet_len = sizeof(packet);

                    printf("#Network# Forwarding packet to %02x:%02x with payload: %s\n", parent.node_addr.u8[0], parent.node_addr.u8[1], packet.payload);

                    NETSTACK_NETWORK.output(&parent.node_addr);
                } else if(linkaddr_cmp(&packet.dst_addr, &multicast_addr)){
                    nullnet_buf = (uint8_t *)&packet;
                    nullnet_len = sizeof(packet);

                    printf("#Network# Multi-casting packet to children nodes of type %d with payload: %s\n", packet.dst_type, packet.payload);

                    for (int i = 0; i < children_nodes_count; i++) {
                        if (children_nodes[i].type == packet.dst_type) {
                            NETSTACK_NETWORK.output(&children_nodes[i].node_addr);
                        }
                    }
                }
                break;
            default:
                printf("Recieved an unknown packet type from %02x:%02x\n", packet.src_addr.u8[0], packet.src_addr.u8[1]);
                break;
        }
    }
}

/*---------------------------------------------------------------------------*/
PROCESS(sub_gateway, "Sub-Gateway");
AUTOSTART_PROCESSES(&sub_gateway);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(sub_gateway, ev, data) {
    static struct etimer periodic_timer;

    PROCESS_BEGIN();

    set_radio_channel();
    nullnet_set_input_callback(input_callback);

    etimer_set(&periodic_timer, BROADCAST_DELAY);

    while (1) {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
        send_node_hello(1);
        etimer_reset(&periodic_timer);
    }

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/