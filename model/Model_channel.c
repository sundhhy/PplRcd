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

#define 	DEF_TYPE_MAX		13
const int16_t	def_lower_up_limit[14][2] = {
	{250,1820},			//B
	{-148,1000},		//E
	{-148,1200},		//J
	{-148,1370},		//K
	{-50,1760},			//S
	{-400,4000},		//T
	{-999,8500},		//pt100
	{-500,1500},		//Cu50	
	{0,1000},			//0 - 20 mv
	{0,1000},			//0 - 10 mv
	{0,1000},			//0 - 5V
	{0,1000},			//1 - 5V
	{0,1000},			//0 - 10mA
	{0,1000},			//4 - 20mA
		
};

const uint8_t	def_decimal_places[14] = {
	1,			//B
	1,		//E
	1,		//J
	1,		//K
	1,			//S
	1,		//T
	1,		//pt100
	1,		//Cu50	
	0,			//0 - 20 mv
	0,			//0 - 10 mv
	0,			//0 - 5V
	0,			//1 - 5V
	0,			//0 - 10mA
	0,			//4 - 20mA
		
};
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
#define  TDD_SAVE_DATA		0
#define UART_WAIT_AFTER_WRITE_MS		10
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


static void MdlChn_Init_alm_mgr_by_STG_alm(Model_chn *cthis);

static void MdlChn_Save_2_conf(mdl_chn_save_t *p_mcs, chn_info_t *p_cnf, uint8_t direct);
static void MdlChn_Save_2_alarm(mdl_chn_save_t *p_mcs, chn_alarm_t *p_alr, uint8_t direct);

static	int16_t Zero_shift_K_B(chn_info_t *p, int16_t	d);
static	int16_t Cut_small_signal(chn_info_t *p, int16_t	d);
static 	int16_t	MdlChn_Get_def_lower_limit(uint8_t t);
static 	int16_t	MdlChn_Get_def_up_limit(uint8_t t);
static void Signal_Alarm(Model_chn *cthis);
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
	if(p_mc  == NULL) while(1);
	p_mdl = SUPER_PTR(p_mc, Model);
	arr_p_mdl_chn[n] = p_mdl;
//	p_mdl->init(p_mdl, (void *)&n);
	
	
	return p_mc;
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


uint8_t  MdlChn_Cal_prc(Model *self, int val)
{
	Model_chn		*cthis = SUB_PTR( self, Model, Model_chn);
	uint8_t			prc = 0;
	
	if(val <= cthis->chni.lower_limit)
		prc = 0;
	else if(val >= cthis->chni.upper_limit)
		prc = 100;
	else
	{
		prc = ((val - cthis->chni.lower_limit) * 100) / (cthis->chni.upper_limit - cthis->chni.lower_limit);
		
	}
	
	
	
	return prc;
}
void MdlChn_default_conf(int chn_num)
{
	Model_chn *p_mdl= Get_Mode_chn(chn_num);
	
	memset(&p_mdl->chni, 0, sizeof(p_mdl->chni));
	p_mdl->chni.signal_type = AI_Pt100;
	p_mdl->chni.unit = eu_oC;
	p_mdl->chni.chn_NO = chn_num;
	p_mdl->chni.tag_NO = chn_num;
	p_mdl->chni.MB = 2;
	p_mdl->chni.k = 100;
	p_mdl->chni.b = 0;
	p_mdl->chni.lower_limit = MdlChn_Get_def_lower_limit(p_mdl->chni.signal_type);
	p_mdl->chni.upper_limit = MdlChn_Get_def_up_limit(p_mdl->chni.signal_type);
	p_mdl->chni.decimal_places = def_decimal_places[p_mdl->chni.signal_type];
}


void MdlChn_default_alarm(int chn_num)
{
	Model_chn *p_mdl= Get_Mode_chn(chn_num);
	
	memset(&p_mdl->alarm, 0, sizeof(p_mdl->alarm));
	p_mdl->alarm.alarm_hh = 0x7fff;
	p_mdl->alarm.alarm_hi = 0x7fff;
	
	
}

