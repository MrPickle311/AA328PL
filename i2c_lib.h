#ifndef I2C_LIB_H_
#define I2C_LIB_H_
//!!!
//////////////////////////////////////////////////////////////////////////
//Biblioteka wymaga prawid³owego zdefiniowania symbolu F_CPU oraz I2CBUSCLOCK
//////////////////////////////////////////////////////////////////////////
//Pamiêtaj o inicjalizacji I2C za pomoc¹ I2C_init()
//!!!!

//Rejestry
//TWBR  (ang. TW Bit Rate Register) -> oznacza on czêstotliwoœæ pracy magistrali TWI
//TWSR  (ang. Two Wire Status Register) -> okreœla konfiguracjê  

//TW_STATUS = TWSR & TW_STATUS_MASK

#include <util/twi.h>

//definicje i obs³uga b³êdów
#define I2C_STARTError 1
#define I2C_NoNACK 3
#define I2C_NoACK 4

uint8_t I2C_Error;

inline void I2C_setError(uint8_t err)
{
	I2C_Error = err;
}

//Rejestry TWI 
/*
TWINT - bit sygnalizuj¹cy koniec czynnoœci na magistrali I2C
TWEA - bit zezwalaj¹cy na generacjê sygna³u akceptacji ACK
TWSTA - generacja sekwencji START ( sprawdza zajêtoœæ linii SDA ) kasowany programowo
TWSTO - generacja sekwencji STOP
TWWC - bit detekcji kolizji na magistrali
TWEN - w³¹czenie interfejsu I2C (aktywny stan wysoki )
TWIE - bit maski przerwañ od interfejsu I2C

TWPS0 , TWPS1 - bity sterowania preskalerem
TWS7-TWS3  - bity statusu interfejsu I2C
*/
//Wzór na prêdkoœæ transmisji w zale¿nosci od TWBR to (jest on w dokumentacji oraz w internecie):
// SCL_fraquency = F_CPU / (16 +2*TWBR * 4^TWPS)

void I2C_setBusSpeed(uint16_t speed)//przyjmuje czêstotliwoœæ pracy magistrali TWI
{
	speed = (F_CPU / speed / 100 - 16) / 2; 
	//wyliczanie preskalera 
	//ale to przydatne tylko ,gdy zamierzamy zmieniaæ prêdkoœæ magistrali w trakcie pracy
	uint8_t prescaler = 0;
	while (speed > 255) //mapowanie
	{
		++prescaler;
		speed /= 4;
	}
	TWSR = ( TWSR & ( _BV(TWPS1) | _BV(TWPS0) ) ) | prescaler;
	TWBR = speed; // ten rejestr jest odpowiedzialny za generowanie sygna³u zegarowego
}

static inline void I2C_waitForComplete()
{
	while(!(TWCR & _BV(TWINT)));//czekanie na wyzerowanie
	//flagi TWINT, sygnalizuj¹cej zakoñczenie operacji na magistrali TWI
}

void I2C_init()
{
	TWCR = _BV(TWEA) | _BV(TWEN) ;//w³¹cz interfejs I2C , ewentualnie dowal | _BV(TWSTA)
	//while( !(TWCR & _BV(TWSTO) ) );// oczekujemy na ponowne ustawienie flagi TWINT, co potwierdzi, ¿e u
	//k³ad sprzêtowy wykona³ prawid³owo sekwencjê startu transmisji.
	//bit TWEN w³¹cza I2C , a bit TWEA -> automatyczne generowanie bitu ACK
	I2C_setBusSpeed(I2CBUSCLOCK/100);
}

void I2C_start()
{
	//Sygna³ START generowany jest automatycznie przez procesor poprzez ustawienie bitu
	//TWSTA (ang. TWI START Condition Bit).
	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
	//czekamy na nadanie sygna³u
	I2C_waitForComplete();
	//Jeœli wszystko przebieg³o prawid³owo, to rejestr stanu powinien zawieraæ wartoœæ
	//równ¹ definicji TW_START
	if(TW_STATUS != TW_START)
		I2C_setError(I2C_STARTError);
}

//Funkcja ta korzysta ze sprzêtowej realizacji sygna³u STOP (ustawienie bitu TWSTO). Zakoñczenie
//tej operacji sygnalizowane jest wyzerowaniem bitu TWSTO
static inline void I2C_stop()
{
	TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWSTO);
}

