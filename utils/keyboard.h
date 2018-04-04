#ifndef __INC_keyboard_H
#define __INC_keyboard_H
//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#include "lw_oopc.h"
#include "basis/macros.h"
#include "utils/time.h"
#include <stdint.h>
#include "device.h"
#include "system.h"
//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------


#define NUM_KEYS			6
#define MAX_OBS				4
#define NUM_KEFIFO			16	//必须是2的幂

//顺序与硬件上的引脚连接保持一致
#define	KEYCODE_RIGHT	0x01
#define	KEYCODE_LEFT	0x02
#define	KEYCODE_UP		0x03
#define	KEYCODE_DOWN	0x04
#define	KEYCODE_ENTER	0x05
#define	KEYCODE_ESC		0x06
#define KEYCODE_NONE				0xff
#define IS_LEGAL_KCD(k)			(k >= KEYCODE_RIGHT && k <= KEYCODE_ESC)


//根据原理图来设置
/*
按键位置：
上
下 左 右 确认 ESC
*/
//#define	KEYGPIOID_RIGHT		DEVID_GPIO_D2
//#define	KEYGPIOID_LEFT		DEVID_GPIO_D3
//#define	KEYGPIOID_UP		DEVID_GPIO_D5
//#define	KEYGPIOID_DOWN		DEVID_GPIO_D4
//#define	KEYGPIOID_ENTER		DEVID_GPIO_D0
//#define	KEYGPIOID_ESC		DEVID_GPIO_D1


/*
按键位置：
ESC
上 下 左 右 确认
*/

#define	KEYGPIOID_RIGHT		DEVID_GPIO_D0
#define	KEYGPIOID_LEFT		DEVID_GPIO_D2
#define	KEYGPIOID_UP		DEVID_GPIO_D4
#define	KEYGPIOID_DOWN		DEVID_GPIO_D3
#define	KEYGPIOID_ENTER		DEVID_GPIO_D1
#define	KEYGPIOID_ESC		DEVID_GPIO_D5


//原始事件
#define KEY_PUSH			0x01
#define KEY_RLS				0x02			//释放
#define KEY_ERR				0x05			//按键事件接收时出现了错误，是由于入队列失败产生的

#define KEY_UNKONW			0x03

//由原始事件运算后得出的事件
#define KEYEVENT_HIT		0x10
#define KEYEVENT_DHIT		0x20			//双击
#define KEYEVENT_LPUSH		0x30			//长按
#define KEYEVENT_UP			0x80
#define KEYEVENT_ERR		0x40

#define IS_KEYUP( eventcode) ( eventcode & KEYEVENT_UP)
//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------

//发送给用户程序
typedef struct {
	uint8_t		keyCode;
	uint8_t		eventCode;
	uint8_t		none[2];
}keyMsg_t;

typedef int (*keyHdl)(  char num, keyMsg_t arr_msg[]);


INTERFACE( keyObservice)
{
	
	int (*update)( keyObservice *self,  uint8_t num, keyMsg_t arr_msg[]);
	
};

typedef struct {
	
	keyObservice *p_ob;
	char		id;
	char		flag;
	char		none[2];
}keyObM_t;

#if CONF_KEYSCAN_POLL == 0
typedef struct{
	
	uint64_t	timestamp;
	uint8_t		event;	
	uint8_t		key;
	uint8_t		none[2];
	
}keyevent_t;

typedef struct {
	uint8_t		keyCode;
	uint8_t		eventCode;
	uint16_t	pushtime_ms;		//
	uint64_t	timestamp;

	
	
}keyStatus_t;

typedef struct {
	
	keyevent_t arr_ke[ NUM_KEFIFO];
	uint8_t		write, read;
	uint8_t		size;		//size 必须是2的幂
	uint8_t		none;
}KEFifo_t;
#else 
typedef struct {
	uint8_t		key_code;
	uint8_t		up_flag;			//用于记录： 单击是否已经被上报过了
	uint16_t	long_push_delay;		//长按一次以后，下一次上报要延迟一段时间
	uint32_t	sum_count;
	uint32_t	last_pressed_count;
}key_pin_info_t;

#endif





CLASS( Keyboard)
{
	keyObM_t 		arr_p_obm[MAX_OBS] ;
	I_dev_Char		*arr_p_devGpio[ NUM_KEYS];
#if CONF_KEYSCAN_POLL == 0
	keyStatus_t		arr_ks[ NUM_KEYS];
	KEFifo_t		kef;
#else
	key_pin_info_t	arr_key_pins[NUM_KEYS];
#endif
	
	uint32_t		run_count;			//run 运行的次数
	uint32_t		cycle_ms;			//运行的周期
	int (*init)(Keyboard *self, IN void *arg);
	int (*addOb)(Keyboard *self, keyObservice *ob);
	int	(*delOb)(Keyboard *self, char id);
	void (*run)(Keyboard *self);
	//privice
	void	(*notify)( Keyboard *self, uint8_t num, keyMsg_t arr_msg[]);
	void	(*scan_key_pins)(Keyboard *self);
	void	(*identify_key_msg)(Keyboard *self, keyMsg_t *p_key_msg, int *num);
};

CLASS(KbTestOb)
{
	IMPLEMENTS(keyObservice);
	keyHdl hdl;
	void (*setKeyHdl)(KbTestOb *self, keyHdl hdl);
};
//------------------------------------------------------------------------------
// Type definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
Keyboard *GetKeyInsance( void);
void Keycode2Str( uint8_t keycode, int buflen, char *buf);
void Keyevnet2Str( uint8_t eventCode, int buflen, char *buf);
#endif
