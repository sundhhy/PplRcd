//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#ifndef __INC_usb_H__
#define __INC_usb_H__

#include "Ch376.h"

//------------------------------------------------------------------------------
// check for correct compilation options
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

#define USB_MAX_WRITE_BYTE			DEF_SECTOR_SIZE

//USB设备错误信息定义
// #define	NOSUPPORTDEV		0	//设备不存在
// #define	SUPPORTDEV			1	//设备已插入
// #define	FILESYSTEMERR		2	//文件系统错误
// #define	FILECOPYCOMPLETE	3	//文件拷贝完成
// #define	FILECOPYERR			4	//文件拷贝错误
// #define	USBWRITEPROTECT		(5)	//U盘写保护

#define	NOSUPPORTDEV		0	//设备不存在
#define	SUPPORTDEV			1	//设备已插入
#define U_HAVEREADY			(2)	//U盘已就绪
#define U_NOHAVEREADY		(3)	//U盘未就绪
#define U_GETINFO			(4)	//U盘已获取产品信息
#define U_NOGETINFO			(5)	//U盘未获取产品信息
#define U_NOPROTECT			(6)	//U盘未写保护
#define	U_PROTECT			(7)	//U盘写保护
#define	U_QUREY				(8)	//U盘获取可用空间
#define	U_NOQUREY			(9)	//U盘未获取可用空间
#define	U_CREAT				(10)//U盘新建文件成功
#define	U_NOCREAT			(11)//U盘新建文件失败
#define	U_WRITE				(12)//U盘写文件成功
#define	U_NOWRITE			(13)//U盘写文件失败
#define	U_CLOSE				(14)//U盘关闭文件成功
#define	U_NOCLOSE			(15)//U盘关闭文件失败

#define	U_PROTECTERROR		(16)//U盘写保护查询出错





#define USB_NOOPER			(0)		//U盘未操作
#define USB_GETINFO			(1)		//U盘厂商信息
#define USB_CHECKPROTECT	(2)		//检测是否写保护
#define USB_QUREYINFO		(3)		//获取U盘可用空间
#define USB_CREATFILE		(4)		//创建文件
#define USB_WRITEFILE		(5)		//写文件
#define USB_CLOSEFILE		(6)		//关闭文件


#define USB_FM_READ		1
#define USB_FM_WRITE	2
#define USB_FM_COVER	4		//创建文件时，如果设置该位则会覆盖已有文件

#define USB_MSG_INDEX(msg)		(msg - 1)		//将e_usb_msg 转化成数组的索引
//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------
typedef int (*usb_event_hdl)(int	ety);

typedef struct {
	
	char		is_exist;
	char		none[3];
}USB_file_info;

//消息是给usb自己消费的
//事件是给外部模块使用的
typedef enum {
	//硬件消息
	hd_storage_insert	= 1,
	hd_storage_remove,
	hd_usb_comm_fail,
	//软件消息
	sf_identify_device,		
	usb_msg_max,
	//事件，是发给外部模块的
	et_unkow_device,
	et_protect,
	et_remove,
	et_ready,
}e_usb_msg;



//------------------------------------------------------------------------------
// global variable declarations
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
int USB_Init(void *hard_op);
int USB_Run(char search);
int USB_Open_file(char *file_name, char mode);
int USB_Colse_file(int fd);
int USB_Write_file(int fd, char *buf, int len);
int USB_Create_file(char *file_name, char mode);
int	USB_Rgt_event_hdl(usb_event_hdl hdl);
void USB_Del_event_hdl(int hdl_id);

//以下接口未实现
int USB_Get_file_info(char *file_name, USB_file_info *finfo);
int USB_Get_file_info_f(int fd, USB_file_info *finfo);
int USB_Delete_file(char *file_name, char mode);

int USB_Read_file(int fd, char *buf, int len);
int USB_flush_file(int fd);
void	USB_Power_off(void);
int	USB_Remove_disk(void );
	



#endif





//#ifndef __USB_H__
//#define __USB_H__

//#include "config.h"
////#include "ConfigInfo.h"
////#include "Func.h"
//#include "Ch376.h"
//#include "Ch376INC.h"



//#define YEAR_POSITION     		9 
//#define MONTH_POSITION    		5 
//#define DATA_POSITION     		0 
//#define HOUR_POSITION     		(11 + 16)
//#define MINUTE_POSITION  		(5 + 16) 
//#define SECOND_POSITION  		(0 + 16)










////参数地址定义
//#define	SYSPARA_USB_ADDR		1600
//#define	OUTPARA_USB_ADDR		(SYSPARA_USB_ADDR+44)
//#define	GROUP_USB_ADDR			(OUTPARA_USB_ADDR+16)
//#define	CONFIGPARA_USB_ADDR		(GROUP_USB_ADDR+72)	


////软件版本号在Usb文件传输中的地址
//#define EDIDION_ADDR	(0x3FF0)


//#define SECTORNUM	(32)	//每次写入U盘的扇区数：16*1024/512




//extern 	uint8	UsbExist;
//extern  uint16	Buff_Free;	//U 盘可用空间
//extern	uint8	g_IsDiskWriteProtect;
//extern	uint8  UsbReadyFlag;



//extern	void	InsertUdisk(void);
//extern	void	RemoveUdisk(void);

//extern	void	DealWithUsb(void);
//extern	void DetectUsb(void);


//#endif


