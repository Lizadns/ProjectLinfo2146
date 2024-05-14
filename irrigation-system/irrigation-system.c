#include "contiki.h"
#include "dev/leds.h"
#include "dev/button-sensor.h"
#include <stdio.h> 

#include "net/netstack.h"
#include "net/packetbuf.h"
#include "net/nullnet/nullnet.h"
#include "lib/random.h"
#include "dev/radio.h"
#include <string.h>
#include "network_greenhouse.h"

#define NODE_TYPE 3

static network_node_t parent;
static uint8_t has_parent = 0;

static network_node_t children_nodes[20];
static int children_nodes_count = 0;

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
        if (has_parent) {
          nullnet_buf = (uint8_t *)&packet;
          nullnet_len = sizeof(packet);

          printf("#Network# Forwarding packet to %02x:%02x\n", parent.node_addr.u8[0], parent.node_addr.u8[1]);
          NETSTACK_NETWORK.output(&parent.node_addr);
        }

      default:
        break;
    }
  }
}

/*---------------------------------------------------------------------------*/
PROCESS(irrigation_system, "Irrigation system with timer");
AUTOSTART_PROCESSES(&irrigation_system);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(irrigation_system, ev, data)
{   static struct etimer report_timer;
    PROCESS_BEGIN();

    set_radio_channel();
    nullnet_set_input_callback(input_callback);
    send_node_hello(NODE_TYPE);
    
    SENSORS_ACTIVATE(button_sensor);
    printf("+       All irrigation system are off     +\n\n");   
    printf("Waiting for the signal\n\n");

    while(1) {
        PROCESS_WAIT_EVENT();
        
        if(ev == sensors_event && data == &button_sensor) {
            printf("+ Irrigation System ............. [ON]\n");
            //sprintf(packet.payload, "Start of irrigation system");
            etimer_set(&report_timer, CLOCK_SECOND * 300); // Set the timer for 300 seconds = 5 minutes
            PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&report_timer)); // Wait until the timer expires
            printf("+ Irrigation System ............. [OFF]\n");
            
            if(has_parent){
                network_packet_t packet = {
                  .src_addr = linkaddr_node_addr,
                  .src_type = NODE_TYPE,
                  .dst_addr = parent.node_addr,
                  .dst_type = parent.type,
                  .type = 2,
                  .signal_strength = parent.signal_strength,
                };
                sprintf(packet.payload, "The irrigation system worked properly");

                nullnet_buf = (uint8_t *)&packet;
                nullnet_len = sizeof(packet);

                printf("#Network# Sending 'The irrigation system worked properly' to %02x:%02x\n", parent.node_addr.u8[0], parent.node_addr.u8[1]);

                NETSTACK_NETWORK.output(&parent.node_addr);
            }
            else {
                send_node_hello(NODE_TYPE);
            }
            etimer_reset(&report_timer);
           
        }
    }		
    PROCESS_END();
}
/*---------------------------------------------------------------------------*/