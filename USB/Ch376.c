//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

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
#include <string.h>		//NULL 
#include "Ch376.h"
#include "usb_hardware_interface.h"

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------




//#define xWriteCH376Data(d)		(ch376_dev->write(ch376_dev, &d, 1))
//#define xWriteCH376Data_p(ptr, n)		(ch376_dev->write(ch376_dev, ptr, n))
//#define xReadCH376Data(d)		(ch376_dev->read(ch376_dev, &d, 1))
//#define xReadCH376Data_p(ptr, n)		(ch376_dev->read(ch376_dev, ptr, n))
//#define	xEndCH376Cmd( )			p_ch376->usb_cs_off()//结束CH376命令,仅用于SPI接口方式
//#define CH376_CMD_PORT	(ch376_dev->write(ch376_dev, &d, 1))

#define CH376_DATABUF_SIZE			64
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
static usb_op_t	*p_ch376;
static uint8_t DataBuff[CH376_DATABUF_SIZE];
static uplevel_intr ch376_up_irq = NULL;

#define xWriteCH376Data(d)		(p_ch376->usb_write_bytes(&d, 1))
#define xWriteCH376Data_p(ptr, n)		(p_ch376->usb_write_bytes(ptr, n))
#define xReadCH376Data(d)		(p_ch376->usb_read_bytes(&d, 1))
#define xReadCH376Data_p(ptr, n)		(p_ch376->usb_read_bytes(ptr, n))
#define	xEndCH376Cmd( )			p_ch376->usb_cs_off()//结束CH376命令,仅用于SPI接口方式
#define	xStartCH376Cmd( )			p_ch376->usb_cs_on()//开始CH376命令,仅用于SPI接口方式
#define CH376_CMD_PORT	(p_ch376->usb_write_bytes(&d, 1))

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------

//static void p_ch376->usb_delay_ms(unsigned long time);
//static void Delay10us(unsigned long time);
static void xWriteCH376Data_u32(uint32_t data_u32);
static void xWriteCH376Cmd(uint8_t mCmd);
static uint8_t CH376GetIntStatus(void);
static uint8_t	CH376SendCmdWaitInt( uint8_t mCmd );
static uint8_t Wait376Interrupt(int set_irq )  ;
static uint8_t	CH376ReadVar8( uint8_t var );  /* 读CH376芯片内部的8位变量 */
static void	CH376WriteHostBlock( uint8_t *buf, uint8_t len );
static uint8_t	CH376DiskReqSense( void );  
static void	CH376SetFileName(char *name );
static uint8_t	CH376SendCmdDatWaitInt( uint8_t mCmd, uint8_t mDat )  ;
static void	CH376WriteVar32(uint8_t var, uint32_t dat );  /* 写CH376芯片内部的32位变量 */
static uint32_t	CH376Read32bitDat( void );
static  void Ch376_intr(void);
static uint8_t	CH376DiskWriteSec( uint8_t *buf, uint32_t iLbaStart, uint8_t iSectorCount );
static  uint8_t	CH376WriteReqBlock( uint8_t *buf );
static void	CH376WriteOfsBlock(uint8_t * buf, uint8_t ofs, uint8_t len );
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
int	Init_Ch376(void *op, uplevel_intr up_irq)
{
	int ret = 0;
	
	p_ch376 = (usb_op_t *)op;
	p_ch376->usb_irq_cb = Ch376_intr;
	p_ch376->usb_reset();
//	ret = Dev_open(DEVID_GPIO_USB_INIT, (void *)&ch376_int);
	p_ch376->usb_set_irq(0);
//	
//	ret = Dev_open(dev_id, (void *)&ch376_dev);
//	
//	ch376_int->ioctol(ch376_int, DEVCMD_SET_IRQHDL, Ch376_intr, (void *)NULL);
//	HRst_Ch376();
	ch376_up_irq = up_irq;
 
	ret = mInitCH376Host();
	
	p_ch376->usb_set_irq(1);
	
	if( CH376GetIntStatus() == USB_INT_CONNECT)
	{
		
		ch376_up_irq(USB_INT_CONNECT);
	}
	return ret;
	
}

