//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//
#include "sys_cmd.h"
#include "system.h"
#include "Communication/modbusRTU_cli.h"
#include "sdhDef.h"
#include "device.h"
#include "hardwareConfig.h"
#include "channel_accumulated.h"
#include <string.h>

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------

rcd_chn_accumlated_t	arr_chn_acc[NUM_CHANNEL];
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

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------


		
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
int CNA_Init(void)
{
	int 				i;
	struct  tm			t;
	Storage			*stg = Get_storage();
	//从存储器中读取存储的累积信息
	System_time(&t);
	for(i = 0 ;i < NUM_CHANNEL; i++)
	{
		STG_Set_file_position(STG_CHN_SUM(i), STG_DRC_READ, 0);
		
		stg->rd_stored_data(stg, STG_CHN_SUM(i), arr_chn_acc + i, sizeof(rcd_chn_accumlated_t));
		if(arr_chn_acc[i].sum_start_year == 0)
		{

			arr_chn_acc[i].sum_start_year = t.tm_year;
		}	
		if(arr_chn_acc[i].sum_start_month == 0)
		{

			arr_chn_acc[i].sum_start_month = t.tm_mon;
		}	
		if(arr_chn_acc[i].sum_start_day == 0)
		{

			arr_chn_acc[i].sum_start_day = t.tm_mday;
		}			
		
	}
	
	
	
	return RET_OK;
}


void CNA_Run(int cyc_ms)
{
	
	
	
	
}

void CNA_Print_enable(char *s, char	enable)
{
	if(enable)
	{
		
		sprintf(s, "是");
	}
	else
	{
		
		sprintf(s, "否");
	}
	
	
}

//prec 表示有几位小数
uint64_t CNA_arr_u16_2_u64(uint16_t *p_u16, char num_data)
{
	
	uint64_t rst = 0;
	int 		i;
	
	for(i = num_data - 1; i >= 0; i--)
	{
		rst <<= 16;
		rst |= p_u16[i];
		
	}
	
//	return rst;
	return 0xffffffffffff;
}

//pos 小数点位数
//字符串总长度不超过14
//如果有小数点，则整数部分长度不超过12
void CNA_Print_acc_val(uint16_t *p_val, char *s, char pos)
{
	
	
	unsigned  long templong1,templong2;
	uint16_t temd1,temd2,temd3;
	uint8_t i,j,d;	
	
	uint8_t  m;	
	
	
	temd1=*p_val;
	temd2=*(p_val+1);
	temd3=*(p_val+2);

	temd1 = 0;
	temd2 = 0;
	temd3 = 11;
		
	for(i=0;i<14;i++)
	{
	    *(s+i)= ' ';
	}	
	templong1=(unsigned long )temd1*0x10000+temd2;
	j=0;
	
	while((templong1!=0)||(temd3!=0))
	{
		d=templong1%10;
		templong1/=10;	
		templong2=(unsigned long)d*0x10000+temd3;
		temd3=templong2/10;
		*(s+13-j)=templong2%10 + '0';
		j++;
	}
	
	//加上小数点
	if(pos == 0)
		return;
	j=13;
	m = pos;
	
	for(i=0;i<13 - m;i++)
	{
		*(s+i)=*(s+1+i);
	}
	*(s+i)= '.';
	//避免出现.1这种情况，所以如果小数点前面要加0
	if(*(s+i - 1)== ' ') 
		*(s+i - 1)= '0';
	
}

void CNA_u64_2_arr_u16(uint16_t *p_u16, uint64_t val, char prec)
{
	
	
	
	
	
}

int		CNA_Commit(char chn_num)
{
	Storage			*stg = Get_storage();
	
	STG_Set_file_position(STG_CHN_SUM(chn_num), STG_DRC_WRITE, 0);
	stg->wr_stored_data(stg, STG_CHN_SUM(chn_num), arr_chn_acc + chn_num, sizeof(rcd_chn_accumlated_t));
	
	return RET_OK;

}

int		CNA_Clear(char chn_num)
{
	
	char *p_clear;
	Storage			*stg = Get_storage();
	
	p_clear = (char *)(arr_chn_acc + chn_num);
	p_clear += 6;
	
	memset(p_clear, 0, sizeof(rcd_chn_accumlated_t) - 6);
	
	STG_Set_file_position(STG_CHN_SUM(chn_num), STG_DRC_WRITE, 0);
	stg->wr_stored_data(stg, STG_CHN_SUM(chn_num), arr_chn_acc + chn_num, sizeof(rcd_chn_accumlated_t));
	
	return RET_OK;

}

//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{

