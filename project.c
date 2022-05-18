#include "tm4c123gh6pm.h"
#include "stdbool.h"
#include "stdlib.h"
#include "string.h"
#include "LCD.h"

char keypad[4][4] = 
{
	{ '1', '2', '3', 'A'},
	{ '4', '5', '6', 'B'},
	{ '7', '8', '9', 'C'},
	{ '*', '0', '#', 'D'},
};

void PortA_Init()
{
	SYSCTL_RCGCGPIO_R |= 0X01;
	while(!(SYSCTL_PRGPIO_R & 0X01)){};
	GPIO_PORTA_LOCK_R = GPIO_LOCK_KEY;
	GPIO_PORTA_CR_R |= 0xFC;

	GPIO_PORTA_DEN_R |= 0xFC;
	GPIO_PORTA_AFSEL_R = 0;
	GPIO_PORTA_PCTL_R = 0;
	GPIO_PORTA_AMSEL_R = 0;
	GPIO_PORTA_DIR_R = 0xFC;
	GPIO_PORTA_DATA_R = 0x00;

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


void PortE_Init()
{
	SYSCTL_RCGCGPIO_R |= 0X10;
	while((SYSCTL_PRGPIO_R & 0X10)==0){};
	GPIO_PORTE_LOCK_R = GPIO_LOCK_KEY;
	GPIO_PORTE_CR_R = 0X1E;

	GPIO_PORTE_DEN_R |= 0X1E;
	GPIO_PORTE_AFSEL_R &= ~0X1E;
	GPIO_PORTE_PCTL_R &= ~0XFFFF0;
	GPIO_PORTE_AMSEL_R &= ~0X1E;
	GPIO_PORTE_DIR_R = 0X1E;
	GPIO_PORTE_DATA_R = 0X1E;
	GPIO_PORTE_DIR_R = 0X00;
	GPIO_PORTE_PDR_R = 0x1E;
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

void LCD_Init(void)
{		
	 LCD_Command(SetTo8Bits);
	 LCD_Command(EntryMode);
	 LCD_Command(Cursor_On);
	 Systick_Wait_1ms(2);
}

void LCD_Display(char data) 
{ 
	GPIO_PORTA_DATA_R &= ~0x60;
	GPIO_PORTA_DATA_R |= 0x20; //RS=1,E=0
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

void Set(unsigned short i)
{
	if (i < 4)
	{
		GPIO_PORTA_DATA_R |= (0X04<<i);
	}
	else
	{
		GPIO_PORTA_DATA_R |= 0x80;
	}
}

char Read_Keypad()
{
	unsigned short i,j;
	while(1){
		for(i = 0; i <= 4; i++){ 			//COLUMNS
			Set(i);
			Systick_Wait_1us(3);
			for( j = 0; j < 4; j++){   //COLUMN
				if((GPIO_PORTE_DATA_R & 0x1E) & (1U << j+1)){
					if(i ==4) i--;
					return keypad[j][i];
				}
		}
		GPIO_PORTA_DATA_R &= ~0X9C;
	}
}
}

bool SW3()
{
	return 1;
}
bool SW2()
{
	return(GPIO_PORTF_DATA_R & 0X01); //reading SW2
}
bool SW1()
{
	return(GPIO_PORTF_DATA_R & 0X10); //reading SW1
}

void LCD_Countdown(char* time)   // 15:45   0=>9   48=>57        : => 58
{
	char i;
	char temp[5];
	for(i = 0; i<5;i++)
	{
		temp[i]=time[i];
	}
	for(;temp[0] >= 48; temp[0]--)
	{
		for(;temp[1] >= 48; temp[1]--)
		{
				for(;temp[3] >= 48; temp[3]--)
				{
					for(;temp[4] >= 48;temp[4]--)
					{
						LCD_Write_Data(temp,strlen(temp));
						// while(0 /SW3() == 0){}
						if(0)//SW1() ==  0/)
						{
							Systick_Wait_1ms(500);
							while(SW2() == 1 && SW1() == 1){}
							if(SW1() == 0){return;}
						}
						Systick_Wait_1ms(1000);
					}
					temp[4]= 57;
				}
				temp[3]= 53;
		}
		temp[1]= 57;
	}
}

char* Timer (char kilo_in_numbers, char time)
{
	unsigned short time_in_seconds;
	char time_in_char[5];
	time_in_seconds = kilo_in_numbers * time;
	time_in_char[0] = (time_in_seconds/600)+48;
	time_in_seconds = time_in_seconds % 600 ;
	time_in_char[1] = (time_in_seconds/60)+48;
	time_in_seconds = time_in_seconds % 60 ;
	time_in_char[2] = ':';
	time_in_char[3] = (time_in_seconds/10)+48;
	time_in_seconds = time_in_seconds % 10;
	time_in_char[4] = time_in_seconds+48;
	return time_in_char;
}

void Wait_For_Input()
{
	while(((GPIO_PORTE_DATA_R & 0x3E) == 0x3E )&&((GPIO_PORTF_DATA_R & 0x11) == 0x11)){}
}

char* Enter_Time()
{
	char i, counter = 1;
	char time[5] = "00:00";
	LCD_Write_Data(time, 5);
	Wait_For_Input();
	while(SW2() == 1)
	{
		if(counter == 1){
			time[4] = Read_Keypad();
			LCD_Write_Data(time, 5);
			counter++;}
		else if(counter == 2){
			time[3] = time[4];
			time[4] = Read_Keypad();
			LCD_Write_Data(time, 5);			
			counter++;}
		else if(counter == 3){
			time[1] = time[3];
			time[3] = time[4];
			time[4] = Read_Keypad();
			LCD_Write_Data(time, 5);
			counter++;}
		else if(counter == 4){
			time[0] = time[1];
			time[1] = time[3];
			time[3] = time[4];
			time[4] = Read_Keypad();
			LCD_Write_Data(time, 5);
			counter++;}
		Wait_For_Input();
	}
	return time;
}

void case_bc(char* str,unsigned short time)
{
	char kilo;
	char* ptr_kilo = &kilo;						//char* ptr_kilo = kilo; pointer to var address 
	unsigned short kilo_int;
	char i;
	char* timer_ptr;
	char timer[5];
	for(i = 0; i<5;i++)
	{
			timer[i]=timer_ptr[i];
	}
		LCD_Countdown(timer);
	
	Loop:
			LCD_Write_Data(str, strlen(str));
	    Systick_Wait_1ms(500);
			kilo = Read_Keypad();					// outputs a character value
			kilo_int = atoi(ptr_kilo);				// converts char into Integer
	  	 if(kilo_int >= 1 && kilo_int <= 9 ) 	//to check if decimal or not (&& kilo%1 == 0)
					{
						LCD_Command(Clear_Display);		
						LCD_Write_Data(ptr_kilo,1);					//LCD_Display(kilo);
						Systick_Wait_1s(2);							// show weight for 2 seconds
						LCD_Command(Clear_Display);
						Systick_Wait_1ms(200);
						timer_ptr = Timer(kilo_int, time);
						for(i = 0; i<5;i++)
						{
								timer[i]=timer_ptr[i];
						}
						LCD_Countdown(timer);
						/*while (SW3 == 0 && SW2 == 0)
							{
								LCD_Command(Clear_Display);
								Systick_Wait_1ms(200);
								LCD_Countdown("01:00");	
							}*/
					}		  
			else //(kilo== '*'||kilo=='#')				//wrong char inputed
					{
						LCD_Write_Data("Err",3);		
						Systick_Wait_1s(2);							// show “Err” for 2 seconds
						goto Loop;
					}
			
			
					
			/*sw3 = SW3();				//putting return of func into var
		  if(sw3 == 0) //door closed = 0
			{
			  LCD_Command(Clear_Display);
				Systick_Wait_1ms(200);
		    if(sw2 ==0)		LCD_Countdown(kilo * time);			
		// Countdown 60 sec
		    LCD_Command(Clear_Display);
		    LED_Blink(3);
		    Buzzer(3);
			}
			else
			{
				LCD_Write_Data("Please Close Door", 17); //Display for 3 seconds
				Systick_Wait_1s(3);
				goto Loop;
			}*/
}


int main(void)
{
	char key ;
	PortA_Init();
	PortB_Init();
	PortE_Init();
	PortF_Init();
	Systick_Init();
	LCD_Init();
	LCD_Command(Clear_Display);
	LCD_Command(Return_Home);
	
	
	while(1) {
		char i;
		char* time_ptr;
		char time[5];
		LCD_Write_Data("What to eat ?",12);
		Systick_Wait_1ms(500);
		key = Read_Keypad();
		switch(key){
		case 'A':
		  POPCORN:
		  LCD_Write_Data("PopCorn", 7);
		  Systick_Wait_1s(3);
		  LCD_Command(Next_Line);
		  LCD_Countdown("01:00"); 
	    break;
		
			/*if(check_door() == 0) //door closed = 0
		  {  
		  	LCD_Command(Clear_Display); //Clear please close the door if closed before 5 sec
		  	//LCD_Countdown(60);
		  	// Countdown 60 sec
		  	LCD_Command(Clear_Display);  //clear lcd
		  	LED_Blink(3);
		  	Buzzer(3);
		  }
		  else
		 {
			LCD_Write_Data("Please Close Door",17); //Display for 5 seconds
			Systick_Wait_1s(3);
			goto POPCORN;
		}
		break;*/
		case 'B':
			case_bc("Beef Weight ?",30);
		break;
		case 'C':
		  case_bc("Chicken Weight ?",12);
		break;
		case 'D':
			time_ptr = Enter_Time();
			for(i = 0; i < 4; i++)
			{
				time[i] = time_ptr[i];
			}
			LCD_Countdown(time);
			break;
		}
	
}}
