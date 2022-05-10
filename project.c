#include "stdbool.h"
#include <stdlib.h>
#include "string.h"
#include "tm4c123gh6pm.h"

void SYSTICK_Init()
{
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_CURRENT_R = 0;
	NVIC_ST_CTRL_R = 0X05;
}

void PortF_Init()
{
	SYSCTL_RCGCGPIO_R |= 0X20;
	while(!(SYSCTL_PRGPIO_R & 0X20)){};
	GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
	GPIO_PORTF_CR_R = 0X0E;

	GPIO_PORTF_DEN_R |= 0X0E;
	GPIO_PORTF_AFSEL_R &= ~0X0E;
	GPIO_PORTF_PCTL_R &= ~0X0FFF0;
	GPIO_PORTF_AMSEL_R &= ~0X0E;
	GPIO_PORTF_DIR_R |= 0X0E;
	GPIO_PORTF_DATA_R |= 0X0E;
}



void PortE_Init()
{
	SYSCTL_RCGCGPIO_R |= 0X10;
	while(!(SYSCTL_PRGPIO_R & 0X10)){};
	GPIO_PORTE_LOCK_R = GPIO_LOCK_KEY;
	GPIO_PORTE_CR_R = 0X30;

	GPIO_PORTE_DEN_R |= 0X30;
	GPIO_PORTE_AFSEL_R &= ~0X30;
	GPIO_PORTE_PCTL_R &= ~0XFF0000;
	GPIO_PORTE_AMSEL_R &= ~0X30;
	GPIO_PORTE_DIR_R |= 0X20;
	GPIO_PORTE_DATA_R = 0X00;
}

void PortB_Init()
{
	SYSCTL_RCGCGPIO_R |= 0X02;
	while(!(SYSCTL_PRGPIO_R & 0X02)){};
	GPIO_PORTB_LOCK_R = GPIO_LOCK_KEY;
	GPIO_PORTB_CR_R = 0XFF;

	GPIO_PORTB_DEN_R |= 0XFF;
	GPIO_PORTB_AFSEL_R &= ~0XFF;
	GPIO_PORTB_PCTL_R &= ~0XFFFFFFFF;
	GPIO_PORTB_AMSEL_R &= ~0XFF;
	GPIO_PORTB_DIR_R |= 0XFF;
	GPIO_PORTB_DATA_R = 0X00;
}

void PortA_Init()
{
	SYSCTL_RCGCGPIO_R |= 0X01;
	while(!(SYSCTL_PRGPIO_R & 0X01)){};
	GPIO_PORTB_LOCK_R = GPIO_LOCK_KEY;
	GPIO_PORTB_CR_R = 0XE0;

	GPIO_PORTB_DEN_R |= 0XE0;
	GPIO_PORTB_AFSEL_R &= ~0XE0;
	GPIO_PORTB_PCTL_R &= ~0XFFF00000;
	GPIO_PORTB_AMSEL_R &= ~0XE0;
	GPIO_PORTB_DIR_R |= 0XE0;
	GPIO_PORTB_DATA_R = 0X00;
}


void Systick_Wait(unsigned long delay)
{
	NVIC_ST_CURRENT_R = 0;
	NVIC_ST_RELOAD_R = delay - 1;
	while(!(NVIC_ST_CTRL_R & 0X10000)){};
}

void Systick_Wait_1s(unsigned long delay)
{
	unsigned long i;
	for(i = 0; i < delay; i++)
	{
		Systick_Wait(16000000);
	}
}


void delay_milli(int n)
	{
	int i,j;
	for(i=0;i<n;i++)
	for(j=0;j<3180;j++){}
}
	
void delay_micro(int n)
{
	int i,j;
	for(i=0;i<n;i++)
	for(j=0;j<3;j++){}
}
	
