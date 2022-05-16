#include <stdlib.h>
#include "string.h"
//#include <Io.h>
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
	 GPIO_PORTD_AFSEL_R &= ~0XFF;
	 GPIO_PORTD_PCTL_R &= ~0XFFFFFFFF;
	 GPIO_PORTD_AMSEL_R = 0X00;
	 GPIO_PORTD_PUR_R = 0XF0;
	 GPIO_PORTD_DATA_R |= 0X8F;
}
void SYSTICK_Init()
{
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_CURRENT_R = 0;
	NVIC_ST_CTRL_R = 0X05;
}

void PortF_Init()
{
	SYSCTL_RCGCGPIO_R |= 0X20; //PortF clock enable
	//delay = SYSCTL_RCGCGPIO_R //Delay
	while ((SYSCTL_PRGPIO_R & 0x20) == 0){}; //Delay
	GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
	GPIO_PORTF_CR_R = 0X1F;

	GPIO_PORTF_DEN_R |= 0X1F;
	GPIO_PORTF_AFSEL_R &= ~0X1F;
	GPIO_PORTF_PCTL_R &= ~0XFFFFF;
	GPIO_PORTF_AMSEL_R &= ~0X1F;
	GPIO_PORTF_DIR_R = 0X0E;
	GPIO_PORTF_DATA_R &= ~0X0E;
	//GPIO_PORTF_PUR_R |= 0X11;
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
	GPIO_PORTE_DATA_R = 0X07;
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
	GPIO_PORTA_LOCK_R = GPIO_LOCK_KEY;
	GPIO_PORTA_CR_R = 0XE0;

	GPIO_PORTA_DEN_R |= 0XE0;
	GPIO_PORTA_AFSEL_R &= ~0XE0;
	GPIO_PORTA_PCTL_R &= ~0XFFF00000;
	GPIO_PORTA_AMSEL_R &= ~0XE0;
	GPIO_PORTA_DIR_R |= 0XE0;
	GPIO_PORTA_DATA_R = 0X00;
}


void Systick_Wait(unsigned long delay)
{
	NVIC_ST_CURRENT_R = 0;
	NVIC_ST_RELOAD_R = delay - 1;
	while(!(NVIC_ST_CTRL_R & 0X10000)){};
}

void Systick_Wait_1s(unsigned long delay)	//dealy 1 sec
{
	unsigned long i;
	for(i = 0; i < delay; i++)
	{
		Systick_Wait(16000000);
	}
}

void Systick_Wait_1ms(unsigned long delay)	//delay 1 milli
{
	unsigned long i;
	for(i = 0; i < delay; i++)
	{
		Systick_Wait(16000);
	}
}

void Systick_Wait_1us(unsigned long delay)	//delay 1 micro
{
	unsigned long i;
	for(i = 0; i < delay; i++)
	{
		Systick_Wait(16);
	}
}


void LCD_Command(unsigned char command)
{
	GPIO_PORTA_DATA_R = 0x00; //RS =0, E=0, RW=0
	GPIO_PORTB_DATA_R =command;
	GPIO_PORTA_DATA_R =0x80; //E=1 to secure command
	Systick_Wait_1us(2);
	GPIO_PORTA_DATA_R =0x00;
	if(command <4) Systick_Wait_1ms(2); else Systick_Wait_1us(37); 
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
	Systick_Wait_1us(2);
}
void LCD_Write_Data(char* data, unsigned long size)
{
	unsigned long i = 0;
	LCD_Command(1); //clears
	LCD_Command(2); //return home
	for (i = 0; i < size; i++){
		LCD_Display(data[i]);
		LCD_Command(0X14); //shift cursor to the right
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


/*unsigned short SW3() // function name
{
	unsigned short SW_3;
	SW_3 = (GPIO_PORTE_DATA_R & 0X10); //reading SW3 (door position)
	return SW_3;
}*/
	

unsigned short check_door()	// bool check_door()
{
	unsigned short SW_3;
	SW_3 = ((~GPIO_PORTE_DATA_R & 0X10)? 0:1 ); //reading SW3 (door position)
	return SW_3;
}


unsigned short Check (unsigned short j)
	{
	if (j==0){
		return 0;   //Testing
		//return (~GPIO_PORTD_DATA_R & 0X80);
	}
	else {
		return (8&(1<<j));   //Testing
		//return(~GPIO_PORTE_DATA_R & (1<<j));
	}
}

char Read_keypad()
{
	while(1){
	unsigned short i,j;
		for(i=0; i<4; i++){     //ROWS
			GPIO_PORTD_DATA_R &= ~(1<<i); 
			for(j=0; j<4; j++){    //COLUMN
				if(Check(j)){
					GPIO_PORTD_DATA_R |= 0X0F;
					return keypad[i][j];
				}
			}
			GPIO_PORTD_DATA_R |= 0X0F;
		}
	}
} 

int main()
{
//		char key = 0;   //for test
    SYSTICK_Init();
	  PortF_Init();
		PortE_Init();
	  PortB_Init();
		PortA_Init();
		PortD_Init();


		while(1){
			//Read_keypad();
			//key = Read_keypad();
			LCD_Command(0x01); //Clear
			LCD_Write_Data("PopCorn", 7);
				Systick_Wait_1s(10);
				}

}
