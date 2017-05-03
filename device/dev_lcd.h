#ifndef __DEV_LCD_H_
#define __DEV_LCD_H_
#include <stdint.h>

#define FONT_12			1
#define FONT_16			2
#define FONT_24			3
#define FONT_32			4
#define FONT_48			5
#define FONT_64			6


#define COLOUR_RED			1
#define COLOUR_GREN			2
#define COLOUR_BLUE			3
#define COLOUR_YELLOW		4
#define COLOUR_OTHER(n)		n

//字符型设备的接口
//包括uart设备

typedef struct{
	int ( *open)( void);
	int ( *close)( void);
	int ( *Clear)( void);
	int ( *wrString)( char *string, int x, int y, int font, int c);
	
	
}I_dev_lcd;

int DevLCD_open( int major, int minor, void **dev);




#endif


