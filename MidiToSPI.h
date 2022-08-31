//#ifndef	MidiToSPI_h
#define	MidiToSPI_h


#include <avr/io.h>


//Для SPI
#define Spi_Port PORTB
#define LoadPin  1
#define ClockPin 0
#define DataPin  2
#define Set_Load_Hight  Spi_Port |= (1<<LoadPin)
#define Set_Load_Low	Spi_Port&=~ (1<<LoadPin)
#define Set_Clock_Hight Spi_Port |= (1<<ClockPin)
#define Set_Clock_Low	Spi_Port &=~ _BV(ClockPin)
#define Set_Data_Hight	Spi_Port |= (1<<DataPin)
#define Set_Data_Low	Spi_Port&=~ (1<<DataPin)

//для ЖК и Led
volatile unsigned char Digit1; //ПРАВЫЙ!!! (HC1) символ на 7*2
volatile unsigned char Digit2; //ЛЕВЫЙ!!! (HC2) символ на 7*2
volatile unsigned char led; // (HC3)


void Midi_to_Spi (unsigned char data);
//for test
void Hello (void);
void Clear_Dig (void);

									
unsigned char Dig1 [29] {0b00111111, //0
						 0b00001001,
						 0b01101110,
						 0b01101101,
						 0b01011001,
						 0b01110101,
						 0b01110111,
						 0b00101001,
						 0b01111111,
						 0b01111101, //9
						 0b01111011, //A 11
						 0b01010111, //b
						 0b00110110,
						 0b01001111, //d 14
						 0b01110110, //E 15
						 0b01110010, //F 16
						 0b00110111, //G 17
						 0b01011011, //H 18
						 0b01010011, //h 19
						 0b00010010, //I 20
						 0b00000010, //i_
						 0b00000001, //_i
						 0b00010110, //L 23
						 0b01000011, //n 24
						 0b01000111, //o 25
						 0b01111010, //P 26
						 0b01011101, //y 27
						 0b01000000, //- 28
						 0b01000010  //r 29
						 };
unsigned char Dig2 [29] {0b01111110, //0
						 0b01001000, //1
						 0b00111011, //2
						 0b01011011, //3
						 0b01001101,
						 0b01010111,
						 0b01110111,
						 0b01001010,
						 0b01111111,
						 0b01011111, //9
						 0b01101111, //A
						 0b01110101, //b
						 0b00110110,
						 0b01111001, //d
						 0b00110111, //E
						 0b00100111, //F
						 0b01110110, //G
						 0b01101101, //H
						 0b01100101, //h
						 0b00100100, //I
						 0b00100000, //i_
						 0b01000000, //_i
						 0b00110100, //L
						 0b01100001, //n
						 0b01110001, //o
						 0b00101111, //P
						 0b01011101, //y ?
						 0b00000001, //-
						 0b00100001  //r
						 };
unsigned char Led_Base[6] {0x00,0x01, 0x02,0x04, 0x08,0xfe};


void SPI_init (void)
{
	DDRB |= (1<<DataPin)|(1<<ClockPin)|(1<<LoadPin); //настройка портов на выход
	//PORTB|= (1<<DataPin)|(1<<ClockPin)|(1<<LoadPin);
	}


void Dec (unsigned char Data)
{
	unsigned char santi;
	unsigned char decimile;	

	if (Data >=10)
	{	
		if (Data >127 && Data <=255)
		{	Data = Data<<4;
			santi = Data>>4; //santi будет от 0 до 15
			decimile = 27; // второй разряд -
			} 
		if (Data >=120 && Data <=127)
		{	santi = Data-120;
			decimile = 22;	
			} //L
		if (Data >=110 && Data<120)
		{	santi = Data-110;
			decimile = 18; 
			} //I
		
		if (Data >= 100 && Data<110)
		{	santi = Data-100;
			decimile = 24; 
			} //o
				
		if (Data <100)
		{	decimile = Data/10;
			santi = Data - decimile*10; 
			}
	}
	else { //if (Data <10)
		decimile =0;
		santi = Data;	
			}
		
			
	Digit2 = decimile;
	Digit1 = santi;
}


