#include <stdint.h>
#include "testGlyph.h"
#include "Glyph.h"

#include <string.h>


#include "basis/sdhDebug.h"
#include "sdhDef.h"
#include "basis/sdhError.h"





static int Init( Glyph *self, I_dev_lcd *lcd)
{
	
//	self->myLcd = lcd;
//	self->font = DEF_FONT;
//	self->colour = DEF_COLOUR;
	return RET_OK;
}
static int SetFont( Glyph *self, int font)
{
	if( CHECK_FONT(  font))
	{
		
		return RET_OK;
	}

	return ERR_BAD_PARAMETER;
	
}
static int SetClu( Glyph *self, int c)
{

	return RET_OK;
	
}

static int SetBgC( Glyph *self, int c)
{
//	self->disArg.bgC = c;
	return RET_OK;
	
}

//static int setWidth( Glyph *self, uint16_t wd)
//{
//	
//	return RET_OK;
//	
//}
//static int setHeight( Glyph *self, uint16_t he)
//{
//	return RET_OK;
//	
//}


static int Clean( Glyph *self)
{
	return RET_OK;;
	
}

static void Insert( Glyph *self, void *context, int len)
{
	
	
}


static void Draw( Glyph *self, dspContent_t *cnt, dspArea_t *area)
{
	
	
}

//static int DrawArea( Glyph *self, short x1, short y1, short x2, short y2,  int len)
//{
//	return -1;
//	
//}




ABS_CTOR( Glyph)
FUNCTION_SETTING( init, Init);
FUNCTION_SETTING( setFont, SetFont);
FUNCTION_SETTING( setClu, SetClu);
FUNCTION_SETTING( setBgC, SetBgC);
//FUNCTION_SETTING( setWidth, setWidth);
//FUNCTION_SETTING( setHeight, setHeight);
FUNCTION_SETTING( clean, Clean);
//FUNCTION_SETTING( insert, Insert);
FUNCTION_SETTING( draw, Draw);

END_ABS_CTOR

int String2Colour( char *s)
{
	int c;
	if( strstr( s, "red"))
	{
		c = COLOUR_RED;
		
	}
	else if( strstr( s, "gren"))
	{
		c = COLOUR_GREN;
		
	}
	else if( strstr( s, "blue") )
	{
		c = COLOUR_BLUE;
		
	}
	else if( strstr( s, "yellow") )
	{
		c = COLOUR_YELLOW;
		
	}
	else if( strstr( s, "purple") )
	{
		c = COLOUR_PURPLE;
		
	}
	else if( strstr( s, "gray") )
	{
		c = COLOUR_GRAY;
		
	}
	else if( strstr( s, "white") )
	{
		c = COLOUR_WHITE;
		
	}
	else if( strstr( s, "black") )
	{
		c = COLOUR_BLACK;
		
	}
	else
	{
		c = ERR_COLOUR;
	}
	return c;
}

int	String2Bkc( char *s)
{
	char *pp ;
	char colour[8] = {0};
	char i = 0;
	pp =  strstr( s, "bkc");
	if( pp == NULL)
		return ERR_COLOUR;
	pp =  strstr( s, "=");
	if( pp == NULL)
		return ERR_COLOUR;
	
	pp ++;
	while(1)
	{
		colour[ i] = pp[i];
		i ++;
		if( pp[i] == ' ')
				break;
		if( i > 6)
			break;
		
	}
	colour[ i] = 0;
	return String2Colour( colour);
}

int String2Clr( char *s)
{
	char *pp ;
	pp =  strstr( s, "clr=");
	if( pp == NULL)
		return ERR_COLOUR;
	
	pp ++;
	
	return String2Colour( pp);
}

int String2Font( char *s)
{
	int f = 0;
	
	if( strstr( s, "f=12") )
	{
		f = FONT_12;
	}
	else if( strstr( s, "f=16") )
	{
		f = FONT_16;
	}
	else if( strstr( s, "f=24") )
	{
		f = FONT_24;
	}
	else if( strstr( s, "f=32") )
	{
		f = FONT_32;
	}
	else if( strstr( s, "f=48") )
	{
		f = FONT_48;
	}
	else if( strstr( s, "f=64") )
	{
		f = FONT_64;
	}
	else
	{
		f = DEF_FONT;
	}
	
	return f;
}

int	String2Align( char *s)
{
	int ali = ALIGN_LEFT;
	
	if( strstr( s, "ali=l") )
	{
		ali = ALIGN_LEFT;
	}
	else if( strstr( s, "ali=m") )
	{
		ali = ALIGN_MIDDLE;
	}
	else if( strstr( s, "ali=r") )
	{
		ali = ALIGN_RIGHT;
	}
	
	return ali;
}


int	DefaultColour( void *arg)
{
	int c = 0;
	if( arg == NULL)
	{
		c = DEF_COLOUR;
		
	}
	
	
	return c;
	
}

int	String2CntEff( char *s)
{
	int eff = 0;
	
	
	
	return eff;
}
