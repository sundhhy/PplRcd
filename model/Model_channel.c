#include "Model_channel.h"
#include <string.h>
#include <stdlib.h>
#include "mem/CiiMem.h"
#include "sdhDef.h"
#include "utils/Storage.h"

//通道模型:一个通道模型对应一个通道
//因此该模型在系统中会存在多个,不能使用单例
//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------

Model		*arr_p_mdl_chn[NUM_CHANNEL];
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
/* Cycle/Sync Callback functions */
static int MdlChn_init( Model *self, IN void *arg);
static int MdlChn_getData(  Model *self, IN int aux, void *arg) ;

static char* MdlChn_to_string( Model *self, IN int aux, void *arg);
static int  MdlChn_to_percentage( Model *self, void *arg);
static int MdlChn_set_by_string( Model *self, IN int aux, void *arg);

static void Read_default_conf(chn_info_t *p_ci, int chnnum);
static void Pe_singnaltype(e_signal_t sgt, char *str);
static int Str_to_data(char *str, int prec);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
Model_chn *Get_Mode_chn(int n)
{
	Model_chn *p_mc = NULL;
	Model	*p_mdl;
	if(n > NUM_CHANNEL)
		return NULL;
	if(arr_p_mdl_chn[n])
		return SUB_PTR(arr_p_mdl_chn[n], Model, Model_chn);
	
	p_mc = Model_chn_new();
	p_mdl = SUPER_PTR(p_mc, Model);
	p_mdl->init(p_mdl, (void *)&n);
	arr_p_mdl_chn[n] = p_mdl;
	
	return p_mc;
}

CTOR( Model_chn)
SUPER_CTOR( Model);
FUNCTION_SETTING( Model.init, MdlChn_init);
FUNCTION_SETTING( Model.getMdlData, MdlChn_getData);
FUNCTION_SETTING( Model.to_string, MdlChn_to_string);
FUNCTION_SETTING( Model.to_percentage, MdlChn_to_percentage);
FUNCTION_SETTING( Model.set_by_string, MdlChn_set_by_string);

