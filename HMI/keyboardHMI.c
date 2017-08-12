#include "keyboardHMI.h"
#include "commHMI.h"

#include <string.h>
#include "ExpFactory.h"



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
#define KEYBBUFLEN		16
#define NUM_VIRKEY		4

#define DIRE_UP				0
#define DIRE_DOWN			1
#define DIRE_LEFT			2
#define DIRE_RIGHT		3

const char virKey_uppercase_Code[] = { "<pic vx0=0 vy0=40 >15</>" };
const char virKey_lowercase_Code[] = { "<pic vx0=0 vy0=0 >15</>" };
const char virKey_digit_Code[] = { "<pic vx0=0 vy0=0 >15</>" };
const char virKey_specialsymbols_Code[] = { "<pic vx0=0 vy0=0 >15</>" };
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------
typedef struct {
	uint8_t		vx0, vy0;
	uint8_t		vx1, vy1;
	uint8_t		val;
	uint8_t		none[3];
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
	void (*vkey_CalculateAroundKey)[4]( virKeyBlock_t *p_vkb);
//	void (*vkey_move)( virKeyBlock_t *p_vkb, int direction);
//	int (*vkey_getVal)( virKeyInfo_t *p_vkey);
}virKeyOp_t;
	


	
//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
static keyboardHMI *singalKBHmi;
static char keybrdbuf[KEYBBUFLEN];

virKeyBlock_t	actKeyBlk;
sheet		shtInputSave;

//----vir key ---------------------
static void VK_Uppercase_init( virKeyBlock_t *p_vkb);
static void Upp_CalculateUp( *p_vkb);
static void Upp_CalculateDown( *p_vkb);
static void Upp_CalculateLeft( *p_vkb);
static void Upp_CalculateRight( *p_vkb);

virKeyOp_t uppVKO = { \
	VK_Uppercase_init, \
	{ Upp_CalculateUp, Upp_CalculateDown, Upp_CalculateLeft, Upp_CalculateRight} \
};

virKeyOp_t lowerVKO = { \
	VK_Uppercase_init, \
	{ Upp_CalculateUp, Upp_CalculateDown, Upp_CalculateLeft, Upp_CalculateRight} \
};

virKeyOp_t digitVKO = { \
	VK_Uppercase_init, \
	{ Upp_CalculateUp, Upp_CalculateDown, Upp_CalculateLeft, Upp_CalculateRight} \
};

virKeyOp_t specVKO = { \
	VK_Uppercase_init, \
	{ Upp_CalculateUp, Upp_CalculateDown, Upp_CalculateLeft, Upp_CalculateRight} \
};

virKeyOp_t *p_vko[4] = { &uppVKO, &lowerVKO, &digitVKO, &specVKO};
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
	
	cthis->p_shtVkey[1] = Sheet_alloc( p_shtctl);
	p_exp = ExpCreate( "pic");
	p_exp->inptSht( p_exp, (void *)virKey_lowercase_Code, cthis->p_shtVkey[1]) ;
	
	cthis->p_shtVkey[2] = Sheet_alloc( p_shtctl);
	p_exp = ExpCreate( "pic");
	p_exp->inptSht( p_exp, (void *)virKey_digit_Code, cthis->p_shtVkey[2]) ;
	
	cthis->p_shtVkey[3] = Sheet_alloc( p_shtctl);
	p_exp = ExpCreate( "pic");
	p_exp->inptSht( p_exp, (void *)virKey_specialsymbols_Code, cthis->p_shtVkey[3]) ;
	
	cthis->curVkey = 0;

	
	
	return RET_OK;
}

