/*
 * spi.c
 *
 * Created: 05/05/2021 18:03:55
 *  Author: Damian Wójcik
 */ 

#include "spi.h"
#include <util/atomic.h>

#define SPI0_ResetSS											CLEAR_BIT_AT(PORTB,DDB2);

#define SPI0_SetSS												SET_BIT_AT(PORTB,DDB2);

#ifdef _AVR_ATMEGA328PB_H_INCLUDED

#define SPI1_ResetSS											CLEAR_BIT_AT(PORTE,DDE2);
		   
#define SPI1_SetSS												SET_BIT_AT(PORTE,DDE2);

#endif

#define MSTR_pos 4

static inline void _setupMode(register_t* spi_control_register, enum SPI_Mode mode)
{
	CLEAR_BIT_AT(*spi_control_register , MSTR_pos );
	SET_SHIFTED_BIT_MASK(*spi_control_register , mode , MSTR_pos);
}

#define setupMode(spi_nmbr , spi_mode) _setMode(&SPCR##spi_nmbr , spi_mode)						
					
					
#define DORD_pos 5

static inline void _setupTransmitionMode(register_t* spi_control_register, enum SPI_TransmitionMode mode)
{
	CLEAR_BIT_AT(*spi_control_register , DORD_pos );
	SET_SHIFTED_BIT_MASK(*spi_control_register , mode , DORD_pos);
}

#define setupTransmitionMode(spi_nmbr , transmition_mode) _setMode(&SPCR##spi_nmbr , transmition_mode)	


#define SIE_pos 6

static inline void _setupEnable(register_t* spi_control_register, bool is_enable)
{
	CLEAR_BIT_AT(*spi_control_register , SIE_pos)
	if(is_enable)
		SET_BIT_AT(*spi_control_register , SIE_pos);
}

#define setupEnabling(spi_nmbr , is_enable)	_setupEnable(&SPCR##spi_nmbr , is_enable)

#define SPIE_pos 7

static inline void _setupInterruption(register_t* spi_control_register, bool enable_interrupts)
{
	CLEAR_BIT_AT(*spi_control_register , SPIE_pos)
	if(enable_interrupts)
		SET_BIT_AT(*spi_control_register , SPIE_pos);
}

#define setupInterruption(spi_nmbr , enable_interrupts)	_setupInterruption(&SPCR##spi_nmbr , sending_mode)


#define CPOL_pos 3

static inline void _setupClockPolarization(register_t* spi_control_register, enum SPI_ClockPolarization polarization)
{
	CLEAR_BIT_AT(*spi_control_register , CPOL_pos );
	SET_SHIFTED_BIT_MASK(*spi_control_register , polarization , CPOL_pos);
}

#define setupClockPolarization(spi_nmbr , polarization_mode)	_setupClockPolarization(&SPCR##spi_nmbr , polarization_mode)


#define CPHA_pos 2 

static inline void _setupSampledEdge(register_t* spi_control_register, enum SPI_SCK_SampledEdge sampled_edge)
{
	CLEAR_BIT_AT(*spi_control_register , CPHA_pos );
	SET_SHIFTED_BIT_MASK(*spi_control_register , sampled_edge , CPHA_pos);
}

#define setupSampledEdge(spi_nmbr , sampled_edge)		_setupSampledEdge(&SPCR##spi_nmbr , sampled_edge )


#define SPR0_pos  0
#define SPR1_pos  1
#define SPI2X_pos 0

static inline void _setupSCKFraquency(register_t* spi_control_register,
									  register_t* spi_status_register,
									  enum SPI_SCK_Fraquency fraquency)
{
	clearBitsAt(spi_control_register , SPR1_pos,SPR0_pos );
	CLEAR_BIT_AT(*spi_status_register , SPI2X_pos);
	SET_SHIFTED_BIT_MASK(*spi_control_register, EXTRACT_BIT_MASK_FROM(fraquency, SPR0_pos, SPR1_pos), NOTHING);
	SET_SHIFTED_BIT_MASK(*spi_control_register, EXTRACT_BIT_MASK_FROM(fraquency, SPI2X_pos, SPI2X_pos), NOTHING);
}

#define  setupSCKFraquency(spi_nmbr , sck_fraquency)		_setupSCKFraquency(&SPCR##spi_nmbr ,\
																		       &SPSR##spi_nmbr ,\
																		       sck_fraquency )

