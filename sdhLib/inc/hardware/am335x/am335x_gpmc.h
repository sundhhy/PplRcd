/*
 * am335x_gpmc.h
 *
 *  Created on: 2016-6-30
 *      Author: Administrator
 */

#ifndef AM335X_GPMC_H_
#define AM335X_GPMC_H_
#include <stdint.h>
#include <arm/omap3530.h>
#include <arm/omap2420.h>
#include "lw_oopc.h"
#include "basis/sdhDebug.h"
#include "basis/sdhError.h"
#include "hd_conf_def.h"

//-- GPMC_CONFIG1
#define SET_DEVICE_TYPE(regval, setval)			\
	regval = ( ( regval & 0xfffff3ff ) | ( ( setval & 3) << 10))

#define SET_DEVICE_SIZE(regval, setval)			\
	regval = ( ( regval & 0xffffcfff ) | ( ( setval & 3) << 12))
#define SET_MUXADDDATA(regval, setval)			\
	regval = ( ( regval & 0xfffffcff ) | ( ( setval & 3) << 8))



#define SET_READMULTIPLE(regval, setval)			\
	regval = ( ( regval & 0xbfffffff ) | ( ( setval & 1) << 30))
#define SET_READTYPE(regval, setval)			\
	regval = ( ( regval & 0xdfffffff ) | ( ( setval & 1) << 29))
#define SET_WRITEMULTIPLE(regval, setval)			\
	regval = ( ( regval & 0xefffffff ) | ( ( setval & 1) << 28))
#define SET_WRITETYPE(regval, setval)			\
	regval = ( ( regval & 0xf7ffffff ) | ( ( setval & 1) << 27))
#define SET_WAITREADMONITORING(regval, setval)			\
	regval = ( ( regval & 0xffbfffff ) | ( ( setval & 0x1) << 22))
#define SET_WAITWRITEMONITORING(regval, setval)			\
	regval = ( ( regval & 0xffdfffff ) | ( ( setval & 0x1) << 21))
#define SET_WAITPINSELECT(regval, setval)			\
	regval = ( ( regval & 0xfffcffff ) | ( ( setval & 0x3) << 16))


// GPMC_CONFIG2
#define SET_CSWROFFTIME(regval, setval)			\
	regval = ( ( regval & 0xffe0ffff ) | ( ( setval & 0x1f) << 16))
#define SET_CSRDOFFTIME(regval, setval)			\
	regval = ( ( regval & 0xffffe0ff ) | ( ( setval & 0x1f) << 8))
#define SET_CSONTIME(regval, setval)			\
	regval = ( ( regval & 0xfffffff0 ) | ( ( setval & 0xf) << 0))

// GPMC_CONFIG4
#define SET_WEOFFTIME(regval, setval)			\
	regval = ( ( regval & 0xe0ffffff ) | ( ( setval & 0x1f) << 24))
#define SET_WEONTIME(regval, setval)			\
	regval = ( ( regval & 0xfff0ffff ) | ( ( setval & 0xf) << 16))
#define SET_OEOFFTIME(regval, setval)			\
	regval = ( ( regval & 0xffffe0ff ) | ( ( setval & 0x1f) << 8))
#define SET_OEONTIME(regval, setval)			\
	regval = ( ( regval & 0xfffffff0 ) | ( ( setval & 0xf) << 0))

// GPMC_CONFIG5

#define SET_RDACCESSTIME(regval, setval)			\
	regval = ( ( regval & 0xffe0ffff ) | ( ( setval & 0x1f) << 16))
#define SET_WRCYCLETIME(regval, setval)			\
	regval = ( ( regval & 0xffffe0ff ) | ( ( setval & 0x1f) << 8))
#define SET_RDCYCLETIME(regval, setval)			\
	regval = ( ( regval & 0xffffffe0 ) | ( ( setval & 0x1f) << 0))

//GPMC_CONFIG6
#define SET_WRACCESSTIME(regval, setval)			\
	regval = ( ( regval & 0xe0ffffff ) | ( ( setval & 0x1f) << 24))
#define SET_CYCLE2CYCLEDELAY(regval, setval)			\
	regval = ( ( regval & 0xfffff0ff ) | ( ( setval & 0xf) << 8))
#define SET_CYCLE2CYCLESAMECSEN(regval, setval)			\
	regval = ( ( regval & 0xffffff7f ) | ( ( setval & 0x1) << 7))
#define SET_CYCLE2CYCLEDIFFCSEN(regval, setval)			\
	regval = ( ( regval & 0xffffffdf ) | ( ( setval & 0x1) << 6))
