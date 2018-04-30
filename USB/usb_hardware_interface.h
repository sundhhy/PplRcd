//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#ifndef __INC_usb_hardware_interface_H__
#define __INC_usb_hardware_interface_H__

#include <stdint.h>
//------------------------------------------------------------------------------
// check for correct compilation options
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------
typedef struct  {
				short  year;   /* 00 - 99                   */
        
        char  mday;   /* day of the month         -- [1,31] */
        char  mon;    /* months since January     -- [0,11] */
        char  sec;    /* seconds after the minute -- [0,61] */
        char  min;    /* minutes after the hour   -- [0,59] */
        char  hour;   /* hours after midnight     -- [0,23] */
				char	none;
}usb_file_tm;

typedef struct {
	uint32_t		cpu_clk_hz;
	
	
	void  (*usb_irq_cb)(void);		

	
	void				(*usb_power)(int	on);		//on 1:打开电源  0 关闭电源
	void				(*usb_reset)(void);
	
	void				(*usb_set_irq)(int on);
	int					(*usb_read_intr_pin)(void);
	
	void				(*usb_delay_ms)(int ms);
	
	//spi的接口操作
	void				(*usb_cs_on)(void);
	void				(*usb_cs_off)(void);	
	
		//读写接口的返回值都是实际读写的字节数量
	int					(*usb_write_bytes)(uint8_t *buf, int write_len);
	int					(*usb_read_bytes)(uint8_t *buf, int read_len);
	
		
	void				(*usb_get_time)(usb_file_tm *t);
	
	
	
}usb_op_t;

//------------------------------------------------------------------------------
// global variable declarations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
int UHI_Init(usb_op_t *hop);



#endif


