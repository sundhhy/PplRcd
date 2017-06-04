#include "device.h"
#include <stdint.h>
#include <string.h>
#include "cmsis_os.h"                                           // CMSIS RTOS header file
#include "dev_LCD/UsartGpu/dev_LcdUsartGpu.h"


#include "basis/sdhDebug.h"
#include "sdhDef.h"
#include "basis/sdhError.h"

#define LCDBUF_MAX		128
#define LCD_DELAY_MS  7

static char	lcdBuf[LCDBUF_MAX];

I_dev_Char *I_sendDev;
static int ClearLcd( int c);
static int Dev_UsartdeInit( void);
static int GpuWrString( char *string,  int len, int x, int y, int font, char c);
static int GpuBox( int x1, int y1, int x2, int y2, char type, char c);
static void GpuBKColor( char c);
int GpuLabel( char *string,  int len, scArea_t *area, int font, char c, char ali);
static int GpuStrSize( int font, uint16_t	*width, uint16_t	*heigh);

I_dev_lcd g_IUsartGpu =
{
	Dev_UsartInit,
	Dev_UsartdeInit,
	ClearLcd,
	GpuWrString,
	GpuLabel,
	GpuBKColor,
	GpuBox,
	GpuStrSize,
	
};

int Dev_UsartInit( void)
{
	
	return Dev_open( DEVID_UART1, ( void *)&I_sendDev);
	
}

static int Dev_UsartdeInit( void)
{
	return   I_sendDev->close( );
	
	
}

static int ClearLcd( int c)
{
	sprintf( lcdBuf, "CLS(%d);\r\n", c);
	GpuSend(lcdBuf);
	osDelay(20);
	return RET_OK;
}

//画个方框
//type 0  空心 1 实心
static int GpuBox( int x1, int y1, int x2, int y2, char type, char c)
{
	if( type)
	{
		sprintf( lcdBuf, "BOXF(%d,%d,%d,%d,%d);\r\n", x1, y1, x2, y2,c);
	}
	else
	{
		sprintf( lcdBuf, "BOX(%d,%d,%d,%d,%d);\r\n", x1, y1, x2, y2,c);
	}
	
	GpuSend(lcdBuf);
	osDelay(LCD_DELAY_MS);
	return RET_OK;
	
}

static int GpuWrString( char *string, int len, int x, int y, int font, char c)
{
	
	char colour[16];
	short		charMax = LCDBUF_MAX;
	
	
	switch( font)
	{
		case FONT_12:
			sprintf( lcdBuf, "DS12(%d,%d,'", x, y);
			
			break;
		case FONT_16:
			sprintf( lcdBuf, "DS16(%d,%d,'", x, y);
			break;
		case FONT_24:
			sprintf( lcdBuf, "DS24(%d,%d,'", x, y);
			break;
		case FONT_32:
			sprintf( lcdBuf, "DS32(%d,%d,'", x, y);
			break;
		case FONT_48:
			sprintf( lcdBuf, "DS48(%d,%d,'", x, y);
			break;
		case FONT_64:
			sprintf( lcdBuf, "DS64(%d,%d,'", x, y);
			break;
		default:
			sprintf( lcdBuf, "DS12(%d,%d,'", x, y);
			break;
	}	
	sprintf(colour, "',%d);\r\n",c);
	
	charMax -= strlen( lcdBuf) + strlen( colour);
	if( len > charMax)
		len = charMax;
	
	strncat( lcdBuf,string, len);
	strcat( lcdBuf,colour);
	GpuSend(lcdBuf);
	osDelay(LCD_DELAY_MS);
	return RET_OK;
}

int GpuLabel( char *string,  int len, scArea_t *area, int font, char c, char ali)
{
	char m = 0;
	short		charMax = LCDBUF_MAX;
	char tail[16];
	switch( font)
	{
		case FONT_12:
			m = 12;
			break;
		case FONT_16:
			m = 16;
			break;
		case FONT_24:
			m = 24;
			break;
		case FONT_32:
			m = 32;
			break;
		case FONT_48:
			m = 48;
			break;
		case FONT_64:
			m = 64;
			break;
		default:
			m = 12;
			break;
		
	}
	sprintf( lcdBuf, "LABL(%d,%d,%d,%d,'", m, area->x1, area->y1, area->x2);
	
	sprintf(tail, "',%d,%d);\r\n",c,ali);
	charMax -= strlen( lcdBuf) + strlen( tail);
	if( len > charMax)
		len = charMax;
	strncat( lcdBuf,string, len);
	strcat( lcdBuf,tail);
	GpuSend(lcdBuf);
	osDelay(LCD_DELAY_MS);
	return RET_OK;

}	

static void GpuBKColor( char c)
{
	if( c == 0)
		return;
	sprintf( lcdBuf, "SBC(%d);", c);
	GpuSend(lcdBuf);
	osDelay(LCD_DELAY_MS);
	memset( lcdBuf, 0, LCDBUF_MAX);

}


//int GpuWrSection( dspArea_t *area, dspContent_t *arg)
//{
//	
//	char colour[16];
//	short		charMax = LCDBUF_MAX;
//	
//	
//	switch( arg->font)
//	{
//		case FONT_12:
//			sprintf( lcdBuf, "BS12(%d,%d,%d,%d,'", area->x1, area->y1, area->x2, area->rowW);
//			break;
//		case FONT_16:
//			sprintf( lcdBuf, "BS12(%d,%d,%d,%d,'", area->x1, area->y1, area->x2, area->rowW);
//			break;
//		case FONT_24:
//			sprintf( lcdBuf, "BS12(%d,%d,%d,%d,'", area->x1, area->y1, area->x2, area->rowW);
//			break;
//		default:
//			sprintf( lcdBuf, "BS12(%d,%d,%d,%d,'", area->x1, area->y1, area->x2, area->rowW);
//			break;
//	}	
//	sprintf(colour, "',%d);\r\n",arg->colour);
//	
//	charMax -= strlen( lcdBuf) + strlen( colour);
//	if( arg->len > charMax)
//		arg->len = charMax;
//	
//	strncat( lcdBuf,arg->data, arg->len);
//	strcat( lcdBuf,colour);
//	GpuSend(lcdBuf);
//	osDelay(LCD_DELAY_MS);
//	
//	return RET_OK;
//}


static int GpuStrSize( int font, uint16_t	*width, uint16_t	*heigh)
{
	
	uint16_t x, y;
	switch( font)
	{
		case FONT_12:
			x = 6;
			y = 8;
			break;
		case FONT_16:
			x = 8;
			y = 16;
			break;
		case FONT_24:
			x = 12;
			y = 24;
			break;
		case FONT_32:
			x = 16;
			y = 32;
			break;
		case FONT_48:
			x = 24;
			y = 48;
			break;
		case FONT_64:
			x = 32;
			y = 64;
			break;
		default:
			x = 6;
			y = 8;
			break;
	}
	
	if( width)
		*width = x;
	if( heigh)
		*heigh = y;
	
	return RET_OK;
}

void GpuSend(char * buf)
{
	int 	len = strlen( buf);
	int ret = 0;
	ret = I_sendDev->write( buf, len);
	
	if( ret )
		osDelay(20);
		
	
}
