#include "global_utils.h"
#include "twi_master.h"
#include <util/delay.h>

volatile uint16_t voltage;
#define SLAVE_ADDRESS 0b1//uzupe³niæ

void setupTwi()
{
	TWI_Master_Setup twi = TWI_Master_defaultSetup;
	twi.use_standard_twi_speed_ = true;
	twi.startup_enable_ = true;
	twi.standard_speed_ = TWI_100Kbps;
	TWI0_Master_init(twi);
}

void twi_com_master_main()
{
	char result[7];
	_delay_ms(1000);
	setupTwi();
	
	TWI0_selectDeviceForSending(SLAVE_ADDRESS);
	TWI0_sendByte_ACK('c');
	TWI0_stopSequence();
	
	TWI0_selectDeviceForSending(SLAVE_ADDRESS);
	TWI0_sendByte_ACK('g');
	TWI0_sendByte_ACK(0);
	TWI0_sendByte_ACK(0);
	TWI0_stopSequence();
	
	TWI0_selectDeviceForSending(SLAVE_ADDRESS);
	TWI0_sendByte_ACK('t');
	TWI0_sendText_ACK("Voltage:");
	TWI0_stopSequence();
	
	
	
	while(1)
	{
		TWI0_selectDeviceForReceiving(SLAVE_ADDRESS);
		voltage = TWI0_receiveByte_ACK();
	}	
}