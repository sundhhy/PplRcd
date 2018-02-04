#include "GeometricsExpr.h"
#include "basis/sdhDebug.h"
#include "basis/sdhError.h"
#include "sdhDef.h"
#include "dev_lcd.h"
#include "Gh_txt.h"

#include <string.h>

#include "Reader.h"
#include "Gh_Rectangle.h"

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
static GmtrExpr *signaExpr;


//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------

static void * GeoInptSht( Expr *self, void *context, sheet *p_sht);
//static void * GmtrInterpret( Expr *self, void *faVd, void *context);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

GmtrExpr *GetGmtrExpr(void)
{
	if( signaExpr == NULL)
	{
		signaExpr = GmtrExpr_new();
		if(signaExpr  == NULL) while(1);
	}
	return signaExpr;
}

CTOR( GmtrExpr)
SUPER_CTOR( Expr);
//FUNCTION_SETTING( Expr.interpret, GmtrInterpret);
FUNCTION_SETTING( Expr.inptSht, GeoInptSht);

END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{

//几何图形:矩形 

static void * GeoInptSht( Expr *self, void *context, sheet *p_sht)
{
	GhRectangle 	*gpGmtr = NULL;
	Glyph			*myGp = NULL;
	char	name[8];
	int 	len = 0;
	char	*pp;
	char	*att = expTempBUf;

	GetAttribute( context, att, TEMPBUF_LEN);
	
	len = 8;
	pp = context;
	len = GetName( pp, name, len);
	
	
	
	Set_shtContextAtt( att, p_sht);
	Set_shtAreaAtt( att,  p_sht);
	
	gpGmtr = Get_GhRectangle();
	if( !strcasecmp( name, "line")) {	
//		gpGmtr = Get_GhRectangle();	
		p_sht->cnt.subType = GMT_LINE;
	}
//	else if( !strcasecmp( name, "input")){
//		gpGmtr = Get_GhRectangle();
//	}
	else {
//		gpGmtr = Get_GhRectangle();
		p_sht->cnt.subType = GMT_RECT;
		
	}
	
	myGp = (Glyph *)gpGmtr;
	p_sht->p_gp = myGp;
	
	
	memset( expTempBUf, 0, sizeof( expTempBUf));
	
	return pp;
}





//static void * GmtrInterpret( Expr *self, void *faVd, void *context)
//{
////	GmtrExpr 		*cthis = ( GmtrExpr *)self;
//	GhRectangle 	*gpGmtr = NULL;
//	Glyph			*myGp = NULL;
//	char			*att = expTempBUf;
//	char 			tmpbuf[4] = {0};
////	char			*pp;
//	char 			*pnewPosition = context;	
//	ViewData_t		*vd;

//	GetAttribute( context, att, TEMPBUF_LEN);
//	
//	
//	vd = self->ction->allocVD( self->ction);	
//	if( vd == NULL)
//		goto exit;
//	
//	pnewPosition = RemoveHead( pnewPosition);
//	
//	vd->dspCnt.colour = String2Clr( att);
//	vd->dspCnt.bkc  = String2Bkc( att);
//	if( GetKeyVal( att, "x", tmpbuf, 4))
//	{
//		vd->dspArea.sizeX = atoi( tmpbuf);
//		
//	}
//	else
//	{
//		
//		vd->dspArea.sizeX = SIZE_ERR;
//	}
//	if( GetKeyVal( att, "y", tmpbuf, 4))
//	{
//		vd->dspArea.sizeY = atoi( tmpbuf);
//		
//	}
//	else
//	{
//		vd->dspArea.sizeY = SIZE_ERR;
//		
//	}
//	
//	UsePrntAttIfNeed( ( ViewData_t	*)faVd, vd);
//	if( vd->dspCnt.colour == ERR_COLOUR)
//	{
//		
//		vd->dspCnt.colour = DefaultColour(NULL);
//		
//	}
//	if( vd->dspArea.ali == ALIGN_ERR)
//	{
//		
//		vd->dspArea.ali = ALIGN_DEFAULT;
//		
//	}
//	
//	
//	
//	if( !strcasecmp( self->variable, "rct"))
//	{
//		gpGmtr = Get_GhRectangle();
//		
////		vd->dspCnt.subType = GMTR_ST_RCT;
//		
//	}
//	else
//	{
//		
//		gpGmtr = Get_GhRectangle();
////		vd->dspCnt.subType = GMTR_ST_RCT;
//	}
//	
//	myGp = (Glyph *)gpGmtr;
//	vd->dspCnt.len = 1;
//	vd->gh = myGp;
////	
//	self->ction->insertVD( self->ction, faVd, vd);
//	pnewPosition = RemoveTail( pnewPosition, att, TEMPBUF_LEN);
//	exit:
//	
//	return pnewPosition;
//}








