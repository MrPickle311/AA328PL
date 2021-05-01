/*
 * ports.h
 *
 * Created: 01/03/2021 18:03:03
 *  Author: Damian
 *  I need make it ATOMIC!!
 */ 
#ifndef PORTS_H_
#define PORTS_H_

#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <stdbool.h>
#include "bit_utils.h"
#include "global_utils.h"

enum PORT_PinState
{
	Low = 0,
	High = 1
};

enum InputConfig
{
	PullUp,
	TripleState
};

typedef port_t port_state_t;
typedef port_t port_config_t;
typedef port_t port_input_state_t;

#define PORT_STATE(s)			&PORT##s
#define PORT_CONFIG(s)			&DDR##s
#define PORT_INPUT_STATE(s)		&PIN##s

//write operations on single pins 

void PORT_setPinHigh(port_state_t* port_state,pin_t pin);

void PORT_setPinLow(port_state_t* port_state,pin_t pin);

void PORT_invertPin(port_state_t* port_state,pin_t pin);

//read operations on single bits

bool PORT_isPinHigh(port_input_state_t* read_port,pin_t pin_nmbr);

bool PORT_isPinLow(port_input_state_t* read_port,pin_t pin_nmbr);

bool PORT_readPinState(port_input_state_t* read_port,pin_t pin_nmbr);

//configuring single pins

void PORT_setPinAsOutput(port_config_t* port_config,pin_t pin_nmbr);

void PORT_setPinAsInput(port_config_t* port_config,port_state_t* port_state,enum InputConfig input_config,pin_t pin_nmbr);

//operations on ports

void PORT_setMask(port_state_t* port_state,mask_8bit_t mask);

void PORT_clearPort(port_state_t* port_state);

void PORT_fillPort(port_state_t* port_state);

void PORT_invertPort(port_state_t* port_state);


//implementaion functions,not for user use

void __setPinsAsOutput(port_config_t* port_config,pin_t pins_nmbr,...);

void  __setPinsAsInput(port_config_t* port_config,port_state_t* port_state,enum InputConfig config,uint8_t pins_nmbr,...);

//

#define PORT_setHighPins(port_state, ...)				   setBitsAt(port_state, __VA_ARGS__)

#define PORT_setLowPins(port_state, ...)				   clearBitsAt(port_state, __VA_ARGS__)

#define PORT_invertPins(port_state, ...)				   reverseBitsAt(port_state, __VA_ARGS__)

#define PORT_setPinsAsOutput(port_config, ...)			   __setPinsAsOutput(port_config, PP_NARG(__VA_ARGS__), __VA_ARGS__)

#define PORT_setPinsAsInput(port_config,input_config, ...) __setPinsAsInput(port_config,input_config,PP_NARG(__VA_ARGS__), __VA_ARGS__)

#endif /* PORTS_H_ */