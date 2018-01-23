#include "Model_channel.h"
#include <string.h>
#include <stdlib.h>
#include "mem/CiiMem.h"
#include "sdhDef.h"
#include "utils/Storage.h"
#include "Communication/smart_bus.h"
#include "device.h"
#include "os/os_depend.h"
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
static void MdlChn_run(Model *self);
static int MdlChn_self_check(Model *self);
static int MdlChn_getData(  Model *self, IN int aux, void *arg) ;
static int MdlChn_setData(  Model *self, IN int aux, void *arg) ;

static char* MdlChn_to_string( Model *self, IN int aux, void *arg);
static int  MdlChn_to_percentage( Model *self, void *arg);
static int MdlChn_set_by_string( Model *self, IN int aux, void *arg);
static int MdlChn_modify_sconf(Model *self, IN int aux, char *s, int op, int val);
static void Pe_singnaltype(e_signal_t sgt, char *str);
static void Pe_touch_spot(int spot, char *str);

static void MdlChn_Save_2_conf(mdl_chn_save_t *p_mcs, chn_info_t *p_cnf, uint8_t direct);

static void MdlChn_Save_2_alarm(mdl_chn_save_t *p_mcs, chn_alarm_t *p_alr, uint8_t direct);

static	uint16_t Zero_shift_K_B(chn_info_t *p, uint16_t	d);
static	uint16_t Cut_small_signal(chn_info_t *p, uint16_t	d);
static void Signal_Alarm(chn_info_t *p, chn_alarm_t *p_alm);
//static int Str_to_data(char *str, int prec);
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
	arr_p_mdl_chn[n] = p_mdl;
	p_mdl->init(p_mdl, (void *)&n);
	
	
	return p_mc;
}

CTOR( Model_chn)
SUPER_CTOR( Model);
FUNCTION_SETTING( Model.init, MdlChn_init);
FUNCTION_SETTING( Model.run, MdlChn_run);

FUNCTION_SETTING( Model.self_check, MdlChn_self_check);
FUNCTION_SETTING( Model.getMdlData, MdlChn_getData);
FUNCTION_SETTING( Model.setMdlData, MdlChn_setData);

FUNCTION_SETTING( Model.to_string, MdlChn_to_string);
FUNCTION_SETTING( Model.to_percentage, MdlChn_to_percentage);
FUNCTION_SETTING( Model.modify_str_conf, MdlChn_modify_sconf);

FUNCTION_SETTING( Model.set_by_string, MdlChn_set_by_string);

