//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//
/*

V010 171226 :
支持：
支持多个分区，每个分区对应一块flash

不支持：

不支持文件的分散存储，所有的文件都是连续的。
不支持动态扩展,文件一旦建立，长度就固定了。
不支持一个存储器上多个分区。
对文件的读写权限不作处理，即不对文件的读写进行权限判断
*/
#include "fs/easy_fs.h"
#include <string.h>
#include "sdhDef.h"
#ifdef NO_ASSERT
#include "basis/assert.h"
#else
#include "assert.h"
#endif
#include "mem/CiiMem.h"
#include "os/os_depend.h"
#include "sys_cmd.h"
#include "arithmetic/bit.h"

#include "HMI/HMI.h"

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
//#define FS_RLB_LEVEL					1	
#define	EFS_MGR_FSH_NO					0
#define	EFS_MGR_NUM_FSH					2
#define EFS_NAME_LEN					14
#define EFS_MAX_NUM_FILES				10



#define EFS_NUM_IDLE_FILES				0		//空闲区域


#define EFS_SYS								phn_sys
#define EFS_FS								phn_sys.fs
#define EFS_FSH(NO)							phn_sys.arr_fsh[NO]
#define EFS_LOWSPACE_ALARM_0				0
#define EFS_LOWSPACE_ALARM_1				64



//文件系统的标志
#define EFS_BUSY							0x1

#define EFS_WAIT_WR_MS						1000
#define EFS_FLUSH_CYCLE_S					5

const Except_T EFS_Failed = { "Alloc Sheet Failed" };
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

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------
typedef struct {
	char 		efs_flag;
	char		efile_fsh_NO;
	char		efile_name[EFS_NAME_LEN];
	uint16_t	efile_start_pg;
	uint16_t	efile_num_pg;
	uint32_t	efile_wr_position;
//	uint16_t	efile_space_low_bytes;		//文件容量小于这个值的时候，报警
}efs_file_mgt_t;

typedef struct {
	uint32_t		start_addr;
	uint32_t		free_bytes;
	
	
}space_t;
	
typedef struct {
	uint8_t					free_spac_num;
	uint8_t					efs_flag;
	uint8_t					none[2];
	uint8_t					*pg_buf;
	
	//arr_static_info是会存到flash里面的 arr_dynamic_info不会，只是运行中的数据
	efs_file_mgt_t  	arr_static_info[EFS_MAX_NUM_FILES + EFS_NUM_IDLE_FILES];			//文件管理要留一点空间
	file_info_t			arr_dynamic_info[EFS_MAX_NUM_FILES];
}efs_mgr_t;
//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
static efs_mgr_t 		efs_mgr;

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
int		EFS_open(uint8_t		prt, char *path, char *mode, int	file_size);
int		EFS_close(int fd);
int		EFS_write(int fd, uint8_t *p, int len);
int		EFS_Raw_write(int fd, uint8_t *p, int len);
int		EFS_read(int fd, uint8_t *p, int len);
int		EFS_Raw_read(int fd, uint8_t *p, int len);

int 	EFS_Lseek(int fd, int whence, int32_t offset);
int	EFS_resize(int fd, char *path, int new_size);
file_info_t		*EFS_file_info(int fd);
int		EFS_delete(int fd, char *path);
void 	EFS_Erase_file(int fd, uint32_t start, uint32_t size);
void 	EFS_Shutdown(void);

static int EFS_format(void);
static int EFS_search_file(char *path);
static int EFS_create_file(uint8_t	fd, uint8_t	prt, char *path, int size);


static void EFS_file_mgr_info(efs_file_mgt_t	*file_mgr, file_info_t *file_info);
//static int EFS_malloc_file_info(void);
static int EFS_malloc_file_mgr(void);
static void	EFS_flush_mgr(int No);
static void EFS_Change_file_size(int fd, uint32_t new_size);
static void	EFS_Regain_space(void);
static void	EFS_flush_wr_position(int fd);
static void EFS_run(void);
static void EFS_Flush(void *);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
int 	EFS_init(char num_flash, char read_back_check)
{
	short	i, pg_size = 0;
	EFS_FS.num_partitions = num_flash;
	EFS_FS.reliable_level = read_back_check;
	if(EFS_FS.reliable_level)
	{
		for(i = 0; i < num_flash; i ++)
			EFS_FSH(i).fnf.fnf_flag |= FSH_FLAG_READBACK_CHECK;
		
	}
	
	for(i = 0; i < num_flash; i ++)
	{
		if(EFS_FSH(i).fnf.page_size > pg_size)
			pg_size = EFS_FSH(i).fnf.page_size;
		
	}
	for(i = 0; i < EFS_MAX_NUM_FILES; i ++)
	{
		
		efs_mgr.arr_dynamic_info[i].file_sem = 0xff;
	}
	efs_mgr.pg_buf = ALLOC(pg_size);
	
	
	EFS_FS.fs_open = EFS_open;
	EFS_FS.fs_close = EFS_close;
	EFS_FS.fs_lseek = EFS_Lseek;
	
	EFS_FS.fs_read = EFS_read;
	EFS_FS.fs_write = EFS_write;
	
	EFS_FS.fs_raw_write = EFS_Raw_write;
	EFS_FS.fs_raw_read = EFS_Raw_read;
	EFS_FS.fs_delete = EFS_delete;
	EFS_FS.fs_resize = EFS_resize;
	EFS_FS.fs_erase_file = EFS_Erase_file;
	EFS_FS.fs_shutdown = EFS_Shutdown;
	EFS_FS.fs_file_info = EFS_file_info;
	
	Cmd_Rgt_idle_task(EFS_run);
	Cmd_Rgt_time_task(EFS_Flush, NULL, EFS_FLUSH_CYCLE_S);
	return EFS_format();
	
	
}

