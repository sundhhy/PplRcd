#include "TxtExpr.h"
//#include "basis/sdhDebug.h"
//#include "basis/sdhError.h"
#include "sdhDef.h"
#include "dev_lcd.h"
#include "Gh_txt.h"

#include <string.h>

static TxtExpr *signalTxtExpr;


TxtExpr *GetTxtExpr(void)
{
	if( signalTxtExpr == NULL)
	{
		signalTxtExpr = TxtExpr_new();
		if(signalTxtExpr  == NULL) while(1);
	}
	return signalTxtExpr;
}




static void * TxtInptSht( Expr *self, void *context, sheet *p_sht)
{
	GhTxt 	*mytxt = Get_GhTxt();
	Glyph	*myGp = (Glyph *)mytxt;
	char	name[8];
	int 	len = 0;
	char	*pp;
	char 	*pnewPosition;
	char	*att = expTempBUf;
	int		ret = 0;

	GetAttribute( context, att, TEMPBUF_LEN);
	
	len = 8;
	pp = context;
	len = GetName( pp, name, len);
	
	pnewPosition = GetNameVale( context, name, &pp, &len);
	if( len == 0)
		goto exit;
	
	Set_shtContextAtt( att, p_sht);
	ret =  Set_shtAreaAtt( att,  p_sht);
	if( ret & SET_ATT_BSIZEFAIL)
	{
		myGp->getSize( myGp,  p_sht->cnt.font, &p_sht->bxsize, &p_sht->bysize);
		p_sht->bxsize = p_sht->bxsize * len;
	}
	
	p_sht->cnt.data = pp;
	p_sht->cnt.len = len;
	p_sht->p_gp = myGp;
	
	//对标题类的特殊处理
	if( !strcasecmp( name, "title"))
	{
		if( p_sht->cnt.bkc == ERR_COLOUR)
		{
			
			p_sht->cnt.bkc = COLOUR_BLUE;
		}
		if( p_sht->cnt.font < FONT_16)
			p_sht->cnt.font = FONT_16;
//		p_sht->cnt.subType = TEXT_ST_LABLE;
		p_sht->bxsize = SIZE_BOUNDARY;
		
	}
	
	pnewPosition = RemoveTail( pnewPosition, att, 16);
	exit:
	memset( expTempBUf, 0, sizeof( expTempBUf));
	
	return pnewPosition;
}


CTOR( TxtExpr)
SUPER_CTOR( Expr);
FUNCTION_SETTING( Expr.inptSht, TxtInptSht);

END_CTOR






