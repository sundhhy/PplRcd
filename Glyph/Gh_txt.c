#include "Gh_txt.h"
#include "device.h"

#define GHTXTBUFLEN		64


static GhTxt *signalGhTxt;


GhTxt *Get_GhTxt(void)
{
	Glyph *gh;
	
	if( signalGhTxt == NULL)
	{
		signalGhTxt = GhTxt_new();
		gh = ( Glyph *)signalGhTxt;
		signalGhTxt->buf = malloc( GHTXTBUFLEN);
		signalGhTxt->cq_buf = malloc( GHTXTBUFLEN);
		gh->setFont( gh, DEF_FONT);
		gh->setClu( gh, DEF_COLOUR);
		CQ_Init( &signalGhTxt->cq, signalGhTxt->cq_buf, GHTXTBUFLEN);
		
	}
	

	return signalGhTxt;
}









static int GhTxt_Clean( Glyph *self)
{
	GhTxt *cthis = ( GhTxt *)self;
	return CQ_Clean( &cthis->cq);
	
}

static void GhTxt_Insert( Glyph *self, void *context, int len)
{
	GhTxt *cthis = ( GhTxt *)self;
	CQ_Write( &cthis->cq, context, len);
	
}

static void GhTxt_Draw( Glyph *self, int x, int y, int len)
{
	I_dev_lcd *lcd;
	
	GhTxt *cthis = ( GhTxt *)self;
	char *p = cthis->buf;
	int txtLen = 0;
	

	txtLen = CQ_Read( &cthis->cq, p, len);
//	txtLen = CQ_GetPtr( &cthis->cq, &p, len);
	if( txtLen == 0)
		return;
	
	Dev_open( LCD_DEVID, (void *)&lcd);
	
//	lcd->Clear();
	lcd->wrString( p, txtLen, x, y, self->font, self->colour);
	
}
static void GhTxt_Flush( Glyph *self, int x, int y)
{
	I_dev_lcd *lcd;
	
	GhTxt *cthis = ( GhTxt *)self;
	char *p;
	int txtLen = CQ_GetPtr( &cthis->cq, &p, CQ_LENALL);
	
	Dev_open( LCD_DEVID, (void *)&lcd);
	
//	lcd->Clear();
	lcd->wrString( p, txtLen, x, y, self->font, self->colour);
//	lcd->wrString( "text", 4, x, y, self->font, self->colour);
	
}
static int GhTxt_GetSize(Glyph *self, int font, uint16_t *x, uint16_t *y)
{
	I_dev_lcd *lcd;
	
	Dev_open( LCD_DEVID, (void *)&lcd);
	
	return lcd->getStrSize( font, x, y);
	
}
static int GhTxt_GetWidth(Glyph *self)
{
//	GhTxt *cthis = ( GhTxt *)self;
	uint16_t	width = 0;
	I_dev_lcd *lcd;
	
	Dev_open( LCD_DEVID, (void *)&lcd);
	
	lcd->getStrSize( self->font, &width, NULL);
	
	return width;
	
}
static int GhTxt_GetHeight(Glyph *self)
{
//	GhTxt *cthis = ( GhTxt *)self;
	uint16_t	height = 0;
	I_dev_lcd *lcd;
	
	Dev_open( LCD_DEVID, (void *)&lcd);
	
	lcd->getStrSize( self->font, NULL, &height);
	
	return height;
}

static int GhTxt_GetNum(Glyph *self)
{
	GhTxt *cthis = ( GhTxt *)self;
	
	int txtLen = CQ_Len( &cthis->cq);
	
	
	
	return txtLen;
}






CTOR( GhTxt)
SUPER_CTOR( Glyph);

FUNCTION_SETTING( Glyph.clean, GhTxt_Clean);
FUNCTION_SETTING( Glyph.insert, GhTxt_Insert);
FUNCTION_SETTING( Glyph.draw, GhTxt_Draw);
FUNCTION_SETTING( Glyph.flush, GhTxt_Flush);


FUNCTION_SETTING( Glyph.getSize, GhTxt_GetSize);

FUNCTION_SETTING( Glyph.getWidth, GhTxt_GetWidth);
FUNCTION_SETTING( Glyph.getHeight, GhTxt_GetHeight);
FUNCTION_SETTING( Glyph.getNum, GhTxt_GetNum);

END_CTOR
