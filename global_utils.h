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

typedef uint8_t byte_t;

typedef uint8_t length_t;

enum OperationStatus
{
	Success,
	Failure
};



#endif /* GLOBAL_UTILS_H_ */