void 	EFS_Shutdown(void)
{
	int i;
	
//	for(i = 0; i < NUM_FSH; i ++)
//	{
//		EFS_FSH(i).fsh_flush();
//		
//	}
	
	
	for(i = 0; i < EFS_MAX_NUM_FILES; i++)
	{
		if(efs_mgr.arr_dynamic_info[i].file_flag & EFILE_OPENED)
		{
			EFS_close(i);
			
		}
		
	}
	
	
	
}
static void EFS_Init_file_sem(int fd)
{
	int sm;
	if((fd >= 0) && (efs_mgr.arr_dynamic_info[fd].file_sem == 0xff))
	{
		sm = Alloc_sem();
		if(sm < 0)
			Except_raise(&EFS_Failed, __FILE__, __LINE__);
		Sem_init(&sm);
		Sem_post(&sm);
		efs_mgr.arr_dynamic_info[fd].file_sem = sm;
	}
	
}
//mode:rw 或者log 为Log表示这个文件时log文件，不应该在文件系统复位时擦除
int	EFS_open(uint8_t prt, char *path, char *mode, int	file_size)
{
	int new_fd = 0;
	char *p;
//	int	i = 0;
	
	//先看看有没有已经存在
	new_fd = EFS_search_file(path);
	
	
	//创建新的文件
	if(new_fd < 0)
	{
		new_fd = EFS_malloc_file_mgr();
		new_fd = EFS_create_file(new_fd, prt, path, file_size);
		
		
		
		EFS_Init_file_sem(new_fd);
		
		
		EFS_Erase_file(new_fd, 0, 0);
		
	}
	if(new_fd >= 0)
	{
		//如果第一次打开的话，就初始化读取位置为0
		//不每次打开都初始化读取位置是因为有些场合下不希望该值被初始化为0
		if((efs_mgr.arr_dynamic_info[new_fd].file_flag & EFILE_OPENED) == 0)
			efs_mgr.arr_dynamic_info[new_fd].read_position = 0;
		efs_mgr.arr_dynamic_info[new_fd].file_flag |= EFILE_OPENED;
		
	}
	
	
	p = strstr(mode, "log");
	if(p)
	{
		
		efs_mgr.arr_static_info[new_fd].efs_flag  |= EFILE_LOG;
	}
	
	EFS_Init_file_sem(new_fd);
//	if((new_fd >= 0) && (efs_mgr.arr_dynamic_info[new_fd].file_sem == 0xff))
//	{
//		i = Alloc_sem();
//		if(i < 0)
//			Except_raise(&EFS_Failed, __FILE__, __LINE__);
//		Sem_init(&i);
//		Sem_post(&i);
//		efs_mgr.arr_dynamic_info[new_fd].file_sem = i;
//	}
//		
	
	return new_fd;
}
int	EFS_close(int fd)
{
//	file_info_t *f = &efs_mgr.arr_dynamic_info[fd];
//	efs_file_mgt_t	*f_mgr = &efs_mgr.arr_static_info[fd];
	
	if(fd > EFS_MAX_NUM_FILES)
		return -1;
	
	
	efs_mgr.arr_dynamic_info[fd].read_position = 0;
	
	EFS_flush_wr_position(fd);
	//更新写位置
//	if(Sem_wait(&f->file_sem, EFS_WAIT_WR_MS) <= 0)												
//		return -1;
//	if(f_mgr->efile_wr_position != f->write_position)
//	{
//		
//		f_mgr->efile_wr_position = f->write_position;
//		EFS_flush_mgr(fd);
//	}
	efs_mgr.arr_dynamic_info[fd].file_flag &= ~EFILE_OPENED;
//	Sem_post(&f->file_sem);
	return 0;
}

