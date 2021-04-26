/*
 * bit_utils.c
 *
 * Created: 01/04/2021 00:38:48
 *  Author: Damian Wójcik
 */ 

#include "bit_utils.h"

void __setBitsAt(volatile uint8_t* target,uint8_t bits_count,...)
{
	va_list valist;
	va_start(valist,bits_count);
	
	uint8_t temp_mask = *target;
	
	for (uint8_t i = 0; i < bits_count ; ++i)
		SET_BIT_AT(temp_mask,va_arg(valist,size_t));

	*target = temp_mask;
	
	va_end(valist);
}

void __clearBitsAt(volatile uint8_t* target, uint8_t bits_count,...)
{
	va_list valist;
	va_start(valist,bits_count);
	
	uint8_t temp_mask = *target;
	
	for (uint8_t i = 0; i < bits_count ; ++i)
		CLEAR_BIT_AT(temp_mask,va_arg(valist,size_t));
	
	*target = temp_mask;
	
	va_end(valist);
}

void __reverseBitsAt(volatile uint8_t* target, uint8_t bits_count,...)
{
	va_list valist;
	va_start(valist,bits_count);
	
	uint8_t temp_mask = *target;
	
	for (uint8_t i = 0; i < bits_count ; ++i)
		REVERSE_BIT_AT(temp_mask,va_arg(valist,size_t));
	
	*target = temp_mask;
	
	va_end(valist);
}

uint8_t createBitMaskOfOnes(uint8_t from, uint8_t to)
{
	uint8_t temp = 0x0;
	for (uint8_t i = from; i <= to; i++)
		temp |= 1 << i;

	return temp;
}