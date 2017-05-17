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
	char		c;
	uint8_t		dealNum = 0;
	uint8_t		dealIdx = 0;
	uint16_t	usableX, usableY;
	uint16_t	needX, needY;
	
	//一个字符一个字符来分配
	
	
	
	//将分配的区域x1,y1,x2,y2都初始化为光标的位置
	pvd->area_x1 = area->cursorX;
	pvd->area_y1 = area->cursorY;
	pvd->area_x2 = area->cursorX;
	pvd->area_y2 = area->cursorY;
	
	//如果一个x轴一个字符也放不下就直接退出
	if( pvd->size_x > area->LcdSizeX)
		return;

	//初始化处理字符索引为0
	dealIdx = 0; 
	while( dealIdx < pvd->len)
	{
	
	//取出一个字符,设置处理长度为1
		c = pvd->data[ dealIdx];
		dealNum = 1;
	//判断是否为汉字，是汉字的话处理长度设置为2
		if( IS_CHINESE( c))
		{
			dealNum = 2;
			
		}
			
	//计算本次处理需要的x轴空间
		needX = dealNum * pvd->size_x;
	//计算本次处理需要的y轴空间
		needY =  pvd->size_y;
		while(1)
		{
			//使用光标位置和屏幕的界限来计算x轴和y轴上的空闲空间 A
		
			usableX = area->LcdSizeX - area->cursorX;
			usableY = area->LcdSizeY - area->cursorY;
			//如果y轴方向放的下
			if( needY < usableY)
			{
				//如果x轴放的下，光标在x轴移动相应的位置，处理索引也移动相应的位置，退出本次处理
				//并分配空间
				if( needX < usableX)
				{
					area->cursorX += needX;
					dealIdx += dealNum;
					pvd->area_x2 += needX;
					
					break;
				}
				//如果x轴放不下，新增一行,移动光标，回到A继续判断
				else
				{
					
					area->cursorY += needY;
					pvd->area_y2 += needY;
					area->cursorX = 0;
				}
			}
			else
			//如果y轴放不下，设置需要分页标志，直接退出
			{
				pvd->more	= 1;
				return ;
			}
	
		}
	}
	

	
}

//static void VDapply(void **ppvd, void *cl)
//{
//	ViewData_t *pvd = *ppvd;
//	area_t	*area = ( area_t *)cl;

	
//	short 	num = 0;
//	short 	yLeft = 0;		//y轴上的空余空间
//	char	row = 0;
//	char	maxRow = 0;		//可分配的行数
//	char	maxinrow = 0;
//	
//	
//	
//	
//	
//	//该行允许显示的数量
//	char	allowNum = 0;
//	
//	
//	
//	//分配起始坐标为光标的当前位置
//	pvd->area_x1 = area->cursorX;
//	pvd->area_y1 = area->cursorY;
//	pvd->area_x2 = area->cursorX;
//	pvd->area_y2 = area->cursorY;
//	yLeft = area->LcdSizeY - area->cursorY ;
//	
//	//每行能够显示的数量
//	maxinrow = area->LcdSizeX / pvd->size_x;
//	maxRow = ( area->LcdSizeY -  pvd->area_y1)/ pvd->size_y;
//	//分配结束坐标
//	num = pvd->len;
//	//考虑到中文字符有两个位置，所以一定要分配偶数个位置
//	if( num & 1)
//		num ++;
//	
//	
//	//计算能够分配的数据
//	
//	//分配1行
//	row = 1;
//	yLeft -= pvd->size_y;
//	if( yLeft < 0)
//		goto exit;
//	
//	allowNum =  ( area->LcdSizeX - pvd->area_x1) / pvd->size_x;
//	//如果分配的字符数大于1个，就不允许出现奇数个字符被显示在一行的情况出现
//	if( pvd->len > 1)
//	{
//		if( allowNum & 1)
//			allowNum --;
//		
//	} 
//	while(1)
//	{	
//		
//		//不够显示的，就多加一行
//		if(  ( num > allowNum) && ( yLeft >=0))
//		{

