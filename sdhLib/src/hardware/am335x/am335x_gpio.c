/*
 * am335x_gpio.c
 *
 *  Created on: 2016-7-4
 *      Author: Administrator
 */

#include "hardware/am335x/am335x_gpio.h"
#include "basis/sdhDebug.h"
#include <sys/mman.h>
#include <hw/inout.h>
#include <assert.h>
#include <atomic.h>


#include "hardware/am335x/hw_cm_wkup.h"
#include "hardware/am335x/hw_cm_per.h"
#include "hardware/am335x/pin_mux.h"
#include <errno.h>

#define INTR_THREAD			1   //用线程来处理中断
//#define DEBUG_GPIO

void GPIO0ModuleClkConfig(void);
void GPIO1ModuleClkConfig(void);
void GPIO2ModuleClkConfig(void);
void GPIO3ModuleClkConfig(void);
void GPIO4ModuleClkConfig(void);

void GPIOModuleEnable(uintptr_t baseAdd);
void GPIOModuleDisable(uintptr_t baseAdd);

static void *Intr_thread(void *arg);

static void (*gpioModuleConfig[4])(void) =
		{
				GPIO0ModuleClkConfig, GPIO1ModuleClkConfig, GPIO2ModuleClkConfig, GPIO3ModuleClkConfig
		};
static char	group_flag[4] = {0};
//GPIOINT0..3
const uint8_t GpioIntNum[2][4] =
{
		{ 96, 98, 32, 62,},			//intr line 1
		{ 97, 99, 33, 63,},			//intr line 2
};

const uint32_t	GpioBase[4] =
{ AM335X_GPIO0_BASE, AM335X_GPIO1_BASE, AM335X_GPIO2_BASE, AM335X_GPIO3_BASE};

uintptr_t arr_gpioVbase[4] = { MAP_DEVICE_FAILED};

static err_t gpio_init(Drive_Gpio *t)
{
	Drive_Gpio 		*cthis = ( Drive_Gpio *)t ;
	uint32_t		tmp_reg = 0;
	gpio_cfg 		*config = cthis->config;
	uintptr_t	init_crtl;
	int 			intrNum;
	int 			i = 0;

	 assert( config->intr_line < 2);


	config_pin( config->pin_ctrl_off, MODE(7), 0x30);
	cthis->gpio_vbase = mmap_device_io( AM335X_GPIO_SIZE, GpioBase[ config->pin_group]);
#ifdef DEBUG_GPIO
	TRACE_INFO("Drive Piling :%s-%s-%d \r\n", __FILE__, __func__, __LINE__);
	TRACE_INFO("pin config 0x%04x \r\n", rd_pin_conf( config->pin_ctrl_off));
	return EXIT_SUCCESS;
#else
	if (cthis->gpio_vbase == MAP_DEVICE_FAILED)
	{

		return ERROR_T( gpio_init_mapio_fail);
	}


	//todo:应该把这两句放到其他位置，不然每次初始化一个已经都会把GPIO模块重新初始化一次
	if(config->clk_init) {
		gpioModuleConfig[ config->pin_group]();
		GPIOModuleEnable( cthis->gpio_vbase);
		group_flag[config->pin_group] = 1;
	}


	//配置引脚方向为输入
	tmp_reg = in32( cthis->gpio_vbase + GPIO_OE);
	tmp_reg |= 1 << config->pin_number;
	out32( cthis->gpio_vbase + GPIO_OE, tmp_reg );


	//使能消抖
	if(config->debou_time) {
		tmp_reg=in32( cthis->gpio_vbase + GPIO_DEBOUNCENABLE);
		tmp_reg &=~( 1 << config->pin_number);
		tmp_reg |= ( 1 << config->pin_number);
		out32( cthis->gpio_vbase + GPIO_DEBOUNCENABLE, tmp_reg);

		//消抖时间
		out32(cthis->gpio_vbase + GPIO_DEBOUNCINGTIME, ( config->debou_time ));
	}

	//配置中断监测类型
	for( i = 0; i < 4; i ++)
	{
		if( ( ( 1 << i) & config->intr_type) )
		{
			//使能该类型
			tmp_reg = in32( cthis->gpio_vbase + GPIO_DETECT(i));
			tmp_reg |= 1 << config->pin_number;
			out32(cthis->gpio_vbase + GPIO_DETECT(i), tmp_reg);
		}
		else
		{
			//
			tmp_reg = in32( cthis->gpio_vbase + GPIO_DETECT(i));
			tmp_reg &= ~(1 << config->pin_number);
			out32(cthis->gpio_vbase + GPIO_DETECT(i), tmp_reg);
		}
	}


//	if(config->clk_init)
	{
		intrNum = GpioIntNum[ config->intr_line][ config->pin_group];
		init_crtl = mmap_device_io(0x1000,SOC_AINTC_REGS);//映射中断控制器
		out32(init_crtl+INTC_ILR( intrNum),(( config->intrPrio << 0x02 )& INTC_ILR_PRIORITY)|0);//设置优先级
		out32(init_crtl+INTC_MIR_CLEAR( intrNum >>5),(1<<(intrNum&0x1F)));//使能GPIO0端口中断
		munmap_device_io(init_crtl, 0x1000);
	}


#ifdef DEBUG_GPIO
	dump_gpio_reg( cthis->gpio_vbase);
#endif
	SIGEV_INTR_INIT(&cthis->isr_event );

#if INTR_THREAD == 1
	pthread_create (&cthis->pid, NULL, Intr_thread, cthis);
#else
	cthis->irq_id = InterruptAttach_r ( GpioIntNum[ config->intr_line][ config->pin_group], gpioExtInteIsr, cthis, 1, _NTO_INTR_FLAGS_END );
#endif
	return EXIT_SUCCESS;

#endif
}

