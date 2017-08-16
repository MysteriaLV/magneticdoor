#include <Arduino.h>
#include <Automaton.h>
#include "magnetic_door_modbus.h"

extern void df_mp3_setup();
extern void df_mp3_loop();
extern void df_mp3_play(int fileNumber);

/*
 * 2    CardReader Power OUT
 * 3    Door Relay OUT
 * 6-10 ModBus
 * 11   MP3 TX (3)
 * 12   MP3 RX (2) via 1K
 * 13   RFID reader IN
 */

Atm_button rfid_reader_in;
Atm_timer door_open_delay;
Atm_led cardreader_relay_out, doormagnet_relay_out;
Atm_fan input_event_distributor;

void rfid_reader_event( int idx, int v, int up ) {
	if ( v ) {
		modbus_set(COMPLETE, 1);
		df_mp3_play(1);
	}
}

void setup() {
	Serial.begin(115200);
	modbus_setup();
	df_mp3_setup();

	rfid_reader_in
			.trace(Serial)
			.begin(13)
			.onPress(input_event_distributor, Atm_fan::EVT_INPUT);

	input_event_distributor
			.trace(Serial)
			.begin()
			.onInput(&rfid_reader_event)
			.onInput(cardreader_relay_out, Atm_led::EVT_ON)
			.onInput(door_open_delay, Atm_timer::EVT_START);

	cardreader_relay_out
			.trace(Serial)
			.begin(2, true).on();		// TODO control via modbus

	door_open_delay
//			.trace(Serial)  (don't enable, breaks stuff)
			.begin(10000)
			.onFinish(doormagnet_relay_out, Atm_led::EVT_ON);

	doormagnet_relay_out
			.trace(Serial)
			.begin(3, true);
}

void loop() {
	modbus_loop();
	df_mp3_loop();
	automaton.run();
}