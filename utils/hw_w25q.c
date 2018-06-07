/*************************************************
Copyright (C), 
File name: w25q.c
Author:		sundh
Version:	v1.0
Date: 		14-02-27
Description: 
w25Q的驱动程序，实现W25Q的标准SPI操作模式
使用MCU的SPI0来与w25q通讯
w25q要提供很多复杂的接口。
w25q不实现驱动适配器接口。
为了尽量减少对flash的操作，所有对flash的写操作都以扇区为单位进行。对于读取提供了随机地址读的方法。
Others: 
Function List: 
1. w25q_init
初始化flash，并获取id和容量信息
History: 
1. Date:
Author:
Modification:
2. w25q_Erase_Sector
擦除flash中的选定的某一个扇区
History: 
1. Date:
Author:
Modification:
3. w25q_Write_Sector_Data
像一个指定的扇区写入数据
History: 
1. Date:
Author:
Modification:
4. w25q_Read_Sector_Data
读取一个扇区
History: 
1. Date:
Author:
Modification:
5. w25q_rd_data
读取随机地址
History: 
1. Date:
Author:
Modification:
*************************************************/



//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//
#include "hw_w25q.h"
#include <string.h>
#include "sdhdef.h"
#include "os/os_depend.h"
#include "device.h"

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

static	I_dev_Char	*w25q_spi;

#define W25Q_FLAG_READED		1
#define W25Q_FLAG_DATA_CHANGED	2

///移植时需要修改的接口
#define W25Q_SPI_DEVID				DEVID_SPI2
#define W25Q_FSH					phn_sys.arr_fsh[FSH_W25Q_NUM]

#define W25Q_DELAY_MS(ms)			delay_ms(ms)	
#define W25Q_Enable_CS          	GPIO_ResetBits(PORT_W25Q_nCS, PIN_W25Q_nCS)
#define W25Q_Disable_CS         	GPIO_SetBits(PORT_W25Q_nCS, PIN_W25Q_nCS)  

#define W25Q_Enable_WP          	GPIO_ResetBits(PORT_FSH_nWP, PIN_FSH_nWP)
#define W25Q_Disable_WP         	GPIO_SetBits(PORT_FSH_nWP, PIN_FSH_nWP) 

#define SPI_WRITE(data, len)		w25q_spi->write(w25q_spi, data, len)
#define SPI_READ(buf, len)			w25q_spi->read(w25q_spi, buf, len)

#define DUG_STG_HOLE					1		//调试存储空洞的问题的代码
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------
typedef struct {
	
	sem_t			w25_sem;
	uint16_t		cur_sct;
	uint16_t		sct_size;
//	uint16_t		num_sct;

	uint8_t			w25q_flag;
	uint8_t			cache_earse;
	uint8_t			none[2];

	uint8_t			*p_sct_buf;
	uint8_t			*p_chk_buf;
	
}w25q_mgr_t;
//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------

static w25q_mgr_t		w25q_mgr;
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static uint8_t	W25Q_tx_buf[16];
static int w25q_wr_enable(void);
static uint8_t w25q_ReadSR(void);
static int W25Q_Raw_write(uint8_t *pBuffer, uint32_t WriteAddr, uint32_t WriteBytesNum);
//static int w25q_write_waitbusy(uint8_t *data, int len);
static int W25Q_send_wait(uint8_t *data, uint16_t len, uint16_t ms);
static int w25q_read_id(uint8_t  buf[2]);
static int w25q_Erase_Sector(uint32_t Sector_Number);
static int w25q_Erase_block(uint16_t block_Number);
static int w25q_Erase_chip_c7(void);
static int	W25Q_wr_fsh(uint8_t *pBuffer, uint32_t WriteAddr, uint32_t WriteBytesNum);
static int W25Q_Raw_Read(uint8_t *pBuffer, uint32_t rd_add, uint32_t len);
static int w25q_Read_flash(uint8_t *pBuffer, uint32_t rd_add, uint32_t len);
//static int w25q_Erase_chip_60(void);

void W25Q_WP(int protect);

void w25q_info(fsh_info_t *info);
int W25Q_erase(int opt, uint32_t num);
void W25Q_Erase_addr(uint32_t st, uint32_t sz);
//int w25q_erase(uint32_t offset, uint32_t len);

int w25q_Write(uint8_t *pBuffer, uint32_t WriteAddr, uint32_t WriteBytesNum);
//int w25q_Direct_write(uint8_t *pBuffer, uint32_t WriteAddr, uint32_t WriteBytesNum);
int w25q_rd_data(uint8_t *pBuffer, uint32_t rd_add, uint32_t len);
void W25Q_Flush(void);
int	W25Q_lock(uint32_t ms);
int	W25Q_unlock(void);
//int w25q_close(void);


static int w25q_Read_Sector_Data(uint8_t *pBuffer, uint16_t Sector_Num);
static int w25q_Write_Sector_Data(uint8_t *pBuffer, uint16_t Sector_Num);

