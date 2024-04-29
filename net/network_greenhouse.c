#include "network_greenhouse.h"
#include "net/netstack.h"
#include "dev/radio.h"

void set_radio_channel(void) {
    radio_value_t channel = CHANNEL;
    NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, channel);
}
