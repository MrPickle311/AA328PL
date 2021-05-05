/*
 * twi.c
 *
 * Created: 05/04/2021 23:36:06
 *  Author: Damian Wójcik
 */ 

#include "twi_master.h"
#include <avr/power.h>
#include "util/delay.h"
#include <avr/power.h>
#include <assert.h>

#define _startOperation(twi_nmbr)							REPLACE_REGISTER(TWCR##twi_nmbr , BIT_MASK_OF(TWINT) WITH BIT_MASK_OF(TWEN))				

#define _startElementaryOperation(twi_nmbr,operation_bit)	REPLACE_REGISTER(TWCR##twi_nmbr ,  \
																						 BIT_MASK_OF(TWINT) WITH \
																						 BIT_MASK_OF(TWEN)  WITH \
																						 BIT_MASK_OF(operation_bit))

#define TWI0_sendToRegister(data)										REPLACE_REGISTER(TWDR0, data)
#define TWI0_receive()													TWDR0

#ifdef _AVR_ATMEGA328PB_H_INCLUDED

#define TWI1_sendToRegister(data)										REPLACE_REGISTER(TWDR1, data)
#define TWI1_receive()													TWDR1

#endif

volatile TWI_Master_Setup TWI_Master_defaultSetup = {
						true,
						true,
						0,
						true,
						TWI_100Kbps
};

static inline void _setSpeed_impl(register_t* twi_status_register,
								  register_t* twi_bit_rate_register,
								  uint16_t speed)
{
	if(speed * 16 < F_CPU)
	{
		clearBitsAt(twi_status_register,TWPS0,TWPS1);
		speed = (F_CPU / speed / 1000 - 16) / 2;
		uint8_t prescaler = 0;
		while (speed > 255)
		{
			++prescaler;
			speed /= 4;
		}
		REPLACE_REGISTER(*twi_status_register, 
						(*twi_status_register AND_MUST ( BIT_MASK_OF(TWPS0) WITH BIT_MASK_OF(TWPS1) ) ) WITH prescaler );
		REPLACE_REGISTER(*twi_bit_rate_register, speed);
	}
	else assert(false);//if not speed * 16 < F_CPU force halt all program
}

#define _setSpeed(twi_nmbr,speed) _setSpeed_impl(&TWSR##twi_nmbr , \
												 &TWBR##twi_nmbr , \
												 speed)
//MAGIC NUMBERS
static inline void _twi0_setStandardSpeed(TWI_Master_Setup* setup)
{
	if(setup->standard_speed_ == TWI_100Kbps)
		_setSpeed(0, 100);
	else if(setup->standard_speed_ == TWI_400Kbps)
		_setSpeed(0, 400);
}

#ifdef _AVR_ATMEGA328PB_H_INCLUDED

static inline void _twi1_setStandardSpeed(TWI_Master_Setup* setup)
{
	if(setup->standard_speed_ == TWI_100Kbps)
	_setSpeed(1, 100);
	else if(setup->standard_speed_ == TWI_400Kbps)
	_setSpeed(1, 400);
}

#endif

static inline void _basicSetup_impl(register_t* twi_control_register,
								    TWI_Master_Setup* setup) 
{
	if(setup->startup_generate_acknowledge_signal_)
		SET_BIT_AT(*twi_control_register,TWEA);
	if(setup->startup_enable_)
		SET_BIT_AT(*twi_control_register,TWEN);
}

#define _basicSetup(twi_nmbr, setup_ptr) _basicSetup_impl(&TWCR##twi_nmbr , setup_ptr)

void TWI0_Master_init(TWI_Master_Setup setup)
{
	//power_twi_enable();
	//PRR0 |= _BV(PRTWI0);
	
	_basicSetup(0,&setup);
	
	if(setup.use_standard_twi_speed_)
		_twi0_setStandardSpeed(&setup);
	else _setSpeed(0,setup.speed_);
	
}

#ifdef _AVR_ATMEGA328PB_H_INCLUDED

void TWI1_Master_init(TWI_Master_Setup setup)
{
	_basicSetup(1,&setup);
	
	if(setup.use_standard_twi_speed_)
		_twi1_setStandardSpeed(&setup);
	else _setSpeed(1,setup.speed_);
}

