/*
 * bit_utils.h
 *
 * Created: 16/03/2021 23:12:03
 *  Author: Damian Wójcik
 */ 


#ifndef BIT_UTILS_H_
#define BIT_UTILS_H_

#include "arghelper.h"

#define BIT_MASK_OF(pos) ( 1 << pos )
#define NEGATIVE_BIT_MASK(pos) ~BIT_MASK_OF(pos)

#define SET_BIT_AT(target,pos) ( target |= BIT_MASK_OF(pos) )
#define CLEAR_BIT_AT(target,pos) ( target &= ~BIT_MASK_OF(pos)  )
 
 #define SET_SHIFTED_BIT_MASK(target,mask,shift) ( target |= ( mask << shift ) ) 
 
void __setBitsAt(volatile uint8_t* target,uint8_t bits_count,...)
{
	va_list valist;
	va_start(valist,bits_count);
	
	for (uint8_t i = 0; i < bits_count ; ++i)
		SET_BIT_AT(*target,va_arg(valist,size_t));
		
	va_end(valist);
}

#define setBitsAt(target, ...) __setBitsAt(target, PP_NARG(__VA_ARGS__), __VA_ARGS__)
 
#define TOGGLE_BIT_AT(target,pos) ( target ^= BIT_MASK_OF(pos) )

#define WRITE_BIT_AT(target,pos,value) ( value ? SET_BIT_AT(target,pos) : CLEAR_BIT_AT(target,pos) )

#define CHECK_BIT_AT(target,pos) ( ( target & BIT_MASK_OF(pos) ) != 0x0 )
#define IS_BIT_SET_AT(target,pos) CHECK_BIT_AT(target,pos)
#define IS_BIT_CLEARED_AT(target,pos) !IS_BIT_SET_AT(target,pos)

#endif /* BIT_UTILS_H_ */