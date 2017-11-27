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
#include "Ch376.h"
#include "os/os_depend.h"
#include "device.h"
#include "sdhDef.h"

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

static	I_dev_Char	*ch376_dev;


//#define	xReadCH376Status( )		( CH376_CMD_PORT )  // 从CH376读状态
#define xWriteCH376Data(d)		(ch376_dev->write(ch376_dev, &d, 1))
#define xReadCH376Data(d)		(ch376_dev->read(ch376_dev, &d, 1))
//#define	xEndCH376Cmd( )			//结束CH376命令,仅用于SPI接口方式
#define CH376_CMD_PORT	(ch376_dev->write(ch376_dev, &d, 1))
//#define CH376_DATA_PORT	(*((volatile unsigned char *) 0x82000000))
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------

uint8_t DataBuff[64];
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------

//static void delay_ms(unsigned long time);
//static void Delay10us(unsigned long time);

static void xWriteCH376Cmd(uint8_t mCmd);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
int	Init_Ch386(int dev_id)
{
	int ret = RET_OK;
	ret = Dev_open(dev_id, (void *)&ch376_dev);
	
	Power_Ch376(1);
	HRst_Ch376();

    SET_CH376ENA_LOW;
	mInitCH376Host();
	SET_CH376ENA_HIGH;
	Power_Ch376(0);
	return ret;
	
}

void Power_Ch376(int on)
{
	if(on) 
	{
		SET_CH376PWR_LOW;
	}
	else 
	{
		
		
		SET_CH376PWR_HIGH;
	}
	
}

void HRst_Ch376(void)
{
	
	SET_CH376RST_HIGH;
	delay_ms(100);
	
	SET_CH376RST_LOW;
	delay_ms(100);

	SET_CH376ENA_HIGH;
//	delay_ms(100);

}

