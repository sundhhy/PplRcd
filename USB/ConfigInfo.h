/*
*********************************************************************************************************
*                                               				SunyVPR130-RG
* 版	     本:V1.0									
* 修改日期:2007.1.19
* 文           件: ConfigInfo.h
* 功           能: 配置信息
* 编           写: fj
********************************************************************************************************
*/
#ifndef	CONFIGINFO_H
#define	CONFIGINFO_H
/*
**************************************************************************************************
*										头文件包含
**************************************************************************************************
*/
#include "RDB.h"
#include "Func.h"
/*
**************************************************************************************************
*										常量定义
**************************************************************************************************
*/

#define	RDBBASEADDR		(CONFIGURATIONADDR)		/*铁电中存放参数信息的起始地址*/
#define	SIZE_OF_SYSPARA	(sizeof(_SysParameter))
#define	SIZE_OF_GROUP		(sizeof(_CurveGroup)*4)
#define	SIZE_OF_ALARMLINEINFO		(sizeof(_AlarmLine_Info)*4)
/*
**************************************************************************************************
*										类型定义
**************************************************************************************************
*/
/*系统信息定义,在FRAM中为系统参数预留了44字节的空间,该结构大小不能操作44字节*/
typedef struct 
{
	uint8	SysRom;		/*仪表FLASH容量*/
	uint8	SysAlm;			/*报警触点数量*/
	uint8 	SysNum;		/*仪表物理通道数*/
	uint8	SysAddr;		/*仪表通讯地址*/
	uint8	CommBPS;		/*通讯波特率*/
	uint8	ComType;		/*通讯方式*/
	uint8	Parity;			/*校验方式*/
	uint8 	Tcode[3];		/*用户密码*/	
	uint8 	SysTD;			/*用户设定的通道数*/
	uint8 	SysDBD;			/*用户设定的对比度*/
	uint8	ScaleType;		/*记录间隔*/
	uint8	RecordCycle;
	uint8	Cool;			/*冷端温度*/
	uint8	BreakTC;		/*断偶处理方式*/
	uint8	BreakRTD;		/*断阻处理方式*/
	uint8	USBFLAG;		/*是否有USB 功能*/
	uint8	PrintFLAG;		/*是否有打印功能*/
	uint8	SumFLAG;		/*是否累积功能*/
	uint8	PIDFLAG;		/*是否有PID调节功能*/
	uint8	PID_SET_Enable;/*是否允许调节参数修改*/
	uint8       CoolMode;           /*冷端补偿方式0:外部  1:设定*/
	uint8       SoundEnable;      /*按键声音0:禁止  1:允许*/
	uint8       GroupSwitchCycle;  /*各曲线组自动切换时间*/
	uint8       ComStatusDisEnable;/*通讯状态显示允许*/
	uint8       Ocode[3];            /*出厂组态密码*/
	uint8       reverseee;
	uint16	SysCRC;			/*校验码*/

}_SysParameter;
typedef struct {
	uint8 	UsbStatu;		/*状态 0:停止 1:正在传送*/
	uint8 	StarCh;			/*起始通道号*/
	uint8 	EndCh;			/*终止通道号*/
	uint8 	NowCh;			/*当前通道号*/
	uint32 	StarDptr;		/*当前通道传送数据起始指针*/
	uint32 	EndDptr;		/*当前通道传送数据终止指针*/
	uint32 	NowDptr;		/*当前指针*/
	uint16 	ProgressNum;	/*进度条状态0-200*/
	Time	StatTime;		/*起始时间*/
	Time	EndTime;		/*终止时间*/
	uint32 	AddTimes;		/*总共许需要的传送次数*/
	uint32 	CurTimes;		/*当前的传送次数*/
	uint8 	FristFlag;		/*第一次传送标志*/
	uint8 	UsbIrq;			/*usb中断标志*/
	uint8 	Finish;			/*完成的百分数*/
	uint8 	ErrStatu;    		/*usb错误状态*/
	uint32     Length;			/*通道总容量*/
	uint8 	InitWait;		/*USB初始化延时*/
	uint8 	SecondFlag;
	uint8  	fInOut ;
	uint8  	Step;
	uint32  	TotalLength;
	uint8	FileName[12];	
}_UsbParameter;
typedef struct
{
	Time  	StartTime;             /*起始打印时间*/
	Time  	EndTime;               /*终止打印时间*/
	Time	NowTime;              /*当前打印时间*/
	Time	PrintTime;             /*打印时间*/
	uint32     StartDptr;             /*当前通道传送数据的起始指针*/
	uint32     EndDptr;                /*当前通道传送数据终止指针*/
	uint32     NowDptr;              /*当前指针*/
	uint8       PrintStatus;         /*0:停止  1:正在打印  2:要求停止 3:要求打印// 4:不打印*/								
	uint8       PrintType;	        /*0:数据  1:图形   2：报警*/
	uint8       CurveFlag;	        /*0~2:第一次画曲线  3:非第一次画曲线*/
	uint8       PrintTimeNum;	/*打印时间的第几位  1~8   0 : 停止*/
	uint16	LastValue;
	uint8       Printdata[6];
	uint8       PrintCycle;              /*打印间隔*/
	uint8       PrintProgressNum;   /*打印进度*/
	uint8       PrintTagid;              /*打印通道*/
}_PrintParameter;

 typedef struct	
{
	 _RdbFile_Head 			RdbFile_Head;
	 _RdbBase_Record			RdbBase_Record[RDB_BASE_MAXNUM];
	 _RdbSample_Record 		RdbSample_Record[RDB_SAMPLE_MAXNUM];
	 _RdbRecord_Record 		RdbRecord_Record[RDB_RECORD_MAXNUM];
	 _RdbAlarm_Record		RdbAlarm_Record[RDB_ALARM_MAXNUM];
	 _SumMod				SumMod[FUN_SUMMOD_MAXNUM];
	 _FluxMod				FluxMod[FUN_FLUXMOD_MAXNUM];
	 _PidMod					PidMod[FUN_PIDMOD_MAXNUM];
	 _FilterMod				FilterMod[FUN_FILTERMOD_MAXNUM];
	 _TransformMod			TranformMod[FUN_TRANFORMMOD_MAXNUM];
}_Configuration;
typedef struct
{
  uint8 Dispcolor[4];		/*曲线组成员显示的颜色*/
  uint8 Groupmember[4];	/*曲线组成员*/
  uint8 DispOrNot[4];  		/*曲线组成员是否消隐1:显示   0:消隐*/
} _CurveGroup;

