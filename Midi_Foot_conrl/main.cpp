/*
 * Midi_Foot_conrl.cpp
 *
 * Created: 08.10.2016 22:21:22
 * Author : Tyassa
 */
/*
Архитектура : 4 кн работчие, UP, Down (разгон до 8)    Кн1 (--)___Кн2(EDIT)____Кн3(CANCEL)_____КН4(SAVE)
			Экран показ № пресета, при зажатии на 5 сек Edit - переход к редактору
			4х (1100+0000(Midi ch+1), Preset#)
			4x (1011+0000(Midi ch)X, CC_numberX, CC_Value_X 

Формат структуры Preset (Внутренний) { [21] {№прес,Midi_CH_Pr1, Midi_pres1, Midi_CH_Pr2, Midi_pres2, Midi_CH_Pr3, Midi_pres3, Midi_CH_Pr4, Midi_pres4, 
										Midi_CH_CC1, Midi_CC1, Midi_CC_Value1,
										Midi_CH_CC2, Midi_CC2, Midi_CC_Value2,
										Midi_CH_CC3, Midi_CC3, Midi_CC_Value3,
										Midi_CH_CC4, Midi_CC4, Midi_CC_Value4,}
										.. }
				пример Preset { {1,1,1,2,2,3,3,0,0,0,0,0,0,0,0..0} 
								{2,1,1,2,2,3,3,0,0,0,0,0,0,0,0..0}
								{3,1,1,2,2,3,3,0,0,0,0,0,0,0,0..0}
								{4,1,1,2,2,3,3,0,0,0,0,0,0,0,0..0}
				начальный адрес пресетов (Preset1)
				Перелистываются # Preset с привязкой к Led (по таблице)
				При выборе пресета обращение к EEPROM (24AT//) и запись значений к пременные Midi_CH_PR1... Midi_CC_Value из (начальный адрес= 32*№Preset)
				Посыл на Миди местных переменных
		При редактировании: вывод,ред местных переменных --- посыл на Midi --- запись в EE(24at..)
		
Посыл на SPI Led(1)---Digit2---Digit1
*/

#include <avr/io.h>
#define F_CPU 8000000UL //8MHz
#include <avr/delay.h>

#include <C:\Users\Tyassa\Documents\Atmel Studio\7.0\Midi_Foot_V3\i2c_eeprom.h>
#include <C:\Users\Tyassa\Documents\Atmel Studio\7.0\Midi_Foot_V3\buttons.h>
#include <C:\Users\Tyassa\Documents\Atmel Studio\7.0\Midi_Foot_V3\MidiToSPI.h>

#define Cycle_tic 5000


//Для EEPROM
#define adr_Pres1 32//начальный адерес НАСТРОЕК
#define adr_last_led 0
#define adr_last_preset 8
#define adr_after_reboot 16
//Для UARt
//#define MidiBautRate 31250
#define FOSC F_CPU //CLock speed
#define BAUD 31250 //как пример, из ДШ, для МИДИ порта 31250 б/сек на выходе
#define MYUBRR FOSC/16/BAUD-1


//общие
volatile unsigned char Current_state;
volatile unsigned char Current_preset;
volatile unsigned char Current_position;
volatile unsigned char Last_led;
volatile unsigned char Last_preset;
volatile unsigned char After_reboot; 
volatile unsigned char Save = 0; //флаг 1= была запись,

volatile unsigned char chanal_yes =0;
unsigned char Edit_Val =0;
//volatile unsigned char Edit_Val =0;
volatile unsigned char Knob_press=0;

//для Midi
unsigned char Midi_MES[21]; 
//volatile unsigned char Midi_MES[21]; 
/*
PresN  ;//№ Pres 0
Midi_CH_Pr1 ; 1
Midi_pres1 ; 2
Midi_CH_Pr2 ; 3 
Midi_pres2 ; 4 
Midi_CH_Pr3 ; 5 
Midi_pres3 ; 6 
Midi_CH_Pr4 ; 7 
Midi_pres4 ; 8
Midi_CH_CC1 ; 9
Midi_CC1 ; 10
Midi_CC_Value1;11
Midi_CH_CC2 ; 12 
Midi_CC2 ; 13 
Midi_CC_Value2;14
Midi_CH_CC3 ; 15 
Midi_CC3 ; 16 
Midi_CC_Value3; 17 
Midi_CH_CC4 ; 18 
Midi_CC4 ; 19 
Midi_CC_Value4; 20
*/

