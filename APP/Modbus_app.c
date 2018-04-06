//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//
#include "sys_cmd.h"
#include "system.h"
#include "Communication/modbusRTU_cli.h"
#include "sdhDef.h"
#include "device.h"
#include "hardwareConfig.h"
#include "ModelFactory.h"
#include "channel_accumulated.h"
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
#define MBA_UART_ID				DEVID_UART4
#define MBA_UART_CONF			g_confUart4
#define MBA_SYSTEM				phn_sys

#define MBA_ACC_READ				0
#define MBA_ACC_WRITE				1



const	uint16_t	param_system_area[2] = {0x7000, 0x7009};
const	uint16_t	param_channel_area[2] = {0x6000, 0x62ff};		//6个通道，每个通道0x30个寄存器
const	uint16_t	param_transmitting_area[2] = {0x5000, 0x502f};		
const	uint16_t	param_accumulation_area[2] = {0x5030, 0x504f};
const	uint16_t	param_PID_area[2] = {0x5050, 0x514F};
const	uint16_t	param_flow_area[2] = {0x5150, 0x534F};
const	uint16_t	data_real_time_area[2] = {0x7FFA, 0x801F};
const	uint16_t	data_accumulation_area[2] = {0x8100, 0x897f};	
const	uint16_t	*arr_areas[8] = {param_system_area, param_channel_area, param_transmitting_area, \
		param_accumulation_area, param_PID_area, param_flow_area, data_real_time_area, data_accumulation_area};
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------
typedef int (*MBA_access_area)(uint16_t	offset, char rd_or_wr, uint16_t *p);
//typedef int (*MBA_write_area)(uint16_t	offset, uint16_t v);		
//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------

static I_dev_Char *p_MBA_uart;
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------

static void MBA_Run(void);
static int MBA_reg_2_area(uint16_t reg_addr, uint16_t num_reg);
static MBA_access_area MBA_Get_acc_func(char area);
//static MBA_write_area MBA_Get_write_func(char area);
		
static int MBA_Acc_param_system(uint16_t	offset, char rd_or_wr, uint16_t *p);
static int MBA_Acc_param_channel(uint16_t	offset, char rd_or_wr, uint16_t *p);
static int MBA_Acc_param_transmitting(uint16_t	offset, char rd_or_wr, uint16_t *p);
static int MBA_Acc_param_accumulation(uint16_t	offset, char rd_or_wr, uint16_t *p);
static int MBA_Acc_param_PID(uint16_t	offset, char rd_or_wr, uint16_t *p);
static int MBA_Acc_param_flow(uint16_t	offset, char rd_or_wr, uint16_t *p);
static int MBA_Acc_data_real_time(uint16_t	offset, char rd_or_wr, uint16_t *p);
static int MBA_Acc_data_accumulation(uint16_t	offset, char rd_or_wr, uint16_t *p);

//static int MBA_Write_param_system(uint16_t	offset, uint16_t v);
//static int MBA_Write_param_channel(uint16_t	offset, uint16_t v);
//static int MBA_Write_param_transmitting(uint16_t	offset, uint16_t v);
//static int MBA_Write_param_accumulation(uint16_t	offset, uint16_t v);
//static int MBA_Write_param_PID(uint16_t	offset, uint16_t v);
//static int MBA_Write_param_flow(uint16_t	offset, uint16_t v);
//static int MBA_Write_data_real_time(uint16_t	offset, uint16_t v);
//static int MBA_Write_data_accumulation(uint16_t	offset, uint16_t v);
		
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
int MBA_Init(void)
{
	int ret = RET_OK;
	
	if(MBA_SYSTEM.sys_conf.baud_rate)
		MBA_UART_CONF.cfguart->USART_BaudRate = MBA_SYSTEM.sys_conf.baud_rate;
	
	ret = Dev_open(MBA_UART_ID, (void *)&p_MBA_uart);
	if(ret != RET_OK)
		goto exit;
	
	
	
//	ret = p_MBA_ser->open(p_MBA_ser, &MBA_UART_CONF);
//	if(ret != RET_OK)
//		goto exit;
	
	p_MBA_uart->ioctol(p_MBA_uart, DEVCMD_SET_TXWAITTIME_MS, 1000);
	p_MBA_uart->ioctol(p_MBA_uart, DEVCMD_SET_TXWAITTIME_MS, 100);
	
	ret = Cmd_Rgt_recv(MBA_Run);
	if(ret < 0)
	{
		ret = ERR_RSU_UNAVAILABLE;
		goto exit;
		
	}

	exit:
	return ret;
	
	
}

