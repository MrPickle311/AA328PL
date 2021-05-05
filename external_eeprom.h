/*
 * eeprom.h
 *
 * Created: 06/04/2021 21:46:57
 *  Author: Damian Wójcik
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_

#include "global_utils.h"

volatile uint8_t eeprom_basic_address = 0b01010000;//0x50
volatile uint8_t eeprom_new = 0b10100000;
#include "twi.h"
#define I2CBUSCLOCK		50000UL
#include "i2c_lib.h"

void EEPROM_sendAddress_withoutStop(address_t address)
{
	TWI0_startSequence_NoACK();
	TWI0_sendDeviceAddressForSending_ACK(eeprom_basic_address);
	TWI0_sendByte_ACK(address);
}

void EEPROM_writeSingleByte(byte_t byte,address_t address)
{
	EEPROM_sendAddress_withoutStop(address);
	TWI0_sendByte_ACK(byte);
	TWI0_stopSequence();
	_delay_ms(5);
}

byte_t EEPROM_readByte(address_t address)
{
	byte_t data = 0;
	
	EEPROM_sendAddress_withoutStop(address);
	
	TWI0_stopSequence();
	
	TWI0_startSequence_NoACK();
	TWI0_sendDeviceAddreessForReceiving_ACK(eeprom_basic_address);
	data = TWI0_receiveByte_NACK();
	
	TWI0_stopSequence();
	return data;
}

//deprecated or repair
void EEPROM_write8Bytes(byte_t* data,address_t start_address)
{
	EEPROM_sendAddress_withoutStop(start_address);
	for_N(i,8)
	{
		TWI0_sendByte_ACK(data[i]);
		_delay_ms(5);
	}
	
	TWI0_stopSequence();
}

void EEPROM_writeNBytes(byte_t* data,length_t size,address_t start_address)
{
	for_N(i,size)
	{
		EEPROM_writeSingleByte(data[i],start_address);
		++start_address;
	}
}

//deprecated or repair
byte_t* EEPROM_read8Bytes(address_t start_address)
{
	byte_t* data = malloc(sizeof(byte_t)*8);
	
	EEPROM_sendAddress_withoutStop(start_address);
	
	TWI0_startSequence_NoACK();
	TWI0_sendDeviceAddreessForReceiving_ACK(eeprom_basic_address);
	
	for_N(i,8)
	{
		//if(i == 7)
			data[i] = TWI0_receiveByte_NACK();
		//else data[i] = TWI_receiveByte_ACK();
	}
	
	TWI0_stopSequence();
	return data;
}

byte_t* EEPROM_readNBytes(address_t start_address,length_t count)
{
	byte_t* data = malloc(sizeof(byte_t)*count);
	
	for_N(i,count)
	{
		data[i] = EEPROM_readByte(start_address);
		++start_address;
	}
	
	return data;
}

#endif /* EEPROM_H_ */