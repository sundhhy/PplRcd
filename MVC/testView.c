#include "testView.h"
#include "TxtExpr.h"
#include "Composition.h"
#include "Compositor.h"

 
static const char testContext[] = \
" <body> <h2>screen char test!</> <h2>screen char test2!</>\
<h2>screen char test3!</><h2>screen char test4!</></body>";



void View_test(void)
{
//	TestViewShow();
	void *pp = (void *)testContext;
	void *pnew;
	
	Composition *ct = Get_Composition();
	Compositor *ctor = (Compositor *)Get_SimpCtor();
	Expr *myexp = (Expr* )GetTxtExpr();
	ct->setCtor( ct, ctor);
	ct->clean( ct);
	myexp->setCtion( myexp, ct);
	myexp->setVar( myexp, "h2");		//跟据Context中的变量来设置
	pnew = myexp->interpret( myexp, pp);
	
	myexp->setVar( myexp, "h2");		
	pnew = myexp->interpret( myexp, pnew);
	
	myexp->setVar( myexp, "h2");		
	pnew = myexp->interpret( myexp, pnew);
	
	myexp->setVar( myexp, "h2");		
	pnew = myexp->interpret( myexp, pnew);
}



