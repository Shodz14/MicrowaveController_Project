#include "tm4c123gh6pm.h" 
#include "LCD.h"
#include "systick.h"
#include "ports.h"
#include "keypad.h"

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

void LCD_Init(void)
{		
	 LCD_Command(SetTo8Bits);				// using 8-bits configuration
	 LCD_Command(EntryMode);				
	 LCD_Command(Cursor_Off);				// turning off cursor blinking
	 Systick_Wait_1ms(2);
}

void LCD_Display(char data) 				//for writing characters
{ 
	GPIO_PORTA_DATA_R &= ~0x60;				//RS=0 , E=0
	GPIO_PORTA_DATA_R |= 0x20;		 		//RS=1 , E=0
	GPIO_PORTB_DATA_R = data;				// read data
	GPIO_PORTA_DATA_R |= 0x40;			 	//E=1
	Systick_Wait_1us(10);					//delay for 10 micro seconds
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

void LCD_Countdown(char* time)   // 15:45   0=>9   48=>57        : => 58
{
	char i,j,temp[5];
	LOOP:
	if((time[3]>'5') || ( (time[0]=='0')&&(time[1]=='0')&&(time[3]=='0')&&(time[4]=='0') ))	//to avoid numbers more than 5 in ten seconds or "00:00"
	{
		LCD_Write_Data("Err",3);		
		Systick_Wait_1ms(2000);
		time = Enter_Time();
		goto LOOP;
	}
	else if(time[0] >= '3')					//restricting maximum time to 30:00
	{
		temp [0] = '3';
		temp [1] = '0';
		temp [2] = ':';
		temp [3] = '0';
		temp [4] = '0';
	}
	else
	{
		for(i = 0; i<5;i++)
		{
			temp[i]=time[i];
		}
	}
	GPIO_PORTF_DATA_R |= 0X0E;
	while(SW3() == 0){}
	for(;temp[0] >= '0'; temp[0]--)
	{
		for(;temp[1] >= '0'; temp[1]--)
		{
			for(;temp[3] >= '0'; temp[3]--)
			{
				for(;temp[4] >= '0';temp[4]--)
				{
					LCD_Write_Data(temp,5);
					for (i = 0;i < 10;i++)
					{
						while(SW3() == 0)			//check for door
						{
							for(j = 0; j<10; j++)
							{
								if(j == 0 || j == 5)
								{
									GPIO_PORTF_DATA_R ^= 0X0E; 				//LED BLink
								}
								Systick_Wait_1ms(100);
							}
						}
						if(SW1() ==  0)
						{
							Systick_Wait_1ms(300);				// to run the previous condition correctly
							while(SW2() == 1 || SW1() == 1)		  //as long as sw1 and sw2 are not pressed ->> while loop
							{
								for(j = 0; j<100; j++)
								{
									if(j == 0 || j == 50)
									{
										GPIO_PORTF_DATA_R ^= 0X0E; 					//LED BLink
									}
									Systick_Wait_1ms(10);
									if(SW2() == 0 || SW1() == 0) break;
								}
								if(SW2() == 0 || SW1() == 0) break;
							}
							if(SW1() == 0)
							{
								GPIO_PORTF_DATA_R &= ~0X0E;
								LCD_Command(Clear_Display);
								return;
							}
						}
						GPIO_PORTF_DATA_R |= 0X0E;
						Systick_Wait_1ms(100);
					}
				}
				temp[4]= '9';
			}
			temp[3]= '5';
		}
		temp[1]= '9';
	}
	GPIO_PORTF_DATA_R &= ~0X0E;
	LED_Blink(3);
}


char* Timer (char kilo, char time)					//Calculate time in 12:45 format from no of seconds
{
	unsigned short time_in_seconds;
	char kilo_in_numbers,time_in_char[5];
	kilo_in_numbers = kilo - '0';					//change kilo to numbers instead of ASCII
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
	if (tmp == '!') goto SW1;			//if SW1 is pressed clear time
	if (tmp == '@') return time;		//if SW2 is pressed start countdown
	time[4] = tmp;						//if 1 is entered --> time is 00:01
	LCD_Write_Data(time, 5);
	Systick_Wait_1ms(100);
	
	tmp = Read_Keypad();
	Systick_Wait_1ms(200);
	if (tmp == '!') goto SW1;			//if SW1 is pressed clear time
	if (tmp == '@') return time;		//if SW2 is pressed start countdown
	time[3] = time[4];					//time is 00:10
	time[4] = tmp;						//if 2 is entered --> time is 00:12
	LCD_Write_Data(time, 5);
	Systick_Wait_1ms(100);

	tmp = Read_Keypad();
	Systick_Wait_1ms(200);
	if (tmp == '!') goto SW1;			//if SW1 is pressed clear time
	if (tmp == '@') return time;		//if SW2 is pressed start countdown
	time[1] = time[3];					//time is 01:02
	time[3] = time[4];					//time is 01:20
	time[4] = tmp;						//if 4 is entered --> time is 01:24
	LCD_Write_Data(time, 5);
	Systick_Wait_1ms(100);

	tmp = Read_Keypad();
	Systick_Wait_1ms(200);
	if (tmp == '!') goto SW1;			//if SW1 is pressed clear time
	if (tmp == '@') return time;		//if SW2 is pressed start countdown
	time[0] = time[1];					//time is 10:20
	time[1] = time[3];					//time is 12:04
	time[3] = time[4];					//time is 12:40
	time[4] = tmp;						//if 5 is entered --> time is 12:45
	LCD_Write_Data(time, 5);
	Systick_Wait_1ms(100);
	tmp = Read_Keypad();
	if (tmp == '!') goto SW1;			//if SW1 is pressed clear time
	if (tmp == '@') return time;		//if SW2 is pressed start countdown
	while(SW2()== 1){};
	return time;
}
