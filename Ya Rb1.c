#include "tm4c123gh6pm.h"

#define Clear_Display   		0x01
// Clears Display

#define SetTo8Bits          0x38
// 8-bit length, 2 display lines, 5x8 font

#define SetTo4Bits          0x28    // delay 37us
// 4-bit length, 2 display lines, 5x8 font

#define EntryMode           0x06    // delay 37us
// Increments DDRAM address by 1 when a character is written into it

#define Cursor_On						0x0F

#define Return_Home         0x02

#define Next_Line 					0xC0

void PortA_Init()
{
	SYSCTL_RCGCGPIO_R |= 0X01;
	while(!(SYSCTL_PRGPIO_R & 0X01)){};
	GPIO_PORTA_LOCK_R = GPIO_LOCK_KEY;
	GPIO_PORTA_CR_R |= 0x60;

	GPIO_PORTA_DEN_R |= 0x60;
	GPIO_PORTA_AFSEL_R = 0;
	GPIO_PORTA_PCTL_R = 0;
	GPIO_PORTA_AMSEL_R = 0;
	GPIO_PORTA_DIR_R |= 0x60;
	GPIO_PORTA_PUR_R = 0;
}

void PortB_Init(void)
{
	SYSCTL_RCGCGPIO_R |= 0x02;
	while(!(SYSCTL_PRGPIO_R & 0x02)){};
	GPIO_PORTB_LOCK_R = GPIO_LOCK_KEY;
	GPIO_PORTB_CR_R |= 0xFF;

	GPIO_PORTB_DEN_R |= 0xFF;
	GPIO_PORTB_AFSEL_R = 0;
	GPIO_PORTB_PCTL_R = 0;
	GPIO_PORTB_AMSEL_R = 0;
	GPIO_PORTB_DIR_R |= 0xFF;
	GPIO_PORTB_PUR_R = 0;
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
	GPIO_PORTF_DATA_R |= 0X00;
}

void Systick_Init()
{
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_RELOAD_R  = 0x00FFFFFF;
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

void Systick_Wait_1ms(unsigned long delay)
{
	unsigned long i;
	for(i = 0; i < delay; i++)
	{
		Systick_Wait(16000);
	}
}

void Systick_Wait_1us(unsigned long delay)
{
	unsigned long i;
	for(i = 0; i < delay; i++)
	{
		Systick_Wait(16);
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

void LCD_Display(char data) 
{ 
	GPIO_PORTA_DATA_R = 0x20; //RS=1,E=0
	GPIO_PORTB_DATA_R = data;
	GPIO_PORTA_DATA_R |= 0x40; //E=1
	Systick_Wait_1us(10);
	GPIO_PORTA_DATA_R &= ~0x60;
	GPIO_PORTB_DATA_R = 0;
}

void LCD_Write_Data(char* data, unsigned short size)
{
	unsigned long i = 0;
	LCD_Command(Clear_Display);
	LCD_Command(Return_Home);
	for (i = 0; i < size; i++){
		LCD_Display(*(data+i));
		Systick_Wait_1us(10);
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

void LCD_Init(void)
{		
	 LCD_Command(SetTo8Bits);
	 LCD_Command(EntryMode);
	 LCD_Command(Cursor_On);
	 Systick_Wait_1ms(2);
}

int main(void)
{
	PortA_Init();
	PortB_Init();
	PortF_Init();
	Systick_Init();
	LCD_Init();
	LCD_Command(Clear_Display);
	LCD_Command(Return_Home);
	
	while(1) {
				LCD_Write_Data("Popcorn",7);
				Systick_Wait_1ms(500);
				LCD_Command(Next_Line);
		}
	
}
