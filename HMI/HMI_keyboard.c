#include "HMI_keyboard.h"
#include "HMI_comm.h"

#include <string.h>
#include "ExpFactory.h"

#include "format.h"



//提供 按键，事件，消息，窗口，报警，时间，复选框的图层
//这些图层可能会被其他界面所使用
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
#define KEYBBUFLEN				16

#define DIRE_UP					0
#define DIRE_DOWN				1
#define DIRE_LEFT				2
#define DIRE_RIGHT				3

#define KEY_NONE				0

#define INPUTMETHOD_UPP			0
#define INPUTMETHOD_LOWER			1
#define INPUTMETHOD_DIGIT			2

const char KEY_123			=		0xff;
const char KEY_backspace	=		0xfe;
const char KEY_GO			=		0xfd;
const char KEY_Return		=		0xfc;

const char virKey_uppercase_Code[] = { "<pic vx0=0 vy0=40 >1</>" };
const char virKey_uppercaseFocus_Code[] = { "<cpic vx0=0 vy0=0 >2</>" };

const char virKey_digit_Code[] = { "<pic vx0=0 vy0=40 >3</>" };
const char virKey_digitFocus_Code[] = { "<cpic vx0=0 vy0=0 >4</>" };


const char edit_TxtCode[] = { "<text vx0=0 vy0=0 f=16 clr=white > </>" };
const char edit_CursorCode[] = { "<box bkc=black ></>" };
const char edit_NotifyCode[] = { "<bu clr=blue  vx0=270 vy0=0 rg=4 cg=4 ><text f=16 xali=r yali=r clr=red>小写</></>" };
const char	notify_uppcase[] = "大写";
const char	notify_lowercase[] = "小写";
const char	notify_emptycase[] = "数字";

const 	char	uppKeyVal[4][10] = { \
	{ 'Q','W','E','R','T','Y','U','I','O','P'},\
	{ 'A','S','D','F','G','H','J','K','L',KEY_NONE},\
	{ KEY_123,'Z','X','C','V','B','N','M',KEY_backspace,KEY_NONE},\
	{ ' ', KEY_GO} \
};

const 	char	lowerKeyVal[4][10] = { \
	{ 'q','w','e','r','t','y','u','i','o','p'},\
	{ 'a','s','d','f','g','h','j','k','l',KEY_NONE},\
	{ KEY_123,'z','x','c','v','b','n','m',KEY_backspace,KEY_NONE},\
	{ ' ', KEY_GO} \
};

const 	char	digitKeyVal[4][10] = { \
	{ '1','2','3','4','5','6','7','8','9','0'},\
	{ ':','@','#','$','%','\'','*','(',')',KEY_NONE},\
	{ KEY_Return,'+','-','?',',','.','/','=',KEY_backspace,KEY_NONE},\
	{ ' ', KEY_GO} \
};
typedef const char (*keyvalptr)[10];

keyvalptr  arr_p_keyval[NUM_VIRKEY]= { uppKeyVal, lowerKeyVal, digitKeyVal};
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------
typedef struct {
	uint16_t		pvx0, pvy0;
	uint16_t		pvx1, pvy1;
	uint8_t			val;
	uint8_t			rownum, colnum;
	uint8_t			none;
}virKeyInfo_t;

typedef struct {
	//当前按键四周的按键信息:上下左右
//	virKeyInfo_t	vkeyUp;		
//	virKeyInfo_t	vkeyDown;		
//	virKeyInfo_t	vkeyLeft;		
//	virKeyInfo_t	vkeyRight;		
	
	//中间的按键，即被选中的按键
	virKeyInfo_t	vkenCenter;						
}virKeyBlock_t;

typedef struct {
	
	
	void (*vkey_init)( virKeyBlock_t *p_vkb);
	void (*vkey_CalculateAroundKey[4])( virKeyInfo_t *p_vkey);

}virKeyOp_t;
	
typedef struct {
	sheet			*p_shtNotify;					//提示框如当前是 大写还是小写状态
	sheet			*p_shtTxt;					//输入框上的文本
	sheet			*p_shtCursor;					//输入框上的光标
	keyvalptr		p_arr_keyval;
	char 			keybrdbuf[KEYBBUFLEN];
	uint8_t			bufidx;
	uint8_t			maxidx;
	uint8_t			none;
//	uint8_t			cleansize;		//输入框的空余空间
	uint8_t			inputMethod;
}edit_t;
	
	
//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
static keyboardHMI *singalKBHmi;
static	edit_t		keyEdit;

virKeyBlock_t	actKeyBlk;
sheet		shtInputSave;

//----vir key ---------------------
static void VK_Uppercase_init(virKeyBlock_t *p_vkb);
static void Upp_CalculateUp(virKeyInfo_t *p_vkey);
static void Upp_CalculateDown(virKeyInfo_t *p_vkey);
static void Upp_CalculateLeft(virKeyInfo_t *p_vkey);
static void Upp_CalculateRight(virKeyInfo_t *p_vkey);
static void SwitchVirKey( sheet	**arr_p_shtVkey, sheet	**arr_p_shtFocus, uint8_t iptMth);
virKeyOp_t uppVKO = { \
	VK_Uppercase_init, \
	{ Upp_CalculateUp, Upp_CalculateDown, Upp_CalculateLeft, Upp_CalculateRight} \
};

