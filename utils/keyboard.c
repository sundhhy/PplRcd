#include <string.h>
#include "keyboard.h"

#include "sdhDef.h"
#include "GPIO/drive_gpio.h"
#include "os/os_depend.h"

#include "time.h"

//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define KEYCODE_STR_RIGHT		"key_right"
#define KEYCODE_STR_LEFT		"key_left"
#define KEYCODE_STR_UP			"key_up"
#define KEYCODE_STR_DOWN		"key_down"
#define KEYCODE_STR_ENTER		"key_ENTER"
#define KEYCODE_STR_ESC			"key_ESC"

#define KEYEVENT_STR_HIT		"hit"
#define KEYEVENT_STR_DHIT		"double hit"
#define KEYEVENT_STR_LPUSH		"long push"

#define	BEEP_ON					GPIO_SetBits(GPIO_PORT_BEEP, GPIO_PIN_BEEP)
#define	BEEP_OFF				GPIO_ResetBits(GPIO_PORT_BEEP, GPIO_PIN_BEEP)

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
//#define  DOUBLEHIT_TIME_MS		300
//#define  LONGPUSH_TIME_MS		2000
//#define  REPORT_TIME_MS			500


#define  LONGPUSH_TIME_MS		1000

#define  DOUBLEHIT_TIME_MS		160
#define  REPORT_TIME_MS			200

const uint32_t	arr_keyGpioID[ NUM_KEYS] = { KEYGPIOID_RIGHT, KEYGPIOID_LEFT, \
	KEYGPIOID_UP, KEYGPIOID_DOWN, KEYGPIOID_ENTER, KEYGPIOID_ESC};

const uint8_t	arr_keyCode[ NUM_KEYS] = { KEYCODE_RIGHT, KEYCODE_LEFT, \
	KEYCODE_UP, KEYCODE_DOWN, KEYCODE_ENTER, KEYCODE_ESC};

#if CONF_KEYSCAN_POLL == 0
static keyMsg_t arr_keyMsg[ NUM_KEYS];
#endif
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
#if CONF_KEYSCAN_POLL == 0
static int first = 1;
#endif
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
/* Cycle/Sync Callback functions */

static int Keyboard_init( Keyboard *self, IN void *arg);
static int Key_addOb( Keyboard *self, keyObservice *ob);
static int Key_DelOb( Keyboard *self, char obId);
static void	Key_Run( Keyboard *self);
static void	Key_notify( Keyboard *self, uint8_t num, keyMsg_t arr_msg[]);
static void Key_scan_pins(Keyboard *self);
static void	Key_identify_key_msg(Keyboard *self, keyMsg_t *p_key_msg, int *num);

	
static int Key_TestUpdate( keyObservice *self,  uint8_t num, keyMsg_t arr_msg[]);
static void KeyTest_setKeyHdl( KbTestOb *self, keyHdl hdl);

#if CONF_KEYSCAN_POLL == 0
static  void GpioIrqHdl( void *arg, int type, int encode);
static keyStatus_t *findPks( keyStatus_t arrKs[], uint8_t keycode);
static void KeyPush( keyStatus_t *p_ks, keyevent_t *p_ke);
static void KeyRls( keyStatus_t *p_ks, keyevent_t *p_ke);
static void KEFifo_Reset ( KEFifo_t *p_kef);
static int PopKE( KEFifo_t *p_kef, keyevent_t *p_ke);
static int PushKE( KEFifo_t *p_kef, keyevent_t *p_ke);
static void KEFifo_Init ( KEFifo_t *p_kef, int size);
static int	KEFifo_len( KEFifo_t *p_kef);
static void KeyErr( Keyboard *self);

#endif


//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

Keyboard *GetKeyInsance( void)
{
	static Keyboard *signalKey = NULL;
	if( signalKey == NULL)
	{
		signalKey = Keyboard_new();
		if(signalKey == NULL)
			while(1);
	}
	
	return signalKey;
	
}

