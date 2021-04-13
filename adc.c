/*
 * adc.c
 *
 * Created: 13/04/2021 09:35:35
 *  Author: Damian Wójcik
 */ 

#include "adc.h"
#include <avr/power.h>
#include "bit_utils.h"
#include <util/atomic.h>

//adc init implementations

static volatile uint16_t* result_source;

volatile ADC_Setup ADC_DefaultSettings = {
						true,
						false,
						false,
						false,
						ADC_Prescaler_2,
						false,
						ADC_1v1,//to ensure safety
						ADC_Vref_ReferencePin,
						ADC_FreeRunning,
						0b0//again , safety
	};

static inline void _setupPrescaler(enum ADC_Prescaler prescaler)
{
	//wiping current state
	clearBitsAt(&ADCSRA,ADPS2,ADPS1,ADPS0);
	//inject a new state
	SET_SHIFTED_BIT_MASK(ADCSRA,prescaler,0);
};

//disabling digital functions of several pins
static inline void _setupDisabledPins(uint8_t pins_mask)
{
	//wiping current state
	clearBitsAt(&DIDR0,ADC5D,ADC4D,ADC3D,ADC2D,ADC1D,ADC0D);
	//inject a new state
	SET_SHIFTED_BIT_MASK(DIDR0,pins_mask,0);
}

//selecting voltage reference
static inline void _setupVoltageReferenceSelection
					(enum ADC_VoltageReferenceSelection selection)
{
	//wiping current state
	clearBitsAt(&ADMUX,REFS1,REFS0);
	//inject a new state
	SET_SHIFTED_BIT_MASK(ADMUX,selection,6);
};

//choose a input pin by changing the multiplexer input
static inline void _setupMutliplexer(enum ADC_InputPins input_pin)
{
	//wiping current state
	clearBitsAt(&ADMUX,MUX3,MUX2,MUX1,MUX0);
	//inject a new state
	
	SET_SHIFTED_BIT_MASK(ADMUX,input_pin,0);
};

static inline void _setupAutoTriggerSource
					(enum ADC_AutoTriggerSource advanced_source)
{
	//wiping current state
	clearBitsAt(&ADCSRB,ADTS2,ADTS1,ADTS0);
	//inject a new state
	SET_SHIFTED_BIT_MASK(ADCSRB,advanced_source,0);
	SET_BIT_AT(ADCSRA,ADATE);
};

void ADC_init(ADC_Setup setup)
{
	//power turn on
	power_adc_enable();
	
	_setupVoltageReferenceSelection(setup.voltage_reference_selection_);
	_setupDisabledPins(setup.digital_pins_disabled_);
	_setupMutliplexer(setup.input_pins_);
	
	if(setup.startup_8bit_results_)
		ADC_enable8BitConversionResults();
	else ADC_enableNormalConversionResults();
	
	if(setup.auto_trigger_enable_)
		_setupAutoTriggerSource(setup.auto_trigger_source_);
	
	_setupPrescaler(setup.prescaler_);
	
	if(setup.enable_adc_interrupt_)
		ADC_enableInterrupt();
	else ADC_disableInterrupt();
	
	if(setup.startup_enable_)
		ADC_resume();
	else ADC_stop();
	
	if(setup.startup_force_conversion_ || setup.auto_trigger_enable_)
		ADC_startConversion();
	
}

void ADC_shutDown()
{
	power_adc_disable();
	
	WIPE_REGISTER(ADCSRA);
	WIPE_REGISTER(ADMUX);
	WIPE_REGISTER(DIDR0);
	clearBitsAt(&ADCSRB,ADTS2,ADTS1,ADTS0);
	
}

void ADC_reconfigure(ADC_Setup setup)
{
	ADC_shutDown();
	ADC_init(setup);
}

void ADC_changeVoltageInput(enum ADC_InputPins pin)
{
	_setupMutliplexer(pin);
}

void ADC_changePrescaler(enum ADC_Prescaler prescaler)
{
	_setupPrescaler(prescaler);
}

void ADC_changeAutoTriggerSource(enum ADC_AutoTriggerSource src)
{
	_setupAutoTriggerSource(src);
}

void ADC_changeVoltageReferenceSelection(enum ADC_VoltageReferenceSelection voltage_selection)
{
	_setupVoltageReferenceSelection(voltage_selection);
}

//enable adc interrupt
void ADC_enableInterrupt()
{
	SET_BIT_AT(ADCSRA,ADIE);
}

//disable adc interrupt
void ADC_disableInterrupt()
{
	CLEAR_BIT_AT(ADCSRA,ADIE);
}

//adc enable
void ADC_resume()
{
	SET_BIT_AT(ADCSRA,ADEN);
}

//adc stop
void ADC_stop()
{
	CLEAR_BIT_AT(ADCSRA,ADEN);
}

//force start conversion
void ADC_startConversion()
{
	SET_BIT_AT( ADCSRA ,ADSC );
}
ACME
bool ADC_isConversionDone()
{
	return IS_BIT_SET_AT( ADCSRA , ADIF );
}

void ADC_enable8BitConversionResults()
{
	result_source = &ADCH;//res source points to 8-bit value
	SET_BIT_AT(ADMUX,ADLAR);//setting up
}

void ADC_enableNormalConversionResults()
{
	result_source = &ADC;//res source points to 10-bit value
	CLEAR_BIT_AT(ADMUX,ADLAR);//setting up
}

uint16_t ADC_getConversionResult()
{
	return *result_source;
}

//this function does not wait for new adc value, so its unsafe
uint16_t ADC_getConversionResultFrom(enum ADC_InputPins pin)
{
	_setupMutliplexer(pin);
	return *result_source;
}

uint16_t ADC_waitAndGetNextResult()
{
	ADC_waitForConversionToFinish();
	ADC_resetConversionWaitingFlag();
	return *result_source;
}

//more safe version
uint16_t ADC_waitAndGetNextResultFrom(enum ADC_InputPins pin)
{
	_setupMutliplexer(pin);
	ADC_waitForConversionToFinish();
	ADC_resetConversionWaitingFlag();
	return *result_source;
}


ISR(ADC_vect)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if(ADC_InterruptHandler != NULL)
			ADC_InterruptHandler(*result_source);
	}
}
