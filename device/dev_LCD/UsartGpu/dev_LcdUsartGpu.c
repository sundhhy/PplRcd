#include "device.h"
#include <stdint.h>
#include <string.h>
#include "cmsis_os.h"                                           // CMSIS RTOS header file
#include "dev_LCD/UsartGpu/dev_LcdUsartGpu.h"
#include "os/os_depend.h"


#include "basis/sdhDebug.h"
#include "sdhDef.h"
#include "basis/sdhError.h"
//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define LCDBUF_MAX				128
#define LCD_DELAY_MS  			20
#define	UGPU_CMDBUF_LEN			1024

#define USE_CMD_BUF				1
//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------
I_dev_Char *I_sendDev;

//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------

//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------

static short 	cmd_count = 0;
//¸ø´®¿ÚÆÁÄ»ÓÃµÄÐÅºÅÁ¿, ÖµÎª6ÊÇÒòÎª6Ö®Ç°µÄÐÅºÅÁ¿¶¼±»´®¿ÚÊ¹ÓÃµôÁË¡£
//todo: µ±È»£¬Èç´ËÊÖ¶¯·ÖÅäÐÅºÅÁ¿ÊÇ²»ºÏÀí£¬ÒÔºóÓÐÊ±¼ä¸Ä½ø
static sem_t	gpu_sem = 6;	

static char	lcdBuf[LCDBUF_MAX];


