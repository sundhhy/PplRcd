#include "keyboardHMI.h"
#include "commHMI.h"

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
#define NUM_VIRKEY				4

#define DIRE_UP					0
#define DIRE_DOWN				1
#define DIRE_LEFT				2
#define DIRE_RIGHT				3

#define KEY_NONE				0
const char KEY_123			=		-1;
const char KEY_backspace	=		-2;
const char KEY_GO			=		-3;
const char KEY_Return		=		-4;

const char virKey_uppercase_Code[] = { "<pic vx0=0 vy0=40 >16</>" };
const char virKey_uppercaseFocus_Code[] = { "<cpic vx0=0 vy0=0 >17</>" };

const char virKey_digit_Code[] = { "<pic vx0=0 vy0=40 >18</>" };
const char virKey_digitFocus_Code[] = { "<cpic vx0=0 vy0=0 >19</>" };





const 	char	uppKeyVal[4][10] = { \
	{ 'Q','W','E','R','T','Y','U','I','O','P'},\
	{ 'A','S','D','F','G','H','J','K','L',KEY_NONE},\
	{ KEY_123,'Z','X','C','V','B','N','M',KEY_backspace,KEY_NONE},\
	{ ' ', KEY_GO} \
};

const 	char	digitKeyVal[4][10] = { \
	{ '1','2','3','4','5','6','7','8','9','0'},\
	{ ':','@','#','$','%','\'','*','(',')',KEY_NONE},\
	{ KEY_Return,'+','-','?',',','.','/','=',KEY_backspace,KEY_NONE},\
	{ ' ', KEY_GO} \
};
typedef const char (*keyvalptr)[10];

keyvalptr  arr_p_keyval[2]= { uppKeyVal, digitKeyVal};
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------
typedef struct {
	uint16_t		pvx0, pvy0;
	uint16_t		pvx1, pvy1;
	char		val;
	uint8_t		rownum, colnum;
	uint8_t		none;
}virKeyInfo_t;

typedef struct {
	//当前按键四周的按键信息:上下左右
	virKeyInfo_t	vkeyUp;		
	virKeyInfo_t	vkeyDown;		
	virKeyInfo_t	vkeyLeft;		
	virKeyInfo_t	vkeyRight;		
	
	//中间的按键，即被选中的按键
	virKeyInfo_t	vkenCenter;						
}virKeyBlock_t;

typedef struct {
	
	
	void (*vkey_init)( virKeyBlock_t *p_vkb);
	void (*vkey_CalculateAroundKey[4])( virKeyBlock_t *p_vkb);

}virKeyOp_t;
	


	
//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
static keyboardHMI *singalKBHmi;
static char keybrdbuf[KEYBBUFLEN];

virKeyBlock_t	actKeyBlk;
sheet		shtInputSave;

//----vir key ---------------------
static void VK_Uppercase_init(virKeyBlock_t *p_vkb);
static void Upp_CalculateUp(virKeyBlock_t *p_vkb);
static void Upp_CalculateDown(virKeyBlock_t *p_vkb);
static void Upp_CalculateLeft(virKeyBlock_t *p_vkb);
static void Upp_CalculateRight(virKeyBlock_t *p_vkb);
static void SwitchVirKey( sheet	**arr_p_shtVkey, sheet	**arr_p_shtFocus, uint8_t *p_curvk);
virKeyOp_t uppVKO = { \
	VK_Uppercase_init, \
	{ Upp_CalculateUp, Upp_CalculateDown, Upp_CalculateLeft, Upp_CalculateRight} \
};



virKeyOp_t digitVKO = { \
	VK_Uppercase_init, \
	{ Upp_CalculateUp, Upp_CalculateDown, Upp_CalculateLeft, Upp_CalculateRight} \
};



virKeyOp_t *p_vko[NUM_VIRKEY] = { &uppVKO,  &digitVKO};
static sheet	*p_keyboardPic;
static sheet	*p_keyboardFocusPic;
static const char		(*p_keyval)[10];
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static int	Init_kbmHmi( HMI *self, void *arg);

static void KeyboardEnterCmdHdl( shtCmd *self, struct SHEET *p_sht, void *arg);
static void	KeyboardShow( HMI *self );
static void KBHide( HMI *self );
static void KBInitSheet( HMI *self );

static void	KeyboardHitHandle( HMI *self, char *s);

