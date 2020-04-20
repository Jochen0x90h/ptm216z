#include "util.h"

int const channel = 15;

uint8_t packet[200];

int main(void) {

	initClock();
	initLeds();
	initRadio();
	setRadioChannel(channel);
	
	// enable receive and wait until ready
	NRF_RADIO->TASKS_RXEN = N(RADIO_TASKS_RXEN_TASKS_RXEN, Trigger);
	while (!T(NRF_RADIO->EVENTS_READY, RADIO_EVENTS_READY_EVENTS_READY, Generated));
	NRF_RADIO->EVENTS_READY = 0;
	
	while (true) {
		// set packet pointer
		NRF_RADIO->PACKETPTR = (uint32_t)packet;

		// start receive and wait until packet arrived
		NRF_RADIO->TASKS_START = N(RADIO_TASKS_START_TASKS_START, Trigger);
		while (!T(NRF_RADIO->EVENTS_END, RADIO_EVENTS_END_EVENTS_END, Generated));
		NRF_RADIO->EVENTS_END = 0;
	
		setLeds(packet[1 + 7 + 11]);
	}
}
