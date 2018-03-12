#include "Storage.h"
#include "sdhDef.h"
#include <string.h>
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


#define STG_PAS_SIZE								(STG_PWR_FILE_SIZE  + STG_ALARM_FILE_SIZE + STG_SUM_FILE_SIZE)

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
//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
//Observer		strg_mdl_ob;
#if STG_RCD_FULL_ACTION != STG_ERASE
static uint8_t			stg_buf[4096];
static STG_wr_buf_mgr_t stg_wr_mgr = {0, 4096, stg_buf};
#endif
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

static int	STG_Acc_sys_conf(uint8_t	drc, void *p);
static int	STG_Acc_lose_pwr(uint8_t	drc, void *p, int len);


//------- 记录通道数据的函数--------------------------------------------------//
#if STG_RCD_FULL_ACTION != STG_ERASE
static void STG_WR_cache_mgr(void);
static void STG_flush_wr_cache(uint8_t type);
#endif
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
	uint32_t			erase_addr[2];
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
	uint8_t				ct;
	if(chn_pwr < NUM_CHANNEL)
	{
		ct = STG_CHN_ALARM(chn_pwr);
		
	}
	else
	{
		ct = STG_LOSE_PWR;

	}		
	
//	fnf = STRG_SYS.fs.fs_file_info(fd);

//	fd = STG_Open_file(ct, STG_DEF_FILE_SIZE);
//	STRG_SYS.fs.fs_lseek(fd, RD_SEEK_SET, start * sizeof(rcd_alm_pwr_t));
	STG_Set_file_position(ct, STG_DRC_READ, start * sizeof(rcd_alm_pwr_t));
