/*
 * timer.h
 *
 * Created: 19/03/2021 08:27:20
 *  Author: Damian Wójcik
 */ 

//Czêstotliwoœæ zegara 16-bitowego = F_CLK / ( preskaler * 65536) ;

#ifndef TIMER_H_
#define TIMER_H_

//tryb prosty 

//timer potrafi sterowaæ pinami 
//tutaj ju¿ nie trzeba ogarniaæ przerwañ , init ustawia timer tak ,¿e on sam zmienia stany pinów sprzêtowo
//wiêc nawet sei nie trzeba dawaæ 
void initTimer()
{
	//przy ka¿dym zdarzeniu Compare Match stan portu zmienia siê na przeciwny
	TCCR1A|=(_BV(COM1A0) | _BV(COM1B0)); // W³¹cz sterowanie pinami IO
	//ale wtedy ju¿ nie mo¿na sterowaæ nimi za pomoc¹ PORTx
	
	//Zdarzenie to wyst¹pi po zliczeniu 32678 impulsów
	//ustawienie wype³nienia na 50% 
	OCR1A=32678; //dla pinu A
	OCR1B=32768; // dla pinu B
	//Stosunek wartoœci tych rejestrów do sta³ej 65 536 okreœla wspó³czynnik wype³nienia
	//tutaj rejestry OCR1A i OCR1B zawieraj¹ czas, po jakim ma nast¹piæ zdarzenie Compare Match
	//Czas ten jest równy OCR·preskaler taktów zegara
	
	//Ogólnie up³yw zadanego czasu sygnalizuje ustawienie
	//bitów zwi¹zanych z rejestrem OCR lub wywo³anie odpowiedniej funkcji obs³ugi przerwania
	//(TIMER1_COMPA_vect lub TIMER1_COMPB_vect).
	
	
	TCCR1B=_BV(CS12); // Preskaler 256
	TCCR1C=_BV(FOC1A); // Zmieñ stan jednego z pinów na przeciwny
	DDRB|=(_BV(PB1) | _BV(PB2)); // Piny steruj¹ce diodami s¹ wyjœciem
}

//za pomoc¹ przerwañ

void timer_init()
{
	TIMSK1=_BV(TOIE1);
	TCCR1B=_BV(CS12); // Preskaler 256
	DDRB|=(_BV(PB1) | _BV(PB2));
}

ISR(TIMER1_OVF_vect)
{
	static uint8_t LED=0b010; // Pocz¹tkowy stan diod
	LED^=0b110; // Zmieniamy go na przeciwny
	uint8_t tmp=PORTB & 0b11111001; // a nastêpnie wpisujemy do rejestru
	PORTB=(tmp | LED);
}

enum TimerMode
{
	TimerDisabled = 0b000,
	TimerNoPrescaling = 0b001,
	TimerPrescaler8 = 0b010,
	TimerPrescaler64 = 0b011,
	TimerPrescaler256 = 0b100,
	TimerPrescaler1024 = 0b101, 
	TimerExternalClockSourceFallingEdge = 0b110,
	TimerExternalClockSourceRisingEdge = 0b111
};

enum Timer0WaveformGeneratorMode
{
	Timer0Normal = 0b000,
	Timer0PWMPhaseCorrected = 0b001,
	Timer0CTC = 0b010,
	Timer0FastPWMWithoutPinControl = 0b011,
	Timer0FastPWMWithPinControl = 0111,
	Timer0FastPWMPhaseCorrected = 0b101,
};

enum Timer0InterruptMode
{
	Timer0CompareMatchA = BIT_MASK_OF(OCIE0A),
	Timer0CompareMatchB = BIT_MASK_OF(OCIE0A),
	Timer0Overflow = BIT_MASK_OF(TOIE0)
};

enum TimerPinControlMode
{
	TimerNotControlPin = 0b00,
	TimerCompareMatchTogglePin = 0b01,
	TimerCompareMatchClearPin = 0b10,
	TimerCompareMatchSetPin = 0b11
};

enum TimerPinControlSetup
{
	None,
	OnlyPinA,
	OnlyPinB,
	BothPins
};

#define  IS_TIMER0_COUNTING ( TIFR0 & BIT_MASK_OF(TOV0) ) > 0
#define  GET_TIMER0_CURRENT_VALUE TCNT0
#define  TIMER0_RESET_OVERFLOW_FLAG TIFR0 SET_BIT_AT(TIFR0,TOV0)

#define  TIMER_0_OVERFLOW_HAPPENED IS_BIT_SET_AT(TIFR0,0)
#define  TIMER_0_COMPARE_MATCH_A_HAPPENED IS_BIT_SET_AT(TIFR0,1)
#define  TIMER_0_COMPARE_MATCH_B_HAPPENED IS_BIT_SET_AT(TIFR0,2)

void __timer0SetupMode(enum TimerMode timer_mode)
{
	SET_SHIFTED_BIT_MASK(TCCR0B,timer_mode,0);
}

void __timer0SetupWaveformGenerator(enum Timer0WaveformGeneratorMode wave_form_generator)
{
	SET_SHIFTED_BIT_MASK(TCCR0A,EXTRACT_BIT_MASK_FROM(wave_form_generator,0,1),0);
	SET_SHIFTED_BIT_MASK(TCCR0B,EXTRACT_BIT_MASK_FROM(wave_form_generator,2,2),1);
}

void __timer0SetupInterruptMode(enum Timer0InterruptMode interrupt_mode)
{
	SET_SHIFTED_BIT_MASK(TIMSK0,interrupt_mode,0);
}

void __setupTmer0PinControlMode(enum TimerPinControlMode timer_pin_control_mode,enum TimerPinControlSetup timer_pin_control_setup)
{
	switch(timer_pin_control_setup)
	{
		case None: return;
		case OnlyPinA:
			SET_SHIFTED_BIT_MASK(TCCR0A,timer_pin_control_mode,6);
			return;
		case OnlyPinB:
			SET_SHIFTED_BIT_MASK(TCCR0A,timer_pin_control_mode,4);
			return;
		case BothPins:
			SET_SHIFTED_BIT_MASK(TCCR0A,timer_pin_control_mode,6);
			SET_SHIFTED_BIT_MASK(TCCR0A,timer_pin_control_mode,4);
			return;
	}
};

void setupTimer0(enum TimerMode timer_mode,enum Timer0WaveformGeneratorMode wave_form_generator,
				 enum Timer0InterruptMode interrupt_mode,enum TimerPinControlMode timer_pin_control_mode,
				 enum TimerPinControlSetup timer_pin_control_setup,uint8_t timer_top_count_value)
{
	 __timer0SetupMode(timer_mode);
	 __timer0SetupWaveformGenerator(wave_form_generator);
	 __timer0SetupInterruptMode(interrupt_mode);
	 __setupTmer0PinControlMode(timer_pin_control_mode,timer_pin_control_setup);
	 OCR0A = timer_top_count_value;
}


#endif /* TIMER_H_ */