END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{


static int MdlChn_init(Model *self, IN void *arg)
{
	int					chn_num = *((int *)arg);
	Model_chn		*cthis = SUB_PTR(self, Model, Model_chn);
	
	cthis->str_buf = CALLOC(1,8);
	cthis->unit_buf = CALLOC(1,8);
	cthis->alarm_buf = CALLOC(1,8);
	self->mdl_id = MDLID_CHN(chn_num);
	cthis->chni.chn_NO = chn_num;
	
	if(Strg_rd_chnConf(&cthis->chni, chn_num) != RET_OK) {

		Read_default_conf(&cthis->chni, chn_num);
	}		

	
	return RET_OK;
}

static int MdlChn_getData(  Model *self, IN int aux, void *arg) 
{
	Model_chn		*cthis = SUB_PTR( self, Model, Model_chn);
	

	
	switch(aux) {
		case AUX_PERCENTAGE:
			
			return self->to_percentage(self, arg); 
		default:
			break;
	}
		
	
	return cthis->chni.value;
	
}

static char* MdlChn_to_string( Model *self, IN int aux, void *arg)
{
	Model_chn		*cthis = SUB_PTR( self, Model, Model_chn);
	char		*p = (char *)arg;
	int 			hh = cthis->chni.upper_limit;
	int 			hi = cthis->chni.upper_limit ;
	int 			li = cthis->chni.lower_limit ;
	int 			ll = 0;
	
	
	
	switch(aux) {
		case AUX_DATA:
			if( arg) {
				p = (char *)arg;		
			} else {
				p = cthis->str_buf;
			}

			sprintf( p, "%3d.%d", cthis->chni.value/10, cthis->chni.value%10);
			return p;
		case AUX_UNIT:
			if( arg) {
				p = (char *)arg;		
			} else {
				p = cthis->unit_buf;
			}
			Mdl_unit_to_string( cthis->chni.unit, p, 8);
			return p;
		case AUX_ALARM:
			if( arg) {
				p = (char *)arg;		
			} else {
				p = cthis->alarm_buf;
			}
		
			memset(p, 0, 8);
			if(cthis->chni.value > hh) {
				strcat(p, "HH ");
			}
			
			if(cthis->chni.value > hi) {
				strcat(p, "Hi ");
			}
			
			if(cthis->chni.value < li) {
				strcat(p, "Li ");
			}
			
			if(cthis->chni.value < ll) {
				strcat(p, "LL ");
			}
			
			//把空间填满，来达到清除掉上一次的显示的目的
			if( strlen( p) < 4) {
				strcat(p, "   ");
			}
			return p;
//		case AUX_PERCENTAGE:
//			if( arg) {
//				p = (char *)arg;		
//			} else {
//				p = cthis->str_buf;
//			}
//			if(cthis->range == 1000)
//				sprintf( p, "%d.%d", cthis->i_rand/10, cthis->i_rand%10);
//			else
//				sprintf( p, "%d", cthis->i_rand);
//			return p;
			
		case AUX_SIGNALTYPE:
			Pe_singnaltype((e_signal_t)cthis->chni.signal_type, (char *)arg);
			break;
		case AUX_CHN_lower_limit:
			Pe_float(cthis->chni.lower_limit, 1, (char *)arg);
			break;
		case AUX_CHN_upper_limit:
			Pe_float(cthis->chni.upper_limit, 1, (char *)arg);
			break;
		case AUX_CHN_small_signal:
			Pe_float(cthis->chni.small_signal, 1, (char *)arg);
			strcat((char *)arg, " %");
			break;
		case AUX_CHN_K:
			Pe_float(cthis->chni.k, 2, (char *)arg);
			break;
		case AUX_CHN_B:
			Pe_float(cthis->chni.k, 1, (char *)arg);
			break;	
		
		default:
			break;
			
		
	}
	return NULL;
}

static int MdlChn_set_by_string(Model *self, IN int aux, void *arg)
{
//	Model_chn		*cthis = SUB_PTR( self, Model, Model_chn);
//	char		*p = (char *)arg;
//	switch(aux) {
//		case AUX_DATA:
//		
//			return p;
//		case AUX_UNIT:
//			if( arg) {
//				p = (char *)arg;		
//			} else {
//				p = cthis->unit_buf;
//			}
//			Mdl_unit_to_string( cthis->chni.unit, p, 8);
//			return p;
//		case AUX_ALARM:
//			if( arg) {
//				p = (char *)arg;		
//			} else {
//				p = cthis->alarm_buf;
//			}
//		
//			memset(p, 0, 8);
//			if(cthis->chni.value > hh) {
//				strcat(p, "HH ");
//			}
//			
//			if(cthis->chni.value > hi) {
//				strcat(p, "Hi ");
//			}
//			
//			if(cthis->chni.value < li) {
//				strcat(p, "Li ");
//			}
//			
//			if(cthis->chni.value < ll) {
//				strcat(p, "LL ");
//			}
//			
//			//把空间填满，来达到清除掉上一次的显示的目的
//			if( strlen( p) < 4) {
//				strcat(p, "   ");
//			}
//			return p;
////		case AUX_PERCENTAGE:
////			if( arg) {
////				p = (char *)arg;		
////			} else {
////				p = cthis->str_buf;
////			}
////			if(cthis->range == 1000)
////				sprintf( p, "%d.%d", cthis->i_rand/10, cthis->i_rand%10);
////			else
////				sprintf( p, "%d", cthis->i_rand);
////			return p;
//			
//		case AUX_SIGNALTYPE:
//			Pe_singnaltype((e_signal_t)cthis->chni.signal_type, (char *)arg);
//			break;
//		case AUX_CHN_lower_limit:
//			Pe_float(cthis->chni.lower_limit, 1, (char *)arg);
//			break;
//		case AUX_CHN_upper_limit:
//			Pe_float(cthis->chni.upper_limit, 1, (char *)arg);
//			break;
//		case AUX_CHN_small_signal:
//			Pe_float(cthis->chni.small_signal, 1, (char *)arg);
//			strcat((char *)arg, " %");
//			break;
//		case AUX_CHN_K:
//			Pe_float(cthis->chni.k, 2, (char *)arg);
//			break;
//		case AUX_CHN_B:
//			Pe_float(cthis->chni.k, 1, (char *)arg);
//			break;	
//		
//		default:
//			break;
//			
//		
//	}
//	
	
}

static int  MdlChn_to_percentage( Model *self, void *arg)
{
	Model_chn		*cthis = SUB_PTR( self, Model, Model_chn);
	
	
	
	return cthis->chni.value ;
}


static void Read_default_conf(chn_info_t *p_ci, int chnnum)
{
	
	
}

static void Pe_singnaltype(e_signal_t sgt, char *str)
{
	switch(sgt)
	{
		case AI_0_5_V:
			sprintf(str, "0~5V");
			break;
		case AI_0_10_mA:
			sprintf(str, "0~10mA");
			break;		
		case AI_1_5_V:
			sprintf(str, "1~5V");
			break;
		case AI_4_20_mA:
			sprintf(str, "4~20mA");
			break;	
		case AI_0_20_mA:
			sprintf(str, "0~20mA");
			break;
		case AI_0_100_mA:
			sprintf(str, "0~100mA");
			break;	
		case AI_Pt100:
			sprintf(str, "Pt100");
			break;
		case AI_Cu50:
			sprintf(str, "Cu50");
			break;
		case AI_B:
			sprintf(str, "B");
			break;		
		case AI_E:
			sprintf(str, "E");
			break;
		case AI_J:
			sprintf(str, "J");
			break;		
		case AI_K:
			sprintf(str, "K");
			break;
		case AI_S:
			sprintf(str, "S");
			break;	
		case AI_T:
			sprintf(str, "T");
			break;
		case AI_0_400_ohm:
			sprintf(str, "0-400 O");
			break;		
		case PI_0_30_kHz:
			sprintf(str, "PI");
			break;
		case DI_8_30_V:
			sprintf(str, "DI 8~30V");
			break;		
		case DI_0_5_V:
			sprintf(str, "DI 0~5V");
			break;	
		case AO_4_20_mA:
			sprintf(str, "AO");
			break;			
	}
			
	
	
}



static int Str_float_to_int(char *str, int prec)
{
	
	return 0;
}

