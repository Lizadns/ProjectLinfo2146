#include "contiki.h"
#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#include "dev/radio.h"
#include <string.h>
#include <stdio.h>
#include "network_greenhouse.h"

static void broadcast_presence(void) {
    network_packet_t packet;
    packet.node_id = linkaddr_node_addr;
    packet.type = 0;
    strcpy(packet.payload, "Sub-Gateway Presence");

    nullnet_buf = (uint8_t *)&packet;
    nullnet_len = sizeof(packet);

    printf("Broadcasting presence\n");

    NETSTACK_NETWORK.output(NULL);
}

static void input_callback(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest) {
    if (len == sizeof(network_packet_t)) {
        network_packet_t packet;
        memcpy(&packet, data, sizeof(packet));

        switch (packet.type)
        {
            case 0:
                if (strcmp(packet.payload, "Node hello") == 0) {
                    printf("Received node hello from %02x:%02x\n", src->u8[0], src->u8[1]);
                    broadcast_presence();
                }
                break;
            case 1:
                printf("Received packet from %02x:%02x with signal strength %d)\n", src->u8[0], src->u8[1], packet.signal_strength);
                break;
            case 2:
                printf("Received packet from %02x:%02x with payload: %s\n", src->u8[0], src->u8[1], packet.payload);
                break;
            default:
                printf("Recieved an unknown packet type from %02x:%02x\n", src->u8[0], src->u8[1]);
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

    etimer_set(&periodic_timer, CLOCK_SECOND * 30);

    while (1) {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
        broadcast_presence();
        etimer_reset(&periodic_timer);
    }

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/