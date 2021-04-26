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


//#include "lcd.h"
#include <stdio.h>

#define LEDDISPNO	4

volatile uint8_t LEDDIGITS[LEDDISPNO];

//Cyfry 0,1,2,3,4,5,6,7,8,9 i symbol -
static const uint8_t __flash DIGITS[11]={0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90, 0xBF};
const uint8_t DP=0x80;

struct ISR_Status
{
	uint8_t no : 4;
	uint8_t byte : 1;
};

static inline void SPI_Set_SS()
{
	PORTB|=_BV(PB2);
}

static inline void SPI_Reset_SS()
{
	PORTB&=~(_BV(PB2));
}
/*
ISR(SPI_STC_vect)
{
	static struct ISR_Status status;

	if(status.byte==0)
	{
		SPI_Set_SS();          //Przepisz zawartoœæ rejestru do zatrzasków wyjœciowych
		asm volatile ("nop");  //Konieczne ze wzglêdu na synchronizator
		SPI_Reset_SS();        //Wsuwamy now¹ wartoœæ
		uint8_t tmp=0xFF;
		uint8_t val=LEDDIGITS[status.no];  //Cyfra do wyœwietlenia
		if((val & 0x7F)<11) tmp=DIGITS[val & 0x7F]; //Jej reprezentacja na LED
		if(val & DP) tmp&=~(DP); //Kropka dziesiêtna
		SPDR=tmp;  //Wyœlij dane o wyœwietlanej cyfrze
	}
	else
	{
		SPDR=~(1<<status.no);	//Wybierz wyœwietlacz
		status.no=(status.no+1)%LEDDISPNO;
	}
	status.byte^=1;
}
*/
void SPI_master_init()
{
	SPI_Set_SS();
	DDRB|=(_BV(PB2) | _BV(PB3) | _BV(PB5));	//Piny SS, MOSI, SCK jako wyjœcie
	SPCR=_BV(SPIE) | _BV(SPE) | _BV(MSTR); //Tryb master, CLK/128, przerwania
	SPCR|=_BV(SPR1) | _BV(SPR0);
	SPSR;
	SPDR; //Skasuj flagê SPIF
}


void adc_init()
{
	ADMUX=_BV(REFS0)  | 0b0011 ; // Wew. referencyjne, kana³ 5, wyrównanie do prawej
	ADCSRA=_BV(ADEN) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
	// W³¹cz ADC, przerwania, preskaler 128
	ADCSRA|= _BV(ADATE) ;
	ADCSRA |=  _BV(ADSC);
}


#define NOOFSAMPLES 128
volatile uint32_t adc_val;

/*
ISR(ADC_vect)
{
	static uint32_t adc_accum;
	static uint8_t  sample_nmbr;
	
	adc_accum += ADC;
	
	++sample_nmbr;
	
	if(sample_nmbr == NOOFSAMPLES)
	{
		adc_val = adc_accum;
		adc_accum = 0;
		sample_nmbr = 0;
	}
}
*/

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

/*
void ADC_init()
{
	ADMUX |= _BV(REFS0);
	ADCSRA |= _BV(ADPS2);
	ADCSRA |= _BV(ADEN); 
}
*/

int main()
{
	/*
	SPI_master_init();

	sei();

	LEDDIGITS[0]=1;
	LEDDIGITS[1]=2;
	LEDDIGITS[2]=3;
	LEDDIGITS[3]=4;

	SPDR=0; //Zainicjuj przerwania SPI
	*/
	
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
}