/*
 * am335x_gpio.h
 *
 *  Created on: 2016-7-4
 *      Author: Administrator
 */

#ifndef AM335X_GPIO_H_
#define AM335X_GPIO_H_
#include <stdint.h>
#include <arm/omap3530.h>
#include <arm/omap2420.h>
#include "lw_oopc.h"
#include "basis/sdhError.h"
#include "hd_conf_def.h"
#include "am335x.h"
#include <sys/neutrino.h>
#include <pthread.h>


#define		LOW_LEVELDETECT 	( 1 << 0)
#define		HIG_LEVELDETECT 	( 1 << 1)
#define		RISINGDETECT 		( 1 << 2)
#define		FALLINGDETECT 		( 1 << 3)

#define 	GPIO_DETECT(n)   ( GPIO_LEVELDETECT0 + (n * 4))
#define 	GPIO_IRQSTATUS_SET(n)   (GPIO_IRQSTATUS_SET_0 + (n * 4))
#define 	GPIO_IRQSTATUS_CLR(n)   (GPIO_IRQSTATUS_CLR_0 + (n * 4))
#define 	GPIO_GPIO_IRQSTATUS(n)   (GPIO_IRQSTATUS_0 + (n * 4))
#define 	GPIO_IRQSTATUS_RAW(n)   (GPIO_IRQSTATUS_RAW_0 + (n * 4))

CLASS(Drive_Gpio)
{
	err_t 	(*init)(Drive_Gpio *);
	err_t	( *enableIrq)( Drive_Gpio *);
	err_t	( *disableIrq)( Drive_Gpio *);
	err_t	( *deatory)( Drive_Gpio *);

	void	( *irq_handle)( void *);
	void	*irq_handle_arg;
	int					irq_id;
	uintptr_t			gpio_vbase;
	gpio_cfg			*config;
	pthread_t			pid;
	volatile uint32_t			states;
	struct sigevent		isr_event;

};


typedef enum {
	//for drives management
	ERROR_GPIO_BEGIN = ERROR_BEGIN(DRIVE_GPIO),
	gpio_init_mapio_fail,






}err_gpio_t;
const struct sigevent *gpioExtInteIsr (void *area, int id);
//extern struct sigevent	Gpio_event;
extern void GPIOModuleEnable(uintptr_t baseAdd);
void GPIO3ModuleClkConfig(void);
int InitOutputPin(gpio_cfg *p_cfg);
int PinOutput(gpio_cfg *p_cfg, char val);
int DestoryOutputPin(gpio_cfg *p_cfg);
void dump_gpio_reg( uintptr_t baseAdd);
#endif /* AM335X_GPIO_H_ */
