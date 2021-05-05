/*
 * spi.h
 *
 * Created: 05/05/2021 18:03:42
 *  Author: Damian Wójcik
 */ 

#ifndef SPI_H_
#define SPI_H_

#include <stdbool.h>
#include "bit_utils.h"
#include "global_utils.h"

//checking transmition state

#ifndef _AVR_ATMEGA328PB_H_INCLUDED

#define SPCR0	SPCR
#define SPSR0	SPSR
#define SPDR0	SPDR

#endif

#define SPI0_transmitionCompleted()					IS_BIT_SET_AT(SPSR0, SPIF)

#define SPI0_clearTransmitionCompleted_Flag()		SPDR0

#define SPI0_collisionOccured()						IS_BIT_SET_AT(SPSR0, WCOL)

#define SPI0_clearCollisionOccured_Flag()			SPSR0

#define SPI0_waitForTransmitionCompleted()			while(!(SPSR0 & _BV(SPIF)))

#define SPI0_receiveByte()							SPDR0

#define SPI0_sendByte(byte)							SPDR0 = byte

#ifdef _AVR_ATMEGA328PB_H_INCLUDED()

#define SPI1_transmitionCompleted()					IS_BIT_SET_AT(SPSR1, SPIF)

#define SPI1_clearTransmitionCompleted_Flag()		SPDR1

#define SPI1_collisionOccured()						IS_BIT_SET_AT(SPSR1, WCOL)

#define SPI1_clearCollisionOccured_Flag()			SPSR1

#define SPI1_waitForTransmitionCompleted			while(!(SPSR1 & _BV(SPIF)))

#define SPI1_receiveByte()							SPDR1
														
#define SPI1_sendByte(byte)							SPDR1 = byte

#endif

enum SPI_Mode
{
	SPI_Master = 0b1,
	SPI_Slave  = 0b0
};

enum SPI_TransmitionMode
{
	SPI_MSB_AtFirst = 0b0,
	SPI_LSB_AtFirst = 0b1
};

enum SPI_ClockPolarization
{
	SPI_LowState_AtIdleState  = 0b0,
	SPI_HighState_AtIdleState = 0b1
};

enum SPI_SCK_Fraquency
{
	SPI_SCK_2   = 0b100,
	SPI_SCK_4   = 0b000,
	SPI_SCK_8   = 0b101,
	SPI_SCK_16  = 0b001,
	SPI_SCK_32  = 0b110,
	SPI_SCK_64  = 0b111,
	SPI_SCK_128 = 0b011
};

enum SPI_SCK_SampledEdge
{
	SPI_RisingEdge  = 0b0,
	SPI_FallingEdge = 0b1
};

struct SPI_Setup
{
	enum SPI_Mode mode_;
	
	bool enable_interrupts_;//SPIE
	bool startup_enable_;//SPCR bit : SPE
	bool use_startup_interrupt_;
	
	enum SPI_SCK_SampledEdge   sampled_edge_;
	enum SPI_SCK_Fraquency	   sck_fraquency_;
	enum SPI_ClockPolarization clock_polarization_;
	enum SPI_TransmitionMode   _transmition_mode_;
};

typeid struct SPI_Setup SPI_Setup;

volatile SPI_Setup SPI_DefaultSetup;

void SPI0_Init(SPI_Setup setup);

#ifdef _AVR_ATMEGA328PB_H_INCLUDED

void SPI1_Init(SPI_Setup setup);

//transmition functions 

#endif

byte_t SPI0_exchangeByte(byte_t byte);

byte_t (*SPI0_InterruptHandler)(byte_t byte);//IT CANNOT BE NULL IF USE INTERRUPTS

void (*SPI0_StartTransmitionHandler)();

#ifdef _AVR_ATMEGA328PB_H_INCLUDED

byte_t SPI1_exchangeByte(byte_t byte);

byte_t (*SPI1_InterruptHandler)(byte_t byte);//IT CANNOT BE NULL IF USE INTERRUPTS

void (*SPI1_StartTransmitionHandler)();

#endif

#endif /* SPI_H_ */