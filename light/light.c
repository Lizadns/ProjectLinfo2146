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
#include "dev/leds.h"

#define NODE_TYPE 4
#define LIGHTING_TIME 30

static process_event_t event_data_ready;

static network_node_t parent;
static uint8_t has_parent = 0;

static network_node_t children_nodes[20];
static int children_nodes_count = 0;

PROCESS(light, "Light");
/*---------------------------------------------------------------------------*/
void input_callback(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest) {
  if (len == sizeof(network_packet_t)) {
    network_packet_t packet;
    memcpy(&packet, data, sizeof(packet));

    switch (packet.type)
    {
      case 0:
        if (strcmp(packet.payload, "Node Hello") == 0) {
          if (packet.src_type == 1) {
            assign_parent(packet, &parent, &has_parent, NODE_TYPE,1);
          } else {
            send_node_hello_response(packet, NODE_TYPE);
          }
        } else if (strcmp(packet.payload, "Node Hello Response") == 0) {
          assign_parent(packet, &parent, &has_parent, NODE_TYPE,1);
        } else if (strcmp(packet.payload, "Parent Join") == 0) {
          assign_child(packet, children_nodes, &children_nodes_count);
        } else if (strcmp(packet.payload, "Parent Leave") == 0) {
          unassign_child(packet, children_nodes, &children_nodes_count);
        }
        break;
      
      case 2:
        if (packet.dst_type == NODE_TYPE) {
          if (strcmp(packet.payload, "Turn on the light") == 0) {
            process_post(&light, event_data_ready, NULL);
          }
        } else {
          if (has_parent) {
            nullnet_buf = (uint8_t *)&packet;
            nullnet_len = sizeof(packet);

            printf("#Network# Forwarding packet to %02x:%02x\n", parent.node_addr.u8[0], parent.node_addr.u8[1]);
            NETSTACK_NETWORK.output(&parent.node_addr);
          }
        }
        break;

      default:
        break;
    }
  }
}
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
AUTOSTART_PROCESSES(&light);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(light, ev, data)
{   
  static struct etimer report_timer;
  PROCESS_BEGIN();

  event_data_ready = process_alloc_event();
  set_radio_channel();
  nullnet_set_input_callback(input_callback);
  send_node_hello(NODE_TYPE);

  while(1) {
    PROCESS_WAIT_EVENT();
    
    if(ev == event_data_ready) {
        printf("#Operation# Light : Start\n");
	    leds_toggle(LEDS_ALL);
        etimer_set(&report_timer, CLOCK_SECOND * LIGHTING_TIME);
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&report_timer));
        printf("#Operation# Light : End\n");
	    leds_off(LEDS_ALL);
        etimer_reset(&report_timer);
    }
  }		
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/