#endif

//sending START bit

static inline void _twi0_startSequence_impl_()
{
	_startElementaryOperation(0,TWSTA);
	TWI0_Master_waitForOperationComplete();	
}

void TWI0_startSequence_ACK()
{
	_twi0_startSequence_impl_();
	TWI0_Master_waitForStart_ACK();//ACK
}

void TWI0_startSequence_NoACK()
{
	_twi0_startSequence_impl_();
}

void TWI0_repeatStartSequence()
{
	_twi0_startSequence_impl_();
	TWI0_Master_waitForRepeatedStart_ACK();
}

#ifdef _AVR_ATMEGA328PB_H_INCLUDED

static inline void _twi1_startSequence_impl_()
{
	_startElementaryOperation(1,TWSTA);
	TWI0_Master_waitForOperationComplete();
}

void TWI1_startSequence_ACK()
{
	_twi1_startSequence_impl_();
	TWI1_Master_waitForStart_ACK();
}

void TWI1_startSequence_NoACK()
{
	_twi1_startSequence_impl_();
}

void TWI1_repeatStartSequence()
{
	_twi1_startSequence_impl_();
	TWI1_Master_waitForRepeatedStart_ACK();
}

#endif

#define LSB 0

#define prepareAddressToSend(address)\
		SHIFT_MASK_LEFT(address,1);

#define prepareAddressToReceive(address)\
		SHIFT_MASK_LEFT(address,1);\
		SET_BIT_AT(address,0)

static inline void _twi0_sendAddress_impl(address_t address)
{
	TWI0_sendToRegister(address);
	_startOperation(0);
	TWI0_Master_waitForOperationComplete();
}

//sending address for writing

static inline void _twi0_sendDeviceAddressForSending_impl_(address_t address)
{
	prepareAddressToSend(address);
	_twi0_sendAddress_impl(address);	
}

void TWI0_sendDeviceAddressForSending_ACK(address_t address)
{
	_twi0_sendDeviceAddressForSending_impl_(address);
	TWI0_MasterTransmitter_waitForSlaveAddressSent_ACK();
}

void TWI0_sendDeviceAddressForSending_NoACK(address_t address)
{
	_twi0_sendDeviceAddressForSending_impl_(address);
}

void TWI0_sendDeviceAddressForSending_NACK(address_t address)
{
	_twi0_sendDeviceAddressForSending_impl_(address);
	TWI0_MasterTransmitter_waitForSlaveAddressSent_NACK();
}

#ifdef _AVR_ATMEGA328PB_H_INCLUDED

static inline void _twi1_sendAddress_impl(address_t address)
{
	TWI1_sendToRegister(address);
	_startOperation(1);
	TWI1_Master_waitForOperationComplete();
}

static inline void _twi1_sendDeviceAddressForSending_impl_(address_t address)
{
	prepareAddressToSend(address);
	_twi1_sendAddress_impl(address);
}

void TWI1_sendDeviceAddressForSending_ACK(address_t address)
{
	_twi1_sendDeviceAddressForSending_impl_(address);
	TWI1_MasterTransmitter_waitForSlaveAddressSent_ACK();
}

void TWI1_sendDeviceAddressForSending_NoACK(address_t address)
{
	_twi1_sendDeviceAddressForSending_impl_(address);
}

void TWI1_sendDeviceAddressForSending_NACK(address_t address)
{
	_twi1_sendDeviceAddressForSending_impl_(address);
	TWI1_MasterTransmitter_waitForSlaveAddressSent_NACK();
}

#endif

//sending address for receiving

static inline void _twi0_sendDeviceAddressForReceiving_impl_(address_t address)
{
	prepareAddressToReceive(address);
	_twi0_sendAddress_impl(address);
}

void TWI0_sendDeviceAddreessForReceiving_ACK(address_t address)
{
	_twi0_sendDeviceAddressForReceiving_impl_(address);
	TWI0_MasterReceiver_waitForSlaveAddressSent_ACK();
}

void TWI0_sendDeviceAddreessForReceiving_NoACK(address_t address)
{
	_twi0_sendDeviceAddressForReceiving_impl_(address);
}

