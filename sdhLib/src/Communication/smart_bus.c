//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//
#include "Communication/smart_bus.h"
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

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static int SmBus_Check_head(IN smBus_head_t *head);
static uint8_t SmBus_crc(uint8_t	*frame);
static int	SmBus_dcd_query(uint8_t *frame, OUT uint8_t *chn_num);
static int	SmBus_dcd_read(uint8_t *frame, OUT uint8_t *data_buf, int buf_size);

static int	SmBus_dcd_AI_read(uint8_t *frame, OUT SmBus_result_t *rst);
static void SmBus_Tail(IN uint8_t cal_xor, smBus_tail_t *tail);
static void SmBus_U16(uint8_t *buf, uint16_t val);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//


//chn_type AI AO 。。。
//错误：负数的错误码
//正确：报文长度
int	SmBus_Query(int chn, uint8_t *frame,  int f_size)
{
	smBus_head_t	*s_head = (smBus_head_t *)frame;
	smBus_tail_t	*s_tail = (smBus_tail_t *)(frame + sizeof(smBus_head_t));
	uint8_t			sb_xor = 0;
	if(f_size < 7)
		return ERR_MEM_LACK;
	
	s_head->cmd = SMBUS_CMD_QUERY;
	s_head->src_addr = SMBUS_SRC_ADDR;
	s_head->dst_addr = chn;
	s_head->len = 0xB0;
	sb_xor = SmBus_crc(frame);
	
	
	SmBus_Tail(sb_xor, s_tail);
	return 7;
}

int SmBus_rd_signal_type(IN uint8_t chn, OUT uint8_t *frame_buf, int buf_size)
{
	
	return SmBus_Read(chn, 0x61,1, frame_buf, buf_size);
}

//int SmBus_rd_h_l_limit(IN uint8_t chn, OUT uint8_t *frame_buf, int buf_size)
//{
//	
//	return SmBus_Read(chn, 0x72,4, frame_buf, buf_size);
//}

int	SmBus_Read(IN int chn, IN uint8_t addr, IN uint8_t len, OUT uint8_t	*frame_buf, int buf_size)
{
	smBus_head_t	*s_head = (smBus_head_t *)frame_buf;
	uint8_t			*playload = frame_buf + sizeof(smBus_head_t);
	smBus_tail_t	*s_tail = (smBus_tail_t *)(playload + 4);
	uint8_t			sb_xor = 0;
	
	if(buf_size < 11)
		return -1;
	
	s_head->cmd = SMBUS_CMD_READ;
	s_head->src_addr = SMBUS_SRC_ADDR;
	s_head->dst_addr = chn;
	s_head->len = 0xB4;
	playload[0] = 0x84;
	playload[1] = SMBUS_BUILD_BYTE(0x80, len);
	playload[2] = SMBUS_BUILD_BYTE(0x80, addr );
	playload[3] = SMBUS_BUILD_BYTE(0x80, addr >> 4);
	sb_xor = SmBus_crc(frame_buf);
	SmBus_Tail(sb_xor, s_tail);
	
	return 11;
}

int	SmBus_Write(IN int chn, IN uint8_t addr, IN uint8_t len, IN uint8_t *wr_buf, OUT uint8_t *frame_buf, int buf_size)
{
	smBus_head_t	*s_head = (smBus_head_t *)frame_buf;
	uint8_t			*playload = frame_buf + sizeof(smBus_head_t);
	smBus_tail_t	*s_tail = (smBus_tail_t *)(playload + 4 + len * 2 );
	uint8_t			sb_xor = 0;
	uint8_t			i = 0;
	uint8_t			p_len = len * 2;
	if(buf_size < (11 + p_len))
		return -1;
	
	s_head->cmd = SMBUS_CMD_READ;
	s_head->src_addr = SMBUS_SRC_ADDR;
	s_head->dst_addr = chn;
	s_head->len = 0xB0 | (p_len + 4);
	playload[0] = 0x85;
	playload[1] = SMBUS_BUILD_BYTE(0x80, len);
	playload[2] = SMBUS_BUILD_BYTE(0x80, addr );
	playload[3] = SMBUS_BUILD_BYTE(0x80, addr >> 4);
	
	for(i = 0; i < len; i ++)
	{
		playload[4 + i * 2] = SMBUS_BUILD_BYTE(0x80, wr_buf[i] );
		playload[4 + i * 2 + 1] = SMBUS_BUILD_BYTE(0x80, wr_buf[i] >> 4);
	}
	
	sb_xor = SmBus_crc(frame_buf);
	SmBus_Tail(sb_xor, s_tail);
	
	return (11 + p_len);
}


