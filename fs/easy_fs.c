//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//
/*

V010 171226 :
֧�֣�
֧�ֶ��������ÿ��������Ӧһ��flash

��֧�֣�

��֧���ļ��ķ�ɢ�洢�����е��ļ����������ġ�
��֧�ֶ�̬��չ,�ļ�һ�����������Ⱦ͹̶��ˡ�
��֧��һ���洢���϶��������
���ļ��Ķ�дȨ�޲����������������ļ��Ķ�д����Ȩ���ж�
*/

#include "fs/easy_fs.h"
#include <string.h>
#include "sdhDef.h"
#include "mem/CiiMem.h"

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define FS_RLB_LEVEL					1	
#define	EFS_MGR_FSH_NO					0
#define	EFS_MGR_NUM_FSH					2
#define EFS_NAME_LEN					12
#define EFS_MAX_NUM_FILES				7
#define EFS_NUM_IDLE_FILES				0		//��������


#define EFS_SYS								phn_sys
#define EFS_FS								phn_sys.fs
#define EFS_FSH(NO)							phn_sys.arr_fsh[NO]
#define EFS_LOWSPACE_ALARM_0				0
#define EFS_LOWSPACE_ALARM_1				64


#define EFS_FLAG_ALLOCED					1
#define EFS_FLAG_USED							2
//#define EFS_FLAG_SEARCHED					4		//������һ�η���flashʱ������Ѿ������ҹ����ļ�����
#define EFS_FLAG_RECYCLE					8
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
//	uint16_t	efile_space_low_bytes;		//�ļ�����С�����ֵ��ʱ�򣬱���
}efs_file_mgt_t;

typedef struct {
	uint32_t		start_addr;
	uint32_t		free_bytes;
	
	
}space_t;
	
