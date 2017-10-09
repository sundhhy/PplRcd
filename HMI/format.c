#include "format.h"
#include <string.h>
//提供 按键，事件，消息，窗口，报警，时间，复选框的图层
//这些图层可能会被其他界面所使用
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
//typedef void (*formatX)( uint16_t *freeSize, sheet *p_dealSht);

////y轴的长度是根据改行最大的哪个来定的
//typedef void (*formaty)( uint16_t colSize, uint16_t *freeSize, sheet *p_dealSht);
//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
//static void DealXAlignLeft( uint16_t *freeSize, sheet *p_dealSht);
//static void DealXAlignMid( uint16_t *freeSize, sheet *p_dealSht);
//static void DealXAlignRight( uint16_t *freeSize, sheet *p_dealSht);
//static void DealYAlignLeft( uint16_t colSize, uint16_t *freeSize, sheet *p_dealSht);
//static void DealYAlignMid( uint16_t colSize, uint16_t *freeSize, sheet *p_dealSht);
//static void DealYAlignRight(  uint16_t colSize, uint16_t *freeSize, sheet *p_dealSht);
//static const formatX func_dealX[ALIGN_MAX] = { DealXAlignLeft, DealXAlignMid, DealXAlignRight};
//static const formaty func_dealY[ALIGN_MAX] = { DealYAlignLeft, DealYAlignMid, DealYAlignRight};

static int CoordinateCalculation( int curCrdn, uint16_t freesize, uint16_t needsize, char total, char align, uint8_t *tail_idle);
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
int FormatSheet( const hmiAtt_t *p_hmiAtt, video_t *p_v, sheet **pp_shts)
{
	sheet **pp_shtRow0;
	sheet *p_shtCol;
	
	uint8_t i = 0, j = 0;
	uint8_t	count_sht = 0;
	uint8_t	empty;
	uint8_t colSize[NUMROW_MAX] = {0};		
	
	short sumXsize;
	short sumYsize;
	
	short freeXsize;
	short freeYsize;
	
	int	crdn = 0;
	int	crdny = 0;
	

	if( p_hmiAtt->numRow > NUMROW_MAX)
		goto err;
	
	
	
	//
	sumYsize = 0;
	for( i = 0; i < p_hmiAtt->numRow; i++)
	{
		pp_shtRow0 = pp_shts + i * p_hmiAtt->numCol;
		
		
		sumXsize = 0;
		count_sht = 0;

		//计算一行的图形的总长度，然后根据总长度来计算每个图层的坐标
		for( j = 0; j < p_hmiAtt->numCol; j++)
		{
			p_shtCol = pp_shtRow0[j];
			if( p_shtCol == NULL)
				continue;
			
			if( p_shtCol->bxsize == SIZE_BOUNDARY)
				p_shtCol->bxsize = p_v->vxsize;
			if( p_shtCol->bysize == SIZE_BOUNDARY)
				p_shtCol->bysize = p_v->vysize;
			
			if( colSize[i] < p_shtCol->bysize)
				colSize[i] = p_shtCol->bysize; 
			
			sumXsize += p_shtCol->bxsize;
			count_sht ++;
			
			//多个图层之间才需要插入空隙
			if( j)
			{
				sumXsize += p_hmiAtt->colGrap;
			}
				
		}
		sumYsize += colSize[i];
		//多个图层之间才需要插入空隙
		if( i)
		{
			sumYsize += p_hmiAtt->rowGrap;
		}
		
		//根据总长来进行格式化
		freeXsize = p_v->vxsize;
		crdn = 0;
		
		for( j = 0; j < p_hmiAtt->numCol; j++)
		{
			p_shtCol = pp_shtRow0[j];
			if( p_shtCol == NULL)
				continue;
			if( p_shtCol->area.alix >= ALIGN_MAX)
			{
				break;
			}
			crdn = CoordinateCalculation( crdn, freeXsize, sumXsize, count_sht, p_shtCol->area.alix, &empty);
			if( crdn < 0)
				break; 
			p_shtCol->area.x0 = crdn;
			p_shtCol->area.x1 = crdn + p_shtCol->bxsize;
			
			count_sht --;
			sumXsize = sumXsize - p_shtCol->bxsize -  p_hmiAtt->colGrap;
			
			crdn +=  p_shtCol->bxsize +  p_hmiAtt->colGrap + empty;
			freeXsize  = p_v->vxsize - crdn;		
		}
	}
	
	//计算vy
	freeYsize = p_v->vysize;
	crdn = 0;
	
	crdny = 0;
	count_sht = p_hmiAtt->numRow;
	for( i = 0; i < p_hmiAtt->numRow; i++)
	{
		pp_shtRow0 = pp_shts + i * p_hmiAtt->numCol;
		
		//给这一行的每个图层的y坐标赋值
		for( j = 0; j < p_hmiAtt->numCol; j++)
		{
			p_shtCol = pp_shtRow0[j];
			if( p_shtCol == NULL)
				continue;
			if( p_shtCol->area.alix >= ALIGN_MAX)
			{
				break;
			}
			crdn = CoordinateCalculation( crdny, freeYsize, p_shtCol->bysize, count_sht, p_shtCol->area.aliy,  &empty);
			if( crdn < 0)
				continue; 
			p_shtCol->area.y0 = crdn;
			p_shtCol->area.y1 = crdn + p_shtCol->bysize;	
			 
		}
		
		//去掉这一行
		sumYsize = sumYsize - colSize[i] -  p_hmiAtt->rowGrap;
		count_sht --;
		
		crdny += colSize[i] +  p_hmiAtt->rowGrap + empty;
		freeYsize  = p_v->vysize  - crdny;
		
		
		
		
	}
	
	return RET_OK;
	err:

		return ERR_OPT_FAILED;
	
}

