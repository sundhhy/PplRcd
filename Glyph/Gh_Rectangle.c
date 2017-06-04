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




//static int SetFont( Glyph *self, int font)
//{
//	GhRectangle *cthis = ( GhRectangle *)self;
//	
//	cthis->width = font & 0xffff;
//	cthis->height = ( font & 0xffff0000) >> 16;
//	
//	return RET_OK;
//	
//}




static int GhRec_Clean( Glyph *self)
{
//	GhRectangle *cthis = ( GhRectangle *)self;
	return RET_OK;;
	
}

//static void GhRec_Insert( Glyph *self, void *context, int len)
//{
//	
//	
//}
//static int GhRec_SetWidth( Glyph *self, uint16_t wd)
//{
//	GhRectangle *cthis = ( GhRectangle *)self;
//	cthis->width = wd;
//	return RET_OK;
//	
//}
//static int GhRec_SetHeight( Glyph *self, uint16_t he)
//{
//	GhRectangle *cthis = ( GhRectangle *)self;
//	cthis->height = he;
//	return RET_OK;
//	
//}

//static void GhRec_Draw( Glyph *self, int x, int y, int len)
//{
//	I_dev_lcd *lcd;
//	short x2, y2;
//	GhRectangle *cthis = ( GhRectangle *)self;
//	

//	
//	x2 = x + cthis->width;
//	y2 = y + cthis->height;

//	
//	Dev_open( LCD_DEVID, (void *)&lcd);
//	
//	lcd->Box( x, y, x2, y2, 0, self->colour);
	
//}
//static void GhTxt_Flush( Glyph *self, int x, int y)
//{
//	I_dev_lcd *lcd;
//	
//	GhTxt *cthis = ( GhTxt *)self;
//	char *p;
//	int txtLen = CQ_GetPtr( &cthis->cq, &p, CQ_LENALL);
//	
//	Dev_open( LCD_DEVID, (void *)&lcd);
//	
////	lcd->Clear();
//	lcd->wrString( p, txtLen, x, y, self->font, self->colour);
////	lcd->wrString( "text", 4, x, y, self->font, self->colour);
//	
//}
//static int GhTxt_GetSize(Glyph *self, int font, uint16_t *x, uint16_t *y)
//{
//	I_dev_lcd *lcd;
//	
//	Dev_open( LCD_DEVID, (void *)&lcd);
//	
//	return lcd->getStrSize( font, x, y);
//	
//}
//static int GhTxt_GetWidth(Glyph *self)
//{
//	GhTxt *cthis = ( GhTxt *)self;
//	uint16_t	width = 0;
//	I_dev_lcd *lcd;
//	
//	Dev_open( LCD_DEVID, (void *)&lcd);
//	
//	lcd->getStrSize( self->font, &width, NULL);
//	
//	return width;
//	
//}
//static int GhTxt_GetHeight(Glyph *self)
//{
//	GhTxt *cthis = ( GhTxt *)self;
//	uint16_t	height = 0;
//	I_dev_lcd *lcd;
//	
//	Dev_open( LCD_DEVID, (void *)&lcd);
//	
//	lcd->getStrSize( self->font, NULL, &height);
//	
//	return height;
//}

//static int GhTxt_GetNum(Glyph *self)
//{
//	GhTxt *cthis = ( GhTxt *)self;
//	
//	int txtLen = CQ_Len( &cthis->cq);
//	
//	
//	
//	return txtLen;
//}






CTOR( GhRectangle)
SUPER_CTOR( Glyph);
//FUNCTION_SETTING( Glyph.setFont, SetFont);
//FUNCTION_SETTING( Glyph.setWidth, GhRec_SetWidth);
//FUNCTION_SETTING( Glyph.setHeight, GhRec_SetHeight);
FUNCTION_SETTING( Glyph.clean, GhRec_Clean);
//FUNCTION_SETTING( Glyph.insert, GhRec_Insert);
//FUNCTION_SETTING( Glyph.draw, GhRec_Draw);
//FUNCTION_SETTING( Glyph.flush, GhTxt_Flush);


//FUNCTION_SETTING( Glyph.getSize, GhTxt_GetSize);

//FUNCTION_SETTING( Glyph.getWidth, GhTxt_GetWidth);
//FUNCTION_SETTING( Glyph.getHeight, GhTxt_GetHeight);
//FUNCTION_SETTING( Glyph.getNum, GhTxt_GetNum);

END_CTOR
