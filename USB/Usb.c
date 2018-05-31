//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//
//提供对U盘或SD卡（SD卡功能没有在这里实现）的文件的CRUD功能
//版本：V010 
//限制
//1 只支持在根目录下操作;
//2 只支持对一个文件的操作
//这来自ch376的限制:文件名长度不得超过11，主文件名不超过8，扩展名不超过3,必须是大写字母，数字

#include <string.h>
#include "Usb.h"
//#include "os/os_depend.h"
//#include "deviceId.h"
//#include "sdhDef.h"
#include "usb_queue.h"
//#include "model/ModelTime.h"
//#include "system.h"

#include "usb_hardware_interface.h"


//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define		ONLY_ROOT_PATH		1
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
#define NUM_EHDS					3
#define LEN_USB_CQBUF			16
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------
typedef struct tm Time;
							/* CMD_DiskQuery, 查询磁盘信息 */


typedef struct {
	

	
	CycQueus_t	usb_cq;
	
	//USB的事件通知回调管理
	usb_event_hdl	arr_event_hdl[3];
	
	//u盘信息
	uint16_t		buf_total_MB;
	uint16_t		buf_free_MB;
	uint8_t			fat	;		//1-FAT12,2-FAT16,3-FAT32 

	uint8_t			set_ehd;		//bit 为1说明该hdl可用
	uint8_t			cur_state;
	uint8_t			err_status;
	char				is_usb_exist;
	char				is_protect;
	uint8_t			is_file_changed;		//文件被修改
	uint8_t			none;
	
}usb_control_t;

typedef int	(*usb_deal_msg)(void);
//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------

static 	usb_control_t		usb_ctl;
static	uint8_t 			usb_cq_buf[LEN_USB_CQBUF];
static usb_op_t 		*p_usb;
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------

static  void Deal_status(int ch376_status);
static void  Reset_Ch376(void);

/**文件操作相关  */
static int	Last_name_offset(char *path ) ;
//static void UsbCreatFileHead(void);
//static int	Open_dir(char* path);
/******************/

/***** deal msg ***************************/
static int	Usb_deal_insert(void);
static int	Usb_deal_remove(void);
static int	Usb_deal_identify(void);
static int	Usb_deal_fail(void);
static usb_deal_msg			arr_deal_msg[USB_MSG_INDEX(usb_msg_max)] = {Usb_deal_insert, Usb_deal_remove,\
Usb_deal_fail, Usb_deal_identify};

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
int USB_Run(char search)
{
	int			ret = 0;
	uint8_t	msg = 0;
	uint8_t	i = 0;
	if(search)
		Search_u_disk();
	
	CQ_Read(&usb_ctl.usb_cq, &msg, 1);

	if((msg < usb_msg_max) &&(msg > 0))
	{
		arr_deal_msg[USB_MSG_INDEX(msg)]();
		
	} 
	else if((msg > 0))
	{
		//将事件报给外部
		for(i = 0; i < NUM_EHDS; i++)
		{
			if(usb_ctl.arr_event_hdl[i])
			{
				
				usb_ctl.arr_event_hdl[i](msg);
			}
			
		}
		
	}
	
	//处理周期性的一些任务
	
	
	
	return ret;
}


