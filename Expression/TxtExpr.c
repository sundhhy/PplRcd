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


static void * TxtInterpret( Expr *self, void *faVd, void *context)
{
//	TxtExpr *cthis = ( TxtExpr *)self;
	GhTxt *mytxt = Get_GhTxt();
	Glyph	*myGp = (Glyph *)mytxt;
	int len = 0;
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
	
	self->ction->insertVD( self->ction, faVd, vd);
	
	exit:
	
	return pnewPosition;
}

CTOR( TxtExpr)
SUPER_CTOR( Expr);
FUNCTION_SETTING( Expr.interpret, TxtInterpret);

END_CTOR






