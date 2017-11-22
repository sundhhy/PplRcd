
#include "Usb.h"


uint8	UsbExist;
uint16	Buff_Free;	//U 盘可用空间(MBytes)
uint32	Sectornum;	//总的空闲的扇区数，为了计算U盘可用空间
uint8	g_UsbBuff[16*1024];
uint8	g_IsDiskWriteProtect = 0;
uint8  UsbReadyFlag = 0;


/*
**************************************************************************
* 函数名称：UsbCBGetSysTime
* 输入参数：无
* 输出参数：当前时间
* 功能描述：取得系统时间
**************************************************************************
*/
uint32  UsbCBGetSysTime(void)
{
	uint32 year, month, data;
	uint32 hour, minute, second;
	uint32 value;
	
	/* this is sample codes, users should get actual time from the system */
	year = SysTime.year;		//年的范围为0~99
	year+=2000;
	month = SysTime.month;
	data =  SysTime.day;
	hour =  SysTime.hour;
	minute = SysTime.minute;
	second =  SysTime.second;
	
	value = hour << HOUR_POSITION |minute << MINUTE_POSITION | second <<SECOND_POSITION |
		(year-1980)<< YEAR_POSITION |month <<MONTH_POSITION |data << DATA_POSITION;
	return value;
}




/*
**************************************************************************
* 函数名称：Reset_Ch376
* 输入参数：无
* 输出参数：无
* 功能描述：复位ch376芯片
**************************************************************************
*/

//DelayMs(50) : 关中断与开中断之间需8.4 ms
//DelayMs(100) : 关中断与开中断之间需16 ms
void  Reset_Ch376(void)
{
	
	HRst_Ch376();
	mInitCH376Host();
}




/*
**************************************************************************
* 函数名称：InsertUdisk
* 输入参数：无
* 输出参数：无
* 功能描述：插入中断的钩子函数
**************************************************************************
*/
void InsertUdisk(void)
{
	UsbPara.UsbIrq = 1;
	g_IsDiskWriteProtect = 0;
	UsbExist = TRUE;

}



/*
*************************************************************************
 函数名称：RemoveUdisk
 输入参数：无
 输出参数：无
 功能描述：
*************************************************************************
*/
void RemoveUdisk(void)
{
	UsbExist = FALSE;
	UsbPara.ErrStatu = NOSUPPORTDEV;
	UsbPara.ProgressNum = 0;
	Buff_Free = 0;
	g_IsDiskWriteProtect = 0;
	g_PressCopy = 0;

	UsbPara.Step = 0;
	UsbPara.UsbIrq = 0;
	UsbPara.UsbStatu = 0;

	Reset_Ch376();
}



/*
*************************************************************************
 函数名称：ENTER_CRITICAL
 输入参数：无
 输出参数：无
 功能描述：关中断
*************************************************************************
*/
void ENTER_CRITICAL(void)
{
	unsigned short temp;
	
	__asm
	{
		mrs	temp,CPSR
		orr temp,temp,#0x80
		msr CPSR_c,temp
	}
}


/*
*************************************************************************
 函数名称：EXIT_CRITICAL
 输入参数：无
 输出参数：无
 功能描述：全局中断使能
*************************************************************************
*/
void EXIT_CRITICAL(void)
{
	unsigned short temp;
	
	__asm
	{
		mrs	temp,CPSR
		bic temp,temp,#0x80
		msr CPSR_c,temp
	}
}




