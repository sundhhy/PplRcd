#include <stdint.h>
#include "testGlyph.h"
#include "Glyph.h"



#include "basis/sdhDebug.h"
#include "sdhDef.h"
#include "basis/sdhError.h"



static int Init( Glyph *self, I_dev_lcd *lcd)
{
	
	self->myLcd = lcd;
	self->font = DEF_FONT;
	self->colour = DEF_COLOUR;
	return RET_OK;
}
static int SetFont( Glyph *self, int font)
{
	if( CHECK_FONT(  font))
	{
		self->font = font;
		return RET_OK;
	}
	
	self->font = DEF_FONT;
	
	return ERR_BAD_PARAMETER;
	
}
static int SetClu( Glyph *self, int c)
{
	self->colour = c;
	return RET_OK;
	
}

ABS_CTOR( Glyph)
FUNCTION_SETTING( init, Init);
FUNCTION_SETTING( setFont, SetFont);
FUNCTION_SETTING( setClu, SetClu);

END_ABS_CTOR