void	Search_u_disk(void)
{
	uint8_t  s;
	
	if(ch376_up_irq == NULL)
		return;
	
	s = CH376GetIntStatus();
	if( s == USB_INT_CONNECT)
		ch376_up_irq(USB_INT_CONNECT);
//	else if(s ==USB_INT_DISCONNECT) 
//		ch376_up_irq(USB_INT_DISCONNECT);
	
}

//void Power_Ch376(int on)
//{
//	if(on) 
//	{
//		SET_CH376PWR_LOW;
//	}
//	else 
//	{
//		
//		
//		SET_CH376PWR_HIGH;
//	}
//	
//}

//void p_ch376->usb_set_irq(int ed)
//{
//	if(ed) 
//	{
//		ch376_int->ioctol(ch376_int,DEVCMD_ENABLE_IRQ);
//		
//	}
//	else 
//	{
//		
//		ch376_int->ioctol(ch376_int,DEVCMD_DISABLE_IRQ);
//	}
//	
//	
//}

//void HRst_Ch376(void)
//{
//	SET_CH376RST_HIGH;
//	p_ch376->usb_delay_ms(100);
//	
//	SET_CH376RST_LOW;
//	p_ch376->usb_delay_ms(100);
//	
//	p_ch376->usb_cs_off();
//	p_ch376->usb_delay_ms(100);
//	
////	
//	
////	uint8_t	s = 0;
////	

////	p_ch376->usb_cs_off();
////	s = CH376SendCmdWaitInt(CMD00_RESET_ALL);
////	xEndCH376Cmd();
////	p_ch376->usb_delay_ms(50);			//通常35ms内完成

//}

//初始化CH376
uint8_t mInitCH376Host(void)
{
	uint8_t res = 0;
	uint8_t	usb_data;

	//检测通讯接口
	
	xWriteCH376Cmd(CMD11_CHECK_EXIST);
	p_ch376->usb_delay_ms(100);
	usb_data = 0x65;
	xWriteCH376Data(usb_data);
	p_ch376->usb_delay_ms(100);
	xReadCH376Data(res);
	p_ch376->usb_cs_off();
	if (res != 0x9A)
	{
		return (ERR_USB_UNKNOWN);
	}


	//获取芯片及固件版本
	xWriteCH376Cmd(CMD01_GET_IC_VER);
	p_ch376->usb_delay_ms(1000);
	xReadCH376Data(res);
	p_ch376->usb_cs_off();
	if (res != 0x41)
	{
//		return (ERR_USB_UNKNOWN);
		res = 0x41;
	}


	//设置USB工作模式
	xWriteCH376Cmd(CMD11_SET_USB_MODE);
	usb_data = 0x06;
	xWriteCH376Data(usb_data);	//已启用的主机方式并且自动产生SOF包
	p_ch376->usb_delay_ms(100);
	xReadCH376Data(res);
	p_ch376->usb_cs_off();
	if (res == CMD_RET_SUCCESS)
	{
		return USB_INT_SUCCESS;
	}
	else
	{
		return ERR_USB_UNKNOWN;
	}
}

// 检查U盘是否连接,不支持SD卡 
uint8_t Ch376DiskConnect(void)
{
	
	CH376GetIntStatus( );  // 清除中断
	return( CH376SendCmdWaitInt( CMD0H_DISK_CONNECT ) );
	
}

// 初始化磁盘并测试磁盘是否就绪
uint8_t CH376DiskMount( void ) 
{
	return( CH376SendCmdWaitInt( CMD0H_DISK_MOUNT ) );
}


