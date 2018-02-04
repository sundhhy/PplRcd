#include "NtButton.h"

#include "ExpFactory.h"
#include <string.h>
#include "Gh_Rectangle.h"

#include "Reader.h"
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
static NtButton *signalNtButton;


//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------

static void * BuInptSht( Expr *self, void *context, sheet *p_sht);

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
NtButton *GetNtButton(void)
{
	if( signalNtButton == NULL)
	{
		signalNtButton = NtButton_new();
		if(signalNtButton  == NULL) while(1);
	}
	return signalNtButton;
}

dspContent_t	*Button_Get_subcnt(sheet *p_bu)
{
	return &p_bu->pp_sub[0]->cnt;
}

CTOR( NtButton)
SUPER_CTOR( Expr);
FUNCTION_SETTING( Expr.inptSht, BuInptSht);

END_CTOR

//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{








static void * BuInptSht( Expr *self, void *context, sheet *p_sht)
{

	Glyph	*myGp = (Glyph *)Get_GhRectangle();
	shtctl 	*p_shtctl = GetShtctl();	
	char 	*pnewPosition = context;
	char	*att = expTempBUf;
	Expr 	*p_exp ;
	int 	ret = 0;
//	char	name[7];
	char	nameLen = 7;
//	char 	tmpbuf[4] = {0};

	GetAttribute( context, att, TEMPBUF_LEN);
	Set_shtContextAtt( att, p_sht);
	ret = Set_shtAreaAtt( att,  p_sht);
	
	
	
	//一个buton应该有:text
	if(p_sht->pp_sub == NULL) {
		p_sht->pp_sub = malloc( sizeof( sheet *));
		p_sht->subAtt.numSubRow = 1;
		p_sht->subAtt.numSubCol = 1;	
		p_sht->pp_sub[0] = Sheet_alloc( p_shtctl);
	}
	pnewPosition = RemoveHead( pnewPosition);
	
	
	memset( expTempBUf, 0, sizeof( expTempBUf));
	nameLen = GetName( pnewPosition, att, TEMPBUF_LEN);
	if( nameLen == 0) {
		return pnewPosition;
	}
	p_exp = ExpCreate( att);
	if( p_exp == NULL){
		return pnewPosition;
	}
			
//	p_exp = ExpCreate( "text");
	pnewPosition = p_exp->inptSht( p_exp, (void *)pnewPosition, p_sht->pp_sub[0]) ;
	
	
	if( ret & SET_ATT_BSIZEFAIL) {
		//按钮未指定自己的尺寸
		//将子图层的尺寸汇总作为自己的尺寸
		p_sht->bxsize = p_sht->pp_sub[0]->bxsize +  p_sht->area.offset_x * 2;
		p_sht->bysize = p_sht->pp_sub[0]->bysize + p_sht->area.offset_y * 2;
		p_sht->area.x1 = p_sht->area.x0 + p_sht->bxsize;
		p_sht->area.y1 = p_sht->area.y0 + p_sht->bysize;
		
		
	} else {
		p_sht->area.offset_x = 1;
		p_sht->area.offset_y = 1;
		
	}
	

	p_sht->p_gp = myGp;	
	memset( expTempBUf, 0, sizeof( expTempBUf));
	pnewPosition = RemoveTail( pnewPosition, NULL, 0);
	return pnewPosition;

}








