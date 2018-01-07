//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#ifndef __INC_system_H_
#define __INC_system_H_
#include <stdint.h>
#include "utils/time.h"


//------------------------------------------------------------------------------
// check for correct compilation options
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//系统配置类宏定义
#define CONF_KEYSCAN_POLL		1		//按键扫描：轮询方式,该值为0，则为中断触发式
#define	CONF_KEYSCAN_CYCLEMS	100
#define NUM_CHANNEL			6
#define CURVE_POINT			240			//曲线点数最多240点,但是数据的长度要加上1个起始点


#define OP_ADD				0
#define OP_SUB				1

#define FSH_FM25_NUM			0
#define FSH_W25Q_NUM			1
#define FSH_OPT_SECTOR			0
#define FSH_OPT_BLOCK			1
#define FSH_OPT_CHIP			2
#define NUM_FSH					2
#define FSH_FLAG_READBACK_CHECK		2			//

#define FS_ALARM_LOWSPACE		1

#define	CHG_SYSTEM_CONF				1	
#define	CHG_MODCHN_CONF(n)			(1 << (n + 1))


//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------
typedef enum {
	es_psd = 0,
	es_rcd_t_s,
	es_brk_cpl,
	es_brk_rss,
	es_cmn_md,
	es_baud,
	es_id,
	es_mdfy_prm,
	es_CJC,
	es_vcs,
	es_beep,
}e_system_t;

typedef struct {
	uint8_t		num_chn;
	uint8_t		password[3];
	
	uint16_t	record_gap_s;
	uint8_t		break_couple;		//断偶处理方式: 始点，保持，终点
	uint8_t		break_resistor;		//断阻处理
	
	uint8_t		communication_mode;			//仪表与pc连接： 通讯； 仪表与打印机连接: 打印
	uint8_t		id;											// 1 - 63
	uint8_t		baud_idx;
	uint8_t		sys_flag;
	int 		baud_rate;
	
	uint8_t		CJC;								//冷端补偿 0-99 为设定模式， 100为外部，通过冷端补偿器温度进行补偿
	uint8_t		disable_modify_adjust_paramter;		//禁止修改调节参数
	uint8_t		disable_view_chn_status;					//禁止通道状态显示
	uint8_t		enable_beep;											//按键声音允许
}system_conf_t;

//---------- flash驱动的定义 --------------------------------------

typedef struct {
	uint16_t		num_sct;
	uint16_t		num_blk;
	uint32_t		total_pagenum;					///整个存储器的页数量
	
	uint16_t		page_size;						///一页的长度
	
	
	uint8_t			fnf_flag;
	uint8_t			none;
}fsh_info_t;

typedef struct {
	
	fsh_info_t	fnf;
	
//	int (*fsh_init)(void);
	void (*fsh_wp)(int p);
	void (*fsh_info)(fsh_info_t *nf);


	int	(*fsh_ersse)(int opt, uint32_t	num);
//	int	(*fsh_wr_sector)(uint8_t *wr_buf, uint16_t num_sector);
//	int	(*fsh_rd_sector)(uint8_t *rd_buf, uint16_t num_sector);
	int (*fsh_write)(uint8_t *wr_buf, uint32_t wr_addr, uint32_t num_bytes);
	int (*fsh_read)(uint8_t *wr_buf, uint32_t rd_addr, uint32_t num_bytes);
	
	void (*fsh_flush)(void);
}flash_t;

//----------------文件系统的定义 --------------------------------

typedef enum {
	WR_SEEK_SET = 0,
	WR_SEEK_CUR = 1,
	WR_SEEK_END = 2,
	RD_SEEK_SET = 3,
	RD_SEEK_CUR = 4,
	RD_SEEK_END = 5,
	GET_WR_END = 6,
	GET_RD_END = 7,
}lseek_whence_t;
typedef struct {
	uint8_t			fsh_No;				//对应的存储器编号
	uint8_t			opt_mode;			//0  只读  1 读写
	uint8_t			file_flag;
	uint8_t			low_pg;
	
	uint16_t		start_page;
	uint16_t		num_page;
	
	uint32_t		file_size;
	uint32_t		read_position;
	uint32_t		write_position;

	char			*p_name;
}file_info_t;

typedef struct {
	uint8_t		num_partitions;
	
	//可靠性等级, 0 一般，在写文件时不回读判断 1 高，写文件时，要回读判断
	uint8_t		reliable_level;		
	uint16_t	err_code;
	
	//file_size在文件不存在时，需要创建时使用
	int		(*fs_open)(uint8_t		prt, char *path, char *mode, int	file_size);	
	int		(*fs_close)(int fd);
	int		(*fs_delete)(int fd);
	int		(*fs_write)(int fd, uint8_t *p, int len);
	int		(*fs_read)(int fd, uint8_t *p, int len);
	int		(*fs_resize)(int fd, char *name, int new_size);
	int 	(*fs_lseek)(int fd, int whence, uint32_t offset);
	void 	(*fs_shutdown)(void);
	file_info_t*		(*fs_file_info)(int fd);
			
	
}fs_t;

//--------------------------------------------------------------------------

typedef struct {
	uint8_t				major_ver;
	uint8_t				minor_ver;
	uint8_t				save_chg_flga;		//可存储的配置信息的变化标志
	uint8_t				none;
	system_conf_t		sys_conf;
	flash_t				arr_fsh[NUM_FSH];
	fs_t				fs;
}system_t;
	

//------------------------------------------------------------------------------
// global variable declarations
//------------------------------------------------------------------------------
//extern 	system_conf_t		phn_sys.sys_conf;
extern	char 				*arr_p_vram[16];
extern 	int 				g_set_weight;

extern	short 			cmd_count;
extern	uint16_t		next_record;			//下次记录的时间

extern 	char				g_setting_chn;
extern	char				flush_flag;
extern 	char				g_system_none[2];

extern system_t		phn_sys;
//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
extern void Str_Calculations(char *p_str, int len,  int op, int val, int rangel, int rangeh);
extern int	Operate_in_tange(int	arg1, int op, int arg2, int rangel, int rangeh);

extern void System_init(void);
extern void System_time(struct  tm *stime);
extern uint32_t System_tm_2_u32(struct  tm *stime);
extern int System_u32_2_tm(uint32_t time_u32, struct  tm *stime);
extern int System_set_time(struct  tm *stime);
extern void System_default(void);
void System_modify_string(char	*p_s, int aux, int op, int val);
void System_to_string(void *p_data, char	*p_s, int len, int aux);
void Password_set_by_str(char	*p_s_psd);

int Str_Password_match(char *p_s_psd);
void Password_modify(char	*p_s_psd, int idx, int op);
int Password_iteartor(char	*p_time_text, int idx, int director);
int Get_str_data(char *s_data, char* separator, int num, uint8_t	*err);
#endif