void TWI0_sendDeviceAddreessForReceiving_NACK(address_t address)
{
	_twi0_sendDeviceAddressForReceiving_impl_(address);
	TWI0_MasterReceiver_waitForSlaveAddressSent_NACK();
}

#ifdef _AVR_ATMEGA328PB_H_INCLUDED

static inline void _twi1_sendDeviceAddressForReceiving_impl_(address_t address)
{
	prepareAddressToReceive(address);
	_twi1_sendAddress_impl(address);
}

void TWI1_sendDeviceAddreessForReceiving_ACK(address_t address)
{
	_twi1_sendDeviceAddressForReceiving_impl_(address);
	TWI1_MasterReceiver_waitForSlaveAddressSent_ACK();
}

void TWI1_sendDeviceAddreessForReceiving_NoACK(address_t address)
{
	_twi1_sendDeviceAddressForReceiving_impl_(address);
}

void TWI1_sendDeviceAddreessForReceiving_NACK(address_t address)
{
	_twi1_sendDeviceAddressForReceiving_impl_(address);
	TWI1_MasterReceiver_waitForSlaveAddressSent_NACK();
}

#endif
//sending bytes 

static inline void _twi0_sendByte_impl_(byte_t byte)
{
	TWI0_sendToRegister(byte);
	_startOperation(0);
	TWI0_Master_waitForOperationComplete();	
}

void TWI0_sendByte_ACK(byte_t byte)
{
	_twi0_sendByte_impl_(byte);
	TWI0_MasterTransmitter_waitForByteSent_ACK();
}

void TWI0_sendByte_NoACK(byte_t byte)
{
	_twi0_sendByte_impl_(byte);
}

void TWI0_sendByte_NACK(byte_t byte)
{
	_twi0_sendByte_impl_(byte);	
	TWI0_MasterTransmitter_waitForByteSent_NACK();
}

#ifdef _AVR_ATMEGA328PB_H_INCLUDED

static inline void _twi1_sendByte_impl_(byte_t byte)
{
	TWI1_sendToRegister(byte);
	_startOperation(1);
	TWI1_Master_waitForOperationComplete();
}

void TWI1_sendByte_ACK(byte_t byte)
{
	_twi1_sendByte_impl_(byte);
	TWI1_MasterTransmitter_waitForByteSent_ACK();
}

void TWI1_sendByte_NoACK(byte_t byte)
{
	_twi1_sendByte_impl_(byte);
}

void TWI1_sendByte_NACK(byte_t byte)
{
	_twi1_sendByte_impl_(byte);
	TWI1_MasterTransmitter_waitForByteSent_NACK();
}

#endif
//sending series of data
//MOZE PRZENIESC IMPLEMENTACJE DO JEDNEJ FUNKCJI !!!
//WSKAZNIKI DO FUNKCJI
void TWI0_sendByteSeries_ACK(byte_t* bytes,length_t count)
{
	length_t array_idx = 0;
	while(count--)
	{
		TWI0_sendByte_ACK(bytes[array_idx]);
		++array_idx;	
	}
}

void TWI0_sendByteSeries_NoACK(byte_t* bytes,length_t count)
{
	length_t array_idx = 0;
	while(count--)
	{
		TWI0_sendByte_NoACK(bytes[array_idx]);
		++array_idx;
	}
}

void TWI0_sendByteSeries_NACK(byte_t* bytes,length_t count)
{
	length_t array_idx = 0;
	while(count--)
	{
		TWI0_sendByte_NACK(bytes[array_idx]);
		++array_idx;
	}
}

#ifdef _AVR_ATMEGA328PB_H_INCLUDED

void TWI1_sendByteSeries_ACK(byte_t* bytes,length_t count)
{
	length_t array_idx = 0;
	while(count--)
	{
		TWI1_sendByte_ACK(bytes[array_idx]);
		++array_idx;
	}
}

void TWI1_sendByteSeries_NoACK(byte_t* bytes,length_t count)
{
	length_t array_idx = 0;
	while(count--)
	{
		TWI1_sendByte_NoACK(bytes[array_idx]);
		++array_idx;
	}
}

