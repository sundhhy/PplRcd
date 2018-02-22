//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//
#include "sys_cmd.h"
#include "system.h"
#include "Communication/modbusRTU_cli.h"
#include "sdhDef.h"
#include "device.h"
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
#define MBA_UART_ID				DEVID_SPI2
#define MBA_UART_CONF			g_confUart1
#define MBA_SYSTEM				phn_sys
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------

static I_dev_Char *p_MBA_uart;
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------

static void MBA_Run(void *arg);
static int MBA_reg_2_area(uint16_t reg_addr, uint16_t num_reg);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
int MBA_Init(void)
{
	int ret = RET_OK;
	
	ret = Dev_open(MBA_UART_ID, (void *)&p_MBA_uart);
	if(ret != RET_OK)
		goto exit;
	
	
	
//	ret = p_MBA_ser->open(p_MBA_ser, &MBA_UART_CONF);
//	if(ret != RET_OK)
//		goto exit;
	
	p_MBA_uart->ioctol(p_MBA_uart, DEVCMD_SET_TXWAITTIME_MS, 1000);
	p_MBA_uart->ioctol(p_MBA_uart, DEVCMD_SET_TXWAITTIME_MS, 100);
	
	ret = Cmd_Rgt_recv(MBA_Run, NULL);
	if(ret < 0)
	{
		ret = ERR_RSU_UNAVAILABLE;
		goto exit;
		
	}

	exit:
	return ret;
	
	
}


int			MBC_reg_2_ram(uint16_t	reg, uint16_t reg_num, char mbc_cmd, void *ram_ptr)
{
	int ret = RET_OK;
	uint16_t **pp_u16 = (uint16_t **)ram_ptr;
	
	
	//先判断访问的地址区域是否落在合法的地址区间内
	ret = MBA_reg_2_area(reg, reg_num);
	if(ret < 0)
	{
		ret = ERR_CMM_ADDR_ERR;
		goto exit;
		
	}
	//然后再依次访问数据
	
	switch(mbc_cmd)
	{
		case READ_HOLD:
			
		
			break;
			
		case WRITE_1_HOLD:
			
			break;
		case WRITE_N_HOLD:
			
		
			break;
		default:
			ret = ERR_CMM_CMDERR;
			
		
		
		
	}
	
	
	exit:
	return ret;
	
}
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{
static void MBA_Run(void *arg)
{
	uint8_t		modbus_buf[96];  //系统最长的连续寄存器数量是32个，所以这么多字节能放下了
	uint8_t		modbus_ack_buf[96];  //系统最长的连续寄存器数量是32个，所以这么多字节能放下了
	int			read_len;
	
	read_len = p_MBA_uart->read(p_MBA_uart, modbus_buf, 96);
	if(read_len <= 0)
		return;
	if(MBC_Get_pkt_addr(modbus_buf) != MBA_SYSTEM.sys_conf.id)
		return;
	
	read_len = MBC_Decode_pkt(modbus_buf, read_len, modbus_ack_buf, 96);
	
	if(read_len <= 0)
		return;
	p_MBA_uart->write(p_MBA_uart, modbus_ack_buf, read_len);

	
	
}

static int MBA_reg_2_area(uint16_t reg_addr, uint16_t num_reg)
{
	uint16_t	param_system_area[2] = {0x7000, 0x7009};
	uint16_t	param_channel_area[2] = {0x6000, 0x601E};
	uint16_t	param_transmitting_area[2] = {0x5000, 0x5005};		
	uint16_t	param_accumulation_area[2] = {0x5030, 0x5031};
	uint16_t	param_PID_area[2] = {0x5050, 0x506F};
	uint16_t	param_flow_area[2] = {0x5150, 0x516F};
	uint16_t	data_real_time_area[2] = {0x7FFA, 0x801F};
	uint16_t	data_accumulation_area[2] = {0x8100, 0x8187};	
	uint16_t	*arr_areas[8] = {param_system_area, param_channel_area, param_transmitting_area, \
		param_accumulation_area, param_PID_area, param_flow_area, data_real_time_area, data_accumulation_area};

	uint16_t	reg_area[2];
	
	int			i;
	int			area_num = -1;
	
	reg_area[0] = reg_addr;
	reg_area[1] = reg_addr + num_reg;
	
	for(i = 0; i < 8; i++)
	{
		if((reg_area[0] >= arr_areas[i][0]) && (reg_area[1] <= arr_areas[i][1]))
		{
			
			area_num = i;
			break;
			
		}
		
		
		
	}
	
	
	return area_num;
	
}


