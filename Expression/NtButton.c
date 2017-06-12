#include "NtButton.h"

#include "ExpFactory.h"
#include <string.h>
#include "Gh_Rectangle.h"

#include "Reader.h"

static const char buData[] = "bu";

static NtButton *signalNtButton;


NtButton *GetNtButton(void)
{
	if( signalNtButton == NULL)
	{
		signalNtButton = NtButton_new();
	}
	return signalNtButton;
}



static void * BuInterpret( Expr *self, void *fa, void *context)
{
	NtButton *cthis = ( NtButton *)self;
	GhRectangle *myRec = Get_GhRectangle();
	Glyph	*myGp = (Glyph *)myRec;
	char	*pp;
	char 	*pnewPosition = context;
	char	*att = expTempBUf;
	Expr 	*myexp ;
	char 	tmpbuf[4] = {0};
	short	nameLen;
	
	ViewData_t	*vd;

	GetAttribute( context, att, TEMPBUF_LEN);
	
	vd = self->ction->allocVD( self->ction);
//	vd->dspArea.ali = String2Align( att);
	GetKeyVal( att, "xali", tmpbuf, 4);
	vd->dspArea.ali = String2Align( tmpbuf);
	vd->dspCnt.font = String2Font( att);
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
	
	
	UsePrntAttIfNeed( ( ViewData_t	*)fa, vd);
	
	
	//自己的尺寸已经被设置了，就不要使用子图元来设置了
	if( vd->dspArea.sizeX != SIZE_ERR && vd->dspArea.sizeY != SIZE_ERR)
	{
		vd->donotUseChldSize = 1;
		
	}

	pnewPosition = RemoveHead( context);
	
	
	while(1)
	{
		
		
		
		memset( expTempBUf, 0, sizeof( expTempBUf));
		nameLen = GetName( pnewPosition, att, TEMPBUF_LEN);
		if( nameLen == 0)
			break;
		myexp = ExpCreate( att);
		if( myexp == NULL)
			break;
			
		myexp->setCtion( myexp, self->ction);
		myexp->setVar( myexp, att);		//跟据Context中的变量来设置
		pnewPosition = myexp->interpret( myexp, vd, pnewPosition);
		
		
	}
	
	vd->gh = myGp;
	vd->dspCnt.len = 1;
	self->ction->insertVD( self->ction, fa, vd);
		
	
	memset( expTempBUf, 0, sizeof( expTempBUf));
	pnewPosition = RemoveTail( pnewPosition, NULL, 0);
	exit:
	
	
	return pnewPosition;
}

CTOR( NtButton)
SUPER_CTOR( Expr);
FUNCTION_SETTING( Expr.interpret, BuInterpret);

END_CTOR







