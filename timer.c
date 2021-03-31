#include "timer.h"
#include <avr/power.h>

#define power_timer_enable(n) power_timer##n##_enable()

Timer0Setup Timer0_DefaultSettings = {0x0,0x0,0x0,255,255,0x0,0x0,0x0};
Timer1Setup Timer1_DefaultSettings = {0x0,0x0,0x0,0xFFFF,0xFFFF,0xFFFF,0x0,0x0,0x0,false,InputCaptureFallingEdge};
Timer2Setup Timer2_DefaultSettings = {0x0,0x0,0x0,255,255,0x0,0x0,0x0,false,false};	
static inline void _setTimerMode_impl(volatile uint8_t* control_register_A,volatile uint8_t* control_register_B,
							          uint8_t mask)
{
	clearBitsAt(control_register_A,0,1);
	clearBitsAt(control_register_B,3,4);
	uint8_t mask_A = EXTRACT_BIT_MASK_FROM(mask,0,1);
	uint8_t mask_B = EXTRACT_BIT_MASK_FROM(mask,2,3);
	SET_SHIFTED_BIT_MASK(*control_register_A,mask_A,0);
	SET_SHIFTED_BIT_MASK(*control_register_B,mask_B,3);
}

#define _setTimerMode(timer_number,timer_mode) \
	    _setTimerMode_impl((volatile uint8_t*)&TCCR##timer_number##A,(volatile uint8_t*)&TCCR##timer_number##B,timer_mode)

static inline void _setTimerPinMode_impl(volatile uint8_t* timer_control_register_A,
										 enum TimerPinMode pin_A_mode,
										 enum TimerPinMode pin_B_mode,
										 enum PinsUnderTimerControl pins_under_control)
{
	clearBitsAt(timer_control_register_A,7,6,5,4);
	
	uint8_t pin_mode_temp = GET_SHIFTED_BIT_MASK_OF(pin_A_mode,6)  WITH  GET_SHIFTED_BIT_MASK_OF(pin_B_mode,4);
	pin_mode_temp = FILTER_BIT_MASK(pin_mode_temp,pins_under_control);
	
	SET_SHIFTED_BIT_MASK(*timer_control_register_A,pin_mode_temp,0);
}

#define  _setTimerPinMode(timer_number,pin_A_mode,pin_B_mode,pins_under_control) \
		 _setTimerPinMode_impl((volatile uint8_t*)&TCCR##timer_number##A,pin_A_mode,pin_B_mode,pins_under_control)

static inline void _setTimerInterruptMode_impl(volatile uint8_t* timer_interrupt_register,
									           uint8_t interrupt_mode)
{
	WIPE_REGISTER(*timer_interrupt_register);
	SET_SHIFTED_BIT_MASK(*timer_interrupt_register,interrupt_mode,0);
};

#define _setTimerInterruptMode(timer_number,interrupt_mode) \
	_setTimerInterruptMode_impl((volatile uint8_t*)&TIFR##timer_number,interrupt_mode)

static inline void _setTimer0Prescaler(enum Timer0Prescaler prescaler)
{
	clearBitsAt((volatile uint8_t*)&TCCR0B,CS00,CS01,CS02);
	SET_SHIFTED_BIT_MASK(TCCR0B,prescaler,0);
};

static inline void _setTimer0CustomCompareValue(uint8_t custom_compare_value_A,
												uint8_t custom_compare_value_B)
{
	OCR0A = custom_compare_value_A;
	OCR0B = custom_compare_value_B;
}

void timer0Init(Timer0Setup setup,bool halt_all_timers_before_begin)
{
	power_timer_enable(0);//w³¹czenie zasilania dla zegara timera 0

	_setTimerMode(0,setup.mode_);
	_setTimer0Prescaler(setup.prescaler_);
	_setTimer0CustomCompareValue(setup.custom_compare_value_A_,setup.custom_compare_value_B_);
	_setTimerPinMode(0,setup.pin_A_mode_,setup.pin_B_mode_,setup.pins_under_control_);

	if(halt_all_timers_before_begin)
		haltAllTimers();
		
	_setTimerInterruptMode(0,setup.interrupt_mode_);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static inline void _setTimer2Prescaler(enum Timer2Prescaler prescaler)
{
	clearBitsAt(&TCCR2B,CS20,CS21,CS22);
	SET_SHIFTED_BIT_MASK(TCCR2B,prescaler,0);
};

static inline void _setTimer2CustomCompareValue(uint8_t custom_compare_value_A,
												uint8_t custom_compare_value_B)
{
	OCR2A = custom_compare_value_A;
	OCR2B = custom_compare_value_B;
}

static inline void _setupTimer2AsyncExternalClock(Timer2Setup setup)
{
	if(setup.use_asynchronous_mode_ && setup.use_external_clock_)
	{
		SET_BIT_AT(ASSR,EXCLK);
		setBitsAt((volatile uint8_t*)&ASSR,EXCLK,AS2);
	}
	else if(setup.use_external_clock_)
		SET_BIT_AT(ASSR,EXCLK);
	else if(setup.use_asynchronous_mode_)
		SET_BIT_AT(ASSR,AS2);
}

void timer2Init(Timer2Setup setup,bool halt_all_timers_before_begin)
{
	power_timer_enable(2);

	_setTimerMode(2,setup.mode_);
	_setTimerPinMode(2,setup.pin_A_mode_,setup.pin_B_mode_,setup.pins_under_control_);
	_setTimer2Prescaler(setup.prescaler_);
	_setTimer2CustomCompareValue(setup.custom_compare_value_A_,setup.custom_compare_value_B_);

	if(halt_all_timers_before_begin)
		haltAllTimers();

	_setTimerInterruptMode(2,setup.interrupt_mode_);
	_setupTimer2AsyncExternalClock(setup);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static inline void _setTimer1Prescaler(enum Timer1Prescaler prescaler)
{
	clearBitsAt(&TCCR1B,CS10,CS11,CS12);
	SET_SHIFTED_BIT_MASK(TCCR1B,prescaler,0);
};

static inline void _setTimer1InputCapture(bool input_compare_filtration,
										  enum InputCaptureEdge edge_mode)
{
	CLEAR_BIT_AT(TCCR1B,ICES1);
	SET_SHIFTED_BIT_MASK(TCCR1B,ICES1,6);
	if(input_compare_filtration)
		SET_BIT_AT(TCCR1B,ICNC1);
};

static inline void _setTimer1CustomOutputCompareValue(uint16_t custom_compare_value_A,
										              uint16_t custom_compare_value_B)
{
	OCR1A = custom_compare_value_A;
	OCR1B = custom_compare_value_B;
}

static inline void _setTimer1CustomInputCompareValue(uint16_t custom_input_compare_value)
{
	ICR1 = custom_input_compare_value; 
}

void timer1Init(Timer1Setup setup,bool halt_all_timers_before_begin)
{
	power_timer_enable(1);

	_setTimerPinMode(1,setup.pin_A_mode_,setup.pin_B_mode_,setup.pins_under_control_);
	_setTimerMode(1,setup.mode_);

	_setTimer1InputCapture(setup.input_compare_filtration_,setup.edge_mode_);
	_setTimer1Prescaler(setup.prescaler_);

	_setTimer1CustomOutputCompareValue(setup.custom_output_compare_value_A_,
									   setup.custom_output_compare_value_B_);
	_setTimer1CustomInputCompareValue(setup.custom_input_compare_value_);

	if(halt_all_timers_before_begin)
		haltAllTimers();

	_setTimerInterruptMode(1,setup.interrupt_mode_);
}