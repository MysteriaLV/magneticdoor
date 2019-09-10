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
Atm_led cardreader_relay_out, doormagnet_relay_out;
Atm_fan input_event_distributor;

Atm_controller single_state;
Atm_bit state_lock;

void rfid_reader_event( int idx, int v, int up ) {
	if ( v ) {
		modbus_set(OPENED, 1);
		df_mp3_play(1);
	}
}

void setup() {
	Serial.begin(115200);
	df_mp3_setup();
	modbus_setup(); // Goes after df_mp3_setup() to be active listener

	state_lock.begin();
	single_state.begin()
			.IF(rfid_reader_in, '=', Atm_button::PRESSED)
			.AND(state_lock, '=', Atm_bit::OFF)
			.onChange(true, input_event_distributor, Atm_fan::EVT_INPUT)
			.trace(Serial);

	rfid_reader_in
			.begin(13);

	input_event_distributor
			.begin()
			.onInput(&rfid_reader_event)
			.onInput(state_lock, Atm_bit::EVT_ON)
			.onInput(cardreader_relay_out, Atm_led::EVT_OFF)
			.onInput(doormagnet_relay_out, Atm_led::EVT_ON);

	cardreader_relay_out
			.trace(Serial)
			.begin(2, false);

#ifdef MY_TEST_MODE
    cardreader_relay_out.on();
#endif

	doormagnet_relay_out
			.trace(Serial)
			.begin(3, false);
}

void loop() {
	modbus_loop();
//	df_mp3_loop();  // We are not listening anyway, why pretend?
	automaton.run();
}