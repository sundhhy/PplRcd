#include "expression.h"
#include <string.h>
/*
expression :: = 

variable = [ A-Z, a-z]*



*/

/*缩写
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

pmx/y : permille x/y轴长度占屏幕的x/y轴尺寸的千分比
*/


//每种表达式在解析时使用，但是用完就必须恢复为0
char expTempBUf[TEMPBUF_LEN];


static void SetVar( Expr *self, char *variable)
{
	//截断
	if( strlen( variable) > VAR_LEN)
		variable[ VAR_LEN] = 0;
		
	strcpy( self->variable, variable);
}

//static void SetFather( Expr *self, Expr *fa)
//{
//	
//	self->father = fa;
//}
static void SetCtion( Expr *self, Composition *ct)
{
	self->ction = ct;
	
}













ABS_CTOR( Expr)
FUNCTION_SETTING( setVar, SetVar);
FUNCTION_SETTING( setCtion, SetCtion);
//FUNCTION_SETTING( setFather, SetFather);
//FUNCTION_SETTING( getBgc, ExpGetBackgroudColor);



//FUNCTION_SETTING( str2colour, ExpStr2colour);
//FUNCTION_SETTING( str2font, ExpStr2font);

END_ABS_CTOR





