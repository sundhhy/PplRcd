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
//	NtButton *cthis = ( NtButton *)self;
	Expr *myexp ;
	GhRectangle *myRec = Get_GhRectangle();
	Glyph	*myGp = (Glyph *)myRec;
//	int len = 0;
//	char	*pp;
	void 	*pnewPosition = context;
	char	name[8];
	int		nameLen;
	char	att[16];
	ViewData_t	*vd;
	
	

	GetAttribute( context, att, 16);
	
	
	pnewPosition = RemoveHead( context);
	
	if( pnewPosition == context)
		goto exit;
	nameLen = 8;
	nameLen = GetName( pnewPosition, name, nameLen);
	if( nameLen == 0)
		goto exit;
	
	myexp = ExpCreate( name);
	if( myexp == NULL)
		goto exit;
	
	vd = self->ction->allocVD( self->ction);	
	
	vd->colour = self->str2colour( att);
	
	myexp->setCtion( myexp, self->ction);
	myexp->setVar( myexp, name);		//跟据Context中的变量来设置
	pnewPosition = myexp->interpret( myexp, vd, pnewPosition);
	
	vd->childAttr = SET_CHILDASWHOLE( vd->childAttr);
	
	vd->data = (char *)buData;
	vd->len = 1;
	//设置边框
	vd->size_x = 4;	
	vd->size_y = 6;	
	
	vd->gh = myGp;
	self->ction->insertVD( self->ction, NULL, vd);
	
	pnewPosition = RemoveTail( pnewPosition);
	
	exit:
	
	return pnewPosition;
}

CTOR( NtButton)
SUPER_CTOR( Expr);
FUNCTION_SETTING( Expr.interpret, BuInterpret);

END_CTOR







