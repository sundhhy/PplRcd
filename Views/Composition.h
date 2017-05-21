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



typedef struct {
	
	int16_t		LcdSizeX;
	int16_t		LcdSizeY;
	//光标位置
	int16_t		cursorX;
	int16_t		cursorY;

}area_t;

typedef struct {
	void 		*paraent;
	List_T		t_childen;
	
	char		*data;
	uint16_t 	len;
	int8_t		font;
	int8_t		colour;
	//每个显示元素的尺寸
	uint16_t		size_x;
	uint16_t		size_y;
	
	//记录显示的数据被所分配的显示区域
	int16_t		area_x1;
	int16_t		area_y1;
	int16_t		area_x2;
	int16_t		area_y2;
	
	Glyph		*gh;
	
	char		more;		//需要分页显示
	short		done;		//是否显示好了
	uint8_t		childAttr;		//子元素的特性，如是否可以被分到两行显示，尺寸分配是否以子元素优先
	
}ViewData_t;

CLASS( Composition)
{
	List_T				t_vd;
	uint16_t			x;
	uint16_t			y;
	void				*ctor;
	
	uint16_t	lcdWidth, lcdHeight;
	uint16_t	ghWidth, ghHeight;
	
	area_t		lcdArea;
	
	
	void ( *setCtor)( Composition *self, void *ctor);
	
	ViewData_t	*( *allocVD)( Composition *self);
	int ( *insertVD)( Composition *self, ViewData_t *faVd, ViewData_t *vd);
	
	int ( *clean)( Composition *self);
	int ( *flush)( Composition *self);
	
	
	int ( *insert)( Composition *self, Glyph *gh);
	int ( *addRow)( Composition *self);
};


Composition *Get_Composition(void);

#endif
