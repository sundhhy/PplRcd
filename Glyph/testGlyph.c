#include "testGlyph.h"
#include "dev_LCD/UsartGpu/dev_LcdUsartGpu.h"
#include "cmsis_os.h"                                           // CMSIS RTOS header file
#include <string.h>
#include "device.h"

extern short  g_majorVer;		
extern short  g_childVer;

I_dev_lcd	*myLcd ;
void TestViewShow(void)
{
	 Dev_open( DEVID_USARTGPULCD, ( void *)&myLcd);

//	myLcd->Clear();
//	myLcd->wrString( "test", 0, 0, FONT_12, COLOUR_RED);
//	myLcd->wrString( "test", 0, 12, FONT_16, COLOUR_BLUE);
//	myLcd->wrString( "test", 0, 28, FONT_24, COLOUR_GREN);
//	myLcd->wrString( "test", 0, 60, FONT_32, COLOUR_YELLOW);
//	myLcd->wrString( "²âÊÔ", 0, 92, FONT_48, COLOUR_OTHER(5));
//	myLcd->wrString( "test", 0, 150, FONT_64, COLOUR_OTHER(6));
//	GpuSend( "CLS(0);PIC(0,0,9);\r\n");


}
