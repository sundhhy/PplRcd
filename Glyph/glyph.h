#ifndef __GLYPH_H_
#define __GLYPH_H_
#include "lw_oopc.h"
#include <stdint.h>
#include "device.h"


#define IS_CHINESE( c) ( c & 0x80)
#define IS_BR( c) ( c == '\n')
#define IS_TAB( c) ( c == '\r')



//文本类的子类型
#define TEXT_ST_TEXT					0
#define TEXT_ST_LABLE					1    //子类型

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
	uint8_t		id;
}dspContent_t;



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


ABS_CLASS( Glyph)
{
	

	I_dev_lcd	*myLcd ;
	int (*init)( Glyph *self, I_dev_lcd *lcd);
	int (*setFont)( Glyph *self, int font);
	int (*setClu)( Glyph *self, int c);
	int (*setBgC)( Glyph *self, int c);
	//abs func
	//
	abstract int ( *getSize)(Glyph *self, int font, uint16_t *size_x, uint16_t *size_y);
	
	//---------------------------------------
	abstract int (*clean)( Glyph *self);
//	abstract void (*insert)( Glyph *self, void *context, int len);
	
	abstract void (*draw)( Glyph *self, dspContent_t *cnt, dspArea_t *area);		
	
	
	abstract void (*flush)( Glyph *self, int x, int y);
//	abstract int ( *getWidth)(Glyph *self);
//	abstract int ( *getHeight)(Glyph *self);
//	abstract int ( *setWidth)(Glyph *self, uint16_t wd);
//	abstract int ( *setHeight)(Glyph *self, uint16_t he);
//	abstract int ( *getNum)(Glyph *self);
	
	
};
void View_test(void);

int	String2Bkc( char *s);
int String2Clr( char *s);

int String2Colour( char *s);
int	DefaultColour( void *arg);


int String2Font( char *s);
int	String2Align( char *s);

int	String2CntEff( char *s);

int GetKeyVal( char *s, char *key, char *val, short size);

#endif