//			num -= allowNum;
//			//增加一行
//			row ++;
//			yLeft -= pvd->size_y;
//			if( yLeft < 0)
//			{
//					//回收这一行
//				row --;
//				num = allowNum;
//				
//			}
//			else
//			{
//			
//				allowNum = maxinrow;
//			}
//			
//		}	
//		else
//		{
//			//够了的话，就计算x方向的结尾
//			
//			//与起始位置在同一行的话就要从x1处偏移
//			//不同行就不需要了
//			
//			
//			if( row == 1)
//				pvd->area_x2 = pvd->area_x1 + num * pvd->size_x ;
//			else
//				pvd->area_x2 = num * pvd->size_x ;
//			break;
//		}
//		
//		
//		
//	}
//	
//	
//	//计算y方向上的结尾
//	pvd->area_y2 = pvd->area_y1 + (row -1 )* pvd->size_y;
//	
//	exit:
//	if( yLeft < 0)
//	{
//		
//		pvd->more	= 1;
//	}
//	
//	
//	//移动光标
//	area->cursorX = pvd->area_x2 ;
//	area->cursorY = pvd->area_y2;
	
//}
//在跨行显示的时候，为了防止一个汉字被拆分显示，所以回退一格
static void VDShow(void **ppvd, void *cl)
{
	ViewData_t *pvd = *ppvd;
	Composition	*ction = ( Composition *)cl;
	//每行能够显示的量	
	short 	numInrow = 0;
	
	
	short	displayed = 0;	//已经被显示的数量
	short	 y = 0;
	short 	xend = 0 ;
	short	xstart	= 0, ableNum;
	char	c;
	
	int16_t		oldx1 = pvd->area_x1;
	int16_t		oldy1 = pvd->area_y1;
	
	//设置字体和颜色
	pvd->gh->setClu( pvd->gh, pvd->colour);
	pvd->gh->setFont( pvd->gh, pvd->font);
	pvd->gh->insert( pvd->gh, pvd->data, pvd->len);

	displayed = 0;
	for( y = pvd->area_y1; y <= pvd->area_y2; y += pvd->size_y)
	{
		//在第一行，x轴起始位置是分配区域的x轴起始位置
		if( y == pvd->area_y1)
			xstart = pvd->area_x1;
		else
			xstart = 0;
		//在最后一行，x轴的结束位置是分配区域的结束位置
		if( y == pvd->area_y2 || pvd->area_y1 == pvd->area_y2)
			xend = pvd->area_x2;
		else
			xend = ction->lcdArea.LcdSizeX;
		
		ableNum = ( xend - xstart) / pvd->size_x;
		
		//计算这一行能显示几个字符
		numInrow  = 0;
		while(1)
		{
			c = pvd->data[ displayed];
			
			if( IS_CHINESE( c))
			{
				if( ableNum > 1)
				{
					numInrow += 2;
					displayed += 2;
					ableNum -= 2;
				}
				
				
				
			}
			else
			{
				if( ableNum >  0)
				{
					numInrow ++;
					displayed ++;
					ableNum -= 1;
				}	
				
			}
			//如果显示够了就退出
			if( displayed >= pvd->len)
				break;
			//如果本行已经没有空间了，就显示本行
			if( ableNum == 0)
				break;
			//如果本行还能显示一个字符，看看下一个是不是能放进去了
			if( ableNum == 1)
			{		
				c = pvd->data[ displayed];
				if( !IS_CHINESE( c))
				{
					displayed ++;
					numInrow ++;
				}
				break;
			}
				
		}
		pvd->gh->draw( pvd->gh, pvd->area_x1, pvd->area_y1, numInrow);
		pvd->area_x1 = 0;
		pvd->area_y1 += pvd->size_y;
		
	}
	
	
	pvd->done = displayed;
	
	
	//恢复起始位置，方便下次显示
	pvd->area_x1 = oldx1;
	pvd->area_y1 = oldy1;

	
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

