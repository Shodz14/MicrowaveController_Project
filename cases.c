#include "LCD.h"
#include "systick.h"
#include "string.h"
#include "stdlib.h"
#include "keypad.h"

void case_bc(char* str,unsigned short time)
{
	char kilo,i,*timer_ptr,timer[5];					
	
	Loop:
			LCD_Write_Data(str, strlen(str));
	    Systick_Wait_1ms(500);
			kilo = Read_Keypad();					// outputs a character value
	  	 if(kilo >= '1' && kilo <= '9' ) 	//to check if decimal or not (&& kilo%1 == 0)
					{
						LCD_Command(Clear_Display);		
	          LCD_Command(Return_Home);
						LCD_Display(kilo);
						Systick_Wait_1ms(2000);							// show weight for 2 seconds
						LCD_Command(Clear_Display);					
						Systick_Wait_1ms(200);							
						timer_ptr = Timer(kilo, time);	
						for(i = 0; i<5;i++)	
						{
								timer[i]=timer_ptr[i];					
						}
						LCD_Countdown(timer);
					}		  
			else 																			//If an illegal number is entered
					{
						LCD_Write_Data("Err",3);		
						Systick_Wait_1ms(2000);							// show “Err” for 2 seconds
						goto Loop;
					}
}
