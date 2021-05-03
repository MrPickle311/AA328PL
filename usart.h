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
	USART_AsynchronousOperations =  0b00,
	USART_SynchronousOperations  =  0b01,
	USART_MasterSPIMode         =  0b11
};

enum USART_ParityMode
{
	USART_ParityDisabled = 0b00,
	USART_EvenParity     = 0b10,
	USART_OddParity      = 0b11
};

enum USART_StopBits
{
	USART_1StopBit		 = 0b0,
	USART_2StopBit		 = 0b1
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

#if USART_0_INTERRUPT_MODE || USART_1_INTERRUPT_MODE

enum USART_InterruptsMode
{
	USART_NoInterrupts									 = 0b000,
	USART_OnReceivedCharInterrupt_Enable				 = 0b100,
	USART_OnEmptyDataRegisterAndBufferInterrupt_Enable   = 0b010,
	USART_OnTransmitBufferAvalaibleSpaceInterrupt_Enable = 0b001 
};

#endif

enum USART_MultiprocessorMode
{
	USART_MultiprocessorMode_Disable = 0b0,
	USART_MultiprocessorMode_Enable  = 0b1
};

//manually enabling/disabling usart functions

void USART0_receiveEnable();
void USART0_transmitEnable();
void USART0_receiveDisable();
void USART0_transmitDisable();

#ifdef MCU_328PB

void USART1_receiveEnable();
void USART1_transmitEnable();
void USART1_receiveDisable();
void USART1_transmitDisable();

#endif

void USART0_enable();
void USART0_disable();

#ifdef MCU_328PB

void USART1_enable();
void USART1_disable();

#endif

//waiting for device I/O ready

void USART0_waitForReceiveReady();
void USART0_waitForTransmitReady();

#ifdef MCU_328PB

void USART1_waitForReceiveReady();
void USART1_waitForTransmitReady();

#endif

//force transmit and receiving macros

#define USART0_transmitByte(byte)				 UDR0 = byte
#define USART0_receiveByte()					 UDR0

#ifdef MCU_328PB

#define USART1_transmitByte(byte)				 UDR1 = byte
#define USART1_receiveByte()					 UDR1

//start frame detector flag operations

bool USART0_startConditionDetected();
void USART0_clearStartConditionDetectedFlag();

bool USART1_startConditionDetected();
void USART1_clearStartConditionDetectedFlag();

#endif

//transmition-receive state checking 

bool USART0_isTransmitReady();

#define USART0_isBufferEmpty()  USART0_isTransmitReady()

bool USART0_isReceiveReady();

#define USART0_isByteAvalaible() USART0_isReceiveReady()

bool USART0_isTransmiterBusy();

bool USART0_transmitionCompleted();

bool USART0_frameErrorOccured();

bool USART0_parityErrorOccured();

#ifdef MCU_328PB

bool USART1_isTransmitReady();

#define USART1_isBufferEmpty()  USART1_isTransmitReady()

bool USART1_isReceiveReady();

#define USART1_isByteAvalaible() USART1_isReceiveReady()

bool USART1_isTransmitBusy();

bool USART1_transmitionCompleted();

bool USART1_frameErrorOccured();

bool USART1_parityErrorOccured();

#endif

//force sending/receiving bytes

inline byte_t USART0_getByteWithoutWait()
{
	return USART0_receiveByte();
}

inline void USART0_sendByteWithoutWait(byte_t byte)
{
	USART0_transmitByte(byte);
}

#ifdef MCU_328PB

inline byte_t USART1_getByteWithoutWait()
{
	return USART1_receiveByte();
}

inline void USART1_sendByteWithoutWait(byte_t byte)
{
	USART1_transmitByte(byte);
}

#endif

//sending bytes/chars/9-bit data with waiting for usart ready

byte_t USART0_getByte();

typedef uint16_t usart_9bit_data_t;

usart_9bit_data_t USART0_get9BitData();

char USART0_getChar();

void USART0_sendByte(byte_t byte);

void USART0_sendChar(char character);

void USART0_send9BitData(usart_9bit_data_t data);

#ifndef MCU_328PB

byte_t USART1_getByte();

usart_9bit_data_t USART1_get9BitData();

char USART1_getChar();

void USART1_sendByte(byte_t byte);

void USART1_sendChar(char character);

void USART1_send9BitData(usart_9bit_data_t data);

#endif

//sending string of bytes/chars/9-bit data

void USART0_sendDataSeries(const byte_t __memx* series, length_t length);

void USART0_sendText(const char __memx* text);

void USART0_receiveSomeBytes(byte_t* target, length_t count);

#ifndef MCU_328PB

void USART1_sendDataSeries(const byte_t __memx* series, length_t length);

void USART1_sendText(const char __memx* text);

void USART1_receiveSomeBytes(byte_t* target, length_t count);

#endif

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
	
#if  USART_0_INTERRUPT_MODE || USART_1_INTERRUPT_MODE
	
	enum USART_InterruptsMode interrupt_mode_;
	
	length_t receive_buffer_size_;
	length_t transmit_buffer_size_;

#endif 

#ifdef MCU_328PB
	
	bool enable_frame_detector_;//reg: UCSRnD , bit : SFDE
	
	bool enable_frame_detector_interrupt_;//reg: UCSRnD UCSRnB , bits  RXSIE ,RXCIE
	
#endif

};

//forwarding names and default setting for user
typedef struct USART_Setup_struct USART_Setup;
extern const volatile USART_Setup __flash USART_DefaultSettings;


//Interrupts mode


#if USART_0_INTERRUPT_MODE || USART_1_INTERRUPT_MODE

