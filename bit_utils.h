/*
 * bit_utils.h
 *
 * Created: 16/03/2021 23:12:03
 *  Author: Damian Wójcik
 */ 


#ifndef BIT_UTILS_H_
#define BIT_UTILS_H_

#include "arghelper.h"
#include <stdint.h>
// |= - set
// = replace

#define BIT_MASK_OF(pos) ( 1 << pos )
#define NEGATIVE_BIT_MASK(pos) ~BIT_MASK_OF(pos)

#define SET_BIT_AT(target,pos) ( target |= BIT_MASK_OF(pos) )
#define CLEAR_BIT_AT(target,pos) ( target &= ~BIT_MASK_OF(pos)  )

#define WIPE_REGISTER(target) target = 0x0
#define REPLACE_REGISTER(target,value) target = value
 
#define SHIFT_MASK(target,shift) ( mask =<< shift ) 
#define SET_SHIFTED_BIT_MASK(target,mask,shift) ( target |= ( mask << shift ) ) 
#define GET_SHIFTED_BIT_MASK_OF(target,shift) ( target << shift )
 
void __setBitsAt(volatile uint8_t* target,uint8_t bits_count,...);

void __clearBitsAt(volatile uint8_t* target,uint8_t bits_count,...);

uint8_t createBitMaskOfOnes(uint8_t from, uint8_t to);

#define WITH |
#define AND_MUST &

#define setBitsAt(target, ...) __setBitsAt(target, PP_NARG(__VA_ARGS__), __VA_ARGS__)
#define clearBitsAt(target, ...) __clearBitsAt(target, PP_NARG(__VA_ARGS__), __VA_ARGS__)
 
#define TOGGLE_BIT_AT(target,pos) ( target ^= BIT_MASK_OF(pos) )

#define WRITE_BIT_AT(target,pos,value) ( value ? SET_BIT_AT(target,pos) : CLEAR_BIT_AT(target,pos) )

#define EXTRACT_BIT_MASK_FROM(target,from,to)  ( target & createBitMaskOfOnes(from,to) )
#define FILTER_BIT_MASK(target,filter) ( target & filter )

#define CHECK_BIT_AT(target,pos) ( ( target & BIT_MASK_OF(pos) ) != 0x0 )
#define IS_BIT_SET_AT(target,pos) CHECK_BIT_AT(target,pos)
#define IS_BIT_CLEARED_AT(target,pos) !IS_BIT_SET_AT(target,pos)

#endif /* BIT_UTILS_H_ */