//static int w25q_Read_page_Data(uint8_t *pBuffer, uint16_t num_page);
static uint16_t W25Q_Addr_2_sct(uint32_t addr);
static int	W25Q_wr_cache(uint32_t addr, uint8_t *wr_buf, uint16_t wr_len);
static int W25Q_rd_cache(uint32_t addr, uint8_t *rd_buf, uint16_t	rd_len);
static int W25Q_sct_offset(uint16_t	sct_num, uint32_t	addr);
static int W25Q_Change_cache_sct(uint16_t	sct_num);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
int w25q_init(void)
{
	int  ret = RET_OK;
	if(W25Q_FSH.fnf.fnf_flag == 0)
	{
		w25q_mgr.w25_sem = Alloc_sem();
		
		Sem_init(&w25q_mgr.w25_sem);
		Sem_post(&w25q_mgr.w25_sem);
			
		Dev_open(W25Q_SPI_DEVID, (void *)&w25q_spi);
		W25Q_Disable_CS;
		
		W25Q_tx_buf[0] = 0xff;
		W25Q_FSH.fnf.fnf_flag = 1;
	}
	
	
//	phn_sys.arr_fsh[FSH_W25Q_NUM].fsh_init = w25q_init;
	phn_sys.arr_fsh[FSH_W25Q_NUM].fsh_info = w25q_info;
	phn_sys.arr_fsh[FSH_W25Q_NUM].fsh_wp = W25Q_WP;

	phn_sys.arr_fsh[FSH_W25Q_NUM].fsh_ersse = W25Q_erase;
	phn_sys.arr_fsh[FSH_W25Q_NUM].fsh_ersse_addr = W25Q_Erase_addr;

//	phn_sys.arr_fsh[FSH_W25Q_NUM].fsh_wr_sector = w25q_Write_Sector_Data;
//	phn_sys.arr_fsh[FSH_W25Q_NUM].fsh_rd_sector = w25q_Read_Sector_Data;
	phn_sys.arr_fsh[FSH_W25Q_NUM].fsh_write = w25q_Write;
	phn_sys.arr_fsh[FSH_W25Q_NUM].fsh_raw_write = W25Q_Raw_write;
	phn_sys.arr_fsh[FSH_W25Q_NUM].fsh_read = w25q_rd_data;
	phn_sys.arr_fsh[FSH_W25Q_NUM].fsh_raw_read = W25Q_Raw_Read;
	phn_sys.arr_fsh[FSH_W25Q_NUM].fsh_lock = W25Q_lock;
	phn_sys.arr_fsh[FSH_W25Q_NUM].fsh_unlock = W25Q_unlock;
	
	W25Q_FSH.fsh_flush = W25Q_Flush;
	W25Q_Disable_WP;
	
	w25q_info(&W25Q_FSH.fnf);
	
	w25q_mgr.w25q_flag  = 0;
	w25q_mgr.cache_earse = 0;
	w25q_mgr.p_sct_buf = malloc(w25q_mgr.sct_size);
	memset(w25q_mgr.p_sct_buf, 0xff, w25q_mgr.sct_size);
	
	return ret;
}

///这个函数要在w25q_init成功之后调用才有用
void w25q_info(fsh_info_t *info)
{
	uint8_t		id[2];
	int			ret = w25q_read_id(id);
	
	
	if(ret != RET_OK)
	{
		
		memset(info, 0, sizeof(fsh_info_t));
		return;
		
	}
	if( id[0] == 0xEF && id[1] == 0x17)		//w25Q128
	{
		info->total_pagenum = 65536;
		
		
	}
	
	if( id[0] == 0xEF && id[1] == 0x16)		//w25Q64
	{
		info->total_pagenum = 32768;
		
		
	}
	info->page_size = PAGE_SIZE;
	info->num_sct = info->total_pagenum / SECTOR_HAS_PAGES;
	info->num_blk = info->num_sct / BLOCK_HAS_SECTORS;
	
	w25q_mgr.sct_size = SECTOR_HAS_PAGES * info->page_size ;
}

void W25Q_WP(int protect)
{
	
	if(protect)
		W25Q_Enable_WP;
	else
		W25Q_Disable_WP;
}

int W25Q_erase(int opt, uint32_t num)
{
	int ret = ERR_DEV_FAILED;
	switch(opt)
	{
		
		case FSH_OPT_SECTOR:
			ret = w25q_Erase_Sector(num);
			break;
		case FSH_OPT_BLOCK:
			ret = w25q_Erase_block(num);
			break;
		case FSH_OPT_CHIP:
			ret = w25q_Erase_chip_c7();
			break;
		default:
			break;
		
	}
	
	return ret;
	
}

