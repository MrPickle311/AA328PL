/*
 * clock.c
 *
 * Created: 16/03/2021 21:22:49
 * Author : Damian Wójcik
 */ 

#include <avr/io.h>
#define F_CPU		    16000000UL
#define I2CBUSCLOCK		50000UL
#include "i2c_lib.h"
#include "lcd.h"
#include <stdio.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "ports.h"
#include "external_interrupt.h"
#include "timer.h"

volatile LCD_State lcd;
volatile int counter = 0;

advancedInterruptPin2PortD
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		sprintf((char*)lcd.top_line,"Kochana %i",counter);
		LCD_display(&lcd);
		++counter;
	}	
}

ISR(TIMER0_COMPA_vect)
{
	if(isPinLow(PORT_STATE(D),4))
		enablePin(PORT_STATE(D),4);
	else disablePin(PORT_STATE(D),4);
} 

ISR(TIMER0_OVF_vect)
{
	if(isPinLow(PORT_STATE(D),4))
		enablePin(PORT_STATE(D),4);
	else disablePin(PORT_STATE(D),4);
}

int main(void)
{
	/*
	I2C_init();

	lcd.address = 0x27 << 1;
	lcd.blink_on = true;
	LCD_init(&lcd);
	
	sprintf((char*)lcd.top_line,"Kochana");
	sprintf((char*)lcd.bottom_line,"Madzia");
	LCD_display(&lcd);
	
	PortSetup info;//poprawiæ to, bo siê 
	initPortSetup(&info,PORT_STATE(D),PORT_CONFIG(D));//zmieñ nazwê na port set,albo utwórz strukturê portset
	setPinAsInput(&info,PullUp,2);//tu nie musi byæ pullup,gdy¿ jest wy¿ej
	
	setupAdvancedInterrupt(Pin2PortDAdvancedInterrupt,ActivatedByFallingEdge);
	*/
	//TCCR0A=(_BV(COM0A0) | _BV(COM0B0));
	//TCCR0B |= _BV(FOC0A);
	//OCR1A=F_CPU/2024/1;
	//OCR0A = 50;
	//TCCR0B |=_BV(WGM02) | _BV(CS02) | _BV(CS00); // Preskaler 1024, CTC
	setupTimer0(TimerPrescaler1024,Timer0Normal,Timer0Overflow,TimerNotControlPin,None,128);
	setPinsAsOutput(PORT_CONFIG(D),6,5,4);
	enableInterrupts();
    while (1) 
    {
		
    }
}

