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
	
}rcd_channel_t;


	
//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
Observer		strg_mdl_ob;

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
/* Cycle/Sync Callback functions */
static int	Strg_RD_chn_conf(uint8_t type, void *p);
static int	Strg_RD_sys_conf(uint8_t type, void *p);
static int	Strg_WR_chn_conf(uint8_t type, void *p);
static int	Strg_WR_sys_conf(uint8_t type, void *p);
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

int Strg_init(Storage *self)
{
	Model 		*p_md;
	char			chn_name[7];
	char			i;
	uint16_t	pg_size;
	uint32_t	num_pg;
	for(i = 0; i < NUM_CHANNEL; i++)
	{
		sprintf(chn_name,"chn_%d", i);
		p_md = ModelCreate(chn_name);
		p_md->attach(p_md, &self->Observer);

	}
	
	pg_size = STRG_SYS.arr_fsh[STRG_FSH_NUM].fnf.page_size;
	num_pg = STRG_SYS.arr_fsh[STRG_FSH_NUM].fnf.total_pagenum;
	
	
	
	
	self->rcd_mgr_fd = STRG_SYS.fs.fs_open(STRG_FSH_NUM, "mdl_chn.mgr", "rw", sizeof(rcd_mgr_t) * NUM_CHANNEL);
	num_pg --;
	//为了减少w25q的擦写次数，把所有通道都记录在一个文件里面，这样在切换记录通的时候，能够尽量在同一个扇区内
	self->rcd_fd = STRG_SYS.fs.fs_open(STRG_FSH_NUM, "mdl_chn.rcd", "rw",pg_size * num_pg - sizeof(self->arr_rcd_mgr));
	if((self->rcd_fd >= 0) && (self->rcd_mgr_fd >= 0))
		return RET_OK;
	else
		return ERR_FS_OPEN_FAIL;
	
	
}

int			Strg_rd_stored_data(Storage *self, uint8_t	cfg_type, void *cfg_buf)
{
	
		Strg_RD_chn_conf(cfg_type, cfg_buf);
		Strg_RD_sys_conf(cfg_type, cfg_buf);
		
	
	
//	self->chn_cfg_fd = STRG_SYS.fs.fs_open(STRG_CFG_FSH_NUM, "mdl_chn.cfg", "rw", (sizeof(chn_info_t) + sizeof(chn_alarm_t)) * NUM_CHANNEL);
//	if(self.chn_cfg_fd < 0)
//	{
//		
//		return ERR_FS_OPEN_FAIL;
//		
//	}
	
	return RET_OK;
	
}

int		Strg_WR_stored_data(Storage *self, uint8_t	cfg_type, void *buf)
{
	Strg_WR_chn_conf(cfg_type, buf);
	Strg_WR_sys_conf(cfg_type, buf);
	return RET_OK;
}

int Save_channel_data( Observer *self, void *p_srcMdl)
{
	
	Storage 				*cthis = SUB_PTR(self, Observer, Storage);
	Model 						*p_mdl;
	rcd_channel_t			rchn = {0};
	
	p_mdl = ModelCreate("time");
	p_mdl->getMdlData(p_mdl, TIME_U32, &rchn.rcd_time_s);
	
	p_mdl = (Model *)p_srcMdl;
	if(p_mdl->getMdlData(p_mdl, AUX_DATA, &rchn.rcd_val) < 0)
		rchn.rcd_flag |= RCD_ERR;
	p_mdl->getMdlData(p_mdl, MDHCHN_CHN_NUM, &rchn.chn_num);
	if(cthis->arr_rcd_mgr[rchn.chn_num].rcd_count < cthis->arr_rcd_mgr[rchn.chn_num].rcd_maxcount)
	{
		cthis->arr_rcd_mgr[rchn.chn_num].rcd_count ++;
		STRG_SYS.fs.fs_write(cthis->rcd_fd, (uint8_t *)&rchn, sizeof(rcd_channel_t));
	}
	else
	{
		
		
	}
	
	
}


CTOR(Storage)
FUNCTION_SETTING(init, Strg_init);
FUNCTION_SETTING(rd_stored_data, Strg_rd_stored_data);
FUNCTION_SETTING(wr_stored_data, Strg_WR_stored_data);

