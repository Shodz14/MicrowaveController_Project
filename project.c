#include "stdbool.h"
#include "stdlib.h"
#include "string.h"
#include "tm4c123gh6pm.h"

char keypad[4][4] = 
{
	{ '1', '2', '3', 'A'},
	{ '4', '5', '6', 'B'},
	{ '7', '8', '9', 'C'},
	{ '*', '0', '#', 'D'},
};

void PortD_Init(void)
	{
	 SYSCTL_RCGCGPIO_R |= 0X08; // Enable clock to PORTD
	 while(!(SYSCTL_PRGPIO_R & 0X08)){};
	 GPIO_PORTD_LOCK_R = GPIO_LOCK_KEY;
	
	 GPIO_PORTD_CR_R = 0XFF;  // Allow settings for all pins of PORTD
	 GPIO_PORTD_DEN_R |= 0XFF; // Set PORTD as digital pins
	 GPIO_PORTD_DIR_R |= 0X0F;  // Set PORTD as digital input
	 GPIO_PORTD_DATA_R = 0X0F;
	 GPIO_PORTD_AFSEL_R = 0X00;
	 GPIO_PORTD_PCTL_R = 0X00000000;
	 GPIO_PORTD_AMSEL_R = 0X00;
	 GPIO_PORTD_PUR_R = 0XF0; 
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
	GPIO_PORTF_DATA_R =| 0X00;
}



void PortE_Init()
{
	SYSCTL_RCGCGPIO_R |= 0X10;
	while((SYSCTL_PRGPIO_R & 0X10)==0){};
	GPIO_PORTE_LOCK_R = GPIO_LOCK_KEY;
	GPIO_PORTE_CR_R = 0X3F;

	GPIO_PORTE_DEN_R |= 0X3F;
	GPIO_PORTE_AFSEL_R &= ~0X3F;
	GPIO_PORTE_PCTL_R &= ~0XFFFFFF;
	GPIO_PORTE_AMSEL_R &= ~0X3F;
	GPIO_PORTE_DIR_R = 0X20;
	GPIO_PORTE_DATA_R &= 0X00;
	GPIO_PORTE_PUR_R = 0X0F;
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

void SYSTICK_Init()
{
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_CURRENT_R = 0;
	NVIC_ST_CTRL_R = 0X05;
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

void LCD_Init(void)
{
	/*SYSCTL_RCGCGPIO_R |= 0x01; //PORTA clock
	SYSCTL_RCGCGPIO_R |= 0x02;
	GPIO_PORTA_DIR_R |=0xE0; //PORTA controls RS,E and R/W
	GPIO_PORTA_DEN_R |=0xE0;
	GPIO_PORTB_DIR_R |=0xFF; //PORTB D0-D7
	GPIO_PORTB_DEN_R |=0xFF; //PORTB D0-D7  */
	LCD_Command(0x38); //8-bits,2 display lines, 5x7 font
	//LCD_Command(0x06); //increments automatically
	LCD_Command(0x0F); //Turn on display
	LCD_Command(0x01); //clear display
}


void LCD_Display(unsigned char data) 
{ 
	GPIO_PORTA_DATA_R =0x20; //RS=1, E=0,RW=0
	GPIO_PORTB_DATA_R =data;
	GPIO_PORTA_DATA_R |= 0x80;
	GPIO_PORTA_DATA_R =0x00;
	delay_micro(0);
}
void LCD_Write_Data(char* data, unsigned long size)
{
	unsigned long i = 0;
	LCD_Command(1);
	LCD_Command(2);
	for (i = 0; i < size; i++){
		LCD_Display(data[i]);
		LCD_Command(0X14);
	}
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

/*void LCD_Countdown(int time)
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
// time is in seconds
}*/

unsigned short Check (unsigned short j)
	{
	if (j==0){
		return (~GPIO_PORTD_DATA_R & 0X80);
	}
	else {
		return(~GPIO_PORTE_DATA_R & (1<<j));
	}
}

char Read_keypad()
{
	unsigned short i,j;
	while(1){
		for(i=0; i<4; i++){     //ROWS
			GPIO_PORTD_DATA_R &= ~(1<<i); 
			for(j=0; j<4; j++){   //COLUMN
				if(Check(j)){
					return keypad[i][j];
				}
		}	GPIO_PORTD_DATA_R |= 0X0F;
	}
}
}

void case_bc(char* str,unsigned short time)
{
	char kilo;
	char* ptr_kilo = &kilo;
	Loop:
			kilo = Read_keypad();
	    kilo = atoi(ptr_kilo);
			LCD_Write_Data(str, strlen(str));
		  if(kilo < 1 || kilo > 9 || kilo%1 != 0)
			{
				LCD_Write_Data("Err",3);
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
				LCD_Write_Data("Please Close Door", 17); //Display for 3 seconds
				Systick_Wait_1s(3);
				goto Loop;
			}
}

int main()
{
	char key;
        SYSTICK_Init();
        PortE_Init();
	PortF_Init();
	PortB_Init();
	PortD_Init();
	PortA_Init();
	
	key = Read_keypad();
	switch(key){
		case 'A':
		  POPCORN:
		  LCD_Write_Data("PopCorn", 7);
		  Systick_Wait_1s(10);
		// Display "PopCorn" for 10 sec 
			
		  if(check_door() == 0) //door closed = 0
		  {  
		  	LCD_Command(0x01); //Clear please close the door if closed before 5 sec
		  	/*LCD_Countdown(60);
		  	// Countdown 60 sec
		  	LCD_Command(0x01);  //clear lcd
		  	LED_Blink(3);
		  	Buzzer(3);
		  }
		  else
		 {
			LCD_Write_Data("Please Close Door"); //Display for 5 seconds
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
}	int num_2 = 0;
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
		}*/}}}
