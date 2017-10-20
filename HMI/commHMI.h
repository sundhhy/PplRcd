#ifndef _INC_commHMI_H_
#define _INC_commHMI_H_
#include "HMI.h"
#include "ModelFactory.h"
#include "sheet.h"
#include "keyboardHMI.h"
//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#include <stdint.h>
//------------------------------------------------------------------------------
// check for correct compilation options
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define NUM_CHANNEL			6

#define	ICO_ID_MENU				0x20
#define	ICO_ID_PGUP				0x21
#define	ICO_ID_PGDN				0x22
#define	ICO_ID_ERASETOOL		0x23
#define	ICO_ID_SEARCH			0x24

#define	SHEET_BOXLIST				0x2a
#define	SHEET_G_TEXT				0x2b

#define IS_CHECK(n)				((n&0xf0) == 0x30)
#define SHTID_CHECK(n)			(0x30 + n)

#define	GET_CHN_FROM_ID(id)		(id & 0x0f)

//条纹背景图片的属性,这是从背景图片中得到的数据
#define STRIPE_MAX_ROWS			11
#define STRIPE_VY0				34		//第一行宽2个像素点，就单独出来
#define STRIPE_VY1				50
#define STRIPE_SIZE_Y			16
#define STRIPE_CLR_1			COLOUR_GRAY
#define STRIPE_CLR_2			COLOUR_BLACK
#define STRIPE_CLR_FOCUSE		COLOUR_BLUE
 //------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------
CLASS( cmmHmi)
{
	Model			*p_mdlRtc;
//	shtctl			*p_shtctl;
//	sheet			*p_shtTime;
	EXTENDS( HMI);
	
	
	
};
//------------------------------------------------------------------------------
// global variable declarations
//------------------------------------------------------------------------------

//sheet
extern	sheet			*g_p_sht_bkpic, *g_p_sht_title, *g_p_shtTime, *g_p_text, *g_p_boxlist;

extern 	sheet			*g_p_ico_memu, *g_p_ico_digital, *g_p_ico_bar,  *g_p_ico_trend, *g_p_cpic;
extern 	sheet			*g_p_ico_pgup;
extern 	sheet			*g_p_ico_pgdn;
extern	sheet			*g_p_ico_eraseTool;
extern	sheet			*g_p_ico_search;

extern  sheet			*g_arr_p_chnData[NUM_CHANNEL];
extern	sheet			*g_arr_p_chnUtil[NUM_CHANNEL];
extern	sheet			*g_arr_p_chnAlarm[NUM_CHANNEL];
extern	sheet  			*g_arr_p_check[NUM_CHANNEL]; 		//通道的勾选图标


//values
extern 	char			prn_buf[NUM_CHANNEL][8];
extern	const char		arr_clrs[NUM_CHANNEL];
extern	hmiAtt_t		CmmHmiAtt;
extern 	keyboardHMI		*g_keyHmi;
extern 	ro_char 		news_cpic[];
//HMI
extern 	HMI 			*g_p_mainHmi;
extern 	HMI 			*g_p_HMI_menu;
extern  HMI 			*g_p_dataHmi;
extern 	HMI 			*g_p_barGhHmi;
extern 	HMI 			*g_p_RLT_trendHmi;

extern 	HMI 			*g_p_News_Alarm_HMI;
extern	HMI 			*g_p_News_PwrDn_HMI;
extern	HMI 			*g_p_History_HMI;
extern	HMI 			*g_p_Accm_HMI;

extern 	HMI 			*g_p_Setup_HMI;
extern 	HMI 			*g_p_Setting_HMI;


extern char			*arr_p_hmi_buf[6];
//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
cmmHmi *GetCmmHMI(void);

extern void Build_ChnSheets(void);
extern int Stripe_clean_clr(int row);
extern int Stripe_vy(int row) ;
#endif
