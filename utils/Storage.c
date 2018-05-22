#include "Storage.h"
#include "sdhDef.h"
#include <string.h>
#include "log.h"
//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//记录文件结构：[ 掉电记录 | 通道报警记录 * NUM_CHANNEL | 累计值]

#define STG_LSTPWR_FILE_OFFSET			0
#define STG_PWR_FILE_SIZE				STG_MAX_NUM_LST_PWR * sizeof(rcd_alm_pwr_t)
	
#define STG_ALARM_FILE_OFFSET			STG_PWR_FILE_SIZE
#define STG_CHN_ALARM_FILE_SIZE 		STG_MAX_NUM_CHNALARM * sizeof(rcd_alm_pwr_t)
#define STG_ALARM_FILE_SIZE				STG_CHN_ALARM_FILE_SIZE * NUM_CHANNEL
	
#define STG_SUM_FILE_OFFSET				STG_ALARM_FILE_OFFSET + STG_ALARM_FILE_SIZE
#define STG_CHN_SUM_FILE_SIZE 			sizeof(rcd_chn_accumlated_t)
#define STG_SUM_FILE_SIZE 				NUM_CHANNEL * STG_CHN_SUM_FILE_SIZE


#define STG_PAS_SIZE					(STG_PWR_FILE_SIZE  + STG_ALARM_FILE_SIZE + STG_SUM_FILE_SIZE)
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
#define STRG_SYS				phn_sys
#define STRG_RCD_FSH_NUM		FSH_FM25_NUM
#define STRG_CFG_FSH_NUM		FSH_FM25_NUM
#define STRG_CHN_DATA_FSH_NUM	FSH_W25Q_NUM
#define RCD_ERR					1
#define RCD_READED			2
#define NUM_SAVE_DATA		4
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------


#if STG_RCD_FULL_ACTION != STG_ERASE
//通过缓存，将分散的通道记录，汇集起来，然后一致的存入FLASH
typedef struct {
	uint16_t			cur_idx;
	uint16_t			buf_size;
	uint8_t				*p_buf;
}STG_wr_buf_mgr_t;
	
typedef struct {
	uint32_t		time_s;
	uint16_t		val;
	uint8_t			flag;
	uint8_t			chn;
}STG_cache_t;
#endif

//通道数据存储是先存着，然后在另外的线程里面进行存储。这样减轻采集线程的负担。
typedef struct {
	uint8_t			decimal_point;
	uint8_t			none;
	int16_t		value;
	uint32_t	time_s;
	
	
}save_data_t;

typedef struct {
	save_data_t		arr_save_data[NUM_CHANNEL][NUM_SAVE_DATA];
	uint8_t				arr_head[NUM_CHANNEL];
	uint8_t				arr_tail[NUM_CHANNEL];
	
	
}save_data_server_t;
//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
//mdl_observer		strg_mdl_ob;
#if STG_RCD_FULL_ACTION != STG_ERASE
static uint8_t			stg_buf[4096];
static STG_wr_buf_mgr_t stg_wr_mgr = {0, 4096, stg_buf};
#endif

save_data_server_t		sds;
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static int Strg_init(Storage *self);
static int	Strg_rd_stored_data(Storage *self, uint8_t	cfg_type, void *buf, int len);
static int		Strg_WR_stored_data(Storage *self, uint8_t	cfg_type, void *buf, int len);
/* Cycle/Sync Callback functions */
//static int	Strg_RD_chn_conf(uint8_t type, void *p);
//static int	Strg_RD_sys_conf(uint8_t type, void *p);
//static int	Strg_WR_chn_conf(uint8_t type, void *p);
//static int	Strg_WR_sys_conf(uint8_t type, void *p);
static void Strg_Updata_rcd_mgr(uint8_t	num, mdl_chn_save_t *p);

static int 	STG_Open_file(uint8_t type, uint32_t file_size);

static int	STG_Acc_chn_conf(uint8_t	tp, uint8_t	drc, void *p);
static int	STG_Acc_chn_alarm(uint8_t	type, uint8_t	drc, void *p, int len);
static int	STG_Acc_chn_data(uint8_t	type, uint8_t	drc, void *p, int len);
static int	STG_Acc_chn_sum(uint8_t	type, uint8_t	drc, void *p, int len);
static int	STG_Acc_file(uint8_t	type, uint8_t	drc, void *p, int len);
static int	STG_Acc_sys_conf(uint8_t	drc, void *p);
static int	STG_Acc_lose_pwr(uint8_t	drc, void *p, int len);

static int STG_Read_chn_data_will_retry(int f, int retry, data_in_fsh_t *dif);


//------- 记录通道数据的函数--------------------------------------------------//
#if STG_RCD_FULL_ACTION != STG_ERASE
static void STG_WR_cache_mgr(void);
static void STG_flush_wr_cache(uint8_t type);
#endif
static int STG_En_save_data(uint8_t chn, uint8_t dp, uint16_t value, uint32_t time_s);
static int STG_Out_save_data(uint8_t chn, data_in_fsh_t	*dif);
static void STG_Remove_save_data(uint8_t chn);
static void STG_Clean_save_data(uint8_t chn);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
Storage		*Get_storage()
{
	static Storage *sington_strg = NULL;
	
	if(sington_strg == NULL)
	{
		sington_strg = Storage_new();
		if(sington_strg == NULL)
			while(1);
	}
	
	
	return sington_strg;
	
}


