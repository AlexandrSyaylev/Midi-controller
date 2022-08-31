//***************************************************************************
//
//  Author(s)...: Pashgan    http://ChipEnable.Ru
//
//  Target(s)...: любой микроконтроллер
//
//  Compiler....: IAR EWA 5.11A
//
//  Description.: драйвер 4-ех кнопочного джойстика
//
//  Data........: 25.10.09
//
//***************************************************************************
#ifndef	BUTTONS_h
#define	BUTTONS_h

#include <avr/io.h>
//порт, к которому подключены кнопки
#define PORT_BUTTON 	PORTD
#define PIN_BUTTON 	PIND
#define DDRX_BUTTON 	DDRD

//номера выводов, к которым подключены кнопки
#define CH1    2
#define CH2    3
#define CH3    4
#define CH4    5

#define DOWN 	7
#define UP		6


//коды, которые будут записываться в буфер
#define KEY_NULL   0
#define KEY_CH1    1
#define KEY_CH2    2
#define KEY_CH3    3
#define KEY_CH4    4
#define KEY_UP     5
#define KEY_DOWN   6

//сколько циклов опроса кнопка должна удерживаться
#define THRESHOLD 50 //20
#define THRESHOLD2 200 //зажатая кнопка 300

#define MASK_BUTTONS 	(1<<CH1)|(1<<CH2)|(1<<CH3)|(1<<CH4)|(1<<DOWN)|(1<<UP)
#define ClearBit(reg, bit)       (reg) &= (~(1<<(bit)))
#define SetBit(reg, bit)          (reg) |= (1<<(bit))	
#define BitIsClear(reg, bit)    ((reg & (1<<(bit))) == 0)
#define BitIsSet(reg, bit)       ((reg & (1<<(bit))) != 0)

volatile unsigned char pressedKey = 0;
volatile unsigned char longpressed = 0; //флаг зажатой кнопки
unsigned char comp = 0;



/**************************************************************************
*   Function name : BUT_Init
*   Returns :       нет
*   Parameters :    нет
*   Purpose :       инициализация портов ввода/вывода
*                   вызывается обычно в начале main`a
****************************************************************************/
void BUT_Init(void);

/**************************************************************************
*   Function name : BUT_Debrief
*   Returns :       нет
*   Parameters :    нет
*   Purpose :       опрашивает кнопки. вызывается обычно из прерывания
*                   если кнопка нажата в течении 20 прерываний,
*                   ее номер записывается в буфер
****************************************************************************/
void BUT_Debrief(void);

/**************************************************************************
*   Function name : BUT_GetKey
*   Returns :       номер нажатой кнопки
*   Parameters :    нет
*   Purpose :       возвращает содержимое кнопочного буфера
*                   при этом буфер очищается
*                   вызывается обычно в main`e в цикле while
*
****************************************************************************/
unsigned char BUT_GetKey(void);


/**************************************************************************
*   Function name : BUT_Init
*   Returns :       нет
*   Parameters :    номер кнопки
*   Purpose :       записывает в кнопочный буфер значение
*                   требуется иногда для имитации нажатия кнопок
****************************************************************************/
void BUT_SetKey(unsigned char key);

//_______________________________________
void BUT_Init(void)
{
  DDRX_BUTTON &= ~(MASK_BUTTONS); 
  PORT_BUTTON |= MASK_BUTTONS;
}

//_______________________________________
void BUT_Debrief(void)
{
unsigned char key;

  //последовательный опрос выводов мк
  if (BitIsClear(PIN_BUTTON, CH1))     
    key = KEY_CH1;
  else if (BitIsClear(PIN_BUTTON, CH2))    
    key = KEY_CH2;
  else if (BitIsClear(PIN_BUTTON, CH3))
  key = KEY_CH3;
  else if (BitIsClear(PIN_BUTTON, CH4))
  key = KEY_CH4;
  else if (BitIsClear(PIN_BUTTON, UP))        
    key = KEY_UP;        
  else if (BitIsClear(PIN_BUTTON, DOWN))      
    key = KEY_DOWN;
  else {
    key = KEY_NULL;
  }

  //если во временной переменной что-то есть
   if (key)
  {
    if (comp > THRESHOLD2)
    {
      comp = THRESHOLD2 - 40;
      pressedKey = key;
	longpressed = 1;

      return;
    }
    else comp++;
    
    if (comp == THRESHOLD) 
    {
     pressedKey = key;
      return;
    }
  }
  else comp=0;
  
}

//__________________________
unsigned char BUT_GetKey(void)
{
  unsigned char key = pressedKey;
  pressedKey = KEY_NULL;
  return key;
}


//____________________________
void BUT_SetKey(unsigned char key)
{
    pressedKey = key;
}


#endif //BUTTONS_H