void MBA_Init_uart(int baud)
{
	p_MBA_uart->ioctol(p_MBA_uart, DEVCMD_SET_BAUD, baud);
	
}


int			MBC_reg_2_ram(uint16_t	reg, uint16_t reg_num, char mbc_cmd, void *ram_ptr)
{
	static uint16_t  			mba_ram = 0;
	int 									ret = RET_OK;
	uint16_t 							**pp_u16 = (uint16_t **)ram_ptr;
	MBA_access_area					acc_func = NULL;
	uint16_t							wr_val;
	
	//先判断访问的地址区域是否落在合法的地址区间内
	ret = MBA_reg_2_area(reg, reg_num);
	if(ret < 0)
	{
		ret = ERR_CMM_ADDR_ERR;
		goto exit;
		
	}
	if(ram_ptr == NULL)
	{
		//只是查询地址范围是否正确的时候会传入NULL
		ret = RET_OK;
		goto exit;
		
	}
	//然后再依次访问数据
	acc_func = MBA_Get_acc_func(ret);
	switch(mbc_cmd)
	{
		case READ_HOLD:
			
			if(acc_func(reg - arr_areas[ret][0], MBA_ACC_READ, &mba_ram) != RET_OK)
			{
				ret = ERR_CMM_ADDR_ERR;
				goto exit;
				
			}
			ret = RET_OK;
			*pp_u16 = &mba_ram;
			break;
			
			//因为涉及到字节序的问题，因此就不支持连续的写了
			//让底层把连续的写转化成多个单个写
		case WRITE_1_HOLD:
		case WRITE_N_HOLD:
			wr_val = *(uint16_t *)ram_ptr;
			if(acc_func(reg - arr_areas[ret][0], MBA_ACC_WRITE, &wr_val) != RET_OK)
			{
				ret = ERR_CMM_ADDR_ERR;
				goto exit;
				
			}
			ret = RET_OK;
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
static void MBA_Run(void)
{
	uint8_t		modbus_buf[96];  //系统最长的连续寄存器数量是32个，所以这么多字节能放下了
	uint8_t		modbus_ack_buf[280];  //系统最长的连续寄存器数量是32个，所以这么多字节能放下了
	int			read_len;
	
	read_len = p_MBA_uart->read(p_MBA_uart, modbus_buf, 96);
	if(read_len <= 0)
		return;
	if(MBC_Get_pkt_addr(modbus_buf) != MBA_SYSTEM.sys_conf.id)
		return;
	
	read_len = MBC_Decode_pkt(modbus_buf, read_len, modbus_ack_buf, 280);
	
	if(read_len <= 0)
		return;
	p_MBA_uart->write(p_MBA_uart, modbus_ack_buf, read_len);

	
	
}

static int MBA_reg_2_area(uint16_t reg_addr, uint16_t num_reg)
{


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

static MBA_access_area MBA_Get_acc_func(char area)
{
	switch(area)
	{
		
		case 0:
			return MBA_Acc_param_system;
		case 1:
			return MBA_Acc_param_channel;
		case 2:
			return MBA_Acc_param_transmitting;
		case 3:
			return MBA_Acc_param_accumulation;
		case 4:
			return MBA_Acc_param_PID;
		case 5:
			return MBA_Acc_param_flow;
		case 6:
			return MBA_Acc_data_real_time;
		case 7:
			return MBA_Acc_data_accumulation;
		default:
			return NULL;
	}
}

//static MBA_write_area MBA_Get_write_func(char area)
//{
//	
//	
//	switch(area)
//	{
//		
//		case 0:
//			return MBA_Write_param_system;
//		case 1:
//			return MBA_Write_param_channel;
//		case 2:
//			return MBA_Write_param_transmitting;
//		case 3:
//			return MBA_Write_param_accumulation;
//		case 4:
//			return MBA_Write_param_PID;
//		case 5:
//			return MBA_Write_param_flow;
//		case 6:
//			return MBA_Write_data_real_time;
//		case 7:
//			return MBA_Write_data_accumulation;
//		default:
//			return NULL;
//	}
//	
//}


static int MBA_Acc_param_system(uint16_t	offset, char rd_or_wr, uint16_t *p)
{
	system_conf_t	*p_sys_conf = &MBA_SYSTEM.sys_conf;
	int						ret = RET_OK;
	switch(offset)
	{
		case 0:
			if(rd_or_wr == MBA_ACC_READ)
			{
				*p = p_sys_conf->record_gap_s;
			}
			else 
			{
				if(*p > 240)
				{
					ret = ERR_OUT_OF_LIMIT;
					break;
					
				}
				
				p_sys_conf->record_gap_s = *p;
				
			}
			break;
		case 1:
			if(rd_or_wr == MBA_ACC_READ)
			{
				*p = p_sys_conf->break_couple;
			}
			else 
			{
				if(*p > 2)
				{
					ret = ERR_OUT_OF_LIMIT;
					break;
					
				}
				
				p_sys_conf->break_couple = *p;
				
			}
			break;
		case 2:
			if(rd_or_wr == MBA_ACC_READ)
			{
				*p = p_sys_conf->break_resistor;
			}
			else 
			{
				if(*p > 2)
				{
					ret = ERR_OUT_OF_LIMIT;
					break;
					
				}
				
				p_sys_conf->break_resistor = *p;
				
			}
			break;
		case 3:
			if(rd_or_wr == MBA_ACC_READ)
			{

					*p = p_sys_conf->cold_end_way;
			}
			else 
			{
				if(*p > 1)
				{
					ret = ERR_OUT_OF_LIMIT;
					break;
					
				}
				
				p_sys_conf->cold_end_way = *p;
				
			}
			break;
		case 4:
			if(rd_or_wr == MBA_ACC_READ)
			{
				*p = p_sys_conf->CJC;
			}
			else 
			{
				if(p_sys_conf->cold_end_way == 0)
				{
					ret = ERR_NOT_SUPPORT;		//外部时，不允许设置
					break;
					
				}
				if(*p > 99)
				{
					ret = ERR_OUT_OF_LIMIT;
					break;
					
				}
				
				p_sys_conf->CJC = *p;
				
			}
			break;
		case 5:
		case 6:
			if(rd_or_wr == MBA_ACC_READ)
			{
				*p = 0;
			}
		
			break;
		case 7:
			if(rd_or_wr == MBA_ACC_READ)
			{

				*p = phn_sys.major_ver* 10 + phn_sys.minor_ver;
			}
			else 
			{
				
				ret = ERR_NOT_SUPPORT;		
					
				
			}
			break;
		case 8:
			if(rd_or_wr == MBA_ACC_READ)
			{

				*p = HRD_VER;
			}
			else 
			{
				
				ret = ERR_NOT_SUPPORT;		
					
				
			}
			break;
		case 9:
			if(rd_or_wr == MBA_ACC_READ)
			{

				*p = p_sys_conf->num_chn;
			}
			else 
			{
				
				ret = ERR_NOT_SUPPORT;		
					
				
			}
			break;
		default:
			ret = ERR_NOT_SUPPORT;		
	}
	
	return ret;
}
static int MBA_Acc_param_channel(uint16_t	offset, char rd_or_wr, uint16_t *p)
{
	Model_chn			*p_mc;
//	Model					*p_md;
	int							ret = RET_OK;
	int16_t				*p_s16 = (int16_t *)p;
	uint8_t				chn_offset = 0;
	uint8_t 			chn_num = 0;

	
	chn_num = offset / 0x30;
	if(chn_num > NUM_CHANNEL)
	{
		ret = ERR_OUT_OF_RANGE;
		goto exit;
		
	}
	
	
	chn_offset = offset % 0x30;
	
	p_mc = Get_Mode_chn(chn_num);
//	p_md = SUPER_PTR(p_mc, Model);
	
	switch(chn_offset)
	{
		case 0:
			if(rd_or_wr == MBA_ACC_READ)
			{
				*p = p_mc->chni.signal_type;
			}
			else 
			{
				if(*p >= es_max)
				{
					ret = ERR_OUT_OF_LIMIT;
					break;
					
				}
				
				p_mc->chni.signal_type = *p;
				
			}
			break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			if(rd_or_wr == MBA_ACC_READ)
			{
				*p = p_mc->chni.tag_NO;
			}
			else 
			{
				if(*p > 255)
				{
					ret = ERR_OUT_OF_LIMIT;
					break;
					
				}
				
				p_mc->chni.tag_NO = *p;
				
			}
			break;
		
		case 8:
			if(rd_or_wr == MBA_ACC_READ)
			{
				*p = p_mc->chni.upper_limit;
			}
			else 
			{
				if(*p > 9999)
				{
					ret = ERR_OUT_OF_LIMIT;
					break;
					
				}
				
				p_mc->chni.upper_limit = *p;
				
			}
			break;
		case 9:
			if(rd_or_wr == MBA_ACC_READ)
			{

					*p = p_mc->chni.lower_limit;
			}
			else 
			{
				if(*p > 9999)
				{
					ret = ERR_OUT_OF_LIMIT;
					break;
					
				}
				
				p_mc->chni.lower_limit = *p;
				
			}
			break;
		case 10:
			if(rd_or_wr == MBA_ACC_READ)
			{
				*p = 1;
			}
			else 
			{
				ret = ERR_NOT_SUPPORT;		//外部时，不允许设置
			}
			break;
		case 11:
			if(rd_or_wr == MBA_ACC_READ)
			{
				*p = p_mc->chni.unit;
			}
			else 
			{
				if(*p >= eu_max)
				{
					ret = ERR_OUT_OF_LIMIT;
					break;
					
				}
				
				p_mc->chni.unit = *p;
				
			}
			break;
		case 0x11:
			if(rd_or_wr == MBA_ACC_READ)
			{
				*p = p_mc->chni.filter_time_s;
			}
			else 
			{
				if(*p > 99)
				{
					ret = ERR_OUT_OF_LIMIT;
					break;
					
				}
				
				p_mc->chni.filter_time_s = *p;
				
			}
			break;	
		case 0x12:
			if(rd_or_wr == MBA_ACC_READ)
			{
				*p = p_mc->chni.small_signal;
			}
			else 
			{
				if(*p > 100)
				{
					ret = ERR_OUT_OF_LIMIT;
					break;
					
				}
				
				p_mc->chni.small_signal = *p;
				
			}
			break;
		case 0x13:
			if(rd_or_wr == MBA_ACC_READ)
			{
				*p_s16 = p_mc->chni.k;
			}
			else 
			{
				if(*p_s16 > 999 || *p_s16 < -999)
				{
					ret = ERR_OUT_OF_LIMIT;
					break;
					
				}
				
				p_mc->chni.k = *p_s16;
				
			}
			break;		
		case 0x14:
			if(rd_or_wr == MBA_ACC_READ)
			{
				*p_s16 = p_mc->chni.b;
			}
			else 
			{
				if(*p_s16 > 999 || *p_s16 < -999)
				{
					ret = ERR_OUT_OF_LIMIT;
					break;
					
				}
				
				p_mc->chni.b = *p_s16;
				
			}
			break;				
		case 0x16:
			if(rd_or_wr == MBA_ACC_READ)
			{
				*p_s16 = p_mc->alarm.alarm_hh;
			}
			else 
			{
				if(*p_s16 > 30000 || *p_s16 < -999)
				{
					ret = ERR_OUT_OF_LIMIT;
					break;
					
				}
				
				p_mc->alarm.alarm_hh = *p_s16;
				
			}
			break;		
		case 0x17:
			if(rd_or_wr == MBA_ACC_READ)
			{
				*p = p_mc->alarm.touch_spot_hh;
			}
			else 
			{
				if(*p > 6)
				{
					ret = ERR_OUT_OF_LIMIT;
					break;
					
				}
				
				p_mc->alarm.touch_spot_hh = *p;
				
			}
			break;				
		case 0x18:
			if(rd_or_wr == MBA_ACC_READ)
			{
				*p_s16 = p_mc->alarm.alarm_hi;
			}
			else 
			{
				if(*p_s16 > 30000 || *p_s16 < -999)
				{
					ret = ERR_OUT_OF_LIMIT;
					break;
					
				}
				
				p_mc->alarm.alarm_hi = *p_s16;
				
			}
			break;		
		case 0x19:
			if(rd_or_wr == MBA_ACC_READ)
			{
				*p = p_mc->alarm.touch_spot_hi;
			}
			else 
			{
				if(*p > 6)
				{
					ret = ERR_OUT_OF_LIMIT;
					break;
					
				}
				
				p_mc->alarm.touch_spot_hi = *p;
				
			}
			break;				
		case 0x1A:
			if(rd_or_wr == MBA_ACC_READ)
			{
				*p_s16 = p_mc->alarm.alarm_lo;
			}
			else 
			{
				if(*p_s16 > 30000 || *p_s16 < -999)
				{
					ret = ERR_OUT_OF_LIMIT;
					break;
					
				}
				
				p_mc->alarm.alarm_lo = *p_s16;
				
			}
			break;		
		case 0x1B:
			if(rd_or_wr == MBA_ACC_READ)
			{
				*p = p_mc->alarm.touch_spot_lo;
			}
			else 
			{
				if(*p > 6)
				{
					ret = ERR_OUT_OF_LIMIT;
					break;
					
				}
				
				p_mc->alarm.touch_spot_lo = *p;
				
			}
			break;				
		case 0x1C:
			if(rd_or_wr == MBA_ACC_READ)
			{
				*p_s16 = p_mc->alarm.alarm_ll;
			}
			else 
			{
				if(*p_s16 > 30000 || *p_s16 < -999)
				{
					ret = ERR_OUT_OF_LIMIT;
					break;
					
				}
				
				p_mc->alarm.alarm_ll = *p_s16;
				
			}
			break;		
		case 0x1D:
			if(rd_or_wr == MBA_ACC_READ)
			{
				*p = p_mc->alarm.touch_spot_ll;
			}
			else 
			{
				if(*p > 6)
				{
					ret = ERR_OUT_OF_LIMIT;
					break;
					
				}
				
				p_mc->alarm.touch_spot_ll = *p;
				
			}
			break;			
		case 0x1E:
			if(rd_or_wr == MBA_ACC_READ)
			{
				*p = p_mc->alarm.alarm_backlash;
			}
			else 
			{
				if(*p > 100)
				{
					ret = ERR_OUT_OF_LIMIT;
					break;
					
				}
				
				p_mc->alarm.alarm_backlash = *p;
				
			}
			break;			
		case 6:
		case 7:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 0x15:
			if(rd_or_wr == MBA_ACC_READ)
			{
				*p = 0;
			}
		
			break;
		default:
			if(chn_offset < 0x2F)
			{
				
				if(rd_or_wr == MBA_ACC_READ)
				{
					*p = 0;
				}
				break;
				
			}
			ret = ERR_NOT_SUPPORT;	
			break;
	}
	
	exit:
	return ret;
}
static int MBA_Acc_param_transmitting(uint16_t	offset, char rd_or_wr, uint16_t *p)
{
	if(rd_or_wr == MBA_ACC_READ)
	{

		*p = 0;
	}
	return RET_OK;
	
}
static int MBA_Acc_param_accumulation(uint16_t	offset, char rd_or_wr, uint16_t *p)
{
	
	if(rd_or_wr == MBA_ACC_READ)
	{

		*p = 0;
	}
	return RET_OK;
}
static int MBA_Acc_param_PID(uint16_t	offset, char rd_or_wr, uint16_t *p)
{
	if(rd_or_wr == MBA_ACC_READ)
	{

		*p = 0;
	}
	return RET_OK;
	
}
static int MBA_Acc_param_flow(uint16_t	offset, char rd_or_wr, uint16_t *p)
{
	if(rd_or_wr == MBA_ACC_READ)
	{

		*p = 0;
	}
	return RET_OK;
	
}
static int MBA_Acc_data_real_time(uint16_t	offset, char rd_or_wr, uint16_t *p)
{
	Model_chn			*p_mc;
	Model					*p_md;
	struct  tm		time;

	
	
	if(rd_or_wr == MBA_ACC_WRITE)
	{
		
		return ERR_NOT_SUPPORT;
	}

	

	
	
	if(offset < 6)
	{
		p_md = Create_model("time");
		p_md->getMdlData(p_md, TIME_TM, &time);
	}
	switch(offset)
	{
		case 0:
			*p = time.tm_year;
			break;
		case 1:
			*p = time.tm_mon;
			break;
		case 2:
			*p = time.tm_mday;
			break;
		case 3:
			*p = time.tm_hour;
			break;
		case 4:
			*p = time.tm_min;
			break;
		case 5:
			*p = time.tm_sec;
			break;
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
			p_mc = Get_Mode_chn(offset - 6);
			p_md = SUPER_PTR(p_mc, Model);
			p_md->getMdlData(p_md, AUX_DATA, p);
			break;
		default:
			*p = 0;
			break;
	}
	
	return RET_OK;
	
}
static int MBA_Acc_data_accumulation(uint16_t	offset, char rd_or_wr, uint16_t *p)
{
	
	rcd_chn_accumlated_t	*p_cna;
	char					chn_num = offset / 0x88;
	uint8_t					chn_offset = offset % 0x88;
	char					day_or_month = 0;
	char					part = 0;
	
	if(rd_or_wr == MBA_ACC_WRITE)
	{
		
		return ERR_NOT_SUPPORT;
	}
	
	p_cna = arr_chn_acc + chn_num;
	
	if(chn_offset < 0x5d)
	{
		day_or_month = chn_offset / 3;
		part = chn_offset % 3;
		*p = p_cna->accumlated_day[day_or_month][part];
		
	}
	else if((chn_offset > 0x5c) && (chn_offset < 0x81))
	{
		day_or_month = (offset - 0x5d) / 3;
		part = (offset - 0x5d)  % 3;
		*p = p_cna->accumlated_month[day_or_month][part];
	}
	else if((chn_offset > 0x80) && (chn_offset < 0x84))
	{
		part = (offset - 0x81)  % 3;
		*p = p_cna->accumlated_year[part];
	}
	else if((chn_offset > 0x83) && (chn_offset < 0x88))
	{
		part = (offset - 0x84)  % 3;
		*p = p_cna->accumlated_total[part];
	}
	return RET_OK;
}