typedef struct __AlarmLine_Info
{
	short AlarmLine_HH;
	short AlarmLine_H;
	short AlarmLine_L;
	short AlarmLine_LL;
	unsigned char Color_HH;
	unsigned char Color_H;
	unsigned char Color_L;
	unsigned char Color_LL;
	unsigned char LineThick_HH;
	unsigned char LineThick_H;
	unsigned char LineThick_L;
	unsigned char LineThick_LL;
}_AlarmLine_Info;


typedef struct
{
 uint8 Out_SysNum;
 uint8 Out_SysRom;
 uint8 Out_SysAlm;
 uint8 Out_PIDFLAG;
 uint8 Out_SumFLAG;
 uint8 Out_PrintFLAG;
 uint8 Out_USBFLAG;
 uint8  reverse;
} _OutConfig;
/*
**************************************************************************************************
*										允许外部使用变量声明
**************************************************************************************************
*/
extern	_Configuration		ConfigPara;
extern	_RdbFile_Head 		*RdbFile_Head;
extern	_RdbBase_Record		*RdbBase_Record[RDB_BASE_MAXNUM];
extern	_RdbSample_Record 	*RdbSample_Record[RDB_SAMPLE_MAXNUM];
extern	_RdbRecord_Record 	*RdbRecord_Record[RDB_RECORD_MAXNUM];
extern	_RdbAlarm_Record		*RdbAlarm_Record[RDB_ALARM_MAXNUM];
extern	_SumMod			*SumMod[FUN_SUMMOD_MAXNUM];
extern	_FluxMod				*FluxMod[FUN_FLUXMOD_MAXNUM];
extern	_PidMod				*PidMod[FUN_PIDMOD_MAXNUM];
extern	_FilterMod			*FilterMod[FUN_FILTERMOD_MAXNUM];
extern	_TransformMod		*TranformMod[FUN_TRANFORMMOD_MAXNUM];
extern	_SumRecord			SumRecord[FUN_SUMMOD_MAXNUM];
extern	_SysParameter		SysPara;
extern	_UsbParameter		UsbPara;
extern	_PrintParameter            PrintPara;
extern	Time				SysTime;
extern	Time				PowerDownTime;
extern	_CurveGroup             	CurveGroup[4];

extern _AlarmLine_Info    g_AlarmLine_Info[4];

extern     _OutConfig                   OutConfig;
/*
**************************************************************************************************
*										允许外部使用函数声明
**************************************************************************************************
*/
extern	void  SaveConfigPara(void);
extern     uint8  CompareRDBPara(void);
extern	void MakeDefaultConfiguration(void);
#endif


