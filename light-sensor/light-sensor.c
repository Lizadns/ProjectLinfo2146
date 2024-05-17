#include "contiki.h"
#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#include "lib/random.h"
#include "dev/radio.h"
#include <stdio.h>
#include <string.h>
#include "network_greenhouse.h"

#define DAY_DURATION (600 * CLOCK_SECOND)
#define NIGHT_DURATION (600 * CLOCK_SECOND)
#define REPORT_INTERVAL (30 * CLOCK_SECOND)

#define NODE_TYPE 2

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
        if(linkaddr_cmp(&packet.dst_addr, &multicast_addr) && children_nodes_count > 0){
            nullnet_buf = (uint8_t *)&packet;
            nullnet_len = sizeof(packet);

            printf("#Network# Multi-casting packet to children nodes of type %d with payload: %s\n", packet.dst_type, packet.payload);

            for (int i = 0; i < children_nodes_count; i++) {
              NETSTACK_NETWORK.output(&children_nodes[i].node_addr);
            }
        }

        if (packet.dst_type == NODE_TYPE) {
          if (strcmp(packet.payload, "Check up from the mobile terminal") == 0) {
            network_packet_t response = {
              .src_addr = linkaddr_node_addr,
              .src_type = NODE_TYPE,
              .dst_addr = packet.src_addr,
              .dst_type = 5,
              .type = 2,
              .signal_strength = 0,
            };
            sprintf(response.payload, "Check up response from the light sensor");

            nullnet_buf = (uint8_t *)&response;
            nullnet_len = sizeof(response);

            printf("#Network# Sending 'Check up response from the light sensor' to %02x:%02x\n", packet.src_addr.u8[0], packet.src_addr.u8[1]);

            NETSTACK_NETWORK.output(&parent.node_addr);
          }
        } else if(has_parent && linkaddr_cmp(&packet.dst_addr, &linkaddr_null)){
          nullnet_buf = (uint8_t *)&packet;
          nullnet_len = sizeof(packet);

          printf("#Network# Forwarding packet to %02x:%02x with payload: %s\n", parent.node_addr.u8[0], parent.node_addr.u8[1], packet.payload);
          NETSTACK_NETWORK.output(&parent.node_addr);
        }
      default:
        break;
    }
  }
}

/*---------------------------------------------------------------------------*/
PROCESS(light_sensor, "Light Sensor Process");
AUTOSTART_PROCESSES(&light_sensor);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(light_sensor, ev, data)
{
  static struct etimer report_timer;
  static struct etimer cycle_timer;
  static int is_day = 0;
  static uint8_t light_intensity;

  PROCESS_BEGIN()
  
  set_radio_channel();
  nullnet_set_input_callback(input_callback);
  send_node_hello(NODE_TYPE);

  etimer_set(&cycle_timer, DAY_DURATION);
  etimer_set(&report_timer, REPORT_INTERVAL);

  while (1) {
    
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&report_timer) || etimer_expired(&cycle_timer));

    if (etimer_expired(&report_timer)) {
      if (is_day) {
        light_intensity = (random_rand() % 40) + 60;

        // Case where a thunderstorm occurs
        if (random_rand() % 20 == 0) { 
          light_intensity -= 30;
        }
      } else {
        light_intensity = random_rand() % 20;
      }
    }

    if(has_parent) {
      network_packet_t packet = {
        .src_addr = linkaddr_node_addr,
        .src_type = NODE_TYPE,
        .dst_addr = linkaddr_null,
        .dst_type = 0,
        .type = 2,
        .signal_strength = parent.signal_strength,
      };
      sprintf(packet.payload, "Current temperature: %d", light_intensity);

      nullnet_buf = (uint8_t *)&packet;
      nullnet_len = sizeof(packet);

      printf("#Network# Sending 'Current temperature:%d' to %02x:%02x\n", light_intensity, parent.node_addr.u8[0], parent.node_addr.u8[1]);

      NETSTACK_NETWORK.output(&parent.node_addr);
    } else {
        send_node_hello(NODE_TYPE);
    }

    etimer_reset(&report_timer);

    if (etimer_expired(&cycle_timer)) {
      is_day = !is_day;
      etimer_set(&cycle_timer, is_day ? DAY_DURATION : NIGHT_DURATION);
      printf("Cycle changed: %s\n", is_day ? "Daytime" : "Nightime");
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
