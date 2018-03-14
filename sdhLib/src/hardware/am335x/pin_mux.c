/*
 * pin_mux.c
 *
 *  Created on: 2016-7-7
 *      Author: Administrator
 *      提供引脚配置模块的访问功能
 */
#include "hardware/am335x/am335x.h"
#include <sys/neutrino.h>
#include <stdint.h>
#include <sys/mman.h>
#include <hw/inout.h>
#include "hardware/am335x/pin_mux.h"



int config_pin( int pin_off, int mode, int conf)
{
	uintptr_t		gpio_crtl;

	gpio_crtl = mmap_device_io(0x2000, SOC_CONTROL_REGS);
	out32(gpio_crtl+pin_off, mode | conf);
	munmap_device_io(gpio_crtl, 0x2000);
	return 0;
}

uint32_t rd_pin_conf(int pin_off )
{
	uintptr_t		gpio_crtl;
	uint32_t 	tmp_reg;
	gpio_crtl = mmap_device_io(0x2000, SOC_CONTROL_REGS);
	tmp_reg = in32(gpio_crtl+pin_off);
	munmap_device_io(gpio_crtl, 0x2000);
	return tmp_reg;
}