//----- vir key -------------
static void FocusKey_move( virKeyOp_t *p_keyop, virKeyBlock_t *p_vkb, int direction);
static void DrawFocus( char vkeytype, virKeyInfo_t *p_focus);
static void CleanFocus( char vkeytype, virKeyInfo_t *p_focus);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
keyboardHMI *GetkeyboardHMI(void)
{
	if( singalKBHmi == NULL)
	{
		singalKBHmi = keyboardHMI_new();
		
	}
	
	return singalKBHmi;
	
}


CTOR( keyboardHMI)
SUPER_CTOR( HMI);
FUNCTION_SETTING( HMI.init, Init_kbmHmi);
FUNCTION_SETTING( HMI.show, KeyboardShow);
FUNCTION_SETTING( HMI.initSheet, KBInitSheet);
FUNCTION_SETTING( HMI.hide, KBHide);
FUNCTION_SETTING( HMI.hitHandle, KeyboardHitHandle);


FUNCTION_SETTING( shtCmd.shtExcute, KeyboardEnterCmdHdl);
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
	
	
	cthis->p_shtVkey[0] = Sheet_alloc( p_shtctl);
	p_exp = ExpCreate( "pic");
	p_exp->inptSht( p_exp, (void *)virKey_uppercase_Code, cthis->p_shtVkey[0]) ;
	
	cthis->p_shtvKeyCursor[0] = Sheet_alloc( p_shtctl);
	p_exp = ExpCreate( "pic");
	p_exp->inptSht( p_exp, (void *)virKey_uppercaseFocus_Code, cthis->p_shtvKeyCursor[0]) ;
	
	cthis->p_shtVkey[1] = Sheet_alloc( p_shtctl);
	p_exp = ExpCreate( "pic");
	p_exp->inptSht( p_exp, (void *)virKey_digit_Code, cthis->p_shtVkey[1]) ;
	
	cthis->p_shtvKeyCursor[1] = Sheet_alloc( p_shtctl);
	p_exp = ExpCreate( "pic");
	p_exp->inptSht( p_exp, (void *)virKey_digitFocus_Code, cthis->p_shtvKeyCursor[1]) ;

	
	cthis->curVkey = 0;

	cthis->p_shtInput = NULL;
	
	return RET_OK;
}

static void KBInitSheet( HMI *self )
{
	keyboardHMI		*cthis = SUB_PTR( self, HMI, keyboardHMI);
	
		
//	Sheet_updown( cthis->p_shtVkey[ cthis->curVkey], 0);
	
	if( cthis->p_shtInput != NULL) {
		//保存旧的设置
		shtInputSave.area.x0 = cthis->p_shtInput->area.x0;
		shtInputSave.area.y0 = cthis->p_shtInput->area.y0;
		shtInputSave.area.x1 = cthis->p_shtInput->area.x1;
		shtInputSave.area.y1 = cthis->p_shtInput->area.y1;
		
		//设置新的坐标
		cthis->p_shtInput->area.x0 = 0;
		cthis->p_shtInput->area.y0 = 30 - ( shtInputSave.area.y1 - shtInputSave.area.y0);
		cthis->p_shtInput->area.x1 = shtInputSave.area.x1 - shtInputSave.area.x0;
		cthis->p_shtInput->area.y1 = 30;
		
		FormatSheetSub( cthis->p_shtInput);
		Sheet_updown( cthis->p_shtInput, 0);
	}
	
}

static void KBHide( HMI *self )
{
	keyboardHMI		*cthis = SUB_PTR( self, HMI, keyboardHMI);
	
	//恢复输入框的设置
	if( cthis->p_shtInput != NULL) {
		//保存旧的设置
		cthis->p_shtInput->area.x0 = shtInputSave.area.x0;
		cthis->p_shtInput->area.y0 = shtInputSave.area.y0;
		cthis->p_shtInput->area.x1 = shtInputSave.area.x1;
		cthis->p_shtInput->area.y1 = shtInputSave.area.y1;
				
		FormatSheetSub( cthis->p_shtInput);
		Sheet_updown( cthis->p_shtInput, -1);
		
		cthis->p_shtInput = NULL;
	}
//	Sheet_updown( cthis->p_shtVkey[  cthis->curVkey], -1);
}

static void	KeyboardShow( HMI *self )
{
	keyboardHMI		*cthis = SUB_PTR( self, HMI, keyboardHMI);
	I_dev_lcd 	*p_lcd;
	virKeyOp_t	*p_op;
	g_p_curHmi = self;
	Dev_open( LCD_DEVID, (void *)&p_lcd);
	p_lcd->Clear( CmmHmiAtt.bkc);
	Sheet_refresh( cthis->p_shtVkey[ cthis->curVkey]);
	

	cthis->curVkey = 1;		//切换到0 要先这么做
	SwitchVirKey( cthis->p_shtVkey, cthis->p_shtvKeyCursor, &cthis->curVkey);
	
	p_op = p_vko[ cthis->curVkey];
	p_op->vkey_init( &actKeyBlk); 

	CleanFocus( cthis->curVkey, &actKeyBlk.vkenCenter);		
	DrawFocus( 0, &actKeyBlk.vkenCenter);
	
	
}

