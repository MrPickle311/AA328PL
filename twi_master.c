/*
 * twi.c
 *
 * Created: 05/04/2021 23:36:06
 *  Author: Damian W�jcik
 */ 

#include "twi_master.h"
#include <avr/power.h>
#include "util/delay.h"
#include <avr/power.h>

volatile uint8_t twi0_twcr_settings = 0x0;

#define TWI_startOperation()											REPLACE_REGISTER(TWCR0, BIT_MASK_OF(TWINT) WITH BIT_MASK_OF(TWEN))				

#define startElementaryOperation(operation_bit)							REPLACE_REGISTER(TWCR0,BIT_MASK_OF(TWINT) WITH BIT_MASK_OF(TWEN)  WITH BIT_MASK_OF(operation_bit))

#define TWI_sendToRegister(data)										REPLACE_REGISTER(TWDR0,data)
#define TWI_receive()													TWDR0

#define expectTWIStatus(expected_status,possible_twi_error)\
		if(TW_STATUS != expected_status)\
			current_twi_error = possible_twi_error

#ifdef TWI_MASTER

volatile TWI_Setup TWI_defaultSetup = {
						true,
						true,
						0,
						true,
						TWI_100Kbps
};

volatile enum TWI_Error current_twi_error = TWI_WithoutErrors;

static inline void _setSpeed(uint16_t speed)
{
	if(speed * 16 < F_CPU)
	{
		clearBitsAt(&TWSR0,TWPS0,TWPS1);
		speed = (F_CPU / speed / 1000 - 16) / 2;
		uint8_t prescaler = 0;
		while (speed > 255)
		{
			++prescaler;
			speed /= 4;
		}
		REPLACE_REGISTER(TWSR0, ( TWSR0 AND_MUST ( BIT_MASK_OF(TWPS0) WITH BIT_MASK_OF(TWPS1) ) ) WITH prescaler );
		REPLACE_REGISTER(TWBR0,speed);
	}
	else current_twi_error = TWI_busSpeedError;
}

static inline void _setStandardSpeed(TWI_Setup* setup)
{
	if(setup->standard_speed_ == TWI_100Kbps)
		_setSpeed(100);
	else if(setup->standard_speed_ == TWI_400Kbps)
		_setSpeed(400);
	else current_twi_error = TWI_busSpeedError;
}

static inline void _basicSetup(TWI_Setup* setup) 
{
	if(setup->generate_acknowledge_signal_)
		SET_BIT_AT(TWCR0,TWEA);
	if(setup->startup_enable_)
		SET_BIT_AT(TWCR0,TWEN);
	
	twi0_twcr_settings;
}

void TWI_init(TWI_Setup setup)
{
	//power_twi_enable();
	//PRR0 |= _BV(PRTWI0);
	
	_basicSetup(&setup);
	
	if(setup.use_standard_twi_speed_)
		_setStandardSpeed(&setup);
	else _setSpeed(setup.speed_);
	
}

//sending START bit

static inline void _startSequence_impl_()
{
	startElementaryOperation(TWSTA);
	TWI_waitForOperationComplete();	
}

void TWI_startSequence_ACK()
{
	_startSequence_impl_();
	TWI_waitForStart_ACK();//ACK
}

void TWI_startSequence_NoACK()
{
	_startSequence_impl_();
}

void TWI_repeatStartSequence()
{
	_startSequence_impl_();
	TWI_waitForRepeatedStart_ACK();
}

#define LSB 0

#define prepareAddressToSend(address)\
		SHIFT_MASK_LEFT(address,1);

#define prepareAddressToReceive(address)\
		SHIFT_MASK_LEFT(address,1);\
		SET_BIT_AT(address,LSB)

static inline void _sendAddress_impl(address_t address)
{
	TWI_sendToRegister(address);
	TWI_startOperation();
	TWI_waitForOperationComplete();
}

//sending address for writing

static inline void _sendDeviceAddressForSending_impl_(uint8_t address)
{
	prepareAddressToSend(address);
	_sendAddress_impl(address);	
}

void TWI_sendDeviceAddressForSending_ACK(address_t address)
{
	_sendDeviceAddressForSending_impl_(address);
	TWI_MasterTransmitter_waitForSlaveAddressSent_ACK();
}

void TWI_sendDeviceAddressForSending_NoACK(address_t address)
{
	_sendDeviceAddressForSending_impl_(address);
}

void TWI_sendDeviceAddressForSending_NACK(address_t address)
{
	_sendDeviceAddressForSending_impl_(address);
	TWI_MasterTransmitter_waitForSlaveAddressSent_NACK();
}

//sending address for receiving

static inline void _sendDeviceAddressForReceiving_impl_(uint8_t address)
{
	prepareAddressToReceive(address);
	_sendAddress_impl(address);
}

