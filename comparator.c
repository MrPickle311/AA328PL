/*
 * comparator.c
 *
 * Created: 13/04/2021 23:19:50
 *  Author: Damian Wójcik
 */ 

#include "comparator.h"
#include <avr/power.h>
#include "bit_utils.h"
#include <util/atomic.h>

static inline void _setupMultiplexer(enum COMPARATOR_MultiplexerNegativeInput negative_input)
{
	clearBitsAt(&ADMUX,MUX2,MUX1,MUX0);
	SET_BIT_AT(ADCSRB,ACME);//comparator now controls the multiplexer
	SET_SHIFTED_BIT_MASK(ADMUX,negative_input,0);
}

static inline void _setupInterrupts(enum COMPARATOR_InterruptMode interrupt_mode)
{
	clearBitsAt(&ACSR,ACIS1,ACIS0);
	SET_SHIFTED_BIT_MASK(ACSR,interrupt_mode,0);
}

static inline void _disableDigitalMultiplexerPins(enum COMPARATOR_MultiplexerDigitalPinsDisabled 
												  digit_multiplexer_pins_disabled)
{
	clearBitsAt(&ACSR,MUX3,MUX2,MUX1,MUX0);
	SET_SHIFTED_BIT_MASK(ACSR,digit_multiplexer_pins_disabled,0);											  
}

static inline void _setupComparatorPositiveInput(enum COMPARATOR_PositiveInput positive_input)
{
	CLEAR_BIT_AT(ACSR,ACBG);
	SET_SHIFTED_BIT_MASK(ACSR,positive_input,6);
}

static inline void _enableTimer()
{
	SET_BIT_AT(ACSR,ACIC);
}

static inline void _disableDigitalPins(enum COMPARATOR_DigitalPinsDisabled digit_pins_disabled)
{
	clearBitsAt(&DIDR1,AIN1D,AIN0D);
	SET_SHIFTED_BIT_MASK(DIDR1,digit_pins_disabled,0);
}

void COMPARATOR_init(COMPARATOR_Setup setup)
{
	//startup kit to enabling comparator
	power_acb_enable();
	SET_BIT_AT(ACSR,ACI);
	
	//setting up multiplexer
	if(setup.use_multiplexer_)
	{
		_setupMultiplexer(setup.negative_input);
		_disableDigitalMultiplexerPins(setup.digit_multiplexer_pins_disabled_);
	}
	
	_setupComparatorPositiveInput(setup.positive_input_);
	_disableDigitalPins(setup.digit_pins_disabled_);
	
	//setting up interrupts
	if(setup.use_interrupts_)
		_setupInterrupts(setup.interrupt_mode_);
	
	if(setup.use_timer_triggering_)//timer will trigger comparator compare
		_enableTimer();
	
	if(setup.startup_enable_)
		COMPARATOR_powerOn();
	
}

ISR(ANALOG_COMP_vect)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if(COMPARATOR_InterruptHandler != NULL)
			COMPARATOR_InterruptHandler();
	}
}