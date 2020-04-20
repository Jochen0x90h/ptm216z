#pragma once

#include <nrf.h>
#include <stdbool.h>


// construct bitfield from value
#define V(field, value) ((value) << field##_Pos)

// construct bitfield from named value
#define N(field, value) (field##_##value << field##_Pos)

// get bitfield
#define G(reg, field) (((reg) & field##_Msk) >> field##_Pos)

// test condition
#define T(reg, field, value) (((reg) & field##_Msk) == field##_##value << field##_Pos)



// configure a gpio as input with pullup
void configureInputWithPullup(uint8_t index) {
	NRF_P0->PIN_CNF[index] = N(GPIO_PIN_CNF_DIR, Input)
		| N(GPIO_PIN_CNF_INPUT, Connect)
		| N(GPIO_PIN_CNF_PULL, Pullup);
}

// configure a gpio as output
void configureOutput(uint8_t index) {
	NRF_P0->PIN_CNF[index] = N(GPIO_PIN_CNF_DIR, Output)
		| N(GPIO_PIN_CNF_INPUT, Disconnect)
		| N(GPIO_PIN_CNF_PULL, Disabled)
		| N(GPIO_PIN_CNF_DRIVE, S0S1);
}


// clock

void initClock() {
	// configure clock
	NRF_CLOCK->HFXODEBOUNCE = N(CLOCK_HFXODEBOUNCE_HFXODEBOUNCE, Db1024us);
	NRF_CLOCK->LFCLKSRC = N(CLOCK_LFCLKSRC_SRC, Synth);

	// start HF clock and wait
	NRF_CLOCK->TASKS_HFCLKSTART = N(CLOCK_TASKS_HFCLKSTART_TASKS_HFCLKSTART, Trigger);
	while (!T(NRF_CLOCK->EVENTS_HFCLKSTARTED, CLOCK_EVENTS_HFCLKSTARTED_EVENTS_HFCLKSTARTED, Generated));
	NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;

	// start LF clock and wait
	NRF_CLOCK->TASKS_LFCLKSTART = N(CLOCK_TASKS_LFCLKSTART_TASKS_LFCLKSTART, Trigger);
	while (!T(NRF_CLOCK->EVENTS_LFCLKSTARTED, CLOCK_EVENTS_LFCLKSTARTED_EVENTS_LFCLKSTARTED, Generated));
	NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
}


// leds

#define LED_RED_PIN 23
#define LED_GREEN_PIN 22
#define LED_BLUE_PIN 24

void initLeds() {
	configureOutput(LED_RED_PIN);
	configureOutput(LED_GREEN_PIN);
	configureOutput(LED_BLUE_PIN);
}

void setRed(bool on) {
	if (on)
		NRF_P0->OUTCLR = 1 << LED_RED_PIN;
	else
		NRF_P0->OUTSET = 1 << LED_RED_PIN;
}

void setGreen(bool on) {
	if (on)
		NRF_P0->OUTCLR = 1 << LED_GREEN_PIN;
	else
		NRF_P0->OUTSET = 1 << LED_GREEN_PIN;
}

void setBlue(bool on) {
	if (on)
		NRF_P0->OUTCLR = 1 << LED_BLUE_PIN;
	else
		NRF_P0->OUTSET = 1 << LED_BLUE_PIN;
}

void setLeds(int value) {
	setRed(value & 1);
	setGreen(value & 2);
	setBlue(value & 4);	
}


// radio

void initRadio() {
	// set modulation mode
	NRF_RADIO->MODE = N(RADIO_MODE_MODE, Ieee802154_250Kbit);
	
	// configure crc
	NRF_RADIO->CRCCNF =
		N(RADIO_CRCCNF_SKIPADDR, Ieee802154) // CRC starts after first byte of length field
		| N(RADIO_CRCCNF_LEN, Two); // CRC-16
	NRF_RADIO->CRCPOLY = 0x11021;
	NRF_RADIO->CRCINIT = 0;

	// configure packet
	NRF_RADIO->PCNF0 =
		N(RADIO_PCNF0_PLEN, 32bitZero) // preamble is 32 bit zero
		| N(RADIO_PCNF0_CRCINC, Include) // length includes CRC
		| V(RADIO_PCNF0_LFLEN, 8); // length is 8 bit
	NRF_RADIO->PCNF1 = N(RADIO_PCNF1_ENDIAN, Little); // little endian
}

void setRadioChannel(int channel) {
	// set channel
	NRF_RADIO->FREQUENCY = V(RADIO_FREQUENCY_FREQUENCY, 2405 + 5 * (channel - 11) - 2400);
}
