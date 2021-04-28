#include "timer.h"
#include <avr/power.h>
#include "global_utils.h"

#define power_timer_enable(n) power_timer##n##_enable()

TIMER_0_Setup TIMER_0_DefaultSettings = {0x0, 0x0, 0x0, 255, 255, 0x0, 0x0, 0x0};
TIMER_16BitSetup TIMER_16bit_DefaultSettings  = {0x0, 0x0, 0x0, 0xFFFF, 0xFFFF, 0xFFFF, 0x0, 0x0, 0x0, false, TIMER_16Bit_InputCapture_FallingEdge};
TIMER_2_Setup TIMER_2_DefaultSettings = {0x0, 0x0, 0x0, 255, 255, 0x0, 0x0, 0x0,false, false};	

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
	clearBitsAt(TCCRnA_reg, WGMn0_pos, WGMn1_pos);
	clearBitsAt(TCCRnB_reg, WGMn2_pos, WGMn3_pos);
	mask_8bit_t TCCRnA_settings_mask = EXTRACT_BIT_MASK_FROM(settings_mask, WGMn0_in_settings_mask_pos, WGMn1_in_settings_mask_pos);
	mask_8bit_t TCCRnB_settings_mask = EXTRACT_BIT_MASK_FROM(settings_mask, WGMn2_in_settings_mask_pos, WGMn3_in_settings_mask_pos);
	SET_SHIFTED_BIT_MASK(*TCCRnA_reg, TCCRnA_settings_mask, NOT_SHIFT);
	SET_SHIFTED_BIT_MASK(*TCCRnB_reg, TCCRnB_settings_mask, WGMn1_shift);
}

#define _setTimerMode(timer_number, timer_mode) \
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
	clearBitsAt(TCCRnA_reg, COMnA1_pos, COMnA0_pos, COMnB1_pos, COMnB0_pos);
	
	//propably to repair
	mask_8bit_t pin_mode_settings = GET_LEFT_SHIFTED_BIT_MASK_OF(pin_A_mode, COMnA0_pos)  WITH  
									GET_LEFT_SHIFTED_BIT_MASK_OF(pin_B_mode, COMnB0_pos);
									
	pin_mode_settings = FILTER_BIT_MASK(pin_mode_settings, pins_under_control);
	
	SET_SHIFTED_BIT_MASK(*TCCRnA_reg, pin_mode_settings, NOT_SHIFT);
}

#define  _setTimerPinMode(timer_number,\
						  pin_A_mode,\
						  pin_B_mode,\
						  pins_under_control)\
		 _setTimerPinMode_impl(&TCCR##timer_number##A,\
							   pin_A_mode,\
							   pin_B_mode,\
							   pins_under_control)


//common implementation for setting interrupts in a timer


static inline void _setTimerInterruptMode_impl(register_t* TIMSKn_reg,
									           mask_8bit_t interrupt_mode_settings_mask)
{
	WIPE_REGISTER(*TIMSKn_reg);
	SET_SHIFTED_BIT_MASK(*TIMSKn_reg, interrupt_mode_settings_mask, NOT_SHIFT);
}

#define _setTimerInterruptMode(timer_number, interrupt_mode) \
	    _setTimerInterruptMode_impl(&TIMSK##timer_number, interrupt_mode)


//common implementation for setting prescaler in a timer

//in registers
#define CSn0_pos 0
#define CSn1_pos 1
#define CSn2_pos 2

static inline void _setTimerPrescaler_impl(register_t* TCCRnB_reg,
										   mask_8bit_t prescaler_mode_settings_mask)
 {
	 clearBitsAt(TCCRnB_reg, CSn2_pos, CSn1_pos, CSn0_pos);
	 SET_SHIFTED_BIT_MASK(*TCCRnB_reg, prescaler_mode_settings_mask, NOT_SHIFT);
 }

#define _setTimerPrescaler(timer_number, prescaler_settings) \
	    _setTimerPrescaler_impl(&TCCR##timer_number##B,\
							    prescaler_settings)

//common implementation for setting custom output compare value in a timer

static inline void _setTimerCustomOutputCompareValue_impl(register_16bit_t* OCRnA_reg,
														  register_16bit_t* OCRnB_reg,
														  uint16_t custom_out_compare_value_A,
														  uint16_t custom_out_compare_value_B)
{
	*OCRnA_reg = custom_out_compare_value_A;
	*OCRnB_reg = custom_out_compare_value_B;
}

#define _setTimerCustomOutputCompareValue(timer_number,\
									custom_compare_value_A,\
									custom_compare_value_B) \
		_setTimerCustomOutputCompareValue_impl((volatile uint16_t*)&OCR##timer_number##A,\
										       (volatile uint16_t*)&OCR##timer_number##B,\
										       custom_compare_value_A,\
										       custom_compare_value_B)

