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
#include "Component_progress_bar.h"
//------------------------------------------------------------------------------
// check for correct compilation options
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define HMIKEY_UP				"up"
#define HMIKEY_DOWN			"down"
#define HMIKEY_LEFT			"left"
#define HMIKEY_RIGHT		"right"
#define HMIKEY_ENTER		"enter"
#define HMIKEY_ESC			"esc"

#define HMI_KEYCODE_UP		0
#define HMI_KEYCODE_DN		1
#define HMI_KEYCODE_LT		2
#define HMI_KEYCODE_RT		3
#define HMI_KEYCODE_ER		4
#define HMI_KEYCODE_ESC		5


/********图层的ID	***********************************/
#define	ICO_ID_MENU					0x20
#define	ICO_ID_BAR					0x21
#define	ICO_ID_DIGITAL			0x22
#define	ICO_ID_TREND				0x23
#define	ICO_ID_PGUP					0x24
#define	ICO_ID_PGDN					0x25
#define	ICO_ID_ERASETOOL		0x26
#define	ICO_ID_SEARCH				0x27
#define	ICO_ID_COPY					0x28
#define	ICO_ID_STOP					0x29
#define	ICO_ID_LOOP					0x2a

#define	SHEET_BOXLIST				0x30
#define	SHEET_G_TEXT				0x31
#define	SHEET_PSD_TEXT			0x32

#define IS_CHECK(n)				((n&0xf0) == 0x30)
#define SHTID_CHECK(n)			(0x30 + n)

#define	GET_CHN_FROM_ID(id)		(id & 0x0f)


#define SHTID_RTL_MDIV				0x40

#define SHT_BTN_ID(n)			(0x30 | n)

/***************************************************************/


#define	HMI_FLAG_HIDE	0
#define	HMI_FLAG_SHOW	1

#define SY_KEYTYPE_HIT				0
#define SY_KEYTYPE_DHIT				1
#define SY_KEYTYPE_LONGPUSH			2

//窗口界面来负责对该位的操作
//表示界面接下来要跟窗口进行交互，因此在窗口切换回界面的时候，会处理一些额外的交互信息
//故每个需要与窗口交互的界面，在其show方法中，都应该有与窗口界面交互的处理 
#define	HMIFLAG_WIN									0x10		
#define	HMIFLAG_KEYBOARD						0x20
//界面当前的焦点位于按钮区间
#define	HMIFLAG_FOCUS_IN_BTN				0x40
#define IS_HMI_HIDE(flag)	((flag&1) == 0)
#define IS_HMI_KEYHANDLE(flag)	((flag&2))



//策略类

 //------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------
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
	void	(*exit)(void);
	strategy_focus_t	sf;
	void				*p_cmd_rcv;
	stategy_cmd			cmd_hdl;
	uint8_t				sty_id;
	uint8_t				sty_some_fd;
	uint8_t				none[2];
}strategy_t;


ABS_CLASS(HMI)
{
	HMI*			prev;
	HMI*			next;
	
	focus_user_t	*p_fcuu;
	uint8_t			flag;
	uint8_t			arg[3];		//切换屏幕时，用于屏幕间传递一些参数
	
	
	//显示静态画面
	int		(*init)( HMI *self, void *arg);
	void	(*show)( HMI *self);
	
	
	//数据相关
	void	(*dataVisual)(HMI *self, void *arg);		//17-09-20 数据可视化处理
	
	// initSheet hide 
	void		(*hide)( HMI *self);
	void		(*initSheet)( HMI *self);
	void		(*switchHMI)( HMI *self, HMI *p_hmi);
	void		(*switchBack)( HMI *self);
	
	//按键动作
	void		(*hitHandle)( HMI *self, char *s_key);
	void		(*dhitHandle)( HMI *self, char *s_key);
	void		(*longpushHandle)( HMI *self,  char *s_key);
	void		(*conposeKeyHandle)( HMI *self, char *s_key1, char *s_key2);
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
//	void		(*btn_hit)(HMI *self);
	
};

typedef   const char  ro_char;


//------------------------------------------------------------------------------
// global variable declarations
//------------------------------------------------------------------------------
extern const Except_T Hmi_Failed;
//extern  ro_char str_endRow[];
//extern	ro_char str_endCol[];
extern HMI *g_p_curHmi, *g_p_lastHmi, *g_p_win_last;
//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
extern void Set_flag_show(uint8_t	*p_flag, int val);
extern void Set_flag_keyhandle(uint8_t	*p_flag, int val);
void STY_Duild_button(void *arg);

//很多界面的处理与主界面的按键处理一样，所以就把主界面的处理开放出来
void Main_btn_hdl(void *arg, uint8_t btn_id);
void Main_HMI_build_button(HMI *self);
void Main_HMI_hit( HMI *self, char *s);



#endif