int USB_Init(void *hard_op)
{
	int	ret = -1;
	//usb管理器初始化
	usb_ctl.set_ehd = 0xff;
	CQ_Init(&usb_ctl.usb_cq, usb_cq_buf, LEN_USB_CQBUF);
	
	//ch376硬件初始化
	p_usb = (usb_op_t *)hard_op;
	
	p_usb->usb_power(1);
//	Power_Ch376(1);
	ret = Init_Ch376(hard_op, Deal_status);
	if( ret == USB_INT_SUCCESS)
		ret = 0;
	else 
	{
		
		Deal_status(ret);
		ret = -1;
	}
	return ret;
	
}
//成功返回1
int USB_Open_file(char *file_name, char mode)
{
//	int	ret = 0;
	uint8_t	s;
#if ONLY_ROOT_PATH == 1	
	char	*p_name;
	p_name = file_name + Last_name_offset(file_name);
	
	
	
	s = CH376FileOpen(p_name);
	
	if(s == USB_INT_SUCCESS)
	{
		return 1;
	}
	else 
	{
		
		Deal_status(s);
		return 0;
	}
	
#endif	
	
}
//0 成功， -1 失败
int USB_Colse_file(int fd)
{
	int	ret = -1;
	usb_file_tm cur_time;
	uint16_t	dtm_u16 = 0;
	uint8_t 	s = 0;
	
	p_usb->usb_get_time(&cur_time);
	if(usb_ctl.is_file_changed == 1)
	{
		usb_ctl.is_file_changed = 0; 
		
		cur_time.year  %= 100;
		dtm_u16 = MAKE_FILE_DATE(cur_time.year + 2000, cur_time.mon, cur_time.mday);
		CH376_Set_Data_Time(DTM_CHANGE_DATE, dtm_u16);
		dtm_u16 = MAKE_FILE_TIME(cur_time.hour, cur_time.min, cur_time.sec);
		CH376_Set_Data_Time(DTM_CHANGE_TIME, dtm_u16);


	}		
	
	dtm_u16 = MAKE_FILE_DATE(cur_time.year + 2000, cur_time.mon, cur_time.mday);
	CH376_Set_Data_Time(DTM_LASTACC_DATE, dtm_u16);

	s = CH376FileClose(1);
	if(s == USB_INT_SUCCESS)
		ret = 0;
	else 
	{
		
		Deal_status(s);
	}
	
	
	return ret;
}


int USB_Get_file_info(char *file_name, USB_file_info *finfo)
{
	int	ret = 0;
	
	
	return ret;
}

int USB_Get_file_info_f(int fd, USB_file_info *finfo)
{
	int	ret = 0;
	
	
	return ret;
}

//成功返回大于0的句柄
//-1 失败 -2 文件已经存在
int USB_Create_file(char *file_name, char mode)
{
	int				ret = -1;
	int				fd = 0;
	usb_file_tm cur_time;
	char			*p_name;
	uint16_t		dtm_u16 = 0;
	uint8_t	s;
	
	//文件已存在
	//如果未设置覆盖位就直接返回已存在错误
	//如果设置覆盖位，就直接进行创建
#if ONLY_ROOT_PATH == 1	
	p_name = file_name + Last_name_offset(file_name);

	fd = USB_Open_file(p_name, mode);
	if(fd)
	{
		if((mode & USB_FM_COVER) == 0)
		{
			USB_Colse_file(fd);
			ret = -2;
			goto exit;
		}

		
		
		
		
	}
	
	//文件以及存在就不必创建了，直接覆盖
	if(fd == 0)	
		s = CH376FileCreate(p_name);
	

	if((s == USB_INT_SUCCESS) || (fd))
	{
		p_usb->usb_get_time(&cur_time);
		
		cur_time.year %= 100;
		dtm_u16 = MAKE_FILE_DATE(cur_time.year + 2000, cur_time.mon, cur_time.mday);
		CH376_Set_Data_Time(DTM_CREATE_DATE, dtm_u16);
		CH376_Set_Data_Time(DTM_CHANGE_DATE, dtm_u16);
		dtm_u16 = MAKE_FILE_TIME(cur_time.hour, cur_time.min, cur_time.sec);
		CH376_Set_Data_Time(DTM_CREATE_TIME, dtm_u16);
		CH376_Set_Data_Time(DTM_CHANGE_TIME, dtm_u16);
		ret = 1;
		usb_ctl.is_file_changed = 0; 
	} 
	else 
	{
		
		Deal_status(s);
	}
	
#endif
	exit:
	return ret;
}

int USB_Delete_file(char *file_name, char mode)
{
	int	ret = 0;
	
	
	return ret;
}

int USB_Read_file(int fd, char *buf, int len)
{
	int	ret = 0;
	
	
	return ret;
}

