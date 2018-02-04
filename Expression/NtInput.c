#include "NtInput.h"

#include "ExpFactory.h"
#include <string.h>
#include "Gh_None.h"

#include "Reader.h"

#include "Gh_txt.h"
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

static NtInput *signalNtInput;

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------

static void * InputInptSht( Expr *self, void *context, sheet *p_sht);

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
NtInput *GetNtInput(void)
{
	if( signalNtInput == NULL)
	{
		signalNtInput = NtInput_new();
		if(signalNtInput  == NULL) while(1);
	}
	return signalNtInput;
}

CTOR( NtInput)
SUPER_CTOR( Expr);
FUNCTION_SETTING( Expr.inptSht, InputInptSht);
END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{








static void * InputInptSht( Expr *self, void *context, sheet *p_sht)
{
	Glyph	*myGp = (Glyph *)Get_GhNone();
	shtctl 	*p_shtctl = GetShtctl();	
	char 	*pnewPosition = context;
	char	*att = expTempBUf;
	Expr 	*p_exp ;
//	char 	tmpbuf[4] = {0};

	GetAttribute( context, att, TEMPBUF_LEN);
	Set_shtContextAtt( att, p_sht);
	Set_shtAreaAtt( att,  p_sht);
	
//	if( GetKeyVal( att, "cg", tmpbuf, 4))
//	{
//		p_sht->subAtt.subColGrap = atoi( tmpbuf);
//		
//	}
//	else
//	{
//		p_sht->subAtt.subColGrap = 0;
//	}
//	if( GetKeyVal( att, "rg", tmpbuf, 4))
//	{
//		p_sht->subAtt.subRowGrap  = atoi( tmpbuf);
//		
//	}
//	else
//	{
//		p_sht->subAtt.subRowGrap = 0;
//	}
	
	//一个input应该有:text和rct两个子图元
	p_sht->pp_sub = malloc( 2 * sizeof( sheet *));
	p_sht->subAtt.numSubRow = 1;
	p_sht->subAtt.numSubCol = 2;
	
	p_sht->pp_sub[0] = Sheet_alloc( p_shtctl);
	pnewPosition = RemoveHead( pnewPosition);
	p_exp = ExpCreate( "text");
	pnewPosition = p_exp->inptSht( p_exp, (void *)pnewPosition, p_sht->pp_sub[0]) ;
	
	p_sht->pp_sub[1] = Sheet_alloc( p_shtctl);
	p_exp = ExpCreate( "box");
	p_exp->inptSht( p_exp, (void *)pnewPosition, p_sht->pp_sub[1]) ;
	
	//将子图层的尺寸汇总作为自己的尺寸
	p_sht->bxsize = p_sht->pp_sub[0]->bxsize + p_sht->pp_sub[1]->bxsize + p_sht->subAtt.subColGrap;
	p_sht->bysize = ( p_sht->pp_sub[0]->bysize > p_sht->pp_sub[1]->bysize)? p_sht->pp_sub[0]->bysize : p_sht->pp_sub[1]->bysize;
	p_sht->p_gp = myGp;
//	while(1)
//	{
//		
//		
//		
//		memset( expTempBUf, 0, sizeof( expTempBUf));
//		nameLen = GetName( pnewPosition, att, TEMPBUF_LEN);
//		if( nameLen == 0)
//			break;
//		myexp = ExpCreate( att);
//		if( myexp == NULL)
//			break;
//			
//		myexp->setCtion( myexp, self->ction);
//		myexp->setVar( myexp, att);		//跟据Context中的变量来设置
//		pnewPosition = myexp->interpret( myexp, vd, pnewPosition);
//		
//		
//	}
		
	memset( expTempBUf, 0, sizeof( expTempBUf));
	pnewPosition = RemoveTail( pnewPosition, NULL, 0);
//	exit:
	
	
	return pnewPosition;
	
}









