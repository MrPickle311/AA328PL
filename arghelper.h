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

#define __CONCATENATE(arg1, arg2) __CONCATENATE2(arg1, arg2)
#define __CONCATENATE1(arg1, arg2) __CONCATENATE2(arg1, arg2)
#define __CONCATENATE2(arg1, arg2) arg1##arg2

#define __FOREACH_MACRO_RECURSION_1(what, x, ...) what(x)
#define __FOREACH_MACRO_RECURSION_2(what, x, ...) what(x)__FOREACH_MACRO_RECURSION_1(what, __VA_ARGS__)
#define __FOREACH_MACRO_RECURSION_3(what, x, ...) what(x)__FOREACH_MACRO_RECURSION_2(what, __VA_ARGS__)
#define __FOREACH_MACRO_RECURSION_4(what, x, ...) what(x)__FOREACH_MACRO_RECURSION_3(what, __VA_ARGS__)
#define __FOREACH_MACRO_RECURSION_5(what, x, ...) what(x)__FOREACH_MACRO_RECURSION_4(what, __VA_ARGS__)
#define __FOREACH_MACRO_RECURSION_6(what, x, ...) what(x)__FOREACH_MACRO_RECURSION_5(what, __VA_ARGS__)
#define __FOREACH_MACRO_RECURSION_7(what, x, ...) what(x)__FOREACH_MACRO_RECURSION_6(what, __VA_ARGS__)
#define __FOREACH_MACRO_RECURSION_8(what, x, ...) what(x)__FOREACH_MACRO_RECURSION_7(what, __VA_ARGS__)

#define __FOREACH_MACRO_RECURSION_NARG(...) __FOREACH_MACRO_RECURSION_NARG_(__VA_ARGS__, __FOREACH_MACRO_RECURSION_RSEQ_N())
#define __FOREACH_MACRO_RECURSION_NARG_(...) __FOREACH_MACRO_RECURSION_ARG_N(__VA_ARGS__)
#define __FOREACH_MACRO_RECURSION_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, N, ...) N
#define __FOREACH_MACRO_RECURSION_RSEQ_N() 8, 7, 6, 5, 4, 3, 2, 1, 0

#define __FOREACH_MACRO_RECURSION(N, what, x, ...) __CONCATENATE(__FOREACH_MACRO_RECURSION_, N)(what, x, __VA_ARGS__)
#define FOREACH_MACRO(what, x, ...) __FOREACH_MACRO_RECURSION(__FOREACH_MACRO_RECURSION_NARG(x, __VA_ARGS__), what, x, __VA_ARGS__)



#endif /* ARGHELPER_H_ */