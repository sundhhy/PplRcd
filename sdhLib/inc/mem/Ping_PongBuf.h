#ifndef _PINGPONGBUF_H_
#define _PINGPONGBUF_H_
#include <stdint.h>
#define BUF_NONE	2
#define NUM_PINGPONGBUF		2
#define BUF_PING	0
#define BUF_PONG	1

//缓存的状态，只能市三者之一
#define	PPBUF_STATUS_IDLE   		0
#define	PPBUF_STATUS_LOADING   		1
#define	PPBUF_STATUS_PLAYLOAD   	2
typedef struct
{

	char 	*ping_buf;
	char 	*pong_buf;
	short 	buf_len;
	
	short	sw;			//是否提供切换缓存服务
	
	char	*idleBuf[ NUM_PINGPONGBUF];
	char	*loadBuf;
	char	*dataBuf;
	
//	short 	ping_len;
	
	
//	uint8_t	ping_status;
//	uint8_t	pong_status;
//	uint8_t	loading_buf;		//外设接收中的buf
//	uint8_t	playload_buf;		//数据已经结束完成的buf
	
	
}PPBuf_t;
void  init_pingponfbuf( PPBuf_t *ppbuf, char *buf, int len, int sw);
void switch_receivebuf( PPBuf_t *ppbuf, char **buf, short *len);
char *get_playloadbuf( PPBuf_t *ppbuf);
int get_loadbuflen( PPBuf_t *ppbuf);
void free_playloadbuf( PPBuf_t *ppbuf);
#endif