int InitOutputPin(gpio_cfg *p_cfg)
{
	uintptr_t			gpio_vbase;
	uint32_t			tmp_reg = 0;

	config_pin( p_cfg->pin_ctrl_off, MODE(7), 0x10);
	gpio_vbase = mmap_device_io( AM335X_GPIO_SIZE, GpioBase[ p_cfg->pin_group]);
	if ( gpio_vbase == MAP_DEVICE_FAILED)
	{

		return ERROR_T( gpio_init_mapio_fail);
	}

	arr_gpioVbase[ p_cfg->pin_group] = gpio_vbase;

	//配置引脚方向为输出
	tmp_reg = in32( gpio_vbase + GPIO_OE );
	tmp_reg &= ~( 1 << p_cfg->pin_number);
	out32( gpio_vbase + GPIO_OE, tmp_reg );

	return EXIT_SUCCESS;

}

int PinOutput(gpio_cfg *p_cfg, char val)
{
	uintptr_t			gpio_vbase;
	uint32_t			tmp_reg = 0;
	gpio_vbase = arr_gpioVbase[ p_cfg->pin_group];
	if ( gpio_vbase == MAP_DEVICE_FAILED)
	{

		return ERROR_T( gpio_init_mapio_fail);
	}


	val &= 1;
	tmp_reg = in32( gpio_vbase + GPIO_DATAOUT );
	tmp_reg &= ~(1 << p_cfg->pin_number);
	tmp_reg |=  val << p_cfg->pin_number;
	out32(gpio_vbase + GPIO_DATAOUT, tmp_reg );

	return EXIT_SUCCESS;

}

int DestoryOutputPin(gpio_cfg *p_cfg)
{
	uintptr_t			gpio_vbase;

	gpio_vbase = arr_gpioVbase[ p_cfg->pin_group];
	if ( gpio_vbase != MAP_DEVICE_FAILED)
	{
		munmap_device_io(gpio_vbase,  AM335X_GPIO_SIZE);
		arr_gpioVbase[ p_cfg->pin_group] = MAP_DEVICE_FAILED;
	}




	return EXIT_SUCCESS;

}

static err_t gpio_enableIrq(Drive_Gpio *t)
{
	Drive_Gpio 		*cthis = ( Drive_Gpio *)t ;
	uint32_t		tmp_reg = 0;
#ifdef DEBUG_GPIO
	TRACE_INFO("Drive Piling :%s-%s-%d \r\n", __FILE__, __func__, __LINE__);
	return EXIT_SUCCESS;
#else
	tmp_reg = in32( cthis->gpio_vbase + GPIO_IRQSTATUS_SET( cthis->config->intr_line));
	tmp_reg |= 1 << cthis->config->pin_number;
	out32(cthis->gpio_vbase + GPIO_IRQSTATUS_SET( cthis->config->intr_line), tmp_reg);
	return EXIT_SUCCESS;

#endif
}