//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static int ClearLcd( int c);
static int Dev_UsartdeInit( void);
static int GpuWrString( char m, char *string,  int len, int x, int y, int font, char c);
static int GpuBox( int x1, int y1, int x2, int y2, char type, char c);
static void GpuBKColor( char c);
int GpuLabel( char *string,  int len, scArea_t *area, int font, char c, char ali);
static int GpuStrSize( int font, uint16_t	*width, uint16_t	*heigh);
static void GetScrnSize( uint16_t *xsize, uint16_t *ysize);
static void GpuPic( int x1, int y1, char num);
static void GpuCutPicture( short x1, short y1, short num, short px1, short py1, short w, short h);
static void GpuBPic( char m, int x1, int y1, char num);
static void GpuDone( void);
static void Cmdbuf_manager(char *p_cmd);
static void GpuIcon(int x1, int y1, char num, int xn, int yn, int n);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

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
	GetScrnSize,
	GpuPic,
	GpuCutPicture,
	GpuBPic,
	GpuDone,
	GpuIcon,
	
};
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{



static void GpuCutPicture( short x1, short y1, short num, short px1, short py1, short w, short h)
{
#if USE_CMD_BUF == 1
	Sem_wait(&gpu_sem, FOREVER);
	sprintf( lcdBuf, "CPIC(%d,%d,%d,%d,%d,%d,%d);", x1, y1, num, px1, py1, w, h );
	Cmdbuf_manager(lcdBuf);
	GpuSend(lcdBuf);
	Sem_post(&gpu_sem);
#else
	sprintf( lcdBuf, "CPIC(%d,%d,%d,%d,%d,%d,%d);\r\n", x1, y1, num, px1, py1, w, h );
	GpuSend(lcdBuf);
	osDelay(20);
#endif
	
}

static void GpuPic( int x1, int y1, char num)
{
#if USE_CMD_BUF == 1
	Sem_wait(&gpu_sem, FOREVER);
	sprintf( lcdBuf, "PIC(%d,%d,%d);", x1, y1, num);
	Cmdbuf_manager(lcdBuf);
	GpuSend(lcdBuf);
	Sem_post(&gpu_sem);
#else	
	sprintf( lcdBuf, "PIC(%d,%d,%d);\r\n", x1, y1, num);
	GpuSend(lcdBuf);
	osDelay(40);
#endif
}


static void GpuIcon(int x1, int y1, char num, int xn, int yn, int n)
{
#if USE_CMD_BUF == 1
	Sem_wait(&gpu_sem, FOREVER);
	sprintf( lcdBuf, "ICON(%d,%d,%d,%d,%d,%d);", x1, y1, num, xn, yn, n);
	Cmdbuf_manager(lcdBuf);
	GpuSend(lcdBuf);
	Sem_post(&gpu_sem);
#else	
	sprintf( lcdBuf, "ICON(%d,%d,%d,%d,%d,%d);\r\n", x1, y1, num, xn, yn, n);
	GpuSend(lcdBuf);
	osDelay(40);
#endif
}

static void GpuBPic( char m, int x1, int y1, char num)
{
#if USE_CMD_BUF == 1
	Sem_wait(&gpu_sem, FOREVER);
	sprintf( lcdBuf, "BPIC(%d,%d,%d,%d);",m,  x1, y1, num);
	Cmdbuf_manager(lcdBuf);
	GpuSend(lcdBuf);
	Sem_post(&gpu_sem);
#else	
	sprintf( lcdBuf, "BPIC(%d,%d,%d,%d);\r\n",m,  x1, y1, num);
	GpuSend(lcdBuf);
	osDelay(160);
#endif
}




int Dev_UsartInit( void)
{
	Sem_init(&gpu_sem);
	Sem_post(&gpu_sem);
	return Dev_open( DEVID_UART1, ( void *)&I_sendDev);
	
}

static int Dev_UsartdeInit( void)
{
	return   I_sendDev->close( I_sendDev);
	
	
}

static int ClearLcd( int c)
{
#if USE_CMD_BUF == 1
	Sem_wait(&gpu_sem, FOREVER);
	sprintf( lcdBuf, "CLS(%d);", c);
	Cmdbuf_manager(lcdBuf);
	GpuSend(lcdBuf);
	Sem_post(&gpu_sem);
#else	
	sprintf( lcdBuf, "CLS(%d);\r\n", c);
	GpuSend(lcdBuf);
	osDelay(20);
#endif	
	
	
	
	return RET_OK;
}

static void GetScrnSize( uint16_t *xsize, uint16_t *ysize)
{
	*xsize = 320;
	*ysize = 240;
}

//»­¸ö·½¿ò
//type 0  ¿ÕÐÄ 1 ÊµÐÄ
static int GpuBox( int x1, int y1, int x2, int y2, char type, char c)
{
	
#if USE_CMD_BUF == 1
	Sem_wait(&gpu_sem, FOREVER);
	if( type == FILLED_RECTANGLE)
	{
		sprintf( lcdBuf, "BOXF(%d,%d,%d,%d,%d);", x1, y1, x2, y2,c);
	}
	else if( type == EMPTY_RECTANGLE)
	{
		sprintf( lcdBuf, "BOX(%d,%d,%d,%d,%d);", x1, y1, x2, y2,c);
	}
	else if( type == LINE)
	{
		//todo:Ïß¶
		sprintf( lcdBuf, "PL(%d,%d,%d,%d,%d);", x1, y1, x2, y2,c);
	}
	Cmdbuf_manager(lcdBuf);
	GpuSend(lcdBuf);
	Sem_post(&gpu_sem);
#else	
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
#endif	

	return RET_OK;
	
}

static int GpuWrString( char m ,char *string, int len, int x, int y, int font, char c)
{
	
	char 		colour[16];
	short		charMax = LCDBUF_MAX;
	short 		f = font;
	
	if( CHECK_FONT(f) == 0) {
		f = FONT_12;
	}
		
	if( m < 0x80) {
		
		sprintf( lcdBuf, "PS%d(%d,%d,%d,'",f, m, x, y);
		sprintf(colour, "',%d, 0);",c);
		
	} else {
		
		sprintf( lcdBuf, "DS%d(%d,%d,'",f, x, y);
		sprintf(colour, "',%d);",c);
	}

	
	
	charMax -= strlen( lcdBuf) + strlen( colour);
	if( len > charMax)
		len = charMax;
	
	strncat( lcdBuf,string, len);
	strcat( lcdBuf,colour);
	
#if USE_CMD_BUF == 1
	Sem_wait(&gpu_sem, FOREVER);
	Cmdbuf_manager(lcdBuf);
	GpuSend(lcdBuf);
	Sem_post(&gpu_sem);
#else	
	strcat( lcdBuf,"\r\n");
	GpuSend(lcdBuf);
	osDelay(LCD_DELAY_MS);
#endif	
	
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
#if USE_CMD_BUF == 1
	sprintf(tail, "',%d,%d);",c,ali);
#else	
	sprintf(tail, "',%d,%d);\r\n",c,ali);
#endif	
	
	charMax -= strlen( lcdBuf) + strlen( tail);
	if( len > charMax)
		len = charMax;
	strncat( lcdBuf,string, len);
	strcat( lcdBuf,tail);
	
#if USE_CMD_BUF == 1
	Sem_wait(&gpu_sem, FOREVER);
	Cmdbuf_manager(lcdBuf);
	GpuSend(lcdBuf);
	Sem_post(&gpu_sem);
#else	
	GpuSend(lcdBuf);
	osDelay(LCD_DELAY_MS);
#endif	
	return RET_OK;

}	

static void GpuBKColor( char c)
{
	if( c == ERR_COLOUR)
		return;
	sprintf( lcdBuf, "SBC(%d);", c);
#if USE_CMD_BUF == 1
	Sem_wait(&gpu_sem, FOREVER);
	Cmdbuf_manager(lcdBuf);
	GpuSend(lcdBuf);
	Sem_post(&gpu_sem);
#else	
	GpuSend(lcdBuf);
	memset( lcdBuf, 0, LCDBUF_MAX);
#endif	
	

}
static void Cmdbuf_manager(char *p_cmd)
{

	uint8_t cmd_len = strlen(p_cmd);
	
	
	if((cmd_count +  cmd_len) > UGPU_CMDBUF_LEN) {
		GpuDone();
		osDelay(200);
	} 
		
	cmd_count += cmd_len;
}
static void GpuDone( void)
{
#if USE_CMD_BUF == 1
	char tmpbuf[8] = {0};
	int		ret = 0;
	Sem_wait(&gpu_sem, FOREVER);
	while(1) {
		strcpy(tmpbuf, "\r\n");
		GpuSend(tmpbuf);
		ret = I_sendDev->read(I_sendDev, tmpbuf, 8);
		if(ret > 0) {
			if(tmpbuf[0] == 'O' && tmpbuf[1] == 'K')
				break; 
			else
				goto err;
		} else
			goto err;
		
			
		
	}
	//todo:ÐèÒªÔö¼Ó´íÎó´¦Àí
	err:
//	osDelay(200);
	cmd_count = 0;
	Sem_post(&gpu_sem);
#endif
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
	ret = I_sendDev->write( I_sendDev, buf, len);
	
	if( ret )
		osDelay(20);
		
	
}
