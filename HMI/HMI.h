#ifndef _INC_HMI_H_
#define _INC_HMI_H_

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#include <stdint.h>
#include <string.h>
#include "lw_oopc.h"

#include "sdhDef.h"
#ifdef NO_ASSERT
#include "basis/assert.h"
#else
#include "assert.h"
#endif

#include "sheet.h"
#include "focus.h"

#include "Component_Button.h"
#include "Component_curve.h"
#include "Component_progress_bar.h"
#include "Component_tips.h"
#include "utils/keyboard.h"
#include "os/os_depend.h"
//------------------------------------------------------------------------------
// check for correct compilation options
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//显示用的方向
#define		HMI_DIR_UP					0
#define		HMI_DIR_DOWN				1
#define		HMI_DIR_LEFT				2
#define		HMI_DIR_RIGHT				3

#define		HMI_CMP_ALL					0xff		//



#define TIP_ICO_USB						0
#define TIP_ICO_CLEAR_FILE		1
#define TIP_ICO_WARING				2
#define TIP_ICO_ERR						3

/********图层的ID	***********************************/
#define	ICO_ID_MENU					0x20	//主菜单
#define	ICO_ID_BAR					0x21	//棒图
#define	ICO_ID_DIGITAL				0x22	//数显
#define	ICO_ID_TREND				0x23	//趋势
#define	ICO_ID_PGUP					0x24	//向上翻页
#define	ICO_ID_PGDN					0x25	//向下翻页
#define	ICO_ID_ERASETOOL			0x26	//擦除工具
#define	ICO_ID_SAVE					0x27	//保存
#define	ICO_ID_COPY					0x28	//复制
#define	ICO_ID_STOP					0x29	//停止
#define	ICO_ID_LOOP					0x2a	//循环显示


#define	SHEET_BOXLIST				0x30
#define	SHEET_G_TEXT				0x31
#define	SHEET_PSD_TEXT			0x32

#define IS_CHECK(n)				((n&0xf0) == 0x30)
#define SHTID_CHECK(n)			(0x30 + n)

#define	GET_CHN_FROM_ID(id)		(id & 0x0f)


#define SHTID_RTL_MDIV				0x40
#define SHT_FIRST_TIME				0x41

#define SHT_BTN_ID(n)			(0x30 | n)

/***************************************************************/


#define	HMI_FLAG_HIDE			1
#define	HMI_FLAG_HSA_SEM			2		//本界面已经获得了信号量
#define	HMI_FLAG_DEAL_HIT		3	
//#define	HMI_FLAG_KEEP			4			//有些画面切换时，要求画面不要重新初始化状态
#define	HMI_FLAG_ERR			0x10

//窗口界面来负责对该位的操作
//表示界面接下来要跟窗口进行交互，因此在窗口切换回界面的时候，会处理一些额外的交互信息
//故每个需要与窗口交互的界面，在其show方法中，都应该有与窗口界面交互的处理 
//#define	HMIFLAG_WIN				0x20		
//#define	HMIFLAG_KEYBOARD		0x40
//界面当前的焦点位于按钮区间
#define	HMIFLAG_FOCUS_IN_BTN	0x80


#define HMI_ATT_KEEP			1
#define HMI_ATT_SELF_ERR			2		
#define HMI_ATT_NOT_RECORD			4		//窗口或者按键界面返回的时候，不用把它们存到历史画面列表中

#define IS_HMI_HIDE(flag)	(flag & HMI_FLAG_HIDE)
//#define IS_HMI_KEYHANDLE(flag)	((flag&2))


#define SY_KEYTYPE_HIT				0
#define SY_KEYTYPE_DHIT				1
#define SY_KEYTYPE_LONGPUSH			2

//策略类

 //------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------

typedef int	(*keyboard_commit)(void *self, void *data, int len);


typedef struct {
	uint8_t		rowGrap;			//行间距
	uint8_t		colGrap;			//列间距
	uint8_t		bkc;
	uint8_t		numRow;
	uint8_t		numCol;
	uint8_t		none[3];
}hmiAtt_t;

typedef struct {
	uint8_t		f_row, f_col;
	uint8_t		start_byte, num_byte;
}strategy_focus_t;



typedef struct {
	uint8_t		key_type;
	uint8_t		none[3];
}strategy_keyval_t;

typedef int (*stategy_cmd)(void *p_rcv, int cmd, void *arg);