END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{

//
static int16_t	Percent_to_data(chn_info_t *p,uint16_t prc, char	point)
{	
	int16_t j;
	int16_t fct = 100;
	
	if(point == 1)
		fct = 1000;
	else if(fct == 2)
		fct = 10000;
	
	j = (long)prc * (p->upper_limit - p->lower_limit) / fct;
	return(j);	
}

static void Signal_Alarm(chn_info_t *p, chn_alarm_t *p_alm)
{
	uint8_t flag, new_flag = 0;
	int16_t tempS2,tempS3,bjhc, prc_data;
	int32_t temps4;

	flag = p_alm->alm_flag;
	
	temps4 = p_alm->alarm_backlash * 0x10000;
	bjhc = temps4 / 1000;
	prc_data = Percent_to_data(p, bjhc, 2);	
	
	tempS2 = p_alm->alarm_ll;				//低低报时加上回差
	if(flag & ALM_LL)
		tempS2 += prc_data;	

	tempS3 = p_alm->alarm_lo;		//低报时加上回差
	if(flag & ALM_LO)
		tempS3 += prc_data;	
	
	p->value = p->value;
	//报警判断及处理
	if(p->value < tempS3)		
	{
		if(p->value<tempS2)
		{	//低低报
			phn_sys.DO_err |= 1 << p_alm->touch_spot_ll;
			new_flag |= ALM_LL;
		}
		else
		{	//低报
			phn_sys.DO_err |= 1 << p_alm->touch_spot_lo;
			new_flag |= ALM_LO;
		}
	}
	else
	{
		tempS2 = p_alm->alarm_hh;
		if(flag & ALM_HH)
			tempS2 -= prc_data;	

		tempS3 = p_alm->alarm_hi;
		if(flag & ALM_HI)
			tempS3 -= prc_data;	
		
		if(p->value>tempS3)
		{
				if(p->value>tempS2)
				{		//高高报
					phn_sys.DO_err |= 1 << p_alm->touch_spot_hh;
					new_flag |= ALM_HH;
				}
				else
				{
					//高报
					phn_sys.DO_err |= 1 << p_alm->alarm_hi;
					new_flag |= ALM_HI;
				}
				
			}		
	}
	//todo: 180115  产生新报警或者消除旧的报警的时候，是需要进行报警的记录处理
	p_alm->alm_flag = new_flag;
}

static	uint16_t Zero_shift_K_B(chn_info_t *p, uint16_t	d)
{
	uint32_t	tmp_u32;
	uint16_t 	rst = d;
	
	if(p->k == 0)
		goto exit;
	
	
		tmp_u32 = p->k*(long)d/100 + p->b;
		if( tmp_u32 > p->upper_limit) 	
			tmp_u32 = p->upper_limit;
		
		if( tmp_u32 < p->lower_limit) 	
			tmp_u32 = p->lower_limit;
	
	
	rst = tmp_u32;
	exit:
	return rst;
}

static	uint16_t Cut_small_signal(chn_info_t *p, uint16_t	d)
{
	
	uint32_t	tmp_u32;
	uint16_t 	rst = d;
	
	if(p->small_signal == 0)
		goto exit;
	
	tmp_u32 = p->upper_limit - p->lower_limit;
	tmp_u32 = tmp_u32 * p->small_signal / 1000;
	if(rst < tmp_u32)
		rst = p->lower_limit;
	
	exit:
	return rst;
}



static int MdlChn_init(Model *self, IN void *arg)
{
	int					chn_num = *((int *)arg);
	Model_chn		*cthis = SUB_PTR(self, Model, Model_chn);
	Storage			*stg = Get_storage();
	mdl_chn_save_t		save;
	
	cthis->str_buf = CALLOC(1,8);
	cthis->unit_buf = CALLOC(1,8);
	cthis->alarm_buf = CALLOC(1,8);
	self->mdl_id = MDLID_CHN(chn_num);
	cthis->chni.chn_NO = chn_num;
	
	if(stg->rd_stored_data(stg, STG_CHN_CONF(cthis->chni.chn_NO), &save, sizeof(save)) != RET_OK) 
	{

		MdlChn_default_conf(chn_num);
		MdlChn_default_alarm(chn_num);
	}		
	else
	{
		MdlChn_Save_2_conf(&save, &cthis->chni, 0);
		MdlChn_Save_2_alarm(&save, &cthis->alarm, 0);
		
	}

	
	return RET_OK;
}

int	MdlChn_save_data(uint8_t chn_num, mdl_chn_save_t *p)
{
	
	Model_chn *p_mdl= Get_Mode_chn(chn_num);
	
	if(p_mdl == NULL)
		return 0;
	
	MdlChn_Save_2_conf(p, &p_mdl->chni, 1);
	MdlChn_Save_2_alarm(p, &p_mdl->alarm, 1);
	
	return (sizeof(p_mdl->chni) + sizeof(p_mdl->alarm));
}

void MdlChn_default_conf(int chn_num)
{
	Model_chn *p_mdl= Get_Mode_chn(chn_num);
	
	memset(&p_mdl->chni, 0, sizeof(p_mdl->chni));
	p_mdl->chni.signal_type = AI_0_400_ohm;
	p_mdl->chni.chn_NO = chn_num;
	p_mdl->chni.tag_NO = chn_num;
	p_mdl->chni.MB = 2;
}


void MdlChn_default_alarm(int chn_num)
{
	Model_chn *p_mdl= Get_Mode_chn(chn_num);
	
	memset(&p_mdl->alarm, 0, sizeof(p_mdl->alarm));
	p_mdl->alarm.alarm_hh = 0x7fff;
	p_mdl->alarm.alarm_hi = 0x7fff;
	
	
}

static int MdlChn_self_check( Model *self)
{
	Model_chn		*cthis = SUB_PTR(self, Model, Model_chn);
	
	uint8_t			chk_buf[32] = {0};
	I_dev_Char 		*I_uart3 = NULL;
	int				ret  = RET_OK;
	uint16_t		tmp_u16[2];
	uint8_t 			i, j;
//	uint8_t				tmp_u8;
	
	Dev_open(DEVID_UART3, ( void *)&I_uart3);
	i = SmBus_Query(SMBUS_MAKE_CHN(SMBUS_CHN_AI, cthis->chni.chn_NO), chk_buf, 32);
	if( I_uart3->write(I_uart3, chk_buf, i) != RET_OK)
	{
		ret = -1;
		goto err;
	}
	i = I_uart3->read(I_uart3, chk_buf, 32);
	if(i <= 0)
	{
		ret = -2;
		goto err;
	}
	SmBus_decode(SMBUS_CMD_QUERY, chk_buf, &j, 1);
	if(j != cthis->chni.chn_NO)
	{
		ret = -3;
		goto err;
	}
	
	delay_ms(100);
//	i = SmBus_rd_signal_type(SMBUS_MAKE_CHN(SMBUS_CHN_AI, cthis->chni.chn_NO), chk_buf, 32);
//	if( I_uart3->write(I_uart3, chk_buf, i) != RET_OK)
//	{
//		ret = 4;
//		goto err;
//	}
//	i = I_uart3->read(I_uart3, chk_buf, 32);
//	if(i <= 0)
//	{
//		ret = -5;
//		goto err;
//	}
//	if(SmBus_decode(SMBUS_CMD_READ, chk_buf, &tmp_u8, 1) != RET_OK)
//	{
//		ret = -6;
//		goto err;
//	}
//	cthis->chni.signal_type = tmp_u8;
	if(self->getMdlData(self, AUX_SIGNALTYPE, NULL) != RET_OK)
	{
		ret = -1;
		goto err;
	}
	delay_ms(100);
	self->getMdlData(self, chnaux_lower_limit, NULL);
	delay_ms(100);
	self->getMdlData(self, chnaux_upper_limit, NULL);
	
//	delay_ms(100);
//	i = SmBus_RD_hig_limit(SMBUS_MAKE_CHN(SMBUS_CHN_AI, cthis->chni.chn_NO), chk_buf, 32);
//	if( I_uart3->write(I_uart3, chk_buf, i) != RET_OK)
//	{
//		ret = 7;
//		goto err;
//	}
//	i = I_uart3->read(I_uart3, chk_buf, 32);
//	if(i <= 0)
//	{
//		ret = -8;
//		goto err;
//	}
//	if(SmBus_decode(SMBUS_CMD_READ, chk_buf, tmp_u16, 4) != RET_OK)
//	{
//		ret = -9;
//		goto err;
//	}
//	
//	cthis->chni.upper_limit = tmp_u16[0];
//	
//	
//	delay_ms(100);
//	i = SmBus_RD_low_limit(SMBUS_MAKE_CHN(SMBUS_CHN_AI, cthis->chni.chn_NO), chk_buf, 32);
//	if( I_uart3->write(I_uart3, chk_buf, i) != RET_OK)
//	{
//		ret = 7;
//		goto err;
//	}
//	i = I_uart3->read(I_uart3, chk_buf, 32);
//	if(i <= 0)
//	{
//		ret = -8;
//		goto err;
//	}
//	if(SmBus_decode(SMBUS_CMD_READ, chk_buf, tmp_u16, 4) != RET_OK)
//	{
//		ret = -9;
//		goto err;
//	}
	
	//todo：这段代码受到IOM模块的数据结构定义影响
	cthis->chni.upper_limit = tmp_u16[0];
	
	
	cthis->chni.lower_limit = tmp_u16[1];
	
	cthis->chni.decimal = 1;		//目前的smartbus的 工程值小数点只有1位
	return RET_OK;
	err:
		return ret;
}

static void MdlChn_run(Model *self)
{
	Model_chn					*cthis = SUB_PTR(self, Model, Model_chn);
	Storage						*stg = Get_storage();
	uint8_t						chk_buf[16];
	SmBus_result_t		rst;
//	do_out_t			d = {0};
	
	I_dev_Char 			*I_uart3 = NULL;
	uint8_t 			i;
//	uint8_t				old_do;
	
	Dev_open(DEVID_UART3, ( void *)&I_uart3);
	
	//在通道0上要采集冷端温度
	if(cthis->chni.chn_NO == 0)
	{
		i = SmBus_AI_Read(CDT_CHN, AI_READ_ENGVAL, chk_buf, 16);
		if( I_uart3->write(I_uart3, chk_buf, i) != RET_OK)
			goto rd_smpval;
		i = I_uart3->read(I_uart3, chk_buf, 16);
		if(i <= 0)
			goto rd_smpval;
		if(SmBus_decode(SMBUS_AI_READ, chk_buf, &rst, sizeof(SmBus_result_t)) != RET_OK)
			goto rd_smpval;
		
		phn_sys.code_end_temperature = rst.val;
			
	}
//	
//	//读取采样值
	rd_smpval:
//	cthis->chni.smp_flag = 0;
//	i = SmBus_AI_Read(cthis->chni.chn_NO, AI_READ_SMPVAL, chk_buf, 16);
//	if( I_uart3->write(I_uart3, chk_buf, i) != RET_OK)
//		goto rd_engval;
//	i = I_uart3->read(I_uart3, chk_buf, 16);
//	if(i <= 0)
//		goto rd_engval;
//	if(SmBus_decode(SMBUS_AI_READ, chk_buf, &rst, sizeof(SmBus_result_t)) != RET_OK)
//		goto rd_engval;
//	cthis->chni.smp_flag = 1;
//	cthis->chni.sample_value = rst.val;
	
	//读取工程值
//	rd_engval:	
	i = SmBus_AI_Read(SMBUS_MAKE_CHN(SMBUS_CHN_AI, cthis->chni.chn_NO), AI_READ_ENGVAL, chk_buf, 16);
	if( I_uart3->write(I_uart3, chk_buf, i) != RET_OK)
		goto err;
	i = I_uart3->read(I_uart3, chk_buf, 16);
	if(i <= 0)
		goto err;
	if(SmBus_decode(SMBUS_AI_READ, chk_buf, &rst, sizeof(SmBus_result_t)) != RET_OK)
		goto err;

	if(rst.chn_num != cthis->chni.chn_NO)
	{
		goto err;
	}
	else
	{
		cthis->chni.flag_err = 0;
	}
	
	rst.val = Zero_shift_K_B(&cthis->chni, rst.val);
	rst.val = Cut_small_signal(&cthis->chni, rst.val);
	
	if(rst.val != cthis->chni.value)
	{
		
		
		cthis->chni.signal_type = rst.signal_type;
		cthis->chni.value = rst.val;
//		if((phn_sys.sys_flag & SYSFLAG_SETTING) == 0)
			self->notify(self);
		
		Signal_Alarm(&cthis->chni, &cthis->alarm);
		
	}
	
	stg->wr_stored_data(stg, STG_CHN_DATA(cthis->chni.chn_NO), &cthis->chni.value, 2);
	

	return;
	err:
		cthis->chni.flag_err = 1;
	
}

static int MdlChn_getData(Model *self, IN int aux, void *arg) 
{
	Model_chn		*cthis = SUB_PTR( self, Model, Model_chn);
	int16_t			*p_s16;
	uint8_t			*p_u8;
	I_dev_Char 			*I_uart3 = NULL;	
	uint8_t			sbus_buf[32];
	uint16_t		tmp_u16;
	uint8_t			tmp_u8, i;
	
	Dev_open(DEVID_UART3, ( void *)&I_uart3);
	switch(aux) {
		case AUX_DATA:
			if(cthis->chni.flag_err)
				return -1;
			p_s16 = (int16_t *)arg;
			*p_s16 = cthis->chni.value;
			break;
		case AUX_SIGNALTYPE:
			i = SmBus_rd_signal_type(SMBUS_MAKE_CHN(SMBUS_CHN_AI, cthis->chni.chn_NO), sbus_buf, 32);
			if( I_uart3->write(I_uart3, sbus_buf, i) != RET_OK)
			{
				return -1;
			}
			i = I_uart3->read(I_uart3, sbus_buf, 32);
			if(i <= 0)
			{
				return -1;
			}
			if(SmBus_decode(SMBUS_CMD_READ, sbus_buf, &tmp_u8, 1) != RET_OK)
			{
				return -1;
			}
			cthis->chni.signal_type = tmp_u8;
		
			if(arg)
			{
				p_u8 = (uint8_t *)arg;
				*p_u8 = cthis->chni.signal_type;
			}
		
		
		
		
			break;
		
		case chnaux_lower_limit:
		case chnaux_upper_limit:

			
			if(arg)
				p_s16 = (int16_t *)arg;
		
			if(aux == chnaux_upper_limit)
				i = SmBus_RD_hig_limit(SMBUS_MAKE_CHN(SMBUS_CHN_AI, cthis->chni.chn_NO), sbus_buf, 32);
			else
				i = SmBus_RD_low_limit(SMBUS_MAKE_CHN(SMBUS_CHN_AI, cthis->chni.chn_NO), sbus_buf, 32);
			
			if( I_uart3->write(I_uart3, sbus_buf, i) != RET_OK)
			{
				return -1;
			}
			i = I_uart3->read(I_uart3, sbus_buf, 32);
			if(i <= 0)
			{
				return -1;
			}
			if(SmBus_decode(SMBUS_CMD_READ, sbus_buf, &tmp_u16, 2) != RET_OK)
			{
				return -1;
			}
			
			if(aux == chnaux_upper_limit)
				cthis->chni.upper_limit = tmp_u16;
			else
				cthis->chni.lower_limit = tmp_u16;
			if(arg)
				*p_s16 = tmp_u16;
			break;
		case DO_output:
			if(*(uint8_t *)arg)
				phn_sys.DO_val |= 1 << cthis->chni.chn_NO;
			else
				phn_sys.DO_val &=~(1 << cthis->chni.chn_NO);
			i = SmBus_Read_DO(phn_sys.DO_val, sbus_buf, 32);
			if(I_uart3->write(I_uart3, sbus_buf, i) != RET_OK)
				return ERR_OPT_FAILED;
			
			if(i <= 0)
				return ERR_OPT_FAILED;
			i = I_uart3->read(I_uart3, sbus_buf, 32);
			if(i <= 0)
				return ERR_OPT_FAILED;
			if(SmBus_decode(SMBUS_CMD_READ, sbus_buf, &tmp_u8, 1) != RET_OK)
				return ERR_OPT_FAILED;
			
			phn_sys.DO_val = tmp_u8;
			if(arg)
			{
				if(tmp_u8 & (1 << cthis->chni.chn_NO))
					*(uint8_t *)arg = 1;
				else
					*(uint8_t *)arg = 0;
			}
			break;
			
		case AUX_PERCENTAGE:
			
			return self->to_percentage(self, arg); 
		
		
		case MDHCHN_CHN_NUM:
			p_u8 = (uint8_t *)arg;
			*p_u8 = cthis->chni.chn_NO;
			break;
		default:
			break;
	}
	return RET_OK;
	
}



static int MdlChn_setData(  Model *self, IN int aux, void *arg) 
{
	Model_chn		*cthis = SUB_PTR( self, Model, Model_chn);
	uint8_t			*p_u8;
	uint16_t		*p_u16;
	do_out_t		*p_d;
	SmBus_conf_t	sb_conf ;
	uint8_t			sbub_buf[32] = {0};
	I_dev_Char 			*I_uart3 = NULL;
	uint8_t 			i, tmp_u8;
	
	
	Dev_open(DEVID_UART3, ( void *)&I_uart3);
	switch(aux) {
		case AUX_SIGNALTYPE:
			if(arg)
				p_u8 = (uint8_t *)arg;
			else
				p_u8 = &cthis->chni.signal_type;
			
		

			sb_conf.signal_type = *p_u8;
			sb_conf.decimal = cthis->chni.decimal;
		
			//180113 smart bus的组态命令中对上下限的设置是无效的，因此填充0即可
			sb_conf.lower_limit = 0;
			sb_conf.upper_limit = 0;
			i = SmBus_AI_config(cthis->chni.chn_NO, &sb_conf, sbub_buf, 32);
			if( I_uart3->write(I_uart3, sbub_buf, i) != RET_OK)
				return ERR_OPT_FAILED;
			i = I_uart3->read(I_uart3, sbub_buf, 32);
			if(i <= 0)
				return ERR_OPT_FAILED;
			if(SmBus_decode(SMBUS_AI_CONFIG, sbub_buf, &tmp_u8, 1) != RET_OK)
				return ERR_OPT_FAILED;
			if(tmp_u8 != cthis->chni.chn_NO)
				return ERR_OPT_FAILED;
			//回读检查
			
			i = SmBus_rd_signal_type(SMBUS_MAKE_CHN(SMBUS_CHN_AI, cthis->chni.chn_NO), sbub_buf, 32);
			if( I_uart3->write(I_uart3, sbub_buf, i) != RET_OK)
				return ERR_OPT_FAILED;
			i = I_uart3->read(I_uart3, sbub_buf, 32);
			if(i <= 0)
				return ERR_OPT_FAILED;
			if(SmBus_decode(SMBUS_CMD_READ, sbub_buf, &tmp_u8, 1) != RET_OK)
				return ERR_OPT_FAILED;
			if(cthis->chni.signal_type != tmp_u8)
			{
				cthis->chni.signal_type = tmp_u8;
				return ERR_OPT_FAILED;
			}
			
			return RET_OK;
		case chnaux_lower_limit:
		case chnaux_upper_limit:
			
			
			if(aux == chnaux_upper_limit)
			{
				if(arg == NULL)
					p_u16 = &cthis->chni.upper_limit;
				else
					p_u16 = arg;
//				cthis->chni.upper_limit = *(uint16_t *)arg; 
				i = SmBus_WR_hig_limit(SMBUS_MAKE_CHN(SMBUS_CHN_AI, cthis->chni.chn_NO), p_u16, sbub_buf, 32);
				
			}
			else
			{
				if(arg == NULL)
					p_u16 = &cthis->chni.lower_limit;
				else
					p_u16 = arg;
//				cthis->chni.lower_limit = *(uint16_t *)arg; 
				i = SmBus_WR_low_limit(SMBUS_MAKE_CHN(SMBUS_CHN_AI, cthis->chni.chn_NO),  p_u16, sbub_buf, 32);
				
			}
			
			if(I_uart3->write(I_uart3, sbub_buf, i) != RET_OK)
				return ERR_OPT_FAILED;
			
			if(i <= 0)
				return ERR_OPT_FAILED;
			i = I_uart3->read(I_uart3, sbub_buf, 32);
			if(i <= 0)
				return ERR_OPT_FAILED;
			if(SmBus_decode(SMBUS_CMD_WRITE, sbub_buf, &tmp_u8, 1) != RET_OK)
				return ERR_OPT_FAILED;
			if(tmp_u8 != cthis->chni.chn_NO)
				return ERR_OPT_FAILED;
			
			self->getMdlData(self, aux, NULL);
			
			if(aux == chnaux_upper_limit)
			{
				
				if( cthis->chni.upper_limit == *(uint16_t *)arg)
					return RET_OK;
				
			}
			else 
			{
				
				if(cthis->chni.lower_limit == *(uint16_t *)arg)
					return RET_OK;
				
			}
			return ERR_OPT_FAILED;
		
		case DO_output:
			if(arg == NULL)
				break;
			p_d = (do_out_t *)arg;
			if(p_d->do_chn >= MAX_TOUCHSPOT)
				break;
			tmp_u8 = phn_sys.DO_val;
			if(p_d->val)
				tmp_u8 |= 1 << p_d->do_chn;
			else
				tmp_u8 &=~(1 << p_d->do_chn);
			i = SmBus_DO_output(p_d->do_chn, tmp_u8, sbub_buf, 32);
			if(I_uart3->write(I_uart3, sbub_buf, i) != RET_OK)
				return ERR_OPT_FAILED;
			
			if(i <= 0)
				return ERR_OPT_FAILED;
			i = I_uart3->read(I_uart3, sbub_buf, 32);
			if(i <= 0)
				return ERR_OPT_FAILED;
			if(SmBus_decode(SMBUS_CMD_WRITE, sbub_buf, NULL, 0) != RET_OK)
				return ERR_OPT_FAILED;
			phn_sys.DO_val = tmp_u8;
			
			break;
		case AUX_PERCENTAGE:
			
			return self->to_percentage(self, arg); 
		default:
			break;
	}
		
	
	return ERR_OPT_ILLEGAL;
	
}

static char* MdlChn_to_string( Model *self, IN int aux, void *arg)
{
	Model_chn		*cthis = SUB_PTR( self, Model, Model_chn);
	char		*p = (char *)arg;
	int 			hh = cthis->alarm.alarm_hh;
	int 			hi = cthis->alarm.alarm_hi;
	int 			li = cthis->alarm.alarm_lo;
	int 			ll = cthis->alarm.alarm_ll;
	int				i;
	
	
	switch(aux) {
		case AUX_DATA:
			if( arg) {
				p = (char *)arg;		
			} else {
				p = cthis->str_buf;
			}

			sprintf( p, "%4d.%d", cthis->chni.value/10, cthis->chni.value%10);
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
				strcat(p, "Hi");
			}
			
			if(cthis->chni.value < li) {
				strcat(p, "Li ");
			}
			
			if(cthis->chni.value < ll) {
				strcat(p, "LL");
			}
			
			//把空间填满，来达到清除掉上一次的显示的目的
			for(i = strlen(p); i < 5; i++)
			{
				
				p[i] = ' ';
				
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
		case chnaux_record_mb:
			sprintf(arg, "%d M", cthis->chni.MB);
			break;
		case chnaux_filter_ts:
			sprintf(arg, "%d S", cthis->chni.filter_time_s);
			break;
		case chnaux_lower_limit:
			sprintf(arg, "%-5d", cthis->chni.lower_limit);
//			Pe_float(cthis->chni.lower_limit, 1, (char *)arg);
			break;
		case chnaux_upper_limit:
			sprintf(arg, "%-5d", cthis->chni.upper_limit);
//			Pe_float(cthis->chni.upper_limit, 1, (char *)arg);
			break;
		case chnaux_small_signal:
			Pe_float(cthis->chni.small_signal, 1, (char *)arg);
			strcat((char *)arg, " %");
			break;
		case chnaux_k:
//			Pe_frefix_float(cthis->chni.k, 2, "K:",(char *)arg);
			Pe_float(cthis->chni.k, 2, (char *)arg);
			break;
		case chnaux_b:
//			Pe_frefix_float(cthis->chni.b, 1, "B:", (char *)arg);
			Pe_float(cthis->chni.b, 1, (char *)arg);
			break;	
	
		case alarm_hh:
			sprintf(arg, "%-5d", cthis->alarm.alarm_hh);
//			Pe_float(cthis->alarm.alarm_hh, 1, (char *)arg);
			break;
		case alarm_hi:
			sprintf(arg, "%-5d", cthis->alarm.alarm_hi);
//			Pe_float(cthis->alarm.alarm_hi, 1, (char *)arg);
			break;
		case alarm_lo:
			sprintf(arg, "%-5d", cthis->alarm.alarm_lo);
//			Pe_float(cthis->alarm.alarm_lo, 1, (char *)arg);
			break;
		case alarm_ll:
			sprintf(arg, "%-5d", cthis->alarm.alarm_ll);
//			Pe_float(cthis->alarm.alarm_ll, 1, (char *)arg);
			break;
		case tchspt_hh:
			Pe_touch_spot(cthis->alarm.touch_spot_hh, (char *)arg);
			break;
		case tchspt_hi:
			Pe_touch_spot(cthis->alarm.touch_spot_hi, (char *)arg);
			break;
		case tchspt_lo:
			Pe_touch_spot(cthis->alarm.touch_spot_lo, (char *)arg);	
			break;
		case tchspt_ll:
			Pe_touch_spot(cthis->alarm.touch_spot_ll, (char *)arg);	
			break;	
		case alarm_backlash:
			Pe_float(cthis->alarm.alarm_backlash, 1, (char *)arg);
			break;
		default:
			break;
			
		
	}
	return NULL;
}
static int MdlChn_modify_sconf(Model *self, IN int aux, char *s, int op, int val)
{
	Model_chn		*cthis = SUB_PTR( self, Model, Model_chn);
	
	uint8_t			tmp_u8 = 0;
	phn_sys.save_chg_flga |= CHG_MODCHN_CONF(cthis->chni.chn_NO);
	switch(aux) {
		case AUX_UNIT:
			tmp_u8 = Operate_in_tange(cthis->chni.unit, op, 1, 0, eu_max - 1);
			if(tmp_u8 != cthis->chni.unit)
			{
				cthis->chni.unit = tmp_u8;
				
			}
//			cthis->chni.unit = Operate_in_tange(cthis->chni.unit, op, 1, 0, eu_max - 1);
			self->to_string(self, AUX_UNIT, s);
			break;
		case chnaux_tag_NO:
			cthis->chni.tag_NO = Operate_in_tange(cthis->chni.tag_NO, op, 1, 0, 9);
			sprintf(s, "%d", cthis->chni.tag_NO);
			break;
		case AUX_SIGNALTYPE:
			cthis->chni.signal_type = Operate_in_tange(cthis->chni.signal_type, op, 1, 0, es_max - 1);
			self->to_string(self, AUX_SIGNALTYPE, s);
			break;
		case chnaux_record_mb:
			cthis->chni.MB = Operate_in_tange(cthis->chni.MB, op, val, 0, 99);
			
			sprintf(s, "%d M", cthis->chni.MB);
			break;
		case chnaux_filter_ts:
			cthis->chni.filter_time_s = Operate_in_tange(cthis->chni.filter_time_s, op, val, 0, 99);
			
			sprintf(s, "%d S", cthis->chni.filter_time_s);
			break;
		case chnaux_lower_limit:
			cthis->chni.lower_limit = Operate_in_tange(cthis->chni.lower_limit, op, val, -999999, 999999);
			self->to_string(self, chnaux_lower_limit, s);
			
			break;
		case chnaux_upper_limit:
			cthis->chni.upper_limit = Operate_in_tange(cthis->chni.upper_limit, op, val, -999999, 999999);
			self->to_string(self, chnaux_upper_limit, s);
			break;
		case chnaux_small_signal:
			cthis->chni.small_signal = Operate_in_tange(cthis->chni.small_signal, op, val, 0, 100);
			self->to_string(self, chnaux_small_signal, s);
			break;
		case chnaux_k:
			cthis->chni.k = Operate_in_tange(cthis->chni.k, op, val, -100, 100);
			self->to_string(self, chnaux_k, s);
			break;
		case chnaux_b:
			cthis->chni.b = Operate_in_tange(cthis->chni.b, op, val, -100, 100);
			self->to_string(self, chnaux_b, s);
			break;
		
		case alarm_hh:
			cthis->alarm.alarm_hh = Operate_in_tange(cthis->alarm.alarm_hh, op, val, 0, 0xffff);
			self->to_string(self, aux, s);
//			Pe_float(cthis->alarm.alarm_hh, 1, s);
			break;
		case alarm_hi:
			cthis->alarm.alarm_hi = Operate_in_tange(cthis->alarm.alarm_hi, op, val, 0, 0xffff);
//			Pe_float(cthis->alarm.alarm_hi, 1, s);
			self->to_string(self, aux, s);
			break;
		case alarm_lo:
			cthis->alarm.alarm_lo = Operate_in_tange(cthis->alarm.alarm_lo, op, val, 0, 0xffff);
//			Pe_float(cthis->alarm.alarm_lo, 1, s);
			self->to_string(self, aux, s);
			break;
		case alarm_ll:
			cthis->alarm.alarm_ll = Operate_in_tange(cthis->alarm.alarm_ll, op, val, 0, 0xffff);
//			Pe_float(cthis->alarm.alarm_ll, 1, (char *)s);
			self->to_string(self, aux, s);
			break;
		case tchspt_hh:
			cthis->alarm.touch_spot_hh = Operate_in_tange(cthis->alarm.touch_spot_hh, op, val, 1, MAX_TOUCHSPOT - 1);
			Pe_touch_spot(cthis->alarm.touch_spot_hh, (char *)s);
			break;
		case tchspt_hi:
			cthis->alarm.touch_spot_hi = Operate_in_tange(cthis->alarm.touch_spot_hi, op, val, 1, MAX_TOUCHSPOT - 1);
			Pe_touch_spot(cthis->alarm.touch_spot_hi, (char *)s);
			break;
		case tchspt_lo:
			cthis->alarm.touch_spot_lo = Operate_in_tange(cthis->alarm.touch_spot_lo, op, val, 1, MAX_TOUCHSPOT - 1);
			Pe_touch_spot(cthis->alarm.touch_spot_lo, (char *)s);	
			break;
		case tchspt_ll:
			cthis->alarm.touch_spot_ll = Operate_in_tange(cthis->alarm.touch_spot_ll, op, val, 1, MAX_TOUCHSPOT - 1);
			Pe_touch_spot(cthis->alarm.touch_spot_ll, (char *)s);	
			break;	
		case alarm_backlash:
			cthis->alarm.alarm_backlash = Operate_in_tange(cthis->alarm.alarm_backlash, op, val, 0, 100);
			Pe_float(cthis->alarm.alarm_backlash, 1, (char *)s);
			break;
		default:
			
			phn_sys.save_chg_flga &= ~CHG_MODCHN_CONF(cthis->chni.chn_NO);
			break;
		
		
	}
	self->setMdlData(self, aux, NULL);
	return RET_OK;
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
//		case chnaux_lower_limit:
//			Pe_float(cthis->chni.lower_limit, 1, (char *)arg);
//			break;
//		case chnaux_upper_limit:
//			Pe_float(cthis->chni.upper_limit, 1, (char *)arg);
//			break;
//		case chnauxsmall_signal:
//			Pe_float(cthis->chni.small_signal, 1, (char *)arg);
//			strcat((char *)arg, " %");
//			break;
//		case chnaux_k:
//			Pe_float(cthis->chni.k, 2, (char *)arg);
//			break;
//		case chnaux_b:
//			Pe_float(cthis->chni.k, 1, (char *)arg);
//			break;	
//		
//		default:
//			break;
//			
//		
//	}
//	

	return 0;
	
}

static int  MdlChn_to_percentage( Model *self, void *arg)
{
	Model_chn		*cthis = SUB_PTR( self, Model, Model_chn);
	uint8_t			*p = (uint8_t *)arg;
	
	*p = cthis->chni.value;
	
	return RET_OK;
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
		case AI_0_20_mV:
			sprintf(str, "0~20mV");
			break;
		case AI_0_100_mV:
			sprintf(str, "0~100mV");
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

static void Pe_touch_spot(int spot, char *str)
{
	
	if(spot < MAX_TOUCHSPOT && spot >= 0)
		sprintf(str, "%d", spot);
	else
		sprintf(str, "无");
}

static void MdlChn_Save_2_conf(mdl_chn_save_t *p_mcs, chn_info_t *p_cnf, uint8_t direct)
{
	
	if(direct == 0)
	{
		p_cnf->b = p_mcs->b;
		p_cnf->decimal = p_mcs->decimal;
		p_cnf->filter_time_s = p_mcs->filter_time_s;
		p_cnf->k = p_mcs->k;
		p_cnf->lower_limit = p_mcs->lower_limit;
		p_cnf->MB = p_mcs->MB;
		p_cnf->signal_type = p_mcs->signal_type;
		p_cnf->small_signal = p_mcs->small_signal;
		p_cnf->tag_NO = p_mcs->tag_NO;
		p_cnf->unit = p_mcs->unit;
		p_cnf->upper_limit = p_mcs->upper_limit;
		
		
		
		
	}
	else
	{
		p_mcs->b = p_cnf->b;
		p_mcs->decimal = p_cnf->decimal;
		p_mcs->filter_time_s = p_cnf->filter_time_s;
		p_mcs->k = p_cnf->k;
		p_mcs->lower_limit = p_cnf->lower_limit;
		p_mcs->MB = p_cnf->MB;
		p_mcs->signal_type = p_cnf->signal_type;
		p_mcs->small_signal = p_cnf->small_signal;
		p_mcs->tag_NO = p_cnf->tag_NO;
		p_mcs->unit = p_cnf->unit;
		p_mcs->upper_limit = p_cnf->upper_limit;
		
	}
	
}

static void MdlChn_Save_2_alarm(mdl_chn_save_t *p_mcs, chn_alarm_t *p_alr, uint8_t direct)
{
	if(direct == 0)
	{
		p_alr->alarm_backlash = p_mcs->alarm_backlash;
		p_alr->alarm_hh = p_mcs->alarm_hh;
		p_alr->alarm_hi = p_mcs->alarm_hi;
		p_alr->alarm_ll = p_mcs->alarm_ll;
		p_alr->alarm_lo = p_mcs->alarm_lo;
		p_alr->touch_spot_hh = p_mcs->touch_spot_hh;
		p_alr->touch_spot_hi = p_mcs->touch_spot_hi;
		p_alr->touch_spot_ll = p_mcs->touch_spot_ll;
		p_alr->touch_spot_lo = p_mcs->touch_spot_lo;
		
		
		
		
		
	}
	else
	{
		p_mcs->alarm_backlash = p_alr->alarm_backlash;
		p_mcs->alarm_hh = p_alr->alarm_hh;
		p_mcs->alarm_hi = p_alr->alarm_hi;
		p_mcs->alarm_ll = p_alr->alarm_ll;
		p_mcs->alarm_lo = p_alr->alarm_lo;
		p_mcs->touch_spot_hh = p_alr->touch_spot_hh;
		p_mcs->touch_spot_hi = p_alr->touch_spot_hi;
		p_mcs->touch_spot_ll = p_alr->touch_spot_ll;
		p_mcs->touch_spot_lo = p_alr->touch_spot_lo;
		
	}
	
	
}

//static int Str_float_to_int(char *str, int prec)
//{
//	
//	return 0;
//}