void FormatSheetSub( sheet *p_sht)
{
	sheet **pp_shtRow0;
	sheet *p_shtCol;
	video_t	v;
	hmiAtt_t att;
	short i = 0, j = 0;

	if( p_sht == NULL || p_sht->pp_sub == NULL)
		return;
	
	v.vxsize = p_sht->area.x1 - p_sht->area.x0 - p_sht->area.offset_x * 2;
	v.vysize = p_sht->area.y1 - p_sht->area.y0 - p_sht->area.offset_y * 2;
	
	att.numCol = p_sht->subAtt.numSubCol;
	att.numRow = p_sht->subAtt.numSubRow;
	att.colGrap = p_sht->subAtt.subColGrap;
	att.rowGrap = p_sht->subAtt.subRowGrap;
	
	FormatSheet( &att, &v, p_sht->pp_sub);
	
	//修正位置
	for( i = 0; i < p_sht->subAtt.numSubRow; i++)
	{
		pp_shtRow0 =  p_sht->pp_sub + i * att.numCol;
		for( j = 0; j < p_sht->subAtt.numSubCol; j++)
		{
			p_shtCol = pp_shtRow0[j];
			if( p_shtCol == NULL)
				continue;
			p_shtCol->area.x0 += p_sht->area.x0 + p_sht->area.offset_x;
			p_shtCol->area.x1 += p_sht->area.x0 + p_sht->area.offset_x;
			p_shtCol->area.y0 += p_sht->area.y0 + p_sht->area.offset_y;
			p_shtCol->area.y1 += p_sht->area.y0 + p_sht->area.offset_y;
		}
		
	}


}
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//

static int CoordinateCalculation( int curCrdn, uint16_t freesize, uint16_t needsize, char total, char align, uint8_t *tail_idle)
{
	uint16_t offset = 0;
	short	idle = freesize - needsize;
	if( freesize < needsize)
		return -1;
	if( total)
		idle = idle / total;
	*tail_idle = 0;
	
	if( align == ALIGN_MIDDLE)
	{
		offset = ( idle) >> 1;
		*tail_idle = offset;
	}
	else if( align == ALIGN_RIGHT)
	{
		offset =  idle;
	}
	else
	{
		offset = 0;
	}
	
	
	
	return ( curCrdn + offset);
	
}

//static void DealXAlignMid( uint16_t *freeSize, sheet *p_dealSht)
//{
//	
//}

//static void DealXAlignRight( uint16_t *freeSize, sheet *p_dealSht)
//{
//	
//}

//static void DealYAlignLeft( uint16_t cloSize, uint16_t *freeSize, sheet *p_dealSht)
//{
//	
//}

//static void DealYAlignMid( uint16_t cloSize, uint16_t *freeSize, sheet *p_dealSht)
//{
//	
//}

//static void DealYAlignRight( uint16_t cloSize, uint16_t *freeSize, sheet *p_dealSht)
//{
//	
//}

