//todo: @sdhlib 
//从某一段连续的地址中，按照区块大小，取出连续的区域
//存放的结果是：起始地址，结束地址
//成功返回0，失败返回1
int W25Q_Cal_area(uint32_t start, uint32_t size, uint32_t area_sz, uint32_t rst[2])
{
	uint32_t  	cur_cache_addr = w25q_mgr.cur_sct * SECTOR_SIZE ;
	if(size < area_sz)
		return 1;
	
	rst[0] = ((start - 1) / area_sz + 1) * area_sz;
	rst[1] = ((start + size) / area_sz) * area_sz;
	if(rst[1] == rst[0])
		return 1;
	
	//计算擦除的区域是否与当前缓存的扇区重合
	
	if((cur_cache_addr > rst[0]) && (cur_cache_addr < rst[0]))
		w25q_mgr.cache_earse = 1;
	return 0;
}

//如果当前的缓存扇区位于擦除地址范围之内，会将当前缓存扇区数据刷到flash中
void W25Q_Erase_addr(uint32_t st, uint32_t sz)
{
	uint32_t		head_area[2] ;
	uint32_t		tail_area[2] ;
	uint32_t		middle_area[2];
	uint16_t		i;
	uint16_t		n1, n2;
	
	head_area[0] = st;
	head_area[1] = st + sz;

	tail_area[0] = st + sz;
	tail_area[1] = st + sz;
	
	//先取出块来进行擦除
	//1 计算出起始块号
	//2 计算出块的数量
	if(W25Q_Cal_area(st, sz, BLOCK_SIZE, middle_area))
		goto erase_sec;
	//3 擦除这些块
	n1 = middle_area[0] / BLOCK_SIZE;
	n2 = middle_area[1] / BLOCK_SIZE;
	for(i = n1; i < n2; i ++)
	{
		W25Q_erase(FSH_OPT_BLOCK, i);
		
	}
	//4 计算去掉被擦除的块之后的头部和尾部的位置
	head_area[0] = st;
	head_area[1] = middle_area[0];
	tail_area[0] = middle_area[1];
	tail_area[1] = st + sz;
	
	erase_sec:
	//再从剩下的头部和尾部两片区域中取出扇区进行擦除
	//此时与前一步块相连的部分是肯定扇区对齐的，因此不用考虑相连部分无法被扇区擦除擦干净
	//1 擦除头部区域的扇区
	//1.1 计算出起始扇区号
	//1.2 计算出块的扇区数量
	if(W25Q_Cal_area(head_area[0], head_area[1] - head_area[0], SECTOR_SIZE, middle_area))
		goto erase_sec_tial;
	n1 = middle_area[0] / SECTOR_SIZE;
	n2 = middle_area[1] / SECTOR_SIZE;
	//1.3 擦除这些扇区
	for(i = n1; i < n2; i ++)
	{
		W25Q_erase(FSH_OPT_SECTOR, i);
		
	}
	//1.4 计算去掉被擦除之后的剩余头部
	head_area[0] = st;
	head_area[1] = middle_area[0];
	
	erase_sec_tial:
	//2 擦除尾部区域的扇区
	//1.1 计算出起始扇区号
	//1.2 计算出块的扇区数量
	if(W25Q_Cal_area(tail_area[0], tail_area[1] - tail_area[0], SECTOR_SIZE, middle_area))
		goto erase_byte;
	n1 = middle_area[0] / SECTOR_SIZE;
	n2 = middle_area[1] / SECTOR_SIZE;
	//1.3 擦除这些扇区
	for(i = n1; i < n2; i ++)
	{
		W25Q_erase(FSH_OPT_SECTOR, i);
		
	}
	//1.4 计算去掉被擦除之后的剩余尾部
	tail_area[0] = middle_area[1];
	tail_area[1] = st + sz;
	
	erase_byte:
	//再从剩下的头部和尾部两片区域按字节写入oxff
	//1 擦除头部
	if(head_area[1] == st)
		goto erase_tail;
	//计算头部所处于的扇区号，读取该扇
	n1 = head_area[0] / SECTOR_SIZE;
	
//	if(n1 != w25q_mgr.cur_sct)
//	{
	
	W25Q_Flush();
	w25q_Read_Sector_Data(w25q_mgr.p_sct_buf, n1);
//	}
//	else
//	{
//		w25q_mgr.cache_earse = 1;
//		
//	}
	//擦除该扇区
	W25Q_erase(FSH_OPT_SECTOR, n1);
	//计算扣除头部位置之后要写入该扇区的字节数并写入
		
	
	i = head_area[0] - n1 * SECTOR_SIZE;
	n2 =  head_area[1] - n1 * SECTOR_SIZE;
	for(; i < n2; i++)
		w25q_mgr.p_sct_buf[i] = 0xff;
	w25q_Write_Sector_Data(w25q_mgr.p_sct_buf, n1);
		
	erase_tail:
	//2 擦除尾部
		if(tail_area[0] == st + sz)
			goto exit;
	//计算尾部所处于的扇区号，读取该扇
		n1 = tail_area[0] / SECTOR_SIZE;
		W25Q_Flush();
		w25q_Read_Sector_Data(w25q_mgr.p_sct_buf, n1);
	//擦除该扇区
		W25Q_erase(FSH_OPT_SECTOR, n1);
	//计算跳过尾部位置之后的数据长度，并在缓存中跳过尾部的长度后，写入flash
		n2 = tail_area[1] - n1 * SECTOR_SIZE;
		for(i = 0 ; i < n2; i++)
			w25q_mgr.p_sct_buf[i] = 0xff;
		w25q_Write_Sector_Data(w25q_mgr.p_sct_buf, n1);
		
		exit:
		
		//更新当前的缓存页面
		if(w25q_mgr.cur_sct != n1)
		{
	
		
			w25q_Read_Sector_Data(w25q_mgr.p_sct_buf, w25q_mgr.cur_sct);
			w25q_mgr.w25q_flag |= W25Q_FLAG_READED;
			w25q_mgr.w25q_flag &= ~W25Q_FLAG_DATA_CHANGED;
			
		}
		
		w25q_mgr.cache_earse = 0;

}