static void KBInitSheet( HMI *self )
{
	keyboardHMI		*cthis = SUB_PTR( self, HMI, keyboardHMI);
	
		
	Sheet_updown( cthis->p_shtVkey[ cthis->curVkey], 0);
	
	if( self->p_shtInput != NULL) {
		//保存旧的设置
		shtInputSave.area.x0 = self->p_shtInput->area.x0;
		shtInputSave.area.y0 = self->p_shtInput->area.y0;
		shtInputSave.area.x1 = self->p_shtInput->area.x1;
		shtInputSave.area.y1 = self->p_shtInput->area.y1;
		
		//设置新的坐标
//		self->p_shtInput->area.x0 = cthis->p_shtVkey[ cthis->curVkey]->
		
		FormatSheetSub( self->p_shtInput);
		Sheet_updown( cthis->p_shtVkey[ cthis->curVkey], 1);
	}
	
}

static void KBHide( HMI *self )
{
	keyboardHMI		*cthis = SUB_PTR( self, HMI, keyboardHMI);
	
	//恢复输入框的设置
	if( self->p_shtInput != NULL) {
		//保存旧的设置
		self->p_shtInput->area.x0 = shtInputSave.area.x0;
		self->p_shtInput->area.y0 = shtInputSave.area.y0;
		self->p_shtInput->area.x1 = shtInputSave.area.x1;
		self->p_shtInput->area.y1 = shtInputSave.area.y1;
				
		FormatSheetSub( self->p_shtInput);
		Sheet_updown( cthis->p_shtVkey[ cthis->curVkey], -1);
	}
	Sheet_updown( cthis->p_shtVkey[  cthis->curVkey], -1);
}

static void	KeyboardShow( HMI *self )
{
	keyboardHMI		*cthis = SUB_PTR( self, HMI, keyboardHMI);
	I_dev_lcd 	*p_lcd;
	g_p_curHmi = self;
	Dev_open( LCD_DEVID, (void *)&p_lcd);
	p_lcd->Clear( CmmHmiAtt.bkc);
	Sheet_refresh( cthis->p_shtVkey[ cthis->curVkey]);
}

static void CleanFocus( char vkeytype, virKeyInfo_t *p_focus)
{
	
	
}

static void DrawFocus( char vkeytype, virKeyInfo_t *p_focus)
{
	
	
}
static void	KeyboardHitHandle( HMI *self, char *s)
{
	keyboardHMI		*cthis = SUB_PTR( self, HMI, keyboardHMI);
	shtCmd		*p_cmd;
	uint8_t		fouseRow = cthis->fouseRow;
	uint8_t		fouseCol = cthis->fouseCol;
	char			chgFouse = 0;
	
	
	
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
		FocusKey_move( p_vko[ cthis->curVkey], &actKeyBlk, DIRE_Down);
		DrawFocus( cthis->curVkey, &actKeyBlk.vkenCenter);
	}
	else if( !strcmp( s, HMIKEY_LEFT))
	{
		CleanFocus( cthis->curVkey, &actKeyBlk.vkenCenter);
		FocusKey_move( p_vko[ cthis->curVkey], &actKeyBlk, DIRE_Left);
		DrawFocus( cthis->curVkey, &actKeyBlk.vkenCenter);
	}
	else if( !strcmp( s, HMIKEY_RIGHT))
	{
		CleanFocus( cthis->curVkey, &actKeyBlk.vkenCenter);
		FocusKey_move( p_vko[ cthis->curVkey], &actKeyBlk, DIRE_Down);
		DrawFocus( cthis->curVkey, &actKeyBlk.vkenCenter);
	}
	
	if( !strcmp( s, HMIKEY_ESC))
	{
		self->switchBack(self);
	}
}

static void KeyboardEnterCmdHdl( shtCmd *self, struct SHEET *p_sht, void *arg)
{
	keyboardHMI	*cthis = SUB_PTR( self, shtCmd, keyboardHMI);
	HMI			*selfHmi = SUPER_PTR( cthis, HMI);
	HMI		*srcHmi = ( HMI *)arg;
	
	cthis->p_shtInput = p_sht;
	srcHmi->switchHMI( srcHmi, selfHmi);
//	selfHmi->show( selfHmi);
	
}



