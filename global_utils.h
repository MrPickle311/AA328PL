/*
 * global_utils.h
 *
 * Created: 03/04/2021 19:22:42
 *  Author: Damian Wójcik
 */ 


#ifndef GLOBAL_UTILS_H_
#define GLOBAL_UTILS_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "config.h"

typedef uint8_t pin_t;

typedef uint8_t mask_8bit_t;

typedef uint8_t byte_t;

typedef uint16_t double_byte_t;

typedef uint8_t length_t;

typedef uint8_t address_t;

typedef volatile uint8_t register_t;

typedef volatile uint8_t port_t;

enum OperationStatus
{
	Success,
	Failure
};

#define for_N(iterator_name,count_to)\
		for(uint8_t iterator_name = 0; iterator_name < count_to ;++iterator_name)

#endif /* GLOBAL_UTILS_H_ */