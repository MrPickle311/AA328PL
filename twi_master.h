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
#include "twi_status.h"

//several compatibility defines for a 328P
#ifndef _AVR_ATMEGA328PB_H_INCLUDED
	#undef  TWBR0
	#define TWDR0		TWDR
	#define TWCR0		TWCR
	#define TWSR0		TWSR
	#define TWBR0		TWBR
#endif

//more readable definitions
#define TWI0_Master_status TW0_STATUS

#ifdef _AVR_ATMEGA328PB_H_INCLUDED

#define TWI1_Master_status TW1_STATUS

#endif

#define TWI_stopBit														BIT_MASK_OF(TWSTO)
#define TWI_startBit													BIT_MASK_OF(TWSTA)
#define TWI_generateAcknowledgeBit
#define TWI_collisionFLag												BIT_MASK_OF(TWWC)
#define TWI_finishedOperationFlag										BIT_MASK_OF(TWINT)

enum TWI_Master_StandardSpeed
{
	TWI_100Kbps,
	TWI_400Kbps
};

//waiting for elementary operations

//Basic

#define TWI0_Master_waitForOperationComplete()				 while (!(TWCR0 & _BV(TWINT)))//ZAMASKOWAC
#define TWI0_Master_waitForStopBitSent()					 while (TWCR0 & _BV(TWSTO))//ZAMASKOWAC
															 
#define TWI0_Master_waitForStatus(status)					 while (TWI0_Master_status != status)
															 
#define TWI0_Master_waitForStart_ACK()						 TWI0_Master_waitForStatus(TW_START)
#define TWI0_Master_waitForRepeatedStart_ACK()				 TWI0_Master_waitForStatus(TW_REP_START)

#ifdef _AVR_ATMEGA328PB_H_INCLUDED

#define TWI1_Master_waitForOperationComplete()				 while (!(TWCR1 & _BV(TWINT)))//ZAMASKOWAC
#define TWI1_Master_waitForStopBitSent()					 while (TWCR1 & _BV(TWSTO))//ZAMASKOWAC
		   
#define TWI1_Master_waitForStatus(status)					 while (TWI1_Master_status != status)
		   
#define TWI1_Master_waitForStart_ACK()						 TWI1_Master_waitForStatus(TW_START)
#define TWI1_Master_waitForRepeatedStart_ACK()				 TWI1_Master_waitForStatus(TW_REP_START)

#endif

//Master Transmitter

#define TWI0_MasterTransmitter_waitForSlaveAddressSent_ACK()	 TWI0_Master_waitForStatus(TW_MT_SLA_ACK)
#define TWI0_MasterTransmitter_waitForSlaveAddressSent_NACK()	 TWI0_Master_waitForStatus(TW_MT_SLA_NACK)
		   
#define TWI0_MasterTransmitter_waitForByteSent_ACK()			 TWI0_Master_waitForStatus(TW_MT_DATA_ACK)
#define TWI0_MasterTransmitter_waitForByteSent_NACK()			 TWI0_Master_waitForStatus(TW_MT_DATA_NACK)
		   
#define TWI0_MasterTransmitter_waitForArbitrationLost()			 TWI0_Master_waitForStatus(TW_MT_ARB_LOST)

#ifdef _AVR_ATMEGA328PB_H_INCLUDED

#define TWI1_MasterTransmitter_waitForSlaveAddressSent_ACK()	 TWI1_Master_waitForStatus(TW_MT_SLA_ACK)
#define TWI1_MasterTransmitter_waitForSlaveAddressSent_NACK()	 TWI1_Master_waitForStatus(TW_MT_SLA_NACK)
		   															
#define TWI1_MasterTransmitter_waitForByteSent_ACK()			 TWI1_Master_waitForStatus(TW_MT_DATA_ACK)
#define TWI1_MasterTransmitter_waitForByteSent_NACK()			 TWI1_Master_waitForStatus(TW_MT_DATA_NACK)
		   															
#define TWI1_MasterTransmitter_waitForArbitrationLost()			 TWI1_Master_waitForStatus(TW_MT_ARB_LOST)

#endif

//MasterReceiver

#define TWI0_MasterReceiver_waitForSlaveAddressSent_ACK()		 TWI0_Master_waitForStatus(TW_MR_SLA_ACK)
#define TWI0_MasterReceiver_waitForSlaveAddressSent_NACK()		 TWI0_Master_waitForStatus(TW_MR_SLA_NACK)
		   														 
#define TWI0_MasterReceiver_waitForByteReceived_ACK()			 TWI0_Master_waitForStatus(TW_MR_DATA_ACK)
#define TWI0_MasterReceiver_waitForByteReceived_NACK()			 TWI0_Master_waitForStatus(TW_MR_DATA_NACK)
		   														 
#define TWI0_MasterReceiver_waitForArbitrationLost()		     TWI0_Master_waitForStatus(TW_MR_ARB_LOST)

#ifdef _AVR_ATMEGA328PB_H_INCLUDED

#define TWI1_MasterReceiver_waitForSlaveAddressSent_ACK()		 TWI1_Master_waitForStatus(TW_MR_SLA_ACK)
#define TWI1_MasterReceiver_waitForSlaveAddressSent_NACK()		 TWI1_Master_waitForStatus(TW_MR_SLA_NACK)

#define TWI1_MasterReceiver_waitForByteReceived_ACK()			 TWI1_Master_waitForStatus(TW_MR_DATA_ACK)
#define TWI1_MasterReceiver_waitForByteReceived_NACK()			 TWI1_Master_waitForStatus(TW_MR_DATA_NACK)

#define TWI1_MasterReceiver_waitForArbitrationLost()		     TWI1_Master_waitForStatus(TW_MR_ARB_LOST)