// 从当前主机端点的接收缓冲区读取数据块,返回长度 
uint8_t	CH376ReadBlock( uint8_t *buf, int buf_size  )
{
	uint8_t	s, l;

	xWriteCH376Cmd( CMD01_RD_USB_DATA0 );
	xReadCH376Data(l);  /* 长度 */
//	xReadCH376Data(s); /* 长度 */
	
	if ( l ) {
		if(l > buf_size)
			l = buf_size;
		s = xReadCH376Data_p(buf, l);
		
	}
	xEndCH376Cmd( );
	return( s );
}

// 检查U盘是否写保护, 返回USB_INT_SUCCESS说明可以写,返回0xFF说明只读/写保护,返回其它值说明是错误代码 
uint8_t	IsDiskWriteProtect( void )
{
	uint8_t	s, SysBaseInfo;
	P_BULK_ONLY_CBW	pCbw;

	SysBaseInfo = CH376ReadVar8( VAR_SYS_BASE_INFO );  // 当前系统的基本信息 
	pCbw = (P_BULK_ONLY_CBW)DataBuff;
	for ( s = 0; s != sizeof( pCbw -> CBW_CB_Buf ); s ++ )
		pCbw -> CBW_CB_Buf[ s ] = 0;  // 默认清0 
	pCbw -> CBW_DataLen0 = 0x10;  // 数据传输长度 
	pCbw -> CBW_Flag = 0x80;  // 传输方向为输入 
	if ( SysBaseInfo & 0x40 )
	{  
		// SubClass-Code子类别非6 
		pCbw -> CBW_CB_Len = 10;  // 命令块的长度 
		pCbw -> CBW_CB_Buf[0] = 0x5A;  // 命令块首字节, MODE SENSE(10) 
		pCbw -> CBW_CB_Buf[2] = 0x3F;
		pCbw -> CBW_CB_Buf[8] = 0x10;
	}
	else
	{ 
		// SubClass-Code子类别为6 
		pCbw -> CBW_CB_Len = 6;  // 命令块的长度 
		pCbw -> CBW_CB_Buf[0] = 0x1A;  // 命令块首字节, MODE SENSE(6) 
		pCbw -> CBW_CB_Buf[2] = 0x3F;
		pCbw -> CBW_CB_Buf[4] = 0x10;
	}

	// 向USB主机端点的发送缓冲区写入数据块,剩余部分CH376自动填补 
	CH376WriteHostBlock( (uint8_t *)pCbw, sizeof( BULK_ONLY_CBW ) );
	// 对U盘执行BulkOnly传输协议 
	s = CH376SendCmdWaitInt( CMD0H_DISK_BOC_CMD );  
	if ( s == USB_INT_SUCCESS )
	{
		// 从当前主机端点的接收缓冲区读取数据块,返回长度 
		s = CH376ReadBlock( DataBuff, CH376_DATABUF_SIZE );  
		if ( s > 3 )
		{
			// MODE SENSE命令返回数据的长度有效 
			if ( SysBaseInfo & 0x40 )
				s = DataBuff[3];  // MODE SENSE(10), device specific parameter 
			else
				s = DataBuff[2];  // MODE SENSE(6), device specific parameter 
			if ( s & 0x80 )
				return( 0xFF );  // U盘写保护 
			else
				return( USB_INT_SUCCESS );  // U盘没有写保护 
		}
		return( USB_INT_DISK_ERR );
	}

	CH376DiskReqSense( );  // 检查USB存储器错误 
	return( s );
}


