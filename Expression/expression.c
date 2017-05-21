#include "expression.h"
#include <string.h>
/*
expression :: = 

variable = [ A-Z, a-z]*



*/

/*ËõÐ´
c : colour
bu : button



*/


static void SetVar( Expr *self, char *variable)
{
	//½Ø¶Ï
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

int	ExpStr2colour( char *s)
{
	int c = 0;
	if( strstr( s, "c=red"))
	{
		c = COLOUR_RED;
		
	}
	else if( strstr( s, "c=gren"))
	{
		c = COLOUR_RED;
		
	}
	else if( strstr( s, "c=blue") )
	{
		c = COLOUR_BLUE;
		
	}
	else if( strstr( s, "c=yellow") )
	{
		c = COLOUR_YELLOW;
		
	}
	else if( strstr( s, "c=purple") )
	{
		c = COLOUR_PURPLE;
		
	}
	else if( strstr( s, "c=gray") )
	{
		c = COLOUR_GRAY;
		
	}
	else
	{
		c = DEF_COLOUR;
	}
	
	return c;
	
}

int	ExpStr2font( char *s)
{
	int f = 0;
	
	if( strcasecmp( s, "h1") == 0)
	{
		f = FONT_12;
	}
	else if( strcasecmp( s, "h2") == 0)
	{
		f = FONT_16;
	}
	else if( strcasecmp( s, "h3") == 0)
	{
		f = FONT_24;
	}
	else if( strcasecmp( s, "h4") == 0)
	{
		f = FONT_32;
	}
	else if( strcasecmp( s, "h5") == 0)
	{
		f = FONT_48;
	}
	else if( strcasecmp( s, "h6") == 0)
	{
		f = FONT_64;
	}
	else
	{
		f = DEF_FONT;
	}
	
	return f;
}


ABS_CTOR( Expr)
FUNCTION_SETTING( setVar, SetVar);
FUNCTION_SETTING( setCtion, SetCtion);
//FUNCTION_SETTING( setFather, SetFather);



FUNCTION_SETTING( str2colour, ExpStr2colour);
FUNCTION_SETTING( str2font, ExpStr2font);

END_ABS_CTOR