/*
*************************************************************************
 函数名称：UsbCreatFileHead
 输入参数：无
 输出参数：无
 功能描述：建立USB文件信息段部分的数据
*************************************************************************
*/
void UsbCreatFileHead(void)
{
	uint16	i,j;
	uint8	*pt;
	Get_Batch_information(UsbPara.StarCh, UsbPara.EndCh, &UsbPara.StatTime, &UsbPara.EndTime);
	UsbPara.AddTimes=1;
	for(i=UsbPara.StarCh;i<=UsbPara.EndCh;i++)
			UsbPara.AddTimes+=BATCH_Achan_sta[i].Batch_blonum;
	UsbPara.ProgressNum=0;
	UsbPara.CurTimes=0;
	UsbPara.StarDptr=0;
	UsbPara.NowDptr=0;
	UsbPara.EndDptr=BATCH_Achan_sta[UsbPara.StarCh].Batch_blonum;
	UsbPara.Length=BATCH_Achan_sta[UsbPara.StarCh].Batch_blonum;
	UsbPara.NowCh=UsbPara.StarCh;
	for(i=0;i<0x4000;i++)	g_UsbBuff[i]=0;
	for(i=0;i<12;i++)
		g_UsbBuff[i]=UsbPara.FileName[i];	/*文件名*/
	pt=(uint8 *)&SysTime;
	for(i=0;i<6;i++)
		g_UsbBuff[12+i]=*pt++;			/*文件创建时间*/
	g_UsbBuff[18]=UsbPara.StarCh;			/*起始通道号*/
	g_UsbBuff[19]=UsbPara.EndCh;			/*结束通道号*/
	pt=(uint8 *)&UsbPara.StatTime;				/*用户设定的起始时间*/
	for(i=20;i<26;i++)
		g_UsbBuff[i]=*pt++;
	pt=(uint8 *)&UsbPara.EndTime;				/*用户设定的结束时间*/
	for(i=26;i<32;i++)
		g_UsbBuff[i]=*pt++;
	pt=(uint8 *)Sec_of_channo_para;
	for(i=UsbPara.StarCh;i<=UsbPara.EndCh;i++)/*各个通道所占用的记录块*/
	{
		for(j=0;j<96;j++)
			g_UsbBuff[32+i*98+j]=pt[i*96+j];/*各通道的参数信息*/
		g_UsbBuff[32+i*98+96]=(BATCH_Achan_sta[i].Batch_blonum>>8)&0xff;
		g_UsbBuff[32+i*98+97]=BATCH_Achan_sta[i].Batch_blonum&0xff;
	}
	pt=(uint8 *)&SysPara;
	for(i=SYSPARA_USB_ADDR;i<(SYSPARA_USB_ADDR+44);i++)
		g_UsbBuff[i]=*pt++;				/*系统参数*/
	pt=(uint8 *)&OutConfig;
	for(i=OUTPARA_USB_ADDR;i<(OUTPARA_USB_ADDR+16);i++)
		g_UsbBuff[i]=*pt++;				/*出厂组态参数*/
	pt=(uint8 *)&CurveGroup[0];
	for(i=GROUP_USB_ADDR;i<(GROUP_USB_ADDR+72);i++)
		g_UsbBuff[i]=*pt++;				/*分组信息*/
	pt=(uint8 *)&ConfigPara;			
	for(i=CONFIGPARA_USB_ADDR;i<(CONFIGPARA_USB_ADDR+SIZE_OF_CONFIGURATION);i++)
		g_UsbBuff[i]=*pt++;				/*各组态参数*/

	//软件版本号V2.1
	g_UsbBuff[EDIDION_ADDR] = 22;
}

//检测USB是否插入或拔出
void DetectUsb(void)
{
	int	g_status;

	g_status = Ch376DiskConnect();
	if (UsbExist == 0)
	{
		if (g_status == USB_INT_SUCCESS)
		{
			InsertUdisk();
			UsbPara.ErrStatu = SUPPORTDEV;
			
		}
		else
		{
			UsbPara.ErrStatu = NOSUPPORTDEV;
		}
	}
	else
	{
		if (g_status != USB_INT_SUCCESS)
		{
			RemoveUdisk();
		}
	}
}

