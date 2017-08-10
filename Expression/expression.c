

/*
expression :: = 

variable = [ A-Z, a-z]*



*/




/*缩写
警告！ name=val 的等号前后不允许有空格！！！！
clr : colour
f : font
bu : button
UDT缩写
pa	: 	paragraph段落
rct :	rectangle 矩形

scr : 	Screen region 屏幕区域
sel	:	selection 	选中区域
ali :  aligning 对齐方式

cg	:	columnGap 列间距
ls	:	lineSpacing	行间距

gr	:	grid
bx/by : 图像在x/y轴上的长度

bndx1/y1/x2/y2 : bonduary x1/y1/x2/y2
spr: separator分隔符

pmx/y : permille x/y轴长度占屏幕的x/y轴尺寸的千分比
*/

/*

子图元可以继承父图元的属性，如颜色，背景色，字体
如果子图元自己指定了属性，则优先使用自己制定的
如果自己没有指定，就先找父图元的属性；如果父图元没有提供，就使用系统默认的属性

*/



















//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//
#include "expression.h"
#include <string.h>
//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
const Except_T Exp_Failed = { "Exp Failed" };

//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------
//每种表达式在解析时使用，但是用完就必须恢复为0
char expTempBUf[TEMPBUF_LEN];



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

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static void SetVar( Expr *self, char *variable);
static void SetCtion( Expr *self, Composition *ct);

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

int Set_shtContextAtt( char *p_att, sheet *p_sht)
{
	
	p_sht->cnt.colour = String2Clr( p_att);
	p_sht->cnt.bkc  = String2Bkc( p_att);
	p_sht->cnt.font = String2Font( p_att);
	
	return SET_ATT_OK;
}

int Set_shtAreaAtt( char *p_att, sheet *p_sht)
{
	int 	ret = SET_ATT_OK;
	
	char 	tmpbuf[4] = {0};


	GetKeyVal( p_att, "xali", tmpbuf, 4);
	p_sht->area.alix = String2Align( tmpbuf);
	GetKeyVal( p_att, "yali", tmpbuf, 4);
	p_sht->area.aliy = String2Align( tmpbuf);
	
	if( GetKeyVal( p_att, "vx0", tmpbuf, 4))
	{
		p_sht->area.x0 = atoi( tmpbuf);
	}
	if( GetKeyVal( p_att, "vy0", tmpbuf, 4))
	{
		p_sht->area.y0 = atoi( tmpbuf);
	}
	
	if( GetKeyVal( p_att, "bx", tmpbuf, 4))
	{
		p_sht->bxsize = atoi( tmpbuf);
	}
	else
	{
		ret |= SET_ATT_BSIZEFAIL;
	}
	if( GetKeyVal( p_att, "by", tmpbuf, 4))
	{
		p_sht->bysize = atoi( tmpbuf);
	}
	else
	{
		ret |= SET_ATT_BSIZEFAIL;
	}
	
	
	return ret;
}

ABS_CTOR( Expr)
FUNCTION_SETTING( setVar, SetVar);
FUNCTION_SETTING( setCtion, SetCtion);
//FUNCTION_SETTING( setFather, SetFather);
//FUNCTION_SETTING( getBgc, ExpGetBackgroudColor);



//FUNCTION_SETTING( str2colour, ExpStr2colour);
//FUNCTION_SETTING( str2font, ExpStr2font);

END_ABS_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{
static void SetVar( Expr *self, char *variable)
{
	//截断
	if( strlen( variable) > VAR_LEN)
		variable[ VAR_LEN] = 0;
		
	strcpy( self->variable, variable);
}

static void SetCtion( Expr *self, Composition *ct)
{
	self->ction = ct;
	
}



