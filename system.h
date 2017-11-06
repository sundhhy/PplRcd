//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#ifndef __INC_system_H_
#define __INC_system_H_
#include <stdint.h>
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
#define CURVE_POINT			240


#define OP_ADD				0
#define OP_SUB				1
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
	uint8_t		none;
	int 			baud_rate;
	
	uint8_t		CJC;								//冷端补偿 0-99 为设定模式， 100为外部，通过冷端补偿器温度进行补偿
	uint8_t		disable_modify_adjust_paramter;		//禁止修改调节参数
	uint8_t		disable_view_chn_status;					//禁止通道状态显示
	uint8_t		enable_beep;											//按键声音允许
}system_conf_t;
	
//------------------------------------------------------------------------------
// global variable declarations
//------------------------------------------------------------------------------
extern 	system_conf_t		g_system;
extern	char 				*arr_p_vram[16];
extern 	int 				g_set_weight;
extern 	char				g_setting_chn;
//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
extern void Str_Calculations(char *p_str, int len,  int op, int val, int rangel, int rangeh);
extern int	Operate_in_tange(int	arg1, int op, int arg2, int rangel, int rangeh);

extern void System_init(void);
extern void Sys_default(system_conf_t *arg);
void System_modify_string(char	*p_s, int aux, int op, int val);
void System_to_string(void *p_data, char	*p_s, int len, int aux);
void Password_set_by_str(char	*p_s_psd);

int Str_Password_match(char *p_s_psd);
void Password_modify(char	*p_s_psd, int idx, int op);
int Password_iteartor(char	*p_time_text, int idx, int director);
#endif
