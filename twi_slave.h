/*
 * twi_slave.h
 *
 * Created: 04/05/2021 16:26:29
 *  Author: Damian Wójcik
 */ 


#ifndef TWI_SLAVE_H_
#define TWI_SLAVE_H_

#include "twi_byte_buffer.h"
#include "bit_utils.h"
#include "twi_status.h"

//user has full access to buffers

volatile TWI_NormalBuffer TWI0_Buffer;

#ifdef _AVR_ATMEGA328PB_H_INCLUDED

volatile TWI_NormalBuffer TWI1_Buffer;

#endif

//slave transmitter definitions

#define TWI_SlaveTransmitter_TransmitRequest_ACKSent						TW_ST_SLA_ACK//start transmition slave -> master,

#define TWI_SlaveTransmitter_ArbitrationError								TW_ST_ARB_LOST_SLA_ACK

#define TWI_SlaveTransmitter_ByteSent_ACKReceived							TW_ST_DATA_ACK			

#define TWI_SlaveTransmitter_ByteSent_NACKReceived							TW_ST_DATA_NACK

#define TWI_SlaveTransmitter_LastByteSent_ACKReceived						TW_ST_LAST_DATA

//slave receiver definitions

#define TWI_SlaveReceiver_ReceiveRequest_ACKSent							TW_SR_SLA_ACK

#define TWI_SlaveReceiver_ArbitrationLost									TW_SR_ARB_LOST_SLA_ACK

#define TWI_SlaveReceiver_ReceiveRequest_BroadCastAddress_ACKSent			TW_SR_GCALL_ACK

#define TWI_SlaveReceiver_ArbitrationLost_BroadCastAddress					TW_SR_ARB_LOST_GCALL_ACK

#define TWI_SlaveReceiver_ByteReceived_ACKSent								TW_SR_DATA_ACK

#define TWI_SlaveReceiver_ByteReceived_NACKSent								TW_SR_DATA_NACK

#define TWI_SlaveReceiver_ByteReceived_BroadCastAddress_ACKSent				TW_SR_GCALL_DATA_ACK

#define TWI_SlaveReceiver_ByteReceived_BroadCastAddress_NACKSent			TW_SR_GCALL_DATA_NACK

#define TWI_SlaveReceiver_STOP_Or_RepeatedSTART_Received					TW_SR_STOP

struct TWI_Slave_Setup
{
	address_t	device_address_;
	mask_8bit_t device_address_mask_;
	length_t	buffer_size_;
}; 

typedef struct TWI_Slave_Setup TWI_Slave_Setup;

void TWI0_Slave_init(TWI_Slave_Setup setup);

void TWI0_emptyBuffer();


#ifdef _AVR_ATMEGA328PB_H_INCLUDED

void TWI1_Slave_init(TWI_Slave_Setup setup);

void TWI1_emptyBuffer();


#endif

//sending bytes hooks
//IT MUST BE DEFINED !!!
byte_t (*TWI0_SendByteHandler)(void);

#ifdef _AVR_ATMEGA328PB_H_INCLUDED

byte_t (*TWI1_SendByteHandler)(void);

#endif

#endif /* TWI_SLAVE_H_ */