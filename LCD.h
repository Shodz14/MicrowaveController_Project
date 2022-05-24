#define Clear_Display   		0x01		// Clears Display
#define SetTo8Bits          0x38		// 8-bit length, 2 display lines, 5x8 font
#define EntryMode           0x06    // Increments DDRAM address by 1 when a character is written into it
#define Cursor_On						0x0F		
#define Return_Home         0x02		//returns to idle state which is " What to eat? "
#define Next_Line 					0xC0
#define Cursor_Off					0x0C		// turns off curser blinking

void LCD_Command(char command);

void LCD_Init(void);

void LCD_Display(char data);

void LCD_Write_Data(char* data, unsigned short size);

void LCD_Countdown(char* time);

char* Timer (char kilo_in_numbers, char time);

char* Enter_Time(void);

void LED_Blink(unsigned short t);
