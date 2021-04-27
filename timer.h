/*
Rejestry:

PRR:

Oszczêdzanie energii odbywa siê poprzez wy³¹czenie zegara taktuj¹cego wybrany
blok, co powoduje, ¿e stan danego bloku zostaje „zamro¿ony”

Poniewa¿ stan
bloku nie zmienia siê w momencie wy³¹czenia, po ponownym w³¹czeniu zegara taktuj¹cego
dany podsystem powraca do stanu, w jakim go „zamro¿ono”.

Po wy³¹czeniu danego podsystemu zwi¹zane z nim rejestry IO staj¹ siê niedostêpne
i nie mo¿na siê do nich odwo³ywaæ

<avr\power.h> <- tu s¹ makra w³¹czaj¹co - wy³¹czaj¹ce
power_timer0_enable()   (PRR &= (uint8_t)~(1 << PRTIM0))

WCHODZENIE DO PRZERWAÑ AUTOMATYCZNIE KASUJE FLAGI REJESTRÓW TIFRn

timer2 jest asynchroniczny
*/

#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/builtins.h>
#include "bit_utils.h"
#include "global_utils.h"

enum TIMER_8BitMode //other bits
{
	TIMER_8Bit_Normal							= 0b000,
	TIMER_8Bit_PWM_PhaseCorrected_Top			= 0b001,
	TIMER_8Bit_CTC								= 0b010,
	TIMER_8Bit_FastPWM_Top						= 0b011,
	TIMER_8Bit_FastPWM_PhaseCorrected_Output	= 0b101,
	TIMER_8Bit_FastPWM_Output					= 0b111
};

//WGM10,WGM11,WGM12,WGM13
enum TIMER_16BitMode
{
	TIMER_16Bit_Normal												= 0b0000,
	
	TIMER_16Bit_8BitPWM_PhaseCorrected_Top							= 0b0001,
	TIMER_16Bit_9BitPWM_PhaseCorrected_Top							= 0b0010,
	TIMER_16Bit_10BitPWM_PhaseCorrected_Top						    = 0b0011,
	
	TIMER_16Bit_CTC_Output											= 0b0100,
	TIMER_16Bit_CTC_Input											= 0b1100,
	
	TIMER_16Bit_8BitFastPWM_Top										= 0b0101,
	TIMER_16Bit_9BitFastPWM_Top										= 0b0110,
	TIMER_16Bit_10BitFastPWM_Top									= 0b0111,
	
	TIMER_16Bit_PWM_PhaseAndFrequencyCorrect_Input					= 0b1000,
	TIMER_16Bit_PWM_PhaseAndFrequencyCorrect_Output					= 0b1001,
	
	TIMER_16Bit_PWM_PhaseCorrect_Input								= 0b1010,
	TIMER_16Bit_PWM_PhaseCorrect_Output								= 0b1011,
	
	TIMER_16Bit_FastPWM_Input										= 0b1110,
	TIMER_16Bit_FastPWM_Output										= 0b1111
};


//COM pins for all timers
enum TIMER_PinMode
{
	TIMER_NoControlPin		   = 0b00,
	TIMER_CompareMatchToogle   = 0b01,
	TIMER_CompareMatchClearPin = 0b10,
	TIMER_CompareMatchSetPin   = 0b11
};

enum TIMER_PinsUnderControl
{
	TIMER_OnlyPinA = 0b11000000,
	TIMER_OnlyPinB = 0b00110000,
	TIMER_BothPins = 0b11110000
};

enum TIMER_SynchronousPrescaler
{
	TIMER_Synchronous_Disabled						   = 0b000,
	TIMER_Synchronous_NoPrescaling					   = 0b100,
	TIMER_Synchronous_Prescaler8					   = 0b010,
	TIMER_Synchronous_Prescaler64					   = 0b011,
	TIMER_Synchronous_Prescaler256					   = 0b100,
	TIMER_Synchronous_Prescaler1024					   = 0b101,
	TIMER_Synchronous_ExternalClockSourceT0FallingEdge = 0b110,
	TIMER_Synchronous_ExternalClockSourceT0RisingEdge  = 0b111
};

enum TIMER_AsyncPrescaler
{
	TIMER_Async_Disabled      = 0b000,
	TIMER_Async_NoPrescaling  = 0b001,
	TIMER_Async_Prescaler8    = 0b010,
	TIMER_Async_Prescaler32   =	0b011,
	TIMER_Async_Prescaler64   =	0b100,
	TIMER_Async_Prescaler128  =	0b101,
	TIMER_Async_Prescaler256  =	0b110,
	TIMER_Async_Prescaler1024 = 0b111
};


enum TIMER_8Bit_InterruptMode
{
	TIMER_8Bit_NoInterrupts  = 0b000,
	TIMER_8Bit_Overflow	     = 0b001,
	TIMER_8Bit_CompareMatchA = 0b010,
	TIMER_8Bit_CompareMatchB = 0b100
};

enum TIMER_16Bit_InterruptMode
{
	TIMER_16Bit_NoInterrupts	   = 0b000000,
	TIMER_16Bit_TimerOverflow      = 0b000001,
	TIMER_16Bit_TimerCompareMatchA = 0b000010,
	TIMER_16Bit_TimerCompareMatchB = 0b000100,
	TIMER_16Bit_TimerCaptureEvent  = 0b100000
};

enum TIMER_InputCaptureEdge
{
	TIMER_16Bit_InputCaptureFallingEdge = 0b0,
	TIMER_16Bit_InputCaptureRisingEdge =  0b1
};