//将提供的扇区进行擦除操作。扇区号的范围是0 - 4096 （w25q128）




//返回写入的长度，或者小于0 的错误码
int w25q_Write(uint8_t *pBuffer, uint32_t WriteAddr, uint32_t WriteBytesNum)
{

//		short 			step = 0;
//		short 			count = 1000;
		int 				ret = 0;
		int					len;
		uint16_t		sct_num;
		uint16_t		sum = 0;
		sct_num = W25Q_Addr_2_sct(WriteAddr);
		
		//判断缓存区中是否存在该扇区的数据
		if((w25q_mgr.w25q_flag & W25Q_FLAG_READED) == 0) 
		{
			w25q_Read_Sector_Data(w25q_mgr.p_sct_buf ,sct_num);
			w25q_mgr.w25q_flag |= W25Q_FLAG_READED;
		}
		
		//要考虑写数据跨扇区的情况
		while(WriteBytesNum)
		{
			if(w25q_mgr.cur_sct != sct_num)
			{
				
				ret = W25Q_Change_cache_sct(sct_num);
			}
			
			if(ret < 0)
				return ret;
			len = W25Q_wr_cache(WriteAddr, pBuffer, WriteBytesNum);
			if(len < 0)
				return -1;
			
			WriteBytesNum -= len;
			WriteAddr += len;
			sct_num = W25Q_Addr_2_sct(WriteAddr);
			sum += len;
			pBuffer += len;
		}
	
		

		
		
		return sum;
}







//int w25q_Read_page_Data(uint8_t *pBuffer, uint16_t num_page)
//{
//	return w25q_rd_data( pBuffer, num_page * PAGE_SIZE, PAGE_SIZE);
//}

int w25q_rd_data(uint8_t *pBuffer, uint32_t rd_add, uint32_t len)
{
	
	int 			ret = 0;
	int 			rd_len;
	uint16_t		sct_num;
	uint16_t		sum = 0;
	sct_num = W25Q_Addr_2_sct(rd_add);
	
	//判断缓存区中是否存在该扇区的数据
	if((w25q_mgr.w25q_flag & W25Q_FLAG_READED) == 0) 
	{
		w25q_Read_Sector_Data(w25q_mgr.p_sct_buf ,sct_num);
		w25q_mgr.w25q_flag |= W25Q_FLAG_READED;
	}
	
	//要考虑写数据跨扇区的情况
	ret = 0;
	while(len)
	{
		if(w25q_mgr.cur_sct != sct_num)
		{
			
			ret = W25Q_Change_cache_sct(sct_num);
		}
		
		if(ret < 0)
			return ret;
		rd_len = W25Q_rd_cache(rd_add, pBuffer, len);
		if(rd_len < 0)
			return -1;
		len -= rd_len;
		rd_add += rd_len;
		sct_num = W25Q_Addr_2_sct(rd_add);
		sum += rd_len;
		pBuffer += rd_len;
	}
	return sum;
//	int  ret = ERR_DEV_FAILED;
//	if( len > PAGE_SIZE)
//		return ERR_BAD_PARAMETER;
//	W25Q_tx_buf[0] = W25Q_INSTR_READ_DATA;
//	W25Q_tx_buf[1] = (uint8_t)((rd_add&0x00ff0000)>>16);
//	W25Q_tx_buf[2] = (uint8_t)((rd_add&0x0000ff00)>>8);
//	W25Q_tx_buf[3] = (uint8_t)rd_add;
//	W25Q_Enable_CS;
//	
//	if(SPI_WRITE( W25Q_tx_buf, 4) != 4)
//		return ERR_DRI_OPTFAIL;
//	
//	ret = SPI_READ( pBuffer, len);
//	W25Q_Disable_CS;
//	return ret;

}

