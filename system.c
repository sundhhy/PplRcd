//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//
#include "system.h"
#include <string.h>
#include <stdio.h>
//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------
system_conf_t	g_system;

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
static void Break_deal_string(char *p, int deal);
static void Disable_string(char *p, int able);

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
void System_default(system_conf_t *p_s)
{
	
	
}


void System_to_string(void *p_data, char	*p_s, int len, int aux)
{
	uint8_t *p_u8;
	switch(aux)
	{
		case es_psd:
			if(p_data)
				p_u8 = (uint8_t *)p_data;
			else 
				p_u8 = g_system.password;
			
			sprintf(p_s, "%02d %02d %02d", p_u8[0], p_u8[1], p_u8[2]);
			break;
		
		case es_brk_cpl:
			if(p_data)
				p_u8 = (uint8_t *)p_data;
			else 
				p_u8 = &g_system.break_couple;
			
			Break_deal_string(p_s, *p_u8);
			break;
		case es_brk_rss:
			if(p_data)
				p_u8 = (uint8_t *)p_data;
			else 
				p_u8 = &g_system.break_resistor;
			
			Break_deal_string(p_s, *p_u8);
			break;
		case es_cmn_md:
			if(p_data)
				p_u8 = (uint8_t *)p_data;
			else 
				p_u8 = &g_system.communication_mode;
			
			if(*p_u8 == 0)
				sprintf(p_s, "通讯");
			else 
				sprintf(p_s, "打印");
			break;
		case es_mdfy_prm:
			if(p_data)
				p_u8 = (uint8_t *)p_data;
			else 
				p_u8 = &g_system.disable_modify_adjust_paramter;
			
			Disable_string(p_s, *p_u8);
			break;
			
		case es_CJC:
			if(p_data)
				p_u8 = (uint8_t *)p_data;
			else 
				p_u8 = &g_system.CJC;
			if(*p_u8 < 100) {
				
				sprintf(p_s, "设定 %d", *p_u8);
			} else {
				sprintf(p_s, "外部    ");
			}
			break;
			
		case es_vcs:
			if(p_data)
				p_u8 = (uint8_t *)p_data;
			else 
				p_u8 = &g_system.disable_view_chn_status;
			
			Disable_string(p_s, *p_u8);
			break;
		case es_beep:
			if(p_data)
				p_u8 = (uint8_t *)p_data;
			else 
				p_u8 = &g_system.enable_beep;
			
			Disable_string(p_s, *p_u8);
			break;
	}
	
	
}



void System_modify_string(void *p_data, char	*p_s, int aux, int op, int val)
{
//	uint8_t *p_u8;
//	switch(aux)
//	{
//		case es_psd:
//			if(p_data)
//				p_u8 = (uint8_t *)p_data;
//			else 
//				p_u8 = g_system.password;
//			
//			sprintf(p_s, "%02d %02d %02d", p_u8[0], p_u8[1], p_u8[2]);
//			break;
//		
//		case es_brk_cpl:
//			if(p_data)
//				p_u8 = (uint8_t *)p_data;
//			else 
//				p_u8 = &g_system.break_couple;
//			
//			Break_deal_string(p_s, *p_u8);
//			break;
//		case es_brk_rss:
//			if(p_data)
//				p_u8 = (uint8_t *)p_data;
//			else 
//				p_u8 = &g_system.break_resistor;
//			
//			Break_deal_string(p_s, *p_u8);
//			break;
//		case es_cmn_md:
//			if(p_data)
//				p_u8 = (uint8_t *)p_data;
//			else 
//				p_u8 = &g_system.communication_mode;
//			
//			if(*p_u8 == 0)
//				sprintf(p_s, "通讯");
//			else 
//				sprintf(p_s, "打印");
//			break;
//		case es_mdfy_prm:
//			if(p_data)
//				p_u8 = (uint8_t *)p_data;
//			else 
//				p_u8 = &g_system.disable_modify_adjust_paramter;
//			
//			Disable_string(p_s, *p_u8);
//			break;
//			
//		case es_CJC:
//			if(p_data)
//				p_u8 = (uint8_t *)p_data;
//			else 
//				p_u8 = &g_system.CJC;
//			if(*p_u8 < 100) {
//				
//				sprintf(p_s, "设定 %d", *p_u8);
//			} else {
//				sprintf(p_s, "外部    ");
//			}
//			break;
//			
//		case es_vcs:
//			if(p_data)
//				p_u8 = (uint8_t *)p_data;
//			else 
//				p_u8 = &g_system.disable_view_chn_status;
//			
//			Disable_string(p_s, *p_u8);
//			break;
//		case es_beep:
//			if(p_data)
//				p_u8 = (uint8_t *)p_data;
//			else 
//				p_u8 = &g_system.enable_beep;
//			
//			Disable_string(p_s, *p_u8);
//			break;
//	}
	
	
}


//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{
static void Break_deal_string(char *p, int deal)
{
	
	switch(deal)
	{
		case 0:
			sprintf(p, "始点");
			break;
		case 1:
			sprintf(p, "保持");
			break;
		case 2:
			sprintf(p, "终点");
			break;
		default:
			break;
		
	}
	
}

static void Disable_string(char *p, int able)
{
	
	if(able == 0)
		sprintf(p, "禁止");
	else 
		sprintf(p, "允许");
	
}