#endif

struct TWI_Master_Setup
{
	bool startup_enable_;
	bool startup_generate_acknowledge_signal_;
	
	uint16_t speed_;
	
	bool use_standard_twi_speed_;
	enum TWI_Master_StandardSpeed standard_speed_;
};

typedef struct TWI_Master_Setup TWI_Master_Setup;

extern volatile TWI_Master_Setup TWI_Master_defaultSetup;

void TWI0_Master_init(TWI_Master_Setup setup);

#ifdef _AVR_ATMEGA328PB_H_INCLUDED

void TWI1_Master_init(TWI_Master_Setup setup);

#endif

//	low level TWI operations	//

//sending START bit

void TWI0_startSequence_ACK();

void TWI0_startSequence_NoACK();

void TWI0_repeatStartSequence();

#ifdef _AVR_ATMEGA328PB_H_INCLUDED

void TWI1_startSequence_ACK();
		
void TWI1_startSequence_NoACK();
		
void TWI1_repeatStartSequence();

#endif

//sending address for writing

void TWI0_sendDeviceAddressForSending_ACK(address_t address);

void TWI0_sendDeviceAddressForSending_NoACK(address_t address);

void TWI0_sendDeviceAddressForSending_NACK(address_t address);

#ifdef _AVR_ATMEGA328PB_H_INCLUDED

void TWI1_sendDeviceAddressForSending_ACK(address_t address);
		
void TWI1_sendDeviceAddressForSending_NoACK(address_t address);
		
void TWI1_sendDeviceAddressForSending_NACK(address_t address);

#endif

//sending address for receiving

void TWI0_sendDeviceAddreessForReceiving_ACK(address_t address);

void TWI0_sendDeviceAddreessForReceiving_NoACK(address_t address);

void TWI0_sendDeviceAddreessForReceiving_NACK(address_t address);

#ifdef _AVR_ATMEGA328PB_H_INCLUDED

void TWI1_sendDeviceAddreessForReceiving_ACK(address_t address);

void TWI1_sendDeviceAddreessForReceiving_NoACK(address_t address);

void TWI1_sendDeviceAddreessForReceiving_NACK(address_t address);

#endif

//sending bytes 

void TWI0_sendByte_ACK(byte_t byte);

void TWI0_sendByte_NoACK(byte_t byte);

void TWI0_sendByte_NACK(byte_t byte);

#ifdef _AVR_ATMEGA328PB_H_INCLUDED

void TWI1_sendByte_ACK(byte_t byte);
		
void TWI1_sendByte_NoACK(byte_t byte);
		
void TWI1_sendByte_NACK(byte_t byte);

#endif

//sending series of data

void TWI0_sendByteSeries_ACK(byte_t* bytes,length_t count);

void TWI0_sendByteSeries_NoACK(byte_t* bytes,length_t count);

void TWI0_sendByteSeries_NACK(byte_t* bytes,length_t count);

#ifdef _AVR_ATMEGA328PB_H_INCLUDED

void TWI1_sendByteSeries_ACK(byte_t* bytes,length_t count);
		
void TWI1_sendByteSeries_NoACK(byte_t* bytes,length_t count);
		
void TWI1_sendByteSeries_NACK(byte_t* bytes,length_t count);

#endif

//sending text strings

void TWI0_sendText_ACK(char* text);

void TWI0_sendText_NoACK(char* text);

void TWI0_sendText_NACK(char* text);

#ifdef _AVR_ATMEGA328PB_H_INCLUDED

void TWI1_sendText_ACK(char* text);
		
void TWI1_sendText_NoACK(char* text);
		
void TWI1_sendText_NACK(char* text);

#endif

//receiving bytes

byte_t TWI0_receiveByte_ACK();

byte_t TWI0_receiveByte_NoACK();

byte_t TWI0_receiveByte_NACK();


#ifdef _AVR_ATMEGA328PB_H_INCLUDED

byte_t TWI1_receiveByte_NACK();
		  
byte_t TWI1_receiveByte_NoACK();
		  
byte_t TWI1_receiveByte_ACK();

#endif

//stop operations

void TWI0_stopSequence();

#ifdef _AVR_ATMEGA328PB_H_INCLUDED

void TWI1_stopSequence();

#endif

//more High level operations

void TWI0_selectDeviceForSending(address_t address);

void TWI0_waitUntilDeviceSelectForSending(address_t address);

void TWI0_selectDeviceForReceiving(address_t address);

void TWI0_waitUntilDeviceSelectForReceiving(address_t address);

void TWI0_sendByteToDeviceRegister(address_t device_address,
								  address_t register_address,
								  byte_t byte);

byte_t TWI0_receiveByteFromDeviceRegister(address_t device_address,
										 address_t register_address);

#ifdef _AVR_ATMEGA328PB_H_INCLUDED

void TWI1_selectDeviceForSending(address_t address);
 
void TWI1_waitUntilDeviceSelectForSending(address_t address);
 		
void TWI1_selectDeviceForReceiving(address_t address);

void TWI1_waitUntilDeviceSelectForReceiving(address_t address);

void TWI1_sendByteToDeviceRegister(address_t device_address,
								   address_t register_address,
								   byte_t byte);

byte_t TWI1_receiveByteFromDeviceRegister(address_t device_address,
										  address_t register_address);

#endif


//recognize all TWI devices and saves their addresses into an array
void TWI0_scanBus(address_t* adrresses,uint8_t expected_devices_nmbr);

#ifdef _AVR_ATMEGA328PB_H_INCLUDED

void TWI1_scanBus(address_t* adrresses,uint8_t expected_devices_nmbr);

#endif


#endif /* TWI_H_ */