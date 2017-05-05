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

I_dev_lcd g_IUsartGpu =
{
	Dev_UsartInit,
	Dev_UsartdeInit,
	ClearLcd,
	GpuWrString,
	
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

void GpuSend(char * buf)
{
	int 	len = strlen( buf);
	int ret = 0;
	ret = I_sendDev->write( buf, len);
	
	if( ret )
		osDelay(20);
		
	
}
