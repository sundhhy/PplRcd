#include "Compositor.h"
#include "device.h"

#include "Composition.h"


#include "basis/sdhDebug.h"
#include "basis/sdhError.h"

SimpCtor *signalSimpCtor;

SimpCtor *Get_SimpCtor(void)
{
	
	if( signalSimpCtor == NULL)
	{
		signalSimpCtor = SimpCtor_new();
		
	}
	
	return signalSimpCtor;
}




static int Simp_Compose( Composition *ction, Glyph *gh)
{
	int num;
	
	ction->ghWidth = gh->getWidth( gh);
	ction->ghHeight = gh->getHeight( gh);
	num = gh->getNum( gh);
	while( num)
	{
		//检查是否会从宽度上超出屏幕
		while( 1)
		{
			if( ( ction->x + num * ction->ghWidth ) <= ction->lcdWidth )
				break;
			if( num > 2)
				num -= 2;
			else 
				ction->addRow( ction);
				
			
		}
		gh->draw( gh, ction->x, ction->y, num);
		num = gh->getNum( gh);
		//会的话就增加一行
		if( num)
			ction->addRow( ction);
	}
	
	//检查是否会从列上超出屏幕
	
	//会的话就要求分页显示
	
	return RET_OK;
}


CTOR( SimpCtor)
FUNCTION_SETTING( Compositor.compose, Simp_Compose);


END_CTOR

