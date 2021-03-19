/*
 * clock.c
 *
 * Created: 16/03/2021 21:22:49
 * Author : Damian W�jcik
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

int main(void)
{
	I2C_init();

	lcd.address = 0x27 << 1;
	lcd.blink_on = true;
	LCD_init(&lcd);
	
	sprintf((char*)lcd.top_line,"Kochana");
	sprintf((char*)lcd.bottom_line,"Madzia");
	LCD_display(&lcd);
	
	PortSetup info;//poprawi� to, bo si� 
	initPortSetup(&info,PORT_STATE(D),PORT_CONFIG(D));//zmie� nazw� na port set,albo utw�rz struktur� portset
	setPinAsInput(&info,PullUp,2);//tu nie musi by� pullup,gdy� jest wy�ej
	
	setupAdvancedInterrupt(Pin2PortDAdvancedInterrupt,ActivatedByFallingEdge);

	enableInterrupts();
    while (1) 
    {

    }
}

