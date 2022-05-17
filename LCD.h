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
