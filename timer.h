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

enum Timer8BitMode //other bits
{
	Timer8Bit_Normal						= 0b000,
	Timer8Bit_PWM_PhaseCorrected_Top		= 0b001,
	Timer8Bit_CTC							= 0b010,
	Timer8Bit_FastPWM_Top					= 0b011,
	Timer8Bit_FastPWM_PhaseCorrected_Output = 0b101,
	Timer8Bit_FastPWM_Output				= 0b111
};

//WGM10,WGM11,WGM12,WGM13
enum Timer16BitMode
{
	Timer16Bit_Normal =								 0b0000,
	
	Timer16Bit_8BitPWM_PhaseCorrected_Top =			 0b0001,
	Timer16Bit_9BitPWM_PhaseCorrected_Top =			 0b0010,
	Timer16Bit_10BitPWM_PhaseCorrected_Top =		 0b0011,
	
	Timer16Bit_CTC_Output =							 0b0100,
	Timer16Bit_CTC_Input =							 0b1100,
	
	Timer16Bit_8BitFastPWM_Top =					 0b0101,
	Timer16Bit_9BitFastPWM_Top =					 0b0110,
	Timer16Bit_10BitFastPWM_Top =					 0b0111,
	
	Timer16Bit_PWM_PhaseAndFrequencyCorrect_Input =  0b1000,
	Timer16Bit_PWM_PhaseAndFrequencyCorrect_Output = 0b1001,
	
	Timer16Bit_PWM_PhaseCorrect_Input =				 0b1010,
	Timer16Bit_PWM_PhaseCorrect_Output =			 0b1011,
	
	Timer16Bit_FastPWM_Input =						 0b1110,
	Timer16Bit_FastPWM_Output =						 0b1111
};


//COM pins for all timers
enum TimerPinMode
{
	TimerNoControlPin    = 0b00,
	CompareMatchToogle   = 0b01,
	CompareMatchClearPin = 0b10,
	CompareMatchSetPin   = 0b11
};

enum Timer_PinsUnderControl
{
	Timer_OnlyPinA = 0b11000000,
	Timer_OnlyPinB = 0b00110000,
	Timer_BothPins = 0b11110000
};

enum Timer0Prescaler
{
	TimerDisabled						  = 0b000,
	TimerNoPrescaling					  = 0b100,
	TimerPrescaler8						  = 0b010,
	TimerPrescaler64					  = 0b011,
	TimerPrescaler256					  = 0b100,
	TimerPrescaler1024					  = 0b101,
	TimerExternalClockSourceT0FallingEdge = 0b110,
	TimerExternalClockSourceT0RisingEdge  = 0b111
};

#define Timer1Prescaler Timer0Prescaler

enum Timer2Prescaler
{
	Timer2_Disabled = 0b000,
	Timer2_NoPrescaling = 0b001,
	Timer2_Prescaler8 = 0b010,
	Timer2_Prescaler32 = 0b011,
	Timer2_Prescaler64 = 0b100,
	Timer2_Prescaler128 = 0b101,
	Timer2_Prescaler256 = 0b110,
	Timer2_Prescaler1024 = 0b111
};


enum Timer8BitInterruptMode
{
	Timer8Bit_NoInterrupts  = 0b000,
	Timer8Bit_Overflow	    = 0b001,
	Timer8Bit_CompareMatchA = 0b010,
	Timer8Bit_CompareMatchB = 0b100
};

enum Timer16BitInterruptMode
{
	Timer16Bit_NoInterrupts =	    0b000000,
	Timer16Bit_TimerOverflow =		0b000001,
	Timer16Bit_TimerCompareMatchA = 0b000010,
	Timer16Bit_TimerCompareMatchB = 0b000100,
	Timer16Bit_TimerCaptureEvent =  0b100000
};

enum InputCaptureEdge
{
	Timer16Bit_InputCaptureFallingEdge = 0b0,
	Timer16Bit_InputCaptureRisingEdge =  0b1
};

#define timerOverflowInterruptIsExecuting(timer_number)			IS_BIT_CLEARED_AT(TIFR##timer_number,0)
#define timerCompareMathAOccurred(timer_number)					IS_BIT_CLEARED_AT(TIFR##timer_number,1)
#define timerCompareMathBOccurred(timer_number)					IS_BIT_CLEARED_AT(TIFR##timer_number,2)

#define getTimerActualValue(timer_number)						TCNT##timer_number
#define setTimerActualValue(timer_number,value)					TCNT##timer_number = value;

#define Timer0_forceCompareMatchA()								SET_BIT_AT(TCCR0B,7)
#define Timer0_forceCompareMatchB()								SET_BIT_AT(TCCR0B,6)

