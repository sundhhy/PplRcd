#include "TxtExpr.h"
#include "basis/sdhDebug.h"
#include "basis/sdhError.h"
#include "sdhDef.h"
#include "dev_lcd.h"
#include "Gh_txt.h"

#include <string.h>

#include "Reader.h"

static TxtExpr *signalTxtExpr;


TxtExpr *GetTxtExpr(void)
{
	if( signalTxtExpr == NULL)
	{
		signalTxtExpr = TxtExpr_new();
	}
	return signalTxtExpr;
}


static void * TxtInterpret( Expr *self, void *context)
{
//	TxtExpr *cthis = ( TxtExpr *)self;
	GhTxt *mytxt = Get_GhTxt();
	Glyph	*myGp = (Glyph *)mytxt;
	int len = 0;
	//todo : 做成一个解析器
	char	*pp;
	void 	*pnewPosition;
//	pp = strstr((const char*)context, self->variable);
//	if( pp == NULL)
//		return ERR_BAD_PARAMETER;
//	while( *pp != '>')
//		pp ++;
//	pp ++;
//	while( pp[len] != '<')
//	{
//			len ++;
//	}
	
	pnewPosition = GetNameVale( context, self->variable, &pp, &len);
	if( len == 0)
		return pnewPosition;
	
	myGp->insert( myGp, pp, len);
	
	if( strcmp( self->variable, "h1") == 0)
	{
		myGp->setFont( myGp, FONT_64);
	}
	else if( strcmp( self->variable, "h2") == 0)
	{
		myGp->setFont( myGp, FONT_48);
	}
	else if( strcmp( self->variable, "h3") == 0)
	{
		myGp->setFont( myGp, FONT_24);
	}
	self->ction->insert( self->ction, myGp);
//	myGp->draw( myGp, 0, 0,3);
//	myGp->flush( myGp, 0, 0);
	return pnewPosition;
}

CTOR( TxtExpr)
SUPER_CTOR( Expr);
FUNCTION_SETTING( Expr.interpret, TxtInterpret);

END_CTOR






