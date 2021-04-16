/*
 * comparator.h
 *
 * Created: 13/04/2021 21:10:18
 *  Author: Damian Wójcik
 */ 


#ifndef COMPARATOR_H_
#define COMPARATOR_H_

#include "bit_utils.h"
#include <stdbool.h>

//following macro clears comparator interruption flag by setting 1
#define COMPARATOR_clearInterruptionFlag() SET_BIT_AT(ACSR,ACI);

//hardware clears ACI bit when comparator interruption occured
#define COMPARATOR_interruptionOccured() IS_BIT_CLEARED_AT(ACSR,ACI)

//when the voltage on the
//positive pin AIN0 is higher than the voltage on the 
//negative pin AIN1, the analog comparator output, ACO, is set
#define COMPARATOR_getResult() ACO

enum COMPARATOR_InterruptMode
{
	COMPRARTOR_BothEdges   = 0b00,
	COMPARATOR_FallingEdge = 0b10,
	COMPARATOR_RisingEdge  = 0b11
};

enum COMPARATOR_PositiveInput
{
	COMPARATOR_PositiveInput_AIN0 = 0b0,
	COMPARATOR_PositiveInput_InternalReferenceVoltage = 0b1
};

enum COMPARATOR_MultiplexerNegativeInput
{
	COMPARATOR_ADC0 = 0b000,
	COMPARATOR_ADC1 = 0b001,
	COMPARATOR_ADC2 = 0b010,
	COMPARATOR_ADC3 = 0b011,
	COMPARATOR_ADC4 = 0b100,
	COMPARATOR_ADC5 = 0b101
};

enum COMPARATOR_MultiplexerDigitalPinsDisabled
{
	COMPARATOR_Disable_ADC0 = 0b1 ,
	COMPARATOR_Disable_ADC1 = 0b10 ,
	COMPARATOR_Disable_ADC2 = 0b100 ,
	COMPARATOR_Disable_ADC3 = 0b1000 ,
	COMPARATOR_Disable_ADC4 = 0b10000 ,
	COMPARATOR_Disable_ADC5 = 0b100000
};

enum COMPARATOR_DigitalPinsDisabled
{
	COMPARATOR_disable_AIN0 = 0b1,
	COMPARATOR_disable_AIN1 = 0b10
};

struct COMPARATOR_Setup_struct
{
	bool use_multiplexer_;//reg: ADCSRB , bit: ACME,disable ADC
	
	enum COMPARATOR_MultiplexerNegativeInput negative_input;//reg: ADMUX , bits: MUX2-0
	
	
	enum COMPARATOR_PositiveInput positive_input_;//reg: ACSR , bit : ACBG
	
	
	bool use_interrupts_;//reg: ACSR , bit ACIE
	
	enum COMPARATOR_InterruptMode interrupt_mode_;//reg: ACSR , bits: ACIS1,ACIS0
	
	
	bool use_timer_triggering_;//reg: ACSR , bit: ACIC
	
	
	enum COMPARATOR_DigitalPinsDisabled digit_pins_disabled_;//reg: DIDR1, bits: AIN1, AIN0
	
	enum COMPARATOR_MultiplexerDigitalPinsDisabled digit_multiplexer_pins_disabled_;//reg DIDR0 , bits: ADC0-5


	bool startup_enable_;//reg: ACSR bit: ACD
};

typedef struct COMPARATOR_Setup_struct COMPARATOR_Setup;

//manual disabling and enabling(power on / power off) comparator
#define COMPARATOR_powerOn()	CLEAR_BIT_AT(ACSR,ACD)
#define COMPARATOR_powerOff()	SET_BIT_AT(ACSR,ACD)

void COMPARATOR_init(COMPARATOR_Setup setup);


volatile void (*COMPARATOR_InterruptHandler)();

#endif /* COMPARATOR_H_ */