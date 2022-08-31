//***************************************************************************
//
//  Author(s)...: Pashgan    http://ChipEnable.Ru
//
//  Target(s)...: ����� ���������������
//
//  Compiler....: IAR EWA 5.11A
//
//  Description.: ������� 4-�� ���������� ���������
//
//  Data........: 25.10.09
//
//***************************************************************************
#ifndef	BUTTONS_h
#define	BUTTONS_h

#include <avr/io.h>
//����, � �������� ���������� ������
#define PORT_BUTTON 	PORTD
#define PIN_BUTTON 	PIND
#define DDRX_BUTTON 	DDRD

//������ �������, � ������� ���������� ������
#define CH1    2
#define CH2    3
#define CH3    4
#define CH4    5

#define DOWN 	7
#define UP		6


//����, ������� ����� ������������ � �����
#define KEY_NULL   0
#define KEY_CH1    1
#define KEY_CH2    2
#define KEY_CH3    3
#define KEY_CH4    4
#define KEY_UP     5
#define KEY_DOWN   6

//������� ������ ������ ������ ������ ������������
#define THRESHOLD 50 //20
#define THRESHOLD2 200 //������� ������ 300

#define MASK_BUTTONS 	(1<<CH1)|(1<<CH2)|(1<<CH3)|(1<<CH4)|(1<<DOWN)|(1<<UP)
#define ClearBit(reg, bit)       (reg) &= (~(1<<(bit)))
#define SetBit(reg, bit)          (reg) |= (1<<(bit))	
#define BitIsClear(reg, bit)    ((reg & (1<<(bit))) == 0)
#define BitIsSet(reg, bit)       ((reg & (1<<(bit))) != 0)

volatile unsigned char pressedKey = 0;
volatile unsigned char longpressed = 0; //���� ������� ������
unsigned char comp = 0;



/**************************************************************************
*   Function name : BUT_Init
*   Returns :       ���
*   Parameters :    ���
*   Purpose :       ������������� ������ �����/������
*                   ���������� ������ � ������ main`a
****************************************************************************/
void BUT_Init(void);

/**************************************************************************
*   Function name : BUT_Debrief
*   Returns :       ���
*   Parameters :    ���
*   Purpose :       ���������� ������. ���������� ������ �� ����������
*                   ���� ������ ������ � ������� 20 ����������,
*                   �� ����� ������������ � �����
****************************************************************************/
void BUT_Debrief(void);

/**************************************************************************
*   Function name : BUT_GetKey
*   Returns :       ����� ������� ������
*   Parameters :    ���
*   Purpose :       ���������� ���������� ���������� ������
*                   ��� ���� ����� ���������
*                   ���������� ������ � main`e � ����� while
*
****************************************************************************/
unsigned char BUT_GetKey(void);


/**************************************************************************
*   Function name : BUT_Init
*   Returns :       ���
*   Parameters :    ����� ������
*   Purpose :       ���������� � ��������� ����� ��������
*                   ��������� ������ ��� �������� ������� ������
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

  //���������������� ����� ������� ��
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

  //���� �� ��������� ���������� ���-�� ����
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
