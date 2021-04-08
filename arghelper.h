/*
 * arghelper.h
 *
 * Created: 17/03/2021 09:12:39
 *  Author: Damian Wójcik
 */ 

#ifndef ARGHELPER_H_
#define ARGHELPER_H_

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>

#define PP_NARG(...) \
PP_NARG_(__VA_ARGS__,PP_RSEQ_N())

#define PP_NARG_(...) \
PP_16TH_ARG(__VA_ARGS__)

#define PP_16TH_ARG( \
_1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
_11,_12,_13,_14,_15,N,...) N

#define PP_RSEQ_N() \
15,14,13,12,11,10, \
9,8,7,6,5,4,3,2,1,0

//foreach macros

#define SET_BITS_AT(target,...) 

#endif /* ARGHELPER_H_ */