// 查询磁盘剩余空间信息,扇区数 
uint8_t	CH376DiskQuery(uint32_t *DiskAll, uint32_t *DiskFre, uint8_t *diskFat  )  
{
	uint8_t	s,temp;
	uint8_t	c0, c1, c2, c3;

	s = CH376SendCmdWaitInt( CMD0H_DISK_QUERY );
	if ( s == USB_INT_SUCCESS )
	{
		// 参考CH376INC.H文件中CH376_CMD_DATA结构的DiskQuery 
		xWriteCH376Cmd( CMD01_RD_USB_DATA0 );
		xReadCH376Data(c0 );  // 长度总是sizeof(CH376_CMD_DATA.DiskQuery) 
		xReadCH376Data(c1 );  // CH376_CMD_DATA.DiskQuery.mTotalSector 
		xReadCH376Data(c2 );
		xReadCH376Data(c3 );
		*DiskAll = c0 | (uint16_t)c1 << 8 | (uint32_t)c2 << 16 | (uint32_t)c3 << 24;
		xReadCH376Data(c0 );  // CH376_CMD_DATA.DiskQuery.mFreeSector 
		xReadCH376Data(c1 );
		xReadCH376Data(c2 );
		xReadCH376Data(c3 );
		*DiskFre = c0 | (uint16_t)c1 << 8 | (uint32_t)c2 << 16 | (uint32_t)c3 << 24;

		xReadCH376Data(temp );  // CH376_CMD_DATA.DiskQuery.mDiskFat 
		*diskFat = temp;
		xEndCH376Cmd( );
		
	}
	else
		*DiskFre = 0;
	return( s );
}


uint8_t	CH376FileOpen(char* name )  /* 在根目录或者当前目录下打开文件或者目录(文件夹) */
{
	CH376SetFileName( name );  /* 设置将要操作的文件的文件名 */
	if ( name[0] == DEF_SEPAR_CHAR1 || name[0] == DEF_SEPAR_CHAR2 ) CH376WriteVar32(VAR_CURRENT_CLUST, 0 );
	return( CH376SendCmdWaitInt( CMD0H_FILE_OPEN ) );
}

// 在根目录或者当前目录下新建文件,如果文件已经存在那么先删除 
uint8_t	CH376FileCreate(char *name )
{
	// 设置将要操作的文件的文件名 
	if ( name )
		CH376SetFileName( name );  
//	p_ch376->usb_delay_ms(100);
	return(CH376SendCmdWaitInt(CMD0H_FILE_CREATE ) );
}

// 关闭当前已经打开的文件或者目录(文件夹)
uint8_t	CH376FileClose( uint8_t is_update_size )
{
	return( CH376SendCmdDatWaitInt( CMD1H_FILE_CLOSE, is_update_size ) );
}

// 以扇区为单位在当前位置写入数据块,不支持SD卡 
//数据缓冲区、要写入的扇区数、返回真正写入的数据个数
uint8_t	CH376SecWrite( uint8_t *buf, uint8_t ReqCount, uint8_t *RealCount )  
{
	uint8_t	s;
	uint8_t	cnt,temp;
	uint32_t	StaSec;

	if ( RealCount )
		*RealCount = 0;

	do
	{
		xWriteCH376Cmd( CMD1H_SEC_WRITE );
		xWriteCH376Data( ReqCount );
		xEndCH376Cmd( );

		s = Wait376Interrupt( 1);
		if ( s != USB_INT_SUCCESS )
			return( s );
		xWriteCH376Cmd( CMD01_RD_USB_DATA0 );
		xReadCH376Data(temp );  // 长度总是sizeof(CH376_CMD_DATA.SectorWrite) 

		xReadCH376Data(cnt );  // CH376_CMD_DATA.SectorWrite.mSectorCount 
		xReadCH376Data(temp );
		xReadCH376Data(temp );
		xReadCH376Data(temp );

		// CH376_CMD_DATA.SectorWrite.mStartSector,从CH376芯片读取32位的数据并结束命令 
		StaSec = CH376Read32bitDat( ); 
		if ( cnt == 0 )
			break;
		s = CH376DiskWriteSec( buf, StaSec, cnt );  // 将缓冲区中的多个扇区的数据块写入U盘 
		if ( s != USB_INT_SUCCESS )
			return( s );
		buf += cnt * DEF_SECTOR_SIZE;
		if ( RealCount ) *RealCount += cnt;
		ReqCount -= cnt;
	} while ( ReqCount );

	return( s );
}

