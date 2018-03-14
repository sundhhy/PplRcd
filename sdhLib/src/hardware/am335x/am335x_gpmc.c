/*
 * am335x_gpmc.c
 *
 *  Created on: 2016-6-30
 *      Author: Administrator
 */

#include "hardware_cfg.h"
#include <sys/mman.h>
#include "basis/sdhDebug.h"

#include "hardware/am335x/am335x_gpmc.h"
#include "hardware/am335x/am335x_gpio.h"
#include "hardware/am335x/pin_mux.h"
#include "hardware/am335x/am335x.h"


#include <hw/inout.h>


#define GPMC_RD32_REG(cthis, reg) in32(  cthis->gpmc_vbase + cthis->cs_regoffset + reg)
#define GPMC_WR32_REG(cthis, reg, val) out32(  cthis->gpmc_vbase + cthis->cs_regoffset + reg, val)
static char Vbase_reference = 0;
static SINGLETON Vbase = MAP_DEVICE_FAILED;


const uint16_t	Gpmc_config_offset[] = {
		OMAP2420_GPMC_CS0, OMAP2420_GPMC_CS1, OMAP2420_GPMC_CS2, OMAP2420_GPMC_CS3, OMAP2420_GPMC_CS4, OMAP2420_GPMC_CS5,\
		OMAP2420_GPMC_CS6,OMAP2420_GPMC_CS7
};


static SINGLETON get_single_vbase( void);
static SINGLETON free_single_vbase( void);

static void set_rdtiming( Drive_Gpmc *cthis, gpmc_rd_timing *timing_cfg);
static void set_wrtiming( Drive_Gpmc *cthis, gpmc_wr_timing *timing_cfg);
static void set_comtiming( Drive_Gpmc *cthis, gpmc_common_timing *timing_cfg);

