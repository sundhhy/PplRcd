#include "BrExpr.h"
#include "basis/sdhDebug.h"
#include "basis/sdhError.h"
#include "sdhDef.h"
#include "Gh_none.h"

#include <string.h>

#include "Reader.h"

static BrExpr *signalBrExpr;
static const char brData[] = "\n";


BrExpr *GetBrExpr(void)
{
	if( signalBrExpr == NULL)
	{
		signalBrExpr = BrExpr_new();
	}
	return signalBrExpr;
}


static void * BrInterpret( Expr *self, void *faVd, void *context)
{
//	GhNone *myGh = Get_GhNone();
	void 	*pnewPosition;
//	ViewData_t	*vd;


//	vd = self->ction->allocVD( self->ction);
//	
//	
//	
//	if( vd == NULL)
//		goto exit;

//	vd->data =( char *) brData;
//	vd->len = 1;
//	vd->gh = (Glyph *)myGh;
//	
//	self->ction->insertVD( self->ction, NULL, vd);
//	exit:
//	pnewPosition = RemoveHead( context);
	
	
	return pnewPosition;
}

CTOR( BrExpr)
SUPER_CTOR( Expr);
FUNCTION_SETTING( Expr.interpret, BrInterpret);

END_CTOR