//初始化CH376
uint8_t mInitCH376Host(void)
{
	uint8_t res = 0;
	uint8_t	usb_data;

	//检测通讯接口
	xWriteCH376Cmd(CMD11_CHECK_EXIST);
	delay_ms(100);
	usb_data = 0x65;
	xWriteCH376Data(usb_data);
	delay_ms(100);
	xReadCH376Data(res);
	if (res != 0x9A)
	{
		return (ERR_USB_UNKNOWN);
	}


	//获取芯片及固件版本
	xWriteCH376Cmd(CMD01_GET_IC_VER);
	delay_ms(1000);
	xReadCH376Data(res);
	if (res != 0x41)
	{
//		return (ERR_USB_UNKNOWN);
		res = 0x41;
	}


	//设置USB工作模式
	xWriteCH376Cmd(CMD11_SET_USB_MODE);
	usb_data = 0x06;
	xWriteCH376Data(usb_data);	//已启用的主机方式并且自动产生SOF包
	delay_ms(100);
	xReadCH376Data(res);
	if (res == CMD_RET_SUCCESS)
	{
		return USB_INT_SUCCESS;
	}
	else
	{
		return ERR_USB_UNKNOWN;
	}
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

	ch376_dev->write(ch376_dev, &mCmd, 1);
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



//time = 1，1ms的延时
//time = 10，4.2ms的延时
//time = 20，8.2ms的延时
//static void delay_ms(unsigned long time)
//{
//	unsigned long i,j;
//	
//	for(i=0; i<2800; i++)
//		for(j=0; j<time; j++);
//}	


////time = 1，11us的延时
//static void Delay10us(unsigned long time)
//{
//	unsigned long i,j;
//	
//	for(i=0; i<30; i++)
//		for(j=0; j<time; j++);
//}



//检查usb电流过载
//电流过载时，USBOCA被拉低，则应该立即将USBENA拉成低电平
//void Test_USBOCA(void)
//{
//	if(!(IO0PIN & USB_OCA))
//	{
//		SET_CH376ENA_LOW;
//		delay_ms(100);
//	}
//}














////查询CH376中断(INT#低电平)
//uint8_t Query376Interrupt(void)
//{
//	//从命令端口读取接口状态，位7：中断标志，低有效
//	//return (xReadCH376Status() & PARA_STATE_INTB ? FALSE : TRUE ); 
//	//if ((IO0PIN >> 20) & 0x01 == 0)
//	if(IO0PIN & 0x00100000)
//	{
//		return FALSE;
//	}
//	else
//		return TRUE;
//}


//// 获取中断状态并取消中断请求 

//uint8_t CH376GetIntStatus(void)
//{
//	uint8_t s;

//	xWriteCH376Cmd(CMD01_GET_STATUS);
//	s = xReadCH376Data();
//	xEndCH376Cmd();
//	return (s);
//}


//uint8_t Wait376Interrupt( void )  
//{
//	uint32	i;
//	
//	for ( i = 0; i < 1000000; i ++ )
//	//while(1)
//	{
//		if ( Query376Interrupt( ) )
//			return( CH376GetIntStatus( ) );

//	}
//	return( ERR_USB_UNKNOWN );
//}


////发出命令码后,等待中断 
//uint8_t	CH376SendCmdWaitInt( uint8_t mCmd )  
//{
//	xWriteCH376Cmd( mCmd );
//	xEndCH376Cmd( );
//	return( Wait376Interrupt( ) );
//}

//// 检查U盘是否连接,不支持SD卡 
//uint8_t Ch376DiskConnect(void)
//{
//	
//	if ( Query376Interrupt( ) )
//		CH376GetIntStatus( );  // 检测到中断 
//	return( CH376SendCmdWaitInt( CMD0H_DISK_CONNECT ) );
//	
//}


//// 初始化磁盘并测试磁盘是否就绪
//uint8_t CH376DiskMount( void ) 
//{
//	return( CH376SendCmdWaitInt( CMD0H_DISK_MOUNT ) );
//}



//// 从当前主机端点的接收缓冲区读取数据块,返回长度 
//uint8_t	CH376ReadBlock( uint8_t *buf )
//{
//	uint8_t	s, l;

//	xWriteCH376Cmd( CMD01_RD_USB_DATA0 );
//	s = l = xReadCH376Data( );  /* 长度 */
//	if ( l ) {
//		do {
//			*buf = xReadCH376Data( );
//			delay_ms(5);
//			buf ++;
//		} while ( -- l );
//	}
//	xEndCH376Cmd( );
//	return( s );
//}





//uint8_t	CH376ReadVar8( uint8_t var )  /* 读CH376芯片内部的8位变量 */
//{
//	uint8_t	c0;

//	xWriteCH376Cmd( CMD11_READ_VAR8 );
//	xWriteCH376Data( var );
//	c0 = xReadCH376Data( );
//	xEndCH376Cmd( );
//	return( c0 );
//}


////获取磁盘和文件系统的工作状态
//uint8_t	Ch376GetDiskStatus(void)
//{
//	return (CH376ReadVar8(VAR_DISK_STATUS));
//}


//// 向USB主机端点的发送缓冲区写入数据块 
//void	CH376WriteHostBlock( uint8_t *buf, uint8_t len )
//{
//	xWriteCH376Cmd( CMD10_WR_HOST_DATA );
//	xWriteCH376Data( len );  // 长度 
//	if ( len )
//	{
//		do
//		{
//			xWriteCH376Data( *buf );
//			//delay_ms(2);
//			buf ++;
//		} while ( -- len );
//	}
//	xEndCH376Cmd( );
//}


//// 检查USB存储器错误 
//uint8_t	CH376DiskReqSense( void )  
//{
//	uint8_t	s;
//	delay_ms( 50 );
//	s = CH376SendCmdWaitInt( CMD0H_DISK_R_SENSE );
//	delay_ms( 50 );
//	return( s );
//}


//// 检查U盘是否写保护, 返回USB_INT_SUCCESS说明可以写,返回0xFF说明只读/写保护,返回其它值说明是错误代码 
//uint8_t	IsDiskWriteProtect( void )
//{
//	uint8_t	s, SysBaseInfo;
//	P_BULK_ONLY_CBW	pCbw;

//	SysBaseInfo = CH376ReadVar8( VAR_SYS_BASE_INFO );  // 当前系统的基本信息 
//	pCbw = (P_BULK_ONLY_CBW)DataBuff;
//	for ( s = 0; s != sizeof( pCbw -> CBW_CB_Buf ); s ++ )
//		pCbw -> CBW_CB_Buf[ s ] = 0;  // 默认清0 
//	pCbw -> CBW_DataLen0 = 0x10;  // 数据传输长度 
//	pCbw -> CBW_Flag = 0x80;  // 传输方向为输入 
//	if ( SysBaseInfo & 0x40 )
//	{  
//		// SubClass-Code子类别非6 
//		pCbw -> CBW_CB_Len = 10;  // 命令块的长度 
//		pCbw -> CBW_CB_Buf[0] = 0x5A;  // 命令块首字节, MODE SENSE(10) 
//		pCbw -> CBW_CB_Buf[2] = 0x3F;
//		pCbw -> CBW_CB_Buf[8] = 0x10;
//	}
//	else
//	{ 
//		// SubClass-Code子类别为6 
//		pCbw -> CBW_CB_Len = 6;  // 命令块的长度 
//		pCbw -> CBW_CB_Buf[0] = 0x1A;  // 命令块首字节, MODE SENSE(6) 
//		pCbw -> CBW_CB_Buf[2] = 0x3F;
//		pCbw -> CBW_CB_Buf[4] = 0x10;
//	}

//	// 向USB主机端点的发送缓冲区写入数据块,剩余部分CH376自动填补 
//	CH376WriteHostBlock( (uint8_t *)pCbw, sizeof( BULK_ONLY_CBW ) );
//	// 对U盘执行BulkOnly传输协议 
//	s = CH376SendCmdWaitInt( CMD0H_DISK_BOC_CMD );  
//	if ( s == USB_INT_SUCCESS )
//	{
//		// 从当前主机端点的接收缓冲区读取数据块,返回长度 
//		s = CH376ReadBlock( DataBuff );  
//		if ( s > 3 )
//		{
//			// MODE SENSE命令返回数据的长度有效 
//			if ( SysBaseInfo & 0x40 )
//				s = DataBuff[3];  // MODE SENSE(10), device specific parameter 
//			else
//				s = DataBuff[2];  // MODE SENSE(6), device specific parameter 
//			if ( s & 0x80 )
//				return( 0xFF );  // U盘写保护 
//			else
//				return( USB_INT_SUCCESS );  // U盘没有写保护 
//		}
//		return( USB_INT_DISK_ERR );
//	}

//	CH376DiskReqSense( );  // 检查USB存储器错误 
//	return( s );
//}



//// 查询磁盘剩余空间信息,扇区数 
//uint8_t	CH376DiskQuery( uint32 *DiskFre )  
//{
//	uint8_t	s,temp;
//	uint8_t	c0, c1, c2, c3;

//	s = CH376SendCmdWaitInt( CMD0H_DISK_QUERY );
//	if ( s == USB_INT_SUCCESS )
//	{
//		// 参考CH376INC.H文件中CH376_CMD_DATA结构的DiskQuery 
//		xWriteCH376Cmd( CMD01_RD_USB_DATA0 );
//		temp = xReadCH376Data( );  // 长度总是sizeof(CH376_CMD_DATA.DiskQuery) 
//		temp = xReadCH376Data( );  // CH376_CMD_DATA.DiskQuery.mTotalSector 
//		temp = xReadCH376Data( );
//		temp = xReadCH376Data( );
//		temp = xReadCH376Data( );

//		c0 = xReadCH376Data( );  // CH376_CMD_DATA.DiskQuery.mFreeSector 
//		c1 = xReadCH376Data( );
//		c2 = xReadCH376Data( );
//		c3 = xReadCH376Data( );
//		*DiskFre = c0 | (uint16)c1 << 8 | (uint32)c2 << 16 | (uint32)c3 << 24;

//		temp = xReadCH376Data( );  // CH376_CMD_DATA.DiskQuery.mDiskFat 
//		xEndCH376Cmd( );
//	}
//	else
//		*DiskFre = 0;
//	return( s );
//}



//// 设置将要操作的文件的文件名 
//void	CH376SetFileName( uint8_t *name )
//{
//	uint8_t	c;
//	
//	xWriteCH376Cmd( CMD10_SET_FILE_NAME );

//	c = *name;
//	xWriteCH376Data( c );
//	while ( c )
//	{
//		delay_ms(5);
//		name ++;
//		c = *name;
//		if ( c == DEF_SEPAR_CHAR1 || c == DEF_SEPAR_CHAR2 ) c = 0;  // 强行将文件名截止 
//		xWriteCH376Data( c );
//	}
//	xEndCH376Cmd( );
//}


//// 在根目录或者当前目录下新建文件,如果文件已经存在那么先删除 
//uint8_t	CH376FileCreate( uint8_t *name )
//{
//	// 设置将要操作的文件的文件名 
//	if ( name )
//		CH376SetFileName( name );  

//	return( CH376SendCmdWaitInt( CMD0H_FILE_CREATE ) );
//}


//// 从CH376芯片读取32位的数据并结束命令
//uint32	CH376Read32bitDat( void )  
//{
//	uint8_t	c0, c1, c2, c3;
//	uint32	res = 0;

//	c0 = xReadCH376Data( );
//	c1 = xReadCH376Data( );
//	c2 = xReadCH376Data( );
//	c3 = xReadCH376Data( );
//	xEndCH376Cmd( );

//	res = c0 | (uint16)c1 << 8 | (uint32)c2 << 16 | (uint32)c3 << 24 ;
//	return( res );
//}



//// 向内部指定缓冲区写入请求的数据块,返回长度 
//uint8_t	CH376WriteReqBlock( uint8_t *buf )
//{
//	uint8_t	s, l;

//	xWriteCH376Cmd( CMD01_WR_REQ_DATA );
//	s = l = xReadCH376Data( );  // 长度 
//	if ( l )
//	{
//		do
//		{
//			xWriteCH376Data( *buf );
//			buf ++;
//		} while ( -- l );
//	}
//	xEndCH376Cmd( );
//	return( s );
//}


////以字节为单位向当前位置写入数据块 
//uint8_t	CH376ByteWrite( uint8_t *buf, uint16 ReqCount, uint16 *RealCount )
//{
//	uint8_t	s;

//	xWriteCH376Cmd( CMD2H_BYTE_WRITE );
//	xWriteCH376Data( (uint8_t)ReqCount );
//	xWriteCH376Data( (uint8_t)(ReqCount>>8) );
//	xEndCH376Cmd( );

//	if ( RealCount )
//		*RealCount = 0;

//	while ( 1 )
//	{
//		s = Wait376Interrupt( );
//		if ( s == USB_INT_DISK_WRITE )
//		{
//			//向内部指定缓冲区写入请求的数据块,返回长度 
//			s = CH376WriteReqBlock( buf );
//			xWriteCH376Cmd( CMD0H_BYTE_WR_GO );
//			xEndCH376Cmd( );
//			buf += s;
//			if ( RealCount )
//				*RealCount += s;
//		}
//		else
//			return( s );  // 错误
//	}
//}




//// 将缓冲区中的多个扇区的数据块写入U盘,不支持SD卡 
//// baStart 是写入的线起始性扇区号, iSectorCount 是写入的扇区数 
//uint8_t	CH376DiskWriteSec( uint8_t *buf, uint32 iLbaStart, uint8_t iSectorCount )  
//{
//	uint8_t	s, err;
//	uint16	mBlockCount;

//	for ( err = 0; err != 3; ++ err )
//	{
//		// 出错重试 
//		xWriteCH376Cmd( CMD5H_DISK_WRITE );  // 向USB存储器写扇区 
//		xWriteCH376Data( (uint8_t)iLbaStart );  // LBA的最低8位 
//		xWriteCH376Data( (uint8_t)( (uint16)iLbaStart >> 8 ) );
//		xWriteCH376Data( (uint8_t)( iLbaStart >> 16 ) );
//		xWriteCH376Data( (uint8_t)( iLbaStart >> 24 ) );  // LBA的最高8位 
//		xWriteCH376Data( iSectorCount );  // 扇区数 
//		xEndCH376Cmd( );

//		mBlockCount = iSectorCount * DEF_SECTOR_SIZE / CH376_DAT_BLOCK_LEN;
//		for ( ; mBlockCount != 0; -- mBlockCount )
//		{ 
//			// 数据块计数 
//			s = Wait376Interrupt( );  // 等待中断并获取状态 ，应返回0x1E
//			if ( s == USB_INT_DISK_WRITE )
//			{
//				// USB存储器写数据块,请求数据写入 
//				CH376WriteHostBlock( buf, CH376_DAT_BLOCK_LEN );  // 向USB主机端点的发送缓冲区写入数据块 
//				xWriteCH376Cmd( CMD0H_DISK_WR_GO );  // 继续执行USB存储器的写操作 
//				xEndCH376Cmd( );
//				buf += CH376_DAT_BLOCK_LEN;
//			}
//			else
//				break;  // 返回错误状态 
//		}
//		if ( mBlockCount == 0 )
//		{
//			s = Wait376Interrupt( );  // 等待中断并获取状态 
//			if ( s == USB_INT_SUCCESS )
//				return( USB_INT_SUCCESS );  // 操作成功 
//		}
//		if ( s == USB_INT_DISCONNECT )
//			return( s );  // U盘被移除 
//		CH376DiskReqSense( );  // 检查USB存储器错误 
//	}
//	return( s );  // 操作失败 
//}



//// 以扇区为单位在当前位置写入数据块,不支持SD卡 
////数据缓冲区、要写入的扇区数、返回真正写入的数据个数
//uint8_t	CH376SecWrite( uint8_t *buf, uint8_t ReqCount, uint8_t *RealCount )  
//{
//	uint8_t	s;
//	uint8_t	cnt,temp;
//	uint32	StaSec;

//	if ( RealCount )
//		*RealCount = 0;

//	do
//	{
//		xWriteCH376Cmd( CMD1H_SEC_WRITE );
//		xWriteCH376Data( ReqCount );
//		xEndCH376Cmd( );

//		s = Wait376Interrupt( );
//		if ( s != USB_INT_SUCCESS )
//			return( s );
//		xWriteCH376Cmd( CMD01_RD_USB_DATA0 );
//		temp = xReadCH376Data( );  // 长度总是sizeof(CH376_CMD_DATA.SectorWrite) 

//		cnt = xReadCH376Data( );  // CH376_CMD_DATA.SectorWrite.mSectorCount 
//		temp = xReadCH376Data( );
//		temp = xReadCH376Data( );
//		temp = xReadCH376Data( );

//		// CH376_CMD_DATA.SectorWrite.mStartSector,从CH376芯片读取32位的数据并结束命令 
//		StaSec = CH376Read32bitDat( ); 
//		if ( cnt == 0 )
//			break;
//		s = CH376DiskWriteSec( buf, StaSec, cnt );  // 将缓冲区中的多个扇区的数据块写入U盘 
//		if ( s != USB_INT_SUCCESS )
//			return( s );
//		buf += cnt * DEF_SECTOR_SIZE;
//		if ( RealCount ) *RealCount += cnt;
//		ReqCount -= cnt;
//	} while ( ReqCount );

//	return( s );
//}


//// 发出命令码和一字节数据后,等待中断 
//uint8_t	CH376SendCmdDatWaitInt( uint8_t mCmd, uint8_t mDat )  
//{
//	xWriteCH376Cmd( mCmd );
//	xWriteCH376Data( mDat );
//	xEndCH376Cmd( );
//	return( Wait376Interrupt( ) );
//}



//// 关闭当前已经打开的文件或者目录(文件夹)
//uint8_t	CH376FileClose( uint8_t UpdateSz )
//{
//	return( CH376SendCmdDatWaitInt( CMD1H_FILE_CLOSE, UpdateSz ) );
//}