static err_t gpmc_init(Drive_Gpmc *t, void *arg)
{
	Drive_Gpmc 		*cthis = ( Drive_Gpmc *)t ;
	uintptr_t		pAddr;


	uint32_t		tmp_reg;
//	cthis->config = &Gpmc_cfg_c2;


	cthis->cs_regoffset = Gpmc_config_offset[ cthis->config->chip_instance];

	cthis->gpmc_vbase =  get_single_vbase();
	if (cthis->gpmc_vbase == MAP_DEVICE_FAILED)
	{

		return ERROR_T( init_mapio_fail);
	}

	tmp_reg = GPMC_RD32_REG( cthis, OMAP2420_GPMC_CONFIG1);

	//配置成设备类型，总线宽度，复用类型，访问类型
	SET_DEVICE_TYPE( tmp_reg, cthis->config->device_type);
	SET_DEVICE_SIZE( tmp_reg, cthis->config->device_size);
	SET_MUXADDDATA( tmp_reg, cthis->config->mux_add_data);
//	SET_GPMCFCLKDIVIDER( tmp_reg, cthis->config->gpmc_fclk_divider);
//	SET_CLKACTIVATIONTIME( tmp_reg, cthis->config->clk_activation_time);
	SET_READMULTIPLE( tmp_reg, cthis->config->read_multiple);
	SET_READTYPE( tmp_reg, cthis->config->read_type);
	SET_WRITEMULTIPLE( tmp_reg, cthis->config->write_multiple);
	SET_WRITETYPE( tmp_reg, cthis->config->write_type);

	///< 设置wait引脚功能
	SET_WAITREADMONITORING( tmp_reg, cthis->config->rd_monitoring);
	SET_WAITWRITEMONITORING( tmp_reg, cthis->config->wr_monitoring);
	SET_WAITPINSELECT( tmp_reg, cthis->config->waitpin);
	GPMC_WR32_REG( cthis, OMAP2420_GPMC_CONFIG1, tmp_reg);

	set_comtiming( cthis, cthis->config->cmmTim);
	set_rdtiming( cthis, cthis->config->rdTim);
	set_wrtiming( cthis, cthis->config->wrTim);

	tmp_reg = GPMC_RD32_REG( cthis, OMAP2420_GPMC_CONFIG7);
	SET_MASKADDRESS( tmp_reg, ( MAX_MASKADDRESS - cthis->config->mask_address)/MASKADDRESS_UNIT );
	SET_BASEADDRESS( tmp_reg, (cthis->config->base_address)/MASKADDRESS_UNIT );
	SET_CSVALID( tmp_reg, 1);
	GPMC_WR32_REG( cthis, OMAP2420_GPMC_CONFIG7, tmp_reg);

	pAddr =  mmap_device_io( cthis->config->mmap_size, cthis->config->base_address);
	if ( pAddr == MAP_DEVICE_FAILED)
	{
		munmap_device_io( cthis->gpmc_vbase, OMAP3530_GPMC_SIZE);
		return ERROR_T( init_mapio_fail);
	}
	cthis->pBusU16 = ( volatile uint16_t *)pAddr;
	cthis->pBusU8 = ( volatile uint8_t *)pAddr;
	cthis->map_io = ( volatile uint8_t *)pAddr;

//	printf("GPMC_CONFIG :0x%04x\n", in32(  cthis->gpmc_vbase + OMAP2420_GPMC_CONFIG));
//	printf("GPMC_CONFIG1 :0x%04x\n",GPMC_RD32_REG( cthis, OMAP2420_GPMC_CONFIG1));
//	printf("GPMC_CONFIG2 :0x%04x\n",GPMC_RD32_REG( cthis, OMAP2420_GPMC_CONFIG2));
//	printf("GPMC_CONFIG3 :0x%04x\n",GPMC_RD32_REG( cthis, OMAP2420_GPMC_CONFIG3));
//	printf("GPMC_CONFIG4 :0x%04x\n",GPMC_RD32_REG( cthis, OMAP2420_GPMC_CONFIG4));
//	printf("GPMC_CONFIG5 :0x%04x\n",GPMC_RD32_REG( cthis, OMAP2420_GPMC_CONFIG5));
//	printf("GPMC_CONFIG6 :0x%04x\n",GPMC_RD32_REG( cthis, OMAP2420_GPMC_CONFIG6));
//	printf("GPMC_CONFIG7 :0x%04x\n",GPMC_RD32_REG( cthis, OMAP2420_GPMC_CONFIG7));


/*
 GPMC_CONFIG :0x0010
GPMC_CONFIG1 :0x0000
GPMC_CONFIG2 :0x60901
GPMC_CONFIG3 :0x0000
GPMC_CONFIG4 :0x6026812
GPMC_CONFIG5 :0x1080a0a
GPMC_CONFIG6 :0x87070000
GPMC_CONFIG7 :0x0f59

GPMC_CONFIG1 :0x0000
GPMC_CONFIG2 :0x60901
GPMC_CONFIG3 :0x0000
GPMC_CONFIG4 :0x6026812
GPMC_CONFIG5 :0x1080a0a
GPMC_CONFIG6 :0x87070000
GPMC_CONFIG7 :0x0f5a

*/
	return EXIT_SUCCESS;
}


static err_t Gpmc_map(Drive_Gpmc *t, void *arg)
{
	Drive_Gpmc 		*cthis = ( Drive_Gpmc *)t ;
	uintptr_t		pAddr;
	gpmc_chip_cfg 	*p_conf = (gpmc_chip_cfg 	*)arg;

	pAddr =  mmap_device_io(p_conf->mmap_size,p_conf->base_address);
	if ( pAddr == MAP_DEVICE_FAILED)
	{
		munmap_device_io(cthis->gpmc_vbase, OMAP3530_GPMC_SIZE);
		return ERROR_T( init_mapio_fail);
	}
	cthis->pBusU16 = ( volatile uint16_t *)pAddr;
	cthis->pBusU8 = ( volatile uint8_t *)pAddr;
	cthis->map_io = ( volatile uint8_t *)pAddr;


	return EXIT_SUCCESS;
}

