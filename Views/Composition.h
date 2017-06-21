#ifndef __MODEL_H_
#define __MODEL_H_
#include <stdint.h>
#include "sdhDef.h"

#include "arithmetic/list.h"
#include "lw_oopc.h"
#include "Glyph.h"


#define SET_CHILDASWHOLE(att)		(	att | 1)	//子元素必须作为一个整体
#define IS_CHILDASWHOLE(att)		(	att & 1)	//
#define SET_CHILDFIRST(att)		(	att | 2)	//在分配屏幕空间时，子元素优先，赋予胺素要去配合子元素
#define IS_CHILDFIRST(att)		(	att & 2)	//

#define STEP_LAYOUT		0  //布局阶段
#define STEP_SHOW		1  //显示阶段




//typedef struct {
//	
//	int16_t		LcdSizeX;
//	int16_t		LcdSizeY;
//	//光标位置
//	int16_t		cursorX;
//	int16_t		cursorY;

//}area_t;
typedef int (*dynDataFunc)( IN void *pData, IN int option, OUT void **ppResult);
typedef struct {
	
	void	*pDyndata;
	dynDataFunc	func;

}dynDataHdl_t;

typedef struct {
	void 			*paraent;
	List_T			t_childen;
	
	dspArea_t		dspArea;
	dspContent_t	dspCnt;
	
	Glyph		*gh;
	
	//这是用在父子元素中，子元素的间距
	uint8_t			lineSpacing;
	uint8_t			columnGap;
		//有些图元的对齐可以自动处理，如LABL
	//有些则不行，一般组合的图元要自己处理对齐
	//在分配显示区域时通过这个标志来判断
	uint8_t			notDealAli;
	
	//每行的列数，为0表示不做要求,一列只能显示一个子vd
	uint8_t			cols;
	//一行的子vd数计数器
	uint8_t			colcount;
	char			donotUseChldSize;	//不需要通过子图元来确定自己的尺寸
	uint16_t		tmpSize;
	
	//只有需要动态刷新的显示数据才需要实现这个接口
	int (*updateCnt)(void **ppvd, void *cl);
	
	dynDataHdl_t	*pdyndHdl;
	
}ViewData_t;

CLASS( Composition)
{
	List_T				t_vd;
	List_T				t_dynVd;		//动态显示部分
	void				*ctor;
	void				**dynPtr;			//动态数据指针，通过id作为下标来查找
	scInfo_t			mySCI;
	
//	scArea_t		availableArea;
	//每个图元之间的间距
	uint8_t			lineSpacing;
	uint8_t			columnGap;
	uint8_t			none[2];

	
	
	void ( *setCtor)( Composition *self, void *ctor);
	void ( *setSCBkc)( Composition *self, char* bkc);
	
	ViewData_t	*( *allocVD)( Composition *self);
	int ( *insertVD)( Composition *self, ViewData_t *faVd, ViewData_t *vd);
	int ( *insertDynVD)( Composition *self, ViewData_t *faVd, ViewData_t *vd);
	
	int ( *clean)( Composition *self);
	int ( *flush)( Composition *self);
	
	
	int ( *dynShow)( Composition *self);
};


Composition *Get_Composition(void);

void UsePrntAttIfNeed( ViewData_t *prnt, ViewData_t *vd);
//dspArea_t* GetVdParaent_area( ViewData_t *vd);
//dspContent_t* GetVdParaent_cnt( ViewData_t *vd);

#endif