int 	EFS_Lseek(int fd, int whence, int32_t offset)
{
	file_info_t *f = &efs_mgr.arr_dynamic_info[fd];
	efs_file_mgt_t	*f_mgr = &efs_mgr.arr_static_info[fd];
	switch( whence)
	{
		case WR_SEEK_SET:
			if(EFS_SYS.sys_flag & SYSFLAG_URGENCY)
				goto urgency;
			if(Sem_wait(&f->file_sem, EFS_WAIT_WR_MS) <= 0)
				return -1;
			urgency:
			f->write_position = offset;
			f_mgr->efile_wr_position = offset;
			if(EFS_SYS.sys_flag & SYSFLAG_URGENCY)
				break;
			Sem_post(&f->file_sem);
			break;
		case WR_SEEK_CUR:
			f->write_position += offset;
			f_mgr->efile_wr_position += offset;
			break;
		
		
		case WR_SEEK_END:			//连续5个0xff作为结尾

		
			break;
		
		case RD_SEEK_SET:
			f->read_position = offset;
			break;
		case RD_SEEK_CUR:
			f->read_position += offset;
			break;
		
		
		case RD_SEEK_END:
			
		
			break;
		case GET_WR_END:
			return f->file_size;
		
		default:
			break;
		
	}
	
	return ERR_OK;
	
}
int	EFS_delete(int fd, char *path)
{
	efs_file_mgt_t  		*ef = NULL;
	
	if(fd < 0)
	{
		
		fd = EFS_search_file(path);
	}
	if(fd < 0)
		goto exit;
	
	ef = &efs_mgr.arr_static_info[fd];
	memset(ef, 0, sizeof(efs_file_mgt_t));
	exit:
	return 0;
}
int		EFS_Raw_read(int fd, uint8_t *p, int len)
{
	
	int   ret = 0;
	file_info_t *f = &efs_mgr.arr_dynamic_info[fd];
	int			start_addr = f->start_page * EFS_FSH(f->fsh_No).fnf.page_size;

	if(fd > EFS_MAX_NUM_FILES)
		return -1;
	if(f->file_flag & EFILE_ERASE)
		return ERR_FS_NOT_READY;
	
	if(f->read_position >= f->file_size)
		return 0;
	
	if(efs_mgr.efs_flag & EFS_BUSY)
		return ERR_FS_NOT_READY;
	
	while(Sem_wait(&f->file_sem, EFS_WAIT_WR_MS) <= 0)
		delay_ms(1);
	//这个判断没意思，不会出现这种情况
//	if(len > f->num_page * EFS_FSH(f->fsh_No).fnf.page_size)
//		len = f->num_page * EFS_FSH(f->fsh_No).fnf.page_size;
	
	
	
	
	if((len + f->read_position) >= f->file_size)
		len = f->file_size - f->read_position;
	
	while(EFS_FSH(f->fsh_No).fsh_lock(100) != RET_OK)
	{
		
		delay_ms(1);
	}
		
	ret =  EFS_FSH(f->fsh_No).fsh_raw_read(p, start_addr + f->read_position,len);
	
	EFS_FSH(f->fsh_No).fsh_unlock();
	
	if(ret > 0)
		f->read_position += ret;
	Sem_post(&f->file_sem);
	return ret;
	
}
int		EFS_Raw_write(int fd, uint8_t *p, int len)
{
	
	int   ret = 0;
	file_info_t *f = &efs_mgr.arr_dynamic_info[fd];
	efs_file_mgt_t  *ef = &efs_mgr.arr_static_info[fd];
	int			start_addr = f->start_page * EFS_FSH(f->fsh_No).fnf.page_size;

	if(fd > EFS_MAX_NUM_FILES)
		return RET_FAILED;
	if(f->file_flag & EFILE_ERASE)
		return ERR_FS_NOT_READY;
	
	if(f->write_position >= f->file_size)
		return 0;
	
	
	if(efs_mgr.efs_flag & EFS_BUSY)
		return ERR_FS_NOT_READY;
	while(Sem_wait(&f->file_sem, EFS_WAIT_WR_MS) <= 0)
		delay_ms(1);
	//这个判断没意思，不会出现这种情况
//	if(len > f->num_page * EFS_FSH(f->fsh_No).fnf.page_size)
//		len = f->num_page * EFS_FSH(f->fsh_No).fnf.page_size;
	
	
	
	if((len + f->write_position) >= f->file_size)
		len = f->file_size - f->write_position;

	if(EFS_FSH(f->fsh_No).fsh_lock(10) != RET_OK)
	{
		ret =  ERR_RSU_BUSY;
		goto exit;
	}
	
	ret =  EFS_FSH(f->fsh_No).fsh_raw_write(p, start_addr + f->write_position,len);
	if(ret > 0)
		f->write_position += ret;
	else
		ret = ERR_DEV_FAILED;
	EFS_FSH(f->fsh_No).fsh_unlock();
	
	
//	if((f->num_page - f->write_position / EFS_FSH(f->fsh_No).fnf.page_size) < f->low_pg)
//		EFS_FS.err_code |= FS_ALARM_LOWSPACE;
//	else
//		EFS_FS.err_code &= ~FS_ALARM_LOWSPACE;
	ef->efile_wr_position = f->write_position;
	
	EFS_flush_mgr(fd);
	exit:
	Sem_post(&f->file_sem);
	return ret;
	
	
}

