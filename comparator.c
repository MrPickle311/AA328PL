/*
 * comparator.c
 *
 * Created: 13/04/2021 23:19:50
 *  Author: Damian Wójcik
 */ 

#include "comparator.h"
#include <avr/power.h>

void COMPARATOR_init(COMPARATOR_Setup setup)
{
	//startup kit to enabling comparator
	power_acb_enable();
	COMPARATOR_powerOn();
	SET_BIT_AT(ACSR,ACI);
}