void Midi_Transm (void);

//Edit
void Change_pos (unsigned char position);
//char Encrease_Val (unsigned char Volume);
//char Decrease_Val (unsigned char Volume);
void Encrease_Val (void);
void Decrease_Val (void);
void Edit (void);
void Edit_knob (void);

//eeprom+midi 
void Write_after_reboot (void);
void Recieve_last_used (void);
void Recieve_Midi_from_EE (void);
void Write_Last_Used (void);
void Write_Midi_EE (void);



int main(void)
{
	char no_changes = 0; //флаг 1 =были изменения
	USART_Init(MYUBRR);
	SPI_init();
	BUT_Init();
	
	//________________для EEPROM___________________
	eeInit();
	//Write_after_reboot();
	_delay_ms(10);
	Recieve_last_used();
	Midi_to_Spi(Current_preset);
	_delay_ms(1500);
	Recieve_Midi_from_EE();
	
	//_____________________________________________
	
	Hello();
	//Midi_Transm();
	Midi_to_Spi(Midi_MES[0]);
	
	
    while (1) 
    {			
			longpressed=0;
		BUT_Debrief();
		Knob_press = BUT_GetKey();
		
		Current_position =led;
		
		if (Knob_press == 1) //ch1
		{
			led = 1;
			Change_pos(led); //определение номера текущ пресета и его смена на 1 в банке, сравнение current led & led, если больше led, то прибавить к current_preset+(led-current_led), если меньше current_pres-(current_led-led)
			no_changes = 1;
			Save =0;
		}
		
		if (Knob_press == 2) //кн 2 ch2/edti
		{
			led = 2;			
			if (longpressed>0)
			{Edit();	}
			else {Change_pos(led); //определение номера текущ пресета и его смена на 1 в банке
				no_changes = 1;
				Save =0;
				_delay_ms(350);	
				
				}
		}
		
		if (Knob_press == 3) //кн3 сh3/ cancel
		{
			led = 3;	
			Change_pos(led); //определение номера текущ пресета и его смена на 1 в банке
			no_changes = 1;
			Save =0;
			_delay_ms(350);			
		}
		
		if (Knob_press == 4) //ch4 /save
		{
			led = 4;
			Change_pos(led); //определение номера текущ пресета и его смена на 1 в банке
			no_changes = 1;
			Save =0;
			_delay_ms(350);
		}
		if (Knob_press == 5) //up
		{
			Current_preset = Current_preset+4;
			if (Current_preset >40) //д.б. больше 
			{Current_preset =led;} //БЫЛО 4-LED
			Change_pos(led);
			no_changes = 1;
			Save =0;
			_delay_ms(500);
		}
		if (Knob_press == 6) //down
		{
			if (Current_preset <=4) //д.б. меньше ИЛИ РАВНО
			{Current_preset =Current_preset+40; //должно быть кратно 4
			}
			Current_preset = Current_preset-4;
			Change_pos(led);
			no_changes = 1;
			Save =0;
			_delay_ms(500);
		}
		
	}
	
}



void Recieve_last_used (void) 
{	
	Last_led = eeReadByte(adr_last_led);
	
	Last_preset = eeReadByte(adr_last_preset);
		
	if (Last_preset == 0) //что бы писало только с 32 байта
	{Last_preset =1;	}
	Current_preset = Last_preset;
	led = Last_led;
}
	
void Write_Last_Used (void)
{
	eeWriteByte(adr_last_led, Last_led);
	
	eeWriteByte(adr_last_preset, Last_preset);
	Save = 1;
	
	//для отладки "сохранено"
	Digit1 = 14; //в случае записи выводит SE на 2 сек
	Digit2 = 5;
	Spi_out (~Dig1[Digit1], ~Dig2[Digit2], Led_Base[led]);
	_delay_ms(2000);
}