int USB_Write_file(int fd, char *buf, int len)
{
	int	ret = 0;
	uint16_t	real_len = 0;

//	uint8_t		num_bkls = 0;
//	uint8_t		real_num_bkls = 0;
//	
//	num_bkls =len/ DEF_SECTOR_SIZE;
//	if(num_bkls)
//		CH376SecWrite((uint8_t *)buf, num_bkls, &real_num_bkls);
//	
//	len -= num_bkls * DEF_SECTOR_SIZE;
	if(len)
		CH376ByteWrite((uint8_t *)buf, len, &real_len);
	usb_ctl.is_file_changed = 1; 
//	p_usb->usb_delay_ms(1);
	return ret;
}

int USB_flush_file(int fd)
{
	int	ret = 0;
	
	
	return ret;
	
}

void	USB_Power_off(void)
{
	
	
}

int	USB_Remove_disk(void )
{
	int	ret = 0;
	
	
	return ret;
}

int	USB_Rgt_event_hdl(usb_event_hdl hdl)
{
	
	int	i = 0;
	
	for(i = 0; i < NUM_EHDS; i++)
	{
		if(usb_ctl.set_ehd & (1 << i))
		{
			usb_ctl.arr_event_hdl[i] = hdl;
			
			usb_ctl.set_ehd &= ~(1 << i);
			break;
		}
		
	}
	if(i == NUM_EHDS)
		i = -1;
	
	return i; 
}

void USB_Del_event_hdl(int hdl_id)
{
	
	usb_ctl.set_ehd |= (1 << hdl_id);
	usb_ctl.arr_event_hdl[hdl_id] = NULL;
}




//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{


static  void Deal_status(int ch376_status)
{
	//U盘插入的时候，会产生中断，但是SD卡插入是不会产生中断的
	
	uint8_t		msg = 0;
	switch(ch376_status)
	{
		case USB_INT_CONNECT:
			//检测到U盘连接上了，接下来应该查询U盘的基本信息
			msg = hd_storage_insert;
			break;
		case USB_INT_DISCONNECT:
		case ERR_DISK_DISCON:
			//检测到U盘断开
			msg = hd_storage_remove;
			
			break;
//		default:
//			//USB主机通信失败
//			msg = hd_usb_comm_fail;
//			break;
		
	}
	
	if(msg)
		CQ_Write(&usb_ctl.usb_cq, &msg, 1);
	

}



//从路径中分离出最后一级文件名或者目录（文件夹）名，返回其在路径字符串中的偏移

static int	Last_name_offset(char *path )  
{
	char	*pName;
	
	//找到倒数第一个\或/
	for ( pName = path; *pName != 0; ++ pName ); 
	while ( *pName != DEF_SEPAR_CHAR1 && *pName != DEF_SEPAR_CHAR2 && pName != path ) 
		pName --;  
	
	//若找到路径分隔符，跳过分隔符
	if (pName != path ) 
		pName ++; 
	return(pName - path );
}




//检测USB是否插入或拔出
//static void DetectUsb(void)
//{
//	int	g_status;

//	g_status = Ch376DiskConnect();
////	if (UsbExist == 0)
////	{
////		if (g_status == USB_INT_SUCCESS)
////		{
////			InsertUdisk();
////			UsbPara.ErrStatu = SUPPORTDEV;
////			
////		}
////		else
////		{
////			UsbPara.ErrStatu = NOSUPPORTDEV;
////		}
////	}
////	else
////	{
////		if (g_status != USB_INT_SUCCESS)
////		{
////			RemoveUdisk();
////		}
////	}
//}


///*
//**************************************************************************
//* 函数名称：InsertUdisk
//* 输入参数：无
//* 输出参数：无
//* 功能描述：插入中断的钩子函数
//**************************************************************************
//*/
//void InsertUdisk(void)
//{
//	UsbPara.UsbIrq = 1;
//	g_IsDiskWriteProtect = 0;
//	UsbExist = TRUE;

//}

/**************************************************************************
* 函数名称：Reset_Ch376
* 输入参数：无
* 输出参数：无
* 功能描述：复位ch376芯片
**************************************************************************
*/


static void  Reset_Ch376(void)
{
	p_usb->usb_reset();
	
//	HRst_Ch376();
	mInitCH376Host();
}