//virKeyOp_t lowerVKO = { \
//	VK_Uppercase_init, \
//	{ Upp_CalculateUp, Upp_CalculateDown, Upp_CalculateLeft, Upp_CalculateRight} \
//};

//virKeyOp_t digitVKO = { \
//	VK_Uppercase_init, \
//	{ Upp_CalculateUp, Upp_CalculateDown, Upp_CalculateLeft, Upp_CalculateRight} \
//};



virKeyOp_t *p_vko[NUM_VIRKEY] = { &uppVKO, &uppVKO, &uppVKO};
static sheet	*p_keyboardPic;
static sheet	*p_keyboardFocusPic;
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static int	Init_kbmHmi( HMI *self, void *arg);

//static void KeyboardEnterCmdHdl( shtCmd *self, struct SHEET *p_sht, void *arg);
static void	KeyboardShow( HMI *self );
static void KBHide( HMI *self );
static void KBInitSheet( HMI *self, uint32_t att );

static void	KeyboardHitHandle( HMI *self, char kcd);
static void	KeyboardLngpshHandle( HMI *self,  char kcd);
static void	KeyboardDouHitHandle( HMI *self,  char kcd);
static void		KYB_Show_cmp(HMI *self);


//----- vir key -------------
static void FocusKey_move( virKeyOp_t *p_keyop, virKeyBlock_t *p_vkb, int direction);
static void DrawFocus( char vkeytype, virKeyInfo_t *p_focus);
static void CleanFocus( char vkeytype, virKeyInfo_t *p_focus);

//----- edit ---------------------------------------
void Edit_init( edit_t *p_ed, sheet	*p_shtInput);
void Edit_push( edit_t *p_ed, virKeyInfo_t *p_kinfo);
void Edit_pop( edit_t *p_ed);
static int	Default_input(void *self, void *data, int len);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
keyboardHMI *GetkeyboardHMI(void)
{
	if( singalKBHmi == NULL)
	{
		singalKBHmi = keyboardHMI_new();
		if(singalKBHmi  == NULL) while(1);
		
	}
	
	return singalKBHmi;
	
}

void HKB_Set_input(sheet *p)
{
	singalKBHmi->p_shtInput = p;
	
}


CTOR( keyboardHMI)
SUPER_CTOR( HMI);
FUNCTION_SETTING( HMI.init, Init_kbmHmi);
FUNCTION_SETTING( HMI.show, KeyboardShow);
FUNCTION_SETTING( HMI.initSheet, KBInitSheet);
FUNCTION_SETTING( HMI.hide, KBHide);
FUNCTION_SETTING( HMI.hitHandle, KeyboardHitHandle);
FUNCTION_SETTING( HMI.longpushHandle, KeyboardLngpshHandle);
FUNCTION_SETTING( HMI.dhitHandle, KeyboardDouHitHandle);
FUNCTION_SETTING(HMI.show_cmp, KYB_Show_cmp);

//FUNCTION_SETTING( shtCmd.shtExcute, KeyboardEnterCmdHdl);
END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//

static int	Init_kbmHmi( HMI *self, void *arg)
{
	keyboardHMI	*cthis = SUB_PTR( self, HMI, keyboardHMI);
	shtctl *p_shtctl = NULL;
	Expr *p_exp ;
	
	p_shtctl = GetShtctl();
	
	
	cthis->p_shtVkey[INPUTMETHOD_UPP] = Sheet_alloc( p_shtctl);
	p_exp = ExpCreate( "pic");
	p_exp->inptSht( p_exp, (void *)virKey_uppercase_Code, cthis->p_shtVkey[INPUTMETHOD_UPP]) ;
	
	cthis->p_shtvKeyCursor[INPUTMETHOD_UPP] = Sheet_alloc( p_shtctl);
	p_exp = ExpCreate( "pic");
	p_exp->inptSht( p_exp, (void *)virKey_uppercaseFocus_Code, cthis->p_shtvKeyCursor[INPUTMETHOD_UPP]) ;
	
	cthis->p_shtVkey[INPUTMETHOD_LOWER] = Sheet_alloc( p_shtctl);
	p_exp = ExpCreate( "pic");
	p_exp->inptSht( p_exp, (void *)virKey_uppercase_Code, cthis->p_shtVkey[INPUTMETHOD_LOWER]) ;
	
	cthis->p_shtvKeyCursor[INPUTMETHOD_LOWER] = Sheet_alloc( p_shtctl);
	p_exp = ExpCreate( "pic");
	p_exp->inptSht( p_exp, (void *)virKey_uppercaseFocus_Code, cthis->p_shtvKeyCursor[INPUTMETHOD_LOWER]) ;
	
	cthis->p_shtVkey[INPUTMETHOD_DIGIT] = Sheet_alloc( p_shtctl);
	p_exp = ExpCreate( "pic");
	p_exp->inptSht( p_exp, (void *)virKey_digit_Code, cthis->p_shtVkey[INPUTMETHOD_DIGIT]) ;
	
	cthis->p_shtvKeyCursor[INPUTMETHOD_DIGIT] = Sheet_alloc( p_shtctl);
	p_exp = ExpCreate( "pic");
	p_exp->inptSht( p_exp, (void *)virKey_digitFocus_Code, cthis->p_shtvKeyCursor[INPUTMETHOD_DIGIT]) ;
	
	keyEdit.p_shtTxt = Sheet_alloc( p_shtctl);
	p_exp = ExpCreate( "text");
	p_exp->inptSht( p_exp, (void *)edit_TxtCode, keyEdit.p_shtTxt) ;
	keyEdit.p_shtTxt->cnt.data = keyEdit.keybrdbuf;
	
	keyEdit.p_shtCursor = Sheet_alloc( p_shtctl);
	p_exp = ExpCreate( "box");
	p_exp->inptSht( p_exp, (void *)edit_CursorCode, keyEdit.p_shtCursor) ;
	
	keyEdit.p_shtNotify = Sheet_alloc( p_shtctl);
	p_exp = ExpCreate( "bu");
	p_exp->inptSht( p_exp, (void *)edit_NotifyCode, keyEdit.p_shtNotify) ;
	

	keyEdit.p_shtNotify->area.x1 = keyEdit.p_shtNotify->area.x0 + keyEdit.p_shtNotify->bxsize;
	keyEdit.p_shtNotify->area.y1 = keyEdit.p_shtNotify->area.y0 + keyEdit.p_shtNotify->bysize;
	
	FormatSheetSub( keyEdit.p_shtNotify);
	
	keyEdit.inputMethod = INPUTMETHOD_UPP;

	cthis->p_shtInput = NULL;
	
	return RET_OK;
}

