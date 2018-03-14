//用于设备接收的缓存操作
#include "mem/VectorBuf.h"
#include <string.h>

//vectorbuf manager
static int VecBuf_RecycleAFrame( vectorBufManager_t* p_vbm);

int VecBuf_Init( vectorBufManager_t* p_vbm, char* buf, uint16_t len, int drop)
{
	if( buf == NULL || len < VBM_FRAMEHEAD_LEN)
		return -1;
	if( ( drop != DROP_OLDDATA) && ( drop != DROP_NEWDATA))
		return -1;
	
	p_vbm->buf = buf;
	p_vbm->totalLen = len;
	p_vbm->freeLen = len ;
	p_vbm->drop = drop;
	p_vbm->wrIndex = 0;
	p_vbm->rdIndex = 0;
	memset( buf, 0, len);
	
	return 0;
	
}




int VecBuf_write( vectorBufManager_t* p_vbm, char* data, uint16_t len)
{
	uint16_t 	i = 0;
	short		fillLen = VBM_FILLBYTE_LEN( len);		//
	uint16_t 	numAllByte = len + VBM_FRAMEHEAD_LEN + fillLen;
	frameHead_t 	*p_head;
	
	if( len == 0)
		return 0;
	if( numAllByte >  p_vbm->totalLen )		//
		return 0;
	
	//buf_len 必须是2的幂
	
	//空间不足时，清除未读取的数据来获得足够的空间
	while(  numAllByte > p_vbm->freeLen )
	{
		//如果要求丢弃新数据就直接返回
		if( IS_DROPNEWDATA( p_vbm))
			return 0;
	
		//无法回收更多的内存就退出
		if( VecBuf_RecycleAFrame( p_vbm) == 0)
			return 0;
	}
	
	p_head = ( frameHead_t *)( p_vbm->buf + p_vbm->wrIndex);
	p_head->frameLen = len ;
	VBM_ADD_WRINDEX( p_vbm, VBM_FRAMEHEAD_LEN);
	p_vbm->freeLen -= VBM_FRAMEHEAD_LEN;
	
	i = 0;
	while( len)
	{
		p_vbm->buf[ p_vbm->wrIndex] =  data[i];
		VBM_ADD_WRINDEX( p_vbm, 1);
		p_vbm->freeLen --;
		
		i ++;
		len --;
		
	}
	
	while( fillLen)
	{
		p_vbm->buf[ p_vbm->wrIndex] =  0;
		VBM_ADD_WRINDEX( p_vbm, 1);
		p_vbm->freeLen --;
		
		fillLen--;
		
	}
	
	return p_head->frameLen;
}
int VecBuf_read( vectorBufManager_t* p_vbm, char* buf, uint16_t bufsize)
{
	frameHead_t 	*p_head =( frameHead_t *)( p_vbm->buf + p_vbm->rdIndex);
	short			fillLen = VBM_FILLBYTE_LEN( p_head->frameLen);
	short 			i = 0;
	short			rdByte = 0;
	if( p_vbm->freeLen == p_vbm->totalLen)
		return 0;
	if( p_head->frameLen == 0)
		return 0;
	
		
	
	VBM_ADD_RDINDEX( p_vbm, VBM_FRAMEHEAD_LEN);
	p_vbm->freeLen += VBM_FRAMEHEAD_LEN;

	i = 0;
	rdByte = p_head->frameLen;
	p_head->frameLen = 0;
	while( rdByte )
	{
		if( i < bufsize)
			buf[i++ ] = p_vbm->buf[ p_vbm->rdIndex];
		p_vbm->buf[ p_vbm->rdIndex] = 0;
		VBM_ADD_RDINDEX( p_vbm, 1);
		p_vbm->freeLen ++;
		
		rdByte --;
	}
	
	while( fillLen)
	{
		VBM_ADD_RDINDEX( p_vbm, 1);
		p_vbm->freeLen ++;
		
		fillLen--;
	}
	
	return i;
	
}

//返回丢弃一帧后回收的缓存长度
static int VecBuf_RecycleAFrame( vectorBufManager_t* p_vbm)
{
	frameHead_t *p_head;
	uint16_t recycleByte = 0;
	short	fillLen = 0;
	
	p_head = ( frameHead_t *)( p_vbm->buf + p_vbm->rdIndex );
	fillLen = VBM_FILLBYTE_LEN( p_head->frameLen);
	
	if( p_head->frameLen == 0)
			return 0;
	fillLen = VBM_FILLBYTE_LEN( p_head->frameLen);
	recycleByte = p_head->frameLen + VBM_FRAMEHEAD_LEN + fillLen;
	p_head->frameLen = 0;
	p_vbm->rdIndex += recycleByte;
	VBM_ADD_RDINDEX( p_vbm, recycleByte);
	p_vbm->freeLen += recycleByte;
	
	return (recycleByte);
}