//zadaniem poni¿szej funkcji bêdzie oczekiwanie na zakoñczenie nadawania bitu STOP:
static inline void I2C_waitTillStopWasSent()
{
	while (TWCR & _BV(TWSTO));
}

//funkcja wysy³aj¹ca identyfikator wybranego urz¹dzenia slave
void I2C_sendAddr(uint8_t address)
{
	uint8_t status;
	if((address & 0x01)==0) 
		status = TW_MT_SLA_ACK;
	else status = TW_MR_SLA_ACK;
	TWDR = address;
	TWCR = _BV(TWINT) | _BV(TWEN);
	I2C_waitForComplete();
	if(TW_STATUS!=status) 
		I2C_setError(I2C_NoNACK);
}
//Funkcja ta wpisuje identyfikator wybranego urz¹dzenia slave (jego adres) do rejestru
//TWDR, z którego adres zostanie wys³any na magistralê TWI, a nastêpnie czeka na zakoñczenie
//transmisji i sprawdza rejestr stanu, czy nie wyst¹pi³ b³¹d

//Je¿eli wybieramy
//urz¹dzenie w trybie do zapisu, prawid³owa operacja sygnalizowana jest wartoœci¹
//rejestru stanu równ¹ TW_MT_SLA_ACK; w przypadku wyboru urz¹dzenia w trybie do
//odczytu prawid³owy przebieg operacji sygnalizuje wartoœæ TW_MR_SLA_ACK

//Poniewa¿ zawsze po wys³aniu sygna³u START musimy wys³aæ adres wybieranego urz¹dzenia
//slave, wygodnie jest stworzyæ funkcjê, która przeprowadza obie operacje równoczeœnie:

void I2C_sendStartAndSelect(uint8_t addr)
{
	I2C_start();
	I2C_sendAddr(addr);
}

//Poniewa¿ prawid³owy zapis danych
//generuje inny komunikat w rejestrze stanu ni¿ zapis adresu, potrzebujemy oddzielnej funkcji do zapisu danych:

void I2C_sendByte(uint8_t byte)
{
	TWDR = byte;
	TWCR = _BV(TWINT) | _BV(TWEN);
	I2C_waitForComplete();
	if(TW_STATUS!=TW_MT_DATA_ACK) 
		I2C_setError(I2C_NoACK);
}

//Operacje odczytu na magistrali I2C dziel¹ siê na dwa typy: odczyt z wygenerowaniem
//sygna³u potwierdzenia (ACK) oraz odczyt z wygenerowaniem sygna³u NACK.

//Kiedy odczytujemy
//tylko jeden bajt danych, urz¹dzenie master nie potwierdza otrzymania danych,
//nie generuje wiêc sygna³u ACK, dziêki czemu urz¹dzenie slave albo oczekuje na
//sygna³ koñcz¹cy transmisjê (STOP), albo po prostu samo przerywa transmisjê
//W tym
//przypadku kontynuacja wymiany danych mo¿liwa jest po otrzymaniu kolejnego sygna³u
//START wraz z adresem urz¹dzenia

//Funkcja realizuj¹ca odczyt 8-bitowej danej bez generowania ACK:
uint8_t I2C_receiveData_NACK()
{
	TWCR=_BV(TWINT) | _BV(TWEN);
	I2C_waitForComplete();
	if (TW_STATUS!=TW_MR_DATA_NACK) 
		I2C_setError(I2C_NoNACK);
	return TWDR;
}

//Je¿eli chcemy odczytaæ naraz wiele danych, ka¿dy otrzymany bajt urz¹dzenie master
//musi potwierdziæ, generuj¹c sygna³ ACK. Tylko ostatni bajt danych nale¿y odebraæ bez
//wygenerowania sygna³u ACK, co zakoñczy transmisjê.

//funkcja odczytuj¹ca
//1 bajt i generuj¹c¹ automatycznie potwierdzenie, co umo¿liwi transmisjê
//kolejnego bajta:
uint8_t I2C_receiveData_ACK()
{
	TWCR=_BV(TWEA) | _BV(TWINT) | _BV(TWEN);
	I2C_waitForComplete();
	if(TW_STATUS!=TW_MR_DATA_ACK) 
		I2C_setError(I2C_NoACK);
	return TWDR;
}
//Kilka funkcji czyni¹cych I2C w AVR bardziej ludzkim

//void I2C_sendData()


#endif /* I2C_LIB_H_ */