static void CleanFocus( char vkeytype, virKeyInfo_t *p_focus)
{
	p_keyboardPic->p_gp->vdraw( p_keyboardPic->p_gp, &p_keyboardPic->cnt, &p_keyboardPic->area);
	
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
	
}
static void	KeyboardHitHandle( HMI *self, char *s)
{
	keyboardHMI		*cthis = SUB_PTR( self, HMI, keyboardHMI);
	virKeyOp_t	*p_op;
	
	
	
	if( !strcmp( s, HMIKEY_UP) )
	{
		//清除原来的选中效果
		
		CleanFocus( cthis->curVkey, &actKeyBlk.vkenCenter);
		
		//计算新的选中位置
		FocusKey_move( p_vko[ cthis->curVkey], &actKeyBlk, DIRE_UP);
		
		//绘制新的选中效果
		DrawFocus( cthis->curVkey, &actKeyBlk.vkenCenter);
		
	}
	else if( !strcmp( s, HMIKEY_DOWN) )
	{
		CleanFocus( cthis->curVkey, &actKeyBlk.vkenCenter);
		FocusKey_move( p_vko[ cthis->curVkey], &actKeyBlk, DIRE_DOWN);
		DrawFocus( cthis->curVkey, &actKeyBlk.vkenCenter);
	}
	else if( !strcmp( s, HMIKEY_LEFT))
	{
		CleanFocus( cthis->curVkey, &actKeyBlk.vkenCenter);
		FocusKey_move( p_vko[ cthis->curVkey], &actKeyBlk, DIRE_LEFT);
		DrawFocus( cthis->curVkey, &actKeyBlk.vkenCenter);
	}
	else if( !strcmp( s, HMIKEY_RIGHT))
	{
		CleanFocus( cthis->curVkey, &actKeyBlk.vkenCenter);
		FocusKey_move( p_vko[ cthis->curVkey], &actKeyBlk, DIRE_RIGHT);
		DrawFocus( cthis->curVkey, &actKeyBlk.vkenCenter);
	}
	
	if( !strcmp( s, HMIKEY_ESC))
	{
		self->switchBack(self);
	}
	if( !strcmp( s, HMIKEY_ENTER))
	{
		if( actKeyBlk.vkenCenter.val == KEY_123 || actKeyBlk.vkenCenter.val == KEY_Return) {
			SwitchVirKey( cthis->p_shtVkey, cthis->p_shtvKeyCursor, &cthis->curVkey);
			
			p_op = p_vko[ cthis->curVkey];
			p_op->vkey_init( &actKeyBlk);  

			CleanFocus( cthis->curVkey, &actKeyBlk.vkenCenter);			
			DrawFocus( 0, &actKeyBlk.vkenCenter);
		}
	}
	
	
}

static void KeyboardEnterCmdHdl( shtCmd *self, struct SHEET *p_sht, void *arg)
{
	keyboardHMI	*cthis = SUB_PTR( self, shtCmd, keyboardHMI);
	HMI			*selfHmi = SUPER_PTR( cthis, HMI);
	HMI		*srcHmi = ( HMI *)arg;
	
	cthis->p_shtInput = p_sht;
	srcHmi->switchHMI( srcHmi, selfHmi);
	
}



/*********** vir key *****************************************/
static void VKeyClone( virKeyInfo_t *p_dstVkey, virKeyInfo_t *p_srcVkey)
{
	p_dstVkey->pvx0 = p_srcVkey->pvx0;
	p_dstVkey->pvx1 = p_srcVkey->pvx1;
	p_dstVkey->pvy0 = p_srcVkey->pvy0;
	p_dstVkey->pvy1 = p_srcVkey->pvy1;
	p_dstVkey->val = p_srcVkey->val;
	p_dstVkey->rownum = p_srcVkey->rownum;
	p_dstVkey->colnum = p_srcVkey->colnum;
	
}

static void VKeyClean( virKeyInfo_t *p_vkey)
{
	p_vkey->pvx0 = 0;
	p_vkey->pvx1 = 0;
	p_vkey->pvy0 = 0;
	p_vkey->pvy1 = 0;
	p_vkey->rownum = 0;
	p_vkey->colnum = 0;
}

