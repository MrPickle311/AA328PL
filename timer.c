#include "timer.h"
#include <avr/power.h>
#include "global_utils.h"

#define power_timer_enable(n) power_timer##n##_enable()

Timer0Setup Timer0_DefaultSettings = {0x0,0x0,0x0,255,255,0x0,0x0,0x0};
TIMER_16BitSetup TIMER_16bit_DefaultSettings  = {0x0,0x0,0x0,0xFFFF,0xFFFF,0xFFFF,0x0,0x0,0x0,false,TIMER_16Bit_InputCaptureFallingEdge};
Timer2Setup Timer2_DefaultSettings = {0x0,0x0,0x0,255,255,0x0,0x0,0x0,false,false};	

#define NOT_SHIFT 0

/////////////////////////////////////		timer common implementations

//common implementation for setting mode in a timer

#define WGMn0_in_settings_mask_pos 0
#define WGMn1_in_settings_mask_pos 1
#define WGMn2_in_settings_mask_pos 2
#define WGMn3_in_settings_mask_pos 3

//in TCCRnA bits positions
#define WGMn0_pos 0
#define WGMn1_pos 1
//in TCCRnB bits positions
#define WGMn2_pos 3
#define WGMn3_pos 4
//shift between position in settings_mask and TCCRnB register ( 3 - 2 = 1 )
#define WGMn1_shift 1

static inline void _setTimerMode_impl(register_t* TCCRnA_reg,
									  register_t* TCCRnB_reg,
							          mask_8bit_t settings_mask)
{
	clearBitsAt(TCCRnA_reg,WGMn0_pos,WGMn1_pos);
	clearBitsAt(TCCRnB_reg,WGMn2_pos,WGMn3_pos);
	mask_8bit_t TCCRnA_settings_mask = EXTRACT_BIT_MASK_FROM(settings_mask,WGMn0_in_settings_mask_pos,WGMn1_in_settings_mask_pos);
	mask_8bit_t TCCRnB_settings_mask = EXTRACT_BIT_MASK_FROM(settings_mask,WGMn2_in_settings_mask_pos,WGMn3_in_settings_mask_pos);
	SET_SHIFTED_BIT_MASK(*TCCRnA_reg,TCCRnA_settings_mask,NOT_SHIFT);
	SET_SHIFTED_BIT_MASK(*TCCRnB_reg,TCCRnB_settings_mask,WGMn1_shift);
}

#define _setTimerMode(timer_number,timer_mode) \
	    _setTimerMode_impl(&TCCR##timer_number##A,\
						   &TCCR##timer_number##B,\
						   timer_mode)


//common implementation for setting pin mode in a timer

//in TCCRnA bits positions
#define COMnB0_pos 4
#define COMnB1_pos 5
#define COMnA0_pos 6
#define COMnA1_pos 7

static inline void _setTimerPinMode_impl(register_t* TCCRnA_reg,
										 enum TIMER_PinMode pin_A_mode,
										 enum TIMER_PinMode pin_B_mode,
										 enum TIMER_PinsUnderControl pins_under_control)
{
	clearBitsAt(TCCRnA_reg,COMnA1_pos,COMnA0_pos,COMnB1_pos,COMnB0_pos);
	
	//propably to repair
	mask_8bit_t pin_mode_settings = GET_LEFT_SHIFTED_BIT_MASK_OF(pin_A_mode,COMnA0_pos)  WITH  
									GET_LEFT_SHIFTED_BIT_MASK_OF(pin_B_mode,COMnB0_pos);
									
	pin_mode_settings = FILTER_BIT_MASK(pin_mode_settings,pins_under_control);
	
	SET_SHIFTED_BIT_MASK(*TCCRnA_reg,pin_mode_settings,NOT_SHIFT);
}

#define  _setTimerPinMode(timer_number,pin_A_mode,pin_B_mode,pins_under_control) \
		 _setTimerPinMode_impl(&TCCR##timer_number##A,\
							   pin_A_mode,pin_B_mode,\
							   pins_under_control)


//common implementation for setting interrupts in a timer


