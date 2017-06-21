#include "Gh_Rectangle.h"
#include "device.h"



static GhRectangle *signalGhRectangle;


GhRectangle *Get_GhRectangle(void)
{
	Glyph *gh;
	
	if( signalGhRectangle == NULL)
	{
		signalGhRectangle = GhRectangle_new();
		gh = ( Glyph *)signalGhRectangle;

		gh->setFont( gh, DEF_FONT);
		gh->setClu( gh, DEF_COLOUR);
		
		
	}
	return signalGhRectangle;
}














static void GhRec_Draw( Glyph *self, dspContent_t *cnt, dspArea_t *area)
{
	I_dev_lcd *lcd;
//	short x2, y2;
//	GhRectangle *cthis = ( GhRectangle *)self;
	
	Dev_open( LCD_DEVID, (void *)&lcd);
	if( cnt->bkc == ERR_COLOUR)
	{
		lcd->Box( area->useArea.x1, area->useArea.y1, area->useArea.x2, area->useArea.y2, 0, cnt->colour);
	}
	else
	{
		lcd->Box( area->useArea.x1, area->useArea.y1, area->useArea.x2, area->useArea.y2, 1, cnt->bkc);
		
	}
	
}










CTOR( GhRectangle)
SUPER_CTOR( Glyph);
FUNCTION_SETTING( Glyph.draw, GhRec_Draw);
//FUNCTION_SETTING( Glyph.flush, GhTxt_Flush);


//FUNCTION_SETTING( Glyph.getSize, GhTxt_GetSize);

//FUNCTION_SETTING( Glyph.getWidth, GhTxt_GetWidth);
//FUNCTION_SETTING( Glyph.getHeight, GhTxt_GetHeight);
//FUNCTION_SETTING( Glyph.getNum, GhTxt_GetNum);

END_CTOR
