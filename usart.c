/*
 * usart.c
 *
 * Created: 03/04/2021 12:05:59
 *  Author: Damian Wójcik
 */ 

#include "usart.h"

extern const volatile volatile USART_Setup __flash USART_DefaultSettings = 
{
	USART_AsynchronousOperation,
	USART_ParityDisabled,
	USART_1StopBit,
	USART_8Bit,
	USART_TXRisingEdge_RXFallingEdge,
	USART_MultiprocessorMode_Disable,
	false,
	false
	
#if USART_0_INTERRUPT_MODE || USART_1_INTERRUPT_MODE

	,
	USART_NoInterrupts,
	10,
	10
	
#endif

};

//sending bytes/chars/9-bit data with waiting for usart ready

byte_t USART0_getByte()
{
	USART0_waitForReceiveReady();
	return USART0_receiveByte();
}

double_byte_t USART0_get9BitData()
{
	USART0_waitForReceiveReady();
	uint16_t data = USART0_receiveByte();//get normal byte
	SET_SHIFTED_BIT_MASK(data,EXTRACT_BIT_FROM(UCSR0B,2),8);// get and set 9th bit 
	return data;
}

char USART0_getChar()
{
	USART0_waitForReceiveReady();
	return (char)USART0_receiveByte();
}

void USART0_sendByte(byte_t byte)
{
	USART0_waitForTransmitReady();
	USART0_transmitByte(byte);
}

void USART0_sendChar(char character)
{
	USART0_waitForTransmitReady();
	USART0_transmitByte(character);
}

void USART0_send9BitData(double_byte_t data)
{
	USART0_waitForTransmitReady();
	USART0_transmitByte((uint8_t)data);
	data = EXTRACT_BIT_FROM(data,8);
	SET_SHIFTED_BIT_MASK(UCSR0B,GET_RIGHT_SHIFTED_BIT_MASK_OF(data,9),0);
}

#ifndef MCU_328PB

byte_t USART1_getByte()
{
	USART1_waitForReceiveReady();
	return USART1_receiveByte();
}

double_byte_t USART1_get9BitData()
{
	USART1_waitForReceiveReady();
	uint16_t data = USART1_receiveByte();//get normal byte
	SET_SHIFTED_BIT_MASK(data,EXTRACT_BIT_FROM(UCSR1B,2),8);// get and set 9th bit
	return data;
}

char USART1_getChar()
{
	USART1_waitForReceiveReady();
	return (char)USART1_receiveByte();
}

void USART1_sendByte(byte_t byte)
{
	USART1_waitForTransmitReady();
	USART1_transmitByte(byte);
}

void USART1_sendChar(char character)
{
	USART1_waitForTransmitReady();
	USART1_transmitByte(character);
}

void USART1_send9BitData(double_byte_t data)
{
	USART1_waitForTransmitReady();
	USART1_transmitByte((uint8_t)data);
	data = EXTRACT_BIT_FROM(data,8);
	SET_SHIFTED_BIT_MASK(UCSR1B,GET_RIGHT_SHIFTED_BIT_MASK_OF(data,9),0);
}

#endif

//sending string of bytes/chars/9-bit data

void USART0_sendDataSeries(const byte_t __memx* series,length_t length)
{
	while(--length)
	{
		USART0_sendChar(*series);
		++series;
	}
}

void USART0_sendText(const char __memx* text)
{
	while(*text)
	{
		USART0_sendByte(*text);
		++text;
	}
}

void USART0_receiveSomeBytes(byte_t* target,length_t count)
{
	uint8_t idx = 0;
	while(count--)
	{
		target[idx] = USART0_getByte();
		++idx;
	}
}

void USART1_sendDataSeries(const byte_t __memx* series,length_t length)
{
	while(--length)
	{
		USART1_sendChar(*series);
		++series;
	}
}

void USART1_sendText(const char __memx* text)
{
	while(*text)
	{
		USART1_sendByte(*text);
		++text;
	}
}

void USART1_receiveSomeBytes(byte_t* target,length_t count)
{
	uint8_t idx = 0;
	while(count--)
	{
		target[idx] = USART1_getByte();
		++idx;
	}
}

//manually enabling/disabling usart functions

void USART0_receiveEnable()
{
	SET_BIT_AT(UCSR0B,RXEN0);
}

void USART0_transmitEnable()
{
	SET_BIT_AT(UCSR0B,TXEN0);
}

void USART0_receiveDisable()
{
	CLEAR_BIT_AT(UCSR0B,RXEN0);
}