#define TIMER_overflowInterruptIsExecuting(timer_number)			IS_BIT_CLEARED_AT(TIFR##timer_number,0)
#define TIMER_compareMathA_Occurred(timer_number)					IS_BIT_CLEARED_AT(TIFR##timer_number,1)
#define TIMER_compareMathB_Occurred(timer_number)					IS_BIT_CLEARED_AT(TIFR##timer_number,2)

#define TIMER_getCounterActualValue(timer_number)						TCNT##timer_number
#define TIMER_setCounterActualValue(timer_number,value)					TCNT##timer_number = value;
//TODO:
//add argument timer_number
#define TIMER_forceCompareMatchA()								SET_BIT_AT(TCCR0B,7)
#define TIMER_forceCompareMatchB()								SET_BIT_AT(TCCR0B,6)

#define TIMER_waitForCounterOverflow_Interrupt(timer_number)				while( IS_BIT_CLEARED_AT(TIFR##timer_number,0) )
#define TIMER_waitForCompareMathA_Interrupt(timer_number)			while( IS_BIT_CLEARED_AT(TIFR##timer_number,1) )
#define TIMER_waitForCompareMathB_Interrupt(timer_number)			while( IS_BIT_CLEARED_AT(TIFR##timer_number,2) )

#define TIMER_waitForCounterValue(timer_number,expected_value)	while(TCNT##timer_number < expected_value)

#define TIMER_resetCounterOverflow_InterruptFlag(timer_number)			SET_BIT_AT(TIFR##timer_number,0)
#define TIMER_resetCompareMathA_InterruptFlag(timer_number)		SET_BIT_AT(TIFR##timer_number,1)
#define TIMER_resetTimerCompareMathB_InterruptFlag(timer_number)		SET_BIT_AT(TIFR##timer_number,2)

//TODO:
//change to 16bit timer and allow the user to select an argument
#define TIMER_getInputCompareValue()								ICR1

#define TIMER_forceCompareMatchA()									SET_BIT_AT(TCCR1C,7)
#define TIMER_forceCompareMatchB()									SET_BIT_AT(TCCR1C,6)
//

//Synchronization utilites
//set 1 resets prescaler
//PSRASY - timer2
//PSRSYNC - timer1 and timer0
//TSM halts timers
#define TIMER_haltAll()											setBitsAt((register_t*)&GTCCR,TSM,PSRASY,PSRSYNC)
#define TIMER_haltAsynchronousTimer()											setBitsAt((register_t*)&GTCCR,TSM,PSRASY)
#define TIMER_haltSynchronousTimers()									setBitsAt((register_t*)&GTCCR,TSM,PSRSYNC)

//add macro/function which sets a OCRn /IRCn value

#define TIMER_releaseAll()										WIPE_REGISTER(GTCCR)
#define TIMER_resetAll()										TIMER_releaseAll()
#define TIMER_runAll()											TIMER_releaseAll()

#define TIMER_resetAsynchronousPrescaler()									SET_BIT_AT(GTCCR,PSRASY)
#define TIMER_resetSynchronousPrescaler()							SET_BIT_AT(GTCCR,PSRSYNC)

//TODO: Async compare match ? 
#define timer2ForceCompareMatchA()								SET_BIT_AT(TCCR2B,7)
#define timer2ForceCompareMatchB()								SET_BIT_AT(TCCR2B,6)
//
//SKONCZY£EM TUTAJ
//TODO: maybe delete struct for timer2 
//and create functions for use_external_clock_
struct TIMER_0Setup_struct
{
	enum TIMER_8BitMode mode_;
	enum TIMER_8Bit_InterruptMode interrupt_mode_;
	
	enum TIMER_SynchronousPrescaler prescaler_;
	uint8_t custom_compare_value_A_;
	uint8_t custom_compare_value_B_;
	
	enum TIMER_PinsUnderControl pins_under_control_;
	enum TIMER_PinMode pin_A_mode_;
	enum TIMER_PinMode pin_B_mode_;
};

typedef struct TIMER_0Setup_struct Timer0Setup;
extern Timer0Setup Timer0_DefaultSettings;

struct Timer2Setup_struct
{
	enum TIMER_8BitMode mode_;
	enum TIMER_8Bit_InterruptMode interrupt_mode_;
	
	enum TIMER_AsyncPrescaler prescaler_;
	uint8_t custom_compare_value_A_;
	uint8_t custom_compare_value_B_;
	
	enum TIMER_PinsUnderControl pins_under_control_;
	enum TIMER_PinMode pin_A_mode_;
	enum TIMER_PinMode pin_B_mode_;
	
	bool use_external_clock_;
	bool use_asynchronous_mode_;
};

typedef struct Timer2Setup_struct Timer2Setup;
extern Timer2Setup Timer2_DefaultSettings;
///
struct TIMER_16BitSetup_struct
{
	enum TIMER_16BitMode mode_;
	enum TIMER_16Bit_InterruptMode interrupt_mode_;
	
	enum TIMER_SynchronousPrescaler prescaler_;
	uint16_t custom_output_compare_value_A_;
	uint16_t custom_output_compare_value_B_;
	uint16_t custom_input_compare_value_;
	
	enum TIMER_PinsUnderControl pins_under_control_;
	enum TIMER_PinMode pin_A_mode_;
	enum TIMER_PinMode pin_B_mode_;
	
	bool input_compare_filtration_;
	enum TIMER_InputCaptureEdge edge_mode_;
};

typedef struct TIMER_16BitSetup_struct TIMER_16BitSetup;
extern TIMER_16BitSetup TIMER_16bit_DefaultSettings;

void TIMER_0Init(Timer0Setup setup,bool halt_all_timers_before_begin);
void TIMER_1Init(TIMER_16BitSetup setup,bool halt_all_timers_before_begin);

void TIMER_2Init(Timer2Setup setup,bool halt_all_timers_before_begin);

#endif /* TIMER_H_INCLUDED */
