#include "global_utils.h"
#include "twi_master.h"
#include <util/delay.h>

volatile uint16_t voltage;
#define SLAVE_ADDRESS 0b1//uzupe³niæ

void setupTwi()
{
	TWI_Setup twi = TWI_defaultSetup;
	twi.use_standard_twi_speed_ = true;
	twi.startup_enable_ = true;
	twi.standard_speed_ = TWI_100Kbps;
	TWI_init(twi);
}

void twi_com_master_main()
{
	char result[7];
	_delay_ms(1000);
	setupTwi();
	
	TWI_selectDeviceForSending(SLAVE_ADDRESS);
	TWI_sendByte_ACK('c');
	TWI_stopSequence();
	
	TWI_selectDeviceForSending(SLAVE_ADDRESS);
	TWI_sendByte_ACK('g');
	TWI_sendByte_ACK(0);
	TWI_sendByte_ACK(0);
	TWI_stopSequence();
	
	TWI_selectDeviceForSending(SLAVE_ADDRESS);
	TWI_sendByte_ACK('t');
	TWI_sendText_ACK("Voltage:");
	TWI_stopSequence();
	
	
	
	while(1)
	{
		TWI_selectDeviceForReceiving(SLAVE_ADDRESS);
		voltage = TWI_receiveByte_ACK();
	}	
}