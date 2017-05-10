#ifndef __DEV_UART1_
#define __DEV_UART1_
#include "basis/sdhError.h"
#include "lw_oopc.h"

#include "dev_char.h"

CLASS( Dev_Uart1)
{
	IMPLEMENTS( I_dev_Char);
	
	
};

int Dev_Uart1_init( void);

Dev_Uart1 *Get_DevUart1(void);
#endif
