/*
 * usart.c
 *
 * Created: 03/04/2021 12:05:59
 *  Author: Damian Wójcik
 */ 

#include "usart.h"

volatile USARTSetup USART_DefaultSettings = 
{
	USART_AsynchronousOperation,
	USART_ParityDisabled,
	USART_1StopBit,
	USART_8Bit,
	USART_TXRisingEdge_RXFallingEdge,
	USART_MultiprocessorMode_Disable,
	false,
	false
#ifdef USART_INTERRUPT_MODE
	,
	USART_NoInterrupts,
	10,
	10
#endif
};

#ifdef USART_INTERRUPT_MODE

void (* USART_OnReceivedChar_Handler)(byte_t) = NULL;
void (* USART_OnEmptyDataRegisterAndBuffer_Handler)() = NULL;
void (* USART_OnTransmitBufferAvalaibleSpaceInterrupt_Handler)() = NULL;

volatile byte_t received_byte;
volatile byte_t transmited_byte;
volatile bool transmit_flag;

//Interrupts definitions,user defines only a handler

#define tryToExecuteHandler(handler,...)\
	if(handler != NULL)\
		handler(__VA_ARGS__)

void USART_sendBytesToTransmitBuffer(byte_t* to_send,length_t size)
{
	length_t idx = 0;
	while(size--)
	{
		USART_sendForceByteToTransmitBuffer(to_send[idx]);
		++idx;
	}
}

enum OperationStatus USART_sendByteFromTransmitBufferToDevice()
{
	enum OperationStatus status = Failure;
	status = USART_getByteFromTransmitBuffer(transmited_byte);
	if(status != Failure)
		USART_sendByte(transmited_byte);
	return status;
}

enum OperationStatus USART_sendBytesFromTransmitBufferToDevice(length_t count)
{
	enum OperationStatus status = Failure;
	while(count-- && !CircularBuffer_isEmpty(usart_transmit_buffer))
			status = USART_sendByteFromTransmitBufferToDevice();
	return status;
}

enum OperationStatus USART_getBytesFromReceiveBuffer(byte_t* target,length_t count)
{
	enum OperationStatus status = Success;
#ifdef DEBUG
	assert(target != NULL);
#endif
	
	length_t idx = 0;
	while(count-- && status != Failure)
	{
		status = USART_getByteFromReceiveBuffer(target[idx]);
		++idx;
	}
	
	return status;
}

void USART_emptyTransmitBuffer()
{
	enum OperationStatus status = Success;
	while(status != Failure)
		status = USART_sendByteFromTransmitBufferToDevice();
}

void USART_emptyReceiveBuffer(byte_t* target)
{
	enum OperationStatus status = Success;
	length_t idx = 0;
	while(status != Failure)
	{
		status = USART_getByteFromReceiveBuffer(target[idx]);
		++idx;
	}
}

//this isr handles user handler and receives data
ISR(USART_RX_vect)
{
	received_byte = receiveByte();
	CircularBuffer_forcePush(usart_receive_buffer,received_byte);
	tryToExecuteHandler(USART_OnReceivedChar_Handler,received_byte);
}

//this isr handles only user handler
ISR(USART_TX_vect)
{
	tryToExecuteHandler(USART_OnEmptyDataRegisterAndBuffer_Handler);
}

//this isr handles only user handler
ISR(USART_UDRE_vect)
{
	tryToExecuteHandler(USART_OnTransmitBufferAvalaibleSpaceInterrupt_Handler);
}

#endif

uint8_t USART_getByte()
{
	USART_waitForReceiveReady();
	return receiveByte();
}

void USART_sendByte(uint8_t byte)
{
	USART_waitForTransmitReady();
	transmitByte(byte);
}

void USART_sendChar(char character)
{
	USART_waitForTransmitReady();
	transmitByte(character);
}

void USART_sendDataSeries(const uint8_t __memx* series,uint8_t length)
{
	while(--length)
	{
		USART_sendChar(*series);
		++series;
	}
}

void USART_sendText(const char __memx* text)
{
	while(*text)
	{
		USART_sendByte(*text);
		++text;
	}
}

void USART_receiveSomeBytes(uint8_t* target,uint8_t count)
{
	uint8_t idx = 0;
	while(count--)
	{
		target[idx] = USART_getByte();
		++idx;
	}
}

bool USART_isTransmitReady()
{
	return IS_BIT_SET_AT(UCSR0A,UDRE0);
}

bool USART_isReceiveReady()
{
	return IS_BIT_SET_AT(UCSR0A,RXC0);
}

bool USART_isTransmitBusy()
{
	return IS_BIT_CLEARED_AT(UCSR0A,TXC0);
}