FUNCTION_SETTING(Observer.update, Save_channel_data);


END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{
static void Strg_Updata_rcd_mgr(uint8_t	num, mdl_chn_save_t *p)
{
	mdl_chn_save_t	*p_save = (mdl_chn_save_t *)p;
//	file_info_t			*fnf ;
	Storage					*stg = Get_storage();
	if(p == NULL)
		return;
//	fnf = STRG_SYS.fs.fs_file_info(stg->rcd_fd);
	stg->arr_rcd_mgr[num].rcd_maxcount = p_save->MB * 1024 * 1024 / sizeof(rcd_channel_t);
//	stg->arr_rcd_mgr[num].rcd_count = fnf->write_position / sizeof(rcd_channel_t);
	
}
static int	Strg_RD_chn_conf(uint8_t type, void *p)
{
	int fd;
	int	num = 0;
	int	ret = -1;
	
	
	if(type < NUM_CHANNEL)
		num = type;
	else
		num = -1;
	
	if(num < 0)
		return -1;
	
	fd = STRG_SYS.fs.fs_open(STRG_CFG_FSH_NUM, "phn.cfg", "r", 256);
	STRG_SYS.fs.fs_lseek(fd, RD_SEEK_SET, num * sizeof(mdl_chn_save_t));
	if(STRG_SYS.fs.fs_read(fd,p, sizeof(mdl_chn_save_t)) == sizeof(mdl_chn_save_t))
	{
		Strg_Updata_rcd_mgr(num, p);
		
		ret = RET_OK;
	}
	STRG_SYS.fs.fs_close(fd);
	
	return ret;
}
static int	Strg_RD_sys_conf(uint8_t type, void *p)
{
	int fd;
	int	num = 0;
	int	ret = -1;
	if(!IS_SYS_CONF(type))
		return -1;
	
	fd = STRG_SYS.fs.fs_open(STRG_CFG_FSH_NUM, "phn.cfg", "r", 256);
	STRG_SYS.fs.fs_lseek(fd, RD_SEEK_SET, NUM_CHANNEL * sizeof(mdl_chn_save_t));
	STRG_SYS.fs.fs_lseek(fd, RD_SEEK_SET, num * sizeof(mdl_chn_save_t));
	if(STRG_SYS.fs.fs_read(fd, p, sizeof(system_conf_t)) == sizeof(system_conf_t))
	{
		
		ret = RET_OK;
	}
	STRG_SYS.fs.fs_close(fd);
	
	return ret;
}


static int	Strg_WR_chn_conf(uint8_t type, void *p)
{
	int fd;
	int	num = 0;
	int	ret = -1;
	
	
	if(type < NUM_CHANNEL)
		num = type;
	else
		num = -1;
	
	if(num < 0)
		return -1;
	
	fd = STRG_SYS.fs.fs_open(STRG_CFG_FSH_NUM, "phn.cfg", "r", 256);
	STRG_SYS.fs.fs_lseek(fd, RD_SEEK_SET, num * sizeof(mdl_chn_save_t));
	if(STRG_SYS.fs.fs_write(fd,p, sizeof(mdl_chn_save_t)) == sizeof(mdl_chn_save_t))
	{
		Strg_Updata_rcd_mgr(num, p);
		
		ret = RET_OK;
	}
	STRG_SYS.fs.fs_close(fd);
	
	return ret;
}
static int	Strg_WR_sys_conf(uint8_t type, void *p)
{
	int fd;
	int	num = 0;
	int	ret = -1;
	if(!IS_SYS_CONF(type))
		return -1;
	
	fd = STRG_SYS.fs.fs_open(STRG_CFG_FSH_NUM, "phn.cfg", "r", 256);
	STRG_SYS.fs.fs_lseek(fd, RD_SEEK_SET, NUM_CHANNEL * sizeof(mdl_chn_save_t));
	STRG_SYS.fs.fs_lseek(fd, RD_SEEK_SET, num * sizeof(mdl_chn_save_t));
	if(STRG_SYS.fs.fs_write(fd, p, sizeof(system_conf_t)) == sizeof(system_conf_t))
	{
		
		ret = RET_OK;
	}
	STRG_SYS.fs.fs_close(fd);
	
	return ret;
}