void MdlChn_Clean_Alamr(int chn_num)
{
	Model_chn *p_mdl= Get_Mode_chn(chn_num);
	
	p_mdl->alarm.alm_flag = 0;
	p_mdl->alarm.num_alms_in_stg = 0;
	
	p_mdl->alarm_mgr.alm_hh_index = 0xff;
	p_mdl->alarm_mgr.alm_hi_index = 0xff;
	p_mdl->alarm_mgr.alm_lo_index = 0xff;
	p_mdl->alarm_mgr.alm_ll_index = 0xff;
	
	
}

int MdlChn_Commit_conf(int chn_num)
{
	Model_chn *p_mdl= Get_Mode_chn(chn_num);
	Model		*self = SUPER_PTR(p_mdl, Model);
	Storage		*stg = Get_storage();
	stg->wr_stored_data(stg, STG_CHN_CONF(chn_num), NULL, 0);
	
	self->setMdlData(self, AUX_SIGNALTYPE, NULL);
	self->setMdlData(self, chnaux_lower_limit, NULL);
	self->setMdlData(self, chnaux_upper_limit, NULL);
	
	STG_Resize(STG_CHN_DATA(chn_num), p_mdl->chni.MB * 1024 * 1024);
	
	return RET_OK;
}

//只是把读取冷端温度的请求记录，在定时器采集时才会真正读取
//这么做是为了避免多个线程对串口产生竞争
void MdlChn_Read_code_end_temperature()
{
	phn_sys.sys_flag |= SYSFLAG_READ_CET;
	
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
static 	int16_t	MdlChn_Get_def_lower_limit(uint8_t t)
{
	
	if(t > DEF_TYPE_MAX)
		return 0;
	
	return def_lower_up_limit[t][0];
}
static 	int16_t	MdlChn_Get_def_up_limit(uint8_t t)
{
	if(t > DEF_TYPE_MAX)
		return 30000;
	return def_lower_up_limit[t][1];
}
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

//检查指定的错误是否是新发生的
static void	MdlChn_Check_new_alarm(Model_chn *cthis, uint8_t new_flag, uint8_t alm_flag)
{
	uint8_t *p_index = NULL;
	Storage	*stg = NULL;
	int		ret_len;
	rcd_alm_pwr_t		rap;
	uint8_t					alm_code = 0;
	uint8_t					retry = 5;
	if(cthis->alarm.alm_flag & alm_flag)
		return;		//已经被记录了，直接退出
	if((new_flag & alm_flag) == 0)
		return;	
	//指定的报警是新产生的
	switch(alm_flag)
	{
		case ALM_HH:
			p_index = &cthis->alarm_mgr.alm_hh_index;
			alm_code = ALM_CODE_HH;
			break;
		case ALM_HI:
			p_index = &cthis->alarm_mgr.alm_hi_index;
			alm_code = ALM_CODE_HI;
			break;
		case ALM_LO:
			p_index = &cthis->alarm_mgr.alm_lo_index;
			alm_code = ALM_CODE_LO;
			break;
		case ALM_LL:
			p_index = &cthis->alarm_mgr.alm_ll_index;
			alm_code = ALM_CODE_LL;
			break;
	}
	if(p_index == NULL)
		return;
	
	stg = Get_storage();

	
	*p_index = cthis->alarm.num_alms_in_stg;
	cthis->alarm.num_alms_in_stg ++;
	cthis->alarm.num_alms_in_stg %= STG_MAX_NUM_CHNALARM;
	
	rap.alm_pwr_type = alm_code;
	rap.flag = 1;
	rap.happen_time_s = SYS_time_sec();
	rap.disapper_time_s = 0xffffffff;
	
	
	while(STG_Set_file_position(STG_CHN_ALARM(cthis->chni.chn_NO), STG_DRC_WRITE, *p_index * sizeof(rcd_alm_pwr_t)) < 0)
		delay_ms(1);
	while(1)
	{
		
		ret_len = stg->wr_stored_data(stg, STG_CHN_ALARM(cthis->chni.chn_NO), &rap, sizeof(rcd_alm_pwr_t));
		if(ret_len == 0)
		{
			while(STG_Set_file_position(STG_CHN_ALARM(cthis->chni.chn_NO), STG_DRC_WRITE, 0) < 0)
				delay_ms(1);
			cthis->alarm.num_alms_in_stg = 0;
		}
		if(ret_len == sizeof(rcd_alm_pwr_t))
			break;
		if(retry)
			retry --;
		else
			break;
		
	}
	
}


//通过对比新的报警状态和旧的报警状态，来判断旧的报警是否被取消了
//需要注意的是，如果报警从一种报警，迁移到另外一种报警的时候，也要记录取消时间
static void	MdlChn_Cancle_alarm(Model_chn *cthis, uint8_t new_flag, uint8_t alm_flag)
{
	uint8_t *p_index = NULL;
	Storage	*stg = NULL;
	uint32_t		dsp_time;
//	uint8_t					alm_code = 0;
	uint8_t					retry = 5;
	
	//只有就的报警状态上存在的记录，并且在新的报警状态中不存在的报警，才能说明该报警被取消掉了
	
	if((cthis->alarm.alm_flag & alm_flag) == 0)
		return;		//报警未发生过，也就没有取消报警的说法了
	if((new_flag & alm_flag) )
		return;		//报警还存在，也说明该报警没有取消

	switch(alm_flag)
	{
		case ALM_HH:
			p_index = &cthis->alarm_mgr.alm_hh_index;
//			alm_code = ALM_CODE_HH;
			break;
		case ALM_HI:
			p_index = &cthis->alarm_mgr.alm_hi_index;
//			alm_code = ALM_CODE_HI;
			break;
		case ALM_LO:
			p_index = &cthis->alarm_mgr.alm_lo_index;
//			alm_code = ALM_CODE_LO;
			break;
		case ALM_LL:
			p_index = &cthis->alarm_mgr.alm_ll_index;
//			alm_code = ALM_CODE_LL;
			break;
	}
	if(p_index == NULL)
		return;
	if(*p_index == 0xff)
		return;
	
	stg = Get_storage();
	

	dsp_time = SYS_time_sec();

	
	STG_Set_file_position(STG_CHN_ALARM(cthis->chni.chn_NO), STG_DRC_WRITE, *p_index * sizeof(rcd_alm_pwr_t) +(int)(&((rcd_alm_pwr_t *)0)->disapper_time_s));
	while(stg->wr_stored_data(stg, STG_CHN_ALARM(cthis->chni.chn_NO), &dsp_time, sizeof(uint32_t)) != sizeof(uint32_t))
	{
		
		if(retry)
			retry --;
		else
			break;
		
	}
	
}
static void Signal_Alarm(Model_chn *cthis)
{
	chn_info_t *p = &cthis->chni;
	chn_alarm_t *p_alm = &cthis->alarm;
	uint8_t flag, new_flag = 0;
	int16_t tempS2,tempS3,bjhc, prc_data;
	int32_t temps4;
	
	if(phn_sys.sys_flag & SYSFLAG_POWEROFF)
		return;

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
		if(p->value < tempS2)
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
	
	//检查是否有报警被消除
	MdlChn_Cancle_alarm(cthis, new_flag, ALM_HH);
	MdlChn_Cancle_alarm(cthis, new_flag, ALM_HI);
	MdlChn_Cancle_alarm(cthis, new_flag, ALM_LO);
	MdlChn_Cancle_alarm(cthis, new_flag, ALM_LL);
	
	
	//检查是否产生新的报警
	MdlChn_Check_new_alarm(cthis, new_flag, ALM_HH);
	MdlChn_Check_new_alarm(cthis, new_flag, ALM_HI);
	MdlChn_Check_new_alarm(cthis, new_flag, ALM_LO);
	MdlChn_Check_new_alarm(cthis, new_flag, ALM_LL);

	
	
	p_alm->alm_flag = new_flag;
}

static	int16_t Zero_shift_K_B(chn_info_t *p, int16_t	d)
{
	int32_t	tmp_s32;
	int16_t 	rst = d;
	
//	if(p->k == 0)
//		goto exit;
	
	//k是2位小数,b是整数
	tmp_s32 = p->k*(long)d/100 + p->b;
	if( tmp_s32 > p->upper_limit) 	
		tmp_s32 = p->upper_limit;
	
	if( tmp_s32 < p->lower_limit) 	
		tmp_s32 = p->lower_limit;
	
	
	rst = tmp_s32;
//	exit:
	return rst;
}

static	int16_t Cut_small_signal(chn_info_t *p, int16_t	d)
{
	
	int32_t	tmp_s32;
	int16_t 	rst = d;
	
	if(p->small_signal == 0)
		goto exit;
	
	//small_signal 是1位小数的百分数
	tmp_s32 = p->upper_limit - p->lower_limit;
	tmp_s32 = tmp_s32 * p->small_signal / 1000;
	if(rst < tmp_s32)
		rst = p->lower_limit;
	
	exit:
	return rst;
}



static int MdlChn_init(Model *self, IN void *arg)
{
	Model_chn					*cthis = SUB_PTR(self, Model, Model_chn);
	Storage						*stg = Get_storage();
	mdl_chn_save_t				save;
	uint8_t						chn_num = *((uint8_t *)arg);
	
	I_dev_Char 		*I_uart3 = NULL;
	
	Dev_open(DEVID_UART3, ( void *)&I_uart3);
	I_uart3->ioctol(I_uart3, DEVCMD_SET_TXWAITTIME_MS, 100);
	I_uart3->ioctol(I_uart3, DEVCMD_SET_RXWAITTIME_MS, 10);
	
	cthis->str_buf = CALLOC(1,8);
	cthis->unit_buf = CALLOC(1,8);
	cthis->alarm_buf = CALLOC(1,8);
	self->mdl_id = MDLID_CHN(chn_num);
	cthis->chni.chn_NO = chn_num;
	if(stg->rd_stored_data(stg, STG_CHN_CONF(cthis->chni.chn_NO), &save, sizeof(save)) != RET_OK) 
	{

		MdlChn_default_conf(chn_num);
		MdlChn_default_alarm(chn_num);
		MdlChn_Commit_conf(chn_num);		//保存默认设置到存储器
	}		
	else
	{
		MdlChn_Save_2_conf(&save, &cthis->chni, 0);
		MdlChn_Save_2_alarm(&save, &cthis->alarm, 0);
		
		if(cthis->chni.signal_type > es_max)
		{
			
			MdlChn_default_conf(chn_num);
			MdlChn_default_alarm(chn_num);
			MdlChn_Commit_conf(chn_num);
		}
		stg->wr_stored_data(stg, STG_CHN_CONF(cthis->chni.chn_NO), NULL, 0);
		
	}
	
	MdlChn_Init_alm_mgr_by_STG_alm(cthis);
	cthis->chni.decimal_places = def_decimal_places[cthis->chni.signal_type];

	stg->open_file(STG_CHN_DATA(cthis->chni.chn_NO), cthis->chni.MB * 1024 * 1024);
	return RET_OK;
}



//从存储器中读取报警的记录信息，来确定本次初始化的报警存储起始位置
static void MdlChn_Init_alm_mgr_by_STG_alm(Model_chn *cthis)
{
	
	Storage						*stg = Get_storage();
	rcd_alm_pwr_t			stg_alm = {0};
	int								num_alm = 0;
	
	STG_Set_file_position(STG_CHN_ALARM(cthis->chni.chn_NO), STG_DRC_READ, 0);
	while(stg_alm.flag != 0xff)
	{
		if(stg->rd_stored_data(stg, STG_CHN_ALARM(cthis->chni.chn_NO), \
			&stg_alm, sizeof(rcd_alm_pwr_t)) != sizeof(rcd_alm_pwr_t))
			{
				
				//或者已经读完了
				break;
				
				
			}
			if(stg_alm.flag != 0xff)
				num_alm ++;
		
	}
	
	if(num_alm == STG_MAX_NUM_CHNALARM)
		num_alm = 0;		//报警内存都满了，就重头开始覆盖
	cthis->alarm.num_alms_in_stg = num_alm;
	
	cthis->alarm_mgr.alm_hh_index = 0xff;
	cthis->alarm_mgr.alm_hi_index = 0xff;
	cthis->alarm_mgr.alm_lo_index = 0xff;
	cthis->alarm_mgr.alm_ll_index = 0xff;
	
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
	int16_t					save_buf[2];  //存放实时值及小数点位数
	SmBus_result_t		rst;
//	do_out_t			d = {0};
	
	I_dev_Char 			*I_uart3 = NULL;
	uint8_t 			i;
//	uint8_t				old_do;
	
#if TDD_SAVE_DATA == 1
	short	test_val;
	static short mdl_sub = 0;
	static short mdl_step = 20;
	test_val = cthis->chni.value;
	
	
	cthis->chni.lower_limit = -1000;
	cthis->chni.upper_limit = 1000;
	//decimal_places 在这里用于测试
	if(mdl_sub == 0)
	{
		test_val += mdl_step;
		if(test_val > cthis->chni.upper_limit)
		{
			test_val = cthis->chni.upper_limit - 1;
			mdl_sub = 1;
		}
	}
	else
	{
		if(test_val > cthis->chni.lower_limit)
		{
			test_val -= mdl_step;
			
		}
		else
		{
			test_val = cthis->chni.lower_limit + 1;
//			test_val = 0;
			mdl_sub = 0;
			
		}
		
	}
	cthis->alarm.alarm_hh = cthis->chni.upper_limit - 2 * mdl_step;
	cthis->alarm.alarm_hi = cthis->chni.upper_limit - 1 * mdl_step;
	
	cthis->alarm.alarm_lo = cthis->chni.lower_limit + 2 * mdl_step;
	cthis->alarm.alarm_ll = cthis->chni.lower_limit + 1 * mdl_step;
	
	
	
	test_val = Zero_shift_K_B(&cthis->chni, test_val);
	test_val = Cut_small_signal(&cthis->chni, test_val);
	cthis->chni.value = test_val;
	
	Signal_Alarm(cthis);
	
	self->notify(self);
	save_buf[0] = cthis->chni.value;
	save_buf[1] = cthis->chni.decimal_places;
	stg->wr_stored_data(stg, STG_CHN_DATA(cthis->chni.chn_NO), save_buf, 4);
#else	
	
	
	Dev_open(DEVID_UART3, ( void *)&I_uart3);
	
	if(phn_sys.sys_flag & SYSFLAG_READ_CET)
	{
		i = SmBus_AI_Read(CDT_CHN, AI_READ_ENGVAL, chk_buf, 16);
		if( I_uart3->write(I_uart3, chk_buf, i) != RET_OK)
			goto rd_smpval;
		delay_ms(UART_WAIT_AFTER_WRITE_MS);
		i = I_uart3->read(I_uart3, chk_buf, 16);
		if(i <= 0)
			goto rd_smpval;
		if(SmBus_decode(SMBUS_AI_READ, chk_buf, &rst, sizeof(SmBus_result_t)) != RET_OK)
			goto rd_smpval;
		
		phn_sys.code_end_temperature = rst.val;
		phn_sys.sys_flag &= ~SYSFLAG_READ_CET;	
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
	delay_ms(UART_WAIT_AFTER_WRITE_MS);
	read_again:
	i = I_uart3->read(I_uart3, chk_buf, 16);
	if(i <= 0)
		goto err;
	if(SmBus_decode(SMBUS_AI_READ, chk_buf, &rst, sizeof(SmBus_result_t)) != RET_OK)
		goto err;

	if(rst.chn_num != cthis->chni.chn_NO)
	{
		//可能数据传了，重新读就行了
		goto read_again;
	}
	else
	{
		cthis->chni.flag_err = 0;
	}
	
	rst.val = Zero_shift_K_B(&cthis->chni, rst.val);
	rst.val = Cut_small_signal(&cthis->chni, rst.val);
	if(phn_sys.sys_conf.cold_end_way)
	{
		
		rst.val -= phn_sys.sys_conf.CJC;
	}
	if(rst.val != cthis->chni.value)
	{
		
		
		cthis->chni.signal_type = rst.signal_type;
		cthis->chni.value = rst.val;
//		if((phn_sys.sys_flag & SYSFLAG_SETTING) == 0)
			self->notify(self);
		
		Signal_Alarm(cthis);
		
	}
	
	save_buf[0] = cthis->chni.value;
	save_buf[1] = cthis->chni.decimal_places;
	stg->wr_stored_data(stg, STG_CHN_DATA(cthis->chni.chn_NO), save_buf, 4);
	

	return;
	err:
		cthis->chni.flag_err = 1;
	
#endif
	
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
			#if TDD_SAVE_DATA == 1
			if(aux == chnaux_upper_limit)
				*p_s16 = cthis->chni.upper_limit;
			else
				*p_s16 = cthis->chni.lower_limit;
			break;
			#endif
		
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
//	uint16_t		*p_u16;
	int16_t			*p_s16;
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
//				break;
				p_u8 = &cthis->chni.signal_type;
			
//			if(*p_u8 == cthis->chni.signal_type)
//				break;

			sb_conf.signal_type = *p_u8;
			sb_conf.decimal = cthis->chni.decimal;
			
			
			cthis->chni.lower_limit = MdlChn_Get_def_lower_limit(sb_conf.signal_type);
			cthis->chni.upper_limit = MdlChn_Get_def_up_limit(sb_conf.signal_type);
		
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
			
//			i = SmBus_rd_signal_type(SMBUS_MAKE_CHN(SMBUS_CHN_AI, cthis->chni.chn_NO), sbub_buf, 32);
//			if( I_uart3->write(I_uart3, sbub_buf, i) != RET_OK)
//				return ERR_OPT_FAILED;
//			i = I_uart3->read(I_uart3, sbub_buf, 32);
//			if(i <= 0)
//				return ERR_OPT_FAILED;
//			if(SmBus_decode(SMBUS_CMD_READ, sbub_buf, &tmp_u8, 1) != RET_OK)
//				return ERR_OPT_FAILED;
			
			self->getMdlData(self, AUX_SIGNALTYPE, &tmp_u8);
			if(sb_conf.signal_type != tmp_u8)
			{
				
				return ERR_OPT_FAILED;
			}
			
			return RET_OK;
			
			
			
		case chnaux_lower_limit:
		case chnaux_upper_limit:
			
			
			if(aux == chnaux_upper_limit)
			{
				if(arg == NULL)
					p_s16 = &cthis->chni.upper_limit;
				else
					p_s16 = arg;
//				cthis->chni.upper_limit = *(uint16_t *)arg; 
				i = SmBus_WR_hig_limit(SMBUS_MAKE_CHN(SMBUS_CHN_AI, cthis->chni.chn_NO), p_s16, sbub_buf, 32);
				
			}
			else
			{
				if(arg == NULL)
					p_s16 = &cthis->chni.lower_limit;
				else
					p_s16 = arg;
//				cthis->chni.lower_limit = *(uint16_t *)arg; 
				i = SmBus_WR_low_limit(SMBUS_MAKE_CHN(SMBUS_CHN_AI, cthis->chni.chn_NO),  p_s16, sbub_buf, 32);
				
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
				
				if( cthis->chni.upper_limit == *(int16_t *)arg)
					return RET_OK;
				
			}
			else 
			{
				
				if(cthis->chni.lower_limit == *(int16_t *)arg)
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
			
		case chnaux_k:
			if(arg == NULL)
				break;
			p_s16 = (int16_t *)arg;
			cthis->chni.k = *p_s16;
			break;
			
		case chnaux_b:
			if(arg == NULL)
				break;
			p_s16 = (int16_t *)arg;
			cthis->chni.b = *p_s16;
			break;
		case chnaux_small_signal:
			if(arg == NULL)
				break;
			p_s16 = (int16_t *)arg;
			cthis->chni.small_signal = *p_s16;
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
//	int 			hh = cthis->alarm.alarm_hh;
//	int 			hi = cthis->alarm.alarm_hi;
//	int 			li = cthis->alarm.alarm_lo;
//	int 			ll = cthis->alarm.alarm_ll;
//	int				i;
	
	
	switch(aux) {
		case AUX_DATA:
			if( arg) {
				p = (char *)arg;		
			} else {
				p = cthis->str_buf;
			}
			Print_float(cthis->chni.value, 6,  cthis->chni.decimal_places, p);
//			sprintf( p, "%4d.%d", cthis->chni.value/10, cthis->chni.value%10);
			return p;
			

		case AUX_UNIT:
			if( arg) {
				p = (char *)arg;		
			} else {
				p = cthis->unit_buf;
			}
			Print_unit( cthis->chni.unit, p, 8);
			return p;
		case AUX_ALARM:
			if( arg) {
				p = (char *)arg;		
			} else {
				p = cthis->alarm_buf;
			}
		
			memset(p, 0, 8);
			if(cthis->alarm.alm_flag & ALM_HH)
			{
				strcat(p, "HH");
				
			}
			else if(cthis->alarm.alm_flag & ALM_HI)
			{
				strcat(p, "HI");
				
			}
			else if(cthis->alarm.alm_flag & ALM_LO)
			{
				strcat(p, "LO");
				
			}
			else if(cthis->alarm.alm_flag & ALM_LL)
			{
				strcat(p, "LL");
				
			}
			else
			{
				
				strcat(p, "  ");
			}
			
//			if(cthis->chni.value > hh) {
//				strcat(p, "HH ");
//			}
//			
//			if(cthis->chni.value > hi) {
//				strcat(p, "Hi");
//			}
//			
//			if(cthis->chni.value < li) {
//				strcat(p, "Li ");
//			}
//			
//			if(cthis->chni.value < ll) {
//				strcat(p, "LL");
//			}
//			
//			//把空间填满，来达到清除掉上一次的显示的目的
//			for(i = strlen(p); i < 5; i++)
//			{
//				
//				p[i] = ' ';
//				
//			}
			
			
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
			Print_singnaltype((e_signal_t)cthis->chni.signal_type, (char *)arg);
			break;
		case chnaux_record_mb:
			sprintf(arg, "%2d M", cthis->chni.MB);
			break;
		case chnaux_filter_ts:
			sprintf(arg, "%2d S", cthis->chni.filter_time_s);
			break;
		case chnaux_lower_limit:
			//温度信号没有小数点
//			if(cthis->chni.signal_type <= AI_Cu50)
//				sprintf(arg, "%-6d", cthis->chni.lower_limit);
//			else
				Print_float(cthis->chni.lower_limit, 6, cthis->chni.decimal_places, (char *)arg);
			break;
		case chnaux_upper_limit:
//			if(cthis->chni.signal_type <= AI_Cu50)
//				sprintf(arg, "%-6d", cthis->chni.upper_limit);
//			else
				Print_float(cthis->chni.upper_limit, 6, cthis->chni.decimal_places, (char *)arg);
			break;
		case chnaux_small_signal:
			Print_float(cthis->chni.small_signal, 2, 1, (char *)arg);
			strcat((char *)arg, " %");
			break;
		case chnaux_k:
//			Print_frefix_float(cthis->chni.k, 2, "K:",(char *)arg);
			Print_float(cthis->chni.k, 2 , 2, (char *)arg);
			break;
		case chnaux_b:
//			Print_frefix_float(cthis->chni.b, 1, "B:", (char *)arg);
//			Print_float(cthis->chni.b, 1, (char *)arg);
			sprintf(arg, "%-3d", cthis->chni.b);
			break;	
	
		case alarm_hh:
			sprintf(arg, "%-6d", cthis->alarm.alarm_hh);
//			Print_float(cthis->alarm.alarm_hh, 1, (char *)arg);
			break;
		case alarm_hi:
			sprintf(arg, "%-6d", cthis->alarm.alarm_hi);
//			Print_float(cthis->alarm.alarm_hi, 1, (char *)arg);
			break;
		case alarm_lo:
			sprintf(arg, "%-5d", cthis->alarm.alarm_lo);
//			Print_float(cthis->alarm.alarm_lo, 1, (char *)arg);
			break;
		case alarm_ll:
			sprintf(arg, "%-5d", cthis->alarm.alarm_ll);
//			Print_float(cthis->alarm.alarm_ll, 1, (char *)arg);
			break;
		case tchspt_hh:
			Print_touch_spot(cthis->alarm.touch_spot_hh, (char *)arg);
			break;
		case tchspt_hi:
			Print_touch_spot(cthis->alarm.touch_spot_hi, (char *)arg);
			break;
		case tchspt_lo:
			Print_touch_spot(cthis->alarm.touch_spot_lo, (char *)arg);	
			break;
		case tchspt_ll:
			Print_touch_spot(cthis->alarm.touch_spot_ll, (char *)arg);	
			break;	
		case alarm_backlash:
			Print_float(cthis->alarm.alarm_backlash, 2, 1, (char *)arg);
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
			cthis->chni.lower_limit = Operate_in_tange(cthis->chni.lower_limit, op, val, -30000, cthis->chni.upper_limit - 1);
			self->to_string(self, chnaux_lower_limit, s);
			
			break;
		case chnaux_upper_limit:
			cthis->chni.upper_limit = Operate_in_tange(cthis->chni.upper_limit, op, val, cthis->chni.lower_limit + 1, 30000);
			self->to_string(self, chnaux_upper_limit, s);
			break;
		case chnaux_small_signal:
			cthis->chni.small_signal = Operate_in_tange(cthis->chni.small_signal, op, val, 0, 100);
			self->to_string(self, chnaux_small_signal, s);
			break;
		case chnaux_k:
			cthis->chni.k = Operate_in_tange(cthis->chni.k, op, val, -999, 999);
			self->to_string(self, chnaux_k, s);
			break;
		case chnaux_b:
			cthis->chni.b = Operate_in_tange(cthis->chni.b, op, val, -999, 999);
			self->to_string(self, chnaux_b, s);
			break;
		
		case alarm_hh:
			cthis->alarm.alarm_hh = Operate_in_tange(cthis->alarm.alarm_hh, op, val, 0, 0xffff);
			self->to_string(self, aux, s);
//			Print_float(cthis->alarm.alarm_hh, 1, s);
			break;
		case alarm_hi:
			cthis->alarm.alarm_hi = Operate_in_tange(cthis->alarm.alarm_hi, op, val, 0, 0xffff);
//			Print_float(cthis->alarm.alarm_hi, 1, s);
			self->to_string(self, aux, s);
			break;
		case alarm_lo:
			cthis->alarm.alarm_lo = Operate_in_tange(cthis->alarm.alarm_lo, op, val, 0, 0xffff);
//			Print_float(cthis->alarm.alarm_lo, 1, s);
			self->to_string(self, aux, s);
			break;
		case alarm_ll:
			cthis->alarm.alarm_ll = Operate_in_tange(cthis->alarm.alarm_ll, op, val, 0, 0xffff);
//			Print_float(cthis->alarm.alarm_ll, 1, (char *)s);
			self->to_string(self, aux, s);
			break;
		case tchspt_hh:
			cthis->alarm.touch_spot_hh = Operate_in_tange(cthis->alarm.touch_spot_hh, op, val, 1, MAX_TOUCHSPOT - 1);
			Print_touch_spot(cthis->alarm.touch_spot_hh, (char *)s);
			break;
		case tchspt_hi:
			cthis->alarm.touch_spot_hi = Operate_in_tange(cthis->alarm.touch_spot_hi, op, val, 1, MAX_TOUCHSPOT - 1);
			Print_touch_spot(cthis->alarm.touch_spot_hi, (char *)s);
			break;
		case tchspt_lo:
			cthis->alarm.touch_spot_lo = Operate_in_tange(cthis->alarm.touch_spot_lo, op, val, 1, MAX_TOUCHSPOT - 1);
			Print_touch_spot(cthis->alarm.touch_spot_lo, (char *)s);	
			break;
		case tchspt_ll:
			cthis->alarm.touch_spot_ll = Operate_in_tange(cthis->alarm.touch_spot_ll, op, val, 1, MAX_TOUCHSPOT - 1);
			Print_touch_spot(cthis->alarm.touch_spot_ll, (char *)s);	
			break;	
		case alarm_backlash:
			cthis->alarm.alarm_backlash = Operate_in_tange(cthis->alarm.alarm_backlash, op, val, 0, 100);
			Print_float(cthis->alarm.alarm_backlash, 2, 1, (char *)s);
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
//			Print_unit( cthis->chni.unit, p, 8);
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
//			Print_singnaltype((e_signal_t)cthis->chni.signal_type, (char *)arg);
//			break;
//		case chnaux_lower_limit:
//			Print_float(cthis->chni.lower_limit, 1, (char *)arg);
//			break;
//		case chnaux_upper_limit:
//			Print_float(cthis->chni.upper_limit, 1, (char *)arg);
//			break;
//		case chnauxsmall_signal:
//			Print_float(cthis->chni.small_signal, 1, (char *)arg);
//			strcat((char *)arg, " %");
//			break;
//		case chnaux_k:
//			Print_float(cthis->chni.k, 2, (char *)arg);
//			break;
//		case chnaux_b:
//			Print_float(cthis->chni.k, 1, (char *)arg);
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
//	uint8_t			prc = 0;
//	
//	if(cthis->chni.value <= cthis->chni.lower_limit)
//		prc = 0;
//	else if(cthis->chni.value >= cthis->chni.upper_limit)
//		prc = 100;
//	else
//	{
//		prc = ((cthis->chni.value - cthis->chni.lower_limit) * 100) / (cthis->chni.upper_limit - cthis->chni.lower_limit);
//		
//	}
	
	
	*p = MdlChn_Cal_prc(self, cthis->chni.value);
	
	return RET_OK;
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