void STG_Run(void)
{
	
	Storage				*stg = Get_storage();
	data_in_fsh_t	d;
	int						fd = -1;
	int						acc_len = 0;
	file_info_t		*fnf ;
	uint8_t				chn_num;
	
	//把队列中的数据写到flash里面去
	for(chn_num = 0; chn_num <phn_sys.sys_conf.num_chn; chn_num++)
	{

		if(stg->arr_rcd_mgr[chn_num].rcd_count >= stg->arr_rcd_mgr[chn_num].rcd_maxcount)
		{
			STG_Clean_save_data(chn_num);
			LOG_Add(LOG_CHN_DATA_AUTO_ERASE(chn_num));
			STRG_SYS.fs.fs_erase_file(fd, 0, 0);
			stg->arr_rcd_mgr[chn_num].rcd_count = 0;
			continue;
		}		
		
		fd = STG_Open_file(STG_CHN_DATA(chn_num), STG_DEF_FILE_SIZE);
		//把对应通道的全部数据都存到falsh里面
		while(1)
		{
			
			if(STG_Out_save_data(chn_num, &d) != RET_OK)
				break;
			
			//因为W25Q的驱动提供了回读错误后，会在当前位置之后重新写数据，所以可能会出现写入多个记录的情况（当然，错误的那些记录会被标记）
			acc_len = STRG_SYS.fs.fs_raw_write(fd, (uint8_t *)&d, sizeof(d));
			if(acc_len > 0)
			{
				stg->arr_rcd_mgr[chn_num].rcd_count += acc_len / sizeof(d);	
				STG_Remove_save_data(chn_num);	//成功写入才删除这个数据
			}
			
		}
		
	}
	
	



	
}

int	STG_Set_file_position(uint8_t	file_type, uint8_t rd_or_wr, uint32_t position)
{
	Storage				*stg = Get_storage();
	int						fd = -1;
	int 					whn = 0;
	uint32_t			whr = 0;
	uint8_t				chn_num = STG_GET_CHN(file_type);
	
	
	fd = STG_Open_file(file_type, STG_DEF_FILE_SIZE);
	if(rd_or_wr == STG_DRC_READ)
	{
		whn = RD_SEEK_SET;
		
	}
	else
	{
		whn = WR_SEEK_SET;
	}
	whr = position;
	
	if(IS_CHN_ALARM(file_type))
	{
		//每个通道都有自己的存储区
		if(position > STG_CHN_ALARM_FILE_SIZE)
		{
			return ERR_PARAM_BAD;
			
		}
		
		whr = chn_num * STG_CHN_ALARM_FILE_SIZE + STG_ALARM_FILE_OFFSET + position;
	}
	else if(IS_CHN_SUM(file_type))
	{
		//每个通道都有自己的存储区
		if(position > STG_CHN_SUM_FILE_SIZE)
		{
			return ERR_PARAM_BAD;
			
		}
		
		whr = chn_num * STG_CHN_SUM_FILE_SIZE + STG_SUM_FILE_OFFSET + position;
	}
	else if(IS_LOSE_PWR(file_type))
	{
		if(position > STG_PWR_FILE_SIZE)
		{
			return ERR_PARAM_BAD;
			
		}
		
		whr = STG_LSTPWR_FILE_OFFSET + position;
		
		
	}
	
	return STRG_SYS.fs.fs_lseek(fd, whn, whr);
}

void STG_Resize(uint8_t	file_type, uint32_t	new_size)
{
	Storage				*stg = Get_storage();
	int						fd = -1;
	
	
	fd = STG_Open_file(file_type, STG_DEF_FILE_SIZE);
	
	STRG_SYS.fs.fs_resize(fd, NULL, new_size);
	
}

void STG_Erase_file(uint8_t	file_type)
{
	
	Storage				*stg = Get_storage();
	int						fd = -1;
	uint32_t			erase_addr[2] = {0, 0};
	uint8_t				chn_num = STG_GET_CHN(file_type);
	
	
	fd = STG_Open_file(file_type, STG_DEF_FILE_SIZE);
	
	if(IS_CHN_ALARM(file_type))
	{
		erase_addr[0] = chn_num * STG_CHN_ALARM_FILE_SIZE + STG_ALARM_FILE_OFFSET;
		erase_addr[1] = STG_CHN_ALARM_FILE_SIZE;
	}
	if(IS_CHN_SUM(file_type))
	{
		erase_addr[0] = chn_num * STG_CHN_SUM_FILE_SIZE + STG_SUM_FILE_OFFSET;
		erase_addr[1] = STG_CHN_SUM_FILE_SIZE;
	}
	if(IS_LOSE_PWR(file_type))
	{
		
		erase_addr[0] = STG_LSTPWR_FILE_OFFSET;
		erase_addr[1] = STG_PWR_FILE_SIZE;
	}
	
	STRG_SYS.fs.fs_erase_file(fd, erase_addr[0] , erase_addr[1]);
	
}

//报警和掉电的记录是存放在第一个记录上的
uint16_t STG_Get_alm_pwr_num(uint8_t	chn_pwr)
{
	Storage				*stg = Get_storage();
	uint16_t			n = 0;
	
	
	
	STG_Set_file_position(chn_pwr, STG_DRC_READ, offsetof(rcd_alm_pwr_t, rcd_num));
	if(stg->rd_stored_data(stg, chn_pwr, (uint8_t *)&n, 2) != 2)
			return 0xffff;
	return n;
	
}
int	STG_Set_alm_pwr_num(uint8_t	chn_pwr, uint16_t new_num)
{
	Storage				*stg = Get_storage();
	

	STG_Set_file_position(chn_pwr, STG_DRC_WRITE, offsetof(rcd_alm_pwr_t, rcd_num));
	if(stg->wr_stored_data(stg, chn_pwr, (uint8_t *)&new_num, 2) != 2)
			return RET_FAILED;
	return RET_OK;
	
}

