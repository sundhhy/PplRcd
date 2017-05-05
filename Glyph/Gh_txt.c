#include "Gh_txt.h"
#include "device.h"

static GhTxt *signalGhTxt;

static void GhTxt_draw( Glyph *self, int x, int y)
{
	GhTxt *cthis = ( GhTxt *)self;
	self->myLcd->Clear();
	self->myLcd->wrString( cthis->txt, cthis->len, 0, 0, self->font, self->colour);
	
}

GhTxt *Get_GhTxt(void)
{
	Glyph *gh;
	void *myLcd;
	
	if( signalGhTxt == NULL)
	{
		signalGhTxt = GhTxt_new();
		gh = ( Glyph *)signalGhTxt;
		Dev_open( DEVID_USARTGPULCD, ( void *)&myLcd);
		gh->init( gh,  myLcd);
		
	}
	return signalGhTxt;
}
CTOR( GhTxt)
SUPER_CTOR( Glyph);
FUNCTION_SETTING( Glyph.draw, GhTxt_draw);

END_CTOR