void TWI1_sendByteSeries_NACK(byte_t* bytes,length_t count)
{
	length_t array_idx = 0;
	while(count--)
	{
		TWI1_sendByte_NACK(bytes[array_idx]);
		++array_idx;
	}
}

#endif
//sending text strings

void TWI0_sendText_ACK(char* text)
{
	while(*text)
		TWI0_sendByte_ACK(*text++);
	TWI0_sendByte_ACK(0);//end of string
}

void TWI0_sendText_NoACK(char* text)
{
	while(*text)
		TWI0_sendByte_NACK(*text++);
	TWI0_sendByte_NACK(0);//end of string
}

void TWI0_sendText_NACK(char* text)
{
	while(*text)
		TWI0_sendByte_NACK(*text++);
	TWI0_sendByte_NACK(0);//end of string
}

#ifdef _AVR_ATMEGA328PB_H_INCLUDED

void TWI1_sendText_ACK(char* text)
{
	while(*text)
		TWI1_sendByte_ACK(*text++);
	TWI1_sendByte_ACK(0);//end of string
}

void TWI1_sendText_NoACK(char* text)
{
	while(*text)
		TWI1_sendByte_NACK(*text++);
	TWI1_sendByte_NACK(0);//end of string
}

void TWI1_sendText_NACK(char* text)
{
	while(*text)
		TWI1_sendByte_NACK(*text++);
	TWI1_sendByte_NACK(0);//end of string
}

#endif

//receiving bytes

inline static void _twi0_receiveByte_impl_()
{
	_startOperation(0);
	TWI0_Master_waitForOperationComplete();	
}

byte_t TWI0_receiveByte_ACK()
{
	_startElementaryOperation(0,TWEA);//send ACK to slave that master has received data
	TWI0_Master_waitForOperationComplete();	
	TWI0_MasterReceiver_waitForByteReceived_ACK();
	return TWI0_receive();
}

byte_t TWI0_receiveByte_NoACK()
{
	_twi0_receiveByte_impl_();
	return TWI0_receive();
}

byte_t TWI0_receiveByte_NACK()
{
	_twi0_receiveByte_impl_();
	TWI0_MasterReceiver_waitForByteReceived_NACK();
	return TWI0_receive();
}

#ifdef _AVR_ATMEGA328PB_H_INCLUDED

inline static void _twi1_receiveByte_impl_()
{
	_startOperation(1);
	TWI1_Master_waitForOperationComplete();
}

byte_t TWI1_receiveByte_NACK()
{
	_startElementaryOperation(1,TWEA);//send ACK to slave that master has received data
	TWI1_Master_waitForOperationComplete();
	TWI1_MasterReceiver_waitForByteReceived_ACK();
	return TWI1_receive();
}

byte_t TWI1_receiveByte_NoACK()
{
	_twi1_receiveByte_impl_();
	return TWI1_receive();
}

byte_t TWI1_receiveByte_ACK()
{
	_twi1_receiveByte_impl_();
	TWI1_MasterReceiver_waitForByteReceived_NACK();
	return TWI1_receive();
}

#endif

//stop operations

void TWI0_stopSequence()
{
	_startElementaryOperation(0,TWSTO);
	TWI0_Master_waitForStopBitSent();
}

#ifdef _AVR_ATMEGA328PB_H_INCLUDED

void TWI1_stopSequence()
{
	_startElementaryOperation(1,TWSTO);
	TWI1_Master_waitForStopBitSent();
}

#endif

//more High level operations

void TWI0_selectDeviceForSending(address_t address)
{
	TWI0_startSequence_ACK();
	TWI0_sendDeviceAddressForSending_ACK(address);
}

void TWI0_waitUntilDeviceSelectForSending(address_t address)
{
	do
	{
		TWI0_startSequence_NoACK();
		_twi0_sendDeviceAddressForSending_impl_(address);
	}TWI0_MasterTransmitter_waitForSlaveAddressSent_ACK();
}

void TWI0_selectDeviceForReceiving(address_t address)
{
	TWI0_startSequence_ACK();
	TWI0_sendDeviceAddreessForReceiving_ACK(address);
}

