#ifndef I2C_LIB_H_
#define I2C_LIB_H_
//!!!
//////////////////////////////////////////////////////////////////////////
//Biblioteka wymaga prawid�owego zdefiniowania symbolu F_CPU oraz I2CBUSCLOCK
//////////////////////////////////////////////////////////////////////////
//Pami�taj o inicjalizacji I2C za pomoc� I2C_init()
//!!!!

//Rejestry
//TWBR  (ang. TW Bit Rate Register) -> oznacza on cz�stotliwo�� pracy magistrali TWI
//TWSR  (ang. Two Wire Status Register) -> okre�la konfiguracj�  

//TW_STATUS = TWSR & TW_STATUS_MASK

#include <util/twi.h>

//definicje i obs�uga b��d�w
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
TWINT - bit sygnalizuj�cy koniec czynno�ci na magistrali I2C
TWEA - bit zezwalaj�cy na generacj� sygna�u akceptacji ACK
TWSTA - generacja sekwencji START ( sprawdza zaj�to�� linii SDA ) kasowany programowo
TWSTO - generacja sekwencji STOP
TWWC - bit detekcji kolizji na magistrali
TWEN - w��czenie interfejsu I2C (aktywny stan wysoki )
TWIE - bit maski przerwa� od interfejsu I2C

TWPS0 , TWPS1 - bity sterowania preskalerem
TWS7-TWS3  - bity statusu interfejsu I2C
*/
//Wz�r na pr�dko�� transmisji w zale�nosci od TWBR to (jest on w dokumentacji oraz w internecie):
// SCL_fraquency = F_CPU / (16 +2*TWBR * 4^TWPS)

void I2C_setBusSpeed(uint16_t speed)//przyjmuje cz�stotliwo�� pracy magistrali TWI
{
	speed = (F_CPU / speed / 100 - 16) / 2; 
	//wyliczanie preskalera 
	//ale to przydatne tylko ,gdy zamierzamy zmienia� pr�dko�� magistrali w trakcie pracy
	uint8_t prescaler = 0;
	while (speed > 255) //mapowanie
	{
		++prescaler;
		speed /= 4;
	}
	TWSR = ( TWSR & ( _BV(TWPS1) | _BV(TWPS0) ) ) | prescaler;
	TWBR = speed; // ten rejestr jest odpowiedzialny za generowanie sygna�u zegarowego
}

static inline void I2C_waitForComplete()
{
	while(!(TWCR & _BV(TWINT)));//czekanie na wyzerowanie
	//flagi TWINT, sygnalizuj�cej zako�czenie operacji na magistrali TWI
}

void I2C_init()
{
	TWCR = _BV(TWEA) | _BV(TWEN) ;//w��cz interfejs I2C , ewentualnie dowal | _BV(TWSTA)
	//while( !(TWCR & _BV(TWSTO) ) );// oczekujemy na ponowne ustawienie flagi TWINT, co potwierdzi, �e u
	//k�ad sprz�towy wykona� prawid�owo sekwencj� startu transmisji.
	//bit TWEN w��cza I2C , a bit TWEA -> automatyczne generowanie bitu ACK
	I2C_setBusSpeed(I2CBUSCLOCK/100);
}

void I2C_start()
{
	//Sygna� START generowany jest automatycznie przez procesor poprzez ustawienie bitu
	//TWSTA (ang. TWI START Condition Bit).
	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
	//czekamy na nadanie sygna�u
	I2C_waitForComplete();
	//Je�li wszystko przebieg�o prawid�owo, to rejestr stanu powinien zawiera� warto��
	//r�wn� definicji TW_START
	if(TW_STATUS != TW_START)
		I2C_setError(I2C_STARTError);
}

//Funkcja ta korzysta ze sprz�towej realizacji sygna�u STOP (ustawienie bitu TWSTO). Zako�czenie
//tej operacji sygnalizowane jest wyzerowaniem bitu TWSTO
static inline void I2C_stop()
{
	TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWSTO);
}

