#include "contiki.h"
#include <stdio.h>
#include "net/netstack.h"
#include "net/packetbuf.h"
#include "net/nullnet/nullnet.h"
#include "lib/random.h"
#include "dev/radio.h"
#include <string.h>
#include "network_greenhouse.h"
#include "sys/process.h"

#define NODE_TYPE 5

#define BROADCAST_DELAY (CLOCK_SECOND * 30)
#define HELLO_TIMEOUT (CLOCK_SECOND * 60) // 1 minute

static network_node_t subgateway_id;
static uint8_t is_in_greenhouse = 0;
static uint8_t number_check_up = 3;
static struct etimer hello_timeout_timer;

PROCESS(mobile_terminal, "mobileTerminal");

/*---------------------------------------------------------------------------*/
void input_callback(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest) {
    if (len == sizeof(network_packet_t)) {
        network_packet_t subgateway_hello;
        memcpy(&subgateway_hello, data, sizeof(subgateway_hello));

        switch (subgateway_hello.type) {
            case 0:
                if (strcmp(subgateway_hello.payload, "Node Hello") == 0) {
                    if (subgateway_hello.src_type == 1) {
                        etimer_reset(&hello_timeout_timer);// Restart the hello timeout timer
                        if (!linkaddr_cmp(&subgateway_hello.src_addr, &subgateway_id.node_addr)) {
                            subgateway_id.node_addr = subgateway_hello.src_addr;
                            is_in_greenhouse = 1;
                            number_check_up = 3;
                            
                        }
                    }
                }
                break;
            case 2:
                if (strcmp(subgateway_hello.payload, "Check up response from the light sensor") == 0) {
                    printf("Received 'Check up response from the light sensor' from %02x:%02x\n", subgateway_hello.src_addr.u8[0], subgateway_hello.src_addr.u8[1]);
                }
                break;
            default:
                break;
        }
    }
}

/*---------------------------------------------------------------------------*/
AUTOSTART_PROCESSES(&mobile_terminal);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(mobile_terminal, ev, data) {
    static struct etimer periodic_timer;
    

    PROCESS_BEGIN();

    set_radio_channel();

    nullnet_set_input_callback(input_callback);

    send_node_hello(NODE_TYPE, 1);

    etimer_set(&periodic_timer, BROADCAST_DELAY);
    etimer_set(&hello_timeout_timer, HELLO_TIMEOUT);
    

    while (1) {
        PROCESS_WAIT_EVENT();

        if (ev == PROCESS_EVENT_TIMER) {
            if (data == &periodic_timer) {
                send_node_hello(NODE_TYPE, 1);
                etimer_reset(&periodic_timer);
            } else if (data == &hello_timeout_timer) {
                printf("No 'Node Hello' received from src_type 1 in the last minute. Resetting subgateway_id.\n");
                memset(&subgateway_id, 0, sizeof(subgateway_id)); // Reset subgateway_id
                is_in_greenhouse = 0; // Reset the flag
                etimer_reset(&hello_timeout_timer);
            }
        }

        if (number_check_up != 0 && is_in_greenhouse) {
            network_packet_t packet = {
                .src_addr = linkaddr_node_addr,
                .src_type = NODE_TYPE,
                .dst_addr = multicast_addr,
                .dst_type = 2,
                .type = 2,
                .distance_to_gateway = 1,
                .payload = "Check up from the mobile terminal"
            };

            nullnet_buf = (uint8_t *)&packet;
            nullnet_len = sizeof(packet);

            printf("#Network# Sending 'Check up from the mobile terminal' to %02x:%02x\n", subgateway_id.node_addr.u8[0], subgateway_id.node_addr.u8[1]);

            NETSTACK_NETWORK.output(&subgateway_id.node_addr);
            number_check_up = number_check_up - 1;
        }
    }

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