int	EFS_write(int fd, uint8_t *p, int len)
{
	int   ret;
	file_info_t *f = &efs_mgr.arr_dynamic_info[fd];
	int			start_addr = f->start_page * EFS_FSH(f->fsh_No).fnf.page_size;

	if(EFS_SYS.sys_flag & SYSFLAG_URGENCY)
		goto urgency_wr;
	
	if(fd > EFS_MAX_NUM_FILES)
		return -1;
	
	if(f->file_flag & EFILE_ERASE)
		return ERR_FS_NOT_READY;
	if(efs_mgr.efs_flag & EFS_BUSY)
		return ERR_FS_NOT_READY;
	
	if(len > f->num_page * EFS_FSH(f->fsh_No).fnf.page_size)
		len = f->num_page * EFS_FSH(f->fsh_No).fnf.page_size;
	
	
	if(Sem_wait(&f->file_sem, EFS_WAIT_WR_MS) <= 0)
	{
		return -1;
	}
	
	urgency_wr:
	ret =  EFS_FSH(f->fsh_No).fsh_write(p, start_addr + f->write_position,len);
	if(ret > 0)
		f->write_position += ret;
	if(EFS_SYS.sys_flag & SYSFLAG_URGENCY)		//紧急情况下，没有获取信号量，也就不用执行释放信号量了
		return ret;
//	if((f->num_page - f->write_position / EFS_FSH(f->fsh_No).fnf.page_size) < f->low_pg)
//		EFS_FS.err_code |= FS_ALARM_LOWSPACE;
//	else
//		EFS_FS.err_code &= ~FS_ALARM_LOWSPACE;
	Sem_post(&f->file_sem);
	return ret;
	
}
int	EFS_read(int fd, uint8_t *p, int len)
{
	int 		ret;
	file_info_t *f = &efs_mgr.arr_dynamic_info[fd];
	int			start_addr = f->start_page * EFS_FSH(f->fsh_No).fnf.page_size;

	if(fd > EFS_MAX_NUM_FILES)
		return -1;
	if(f->file_flag & EFILE_ERASE)
		return ERR_FS_NOT_READY;
	if(efs_mgr.efs_flag & EFS_BUSY)
		return ERR_FS_NOT_READY;
//	if(f->read_position >= f->write_position)
//		return 0;
	if(len > f->num_page * EFS_FSH(f->fsh_No).fnf.page_size)
		len = f->num_page * EFS_FSH(f->fsh_No).fnf.page_size;
	while(Sem_wait(&f->file_sem, EFS_WAIT_WR_MS) <= 0)
		delay_ms(1);
	ret =  EFS_FSH(f->fsh_No).fsh_read(p, start_addr + f->read_position,len);
	
	if(ret > 0)
		f->read_position += ret;
	
	Sem_post(&f->file_sem);
	
	return ret;
}

void 	EFS_Erase_file(int fd, uint32_t start, uint32_t size)
{
	file_info_t *f = &efs_mgr.arr_dynamic_info[fd];
	int			start_addr = f->start_page * EFS_FSH(f->fsh_No).fnf.page_size;
	int			sz = f->file_size;

	if(fd > EFS_MAX_NUM_FILES)
		return;
	
	if((start + size) == 0)
	{
		//只擦除部分文件的时候就只能直接进行擦除了
		//因为外部线程无法获取部分擦除的信息
		f->file_flag |= EFILE_ERASE;
		
		return;
	}
	
	while(Sem_wait(&f->file_sem, EFS_WAIT_WR_MS) <= 0)
		delay_ms(1);

	
	if((start + size) > 0)
	{
		start_addr += start;
		
		sz = size;
	}
	
	
	EFS_FSH(f->fsh_No).fsh_ersse_addr(start_addr, sz);
	f->write_position = 0;
	f->read_position = 0;
	Sem_post(&f->file_sem);
}

