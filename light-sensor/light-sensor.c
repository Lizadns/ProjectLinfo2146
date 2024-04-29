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

static linkaddr_t sub_gateway_addr;
static uint8_t has_gateway_addr = 0;
static int8_t signal_strength = 0; 

void input_callback(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest) {
  if (len == sizeof(network_packet_t)) {
    network_packet_t packet;
    memcpy(&packet, data, sizeof(packet));

    if (packet.type == 0 && strcmp(packet.payload, "Sub-Gateway Presence") == 0) {
      int8_t rssi = packetbuf_attr(PACKETBUF_ATTR_RSSI);

      printf("New sub-gateway found: %02x:%02x, RSSI: %d dBm\n", src->u8[0], src->u8[1], rssi);

      // Only update gateway if this one has stronger signal or if no gateway was set
      if (!has_gateway_addr || rssi > signal_strength) {
          memcpy(&sub_gateway_addr, src, LINKADDR_SIZE);
          has_gateway_addr = 1;
          signal_strength = rssi;
          
          printf("Sub-gateway address updated: %02x:%02x\n", sub_gateway_addr.u8[0], sub_gateway_addr.u8[1]);
      }
    }
  }
}

void send_node_hello() {
  network_packet_t packet = {
    .node_id = linkaddr_node_addr,
    .type = 0,
    .payload = "Node hello"
  };

  nullnet_buf = (uint8_t *)&packet;
  nullnet_len = sizeof(packet);

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

    if(has_gateway_addr) {
      network_packet_t packet = {
        .node_id = linkaddr_node_addr,
        .type = 2,
        .signal_strength = signal_strength,
      };
      sprintf(packet.payload, "%d", light_intensity);

      nullnet_buf = (uint8_t *)&packet;
      nullnet_len = sizeof(packet);

      printf("Sending light intensity: %d to sub-gateway %02x:%02x\n", light_intensity, sub_gateway_addr.u8[0], sub_gateway_addr.u8[1]);
      
      NETSTACK_NETWORK.output(&sub_gateway_addr);
    } else {
        send_node_hello();
        printf("No sub-gateway address yet.\n");
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