static void		KYB_Show_cmp(HMI *self)
{
	
	
}

static void KBInitSheet( HMI *self, uint32_t att )
{
	keyboardHMI		*cthis = SUB_PTR( self, HMI, keyboardHMI);
	
		
//	Sheet_updown( cthis->p_shtVkey[ keyEdit.inputMethod], 0);
	
	if( cthis->p_shtInput != NULL) {
		//保存旧的设置
//		shtInputSave.area.x0 = cthis->p_shtInput->area.x0;
//		shtInputSave.area.y0 = cthis->p_shtInput->area.y0;
//		shtInputSave.area.x1 = cthis->p_shtInput->area.x1;
//		shtInputSave.area.y1 = cthis->p_shtInput->area.y1;
//		shtInputSave.cnt.effects = cthis->p_shtInput->cnt.effects;
//		shtInputSave.cnt.bkc = cthis->p_shtInput->cnt.bkc;
//		
//		
//		//设置新的坐标
//		cthis->p_shtInput->area.x0 = 0;
////		cthis->p_shtInput->area.y0 = 30 - ( shtInputSave.area.y1 - shtInputSave.area.y0);
////		cthis->p_shtInput->area.x1 = shtInputSave.area.x1 - shtInputSave.area.x0;
//		cthis->p_shtInput->area.y0 = 0;
//		cthis->p_shtInput->area.x1 = shtInputSave.area.x1 - cthis->p_shtInput->bxsize;
//		cthis->p_shtInput->area.y1 = 30;
//		cthis->p_shtInput->cnt.effects = 0;
//		cthis->p_shtInput->cnt.bkc = COLOUR_BLACK;
		
		
//		FormatSheetSub( cthis->p_shtInput);
//		Sheet_updown( cthis->p_shtInput, 0);

//		shtInputSave.input = cthis->p_shtInput->input;
		Edit_init(&keyEdit, cthis->p_shtInput);
//		if(cthis->p_shtInput->input == NULL)
//			cthis->p_shtInput->input = Default_input;

		Sheet_updown( keyEdit.p_shtNotify, 0);
		Sheet_updown(keyEdit.p_shtTxt, 1);

	}
	
}

static void KBHide( HMI *self )
{
	keyboardHMI		*cthis = SUB_PTR( self, HMI, keyboardHMI);
	
	//恢复输入框的设置
	if( cthis->p_shtInput != NULL) {
		//保存旧的设置
//		cthis->p_shtInput->area.x0 = shtInputSave.area.x0;
//		cthis->p_shtInput->area.y0 = shtInputSave.area.y0;
//		cthis->p_shtInput->area.x1 = shtInputSave.area.x1;
//		cthis->p_shtInput->area.y1 = shtInputSave.area.y1;
//		cthis->p_shtInput->cnt.effects = shtInputSave.cnt.effects;
//		cthis->p_shtInput->cnt.bkc = shtInputSave.cnt.bkc;
//				
//		FormatSheetSub( cthis->p_shtInput);
//		 cthis->p_shtInput->input = shtInputSave.input;
		
		Sheet_updown(keyEdit.p_shtTxt, -1);
		Sheet_updown( keyEdit.p_shtNotify, -1);

//		Sheet_updown( cthis->p_shtInput, -1);
		
		cthis->p_shtInput = NULL;
	}
//	Sheet_updown( cthis->p_shtVkey[  keyEdit.inputMethod], -1);
}

static void	KeyboardShow( HMI *self )
{
	keyboardHMI		*cthis = SUB_PTR( self, HMI, keyboardHMI);
	I_dev_lcd 	*p_lcd;
	virKeyOp_t	*p_op;
	g_p_curHmi = self;
	Dev_open( LCD_DEVID, (void *)&p_lcd);
	p_lcd->Clear( CmmHmiAtt.bkc);
	
	

	SwitchVirKey( cthis->p_shtVkey, cthis->p_shtvKeyCursor, INPUTMETHOD_UPP);
	
	p_op = p_vko[ keyEdit.inputMethod];
	p_op->vkey_init( &actKeyBlk); 

	if( cthis->p_shtInput != NULL)
		Sheet_refresh(keyEdit.p_shtNotify);
	CleanFocus( keyEdit.inputMethod, &actKeyBlk.vkenCenter);		
	DrawFocus( 0, &actKeyBlk.vkenCenter);
	
	Flush_LCD();
	
}

