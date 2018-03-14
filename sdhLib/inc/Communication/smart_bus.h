//---------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#ifndef __INC_SMART_BUS_H__
#define __INC_SMART_BUS_H__
#include <stdint.h>
#include "sdhDef.h"

//------------------------------------------------------------------------------
// check for correct compilation options
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define	SMBUS_CHN_AI		0x50

#define	SMBUS_SRC_ADDR		0x10

#define	SMBUS_PREFIX_LEN		0xB0
#define	SMBUS_PREFIX_XOR		0x90
#define	SMBUS_PREFIX_DATA		0x80


#define	SMBUS_FLAG_END			0xaf
#define	SMBUS_FLAG_ACK			0xc0


#define SMBUS_CMD_QUERY		0xA1
#define SMBUS_CMD_READ		0xAD
#define SMBUS_CMD_WRITE		0xFD		//读写指令的命令字都是一样的，通过子命令来区别
#define SMBUS_AI_CONFIG		0xA2
#define SMBUS_AI_READ		0xA4
#	define	AI_READ_ENGVAL	0x80
#	define	AI_READ_SMPVAL	0x81
#	define	CDT_CHN			5

//扩展命令:是使用基础评论来实现的功能
//#define SECMD_RD_SIGTYPE		0xe0
//#define SECMD_RD_H_L_LIMIT		0xe1		//读取上下限

#define SMBUS_MAKE_CHN(cty, cnum) (cty | (cnum & 0x0f))
#define SMBUS_BUILD_BYTE(flag, val) SMBUS_MAKE_CHN(flag, (val))
//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------
typedef enum {
	SmBus_ok = 0,
	SmBus_e_ack,
	SmBus_e_unsupported,
	SmBus_e_xor,
	SmBus_e_AO_up_limit,
	SmBus_e_AO_floor_limit,
	SmBus_e_mast_xor,
	SmBus_e_timeout,
	SmBus_e_reserve
}SmBus_err_t;

typedef struct {
	uint16_t	lower_limit;
	uint16_t	upper_limit;
	uint8_t		signal_type;
	uint8_t		decimal;		//小数点
	uint8_t		none[2];
	
	
}SmBus_conf_t;

typedef struct {
	int16_t	val;
	uint8_t		chn_num;
	uint8_t		signal_type;
	
	
	
}SmBus_result_t;
typedef struct {
	uint8_t		cmd;
	uint8_t		src_addr;
	uint8_t		dst_addr;
	uint8_t		len;		//0xBn		n具体长度
}smBus_head_t;

typedef struct {
	uint8_t		xor_l;
	uint8_t		xor_h;			//异或和
	
	uint8_t		end_flag;			//0xaf
}smBus_tail_t;
//------------------------------------------------------------------------------
// global variable declarations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
int	SmBus_Query(IN int chn, OUT uint8_t *frame, int f_size);
int	SmBus_decode(IN uint8_t cmd, IN uint8_t *frame, OUT void *result_buf, int buf_szie);
int	SmBus_Read(IN int chn, IN uint8_t addr, IN uint8_t len, OUT uint8_t	*frame_buf, int buf_size);
int	SmBus_Write(IN int chn, IN uint8_t addr, IN uint8_t len, IN uint8_t *wr_buf, OUT uint8_t	*frame_buf, int buf_size);
int SmBus_AI_Read(IN uint8_t chn, IN uint8_t val_type, OUT uint8_t *frame_buf, int buf_size);
int SmBus_rd_signal_type(IN uint8_t chn, OUT uint8_t *frame_buf, int buf_size);
int	SmBus_DO_output(IN uint8_t chn, IN uint8_t val, OUT uint8_t *frame_buf, int buf_size);
int	SmBus_Read_DO(IN uint8_t chn, OUT uint8_t *frame_buf, int buf_size);
//int SmBus_rd_h_l_limit(IN uint8_t chn, OUT uint8_t *frame_buf, int buf_size);
int SmBus_AI_config(IN uint8_t chn, SmBus_conf_t *conf, OUT uint8_t *frame_buf, int buf_size);
int	SmBus_WR_hig_limit(IN uint8_t chn, int16_t *hig_limt, OUT uint8_t *frame_buf, int buf_size);
int	SmBus_WR_low_limit(IN uint8_t chn, int16_t *low_limt, OUT uint8_t *frame_buf, int buf_size);
int	SmBus_RD_hig_limit(IN uint8_t chn, OUT uint8_t *frame_buf, int buf_size);
int	SmBus_RD_low_limit(IN uint8_t chn, OUT uint8_t *frame_buf, int buf_size);
#endif
//------------ End of file ---------------------------
