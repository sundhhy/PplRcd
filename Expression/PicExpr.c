#include "PicExpr.h"

#include "sdhDef.h"
#include "dev_lcd.h"
#include "Gh_Pic.h"

#include <string.h>

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
static PicExpr *signalPicExpr;


//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------

static void * PicInptSht( Expr *self, void *context, sheet *p_sht);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
PicExpr *GetPictExpr(void)
{
	if( signalPicExpr == NULL)
	{
		signalPicExpr = PicExpr_new();
		if(signalPicExpr  == NULL) while(1);
	}
	return signalPicExpr;
}

CTOR( PicExpr)
SUPER_CTOR( Expr);
FUNCTION_SETTING( Expr.inptSht, PicInptSht);

END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{
static void * PicInptSht( Expr *self, void *context, sheet *p_sht)
{
	GhPic 	*gp_pic = Get_GhPic();
	Glyph	*myGp = SUPER_PTR( gp_pic, Glyph);
	char	name[8];
	int 	len = 0;
	char	*pp;
	char 	*pnewPosition;
	char	*att = expTempBUf;
//	int		ret = 0;

	GetAttribute( context, att, TEMPBUF_LEN);
	
	len = 8;
	pp = context;
	len = GetName( pp, name, len);
	
	
	p_sht->cnt.subType = SUBTYPE_NONE;
	
	pp = strstr( name, "cpic");
	if( pp) {
		p_sht->cnt.subType = SUBTYPE_CPIC;
	} 
	pp = strstr( name, "bpic");
	if( pp) {
		p_sht->cnt.subType = SUBTYPE_BPIC;
	} 
	pp = strstr( name, "icon");
	if( pp) {
		p_sht->cnt.subType = SUBTYPE_ICON;
	} 
	pnewPosition = GetNameVale( context, name, &pp, &len);
	if( len == 0)
		goto exit;
	
	Set_shtAreaAtt( att,  p_sht);
	Set_shtContextAtt( att,  p_sht);
	
	
	p_sht->cnt.data = pp;
	p_sht->cnt.len = len;
	p_sht->p_gp = myGp;
	
	
	
	pnewPosition = RemoveTail( pnewPosition, att, 16);
	exit:
	memset( expTempBUf, 0, sizeof( expTempBUf));
	
	return pnewPosition;
}