static void CleanFocus( char vkeytype, virKeyInfo_t *p_focus)
{
	p_keyboardPic->p_gp->vdraw( p_keyboardPic->p_gp, &p_keyboardPic->cnt, &p_keyboardPic->area);
	Flush_LCD();

}

static void DrawFocus( char vkeytype, virKeyInfo_t *p_focus)
{
	p_keyboardFocusPic->area.offset_x = p_keyboardPic->area.x0;
	p_keyboardFocusPic->area.offset_y = p_keyboardPic->area.y0;
	
	p_keyboardFocusPic->area.x0 = p_focus->pvx0;
	p_keyboardFocusPic->area.y0 = p_focus->pvy0;
	p_keyboardFocusPic->area.x1 = p_focus->pvx1;
	p_keyboardFocusPic->area.y1 = p_focus->pvy1;
	
	p_keyboardFocusPic->p_gp->vdraw( p_keyboardFocusPic->p_gp, &p_keyboardFocusPic->cnt, &p_keyboardFocusPic->area);
	Flush_LCD();
}
static void	KeyboardLngpshHandle( HMI *self,  char kcd)
{
	keyboardHMI		*cthis = SUB_PTR( self, HMI, keyboardHMI);
	if(kcd == KEYCODE_ENTER) {
		//大小写切换
		if( keyEdit.inputMethod == INPUTMETHOD_LOWER) {
			
			SwitchVirKey( cthis->p_shtVkey, cthis->p_shtvKeyCursor, INPUTMETHOD_UPP);
		} else if( keyEdit.inputMethod == INPUTMETHOD_UPP) {
			
			SwitchVirKey( cthis->p_shtVkey, cthis->p_shtvKeyCursor, INPUTMETHOD_LOWER);
		}
	}
	
}
static void	KeyboardDouHitHandle( HMI *self,  char kcd)
{
	keyboardHMI		*cthis = SUB_PTR( self, HMI, keyboardHMI);
	if(kcd == KEYCODE_ENTER) {
		//大小写切换
		if( keyEdit.inputMethod == INPUTMETHOD_LOWER) {
			
			SwitchVirKey( cthis->p_shtVkey, cthis->p_shtvKeyCursor, INPUTMETHOD_UPP);
		} else if( keyEdit.inputMethod == INPUTMETHOD_UPP) {
			
			SwitchVirKey( cthis->p_shtVkey, cthis->p_shtvKeyCursor, INPUTMETHOD_LOWER);
		}
	}
	
}
static void	KeyboardHitHandle( HMI *self, char kcd)
{
	keyboardHMI		*cthis = SUB_PTR( self, HMI, keyboardHMI);
	virKeyOp_t		*p_op;
//	HMI				*src_hmi;
	int				cmt_ret = 0;
	
	
	switch(kcd)
	{

		case KEYCODE_UP:
			//清除原来的选中效果
			
			CleanFocus( keyEdit.inputMethod, &actKeyBlk.vkenCenter);
			
			//计算新的选中位置
			FocusKey_move( p_vko[ keyEdit.inputMethod], &actKeyBlk, DIRE_UP);
			
			//绘制新的选中效果
			DrawFocus( keyEdit.inputMethod, &actKeyBlk.vkenCenter);
			break;
		case KEYCODE_DOWN:
			CleanFocus( keyEdit.inputMethod, &actKeyBlk.vkenCenter);
			FocusKey_move( p_vko[ keyEdit.inputMethod], &actKeyBlk, DIRE_DOWN);
			DrawFocus( keyEdit.inputMethod, &actKeyBlk.vkenCenter);
			break;
		case KEYCODE_LEFT:
			CleanFocus( keyEdit.inputMethod, &actKeyBlk.vkenCenter);
			FocusKey_move( p_vko[ keyEdit.inputMethod], &actKeyBlk, DIRE_LEFT);
			DrawFocus( keyEdit.inputMethod, &actKeyBlk.vkenCenter);
			break;
		case KEYCODE_RIGHT:
			CleanFocus( keyEdit.inputMethod, &actKeyBlk.vkenCenter);
			FocusKey_move( p_vko[ keyEdit.inputMethod], &actKeyBlk, DIRE_RIGHT);
			DrawFocus( keyEdit.inputMethod, &actKeyBlk.vkenCenter);
			break;

		case KEYCODE_ENTER:
			if( actKeyBlk.vkenCenter.val == KEY_123 ) {
				SwitchVirKey( cthis->p_shtVkey, cthis->p_shtvKeyCursor, INPUTMETHOD_DIGIT);

				p_op = p_vko[ keyEdit.inputMethod];
				p_op->vkey_init( &actKeyBlk);  

				CleanFocus( keyEdit.inputMethod, &actKeyBlk.vkenCenter);			
				DrawFocus( 0, &actKeyBlk.vkenCenter);
			} else if( actKeyBlk.vkenCenter.val == KEY_Return) {
				SwitchVirKey( cthis->p_shtVkey, cthis->p_shtvKeyCursor, INPUTMETHOD_LOWER);

				p_op = p_vko[ keyEdit.inputMethod];
				p_op->vkey_init( &actKeyBlk);  

				CleanFocus( keyEdit.inputMethod, &actKeyBlk.vkenCenter);			
				DrawFocus( 0, &actKeyBlk.vkenCenter);
			} else if( actKeyBlk.vkenCenter.val == KEY_GO) {

				if(kbr_cmt)
				{

					cmt_ret = kbr_cmt(cthis->p_shtInput, keyEdit.keybrdbuf, keyEdit.bufidx);
				}
				else
				{
					cmt_ret = Default_input(cthis->p_shtInput, keyEdit.keybrdbuf, keyEdit.bufidx);

				}
				if(cmt_ret == RET_OK) {
//					src_hmi = g_p_lastHmi;
//					src_hmi->flag |= HMIFLAG_KEYBOARD;
//					self->switchBack(self, 0);
//					src_hmi->flag &= ~HMIFLAG_KEYBOARD;
					
					self->switchBack(self, HMI_ATT_KEEP);
				} else {
					//todo: 提示错误
				}
			} else if( actKeyBlk.vkenCenter.val == KEY_backspace) {
				Edit_pop (&keyEdit);
			} else {
				Edit_push( &keyEdit, &actKeyBlk.vkenCenter);
			}
			break;		
		case KEYCODE_ESC:
			self->switchBack(self, 0);
			break;	
			
	}
	
//	if( !strcmp( s, HMIKEY_UP) )
//	{
//		//清除原来的选中效果
//		
//		CleanFocus( keyEdit.inputMethod, &actKeyBlk.vkenCenter);
//		
//		//计算新的选中位置
//		FocusKey_move( p_vko[ keyEdit.inputMethod], &actKeyBlk, DIRE_UP);
//		
//		//绘制新的选中效果
//		DrawFocus( keyEdit.inputMethod, &actKeyBlk.vkenCenter);
//		
//	}
//	else if( !strcmp( s, HMIKEY_DOWN) )
//	{
//		CleanFocus( keyEdit.inputMethod, &actKeyBlk.vkenCenter);
//		FocusKey_move( p_vko[ keyEdit.inputMethod], &actKeyBlk, DIRE_DOWN);
//		DrawFocus( keyEdit.inputMethod, &actKeyBlk.vkenCenter);
//	}
//	else if( !strcmp( s, HMIKEY_LEFT))
//	{
//		CleanFocus( keyEdit.inputMethod, &actKeyBlk.vkenCenter);
//		FocusKey_move( p_vko[ keyEdit.inputMethod], &actKeyBlk, DIRE_LEFT);
//		DrawFocus( keyEdit.inputMethod, &actKeyBlk.vkenCenter);
//	}
//	else if( !strcmp( s, HMIKEY_RIGHT))
//	{
//		CleanFocus( keyEdit.inputMethod, &actKeyBlk.vkenCenter);
//		FocusKey_move( p_vko[ keyEdit.inputMethod], &actKeyBlk, DIRE_RIGHT);
//		DrawFocus( keyEdit.inputMethod, &actKeyBlk.vkenCenter);
//	}
//	
//	if( !strcmp( s, HMIKEY_ESC))
//	{
//		self->switchBack(self, 0);
//	}
//	if( !strcmp( s, KEYCODE_ENTER))
//	{
//		if( actKeyBlk.vkenCenter.val == KEY_123 ) {
//			SwitchVirKey( cthis->p_shtVkey, cthis->p_shtvKeyCursor, INPUTMETHOD_DIGIT);
//			
//			p_op = p_vko[ keyEdit.inputMethod];
//			p_op->vkey_init( &actKeyBlk);  

//			CleanFocus( keyEdit.inputMethod, &actKeyBlk.vkenCenter);			
//			DrawFocus( 0, &actKeyBlk.vkenCenter);
//		} else if( actKeyBlk.vkenCenter.val == KEY_Return) {
//			SwitchVirKey( cthis->p_shtVkey, cthis->p_shtvKeyCursor, INPUTMETHOD_LOWER);
//			
//			p_op = p_vko[ keyEdit.inputMethod];
//			p_op->vkey_init( &actKeyBlk);  

//			CleanFocus( keyEdit.inputMethod, &actKeyBlk.vkenCenter);			
//			DrawFocus( 0, &actKeyBlk.vkenCenter);
//		} else if( actKeyBlk.vkenCenter.val == KEY_GO) {
//			
//			if(kbr_cmt)
//			{
//				
//				cmt_ret = kbr_cmt(cthis->p_shtInput, keyEdit.keybrdbuf, keyEdit.bufidx);
//			}
//			else
//			{
//				cmt_ret = Default_input(cthis->p_shtInput, keyEdit.keybrdbuf, keyEdit.bufidx);
//				
//			}
//			if(cmt_ret == RET_OK) {
//				src_hmi = g_p_lastHmi;
//				src_hmi->flag |= HMIFLAG_KEYBOARD;
//				self->switchBack(self, 0);
//				src_hmi->flag &= ~HMIFLAG_KEYBOARD;
//			} else {
//				//todo: 提示错误
//			}
//		} else if( actKeyBlk.vkenCenter.val == KEY_backspace) {
//			Edit_pop (&keyEdit);
//		} else {
//			Edit_push( &keyEdit, &actKeyBlk.vkenCenter);
//		}
//	}
	
	
}