typedef struct {
	uint8_t					free_spac_num;
	uint8_t					none[3];
	uint8_t					*pg_buf;
	efs_file_mgt_t  arr_efiles[EFS_MAX_NUM_FILES + EFS_NUM_IDLE_FILES];			//�ļ�����Ҫ��һ��ռ�
	file_info_t			arr_file_info[EFS_MAX_NUM_FILES];
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
int		EFS_read(int fd, uint8_t *p, int len);
int		EFS_resize(int fd, char *path, int new_size);
file_info_t		*EFS_file_info(int fd);
int		EFS_delete(int fd);


static int EFS_format(void);
static int EFS_search_file(char *path);
static int EFS_create_file(uint8_t	fd, uint8_t	prt, char *path, int size);


static void EFS_file_mgr_info(efs_file_mgt_t	*file_mgr, file_info_t *file_info);
//static int EFS_malloc_file_info(void);
static int EFS_malloc_file_mgr(void);
static void	EFS_flush_mgr(int No);
static void EFS_Change_file_size(int fd, uint32_t new_size);
static void	EFS_Regain_space(void);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
int 	EFS_init(int arg)
{
	short	i, pg_size = 0;
	EFS_FS.num_partitions = arg;
	EFS_FS.reliable_level = FS_RLB_LEVEL;
	if(EFS_FS.reliable_level == 1)
	{
		for(i = 0; i < arg; i ++)
			EFS_FSH(i).fnf.fnf_flag |= FSH_FLAG_READBACK_CHECK;
		
	}
	
	for(i = 0; i < arg; i ++)
	{
		if(EFS_FSH(i).fnf.page_size > pg_size)
			pg_size = EFS_FSH(i).fnf.page_size;
		
	}
	efs_mgr.pg_buf = ALLOC(pg_size);
	
	
	EFS_FS.fs_open = EFS_open;
	EFS_FS.fs_close = EFS_close;
	
	EFS_FS.fs_read = EFS_read;
	EFS_FS.fs_write = EFS_write;
	EFS_FS.fs_delete = EFS_delete;
	EFS_FS.fs_resize = EFS_resize;
	EFS_FS.fs_file_info = EFS_file_info;
	
	return EFS_format();
	
	
}



int	EFS_open(uint8_t prt, char *path, char *mode, int	file_size)
{
	int new_fd = 0;
	
	//�ȿ�����û���Ѿ�����
	new_fd = EFS_search_file(path);
	
	
	//�����µ��ļ�
	if(new_fd < 0)
	{
		new_fd = EFS_malloc_file_mgr();
		new_fd = EFS_create_file(new_fd, prt, path, file_size);
	}
	if(new_fd >= 0)
		efs_mgr.arr_file_info[new_fd].read_position = 0;
	
	return new_fd;
}
int	EFS_close(int fd)
{
	file_info_t *f = &efs_mgr.arr_file_info[fd];
	efs_file_mgt_t	*f_mgr = &efs_mgr.arr_efiles[fd];
	//����дλ��
	if(f_mgr->efile_wr_position != f->write_position)
	{
		
		f_mgr->efile_wr_position = f->write_position;
		EFS_flush_mgr(fd);
	}

	return 0;
}
int	EFS_delete(int fd)
{
	return 0;
}
int	EFS_write(int fd, uint8_t *p, int len)
{
	int   ret;
	file_info_t *f = &efs_mgr.arr_file_info[fd];
	int			start_addr = f->start_page * EFS_FSH(f->fsh_No).fnf.page_size;

	if(len > f->num_page * EFS_FSH(f->fsh_No).fnf.page_size)
		len = f->num_page * EFS_FSH(f->fsh_No).fnf.page_size;
	
	
	
	ret =  EFS_FSH(f->fsh_No).fsh_write(p, start_addr + f->write_position,len);
	if(ret > 0)
		f->write_position += ret;
	
	if((f->num_page - f->write_position / EFS_FSH(f->fsh_No).fnf.page_size) < f->low_pg)
		EFS_FS.err_code |= FS_ALARM_LOWSPACE;
	else
		EFS_FS.err_code &= ~FS_ALARM_LOWSPACE;
	
	return ret;
	
}
int	EFS_read(int fd, uint8_t *p, int len)
{
	int 		ret;
	file_info_t *f = &efs_mgr.arr_file_info[fd];
	int			start_addr = f->start_page * EFS_FSH(f->fsh_No).fnf.page_size;

	if(len > f->num_page * EFS_FSH(f->fsh_No).fnf.page_size)
		len = f->num_page * EFS_FSH(f->fsh_No).fnf.page_size;
	
	ret =  EFS_FSH(f->fsh_No).fsh_read(p, start_addr + f->read_position,len);
	if(ret > 0)
		f->read_position += ret;
	
	return ret;
}

//��fd����path��ָ���ļ�
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
	
	f = &efs_mgr.arr_file_info[fd];
	pg_size = EFS_FSH(f->fsh_No).fnf.page_size;
	start_addr = f->start_page * EFS_FSH(f->fsh_No).fnf.page_size;

	//�Ա�ԭ��С�����жϿռ����Ǳ�С
	old_size = f->file_size;
	
	//��С��ֱ�ӽ�ԭ��С��С����
	if(new_size <= old_size)
	{
		EFS_Change_file_size(fd, new_size);
		return  fd;
		
	}
	//���Ҫ���´������ļ�
		//���ԭ����Ϊ������
	efs_mgr.arr_efiles[fd].efs_flag |= EFS_FLAG_RECYCLE;
	EFS_file_mgr_info(&efs_mgr.arr_efiles[fd], &tmp_file_info);
	tmp_file_info.write_position = f->write_position;
	tmp_file_info.read_position = f->read_position;
	
		//���´������ļ�
	ret = EFS_create_file(fd, f->fsh_No, efs_mgr.arr_efiles[fd].efile_name, new_size);
	
	if(ret == fd)
			//�ҵ��˾Ϳ������µĿռ���ȥ
	{
		
		//��ԭ���Ķ�дλ�ø��Ƹ��µ��ļ���
		new_start_addr = efs_mgr.arr_efiles[fd].efile_start_pg * pg_size;
		end_pg = tmp_file_info.write_position / pg_size + 1;		 //ֻ�����Ѿ���д��Ĳ���
		for(i = 0; i < end_pg; i ++)
		{
			ret = EFS_FSH(f->fsh_No).fsh_read(efs_mgr.pg_buf, start_addr + pg_size * i, pg_size);
			if(ret != pg_size)
			{
				
				goto ERR_RECOVER;
			}
			ret = EFS_FSH(f->fsh_No).fsh_write(efs_mgr.pg_buf, new_start_addr + pg_size * i, pg_size);
			
		}
		
		
		efs_mgr.arr_efiles[fd].efile_wr_position = tmp_file_info.write_position;
		EFS_flush_mgr(fd);
		f->write_position = tmp_file_info.write_position;
		f->read_position = tmp_file_info.read_position;
		
		efs_mgr.arr_efiles[fd].efs_flag &= ~EFS_FLAG_RECYCLE;
		return fd;
		
	}
			//�Ҳ�����ȥ�������ձ�־�����ش���
	
	
	else
	{
		ERR_RECOVER:	
		efs_mgr.arr_efiles[fd].efs_flag &= ~EFS_FLAG_RECYCLE;
		return -1;
	}
}
file_info_t		*EFS_file_info(int fd)
{
	return efs_mgr.arr_file_info + fd;
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
	
	uint8_t		ver[2];
	uint8_t		i = 0;
	
	EFS_FSH(EFS_MGR_FSH_NO).fsh_read(ver, 0, 2);
	
	if(ver[0] != EFS_SYS.major_ver || ver[1] != EFS_SYS.minor_ver)
	{
//		for(i = 0; i < EFS_MGR_NUM_FSH; i++)
		EFS_FSH(i).fsh_ersse(FSH_OPT_CHIP, 0);
		ver[0] = EFS_SYS.major_ver ;
		ver[1] = EFS_SYS.minor_ver;
		EFS_FSH(EFS_MGR_FSH_NO).fsh_write(ver, 0, 2);
		memset((uint8_t *)efs_mgr.arr_efiles, 0, sizeof(efs_mgr));
		EFS_FSH(EFS_MGR_FSH_NO).fsh_write((uint8_t *)efs_mgr.arr_efiles, 2, sizeof(efs_mgr));
	}
	
	
	EFS_FSH(EFS_MGR_FSH_NO).fsh_read((uint8_t *)efs_mgr.arr_efiles, 2, sizeof(efs_mgr));
//	memset(efs_mgr.arr_file_info, 0 ,sizeof(efs_mgr.arr_file_info));
	return RET_OK;
}