void Keycode2Str( uint8_t keycode, int buflen, char *buf)
{
	switch( keycode)
	{
		case KEYCODE_RIGHT:
			strncpy( buf, KEYCODE_STR_RIGHT, buflen);
			break;	
		case KEYCODE_LEFT:
			strncpy( buf, KEYCODE_STR_LEFT, buflen);
			break;
		case KEYCODE_UP:
			strncpy( buf, KEYCODE_STR_UP, buflen);
			break;
		
		case KEYCODE_DOWN:
			strncpy( buf, KEYCODE_STR_DOWN, buflen);
			break;
		
		case KEYCODE_ENTER:
			strncpy( buf, KEYCODE_STR_ENTER, buflen);
			break;
		case KEYCODE_ESC:
			strncpy( buf, KEYCODE_STR_ESC, buflen);
			break;
		default:
			strncpy( buf, "key ukown", buflen);
			break;
		
	}
	
}

void Keyevnet2Str( uint8_t eventCode, int buflen, char *buf)
{
	eventCode &= ~KEYEVENT_UP;
	switch( eventCode)
	{
		case KEYEVENT_HIT:
			strncpy( buf, KEYEVENT_STR_HIT, buflen);
			break;	
		case KEYEVENT_DHIT:
			strncpy( buf, KEYEVENT_STR_DHIT, buflen);
			break;
		case KEYEVENT_LPUSH:
			strncpy( buf, KEYEVENT_STR_LPUSH, buflen);
			break;
		
		
		default:
			if( IS_KEYUP( eventCode))
				strncpy( buf, "up", buflen);
			else
				sprintf( buf, "errcode[%02x]", eventCode);
			break;
		
	}
	
}

CTOR( Keyboard)
FUNCTION_SETTING(init, Keyboard_init);
FUNCTION_SETTING(addOb, Key_addOb);
FUNCTION_SETTING(delOb, Key_DelOb);
FUNCTION_SETTING(run, Key_Run);
FUNCTION_SETTING(notify, Key_notify);
FUNCTION_SETTING(scan_key_pins, Key_scan_pins);
FUNCTION_SETTING(identify_key_msg, Key_identify_key_msg);
END_CTOR

CTOR( KbTestOb)
FUNCTION_SETTING(keyObservice.update, Key_TestUpdate);
FUNCTION_SETTING(setKeyHdl, KeyTest_setKeyHdl);