static err_t gpio_disableIrq(Drive_Gpio *t)
{
	Drive_Gpio 		*cthis = ( Drive_Gpio *)t ;
	uint32_t		tmp_reg = 0;
#ifdef DEBUG_GPIO
	TRACE_INFO("Drive Piling :%s-%s-%d \r\n", __FILE__, __func__, __LINE__);
	return EXIT_SUCCESS;
#else
	tmp_reg = in32( cthis->gpio_vbase + GPIO_IRQSTATUS_CLR( cthis->config->intr_line));
	tmp_reg &= ( 1 << cthis->config->pin_number);
	out32(cthis->gpio_vbase + GPIO_IRQSTATUS_CLR( cthis->config->intr_line), tmp_reg);
	return EXIT_SUCCESS;

#endif
}
static err_t gpio_destory(Drive_Gpio *t)
{
	Drive_Gpio 		*cthis = ( Drive_Gpio *)t ;

	cthis->disableIrq( cthis);
	GPIOModuleDisable( cthis->gpio_vbase);
	InterruptDetach( cthis->irq_id);
	munmap_device_io( cthis->gpio_vbase, AM335X_GPIO_SIZE);
	lw_oopc_delete( cthis);
	return EXIT_SUCCESS;

}

const struct sigevent *gpioExtInteIsr (void *area, int id)
{

	Drive_Gpio 		*cthis = ( Drive_Gpio *)area ;
	uint32_t stats;
	stats = in32( cthis->gpio_vbase + GPIO_GPIO_IRQSTATUS( cthis->config->intr_line));
//	out32( cthis->gpio_vbase + GPIO_GPIO_IRQSTATUS( cthis->config->intr_line), stats);

//	cthis->states = stats;
	if( stats & ( 1<< cthis->config->pin_number))
	{
//		out32( cthis->gpio_vbase + GPIO_GPIO_IRQSTATUS( cthis->config->intr_line), 1 << cthis->config->pin_number);
//		stats &= ~( 1<< cthis->config->pin_number);
//		out32( cthis->gpio_vbase + GPIO_IRQSTATUS_RAW( cthis->config->intr_line), stats);
#if INTR_THREAD == 0
		cthis->irq_handle( cthis->irq_handle_arg);
#endif
//		Dubug_info.irq_count[ cthis->config->instance] ++;

		atomic_set(&cthis->states, 1);

		out32( cthis->gpio_vbase + GPIO_GPIO_IRQSTATUS( cthis->config->intr_line), 1 << cthis->config->pin_number);


	}

//	out32( cthis->gpio_vbase + GPIO_GPIO_IRQSTATUS( cthis->config->intr_line), stats);
	return ( &cthis->isr_event);
	return NULL;

}

static void *Intr_thread(void *arg)
{
	Drive_Gpio 		*cthis = ( Drive_Gpio *)arg ;
//	gpio_cfg 		*config = cthis->config;
//	uint32_t 		reg_val = 0;
	cthis->irq_id = InterruptAttach_r (GpioIntNum[cthis->config->intr_line][cthis->config->pin_group], gpioExtInteIsr, cthis, 1, _NTO_INTR_FLAGS_END );

	pthread_detach(pthread_self());
	while(1) {
		InterruptWait (NULL, NULL);
		if( cthis->states) {
			atomic_clr(&cthis->states, 1);

			cthis->irq_handle( cthis->irq_handle_arg);
		}
//		else
//		{
//			//有时候会漏中断
//			reg_val = in32(cthis->gpio_vbase + GPIO_DATAOUT);
//			reg_val = reg_val & ( 1<< cthis->config->pin_number);
//
//			if(cthis->config->intr_type & RISINGDETECT) {
//				if(reg_val)
//					cthis->irq_handle( cthis->irq_handle_arg);
//			}
//			else if(cthis->config->intr_type & FALLINGDETECT) {
//				if(reg_val == 0)
//					cthis->irq_handle( cthis->irq_handle_arg);
//			}
//		}

	}

	return NULL;
}