/*******************************************************************************
* 函  数  名      : CH376ByteRead
* 描      述      : 以字节为单位从当前位置读取数据块
* 输      入      : PUINT8 buf:
*					指向数据缓冲区.
*                   UINT16 ReqCount：
*                   请求读取的字节数.
*                   PUINT16 RealCount:
*                   实际读取的字节数.
* 返      回      : 中断状态.
*******************************************************************************/
uint8_t	CH376ByteRead(uint8_t* buf, uint16_t ReqCount, uint16_t* RealCount )
{
	uint8_t	s;
	
	xWriteCH376Cmd( CMD2H_BYTE_READ );
	xWriteCH376Data_p((uint8_t *)&ReqCount, 2 );
	xEndCH376Cmd( );
	if ( RealCount ) 
	{
	    *RealCount = 0;
	}
	
	while ( 1 ) 
	{
		s = Wait376Interrupt( 1);
		if ( s == USB_INT_DISK_READ )                                                   /* 请求数据读出 */
		{
			s = CH376ReadBlock( buf, ReqCount );                                                  /* 从当前主机端点的接收缓冲区读取数据块,返回长度 */
			xWriteCH376Cmd( CMD0H_BYTE_RD_GO );                                         /* 继续读 */
			xEndCH376Cmd( );
			buf += s;
			if ( RealCount ) 
			{
			    *RealCount += s;
			}
		}
		else 
		{
		    return( s );                                                                /* 错误 */
		}
	}
}

//以字节为单位向当前位置写入数据块 
uint8_t	CH376ByteWrite( uint8_t *buf, uint16_t ReqCount, uint16_t *RealCount )
{
	uint8_t	s;
	uint8_t	u16_part;
	
	
	p_ch376->usb_set_irq(0);
	xWriteCH376Cmd( CMD2H_BYTE_WRITE );
	u16_part = ReqCount;
	xWriteCH376Data(u16_part);
	u16_part = ReqCount>>8;
	xWriteCH376Data(u16_part);
	xEndCH376Cmd( );

	if ( RealCount )
		*RealCount = 0;

	while ( 1 )
	{
		
		s = Wait376Interrupt(0);
		
		if ( s == USB_INT_DISK_WRITE )
		{
			//向内部指定缓冲区写入请求的数据块,返回长度 
			s = CH376WriteReqBlock( buf );
			xWriteCH376Cmd( CMD0H_BYTE_WR_GO );
			xEndCH376Cmd( );
			buf += s;
			if ( RealCount )
				*RealCount += s;
		}
		else
		{
			p_ch376->usb_set_irq(1);
			return( s );  // 错误
			
		}
	}
	
//	p_ch376->usb_set_irq(1);
	
//	return 0;
}