//static void KeyboardEnterCmdHdl( shtCmd *self, struct SHEET *p_sht, void *arg)
//{
//	keyboardHMI	*cthis = SUB_PTR( self, shtCmd, keyboardHMI);
//	HMI			*selfHmi = SUPER_PTR( cthis, HMI);
//	HMI		*srcHmi = ( HMI *)arg;
//	
//	cthis->p_shtInput = p_sht;
//	srcHmi->switchHMI( srcHmi, selfHmi);
//	
//}

static int	Default_input(void *self, void *data, int len)
{
	sheet		*p = (sheet *)self;
	memset(p->cnt.data, 0, strlen(p->cnt.data));
	memcpy(p->cnt.data, data, len);
	
	return RET_OK;
	
}


/*********** vir key *****************************************/
//static void VKeyClone( virKeyInfo_t *p_dstVkey, virKeyInfo_t *p_srcVkey)
//{
//	p_dstVkey->pvx0 = p_srcVkey->pvx0;
//	p_dstVkey->pvx1 = p_srcVkey->pvx1;
//	p_dstVkey->pvy0 = p_srcVkey->pvy0;
//	p_dstVkey->pvy1 = p_srcVkey->pvy1;
//	p_dstVkey->val = p_srcVkey->val;
//	p_dstVkey->rownum = p_srcVkey->rownum;
//	p_dstVkey->colnum = p_srcVkey->colnum;
//	
//}