///*
//*************************************************************************
// 函数名称：RemoveUdisk
// 输入参数：无
// 输出参数：无
// 功能描述：
//*************************************************************************
//*/
//void RemoveUdisk(void)
//{
//	UsbExist = FALSE;
//	UsbPara.ErrStatu = NOSUPPORTDEV;
//	UsbPara.ProgressNum = 0;
//	Buff_Free = 0;
//	g_IsDiskWriteProtect = 0;
//	g_PressCopy = 0;

//	UsbPara.Step = 0;
//	UsbPara.UsbIrq = 0;
//	UsbPara.UsbStatu = 0;

//	Reset_Ch376();
//}


static int	Usb_deal_insert(void)
{
	int	ret = 0;
	uint8_t		s = 0;
	uint8_t		msg = 0;
	uint16_t		safe_count = 1000;
	uint8_t		usb_info[40] = {0};		//一般36个字节
	p_usb->usb_set_irq(0);
	while(1)
	{
		s = Ch376DiskConnect();
		//如果返回的状态是USB_INT_SUCCESS，说明U盘或者设备连接
		if(s == USB_INT_SUCCESS)
		{
			
			p_usb->usb_delay_ms(1);
			s = CH376DiskMount();
			if(s == USB_INT_SUCCESS)
			{
				usb_ctl.is_usb_exist = 1;
				
 				//获取U盘的厂商和产品信息，返回数据个数
				//有些U盘可能不支持，因此就不作为判断U盘好坏的依据了，读取数据只是用于调试使用
 				s = CH376ReadBlock(usb_info, 40);

				usb_ctl.cur_state = SUPPORTDEV;
				msg = sf_identify_device;
				CQ_Write(&usb_ctl.usb_cq, &msg, 1);
				break;
			}
			else 
			{
				safe_count --;
				
			}
			
		}
		else if(s != 0x22)
		{
			
			msg = hd_storage_remove;
			CQ_Write(&usb_ctl.usb_cq, &msg, 1);
			break;
		}
		
		if(safe_count == 0)
		{
			msg = et_unkow_device;
			CQ_Write(&usb_ctl.usb_cq, &msg, 1);
			break;
			
		}
		p_usb->usb_delay_ms(10);
	}
	p_usb->usb_set_irq(1);
	
	
	return ret;
	
	
}
static int	Usb_deal_remove(void)
{
	int	ret = 0;
	uint8_t	msg = 0;
	
	usb_ctl.cur_state = NOSUPPORTDEV;
	msg = et_remove;
	CQ_Write(&usb_ctl.usb_cq, &msg, 1);
	return ret;	
	
}

static int	Usb_deal_fail(void)
{
	
	Reset_Ch376();
	return 0;
}

static int	Usb_deal_identify(void)
{
	int					ret = 0;
	uint32_t			all_sectornum = 0;
	uint32_t			free_sectornum = 0;
	
	uint8_t			s;
	uint8_t			msg = 0;
	uint16_t		i;
	
	uint8_t			fat = 0;
	if(usb_ctl.cur_state == NOSUPPORTDEV)		//已经被拔除就算了
		return ret;
	
	p_usb->usb_set_irq(0);
	
	//检测U盘的写保护状态
	//这个功能好像也不一定能支持
	for (i=0;i<3;i++)
	{
		s = IsDiskWriteProtect();
		if(s == USB_INT_SUCCESS)
		{

			usb_ctl.is_protect = 0;

			break;
		}
		else if (s == 0xFF)	//说明被写保护了，应提示用户
		{
			usb_ctl.is_protect = 1;
			msg = et_protect;
			CQ_Write(&usb_ctl.usb_cq, &msg, 1);
			goto exit;
		}
	}
	if(i == 3)
		usb_ctl.err_status = U_PROTECTERROR;
	
	//查询U盘可用空间
	for (i=0;i<3;i++)
	{

		s = CH376DiskQuery(&all_sectornum, &free_sectornum, &fat);
		if(s == USB_INT_SUCCESS)
		{
			usb_ctl.buf_total_MB = all_sectornum * DEF_SECTOR_SIZE / 1024 / 1024;
			usb_ctl.buf_free_MB = free_sectornum * DEF_SECTOR_SIZE / 1024 / 1024;
			usb_ctl.fat = fat;
			break;
		}
	}
	
	if(i == 3)
	{
		usb_ctl.err_status = U_NOQUREY;
		usb_ctl.buf_free_MB = 0;
		
	}
	
	
exit:
	usb_ctl.cur_state = U_HAVEREADY;
	msg = et_ready;
	CQ_Write(&usb_ctl.usb_cq, &msg, 1);	

	p_usb->usb_set_irq(1);
	return ret;	
	
}


