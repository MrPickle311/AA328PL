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
#include "port.h"
#include "twi_master.h"
#include "examples/lcd.h"
#include "i2c_comunication_master.h"

/*
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
*/


/*MULTIPLEKSING
//global

#define EMPTY_LED 0

const uint8_t __flash led_numbers[] = 
{
	BIT_MASK_OF(0) WITH BIT_MASK_OF(1) WITH BIT_MASK_OF(2) WITH BIT_MASK_OF(5) WITH BIT_MASK_OF(6) WITH BIT_MASK_OF(7),						//0
	BIT_MASK_OF(2) WITH BIT_MASK_OF(7),																										//1
	BIT_MASK_OF(0) WITH BIT_MASK_OF(1) WITH BIT_MASK_OF(4) WITH BIT_MASK_OF(7) WITH BIT_MASK_OF(6),											//2
	BIT_MASK_OF(1) WITH BIT_MASK_OF(2) WITH BIT_MASK_OF(4) WITH BIT_MASK_OF(6) WITH BIT_MASK_OF(7),											//3
	BIT_MASK_OF(2) WITH BIT_MASK_OF(4) WITH BIT_MASK_OF(5) WITH BIT_MASK_OF(7),																//4
	BIT_MASK_OF(1) WITH BIT_MASK_OF(2) WITH BIT_MASK_OF(4) WITH BIT_MASK_OF(5) WITH BIT_MASK_OF(6),											//5
	BIT_MASK_OF(0) WITH BIT_MASK_OF(1) WITH BIT_MASK_OF(2) WITH BIT_MASK_OF(4) WITH BIT_MASK_OF(5) WITH BIT_MASK_OF(6),						//6
	BIT_MASK_OF(2) WITH BIT_MASK_OF(6) WITH BIT_MASK_OF(7),																					//7
	BIT_MASK_OF(0) WITH BIT_MASK_OF(1) WITH BIT_MASK_OF(2) WITH BIT_MASK_OF(4) WITH BIT_MASK_OF(5) WITH BIT_MASK_OF(6) WITH BIT_MASK_OF(7),	//8
	BIT_MASK_OF(2) WITH BIT_MASK_OF(4) WITH BIT_MASK_OF(5) WITH BIT_MASK_OF(6) WITH BIT_MASK_OF(7),											//9
	EMPTY_LED																																//none 
};

#define LED_NMBR 4

volatile uint16_t number = 0;
volatile char	  led_array[LED_NMBR];

//END global

void initTimer1()
{
	TIMER_16BitSetup timer1 = TIMER_16bit_DefaultSettings;
	timer1.mode_ = TIMER_16Bit_CTC_Output;
	timer1.prescaler_ = TIMER_Synchronous_Prescaler1024;
	timer1.interrupt_mode_ = TIMER_16Bit_TimerCompareMatchA;
	timer1.custom_output_compare_value_A_ = 15624;//desired time -> 1 second ; 16*10^6 / 1024 / 1 - 1
	TIMER_1_Init(timer1,false);
}

void initPorts()
{
	PORT_setPinsAsOutput(PORT_CONFIG(D),7,6,5,4,3,2,1,0);//number on led
	PORT_setPinsAsOutput(PORT_CONFIG(C),3,2,1,0);//led selection
	PORT_setLowPins(PORT_STATE(D),7,6,5,4,3,2,1,0);
	PORT_setLowPins(PORT_STATE(C),3,2,1,0);
}

void wipeCharArray()
{
	for(uint8_t i = 0 ; i < LED_NMBR ; ++i)
		led_array[i] = 'X';
}

void convertCharsToLedFormat()//TODO
{
	for(uint8_t i = 0 ; i < LED_NMBR ; ++i)
		if(led_array[i] != 'X' && led_array[i] != 0)//sprintf ends a string with \0 char
			led_array[i] -= '0';
		else led_array[i] = 10;
}
  
TIMER_CompareMatchA_Interrupt(1)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		++number;
		wipeCharArray();
		sprintf(led_array,"%d",number);
		convertCharsToLedFormat();
	}
}
*/


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
	PORT_setPinAsOutput(PORT_CONFIG(B),5);
	twi_com_master_main();
	LCD_State lcd;
	lcd.address = 0x27;
	lcd.blink_on = true;
	sprintf(&lcd.bottom_line,"");
	sprintf(&lcd.top_line,"");
	
	TWI_Master_Setup setup = TWI_Master_defaultSetup;
	setup.startup_generate_acknowledge_signal_ = true;
	setup.use_standard_twi_speed_ = true;
	setup.standard_speed_ = TWI_100Kbps;
	
	TWI0_Master_init(setup);
	
	//PORT_setPinHigh(PORT_STATE(B),5);
	//_delay_ms(1000);
	
	//PORT_setPinLow(PORT_STATE(B),5);
	
	LCD_init(&lcd);
	
	//PORT_setPinHigh(PORT_STATE(B),5);
	//_delay_ms(1000);
	//PORT_setPinLow(PORT_STATE(B),5);
	
	sprintf(&lcd.top_line,"xd");
	
	LCD_display(&lcd);
	
	/*MULTIPLEKSING
	initTimer1();
	
	initPorts();
	sei();
	uint8_t led_nr = 0;
	*/
	
	while(1)
	{
		/*MULTIPLEKSING
		for( led_nr = 0 ; led_nr < LED_NMBR ; ++led_nr)
		{
			PORT_clearPort(PORT_STATE(C));
			PORT_setPinHigh(PORT_STATE(C),led_nr);
			
			PORT_clearPort(PORT_STATE(D));
			PORT_setMask(PORT_STATE(D),led_numbers[led_array[led_nr]]);
			_delay_ms(2);	
		}
		*/
	}
}