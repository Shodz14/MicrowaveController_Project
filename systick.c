#include "tm4c123gh6pm.h"
#include "systick.h"

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