//#include "Usb.h"


//uint8_t	UsbExist;
//uint16_t	Buff_Free;	//U 盘可用空间(MBytes)
//uint32_t	Sectornum;	//总的空闲的扇区数，为了计算U盘可用空间
//uint8_t	g_UsbBuff[16*1024];
//uint8_t	g_IsDiskWriteProtect = 0;
//uint8_t  UsbReadyFlag = 0;


///*
//**************************************************************************
//* 函数名称：UsbCBGetSysTime
//* 输入参数：无
//* 输出参数：当前时间
//* 功能描述：取得系统时间
//**************************************************************************
//*/
//uint32_t  UsbCBGetSysTime(void)
//{
//	uint32_t year, month, data;
//	uint32_t hour, minute, second;
//	uint32_t value;
//	
//	/* this is sample codes, users should get actual time from the system */
//	year = SysTime.year;		//年的范围为0~99
//	year+=2000;
//	month = SysTime.month;
//	data =  SysTime.day;
//	hour =  SysTime.hour;
//	minute = SysTime.minute;
//	second =  SysTime.second;
//	
//	value = hour << HOUR_POSITION |minute << MINUTE_POSITION | second <<SECOND_POSITION |
//		(year-1980)<< YEAR_POSITION |month <<MONTH_POSITION |data << DATA_POSITION;
//	return value;
//}




///*

















/*
*************************************************************************
 函数名称：UsbCreatFileHead
 输入参数：无
 输出参数：无
 功能描述：建立USB文件信息段部分的数据
*************************************************************************
*/
//static void UsbCreatFileHead(void)
//{
//	uint16_t	i,j;
//	uint8_t	*pt;
//	UsbPara.AddTimes=1;
//	UsbPara.ProgressNum=0;
//	UsbPara.CurTimes=0;
//	UsbPara.StarDptr=0;
//	UsbPara.NowDptr=0;
//	UsbPara.EndDptr=BATCH_Achan_sta[UsbPara.StarCh].Batch_blonum;
//	UsbPara.Length=BATCH_Achan_sta[UsbPara.StarCh].Batch_blonum;
//	UsbPara.NowCh=UsbPara.StarCh;
//	for(i=0;i<0x4000;i++)	g_UsbBuff[i]=0;
//	for(i=0;i<12;i++)
//		g_UsbBuff[i]=UsbPara.FileName[i];	/*文件名*/
//	pt=(uint8_t *)&SysTime;
//	for(i=0;i<6;i++)
//		g_UsbBuff[12+i]=*pt++;			/*文件创建时间*/
//	g_UsbBuff[18]=UsbPara.StarCh;			/*起始通道号*/
//	g_UsbBuff[19]=UsbPara.EndCh;			/*结束通道号*/
//	pt=(uint8_t *)&UsbPara.StatTime;				/*用户设定的起始时间*/
//	for(i=20;i<26;i++)
//		g_UsbBuff[i]=*pt++;
//	pt=(uint8_t *)&UsbPara.EndTime;				/*用户设定的结束时间*/
//	for(i=26;i<32;i++)
//		g_UsbBuff[i]=*pt++;
//	pt=(uint8_t *)Sec_of_channo_para;
//	for(i=UsbPara.StarCh;i<=UsbPara.EndCh;i++)/*各个通道所占用的记录块*/
//	{
//		for(j=0;j<96;j++)
//			g_UsbBuff[32+i*98+j]=pt[i*96+j];/*各通道的参数信息*/
//		g_UsbBuff[32+i*98+96]=(BATCH_Achan_sta[i].Batch_blonum>>8)&0xff;
//		g_UsbBuff[32+i*98+97]=BATCH_Achan_sta[i].Batch_blonum&0xff;
//	}
//	pt=(uint8_t *)&SysPara;
//	for(i=SYSPARA_USB_ADDR;i<(SYSPARA_USB_ADDR+44);i++)
//		g_UsbBuff[i]=*pt++;				/*系统参数*/
//	pt=(uint8_t *)&OutConfig;
//	for(i=OUTPARA_USB_ADDR;i<(OUTPARA_USB_ADDR+16);i++)
//		g_UsbBuff[i]=*pt++;				/*出厂组态参数*/
//	pt=(uint8_t *)&CurveGroup[0];
//	for(i=GROUP_USB_ADDR;i<(GROUP_USB_ADDR+72);i++)
//		g_UsbBuff[i]=*pt++;				/*分组信息*/
//	pt=(uint8_t *)&ConfigPara;			
//	for(i=CONFIGPARA_USB_ADDR;i<(CONFIGPARA_USB_ADDR+SIZE_OF_CONFIGURATION);i++)
//		g_UsbBuff[i]=*pt++;				/*各组态参数*/

