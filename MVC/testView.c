#include "testView.h"
#include "TxtExpr.h"
#include "Composition.h"
#include "Compositor.h"

 
static const char testContext[] = \
" <body> <h2>screen char test!</h2> </body>";



void View_test(void)
{
//	TestViewShow();
	char *pp;
	
	Composition *ct = Get_Composition();
	Compositor *ctor = (Compositor *)Get_SimpCtor();
	Expr *myexp = (Expr* )GetTxtExpr();
	ct->setCtor( ct, ctor);
	ct->clean( ct);
	myexp->setCtion( myexp, ct);
	myexp->setVar( myexp, "h2");		//跟据Context中的变量来设置
	myexp->interpret( myexp, (void *)testContext);
	
}



