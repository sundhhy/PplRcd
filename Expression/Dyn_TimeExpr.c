#include "Dyn_TimeExpr.h"
#include "sdhDef.h"
#include "dev_lcd.h"
#include "Gh_txt.h"
#include <string.h>
#include "Reader.h"
#include "mem/CiiMem.h"

#include "utils/time.h"
//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------

//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
static Dyn_TimeExpr *signalExpr;
static char timeStr[12];

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
/* Cycle/Sync Callback functions */
static int Dyn_timeHdl( void *pDynd, int option, void **ppResult);

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
Dyn_TimeExpr *GetTimeExpr(void)
{
	if( signalExpr == NULL)
	{
		signalExpr = Dyn_TimeExpr_new();
	}
	return signalExpr;
}

static void * TimeInterpret( Expr *self, void *faVd, void *context)
{
	GhTxt *mytxt = Get_GhTxt();
	Glyph	*myGp = (Glyph *)mytxt;
	int len = 0;
	char	*pp;
	char 	*pnewPosition;
	char	*att = expTempBUf;
	char 	tmpbuf[4] = {0};
	
	ViewData_t	*vd;


	GetAttribute( context, att, TEMPBUF_LEN);
	
	
	pnewPosition = GetNameVale( context, self->variable, &pp, &len);
//	if( len == 0)
//		goto exit;
	
	vd = self->ction->allocVD( self->ction);
	
	if( vd == NULL)
		goto exit;
	
	
	
	
	//属性优先级:自己的私有属性， 父属性， 系统默认属性
	
	
	
	vd->dspCnt.colour = String2Clr( att);
	
	
//	vd->dspCnt.effects = String2CntEff( att);
	vd->dspCnt.bkc  = String2Bkc( att);
	
	GetKeyVal( att, "xali", tmpbuf, 4);
	vd->dspArea.ali = String2Align( tmpbuf);
	GetKeyVal( att, "yali", tmpbuf, 4);
	vd->dspArea.aliy = String2Align( tmpbuf);
	vd->dspCnt.font = String2Font( att);
	
//	UsePrntAttIfNeed( ( ViewData_t	*)faVd, vd);
	
	if( vd->dspCnt.colour == ERR_COLOUR)
	{
		
		vd->dspCnt.colour = DefaultColour(NULL);
		
	}
	if( vd->dspArea.ali == ALIGN_ERR)
	{
		
		vd->dspArea.ali = ALIGN_DEFAULT;
		
	}
	
	NEW( vd->dspArea.boundary);
	if( GetKeyVal( att, "bndx1", tmpbuf, 4))
	{
		vd->dspArea.boundary->x1 = atoi( tmpbuf);
		
	}
	else
		goto err;
	if( GetKeyVal( att, "bndx2", tmpbuf, 4))
	{
		vd->dspArea.boundary->x2 = atoi( tmpbuf);
		
	}
	else
		goto err;
	if( GetKeyVal( att, "bndy1", tmpbuf, 4))
	{
		vd->dspArea.boundary->y1 = atoi( tmpbuf);
		
	}
	else
		goto err;
	if( GetKeyVal( att, "bndy2", tmpbuf, 4))
	{
		vd->dspArea.boundary->y2 = atoi( tmpbuf);
		
	}
	else
		goto err;
	
	

	
	
	vd->dspCnt.data = pp;
	vd->dspCnt.len = len;
	
	vd->gh = myGp;
	myGp->getSize( myGp,  vd->dspCnt.font, &vd->dspArea.sizeX, &vd->dspArea.sizeY);
	
	
	
	self->ction->insertDynVD( self->ction, faVd, vd);
	pnewPosition = RemoveTail( pnewPosition, att, 16);
	
	
	
exit:
	memset( expTempBUf, 0, sizeof( expTempBUf));
	return pnewPosition;
//err1:
//	FREE( vd->pdyndHdl);
err:
	FREE( vd->dspArea.boundary);
	free(vd);
	memset( expTempBUf, 0, sizeof( expTempBUf));
	return pnewPosition;
		
		
}

CTOR( Dyn_TimeExpr)
SUPER_CTOR( Expr);
FUNCTION_SETTING( Expr.interpret, TimeInterpret);

END_CTOR

//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{

//static int Dyn_timeHdl( void *pDynd, int option, void **ppResult)
//{
//	struct  tm *pTm = ( struct  tm *)pDynd;
//	if( pDynd == NULL)
//		return ERR_BAD_PARAMETER;
//	
//	sprintf( timeStr, "%02d:%02d:%02d", pTm->tm_hour, pTm->tm_min, pTm->tm_sec);
//	*ppResult = timeStr;
//	return strlen(timeStr) ;
//	
//}














