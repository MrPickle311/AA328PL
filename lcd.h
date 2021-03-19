/*
 * lcd.h
 *
 * Created: 16/03/2021 23:04:49
 *  Author: Damian Wójcik
 */ 

#ifndef LCD_H_
#define LCD_H_

#include <stdbool.h>
#include <stdint.h>
#include "i2c_lib.h"
#include <stdlib.h>
#include <util/delay.h>

enum LCD_Flag
{
	RS = 0x1,
	RW = 0x2,
	EN = 0x4,
	BL = 0x8
};

#define INIT_4_BIT_MODE 0x2
#define INIT_8_BIT_MODE 0x30

#define LCD_CLEAR 0x1

enum LCD_Settings
{
	UnderlineOffBlinkOff = 0xC,
	UnderlineOffBlinkOn = 0xD,
	UnderlineOnBlinkOff = 0xE,
	UnderlineOnBlinkOn = 0xF
};

enum LCD_FirstCharLine
{
	Line1 = 0x80,
	Line2 = 0xC0	
};

typedef struct
{
	uint8_t top_line[17];
	uint8_t bottom_line[17];
	uint8_t address;
	bool blink_on;
}LCD_State;


void LCD_print(uint8_t addr, uint8_t data, uint8_t xpin)
{
	uint8_t splitted_data[4];
	
	
	splitted_data[0] = (data & 0xF0) | EN | xpin;
	splitted_data[1] = (data & 0xF0) | xpin;
	splitted_data[2] = (data << 4)   | EN | xpin;
	splitted_data[3] = (data << 4)   | xpin;
	
	I2C_sendStartAndSelect(addr | TW_WRITE);
	
	for(size_t i = 0; i < 4 ; ++i)
		I2C_sendByte(splitted_data[i]);
	
	I2C_stop();
	
	_delay_ms(5);
}

void LCD_clear(LCD_State* lcd_state)
{
	uint8_t xpin = 0;
	
	if(lcd_state->blink_on)
		xpin = BL;
	
	LCD_print(lcd_state->address,LCD_CLEAR,xpin);
}

void LCD_init(LCD_State* lcd_state)
{
	uint8_t xpin = 0;
	
	if(lcd_state->blink_on)
		xpin = BL;
	
	//unlock
	_delay_ms(40);
	LCD_print(lcd_state->address,INIT_8_BIT_MODE,xpin);
	_delay_ms(5);
	LCD_print(lcd_state->address,INIT_8_BIT_MODE,xpin);
	_delay_ms(1);
	LCD_print(lcd_state->address,INIT_8_BIT_MODE,xpin);
	
	//set 4-bit mode
	
	LCD_print(lcd_state->address,INIT_4_BIT_MODE,xpin);
	
	//set cursor mode 
	LCD_print(lcd_state->address,UnderlineOffBlinkOff,xpin);
	
	LCD_clear(lcd_state);
}

void LCD_display(LCD_State* lcd_state)
{
	uint8_t xpin = 0;
	uint8_t i = 0;
	
	if(lcd_state->blink_on)
		xpin = BL;
		
	LCD_clear(lcd_state);
	
	//first line
	
	LCD_print(lcd_state->address,Line1,xpin);//ok , im sending now 1 line
	while(lcd_state->top_line[i])
	{
		LCD_print(lcd_state->address,lcd_state->top_line[i], ( xpin | RS ) );
		++i;
	}
	
	//second line
	i = 0;
	LCD_print(lcd_state->address,Line2,xpin);//ok , im sending now 1 line
	while(lcd_state->bottom_line[i])
	{
		LCD_print(lcd_state->address,lcd_state->bottom_line[i], ( xpin | RS ) );
		++i;
	}
}



#endif /* LCD_H_ */