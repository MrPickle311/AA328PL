/*
 * port.c
 *
 * Created: 26/04/2021 15:01:07
 *  Author: Damian Wójcik
 */ 
#include "port.h"
#include <stdarg.h>

//operations on single pins 

void PORT_setPinHigh(port_state_t* port_state,pin_t pin)
{
	SET_BIT_AT(*port_state,pin);
}

void PORT_setPinLow(port_state_t* port_state,pin_t pin)
{
	CLEAR_BIT_AT(*port_state,pin);
}

void PORT_invertPin(port_state_t* port_state,pin_t pin)
{
	REVERSE_BIT_AT(*port_state,pin);
}

//read operations on single bits

bool PORT_isPinHigh(port_t* read_port,pin_t pin_nmbr)
{
	return IS_BIT_SET_AT(*read_port,pin_nmbr);
}

bool PORT_isPinLow(port_t* read_port,pin_t pin_nmbr)
{
	return IS_BIT_CLEARED_AT(*read_port,pin_nmbr);
}

bool PORT_readPinState(port_input_state_t* read_port,pin_t pin_nmbr)
{
	return IS_BIT_SET_AT(*read_port,pin_nmbr);
}

//configuring single pins

void PORT_setPinAsOutput(port_config_t* port_config,pin_t pin_nmbr)
{
	SET_BIT_AT(*port_config,pin_nmbr);
}

void PORT_setPinAsInput(port_config_t* port_config,port_state_t* port_state,enum InputConfig input_config,pin_t pin_nmbr)
{
	CLEAR_BIT_AT(*port_config,pin_nmbr);
	
	if(port_config == PullUp)
		SET_BIT_AT(*port_state,pin_nmbr);
	else CLEAR_BIT_AT(*port_state,pin_nmbr);
}

//operations on ports

void PORT_setMask(port_state_t* port_state,mask_8bit_t mask)
{
	REPLACE_REGISTER(*port_state,mask);
}

void PORT_clearPort(port_state_t* port_state)
{
	WIPE_REGISTER(*port_state);
}

void PORT_fillPort(port_state_t* port_state)
{
	FILL_REGISTER(*port_state);
}

void PORT_invertPort(port_state_t* port_state)
{
	REVERSE_MASK(*port_state);
}

//implementaion functions,not for user use


void __setPinsAsOutput(port_config_t* port_config,pin_t pins_nmbr,...)
{
	va_list valist;
	va_start(valist,pins_nmbr);
	
	for (uint8_t i = 0; i < pins_nmbr ; ++i)
		PORT_setPinAsOutput(port_config,va_arg(valist,int));
	
	va_end(valist);
}

void __setPinsAsInput(port_config_t* port_config,port_state_t* port_state,enum InputConfig config,uint8_t pins_nmbr,...)
{
	va_list valist;
	va_start(valist,pins_nmbr);
	
	for (uint8_t i = 0; i < pins_nmbr ; ++i)
		PORT_setPinAsInput(port_config,port_state,config,va_arg(valist,int));
	
	va_end(valist);
}
