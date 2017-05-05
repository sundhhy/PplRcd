#ifndef __DEV_UART2_
#define __DEV_UART2_
#include "basis/sdhError.h"
#include "lw_oopc.h"

#include "dev_char.h"

CLASS( Dev_Uart2)
{
	IMPLEMENTS( I_dev_Char);
	
	
};

int Dev_Uart2_init( void);

Dev_Uart2 *Get_DevUart2(void);
#endif