static void	EFS_flush_mgr(int No)
{
	efs_file_mgt_t	*f_mgr = &efs_mgr.arr_efiles[No];
	
	
		
	EFS_FSH(EFS_MGR_FSH_NO).fsh_write((uint8_t *)f_mgr, No * sizeof(efs_file_mgt_t) + 2,  sizeof(efs_file_mgt_t));
	

}

static int EFS_search_file(char *path)
{
	int i;
	for(i = 0; i < EFS_MAX_NUM_FILES; i++)
	{
		if(strcmp(efs_mgr.arr_efiles[i].efile_name, path) == 0)
		{
			
//			j = EFS_malloc_file_info();
//			if(j < 0)
//				return -1;
			EFS_file_mgr_info(&efs_mgr.arr_efiles[i], &efs_mgr.arr_file_info[i]);
//			efs_mgr.arr_file_info[i].fs_mgr = i;
			return i;
			
		}
		
	}
	
	return -1;
	
}

static void EFS_Regain_space(void)
{
	
	efs_mgr.free_spac_num = 0;
}
 
static int EFS_Cal_free_space(uint8_t prt, space_t *fsp)
{
	
	short i;
	uint32_t	usd_addr_1 = 0, usd_addr_2 = 0, use_size = 0;
	short				fsp_num = efs_mgr.free_spac_num ++;		//��õ�n�����пռ�
	short				count = 0;
	
	
	
	//�ҵ���һ��ʹ�õĿռ�
	usd_addr_1 = 0;
	use_size = 0;
	if(prt == EFS_MGR_FSH_NO)
	{
		use_size = sizeof(efs_mgr.arr_efiles) + EFS_FSH(prt).fnf.page_size;
		usd_addr_1 = 0;
		
	}
	count = 0;
	
	for(i = 0; i < EFS_MAX_NUM_FILES; i++)
	{
		if(count == fsp_num)
			break;
		
		if((efs_mgr.arr_efiles[i].efs_flag & EFS_FLAG_USED) == 0)
			continue;
		if(efs_mgr.arr_efiles[i].efs_flag & EFS_FLAG_RECYCLE)
			continue;
		
		if(efs_mgr.arr_efiles[i].efile_fsh_NO != prt)
			continue;
		
//		if(efs_mgr.arr_efiles[i].efs_flag & EFS_FLAG_SEARCHED)
//		{
//			
//			continue;
//		}
		
		count ++;
		
		if(count == fsp_num)
		{
			usd_addr_1 = efs_mgr.arr_efiles[i].efile_start_pg * EFS_FSH(prt).fnf.page_size;;
			use_size += efs_mgr.arr_efiles[i].efile_num_pg * EFS_FSH(prt).fnf.page_size;
		}
//		efs_mgr.arr_efiles[i].efs_flag |= EFS_FLAG_SEARCHED;
		
	}
	if(i == EFS_MAX_NUM_FILES)
	{
		
		//ָ����ŵĿ��пռ��Ѿ��Ҳ�����
		return -1;
	}
	
	//�ҵ���һ��ʹ�õ�
	usd_addr_2 = EFS_FSH(prt).fnf.page_size *  EFS_FSH(prt).fnf.total_pagenum;
	count = 0;
	for(; i < EFS_MAX_NUM_FILES; i++)
	{
		if(count == (fsp_num + 1))
			break;
	
		if((efs_mgr.arr_efiles[i].efs_flag & EFS_FLAG_USED) == 0)
			continue;
		if(efs_mgr.arr_efiles[i].efs_flag & EFS_FLAG_RECYCLE)
			continue;
		if(efs_mgr.arr_efiles[i].efile_fsh_NO != prt)
			continue;
		
//		if(efs_mgr.arr_efiles[i].efs_flag & EFS_FLAG_SEARCHED)
//			continue;
		
		count ++;
		if(count == (fsp_num + 1))
		{
			usd_addr_2 = efs_mgr.arr_efiles[i].efile_start_pg * EFS_FSH(prt).fnf.page_size;
//			efs_mgr.arr_efiles[i].efs_flag |= EFS_FLAG_SEARCHED;
		}
	}

	
	//���������ռ�֮��Ŀ��пռ�
	fsp->start_addr = usd_addr_1 + use_size;
	fsp->free_bytes = usd_addr_2 - fsp->start_addr;
	
	
		
	return 0;

}