void Spi_out (unsigned char Sym1, unsigned char Sym2, unsigned char Current_led)  //Spi_out (Dig1[digit1], Dig2[Digit2], Led_Base[led]);
{
	//char i =0;
	char tmp =0;
	Set_Load_Low;
	for (tmp=0;tmp<8;tmp++)
	{	if (Current_led&0x80)
		{Set_Data_Hight; }//1
		else {Set_Data_Low;}
		Current_led=Current_led<<1;
		Set_Clock_Low;
		Set_Clock_Hight;
	}
	for (tmp=0;tmp<8;tmp++)
	{	if (Sym2&0x80)
		{Set_Data_Hight; }//1
		else {Set_Data_Low;}
		Sym2=Sym2<<1;
		Set_Clock_Low;
		Set_Clock_Hight;
	}
	for (tmp=0;tmp<8;tmp++)
	{	if (Sym1&0x80)
		{Set_Data_Hight; }//1
		else {Set_Data_Low;}
		Sym1=Sym1<<1;
		Set_Clock_Low;
		Set_Clock_Hight;
	}
	Set_Load_Hight;
}
	



void Midi_to_Spi (unsigned char data)  //для уменьшения объема и повышения читаемости
{
	//if (chanal_yes ==1)
	//{data++;}
	Dec(data);
	Spi_out (~Dig1[Digit1], ~Dig2[Digit2], Led_Base[led]);
}


void Clear_Dig (void)
{
	unsigned char k =0;
	Set_Load_Low; //clear digi
	for (k=0;k<8;k++)
	{Set_Data_Low;
		Set_Clock_Low;
		Set_Clock_Hight;
	}
	for (k=0;k<8;k++)
	{Set_Data_Low;
		Set_Clock_Low;
		Set_Clock_Hight;
	}
	for (k=0;k<8;k++)
	{Set_Data_Low;
		Set_Clock_Low;
		Set_Clock_Hight;
	}
	Set_Load_Hight;
	
}


void Hello (void)
{
	
	Digit1 = 17; //H
	Digit2 = 27; //-
	Spi_out (~Dig1[Digit1], ~Dig2[Digit2], Led_Base[led]);
	_delay_ms(500);
	Digit1 = 14; //e
	Digit2 = 17; //H
	Spi_out (~Dig1[Digit1], ~Dig2[Digit2], Led_Base[led]);
	_delay_ms(500); 
	Digit1 = 22; //L
	Digit2 = 14; //E
	Spi_out (~Dig1[Digit1], ~Dig2[Digit2], Led_Base[led]);
	_delay_ms(500);
	Digit1 = 22; //l
	Digit1 = 22; //l
	Spi_out (~Dig1[Digit1], ~Dig2[Digit2], Led_Base[led]);
	_delay_ms(500);
	Digit1 = 0; //0
	Digit2 = 22; //L
	Spi_out (~Dig1[Digit1], ~Dig2[Digit2], Led_Base[led]);
	_delay_ms(500);
	Digit1 = 27; //_
	Digit2 = 0; //0
	Spi_out (~Dig1[Digit1], ~Dig2[Digit2], Led_Base[led]);
	_delay_ms(1000);
}


//_______________________USART______________________

void USART_Init ( unsigned int ubrr) //процедура по инициализации порта USART, возвращает знач ubrr, для вызова в гланом теле исп USART_Init(MYUBRR)ТОЛЬКО ДЛЯ ПЕРЕДАЧИ!!! 
{
	/*Set baud rate -для миди надо уст здесь*/
	UBRRH = (unsigned char) (ubrr>>8);
	UBRRL = (unsigned char)ubrr;
	//Enable reciever(no) and transmitter(yes)
	UCSRB = (0<<RXEN)|(1<<TXEN);
}


void USART_Transmit ( unsigned int data) 
{
	/*wait for empty transmit buffer */
	while ( !( UCSRA & (1<<UDRE))) ;
	UDR = data;
}

//______________________END___USART________________