/*********************AI 命令 *************************************/

int SmBus_AI_config(IN uint8_t chn, SmBus_conf_t *conf, OUT uint8_t *frame_buf, int buf_size)
{
	smBus_head_t	*s_head = (smBus_head_t *)frame_buf;
	uint8_t			*playload = frame_buf + sizeof(smBus_head_t);
	smBus_tail_t	*s_tail = (smBus_tail_t *)(playload +  10);
	uint8_t			sb_xor = 0;
	if(buf_size < 17)
		return ERR_MEM_LACK;
	
	s_head->cmd = SMBUS_AI_CONFIG;
	s_head->src_addr = SMBUS_SRC_ADDR;
	s_head->dst_addr = chn | SMBUS_CHN_AI;
	s_head->len = 0xBA;
	playload[0] = SMBUS_BUILD_BYTE(0x80, conf->signal_type);
	playload[1] = SMBUS_BUILD_BYTE(0x80, conf->decimal);
	SmBus_U16(playload + 2, conf->lower_limit);
	SmBus_U16(playload + 6, conf->upper_limit);

	sb_xor = SmBus_crc(frame_buf);
	SmBus_Tail(sb_xor, s_tail);
	return 17;
}
int	SmBus_WR_low_limit(IN uint8_t chn, int16_t *low_limie, OUT uint8_t *frame_buf, int buf_size)
{
		
	return SmBus_Write(chn, 0x78,2, (uint8_t *)low_limie, frame_buf, buf_size);
}
int	SmBus_WR_hig_limit(IN uint8_t chn, int16_t *hig_limie, OUT uint8_t *frame_buf, int buf_size)
{
		
	return SmBus_Write(chn, 0x6e,2, (uint8_t *)hig_limie, frame_buf, buf_size);
}
int	SmBus_DO_output(IN uint8_t chn, IN uint8_t val, OUT uint8_t *frame_buf, int buf_size)
{
	uint8_t		do_val = val;
//	if(val)
//		do_val = 3;
//	else
//		do_val = 0;
	return SmBus_Write(0x51, 0x6b, 1, (uint8_t *)&do_val, frame_buf, buf_size);
}

int	SmBus_Read_DO(IN uint8_t chn, OUT uint8_t *frame_buf, int buf_size)
{
	
	return SmBus_Read(0x51, 0x6b, 1, frame_buf, buf_size);
}



int	SmBus_RD_hig_limit(IN uint8_t chn, OUT uint8_t *frame_buf, int buf_size)
{
		
	return SmBus_Read(chn, 0x6e, 2, frame_buf, buf_size);
}

int	SmBus_RD_low_limit(IN uint8_t chn, OUT uint8_t *frame_buf, int buf_size)
{
		
	return SmBus_Read(chn, 0x78, 2, frame_buf, buf_size);
}

//chn为5的时候表示读取冷端温度
int SmBus_AI_Read(IN uint8_t chn, IN uint8_t val_type, OUT uint8_t *frame_buf, int buf_size)
{
	smBus_head_t	*s_head = (smBus_head_t *)frame_buf;
	uint8_t			*playload = frame_buf + sizeof(smBus_head_t);
	smBus_tail_t	*s_tail = (smBus_tail_t *)(playload +  1);
	uint8_t			sb_xor = 0;
	if(buf_size < 9)
		return ERR_MEM_LACK;
	
	s_head->cmd = SMBUS_AI_READ;
	s_head->src_addr = SMBUS_SRC_ADDR;
	s_head->dst_addr = chn | SMBUS_CHN_AI;
	
	
	s_head->len = 0xB1;
	if(chn == 5)	
	{
		s_head->len = 0xBF;
		
	}
	playload[0] = val_type;		//只支持采样值
	
	sb_xor = SmBus_crc(frame_buf);
	
	
	
	SmBus_Tail(sb_xor, s_tail);
	return 8;
	
}



