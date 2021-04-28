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

//prescaler for the timer 2 
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
	TIMER_16Bit_InputCapture_FallingEdge = 0b0,
	TIMER_16Bit_InputCapture_RisingEdge =  0b1
};

//checking whether a timer interrupt happens 

#define TOVn_pos  0
#define OCFnA_pos 1
#define OCFnB_pos 2
#define ICFn_pos  5

#define TIMER_overflowInterrupt_Occured(timer_number)				IS_BIT_SET_AT(TIFR##timer_number, TOVn_pos)
#define TIMER_outputCompareMathA_Occurred(timer_number)				IS_BIT_SET_AT(TIFR##timer_number, OCFnA_pos)
#define TIMER_outputCompareMathB_Occurred(timer_number)				IS_BIT_SET_AT(TIFR##timer_number, OCFnB_pos)
#define TIMER_inputCaptureInterrupt_Occurred(timer_number)			IS_BIT_SET_AT(TIFR##timer_number, ICFn_pos)

//waiting for a timer interrupt

#define TIMER_waitFor_CounterOverflow_Interrupt(timer_number)		while( ! TIMER_overflowInterrupt_Occured(timer_number) )
#define TIMER_waitFor_CompareMathA_Interrupt(timer_number)			while( ! TIMER_outputCompareMathA_Occurred(timer_number) )//while( IS_BIT_CLEARED_AT(TIFR##timer_number,1) )
#define TIMER_waitFor_CompareMathB_Interrupt(timer_number)			while( ! TIMER_outputCompareMathB_Occurred(timer_number) )
#define TIMER_waitFor_InputCapture_Interrupt(timer_number)			while( ! TIMER_inputCaptureInterrupt_Occurred(timer_number) )

//reseting interrupt flags

#define TIMER_resetCounterOverflow_InterruptFlag(timer_number)		SET_BIT_AT(TIFR##timer_number, TOVn_pos)
#define TIMER_resetCompareMathA_InterruptFlag(timer_number)			SET_BIT_AT(TIFR##timer_number, OCFnA_pos)
#define TIMER_resetCompareMathB_InterruptFlag(timer_number)			SET_BIT_AT(TIFR##timer_number, OCFnB_pos)
#define TIMER_inputCapture_InterruptFlag(timer_number)				SET_BIT_AT(TIFR##timer_number, ICFn_pos)

//getters and setters for a timer current counter value 

#define TIMER_getCounterActual_Value(timer_number)					TCNT##timer_number
#define TIMER_setCounterActual_Value(timer_number, value)			TCNT##timer_number = value;

#define TIMER_16Bit_getInputCompare_Value(timer_number)				ICR##timer_number
#define TIMER_16Bit_setInputCompare_Value(timer_number, value)		ICR##timer_number = value

#define TIMER_getOutputCompareA_Value(timer_number)					OCR##timer_number##A
#define TIMER_setOutputCompareA_Value(timer_number, value)			OCR##timer_number##A = value

#define TIMER_getOutputCompareB_Value(timer_number)					OCR##timer_number##B
#define TIMER_setOutputCompareB_Value(timer_number, value)			OCR##timer_number##B = value

//force compare match for a timers

#define FOCnA_pos 7
#define FOCnB_pos 6

#define TIMER_8Bit_forceCompareMatchA(timer_number)					SET_BIT_AT(TCCR##timer_number##B, FOCnA_pos)
#define TIMER_8Bit_forceCompareMatchB(timer_number)					SET_BIT_AT(TCCR##timer_number##B, FOCnB_pos)

#define TIMER_16Bit_forceCompareMatchA(timer_number)				SET_BIT_AT(TCCR##timer_number##C, FOCnA_pos)
#define TIMER_16Bit_forceCompareMatchB(timer_number)				SET_BIT_AT(TCCR##timer_number##C, FOCnB_pos)

//waiting for a expected_value in a timer counter

