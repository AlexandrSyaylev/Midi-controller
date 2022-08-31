# Midi controller v3 

Fully Programable simple to use multi - channel midi controller, support Midi connection thru 5-din cable with 31250 baudRate (standart midi Inteface)

## Each one message for each preset includes: 
4 independed Programm change commands :
  1. Midi Chanal number (1-16) and
  2. Number of Programm (0-127)
  
4 independed Control change commands :
  1. Midi Chanal number (1-16), 
  1. Control Number (0-127) and 
  1. value of current control (0-127)



## Feature
Based on Atmega8, 2 digit lcd connected thru shift register HC595
 
4 Led indicators connected thru one shift register HC595, connected to lcd shift register by cascade model 

4 Knobs for select preset on current bank and edit mode: 
 1. Knob 1 - preset 1 in current bank 
 1. Knob 2 - preset 2 in current bank, long press (5 sec) - enter to Edit mode, edit mode - next value
 1. Knob 3 - preset 3 in current bank, long press - Cancel changes and exit from Edit mode
 1. Knob 4 - preset 4 in current bank, long press - Save changes and exit from Edit mode
  
2 Knobs for navigate -Up and Down - switch current bank and insrese/decrease edit value 

Bank damp store on independed EEPROM 24At XX (code for 24At08) connected by I2c interface.

AutoSave last used present

Boot from last used state (include correct current position active knob and led indication)  

Interlnal delay for knob reliably protect from twice press on knobs.   

## hc595 configuration outout 
SPI Led(1)---Digit2---Digit1 
For correсt work Midi interface requered 8MHz crystall oscillator blocking two 20-22pF capacitors.

Write and debug in Atmel Studio 7.0, 7.2