void USART0_transmitDisable()
{
	CLEAR_BIT_AT(UCSR0B,TXEN0);
}

#ifdef MCU_328PB

void USART1_receiveEnable()
{
	SET_BIT_AT(UCSR1B,RXEN1);
}

void USART1_transmitEnable()
{
	SET_BIT_AT(UCSR1B,TXEN1);
}

void USART1_receiveDisable()
{
	CLEAR_BIT_AT(UCSR1B,RXEN1);
}

void USART1_transmitDisable()
{
	CLEAR_BIT_AT(UCSR1B,TXEN1);
}

#endif

void USART0_enable()
{
	setBitsAt((volatile uint8_t*)&UCSR0B,RXEN0,TXEN0);
}

void USART0_disable()
{
	clearBitsAt((volatile uint8_t*)&UCSR0B,RXEN0,TXEN0);
}

#ifdef MCU_328PB

void USART1_enable()
{
	setBitsAt((volatile uint8_t*)&UCSR1B,RXEN1,TXEN1);
}

void USART1_disable()
{
	clearBitsAt((volatile uint8_t*)&UCSR1B,RXEN1,TXEN1);
}

#endif

//waiting for device I/O ready

void USART0_waitForReceiveReady()
{
	while (IS_BIT_CLEARED_AT(UCSR0A,RXC0));
}

void USART0_waitForTransmitReady()
{
	while (IS_BIT_CLEARED_AT(UCSR0A,UDRE0));
}

#ifdef MCU_328PB

void USART1_waitForReceiveReady()
{
	while (IS_BIT_CLEARED_AT(UCSR1A,RXC1));
}
void USART1_waitForTransmitReady()
{
	while (IS_BIT_CLEARED_AT(UCSR1A,UDRE1));
}

//start frame detector flag operations

void USART0_startConditionDetected()
{
	 IS_BIT_SET_AT(UCSR0D,RXS);
}

void USART0_clearStartConditionDetectedFlag()
{
	SET_BIT_AT(UCSR0D,RXS);//setting up this bit clears the above flag
}

void USART1_startConditionDetected()
{
	IS_BIT_SET_AT(UCSR1D,RXS);
}

void USART1_clearStartConditionDetectedFlag()
{
	SET_BIT_AT(UCSR1D,RXS);//setting up this bit clears the above flag
}

#endif


bool USART0_isTransmitReady()
{
	return IS_BIT_SET_AT(UCSR0A,UDRE0);
}

bool USART0_isReceiveReady()
{
	return IS_BIT_SET_AT(UCSR0A,RXC0);
}

bool USART0_isTransmiterBusy()
{
	return IS_BIT_CLEARED_AT(UCSR0A,TXC0);
}

bool USART0_transmitionCompleted()
{
	return IS_BIT_SET_AT(UCSR0A,TXC0);
}

bool USART0_frameErrorOccured()
{
	return IS_BIT_SET_AT(UCSR0A,FE0);
}

bool USART0_parityErrorOccured()
{
	return IS_BIT_SET_AT(UCSR0A,UPE0);
}

bool USART1_isTransmitReady()
{
	return IS_BIT_SET_AT(UCSR1A,UDRE1);
}

bool USART1_isReceiveReady()
{
	return IS_BIT_SET_AT(UCSR1A,RXC1);
}

bool USART1_isTransmitBusy()
{
	return IS_BIT_CLEARED_AT(UCSR1A,TXC1);
}

bool USART1_transmitionCompleted()
{
	return IS_BIT_SET_AT(UCSR1A,TXC1);
}

bool USART1_frameErrorOccured()
{
	return IS_BIT_SET_AT(UCSR1A,FE1);
}

