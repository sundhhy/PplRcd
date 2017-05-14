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
	
	char	att[16];
	
	ViewData_t	*vd;

	GetAttribute( context, att, 16);
	
	
	pnewPosition = GetNameVale( context, self->variable, &pp, &len);
	if( len == 0)
		goto exit;
	
	vd = self->ction->allocVD( self->ction);
	
	if( vd == NULL)
		goto exit;
	
	vd->colour = self->str2colour( att);
	vd->font = self->str2font( self->variable);

	vd->data = pp;
	vd->len = len;
	vd->gh = myGp;
	myGp->getSize( myGp, vd->font, &vd->size_x, &vd->size_y);
	self->ction->insertVD( self->ction, vd);
	
//	myGp->insert( myGp, pp, len);
	
//	if( strcasecmp( self->variable, "h1") == 0)
//	{
//		myGp->setFont( myGp, FONT_64);
//	}
//	else if( strcasecmp( self->variable, "h2") == 0)
//	{
//		myGp->setFont( myGp, FONT_48);
//	}
//	else if( strcasecmp( self->variable, "h3") == 0)
//	{
//		myGp->setFont( myGp, FONT_32);
//	}
//	else if( strcasecmp( self->variable, "h4") == 0)
//	{
//		myGp->setFont( myGp, FONT_24);
//	}
//	else if( strcasecmp( self->variable, "h5") == 0)
//	{
//		myGp->setFont( myGp, FONT_12);
//	}
//	else
//	{
//		myGp->setFont( myGp, DEF_FONT);
//	}
//	self->ction->insert( self->ction, myGp);

	
	exit:
	
	return pnewPosition;
}

CTOR( TxtExpr)
SUPER_CTOR( Expr);
FUNCTION_SETTING( Expr.interpret, TxtInterpret);

END_CTOR