static void EFS_Change_file_size(int fd, uint32_t new_size)
{
	
	efs_mgr.arr_efiles[fd].efile_num_pg = new_size / EFS_FSH(efs_mgr.arr_efiles[fd].efile_fsh_NO).fnf.page_size + 1;
	efs_mgr.arr_file_info[fd].num_page = efs_mgr.arr_efiles[fd].efile_num_pg;
	efs_mgr.arr_file_info[fd].file_size = EFS_FSH(efs_mgr.arr_efiles[fd].efile_fsh_NO).fnf.page_size * efs_mgr.arr_efiles[fd].efile_num_pg;
	
	return ;
	
}

static void EFS_set_flag(uint8_t  num, uint8_t	flag, char val)
{
	int i = 0;
	if(val)
	{
		for(i = 0; i < num; i++)
		{
			efs_mgr.arr_efiles[i].efs_flag |= flag;
		}
		
	}
	else
	{
		for(i = 0; i < num; i++)
		{
			efs_mgr.arr_efiles[i].efs_flag &= ~flag;
		}
		
	}
		
	
}
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
			strncpy(efs_mgr.arr_efiles[i].efile_name, path, EFS_NAME_LEN);
			efs_mgr.arr_efiles[i].efile_fsh_NO = prt;
			efs_mgr.arr_efiles[i].efs_flag = 1;
			efs_mgr.arr_efiles[i].efile_start_pg = space.start_addr / EFS_FSH(prt).fnf.page_size;
			efs_mgr.arr_efiles[i].efile_num_pg = size / EFS_FSH(prt).fnf.page_size + 1;
			if(prt == 0)
				efs_mgr.arr_file_info[i].low_pg = EFS_LOWSPACE_ALARM_0;
			else
				efs_mgr.arr_file_info[i].low_pg = EFS_LOWSPACE_ALARM_1;
			efs_mgr.arr_efiles[i].efile_wr_position = 0;
			efs_mgr.arr_efiles[i].efs_flag |= EFS_FLAG_USED;
			
			EFS_file_mgr_info(&efs_mgr.arr_efiles[i], &efs_mgr.arr_file_info[i]);
//			efs_mgr.arr_file_info[i].fs_mgr = i;
			
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
//		if(efs_mgr.arr_file_info[i].file_flag == 0)
//		{
//			efs_mgr.arr_file_info[i].file_flag = 1;
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
		if(efs_mgr.arr_efiles[i].efs_flag & EFS_FLAG_ALLOCED)
			continue;
		
		efs_mgr.arr_efiles[i].efs_flag |= EFS_FLAG_ALLOCED;
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
