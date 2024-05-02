#include "contiki.h"
#include "net/netstack.h"
#include "net/packetbuf.h"
#include "net/nullnet/nullnet.h"
#include "lib/random.h"
#include "dev/radio.h"
#include <stdio.h>
#include <string.h>
#include "network_greenhouse.h"

#define DAY_DURATION (600 * CLOCK_SECOND)
#define NIGHT_DURATION (600 * CLOCK_SECOND)
#define REPORT_INTERVAL (10 * CLOCK_SECOND)

static network_node_t parent;
static uint8_t has_parent = 0;

void input_callback(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest) {
  if (len == sizeof(network_packet_t)) {
    network_packet_t packet;
    memcpy(&packet, data, sizeof(packet));

    switch (packet.type)
    {
      case 0:
        if (strcmp(packet.payload, "Sub-Gateway Hello") == 0 || strcmp(packet.payload, "Sub-Gateway Hello Response") == 0) {
          int8_t rssi = packetbuf_attr(PACKETBUF_ATTR_RSSI);
          printf("#Routing# New node found: %02x:%02x, RSSI: %d dBm -> ", packet.src_addr.u8[0], packet.src_addr.u8[1], rssi);

          if (!has_parent || parent.type != 1 || rssi > parent.signal_strength) {
            parent.node_addr = packet.src_addr;
            parent.type = 1;
            parent.signal_strength = rssi;

            has_parent = 1;
            
            printf("Parent (Sub-Gateway) address updated: %02x:%02x\n", parent.node_addr.u8[0], parent.node_addr.u8[1]);
          } else {
            printf("Ignored\n");
          }
        } else if (strcmp(packet.payload, "Node Hello") == 0) {
            network_packet_t response = {
              .src_addr = linkaddr_node_addr,
              .type = 0,
              .payload = "Node Hello Response"
            };

            nullnet_buf = (uint8_t *)&response;
            nullnet_len = sizeof(response);

            printf("#Network# Sending Node Hello Response to %02x:%02x\n", packet.src_addr.u8[0], packet.src_addr.u8[1]);

            NETSTACK_NETWORK.output(&packet.src_addr);
        } else if (strcmp(packet.payload, "Node Hello Response") == 0) {
            int8_t rssi = packetbuf_attr(PACKETBUF_ATTR_RSSI);
            printf("#Routing# New node found: %02x:%02x, RSSI: %d dBm -> ", packet.src_addr.u8[0], packet.src_addr.u8[1], rssi);

            if (!has_parent || (parent.type != 1 && rssi > parent.signal_strength)) {
              parent.node_addr = packet.src_addr;
              parent.type = 2;
              parent.signal_strength = rssi;

              has_parent = 1;

              printf("Parent (Node) address updated: %02x:%02x\n", parent.node_addr.u8[0], parent.node_addr.u8[1]);
            } else {
              printf("Ignored\n");
            }
        }
        break;
      
      case 2:
        nullnet_buf = (uint8_t *)&packet;
        nullnet_len = sizeof(packet);

        printf("#Network# Forwarding packet to %02x:%02x\n", parent.node_addr.u8[0], parent.node_addr.u8[1]);
        NETSTACK_NETWORK.output(&parent.node_addr);

      default:
        break;
    }
  }
}

void send_node_hello() {
  network_packet_t packet = {
    .src_addr = linkaddr_node_addr,
    .type = 0,
    .payload = "Node Hello"
  };

  nullnet_buf = (uint8_t *)&packet;
  nullnet_len = sizeof(packet);

  printf("#Network# Broadcasting Node Hello\n");
  NETSTACK_NETWORK.output(NULL);
}

/*---------------------------------------------------------------------------*/
PROCESS(light_sensor, "Light Sensor Process");
AUTOSTART_PROCESSES(&light_sensor);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(light_sensor, ev, data)
{
  static struct etimer report_timer;
  static struct etimer cycle_timer;
  static int is_day = 1;
  static uint8_t light_intensity;

  PROCESS_BEGIN()
  
  set_radio_channel();
  nullnet_set_input_callback(input_callback);
  send_node_hello();

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
        .type = 2,
        .signal_strength = parent.signal_strength,
      };
      sprintf(packet.payload, "%d", light_intensity);

      nullnet_buf = (uint8_t *)&packet;
      nullnet_len = sizeof(packet);

      printf("#Network# Sending %d to %02x:%02x\n", light_intensity, parent.node_addr.u8[0], parent.node_addr.u8[1]);

      NETSTACK_NETWORK.output(&parent.node_addr);
    } else {
        send_node_hello();
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
