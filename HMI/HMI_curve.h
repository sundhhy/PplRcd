#ifndef _INC_real_time_trendHmi_H_
#define _INC_real_time_trendHmi_H_
#include "HMI.h"
#include "commHMI.h"
//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#include <stdint.h>
//------------------------------------------------------------------------------
// check for correct compilation options
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// const defines 
//------------------------------------------------------------------------------
#define RLTHMI_NUM_CURVE			NUM_CHANNEL
 //------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------
CLASS( RLT_trendHMI)
{
	EXTENDS( HMI);
//	IMPLEMENTS( shtCmd);
//	IMPLEMENTS( Observer);
	
	
	
	sheet  		*p_div;
//	sheet  		*p_clean_chnifo;						//ͨ����Ϣ����ͼ��
	
	
	
//	char			flags;
	uint8_t			min_div;
				
	char 			str_div[3];
	
	uint8_t			count;
	uint8_t			chn_show_map;		//ͨ���Ƿ���ʾ��λͼ
	
	uint8_t			arr_crv_fd[6];
	
};
//------------------------------------------------------------------------------
// global variable declarations
//------------------------------------------------------------------------------
extern HMI *g_p_RLT_trendHmi;
//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
RLT_trendHMI *Get_RLT_trendHMI(void);

#endif