END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{

static int Keyboard_init(Keyboard *self, IN void *arg)
{
	int i ;
	
#if CONF_KEYSCAN_POLL == 1
	uint16_t	cyc_ms = *((uint16_t *)arg);
	self->run_count = 0;
	self->cycle_ms = cyc_ms;
	
	for( i = 0; i < NUM_KEYS; i++)
	{
		if( Dev_open( arr_keyGpioID[ i], ( void *)&self->arr_p_devGpio[ i]))
			goto err;
		
		self->arr_p_devGpio[ i]->ioctol( self->arr_p_devGpio[ i], DEVGPIOCMD_SET_ENCODE, arr_keyCode[i]);
		
		memset( self->arr_key_pins + i, 0, sizeof(key_pin_info_t));
		self->arr_key_pins[i].key_code = arr_keyCode[i];
		self->arr_key_pins[i].last_pressed_count = 0;
		self->arr_key_pins[i].sum_count = 0;
		self->arr_key_pins[i].up_flag = 0;
	}
	return RET_OK;
#else
	
	if( first == 0)
		return ERR_BEEN_INSTALL;

	for( i = 0; i < NUM_KEYS; i++)
	{
		if( Dev_open( arr_keyGpioID[ i], ( void *)&self->arr_p_devGpio[ i]))
			goto err;
		
		self->arr_p_devGpio[ i]->ioctol( self->arr_p_devGpio[ i], DEVCMD_SET_IRQHDL, GpioIrqHdl, (void *)self);
		self->arr_p_devGpio[ i]->ioctol( self->arr_p_devGpio[ i], DEVGPIOCMD_SET_ENCODE, arr_keyCode[i]);
		
		memset( self->arr_ks + i, 0, sizeof( keyStatus_t));
		self->arr_ks[i].keyCode = arr_keyCode[i];
		
	}
	for( i = 0; i < MAX_OBS; i++)
	{
		self->arr_p_obm[i].flag = 0;
	}
	KEFifo_Init( &self->kef, NUM_KEFIFO);
	first = 0;
	return RET_OK;
	

#endif
	
err:
	return ERR_DEV_UNAVAILABLE;
}
//返回observice的编号
//todo: 这个机制不是很好
static int Key_addOb(Keyboard *self, keyObservice *ob)
{
	int i;
	for( i = 0; i < MAX_OBS; i++)
	{
		if( self->arr_p_obm[i].flag == 0)
			break;
	}
	if( i == MAX_OBS)
		return ERR_RSU_UNAVAILABLE;
	self->arr_p_obm[i].flag = 1;
	self->arr_p_obm[i].id = i | 0x80;
	self->arr_p_obm[i].p_ob = ob;
	
	return i;
}

static int Key_DelOb(Keyboard *self, char obId)
{
	int i;
	obId &= ~ 0x80;
	for(i = 0; i < MAX_OBS; i++)
	{
		if( self->arr_p_obm[i].flag == obId)
			break;
	}
	if( i == MAX_OBS)
		return ERR_NOT_EXIST;
	self->arr_p_obm[i].flag = 0;
	self->arr_p_obm[i].id = 0;
	self->arr_p_obm[i].p_ob = NULL;
	
	return i;
}

static void	Key_Run(Keyboard *self)
{
	int  numEvent = 0;
	
#if CONF_KEYSCAN_POLL == 1
	keyMsg_t arr_keyMsg[2] =  {{0}, {0}};		//一次最多就2个组合按键		
	self->run_count ++;
	BEEP_OFF;
	self->scan_key_pins(self);
	numEvent = 2;
	self->identify_key_msg(self, arr_keyMsg, &numEvent);
#else
	keyStatus_t *p_ks;
	keyevent_t ke;
	uint64_t now_ms = 0;
	short		i	= 0;
	char reportflag = 0;
	if( PopKE( &self->kef, &ke) != RET_OK)
		goto noevent;
	
	p_ks = findPks( self->arr_ks, ke.key);
	if( p_ks == NULL)
		return;
	
	if( ke.event == KEY_PUSH)
	{
		KeyPush(p_ks, &ke);
		
	}
	else if( ke.event == KEY_RLS)
	{
		KeyRls(p_ks, &ke);
		
	}
	else if( ke.event == KEY_ERR)
	{
		//发生了错误的按键，把所有的按键事件全部丢弃
		KeyErr(self);
		
	}
	
	noevent:
	//对每个按键状态进行处理
	numEvent = 0;
	reportflag = 0;
	for( i = 0; i < NUM_KEYS; i++)
	{
		if(self->arr_ks[i].eventCode  == 0)
			continue;
		//将至今超过500的按键动作通知
		now_ms = get_time_ms();
		//每个按键都要暂存500ms后才能进行处理，主要是为了双击和组合按键的考虑
		if(((now_ms - self->arr_ks[i].timestamp) > REPORT_TIME_MS) || (reportflag > 0))
		{
			reportflag = 1;
			//识别出长按的按键
			//按下超过3s认为是长按，并且强制认为改按键是长按，而不管它之前的动作是啥
			if((IS_KEYUP(self->arr_ks[i].eventCode) == 0) &&\
				(now_ms - self->arr_ks[i].timestamp) > LONGPUSH_TIME_MS)
			{
				self->arr_ks[i].eventCode = KEYEVENT_LPUSH;

			}
			
			//按键被抬起或者按键被认为是长按的时候，将该按键上报
			if(IS_KEYUP(self->arr_ks[i].eventCode) || (self->arr_ks[i].eventCode == KEYEVENT_LPUSH))
			{
				//如果只有抬起事件，就不处理
				if( self->arr_ks[i].eventCode & (~KEYEVENT_UP))
				{
					arr_keyMsg[numEvent].eventCode = self->arr_ks[i].eventCode;
					arr_keyMsg[numEvent].keyCode = self->arr_ks[i].keyCode;
					
					numEvent ++;
				}
				else
				{
					//只有抬起，说明已经被处理过了，就请除掉
					self->arr_ks[i].eventCode = 0;
					
				}
			}
			//读取IO
			
			//按键被抬起以后就清除掉他的事件代码
			if(IS_KEYUP( self->arr_ks[i].eventCode))
			{
				self->arr_ks[i].eventCode = 0;
				self->arr_ks[i].timestamp = 0;
			}
			
			

		}
		
		 

	}
#endif
	self->notify( self, numEvent, arr_keyMsg);
}

static void	Key_notify( Keyboard *self, uint8_t num, keyMsg_t arr_msg[])
{
	int i;
	if( num == 0)
		return;
	for( i = 0; i < MAX_OBS; i++)
	{
		if( self->arr_p_obm[i].p_ob)
		{
			
			self->arr_p_obm[i].p_ob->update( self->arr_p_obm[i].p_ob, num, arr_msg);
		}
	}
	
}

static void Key_scan_pins(Keyboard *self)
{
	char i;
	char	pin_val = 0;
	for( i = 0; i < NUM_KEYS; i++)
	{
		if(self->arr_p_devGpio[i]->read(self->arr_p_devGpio[i], &pin_val, 0) == RET_OK) 
		{
			if(pin_val == 0) {	//按下 
				if(self->arr_key_pins[i].last_pressed_count == 0)
					self->arr_key_pins[i].sum_count = 0;
				else 
					self->arr_key_pins[i].sum_count += self->run_count - self->arr_key_pins[i].last_pressed_count;
				self->arr_key_pins[i].last_pressed_count = self->run_count;
				
				continue;
			}			
			
		}
		
		//读取失败或者没有按下，都当作抬起处理
		self->arr_key_pins[i].up_flag = 0;
		self->arr_key_pins[i].sum_count = 0;
		self->arr_key_pins[i].last_pressed_count = 0;
	}
	
	
}
static void	Key_identify_key_msg(Keyboard *self, keyMsg_t *p_key_msg, int *num)
{
	char 	i;
	char	num_pressed = 0;
	short	long_count = LONGPUSH_TIME_MS / self->cycle_ms;
	keyMsg_t	*p_msg = NULL;
	
	
	//识别长按
	for( i = 0; i < NUM_KEYS; i++)
	{
		
		
		if(self->arr_key_pins[i].sum_count > long_count) 
		{
			if((self->arr_key_pins[i].long_push_delay % 3) == 0)
			{
				p_key_msg->keyCode = self->arr_key_pins[i].key_code;
				p_key_msg->eventCode = KEYEVENT_LPUSH;
				num_pressed = 1;
			}
			
			self->arr_key_pins[i].long_push_delay ++;
			goto exit;
		}
	}
	
	//没有长按，在检测按下的按键
	//如果按下的按键数量达到了输入的num数量，也直接退出
	for( i = 0; i < NUM_KEYS; i++)
	{
		if(num_pressed >= *num)
			goto exit;
		if((self->arr_key_pins[i].last_pressed_count > 0) && \
			(self->arr_key_pins[i].up_flag == 0)){		//要避免同一个按键不停的上报
			
			p_msg = p_key_msg + num_pressed;
			p_msg->keyCode = self->arr_key_pins[i].key_code;
			p_msg->eventCode = KEYEVENT_HIT;
			self->arr_key_pins[i].up_flag = 1;
			self->arr_key_pins[i].long_push_delay = 0;
			num_pressed ++;
				
				
			if(phn_sys.sys_conf.enable_beep)
			{
				BEEP_ON;
				
			}
		}
	}
	
	
	
	
			
	
	exit:
	*num = num_pressed;
	return;
}

#if CONF_KEYSCAN_POLL == 0
static keyStatus_t *findPks( keyStatus_t arrKs[], uint8_t keycode)
{
	if( keycode >= ( NUM_KEYS + 1))
		return NULL;
	return &arrKs[ keycode - 1];
}

static void KeyPush( keyStatus_t *p_ks,keyevent_t *p_ke)
{
	int diffms = 0;
	//该按键目前没有其他事件的记录，说明此次是Hit操作
	
	if( ( p_ks->eventCode & (~KEYEVENT_UP)) == KEYEVENT_HIT)
	{
		//如果已经有和Hit事件，说明这事双击事件
		
		//双击的时间间隔要小于200ms才有效
		//否则就认为是新的单击事件
		diffms = p_ke->timestamp - p_ks->timestamp ;
//		p_ks->pushtime_ms += cal_timediff_ms( &p_ks->timestamp);
		if( diffms < DOUBLEHIT_TIME_MS)
		{
			p_ks->eventCode = KEYEVENT_DHIT;
			
		}
		else
		{
			p_ks->eventCode = KEYEVENT_HIT;
			p_ks->timestamp = p_ke->timestamp;
		}
		
		
		
	}
	else if( p_ks->eventCode == 0)
	{
		p_ks->eventCode = KEYEVENT_HIT;
//		p_ks->pushtime_ms = 0;
		p_ks->timestamp = p_ke->timestamp;
		
	}
	
	
}
static void KeyRls( keyStatus_t *p_ks, keyevent_t *p_ke)
{
	
	p_ks->eventCode |= KEYEVENT_UP;
//	p_ks->pushtime_ms += cal_timediff_ms( &p_ks->timestamp);
}

static void KeyErr( Keyboard *self)
{
	int i ;
	KEFifo_Reset( &self->kef);
	
	for( i = 0; i < NUM_KEYS; i++)
	{
		self->arr_ks[i].eventCode  = 0;
		self->arr_ks[i].timestamp = 0;	
	}
	
}

//fifo的 操作都假设fifo的长度是2的幂

static void KEFifo_Init ( KEFifo_t *p_kef, int size)
{
	p_kef->size = size;
	p_kef->read = 0;
	p_kef->write = 0;
}

static void KEFifo_Reset ( KEFifo_t *p_kef)
{
	int i;
	
	for( i = 0; i < p_kef->size; i ++)
	{
		p_kef->arr_ke[i].key = 0;
		p_kef->arr_ke[i].event = 0;
		p_kef->arr_ke[i].timestamp = 0;	
		
	}
	p_kef->read = 0;
	p_kef->write = 0;
}

static int	KEFifo_len( KEFifo_t *p_kef)
{
	return ( ( p_kef->write - p_kef->read) & ( p_kef->size - 1));
	
}

static int PushKE( KEFifo_t *p_kef, keyevent_t *p_ke)
{
	if( KEFifo_len( p_kef) == ( p_kef->size - 1))	
		return ERR_MEM_UNAVAILABLE;
	
	p_kef->arr_ke[ p_kef->write].key = p_ke->key;
	p_kef->arr_ke[ p_kef->write].event = p_ke->event;
	p_kef->arr_ke[ p_kef->write].timestamp = p_ke->timestamp;	
		
	p_kef->write = ( p_kef->write + 1) & ( p_kef->size - 1);  
		
	
	return RET_OK;

}	
static int PopKE( KEFifo_t *p_kef, keyevent_t *p_ke)
{
	
	if( KEFifo_len( p_kef) == 0)
		return ERR_MEM_UNAVAILABLE;
	
	p_ke->event = p_kef->arr_ke[ p_kef->read].event ;
	p_ke->key = p_kef->arr_ke[ p_kef->read].key ;
	p_ke->timestamp = p_kef->arr_ke[ p_kef->read].timestamp;	
	p_kef->read = ( p_kef->read + 1) & ( p_kef->size - 1);  
		
	
	return RET_OK;
}
#endif
// keytestob

static int Key_TestUpdate( keyObservice *self,  uint8_t num, keyMsg_t arr_msg[])
{
	KbTestOb *cthis = SUB_PTR(self, keyObservice, KbTestOb);
	if( cthis->hdl)
		cthis->hdl( num, arr_msg);
	
	return RET_OK;
}

static void KeyTest_setKeyHdl( KbTestOb *self, keyHdl hdl)
{
	
	self->hdl = hdl;
}

#if CONF_KEYSCAN_POLL == 0
static  void GpioIrqHdl( void *arg, int type, int encode)
{
	Keyboard *p_kb = ( Keyboard *)arg;
	keyevent_t ke;
	keyevent_t trashKe;		//事件入队列失败时，用来存放将丢弃的事件
	
	ke.timestamp = get_time_ms();
	if( type == GITP_FAILINGEDGE)
	{
		ke.event = KEY_PUSH;
	}
	else if( type == GITP_RISINGEDGE)
	{
		ke.event = KEY_RLS;
	}
	else
	{
		return;
	}
	
	ke.key = encode & 0xff;
	
	
	if( PushKE( &p_kb->kef, &ke) != RET_OK)
	{
		//如队列不成功的时候，要把失败上报
		//取出一个事件丢弃掉，再把错误事件上报
		PopKE( &p_kb->kef, &trashKe) ;
		
		ke.event =  KEY_ERR;
		PushKE( &p_kb->kef, &ke);
	}
	
}
#endif

