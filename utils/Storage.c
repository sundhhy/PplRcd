#include "Storage.h"
#include "sdhDef.h"

//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

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
#define STRG_SYS				phn_sys
#define STRG_FSH_NUM		FSH_W25Q_NUM
#define STRG_CFG_FSH_NUM		FSH_FM25_NUM
#define RCD_ALARM_NUM				100   //记录容量少于这个数值时，要产生报警

#define RCD_ERR					1
#define RCD_READED			2
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------
typedef struct {
	uint32_t		rcd_time_s;
	uint8_t			chn_num;
	uint8_t			rcd_flag;
	uint16_t		rcd_val;
	
}data_in_fsh_t;

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
//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
//Observer		strg_mdl_ob;
static uint8_t			stg_buf[4096];
static STG_wr_buf_mgr_t stg_wr_mgr = {0, 4096, stg_buf};
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

static int 	STG_Open_file(uint8_t type);

static int	STG_Acc_chn_conf(uint8_t	chn_num, uint8_t	drc, void *p);
static int	STG_Acc_chn_alarm(uint8_t	chn_num, uint8_t	drc, void *p, int len);
static int	STG_Acc_chn_data(uint8_t	type, uint8_t	drc, void *p, int len);

static int	STG_Acc_sys_conf(uint8_t	drc, void *p);
static int	STG_Acc_lose_pwr(uint8_t	drc, void *p, int len);


//------- 记录通道数据的函数--------------------------------------------------//
static void STG_WR_cache_mgr(void);
static void STG_flush_wr_cache(uint8_t type);

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
Storage		*Get_storage()
{
	static Storage *sington_strg = NULL;
	
	if(sington_strg == NULL)
		sington_strg = Storage_new();
	
	
	return sington_strg;
	
}


