/*
 * clock.c
 *
 * Created: 16/03/2021 21:22:49
 * Author : Damian Wójcik
 */ 
/*
#include <util/delay.h>
#include <avr/io.h>
#include "lcd.h"
#include <stdio.h>
#include "eeprom.h"

volatile LCD_State lcd;

int main(void)
{
	TWI_Setup setup;
	setup.generate_acknowledge_signal_ = true;
	setup.use_standard_twi_speed_ = true;
	setup.standard_speed_ = TWI_400Kbps;
	setup.startup_enable_ = true;
	TWI_init(setup);
*/
	/*
	lcd.address = 0x27;
	lcd.blink_on = true;
	LCD_init(&lcd);
	
	sprintf((char*)lcd.top_line,"Kochana XD");
	sprintf((char*)lcd.bottom_line,"Madzia XD");
	LCD_display(&lcd);
	*/
	/*
	address_t adr = 0x10;
	byte_t data = 30;
	EEPROM_write(data,adr);
	
	SET_BIT_AT(DDRB,DDB5);
	SET_BIT_AT(PORTB,DDB5);
	
    while (1) 
    {
		
    }
}
*/
	
#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"
#include "eeprom.h"
#include <stdio.h>

void i2c_int()
{
	TWSR = 0x00;
	TWBR = 0x0C;                                  // set the 400K
	TWCR = (1<<TWEN);                            // .kbv use =
}
uint8_t wait(void)                                  // .kbv make non-void
{
	while(!(TWCR& (1<<TWINT)));                 //  wait for next interrupt
	return (TWSR & 0xF8);                       // return status
}
uint8_t Str(void)
{
	TWCR = ((1<<TWINT)|(1<<TWSTA)|(1<<TWEN));   // .kbv use = . clear IRQ, start
	uint8_t status = wait();
	return (status == 0x08 || status == 0x18);  //START or REP_START
}
void sto(void)
{
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);;	// clear IRQ, stop condition
	while (TWCR & (1<<TWSTO)) ;                     // wait for completion
}

uint8_t write(unsigned int x)
{

	TWDR = x;
	TWCR = ((1<<TWINT)|(1<<TWEN));                  //clear IRQ, xmit data
	uint8_t status = wait();
	return status == 0x18 || status == 0x28 || status == 0x40 || status == 0x50; //ACK is true
}

uint8_t read()
{
	TWCR = ((1<<TWINT)|(1<<TWEN)|(0<<TWEA));         //NAK i.e. only one read
	uint8_t status = wait();                        //discard status
	return TWDR;                             // return received data
}

int main(void)
{
	TWI_Setup setup = TWI_defaultSetup;
	TWI_init(setup);
	
	byte_t out = 0b0;
	
	address_t pcf_adr = 0x20;
	
	LCD_State lcd;
	lcd.blink_on = true;
	lcd.address = 0x27;
	
	address_t* adr = malloc(sizeof(address_t)*2);
	TWI_scanBus(adr,2);
	
	sprintf(&lcd.top_line,"%i %i ",adr[0],adr[1]);
	sprintf(&lcd.bottom_line,"");
	
	LCD_init(&lcd);
	LCD_display(&lcd);
	
	PORTB |= _BV(DDB5);
	
	
	
	TWI_selectDeviceForSending(pcf_adr);
	
	while (1)
	{
		TWI_sendByte_ACK(out);
		out ^= 0b11;
		_delay_ms(1000);
	}
	TWI_stopSequence();
}
