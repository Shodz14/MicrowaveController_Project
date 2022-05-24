#include "ports.h"
#include "systick.h"
#include "tm4c123gh6pm.h"

char keypad[4][4] = 
{
	{ '1', '2', '3', 'A'},
	{ '4', '5', '6', 'B'},
	{ '7', '8', '9', 'C'},
	{ '*', '0', '#', 'D'},
};




char Read_Keypad()									// reads any input from keypad
{
	char i,j;
	while(1){
		for(i = 0; i <= 3; i++){ 			//COLUMNS
			Set(i);
			Systick_Wait_1us(3);
			for(j = 0; j < 4; j++){   //ROWS
				if (SW2() == 0) return '@';				//flag for starting 
				if (SW1() == 0) return '!';				//flag for stop
				if((GPIO_PORTE_DATA_R & 0x1E) & (1 << (j+1))){
					return keypad[j][i];
				}
		}
		GPIO_PORTA_DATA_R &= ~0X9C;					//bit clearing the bits holding columns. to stop reading from these columns
	}
}
}