void dump_gpio_reg( uintptr_t baseAdd)
{
//	TRACE_INFO("GPIO_REVISION 0x%04x \r\n", in32( baseAdd + GPIO_REVISION ));
//	TRACE_INFO("GPIO_SYSCONFIG 0x%04x \r\n", in32( baseAdd + GPIO_SYSCONFIG ));
//	TRACE_INFO("GPIO_EOI 0x%04x \r\n", in32( baseAdd + GPIO_EOI ));
	TRACE_INFO("GPIO_IRQSTATUS_RAW_0 0x%04x \r\n", in32( baseAdd + GPIO_IRQSTATUS_RAW_0 ));
	TRACE_INFO("GPIO_IRQSTATUS_RAW_1 0x%04x \r\n", in32( baseAdd + GPIO_IRQSTATUS_RAW_1 ));
	TRACE_INFO("GPIO_IRQSTATUS_0 0x%04x \r\n", in32( baseAdd + GPIO_IRQSTATUS_0 ));
	TRACE_INFO("GPIO_IRQSTATUS_1 0x%04x \r\n", in32( baseAdd + GPIO_IRQSTATUS_1 ));
	TRACE_INFO("GPIO_IRQSTATUS_SET_0 0x%04x \r\n", in32( baseAdd + GPIO_IRQSTATUS_SET_0 ));
	TRACE_INFO("GPIO_IRQSTATUS_SET_1 0x%04x \r\n", in32( baseAdd + GPIO_IRQSTATUS_SET_1 ));
	TRACE_INFO("GPIO_IRQSTATUS_CLR_0 0x%04x \r\n", in32( baseAdd + GPIO_IRQSTATUS_CLR_0 ));
	TRACE_INFO("GPIO_IRQSTATUS_CLR_1 0x%04x \r\n", in32( baseAdd + GPIO_IRQSTATUS_CLR_1 ));
//	TRACE_INFO("GPIO_SYSSTATUS 0x%04x \r\n", in32( baseAdd + GPIO_SYSSTATUS ));
//	TRACE_INFO("GPIO_CTRL 0x%04x \r\n", in32( baseAdd + GPIO_CTRL ));
//	TRACE_INFO("GPIO_OE 0x%04x \r\n", in32( baseAdd + GPIO_OE ));
	TRACE_INFO("GPIO_DATAIN 0x%04x \r\n", in32( baseAdd + GPIO_DATAIN ));
//	TRACE_INFO("GPIO_DATAOUT 0x%04x \r\n", in32( baseAdd + GPIO_DATAOUT ));
//	TRACE_INFO("GPIO_LEVELDETECT0 0x%04x \r\n", in32( baseAdd + GPIO_LEVELDETECT0 ));
//	TRACE_INFO("GPIO_LEVELDETECT1 0x%04x \r\n", in32( baseAdd + GPIO_LEVELDETECT1 ));
//	TRACE_INFO("GPIO_RISINGDETECT 0x%04x \r\n", in32( baseAdd + GPIO_RISINGDETECT ));
//	TRACE_INFO("GPIO_FALLINGDETECT 0x%04x \r\n", in32( baseAdd + GPIO_FALLINGDETECT ));
//	TRACE_INFO("GPIO_DEBOUNCENABLE 0x%04x \r\n", in32( baseAdd + GPIO_DEBOUNCENABLE ));
//	TRACE_INFO("GPIO_DEBOUNCINGTIME 0x%04x \r\n", in32( baseAdd + GPIO_DEBOUNCINGTIME ));
//	TRACE_INFO("GPIO_CLEARDATAOUT 0x%04x \r\n", in32( baseAdd + GPIO_CLEARDATAOUT ));
//	TRACE_INFO("GPIO_SETDATAOUT 0x%04x \r\n", in32( baseAdd + GPIO_SETDATAOUT ));
}

