#include "tm4c123gh6pm.h" // Device header

void delay_milli(int n);
void delay_micro(int n);
void LCD_init(void);
void LCD_Cmd(unsigned char command);
void LCD_Data(unsigned char data);

/*int main()

{
	LCD_init();
	// while(1)
	// {
	LCD_Cmd(0x01);
	LCD_Cmd(0x80);
	delay_milli(500);
	LCD_Data('C');
	delay_milli(1);
	LCD_Data('O');
	delay_milli(1);
	LCD_Data('R');
	delay_milli(1);
	LCD_Data('T');
	delay_milli(1);
	LCD_Data('E');
	delay_milli(1);
	LCD_Data('X');
	delay_milli(1);
	LCD_Data('-');
	delay_milli(1);
	LCD_Data('M');
	delay_milli(1);
	LCD_Data('.');
	delay_milli(1);
	LCD_Data('C');
	delay_milli(1);
	LCD_Data('O');
	delay_milli(1);
	LCD_Data('M');
	delay_milli(500);
	//}
}*/

void LCD_Init(void)
{
	SYSCTL_RCGCGPIO_R |= 0x01; //PORTA clock
	SYSCTL_RCGCGPIO_R |= 0x02;
	GPIO_PORTA_DIR_R |=0xE0; //PORTA controls RS,E and R/W
	GPIO_PORTA_DEN_R |=0xE0;
	GPIO_PORTB_DIR_R |=0xFF; //PORTB D0-D7
	GPIO_PORTB_DEN_R |=0xFF; //PORTB D0-D7
	LCD_Cmd(0x38); //8-bits,2 display lines, 5x7 font
	LCD_Cmd(0x06); //increments automatically
	LCD_Cmd(0x0F); //Turn on display
	LCD_Cmd(0x01); //clear display
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

void LCD_Data(unsigned char data) 
{ 
	GPIO_PORTA_DATA_R =0x20; //RS=1, E=0,RW=0
	GPIO_PORTB_DATA_R =data;
	GPIO_PORTA_DATA_R |= 0x80;
	GPIO_PORTA_DATA_R =0x00;
	delay_micro(0);
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