/*********************AI 命令 *************************************/
//成功返回RET_OK
//错误返回错误码
int	SmBus_decode(IN uint8_t cmd, IN uint8_t *frame, OUT void *result_buf, int buf_szie)
{
	int	ret = RET_OK;
	
	switch(cmd)
	{
		case SMBUS_CMD_WRITE:
		case SMBUS_AI_CONFIG:
		case SMBUS_CMD_QUERY:
			ret = SmBus_dcd_query(frame, result_buf);
			break;
		case SMBUS_CMD_READ:
			ret = SmBus_dcd_read(frame, result_buf, buf_szie);
			break;

		
		
		case SMBUS_AI_READ:
			if(buf_szie < sizeof(SmBus_result_t))
				ret = ERR_MEM_LACK;
			else 
			{
				
				ret = SmBus_dcd_AI_read(frame, result_buf);
			}
			break;
		
		default:
			ret = ERR_CMM_ILLEGAL;
			break;
	}
	
	return ret ;
}

//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{
static uint8_t SmBus_crc(uint8_t	*frame)
{
	smBus_head_t	*s_head = (smBus_head_t *)frame;
	uint8_t			check_xor = 0;
	uint8_t			i;
	uint8_t			f_len = (s_head->len & 0x0f) + 4;
	
	if(s_head->len == 0xbf)
	{
		f_len = 5;
		if((s_head->cmd & 0xf0) == 0xc0)
			f_len = 9;
	}
	for(i = 0; i < f_len; i ++)
		check_xor ^= frame[i];
	return check_xor;
}


//按照低字节在前排列
static void SmBus_U16(uint8_t *buf, uint16_t val)
{
	
	buf[0] = SMBUS_BUILD_BYTE(SMBUS_PREFIX_DATA,val);
	buf[1] = SMBUS_BUILD_BYTE(SMBUS_PREFIX_DATA,val >> 4);
	buf[2] = SMBUS_BUILD_BYTE(SMBUS_PREFIX_DATA,val >> 8);
	buf[3] = SMBUS_BUILD_BYTE(SMBUS_PREFIX_DATA,val >> 12);
}


//成功则返回u16的数值
//失败：返回ERR_CMM_ILLEGAL
static int SmBus_Get_u16(uint8_t *p)
{
	int ret = 0;
	int	i = 0;
	uint8_t	data_u8[4] = {0};
	
	for(i = 0; i < 4; i++)
	{
		if((p[i] & 0xf0 ) != SMBUS_PREFIX_DATA)
		{
			ret = ERR_CMM_ILLEGAL;
			break;
			
		}
		
	}
	if(ret != 0)
	{
		goto exit;
	}
	for(i = 0; i < 4; i++)
	{
		data_u8[i] = p[i] & 0x0f;
		
	}
	ret |= data_u8[3] << 12;
	ret |= data_u8[2] << 8;
	ret |= data_u8[1] << 4;
	ret |= data_u8[0];
	exit:
	return ret;
	
}

static void SmBus_Tail(IN uint8_t cal_xor, smBus_tail_t *tail)
{
	
	tail->xor_l = SMBUS_PREFIX_XOR | (cal_xor & 0x0f);
	cal_xor >>=4;
	tail->xor_h = SMBUS_PREFIX_XOR | (cal_xor & 0x0f);
	tail->end_flag = SMBUS_FLAG_END;
}
//返回命令字中的错误码
static int SmBus_Cmd_err(uint8_t	cmd)
{
	uint8_t  err_code = cmd & 0x0f;
	if((cmd & 0xf0) != SMBUS_FLAG_ACK)
		return SmBus_e_ack;
	
	switch(err_code)
	{
		case 0:
			return SmBus_ok;
		
			
		case 2:
			return SmBus_e_xor;
		case 3:
			return SmBus_e_unsupported;
		case 0xb:
			return SmBus_e_AO_up_limit;
		case 0xc:
			return SmBus_e_AO_floor_limit;
		case 0xd:
			return SmBus_e_mast_xor;
		case 0xf:
			return SmBus_e_timeout;
		default:
			break;
		
	}
	
	return SmBus_e_reserve;
	
}
uint8_t SmBus_Get_crc(IN smBus_tail_t *tail)
{
	
	uint8_t crc_h = tail->xor_h & 0x0f;
	uint8_t crc_l = tail->xor_l & 0x0f;
	
	return (crc_h << 4) | crc_l;
}
static int SmBus_Check_head(IN smBus_head_t *head)
{
	if((head->len & 0xf0) != SMBUS_PREFIX_LEN)
		return ERR_CMM_ILLEGAL;
	
	return RET_OK;
}