//setting input compare match for 16 bit timers

static inline void _setTimer_16Bit_CustomInputCompareValue_impl(register_16bit_t* ICRn_reg,
																uint16_t custom_in_compare_value)
{
	*ICRn_reg = custom_in_compare_value;
}

#define _setTimer_16Bit_CustomInputCompareValue(timer_number, custom_compare_value)\
	    _setTimer_16Bit_CustomInputCompareValue_impl(&ICR##timer_number, custom_compare_value)


#define ICNCn_pos 7
#define ICESn_pos 6

static inline void _setTimerInputCapture_16Bit_impl(register_t* TCCRnB_reg,
										            bool input_compare_filtration,
										            enum TIMER_InputCaptureEdge edge_mode)
{
	CLEAR_BIT_AT(*TCCRnB_reg, ICESn_pos);
	SET_SHIFTED_BIT_MASK(*TCCRnB_reg, edge_mode, ICESn_pos);
	
	if(input_compare_filtration)
		SET_BIT_AT(*TCCRnB_reg, ICNCn_pos);
}

#define _setTimer_16Bit_InputCapture(timer_number,\
							         input_compare_filtration,\
							         edge_mode)\
		_setTimerInputCapture_16Bit_impl(&TCCR##timer_number##B,\
								         input_compare_filtration,\
								         edge_mode)

//turing off implementations