// 将缓冲区中的多个扇区的数据块写入U盘,不支持SD卡 
// baStart 是写入的线起始性扇区号, iSectorCount 是写入的扇区数 
static uint8_t	CH376DiskWriteSec( uint8_t *buf, uint32_t iLbaStart, uint8_t iSectorCount )  
{
	uint8_t		s, err;
	uint16_t	mBlockCount;

	for ( err = 0; err != 3; ++ err )
	{
		// 出错重试 
		xWriteCH376Cmd( CMD5H_DISK_WRITE );  // 向USB存储器写扇区 
		xWriteCH376Data_u32(iLbaStart);
		
		xWriteCH376Data( iSectorCount );  // 扇区数 
		xEndCH376Cmd( );

		mBlockCount = iSectorCount * DEF_SECTOR_SIZE / CH376_DAT_BLOCK_LEN;
		
		for ( ; mBlockCount != 0; -- mBlockCount )
		{ 
			// 数据块计数 
			s = Wait376Interrupt( 1);  // 等待中断并获取状态 ，应返回0x1E
			if ( s == USB_INT_DISK_WRITE )
			{
				// USB存储器写数据块,请求数据写入 
				CH376WriteHostBlock( buf, CH376_DAT_BLOCK_LEN );  // 向USB主机端点的发送缓冲区写入数据块 
				xWriteCH376Cmd( CMD0H_DISK_WR_GO );  // 继续执行USB存储器的写操作 
				xEndCH376Cmd( );
				buf += CH376_DAT_BLOCK_LEN;
			}
			else
				break;  // 返回错误状态 
		}
		if ( mBlockCount == 0 )
		{
			s = Wait376Interrupt(1 );  // 等待中断并获取状态 
			if ( s == USB_INT_SUCCESS )
				return( USB_INT_SUCCESS );  // 操作成功 
		}
		if ( s == USB_INT_DISCONNECT )
			return( s );  // U盘被移除 
		CH376DiskReqSense( );  // 检查USB存储器错误 
	}
	return( s );  // 操作失败 
}
uint8_t	CH376_Set_Data_Time(int dtm_type, uint16_t	dtm)
{
//	FAT_DIR_INFO	*fat_info = (FAT_DIR_INFO	*)DataBuff;
	uint8_t s;
	uint8_t offset = 0;
	//1 读取当前文件的FAT_DIR_INFO结构到内存缓冲区
	s = CH376SendCmdDatWaitInt( CMD1H_DIR_INFO_READ, 0xFF );
	if ( s != USB_INT_SUCCESS ) 
		return s;
	//2 c从内存缓冲区读出数据
	CH376ReadBlock(DataBuff, CH376_DATABUF_SIZE);
	
	//3 再次读取当前文件的FAT_DIR_INFO结构到内存缓冲区
	s = CH376SendCmdDatWaitInt( CMD1H_DIR_INFO_READ, 0xFF );
	if ( s != USB_INT_SUCCESS ) 
		return s;
	//4 根据要修改的参数类型写入内存缓冲偏移地址
	switch(dtm_type)
	{
		case DTM_CREATE_DATE:
			offset = (uint32_t)(&((FAT_DIR_INFO *)0)->DIR_CrtDate);
			break;
		case DTM_CREATE_TIME:
			offset = (uint32_t)(&((FAT_DIR_INFO *)0)->DIR_CrtTime);;
			break;		
		case DTM_CHANGE_DATE:
			offset = (uint32_t)(&((FAT_DIR_INFO *)0)->DIR_WrtDate);;
			break;
		case DTM_CHANGE_TIME:
			offset = (uint32_t)(&((FAT_DIR_INFO *)0)->DIR_WrtTime);;
			break;
		case DTM_LASTACC_DATE:
			offset = (uint32_t)(&((FAT_DIR_INFO *)0)->DIR_LstAccDate);;
			break;
		
		
		default:
			return USB_INT_BUF_OVER;		//数据错误
		
	}
	
	//5 写入要修改的数据
	CH376WriteOfsBlock((uint8_t *)&dtm, offset, 2);
	//6 保存文件信息
	
	return( CH376SendCmdWaitInt( CMD0H_DIR_INFO_SAVE ) );
}
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{


static void xWriteCH376Cmd(uint8_t mCmd)
{
//	uint8_t i,res;

	//ch376将SPI片选有效后第一个字节作为命令
//	SET_CH376ENA_LOW;
//	ch376_dev->write(ch376_dev, &mCmd, 1);
		xStartCH376Cmd();
		xWriteCH376Data(mCmd);

	
	
//	for (i=0;i<10;i++)
//	{
//		//检查状态端口的标志位，位4：忙标志，高有效
//		res = xReadCH376Status();
//		if (( res & PARA_STATE_BUSY) == 0)
//		{
//			break;
//		}
//	}
}

// 获取中断状态并取消中断请求 

static uint8_t CH376GetIntStatus(void)
{
	uint16_t	sc = 1000;
	uint8_t s;

	while(1)
	{
		xWriteCH376Cmd(CMD01_GET_STATUS);
		xReadCH376Data(s);
		xEndCH376Cmd();
//		if((s == USB_INT_DISCONNECT) || (s == ERR_DISK_DISCON))
//		{
//			//操作过程中可能会把U盘拔除
//			//因此要通知上层
//			
//			

//			
//			if(ch376_up_irq)
//				ch376_up_irq(s);
//			
//			//这时候可能中断引脚已经被拉低
//			//要重新清除一次中断
//			xWriteCH376Cmd(CMD01_GET_STATUS);
//			xReadCH376Data(s);
//			xEndCH376Cmd();
//			
//			break;
//		}
//		else 
			
		if(s != CMD01_GET_STATUS)
			break;
		
		if(sc)
			sc --;
		else
			break;
		p_ch376->usb_delay_ms(1);
	}
	return (s);
}


//发出命令码后,等待中断 
static uint8_t	CH376SendCmdWaitInt( uint8_t mCmd )  
{
	uint8_t	itr = 0;
	p_ch376->usb_set_irq(0);
	
	xWriteCH376Cmd( mCmd );
	p_ch376->usb_delay_ms(1);
	xEndCH376Cmd();
	itr = Wait376Interrupt(0);
	p_ch376->usb_set_irq(1);
	return itr;
}

//??CH376??(INT#???)
static int Query376Interrupt(void)
{
	char	pin_val = 0;
//	ch376_int->read(ch376_int, &pin_val, 1);
	pin_val = p_ch376->usb_read_intr_pin();
	if(pin_val)
	{
		return 0;
	}
	else
		return 1;
}
static uint8_t Wait376Interrupt(int	set_irq )  
{
	uint16_t	i = 10000;
	if(set_irq)
		p_ch376->usb_set_irq(0);
	while(1)
	{
		if (Query376Interrupt( ))
		{
			if(set_irq)
				p_ch376->usb_set_irq(1);
			return( CH376GetIntStatus( ) );	
		}
			
		p_ch376->usb_delay_ms(1);
		if(i)
			i --;
		else
			break;
	}
	if(set_irq)
		p_ch376->usb_set_irq(1);
	return( ERR_USB_UNKNOWN );
}

static  void Ch376_intr(void)
{
	
	uint8_t	s = CH376GetIntStatus();// 清除CH376中断
	if(ch376_up_irq)
		ch376_up_irq(s);
	
	
	  
}

// 向USB主机端点的发送缓冲区写入数据块 
static void	CH376WriteHostBlock( uint8_t *buf, uint8_t len )
{
	xWriteCH376Cmd( CMD10_WR_HOST_DATA );
	xWriteCH376Data( len );  // 长度 
	if ( len )
	{
		xWriteCH376Data_p(buf, len);
//		do
//		{
//			xWriteCH376Data( *buf );
//			//p_ch376->usb_delay_ms(2);
//			buf ++;
//		} while ( -- len );
	}
	xEndCH376Cmd( );
}

static void xWriteCH376Data_u32(uint32_t data_u32)
{
	uint8_t u32_part;
	u32_part = data_u32;
	xWriteCH376Data(u32_part);
	u32_part = data_u32 >> 8;
	xWriteCH376Data(u32_part);
	u32_part = data_u32 >> 16;
	xWriteCH376Data(u32_part);
	u32_part = data_u32 >> 24;
	xWriteCH376Data(u32_part);
}

static void	CH376WriteVar32(uint8_t var, uint32_t dat )  /* 写CH376芯片内部的32位变量 */
{
	
	xWriteCH376Cmd( CMD20_WR_OFS_DATA );
	xWriteCH376Data( var );
	xWriteCH376Data_u32(dat);
	
	xEndCH376Cmd( );
}

/*******************************************************************************
* 函  数  名      : CH376WriteOfsBlock
* 描      述      : 向内部缓冲区指定偏移地址写入数据块.
* 输      入      : PUINT8 buf：
*					指向发送数据缓冲区.
*					UINT8 ofs:
*					偏移地址.
*					UINT8 len:
*					数据长度.
* 返      回      : 无.
*******************************************************************************/
static void	CH376WriteOfsBlock(uint8_t * buf, uint8_t ofs, uint8_t len )
{
	xWriteCH376Cmd( CMD20_WR_OFS_DATA );
	xWriteCH376Data( ofs );  															/* 偏移地址 */
	xWriteCH376Data( len );  															/* 数据长度 */
	xWriteCH376Data_p(buf, len );
//	if ( len ) 
//	{
//		do 
//		{
//			xWriteCH376Data( *buf );
//			buf ++;
//		} while ( -- len );
//	}
	xEndCH376Cmd( );
}


//检查usb电流过载
//电流过载时，USBOCA被拉低，则应该立即将USBENA拉成低电平
//void Test_USBOCA(void)
//{
//	if(!(IO0PIN & USB_OCA))
//	{
//		SET_CH376ENA_LOW;
//		p_ch376->usb_delay_ms(100);
//	}
//}






































static uint8_t	CH376ReadVar8( uint8_t var )  /* 读CH376芯片内部的8位变量 */
{
	uint8_t	c0;

	xWriteCH376Cmd( CMD11_READ_VAR8 );
	xWriteCH376Data( var );
	xReadCH376Data(c0);
	xEndCH376Cmd( );
	return( c0 );
}
// 检查USB存储器错误 
static uint8_t	CH376DiskReqSense( void )  
{
	uint8_t	s;
	p_ch376->usb_delay_ms( 50 );
	s = CH376SendCmdWaitInt( CMD0H_DISK_R_SENSE );
	p_ch376->usb_delay_ms( 50 );
	return( s );
}

////获取磁盘和文件系统的工作状态
//uint8_t	Ch376GetDiskStatus(void)
//{
//	return (CH376ReadVar8(VAR_DISK_STATUS));
//}
















// 设置将要操作的文件的文件名 
static void	CH376SetFileName(char *name )
{
	uint8_t	c;
	
	xWriteCH376Cmd( CMD10_SET_FILE_NAME );

	//只有0x43以上的版本才能这么干，目前用的是0x45版本的芯片
	c = *name;
	xWriteCH376Data( c );
	while ( c )
	{
		p_ch376->usb_delay_ms(5);
		name ++;
		c = *name;
		if ( c == DEF_SEPAR_CHAR1 || c == DEF_SEPAR_CHAR2 ) c = 0;  // 强行将文件名截止 
		xWriteCH376Data( c );
	}
	xEndCH376Cmd( );
}





// 从CH376芯片读取32位的数据并结束命令
static uint32_t	CH376Read32bitDat( void )  
{
	uint8_t	c0, c1, c2, c3;
	uint32_t	res = 0;

	xReadCH376Data(c0 );
	xReadCH376Data(c1 );
	xReadCH376Data(c2 );
	xReadCH376Data(c3 );
	xEndCH376Cmd( );

	res = c0 | (uint16_t)c1 << 8 | (uint32_t)c2 << 16 | (uint32_t)c3 << 24 ;
	return( res );
}



// 向内部指定缓冲区写入请求的数据块,返回长度 
static  uint8_t	CH376WriteReqBlock( uint8_t *buf )
{
	uint8_t	s, l;

	xWriteCH376Cmd( CMD01_WR_REQ_DATA );
	xReadCH376Data(l);  // 长度
	s = xWriteCH376Data_p(buf, l);
	
	xEndCH376Cmd( );
	return( s );
}









// 发出命令码和一字节数据后,等待中断 
static uint8_t	CH376SendCmdDatWaitInt( uint8_t mCmd, uint8_t mDat )  
{
	
	uint8_t	itr = 0;

	p_ch376->usb_set_irq(0);
	xWriteCH376Cmd( mCmd );
	xWriteCH376Data( mDat );
	xEndCH376Cmd( );
	itr = Wait376Interrupt(0);
	p_ch376->usb_set_irq(1);
	return itr;
}






