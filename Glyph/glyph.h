#ifndef __GLYPH_H_
#define __GLYPH_H_
#include "lw_oopc.h"
#include <stdint.h>
#include "device.h"




//文本类的子类型
#define TEXT_ST_TEXT					0
#define TEXT_ST_LABLE					1    //子类型
//#define TEXT_ST_UNTIL					2    //单位

#define EFF_FOCUS				1  //被选选中
#define EFF_HIDE				2  //隐藏
#define EFF_BKPIC				4  //具有背景图片的属性,这种情况下，bkc作为背景图片的编号

#define GP_CKECK_EFF( eff, t)  ( eff & t)
#define GP_SET_EFF( eff, t)  ( eff | t)
#define GP_CLR_EFF( eff, t)  ( eff & ~t)


//几何类的子类型
//#define GMTR_ST_RCT					1    //矩形
//#define GMTR_ST_ROUNDRCT				2    //圆角矩形
//#define GMTR_ST_ROUND				3    //圆

#define LCD_DEVID		DEVID_USARTGPULCD


typedef struct {
	char 		*data;
	uint16_t	len;
	uint8_t		colour;
	char		font;

	//特效
	uint8_t		effects;
	uint8_t		bkc;			//背景色
	
	uint8_t		subType;
	uint8_t		none;
//	uint8_t		mdl_aux;		//模型的功能参数
}dspContent_t;

typedef struct {
	int16_t		x0, y0;
	int16_t		x1, y1;
	int8_t		alix;
	int8_t		aliy;
	
	//在切割图片的时候，用来表示屏幕上的坐标与图形上的坐标的偏移
	//或者子图层在父图层上的偏移
	int8_t		offset_x;
	int8_t		offset_y;
	//privice
	uint8_t		grap;			//正方形的内部和边框的间距
	
	//icon使用的额外参数
	uint8_t		xn;
	uint8_t		yn;
	uint8_t		n;
}vArea_t;



/*                   @Deprecated   */
typedef struct {
	int16_t		cursorX;
	int16_t		cursorY;
	
	int16_t		rowSize;
	int8_t		colSize;

	int8_t		scBkc;
	scArea_t	scBoundary;
}scInfo_t;



typedef struct {
	
	scInfo_t	*curScInfo;
	
	scArea_t	*boundary;		//在设置这个值的时候，如果有私有的界限就指向私有界限，否则的话就指向整个屏幕的界限
	scArea_t	useArea;
	
		//单个元素的x y轴尺寸
	uint16_t		sizeX;
	uint16_t		sizeY;
	
	uint8_t		cursorX;		//图元自己的光标		
	uint8_t		cursorY;			
//	uint8_t		numRow;			//行的总数
	uint8_t		ali;		//默认左对齐
	uint8_t		aliy;
}dspArea_t;

/**************************************************************/

ABS_CLASS( Glyph)
{
	

	I_dev_lcd	*myLcd ;
	
	
	
	abstract int ( *getSize)(Glyph *self, int font, uint16_t *size_x, uint16_t *size_y);
	abstract void (*vdraw)( Glyph *self, dspContent_t *cnt, vArea_t *area);
	
	
	/*          170805         @Deprecated   */
	
	int (*init)( Glyph *self, I_dev_lcd *lcd);
	void (*draw_self)(Glyph *self);

//	int (*setFont)( Glyph *self, int font);
//	int (*setClu)( Glyph *self, int c);
//	int (*setBgC)( Glyph *self, int c);
	//abs func
	//
	
	
		
//	abstract int (*clean)( Glyph *self);
//	abstract void (*insert)( Glyph *self, void *context, int len);
//	
//	abstract void (*draw)( Glyph *self, dspContent_t *cnt, dspArea_t *area);		
	
	
//	abstract void (*flush)( Glyph *self, int x, int y);
//	abstract int ( *getWidth)(Glyph *self);
//	abstract int ( *getHeight)(Glyph *self);
//	abstract int ( *setWidth)(Glyph *self, uint16_t wd);
//	abstract int ( *setHeight)(Glyph *self, uint16_t he);
//	abstract int ( *getNum)(Glyph *self);
	
	
};

void Flush_LCD(void);
void Stop_flush_LCD(void);
void LCD_Run(void);
void CLR_LCD(void);
uint8_t ColorInvert( uint8_t clr);
int	String2Bkc( char *s);
int String2Clr( char *s);

int String2Colour( char *s);
int	DefaultColour( void *arg);


int String2Font( char *s);
int	String2Align( char *s);

int	String2CntEff( char *s);

int GetKeyVal( char *s, char *key, char *val, short size);
int Set_effects(dspContent_t *p_cnt, int eff, int val) ;
#endif
