#include "GeometricsExpr.h"
#include "basis/sdhDebug.h"
#include "basis/sdhError.h"
#include "sdhDef.h"
#include "dev_lcd.h"
#include "Gh_txt.h"

#include <string.h>

#include "Reader.h"
#include "Gh_Rectangle.h"


//几何图形:矩形 

static GmtrExpr *signaExpr;


GmtrExpr *GetGmtrExpr(void)
{
	if( signaExpr == NULL)
	{
		signaExpr = GmtrExpr_new();
	}
	return signaExpr;
}


static void * GmtrInterpret( Expr *self, void *faVd, void *context)
{
//	GmtrExpr 		*cthis = ( GmtrExpr *)self;
	GhRectangle 	*gpGmtr = NULL;
	Glyph			*myGp = NULL;
	char			*att = expTempBUf;
	char 			tmpbuf[4] = {0};
//	char			*pp;
	char 			*pnewPosition = context;	
	ViewData_t		*vd;

	GetAttribute( context, att, TEMPBUF_LEN);
	
	
	vd = self->ction->allocVD( self->ction);	
	if( vd == NULL)
		goto exit;
	
	pnewPosition = RemoveHead( pnewPosition);
	
	vd->dspCnt.colour = String2Clr( att);
	vd->dspCnt.bkc  = String2Bkc( att);
	if( GetKeyVal( att, "x", tmpbuf, 4))
	{
		vd->dspArea.sizeX = atoi( tmpbuf);
		
	}
	else
	{
		
		vd->dspArea.sizeX = SIZE_ERR;
	}
	if( GetKeyVal( att, "y", tmpbuf, 4))
	{
		vd->dspArea.sizeY = atoi( tmpbuf);
		
	}
	else
	{
		vd->dspArea.sizeY = SIZE_ERR;
		
	}
	
	UsePrntAttIfNeed( ( ViewData_t	*)faVd, vd);
	if( vd->dspCnt.colour == ERR_COLOUR)
	{
		
		vd->dspCnt.colour = DefaultColour(NULL);
		
	}
	if( vd->dspArea.ali == ALIGN_ERR)
	{
		
		vd->dspArea.ali = ALIGN_DEFAULT;
		
	}
	
	
	
	if( !strcasecmp( self->variable, "rct"))
	{
		gpGmtr = Get_GhRectangle();
		
//		vd->dspCnt.subType = GMTR_ST_RCT;
		
	}
	else
	{
		
		gpGmtr = Get_GhRectangle();
//		vd->dspCnt.subType = GMTR_ST_RCT;
	}
	
	myGp = (Glyph *)gpGmtr;
	vd->dspCnt.len = 1;
	vd->gh = myGp;
//	
	self->ction->insertVD( self->ction, faVd, vd);
	pnewPosition = RemoveTail( pnewPosition, att, TEMPBUF_LEN);
	exit:
	
	return pnewPosition;
}

CTOR( GmtrExpr)
SUPER_CTOR( Expr);
FUNCTION_SETTING( Expr.interpret, GmtrInterpret);

END_CTOR






