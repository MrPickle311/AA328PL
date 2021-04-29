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

typedef port_t EXTI_InterruptionRealPort;

enum EXTI_AdvancedInterruptActivationMode
{
	EXTI_ActivatedByLowLevel	   = 0b00,
	EXTI_ActivatedByAnyLogicChange = 0b01,
	EXTI_ActivatedByFallingEdge    = 0b10,
	EXTI_ActivatedByRisingEdge	   = 0b11
};

//exti interruption flags

#define EXTI_Pin2PortDAdvancedInterrupt_Occured	  IS_BIT_SET_AT(EIFR,INTF0)
#define EXTI_Pin3PortDAdvancedInterrupt_Occured	  IS_BIT_SET_AT(EIFR,INTF1)

#define EXTI_waitUntil_Pin2PortDAdvancedInterrupt while( !EXTI_Pin2PortDAdvancedInterrupt_Occured )
#define EXTI_waitUntil_Pin3PortDAdvancedInterrupt while( !EXTI_Pin3PortDAdvancedInterrupt_Occured )

#define EXTI_resetPin2PortDAdvancedInterrupt_Flag SET_BIT_AT(EIFR,INTF0)
#define EXTI_resetPin3PortDAdvancedInterrupt_Flag SET_BIT_AT(EIFR,INTF1)

#define INT0_pos 0
#define INT1_pos 1

enum EXTI_AdvancedInterruptPin
{
	EXTI_Pin2PortDAdvancedInterrupt = INT0_pos,
	EXTI_Pin3PortDAdvancedInterrupt = INT1_pos
};

#define PCIE0_pos   0
#define PCIE1_pos   1
#define PCIE2_pos   2
#define PCIE3_pos   3

enum EXTI_InterruptionPort
{
	EXTI_InterruptionPortB = PCIE0_pos,
	EXTI_InterruptionPortC = PCIE1_pos,
	EXTI_InterruptionPortD = PCIE2_pos
	
#ifdef MCU_328PB
	,
	EXTI_InterruptionPortE = PCIE3_pos
#endif // MCU_328PB
};

//interruptions supported by PD2(INT0) and PD3(INT1)

#define EXTI_advancedInterruptPin2PortD ISR(INT0_vect)
#define EXTI_advancedInterruptPin3PortD ISR(INT1_vect)

void EXTI_setupAdvancedInterrupt(enum EXTI_AdvancedInterruptPin pin,enum EXTI_AdvancedInterruptActivationMode mode);

void EXTI_setupBasicInterrupt(enum EXTI_InterruptionPort interrupt_port,pin_t pin);

void EXTI_disableAdvancedInterrupt(enum EXTI_AdvancedInterruptPin pin);

void EXTI_disableBasicInterrupt(enum EXTI_InterruptionPort interrupt_port,pin_t pin);

//not for user use
void __setupBasicInterrupts(enum EXTI_InterruptionPort interrupt_port,size_t pins_count,...);

void __disableBasicInterrupts(enum EXTI_InterruptionPort interrupt_port,size_t pins_count,...);

//END not for user use

#define EXTI_setupBasicInterrupts(port, ...) __setupBasicInterrupts(port, PP_NARG(__VA_ARGS__), __VA_ARGS__)

#define EXTI_disableBasicInterrupts(port, ...) __disableBasicInterrupts(port, PP_NARG(__VA_ARGS__), __VA_ARGS__)

//basic interrupts definitions 

#define EXTI_basicInterruptPortB ISR(PCINT0_vect)
#define EXTI_basicInterruptPortC ISR(PCINT1_vect)
#define EXTI_basicInterruptPortD ISR(PCINT2_vect)

#ifdef MCU_328PB

#define EXTI_basicInterruptPortE ISR(PCINT3_vect)

#endif

#endif /* EXTERNAL_INTERRUPT_H_ */