#define GPIO_CTRL_DISABLEMODULE   (0x00000001u)
//#define CM_PER_GPIO1_CLKCTRL		(0xAC)	// This register manages the GPIO1 clocks. Section 8.1.2.1.41
//#define CM_PER_GPIO2_CLKCTRL		(0xB0)	// This register manages the GPIO2 clocks. Section 8.1.2.1.42
//#define CM_PER_GPIO3_CLKCTRL		(0xB4)	// This register manages the GPIO3 clocks. Section 8.1.2.1.43
//#define CM_PER_GPIO4_CLKCTRL		(0xB8)	// This register manages the GPIO4 clocks. Section 8.1.2.1.44
void GPIO0ModuleClkConfig(void)
{
	static char invo_count = 0;
	uintptr_t prcm_base,cm_wkup_regs;
	uint32_t	val;

	if( invo_count)	//第二次调用直接退出
		return;
	invo_count ++;

	prcm_base=mmap_device_io(CM_PRCM_SIZE,PRCM_BASE);
	cm_wkup_regs=prcm_base+0x400;
	/* Writing to MODULEMODE field of CM_WKUP_GPIO0_CLKCTRL register. */
    out32(cm_wkup_regs+CM_WKUP_GPIO0_CLKCTRL,CM_WKUP_GPIO0_CLKCTRL_MODULEMODE_ENABLE);

    /* Waiting for MODULEMODE field to reflect the written value. */
    while(CM_WKUP_GPIO0_CLKCTRL_MODULEMODE_ENABLE !=
          (in32(cm_wkup_regs + CM_WKUP_GPIO0_CLKCTRL) &
           CM_WKUP_GPIO0_CLKCTRL_MODULEMODE));

    /*
    ** Writing to OPTFCLKEN_GPIO0_GDBCLK field of CM_WKUP_GPIO0_CLKCTRL
    ** register.
    */
    val=in32(cm_wkup_regs + CM_WKUP_GPIO0_CLKCTRL);
    val|=CM_WKUP_GPIO0_CLKCTRL_OPTFCLKEN_GPIO0_GDBCLK;
    out32(cm_wkup_regs + CM_WKUP_GPIO0_CLKCTRL,val);

    /* Waiting for OPTFCLKEN_GPIO0_GDBCLK field to reflect the written value. */
    while(CM_WKUP_GPIO0_CLKCTRL_OPTFCLKEN_GPIO0_GDBCLK !=
          (in32(cm_wkup_regs + CM_WKUP_GPIO0_CLKCTRL) &
           CM_WKUP_GPIO0_CLKCTRL_OPTFCLKEN_GPIO0_GDBCLK));

    /* Verifying if the other bits are set to required settings. */

    /*
    ** Waiting for IDLEST field in CM_WKUP_CONTROL_CLKCTRL register to attain
    ** desired value.
    */
    while((CM_WKUP_CONTROL_CLKCTRL_IDLEST_FUNC <<
           CM_WKUP_CONTROL_CLKCTRL_IDLEST_SHIFT) !=
          (in32(cm_wkup_regs + CM_WKUP_CONTROL_CLKCTRL) &
           CM_WKUP_CONTROL_CLKCTRL_IDLEST));

    /*
    ** Waiting for CLKACTIVITY_L3_AON_GCLK field in CM_L3_AON_CLKSTCTRL
    ** register to attain desired value.
    */
    while(CM_WKUP_CM_L3_AON_CLKSTCTRL_CLKACTIVITY_L3_AON_GCLK !=
          (in32(cm_wkup_regs + CM_WKUP_CM_L3_AON_CLKSTCTRL) &
           CM_WKUP_CM_L3_AON_CLKSTCTRL_CLKACTIVITY_L3_AON_GCLK));

    /*
    ** Waiting for IDLEST field in CM_WKUP_L4WKUP_CLKCTRL register to attain
    ** desired value.
    */
    while((CM_WKUP_L4WKUP_CLKCTRL_IDLEST_FUNC <<
           CM_WKUP_L4WKUP_CLKCTRL_IDLEST_SHIFT) !=
          (in32(cm_wkup_regs + CM_WKUP_L4WKUP_CLKCTRL) &
           CM_WKUP_L4WKUP_CLKCTRL_IDLEST));

    /*
    ** Waiting for CLKACTIVITY_L4_WKUP_GCLK field in CM_WKUP_CLKSTCTRL register
    ** to attain desired value.
    */
    while(CM_WKUP_CLKSTCTRL_CLKACTIVITY_L4_WKUP_GCLK !=
          (in32(cm_wkup_regs + CM_WKUP_CLKSTCTRL) &
           CM_WKUP_CLKSTCTRL_CLKACTIVITY_L4_WKUP_GCLK));

    /*
    ** Waiting for CLKACTIVITY_L4_WKUP_AON_GCLK field in CM_L4_WKUP_AON_CLKSTCTRL
    ** register to attain desired value.
    */
    while(CM_WKUP_CM_L4_WKUP_AON_CLKSTCTRL_CLKACTIVITY_L4_WKUP_AON_GCLK !=
          (in32(cm_wkup_regs + CM_WKUP_CM_L4_WKUP_AON_CLKSTCTRL) &
           CM_WKUP_CM_L4_WKUP_AON_CLKSTCTRL_CLKACTIVITY_L4_WKUP_AON_GCLK));


    /* Writing to IDLEST field in CM_WKUP_GPIO0_CLKCTRL register. */
    while((CM_WKUP_GPIO0_CLKCTRL_IDLEST_FUNC <<
           CM_WKUP_GPIO0_CLKCTRL_IDLEST_SHIFT) !=
          (in32(cm_wkup_regs + CM_WKUP_GPIO0_CLKCTRL) &
           CM_WKUP_GPIO0_CLKCTRL_IDLEST));

    /*
    ** Waiting for CLKACTIVITY_GPIO0_GDBCLK field in CM_WKUP_GPIO0_CLKCTRL
    ** register to attain desired value.
    */
    while(CM_WKUP_CLKSTCTRL_CLKACTIVITY_GPIO0_GDBCLK !=
          (in32(cm_wkup_regs + CM_WKUP_CLKSTCTRL) &
           CM_WKUP_CLKSTCTRL_CLKACTIVITY_GPIO0_GDBCLK));
    munmap_device_io(prcm_base, CM_PRCM_SIZE);

}

