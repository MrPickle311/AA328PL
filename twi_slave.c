/*
 * twi_slave.c
 *
 * Created: 04/05/2021 16:26:54
 *  Author: Damian Wójcik
 */ 

#include "twi_slave.h"
#include <avr/interrupt.h>
#include "twi_status.h"
#include "bit_utils.h"

#define TWI_resetBus(twi_nmbr)					REPLACE_REGISTER(TWCR##twi_nmbr , BIT_MASK_OF(TWEN) WITH \
																				  BIT_MASK_OF(TWEA) WITH \
																				  BIT_MASK_OF(TWIE) WITH \
																				  BIT_MASK_OF(TWINT))

#define TWI_setAddress(twi_nmbr, mask)			REPLACE_REGISTER(TWAR##twi_nmbr , mask)

#define TWI_setAddressMask(twi_nmbr, mask)		REPLACE_REGISTER(TWAMR##twi_nmbr , mask)

#define TWI_deteachDeviceFromBus(twi_nmbr)		CLEAR_BIT_AT(TWCR##twi_nmbr , TWEA)

#define TWI_generateACK(twi_nmbr)				SET_BIT_AT(TWCR##twi_nmbr , TWEA)

#define TWI_getByte(twi_nmbr)					TWDR##twi_nmbr

#define TWI_relaseSCL_Line(twi_nmbr)			SET_BIT_AT(TWCR##twi_nmbr , TWINT)

//sending bytes

#define TWI0_sendByte(byte)				TWDR0 = byte;

#ifdef _AVR_ATMEGA328PB_H_INCLUDED

#define TWI1_sendByte(byte)				TWDR1 = byte;

#endif

void TWI0_Slave_init(TWI_Slave_Setup setup)
{
	TWI_NormalBuffer_Init(&TWI0_Buffer, setup.buffer_size_);
	TWI_setAddressMask(0,setup.device_address_mask_);
	TWI_setAddress(0,setup.device_address_);
	TWI_resetBus(0);
}

void TWI0_emptyBuffer()
{
	TWI_Buffer_TotalReset(TWI0_Buffer);
	TWI_generateACK(0);
}

#ifdef _AVR_ATMEGA328PB_H_INCLUDED

void TWI1_Slave_init(TWI_Slave_Setup setup)
{
	TWI_NormalBuffer_Init(&TWI1_Buffer, setup.buffer_size_);
	TWI_setAddressMask(1,setup.device_address_mask_);
	TWI_setAddress(1,setup.device_address_);
	TWI_resetBus(1);
}

void TWI1_emptyBuffer()
{
	TWI_Buffer_TotalReset(TWI1_Buffer);
	TWI_generateACK(1);
}

#endif

//main slave service

ISR(TWI0_vect)
{
	switch(TW0_STATUS)
	{
		//Slave reciever part
		case TWI_SlaveReceiver_ReceiveRequest_ACKSent:
			TWI_Buffer_ResetBytes(TWI0_Buffer);
			TWI_Buffer_Set_DataIsReceived_Status(TWI0_Buffer);
			break;
		case TWI_SlaveReceiver_STOP_Or_RepeatedSTART_Received:
			if(TWI_Buffer_Is_DataReceived(TWI0_Buffer))//STOP received
			{
				TWI_Buffer_Set_DataIsReadyToProcess_Status(TWI0_Buffer);
				TWI_Buffer_Clear_DataIsReceived_Status(TWI0_Buffer);
				TWI_deteachDeviceFromBus(0);
			}
			else//repeated START received
			{
				TWI_generateACK(0);
				TWI_Buffer_ResetBytes(TWI0_Buffer);
				TWI_Buffer_Set_DataIsReceived_Status(TWI0_Buffer);
			}
			break;
			
#ifdef TWI0_GCALL_SERVICE//ADD HOOKS

			case TWI_SlaveReceiver_ReceiveRequest_BroadCastAddress_ACKSent:
			case TWI_SlaveReceiver_ByteReceived_BroadCastAddress_ACKSent:
			case TWI_SlaveReceiver_ByteReceived_BroadCastAddress_NACKSent:
			break;
			
#endif

#ifdef TWI0_ARBITRATION_LOST_SERVICE//ADD HOOKS

			case TWI_SlaveReceiver_ArbitrationLost:
			case TWI_SlaveReceiver_ArbitrationLost_BroadCastAddress:
			break;
			
#endif

		case TWI_SlaveReceiver_ByteReceived_ACKSent:
			TWI_Buffer_AtCurrentByte(TWI0_Buffer) = TWI_getByte(0);
			TWI_Buffer_GoNextByte(TWI0_Buffer);
			break;
		case TWI_SlaveReceiver_ByteReceived_NACKSent:
			TWI_Buffer_AtCurrentByte(TWI0_Buffer) = TWI_getByte(0);
			TWI_Buffer_GoNextByte(TWI0_Buffer);
			TWI_Buffer_Set_DataIsReadyToProcess_Status(TWI0_Buffer);
			TWI_Buffer_Clear_DataIsReceived_Status(TWI0_Buffer);
			TWI_deteachDeviceFromBus(0);
			break;
		
		//Slave transmitter part
		
		case TWI_SlaveTransmitter_TransmitRequest_ACKSent:
			TWI_Buffer_ResetBytes(TWI0_Buffer);
			TWI_Buffer_Set_DataIsTransmitted_Status(TWI0_Buffer);
			//Here should not be break statement!
		case TWI_SlaveTransmitter_ByteSent_ACKReceived:
			TWI0_sendByte(TWI0_SendByteHandler());
			TWI_Buffer_GoNextByte(TWI0_Buffer);
			break;
		case TWI_SlaveTransmitter_ByteSent_NACKReceived:
			TWI_Buffer_Clear_DataIsTransmitted_Status(TWI0_Buffer);
			break;
		case TWI_SlaveTransmitter_LastByteSent_ACKReceived: 
			break;

#if TWI0_ARBITRATION_LOST_SERVICE//ADD HOOKS

		case TWI_SlaveTransmitter_ArbitrationError:
			break;
	
#endif

		//Error service
		
		default:
			TWI_resetBus(0);
			TWI_Buffer_TotalReset(TWI0_Buffer);
			break;
		
	}
	//Buffer overflow control
	if(TWI_Buffer_OverflowOccured(TWI0_Buffer))
		TWI_Buffer_ResetBytes(TWI0_Buffer);
	TWI_relaseSCL_Line(0);
}