void TWI0_waitUntilDeviceSelectForReceiving(address_t address)
{
	do
	{
		TWI0_startSequence_NoACK();
		_twi0_sendDeviceAddressForReceiving_impl_(address);
	}TWI0_MasterReceiver_waitForSlaveAddressSent_ACK();
}

void TWI0_sendByteToDeviceRegister(address_t device_address,
								  address_t register_address,
								  byte_t byte)
{
	TWI0_selectDeviceForSending(device_address);
	TWI0_sendByte_ACK(register_address);
	TWI0_sendByte_ACK(byte);
	TWI0_stopSequence();
}

byte_t TWI0_receiveByteFromDeviceRegister(address_t device_address,
										  address_t register_address)
{
	TWI0_selectDeviceForSending(device_address);
	TWI0_sendByte_ACK(register_address);
	TWI0_selectDeviceForReceiving(device_address);
	byte_t temp = TWI0_receiveByte_NACK();
	TWI0_stopSequence();
	return temp;
}

#ifdef _AVR_ATMEGA328PB_H_INCLUDED

void TWI1_selectDeviceForSending(address_t address)
{
	TWI1_startSequence_ACK();
	TWI1_sendDeviceAddressForSending_ACK(address);
}

void TWI1_waitUntilDeviceSelectForSending(address_t address)
{
	do
	{
		TWI1_startSequence_NoACK();
		_twi1_sendDeviceAddressForSending_impl_(address);
	}TWI1_MasterTransmitter_waitForSlaveAddressSent_ACK();
}

void TWI1_selectDeviceForReceiving(address_t address)
{
	TWI1_startSequence_ACK();
	TWI1_sendDeviceAddreessForReceiving_ACK(address);
}

void TWI1_waitUntilDeviceSelectForReceiving(address_t address)
{
	do
	{
		TWI1_startSequence_NoACK();
		_twi1_sendDeviceAddressForReceiving_impl_(address);
	}TWI1_MasterReceiver_waitForSlaveAddressSent_ACK();
}

void TWI1_sendByteToDeviceRegister(address_t device_address, 
								   address_t register_address, 
								   byte_t byte)
{
	TWI1_selectDeviceForSending(device_address);
	TWI1_sendByte_ACK(register_address);
	TWI1_sendByte_ACK(byte);
	TWI1_stopSequence();
}

byte_t TWI1_receiveByteFromDeviceRegister(address_t device_address, 
										  address_t register_address)
{
	TWI1_selectDeviceForSending(device_address);
	TWI1_sendByte_ACK(register_address);
	TWI1_selectDeviceForReceiving(device_address);
	byte_t temp = TWI0_receiveByte_NACK();
	TWI1_stopSequence();
	return temp;
}

#endif

//recognize all TWI devices and saves their addresses into an array

void TWI0_scanBus(address_t* addresses,uint8_t expected_devices_nmbr)
{
	address_t address = 0;
	uint8_t idx = 0;
	uint8_t devices_found = 0;
	while(address <= 255)
	{
		TWI0_startSequence_NoACK();
		TWI0_sendDeviceAddressForSending_NoACK(address);
		_delay_ms(5);
		if(TWI0_Master_status == TW_MT_SLA_ACK)
		{
			addresses[idx] = address;
			++idx;
			if(++devices_found == expected_devices_nmbr)
				break;
		}
		TWI0_stopSequence();
		++address;
	}
	TWI0_stopSequence();
}

#ifdef _AVR_ATMEGA328PB_H_INCLUDED

void TWI1_scanBus(address_t* adrresses,uint8_t expected_devices_nmbr)
{
	address_t address = 0;
	uint8_t idx = 0;
	uint8_t devices_found = 0;
	while(address <= 255)
	{
		TWI0_startSequence_NoACK();
		TWI0_sendDeviceAddressForSending_NoACK(address);
		_delay_ms(5);
		if(TWI1_Master_status == TW_MT_SLA_ACK)
		{
			adrresses[idx] = address;
			++idx;
			if(++devices_found == expected_devices_nmbr)
				break;
		}
		TWI1_stopSequence();
		++address;
	}
	TWI1_stopSequence();
}

#endif