void GPIO1ModuleClkConfig(void)
{
	static char invo_count = 0;
	uintptr_t prcm_base;
	uint32_t	val;

	if( invo_count)	//第二次调用直接退出
		return;
	invo_count ++;

	prcm_base=mmap_device_io(CM_PRCM_SIZE,PRCM_BASE);

	/* Writing to MODULEMODE field of CM_WKUP_GPIO0_CLKCTRL register. */
	out32(prcm_base+CM_PER_GPIO1_CLKCTRL,CM_PER_GPIO1_CLKCTRL_MODULEMODE_ENABLE);

	/* Waiting for MODULEMODE field to reflect the written value. */
	while(CM_PER_GPIO1_CLKCTRL_MODULEMODE_ENABLE !=
		  (in32(prcm_base + CM_PER_GPIO1_CLKCTRL) &
				  CM_PER_GPIO1_CLKCTRL_MODULEMODE_ENABLE));

	/*
	** Writing to OPTFCLKEN_GPIO0_GDBCLK field of CM_WKUP_GPIO0_CLKCTRL
	** register.
	*/
	val=in32(prcm_base + CM_PER_GPIO1_CLKCTRL);
	val|=CM_PER_GPIO1_CLKCTRL_OPTFCLKEN_GPIO_1_GDBCLK;
	out32(prcm_base + CM_PER_GPIO1_CLKCTRL,val);

	/* Waiting for OPTFCLKEN_GPIO0_GDBCLK field to reflect the written value. */
	while(CM_PER_GPIO1_CLKCTRL_OPTFCLKEN_GPIO_1_GDBCLK !=
		  (in32( prcm_base + CM_PER_GPIO1_CLKCTRL) &
				  CM_PER_GPIO1_CLKCTRL_OPTFCLKEN_GPIO_1_GDBCLK));


}

void GPIO2ModuleClkConfig(void)
{
	//todo

	static char invo_count = 0;
	uintptr_t prcm_base;
	uint32_t	val;

	if( invo_count)	//第二次调用直接退出
		return;
	invo_count ++;

	prcm_base=mmap_device_io(CM_PRCM_SIZE,PRCM_BASE);

	/* Writing to MODULEMODE field of CM_WKUP_GPIO0_CLKCTRL register. */
	out32(prcm_base+CM_PER_GPIO2_CLKCTRL,CM_PER_GPIO2_CLKCTRL_MODULEMODE_ENABLE);

	/* Waiting for MODULEMODE field to reflect the written value. */
	while(CM_PER_GPIO2_CLKCTRL_MODULEMODE_ENABLE !=
		  (in32(prcm_base + CM_PER_GPIO2_CLKCTRL) &
				  CM_PER_GPIO2_CLKCTRL_MODULEMODE_ENABLE));

	/*
	** Writing to OPTFCLKEN_GPIO0_GDBCLK field of CM_WKUP_GPIO0_CLKCTRL
	** register.
	*/
	val=in32(prcm_base + CM_PER_GPIO2_CLKCTRL);
	val|=CM_PER_GPIO2_CLKCTRL_OPTFCLKEN_GPIO_2_GDBCLK;
	out32(prcm_base + CM_PER_GPIO2_CLKCTRL,val);

	/* Waiting for OPTFCLKEN_GPIO0_GDBCLK field to reflect the written value. */
	while(CM_PER_GPIO2_CLKCTRL_OPTFCLKEN_GPIO_2_GDBCLK !=
		  (in32( prcm_base + CM_PER_GPIO2_CLKCTRL) &
				  CM_PER_GPIO2_CLKCTRL_OPTFCLKEN_GPIO_2_GDBCLK));
}


