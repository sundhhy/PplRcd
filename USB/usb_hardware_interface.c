//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//
//提供ch372芯片与单片机之间的硬件接口
//包括：中断引脚的操作，SPI的读写，系统时间的获取。
#include "usb_hardware_interface.h"

#include "os/os_depend.h"
#include "dev_cmd.h"
#include "deviceId.h"
#include "device.h"
#include "sdhDef.h"
#include "system.h"

#include "hardwareConfig.h"
//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------

//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define SET_CH376RST_HIGH	GPIO_SetBits(GPIO_PORT_USBRESET, GPIO_PIN_USBRESET)
#define SET_CH376RST_LOW	GPIO_ResetBits(GPIO_PORT_USBRESET, GPIO_PIN_USBRESET)


#define SET_CH376ENA_HIGH	GPIO_SetBits(GPIO_PORT_SPI1, GPIO_PIN_SPI1_NSS)
#define SET_CH376ENA_LOW	GPIO_ResetBits(GPIO_PORT_SPI1, GPIO_PIN_SPI1_NSS)

#define SET_CH376PWR_HIGH	GPIO_SetBits(GPIO_PORT_POWER, GPIO_PIN_POWER)
#define SET_CH376PWR_LOW	GPIO_ResetBits(GPIO_PORT_POWER, GPIO_PIN_POWER)
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
static	I_dev_Char	*ch376_dev;
static	I_dev_Char			*ch376_intr_pin;
static	usb_op_t *uhi_hop;
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------

static	void				UHI_usb_power(int	on);		
static	void				UHI_usb_reset(void);
	
static	void				UHI_usb_set_irq(int on);
static	int					UHI_usb_read_intr_pin(void);
	
static	void				UHI_usb_delay_ms(int ms);
	
static	void				UHI_usb_cs_on(void);
static	void				UHI_usb_cs_off(void);	
static	int					UHI_usb_write_bytes(uint8_t *buf, int write_len);
static	int					UHI_usb_read_bytes(uint8_t *buf, int read_len);
static	void				UHI_usb_get_time(usb_file_tm *t);


static  void UHI_intr( void *arg, int type, int encode);

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//


int UHI_Init(usb_op_t *hop)
{
	Dev_open(DEVID_GPIO_A10, (void *)&ch376_intr_pin);
	Dev_open(DEVID_SPI1, (void *)&ch376_dev);
	
	
	ch376_intr_pin->ioctol(ch376_intr_pin, DEVCMD_SET_IRQHDL, UHI_intr, (void *)NULL);
	
	hop->usb_power = UHI_usb_power;
	hop->usb_reset = UHI_usb_reset;
	
	hop->usb_set_irq = UHI_usb_set_irq;
	hop->usb_read_intr_pin = UHI_usb_read_intr_pin;
	hop->usb_delay_ms = UHI_usb_delay_ms;
	hop->usb_write_bytes = UHI_usb_write_bytes;	
	hop->usb_read_bytes = UHI_usb_read_bytes;
	hop->usb_get_time = UHI_usb_get_time;
	
	hop->usb_cs_off = UHI_usb_cs_off;
	hop->usb_cs_on = UHI_usb_cs_on;
	

	uhi_hop = hop;
	
	
	return 0;
}








//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{
static	void	UHI_usb_power(int	on)		
{
	if(on) 
	{
		SET_CH376PWR_LOW;
	}
	else 
	{
		
		
		SET_CH376PWR_HIGH;
	}
}
static	void				UHI_usb_reset(void)
{
	SET_CH376RST_HIGH;
	UHI_usb_delay_ms(100);
	
	SET_CH376RST_LOW;
	UHI_usb_delay_ms(100);
	
	UHI_usb_cs_off();
	UHI_usb_delay_ms(100);
}
	
static	void				UHI_usb_set_irq(int on)
{
	if(on) 
	{
		ch376_intr_pin->ioctol(ch376_intr_pin,DEVCMD_ENABLE_IRQ);
		
	}
	else 
	{
		
		ch376_intr_pin->ioctol(ch376_intr_pin,DEVCMD_DISABLE_IRQ);
	}
	
}
static	int					UHI_usb_read_intr_pin(void)
{
	
	char	pin_val = 0;
	ch376_intr_pin->read(ch376_intr_pin, &pin_val, 1);
	return pin_val;
}
	
static	void				UHI_usb_delay_ms(int ms)
{
	delay_ms(ms);
	
}
	
static	void				UHI_usb_cs_on(void)
{
	SET_CH376ENA_LOW;
}
static	void				UHI_usb_cs_off(void)	
{
	SET_CH376ENA_HIGH;
}
static	int					UHI_usb_write_bytes(uint8_t *buf, int write_len)
{
	return ch376_dev->write(ch376_dev, buf, write_len);
}
static	int					UHI_usb_read_bytes(uint8_t *buf, int read_len)
{
	return ch376_dev->read(ch376_dev, buf, read_len);
}
static	void				UHI_usb_get_time(usb_file_tm *t)
{
	struct tm st;
	
	System_time(&st);
	
	t->hour = st.tm_hour;
	t->mday = st.tm_mday;
	t->min = st.tm_min;
	t->mon = st.tm_mon;
	t->sec = st.tm_sec;
	t->year = st.tm_year;
	
}

static  void UHI_intr( void *arg, int type, int encode)
{
	
	if(uhi_hop->usb_irq_cb)
		uhi_hop->usb_irq_cb();
	
}




