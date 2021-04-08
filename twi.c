/*
 * twi.c
 *
 * Created: 05/04/2021 23:36:06
 *  Author: Damian Wójcik
 */ 

#include "twi.h"
#include <avr/power.h>
#include "util/delay.h"

#define startElementaryOperation(operation_bit) setBitsAt(&TWCR,TWINT,TWEN,operation_bit)

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
		clearBitsAt(&TWSR,TWPS0,TWPS1);
		speed = (F_CPU / speed / 1000 - 16) / 2;
		uint8_t prescaler = 0;
		while (speed > 255)
		{
			++prescaler;
			speed /= 4;
		}
		REPLACE_REGISTER(TWSR, ( TWSR AND_MUST ( BIT_MASK_OF(TWPS0) WITH BIT_MASK_OF(TWPS1) ) ) WITH prescaler );
		REPLACE_REGISTER(TWBR,speed);
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
		SET_BIT_AT(TWCR,TWEA);
	if(setup->startup_enable_)
		SET_BIT_AT(TWCR,TWEN);
}

static inline void _setupPins()
{
	setBitsAt(&DDRC,DDC5,DDC4);
}

void TWI_init(TWI_Setup setup)
{
	power_twi_enable();
	
	_basicSetup(&setup);
	
	if(setup.use_standard_twi_speed_)
		_setStandardSpeed(&setup);
	else _setSpeed(setup.speed_);
	
	_setupPins();
	
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

#define prepareAddressToSend(address)\
		SHIFT_MASK_LEFT(address,1);

#define prepareAddressToReceive(address)\
		SHIFT_MASK_LEFT(address,1);\
		SET_BIT_AT(address,0)

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
		if(TWI_status == TW_MT_SLA_ACK)
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