static err_t gpmc_destory(Drive_Gpmc *t)
{
	Drive_Gpmc 		*cthis = ( Drive_Gpmc *)t ;
	munmap_device_io( ( uintptr_t )cthis->map_io, cthis->config->mmap_size);
	free_single_vbase();
	lw_oopc_delete( cthis);
	return EXIT_SUCCESS;
}

static err_t gpmc_write_u8( Drive_Gpmc  *t,  uint32_t addr, uint8_t data)
{
	Drive_Gpmc 		*cthis = ( Drive_Gpmc *)t ;
	* ( cthis->map_io + addr) = data;
	return EXIT_SUCCESS;

}

static uint8_t gpmc_read_u8( Drive_Gpmc  *t, uint32_t addr)
{
	Drive_Gpmc 		*cthis = ( Drive_Gpmc *)t ;
	return *( cthis->map_io + addr);
}


static err_t gpmc_write_u16( Drive_Gpmc  *t,  uint32_t addr, uint16_t data)
{
	Drive_Gpmc 		*cthis = ( Drive_Gpmc *)t ;
#ifdef GPMC_PRINT
	TRACE_INFO("Drive Piling :%s-%s-%d ", __FILE__, __func__, __LINE__);
	switch( data)
	{
		case 0x20:
			TRACE_INFO(" write data: ENC624J600_CMD_RCRU \r\n");
			break;
		case 0x22:
			TRACE_INFO(" write data: ENC624J600_CMD_WCRU \r\n");
			break;
		default:
			TRACE_INFO(" write data: 0x%x \r\n", data);
			break;

	}

	return EXIT_SUCCESS;
#else
	* ( cthis->pBusU16 + ( addr >> 1)) = data;
	return EXIT_SUCCESS;
#endif

}

static uint16_t gpmc_read_u16( Drive_Gpmc  *t, uint32_t addr)
{
	Drive_Gpmc 		*cthis = ( Drive_Gpmc *)t ;
#ifdef GPMC_PRINT
	TRACE_INFO("Drive Piling :%s-%s-%d \r\n", __FILE__, __func__, __LINE__);
	return EXIT_SUCCESS;
#else
	return *( cthis->pBusU16 + ( addr >> 1));
#endif

}






//实现GPMC映射的单例
static SINGLETON get_single_vbase( void)
{


	if( Vbase == MAP_DEVICE_FAILED)
	{
		Vbase = mmap_device_io(OMAP3530_GPMC_SIZE, GPMC_BASE);
	}
	Vbase_reference ++;
	return Vbase;
}

static SINGLETON free_single_vbase( void)
{
	Vbase_reference --;
	if( Vbase_reference == 0)
	{
		munmap_device_io( Vbase, OMAP3530_GPMC_SIZE);
	}

	return EXIT_SUCCESS;
}

static void set_wrtiming( Drive_Gpmc *cthis, gpmc_wr_timing *timing_cfg)
{
	uint32_t		tmp_reg;

	tmp_reg = GPMC_RD32_REG( cthis, OMAP2420_GPMC_CONFIG2);
	SET_CSWROFFTIME( tmp_reg, NS_TO_TICK( timing_cfg->cswr_offtime_ns));
	GPMC_WR32_REG( cthis, OMAP2420_GPMC_CONFIG2, tmp_reg);


	tmp_reg =  GPMC_RD32_REG( cthis, OMAP2420_GPMC_CONFIG4);
	SET_WEOFFTIME( tmp_reg, NS_TO_TICK( timing_cfg->wr_offtime_ns));
	SET_WEONTIME( tmp_reg, NS_TO_TICK( timing_cfg->wr_ontime_ns));
	GPMC_WR32_REG( cthis, OMAP2420_GPMC_CONFIG4, tmp_reg);

	tmp_reg = GPMC_RD32_REG( cthis, OMAP2420_GPMC_CONFIG5);
	SET_WRCYCLETIME( tmp_reg, NS_TO_TICK( timing_cfg->wr_cycletime_ns));
	GPMC_WR32_REG( cthis, OMAP2420_GPMC_CONFIG5, tmp_reg);

	tmp_reg =  GPMC_RD32_REG( cthis, OMAP2420_GPMC_CONFIG6);
	SET_WRACCESSTIME( tmp_reg, NS_TO_TICK( timing_cfg->wr_accesstime_ns));
	GPMC_WR32_REG( cthis, OMAP2420_GPMC_CONFIG6, tmp_reg);
}


