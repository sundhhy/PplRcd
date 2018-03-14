/*
 * hd_conf_def.h
 *
 *  Created on: 2017-10-10
 *      Author: Administrator
 */

#ifndef HD_CONF_DEF_H_
#define HD_CONF_DEF_H_
#include <stdint.h>
#include <stdbool.h>
typedef struct gpmc_config {
	uint32_t	base_address;		//访问的物理内存的地址
	uint32_t	mask_address;		//GPMC的内存地址掩码
	uint32_t	mmap_size;			//映射的内存长度
	// GPMC_CONFIG1_n
	uint8_t		chip_instance;
	char		device_size;
	char		device_type;
	char		mux_add_data;

	char		read_multiple;
	char		read_type;
	char		write_multiple;
	char		write_type;

	char 		waitpin;			///< wait引脚选择
	bool		rd_monitoring;
	bool		wr_monitoring;
	char		rese;

	void		*cmmTim;
	void		*rdTim;
	void		*wrTim;

}gpmc_chip_cfg;


typedef struct gpio_config {

	//GPIOg_n
	uint8_t		pin_group;			//引脚组号
	uint8_t		pin_number;			//引脚组内序号
	uint8_t		intr_type;
	uint8_t		intr_line;		//AM335x的GPIO中断有两个中断线
	uint8_t		intrPrio;			//中断优先级
	uint8_t		debou_time;

	uint16_t	pin_ctrl_off;
	uint8_t		clk_init;
	uint8_t		none[3];

}gpio_cfg;

#endif /* HD_CONF_DEF_H_ */
