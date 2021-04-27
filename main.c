/*
 * clock.c
 *
 * Created: 16/03/2021 21:22:49
 * Author : Damian Wójcik
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "timer.h"
#include "util/atomic.h"
#include "adc.h"
#include <stdfix-gcc.h>
#include "comparator.h"
#include "usart.h"
#include "port.h"

#define NOOFSAMPLES 128
volatile uint32_t adc_val;

volatile bool delay_flag = false;

void my_handler(uint16_t value)
{
	static uint32_t adc_accum;
	static uint8_t  sample_nmbr;
	
	adc_accum += value;
	
	++sample_nmbr;
	
	if(sample_nmbr == NOOFSAMPLES)
	{
		adc_val = adc_accum;
		adc_accum = 0;
		sample_nmbr = 0;
	}
}

void getVoltage(uint8_t* first,uint16_t* last)
{
	ATOMIC_BLOCK(ATOMIC_FORCEON) 
	{ 
		float temp = (float)(( ( 5 * adc_val ) / NOOFSAMPLES )  ) /1024; 
		*first =  temp; 
		*last  =  ( ( temp *1000 - ( *first *1000 ) ) ); 
	}
}

void delay()//funkcja realizuj¹ca opóŸnienie
{
	while(delay_flag == false);
	delay_flag = false;
}

ISR (TIMER1_COMPA_vect)
{
	delay_flag = true;//przerwij pêtlê while w funkcji delay
}

int main()
{
	/*
	
	//Timer1Setup timer = Timer1_DefaultSettings;
	//timer.mode_ = Timer16Bit_CTC_Input;
	//timer.edge_mode_ = Timer16Bit_InputCaptureRisingEdge;
	//timer.pins_under_control_ = Timer_OnlyPinA;
	//timer.pin_A_mode_ = 
	
	//LCD_State lcd;
	//lcd.address = 0x27;
	//lcd.blink_on = true;
	//sprintf(&lcd.bottom_line,"");
	//sprintf(&lcd.top_line,"");
	
	//TWI_Setup setup = TWI_defaultSetup;
	//setup.generate_acknowledge_signal_ = true;
	//setup.use_standard_twi_speed_ = true;
	//setup.standard_speed_ = TWI_100Kbps;
	
	//TWI_init(setup);
	
	//LCD_init(&lcd);
	
	//sprintf(&lcd.top_line,"xd");
	
	//LCD_display(&lcd);
	
	//adc_init();	
	
	ADC_Setup adc = ADC_DefaultSettings;
	
	adc.auto_trigger_enable_ = true;
	adc.digital_pins_disabled_ = ADC_Disable_ADC3;
	adc.startup_force_conversion_  = true;
	adc.voltage_reference_selection_ = ADC_AVcc_ReferencePin;
	adc.input_pins_ = ADC_ADC3;
	adc.startup_enable_ = true;
	adc.prescaler_ = ADC_Prescaler_128;
	adc.auto_trigger_enable_ = true;
	adc.enable_adc_interrupt_ = true;
	adc.auto_trigger_source_ = ADC_FreeRunning;
	
	ADC_InterruptHandler = &my_handler;
	
	ADC_init(adc);

	uint8_t first = 0;
	uint16_t last  = 0;
	
	sei();
	
	while(1)
	{
		getVoltage(&first,&last);
		//sprintf(&lcd.top_line,"vol : %d,%d V",first,last);
		//LCD_display(&lcd);
		_delay_ms(500);
	}
	*/
	
	TIMER_16BitSetup timer1 = TIMER_16bit_DefaultSettings;
	timer1.mode_ = TIMER_16Bit_CTC_Output;
	timer1.prescaler_ = TIMER_Synchronous_Prescaler1024;
	timer1.interrupt_mode_ = TIMER_16Bit_TimerCompareMatchA;
	timer1.custom_output_compare_value_A_ = 15624*0.1;

	TIMER_1Init(timer1,false);
	
	PORT_setPinAsOutput(PORT_CONFIG(B),5);
	PORT_setPinLow(PORT_STATE(B),5);
	//sei();
	
	while(1)
	{
		//delay();
		TIMER_waitForCompareMathA_Interrupt(1);
		TIMER_resetCompareMathA_InterruptFlag(1);
	    //while ( (TIFR1 & (1 << OCF1A) ) == 0);//tak siê oczekuje
		//TIFR1 |= (1 << OCF1A);
		PORT_invertPin(PORT_STATE(B),5);
	}
}