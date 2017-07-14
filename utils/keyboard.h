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
//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define NUM_KEYS			6
#define MAX_OBS			4

//顺序与硬件上的引脚连接保持一致
#define	KEYCODE_RIGHT	0x01
#define	KEYCODE_LEFT	0x02
#define	KEYCODE_UP		0x03
#define	KEYCODE_DOWN	0x04
#define	KEYCODE_ENTER	0x05
#define	KEYCODE_ESC		0x06

#define	KEYGPIOID_RIGHT	DEVID_GPIO_D0
#define	KEYGPIOID_LEFT	DEVID_GPIO_D1
#define	KEYGPIOID_UP	DEVID_GPIO_D2
#define	KEYGPIOID_DOWN	DEVID_GPIO_D3
#define	KEYGPIOID_ENTER	DEVID_GPIO_D4
#define	KEYGPIOID_ESC	DEVID_GPIO_D5

#define KEYEVENT_HIT		0x10
#define KEYEVENT_RLS		0x80			//释放
#define KEYEVENT_DHIT		0x30			//双击
#define KEYEVENT_LPUSH		0x40			//长按

//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------
typedef int (*keyHdl)(  uint8_t keyEven, char numKey, uint8_t arrKeyCode[]);

INTERFACE( keyObservice)
{
	
	int (*update)( keyObservice *self, uint8_t keyEven, char numKey, uint8_t arrKeyCode[]);
	
};

typedef struct {
	
	keyObservice *p_ob;
	char		id;
	char		flag;
	char		none[2];
}keyObM_t;

CLASS( Keyboard)
{
	keyObM_t 	arr_p_obm[MAX_OBS] ;
	I_dev_Char	*arr_p_devGpio[ NUM_KEYS];
	int (*init)( Keyboard *self, IN void *arg);
	int (*addOb)( Keyboard *self, keyObservice *ob);
	int	(*delOb)( Keyboard *self, char id);
	void (*run)( Keyboard *self);
	
	void	(*notify)( Keyboard *self);
	
	
	
};

CLASS( KbTestOb)
{
	IMPLEMENTS( keyObservice);
	keyHdl hdl;
	void (*setKeyHdl)( KbTestOb *self, keyHdl hdl);
};
//------------------------------------------------------------------------------
// Type definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
Keyboard *GetKeyInsance( void);
#endif