void LCD_Command(unsigned char command)
{
	GPIO_PORTA_DATA_R = 0x00; //RS =0, E=0, RW=0
	GPIO_PORTB_DATA_R =command;
	GPIO_PORTA_DATA_R =0x80; //E=1 to secure command
	delay_micro(0);
	GPIO_PORTA_DATA_R =0x00;
	if(command <4) delay_milli(2); else delay_micro(37); 
}

void LED_Blink( unsigned short t)
{
	unsigned long i;
	for(i = 0; i < 2*t; i++)
	{
		GPIO_PORTF_DATA_R ^= 0X0E;
		Systick_Wait_1s(1);
	}
}

void Buzzer(unsigned short time)
{
	GPIO_PORTE_DATA_R |= 0x20;
	Systick_Wait_1s(time);
	GPIO_PORTE_DATA_R &= ~0x20;	//clearing
}


char SW3()
{
	char SW_3;
	SW_3 = (GPIO_PORTE_DATA_R & 0X10); //reading SW3 (door position)
	return SW_3;
}
	

bool check_door()	// bool check_door()
{
	bool checkDoor = true;
	checkDoor = (SW3() == 0) ? 0 : 1; //(SW3() == 0) ? 0:1
	return checkDoor;
}

void LCD_Countdown(int time)
{
	int i;
	for (i = time; i >= 0; i--)
	{
  while (check_door()){};
	Systick_Wait_1s(1);
	// wait for time
	Display_time(i);
	// Countdown the time on LCD for time
	}
}
// time is in seconds

void case_bc(char* str,unsigned short time)
{
	Loop:
			LCD_Display(str);
		  char kilo = read_keypad();
	    kilo = atoi(kilo);
		  if(kilo < 1 || kilo > 9 || kilo%1 != 0)
			{
				LCD_Display("Err");
				Systick_Wait_1s(2);
				goto Loop;
			}

		  if(check_door() == 0) //door closed = 0
			{
			  LCD_Command(0x01); //Clear please close the door if closed before 5 sec
				LCD_Display(kilo);
				Systick_Wait_1s(2);
		    LCD_Countdown(kilo * time);
		// Countdown 60 sec
		    LCD_Command(0x01);
		    LED_Blink(3);
		    Buzzer(3);
			}
			else
			{
				LCD_Display("Please Close Door"); //Display for 3 seconds
				Systick_Wait_1s(3);
				goto Loop;
			}
}

int main()
{
    SYSTICK_Init();
    PortE_Init();
	  PortF_Init();
	  PortB_Init();
	char keypad = read_keypad();
	// read_keypad deals with UART
	switch(keypad){
		case 'A':
			POPCORN:
			LCD_Display("PopCorn");
		  Systick_Wait_1s(3);
		// Display "PopCorn" for 1 sec for words
			if(check_door() == 0) //door closed = 0
			{
			LCD_Command(0x01); //Clear please close the door if closed before 5 sec
		  LCD_Countdown(60);
		// Countdown 60 sec
		  LCD_Command(0x01);
		  LED_Blink(3);
		  Buzzer(3);
			}
			else
			{
				LCD_Display("Please Close Door"); //Display for 5 seconds
				Systick_Wait_1s(3);
				goto POPCORN;
			}
		break;
		case 'B':
			case_bc("Beef Weight ?",30);
		break;
		case 'C':
		  case_bc("Chicken Weight ?",12);
		break;
		case 'D':
			LCD_Display("Cooking Time ?");
	}
}	/*int num_2 = 0;
		for(short i = 0; i < 3; i++)
		{
			if(SW1 == 0)
			{
				Clear_LCD();
				 i = -1;
			}
			else if(SW2 ==0)
			{
				LCD_Countdown(num_2);
			}
			else
			{
			char number = read_keypad();
			number = atoi(number);
			if(i < 2)
			{
				num_2 += num*pow(10,i);
			}
			else if(i ==3)
			{

			}
			Display_time (num_2);
		}
		}*/