typedef enum {
	sycmd_reflush = 0,
	sycmd_reflush_position,  //指定位置的刷新
	sycmd_win_tips,
	sycmd_win_time,
	sycmd_win_psd,
	sycmd_keyboard,
	wincmd_commit,
}e_sycmd_r;

typedef struct {
	
	//将pp_data指定的行号和列号的显示字符串指针，并返回字符串的长度
	//长度为0时表示没有数据
	//所有的显示应该是对齐的，不考虑出现空洞的情况
	int (*entry_txt)(int row, int col,void *pp_text);	
	int	(*init)(void	*arg);
	void (*build_component)(void *arg);
	int	(*key_hit_up)(void	*arg);
	int	(*key_hit_dn)(void	*arg);
	int	(*key_hit_lt)(void	*arg);
	int	(*key_hit_rt)(void	*arg);
	int	(*key_hit_er)(void	*arg);
	int	(*get_focus_data)(void *pp_data,  strategy_focus_t *p_in_syf);		//成功返回大于0，失败返回-1
	int	(*commit)(void	*arg);
	void	(*sty_exit)(void);
	strategy_focus_t	sf;
	void				*p_cmd_rcv;
	stategy_cmd			cmd_hdl;
	uint8_t					total_row;
	uint8_t					total_col;
	uint8_t					none[2];
}strategy_t;


ABS_CLASS(HMI)
{
//	HMI*			prev;
//	HMI*			next;
	
	focus_user_t	*p_fcuu;
	uint8_t			flag;
	uint8_t			arg[3];		//切换屏幕时，用于屏幕间传递一些参数
	
	
	//显示静态画面
	int		(*init)( HMI *self, void *arg);
	void	(*show)( HMI *self);
	
	
	//数据相关
//	void	(*dataVisual)(HMI *self, void *arg);		//17-09-20 数据可视化处理
	
	// initSheet hide 
	void		(*hide)( HMI *self);
	void		(*initSheet)( HMI *self, uint32_t	att_flag );
	void		(*switchHMI)( HMI *self, HMI *p_hmi, uint32_t	att_flag);
	void		(*switchBack)( HMI *self, uint32_t	att_flag);
	
	//按键动作
	void		(*hitHandle)( HMI *self, char kcd);
	void		(*dhitHandle)( HMI *self, char kcd);
	void		(*longpushHandle)( HMI *self,  char kcd);
	void		(*conposeKeyHandle)( HMI *self, char kcd_1, char kcd_2);
	//焦点处理
	void		(*init_focus)(HMI *self);
	void		(*clear_focus)(HMI *self, uint8_t fouse_row, uint8_t fouse_col);
	void		(*show_focus)( HMI *self, uint8_t fouse_row, uint8_t fouse_col);
	
	
		//屏幕上的按钮
	void		(*build_component)(HMI *self);
	void		(*clean_cmp)(HMI *self);
	void		(*show_cmp)(HMI *self);
	int		(*btn_forward)(HMI *self);
	int		(*btn_backward)(HMI *self);
	void		(*btn_jumpout)(HMI *self);
	
	
	//180202 定期执行，用于周期性重刷屏幕或者其他的内部数据的刷新
	void		(*hmi_run)(HMI *self);			
//	void		(*btn_hit)(HMI *self);
	
};

typedef   const char  ro_char;


//------------------------------------------------------------------------------
// global variable declarations
//------------------------------------------------------------------------------
extern keyboard_commit	kbr_cmt;

extern const Except_T Hmi_Failed;
//extern  ro_char str_endRow[];
//extern	ro_char str_endCol[];
extern HMI* g_p_curHmi;
//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
extern void Set_flag_show(uint8_t	*p_flag, int val);
//extern void Set_flag_keyhandle(uint8_t	*p_flag, int val);
void STY_Duild_button(void *arg);


//有时候本界面被切换之后，希望把ESC返回时的界面变成另外一个界面时调用
//要在本界面切换到其他界面之前调用
void HMI_Change_last_HMI(HMI *p);			


//很多界面的处理与主界面的按键处理一样，所以就把主界面的处理开放出来
void Main_btn_hdl(void *arg, uint8_t btn_id);
void Main_HMI_build_button(HMI *self);
void Main_HMI_hit( HMI *self, char kcd);

int HMI_Init(void);


void	HMI_Attach_model_chn(int *fds, mdl_observer *mdl_obs);
void	HMI_detach_model_chn(int *fds);

void HMI_TIP_ICO(uint8_t	type, char ctl);			//控制提示图标
void HMI_Updata_tip_ico(void);
#endif
