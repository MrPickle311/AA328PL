/*
 * interrupt.h
 *
 * Created: 17/03/2021 08:48:20
 *  Author: Damian Wójcik
 */ 

#ifndef EXTERNAL_INTERRUPT_H_
#define EXTERNAL_INTERRUPT_H_

#include <avr/interrupt.h>
#include <avr/io.h>
#include "global_utils.h"
#include <stdbool.h>

//some human-readable macro definitions
#define enableInterrupts() sei()

//supported by PD2(INT0) and PD3(INT1) 
#define advancedInterrupt(number) ISR(INT##number##_vect)

#define advancedInterruptPin2PortD advancedInterrupt(0)
#define advancedInterruptPin3PortD advancedInterrupt(1)

//basic interrupts supported by other pins
#define basicInterrupt(number) ISR(PCINT##number##_vect)

enum AdvancedInterruptActivationMode
{
	ActivatedByLowLevel = 0b00,
	ActivatedByAnyLogicChange = 0b01,
	ActivatedByFallingEdge = 0b10,
	ActivatedByRisingEdge = 0b11
};

enum AdvancedInterruptPin
{
	Pin2PortDAdvancedInterrupt = 0,
	Pin3PortDAdvancedInterrupt = 1
};

void setupAdvancedInterrupt(enum AdvancedInterruptPin pin,enum AdvancedInterruptActivationMode mode)
{
	SET_BIT_AT(EIMSK,pin);
	SET_SHIFTED_BIT_MASK(EICRA,mode,2*pin);
}

typedef volatile uint8_t* InterruptionRealPort;

enum InterruptionPort
{
	InterruptionPortB = 0,
	InterruptionPortC = 1,
	InterruptionPortD = 2
};

uint8_t* __getPhysicalInterruptionPort(enum InterruptionPort port)
{
	switch(port)
	{
		case InterruptionPortB: return &PCMSK0;
		case InterruptionPortC: return &PCMSK1;
		case InterruptionPortD: return &PCMSK2;
		default: return NULL;
	}
}

void __setupInterruptionPort(enum InterruptionPort interrupt_port)
{
	SET_BIT_AT(PCICR,interrupt_port);
}

void __enableInterruptPin(InterruptionRealPort interrupt_real_port,size_t pin)
{
	SET_BIT_AT(*interrupt_real_port,pin);
}

void setupBasicInterrupt(enum InterruptionPort interrupt_port,size_t pin)
{
	uint8_t* real_port = __getPhysicalInterruptionPort(interrupt_port);
	__enableInterruptPin(real_port,pin);
	__setupInterruptionPort(interrupt_port);
}

void __setupBasicInterrupts(enum InterruptionPort interrupt_port,size_t pins_count,...)
{
	va_list valist;
	va_start(valist,pins_count);
	for (uint8_t i = 0; i < pins_count ; ++i)
		setupBasicInterrupt(interrupt_port,va_arg(valist,size_t));
	va_end(valist);
}

#define setupBasicInterrupts(port, ...) __setupBasicInterrupts(port, PP_NARG(__VA_ARGS__), __VA_ARGS__)

#define basicInterruptPortB ISR(PCINT0_vect)
#define basicInterruptPortC ISR(PCINT1_vect)
#define basicInterruptPortB ISR(PCINT2_vect)

#endif /* EXTERNAL_INTERRUPT_H_ */