bool USART1_parityErrorOccured()
{
	return IS_BIT_SET_AT(UCSR1A,UPE1);
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

#ifdef MCU_328PB

static inline void _setupFrameDetector(USART_Setup* setup)
{
	if(setup->enable_frame_detector_)
		SET_BIT_AT(UCSR0D,SFDE);
	if(setup->enable_frame_detector_interrupt_)
		SET_BIT_AT(UCSR0D,RXCIE0);
}

#endif

#ifdef USART_INTERRUPT_MODE

static inline void _setInterruptsMode(enum USART_InterruptsMode interrupt_mode)
{
	clearBitsAt((volatile uint8_t*)&UCSR0B,RXCIE0,TXCIE0,UDRIE0);
	SET_SHIFTED_BIT_MASK(UCSR0B,interrupt_mode,5);
}

static inline void _setupBuffers(USART_Setup* setup)
{
	usart_0_receive_buffer = CircularBuffer_initNormal(setup->receive_buffer_size_);
	usart_0_transmit_buffer = CircularBuffer_initNormal(setup->transmit_buffer_size_);
	usart_0_transmit_flag = false;
}

#endif

static inline void _setMultiprocessorMode(enum USART_MultiprocessorMode multiprocesor_mode)
{
	CLEAR_BIT_AT(UCSR0A,MPCM0);
	SET_SHIFTED_BIT_MASK(UCSR0A,multiprocesor_mode,0);
}


#if USART_0_INTERRUPT_MODE || USART_1_INTERRUPT_MODE

//Interrupts definitions,user defines only a handler

#define tryToExecuteHandler(handler,...)\
	if(handler != NULL)\
		handler(__VA_ARGS__)


#endif 

#ifdef USART_0_INTERRUPT_MODE

void (* USART0_OnReceivedChar_Handler)(byte_t)					  = NULL;
void (* USART0_OnEmptyDataRegisterAndBuffer_Handler)()			  = NULL;
void (* USART0_OnTransmitBufferAvalaibleSpaceInterrupt_Handler)() = NULL;
void (* USART0_OnStartFrameDetection_Handler)()					  = NULL;

volatile byte_t usart_0_received_byte;
volatile byte_t usart_0_transmited_byte;
volatile bool   usart_0_transmit_flag;

void USART0_sendBytesToTransmitBuffer(byte_t* to_send,length_t size)
{
	length_t idx = 0;
	while(size--)
	{
		USART0_sendForceByteToTransmitBuffer(to_send[idx]);
		++idx;
	}
}

enum OperationStatus USART0_sendByteFromTransmitBufferToDevice()
{
	enum OperationStatus status = Failure;
	status = USART0_getByteFromTransmitBuffer(usart_0_transmited_byte);
	if(status != Failure)
		USART0_sendByte(usart_0_transmited_byte);
	return status;
}

enum OperationStatus USART0_sendBytesFromTransmitBufferToDevice(length_t count)
{
	enum OperationStatus status = Failure;
	while(count-- && !CircularBuffer_isEmpty(usart_0_transmit_buffer))
		status = USART0_sendByteFromTransmitBufferToDevice();
	return status;
}

enum OperationStatus USART0_getBytesFromReceiveBuffer(byte_t* target,length_t count)
{
	enum OperationStatus status = Success;
	
	length_t idx = 0;
	while(count-- && status != Failure)
	{
		status = USART0_getByteFromReceiveBuffer(target[idx]);
		++idx;
	}
	
	return status;
}

void USART0_emptyTransmitBuffer()
{
	enum OperationStatus status = Success;
	while(status != Failure)
		status = USART0_sendByteFromTransmitBufferToDevice();
}

void USART0_emptyReceiveBuffer(byte_t* target)
{
	enum OperationStatus status = Success;
	length_t idx = 0;
	while(status != Failure)
	{
		status = USART0_getByteFromReceiveBuffer(target[idx]);
		++idx;
	}
}

//this isr handles user handler and receives data
ISR(USART0_RX_vect)
{
	usart_0_received_byte = USART0_receiveByte();
	CircularBuffer_forcePush(usart_0_receive_buffer,usart_0_received_byte);
	tryToExecuteHandler(USART0_OnReceivedChar_Handler,usart_0_received_byte);
}

//this isr handles only user handler
ISR(USART0_TX_vect)
{
	tryToExecuteHandler(USART0_OnEmptyDataRegisterAndBuffer_Handler);
}

//this isr handles only user handler
ISR(USART0_UDRE_vect)
{
	tryToExecuteHandler(USART0_OnTransmitBufferAvalaibleSpaceInterrupt_Handler);
}

#ifdef MCU_328PB

//this isr handles only user handler
ISR(USART0_START_vect)
{
	tryToExecuteHandler(USART0_OnStartFrameDetection_Handler);
}

#endif

#endif

#ifdef USART_1_INTERRUPT_MODE

void (* USART1_OnReceivedChar_Handler)(byte_t y)					= NULL;
void (* USART1_OnEmptyDataRegisterAndBuffer_Handler)()				= NULL;
void (* USART1_OnTransmitBufferAvalaibleSpaceInterrupt_Handler)()   = NULL;
void (* USART1_OnStartFrameDetection_Handler)()						= NULL;

volatile byte_t usart_1_received_byte;
volatile byte_t usart_1_transmited_byte;
volatile bool   usart_1_transmit_flag;

void USART1_sendBytesToTransmitBuffer(byte_t* to_send,length_t size)
{
	length_t idx = 0;
	while(size--)
	{
		USART1_sendForceByteToTransmitBuffer(to_send[idx]);
		++idx;
	}
}

enum OperationStatus USART1_sendByteFromTransmitBufferToDevice()
{
	enum OperationStatus status = Failure;
	status = USART1_getByteFromTransmitBuffer(usart_1_transmited_byte);
	if(status != Failure)
		USART0_sendByte(usart_1_transmited_byte);
	return status;
}

enum OperationStatus USART1_sendBytesFromTransmitBufferToDevice(length_t count)
{
	enum OperationStatus status = Failure;
	while(count-- && !CircularBuffer_isEmpty(usart_1_transmit_buffer))
		status = USART1_sendByteFromTransmitBufferToDevice();
	return status;
}

enum OperationStatus USART1_getBytesFromReceiveBuffer(byte_t* target,length_t count)
{
	enum OperationStatus status = Success;
	
	length_t idx = 0;
	while(count-- && status != Failure)
	{
		status = USART1_getByteFromReceiveBuffer(target[idx]);
		++idx;
	}
	
	return status;
}

void USART1_emptyTransmitBuffer()
{
	enum OperationStatus status = Success;
	while(status != Failure)
		status = USART1_sendByteFromTransmitBufferToDevice();
}

void USART1_emptyReceiveBuffer(byte_t* target)
{
	enum OperationStatus status = Success;
	length_t idx = 0;
	while(status != Failure)
	{
		status = USART1_getByteFromReceiveBuffer(target[idx]);
		++idx;
	}
}

//this isr handles user handler and receives data
ISR(USART1_RX_vect)
{
	usart_1_received_byte = USART1_receiveByte();
	CircularBuffer_forcePush(usart_1_receive_buffer,usart_1_received_byte);
	tryToExecuteHandler(USART1_OnReceivedChar_Handler,usart_1_received_byte);
}

//this isr handles only user handler
ISR(USART1_TX_vect)
{
	tryToExecuteHandler(USART1_OnEmptyDataRegisterAndBuffer_Handler);
}

//this isr handles only user handler
ISR(USART1_UDRE_vect)
{
	tryToExecuteHandler(USART1_OnTransmitBufferAvalaibleSpaceInterrupt_Handler);
}

//this isr handles only user handler
ISR(USART1_START_vect)
{
	tryToExecuteHandler(USART1_OnStartFrameDetection_Handler);
}

#endif

#ifdef USART_0_USE_STREAM 

int get_0(FILE* in_stream)
{
	USART0_waitForReceiveReady();
	return USART0_receiveByte();
}

int put_0(char c,FILE* out_stream)
{
	USART0_waitForTransmitReady();
	USART0_transmitByte(c);
	return 0;
	
inline void _setupUsartStream0()
{
	FILE temp_file = FDEV_SETUP_STREAM(put_0,get_0,_FDEV_SETUP_RW);
	usart_0_io_file = temp_file;
}

}

#endif

#ifdef USART_1_USE_STREAM

int get_1(FILE* in_stream)
{
	USART1_waitForReceiveReady();
	return USART1_receiveByte();
}

int put_1(char c,FILE* out_stream)
{
	USART1_waitForTransmitReady();
	USART1_transmitByte(c);
	return 0;
}

inline void _setupUsartStream1()
{
	FILE temp_file = FDEV_SETUP_STREAM(put_1,get_1,_FDEV_SETUP_RW);
	usart_1_io_file = temp_file;
}

#endif

//Initialization

void USART0_init(USART_Setup setup)
{
	power_usart0_enable();
	
	_setSyncMode(setup.sync_mode_);
	_setParityMode(setup.parity_mode_);
	_setStopBits(setup.stop_bits_);
	_setDataSize(setup.data_size_);
	_setClockSignalPolarization(setup.clock_signal_polarization_);
	_setMultiprocessorMode(setup.multiprocessor_mode_);

#ifdef USART_0_INTERRUPT_MODE
	
	_setupBuffers(&setup);
	
#endif
	
	if(setup.startup_receive_enable_)
		USART0_receiveEnable();
	if(setup.startup_transmit_enable_)
		USART0_transmitEnable();
	
#ifdef USART_0_USE_STREAM
	
	_setupUsartStream0();
	
#endif

#ifdef USART_0_INTERRUPT_MODE
	_setInterruptsMode(setup.interrupt_mode_);
#endif
	_injectBaudRate();

#ifdef MCU_328PB

	//define here frame checker

#endif


}
