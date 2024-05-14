/*
 * Copyright (c) 2006, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         A very simple Contiki application showing how Contiki programs look
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#include "contiki.h"
#include "dev/serial-line.h"
#include "cpu/msp430/dev/uart0.h"
#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#include "dev/radio.h"
#include <string.h>
#include "network_greenhouse.h"
#include <stdio.h> /* For printf() */

#define BROADCAST_DELAY (CLOCK_SECOND * 50)//toutes les 10 minutes

#define NODE_TYPE 0

static network_node_t children_nodes[20];
static int children_nodes_count = 0;

static void input_callback(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest) {
  if (len == sizeof(network_packet_t)) {
        network_packet_t packet;
        memcpy(&packet, data, sizeof(packet));

        switch (packet.type)
        {
          case 0:
                if (strcmp(packet.payload, "Parent Join") == 0) {
                    if (packet.src_type == 1){//sub-gateway
                        assign_child(packet, children_nodes,&children_nodes_count);
                    }
                    else if (strcmp(packet.payload, "Parent Leave") == 0) {
                        unassign_child(packet, children_nodes, &children_nodes_count);
                    }
                }
                break;
          case 2:
                printf("Received packet from %02x:%02x with payload: %s\n", packet.src_addr.u8[0], packet.src_addr.u8[1], packet.payload);
                break;
          default:
                printf("Recieved an unknown packet type from %02x:%02x\n", packet.src_addr.u8[0], packet.src_addr.u8[1]);
                break;
        }
}
}

/*---------------------------------------------------------------------------*/
PROCESS(test_serial, "Gateway process");
AUTOSTART_PROCESSES(&test_serial);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(test_serial, ev, data)
{
  static struct etimer periodic_timer;

  //static struct etimer timer;

  PROCESS_BEGIN();
  serial_line_init();
  uart0_set_input(serial_line_input_byte);

  set_radio_channel();
  nullnet_set_input_callback(input_callback);
  etimer_set(&periodic_timer, BROADCAST_DELAY);


  //etimer_set(&timer, CLOCK_SECOND * 1000);
  printf("Starting process\n");
  while(1) {

    if(ev == PROCESS_EVENT_TIMER && data == &periodic_timer) {
    // Diffuser la présence
      send_node_hello(0);
    // Réinitialiser le timer pour la prochaine diffusion
      etimer_reset(&periodic_timer);
    }
    PROCESS_YIELD();
    //printf("light intensity : 10\n");
   
    if(ev==serial_line_event_message){
      if (strcmp((char*)data,"Turn on the irrigation system!")==0){
        // envoyer à tout le monde 
        printf("%s \n",(char*) data);
        for (int i = 0; i < children_nodes_count; i++) {
            //printf("je suis ici\n");
            network_packet_t trigger_irrigation = {
            .src_addr = linkaddr_node_addr,
            .src_type = NODE_TYPE,
            .dst_addr = children_nodes[i].node_addr,
            .dst_type = children_nodes[i].type,
            .type = 2,
            .payload = "Turn on the irrigation system!"
            };
            
            nullnet_buf = (uint8_t *)&trigger_irrigation;
            nullnet_len = sizeof(trigger_irrigation);
            NETSTACK_NETWORK.output(&children_nodes[i].node_addr);
        }
      }
        
    }
    
  
  
    //else if(etimer_expired(&timer)){
    //    printf("timer expired\n");
    //    etimer_reset(&timer); 
    //}

    

  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
