#include "rtc_pcf8563.h"
#include "sdhDef.h"
#include "device.h"
#include "dev_IIC/dev_IIC.h"
#include "os/os_depend.h"

//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define PCF8563_ADDR                     (0xA2)
#define PCF8563_RTC_CTRL_REG1              0x00
#define PCF8563_RTC_CTRL_REG2              0x01
#define PCF8563_RTC_CTRL_STOP_BIT         (1<<5)

#define PCF8563_SECONDS_REG        0x02
#define PCF8563_MINUTES_REG        0x03
#define PCF8563_HOURS_REG          0x04
#define PCF8563_DAYS_REG           0x05
#define PCF8563_WEEKS_REG          0x06
#define PCF8563_MONTHS_REG         0x07
#define PCF8563_YEARS_REG          0x08

#define PCF8563_SECONDS_REG_OFFSET        0x00
#define PCF8563_MINUTES_REG_OFFSET        0x01
#define PCF8563_HOURS_REG_OFFSET          0x02
#define PCF8563_DAYS_REG_OFFSET           0x03
#define PCF8563_WEEKS_REG_OFFSET          0x04
#define PCF8563_MONTHS_REG_OFFSET         0x05
#define PCF8563_YEARS_REG_OFFSET          0x06
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

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
static I_dev_Char *pcf8563_i2c;

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
/* Cycle/Sync Callback functions */


//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{

int Pcf8563_init( UtlRtc *self, IN void *arg)
{
	Dev_open(DEVID_IIC1, (void *)&pcf8563_i2c);
	return RET_OK;
}
int Pcf8563_set( UtlRtc *self, IN struct  tm *tm)
{
	
	uint8_t   date[7];
	uint8_t	  retry = 100;
	I2C_slave_t		pcf = {
		PCF8563_ADDR,
		PCF8563_SECONDS_REG,
		
		};
	
		
	date[PCF8563_SECONDS_REG_OFFSET] = BIN2BCD(tm->tm_sec );
	date[PCF8563_MINUTES_REG_OFFSET] = BIN2BCD(tm->tm_min );
	date[PCF8563_HOURS_REG_OFFSET] = BIN2BCD(tm->tm_hour);
	date[PCF8563_DAYS_REG_OFFSET] = BIN2BCD(tm->tm_mday);
	date[PCF8563_WEEKS_REG_OFFSET] = 0;
	date[PCF8563_MONTHS_REG_OFFSET] = BIN2BCD(tm->tm_mon);
	date[PCF8563_YEARS_REG_OFFSET] = BIN2BCD(tm->tm_year % 100);

	pcf8563_i2c->ioctol(pcf8563_i2c, DEVCMD_SET_ARGUMENT, &pcf);
	while(pcf8563_i2c->write(pcf8563_i2c, date, 7) != 7)
	{
		if(retry)
		{
			retry --;
			delay_ms(100);
		}
		else
			break;
	}
	if(retry)
		return RET_OK;
	else
		return ERR_DEV_FAILED;
}


int	Pcf8563_get( UtlRtc *self, OUT struct  tm *tm)
{
	uint8_t   date[7];
	I2C_slave_t		pcf = {
		PCF8563_ADDR,
		PCF8563_SECONDS_REG,
		
		};
	
	pcf8563_i2c->ioctol(pcf8563_i2c, DEVCMD_SET_ARGUMENT, &pcf);
	if(pcf8563_i2c->read(pcf8563_i2c, date, 7) != 7)
		return ERR_OPT_FAILED;
	
	
	
	tm->tm_sec  = BCD2BIN(date[PCF8563_SECONDS_REG_OFFSET] & 0x7f);
	tm->tm_min  = BCD2BIN(date[PCF8563_MINUTES_REG_OFFSET] & 0x7f);
	tm->tm_hour  = BCD2BIN(date[PCF8563_HOURS_REG_OFFSET] & 0x3f);
	tm->tm_wday  = BCD2BIN(date[PCF8563_WEEKS_REG_OFFSET] & 0x7) + 1;
	tm->tm_mday  = BCD2BIN(date[PCF8563_DAYS_REG_OFFSET] & 0x3f);
	tm->tm_mon  = BCD2BIN(date[PCF8563_MONTHS_REG_OFFSET] & 0x1f);
	tm->tm_year  = BCD2BIN(date[PCF8563_YEARS_REG_OFFSET] & 0xff);
	
	if(tm->tm_mon < 1)
		tm->tm_mon = 1;
	return RET_OK;
	
}


int	Pcf8563_readReg(UtlRtc *self, IN uint8_t	reg, OUT uint8_t val[], uint8_t num)
{
	
	return RET_OK;
}
	

int	Pcf8563_writeReg( UtlRtc *self, IN uint8_t	reg, IN uint8_t val[], uint8_t num)
{
	
	return RET_OK;
}




CTOR( Pcf8563)
SUPER_CTOR( UtlRtc);
FUNCTION_SETTING( UtlRtc.init, Pcf8563_init);
FUNCTION_SETTING( UtlRtc.get, Pcf8563_get);
FUNCTION_SETTING( UtlRtc.set, Pcf8563_set);
FUNCTION_SETTING( UtlRtc.readReg, Pcf8563_readReg);
FUNCTION_SETTING( UtlRtc.writeReg, Pcf8563_writeReg);

END_ABS_CTOR