void DealWithUsb(void)
{
	int	g_status;
	int	i;
	uint8	FileName[] = "\\DAT01001.DAT\\";


	if(UsbPara.UsbIrq && (UsbPara.InitWait>30))
	{
		UsbPara.UsbIrq = 0;
		UsbPara.InitWait = 0;
		for (i=0;i<3;i++)
		{
			DelayMs(100);
			g_status = CH376DiskMount();	//初始化U盘并检测U盘是否就绪
			if(g_status == USB_INT_SUCCESS)
			{
				UsbPara.Step=USB_CREATFILE;
				UsbPara.ErrStatu = U_HAVEREADY;
				return;
			}
			else if (g_status == ERR_DISK_DISCON)
			{
				//检测到U盘断开，重新检测
				UsbPara.UsbIrq = 1;
				UsbPara.InitWait = 30;
				return;
			}
			OpenWdt();
// 			g_status = Ch376GetDiskStatus();
// 			if ((g_status >= DEF_DISK_MOUNTED) && (i >= 5))
// 			{
// 				UsbPara.Step=USB_CREATFILE;
// 				UsbPara.ErrStatu = U_HAVEREADY;
// 				return;
// 			}
			
		}
		UsbPara.ErrStatu = U_NOHAVEREADY;
		UsbPara.Step=USB_NOOPER;
	}

	//1、2、3步骤暂时未处理，考虑到有些U盘不支持
	//4、5、6步骤只有在按下复制键以后才有效
	if(UsbPara.Step>0)
	{
		
		switch(UsbPara.Step)
		{
// 			case 1:
// 				//获取U盘的厂商和产品信息，返回数据个数
// 				g_status = CH376ReadBlock(Buff);
// 				if(g_status)
// 				{
// 					UsbExist=TRUE;
// 					UsbPara.UsbStatu=FALSE;
// 					UsbPara.Step=USB_CHECKPROTECT;
// 					UsbPara.ErrStatu = U_GETINFO;
// 				}
// 				else
// 				{
// 					UsbPara.ErrStatu = U_NOGETINFO;
// 					UsbPara.Step=USB_NOOPER;
// 				}
// 				break;
// 
// 			case 2:
// 				//检查U盘是否写保护
// 				for (i=0;i<3;i++)
// 				{
// 					g_status = IsDiskWriteProtect();
// 					if(g_status == USB_INT_SUCCESS)
// 					{
// 						UsbPara.Step=USB_QUREYINFO;
// 						UsbPara.fInOut=1;
// 						UsbPara.ErrStatu = U_NOPROTECT;
// 						return;
// 					}
// 					else if (g_status == 0xFF)	//说明被写保护了，应提示用户
// 					{
// 						g_IsDiskWriteProtect = 1;
// 						UsbPara.ErrStatu = U_PROTECT;
// 						return;
// 					}
// 				}
// 				UsbPara.ErrStatu = U_PROTECTERROR;
// 				break;
// 			
// 			case 3:
// 				//查询U盘可用空间
// 				for (i=0;i<3;i++)
// 				{
// 
// 					g_status = CH376DiskQuery(&Sectornum);
// 					if(g_status == USB_INT_SUCCESS)
// 					{
// 						if (Sectornum)
// 						{
// 							Buff_Free = Sectornum * DEF_SECTOR_SIZE / 1024 / 1024;
// 						}
// 						UsbPara.Step=USB_WRITEFILE;
// 						UsbPara.fInOut=1;
// 						UsbPara.ErrStatu = U_QUREY;
// 						return;
// 					}
// 				}
// 				UsbPara.ErrStatu = U_NOQUREY;
// 				break;

			
			case	4:
				if(g_PressCopy==1)
				{
					// 在根目录下新建文件并打开,如果文件已经存在则先删除后再新建 
					UsbCreatFileHead();
					memcpy(FileName+1,UsbPara.FileName,12);
					g_status = CH376FileCreate(FileName);
					if(g_status == USB_INT_SUCCESS)
					{
						UsbPara.Step = USB_WRITEFILE;
						UsbPara.ErrStatu = U_CREAT;
					}
					else
					{
						UsbPara.ErrStatu = U_NOCREAT;//文件读写错误
						UsbPara.UsbStatu=0;
						UsbPara.Step=USB_NOOPER;
					}
				}
				break;
			
			
			case	5:
				//写文件内容
				//if(UsbPara.UsbStatu==TRUE)
				{
					if(UsbPara.FristFlag == 1 )
					{
						g_status = CH376SecWrite( g_UsbBuff, SECTORNUM, NULL ); 
						if (g_status != USB_INT_SUCCESS)  			
						{
							UsbPara.ErrStatu = U_NOWRITE;//文件写错误
							UsbPara.UsbStatu=0;
							UsbPara.Step=USB_NOOPER;
							return;
						}
						UsbPara.FristFlag=0;
						UsbPara.UsbStatu = TRUE;
					}
					else
					{
						if(UsbPara.UsbStatu==TRUE)
						{
							Read_Data_Batch(UsbPara.NowCh, UsbPara.NowDptr++, g_UsbBuff);
							if(UsbPara.NowDptr >= UsbPara.EndDptr)
							{
								UsbPara.NowCh ++;
								if(UsbPara.NowCh >UsbPara.EndCh)
								{
									UsbPara.Step = USB_CLOSEFILE;//文件写完了
								}
								else
								{
									UsbPara.StarDptr = 0;
									UsbPara.NowDptr = 0;
									UsbPara.EndDptr = BATCH_Achan_sta[UsbPara.NowCh].Batch_blonum;
								}
							}
							g_status = CH376SecWrite( g_UsbBuff, SECTORNUM, NULL ); 
							if (g_status != USB_INT_SUCCESS)  			
							{ 
								UsbPara.ErrStatu = U_NOWRITE;//文件读写错误
								UsbPara.UsbStatu=0;
								UsbPara.Step=USB_NOOPER;
								return;
							}
						}
					}

					if(UsbPara.AddTimes > UsbPara.CurTimes)
					{
						UsbPara.CurTimes ++;
						UsbPara.ProgressNum = UsbPara.CurTimes*200/UsbPara.AddTimes;
					}

				}
			
				break;
				
			
			case	USB_CLOSEFILE:
				//关闭文件
				g_status = CH376FileClose( TRUE );  // 关闭文件 
				if(g_status == USB_INT_SUCCESS)
				{
					UsbPara.ErrStatu = U_CLOSE;
					UsbPara.Step = USB_NOOPER;
				}
				else
				{
					UsbPara.ErrStatu = U_NOCLOSE;
					UsbPara.Step=USB_NOOPER;
				}
				UsbPara.UsbStatu=0;
				break;


			default:
				break;
		}
	}
}