#ifdef _AVR_ATMEGA328PB_H_INCLUDED

ISR(TWI1_vect)
{
	switch(TW1_STATUS)
	{
		//Slave reciever part
		case TWI_SlaveReceiver_ReceiveRequest_ACKSent:
			TWI_Buffer_ResetBytes(TWI1_Buffer);
			TWI_Buffer_Set_DataIsReceived_Status(TWI1_Buffer);
			break;
		case TWI_SlaveReceiver_STOP_Or_RepeatedSTART_Received:
			if(TWI_Buffer_Is_DataReceived(TWI1_Buffer))//STOP received
			{
				TWI_Buffer_Set_DataIsReadyToProcess_Status(TWI1_Buffer);
				TWI_Buffer_Clear_DataIsReceived_Status(TWI1_Buffer);
				TWI_deteachDeviceFromBus(1);
			}
			else//repeated START received
			{
				TWI_generateACK(1);
				TWI_Buffer_ResetBytes(TWI1_Buffer);
				TWI_Buffer_Set_DataIsReceived_Status(TWI1_Buffer);
				}
			break;
		
#ifdef TWI1_GCALL_SERVICE//ADD HOOKS

		case TWI_SlaveReceiver_ReceiveRequest_BroadCastAddress_ACKSent:
		case TWI_SlaveReceiver_ByteReceived_BroadCastAddress_ACKSent:
		case TWI_SlaveReceiver_ByteReceived_BroadCastAddress_NACKSent:
		break;
		
#endif

#ifdef TWI1_ARBITRATION_LOST_SERVICE//ADD HOOKS

		case TWI_SlaveReceiver_ArbitrationLost:
		case TWI_SlaveReceiver_ArbitrationLost_BroadCastAddress:
		break;
		
#endif

		case TWI_SlaveReceiver_ByteReceived_ACKSent:
			TWI_Buffer_AtCurrentByte(TWI1_Buffer) = TWI_getByte(1);
			TWI_Buffer_GoNextByte(TWI1_Buffer);
			break;
		case TWI_SlaveReceiver_ByteReceived_NACKSent:
			TWI_Buffer_AtCurrentByte(TWI1_Buffer) = TWI_getByte(1);
			TWI_Buffer_GoNextByte(TWI1_Buffer);
			TWI_Buffer_Set_DataIsReadyToProcess_Status(TWI1_Buffer);
			TWI_Buffer_Clear_DataIsReceived_Status(TWI1_Buffer);
			TWI_deteachDeviceFromBus(1);
			break;
		
		//Slave transmitter part
		
		case TWI_SlaveTransmitter_TransmitRequest_ACKSent:
			TWI_Buffer_ResetBytes(TWI1_Buffer);
			TWI_Buffer_Set_DataIsTransmitted_Status(TWI1_Buffer);
			//Here should not be break statement!
		case TWI_SlaveTransmitter_ByteSent_ACKReceived:
			TWI1_sendByte(TWI1_SendByteHandler());
			TWI_Buffer_GoNextByte(TWI1_Buffer);
			break;
		case TWI_SlaveTransmitter_ByteSent_NACKReceived:
			TWI_Buffer_Clear_DataIsTransmitted_Status(TWI1_Buffer);
			break;
		case TWI_SlaveTransmitter_LastByteSent_ACKReceived:
			break;

#if TWI1_ARBITRATION_LOST_SERVICE//ADD HOOKS

		case TWI_SlaveTransmitter_ArbitrationError:
			break;
		
#endif

		//Error service
		
		default:
			TWI_resetBus(1);
			TWI_Buffer_TotalReset(TWI1_Buffer);
			break;
		
	}
	//Buffer overflow control
	if(TWI_Buffer_OverflowOccured(TWI1_Buffer))
		TWI_Buffer_ResetBytes(TWI1_Buffer);
	TWI_relaseSCL_Line(1);
}

#endif