#define waitForTimerOverflowInterrupt(timer_number)				while( IS_BIT_SET_AT(TIFR##timer_number,0) )
#define waitForTimerCompareMathAInterrupt(timer_number)			while( IS_BIT_SET_AT(TIFR##timer_number,1) )
#define waitForTimerCompareMathBInterrupt(timer_number)			while( IS_BIT_SET_AT(TIFR##timer_number,2) )

#define waitForTimerCounterValue(timer_number,expected_value)	while(TCNT##timer_number < expected_value)

#define resetTimerOverflowInterruptFlag(timer_number)			SET_BIT_AT(TIFR##timer_number,0)
#define resetTimerCompareMathAInterruptFlag(timer_number)		SET_BIT_AT(TIFR##timer_number,1)
#define resetTimerCompareMathBInterruptFlag(timer_number)		SET_BIT_AT(TIFR##timer_number,2)

#define getTimer1InputCompareValue()							ICR1

#define forceCompareMatchA()									SET_BIT_AT(TCCR1C,7)
#define forceCompareMatchB()									SET_BIT_AT(TCCR1C,6)

//Synchronization utilites
//set 1 resets prescaler
//PSRASY - timer2
//PSRSYNC - timer1 and timer0
//TSM halts timers
#define haltAllTimers()											setBitsAt((volatile uint8_t*)&GTCCR,TSM,PSRASY,PSRSYNC)
#define haltTimer2()											setBitsAt((volatile uint8_t*)&GTCCR,TSM,PSRASY)
#define haltTimer0AndTimer1()									setBitsAt((volatile uint8_t*)&GTCCR,TSM,PSRSYNC)

#define releaseAllTimers()										WIPE_REGISTER(GTCCR)
#define resetAllTimers()										releaseAllTimers()
#define runAllTimers()											releaseAllTimers()

#define resetTimer2Prescaler()									SET_BIT_AT(GTCCR,PSRASY)
#define resetTimer1AndTimer0Prescaler()							SET_BIT_AT(GTCCR,PSRSYNC)

#define timer2ForceCompareMatchA()								SET_BIT_AT(TCCR2B,7)
#define timer2ForceCompareMatchB()								SET_BIT_AT(TCCR2B,6)

struct Timer0Setup_struct
{
	enum Timer8BitMode mode_;
	enum Timer8BitInterruptMode interrupt_mode_;
	
	enum Timer0Prescaler prescaler_;
	uint8_t custom_compare_value_A_;
	uint8_t custom_compare_value_B_;
	
	enum Timer_PinsUnderControl pins_under_control_;
	enum TimerPinMode pin_A_mode_;
	enum TimerPinMode pin_B_mode_;
};

typedef struct Timer0Setup_struct Timer0Setup;
extern Timer0Setup Timer0_DefaultSettings;

struct Timer16BitSetup_struct
{
	enum Timer16BitMode mode_;
	enum Timer16BitInterruptMode interrupt_mode_;
	
	enum Timer1Prescaler prescaler_;
	uint16_t custom_output_compare_value_A_;
	uint16_t custom_output_compare_value_B_;
	uint16_t custom_input_compare_value_;
	
	enum Timer_PinsUnderControl pins_under_control_;
	enum TimerPinMode pin_A_mode_;
	enum TimerPinMode pin_B_mode_;
	
	bool input_compare_filtration_;
	enum InputCaptureEdge edge_mode_;
};

typedef struct Timer16BitSetup_struct Timer16BitSetup;
extern Timer16BitSetup Timer16bit_DefaultSettings;

struct Timer2Setup_struct
{
	enum Timer8BitMode mode_;
	enum Timer8BitInterruptMode interrupt_mode_;
	
	enum Timer2Prescaler prescaler_;
	uint8_t custom_compare_value_A_;
	uint8_t custom_compare_value_B_;
	
	enum Timer_PinsUnderControl pins_under_control_;
	enum TimerPinMode pin_A_mode_;
	enum TimerPinMode pin_B_mode_;
	
	bool use_external_clock_;
	bool use_asynchronous_mode_;
};

typedef struct Timer2Setup_struct Timer2Setup;
extern Timer2Setup Timer2_DefaultSettings;

void timer0Init(Timer0Setup setup,bool halt_all_timers_before_begin);
void timer1Init(Timer16BitSetup setup,bool halt_all_timers_before_begin);

void timer2Init(Timer2Setup setup,bool halt_all_timers_before_begin);

#endif /* TIMER_H_INCLUDED */
