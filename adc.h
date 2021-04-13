/*
 * adc.h
 *
 * Created: 12/04/2021 23:20:53
 *  Author: Damian Wójcik
 */ 


#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>
#include <stdbool.h>

enum ADC_InputPins
{
	ADC_ADC0 = 0b0000 ,
	ADC_ADC1 = 0b0001 ,
	ADC_ADC2 = 0b0010 ,
	ADC_ADC3 = 0b0011 ,
	ADC_ADC4 = 0b0100 ,
	ADC_ADC5 = 0b0101 ,
	ADC_1v1  = 0b1110 ,
	ADC_GND  = 0b1111
};

enum ADC_Prescaler
{
	ADC_Prescaler_2		= 0b000 ,
	ADC_Prescaler_4		= 0b010 ,
	ADC_Prescaler_8		= 0b011 ,
	ADC_Prescaler_16	= 0b100 ,
	ADC_Prescaler_32    = 0b101 ,
	ADC_Prescaler_64	= 0b110 ,
	ADC_Prescaler_128	= 0b111
};

enum ADC_VoltageReferenceSelection
{
	ADC_Vref_ReferencePin			 = 0b00 ,
	ADC_AVcc_ReferencePin			 = 0b01 ,
	ADC_InternalSource_Reference	 = 0b11
};

enum ADC_AutoTriggerSource
{
	ADC_FreeRunning				 = 0b000 ,
	ADC_AnalogComparator		 = 0b001 ,
	ADC_ExternalInterruptRequest = 0b010 ,
	ADC_Timer0_CompareMatchA	 = 0b011 ,
	ADC_Timer0_Overflow			 = 0b100 ,
	ADC_Timer1_CompareMatchB	 = 0b101 ,
	ADC_Timer1_Overflow			 = 0b110 ,
	ADC_Timer1_CaptureEvent		 = 0b111
};
 
enum ADC_DigitalPinsDisabled 
{
	ADC_Disable_ADC0 = 0b1 ,
	ADC_Disable_ADC1 = 0b10 ,
	ADC_Disable_ADC2 = 0b100 ,
	ADC_Disable_ADC3 = 0b1000 ,
	ADC_Disable_ADC4 = 0b10000 ,
	ADC_Disable_ADC5 = 0b100000
};

struct ADC_Setup_struct
{
	bool startup_enable_;// reg: ADCSRA , bit: ADEN
	
	// startup/(and custom) forceon conversion
	bool startup_force_conversion_;// reg: ADCSRA , bit: ADSC
	
	bool auto_trigger_enable_;// reg: ADCSRA , bit: ADATE
	
	bool enable_adc_interrupt_;// reg: ADCSRA , bit: ADIE
	
	enum ADC_Prescaler prescaler_;// reg: ADCSRA , bits: ADPS2-ADPS0
	
	
	bool startup_8bit_results_; // reg: ADMUX , bit: ADLAR
	
	enum ADC_InputPins input_pins_;// reg: ADMUX , bits: MUX0-3
	
	enum ADC_VoltageReferenceSelection voltage_reference_selection_;//  reg: ADMUX , bits: REFS1-REFS0
	
	
	
	enum ADC_AutoTriggerSource auto_trigger_source_;// reg: ADCSRB , bits: ADTS2-ADTS0
	
	
	enum ADC_DigitalPinsDisabled digital_pins_disabled_; //  reg: DIDR0 , bits: ADC0D-ADC5D
	
};

typedef struct ADC_Setup_struct ADC_Setup;

extern volatile ADC_Setup ADC_DefaultSettings;

//hard start-stop functions
void ADC_init(ADC_Setup setup);//init and turn on adc
void ADC_shutDown();//wiping all adc registers and turn off adc
void ADC_reconfigure(ADC_Setup setup);//turn off, reconfigure and turn on ADC

//the following functions can change a some ADC settings in runtime
void ADC_changeVoltageInput(enum ADC_InputPins pin);
void ADC_changePrescaler(enum ADC_Prescaler prescaler);
void ADC_changeAutoTriggerSource(enum ADC_AutoTriggerSource);
void ADC_changeVoltageReferenceSelection(enum ADC_VoltageReferenceSelection voltage_selection);

//interrupt managing at runtime
void ADC_enableInterrupt();
void ADC_disableInterrupt();

//halt-resume conversion
void ADC_resume();
void ADC_stop();

//manual conversion control and conversion status predicating
void ADC_startConversion();//conversion force start 
bool ADC_isConversionDone();
#define ADC_waitForConversionToFinish()   while( IS_BIT_CLEARED_AT( ADCSRA , ADIF ) )
#define ADC_resetConversionWaitingFlag()  SET_BIT_AT( ADCSRA  , ADIF )

//switching between 8bit and 10bit modes
void ADC_enable8BitConversionResults();
void ADC_enableNormalConversionResults();

//getting results 
uint16_t ADC_getConversionResult();//immediately get a current adc result 
uint16_t ADC_getConversionResultFrom(enum ADC_InputPins pin);//immediately get a current adc result from a channel/pin
uint16_t ADC_waitAndGetNextResult();//wait for a adc result and get it
uint16_t ADC_waitAndGetNextResultFrom(enum ADC_InputPins pin);//wait for a adc result and get it from a channel/pin

//user defined interrupt handler
void (*ADC_InterruptHandler)(uint16_t);

#endif /* ADC_H_ */