#define _timer_8Bit_turn_off_impl(timer_number)\
		WIPE_REGISTER(TCCR##timer_number##A);\
		WIPE_REGISTER(TCCR##timer_number##B);\
		WIPE_REGISTER(TIMSK##timer_number);\
		WIPE_REGISTER(TIFR##timer_number);\
		WIPE_REGISTER(TCNT##timer_number);\
		WIPE_REGISTER(OCR##timer_number##A);\
		WIPE_REGISTER(OCR##timer_number##B)

//common things between 8bit and 16bit timers
#define _timer_16Bit_turn_off_impl(timer_number) \
		_timer_8Bit_turn_off_impl(timer_number);\
		WIPE_REGISTER(TCCR##timer_number##C);\
		WIPE_REGISTER(ICR##timer_number)

////////////////////// concrete timer initializations

void TIMER_0_Init(TIMER_0_Setup setup, bool halt_all_timers_before_begin)
{
	power_timer_enable(0);

	_setTimerMode(0, setup.mode_);
	_setTimerPrescaler(0, setup.prescaler_);
	_setTimerCustomOutputCompareValue(0, setup.custom_compare_value_A_, setup.custom_compare_value_B_);
	_setTimerPinMode(0,setup.pin_A_mode_, setup.pin_B_mode_, setup.pins_under_control_);

	if(halt_all_timers_before_begin)
		TIMER_haltAll();
		
	_setTimerInterruptMode(0, setup.interrupt_mode_);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static inline void _setupTimerAsyncExternalClock(TIMER_2_Setup setup)
{
	if(setup.use_asynchronous_mode_ && setup.use_external_clock_)
	{
		SET_BIT_AT(ASSR, EXCLK);
		setBitsAt(&ASSR, EXCLK, AS2);
	}
	else if(setup.use_external_clock_)
		SET_BIT_AT(ASSR, EXCLK);
	else if(setup.use_asynchronous_mode_)
		SET_BIT_AT(ASSR, AS2);
}

void TIMER_2_Init(TIMER_2_Setup setup,bool halt_all_timers_before_begin)
{
	power_timer_enable(2);

	_setTimerMode(2, setup.mode_);
	_setTimerPinMode(2, setup.pin_A_mode_, setup.pin_B_mode_, setup.pins_under_control_);
	_setTimerPrescaler(2, setup.prescaler_);
	_setTimerCustomOutputCompareValue(2, setup.custom_compare_value_A_, setup.custom_compare_value_B_);

	if(halt_all_timers_before_begin)
		TIMER_haltAll();

	_setTimerInterruptMode(2, setup.interrupt_mode_);
	_setupTimerAsyncExternalClock(setup);
}


void TIMER_0_TurnOff()
{
	_timer_8Bit_turn_off_impl(0);
}


void TIMER_1_TurnOff()
{
	_timer_16Bit_turn_off_impl(1);
}


void TIMER_2_TurnOff()
{
	_timer_8Bit_turn_off_impl(2);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TIMER_1_Init(TIMER_16BitSetup setup,bool halt_all_timers_before_begin)
{
	power_timer_enable(1);

	_setTimerPinMode(1, setup.pin_A_mode_, setup.pin_B_mode_, setup.pins_under_control_);
	_setTimerMode(1, setup.mode_);

	_setTimer_16Bit_InputCapture(1,setup.input_compare_filtration_, setup.edge_mode_);
	_setTimerPrescaler(1, setup.prescaler_);

	_setTimerCustomOutputCompareValue(1,
									   setup.custom_output_compare_value_A_,
									   setup.custom_output_compare_value_B_);
	_setTimer_16Bit_CustomInputCompareValue(1, setup.custom_input_compare_value_);

	if(halt_all_timers_before_begin)
		TIMER_haltAll();

	_setTimerInterruptMode(1, setup.interrupt_mode_);
}

#ifdef MCU_328PB

void TIMER_3_Init(TIMER_16BitSetup setup,bool halt_all_timers_before_begin)
{
	power_timer_enable(3);

	_setTimerPinMode(3, setup.pin_A_mode_, setup.pin_B_mode_, setup.pins_under_control_);
	_setTimerMode(3, setup.mode_);

	_setTimer_16Bit_InputCapture(4,setup.input_compare_filtration_, setup.edge_mode_);
	_setTimerPrescaler(3, setup.prescaler_);

	_setTimerCustomOutputCompareValue(3,
									  setup.custom_output_compare_value_A_,
									  setup.custom_output_compare_value_B_);
									  
	_setTimer_16Bit_CustomInputCompareValue(4, setup.custom_input_compare_value_);

	if(halt_all_timers_before_begin)
		TIMER_haltAll();

	_setTimerInterruptMode(3, setup.interrupt_mode_);
}

void TIMER_4_Init(TIMER_16BitSetup setup,bool halt_all_timers_before_begin)
{
	power_timer_enable(4);

	_setTimerPinMode(4, setup.pin_A_mode_, setup.pin_B_mode_, setup.pins_under_control_);
	_setTimerMode(4, setup.mode_);

	_setTimer_16Bit_InputCapture(4,setup.input_compare_filtration_, setup.edge_mode_);
	_setTimerPrescaler(4, setup.prescaler_);

	_setTimerCustomOutputCompareValue(4,
									  setup.custom_output_compare_value_A_,
									  setup.custom_output_compare_value_B_);
	
	_setTimer_16Bit_CustomInputCompareValue(4, setup.custom_input_compare_value_);

	if(halt_all_timers_before_begin)
		TIMER_haltAll();

	_setTimerInterruptMode(4, setup.interrupt_mode_);
}


void TIMER_3_TurnOff()
{
	_timer_16Bit_turn_off_impl(3);
}


void TIMER_4_TurnOff()
{
	_timer_16Bit_turn_off_impl(4);
}

#endif