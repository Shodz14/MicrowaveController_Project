#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <tm4c123gh6pm.h>

void SYSTICK_Init()
{
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_CURRENT_R = 0;
	NVIC_ST_CTRL_R = 0X05;
}

void System_Init()
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

bool check_door()
{
	return ((SW3 == 0) ? 0:1);
}

void LCD_Countdown(int time)
{
	int i;
	for (i = time; i >= 0; i--)
	{
  while (check_door()){};
	Systick_wait_1s(1);
	// wait for time
	Display_time(i);
	// Countdown the time on LCD for time
	}
}
// time is in seconds

void case_bc(char* str,int time)
{
	Loop:
			LCD_Display(str);
		  char kilo = read_keypad();
	    kilo = atoi(kilo);
		  if(kilo < 1 || kilo > 9 || kilo%1 != 0)
			{
				LCD_Display("Err");
				Systick_wait_1s(2);
				goto Loop;
			}
			
		  if(check_door() == 0) //door closed = 0
			{
			  clear_LCD(); //Clear please close the door if closed before 5 sec
				LCD_Display(kilo);
				Systick_wait_1s(2);
		    LCD_Countdown(kilo * time * 60);
		// Countdown 60 sec 
		    clear_LCD();
		    LED_Blink(3);
		    Buzzer(3);
			}
			else
			{
				LCD_Display("Please Close Door"); //Display for 3 seconds
				Systic_wait_1s(3);
				goto Loop;
			}
}

int main(void)
{
	char keypad = read_keypad();
	// read_keypad deals with UART
	switch(keypad)
	{
		case 'A':
			POPCORN:
			LCD_Display("PopCorn");
		  Systic_wait_1s(3);
		// Display "PopCorn" for 1 sec for words
			if(check_door() == 0) //door closed = 0
			{
			clear_LCD(); //Clear please close the door if closed before 5 sec
		  LCD_Countdown(60);
		// Countdown 60 sec 
		  clear_LCD();
		  LED_Blink(3);
		  Buzzer(3);
			}
			else
			{
				LCD_Display("Please Close Door"); //Display for 5 seconds
				Systic_wait_1s(3);
				goto POPCORN;
			}
		break;
		case 'B':
			case_bc("Beef Weight ?",0.5);
		break;
		case 'C':
		  	case_bc("Chicken Weight ?",0.2);
		break;
		case 'D':
			LCD_Display("Cooking Time ?");
		/*int num_2 = 0;
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
	}
}
