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
	char 	*pnewPosition;
	char	*att = expTempBUf;

	
	ViewData_t	*vd;


	GetAttribute( context, att, TEMPBUF_LEN);
	
	
	pnewPosition = GetNameVale( context, self->variable, &pp, &len);
	if( len == 0)
		goto exit;
	
	vd = self->ction->allocVD( self->ction);
	
	if( vd == NULL)
		goto exit;
	
	
	
	
	//属性优先级:自己的私有属性， 父属性， 系统默认属性
	
	
	
	vd->dspCnt.colour = String2Clr( att);
	
	
//	vd->dspCnt.effects = String2CntEff( att);
	vd->dspCnt.bkc  = String2Bkc( att);
	vd->dspArea.ali = String2Align( att);
	vd->dspCnt.font = String2Font( att);
	
	UsePrntAttIfNeed( ( ViewData_t	*)faVd, vd);
	
	if( vd->dspCnt.colour == ERR_COLOUR)
	{
		
		vd->dspCnt.colour = DefaultColour(NULL);
		
	}
	if( vd->dspArea.ali == ALIGN_ERR)
	{
		
		vd->dspArea.ali = ALIGN_DEFAULT;
		
	}
	
	vd->dspCnt.data = pp;
	vd->dspCnt.len = len;
	
	//对标题类的特殊处理
	if( !strcasecmp( self->variable, "title"))
	{
		if( vd->dspCnt.bkc == ERR_COLOUR)
		{
			
			vd->dspCnt.bkc = COLOUR_BLUE;
		}
		if( vd->dspCnt.font < FONT_16)
			vd->dspCnt.font = FONT_16;
		vd->dspCnt.subType = TEXT_ST_LABLE;
		
	}
	else
	{
		vd->dealAli = 1;
	}
	
	
	vd->gh = myGp;
	myGp->getSize( myGp,  vd->dspCnt.font, &vd->dspArea.sizeX, &vd->dspArea.sizeY);
	
	
	
	self->ction->insertVD( self->ction, faVd, vd);
	pnewPosition = RemoveTail( pnewPosition, att, 16);
	exit:
	memset( expTempBUf, 0, sizeof( expTempBUf));
	
	return pnewPosition;
}

CTOR( TxtExpr)
SUPER_CTOR( Expr);
FUNCTION_SETTING( Expr.interpret, TxtInterpret);

END_CTOR