static inline void _spi0_Master_setupPins()
{
	setBitsAt(&DDRB , DDB2 , DDB3 , DDB5);//mosi ,ss ,sck out
}

static inline void _spi0_Slave_setupPins()
{
	SET_BIT_AT(DDRB , DDB4);//miso out
}

static inline void _spi0_setupStartTransmitionInterrupt()
{
	SET_BIT_AT(PCMSK0 , PCINT2 );
	SET_BIT_AT(PCICR , PCIE0);
}

#ifdef _AVR_ATMEGA328PB_H_INCLUDED

static inline void _spi1_setupStartTransmitionInterrupt()
{
	SET_BIT_AT(PCMSK3 , PCINT26 );
	SET_BIT_AT(PCICR , PCIE3);
}

#endif

#ifdef _AVR_ATMEGA328PB_H_INCLUDED

static inline void _spi1_Master_setupPins()
{
	SET_BIT_AT(DDRC , DDC1 );//,sck out
	setBitsAt(&DDRE , DDRE2 , DDRE3);//mosi ,ss out
}

static inline void _spi1_Slave_setupPins()
{
	SET_BIT_AT(DDRC , DDC0);//miso out
}

#endif

void SPI0_Init(SPI_Setup setup)
{
	if(setup.mode_ == SPI_Master)
		SPI0_SetSS();
	
	
	setupMode(0,setup.mode_);
	if(setup.mode_ == SPI_Master)
		_spi0_Master_setupPins();
	else _spi0_Slave_setupPins();
	
	if(setup.use_startup_interrupt_)
		_spi0_setupStartTransmitionInterrupt();
	
	setupClockPolarization(0, setup.clock_polarization_);
	
	setupEnabling(0,  setup.startup_enable_);
	
	setupInterruption(0, setup.enable_interrupts_);
	
	setupSampledEdge(0, setup.sampled_edge_);
	
	setupTransmitionMode(0 , setup._transmition_mode_);
	
	setupSCKFraquency(0, setup.sck_fraquency_);
	
	
	SPI0_clearTransmitionCompleted_Flag();
	SPI0_clearCollisionOccured_Flag();
}


#ifdef _AVR_ATMEGA328PB_H_INCLUDED

void SPI1_Init(SPI_Setup setup)
{
	if(setup.mode_ == SPI_Master)
	SPI1_SetSS();
	
	
	setupMode(1,setup.mode_);
	if(setup.mode_ == SPI_Master)
		_spi1_Master_setupPins();
	else _spi1_Slave_setupPins();
	
	if(setup.use_startup_interrupt_)
		_spi1_setupStartTransmitionInterrupt();
	
	setupClockPolarization(1, setup.clock_polarization_);
	
	setupEnabling(1,  setup.startup_enable_);
	
	setupInterruption(1, setup.enable_interrupts_);
	
	setupSampledEdge(1, setup.sampled_edge_);
	
	setupTransmitionMode(1 , setup._transmition_mode_);
	
	setupSCKFraquency(1, setup.sck_fraquency_);
	
	
	SPI1_clearTransmitionCompleted_Flag();
	SPI1_clearCollisionOccured_Flag();
}

byte_t SPI0_exchangeByte(byte_t byte)
{
	SPI0_sendByte(byte);
	SPI0_waitForTransmitionCompleted();
	return SPI0_receiveByte();
}



#ifdef _AVR_ATMEGA328PB_H_INCLUDED

byte_t SPI1_exchangeByte(byte_t byte)
{
	SPI1_sendByte(byte);
	SPI1_waitForTransmitionCompleted();
	return SPI1_receiveByte();
}

#endif

//interuupts service

ISR(SPI0_STC_vect)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		SPI0_sendByte( SPI0_InterruptHandler( SPI0_receiveByte() ) );
	}
}

ISR(PCINT0_vect)
{
	//DAJ OBS£UGÊ ŒLEDZENIA STANU TEGO PINU PB2
	if(SPI0_StartTransmitionHandler != NULL)
		SPI0_StartTransmitionHandler();
}

#ifdef _AVR_ATMEGA328PB_H_INCLUDED

ISR(SPI1_STC_vect)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		SPI1_sendByte( SPI1_InterruptHandler( SPI1_receiveByte() ) );
	}
}

ISR(PCINT3_vect)
{
	//DAJ OBS£UGÊ ŒLEDZENIA STANU TEGO PINU PB2
	if(SPI1_StartTransmitionHandler != NULL)
		SPI1_StartTransmitionHandler();
}

#endif