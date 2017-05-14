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


static void VDapply(void **ppvd, void *cl)
{
	ViewData_t *pvd = *ppvd;
	area_t	*area = ( area_t *)cl;
	short 	num = 0;
	

	char	row = 0;
	
	char	maxinrow = 0;
	
	//该行允许显示的数量
	char	allowNum = 0;
	
	
	
	//分配起始坐标为光标的当前位置
	pvd->area_x1 = area->cursorX;
	pvd->area_y1 = area->cursorY;
	
	//每行能够显示的数量
	maxinrow = area->LcdSizeX / pvd->size_x;
	
	//分配结束坐标
	num = pvd->len;
	//考虑到中文字符有两个位置，所以一定要分配偶数个位置
	if( num & 1)
		num ++;
	
	
	//计算能够分配的数据
	row = 1;
	allowNum =  ( area->LcdSizeX - pvd->area_x1) / pvd->size_x;
	while(1)
	{	
		
		//不够显示的，就多加一行
		if(  num > allowNum)
		{

			num -= allowNum;
			//增加一行
			row ++;
			allowNum = maxinrow;
			
		}	
		else
		{
			//够了的话，就计算x方向的结尾
			
			//与起始位置在同一行的话就要从x1处偏移
			//不同行就不需要了
			if( row == 1)
				pvd->area_x2 = pvd->area_x1 + num * pvd->size_x ;
			else
				pvd->area_x2 = num * pvd->size_x ;
			break;
		}
		
		
		
	}
	
	
	//计算y方向上的结尾
	pvd->area_y2 = pvd->area_y1 + (row -1 )* pvd->size_y;
	if( pvd->area_y2 > area->LcdSizeY)
	{
		pvd->area_y2 = area->LcdSizeY;
		pvd->more	= 1;
	}
	
	
	//移动光标
	area->cursorX = pvd->area_x2 + pvd->size_x;
	area->cursorY = pvd->area_y2;
	
}

static void VDShow(void **ppvd, void *cl)
{
	ViewData_t *pvd = *ppvd;
	Composition	*ction = ( Composition *)cl;
	//每行能够显示的量	
	short 	numInrow = 0;
	char	numRow = 0;
	
	//把全部要显示的数据转转到gh的缓存中
	pvd->gh->setFont( pvd->gh, pvd->font);
	pvd->gh->insert( pvd->gh, pvd->data, pvd->len);
	
	//计算需要显示的行数
	numRow = ( pvd->area_y2 - pvd->area_y1) / pvd->size_y + 1;
	
	//显示第1行
	numInrow = ( ction->lcdArea.LcdSizeX - pvd->area_x1) / pvd->size_x;
	pvd->gh->draw( pvd->gh, pvd->area_x1, pvd->area_y1, numInrow);
	
	//换行
	pvd->area_y1 += pvd->size_y;
	pvd->area_x1 = 0;
	
	numRow --;
	
	//显示中间的行
	numInrow = ( ction->lcdArea.LcdSizeX ) / pvd->size_x;
	//需要分页显示的时候，最后一行按照中间行来显示
	while( numRow > 1 || pvd->more)
	{
		
		pvd->gh->draw( pvd->gh, pvd->area_x1, pvd->area_y1, numInrow);
		
		//换行
		pvd->area_y1 += pvd->size_y;
		pvd->area_x1 = 0;
		
		numRow --;
	}
	
	//显示最后一行
	if( numRow == 1)
	{
		numInrow = ( pvd->area_x2 ) / pvd->size_x;
		pvd->gh->draw( pvd->gh, pvd->area_x1, pvd->area_y1, numInrow);
	}
	
	
	//检查是否需要分页显示
	if( pvd->more)
	{
		
	}
	
	
}

static int Simp_VdLayout( Composition *ction, List_T t_vd)
{
	//数据中单个元素在x轴和y轴上的尺寸
	List_map( ction->t_vd, VDapply, &ction->lcdArea);
	
	//显示数据
	List_map( ction->t_vd, VDShow, ction);
	
	return RET_OK;
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
			{
				if( ction->addRow( ction) != RET_OK)
				{
					//显示分页标识
					
					goto exit;
				}
					
				
				
			}
				
			
		}
		
		
		gh->draw( gh, ction->x, ction->y, num);	
		ction->x += num  * ction->ghWidth;
		
		num = gh->getNum( gh);
		
		//会的话就增加一行
		if( num)
		{
			if( ction->addRow( ction) != RET_OK)
			{
				//显示分页标识
				
				goto exit;
			}
			
		}
			
	}
	
	
	exit:	
	return RET_OK;
}


CTOR( SimpCtor)
FUNCTION_SETTING( Compositor.vdLayout, Simp_VdLayout);



FUNCTION_SETTING( Compositor.compose, Simp_Compose);


END_CTOR

