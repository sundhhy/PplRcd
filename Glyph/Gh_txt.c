#include "Gh_txt.h"
#include "device.h"

#define GHTXTBUFLEN		64


static GhTxt *signalGhTxt;
static void GhTxt_Insert( Glyph *self, void *context, int len)
{
	GhTxt *cthis = ( GhTxt *)self;
	CQ_Write( &cthis->cq, context, len);
	
}

static void GhTxt_Draw( Glyph *self, int x, int y, int len)
{
	I_dev_lcd *lcd;
	
	GhTxt *cthis = ( GhTxt *)self;
	char *p;
	int txtLen = CQ_GetPtr( &cthis->cq, &p, len);
	
	Dev_open( LCD_DEVID, (void *)&lcd);
	
	lcd->Clear();
	lcd->wrString( p, txtLen, x, y, self->font, self->colour);
	
}
static void GhTxt_Flush( Glyph *self, int x, int y)
{
	I_dev_lcd *lcd;
	
	GhTxt *cthis = ( GhTxt *)self;
	char *p;
	int txtLen = CQ_GetPtr( &cthis->cq, &p, CQ_LENALL);
	
	Dev_open( LCD_DEVID, (void *)&lcd);
	
	lcd->Clear();
	lcd->wrString( p, txtLen, x, y, self->font, self->colour);
//	lcd->wrString( "text", 4, x, y, self->font, self->colour);
	
}



GhTxt *Get_GhTxt(void)
{
	Glyph *gh;
	
	if( signalGhTxt == NULL)
	{
		signalGhTxt = GhTxt_new();
		gh = ( Glyph *)signalGhTxt;
		signalGhTxt->buf = malloc( GHTXTBUFLEN);
		gh->setFont( gh, DEF_FONT);
		gh->setClu( gh, DEF_COLOUR);
		CQ_Init( &signalGhTxt->cq, signalGhTxt->buf, GHTXTBUFLEN);
		
	}
	return signalGhTxt;
}
CTOR( GhTxt)
SUPER_CTOR( Glyph);
FUNCTION_SETTING( Glyph.insert, GhTxt_Insert);
FUNCTION_SETTING( Glyph.draw, GhTxt_Draw);
FUNCTION_SETTING( Glyph.flush, GhTxt_Flush);

END_CTOR