/*********** vir key *****************************************/
static VKeyClone( virKeyInfo_t *p_dstVkey, virKeyInfo_t *p_srcVkey)
{
	p_dstVkey->vx0 = p_srcVkey->vx0;
	p_dstVkey->vx1 = p_srcVkey->vx1;
	p_dstVkey->vy0 = p_srcVkey->vy0;
	p_dstVkey->vy1 = p_srcVkey->vy1;
	p_dstVkey->val = p_srcVkey->val;
	
}

static VKeyClean( virKeyInfo_t *p_vkey)
{
	p_vkey->vx0 = 0;
	p_vkey->vx1 = 0;
	p_vkey->vy0 = 0;
	p_vkey->vy1 = 0;
	
}



static void FocusKey_move( virKeyOp_t *p_keyop, virKeyBlock_t *p_vkb, int direction)
{
	
	switch( direction) 
	{
		case DIRE_UP:
			VKeyClone( &p_vkb->vkeyDown, &p_vkb->vkenCenter);
			VKeyClone( &p_vkb->vkenCenter, &p_vkb->vkeyUp);
		
			p_keyop->vkey_CalculateAroundKey[ DIRE_UP]( p_vkb);
			p_keyop->vkey_CalculateAroundKey[ DIRE_LEFT]( p_vkb);
			p_keyop->vkey_CalculateAroundKey[ DIRE_RIGHT]( p_vkb);
			break;
		case DIRE_DOWN:
			VKeyClone( &p_vkb->Up, &p_vkb->vkenCenter);
			VKeyClone( &p_vkb->vkenCenter, &p_vkb->vkeyDown);
		
			p_keyop->vkey_CalculateAroundKey[ DIRE_DOWN]( p_vkb);
			p_keyop->vkey_CalculateAroundKey[ DIRE_LEFT]( p_vkb);
			p_keyop->vkey_CalculateAroundKey[ DIRE_RIGHT]( p_vkb);
		
			break;
		case DIRE_LEFT:
			VKeyClone( &p_vkb->vkeyRight, &p_vkb->vkenCenter);
			VKeyClone( &p_vkb->vkenCenter, &p_vkb->vkeyLeft);
		
		
			p_keyop->vkey_CalculateAroundKey[ DIRE_LEFT]( p_vkb);
			p_keyop->vkey_CalculateAroundKey[ DIRE_UP]( p_vkb);
			p_keyop->vkey_CalculateAroundKey[ DIRE_DOWN]( p_vkb);
		
			break;
		case DIRE_RIGHT:
			VKeyClone( &p_vkb->vkeyLeft, &p_vkb->vkenCenter);
			VKeyClone( &p_vkb->vkenCenter, &p_vkb->vkeyRight);
		
			p_keyop->vkey_CalculateAroundKey[ DIRE_RIGHT]( p_vkb);
			p_keyop->vkey_CalculateAroundKey[ DIRE_UP]( p_vkb);
			p_keyop->vkey_CalculateAroundKey[ DIRE_DOWN]( p_vkb);
		
			break;
		
	}
	
}

//-----------  大写字母键盘 --------------------------------

//根据keyinfo中的坐标来获取值
static void Upp_GetVal( virKeyInfo_t *p_vkey)
{
	
	
}


//根据中心按键来计算上下左右的按键的信息
static void Upp_CalculateUp( *p_vkb)
{
	
	
}
static void Upp_CalculateDown( *p_vkb)
{
	
	
}
static void Upp_CalculateLeft( *p_vkb)
{
	
	
}
static void Upp_CalculateRight( *p_vkb)
{
	
	
}
static void VK_Uppercase_init( virKeyBlock_t *p_vkb)
{
	
	
	VKeyClean( p_vkb->vkenCenter);
	Upp_GetVal( &p_vkb->vkenCenter);
	
	//初始化四周的按键的初始位置
	Upp_CalculateUp( p_vkb);
	Upp_CalculateDown( p_vkb);
	Upp_CalculateLeft( p_vkb);
	Upp_CalculateRight( p_vkb);
	
}