void Recieve_Midi_from_EE (void)
{
	uint16_t address = 32; //НАЧАЛЬНЫЙ АДРЕС, ВМЕСТО adr_Pres1
	
	address = 32*Current_preset;	//номер пресета
	unsigned char i=0;
	while (i!=20)
	{
		Midi_MES[i]= eeReadByte(address);
		address++;
		i++;
	}
	
	Current_preset = Midi_MES[0] ; // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	
}

void Write_Midi_EE (void)
{
	uint16_t address = Current_preset*32; //НАЧАЛЬНЫЙ АДРЕС*current
	unsigned char i=0;
	while (i!=20)
	{
		//For test
		if (i==1 || i==3 ||i==5 ||i==7 || i==9 || i==12 || i==15 || i==18)
		{//Midi_MES[i]=1; //chanal
			eeWriteByte(address, 1);
		}
		else {eeWriteByte(address, Midi_MES[i]);}
		_delay_ms(5);
		address++;
		i++;
	}
	
}

void Midi_Transm (void)
{
	//USART_Transmit(PresN)  ;//№ Pres

	unsigned char i=1; //пропускаем presN
	while (i!=21)
	{
		if (i==1 || i==3 || i==5 || i==7  ) //chanal Pres
		{USART_Transmit(Midi_MES[i]|=0xC0);}
		else if (i==9 || i==12|| i==15 || i==18) //Canal CC
		{USART_Transmit(Midi_MES[i]|=0xb0);}
		else
		{USART_Transmit(Midi_MES[i]);}
		i++;
	}
}
void Write_after_reboot (void)
{
	uint8_t byte ; 
	uint16_t address ; // проверка на пустоту
	After_reboot = eeReadByte(adr_after_reboot);
	byte =1;
	if (After_reboot == 0)
	{	address = adr_Pres1;
		char i =0;
		for (i=0;i<80;i++) //записать номера, с шагом 30, все остальное по 0
		{
			address = adr_Pres1+32*i;
			eeWriteByte(address, byte);
		}
	After_reboot = 1;
	eeWriteByte(adr_after_reboot,After_reboot);
	//для отладки сохранено
	Digit1 = 11; //в случае записи выводит db на 2 сек
	Digit2 = 13;
	Spi_out (~Dig1[Digit1], ~Dig2[Digit2], Led_Base[led]);
	_delay_ms(2000);
	}
		 
}


void Change_pos (unsigned char position)
{
	//...определение позиции смена на led+1, изменение Current_preset?
	//сравнение current_posit & led, если больше led, то прибавить к current_preset+(led-current_led), если меньше current_pres-(current_led-led)
	//if (led!=Current_preset)
	if (led!=Current_position)
	{
		if (Current_position>led)
		{	Current_preset=Current_preset-(Current_position-led);	}
		if (led>Current_position)
		{	Current_preset=Current_preset+(led-Current_position);	}
	}
	Recieve_Midi_from_EE();
	Midi_to_Spi(Current_preset);
	Midi_Transm();
	//_delay_ms(200);
	
}


