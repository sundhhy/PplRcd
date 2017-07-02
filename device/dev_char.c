#include "dev_char.h"
#include "deviceId.h"


#include "dev_uart/dev_uart.h"
#include "basis/sdhDebug.h"
#include "sdhDef.h"
#include "basis/sdhError.h"

int DevChar_open( int major, int minor, void **dev)
{
	int ret = 0;
//	I_dev_Char	*pdev = NULL;
	switch( major)
	{
		case DEVMAJOR_UART:
			*dev = Get_DevUart( minor);
			if( dev == NULL)
			{
				
				ret = ERR_BAD_PARAMETER;
			}
			break;
		case DEVMAJOR_GPIO:
			
		
			break;
		default:
			ret = ERR_BAD_PARAMETER;
			break;
		
		
	}
	
	return ret;
}
