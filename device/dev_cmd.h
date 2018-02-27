#ifndef _INC_dev_cmd_H_
#define _INC_dev_cmd_H_

#define DEVCMD_DISABLE_TXBLOCK						1
#define DEVCMD_ENABLE_TXBLOCK							2
#define DEVCMD_DISABLE_RXBLOCK						3
#define DEVCMD_ENABLE_RXBLOCK							4
#define DEVCMD_SET_TXWAITTIME_MS				5
#define DEVCMD_SET_RXWAITTIME_MS				6


#define	DEVCMD_SET_IRQHDL								7
#define	DEVCMD_SET_RXIRQHDL							9
#define	DEVCMD_SET_TXIRQHDL							10
	


#define	DEVCMD_GET_RXBUFPTR							11
#define	DEVCMD_FREE_RXBUFPTR						12

#define	DEVCMD_DISABLE_IRQ							13
#define	DEVCMD_ENABLE_IRQ								14

#define DEVCMD_SET_ARGUMENT							15
#define DEVCMD_SET_BAUD										16			
#define	DEVGPIOCMD_SET_ENCODE						0x100

#endif
