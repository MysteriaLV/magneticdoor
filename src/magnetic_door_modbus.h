#ifndef MAGNETIC_DOOR_MODBUS_H
#define MAGNETIC_DOOR_MODBUS_H

#define USE_HOLDING_REGISTERS_ONLY
#include <Arduino.h>
#include <Modbus.h>
#include <Atm_led.hpp>

extern void modbus_setup();
extern void modbus_loop();
extern void modbus_set(word event, word value);
extern Atm_led cardreader_relay_out, doormagnet_relay_out;
extern Atm_bit state_lock;

//////////////// registers of MAGNETIC_DOOR ///////////////////
enum
{
	// The first register starts at address 0
	ACTIONS,      // Always present, used for incoming actions

	// Any registered events, denoted by 'triggered_by_register' in rs485_node of Lua script, 1 and up
    OPENED,

	TOTAL_ERRORS     // leave this one, error counter
};
#endif //MAGNETIC_DOOR_MODBUS_H