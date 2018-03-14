/**
* @file 		Ping_PongBuf.c
* @brief		实现双RAM
* @details		1. 当ping和pong都没被外设使用的时候，优先使用ping。 
*				2. 当ping和pong都被装载了数据时，数据都没被应用程序取走的时候，吧pong的缓存内容放弃掉
* @author		author
* @date		date
* @version	A001
* @par Copyright (c): 
* 		XXX??
* @par History:         
*	version: author, date, desc\n
*/
#include "mem/Ping_PongBuf.h"
#include <string.h>
void  init_pingponfbuf( PPBuf_t *ppbuf, char *buf, int len , int sw)
{
	ppbuf->sw = sw;
	if( sw)
	{
		
		ppbuf->ping_buf = buf;
		ppbuf->buf_len = len/2;
		ppbuf->pong_buf = buf + ppbuf->buf_len;
		
		ppbuf->idleBuf[ 0] = ppbuf->ping_buf;
		ppbuf->idleBuf[ 1] = ppbuf->pong_buf;
		
		//优先使用ping_buf
		//初始化的时候把载入buf设置为ping_buf，是为了在第一次切换内存的时候
		//让dataBuf 与loadbuf一致
		ppbuf->loadBuf = ppbuf->ping_buf;
		ppbuf->dataBuf = NULL;	
	}
	else
	{
		ppbuf->ping_buf = buf;
		ppbuf->buf_len = len;
		ppbuf->pong_buf = NULL;
		
		ppbuf->idleBuf[ 0] = NULL;
		ppbuf->idleBuf[ 1] = NULL;
		ppbuf->loadBuf = ppbuf->ping_buf;
		ppbuf->dataBuf = ppbuf->ping_buf;	
		
	}
		
	
//	ppbuf->ping_status = PPBUF_STATUS_IDLE;
//	ppbuf->pong_status = PPBUF_STATUS_IDLE;
//	ppbuf->loading_buf = BUF_NONE;
//	ppbuf->playload_buf = BUF_NONE;
}
//
void switch_receivebuf( PPBuf_t *ppbuf, char **buf, short *len)
{
	int i = 0;
	if( ppbuf->sw == 0)
	{
		*buf = ppbuf->loadBuf;
		*len = ppbuf->buf_len;
		return ;
	}
	
	for( i = 0; i < NUM_PINGPONGBUF; i++)
	{
		if( ppbuf->idleBuf[ i])
		{
			
			ppbuf->dataBuf = ppbuf->loadBuf;
			ppbuf->loadBuf = ppbuf->idleBuf[ i];
			ppbuf->idleBuf[ i] = NULL;
			break;
		}
		
	}
	
	*buf = ppbuf->loadBuf;
	*len = ppbuf->buf_len;
	
}



char *get_playloadbuf( PPBuf_t *ppbuf)
{
	

	return ppbuf->dataBuf;
//	if( ppbuf->playload_buf == BUF_PING)
//	{
//		ppbuf->ping_status = PPBUF_STATUS_IDLE;
//		return ppbuf->ping_buf;
//		
//	}
//	else if( ppbuf->playload_buf == BUF_PONG)
//	{
//		ppbuf->pong_status = PPBUF_STATUS_IDLE;
//		return ppbuf->pong_buf;
//		
//	}
//	else
//	{
//		
//		return ppbuf->ping_buf;
//	}
}
void free_playloadbuf( PPBuf_t *ppbuf)
{
	int i = 0;
	
	
	if( ppbuf->dataBuf )
		memset( ppbuf->dataBuf, 0, ppbuf->buf_len);
	
	if( ppbuf->sw == 0)
	{
		
		return ;
	}
	
	
	for( i = 0; i < NUM_PINGPONGBUF; i++)
	{
		if( ppbuf->idleBuf[ i] == ppbuf->dataBuf)
		{	
			break;
		}
		if( ppbuf->idleBuf[ i] == NULL)
		{
			ppbuf->idleBuf[ i] = ppbuf->dataBuf;
			break;
		}
		
	}
	ppbuf->dataBuf = ppbuf->loadBuf;

//	if( ppbuf->playload_buf == BUF_PING)
//	{
//		memset( ppbuf->ping_buf, 0, ppbuf->ping_len);
//	}
//	else if( ppbuf->playload_buf == BUF_PONG)
//	{
//		memset( ppbuf->pong_buf, 0, ppbuf->pong_len);
//		
//	}
//	ppbuf->playload_buf = BUF_NONE;
	
}
//当ping和pong的装载标志都为1的时候
//ping缓存的内容未被读取
//pong缓存作为接收缓存一直在被外设数据覆盖
int get_loadbuflen( PPBuf_t *ppbuf)
{
//	if( ppbuf->playload_buf == BUF_PING)
//	{
		return ppbuf->buf_len;
//	}
//	else
//	{
//		return ppbuf->pong_len;
//	}
	
}