#define SET_BUSTURNAROUND(regval, setval)			\
	regval = ( ( regval & 0xfffffff0 ) | ( ( setval & 0xf) << 0))

// GPMC_CONFIG7
#define SET_MASKADDRESS(regval, setval)			\
	regval = ( ( regval & 0xfffff0ff ) | ( ( setval & 0xf) << 8))
#define SET_BASEADDRESS(regval, setval)			\
	regval = ( ( regval & 0xffffffc0 ) | ( ( setval & 0x3f) << 0))
#define SET_CSVALID(regval, setval)			\
	regval = ( ( regval & 0xffffffbf ) | ( ( setval & 0x1) << 6))




#define DEVICE_SIZE_8		0
#define DEVICE_SIZE_16		1
#define NOR_FLASH_LIKE		0
#define	NON_MULTIPLEXED		0

#define READ_SIGNAL_ACCESS		0
#define READ_MULTIPLE_ACCESS	1
#define READ_ASYNCHRONOUS	0
#define READ_SYNCHRONOUS	1

#define WRITE_SIGNAL_ACCESS		0
#define WRITE_MULTIPLE_ACCESS	1
#define WRITE_ASYNCHRONOUS	0
#define WRITE_SYNCHRONOUS	1

#define MAX_MASKADDRESS		0x10000000   //芯片手册中的值，256M
#define MASKADDRESS_UNIT	0x1000000
#define GPMC_TICK_NS		10				//GPMC的FCLK的频率是100MHz，不分频的情况下，每个FCLK时钟滴答是10ns
//#define NS_TO_TICK( ns)    ( ns / GPMC_TICK_NS == 0)? 1: (ns / GPMC_TICK_NS)
#define NS_TO_TICK( ns)	(ns / GPMC_TICK_NS)
typedef uintptr_t SINGLETON;

CLASS(Drive_Gpmc)
{

//	IMPLEMENTS(IBusDrive);

	err_t 	(*init)(Drive_Gpmc * , void * );
	err_t	( *destory)( Drive_Gpmc *);

	//在多进程编程时，总线不必在每个进程都对总线进行初始化
	//对于不必初始化的进程，只需要映射下内存即可操作了
	err_t 	(*map)(Drive_Gpmc * , void * );

	err_t	(*write_u8)(Drive_Gpmc *, uint32_t , uint8_t );
	uint8_t	(*read_u8)(Drive_Gpmc *, uint32_t );

	err_t	(*write_u16)(Drive_Gpmc *, uint32_t , uint16_t );
	uint16_t	(*read_u16)(Drive_Gpmc *, uint32_t );

	SINGLETON			gpmc_vbase;
	volatile uint8_t	*pBusU8;
	volatile uint16_t	*pBusU16;
	volatile uint8_t	*map_io;			//外部设备的总线地址的虚拟地址
	gpmc_chip_cfg		*config;
	uint32_t			cs_regoffset;
//	uint16_t			rese;

};


//时序的配置参数。参考的是异步非复用的时序图来决定需要配置那些参数。
//参考的时序图是：spruh73m   Figure 7-24/25
typedef struct {

	char		cswr_offtime_ns;
	char		wr_offtime_ns;
//	bool		wr_extra_dealy;
	char		wr_ontime_ns;
	char		wr_cycletime_ns;			//写数据一直保持有效到这个时间
	char		wr_accesstime_ns;
	char		rese[3];
}gpmc_wr_timing;

typedef struct {

	short		csrd_offtime_ns;
	short		rd_offtime_ns;
//	bool		rd_extra_dealy;
	short		rd_ontime_ns;
	short		rd_cycletime_ns;			//写数据一直保持有效到这个时间
	short		rd_accesstime_ns;
	char		rese[2];
//	char		rese[3];
}gpmc_rd_timing;

typedef struct {


//	bool		cs_extra_dealy;			//cs信号额外的半个FCLK的延时
	char		cs_ontime_ns;
	char		cycle2cycleDelay_fclk;	//连续访问时的时间间隔 0.. fh Fclk cycles
	char		cycle2cycleSameCsen; //同一个片选的两次成功访问之间增加一个cycle2cycleDelay_fclk， 0或1
	char		cycle2cycleDiffCsen; //不同一个片选的两次成功访问之间增加一个cycle2cycleDelay_fclk， 0或1
	char		bursturnaround_fclk;		// 0.. fh Fclk cycles

}gpmc_common_timing;



typedef enum {
	//for drives management
	ERROR_GPMC_BEGIN = ERROR_BEGIN(DRIVE_GPMC),
	init_invail_instance,
	init_mapio_fail,






}err_gpmc_t;



#endif /* AM335X_GPMC_H_ */