//	//软件版本号V2.1
//	g_UsbBuff[EDIDION_ADDR] = 22;
//}



//void DealWithUsb(void)
//{
//	int	g_status;
//	int	i;
//	uint8_t	FileName[] = "\\DAT01001.DAT\\";


//	if(UsbPara.UsbIrq && (UsbPara.InitWait>30))
//	{
//		UsbPara.UsbIrq = 0;
//		UsbPara.InitWait = 0;
//		for (i=0;i<3;i++)
//		{
//			DelayMs(100);
//			g_status = CH376DiskMount();	//初始化U盘并检测U盘是否就绪
//			if(g_status == USB_INT_SUCCESS)
//			{
//				UsbPara.Step=USB_CREATFILE;
//				UsbPara.ErrStatu = U_HAVEREADY;
//				return;
//			}
//			else if (g_status == ERR_DISK_DISCON)
//			{
//				//检测到U盘断开，重新检测
//				UsbPara.UsbIrq = 1;
//				UsbPara.InitWait = 30;
//				return;
//			}
//			OpenWdt();
//// 			g_status = Ch376GetDiskStatus();
//// 			if ((g_status >= DEF_DISK_MOUNTED) && (i >= 5))
//// 			{
//// 				UsbPara.Step=USB_CREATFILE;
//// 				UsbPara.ErrStatu = U_HAVEREADY;
//// 				return;
//// 			}
//			
//		}
//		UsbPara.ErrStatu = U_NOHAVEREADY;
//		UsbPara.Step=USB_NOOPER;
//	}

//	//1、2、3步骤暂时未处理，考虑到有些U盘不支持
//	//4、5、6步骤只有在按下复制键以后才有效
//	if(UsbPara.Step>0)
//	{
//		
//		switch(UsbPara.Step)
//		{
//// 			case 1:
//// 				//获取U盘的厂商和产品信息，返回数据个数
//// 				g_status = CH376ReadBlock(Buff);
//// 				if(g_status)
//// 				{
//// 					UsbExist=TRUE;
//// 					UsbPara.UsbStatu=FALSE;
//// 					UsbPara.Step=USB_CHECKPROTECT;
//// 					UsbPara.ErrStatu = U_GETINFO;
//// 				}
//// 				else
//// 				{
//// 					UsbPara.ErrStatu = U_NOGETINFO;
//// 					UsbPara.Step=USB_NOOPER;
//// 				}
//// 				break;
//// 
//// 			case 2:
//// 				//检查U盘是否写保护
//// 				for (i=0;i<3;i++)
//// 				{
//// 					g_status = IsDiskWriteProtect();
//// 					if(g_status == USB_INT_SUCCESS)
//// 					{
//// 						UsbPara.Step=USB_QUREYINFO;
//// 						UsbPara.fInOut=1;
//// 						UsbPara.ErrStatu = U_NOPROTECT;
//// 						return;
//// 					}
//// 					else if (g_status == 0xFF)	//说明被写保护了，应提示用户
//// 					{
//// 						g_IsDiskWriteProtect = 1;
//// 						UsbPara.ErrStatu = U_PROTECT;
//// 						return;
//// 					}
//// 				}
//// 				UsbPara.ErrStatu = U_PROTECTERROR;
//// 				break;
//// 			
//// 			case 3:
//// 				//查询U盘可用空间
//// 				for (i=0;i<3;i++)
//// 				{
//// 
//// 					g_status = CH376DiskQuery(&Sectornum);
//// 					if(g_status == USB_INT_SUCCESS)
//// 					{
//// 						if (Sectornum)
//// 						{
//// 							Buff_Free = Sectornum * DEF_SECTOR_SIZE / 1024 / 1024;
//// 						}
//// 						UsbPara.Step=USB_WRITEFILE;
//// 						UsbPara.fInOut=1;
//// 						UsbPara.ErrStatu = U_QUREY;
//// 						return;
//// 					}
//// 				}
//// 				UsbPara.ErrStatu = U_NOQUREY;
//// 				break;