void Encrease_Val (void)
{
	Edit_Val++;
	//if (chanal_yes== 1)
	//	{if (Edit_Val>=16)
	//		{Edit_Val=0;	}
	//	}

	//else {
		if (Edit_Val>127)
		{Edit_Val=0;	}
	//	}

}
void Decrease_Val (void)
{
	//if (chanal_yes ==1)
	//{if (Edit_Val==0)
	//	{Edit_Val=16;	}
	//}
	//else {
		if (Edit_Val==0)
		{Edit_Val=128;}
	//}
	
	Edit_Val--;
	
}  
void Edit (void) //В ЛОБ, МАКСИМАЛЬНО ПРЯМО, ПОСЛЕДОВАТЕЛЬНО
{
	char g=0;
	unsigned char i=0;
	led = 6;
	Digit1 = 13; //d
	Digit2 = 14; //E
	Spi_out (~Dig1[Digit1], ~Dig2[Digit2], Led_Base[led]);
	_delay_ms(1500);
	
	while (i!=21)
	{
		if (i==0)
		{g=0;
			}
		if (i==1 || i==3 ||i==5 ||i==7 || i==9 || i==12 || i==15 || i==18)
		{g=1; //chanal
			}
		if (i==2 ||i==4 ||i==6 ||i==8)
		{g=2; //Preset
			}
		if (i==10 ||i==13 ||i==16 ||i==19)
		{g=3; // CC
			}
		if (i==11 ||i==14 ||i==17 ||i==20)
		{g=4;
			}
		
			switch (g)
			{
				
		
				case 0: 
				{
					//	Digit1 = 28; //r
					//	Digit2 = 25; //P
					//	Spi_out (~Dig1[Digit1], ~Dig2[Digit2], Led_Base[led]);
					//	_delay_ms(1000);
					Midi_to_Spi(Midi_MES[i]);
					_delay_ms(1500);	
					i++;		
					break;
					}
		
				case 1: 
				{
					Digit1 = 18; //h
					Digit2 = 12; //C
					Spi_out (~Dig1[Digit1], ~Dig2[Digit2], Led_Base[led]);
					_delay_ms(750);
					//chanal_yes = 1;
					Midi_to_Spi(i); // for test
					_delay_ms(1500); // for test
					Midi_to_Spi(Midi_MES[i]);
					_delay_ms(1500);
					break;
					}
		
				case 2: 
				{
					Digit1 = 24; //o
					Digit2 = 23; //h
					Spi_out (~Dig1[Digit1], ~Dig2[Digit2], Led_Base[led]);
					_delay_ms(750);
					//chanal_yes = 0;
					break;
					}
		
				case 3: 
				{
					Digit1 = 12; //o CC
					Digit2 = 12; //h
					Spi_out (~Dig1[Digit1], ~Dig2[Digit2], Led_Base[led]);
					_delay_ms(750);
					//chanal_yes = 0;
					break;
					}
		
				case 4: 
				{
						Digit1 = 24; //o Value
						Digit2 = 23; //h
						Spi_out (~Dig1[Digit1], ~Dig2[Digit2], Led_Base[led]);
						_delay_ms(750);
						//chanal_yes = 0;			
						break;
						}
			}
			
		
			
	
		
		Edit_Val = Midi_MES[i];
		Edit_knob();
		Midi_MES[i] = Edit_Val;
		i++;
		
	}
			
	//	Midi_Transm();
		Write_Midi_EE();
		//для отладки "сохранено"
		Digit1 = 14; //в случае записи выводит SE на 2 сек
		Digit2 = 5;
		Spi_out (~Dig1[Digit1], ~Dig2[Digit2], Led_Base[led]);
		_delay_ms(2000);
		Current_preset=Midi_MES[0];
		Midi_to_Spi(Midi_MES[0]);
		
}

void Edit_knob (void)
{
	Midi_to_Spi(Edit_Val);
	_delay_ms(1500);
	
	while (1)
	{	BUT_Debrief();
		Knob_press = BUT_GetKey();
		
		switch (Knob_press)
		{
			
		
		case 1: //nop
			{
			break;
			}
		case 2: //edit/next
			{
			break;
			}
		case 3: //cancel
			{
				Knob_press = 0; //!!!!!!! for reset value knob 3
				//pressedKey = 0; // or this
				//or this BUT_SetKey(0);
			return;
			}
		case 4: //send/save
			{
			//Write_Midi_EE();
			//Midi_Transm();
			break;
			}
		case 5: //up 
			{
			Encrease_Val();
			Midi_to_Spi(Edit_Val);
			_delay_ms(250);
			break;
			}
		case 6: //down 
			{
			Decrease_Val();
			Midi_to_Spi(Edit_Val);
			_delay_ms(250);
			break;
			}
		}
	}
	
}