//	memset(buf, 0, buf_size);
	buf[0] = 0;
	while(1)
	{
		
//		if((buf_offset + 21)> buf_size)		//必须预留最大数据长度的空间
//			break;
		
//		if(fnf->read_position >= fnf->write_position)
//			break;
		
		
		if(stg->rd_stored_data(stg, ct, (uint8_t *)&ap, sizeof(rcd_alm_pwr_t)) != sizeof(rcd_alm_pwr_t))
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
		if(chn_pwr > NUM_CHANNEL)
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
			continue;
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









//int Save_channel_data( Observer *self, void *p_srcMdl)
//{
//	
//	Storage 				*cthis = SUB_PTR(self, Observer, Storage);
//	Model 						*p_mdl;
//	rcd_channel_t			rchn = {0};
//	
//	p_mdl = ModelCreate("time");
//	p_mdl->getMdlData(p_mdl, TIME_U32, &rchn.rcd_time_s);
//	
//	p_mdl = (Model *)p_srcMdl;
//	if(p_mdl->getMdlData(p_mdl, AUX_DATA, &rchn.rcd_val) < 0)
//		rchn.rcd_flag |= RCD_ERR;
//	p_mdl->getMdlData(p_mdl, MDHCHN_CHN_NUM, &rchn.chn_num);
//	if(cthis->arr_rcd_mgr[rchn.chn_num].rcd_count < cthis->arr_rcd_mgr[rchn.chn_num].rcd_maxcount)
//	{
//		cthis->arr_rcd_mgr[rchn.chn_num].rcd_count ++;
//		STRG_SYS.fs.fs_write(cthis->rcd_fd, (uint8_t *)&rchn, sizeof(rcd_channel_t));
//	}
//	else
//	{
//		
//		
//	}
//	
//	return RET_OK;
//}


CTOR(Storage)
FUNCTION_SETTING(init, Strg_init);
FUNCTION_SETTING(rd_stored_data, Strg_rd_stored_data);
FUNCTION_SETTING(wr_stored_data, Strg_WR_stored_data);
FUNCTION_SETTING(open_file, STG_Open_file);

//FUNCTION_SETTING(Observer.update, Save_channel_data);


END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{

static int Strg_init(Storage *self)
{
//	Model 		*p_md;
//	char			chn_name[7];
//	char			i;
//	uint16_t	pg_size;
//	uint32_t	num_pg;
//	for(i = 0; i < NUM_CHANNEL; i++)
//	{
//		sprintf(chn_name,"chn_%d", i);
//		p_md = ModelCreate(chn_name);
//	}
//	
//	pg_size = STRG_SYS.arr_fsh[STRG_FSH_NUM].fnf.page_size;
//	num_pg = STRG_SYS.arr_fsh[STRG_FSH_NUM].fnf.total_pagenum;
	
	
	
	
//	self->rcd_mgr_fd = STRG_SYS.fs.fs_open(STRG_FSH_NUM, "mdl_chn.mgr", "rw", sizeof(rcd_mgr_t) * NUM_CHANNEL);
//	self->alarm_fd = STRG_SYS.fs.fs_open(STRG_FSH_NUM, "mdl_chn.alm", "rw", 4096);
//	self->lose_pwr_fd = STRG_SYS.fs.fs_open(STRG_FSH_NUM, "lose_pwr", "rw", 4096);
////	num_pg --;
////	//为了减少w25q的擦写次数，把所有通道都记录在一个文件里面，这样在切换记录通的时候，能够尽量在同一个扇区内
//	self->rcd_fd = STRG_SYS.fs.fs_open(STRG_FSH_NUM, "mdl_chn.rcd", "rw",pg_size * num_pg - sizeof(self->arr_rcd_mgr));
//	Strg_Updata_rcd_mgr(0, NULL);
//	if((self->alarm_fd >= 0) && (self->lose_pwr_fd >= 0))
//		return RET_OK;
//	else
//		return ERR_FS_OPEN_FAIL;
	
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
	data_in_fsh_t		dinf;
	Storage				*stg = Get_storage();
	int16_t				*p_s16 = p;
	int						fd = -1;
	int					acc_len = 0;
	file_info_t			*fnf ;
	uint8_t				chn_num = STG_GET_CHN(type);

	
	
	

	
	
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
		if(stg->arr_rcd_mgr[chn_num].rcd_count >= stg->arr_rcd_mgr[chn_num].rcd_maxcount)
		{
			STRG_SYS.fs.fs_erase_file(fd, 0, 0);
			stg->arr_rcd_mgr[chn_num].rcd_count = 0;
		}
//		if(chn_num == 0)
//		{
//			STRG_SYS.fs.fs_erase_file(fd, 0, 0);
//			stg->arr_rcd_mgr[chn_num].rcd_count = 0;
//			
//		}
		dinf.rcd_time_s = SYS_time_sec();
		dinf.rcd_val = p_s16[0];
		if(len == 4)
			dinf.decimal_places = p_s16[1];
		
		acc_len = STRG_SYS.fs.fs_raw_write(fd, (uint8_t *)&dinf, sizeof(dinf));
		if(acc_len == sizeof(dinf))
			stg->arr_rcd_mgr[chn_num].rcd_count ++;

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

//static int	Strg_RD_chn_conf(uint8_t type, void *p)
//{
//	int fd;
//	int	num = 0;
//	int	ret = -1;
//	
//	
//	if(type < NUM_CHANNEL)
//		num = type;
//	else
//		num = -1;
//	
//	if(num < 0)
//		return -1;
//	
//	fd = STRG_SYS.fs.fs_open(STRG_CFG_FSH_NUM, "phn.cfg", "r", 256);
//	STRG_SYS.fs.fs_lseek(fd, RD_SEEK_SET, num * sizeof(mdl_chn_save_t));
//	if(STRG_SYS.fs.fs_read(fd,p, sizeof(mdl_chn_save_t)) == sizeof(mdl_chn_save_t))
//	{
//		Strg_Updata_rcd_mgr(num, p);
//		
//		ret = RET_OK;
//	}
//	STRG_SYS.fs.fs_close(fd);
//	
//	return ret;
//}
//static int	Strg_RD_sys_conf(uint8_t type, void *p)
//{
//	int fd;
//	int	ret = -1;
//	if(!IS_SYS_CONF(type))
//		return -1;
//	
//	fd = STRG_SYS.fs.fs_open(STRG_CFG_FSH_NUM, "phn.cfg", "r", 256);
//	STRG_SYS.fs.fs_lseek(fd, RD_SEEK_SET, NUM_CHANNEL * sizeof(mdl_chn_save_t));
//	if(STRG_SYS.fs.fs_read(fd, p, sizeof(system_conf_t)) == sizeof(system_conf_t))
//	{
//		
//		ret = RET_OK;
//	}
//	STRG_SYS.fs.fs_close(fd);
//	
//	return ret;
//}


//static int	Strg_WR_chn_conf(uint8_t type, void *p)
//{
//	int fd;
//	int	num = 0;
//	int	ret = -1;
//	mdl_chn_save_t	save;
//	
//	if(type < NUM_CHANNEL)
//		num = type;
//	else
//		num = -1;
//	
//	if(num < 0)
//		return -1;
//	if(p == NULL)
//	{
//		p = &save;
//		MdlChn_save_data(num, p);
//	}
//	fd = STRG_SYS.fs.fs_open(STRG_CFG_FSH_NUM, "phn.cfg", "r", 256);
//	STRG_SYS.fs.fs_lseek(fd, WR_SEEK_SET, num * sizeof(mdl_chn_save_t));
//	if(STRG_SYS.fs.fs_write(fd, p, sizeof(mdl_chn_save_t)) == sizeof(mdl_chn_save_t))
//	{
//		Strg_Updata_rcd_mgr(num, p);
//		
//		ret = RET_OK;
//	}
//	STRG_SYS.fs.fs_close(fd);
//	
//	return ret;
//}
//static int	Strg_WR_sys_conf(uint8_t type, void *p)
//{
//	int fd;
//	int	ret = -1;
//	if(!IS_SYS_CONF(type))
//		return -1;
//	
//	fd = STRG_SYS.fs.fs_open(STRG_CFG_FSH_NUM, "phn.cfg", "r", 256);
//	STRG_SYS.fs.fs_lseek(fd, WR_SEEK_SET, NUM_CHANNEL * sizeof(mdl_chn_save_t));
//	if(STRG_SYS.fs.fs_write(fd, p, sizeof(system_conf_t)) == sizeof(system_conf_t))
//	{
//		
//		ret = RET_OK;
//	}
//	STRG_SYS.fs.fs_close(fd);
//	
//	return ret;
//}

static int 	STG_Open_file(uint8_t type, uint32_t file_size)
{
	int 		fd = -1;
	Storage		*stg = Get_storage();
	char		name[8];
	
	
	if(IS_SYS_CONF(type))
	{
		fd = STRG_SYS.fs.fs_open(STRG_CFG_FSH_NUM, "phn.cfg", "r", 256);
		
	}
	else if(IS_CHN_CONF(type))
	{
		//通道配置与系统配置存放在同一个文件的不同位置
		fd = STRG_SYS.fs.fs_open(STRG_CFG_FSH_NUM, "phn.cfg", "r", 256);
		
	}
	else if(IS_LOSE_PWR(type))
	{
		fd = STRG_SYS.fs.fs_open(STRG_RCD_FSH_NUM, "alm_lost_pwr", "r", STG_PAS_SIZE);
		
	}
	else if(IS_CHN_ALARM(type))
	{
		fd = STRG_SYS.fs.fs_open(STRG_RCD_FSH_NUM, "alm_lost_pwr", "r", STG_PAS_SIZE);
		
	}
	else if(IS_CHN_SUM(type))
	{
		fd = STRG_SYS.fs.fs_open(STRG_RCD_FSH_NUM, "alm_lost_pwr", "r", STG_PAS_SIZE);
		
	}
	else if(IS_CHN_DATA(type))
	{
		sprintf(name, "chn_%d", STG_GET_CHN(type));
		if(file_size == STG_DEF_FILE_SIZE)
		{
			fd = STRG_SYS.fs.fs_open(STRG_CHN_DATA_FSH_NUM, name, "wr", \
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
