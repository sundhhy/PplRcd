#include "dev_char.h"
#include "deviceId.h"

#include "dev_uart2/dev_uart2.h"

#include "basis/sdhDebug.h"
#include "sdhDef.h"
#include "basis/sdhError.h"

int DevChar_open( int major, int minor, void **dev)
{
	int ret = 0;
	I_dev_Char	*pdev = NULL;
	switch( major)
	{
		case DEVMAJOR_UART:
			if( minor == 1)
			{
//				pdev = ( I_dev_Char*)*dev;
//				pdev = (I_dev_Char	*)Dev_Uart2_new();
//				pdev->open();
				*dev = Get_DevUart2();
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
