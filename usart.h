/*
 * usart.h
 *
 * Created: 03/04/2021 09:38:44
 *  Author: Damian Wójcik
 */ 


#ifndef USART_H_
#define USART_H_ 

#include <stdbool.h>
#include <avr/power.h>
#include "bit_utils.h"
#include "global_utils.h"

#ifndef BAUD
	#error "USART ERROR: Baud rate not set! Set baud rate by ,defining this as compilation option"
		   "eg. -DBAUD=9600"
#endif

#ifndef F_CPU
	#error "USART ERROR: CPU fraquency not set! Set fraquency by ,defining this as compilation option"
		   "eg. -DF_CPU=1000000"
#endif

enum USART_SyncMode
{
	USART_AsynchronousOperation = 0b00,
	USART_SynchronousOperation = 0b01,
	USART_MasterSPIMode = 0b11
};

enum USART_ParityMode
{
	USART_ParityDisabled = 0b00,
	USART_EvenParity = 0b10,
	USART_OddParity = 0b11
};

enum USART_StopBits
{
	USART_1StopBit = 0b0,
	USART_2StopBit = 0b1
};

enum USART_DataSize
{
	USART_5Bit = 0b000,
	USART_6Bit = 0b001,
	USART_7Bit = 0b010,
	USART_8Bit = 0b011,
	USART_9Bit = 0b111
};

enum USART_ClockSignalPolarization
{
	USART_TXRisingEdge_RXFallingEdge = 0b0,
	USART_TXFallingEdge_RXRisingEdge = 0b1
};

#ifdef USART_INTERRUPT_MODE

enum USART_InterruptsMode
{
	USART_NoInterrupts = 0b000,
	USART_OnReceivedCharInterrupt_Enable = 0b100,
	USART_OnEmptyDataRegisterAndBufferInterrupt_Enable = 0b010,
	USART_OnTransmitBufferAvalaibleSpaceInterrupt_Enable = 0b001 
};

#endif

enum USART_MultiprocessorMode
{
	USART_MultiprocessorMode_Disable = 0b0,
	USART_MultiprocessorMode_Enable = 0b1
};

#define USART_receiveEnable()		 SET_BIT_AT(UCSR0B,RXEN0)
#define USART_transmitEnable()		 SET_BIT_AT(UCSR0B,TXEN0)
#define USART_receiveDisable()		 CLEAR_BIT_AT(UCSR0B,RXEN0)
#define USART_transmitDisable()		 CLEAR_BIT_AT(UCSR0B,TXEN0)

#define USART_enable()				 setBitsAt((volatile uint8_t*)&UCSR0B,RXEN0,TXEN0)
#define USART_disable()				 clearBitsAt((volatile uint8_t*)&UCSR0B,RXEN0,TXEN0)

#define USART_waitForReceiveReady()  while (IS_BIT_CLEARED_AT(UCSR0A,RXC0))
#define USART_waitForTransmitReady() while (IS_BIT_CLEARED_AT(UCSR0A,UDRE0))

#define transmitByte(byte)			 UDR0 = byte
#define receiveByte()				 UDR0

bool USART_isTransmitReady();

#define USART_isBufferEmpty() USART_isTransmitReady()

bool USART_isReceiveReady();

#define USART_byteAvalaible() USART_isReceiveReady()

bool USART_isTransmitBusy();

bool USART_transmitionCompleted();

bool USART_frameErrorOccured();

bool USART_parityErrorOccured();

inline uint8_t USART_getByteWithoutWait()
{
	return receiveByte();
}

inline void USART_sendByteWithoutWait(uint8_t byte)
{
	transmitByte(byte);
}

uint8_t USART_getByte();

void    USART_sendByte(uint8_t byte);
void	USART_sendChar(char character);

void USART_sendDataSeries(const uint8_t __memx* series,uint8_t length);

void USART_sendText(const char __memx* text);

void USART_receiveSomeBytes(uint8_t* target,uint8_t count);

struct USART_Setup_struct
{
	enum USART_SyncMode sync_mode_;
	enum USART_ParityMode parity_mode_;
	enum USART_StopBits stop_bits_;
	enum USART_DataSize data_size_;
	enum USART_ClockSignalPolarization clock_signal_polarization_;
	enum USART_MultiprocessorMode multiprocessor_mode_; 
	
	bool startup_receive_enable_;
	bool startup_transmit_enable_;
	
#ifdef USART_INTERRUPT_MODE
	enum USART_InterruptsMode interrupt_mode_;
	uint8_t receive_buffer_size_;
	uint8_t transmit_buffer_size_;
#endif 
};

typedef struct USART_Setup_struct USARTSetup;
extern volatile USARTSetup USART_DefaultSettings;

void USART_init(USARTSetup setup);

#ifdef USART_INTERRUPT_MODE

#include "circular_buffer.h"
#include <avr/interrupt.h>
#include <util/atomic.h>

volatile CircularBuffer usart_receive_buffer;
volatile CircularBuffer usart_transmit_buffer;

extern void (* USART_OnReceivedChar_Handler)(byte_t y);
extern void (* USART_OnEmptyDataRegisterAndBuffer_Handler)();
extern void (* USART_OnTransmitBufferAvalaibleSpaceInterrupt_Handler)();

#define USART_getByteFromReceiveBuffer(target)\
		CircularBuffer_pop(usart_receive_buffer,&target)

#define USART_getByteFromTransmitBuffer(target)\
		CircularBuffer_pop(usart_transmit_buffer,&target)

#define USART_sendByteToTransmitBuffer(byte)\
		CircularBuffer_safePush(usart_transmit_buffer,byte)
		
#define USART_sendForceByteToTransmitBuffer(byte)\
		CircularBuffer_forcePush(usart_transmit_buffer,byte)

void USART_sendBytesToTransmitBuffer(byte_t* to_send,length_t size);

enum OperationStatus USART_sendByteFromTransmitBufferToDevice();

//return Failure if count of sent bytes != count(argument),otherwise Success
enum OperationStatus USART_sendBytesFromTransmitBufferToDevice(length_t count);

enum OperationStatus USART_getBytesFromReceiveBuffer(byte_t* target,length_t count);

void USART_emptyTransmitBuffer();
void USART_emptyReceiveBuffer(byte_t* target);

#endif 

#ifdef USART_USE_STREAM

#include <stdio.h>

int get(FILE* in_stream);
int put(char c,FILE* out_stream);

volatile FILE USART_io_file;

#define USART_getchar()		  fgetc(&USART_io_file)
#define USART_putchar(c)	  fputc(c,&USART_io_file)

#define USART_printf(fmt,...) fprintf(&USART_io_file,fmt,__VA_ARGS__)
#define USART_scanf(fmt,...)  fscanf(&USART_io_file,fmt,__VA_ARGS__)


#endif // USART_USE_STREAM

#endif /* USART_H_ */