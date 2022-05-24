#include "tm4c123gh6pm.h" 
#include "LCD.h"
#include "systick.h"
#include "ports.h"
#include "keypad.h"

void LCD_Command(char command)
{
	GPIO_PORTA_DATA_R &= ~0x60; //RS =0, E=0
	GPIO_PORTB_DATA_R = command;
	GPIO_PORTA_DATA_R |= 0x40; //E=1 to secure command
	if (command < 4) // they take longer
        Systick_Wait_1ms(10);
  else
        Systick_Wait_1us(2000);
	GPIO_PORTA_DATA_R &= ~0x60;
}

void LED_Blink(unsigned short t)						// LEDs blinking function
{
	char i;
	for(i = 0; i < 2*t; i++)
	{
		GPIO_PORTF_DATA_R ^= 0X0E;
		GPIO_PORTE_DATA_R ^= 0X01;
		Systick_Wait_1ms(500);
	}
}

void LCD_Init(void)
{		
	 LCD_Command(SetTo8Bits);				// using 8-bits configuration
	 LCD_Command(EntryMode);				
	 LCD_Command(Cursor_Off);				// turning off cursor blinking
	 Systick_Wait_1ms(2);
}

void LCD_Display(char data) 											//for writing characters
{ 
	GPIO_PORTA_DATA_R &= ~0x60;			//RS=0 , E=0
	GPIO_PORTA_DATA_R |= 0x20;		 	//RS=1 , E=0
	GPIO_PORTB_DATA_R = data;				// read data
	GPIO_PORTA_DATA_R |= 0x40;		 	//E=1
	Systick_Wait_1us(10);						//delay for 10 micro seconds
	GPIO_PORTA_DATA_R &= ~0x60;			
	GPIO_PORTB_DATA_R = 0;					
}

void LCD_Write_Data(char* data, unsigned short size) 				//for writing strings on LCD
{
	unsigned long i = 0;
	LCD_Command(Clear_Display);
	LCD_Command(Return_Home);
	for (i = 0; i < size; i++){					//for loop for displaying charecters one after the other
		LCD_Display(*(data+i));
		Systick_Wait_1us(10);
	}
}


char* Timer (char kilo, char time)
{
	unsigned short time_in_seconds;
	char kilo_in_numbers,time_in_char[5];
	kilo_in_numbers = kilo - '0';
	time_in_seconds = kilo_in_numbers * time;
	time_in_char[0] = (time_in_seconds/600)+'0';
	time_in_seconds = time_in_seconds % 600 ;
	time_in_char[1] = (time_in_seconds/60)+'0';
	time_in_seconds = time_in_seconds % 60 ;
	time_in_char[2] = ':';
	time_in_char[3] = (time_in_seconds/10)+'0';
	time_in_seconds = time_in_seconds % 10;
	time_in_char[4] = time_in_seconds+'0';
	return time_in_char;
}


char* Enter_Time()
{
	char tmp,time[5];
SW1:
	time [0] = '0';
	time [1] = '0';
	time [2] = ':';
	time [3] = '0';
	time [4] = '0';
	
	LCD_Write_Data(time, 5);
	tmp = Read_Keypad();
	Systick_Wait_1ms(200);
	if (tmp == '!') goto SW1;
	if (tmp == '@') return time;
	time[4] = tmp;
	LCD_Write_Data(time, 5);
	Systick_Wait_1ms(100);
	
	tmp = Read_Keypad();
	Systick_Wait_1ms(200);
	if (tmp == '!') goto SW1;
	if (tmp == '@') return time;
	time[3] = time[4];
	time[4] = tmp;
	LCD_Write_Data(time, 5);
	Systick_Wait_1ms(100);

	tmp = Read_Keypad();
	Systick_Wait_1ms(200);
	if (tmp == '!') goto SW1;
	if (tmp == '@') return time;
	time[1] = time[3];
	time[3] = time[4];
	time[4] = tmp;
	LCD_Write_Data(time, 5);
	Systick_Wait_1ms(100);

	tmp = Read_Keypad();
	Systick_Wait_1ms(200);
	if (tmp == '!') goto SW1;
	if (tmp == '@') return time;
	time[0] = time[1];
	time[1] = time[3];
	time[3] = time[4];
	time[4] = tmp;
	LCD_Write_Data(time, 5);
	Systick_Wait_1ms(100);
	tmp = Read_Keypad();
	if (tmp == '!') goto SW1;
	if (tmp == '@') return time;
	while(SW2()== 1){};
	return time;
}