//			
//			case	4:
//				if(g_PressCopy==1)
//				{
//					// 在根目录下新建文件并打开,如果文件已经存在则先删除后再新建 
//					UsbCreatFileHead();
//					memcpy(FileName+1,UsbPara.FileName,12);
//					g_status = CH376FileCreate(FileName);
//					if(g_status == USB_INT_SUCCESS)
//					{
//						UsbPara.Step = USB_WRITEFILE;
//						UsbPara.ErrStatu = U_CREAT;
//					}
//					else
//					{
//						UsbPara.ErrStatu = U_NOCREAT;//文件读写错误
//						UsbPara.UsbStatu=0;
//						UsbPara.Step=USB_NOOPER;
//					}
//				}
//				break;
//			
//			
//			case	5:
//				//写文件内容
//				//if(UsbPara.UsbStatu==TRUE)
//				{
//					if(UsbPara.FristFlag == 1 )
//					{
//						g_status = CH376SecWrite( g_UsbBuff, SECTORNUM, NULL ); 
//						if (g_status != USB_INT_SUCCESS)  			
//						{
//							UsbPara.ErrStatu = U_NOWRITE;//文件写错误
//							UsbPara.UsbStatu=0;
//							UsbPara.Step=USB_NOOPER;
//							return;
//						}
//						UsbPara.FristFlag=0;
//						UsbPara.UsbStatu = TRUE;
//					}
//					else
//					{
//						if(UsbPara.UsbStatu==TRUE)
//						{
//							Read_Data_Batch(UsbPara.NowCh, UsbPara.NowDptr++, g_UsbBuff);
//							if(UsbPara.NowDptr >= UsbPara.EndDptr)
//							{
//								UsbPara.NowCh ++;
//								if(UsbPara.NowCh >UsbPara.EndCh)
//								{
//									UsbPara.Step = USB_CLOSEFILE;//文件写完了
//								}
//								else
//								{
//									UsbPara.StarDptr = 0;
//									UsbPara.NowDptr = 0;
//									UsbPara.EndDptr = BATCH_Achan_sta[UsbPara.NowCh].Batch_blonum;
//								}
//							}
//							g_status = CH376SecWrite( g_UsbBuff, SECTORNUM, NULL ); 
//							if (g_status != USB_INT_SUCCESS)  			
//							{ 
//								UsbPara.ErrStatu = U_NOWRITE;//文件读写错误
//								UsbPara.UsbStatu=0;
//								UsbPara.Step=USB_NOOPER;
//								return;
//							}
//						}
//					}

//					if(UsbPara.AddTimes > UsbPara.CurTimes)
//					{
//						UsbPara.CurTimes ++;
//						UsbPara.ProgressNum = UsbPara.CurTimes*200/UsbPara.AddTimes;
//					}

//				}
//			
//				break;
//				
//			
//			case	USB_CLOSEFILE:
//				//关闭文件
//				g_status = CH376FileClose( TRUE );  // 关闭文件 
//				if(g_status == USB_INT_SUCCESS)
//				{
//					UsbPara.ErrStatu = U_CLOSE;
//					UsbPara.Step = USB_NOOPER;
//				}
//				else
//				{
//					UsbPara.ErrStatu = U_NOCLOSE;
//					UsbPara.Step=USB_NOOPER;
//				}
//				UsbPara.UsbStatu=0;
//				break;


//			default:
//				break;
//		}
//	}
//}


