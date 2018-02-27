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
	Storage			*stg = Get_storage();
	//从存储器中读取存储的累积信息
	for(i = 0 ;i < NUM_CHANNEL; i++)
	{
		STG_Set_file_position(STG_CHN_SUM(i), STG_DRC_READ, 0);
		
		stg->rd_stored_data(stg, STG_CHN_SUM(i), arr_chn_acc + i, sizeof(rcd_chn_accumlated_t));
			
		
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
	p_clear += 4;
	
	memset(p_clear, 0, sizeof(rcd_chn_accumlated_t) - 4);
	
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

