#include "device.h"
#include <stdint.h>
#include <string.h>
#include "cmsis_os.h"                                           // CMSIS RTOS header file
#include "dev_LCD/UsartGpu/dev_LcdUsartGpu.h"


#include "basis/sdhDebug.h"
#include "sdhDef.h"
#include "basis/sdhError.h"

static char	lcdBuf[128];

I_dev_Char *I_sendDev;
static int ClearLcd( void);
static int Dev_UsartdeInit( void);
static int GpuWrString( char *string,  int len, int x, int y, int font, int c);
static int GpuStrSize( int font, uint16_t	*width, uint16_t	*heigh);

I_dev_lcd g_IUsartGpu =
{
	Dev_UsartInit,
	Dev_UsartdeInit,
	ClearLcd,
	GpuWrString,
	GpuStrSize,
	
};

int Dev_UsartInit( void)
{
	
	return Dev_open( DEVID_UART2, ( void *)&I_sendDev);
	
}

static int Dev_UsartdeInit( void)
{
	return   I_sendDev->close( );
	
	
}

static int ClearLcd( void)
{
	GpuSend("CLS(0);\r\n");
	osDelay(20);
	return RET_OK;
}

static int GpuWrString( char *string, int len, int x, int y, int font, int c)
{
	
	char colour[8];
	
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
	
//	GpuSend(lcdBuf);
//	GpuSend(string);
//	
//	sprintf(lcdBuf, "',%d);\r\n",c);
//	
//	GpuSend(lcdBuf);
	
	sprintf(colour, "',%d);\r\n",c);
	strncat( lcdBuf,string, len);
	strcat( lcdBuf,colour);
	GpuSend(lcdBuf);
	osDelay(20);
	return RET_OK;
}


static int GpuStrSize( int font, uint16_t	*width, uint16_t	*heigh)
{
	
	char colour[8];
	uint16_t x, y;
	switch( font)
	{
		case FONT_12:
			x = 12;
			y = 12;
			break;
		case FONT_16:
			x = 16;
			y = 16;
			break;
		case FONT_24:
			x = 24;
			y = 24;
			break;
		case FONT_32:
			x = 32;
			y = 32;
			break;
		case FONT_48:
			x = 48;
			y = 48;
			break;
		case FONT_64:
			x = 64;
			y = 64;
			break;
		default:
			x = 64;
			y = 64;
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
