#include "contiki.h"
#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#include "dev/radio.h"
#include <string.h>
#include <stdio.h>

#define CHANNEL 26

static void set_radio_channel(void) {
    radio_value_t channel = CHANNEL;
    NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, channel);
}

static uint8_t light_intensity;

static void input_callback(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest) {
    memcpy(&light_intensity, data, sizeof(light_intensity));
    printf("Recieved light intensity: %d\n", light_intensity);
}


/*---------------------------------------------------------------------------*/
PROCESS(sub_gateway, "Sub-Gateway");
AUTOSTART_PROCESSES(&sub_gateway);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(sub_gateway, ev, data) {
    static struct etimer periodic_timer;

    PROCESS_BEGIN();

    nullnet_buf = (uint8_t *)&"Gateway Alive";
    nullnet_len = sizeof("Gateway Alive");

    set_radio_channel();

    nullnet_set_input_callback(input_callback);

    etimer_set(&periodic_timer, CLOCK_SECOND * 10);

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
    printf("Broadcasting alive signal\n");
    NETSTACK_NETWORK.output(NULL);
    etimer_reset(&periodic_timer);

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/