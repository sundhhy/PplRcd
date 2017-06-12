#include "NtGrid.h"

#include "ExpFactory.h"
#include <string.h>
#include "Gh_Rectangle.h"

#include "Reader.h"

#include "Gh_txt.h"


static NtGrid *signalThisExp;



NtGrid *GetNtGrid(void)
{
	if( signalThisExp == NULL)
	{
		signalThisExp = NtGrid_new();
	}
	return signalThisExp;
}



static void * NtGrid_Interpret( Expr *self, void *fa, void *context)
{
	NtGrid *cthis = ( NtGrid *)self;
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
	vd->dspCnt.colour = String2Clr( att);
	
	vd->dspCnt.bkc  = String2Bkc( att);
	
	if( GetKeyVal( att, "cg", tmpbuf, 4))
	{
		vd->columnGap = atoi( tmpbuf);
		
	}
	if( GetKeyVal( att, "ls", tmpbuf, 4))
	{
		vd->lineSpacing = atoi( tmpbuf);
		
	}
	if( GetKeyVal( att, "cols", tmpbuf, 4))
	{
		vd->cols= atoi( tmpbuf);
		
	}
	if( GetKeyVal( att, "x", tmpbuf, 4))
	{
		vd->dspArea.sizeX = atoi( tmpbuf);
		
	}
//	else
//	{
//		
//		vd->dspArea.sizeX = SIZE_ERR;
//	}
	if( GetKeyVal( att, "y", tmpbuf, 4))
	{
		vd->dspArea.sizeY = atoi( tmpbuf);
		
	}
//	else
//	{
//		vd->dspArea.sizeY = SIZE_ERR;
//		
//	}
		
	
	
	
	pnewPosition = RemoveHead( pnewPosition);

	//一个input应该有:text和rct两个子图元
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
	
	
	
	vd->dspArea.sizeX = 0;
	vd->dspArea.sizeY = 0;
	vd->dspCnt.len = 1;
	
	
	vd->gh = NULL;
	self->ction->insertVD( self->ction, NULL, vd);
		
	memset( expTempBUf, 0, sizeof( expTempBUf));
	pnewPosition = RemoveTail( pnewPosition, NULL, 0);
	exit:
	
	
	return pnewPosition;
	
}

CTOR( NtGrid)
SUPER_CTOR( Expr);
FUNCTION_SETTING( Expr.interpret, NtGrid_Interpret);
END_CTOR







