#ifndef __DEVICEID_H_
#define __DEVICEID_H_

//字符型设备
#define DEVTYPE_CHAR				1


#define DEVMAJOR_UART				1


//显示设备
#define DEVTYPE_DISPLAY				2

#define DEVMAJOR_LCD				1


//设备ID 
// | 31 .. 24 	| 23 .. 16 	|	15 .. 8		| 7 .. 0 	|
// | 保留		| 设备类型	|	主设备号	| 次设备号	|


#define DEVID_RESERVE_BITS		8
#define DEVID_TYPE_BITS			8
#define DEVID_MAJOR_BITS		8
#define DEVID_MINOR_BITS		8
#define DEVID_TYPE_MASK			( ( 1<< DEVID_TYPE_BITS) -1)
#define DEVID_MAJOR_MASK		( ( 1<< DEVID_MAJOR_BITS) -1)
#define DEVID_MINOR_MASK		( ( 1<< DEVID_MINOR_BITS) -1)

#define	SET_TYPE( type) 				( ( ( type & DEVID_TYPE_MASK) << ( DEVID_MAJOR_BITS + DEVID_MINOR_BITS)) )
#define	SET_MAJOR( major) 				( ( ( major & DEVID_MAJOR_MASK) << ( DEVID_MINOR_BITS)) & DEVID_MAJOR_MASK)
#define	SET_MINOR( minor) 				(  ( minor & DEVID_MINOR_MASK))& DEVID_MINOR_MASK)

#define BUILD_DEVID( type, major, minor) ( SET_TYPE( type)  | SET_MAJOR( major)  | SET_MINOR( minor))

//#define DEVID_UART2  		BUILD_DEVID( DEVTYPE_CHAR, DEVMAJOR_UART, 1)
//#define DEVID_USARTGPULCD  	BUILD_DEVID( DEVTYPE_DISPLAY, DEVMAJOR_LCD, 1)
#define DEVID_UART2			0x00010101
#define DEVID_USARTGPULCD	0x00020101







#endif