static void VKeyClean( virKeyInfo_t *p_vkey)
{
	p_vkey->pvx0 = 0;
	p_vkey->pvx1 = 0;
	p_vkey->pvy0 = 0;
	p_vkey->pvy1 = 0;
	p_vkey->rownum = 0;
	p_vkey->colnum = 0;
}

static void SwitchVirKey( sheet	**arr_p_shtVkey, sheet	**arr_p_shtFocus, uint8_t iptMth)
{
	sheet	*p_notifyText;
	
	p_keyboardPic = arr_p_shtVkey[ iptMth];
	p_keyboardFocusPic  = arr_p_shtFocus[ iptMth];
	keyEdit.inputMethod = iptMth;
	keyEdit.p_arr_keyval = arr_p_keyval[ iptMth];
	
	p_notifyText = keyEdit.p_shtNotify->pp_sub[0];
	if( keyEdit.inputMethod == INPUTMETHOD_UPP) {
		p_notifyText->cnt.data = (char *)notify_uppcase;
		p_notifyText->cnt.len = sizeof( notify_uppcase);
	} else if( keyEdit.inputMethod == INPUTMETHOD_LOWER) {
		p_notifyText->cnt.data = (char *)notify_lowercase;
		p_notifyText->cnt.len = sizeof( notify_lowercase);
	} else  {
		p_notifyText->cnt.data = (char *)notify_emptycase;
		p_notifyText->cnt.len = sizeof( notify_emptycase);
	}
	
//	keyEdit.p_shtNotify->p_gp->vdraw( keyEdit.p_shtNotify->p_gp, &keyEdit.p_shtNotify->cnt, &keyEdit.p_shtNotify->area);
	Sheet_refresh( keyEdit.p_shtNotify);	
}

static void FocusKey_move( virKeyOp_t *p_keyop, virKeyBlock_t *p_vkb, int direction)
{
	
	switch( direction) 
	{
		case DIRE_UP:
//			VKeyClone( &p_vkb->vkenCenter, &p_vkb->vkeyUp);
			p_keyop->vkey_CalculateAroundKey[ DIRE_UP]( &p_vkb->vkenCenter);
			break;
		case DIRE_DOWN:
//			VKeyClone( &p_vkb->vkenCenter, &p_vkb->vkeyDown);
			p_keyop->vkey_CalculateAroundKey[ DIRE_DOWN]( &p_vkb->vkenCenter);
			break;
		case DIRE_LEFT:
//			VKeyClone( &p_vkb->vkenCenter, &p_vkb->vkeyLeft);
			p_keyop->vkey_CalculateAroundKey[ DIRE_LEFT]( &p_vkb->vkenCenter);
			break;
		case DIRE_RIGHT:
//			VKeyClone( &p_vkb->vkenCenter, &p_vkb->vkeyRight);
			p_keyop->vkey_CalculateAroundKey[ DIRE_RIGHT]( &p_vkb->vkenCenter);
			break;
		default:
			goto exit;
		
	}
	
	
	
	
	exit:
	return;
}

static char GetVKeyVal( uint8_t row, uint8_t col)
{
	return keyEdit.p_arr_keyval[row][col];
	
}

//-----------  大写字母键盘 --------------------------------