void TWI_sendDeviceAddreessForReceiving_ACK(address_t address)
{
	_sendDeviceAddressForReceiving_impl_(address);
	TWI_MasterReceiver_waitForSlaveAddressSent_ACK();
}

void TWI_sendDeviceAddreessForReceiving_NoACK(address_t address)
{
	_sendDeviceAddressForReceiving_impl_(address);
}

void TWI_sendDeviceAddreessForReceiving_NACK(address_t address)
{
	_sendDeviceAddressForReceiving_impl_(address);
	TWI_MasterReceiver_waitForSlaveAddressSent_NACK();
}

//sending bytes 

static inline void _sendByte_impl_(byte_t byte)
{
	TWI_sendToRegister(byte);
	TWI_startOperation();
	TWI_waitForOperationComplete();	
}

void TWI_sendByte_ACK(byte_t byte)
{
	_sendByte_impl_(byte);
	TWI_MasterTransmitter_waitForByteSent_ACK();
}

void TWI_sendByte_NoACK(byte_t byte)
{
	_sendByte_impl_(byte);
}

void TWI_sendByte_NACK(byte_t byte)
{
	_sendByte_impl_(byte);	
	TWI_MasterTransmitter_waitForByteSent_NACK();
}

//sending series of data

void TWI_sendByteSeries_ACK(byte_t* bytes,length_t count)
{
	length_t array_idx = 0;
	while(count--)
	{
		TWI_sendByte_ACK(bytes[array_idx]);
		++array_idx;	
	}
}

void TWI_sendByteSeries_NoACK(byte_t* bytes,length_t count)
{
	length_t array_idx = 0;
	while(count--)
	{
		TWI_sendByte_NoACK(bytes[array_idx]);
		++array_idx;
	}
}

void TWI_sendByteSeries_NACK(byte_t* bytes,length_t count)
{
	length_t array_idx = 0;
	while(count--)
	{
		TWI_sendByte_NACK(bytes[array_idx]);
		++array_idx;
	}
}

//sending text strings

void TWI_sendText_ACK(char* text)
{
	while(*text)
		TWI_sendByte_ACK(*text++);
	TWI_sendByte_ACK(0);//end of string
}

void TWI_sendText_NoACK(char* text)
{
	while(*text)
		TWI_sendByte_NoACK(*text++);
	TWI_sendByte_NoACK(0);//end of string
}

void TWI_sendText_NACK(char* text)
{
	while(*text)
		TWI_sendByte_NACK(*text++);
	TWI_sendByte_NACK(0);//end of string
}

//receiving bytes

inline static void _receiveByte_impl_()
{
	TWI_startOperation();
	TWI_waitForOperationComplete();	
}

byte_t TWI_receiveByte_ACK()
{
	_receiveByte_impl_();
	TWI_MasterReceiver_waitForByteReceived_ACK();
	return TWI_receive();
}

byte_t TWI_receiveByte_NoACK()
{
	_receiveByte_impl_();
	return TWI_receive();
}

byte_t TWI_receiveByte_NACK()
{
	_receiveByte_impl_();
	TWI_MasterReceiver_waitForByteReceived_NACK();
	return TWI_receive();
}

//stop operations

void TWI_stopSequence()
{
	startElementaryOperation(TWSTO);
	TWI_waitForStopBitSent();
}

//more High level operations

void TWI_selectDeviceForSending(address_t address)
{
	TWI_startSequence_ACK();
	TWI_sendDeviceAddressForSending_ACK(address);
}

void TWI_selectDeviceForReceiving(address_t address)
{
	TWI_startSequence_ACK();
	TWI_sendDeviceAddreessForReceiving_ACK(address);
}

void TWI_sendByteToDeviceRegister(address_t device_address,
								  address_t register_address,
								  byte_t byte)
{
	TWI_selectDeviceForSending(device_address);
	TWI_sendByte_ACK(register_address);
	TWI_sendByte_ACK(byte);
	TWI_stopSequence();
}

byte_t TWI_receiveByteFromDeviceRegister(address_t device_address,
										 address_t register_address)
{
	TWI_selectDeviceForSending(device_address);
	TWI_sendByte_ACK(register_address);
	TWI_selectDeviceForReceiving(device_address);
	byte_t temp = TWI_receiveByte_NACK();
	TWI_stopSequence();
	return temp;
}

void TWI_scanBus(address_t* addresses,uint8_t expected_devices_nmbr)
{
	uint8_t address = 0;
	uint8_t idx = 0;
	uint8_t devices_found = 0;
	while(address <= 255)
	{
		TWI_startSequence_NoACK();
		TWI_sendDeviceAddressForSending_NoACK(address);
		_delay_ms(5);
		if(TWI0_status == TW_MT_SLA_ACK)
		{
			addresses[idx] = address;
			++idx;
			if(++devices_found == expected_devices_nmbr)
				break;
		}
		TWI_stopSequence();
		++address;
	}
	TWI_stopSequence();
}

#endif //TWI_MASTER