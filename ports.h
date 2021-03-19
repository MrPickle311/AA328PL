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
#include <stdarg.h>
#include <stdbool.h>
#include "bit_utils.h"

typedef struct
{
	unsigned int p0:1;
	unsigned int p1:1;
	unsigned int p2:1;
	unsigned int p3:1;
	unsigned int p4:1;
	unsigned int p5:1;
	unsigned int p6:1;
	unsigned int p7:1;
} VirtualPort;

#define PORT_STATE(s) &PORT##s
#define PORT_CONFIG(s) &DDR##s

#define READ_PORT(s)  &PIN##s

//shorter names
#define _PORTD (*(volatile PORT_STATE*) &PORTD)
#define _PORTB (*(volatile PORT_STATE*) &PORTB)
#define _PORTC (*(volatile PORT_STATE*) &PORTC)

volatile typedef struct
{
	 uint8_t* port_state;
	 uint8_t* port_config;
}PortSetup;

void initPortSetup(volatile PortSetup* port_setup,
				  volatile uint8_t* port_state,
				  volatile uint8_t* port_config)
{
	port_setup->port_state = port_state;
	port_setup->port_config = port_config;
}

enum InputConfig
{
	PullUp,
	TripleState
};

static void __determine_pin__(volatile VirtualPort* port,uint8_t pin_nmbr,uint8_t val)
{
	switch(pin_nmbr)
	{
		case 0:
			port->p0 = val;
			break;
		case 1:
			port->p1 = val;
			break;
		case 2:
			port->p2 = val;
			break;
		case 3:
			port->p3 = val;
			break;
		case 4:
			port->p4 = val;
			break;
		case 5:
			port->p5 = val;
			break;
		case 6:
			port->p6 = val;
			break;
		case 7:
			port->p7 = val;
			break;
	}
}

void enablePin(volatile VirtualPort* port,uint8_t pin)
{
	__determine_pin__(port,pin,1);
}

void disablePin(volatile VirtualPort* port,uint8_t pin)
{
	__determine_pin__(port,pin,0);
}

void enablePinFast(volatile uint8_t* port,uint8_t pin)
{
	*port |= 0x1 << pin;
}

void disablePinFast(volatile uint8_t* port,uint8_t pin)
{
	*port &=  ~(0x1 << pin) ; 
}

void enablePinsFast(volatile uint8_t* port,uint8_t pins_nmbr,...)
{
	va_list valist;
	va_start(valist,pins_nmbr);
	for (uint8_t i = 0; i < pins_nmbr ; ++i)
		enablePinFast(port,va_arg(valist,int));
	va_end(valist);
}

void disablePinsFast(volatile uint8_t* port,uint8_t pins_nmbr,...)
{
	va_list valist;
	va_start(valist,pins_nmbr);
	for (uint8_t i = 0; i < pins_nmbr ; ++i)
		disablePinFast(port,va_arg(valist,int));
	va_end(valist);
}

void setMask(volatile uint8_t* port,uint8_t mask)
{
	*port = mask;
}

void __enablePins(volatile VirtualPort* port,uint8_t pins_nmbr,...)
{
	va_list valist;
	va_start(valist,pins_nmbr);
	for (uint8_t i = 0; i < pins_nmbr ; ++i)
		enablePin(port,va_arg(valist,int));
	va_end(valist);
}

void __disablePins(volatile VirtualPort* port,uint8_t pins_nmbr,...)
{
	va_list valist;
	va_start(valist,pins_nmbr);
	for (uint8_t i = 0; i < pins_nmbr ; ++i)
		disablePin(port,va_arg(valist,int));
	va_end(valist);
}

#define setHighPins(port, ...) __enablePins(port, PP_NARG(__VA_ARGS__), __VA_ARGS__)
#define setLowPins(port, ...) __disablePins(port, PP_NARG(__VA_ARGS__), __VA_ARGS__)

void clearPort(volatile uint8_t* port)
{
	*port = 0x0;
}

void fillPort(volatile uint8_t* port)
{
	*port = 0xFF;
}

void invertPort(volatile uint8_t* port)
{
	*port ^= 0b11111111;
}

void invertPin(volatile uint8_t* port,uint8_t pin)
{
	*port ^= 0x1 << pin ;  
}

void __invertPins(volatile uint8_t* port,uint8_t pins_nmbr,...)
{
	va_list valist;
	va_start(valist,pins_nmbr);
	
	for (uint8_t i = 0; i < pins_nmbr ; ++i)
		invertPin(port,va_arg(valist,int));
	
	va_end(valist);
}

#define invertPins(port, ...) __invertPins(port, PP_NARG(__VA_ARGS__), __VA_ARGS__)

bool readPinState(volatile uint8_t* read_port,uint8_t pin_nmbr)
{
	return *read_port;
}

bool isPinHigh(volatile uint8_t* read_port,uint8_t pin_nmbr)
{
	return ( *read_port & (1 << pin_nmbr) ) == 1;
}

bool isPinLow(volatile uint8_t* read_port,uint8_t pin_nmbr)
{
	return ( *read_port & (1 << pin_nmbr) ) == 0;
}

inline static void setPinAsOutput(volatile uint8_t* port_config,uint8_t pin_nmbr)
{
	SET_BIT_AT(*port_config,pin_nmbr);
}

void __setPinsAsOutput(volatile uint8_t* port_config,uint8_t pins_nmbr,...)
{
	va_list valist;
	va_start(valist,pins_nmbr);
	
	for (uint8_t i = 0; i < pins_nmbr ; ++i)
		setPinAsOutput(port_config,va_arg(valist,int));
	
	va_end(valist);
}

#define setPinsAsOutput(port_config, ...) __setPinsAsOutput(port_config, PP_NARG(__VA_ARGS__), __VA_ARGS__)

inline static void setPinAsInput(volatile PortSetup* port,enum InputConfig config,uint8_t pin_nmbr)
{
	CLEAR_BIT_AT(*port->port_config,pin_nmbr);
	if(config == PullUp)
		SET_BIT_AT(*port->port_state,pin_nmbr);
	else CLEAR_BIT_AT(*port->port_state,pin_nmbr);
}

void __setPinsAsInput(volatile PortSetup* port,enum InputConfig config,uint8_t pins_nmbr,...)
{
	va_list valist;
	va_start(valist,pins_nmbr);
	
	for (uint8_t i = 0; i < pins_nmbr ; ++i)
		setPinAsInput(port,config,va_arg(valist,int));
	
	va_end(valist);
}

#define setPinsAsInput(port_info_type,inuput_config, ...) __setPinsAsInput(port_info_type,inuput_config,PP_NARG(__VA_ARGS__), __VA_ARGS__)

#endif /* PORTS_H_ */