//根据键值推算该键在图片中的坐标
static void Upp_crdnInPic( virKeyInfo_t *p_vkey)
{
	//这些数据都是对键盘图片实测而来
	static char	first_bx[4] = { 2, 18,50,90};		//以第一个字母的位置作为起始
	static char	by[4] = { 3, 54,104,150};		
	
	static char	charsizex = 27;
	static char charsizey = 45;
	static char	colgrap = 5;
	
	if( p_vkey->val == KEY_123 || p_vkey->val == KEY_Return) {
		
		p_vkey->pvx0 = 2;
		p_vkey->pvy0 = 100;
		p_vkey->pvx1 = 48;
		p_vkey->pvy1 = 147;
	} else if( p_vkey->val == KEY_backspace) {
		p_vkey->pvx0 = 273;
		p_vkey->pvy0 = 102;
		p_vkey->pvx1 = 319;
		p_vkey->pvy1 = 149;
		
	} else if( p_vkey->val == ' ') {
		p_vkey->pvx0 = 90;
		p_vkey->pvy0 = 150;
		p_vkey->pvx1 = 228;
		p_vkey->pvy1 = 195;
	} else if( p_vkey->val == KEY_GO) {
		p_vkey->pvx0 = 273;
		p_vkey->pvy0 = 148;
		p_vkey->pvx1 = 319;
		p_vkey->pvy1 = 195;
	} else {
		if( p_vkey->rownum == 2) {
			//第三行的第一个按键不是字母，因此在计算是进行补偿
			p_vkey->pvx0 = first_bx[ p_vkey->rownum] + ( p_vkey->colnum - 1) * ( charsizex + colgrap);
		} else {
			p_vkey->pvx0 = first_bx[ p_vkey->rownum] + p_vkey->colnum * ( charsizex + colgrap);
		}
		p_vkey->pvx1 = p_vkey->pvx0 + charsizex;
		p_vkey->pvy0 = by[ p_vkey->rownum];
		p_vkey->pvy1 = p_vkey->pvy0 + charsizey;
	}
	
}

//根据中心按键来计算上下左右的按键的信息
static void Upp_CalculateUp( virKeyInfo_t *p_vkey)
{
//	virKeyInfo_t *p_vkey = &p_vkb->vkeyUp;
	
//	if( p_vkb->vkenCenter.rownum) {
//		p_vkey->rownum = p_vkb->vkenCenter.rownum - 1;
//	} else {
//		p_vkey->rownum = 3;
//	}
//	
//	p_vkey->colnum = p_vkb->vkenCenter.colnum;
	
	if( p_vkey->rownum) {
		p_vkey->rownum = p_vkey->rownum - 1;
	} else {
		p_vkey->rownum = 3;
	}
	
	
	//判断该位置有无按键，如果没有，就找到这一行离这个位置最近的按键
	while( GetVKeyVal( p_vkey->rownum, p_vkey->colnum) == KEY_NONE) {
		
		if( p_vkey->colnum)
			p_vkey->colnum --;
	}
	p_vkey->val = GetVKeyVal( p_vkey->rownum, p_vkey->colnum); 
	
	Upp_crdnInPic( p_vkey);
	
}
static void Upp_CalculateDown(virKeyInfo_t *p_vkey)
{
//	virKeyInfo_t *p_vkey = &p_vkb->vkeyDown;
	
//	if( p_vkb->vkenCenter.rownum < 3) {
//		p_vkey->rownum = p_vkb->vkenCenter.rownum + 1;
//	} else {
//		p_vkey->rownum = 0;
//	}
//	
//	p_vkey->colnum = p_vkb->vkenCenter.colnum;
	
	if( p_vkey->rownum < 3) {
		p_vkey->rownum = p_vkey->rownum + 1;
	} else {
		p_vkey->rownum = 0;
	}
	
	//判断该位置有无按键，如果没有，就找到这一行离这个位置最近的按键
	while( GetVKeyVal( p_vkey->rownum, p_vkey->colnum) == KEY_NONE) {
		if( p_vkey->colnum)
			p_vkey->colnum --;
	}
	p_vkey->val = GetVKeyVal( p_vkey->rownum, p_vkey->colnum); 
	
	Upp_crdnInPic( p_vkey);
	
}

static void Upp_CalculateLeft(virKeyInfo_t *p_vkey)
{
//	virKeyInfo_t *p_vkey = &p_vkb->vkeyLeft;
	int i = 0;
	
//	p_vkey->rownum = p_vkb->vkenCenter.rownum;
	if( p_vkey->colnum) {
		p_vkey->colnum = p_vkey->colnum - 1;
	} else {
		
		p_vkey->colnum = 9;
	}
	
	//不考虑一行中有空洞的情况
	if( GetVKeyVal( p_vkey->rownum, p_vkey->colnum)== KEY_NONE) {
		
		for( i = 9; i>= 0; i--) {
			if( GetVKeyVal( p_vkey->rownum, i) != KEY_NONE)
				break;
			
		}
		p_vkey->colnum = i;
	}
	

	p_vkey->val = GetVKeyVal( p_vkey->rownum, p_vkey->colnum); 
	
	Upp_crdnInPic( p_vkey);
	
}

static void Upp_CalculateRight(virKeyInfo_t *p_vkey)
{
//	virKeyInfo_t *p_vkey = &p_vkb->vkeyRight;
	int i = 0;
	
//	p_vkey->rownum = p_vkb->vkenCenter.rownum;
	if(p_vkey->colnum < 9) {
		p_vkey->colnum = p_vkey->colnum + 1;
	} else {
		p_vkey->colnum = 0;
		
	}
	
	
	//不考虑一行中有空洞的情况
	if( GetVKeyVal( p_vkey->rownum, p_vkey->colnum) == KEY_NONE) {
		
		for( i = 0; i < 10; i++) {
			if( GetVKeyVal( p_vkey->rownum, i) != KEY_NONE)
				break;
			
		}
		p_vkey->colnum = i;
	}
	
	
	
	
	

	p_vkey->val = GetVKeyVal( p_vkey->rownum, p_vkey->colnum); 
	
	Upp_crdnInPic( p_vkey);
	
}
static void VK_Uppercase_init( virKeyBlock_t *p_vkb)
{
	
	
	VKeyClean( &p_vkb->vkenCenter);
//	Upp_GetVal( &p_vkb->vkenCenter);
	p_vkb->vkenCenter.val = GetVKeyVal( 0, 0); 
	Upp_crdnInPic( &p_vkb->vkenCenter);
	
	//初始化四周的按键的初始位置
	Upp_CalculateUp( &p_vkb->vkenCenter);
	Upp_CalculateDown( &p_vkb->vkenCenter);
	Upp_CalculateLeft( &p_vkb->vkenCenter);
	Upp_CalculateRight( &p_vkb->vkenCenter);
	
	//显示大小写
	
	
}