static inline void _setTimerInterruptMode_impl(register_t* TIMSKn_reg,
									           mask_8bit_t interrupt_mode_settings_mask)
{
	WIPE_REGISTER(*TIMSKn_reg);
	SET_SHIFTED_BIT_MASK(*TIMSKn_reg,interrupt_mode_settings_mask,NOT_SHIFT);
}

#define _setTimerInterruptMode(timer_number,interrupt_mode) \
	    _setTimerInterruptMode_impl(&TIMSK##timer_number,interrupt_mode)


//common implementation for setting prescaler in a timer

//in registers
#define CSn0_pos 0
#define CSn1_pos 1
#define CSn2_pos 2

static inline void _setTimerPrescaler_impl(register_t* TCCRnB_reg,
										   mask_8bit_t prescaler_mode_settings_mask)
 {
	 clearBitsAt(TCCRnB_reg,CSn2_pos,CSn1_pos,CSn0_pos);
	 SET_SHIFTED_BIT_MASK(*TCCRnB_reg,prescaler_mode_settings_mask,NOT_SHIFT);
 }

#define setTimerPrescaler(timer_number,prescaler_settings) \
	    _setTimerPrescaler_impl(&TCCR##timer_number##B,\
							    prescaler_settings)

//
static inline void _setTimer0Prescaler(enum TIMER_SynchronousPrescaler prescaler)
{
	clearBitsAt(&TCCR0B,CS00,CS01,CS02);
	SET_SHIFTED_BIT_MASK(TCCR0B,prescaler,0);
}

static inline void _setTimer0CustomCompareValue(uint8_t custom_compare_value_A,
												uint8_t custom_compare_value_B)
{
	OCR0A = custom_compare_value_A;
	OCR0B = custom_compare_value_B;
}

void TIMER_0Init(Timer0Setup setup,bool halt_all_timers_before_begin)
{
	power_timer_enable(0);//w³¹czenie zasilania dla zegara timera 0

	_setTimerMode(0,setup.mode_);
	_setTimer0Prescaler(setup.prescaler_);
	_setTimer0CustomCompareValue(setup.custom_compare_value_A_,setup.custom_compare_value_B_);
	_setTimerPinMode(0,setup.pin_A_mode_,setup.pin_B_mode_,setup.pins_under_control_);

	if(halt_all_timers_before_begin)
		TIMER_haltAll();
		
	_setTimerInterruptMode(0,setup.interrupt_mode_);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static inline void _setTimer2Prescaler(enum TIMER_AsyncPrescaler prescaler)
{
	clearBitsAt(&TCCR2B,CS20,CS21,CS22);
	SET_SHIFTED_BIT_MASK(TCCR2B,prescaler,0);
}

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

void TIMER_2Init(Timer2Setup setup,bool halt_all_timers_before_begin)
{
	power_timer_enable(2);

	_setTimerMode(2,setup.mode_);
	_setTimerPinMode(2,setup.pin_A_mode_,setup.pin_B_mode_,setup.pins_under_control_);
	_setTimer2Prescaler(setup.prescaler_);
	_setTimer2CustomCompareValue(setup.custom_compare_value_A_,setup.custom_compare_value_B_);

	if(halt_all_timers_before_begin)
		TIMER_haltAll();

	_setTimerInterruptMode(2,setup.interrupt_mode_);
	_setupTimer2AsyncExternalClock(setup);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static inline void _setTimer1Prescaler(enum TIMER_SynchronousPrescaler prescaler)
{
	clearBitsAt(&TCCR1B,CS10,CS11,CS12);
	SET_SHIFTED_BIT_MASK(TCCR1B,prescaler,0);
}

static inline void _setTimer1InputCapture(bool input_compare_filtration,
										  enum TIMER_InputCaptureEdge edge_mode)
{
	CLEAR_BIT_AT(TCCR1B,ICES1);
	SET_SHIFTED_BIT_MASK(TCCR1B,ICES1,6);
	if(input_compare_filtration)
		SET_BIT_AT(TCCR1B,ICNC1);
}

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

void TIMER_1Init(TIMER_16BitSetup setup,bool halt_all_timers_before_begin)
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
		TIMER_haltAll();

	_setTimerInterruptMode(1,setup.interrupt_mode_);
}