static int SmBus_Check_tail(IN smBus_tail_t *tail)
{
	if((tail->xor_h & 0xf0) != SMBUS_PREFIX_XOR)
		return ERR_CMM_ILLEGAL;
	if((tail->xor_l & 0xf0) != SMBUS_PREFIX_XOR)
		return ERR_CMM_ILLEGAL;
	if(tail->end_flag != SMBUS_FLAG_END)
		return ERR_CMM_ILLEGAL;
	return RET_OK;
}

//0xc0,0x50,0x10,0xb0,0x90,0x93,0xaf
static int	SmBus_dcd_query(uint8_t *frame, OUT uint8_t *chn_num)
{
	
	int				ret = 0;
	smBus_head_t	*s_head = (smBus_head_t *)frame;
	smBus_tail_t	*s_tail = (smBus_tail_t *)(frame + sizeof(smBus_head_t));
	uint8_t			rd_crc = 0, cal_crc = 0;
	
	
	ret = SmBus_Check_head(s_head);
	if(ret != RET_OK)
		return ret;
	ret = SmBus_Check_tail(s_tail);
	if(ret != RET_OK)
		return ret;
	
	if( SmBus_Cmd_err(s_head->cmd) != SmBus_ok)
		return ERR_CMM_CMDERR;
	

	
	cal_crc = SmBus_crc(frame);
	rd_crc = SmBus_Get_crc(s_tail);
	
	if(cal_crc != rd_crc)
		return ERR_CMM_CKECKERR;
	
	if(chn_num)
		*chn_num = s_head->src_addr & 0x0f;
	
	return ret;
}

static int	SmBus_dcd_read(uint8_t *frame, OUT uint8_t *data_buf, int buf_size)
{
	int				ret = 0;
	smBus_head_t	*s_head = (smBus_head_t *)frame;
	uint8_t			*play_load = frame + sizeof(smBus_head_t);
	smBus_tail_t	*s_tail = (smBus_tail_t *)(frame + sizeof(smBus_head_t));
	uint8_t			rd_crc = 0, cal_crc = 0;
	uint8_t			pl_len = 0, tmp_u8 = 0;
	uint8_t			i = 0, j;
	
	ret = SmBus_Check_head(s_head);
	if(ret != RET_OK)
		return ret;
	
	if( SmBus_Cmd_err(s_head->cmd) != SmBus_ok)
		return ERR_CMM_CMDERR;
	
	pl_len = s_head->len & 0x0f;
	
	
	s_tail = (smBus_tail_t *)(play_load + pl_len);
	
	if((pl_len / 2) > buf_size)
		return ERR_MEM_LACK;
	
	ret = SmBus_Check_tail(s_tail);
	if(ret != RET_OK)
		return ret;
	cal_crc = SmBus_crc(frame);
	rd_crc = SmBus_Get_crc(s_tail);
	
	if(cal_crc != rd_crc)
		return ERR_CMM_CKECKERR;
	
	j = 0;
	for(i = 0; i < pl_len; )
	{
		tmp_u8 = play_load[i+1] & 0x0f;
		tmp_u8 <<= 4;
		tmp_u8 |= play_load[i] & 0x0f;
		
		data_buf[j++] = tmp_u8;
		i += 2;
	}
	
	return RET_OK;
	
}

//0xc0,0x50,0x10,0xb0,0x90,0x93,0xaf
static int	SmBus_dcd_AI_read(uint8_t *frame, OUT SmBus_result_t *rst)
{
	
	int				ret = 0;
	smBus_head_t	*s_head = (smBus_head_t *)frame;
	uint8_t			*play_load = frame + sizeof(smBus_head_t);
	smBus_tail_t	*s_tail = (smBus_tail_t *)(play_load + 5);
	uint8_t			rd_crc = 0, cal_crc = 0;
	
	
	ret = SmBus_Check_tail(s_tail);
	if(ret != RET_OK)
		return ret;
	
	if( SmBus_Cmd_err(s_head->cmd) != SmBus_ok)
		return ERR_CMM_CMDERR;
	

	
	cal_crc = SmBus_crc(frame);
	rd_crc = SmBus_Get_crc(s_tail);
	
	if(cal_crc != rd_crc)
		return ERR_CMM_CKECKERR;
	
	rst->signal_type = play_load[0] & 0x0f;
	rst->chn_num = s_head->src_addr & 0x0f;
	
	ret  = SmBus_Get_u16(play_load + 1);
	if(ret > 0)
	{
		rst->val = ret;
		ret = RET_OK;
	}
	return ret;
}