//zadaniem poni�szej funkcji b�dzie oczekiwanie na zako�czenie nadawania bitu STOP:
static inline void I2C_waitTillStopWasSent()
{
	while (TWCR & _BV(TWSTO));
}

//funkcja wysy�aj�ca identyfikator wybranego urz�dzenia slave
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
//Funkcja ta wpisuje identyfikator wybranego urz�dzenia slave (jego adres) do rejestru
//TWDR, z kt�rego adres zostanie wys�any na magistral� TWI, a nast�pnie czeka na zako�czenie
//transmisji i sprawdza rejestr stanu, czy nie wyst�pi� b��d

//Je�eli wybieramy
//urz�dzenie w trybie do zapisu, prawid�owa operacja sygnalizowana jest warto�ci�
//rejestru stanu r�wn� TW_MT_SLA_ACK; w przypadku wyboru urz�dzenia w trybie do
//odczytu prawid�owy przebieg operacji sygnalizuje warto�� TW_MR_SLA_ACK

//Poniewa� zawsze po wys�aniu sygna�u START musimy wys�a� adres wybieranego urz�dzenia
//slave, wygodnie jest stworzy� funkcj�, kt�ra przeprowadza obie operacje r�wnocze�nie:

void I2C_sendStartAndSelect(uint8_t addr)
{
	I2C_start();
	I2C_sendAddr(addr);
}

//Poniewa� prawid�owy zapis danych
//generuje inny komunikat w rejestrze stanu ni� zapis adresu, potrzebujemy oddzielnej funkcji do zapisu danych:

void I2C_sendByte(uint8_t byte)
{
	TWDR = byte;
	TWCR = _BV(TWINT) | _BV(TWEN);
	I2C_waitForComplete();
	if(TW_STATUS!=TW_MT_DATA_ACK) 
		I2C_setError(I2C_NoACK);
}

//Operacje odczytu na magistrali I2C dziel� si� na dwa typy: odczyt z wygenerowaniem
//sygna�u potwierdzenia (ACK) oraz odczyt z wygenerowaniem sygna�u NACK.

//Kiedy odczytujemy
//tylko jeden bajt danych, urz�dzenie master nie potwierdza otrzymania danych,
//nie generuje wi�c sygna�u ACK, dzi�ki czemu urz�dzenie slave albo oczekuje na
//sygna� ko�cz�cy transmisj� (STOP), albo po prostu samo przerywa transmisj�
//W tym
//przypadku kontynuacja wymiany danych mo�liwa jest po otrzymaniu kolejnego sygna�u
//START wraz z adresem urz�dzenia

//Funkcja realizuj�ca odczyt 8-bitowej danej bez generowania ACK:
uint8_t I2C_receiveData_NACK()
{
	TWCR=_BV(TWINT) | _BV(TWEN);
	I2C_waitForComplete();
	if (TW_STATUS!=TW_MR_DATA_NACK) 
		I2C_setError(I2C_NoNACK);
	return TWDR;
}

//Je�eli chcemy odczyta� naraz wiele danych, ka�dy otrzymany bajt urz�dzenie master
//musi potwierdzi�, generuj�c sygna� ACK. Tylko ostatni bajt danych nale�y odebra� bez
//wygenerowania sygna�u ACK, co zako�czy transmisj�.

//funkcja odczytuj�ca
//1 bajt i generuj�c� automatycznie potwierdzenie, co umo�liwi transmisj�
//kolejnego bajta:
uint8_t I2C_receiveData_ACK()
{
	TWCR=_BV(TWEA) | _BV(TWINT) | _BV(TWEN);
	I2C_waitForComplete();
	if(TW_STATUS!=TW_MR_DATA_ACK) 
		I2C_setError(I2C_NoACK);
	return TWDR;
}
//Kilka funkcji czyni�cych I2C w AVR bardziej ludzkim

//void I2C_sendData()


#endif /* I2C_LIB_H_ */