bool USART_transmitionCompleted()
{
	return IS_BIT_SET_AT(UCSR0A,TXC0);
}

bool USART_frameErrorOccured()
{
	return IS_BIT_SET_AT(UCSR0A,FE0);
}

bool USART_parityErrorOccured()
{
	return IS_BIT_SET_AT(UCSR0A,UPE0);
}

static inline void _injectBaudRate()
{
	#include <util/setbaud.h>
	REPLACE_REGISTER(UBRR0H,UBRRH_VALUE);
	REPLACE_REGISTER(UBRR0L,UBRRL_VALUE);
	#if USE_2X
		SET_BIT_AT(UCSR0A,U2X0);
	#else
		CLEAR_BIT_AT(UCSR0A,U2X0);
	#endif
}

static inline void _setSyncMode(enum USART_SyncMode sync_mode)
{
	clearBitsAt((volatile uint8_t*)&UCSR0C,UMSEL01,UMSEL00);
	SET_SHIFTED_BIT_MASK(UCSR0C,sync_mode,6);
}

static inline void _setParityMode(enum USART_ParityMode parity_mode)
{
	clearBitsAt((volatile uint8_t*)&UCSR0C,UPM01,UPM00);
	SET_SHIFTED_BIT_MASK(UCSR0C,parity_mode,4);
}

static inline void _setStopBits(enum USART_StopBits stop_bits)
{
	CLEAR_BIT_AT(UCSR0C,USBS0);
	SET_SHIFTED_BIT_MASK(UCSR0C,stop_bits,3);
}

static inline void _setDataSize(enum USART_DataSize data_size)
{
	clearBitsAt((volatile uint8_t*)&UCSR0C,UCSZ01,UCSZ00);
	CLEAR_BIT_AT(UCSR0B,UCSZ02);
	
	SET_SHIFTED_BIT_MASK(UCSR0C,EXTRACT_BIT_MASK_FROM(data_size,0,1),1);
	SET_SHIFTED_BIT_MASK(UCSR0B,EXTRACT_BIT_MASK_FROM(data_size,2,2),2);
}

static inline void _setClockSignalPolarization(enum USART_ClockSignalPolarization polar)
{
	CLEAR_BIT_AT(UCSR0C,UCPOL0);
	SET_SHIFTED_BIT_MASK(UCSR0C,polar,0);
}

#ifdef USART_INTERRUPT_MODE

static inline void _setInterruptsMode(enum USART_InterruptsMode interrupt_mode)
{
	clearBitsAt((volatile uint8_t*)&UCSR0B,RXCIE0,TXCIE0,UDRIE0);
	SET_SHIFTED_BIT_MASK(UCSR0B,interrupt_mode,5);
}

static inline void _setupBuffers(USARTSetup* setup)
{
	usart_receive_buffer = CircularBuffer_initNormal(setup->receive_buffer_size_);
	usart_transmit_buffer = CircularBuffer_initNormal(setup->transmit_buffer_size_);
	transmit_flag = false;
}

#endif

#ifdef USART_USE_STREAM

int get(FILE* in_stream)
{
	USART_waitForReceiveReady();
	return receiveByte();
}

int put(char c,FILE* out_stream)
{
	USART_waitForTransmitReady();
	transmitByte(c);
	return 0;
}

static inline void _setupStream()
{
	FILE temp_file = FDEV_SETUP_STREAM(put,get,_FDEV_SETUP_RW);
	USART_io_file = temp_file;
}

#endif

static inline void _setMultiprocessorMode(enum USART_MultiprocessorMode multiprocesor_mode)
{
	CLEAR_BIT_AT(UCSR0A,MPCM0);
	SET_SHIFTED_BIT_MASK(UCSR0A,multiprocesor_mode,0);
}

void USART_init(USARTSetup setup)
{
	power_usart0_enable();
	
	_setSyncMode(setup.sync_mode_);
	_setParityMode(setup.parity_mode_);
	_setStopBits(setup.stop_bits_);
	_setDataSize(setup.data_size_);
	_setClockSignalPolarization(setup.clock_signal_polarization_);
	_setMultiprocessorMode(setup.multiprocessor_mode_);

#ifdef USART_INTERRUPT_MODE
	
	_setupBuffers(&setup);
	
#endif
	
	if(setup.startup_receive_enable_)
		USART_receiveEnable();
	if(setup.startup_transmit_enable_)
		USART_transmitEnable();
		
#ifdef USART_USE_STREAM
	
	_setupStream();
	
#endif

#ifdef USART_INTERRUPT_MODE
	_setInterruptsMode(setup.interrupt_mode_);
#endif
	_injectBaudRate();
}