void GPIO3ModuleClkConfig(void)
{
	static char invo_count = 0;
	uintptr_t prcm_base;
	uint32_t	val;

	if( invo_count)	//第二次调用直接退出
		return;
	invo_count ++;

	prcm_base=mmap_device_io(CM_PRCM_SIZE,PRCM_BASE);

	/* Writing to MODULEMODE field of CM_WKUP_GPIO0_CLKCTRL register. */
	out32(prcm_base+CM_PER_GPIO3_CLKCTRL,CM_PER_GPIO3_CLKCTRL_MODULEMODE_ENABLE);

	/* Waiting for MODULEMODE field to reflect the written value. */
	while(CM_PER_GPIO3_CLKCTRL_MODULEMODE_ENABLE !=
		  (in32(prcm_base + CM_PER_GPIO3_CLKCTRL) &
				  CM_PER_GPIO3_CLKCTRL_MODULEMODE_ENABLE));

	/*
	** Writing to OPTFCLKEN_GPIO0_GDBCLK field of CM_WKUP_GPIO0_CLKCTRL
	** register.
	*/
	val=in32(prcm_base + CM_PER_GPIO3_CLKCTRL);
	val|=CM_PER_GPIO3_CLKCTRL_OPTFCLKEN_GPIO_3_GDBCLK;
	out32(prcm_base + CM_PER_GPIO3_CLKCTRL,val);

	/* Waiting for OPTFCLKEN_GPIO0_GDBCLK field to reflect the written value. */
	while(CM_PER_GPIO3_CLKCTRL_OPTFCLKEN_GPIO_3_GDBCLK !=
		  (in32( prcm_base + CM_PER_GPIO3_CLKCTRL) &
				  CM_PER_GPIO3_CLKCTRL_OPTFCLKEN_GPIO_3_GDBCLK));




}

void GPIO4ModuleClkConfig(void)
{
	//todo
}
/**
 * \brief  This API is used to enable the GPIO module. When the GPIO module
 *         is enabled, the clocks to the module are not gated.
 *
 * \param  baseAdd    The memory address of the GPIO instance being used
 *
 * \return None
 *
 * \note   Enabling the GPIO module is a primary step before any other
 *         configurations can be done.
 */

void GPIOModuleEnable(uintptr_t baseAdd)
{
	uint32_t	val;
	/* Clearing the DISABLEMODULE bit in the Control(CTRL) register. */
	val  = in32(baseAdd + GPIO_SYSCONFIG);
	val  = val|0x02;
	out32( baseAdd + GPIO_SYSCONFIG, val);	//复位GPIO模块
	 while(!(in32(baseAdd + GPIO_SYSSTATUS) & 0x1));

    val=in32(baseAdd + GPIO_CTRL);
    val&= ~(GPIO_CTRL_DISABLEMODULE);
    out32(baseAdd + GPIO_CTRL,val);
}

void GPIOModuleDisable(uintptr_t baseAdd)
{
	uint32_t	val;
	/* Clearing the DISABLEMODULE bit in the Control(CTRL) register. */
    val=in32(baseAdd + GPIO_CTRL);
    val|= (GPIO_CTRL_DISABLEMODULE);
    out32(baseAdd + GPIO_CTRL,val);
}



CTOR(Drive_Gpio)

FUNCTION_SETTING(init, gpio_init);
FUNCTION_SETTING(enableIrq, gpio_enableIrq);
FUNCTION_SETTING(disableIrq, gpio_disableIrq);
FUNCTION_SETTING(deatory, gpio_destory);

END_CTOR