//用fd或者path来指定文件
int	EFS_resize(int fd, char *path, int new_size)
{
	file_info_t 			*f = NULL;
	file_info_t				tmp_file_info;
	uint32_t				start_addr = 0;
	uint32_t				new_start_addr;
	uint32_t				end_pg;
	uint32_t				pg_size = 0;
	uint32_t				old_size;
	int						ret;
	int						i;
	
	if(fd < 0)
	{
		
		fd = EFS_search_file(path);
	}
	
	if(fd < 0)
		return -1;
	
	f = &efs_mgr.arr_dynamic_info[fd];
	pg_size = EFS_FSH(f->fsh_No).fnf.page_size;
	start_addr = f->start_page * EFS_FSH(f->fsh_No).fnf.page_size;

	//对比原大小，来判断空间变大还是变小
	old_size = f->file_size;
	
	//变小则直接将原大小变小即可
	if(new_size <= old_size)
	{
		EFS_Change_file_size(fd, new_size);
		return  fd;
		
	}
	//变大要重新创建该文件
		//标记原区域为待回收
	
//	if(Sem_wait(&f->file_sem, EFS_WAIT_WR_MS) <= 0)
//		return -1;
	efs_mgr.arr_static_info[fd].efs_flag |= EFILE_RECYCLE;
	EFS_file_mgr_info(&efs_mgr.arr_static_info[fd], &tmp_file_info);
	tmp_file_info.write_position = f->write_position;
	tmp_file_info.read_position = f->read_position;
	
		//重新创建该文件
	ret = EFS_create_file(fd, f->fsh_No, efs_mgr.arr_static_info[fd].efile_name, new_size);
	
	if(ret == fd)
			//找到了就拷贝到新的空间上去
	{
		
		//把原来的读写位置复制给新的文件区
		new_start_addr = efs_mgr.arr_static_info[fd].efile_start_pg * pg_size;
		end_pg = tmp_file_info.write_position / pg_size + 1;		 //只复制已经被写入的部分
		if(tmp_file_info.write_position == 0)		//如果没有写入过，就不必拷贝
			end_pg = 0;
		for(i = 0; i < end_pg; i ++)
		{
			ret = EFS_FSH(f->fsh_No).fsh_read(efs_mgr.pg_buf, start_addr + pg_size * i, pg_size);
			if(ret != pg_size)
			{
				
				goto ERR_RECOVER;
			}
			ret = EFS_FSH(f->fsh_No).fsh_write(efs_mgr.pg_buf, new_start_addr + pg_size * i, pg_size);
			
		}
		
		
		efs_mgr.arr_static_info[fd].efile_wr_position = tmp_file_info.write_position;
		EFS_flush_mgr(fd);
		f->write_position = tmp_file_info.write_position;
		f->read_position = tmp_file_info.read_position;
		
		efs_mgr.arr_static_info[fd].efs_flag &= ~EFILE_RECYCLE;
//		Sem_post(&f->file_sem);
		return fd;
		
	}
			//找不到则去除待回收标志，返回错误
	
	
	else
	{
		ERR_RECOVER:	
//		Sem_post(&f->file_sem);
		efs_mgr.arr_static_info[fd].efs_flag &= ~EFILE_RECYCLE;
		return -1;
	}
}
file_info_t		*EFS_file_info(int fd)
{
	return efs_mgr.arr_dynamic_info + fd;
}

