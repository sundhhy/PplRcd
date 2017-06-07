#ifndef __MODEL_H_
#define __MODEL_H_
#include <stdint.h>
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
	uint8_t			dealAli;
	uint8_t			none;
	
	
	
	
//	char		*data;
//	uint16_t 	len;
//	int8_t		font;
//	int8_t		colour;
//	//每个显示元素的尺寸
//	uint16_t		size_x;
//	uint16_t		size_y;
//	
//	//记录显示的数据被所分配的显示区域
//	int16_t		area_x1;
//	int16_t		area_y1;
//	int16_t		area_x2;
//	int16_t		area_y2;
//	

//	
//	char		more;		//需要分页显示
//	short		done;		//是否显示好了
//	uint8_t		childAttr;		//子元素的特性，如是否可以被分到两行显示，尺寸分配是否以子元素优先
	
}ViewData_t;

CLASS( Composition)
{
	List_T				t_vd;
	void				*ctor;
	
	scInfo_t		mySCI;
	//每个图元之间的间距
	uint8_t			lineSpacing;
	uint8_t			columnGap;
	uint8_t			none[2];

	
//	area_t		lcdArea;
	
	//行和列的尺寸，一般等于前一个显示元素的宽和高，在加行或者增加空格的时候使用
	//在显示和布局时的行和列的尺寸存储在不同的参数中
//	uint8_t	rowSize[2], colSize[2];
//	char	step;	//当前阶段：布局 0 显示 1	
//	char	none[3];
	
	
	void ( *setCtor)( Composition *self, void *ctor);
	void ( *setSCBkc)( Composition *self, char* bkc);
	
	ViewData_t	*( *allocVD)( Composition *self);
	int ( *insertVD)( Composition *self, ViewData_t *faVd, ViewData_t *vd);
	
	int ( *clean)( Composition *self);
	int ( *flush)( Composition *self);
	
	
//	int ( *insert)( Composition *self, Glyph *gh);
	int ( *addRow)( Composition *self);
};


Composition *Get_Composition(void);

#endif