#include "circular_buffer.h"
#include <avr/interrupt.h>
#include <util/atomic.h>

#endif

#ifdef USART_0_INTERRUPT_MODE

volatile CircularBuffer usart_0_receive_buffer;
volatile CircularBuffer usart_0_transmit_buffer;

extern void (* USART0_OnReceivedChar_Handler)(byte_t y);
extern void (* USART0_OnEmptyDataRegisterAndBuffer_Handler)();
extern void (* USART0_OnTransmitBufferAvalaibleSpaceInterrupt_Handler)();

#ifdef MCU_328PB

extern void (* USART0_OnStartFrameDetection_Handler)();

#endif

#define USART0_getByteFromReceiveBuffer(target)\
		CircularBuffer_pop(usart_0_receive_buffer, (byte_t*)&target)

#define USART0_getByteFromTransmitBuffer(target)\
		CircularBuffer_pop(usart_0_transmit_buffer, (byte_t*)&target)

#define USART0_sendByteToTransmitBuffer(byte)\
		CircularBuffer_safePush(usart_0_transmit_buffer, byte)
		
#define USART0_sendForceByteToTransmitBuffer(byte)\
		CircularBuffer_forcePush(usart_0_transmit_buffer, byte)

void USART0_sendBytesToTransmitBuffer(byte_t* to_send, length_t size);

enum OperationStatus USART0_sendByteFromTransmitBufferToDevice();

//return Failure if count of sent bytes != count(argument),otherwise Success
enum OperationStatus USART0_sendBytesFromTransmitBufferToDevice(length_t count);

enum OperationStatus USART0_getBytesFromReceiveBuffer(byte_t* target, length_t count);

#define USART0_waitForByteInReceiveBuffer() 	while(CircularBuffer_isEmpty(usart_0_receive_buffer))
#define USART0_waitUntilReceiveBufferIsFull()	while(!CircularBuffer_isFull(usart_0_receive_buffer))

void USART0_waitForSeveralBytesInReceiveBuffer(byte_t* target, length_t count);

void USART0_emptyTransmitBuffer();
void USART0_emptyReceiveBuffer(byte_t* target);

#endif 

#if USART_1_INTERRUPT_MODE && MCU_328PB

volatile CircularBuffer usart_1_receive_buffer;
volatile CircularBuffer usart_1_transmit_buffer;

extern void (* USART1_OnReceivedChar_Handler)(byte_t y);
extern void (* USART1_OnEmptyDataRegisterAndBuffer_Handler)();
extern void (* USART1_OnTransmitBufferAvalaibleSpaceInterrupt_Handler)();
extern void (* USART1_OnStartFrameDetection_Handler)();

#define USART1_getByteFromReceiveBuffer(target)\
	CircularBuffer_pop(usart_1_receive_buffer, (byte_t*)&target)

#define USART1_getByteFromTransmitBuffer(target)\
	CircularBuffer_pop(usart_1_transmit_buffer, (byte_t*)&target)

#define USART1_sendByteToTransmitBuffer(byte)\
	CircularBuffer_safePush(usart_1_transmit_buffer,byte)

#define USART1_sendForceByteToTransmitBuffer(byte)\
	CircularBuffer_forcePush(usart_1_transmit_buffer,byte)

void USART1_sendBytesToTransmitBuffer(byte_t* to_send, length_t size);

enum OperationStatus USART1_sendByteFromTransmitBufferToDevice();

//return Failure if count of sent bytes != count(argument),otherwise Success
enum OperationStatus USART1_sendBytesFromTransmitBufferToDevice(length_t count);

enum OperationStatus USART1_getBytesFromReceiveBuffer(byte_t* target, length_t count);

void USART1_emptyTransmitBuffer();
void USART1_emptyReceiveBuffer(byte_t* target);

#endif


//Stream mode


#if USART_0_USE_STREAM || USART_1_USE_STREAM

#include <stdio.h>

#endif

#ifdef USART_0_USE_STREAM

volatile FILE usart_0_io_file;

//*(FILE*)&file <- volatile typecasting 

#define USART0_getchar()	   fgetc((FILE*)&usart_0_io_file)
#define USART0_putchar(c)	   fputc(c, (FILE*)&usart_0_io_file)

#define USART0_log(message)	   fprintf((FILE*)&usart_0_io_file, message)
#define USART0_printf(fmt,...) fprintf((FILE*)&usart_0_io_file, fmt, __VA_ARGS__)
#define USART0_scanf(fmt,...)  fscanf((FILE*)&usart_0_io_file, fmt, __VA_ARGS__)

#define USART0_log(message)    fprintf((FILE*)&usart_0_io_file, message)

#endif // USART_1_USE_STREAM

#if USART_1_USE_STREAM && MCU_328PB

volatile FILE usart_1_io_file;

#define USART1_getchar()	    fgetc((FILE*)&usart_1_io_file)
#define USART1_putchar(c)	    fputc(c,(FILE*)&usart_1_io_file)

#define USART1_log(message)	   fprintf((FILE*)&usart_0_io_file, message)
#define USART1_printf(fmt, ...) fprintf((FILE*)&usart_1_io_file, fmt, __VA_ARGS__)
#define USART1_scanf(fmt, ...)  fscanf((FILE*)&usart_1_io_file, fmt, __VA_ARGS__)

#define USART1_log(message)     fprintf((FILE*)&usart_1_io_file, message)

#endif // USART_1_USE_STREAM



//Initializations


void USART0_init(USART_Setup setup);

#if MCU_328PB

void USART1_init(USART_Setup setup);

#endif


#endif /* USART_H_ */