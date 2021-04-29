/*
 * exti.c
 *
 * Created: 28/04/2021 16:13:45
 *  Author: Damian Wójcik
 */ 

#include "exti.h"
#include <stdarg.h>
#include "bit_utils.h"

//implementation

port_t* __getPhysicalInterruptionPort(enum EXTI_InterruptionPort port)
{
	switch(port)
	{
		case EXTI_InterruptionPortB: return &PCMSK0;
		case EXTI_InterruptionPortC: return &PCMSK1;
		case EXTI_InterruptionPortD: return &PCMSK2;
		#ifdef MCU_328PB
		case EXTI_InterruptionPortE: return &PCMSK3;
		#endif
		default: return NULL;
	}
}

//reg: PCICR , bits PCIEn
void __setupInterruptionPort(enum EXTI_InterruptionPort interrupt_port)
{
	SET_BIT_AT(PCICR,interrupt_port);
}

void __disableInterruptionPort(enum EXTI_InterruptionPort interrupt_port)
{
	CLEAR_BIT_AT(PCICR,interrupt_port);
}

//reg: PCMSKn, bits : PCINTn 
void __enableInterruptPin(EXTI_InterruptionRealPort* interrupt_real_port,pin_t pin)
{
	SET_BIT_AT(*interrupt_real_port,pin);
}

void __disableInterruptPin(EXTI_InterruptionRealPort* interrupt_real_port,pin_t pin)
{
	CLEAR_BIT_AT(*interrupt_real_port,pin);
	
}

//END implementation

void EXTI_setupAdvancedInterrupt(enum EXTI_AdvancedInterruptPin pin,enum EXTI_AdvancedInterruptActivationMode mode)
{
	SET_BIT_AT(EIMSK,pin);
	SET_SHIFTED_BIT_MASK(EICRA,mode,2*pin);
}

void EXTI_disableAdvancedInterrupt(enum EXTI_AdvancedInterruptPin pin)
{
	CLEAR_BIT_AT(PCICR,pin);
	bit_t ICSn0_pos = 0 + 2 * pin;//  0 or 2
	bit_t ICSn1_pos = 1 + 2 * pin;//  1 or 3
	clearBitsAt(&EICRA,ICSn0_pos,ICSn1_pos);
}

void EXTI_setupBasicInterrupt(enum EXTI_InterruptionPort interrupt_port,pin_t pin)
{
	EXTI_InterruptionRealPort* physical_port = __getPhysicalInterruptionPort(interrupt_port);
	__enableInterruptPin(physical_port,pin);
	__setupInterruptionPort(interrupt_port);
}

void EXTI_disableBasicInterrupt(enum EXTI_InterruptionPort interrupt_port,pin_t pin)
{
	EXTI_InterruptionRealPort* physical_port = __getPhysicalInterruptionPort(interrupt_port);
	__disableInterruptPin(physical_port,pin);
	__disableInterruptionPort(interrupt_port);
}

void __setupBasicInterrupts(enum EXTI_InterruptionPort interrupt_port,size_t pins_count,...)
{
	va_list valist;
	va_start(valist,pins_count);
	for (uint8_t i = 0; i < pins_count ; ++i)
		EXTI_setupBasicInterrupt(interrupt_port,va_arg(valist,size_t));
	va_end(valist);
}

void __disableBasicInterrupts(enum EXTI_InterruptionPort interrupt_port,size_t pins_count,...)
{
	va_list valist;
	va_start(valist,pins_count);
	for (uint8_t i = 0; i < pins_count ; ++i)
		EXTI_disableBasicInterrupt(interrupt_port,va_arg(valist,size_t));
	va_end(valist);
}