#define TIMER_waitForCounterValue(timer_number,expected_value)		while(TCNT##timer_number != expected_value)
#define TIMER_waitForCounterReset(timer_number)						while(TCNT##timer_number != 0)

//timer settings changing in a runtime 

//input capture edge changing
#define ICESn_pos 6

#define TIMER_16Bit_setInputCapture_FallingEdge(timer_number)		CLEAR_BIT_AT(TCCR##timer_number##B, ICESn_pos)
#define TIMER_16Bit_setInputCapture_RisingEdge(timer_number)		SET_BIT_AT(TCCR##timer_number##B, ICESn_pos)

//pin mode changing

#define COMnA_start_pos 6
#define COMnB_start_pos 4

//changing a pin mode automaticaly enables a pin control
#define TIMER_changePinA_Mode(timer_number, pin_A_mode)				SET_SHIFTED_BIT_MASK(TCCR##timer_number##A, pin_A_mode, COMnA_start_pos)		
#define TIMER_changePinB_Mode(timer_number, pin_B_mode)				SET_SHIFTED_BIT_MASK(TCCR##timer_number##A, pin_B_mode, COMnB_start_pos)

#define TIMER_disablePinControl(timer_number, pins_under_control)	FILTER_BIT_MASK(TCCR##timer_number##A, GET_REVERSE_MASK_OF(pins_under_control))

//Synchronization tools
#define TIMER_haltAll()											setBitsAt(&GTCCR,TSM,PSRASY,PSRSYNC)
#define TIMER_haltAsynchronousTimer()							setBitsAt(&GTCCR,TSM,PSRASY)
#define TIMER_haltSynchronousTimers()							setBitsAt(&GTCCR,TSM,PSRSYNC)

#define TIMER_releaseAll()										WIPE_REGISTER(GTCCR)
#define TIMER_resetAll()										TIMER_releaseAll()
#define TIMER_runAll()											TIMER_releaseAll()

#define TIMER_resetAsynchronousPrescaler()						SET_BIT_AT(GTCCR,PSRASY)
#define TIMER_resetSynchronousPrescaler()						SET_BIT_AT(GTCCR,PSRSYNC)

////////////////////////////////////////////////////////////////////////// TIMER SETUP SECTION

struct TIMER_0_Setup_struct
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

typedef struct TIMER_0_Setup_struct TIMER_0_Setup;
extern TIMER_0_Setup TIMER_0_DefaultSettings;

struct TIMER_2_Setup_struct
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

typedef struct TIMER_2_Setup_struct TIMER_2_Setup;
extern TIMER_2_Setup TIMER_2_DefaultSettings;

//16 bit timers setup

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

void TIMER_0_Init(TIMER_0_Setup setup,bool halt_all_timers_before_begin);
void TIMER_1_Init(TIMER_16BitSetup setup,bool halt_all_timers_before_begin);
void TIMER_2_Init(TIMER_2_Setup setup,bool halt_all_timers_before_begin);

void TIMER_0_TurnOff();
void TIMER_1_TurnOff();
void TIMER_2_TurnOff();

#ifdef MCU_328PB

void TIMER_3_Init(TIMER_16BitSetup setup,bool halt_all_timers_before_begin);
void TIMER_4_Init(TIMER_16BitSetup setup,bool halt_all_timers_before_begin);

void TIMER_3_TurnOff();
void TIMER_4_TurnOff();

#endif

#define TIMER_CounterOverflow_Interrupt(timer_number)	ISR(TIMER##timer_number##_OVF_vect)
#define TIMER_CompareMatchA_Interrupt(timer_number)		ISR(TIMER##timer_number##_COMPA_vect)
#define TIMER_CompareMatchB_Interrupt(timer_number)		ISR(TIMER##timer_number##_COMPB_vect)
#define TIMER_InputCapture_Interrupt(timer_number)		ISR(TIMER##timer_number##_CAPT_vect)

#endif /* TIMER_H_INCLUDED */