//---- edit --------------------------
#define SAVE_SPACE		2
void Edit_init( edit_t *p_ed, sheet	*p_shtInput)
{
//	sheet	*p_InputBox = p_shtInput->pp_sub[1];
	
	
	
//	p_ed->p_shtCursor->area.x0 = p_InputBox->area.x0 + 1;
//	p_ed->p_shtCursor->area.y0 = p_InputBox->area.y0 + 1;
//	p_ed->p_shtCursor->area.x1 = p_InputBox->area.x1 - 1;
//	p_ed->p_shtCursor->area.y1 = p_InputBox->area.y1 - 1;
	
//	p_ed->p_shtCursor->area.x0 = p_InputBox->area.x0 + 1;
//	p_ed->p_shtCursor->area.y0 = p_InputBox->area.y0 + 1;
//	p_ed->p_shtCursor->area.x1 = p_InputBox->area.x1 - 1;
//	p_ed->p_shtCursor->area.y1 = p_InputBox->area.y1 - 1;
	
//	p_ed->vsizex = p_InputBox->area.x1 - p_InputBox->area.x0;
//	p_ed->cleansize = p_ed->vsizex;
	
//	p_ed->p_shtTxt->cnt.font = p_shtInput->cnt.font;
//	p_ed->p_shtTxt->p_gp->getSize( p_ed->p_shtTxt->p_gp,  p_ed->p_shtTxt->cnt.font, \
//		&p_ed->p_shtTxt->bxsize, &p_ed->p_shtTxt->bysize);
//	
//	p_ed->p_shtTxt->area.x0 = p_InputBox->area.x0 + SAVE_SPACE;
//	p_ed->p_shtTxt->area.y0 = p_InputBox->area.y1 - p_ed->p_shtTxt->bysize - SAVE_SPACE;
	
	memset(p_ed->p_shtTxt->cnt.data, ' ', KEYBBUFLEN);
//	if(p_InputBox) {
		memcpy(p_ed->p_shtTxt->cnt.data, p_shtInput->cnt.data, p_shtInput->cnt.len);
//		
////		p_ed->p_shtTxt->cnt.len = p_InputBox->cnt.len;
//	} else {
////		p_ed->p_shtTxt->cnt.len = 0;
//	}
	p_ed->bufidx = p_shtInput->cnt.len;
	p_ed->maxidx = KEYBBUFLEN;
	p_ed->p_shtTxt->cnt.len = KEYBBUFLEN;
	
//	p_ed->maxidx = ( p_ed->vsizex - SAVE_SPACE)/ p_ed->p_shtTxt->bxsize;
//	if( p_ed->maxidx > KEYBBUFLEN) {
//		p_ed->maxidx = KEYBBUFLEN ;
//	}
//		
//	p_ed->bufidx = 0;
	
//	memset( p_ed->keybrdbuf, 0, sizeof( p_ed->keybrdbuf));
	
	
	
	
}

//增加一个显示
void Edit_push( edit_t *p_ed, virKeyInfo_t *p_kinfo)
{
	char val;
	if(p_ed->bufidx >= p_ed->maxidx)
		return;
	
	val = GetVKeyVal( p_kinfo->rownum, p_kinfo->colnum);
	p_ed->keybrdbuf[p_ed->bufidx] = val;
	p_ed->bufidx ++;
//	p_ed->p_shtTxt->cnt.len = 	p_ed->bufidx;
	
	
//	p_ed->p_shtCursor->p_gp->vdraw( p_ed->p_shtCursor->p_gp, &p_ed->p_shtCursor->cnt, &p_ed->p_shtCursor->area);
	p_ed->p_shtTxt->p_gp->vdraw( p_ed->p_shtTxt->p_gp, &p_ed->p_shtTxt->cnt, &p_ed->p_shtTxt->area);
	
	Flush_LCD();
}

void Edit_pop( edit_t *p_ed)
{
	if( p_ed->bufidx == 0)
		return;
	p_ed->bufidx --;
	p_ed->keybrdbuf[p_ed->bufidx] = ' ';
	
//	p_ed->p_shtTxt->cnt.len = 	p_ed->bufidx;
	
	
//	p_ed->p_shtCursor->p_gp->vdraw( p_ed->p_shtCursor->p_gp, &p_ed->p_shtCursor->cnt, &p_ed->p_shtCursor->area);
	p_ed->p_shtTxt->p_gp->vdraw( p_ed->p_shtTxt->p_gp, &p_ed->p_shtTxt->cnt, &p_ed->p_shtTxt->area);
	Flush_LCD();
	
}






