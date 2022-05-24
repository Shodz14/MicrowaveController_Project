#include "tm4c123gh6pm.h"
#include "ports.h"
#include "systick.h"
#include "LCD.h"
#include "keypad.h"
#include "cases.h"

int main(void)
{
	char key, i, *time_ptr, time[5];
	PortA_Init();
	PortB_Init();
	PortE_Init();
	PortF_Init();
	Systick_Init();
	LCD_Init();
	LCD_Command(Clear_Display);
	LCD_Command(Return_Home);
	
	while(1) {
		LCD_Write_Data("What to eat?",12);
		Systick_Wait_1ms(500);
		key = Read_Keypad();
		switch(key){
		case 'A':
		  LCD_Write_Data("PopCorn", 7);				//display PopCorn on LCD
		  Systick_Wait_1ms(3000);							//display PopCorn on LCD for 3 seconds
		  time_ptr = Timer('1',60);							
			for(i = 0; i < 5; i++)							
			{
				time[i] = time_ptr[i];						
			}
		  LCD_Countdown(time); 								
	    break;
	
		case 'B':
			case_bc("Beef Weight?",30);					//display Beef Weight? on LCD 
		break;
		case 'C':
		  case_bc("Chicken Weight?",12);			//display Chicken Weight? on LCD 
		break;
		case 'D':
			LCD_Write_Data("Cooking Time:",13);		
		  Systick_Wait_1ms(1000);							//display Cooking Time: on LCD for 3 seconds
			time_ptr = Enter_Time();						
			for(i = 0; i < 5; i++)
			{
				time[i] = time_ptr[i];						
			}
			LCD_Countdown(time);								
			break;
		}
	
}}