int	STG_Read_alm_pwr(uint8_t	chn_pwr,short start, char *buf, int buf_size, uint32_t *rd_count)		
{
	Storage				*stg = Get_storage();
//	file_info_t			*fnf ;
	int					fd = -1;
	rcd_alm_pwr_t		ap;
	struct  tm			st,et;
//	uint32_t			max_sec = 0;	
	int					buf_offset = 0;
	char				tmp_buf[32];
	char				alarm_code[7];
	
	
//	fnf = STRG_SYS.fs.fs_file_info(fd);

//	fd = STG_Open_file(ct, STG_DEF_FILE_SIZE);
//	STRG_SYS.fs.fs_lseek(fd, RD_SEEK_SET, start * sizeof(rcd_alm_pwr_t));
	STG_Set_file_position(chn_pwr, STG_DRC_READ, start * sizeof(rcd_alm_pwr_t));
//	memset(buf, 0, buf_size);
	buf[0] = 0;
	while(1)
	{
		
//		if((buf_offset + 21)> buf_size)		//必须预留最大数据长度的空间
//			break;
		
//		if(fnf->read_position >= fnf->write_position)
//			break;
		
		
		if(stg->rd_stored_data(stg, chn_pwr, (uint8_t *)&ap, sizeof(rcd_alm_pwr_t)) != sizeof(rcd_alm_pwr_t))
			break;

		
		if(ap.happen_time_s == 0xffffffff)
			continue;
		if(ap.disapper_time_s == 0xffffffff)
			continue;
		
		
		
		Sec_2_tm(ap.happen_time_s, &st);
		Sec_2_tm(ap.disapper_time_s, &et);
		//放置csv格式的数据
		
		switch(ap.alm_pwr_type)
		{
			case ALM_CODE_HH:
				sprintf(alarm_code, "%d,HH",chn_pwr);
				break;
			case ALM_CODE_HI:
				sprintf(alarm_code, "%d,HI",chn_pwr);
				break;
			case ALM_CODE_LO:
				sprintf(alarm_code, "%d,LO",chn_pwr);
				break;
			case ALM_CODE_LL:
				sprintf(alarm_code, "%d,LL",chn_pwr);
				break;
			default:
				sprintf(alarm_code, "0,PWR");
				break;
			
		}
		if(IS_LOSE_PWR(chn_pwr))
			alarm_code[0] = 0;
		sprintf(tmp_buf, "%s,%2d/%02d/%02d,%02d:%02d:%02d,%2d/%02d/%02d,%02d:%02d:%02d\r\n", alarm_code,\
			st.tm_year,st.tm_mon, st.tm_mday, st.tm_hour, st.tm_min, st.tm_sec, \
			et.tm_year,et.tm_mon, et.tm_mday, et.tm_hour, et.tm_min, et.tm_sec);
		if(strlen(tmp_buf) > (buf_size - buf_offset))
		{
			STRG_SYS.fs.fs_lseek(fd, RD_SEEK_CUR, -sizeof(rcd_alm_pwr_t));
			break;
			
		}
		
		strcat(buf, tmp_buf);
		buf_offset = strlen(buf);
		(*rd_count) ++;
	}
	
	return buf_offset;
	
	
}