static void set_rdtiming( Drive_Gpmc *cthis, gpmc_rd_timing *timing_cfg)
{
	uint32_t		tmp_reg;

	tmp_reg = GPMC_RD32_REG( cthis, OMAP2420_GPMC_CONFIG2);
	SET_CSRDOFFTIME( tmp_reg, NS_TO_TICK( timing_cfg->csrd_offtime_ns));
	GPMC_WR32_REG( cthis, OMAP2420_GPMC_CONFIG2, tmp_reg);


	tmp_reg = GPMC_RD32_REG( cthis, OMAP2420_GPMC_CONFIG4);
	SET_OEOFFTIME( tmp_reg, NS_TO_TICK( timing_cfg->rd_offtime_ns));
	SET_OEONTIME( tmp_reg, NS_TO_TICK( timing_cfg->rd_ontime_ns));
	GPMC_WR32_REG( cthis, OMAP2420_GPMC_CONFIG4, tmp_reg);

	tmp_reg = GPMC_RD32_REG( cthis, OMAP2420_GPMC_CONFIG5);
	SET_RDCYCLETIME( tmp_reg, NS_TO_TICK( timing_cfg->rd_cycletime_ns));
	SET_RDACCESSTIME( tmp_reg, NS_TO_TICK( timing_cfg->rd_accesstime_ns));
	GPMC_WR32_REG( cthis, OMAP2420_GPMC_CONFIG5, tmp_reg);
}

static void set_comtiming( Drive_Gpmc *cthis, gpmc_common_timing *timing_cfg)
{
	uint32_t		tmp_reg;

	tmp_reg = GPMC_RD32_REG( cthis, OMAP2420_GPMC_CONFIG2);
	SET_CSONTIME( tmp_reg, NS_TO_TICK( timing_cfg->cs_ontime_ns));
	GPMC_WR32_REG( cthis, OMAP2420_GPMC_CONFIG2, tmp_reg);




	tmp_reg = GPMC_RD32_REG( cthis, OMAP2420_GPMC_CONFIG6);
	SET_CYCLE2CYCLEDELAY( tmp_reg,  timing_cfg->cycle2cycleDelay_fclk);
	SET_CYCLE2CYCLESAMECSEN( tmp_reg,  timing_cfg->cycle2cycleSameCsen);
	SET_CYCLE2CYCLESAMECSEN( tmp_reg, timing_cfg->cycle2cycleDiffCsen);
	SET_BUSTURNAROUND( tmp_reg, timing_cfg->bursturnaround_fclk);
	GPMC_WR32_REG( cthis, OMAP2420_GPMC_CONFIG6, tmp_reg);

	GPMC_WR32_REG( cthis, OMAP2420_GPMC_CONFIG3, 0);		//ADV不需要
}










CTOR(Drive_Gpmc)

FUNCTION_SETTING(init, gpmc_init);
FUNCTION_SETTING(destory, gpmc_destory);
FUNCTION_SETTING(map, Gpmc_map);


FUNCTION_SETTING(write_u8, gpmc_write_u8);
FUNCTION_SETTING(read_u8, gpmc_read_u8);
FUNCTION_SETTING(write_u16, gpmc_write_u16);
FUNCTION_SETTING(read_u16, gpmc_read_u16);

END_CTOR