int	STG_Read_rcd_by_time(uint8_t	cfg_type, uint32_t start_sec, uint32_t end_sec, void *buf, int buf_size, uint32_t *rd_sec)
{
	Storage				*stg = Get_storage();
	int						fd = -1;
											
	STRG_SYS.fs.fs_lseek(fd, RD_SEEK_SET, 0);
	
	
	
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
//		p_md->attach(p_md, &self->Observer);
//		self->arr_rcd_fd[i] = 0xff;
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
	else if(IS_SYS_CONF(cfg_type))
	{
		ret = STG_Acc_chn_conf(STG_GET_CHN(cfg_type), STG_DRC_READ, buf);
		
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
		ret = STG_Acc_chn_alarm(STG_GET_CHN(cfg_type), STG_DRC_READ, buf, len);
		
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
	else if(IS_SYS_CONF(cfg_type))
	{
		ret = STG_Acc_chn_conf(STG_GET_CHN(cfg_type), STG_DRC_WRITE, buf);
		
	}
	else if(IS_LOSE_PWR(cfg_type))
	{
		ret = STG_Acc_lose_pwr(STG_DRC_WRITE, buf, len);
		
	}
	else if(IS_CHN_DATA(cfg_type))
	{
		ret = STG_Acc_chn_data(STG_GET_CHN(cfg_type), STG_DRC_WRITE, buf, len);
		
	}
	else if(IS_CHN_ALARM(cfg_type))
	{
		ret = STG_Acc_chn_alarm(STG_GET_CHN(cfg_type), STG_DRC_WRITE, buf, len);
		
	}
	
	return ret;
}

static void Strg_Updata_rcd_mgr(uint8_t	num, mdl_chn_save_t *p)
{
	mdl_chn_save_t		*p_save = (mdl_chn_save_t *)p;
	file_info_t			*fnf ;
	Storage				*stg = Get_storage();
	int					fd;
	
	fd = STG_Open_file(STG_CHN_DATA(num));
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
		

	
}

static int	STG_Acc_chn_conf(uint8_t	chn_num, uint8_t	drc, void *p)
{
	
	int fd;
	int	num = 0;
	int	ret = -1;
	
	fd = STG_Open_file(STG_CHN_CONF(chn_num));
	STRG_SYS.fs.fs_lseek(fd, RD_SEEK_SET, num * sizeof(mdl_chn_save_t));
	if(drc == STG_DRC_READ)
	{
		if(STRG_SYS.fs.fs_read(fd, p, sizeof(mdl_chn_save_t)) == sizeof(mdl_chn_save_t))
		{
			Strg_Updata_rcd_mgr(num, p);
			
			ret = RET_OK;
		}
	}
	else
	{
		if(STRG_SYS.fs.fs_write(fd, p, sizeof(mdl_chn_save_t)) == sizeof(mdl_chn_save_t))
		{
			ret = RET_OK;
		}
		Strg_Updata_rcd_mgr(num, p);
	}
	STRG_SYS.fs.fs_close(fd);
	
	return ret;
	
	
}

static int	STG_Acc_sys_conf(uint8_t drc, void *p)
{
	int fd;
	int	ret = -1;

	
	fd = STG_Open_file(STG_SYS_CONF);
	STRG_SYS.fs.fs_lseek(fd, RD_SEEK_SET, NUM_CHANNEL * sizeof(mdl_chn_save_t));
	if(drc == STG_DRC_READ)
	{
		if(STRG_SYS.fs.fs_read(fd, p, sizeof(system_conf_t)) == sizeof(system_conf_t))
		{
			
			ret = RET_OK;
		}
	}
	else
	{
		if(STRG_SYS.fs.fs_write(fd, p, sizeof(system_conf_t)) == sizeof(system_conf_t))
		{
			
			ret = RET_OK;
		}
		
	}
	STRG_SYS.fs.fs_close(fd);
	
	
	
}

static int	STG_Acc_chn_alarm(uint8_t	chn_num, uint8_t	drc, void *p, int len)
{
	
}

//返回写入或者读取的字节数
static int	STG_Acc_chn_data(uint8_t	type, uint8_t	drc, void *p, int len)
{
	STG_cache_t		*c;
	Storage				*stg = Get_storage();
	int						fd = -1;
	uint8_t				t = 0;
	uint8_t				chn_num = STG_GET_CHN(type);
	uint16_t			rd_len = 0;
	if(drc == STG_DRC_READ)
	{
		fd = STG_Open_file(type);
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
#elif STG_RCD_FULL_ACTION == STG_ERASE
			fd = STG_Open_file(type);
			STRG_SYS.fs.fs_erase_file(fd);
#else
			STG_flush_wr_cache(type);
			fd = STG_Open_file(type);
			STRG_SYS.fs.fs_lseek(fd, WR_SEEK_SET, 0);
			stg->arr_rcd_mgr[chn_num].rcd_count = 0;
#endif
			
			
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
	
	return 0;
}

static int	STG_Acc_lose_pwr(uint8_t	drc, void *p, int len)
{
	
	
}



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
	int 					fd = STG_Open_file(type);
	
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

static int 	STG_Open_file(uint8_t type)
{
	int 		fd = -1;
	Storage		*stg = Get_storage();
	char		name[8];
	
	
	if(IS_SYS_CONF(type))
	{
		fd = STRG_SYS.fs.fs_open(STRG_CFG_FSH_NUM, "phn.cfg", "r", 256);
		
	}
	else if(IS_LOSE_PWR(type))
	{
		fd = STRG_SYS.fs.fs_open(STRG_CFG_FSH_NUM, "lose_pwe", "r", 1024);
		
	}
	else if(IS_CHN_CONF(type))
	{
		//通道配置与系统配置存放在同一个文件的不同位置
		fd = STRG_SYS.fs.fs_open(STRG_CFG_FSH_NUM, "phn.cfg", "r", 256);
		
	}
	else if(IS_CHN_DATA(type))
	{
		sprintf(name, "chn_%d", STG_GET_CHN(type));
		fd = STRG_SYS.fs.fs_open(STRG_CFG_FSH_NUM, name, "wr", \
		stg->arr_rcd_mgr[STG_GET_CHN(type)].rcd_maxcount * sizeof(data_in_fsh_t));
		
	}
	else if(IS_CHN_ALARM(type))
	{
		fd = STRG_SYS.fs.fs_open(STRG_CFG_FSH_NUM, "alarm", "r", 1024);
		
	}
	return fd;
}
