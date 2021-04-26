/*
 * usart.c
 *
 * Created: 03/04/2021 12:05:59
 *  Author: Damian Wójcik
 */ 

#include "usart.h"

//GLOBAL DECLARATIONS
#define NINTH_BIT_POS 8
#define NOTHING		  0
//in bytes
#define DEFAULT_DEFAULT_BUFFER_SIZE 10

const volatile USART_Setup __flash USART_DefaultSettings = 
{
	USART_AsynchronousOperations,
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
	DEFAULT_DEFAULT_BUFFER_SIZE,
	DEFAULT_DEFAULT_BUFFER_SIZE
	
#endif

};

/////////////////////////////////			USER_INTERFACE_IMPLEMENTATION

//sending bytes/chars/9-bit data with waiting for usart ready

byte_t USART0_getByte()
{
	USART0_waitForReceiveReady();
	return USART0_receiveByte();
}

#define RXB80_TO_9BIT_DISTANCE 7
#define RXB80_IN_REG_POS 1
usart_9bit_data_t USART0_get9BitData()
{
	USART0_waitForReceiveReady();
	usart_9bit_data_t data = USART0_receiveByte();//get normal byte
	SET_SHIFTED_BIT_MASK(data, EXTRACT_BIT_FROM(UCSR0B, RXB80_IN_REG_POS), RXB80_TO_9BIT_DISTANCE);// get and set 9th bit 
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

#define TXB80_IN_REG_POS 0 
#define TXB80_TO_9BIT_DISTANCE 8
void USART0_send9BitData(usart_9bit_data_t data)
{
	USART0_waitForTransmitReady();
	USART0_transmitByte((uint8_t)data);
	data = EXTRACT_BIT_FROM(data, NINTH_BIT_POS);
	SET_SHIFTED_BIT_MASK(UCSR0B, GET_RIGHT_SHIFTED_BIT_MASK_OF(data, TXB80_TO_9BIT_DISTANCE), TXB80_IN_REG_POS);
}

#ifdef MCU_328PB

byte_t USART1_getByte()
{
	USART1_waitForReceiveReady();
	return USART1_receiveByte();
}

#define RXB81_TO_9BIT_DISTANCE 7
#define RXB81_IN_REG_POS 1
usart_9bit_data_t USART1_get9BitData()
{
	USART1_waitForReceiveReady();
	usart_9bit_data_t data = USART1_receiveByte();//get normal byte
	SET_SHIFTED_BIT_MASK(data, EXTRACT_BIT_FROM(UCSR1B, RXB81_IN_REG_POS), RXB81_TO_9BIT_DISTANCE);// get and set 9th bit
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

#define TXB81_IN_REG_POS 0
#define TXB81_TO_9BIT_DISTANCE 8
void USART1_send9BitData(usart_9bit_data_t data)
{
	USART1_waitForTransmitReady();
	USART1_transmitByte((uint8_t)data);
	data = EXTRACT_BIT_FROM(data, TXB81_TO_9BIT_DISTANCE);
	SET_SHIFTED_BIT_MASK(UCSR1B, GET_RIGHT_SHIFTED_BIT_MASK_OF(data, TXB81_TO_9BIT_DISTANCE), TXB81_IN_REG_POS);
}

#endif

//sending string of bytes/chars/9-bit data

void USART0_sendDataSeries(const byte_t __memx* series, length_t length)
{
	while(length--)
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

void USART0_receiveSomeBytes(byte_t* target, length_t count)
{
	uint8_t array_idx = 0;
	while(count--)
	{
		target[array_idx] = USART0_getByte();
		++array_idx;
	}
}

#ifdef MCU_328PB

void USART1_sendDataSeries(const byte_t __memx* series, length_t length)
{
	while(length--)
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

void USART1_receiveSomeBytes(byte_t* target, length_t count)
{
	uint8_t array_idx = 0;
	while(count--)
	{
		target[array_idx] = USART1_getByte();
		++array_idx;
	}
}

#endif

//manually enabling/disabling usart functions

void USART0_receiveEnable()
{
	SET_BIT_AT(UCSR0B, RXEN0);
}

void USART0_transmitEnable()
{
	SET_BIT_AT(UCSR0B, TXEN0);
}

void USART0_receiveDisable()
{
	CLEAR_BIT_AT(UCSR0B, RXEN0);
}

void USART0_transmitDisable()
{
	CLEAR_BIT_AT(UCSR0B, TXEN0);
}

#ifdef MCU_328PB

void USART1_receiveEnable()
{
	SET_BIT_AT(UCSR1B, RXEN1);
}

void USART1_transmitEnable()
{
	SET_BIT_AT(UCSR1B, TXEN1);
}

void USART1_receiveDisable()
{
	CLEAR_BIT_AT(UCSR1B, RXEN1);
}

void USART1_transmitDisable()
{
	CLEAR_BIT_AT(UCSR1B, TXEN1);
}

#endif

void USART0_enable()
{
	setBitsAt((register_t*)&UCSR0B, RXEN0, TXEN0);
}

void USART0_disable()
{
	clearBitsAt((register_t*)&UCSR0B, RXEN0, TXEN0);
}

#ifdef MCU_328PB

void USART1_enable()
{
	setBitsAt((register_t*)&UCSR1B, RXEN1, TXEN1);
}

void USART1_disable()
{
	clearBitsAt((register_t*)&UCSR1B, RXEN1, TXEN1);
}

#endif

//waiting for device I/O ready

void USART0_waitForReceiveReady()
{
	while (IS_BIT_CLEARED_AT(UCSR0A, RXC0));
}

void USART0_waitForTransmitReady()
{
	while (IS_BIT_CLEARED_AT(UCSR0A, UDRE0));
}

#ifdef MCU_328PB

void USART1_waitForReceiveReady()
{
	while (IS_BIT_CLEARED_AT(UCSR1A, RXC1));
}
void USART1_waitForTransmitReady()
{
	while (IS_BIT_CLEARED_AT(UCSR1A, UDRE1));
}

//start frame detector flag operations

bool USART0_startConditionDetected()
{
	 return IS_BIT_SET_AT(UCSR0D, RXS);
}

void USART0_clearStartConditionDetectedFlag()
{
	SET_BIT_AT(UCSR0D, RXS);//setting up this bit clears the above flag
}

bool USART1_startConditionDetected()
{
	return IS_BIT_SET_AT(UCSR1D, RXS);
}

void USART1_clearStartConditionDetectedFlag()
{
	SET_BIT_AT(UCSR1D, RXS);//setting up this bit clears the above flag
}

#endif


bool USART0_isTransmitReady()
{
	return IS_BIT_SET_AT(UCSR0A, UDRE0);
}

bool USART0_isReceiveReady()
{
	return IS_BIT_SET_AT(UCSR0A, RXC0);
}

bool USART0_isTransmiterBusy()
{
	return IS_BIT_CLEARED_AT(UCSR0A, TXC0);
}

bool USART0_transmitionCompleted()
{
	return IS_BIT_SET_AT(UCSR0A, TXC0);
}

bool USART0_frameErrorOccured()
{
	return IS_BIT_SET_AT(UCSR0A, FE0);
}

bool USART0_parityErrorOccured()
{
	return IS_BIT_SET_AT(UCSR0A, UPE0);
}

#ifdef MCU_328PB

bool USART1_isTransmitReady()
{
	return IS_BIT_SET_AT(UCSR1A, UDRE1);
}

bool USART1_isReceiveReady()
{
	return IS_BIT_SET_AT(UCSR1A, RXC1);
}

bool USART1_isTransmitBusy()
{
	return IS_BIT_CLEARED_AT(UCSR1A, TXC1);
}

bool USART1_transmitionCompleted()
{
	return IS_BIT_SET_AT(UCSR1A, TXC1);
}

bool USART1_frameErrorOccured()
{
	return IS_BIT_SET_AT(UCSR1A, FE1);
}

bool USART1_parityErrorOccured()
{
	return IS_BIT_SET_AT(UCSR1A, UPE1);
}

#endif

/////////////////////////////////	END		* USER_INTERFACE_IMPLEMENTATION *		END


/////////////////////////////////		LOCAL INITIALIZE IMPLEMENTATION 

//reg: UBRRnH , UBRRnL , UCSRnA
//bits : U2Xn
#define U2Xn_IN_REG_POS 1 
static inline void _injectBaudRate_impl(register_t* UBRRnL_reg,
										register_t* UBRRnH_reg,
										register_t* UCSRnA_reg)
{
	#include <util/setbaud.h>
	
	REPLACE_REGISTER(*UBRRnH_reg, UBRRH_VALUE);
	REPLACE_REGISTER(*UBRRnL_reg, UBRRL_VALUE);
	
	//UCSRnA , U2Xn
	#if USE_2X
		SET_BIT_AT(*UCSRnA_reg, U2Xn_IN_REG_POS);
	#else
		CLEAR_BIT_AT(*UCSRnA_reg, U2Xn_IN_REG_POS);
	#endif
}

#define _injectBaudRate(usart_nmbr) _injectBaudRate_impl(&UBRR##usart_nmbr##L,\
														 &UBRR##usart_nmbr##H,\
														 &UCSR##usart_nmbr##A)

//UCSRnC
//UMSELn1,UMSELn0
#define UMSELn0_IN_REG_POS 6
#define UMSELn1_IN_REG_POS 7
static inline void _setSyncMode_impl(enum USART_SyncMode sync_mode, register_t* UCSRnC_reg)
{
	clearBitsAt(UCSRnC_reg, UMSELn1_IN_REG_POS, UMSELn0_IN_REG_POS);
	SET_SHIFTED_BIT_MASK(*UCSRnC_reg, sync_mode, UMSELn0_IN_REG_POS);
}

#define _setSyncMode(sync_mode, usart_nmbr) _setSyncMode_impl(sync_mode, &UCSR##usart_nmbr##C)


//reg: UCSRnC
//bits : UPMn1,UPMn0
#define UPMn0_IN_REG_POS 4
#define UPMn1_IN_REG_POS 5
static inline void _setParityMode_impl(enum USART_ParityMode parity_mode, register_t* UCSRnC_reg)
{
	clearBitsAt(UCSRnC_reg, UPMn1_IN_REG_POS, UPMn0_IN_REG_POS);
	SET_SHIFTED_BIT_MASK(*UCSRnC_reg, parity_mode, UPMn0_IN_REG_POS);
}

#define _setParityMode(parity_mode, usart_nmbr) _setParityMode_impl(parity_mode, &UCSR##usart_nmbr##C)

//reg: UCSRnC
//bits: USBSn
#define USBSn_IN_REG_POS 3 
static inline void _setStopBits_impl(enum USART_StopBits stop_bits, register_t* UCSRnC_reg)
{
	CLEAR_BIT_AT(*UCSRnC_reg, USBSn_IN_REG_POS);
	SET_SHIFTED_BIT_MASK(*UCSRnC_reg, stop_bits,USBSn_IN_REG_POS);
}

#define _setStopBits(stop_bits, usart_nmbr) _setStopBits_impl(stop_bits, &UCSR##usart_nmbr##C)

//reg:  UCSRnC, UCSRnB,
//bits: UCSZn2,,UCSZn1,UCSZn0,
#define UCSZn2_IN_REG_POS 2
#define UCSZn1_IN_REG_POS 2
#define UCSZn0_IN_REG_POS 1
#define UCSZn2_IN_SETUP_POS 2
#define UCSZn1_IN_SETUP_POS 1
#define UCSZn0_IN_SETUP_POS 0
static inline void _setDataSize_impl(enum USART_DataSize data_size,
									 register_t* UCSRnB_reg,
									 register_t* UCSRnC_reg)
{
	clearBitsAt(UCSRnC_reg, UCSZn1_IN_REG_POS, UCSZn0_IN_REG_POS);
	CLEAR_BIT_AT(*UCSRnB_reg, UCSZn2_IN_REG_POS);
	
	SET_SHIFTED_BIT_MASK(*UCSRnC_reg, EXTRACT_BIT_MASK_FROM(data_size, UCSZn0_IN_SETUP_POS, UCSZn1_IN_SETUP_POS), UCSZn0_IN_REG_POS);
	SET_SHIFTED_BIT_MASK(*UCSRnB_reg, EXTRACT_BIT_MASK_FROM(data_size, UCSZn2_IN_SETUP_POS, UCSZn2_IN_SETUP_POS), NOTHING);
}

#define _setDataSize(data_size, usart_nmbr) _setDataSize_impl(data_size,\
															  &UCSR##usart_nmbr##B,\
															  &UCSR##usart_nmbr##C)

//reg:  UCSRnC,
//bits: UCPOLn
#define UCPOLn_IN_REG_POS 0
static inline void _setClockSignalPolarization_impl(enum USART_ClockSignalPolarization polar, register_t* UCSRnC_reg)
{
	CLEAR_BIT_AT(*UCSRnC_reg, UCPOLn_IN_REG_POS);
	SET_SHIFTED_BIT_MASK(*UCSRnC_reg, polar, UCPOLn_IN_REG_POS);
}

#define _setClockSignalPolarization(polar, usart_nmbr) _setClockSignalPolarization_impl(polar, &UCSR##usart_nmbr##C)

//reg: UCSRnA
//bit: MPCMn
#define MPCMn_IN_REG_POS 0
static inline void _setMultiprocessorMode_impl(enum USART_MultiprocessorMode multiprocesor_mode, register_t* UCSRnA_reg)
{
	CLEAR_BIT_AT(*UCSRnA_reg, MPCMn_IN_REG_POS);
	SET_SHIFTED_BIT_MASK(*UCSRnA_reg, multiprocesor_mode,MPCMn_IN_REG_POS);
}

#define _setMultiprocessorMode(multiprocesor_mode, usart_nmbr) _setMultiprocessorMode_impl(multiprocesor_mode,&UCSR##usart_nmbr##A)

#ifdef MCU_328PB

//reg: UCSRnD
//bits : SFDE,RXCIE
static inline void _setupFrameDetector_impl(USART_Setup* setup, register_t* UCSRnD_reg)
{
	if(setup->enable_frame_detector_)
		SET_BIT_AT(*UCSRnD_reg, SFDE);
	if(setup->enable_frame_detector_interrupt_)
		SET_BIT_AT(*UCSRnD_reg, RXSIE);
}

#define _setupFrameDetector(setup_ptr, usart_nmbr) _setupFrameDetector_impl(setup_ptr, &UCSR##usart_nmbr##D)

#endif

#if USART_0_INTERRUPT_MODE || USART_1_INTERRUPT_MODE

//reg: UCSRnB
//bits: RXCIEn,TXCIEn,UDRIEn
#define  RXCIEn_IN_REG_POS 7
#define  TXCIEn_IN_REG_POS 6
#define  UDRIEn_IN_REG_POS 5
static inline void _setInterruptsMode_impl(enum USART_InterruptsMode interrupt_mode, register_t* UCSRnB_reg)
{
	clearBitsAt(UCSRnB_reg, RXCIEn_IN_REG_POS, TXCIEn_IN_REG_POS, UDRIEn_IN_REG_POS);
	SET_SHIFTED_BIT_MASK(*UCSRnB_reg, interrupt_mode, UDRIEn_IN_REG_POS);
}

#define _setInterruptsMode(interrupt_mode, usart_nmbr)	_setInterruptsMode_impl(interrupt_mode, &UCSR##usart_nmbr##B)

static inline void _setupBuffers_impl(USART_Setup* setup,
									  volatile CircularBuffer* receive_buffer,
									  volatile CircularBuffer* transmit_buffer,
									  volatile bool* transmit_flag)
{
	*receive_buffer  = CircularBuffer_initNormal(setup->receive_buffer_size_);
	*transmit_buffer = CircularBuffer_initNormal(setup->transmit_buffer_size_);
	*transmit_flag = false;
}

#define _setupBuffers(setup_ptr, usart_nmbr) _setupBuffers_impl(setup_ptr,\
															    &usart_##usart_nmbr##_receive_buffer,\
															    &usart_##usart_nmbr##_transmit_buffer,\
															    &usart_##usart_nmbr##_transmit_flag)

#endif

//////////////////////////////////////////////////////////////////////////		END init_impl


#if USART_0_INTERRUPT_MODE || USART_1_INTERRUPT_MODE

//Interrupts definitions,user defines only a handler

#define tryToExecuteHandler(handler, ...)\
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

void USART0_sendBytesToTransmitBuffer(byte_t* to_send, length_t size)
{
	length_t array_idx = 0;
	while(size--)
	{
		USART0_sendForceByteToTransmitBuffer(to_send[array_idx]);
		++array_idx;
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

enum OperationStatus USART0_getBytesFromReceiveBuffer(byte_t* target, length_t count)
{
	enum OperationStatus status = Success;
	
	length_t array_idx = 0;
	while(count-- && status != Failure)
	{
		status = USART0_getByteFromReceiveBuffer(target[array_idx]);
		++array_idx;
	}
	
	return status;
}


void USART0_waitForSeveralBytesInReceiveBuffer(byte_t* target, length_t count)
{
	length_t array_idx = 0;
	while(count--)
	{
		USART0_waitForByteInReceiveBuffer();
		USART0_getByteFromReceiveBuffer(target[array_idx]);
		++array_idx;
	}
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
	length_t array_idx = 0;
	while(status != Failure)
	{
		status = USART0_getByteFromReceiveBuffer(target[array_idx]);
		++array_idx;
	}
}

//compatibility defines
#ifndef MCU_328PB//for 328P
	#define USART0_RX_vect		USART_RX_vect
	#define USART0_TX_vect		USART_TX_vect
	#define USART0_UDRE_vect	USART_UDRE_vect
#endif

//this isr handles user handler and receives data
ISR(USART0_RX_vect)
{
	usart_0_received_byte = USART0_receiveByte();
	CircularBuffer_forcePush(usart_0_receive_buffer, usart_0_received_byte);
	tryToExecuteHandler(USART0_OnReceivedChar_Handler, usart_0_received_byte);
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

#if USART_1_INTERRUPT_MODE && MCU_328PB

void (* USART1_OnReceivedChar_Handler)(byte_t y)					= NULL;
void (* USART1_OnEmptyDataRegisterAndBuffer_Handler)()				= NULL;
void (* USART1_OnTransmitBufferAvalaibleSpaceInterrupt_Handler)()   = NULL;
void (* USART1_OnStartFrameDetection_Handler)()						= NULL;

volatile byte_t usart_1_received_byte;
volatile byte_t usart_1_transmited_byte;
volatile bool   usart_1_transmit_flag;

void USART1_sendBytesToTransmitBuffer(byte_t* to_send, length_t size)
{
	length_t array_idx = 0;
	while(size--)
	{
		USART1_sendForceByteToTransmitBuffer(to_send[array_idx]);
		++array_idx;
	}
}

enum OperationStatus USART1_sendByteFromTransmitBufferToDevice()
{
	enum OperationStatus status = Failure;
	status = USART1_getByteFromTransmitBuffer(usart_1_transmited_byte);
	if(status != Failure)
		USART1_sendByte(usart_1_transmited_byte);
	return status;
}

enum OperationStatus USART1_sendBytesFromTransmitBufferToDevice(length_t count)
{
	enum OperationStatus status = Failure;
	while(count-- && !CircularBuffer_isEmpty(usart_1_transmit_buffer))
		status = USART1_sendByteFromTransmitBufferToDevice();
	return status;
}

enum OperationStatus USART1_getBytesFromReceiveBuffer(byte_t* target, length_t count)
{
	enum OperationStatus status = Success;
	
	length_t array_idx = 0;
	while(count-- && status != Failure)
	{
		status = USART1_getByteFromReceiveBuffer(target[array_idx]);
		++array_idx;
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
	length_t array_idx = 0;
	while(status != Failure)
	{
		status = USART1_getByteFromReceiveBuffer(target[array_idx]);
		++array_idx;
	}
}

//this isr handles user handler and receives data
ISR(USART1_RX_vect)
{
	usart_1_received_byte = USART1_receiveByte();
	CircularBuffer_forcePush(usart_1_receive_buffer, usart_1_received_byte);
	tryToExecuteHandler(USART1_OnReceivedChar_Handler, usart_1_received_byte);
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

int put_0(char c, FILE* out_stream)
{
	USART0_waitForTransmitReady();
	USART0_transmitByte(c);
	return 0;
}
	
inline void _setupUsartStream0()
{
	FILE temp_file = FDEV_SETUP_STREAM(put_0, get_0, _FDEV_SETUP_RW);
	usart_0_io_file = temp_file;
}

#endif

#if USART_1_USE_STREAM && MCU_328PB

int get_1(FILE* in_stream)
{
	USART1_waitForReceiveReady();
	return USART1_receiveByte();
}

int put_1(char c, FILE* out_stream)
{
	USART1_waitForTransmitReady();
	USART1_transmitByte(c);
	return 0;
}

inline void _setupUsartStream1()
{
	FILE temp_file = FDEV_SETUP_STREAM(put_1, get_1, _FDEV_SETUP_RW);
	usart_1_io_file = temp_file;
}

#endif

//Initializations
//0- describes a USART0 number
void USART0_init(USART_Setup setup)
{
	
	power_usart0_enable();
	
	_setSyncMode(setup.sync_mode_, 0);
	_setParityMode(setup.parity_mode_, 0);
	_setStopBits(setup.stop_bits_, 0);
	_setDataSize(setup.data_size_, 0);
	_setClockSignalPolarization(setup.clock_signal_polarization_, 0);
	_setMultiprocessorMode(setup.multiprocessor_mode_, 0);

#ifdef USART_0_INTERRUPT_MODE
	
	_setupBuffers(&setup, 0);
	
#endif
	
	if(setup.startup_receive_enable_)
		USART0_receiveEnable();
	if(setup.startup_transmit_enable_)
		USART0_transmitEnable();
	
#ifdef USART_0_USE_STREAM
	
	_setupUsartStream0();
	
#endif

#ifdef USART_0_INTERRUPT_MODE

	_setInterruptsMode(setup.interrupt_mode_, 0);
	
#endif

	_injectBaudRate(0);

#ifdef MCU_328PB

	_setupFrameDetector(&setup, 0);

#endif


}

#if MCU_328PB
//1- describes a USART1 number
void USART1_init(USART_Setup setup)
{
	power_usart1_enable();
	
	_setSyncMode(setup.sync_mode_, 1);
	_setParityMode(setup.parity_mode_, 1);
	_setStopBits(setup.stop_bits_, 1);
	_setDataSize(setup.data_size_, 1);
	_setClockSignalPolarization(setup.clock_signal_polarization_, 1);
	_setMultiprocessorMode(setup.multiprocessor_mode_, 1);

	#ifdef USART_1_INTERRUPT_MODE
	
	_setupBuffers(&setup, USART_1);
	
	#endif
	
	if(setup.startup_receive_enable_)
		USART1_receiveEnable();
	if(setup.startup_transmit_enable_)
		USART1_transmitEnable();
	
	#ifdef USART_1_USE_STREAM
	
	_setupUsartStream1();
	
	#endif

	#ifdef USART_1_INTERRUPT_MODE

	_setInterruptsMode(setup.interrupt_mode_, 1);
	
	#endif

	_injectBaudRate(1);

	_setupFrameDetector(&setup, 1);

}

#endif
