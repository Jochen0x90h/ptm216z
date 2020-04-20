#include "util.h"


// built-in button is on pin P0.18. If it resets, connect external button or use https://github.com/makerdiary/nrf52840-mdk-usb-dongle/tree/master/examples/nrf5-sdk/pselreset_erase
#define BUTTON_PIN 18

void initButton() {
	configureInputWithPullup(6);
}

bool getButton() {
	return (NRF_P0->IN & (1 << BUTTON_PIN)) == 0;
}

int const channel = 15;

uint8_t packet[200] = {25 + 2};

int main(void) {

	initClock();
	initLeds();
	initButton();
	initRadio();
	setRadioChannel(channel);

	
	int count = 0;
	while (true) {
		// wait until button is pressed
		while (!getButton());

		// set packet content
		packet[1 + 7 + 11] = count;

		// set packet pointer
		NRF_RADIO->PACKETPTR = (uint32_t)packet;
		
		// enable sender and wait until ready
		NRF_RADIO->TASKS_TXEN = N(RADIO_TASKS_TXEN_TASKS_TXEN, Trigger);
		while (!T(NRF_RADIO->EVENTS_READY, RADIO_EVENTS_READY_EVENTS_READY, Generated));
		NRF_RADIO->EVENTS_READY = 0;
		
		// start sender and wait until packet was sent
		NRF_RADIO->TASKS_START = N(RADIO_TASKS_START_TASKS_START, Trigger);
		while (!T(NRF_RADIO->EVENTS_END, RADIO_EVENTS_END_EVENTS_END, Generated));
		NRF_RADIO->EVENTS_END = 0;

		// disable sender and wait until disabled
		NRF_RADIO->TASKS_DISABLE = N(RADIO_TASKS_DISABLE_TASKS_DISABLE, Trigger);
		while (!T(NRF_RADIO->EVENTS_DISABLED, RADIO_EVENTS_DISABLED_EVENTS_DISABLED, Generated));
		NRF_RADIO->EVENTS_DISABLED = 0;
		
		setLeds(count);
		
		// wait until button is released
		while (getButton());

		++count;
	}
}