static void SwitchVirKey( sheet	**arr_p_shtVkey, sheet	**arr_p_shtFocus, uint8_t *p_curvk)
{
	uint8_t nextvk;
	if( *p_curvk == 0) {
		nextvk = 1;
	} else {
		nextvk = 0;
	}
	p_keyboardPic = arr_p_shtVkey[ nextvk];
	p_keyboardFocusPic  = arr_p_shtFocus[ nextvk];
	p_keyval = arr_p_keyval[ nextvk];
	*p_curvk = nextvk;
		
}

static void FocusKey_move( virKeyOp_t *p_keyop, virKeyBlock_t *p_vkb, int direction)
{
	
	switch( direction) 
	{
		case DIRE_UP:
			VKeyClone( &p_vkb->vkenCenter, &p_vkb->vkeyUp);
			break;
		case DIRE_DOWN:
			VKeyClone( &p_vkb->vkenCenter, &p_vkb->vkeyDown);
			break;
		case DIRE_LEFT:
			VKeyClone( &p_vkb->vkenCenter, &p_vkb->vkeyLeft);
			break;
		case DIRE_RIGHT:
			VKeyClone( &p_vkb->vkenCenter, &p_vkb->vkeyRight);
			break;
		default:
			goto exit;
		
	}
	p_keyop->vkey_CalculateAroundKey[ DIRE_UP]( p_vkb);
	p_keyop->vkey_CalculateAroundKey[ DIRE_DOWN]( p_vkb);
	p_keyop->vkey_CalculateAroundKey[ DIRE_LEFT]( p_vkb);
	p_keyop->vkey_CalculateAroundKey[ DIRE_RIGHT]( p_vkb);
	exit:
	return;
}

static char GetVKeyVal( uint8_t row, uint8_t col)
{
	return p_keyval[row][col];
	
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
static void Upp_CalculateUp(virKeyBlock_t *p_vkb)
{
	virKeyInfo_t *p_vkey = &p_vkb->vkeyUp;
	
	if( p_vkb->vkenCenter.rownum) {
		p_vkey->rownum = p_vkb->vkenCenter.rownum - 1;
	} else {
		p_vkey->rownum = 3;
	}
	
	p_vkey->colnum = p_vkb->vkenCenter.colnum;
	
	//判断该位置有无按键，如果没有，就找到这一行离这个位置最近的按键
	while( uppKeyVal[ p_vkey->rownum][ p_vkey->colnum] == KEY_NONE) {
		
		if( p_vkey->colnum)
			p_vkey->colnum --;
	}
	p_vkey->val = GetVKeyVal( p_vkey->rownum, p_vkey->colnum); 
	
	Upp_crdnInPic( p_vkey);
	
}
static void Upp_CalculateDown(virKeyBlock_t *p_vkb)
{
	virKeyInfo_t *p_vkey = &p_vkb->vkeyDown;
	
	if( p_vkb->vkenCenter.rownum < 3) {
		p_vkey->rownum = p_vkb->vkenCenter.rownum + 1;
	} else {
		p_vkey->rownum = 0;
	}
	
	p_vkey->colnum = p_vkb->vkenCenter.colnum;
	
	//判断该位置有无按键，如果没有，就找到这一行离这个位置最近的按键
	while( GetVKeyVal( p_vkey->rownum, p_vkey->colnum) == KEY_NONE) {
		if( p_vkey->colnum)
			p_vkey->colnum --;
	}
	p_vkey->val = GetVKeyVal( p_vkey->rownum, p_vkey->colnum); 
	
	Upp_crdnInPic( p_vkey);
	
}

static void Upp_CalculateLeft(virKeyBlock_t *p_vkb)
{
	virKeyInfo_t *p_vkey = &p_vkb->vkeyLeft;
	int i = 0;
	
	p_vkey->rownum = p_vkb->vkenCenter.rownum;
	if( p_vkb->vkenCenter.colnum) {
		p_vkey->colnum = p_vkb->vkenCenter.colnum - 1;
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

static void Upp_CalculateRight(virKeyBlock_t *p_vkb)
{
	virKeyInfo_t *p_vkey = &p_vkb->vkeyRight;
	int i = 0;
	
	p_vkey->rownum = p_vkb->vkenCenter.rownum;
	if( p_vkb->vkenCenter.colnum < 9) {
		p_vkey->colnum = p_vkb->vkenCenter.colnum + 1;
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
	Upp_CalculateUp( p_vkb);
	Upp_CalculateDown( p_vkb);
	Upp_CalculateLeft( p_vkb);
	Upp_CalculateRight( p_vkb);
	
	//
	
}