void 	EFS_Reset(void)
{
//	uint8_t		ver[2];
	char			i;
	
	
	efs_mgr.efs_flag |= EFS_BUSY;
	
//	for(i = 1; i < NUM_FSH; i ++)		//文件管理部分时不能擦除的
//	{
//		EFS_FSH(i).fsh_ersse(FSH_OPT_CHIP, 0);
//		
//	}
	for(i = 0; i < EFS_MAX_NUM_FILES; i++)
	{
		//擦除所有已经建立的文件
		if(((efs_mgr.arr_static_info[i].efs_flag & EFILE_LOG) == 0) && (efs_mgr.arr_static_info[i].efs_flag & EFILE_USED))
		{
			EFS_delete(i, NULL);
//			EFS_Erase_file(i, 0, 0);
			EFS_flush_mgr(i);		
		}
		
	}

	efs_mgr.efs_flag &= ~EFS_BUSY;	
	
//	
//	ver[0] = EFS_SYS.major_ver ;
//	ver[1] = EFS_SYS.minor_ver;
//	EFS_FSH(EFS_MGR_FSH_NO).fsh_write(ver, 0, 2);
//	memset((uint8_t *)efs_mgr.arr_static_info, 0, sizeof(efs_mgr.arr_static_info));
//	EFS_FSH(EFS_MGR_FSH_NO).fsh_write((uint8_t *)efs_mgr.arr_static_info, 2, sizeof(efs_mgr.arr_static_info));
	
	
}
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{
static int EFS_format(void)
{
	int 	ret = RET_OK;
	uint8_t		ver[2];
//	uint8_t		i = 0;
	
	EFS_FSH(EFS_MGR_FSH_NO).fsh_read(ver, 0, 2);
//	
	if(ver[0] != EFS_SYS.major_ver || ver[1] != EFS_SYS.minor_ver)
	{
		

		ver[0] = EFS_SYS.major_ver ;
		ver[1] = EFS_SYS.minor_ver;
		EFS_FSH(EFS_MGR_FSH_NO).fsh_write(ver, 0, 2);
//		memset((uint8_t *)efs_mgr.arr_static_info, 0, sizeof(efs_mgr.arr_static_info));
//		EFS_FSH(EFS_MGR_FSH_NO).fsh_write((uint8_t *)efs_mgr.arr_static_info, 2, sizeof(efs_mgr.arr_static_info));
		ret = RET_FAILED;
		
	}
	
	EFS_FSH(EFS_MGR_FSH_NO).fsh_read((uint8_t *)efs_mgr.arr_static_info, 2, sizeof(efs_mgr.arr_static_info));
	return ret;
}



static void	EFS_flush_wr_position(int fd)
{
	file_info_t *f = &efs_mgr.arr_dynamic_info[fd];
	efs_file_mgt_t	*f_mgr = &efs_mgr.arr_static_info[fd];
	
	if(fd > EFS_MAX_NUM_FILES)
		return;
	
	

	if(f_mgr->efile_wr_position != f->write_position)
	{
		
		f_mgr->efile_wr_position = f->write_position;
		EFS_flush_mgr(fd);
	}
}

static void 	EFS_Flush(void *arg)
{
	int i;
	Cmd_Rgt_time_task(EFS_Flush, NULL, EFS_FLUSH_CYCLE_S);
	for(i = 0; i < NUM_FSH; i ++)
	{
		EFS_FSH(i).fsh_flush();
		
	}
	for(i = 0; i < EFS_MAX_NUM_FILES; i++)
		EFS_flush_wr_position(i);
	
	
	
	
	
}

static void EFS_Erase(int fd)
{
	file_info_t *f = &efs_mgr.arr_dynamic_info[fd];
	int			start_addr = f->start_page * EFS_FSH(f->fsh_No).fnf.page_size;
	int			sz = f->file_size;

	if(fd > EFS_MAX_NUM_FILES)
		return;
	
	
//	while(Sem_wait(&f->file_sem, EFS_WAIT_WR_MS) <= 0)
//		delay_ms(1);

	EFS_FSH(f->fsh_No).fsh_ersse_addr(start_addr, sz);
	f->write_position = 0;
	f->read_position = 0;
//	Sem_post(&f->file_sem);
}

static void EFS_run()
{
	file_info_t *f;
//	uint8_t		set_done[EFS_MAX_NUM_FILES/8 + 1] = {0};
	
	uint8_t		i = 0;
	char		need_erase = 0;
	
	
	//先检查一遍，是否有文件需要擦除
	for(i = 0; i < EFS_MAX_NUM_FILES; i++)
	{
		
		f = &efs_mgr.arr_dynamic_info[i];
		if(f->file_flag & EFILE_ERASE)
		{
			need_erase = 1;
		}
	}
	
	if(need_erase)
	{
		efs_mgr.efs_flag |= EFS_BUSY;
		EFS_SYS.sys_flag |= SYSFLAG_EFS_NOTREADY;
		HMI_TIP_ICO(TIP_ICO_CLEAR_FILE, 1);
	}
	else
	{
		efs_mgr.efs_flag &= ~EFS_BUSY;
		EFS_SYS.sys_flag &= ~SYSFLAG_EFS_NOTREADY;
		HMI_TIP_ICO(TIP_ICO_CLEAR_FILE, 0);
	}
		
	
	if(need_erase)
	{
		if(EFS_FSH(f->fsh_No).fsh_lock(100) != RET_OK)
			return;
		
	}
	
	for(i = 0; i < EFS_MAX_NUM_FILES; i++)
	{
		
		f = &efs_mgr.arr_dynamic_info[i];
		if(f->file_flag & EFILE_ERASE)
		{
				
			
				
			EFS_Erase(i);
//				Set_bit(set_done, i);
			f->file_flag &=~ EFILE_ERASE;
			efs_mgr.arr_static_info[i].efile_wr_position = 0;
				
				
		}
		
	}
	
	if(need_erase)
	{
		EFS_FSH(f->fsh_No).fsh_unlock();
		
	}
	
	//放在这里统一清除擦除标志，是为了避免系统在擦除文件的时候，其他线程发起对flash的读写操作导致错误
//	for(i = 0; i < EFS_MAX_NUM_FILES; i++)
//	{
//		if(efs_mgr.arr_dynamic_info[i].file_flag & EFILE_ERASE)
//		{
//			if(Check_bit(set_done, i))		//避免冲突
//				efs_mgr.arr_dynamic_info[i].file_flag &=~ EFILE_ERASE;
//		}
//		
//	}
}

static void	EFS_flush_mgr(int No)
{
	efs_file_mgt_t	*f_mgr = &efs_mgr.arr_static_info[No];
	
	
		
	EFS_FSH(EFS_MGR_FSH_NO).fsh_write((uint8_t *)f_mgr, No * sizeof(efs_file_mgt_t) + 2,  sizeof(efs_file_mgt_t));
	

}

static int EFS_search_file(char *path)
{
	int i;
	for(i = 0; i < EFS_MAX_NUM_FILES; i++)
	{
		if(strcmp(efs_mgr.arr_static_info[i].efile_name, path) == 0)
		{
			
//			j = EFS_malloc_file_info();
//			if(j < 0)
//				return -1;
			EFS_file_mgr_info(&efs_mgr.arr_static_info[i], &efs_mgr.arr_dynamic_info[i]);
//			efs_mgr.arr_dynamic_info[i].fs_mgr = i;
			return i;
			
		}
		
	}
	
	return -1;
	
}

static void EFS_Regain_space(void)
{
	
	efs_mgr.free_spac_num = 0;
}
 
//根据前一个被使用的末尾地址和下一个被使用的起始地址之间的差值来作为空闲空间
static int EFS_Cal_free_space(uint8_t prt, space_t *fsp)
{
	
	uint32_t	usd_addr_1 = 0, usd_addr_2 = 0, use_size_1 = 0, tmp_addr = 0;
	uint32_t	file_start_addr;
	short		fsp_num = efs_mgr.free_spac_num ++;		//获得第n个空闲空间
	short		count = 0;
	short 		i;

	if(efs_mgr.free_spac_num > EFS_MAX_NUM_FILES + 2)
		return -1;
	
	//找到第一个使用的空间
	if((prt == EFS_MGR_FSH_NO) && (fsp_num == 0))
	{
		use_size_1 = sizeof(efs_mgr.arr_static_info);
		usd_addr_1 = 0;
		
	}
	else
	{
		usd_addr_1 = 0;
		use_size_1 = 0;
		
	}
	count = 0;
	//找到指定序号的前一个末尾地址
	for(i = 0; i < EFS_MAX_NUM_FILES; i++)
	{
		if(count == fsp_num)
			break;
		
		if((efs_mgr.arr_static_info[i].efs_flag & EFILE_USED) == 0)
			continue;
		if(efs_mgr.arr_static_info[i].efs_flag & EFILE_RECYCLE)
			continue;
		
		if(efs_mgr.arr_static_info[i].efile_fsh_NO != prt)
			continue;
	
		count ++;	
		if(count == fsp_num)
		{
			usd_addr_1 = efs_mgr.arr_static_info[i].efile_start_pg * EFS_FSH(prt).fnf.page_size;
			use_size_1 = efs_mgr.arr_static_info[i].efile_num_pg * EFS_FSH(prt).fnf.page_size;
		}		
	}

	
	//找到比usd_addr_1右侧的第一个使用区域
	usd_addr_2 = EFS_FSH(prt).fnf.page_size *  EFS_FSH(prt).fnf.total_pagenum;
	count = 0;
	fsp->start_addr = usd_addr_1 + use_size_1;
	tmp_addr = usd_addr_2;
	for(i = 0 ; i < EFS_MAX_NUM_FILES; i++)
	{
		
	
		if((efs_mgr.arr_static_info[i].efs_flag & EFILE_USED) == 0)
			continue;
		if(efs_mgr.arr_static_info[i].efs_flag & EFILE_RECYCLE)
			continue;
		if(efs_mgr.arr_static_info[i].efile_fsh_NO != prt)
			continue;
		
		file_start_addr = efs_mgr.arr_static_info[i].efile_start_pg * EFS_FSH(prt).fnf.page_size;
		if(file_start_addr >= fsp->start_addr)
		{
			if(tmp_addr > file_start_addr)
				tmp_addr = file_start_addr;
		}
		
	}
	usd_addr_2 = tmp_addr;
	
	//计算两个空间之间的空闲空间

	fsp->start_addr = usd_addr_1 + use_size_1;
	fsp->free_bytes = usd_addr_2 - fsp->start_addr;
	
	
	
	
		
	return 0;

}

static void EFS_Change_file_size(int fd, uint32_t new_size)
{
	
	efs_mgr.arr_static_info[fd].efile_num_pg = new_size / EFS_FSH(efs_mgr.arr_static_info[fd].efile_fsh_NO).fnf.page_size + 1;
	efs_mgr.arr_dynamic_info[fd].num_page = efs_mgr.arr_static_info[fd].efile_num_pg;
	efs_mgr.arr_dynamic_info[fd].file_size = EFS_FSH(efs_mgr.arr_static_info[fd].efile_fsh_NO).fnf.page_size * efs_mgr.arr_static_info[fd].efile_num_pg;
	
	return ;
	
}

//static void EFS_set_flag(uint8_t  num, uint8_t	flag, char val)
//{
//	int i = 0;
//	if(val)
//	{
//		for(i = 0; i < num; i++)
//		{
//			efs_mgr.arr_static_info[i].efs_flag |= flag;
//		}
//		
//	}
//	else
//	{
//		for(i = 0; i < num; i++)
//		{
//			efs_mgr.arr_static_info[i].efs_flag &= ~flag;
//		}
//		
//	}
//		
//	
//}
static int EFS_create_file(uint8_t	fd, uint8_t	prt, char *path, int size)
{
	space_t  space;
	int 		ret = 0;
	int			safe_count = 100;
	int			i = fd;
	
//	j = EFS_malloc_file_info();
//	if(j < 0)
//		return -1;
	
	
	if(i < 0)
		return -1;
	EFS_Regain_space();
	while(1)
	{
		
		ret = EFS_Cal_free_space(prt, &space);
		if(ret < 0)
			break;
		if(space.free_bytes >= size)
		{
			strncpy(efs_mgr.arr_static_info[i].efile_name, path, EFS_NAME_LEN);
			efs_mgr.arr_static_info[i].efile_fsh_NO = prt;
			efs_mgr.arr_static_info[i].efs_flag = 1;
			efs_mgr.arr_static_info[i].efile_start_pg = (space.start_addr + EFS_FSH(prt).fnf.page_size - 1)/ EFS_FSH(prt).fnf.page_size;
			efs_mgr.arr_static_info[i].efile_num_pg = (size + + EFS_FSH(prt).fnf.page_size - 1) / EFS_FSH(prt).fnf.page_size;
//			if(size > efs_mgr.arr_static_info[i].efile_num_pg * EFS_FSH(prt).fnf.page_size)
//				efs_mgr.arr_static_info[i].efile_num_pg += 1;
			
//			if(prt == 0)
//				efs_mgr.arr_dynamic_info[i].low_pg = EFS_LOWSPACE_ALARM_0;
//			else
//				efs_mgr.arr_dynamic_info[i].low_pg = EFS_LOWSPACE_ALARM_1;
			efs_mgr.arr_static_info[i].efile_wr_position = 0;
			efs_mgr.arr_static_info[i].efs_flag |= EFILE_USED;
			
			EFS_file_mgr_info(&efs_mgr.arr_static_info[i], &efs_mgr.arr_dynamic_info[i]);
//			efs_mgr.arr_dynamic_info[i].fs_mgr = i;
			
			EFS_flush_mgr(i);
			ret = i;
			break;
		}
		if(safe_count)
			safe_count --;
		else
		{
			ret = -1;
			break;
			
		}
	}
//	EFS_set_flag(EFS_MAX_NUM_FILES, EFS_FLAG_SEARCHED, 0);
	
	return ret;
	
}

//static int EFS_malloc_file_info(void)
//{
//	
//	int i;
//	
//	for(i = 0; i < EFS_MAX_NUM_FILES; i++)
//	{
//		if(efs_mgr.arr_dynamic_info[i].file_flag == 0)
//		{
//			efs_mgr.arr_dynamic_info[i].file_flag = 1;
//			return i;
//		}
//		
//		
//	}
//	
//	return -1;
//}

static int EFS_malloc_file_mgr(void)
{
	
	int i;
	
	for(i = 0; i < EFS_MAX_NUM_FILES; i++)
	{
		if(efs_mgr.arr_static_info[i].efs_flag & EFILE_ALLOCED)
			continue;
		
		efs_mgr.arr_static_info[i].efs_flag |= EFILE_ALLOCED;
		return i;
		
		
		
	}
	
	return -1;
}

static void EFS_file_mgr_info(efs_file_mgt_t	*file_mgr, file_info_t *file_info)
{
	file_info->fsh_No = file_mgr->efile_fsh_NO;
	file_info->p_name = file_mgr->efile_name;
	file_info->start_page = file_mgr->efile_start_pg;
	file_info->num_page = file_mgr->efile_num_pg;
	file_info->write_position = file_mgr->efile_wr_position;
	
	file_info->file_size = file_info->num_page * EFS_FSH(file_info->fsh_No).fnf.page_size;
}

