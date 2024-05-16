#include "network_greenhouse.h"
#include "net/netstack.h"
#include "net/packetbuf.h"
#include "net/nullnet/nullnet.h"
#include "dev/radio.h"
#include <stdio.h>

void set_radio_channel(void) {
    radio_value_t channel = CHANNEL;
    NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, channel);
}

void assign_parent(network_packet_t parent_hello, network_node_t* parent, uint8_t* has_parent, uint8_t node_type, uint8_t gateway_type) {
    int8_t rssi = packetbuf_attr(PACKETBUF_ATTR_RSSI);

    printf("#Routing# New node found: %02x:%02x, RSSI: %d dBm -> ", parent_hello.src_addr.u8[0], parent_hello.src_addr.u8[1], rssi);

    if (!*has_parent || parent->type != gateway_type || rssi > parent->signal_strength) {
        if (parent->type == gateway_type && parent_hello.src_type != gateway_type) {
            printf("Ignored\n");
            return;
        }

        if (*has_parent) {
            leave_parent(*parent);
        }

        parent->node_addr = parent_hello.src_addr;
        parent->type = parent_hello.src_type;
        parent->signal_strength = rssi;

        *has_parent = 1;

        printf("Parent (type: %d) address updated: %02x:%02x\n", parent->type, parent->node_addr.u8[0], parent->node_addr.u8[1]);

        network_packet_t response = {
        .src_addr = linkaddr_node_addr,
        .src_type = node_type,
        .dst_addr = parent_hello.src_addr,
        .dst_type = parent_hello.src_type,
        .type = 0,
        .payload = "Parent Join"
        };

        nullnet_buf = (uint8_t *)&response;
        nullnet_len = sizeof(response);

        NETSTACK_NETWORK.output(&parent_hello.src_addr);
    } else {
        printf("Ignored\n");
    }
}

void leave_parent(network_node_t parent) {
    network_packet_t leave = {
        .src_addr = linkaddr_node_addr,
        .src_type = 0,
        .dst_addr = parent.node_addr,
        .dst_type = parent.type,
        .type = 0,
        .payload = "Parent Leave"
    };

    nullnet_buf = (uint8_t *)&leave;
    nullnet_len = sizeof(leave);

    NETSTACK_NETWORK.output(&parent.node_addr);
}

void assign_child(network_packet_t child_packet, network_node_t* children_nodes, int* children_nodes_count) {
    children_nodes[*children_nodes_count].node_addr = child_packet.src_addr;
    children_nodes[*children_nodes_count].type = child_packet.src_type;
    (*children_nodes_count)++;

    printf("#Routing# Children list changed:");
    for (int i = 0; i < *children_nodes_count; i++) {
        printf(" %02x:%02x (type: %d)", children_nodes[i].node_addr.u8[0], children_nodes[i].node_addr.u8[1], children_nodes[i].type);
    }
    printf("\n");
}

void unassign_child(network_packet_t child_packet, network_node_t* children_nodes, int* children_nodes_count) {
    linkaddr_t child_addr = child_packet.src_addr;
    for (int i = 0; i < *children_nodes_count; i++) {
        if (linkaddr_cmp(&children_nodes[i].node_addr, &child_addr)) {
            for (int j = i; j < *children_nodes_count - 1; j++) {
                children_nodes[j] = children_nodes[j + 1];
            }
            (*children_nodes_count)--;
            break;
        }
    }

    printf("#Routing# Children list changed:");
    for (int i = 0; i < *children_nodes_count; i++) {
        printf(" %02x:%02x (type: %d)", children_nodes[i].node_addr.u8[0], children_nodes[i].node_addr.u8[1], children_nodes[i].type);
    }
    printf("\n");
}

void send_node_hello(uint8_t node_type) {
    network_packet_t packet = {
        .src_addr = linkaddr_node_addr,
        .src_type = node_type,
        .type = 0,
        .signal_strength = 0,
        .payload = "Node Hello"
    };

    nullnet_buf = (uint8_t *)&packet;
    nullnet_len = sizeof(packet);

    printf("#Network# Broadcasting Node Hello\n");

    NETSTACK_NETWORK.output(NULL);
}

void send_node_hello_response(network_packet_t packet, uint8_t node_type) {
    network_packet_t response = {
        .src_addr = linkaddr_node_addr,
        .src_type = node_type,
        .dst_addr = packet.src_addr,
        .dst_type = packet.src_type,
        .type = 0,
        .payload = "Node Hello Response"
    };

    nullnet_buf = (uint8_t *)&response;
    nullnet_len = sizeof(response);

    printf("#Network# Sending Node Hello Response to %02x:%02x\n", packet.src_addr.u8[0], packet.src_addr.u8[1]);

    NETSTACK_NETWORK.output(&packet.src_addr);
}

linkaddr_t convert_to_linkaddr(uint16_t number) {
    linkaddr_t addr;
    // Nœud haut
    addr.u8[0] = (uint8_t)(number & 0xFF);
    // Nœud bas
    addr.u8[1] = 0x00;
    return addr;
}