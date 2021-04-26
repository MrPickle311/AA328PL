/*
 * twi.h
 *
 * Created: 05/04/2021 22:13:24
 *  Author: Damian Wójcik
 */ 


#ifndef TWI_H_
#define TWI_H_

#include "bit_utils.h"
#include "global_utils.h"
#include <util/twi.h>


//more readable definitions
#define TWI_status TW_STATUS

#define TWI_stopBit														BIT_MASK_OF(TWSTO)
#define TWI_startBIT													BIT_MASK_OF(TWSTA)
#define TWI_collisionFLag												BIT_MASK_OF(TWWC)

#define TWI_startOperation()											REPLACE_REGISTER(TWCR, BIT_MASK_OF(TWINT) WITH BIT_MASK_OF(TWEN))
////setBitsAt(&TWCR,TWINT,TWEN)

#define TWI_sendToRegister(what)										TWDR = what
#define TWI_receive()													TWDR

enum TWI_StandardSpeed
{
	TWI_100Kbps,
	TWI_400Kbps
};

enum TWI_Error
{
	TWI_WithoutErrors,
	TWI_busSpeedError
};

volatile enum TWI_Error current_twi_error;

//waiting for elementary operations

#ifdef TWI_MASTER

//Basic

#define TWI_waitForOperationComplete()						 while (!(TWCR & _BV(TWINT)))
#define TWI_waitForStopBitSent()							 while (TWCR & _BV(TWSTO))

#define TWI_waitFor(what)									 while (TWI_status != what)

#define TWI_waitForStart_ACK()								 TWI_waitFor(TW_START)
#define TWI_waitForRepeatedStart_ACK()						 TWI_waitFor(TW_REP_START)

//Master Transmitter

#define TWI_MasterTransmitter_waitForSlaveAddressSent_ACK()	 TWI_waitFor(TW_MT_SLA_ACK)
#define TWI_MasterTransmitter_waitForSlaveAddressSent_NACK() TWI_waitFor(TW_MT_SLA_NACK)

#define TWI_MasterTransmitter_waitForByteSent_ACK()			 TWI_waitFor(TW_MT_DATA_ACK)
#define TWI_MasterTransmitter_waitForByteSent_NACK()		 TWI_waitFor(TW_MT_DATA_NACK)

#define TWI_MasterTransmitter_waitForArbitrationLost()		 TWI_waitFor(TW_MT_ARB_LOST)

//MasterReceiver

#define TWI_MasterReceiver_waitForSlaveAddressSent_ACK()	 TWI_waitFor(TW_MR_SLA_ACK)
#define TWI_MasterReceiver_waitForSlaveAddressSent_NACK()	 TWI_waitFor(TW_MR_SLA_NACK)

#define TWI_MasterReceiver_waitForByteReceived_ACK()		 TWI_waitFor(TW_MR_DATA_ACK)
#define TWI_MasterReceiver_waitForByteReceived_NACK()		 TWI_waitFor(TW_MR_DATA_NACK)

#define TWI_MasterReceiver_waitForArbitrationLost()		     TWI_waitFor(TW_MR_ARB_LOST)

struct TWI_Setup_struct
{
	bool startup_enable_;
	bool generate_acknowledge_signal_;
	
	uint16_t speed_;
	
	bool use_standard_twi_speed_;
	enum TWI_StandardSpeed standard_speed_;
};

typedef struct TWI_Setup_struct TWI_Setup;

extern volatile TWI_Setup TWI_defaultSetup;

void TWI_init(TWI_Setup setup);

//	low level TWI operations	//

//sending START bit

void TWI_startSequence_ACK();

void TWI_startSequence_NoACK();

void TWI_repeatStartSequence();

//sending address for writing

void TWI_sendDeviceAddressForSending_ACK(address_t address);

void TWI_sendDeviceAddressForSending_NoACK(address_t address);

void TWI_sendDeviceAddressForSending_NACK(address_t address);

//sending address for receiving

void TWI_sendDeviceAddreessForReceiving_ACK(address_t address);

void TWI_sendDeviceAddreessForReceiving_NoACK(address_t address);

void TWI_sendDeviceAddreessForReceiving_NACK(address_t address);

//sending bytes 

void TWI_sendByte_ACK(byte_t byte);

void TWI_sendByte_NoACK(byte_t byte);

void TWI_sendByte_NACK(byte_t byte);

//receiving bytes

byte_t TWI_receiveByte_NACK();

byte_t TWI_receiveByte_NoACK();

byte_t TWI_receiveByte_ACK();

//stop operations

void TWI_stopSequence();

//more High level operations


void TWI_selectDeviceForSending(address_t address);

void TWI_selectDeviceForReceiving(address_t address);


void TWI_sendByteToDeviceRegister(address_t device_address,
								  address_t register_address,
								  byte_t byte);

byte_t TWI_receiveByteFromDeviceRegister(address_t device_address,
										 address_t register_address);


//recognize all TWI devices and saves their addresses into array
void TWI_scanBus(address_t* adrresses,uint8_t expected_devices_nmbr);

#endif //TWI_MASTER

#endif /* TWI_H_ */