uint32_t STG_Read_data_by_time(uint8_t	chn, uint32_t sec, uint32_t pos, data_in_fsh_t *r)
{
	#define RDBT_RETRY		3
	
	Storage				*stg = Get_storage();
	file_info_t			*fnf ;
	int					fd = -1;
	int					i;
//	data_in_fsh_t		d;
	uint32_t			lt, mid, rt;
	
	
	fd = STG_Open_file(STG_CHN_DATA(chn), STG_DEF_FILE_SIZE);
	fnf = STRG_SYS.fs.fs_file_info(fd);
	
	

	if(sec == 0)
	{
		STG_Set_file_position(STG_CHN_DATA(chn), STG_DRC_READ, 0);
		if(STG_Read_chn_data_will_retry(fd, RDBT_RETRY, r) != RET_OK)
			r->rcd_time_s = 0xffffffff;
		
		return 0;
		
	}
	
	if(sec == 0xffffffff)
	{
		
		STG_Set_file_position(STG_CHN_DATA(chn), STG_DRC_READ, fnf->write_position - sizeof(data_in_fsh_t));
		if(STG_Read_chn_data_will_retry(fd, RDBT_RETRY, r) != RET_OK)
			r->rcd_time_s = 0xffffffff;
		
		return fnf->write_position / sizeof(data_in_fsh_t) - 1;
	}
	
	//因为可能会有连续读取，所以就先尝试在前几个位置读取一下，有可能会减少二分查找的次数
	lt = pos + 4;
	for(i = pos; i < lt; i ++)
	{
		STG_Set_file_position(STG_CHN_DATA(chn), STG_DRC_READ, i * sizeof(data_in_fsh_t));	
		STG_Read_chn_data_will_retry(fd, RDBT_RETRY, r);
		if(r->rcd_time_s == sec)
			return pos;
	}
	
	lt = pos;
	rt = fnf->write_position / sizeof(data_in_fsh_t);
	
	
	
	//对于指定的某个时间点，采用二分查找
	
	while(1)
	{	
	
		//若上限与下限相等，或者上下限相邻时,跳到错误处理
		if((rt - lt) <= 1)
			goto err;
		
		//根据上下限获取中间位置=（上限 + 下限）/ 2
		mid = ((rt - lt) >> 1) + lt;
		STG_Set_file_position(STG_CHN_DATA(chn), STG_DRC_READ,  mid * sizeof(data_in_fsh_t));
		
		//读取数据，若失败则跳到错误处理
		if(STG_Read_chn_data_will_retry(fd, RDBT_RETRY, r) != RET_OK)
			goto err;
		//若数据的时间大于该时间点，将下限设置为当前的中间位置
		if(r->rcd_time_s > sec)
		{
			rt = mid;
			continue;
		}
		
		//若数据的时间小于该时间点，将上限设置为当前的中间位置
		if(r->rcd_time_s < sec)
		{
			lt = mid;
			continue;
		}
		
		
	
		//若数据的时间等于该时间点，则返回当前的位置
		if(r->rcd_time_s == sec)
			return mid ;
		
		
		
		
		
		
	}
	
err:
	r->rcd_time_s = 0xffffffff;
	return mid ;
//	
//	STG_Set_file_position(STG_CHN_DATA(chn), STG_DRC_READ, pos * sizeof(data_in_fsh_t));	
//	if(sec == 0xffffffff)
//		STG_Set_file_position(STG_CHN_DATA(chn), STG_DRC_READ, fnf->write_position - sizeof(data_in_fsh_t));
//	
//	
//	while(1)
//	{
//		if(fnf->read_position >= fnf->write_position)
//			break;
//		if(STRG_SYS.fs.fs_raw_read(fd, (uint8_t *)&d, sizeof(data_in_fsh_t)) != sizeof(data_in_fsh_t))
//			break;

//		pos ++;
//		if(d.rcd_time_s == 0xffffffff)
//			break;
//		
//		if((sec == 0) || (sec == 0xffffffff))	//返回最早的记录,或最晚的记录
//			sec = d.rcd_time_s;
//		
//		if(d.rcd_time_s > sec)
//			break;
//		
//		if(d.rcd_time_s != sec)
//			continue;
//		
//		
//		
//		r->rcd_time_s = d.rcd_time_s;
//		r->rcd_val = d.rcd_val;
//		r->decimal_places = d.decimal_places;
//		
//		break;
//		
//		
//		
//		
//		
//		
//		
//		
//	}
//	
//	return pos;
	
	
}


