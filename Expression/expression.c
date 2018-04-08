

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
x/yali :  aligning 对齐方式

cg	:	columnGap 列间距
ls	:	lineSpacing	行间距
bx/by : 图像在x/y轴上的长度
vx0,vy0:	在屏幕上的起始坐标
grap: 		矩形中心与边框的间距
m		: 背景图片编号

gr	:	grid
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
const Except_T Exp_Failed = { "[expression]" };

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

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
//特效的优先级高
int Set_shtContextAtt( char *p_att, sheet *p_sht)
{
	
//	char tmpbuf[8] = {0};
	p_sht->cnt.colour = String2Clr( p_att);
	
	p_sht->cnt.bkc  = String2CntEff( p_att);
	if( p_sht->cnt.bkc == 0xff) {
		p_sht->cnt.bkc  = String2Bkc( p_att);
	} else {
		p_sht->cnt.effects = GP_SET_EFF( p_sht->cnt.effects, EFF_BKPIC) ;
	}
	
	p_sht->cnt.font = String2Font( p_att);
	
//	if( GetKeyVal( p_att, "mdl", tmpbuf, 8))
//	{
//		p_sht->p_mdl = Create_model(tmpbuf); 
//		
//		
//	}
//	else {
//		p_sht->p_mdl = NULL;
//		
//	}
	
//	if( GetKeyVal( p_att, "aux", tmpbuf, 8))
//	{
//		p_sht->cnt.mdl_aux = atoi(tmpbuf); 
//		
//		
//	}
//	else {
//		p_sht->cnt.mdl_aux = 0xff;
//		
//	}
	
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
	if( GetKeyVal( p_att, "vx1", tmpbuf, 4))
	{
		p_sht->area.x1 = atoi( tmpbuf);
	}
	if( GetKeyVal( p_att, "vy0", tmpbuf, 4))
	{
		p_sht->area.y0 = atoi( tmpbuf);
	}
	if( GetKeyVal( p_att, "vy1", tmpbuf, 4))
	{
		p_sht->area.y1 = atoi( tmpbuf);
	}
	
	if( GetKeyVal( p_att, "grap", tmpbuf, 4))
	{
		p_sht->area.grap = atoi( tmpbuf);
		
	}
	else
	{
		p_sht->area.grap  = 0;
	}
	
	if( GetKeyVal( p_att, "cg", tmpbuf, 4))
	{
//		p_sht->subAtt.subColGrap = atoi( tmpbuf);
		p_sht->area.offset_x = atoi( tmpbuf);
	}
	else
	{
//		p_sht->subAtt.subColGrap = 0;
		p_sht->area.offset_x = 0;
	}
	
	
	if( GetKeyVal( p_att, "rg", tmpbuf, 4))
	{
//		p_sht->subAtt.subRowGrap  = atoi( tmpbuf);
		p_sht->area.offset_y = atoi( tmpbuf);
	}
	else
	{
//		p_sht->subAtt.subRowGrap = 0;
		p_sht->area.offset_y = 0;
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
	
	if( GetKeyVal( p_att, "xn", tmpbuf, 4))
	{
		p_sht->area.xn = atoi( tmpbuf);
	}
	else
	{
		p_sht->area.xn = 0;
	}
	if( GetKeyVal( p_att, "yn", tmpbuf, 4))
	{
		p_sht->area.yn = atoi( tmpbuf);
	}
	else
	{
		p_sht->area.yn = 0;
	}
	if( GetKeyVal( p_att, "n", tmpbuf, 4))
	{
		p_sht->area.n = atoi( tmpbuf);
	}
	else
	{
		p_sht->area.n = 0;
	}
	
	return ret;
}

ABS_CTOR( Expr)


END_ABS_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{




