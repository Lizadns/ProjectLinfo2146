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



PROCESS(irrigation_system, "Irrigation system with timer");
AUTOSTART_PROCESSES(&irrigation_system);


PROCESS_THREAD(irrigation_system, ev, data)
{   static struct etimer report_timer;
    PROCESS_BEGIN();

    set_radio_channel();
    nullnet_set_input_callback(input_callback);
 
    
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
            
            if(has_gateway_addr){
                network_packet_t packet = {
                    .node_id = linkaddr_node_addr,
                    .type = 2,
                    .signal_strength = signal_strength,
                };
                sprintf(packet.payload, "The irrigation system worked properly");

                nullnet_buf = (uint8_t *)&packet;
                nullnet_len = sizeof(packet);

                printf("Irrigation system to the sub-gateaway : %02x:%02x\n",sub_gateway_addr.u8[0], sub_gateway_addr.u8[1]);
                
                NETSTACK_NETWORK.output(&sub_gateway_addr);
            }
            else {
                send_node_hello();
                printf("No sub-gateway address yet.\n");
            }
            etimer_reset(&report_timer);
           
        }
    }		
    PROCESS_END();
}

