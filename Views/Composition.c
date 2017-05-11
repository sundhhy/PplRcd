#include "Composition.h"
#include "Compositor.h"
#include "device.h"

#include "Gh_txt.h"

#include "basis/sdhDebug.h"
#include "basis/sdhError.h"

Composition *signalComposition;

Composition *Get_Composition(void)
{
	
	if( signalComposition == NULL)
	{
		signalComposition = Composition_new();
		
	}
	
	return signalComposition;
}

static void SetCtor( Composition *self, void *ctor)
{
	self->ctor = ctor;
	
}
static int Ction_Clean( Composition *self)
{
	GhTxt *mytxt = Get_GhTxt();
	Glyph	*myGp = (Glyph *)mytxt;
	I_dev_lcd *lcd;
	
	Dev_open( LCD_DEVID, (void *)&lcd);
//	List_free( &self->Lgh);
	
	self->x = 0;
	self->y = 0;
	lcd->Clear();
	myGp->clean(myGp);
	
	return RET_OK;
}

static int Ction_Insert( Composition *self, Glyph *gh)
{
	
	
	Compositor *thisCtor = ( Compositor *)self->ctor;
	

	
	
	self->lcdWidth = 320;
	self->lcdHeight = 240;
	
	
	thisCtor->compose( self, gh);
	
	
	
	return RET_OK;
}

int Ction_AddRow( Composition *self)
{
	//检查是否超出屏幕范围
	
	//将光标指向下一行起始位置
	self->y += self->ghHeight;
	self->x = 0;
	
	//如果列坐标超过屏幕的高度，撤销并返回错误
	if( self->y > self->lcdHeight)
	{
		self->y -= self->ghHeight;
		return ERR_FAIL;
	}
	return RET_OK;
}

CTOR( Composition)


FUNCTION_SETTING( setCtor, SetCtor);

FUNCTION_SETTING( clean, Ction_Clean);
FUNCTION_SETTING( insert, Ction_Insert);
FUNCTION_SETTING( addRow, Ction_AddRow);


END_CTOR