int	STG_Read_rcd_by_time(uint8_t	chn, uint32_t start_sec, uint32_t end_sec, char *buf, int buf_size, uint32_t *rd_sec)
{
	Storage				*stg = Get_storage();
	file_info_t			*fnf ;
	int					fd = -1;
	data_in_fsh_t		d;
	struct  tm			t;
	uint32_t			max_sec = 0;	
	int					buf_offset = 0;
	char				tmp_buf[32];
	char				str_data[8];
	
	//用二分法先定位起始时间点
	//执行了这条语句，文件的读取位置自然就会移动到对应的位置
	STG_Read_data_by_time(chn, start_sec, 0, &d);	
	
	fd = STG_Open_file(STG_CHN_DATA(chn), STG_DEF_FILE_SIZE);
	fnf = STRG_SYS.fs.fs_file_info(fd);

	buf[0] = 0;
	while(1)
	{
		if(fnf->read_position >= fnf->write_position)
			break;
		if(STRG_SYS.fs.fs_raw_read(fd, (uint8_t *)&d, sizeof(data_in_fsh_t)) != sizeof(data_in_fsh_t))
			break;

		
		if(d.rcd_time_s == 0xffffffff)
			break;
		if(d.rcd_time_s < start_sec)
			continue;
		if(d.rcd_time_s > end_sec)
			continue;
		
		
		Sec_2_tm(d.rcd_time_s, &t);
		//放置csv格式的数据
		sprintf(tmp_buf, "%d,%2d/%02d/%02d,%02d:%02d:%02d,", chn, t.tm_year,t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
//		d.rcd_val = -1;
		if(d.decimal_places == 0)
		{
			Print_float(d.rcd_val, 0, 0, str_data);
		}
		else
		{
			Print_float(d.rcd_val, 0, 1, str_data);
			
		}
		strcat(tmp_buf, str_data);
		strcat(tmp_buf, "\r\n");
		if(strlen(tmp_buf) > (buf_size - buf_offset))
		{
			STRG_SYS.fs.fs_lseek(fd, RD_SEEK_CUR, -sizeof(data_in_fsh_t));
			break;
			
		}
		
		strcat(buf, tmp_buf);
//		sprintf((char *)buf + buf_offset, "%2d/%02d/%02d,%02d:%02d:%02d,%d\r\n", t.tm_year,t.tm_mon, t.tm_mday, \
//				t.tm_hour, t.tm_min, t.tm_sec, d.rcd_val);
		buf_offset = strlen(buf);
		
		
//		if(min_sec > d.rcd_time_s)
//			min_sec = d.rcd_time_s;
		if(max_sec < d.rcd_time_s)
			max_sec = d.rcd_time_s;
		
		
		
		
		
		
		
	}
	*rd_sec = max_sec - start_sec;
	
	//如果只读取了1次数据，就会出现这种情况
	if((*rd_sec == 0) && (buf_offset > 0))
	{
		*rd_sec = 1;
		
	}
	return buf_offset;
		
	
}


CTOR(Storage)
FUNCTION_SETTING(init, Strg_init);
FUNCTION_SETTING(rd_stored_data, Strg_rd_stored_data);
FUNCTION_SETTING(wr_stored_data, Strg_WR_stored_data);
FUNCTION_SETTING(open_file, STG_Open_file);

END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{
static int STG_Read_chn_data_will_retry(int f, int retry, data_in_fsh_t *dif)
{
	
	while(retry)
	{
		if(STRG_SYS.fs.fs_raw_read(f, (uint8_t *)dif, sizeof(data_in_fsh_t)) == sizeof(data_in_fsh_t))
			return RET_OK;
		retry --;
	}
	
	return RET_FAILED;
	
}
static int Strg_init(Storage *self)
{
//	int i;


	
	/*  把所有的文件按照固定的顺序打开一遍，来保证每个仪表在第一次上电时，文件的存储位置是一致的*/
	//这里只处理FM25上的文件
	//通道的数据文件，会自行根据先后顺序排列
	STG_Open_file(STG_SYS_CONF, STG_DEF_FILE_SIZE);
	
	//掉电信息以及素有通道的报警和累积信息都是在同一个文件里面的，因此打开一次就行
	STG_Open_file(STG_CHN_ALARM(0), STG_DEF_FILE_SIZE);			

	STG_Open_file(STG_LOG, STG_DEF_FILE_SIZE);
	

	
	
	return RET_OK;
	
}

static int	Strg_rd_stored_data(Storage *self, uint8_t	cfg_type, void *buf, int len)
{
	int ret = ERR_PARAM_BAD;
//		Strg_RD_chn_conf(cfg_type, cfg_buf);
//		Strg_RD_sys_conf(cfg_type, cfg_buf);
	if(IS_SYS_CONF(cfg_type))
	{
		ret = STG_Acc_sys_conf(STG_DRC_READ, buf);
		
	}
	else if(IS_CHN_CONF(cfg_type))
	{
		ret = STG_Acc_chn_conf(cfg_type, STG_DRC_READ, buf);
		
	}
	else if(IS_LOSE_PWR(cfg_type))
	{
		ret = STG_Acc_lose_pwr(STG_DRC_READ, buf, len);
		
	}
	
	else if(IS_CHN_DATA(cfg_type))
	{
		ret = STG_Acc_chn_data(cfg_type, STG_DRC_READ, buf, len);
		
	}
	else if(IS_CHN_ALARM(cfg_type))
	{
		ret = STG_Acc_chn_alarm(cfg_type, STG_DRC_READ, buf, len);
		
	}
	else if(IS_CHN_SUM(cfg_type))
	{
		ret = STG_Acc_chn_sum(cfg_type, STG_DRC_READ, buf, len);
		
	}
	else
	{
		ret = STG_Acc_file(cfg_type, STG_DRC_READ, buf, len);
		
	}
	return ret;
	
}

static int		Strg_WR_stored_data(Storage *self, uint8_t	cfg_type, void *buf, int len)
{
//	Strg_WR_chn_conf(cfg_type, buf);
//	Strg_WR_sys_conf(cfg_type, buf);
	int ret = ERR_PARAM_BAD;
	
	if(IS_SYS_CONF(cfg_type))
	{
		ret = STG_Acc_sys_conf(STG_DRC_WRITE, buf);
		
	}
	else if(IS_CHN_CONF(cfg_type))
	{
		ret = STG_Acc_chn_conf(cfg_type, STG_DRC_WRITE, buf);
		
	}
	else if(IS_LOSE_PWR(cfg_type))
	{
		ret = STG_Acc_lose_pwr(STG_DRC_WRITE, buf, len);
		
	}
	else if(IS_CHN_DATA(cfg_type))
	{
		ret = STG_Acc_chn_data(cfg_type, STG_DRC_WRITE, buf, len);
		
	}
	else if(IS_CHN_ALARM(cfg_type))
	{
		ret = STG_Acc_chn_alarm(cfg_type, STG_DRC_WRITE, buf, len);
		
	}
	else if(IS_CHN_SUM(cfg_type))
	{
		ret = STG_Acc_chn_sum(cfg_type, STG_DRC_WRITE, buf, len);
		
	}
	else
	{
		
		ret = STG_Acc_file(cfg_type, STG_DRC_WRITE, buf, len);
	}
	
	return ret;
}

static void Strg_Updata_rcd_mgr(uint8_t	num, mdl_chn_save_t *p)
{
	mdl_chn_save_t		*p_save = (mdl_chn_save_t *)p;
	file_info_t			*fnf ;
	Storage				*stg = Get_storage();
	int					fd;
	
	fd = STG_Open_file(STG_CHN_DATA(num), STG_DEF_FILE_SIZE);
	fnf = STRG_SYS.fs.fs_file_info(fd);
	
	stg->arr_rcd_mgr[num].rcd_count = fnf->write_position / sizeof(data_in_fsh_t);
	
	if(p == NULL)
		return;
	//存储的信息有效判断
	if(p_save->small_signal == 0xff)
		return;
	//如果文件的大小没有发生变化，下面这条语句不会执行任何操作
	//大小发生变化的话就会去执行文件大小重新分配额
	if(STRG_SYS.fs.fs_resize(fd, NULL, p_save->MB * 1024 * 1024) == fd)
		stg->arr_rcd_mgr[num].rcd_maxcount = p_save->MB * 1024 * 1024 / sizeof(data_in_fsh_t);
	else
	{
		stg->arr_rcd_mgr[num].rcd_maxcount = fnf->file_size / sizeof(data_in_fsh_t);
		
	}
	
	STRG_SYS.fs.fs_close(fd);
		

	
}

static int	STG_Acc_chn_conf(uint8_t	tp, uint8_t	drc, void *p)
{
	
	int fd;
	int	num = STG_GET_CHN(tp);
	int	ret = -1;
	mdl_chn_save_t	save;
//	uint8_t chn_num = STG_GET_CHN(tp);
	fd = STG_Open_file(tp, STG_DEF_FILE_SIZE);
	
	if(drc == STG_DRC_READ)
	{
		STRG_SYS.fs.fs_lseek(fd, RD_SEEK_SET, num * sizeof(mdl_chn_save_t));
		if(STRG_SYS.fs.fs_read(fd, p, sizeof(mdl_chn_save_t)) == sizeof(mdl_chn_save_t))
		{
			Strg_Updata_rcd_mgr(num, p);
			
			ret = RET_OK;
		}
	}
	else
	{
		
		if(p == NULL)
		{
			p = &save;
			MdlChn_save_data(num, p);
		}
		STRG_SYS.fs.fs_lseek(fd, WR_SEEK_SET, num * sizeof(mdl_chn_save_t));
		if(STRG_SYS.fs.fs_write(fd, p, sizeof(mdl_chn_save_t)) == sizeof(mdl_chn_save_t))
		{
			ret = RET_OK;
		}
		
		
		Strg_Updata_rcd_mgr(num, p);

		
		
	}
	return ret;
	
	
}

static int	STG_Acc_sys_conf(uint8_t drc, void *p)
{
	int fd;
	int	ret = -1;

	
	fd = STG_Open_file(STG_SYS_CONF, STG_DEF_FILE_SIZE);
	
	if(drc == STG_DRC_READ)
	{
		STRG_SYS.fs.fs_lseek(fd, RD_SEEK_SET, NUM_CHANNEL * sizeof(mdl_chn_save_t));
		if(STRG_SYS.fs.fs_read(fd, p, sizeof(system_conf_t)) == sizeof(system_conf_t))
		{
			
			ret = RET_OK;
		}
	}
	else
	{
		STRG_SYS.fs.fs_lseek(fd, WR_SEEK_SET, NUM_CHANNEL * sizeof(mdl_chn_save_t));
		if(STRG_SYS.fs.fs_write(fd, p, sizeof(system_conf_t)) == sizeof(system_conf_t))
		{
			
			ret = RET_OK;
		}
		
	}
	STRG_SYS.fs.fs_close(fd);
	
	return ret;
	
}

//调用方维护读写位置的正确
static int	STG_Acc_lose_pwr(uint8_t	drc, void *p, int len)
{
	
	int fd = STG_Open_file(STG_LOSE_PWR, STG_DEF_FILE_SIZE);
	file_info_t	*p_fnf = STRG_SYS.fs.fs_file_info(fd);
	
	if(drc == STG_DRC_READ)
	{
		if((len + p_fnf->read_position) >= (STG_PWR_FILE_SIZE + STG_LSTPWR_FILE_OFFSET))
			return 0;
		
		return STRG_SYS.fs.fs_read(fd, p, len);
	}
	else
	{
		if((len + p_fnf->write_position) >= (STG_PWR_FILE_SIZE + STG_LSTPWR_FILE_OFFSET))
			return 0;
		return STRG_SYS.fs.fs_write(fd, p, len);
	}
	
}
//调用方维护读写位置的正确

static int	STG_Acc_chn_alarm(uint8_t	type, uint8_t	drc, void *p, int len)
{
	int fd = STG_Open_file(type, STG_DEF_FILE_SIZE);
	file_info_t	*p_fnf = STRG_SYS.fs.fs_file_info(fd);
	uint8_t		chn_num = STG_GET_CHN(type);
	if(chn_num > (NUM_CHANNEL - 1))
		return 0;
	if(len > STG_CHN_ALARM_FILE_SIZE)
		return 0;
	
	if(drc == STG_DRC_READ)
	{
		if((len + p_fnf->read_position) > ((chn_num + 1)* STG_CHN_ALARM_FILE_SIZE + STG_ALARM_FILE_OFFSET))
			return 0;
		return STRG_SYS.fs.fs_read(fd, p, len);
		
	}
	else
	{
		if((len + p_fnf->write_position) > ((chn_num + 1)* STG_CHN_ALARM_FILE_SIZE + STG_ALARM_FILE_OFFSET))
			return 0;
		return STRG_SYS.fs.fs_write(fd, p, len);
	}
	
}
static int	STG_Acc_file(uint8_t	type, uint8_t	drc, void *p, int len)
{
	int fd = STG_Open_file(type, STG_DEF_FILE_SIZE);
	file_info_t	*p_fnf = STRG_SYS.fs.fs_file_info(fd);
	
	if(drc == STG_DRC_READ)
	{
		if((len + p_fnf->read_position) > p_fnf->file_size)
			return 0;
		return STRG_SYS.fs.fs_read(fd, p, len);
		
	}
	else
	{
		if((len + p_fnf->write_position) > p_fnf->file_size)
			return 0;
		return STRG_SYS.fs.fs_write(fd, p, len);
	}
	
}

static int	STG_Acc_chn_sum(uint8_t	type, uint8_t	drc, void *p, int len)
{
	int fd = STG_Open_file(type, STG_DEF_FILE_SIZE);
	file_info_t	*p_fnf = STRG_SYS.fs.fs_file_info(fd);
	uint8_t		chn_num = STG_GET_CHN(type);
	if(chn_num > (NUM_CHANNEL - 1))
		return 0;
	if(len > STG_SUM_FILE_SIZE)
		return 0;
	
	if(drc == STG_DRC_READ)
	{
		if((len + p_fnf->read_position) > ((chn_num + 1)* STG_CHN_SUM_FILE_SIZE + STG_SUM_FILE_OFFSET))
			return 0;
		return STRG_SYS.fs.fs_read(fd, p, len);
		
	}
	else
	{
		if((len + p_fnf->write_position) > ((chn_num + 1)* STG_CHN_SUM_FILE_SIZE + STG_SUM_FILE_OFFSET))
			return 0;
		return STRG_SYS.fs.fs_write(fd, p, len);
	}
	
}

//返回写入或者读取的字节数
static int	STG_Acc_chn_data(uint8_t	type, uint8_t	drc, void *p, int len)
{
#if STG_RCD_FULL_ACTION == STG_ERASE
//	data_in_fsh_t		dinf;
	Storage				*stg = Get_storage();
	int16_t				*p_s16 = p;
	int						fd = -1;
	int					acc_len = 0;
	file_info_t			*fnf ;
	uint8_t				chn_num = STG_GET_CHN(type);
	uint8_t				decimal_places = 0;
	
	
	

	
	
	fd = STG_Open_file(type, STG_DEF_FILE_SIZE);
	
	if(drc == STG_DRC_READ)
	{
		fnf = STRG_SYS.fs.fs_file_info(fd);
		if(fnf->read_position >= fnf->write_position)
			goto exit;
		acc_len = STRG_SYS.fs.fs_raw_read(fd, (uint8_t *)p, len);
		
	}
	else
	{
		
//		dinf.rcd_time_s = SYS_time_sec();
//		dinf.rcd_val = p_s16[0];
//		if(len == 4)
//			dinf.decimal_places = p_s16[1];
		
		
		if(len == 4)
			decimal_places = p_s16[1];
		if(STG_En_save_data(chn_num, decimal_places, p_s16[0], SYS_time_sec()) != RET_OK)
		{
			//错误处理
			
		}
		
//		if(stg->arr_rcd_mgr[chn_num].rcd_count >= stg->arr_rcd_mgr[chn_num].rcd_maxcount)
//		{
//			LOG_Add(LOG_CHN_DATA_AUTO_ERASE(chn_num));
//			STRG_SYS.fs.fs_erase_file(fd, 0, 0);
//			stg->arr_rcd_mgr[chn_num].rcd_count = 0;
//		}

//		acc_len = STRG_SYS.fs.fs_raw_write(fd, (uint8_t *)&dinf, sizeof(dinf));
//		if(acc_len == sizeof(dinf))
//			stg->arr_rcd_mgr[chn_num].rcd_count ++;

	}
	exit:
	return acc_len;
	
		
#else			
	STG_cache_t		*c;

	Storage				*stg = Get_storage();
	int						fd = -1;
	uint8_t				t = 0;
	uint8_t				chn_num = STG_GET_CHN(type);
	uint16_t			rd_len = 0;
	
	fd = STG_Open_file(type, STG_DEF_FILE_SIZE);
	
	if(drc == STG_DRC_READ)
	{
		
		rd_len = len - len % sizeof(data_in_fsh_t);
		rd_len = STRG_SYS.fs.fs_read(fd, p, rd_len);
		
		return rd_len;
	}
	else
	{
		STG_WR_cache_mgr();
		if(stg->arr_rcd_mgr[chn_num].rcd_count >= stg->arr_rcd_mgr[chn_num].rcd_maxcount)
		{
			//flash空间耗尽了
#if STG_RCD_FULL_ACTION == STG_STOP
			return 0;
#endif			
			STG_flush_wr_cache(type);
			fd = STG_Open_file(type, STG_DEF_FILE_SIZE);
			STRG_SYS.fs.fs_lseek(fd, WR_SEEK_SET, 0);
			stg->arr_rcd_mgr[chn_num].rcd_count = 0;

			
			
		}
		c = (STG_cache_t *)(stg_wr_mgr.p_buf + stg_wr_mgr.cur_idx);
		t = SYS_time_sec();
		c->flag = 1;
		c->chn = chn_num;
		c->val = *(uint16_t *)p;
		stg_wr_mgr.cur_idx += sizeof(STG_cache_t);
		stg->arr_rcd_mgr[chn_num].rcd_count ++;
		return len;
	}
#endif	
//	return 0;
}




#if STG_RCD_FULL_ACTION != STG_ERASE
static void STG_WR_cache_mgr(void)
{

	int i;
	if(stg_wr_mgr.cur_idx < stg_wr_mgr.buf_size)
		return;
	for(i = 0; i < NUM_CHANNEL; i++)
		STG_flush_wr_cache(STG_CHN_DATA(i));
	
}
static void STG_flush_wr_cache(uint8_t type)
{
	
	STG_cache_t		*c;
	uint16_t 			i = 0;
	uint8_t				chn_num = STG_GET_CHN(type);
	uint8_t				safe_count = 20;
	int 					fd = STG_Open_file(type, STG_DEF_FILE_SIZE);
	
	while(i < stg_wr_mgr.buf_size)
	{
		safe_count = 20;
		c = (STG_cache_t *)stg_wr_mgr.p_buf + i;
		if(c->flag && c->chn == chn_num)
		{
			//c->flag c->chn 这两个成员不必存储到flash
			while(STRG_SYS.fs.fs_write(fd, (uint8_t *)c, sizeof(STG_cache_t) - 2) != (sizeof(STG_cache_t) - 2))
			{
				if(safe_count)
				{
					safe_count --;
					delay_ms(1);
				}
				else
				{
					//
					break;
				}
				
			}
			c->flag = 0;
			
			
		}
		
		
	}
	
}
#endif


static int 	STG_Open_file(uint8_t type, uint32_t file_size)
{
	int 		fd = -1;
	Storage		*stg = Get_storage();
	char		name[8];
	
	
	if(IS_SYS_CONF(type))
	{
		fd = STRG_SYS.fs.fs_open(STRG_CFG_FSH_NUM, "phn.cfg", "rw", 256);
		
	}
	else if(IS_CHN_CONF(type))
	{
		//通道配置与系统配置存放在同一个文件的不同位置
		fd = STRG_SYS.fs.fs_open(STRG_CFG_FSH_NUM, "phn.cfg", "rw", 256);
		
	}
	else if(IS_LOSE_PWR(type))
	{
		fd = STRG_SYS.fs.fs_open(STRG_RCD_FSH_NUM, "alm_lost_pwr", "rw", STG_PAS_SIZE);
		
	}
	else if(IS_CHN_ALARM(type))
	{
		fd = STRG_SYS.fs.fs_open(STRG_RCD_FSH_NUM, "alm_lost_pwr", "rw", STG_PAS_SIZE);
		
	}
	else if(IS_CHN_SUM(type))
	{
		fd = STRG_SYS.fs.fs_open(STRG_RCD_FSH_NUM, "alm_lost_pwr", "rw", STG_PAS_SIZE);
		
	}
	else if(IS_LOG(type))
	{
		fd = STRG_SYS.fs.fs_open(STRG_RCD_FSH_NUM, "SDH_LOG.CSV", "rw", STG_LOG_FILE_SIZE);
		
	}
	else if(IS_CHN_DATA(type))
	{
		sprintf(name, "chn_%d", STG_GET_CHN(type));
		if(file_size == STG_DEF_FILE_SIZE)
		{
			fd = STRG_SYS.fs.fs_open(STRG_CHN_DATA_FSH_NUM, name, "rw", \
		stg->arr_rcd_mgr[STG_GET_CHN(type)].rcd_maxcount * sizeof(data_in_fsh_t));
			
		}
		else
		{
			
			fd = STRG_SYS.fs.fs_open(STRG_CHN_DATA_FSH_NUM, name, "wr", file_size);
			stg->arr_rcd_mgr[STG_GET_CHN(type)].rcd_maxcount = file_size / sizeof(data_in_fsh_t);
			Strg_Updata_rcd_mgr(STG_GET_CHN(type), NULL);
		}
		
	}
	
	return fd;
}


static int STG_Is_sava_data_full(uint8_t chn)
{
	if(sds.arr_tail[chn] == (NUM_SAVE_DATA - 1))	
		if(sds.arr_head[chn] == 0)
			return 1;
		
	if((sds.arr_tail[chn] + 1) == sds.arr_head[chn])
		return 1;
	
	
	return 0;
	
}

static int STG_Is_sava_data_empty(uint8_t chn)
{
//	if(sds.arr_head[chn] == (NUM_SAVE_DATA - 1))	
//		if(sds.arr_tail[chn] == 0)
//			return 1;
		
	if((sds.arr_head[chn]) == sds.arr_tail[chn])
		return 1;
	
	
	return 0;
	
}

//成功返回RET_OK， 失败返回 RET_FAILED
static int STG_En_save_data(uint8_t chn, uint8_t dp, uint16_t value,  uint32_t time_s)
{
	save_data_t *p_sd;
	
	if(STG_Is_sava_data_full(chn))
		return RET_FAILED;
	p_sd = &sds.arr_save_data[chn][sds.arr_tail[chn]];
	p_sd->decimal_point = dp;
	p_sd->time_s = time_s;
	p_sd->value = value;
	
	sds.arr_tail[chn] ++;
	if(sds.arr_tail[chn] > NUM_SAVE_DATA)
		sds.arr_tail[chn] = 0;
	
	return RET_OK;
	
}
//成功返回RET_OK， 失败返回RET_FAILED

static int STG_Out_save_data(uint8_t chn, data_in_fsh_t	*dif)
{
	save_data_t *p_sd;
	if(STG_Is_sava_data_empty(chn))
		return RET_FAILED;
	
	p_sd = &sds.arr_save_data[chn][sds.arr_head[chn]];
	dif->decimal_places = p_sd->decimal_point;
	dif->rcd_time_s = p_sd->time_s;
	dif->rcd_val = p_sd->value;
	
}

static void STG_Remove_save_data(uint8_t chn)
{
	
		if(STG_Is_sava_data_empty(chn))
			return;
		
	sds.arr_head[chn] ++;
	if(sds.arr_head[chn] > NUM_SAVE_DATA)
		sds.arr_head[chn] = 0;
}

static void STG_Clean_save_data(uint8_t chn)
{
	
	sds.arr_head[chn] = 0;
	sds.arr_tail[chn] = 0;
	
}