void W25Q_Flush(void)
{
	W25Q_Change_cache_sct(w25q_mgr.cur_sct);
	
}
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{


int	W25Q_lock(uint32_t ms)
{
	if(Sem_wait(&w25q_mgr.w25_sem, ms) > 0)
		return RET_OK;
	
	return RET_FAILED;
	
}
int	W25Q_unlock(void)
{
	
	Sem_post(&w25q_mgr.w25_sem);
	
	return RET_OK;

}

//当需要写入回读判断时，若回读出错，则废弃当前的写入记录，在该记录之后重新写入数据，最多尝试3次
//成功返回写入的数据字节数，失败返回-1
static int W25Q_Raw_write(uint8_t *pBuffer, uint32_t WriteAddr, uint32_t WriteBytesNum)
{
//	uint16_t total_wr_len = 0;
//	uint16_t ckeck_len = WriteBytesNum;
	int			ret = 0;
//	int			retry = 3;
	uint32_t	wr_offset;
//	uint8_t	 read_back_buf[32];
	

//	re_write:
	
	
	//第一个字节是被w25q使用的，作为废弃标记
	//赋值为0xff表示该记录正常
	//若需要回读判断，则进行回读判断
	if(W25Q_FSH.fnf.fnf_flag & FSH_FLAG_READBACK_CHECK)
		pBuffer[0] = 0xff;
	
	wr_offset = 0;
	//先写入，如果写入不成功，返回错误
	while(wr_offset < WriteBytesNum)
	{
		ret = W25Q_wr_fsh(pBuffer + wr_offset, WriteAddr + wr_offset, WriteBytesNum - wr_offset);
		if(ret < 0)
			return -1;
		wr_offset += ret;
	}
	
	
	return wr_offset;
	
	
//#if DUG_STG_HOLE == 1
//	
//	if(ckeck_len > 32)
//		ckeck_len = 32;
//	if(ckeck_len > ret)
//		ckeck_len = ret;
//	
//	w25q_Read_flash(read_back_buf, WriteAddr, ckeck_len);
//	
//	for(total_wr_len = 0; total_wr_len < ckeck_len; total_wr_len ++)
//	{
//		
//		if(read_back_buf[total_wr_len] != 0xff)
//			goto  rbk_check;
//		
//	}
//	//刚才写入的数据全是0xff，再次写入一遍
//	if(retry)
//	{
//		
//		retry --;
//		goto re_write;
//	}
//	
//#endif
//	rbk_check:
//	//若需要回读判断，则进行回读判断
//	if((W25Q_FSH.fnf.fnf_flag & FSH_FLAG_READBACK_CHECK) == 0)
//		return ret;
//	
//	//回读检查,最多只检查前32个字节
//	if(ret <= 0)
//		return ret;
//	
//	total_wr_len += ret;


//	if(ckeck_len > 32)
//		ckeck_len = 32;
//	if(ckeck_len > ret)
//		ckeck_len = ret;
//	
//	if(w25q_Read_flash(read_back_buf, WriteAddr, ckeck_len) != ckeck_len)
//		return total_wr_len;		//无法读取就先退出吧，总比用错误的数据进行判断好
//	if(memcmp(read_back_buf, pBuffer, ckeck_len) == 0)
//		return total_wr_len;
//	

//	
//	//如回读判断出错，废弃该记录
//	pBuffer[0] = 0;
//	W25Q_wr_fsh(pBuffer, WriteAddr, 1);
//		//并在下一个位置写入
//	if(retry)
//	{
//		WriteAddr += ret;
//		
//		//todo: 现在想不到什么好的办法来避免超过文件的范围，只能先判断是否超出flash范围了
//		if(WriteAddr >= W25Q_FSH.fnf.page_size * W25Q_FSH.fnf.total_pagenum)
//			return total_wr_len;
//		retry --;
//		goto re_write;
//	}
//	
//	return total_wr_len;
	
	
}

//WriteBytesNum不能大于256. 如果大于256字节，那么数据会从页的开头重新写入。
//因为这个函数使用的写入指令是W25Q_INSTR_Page_Program
//另外如果写入的位置+长度超过一页，会截断写入页面范围内的；没有考虑跨多页的情况
//页面长度必须是2的幂
static int	W25Q_wr_fsh(uint8_t *pBuffer, uint32_t WriteAddr, uint32_t WriteBytesNum)
{
	int ret = -1;
	uint16_t	in_page_offset;
	uint16_t	page_left_size;
	uint8_t 	step = 0;
	uint8_t 	count = 100;
	

	
	//指令W25Q_INSTR_Page_Program 最多只能写入一页的数据
	if(WriteBytesNum > PAGE_SIZE)
		WriteBytesNum = PAGE_SIZE;
	
	//对超出页面范围的数据进行截断
	in_page_offset = WriteAddr & (PAGE_SIZE - 1);
	page_left_size = PAGE_SIZE - in_page_offset;
	if(page_left_size < WriteBytesNum)
		WriteBytesNum = page_left_size;

	while(1)
	{
		switch( step)
		{
			case 0:
				if( w25q_wr_enable() != ERR_OK)
				{
					ret =  ERR_DRI_OPTFAIL;
					goto exit;
				}
				
				W25Q_Enable_CS;
				W25Q_tx_buf[0] = W25Q_INSTR_Page_Program;
				W25Q_tx_buf[1] = (uint8_t)((WriteAddr&0x00ff0000)>>16);
				W25Q_tx_buf[2] = (uint8_t)((WriteAddr&0x0000ff00)>>8);
				W25Q_tx_buf[3] = (uint8_t)WriteAddr;
				if(SPI_WRITE( W25Q_tx_buf, 4) != 4)
				{
					ret =  ERR_DRI_OPTFAIL;
					goto exit;
				}
				
				ret = SPI_WRITE( pBuffer, WriteBytesNum);
				W25Q_Disable_CS;
				
				step++;
				break;
			case 1:
				if(w25q_ReadSR() & W25Q_STATUS1_BUSYBIT)
				{
					W25Q_DELAY_MS(WriteBytesNum/100 + 1);
					if( count)
						count --;
					else
					{
						ret =  ERR_DEV_TIMEOUT;
						goto exit;
					}
					break;
				}
				
				goto exit;
		
			default:
				
				
				step = 0;
				break;
			
		}		//switch
		
		
	}		//while(1)
	exit:
	
	
	return ret;
	
}

//返回值是读取的全部字节数。
//因为可能会出现废弃的记录，本函数要自动识别出来，并往后读取下一个记录，此时返回的字节数就会与传入的长度不一样了
static int W25Q_Raw_Read(uint8_t *pBuffer, uint32_t rd_add, uint32_t len)
{
	int total_len = 0;
	int ret;
	
	reread:
	ret = w25q_Read_flash(pBuffer, rd_add, len);
	
	if(ret != len)
		return -1;
	
	if((W25Q_FSH.fnf.fnf_flag & FSH_FLAG_READBACK_CHECK) == 0)
		return ret;
	
	total_len += ret;
	//如果读取的数据被废弃了，就读取下一个数据
	if(pBuffer[0] != 0xff)
	{
		
		rd_add += ret;

		//todo: 现在想不到什么好的办法来避免超过文件的范围，只能先判断是否超出flash范围了
		if(rd_add >= W25Q_FSH.fnf.page_size * W25Q_FSH.fnf.total_pagenum)
			return total_len;
		goto reread;
	}
	
	return total_len;
}

static int w25q_Read_flash(uint8_t *pBuffer, uint32_t rd_add, uint32_t len)
{
	int read_len;
	int	safe_count = 20;
	while(safe_count)
	{
		W25Q_tx_buf[0] = W25Q_INSTR_READ_DATA;
		W25Q_tx_buf[1] = (uint8_t)((rd_add&0x00ff0000)>>16);
		W25Q_tx_buf[2] = (uint8_t)((rd_add&0x0000ff00)>>8);
		W25Q_tx_buf[3] = (uint8_t)rd_add;
		W25Q_Enable_CS;
		
		if( SPI_WRITE( W25Q_tx_buf, 4) != 4)
			return ERR_DRI_OPTFAIL;
		read_len = SPI_READ( pBuffer, len);
		
		W25Q_Disable_CS;
		if(read_len == len)
			break;
		if(safe_count)
			safe_count --;
		else
			break;
	}
	return read_len;
}

static int w25q_Write_Sector_Data(uint8_t *pBuffer, uint16_t Sector_Num)
{
	int wr_page = 0;
	int		ret;
	int		sum = 0;
	int		retry = 10;
	while(1)
	{
		ret = W25Q_wr_fsh(pBuffer + wr_page*PAGE_SIZE, Sector_Num*SECTOR_SIZE + wr_page * PAGE_SIZE, PAGE_SIZE);
		if(ret != PAGE_SIZE)
		{
			if(retry)
			{
				retry --;
				continue;
			}
			
		}
		retry = 10;
		if( ret != PAGE_SIZE)
			return sum;
		sum += ret;
		wr_page ++;
		if( wr_page >= SECTOR_HAS_PAGES)
			return sum;
	}
	
   
}

static int w25q_Read_Sector_Data(uint8_t *pBuffer, uint16_t Sector_Num)
{
	uint8_t Block_Num = Sector_Num / BLOCK_HAS_SECTORS;
	int		ret = 0;
	if( Sector_Num > W25Q_FSH.fnf.num_sct)
		return ERR_BAD_PARAMETER;
	memset( pBuffer, 0xff, SECTOR_SIZE);
	
	W25Q_Enable_CS;
	W25Q_tx_buf[0] = W25Q_INSTR_READ_DATA;
	W25Q_tx_buf[1] = Block_Num;
	W25Q_tx_buf[2] = Sector_Num<<4;
	W25Q_tx_buf[3] = 0;
	if( SPI_WRITE(W25Q_tx_buf, 4) != 4)
		return ERR_DRI_OPTFAIL;
	ret = SPI_READ(pBuffer, SECTOR_SIZE);
//	if(SPI_READ(pBuffer, SECTOR_SIZE) != SECTOR_SIZE)
//		return ERR_DRI_OPTFAIL;
	W25Q_Disable_CS;
	return ret;;
}
static int w25q_wr_enable(void)
{
	int ret = ERR_DEV_FAILED;
	int count = 100;
	uint8_t cmd = W25Q_INSTR_WR_ENABLE;
	
	
	
	W25Q_Enable_CS;
//	if(SPI_WRITE( &cmd,1) == 1)
//	{
//		
//		ret = RET_OK;
//	}
	SPI_WRITE( &cmd,1);
	W25Q_Disable_CS;
	
	
	ret = ERR_DEV_FAILED;

	while(count)
	{
		if(w25q_ReadSR() & W25Q_STATUS1_WEL)
		{
			ret = RET_OK;
			break;
		}
		count --;
		
	}
	
	exit:
	return ret;
	
	
}

static uint8_t w25q_ReadSR(void)
{
    uint8_t cmd = 0;
	uint8_t	status;
    W25Q_Enable_CS;
	
	
	cmd = W25Q_INSTR_RD_Status_Reg1;
	
	if( SPI_WRITE( &cmd, 1) != 1)
		return 0xff;
	if( SPI_READ( &status, 1) != 1)
		return 0xff;
    W25Q_Disable_CS;
    return status;
}


static int W25Q_send_wait(uint8_t *data, uint16_t len, uint16_t ms)
{
	uint16_t step = 0;
	uint16_t count = ms / 100;
	int ret = -1;
	
	
	while(1)
	{
		switch( step)
		{	
			case 0:
				
				if( w25q_wr_enable() != ERR_OK)
				{
					ret =  ERR_DRI_OPTFAIL;
					goto exit;
				}
				
				W25Q_Enable_CS;
				if( SPI_WRITE( data, len) != len)
				{
					ret =  ERR_DRI_OPTFAIL;
					goto exit;
				}
				step ++;
				W25Q_Disable_CS;

				break;
			case 1:
				if(w25q_ReadSR() & W25Q_STATUS1_BUSYBIT)
				{
					W25Q_DELAY_MS(100);
					if( count)
						count --;
					else
					{
						ret =  ERR_DEV_TIMEOUT;
						goto exit;
					}
					break;
				}
				
				ret =  ERR_OK;
				goto exit;
			default:
				step = 0;
				break;
		}
	}
		
	exit:
	W25Q_Disable_CS;
	return ret;

}

static int W25Q_sct_offset(uint16_t	sct_num, uint32_t	addr)
{
	int 	sct_offset = 0;
	uint16_t	sct = W25Q_Addr_2_sct(addr);
	
	
	
	if(sct < sct_num)
	{
		return -1;
	}
	else
	{
		sct_offset = addr - sct * w25q_mgr.sct_size;
		sct_offset += (sct - sct_num) * w25q_mgr.sct_size;
		return sct_offset;
	}
}

static int W25Q_wr_cache(uint32_t addr, uint8_t *wr_buf, uint16_t	wr_len)
{
	int	offset;
	uint16_t	len;
	
	offset = W25Q_sct_offset(w25q_mgr.cur_sct, addr);
	if(offset < 0)
		return -1;
		
	len = w25q_mgr.sct_size - offset;
	if(wr_len <= len)
		len = wr_len;
	memcpy(w25q_mgr.p_sct_buf + offset, wr_buf, len);
	w25q_mgr.w25q_flag |= W25Q_FLAG_DATA_CHANGED;
	return len;
}

static int W25Q_rd_cache(uint32_t addr, uint8_t *rd_buf, uint16_t	rd_len)
{
	int	offset;
	uint16_t	len;
	
	offset = W25Q_sct_offset(w25q_mgr.cur_sct, addr);
	if(offset < 0)
		return -1;
	len = w25q_mgr.sct_size - offset;
	if(rd_len <= len)
		len = rd_len;
	memcpy(rd_buf, w25q_mgr.p_sct_buf + offset,  len);
	
	
	return len;
}
	


//static int w25q_write_waitbusy(uint8_t *data, int len)
//{
//	short step = 0;
//	short count = 1000;
//	int ret = -1;
//	
//	
//	while(1)
//	{
//		switch( step)
//		{	
//			case 0:
//				
//				if( w25q_wr_enable() != ERR_OK)
//				{
//					ret =  ERR_DRI_OPTFAIL;
//					goto exit;
//				}
//				
//				W25Q_Enable_CS;
//				if( SPI_WRITE( data, len) != len)
//				{
//					ret =  ERR_DRI_OPTFAIL;
//					goto exit;
//				}
//				step ++;
//				W25Q_Disable_CS;

////				W25Q_DELAY_MS(1);
//				break;
//			case 1:
//				if( w25q_ReadSR() & W25Q_STATUS1_BUSYBIT)
//				{
//					W25Q_DELAY_MS(1);
//					if( count)
//						count --;
//					else
//					{
//						ret =  ERR_DEV_TIMEOUT;
//						goto exit;
//					}
//					break;
//				}
//				
//				ret =  ERR_OK;
//				goto exit;
//			default:
//				step = 0;
//				break;
//		}
//	}
//		
//	exit:
//	W25Q_Disable_CS;
//	return ret;

//}


static int w25q_read_id(uint8_t  buf[2])
{
	//read id
	W25Q_tx_buf[0] = 0x90;
	W25Q_tx_buf[1] = 0;
	W25Q_tx_buf[2] = 0;
	W25Q_tx_buf[3] = 0;
	W25Q_Enable_CS;
	
	SPI_WRITE( W25Q_tx_buf, 4);
	SPI_READ( buf, 2);
	
	W25Q_Disable_CS;
	
	if( buf[0] == 0xEF && buf[1] == 0x17)		//w25Q128
	{
		
		return ERR_OK;
	}
	
	if( buf[0] == 0xEF && buf[1] == 0x16)		//w25Q64
	{
		
		return ERR_OK;
	}
	

	return ERR_FAIL;
	
	
}


static int w25q_Erase_Sector(uint32_t Sector_Number)
{

	uint8_t Block_Num = Sector_Number / BLOCK_HAS_SECTORS;
	if( Sector_Number > W25Q_FSH.fnf.num_sct)
			return ERR_BAD_PARAMETER;
	
	Sector_Number %= BLOCK_HAS_SECTORS;
	W25Q_tx_buf[0] = W25Q_INSTR_Sector_Erase_4K;
	W25Q_tx_buf[1] = Block_Num;
	W25Q_tx_buf[2] = Sector_Number<<4;
	W25Q_tx_buf[3] = 0;
	return W25Q_send_wait( W25Q_tx_buf, 4, 2000);
	
}


static int w25q_Erase_block(uint16_t block_Number)
{

	if( block_Number > W25Q_FSH.fnf.num_blk)
		return ERR_BAD_PARAMETER;
	
	W25Q_tx_buf[0] = W25Q_INSTR_BLOCK_Erase_64K;
	W25Q_tx_buf[1] = block_Number;
	W25Q_tx_buf[2] = 0;
	W25Q_tx_buf[3] = 0;
	return W25Q_send_wait( W25Q_tx_buf, 4, 10000);
	
}

static int w25q_Erase_chip_c7(void)
{

	W25Q_tx_buf[0] = W25Q_INSTR_Chip_Erase_C7;
	
	return W25Q_send_wait( W25Q_tx_buf, 1, 45000);
	
}
int w25q_Erase_chip_60(void)
{

	W25Q_tx_buf[0] = W25Q_INSTR_Chip_Erase_60;
	
	return W25Q_send_wait( W25Q_tx_buf, 1, 20000);
	
}

static int W25Q_ReadBack_check(uint16_t sct_num, uint8_t *chk_buf)
{
	
	uint32_t	*p1, *p2;
	short			i = 0, max;
	
	if(w25q_mgr.p_chk_buf == NULL)
	{
		w25q_mgr.p_chk_buf = (uint8_t *)malloc(w25q_mgr.sct_size);
		
	}
	w25q_Read_Sector_Data(w25q_mgr.p_chk_buf, sct_num);
	
	p1 = (uint32_t *)w25q_mgr.p_chk_buf;
	p2 = (uint32_t *)chk_buf;

	max = w25q_mgr.sct_size / 4;
	
	for(i = 0; i < max; i++)
	{
		if(p1[i] != p2[i])
		{
			return 1;
		}
		
	}
	
	return 0;
	
}

static uint16_t W25Q_Addr_2_sct(uint32_t addr)
{
	return addr / w25q_mgr.sct_size;
	
}

static int	W25Q_Change_cache_sct(uint16_t	sct_num)
{
	int	ret = 1;
	int	safe_count = 1000;
	
	if(w25q_mgr.w25q_flag & W25Q_FLAG_DATA_CHANGED)
	{
		while(ret)
		{
			
			
			w25q_Erase_Sector(w25q_mgr.cur_sct);
			w25q_Write_Sector_Data(w25q_mgr.p_sct_buf, w25q_mgr.cur_sct);
			if(W25Q_FSH.fnf.fnf_flag & FSH_FLAG_READBACK_CHECK)
				ret = W25Q_ReadBack_check(w25q_mgr.cur_sct, w25q_mgr.p_sct_buf);
			else
				ret = 0;
		
			if(safe_count)
				safe_count --;
			else
				break;
		}
	}
	
	
	if(safe_count == 0)
		return ERR_DEV_FAILED;
	
	safe_count = 1000;
	if(sct_num != w25q_mgr.cur_sct)
	{
		while(1)
		{
			ret = w25q_Read_Sector_Data(w25q_mgr.p_sct_buf, sct_num);
			if(ret == w25q_mgr.sct_size)
				break;
			if(safe_count)
				safe_count --;
			else
				break;	
		}
		w25q_mgr.cur_sct = sct_num;

	}
	w25q_mgr.w25q_flag |= W25Q_FLAG_READED;
	w25q_mgr.w25q_flag &= ~W25Q_FLAG_DATA_CHANGED;
	
	
	
	
	
	return ret;
}






