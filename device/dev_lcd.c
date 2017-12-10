#include "dev_lcd.h"
#include "deviceId.h"

#include "dev_LCD/UsartGpu/dev_LcdUsartGpu.h"


#include "basis/sdhDebug.h"
#include "sdhDef.h"
#include "basis/sdhError.h"


int DevLCD_open( int major, int minor, void **dev)
{
	int ret = 0;
	I_dev_lcd	*pdev = NULL;
	switch( major)
	{
		case DEVMAJOR_LCD:
			if( minor == 1)
			{
				pdev = (I_dev_lcd	*)( &g_IUsartGpu);
//				pdev->open();
				*dev = pdev;
			}
			else
			{
				
				ret = ERR_BAD_PARAMETER;
			}
			break;
		default:
			ret = ERR_BAD_PARAMETER;
			break;
		
		
	}
	
	return ret;
}

