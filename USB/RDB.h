#ifndef RDB_H
#define	RDB_H
#include "config.h"
//#include "rtc.h"



/*实时数据库基本信息文件定义*/

#define	RDB_BASE_MAXNUM			16
#define	RDB_SAMPLE_MAXNUM  		16
#define	RDB_RECORD_MAXNUM		16
#define	RDB_ALARM_MAXNUM			16
#define	FUN_SUMMOD_MAXNUM		16
#define	FUN_SUMRECORD_MAXNUM	16
#define	FUN_FLUXMOD_MAXNUM		16
#define	FUN_PIDMOD_MAXNUM		8
#define	FUN_FILTERMOD_MAXNUM	16
#define	FUN_TRANFORMMOD_MAXNUM 8
#define 	SIZE_OF_RDBBASE 			(sizeof(_RdbBase_Record))
#define 	SIZE_OF_RDBSAMPLE 		(sizeof(_RdbSample_Record))
#define 	SIZE_OF_RDBRECORD			(sizeof(_RdbRecord_Record))
#define 	SIZE_OF_RDBALARM			(sizeof(_RdbAlarm_Record))
#define 	SIZE_OF_FILEHEAD			64
#define	SIZE_OF_SUMMOD			(sizeof(_SumMod))
#define	SIZE_OF_SumRecord			(sizeof(_SumRecord))
#define	SIZE_OF_FLUXMOD			(sizeof(_FluxMod))
#define	SIZE_OF_PIDMOD			(sizeof(_PidMod))
#define	SIZE_OF_TRANSFORMMOD	(sizeof(_TransformMod))
#define	SIZE_OF_FILTERMOD			(sizeof(_FilterMod))
#define	SIZE_OF_CONFIGURATION	(sizeof(_Configuration))

/*****************************************/
/*	RDB文件头信息描述了文件版本及Rdb变量数目*/
typedef	struct 			/*64 RDB文件头信息结构*/
{
	unsigned char		FileInfo[16];		/*00,16=16文件信息*/
	unsigned char		iVersion[4];		/*16,04=20 版本号*/
	unsigned long		FileDateTime;	/*20,04=24 文件生成时间*/
	unsigned long		FileRightFlag;	/*24,04=28 文件正确标识*/
	unsigned short	MaxRdbId;		/*28,02=30 Rdb最大索引*/
	unsigned short	RdbNumber;		/*30,02=32 Rdb变量数目*/
	unsigned short	CvNumber;		/*32,02=34 转换信息数目*/
	unsigned short	SampleNumber;	/*34,02=36 采集信息数目*/
	unsigned short	AlarmNumber;	/*36,02=38 报警信息数目*/
	unsigned short	RecordNumber;	/*38,02=40 记录信息数目*/
	unsigned short	DeviceNumber;	/*40,02=42 设备信息数目*/
	unsigned short	PortNumber;		/*42,02=44 端口信息数目*/
	unsigned char		nouse20[20];	/*44,20=64 保留*/
}_RdbFile_Head;

typedef struct _RdbBase_Record			/*64 RDB基本信息结构*/
{
	signed long		TagID;			/*00,04=04变量索引	*/
	unsigned char  	Name[11];		/*04,16=20变量名		*/
	unsigned char		nouse[5];           /*保留*/
	short			Value;			/*20,04=24变量值		*/
	short			Reserve4;
	unsigned char		VarType;		/*24,01=25变量类型	*/
	unsigned char		RecordProp;		/*25,01=26保持属性	*/
	unsigned char		RwProp;			/*26,01=27读写属性	*/
	unsigned char		OpenProp;		/*27,01=28开放属性	*/
	float				InitValue;		/*28,04=32变量初值	*/
	//float			MinEVal;		/*32,04=36最小工程值*/
	short			Point;			/*小数位数*/
	short			MinEVal;			/*量程下限*/
	//float			MaxEVal;		/*36,04=40最大工程值*/
	short			HisValue;		/*旧值*/
	short			MaxEVal;		/*量程上限*/		
	//float			MinRVal;			/*40,04=44最小测量值*/
	short                     MinRVal;	/*采样信号下限*/
	short			Reserve2;		/*保留*/
	//float			MaxRVal;		/*44,04=48最大测量值*/
	short                     MaxRVal;    /*采样信号上限*/
	short			Reserve1;		/*保留*/
	unsigned char		CommentInfoID;/*单位*/
	unsigned char        DispColor;           /*显示颜色*/
	signed short		CvInfoID;		/*50,02=52转换信息索引*/
	signed short		SampleInfoID;	/*52,02=54采集信息索引*/
	signed short		AlarmInfoID;	/*54,02=56报警信息索引*/
	signed short		OptionInfoID;	/*56,02=58历史记录及安全信息索引*/
	unsigned char       UnitName[6];      /*58,06=59自定义单位名称*/
	
	       
}_RdbBase_Record;

typedef struct 		/*12 RDB采集信息结构*/
{
	uint16	TagId;							/*00,02=02 采集索引*/
	int16	DeviceId;						/*02,02=04 设备索引*/
//	uint16	AppendInfo1;						/*04,02=06 附加信息1，通道类型*/
	uint8	SingalType;						/*通道类型*/
	uint8	Status;							/*通道状态*/
	uint16	AppendInfo2;					/*06,02=08 附加信息2，模块号*/
	uint16	AppendInfo3;					/*08,02=10 附加信息3，通道号*/
	uint16	SampleValue;					/*10,02=12 附加信息4，采样值*/
}_RdbSample_Record;
/*采集索引：记录Rdb变量采集信息的索引，从0开始线性递增；*/
/* 	数据来源：记录Rdb变量的数据来源，有本地（0x01）和外部设备（0x02）；*/
/* 	设备索引：记录Rdb变量设备信息的索引，当其为-1时表示无设备信息，即为本地，否则根据该值在RDB设备信息中找到相应的记录；*/
/* 	附加信息1：在PLC中为通道类型，在HMI中为数据类型，在VPR中为通道类型；*/
/* 	附加信息2：在PLC中为模块号，在HMI中为变量类型，在VPR中为模块号*/
/* 	附加信息3：在PLC中为通道号，在HMI中为变量地址，在VPR中为通道号；*/
/* 	附加信息4：在HMI中为连续采集的变量个数；*/

/*实时数据库历史记录信息文件定义*/
typedef struct 		/*16 RDB记录信息结构*/
{
	signed short		TagId;				/*00,02=02记录索引*/
	signed char		RecordMethod;		/*02,01=03 记录方法*/
	unsigned char		TagPriority;			/*03,01=04 变量优先级*/
	unsigned long		RecordCycle;		/*04,04=08 时间间隔*/
	float				Sensitivity;			/*08,04=12灵敏度*/
	unsigned short	RecordLen;			/*12,04=16 记录长度*/
	unsigned short	Reserver;
}_RdbRecord_Record;

typedef struct 				/*64 RDB报警信息结构*/
{
	signed short		TagId;				/*00,02=02 报警索引*/
	unsigned char		AlarmStatus;		/*02,01=03 报警状态*/	
	unsigned char		AlarmAck;			/*03,01=04 报警确认*/	
	unsigned short	AlarmNo;			/*04,02=06 报警编号*/
	signed char		AlarmGroup;			/*06,01=07 报警组*/
	unsigned char		AlarmType;			/*07,01=08 报警类型*/
	unsigned char		AlarmFlag;			/*08,01=09 保留*/
	unsigned char		AlarmEnable;		/*09,01=10 报警使能*/
	unsigned short	DelayTime;			/*10,02=12 报警持续时间*/	
	short			Reserve1;
	short			HiHiLimit;
	//float				HiHiLimit;			/*12,04=16 高高报警限*/
	short			Reserve2;
	short			HiLimit;
	//float				HiLimit;				/*16,04=20 高报警限*/
	short			Reserve3;
	short			LoLimit;
	//float				LoLimit;				/*20,04=24 低报警限*/	
	short			Reserve4;
	short			LoLoLimit;
	//float				LoLoLimit;			/*24,04=28 低低报警限*/
	short			LimitDeadArea;		/*28,04=32 报警死区*/	
	short			Reserver5;
	float				RocPctLimit;			/*32,04=36 变化率报警限*/	
	unsigned char		RocPctTime;			/*36,01=37 变化率报警时间限*/	
	unsigned char		HiHiAlarmNo;		/*37,01=38 高高报触点*/
	unsigned char		HiAlarmNo;			/*38,01=39 高报触点*/
	unsigned char		LoAlarmNo;			/*39,01=40 低报触点*/
	unsigned char		LoLoAlarmNo;		/*40,01=41 低低报触点*/
	unsigned char		RocPctNo;			/*41,01=42 变化率报警触点*/
	unsigned char 	AlarmHListNo;		/*42,01 当前报警在报警队列里的序号*/
	unsigned char		AlarmLListNo;		/*43,01 当前报警在报警队列里的序号*/
	unsigned char		AlarmMsg[16];		/*44,16=60 报警消息*/
	unsigned char		nouse4[4];			/*60,04=64 保留*/
}_RdbAlarm_Record;

/*报警记录定义*/
typedef struct	
{
	uint8	TagID;			/*对应RDB的索引*/
	Time 	StartAlmTime;	/*报警起始时间*/
	Time	CancelAlmTime;	/*消除报警时间*/
	uint8	AlmType;		/*报警类型*/
	uint8	Confim;			/*报警确认*/
}_AlmRecord;
/**********************************************************************************/
/*以下为算法结构*/
/**********************************************************************************/
/*累积算法结构定义*/
typedef struct 	/*8Byte*/
{
	uint8	TagID;			/*对应的RDB Tag*/
	uint8 	Enable;			/*是否进行累积*/
	uint8 	StartYear;		/*累积的起始年*/
	uint8 	StartMonth;		/*累积的起始月*/
	uint8 	StartDay;		/*累积的起始日*/
	uint8	Reserve[3];
}_SumMod;
/*累积列表,应存储于FeRAM中,共4352个字节,历史记录占用170个字节*/
 typedef struct 
{
	uint8	SumYear;
	uint8	SumMonth;
	uint16	Sumbuf_day[31][3];		/*日累积*/
	uint16	Sumbuf_month[12][3];	/*月累积*/
	uint16	Sumbuf_year[3];			/*年累积*/
	uint16	Sumbuf_Sum[3];			/*总累计*/
	uint16	Sum_remain;			/*累积余量*/
}_SumRecord;
/*流量算法,补偿结构定义*/
typedef struct	_FluxMod		/*40Byte*/
{
	uint8   	Enable; 				/*是否流量通道*/
	uint8	CompMethod;		/*0:不进行补偿1:压力是否定值2:温度是否定值*/
	uint8	FluxMethod;			/*流量模型*/
	uint8	TempTagID;			/*外给定温度通道时所对应的RDB变量ID*/
	
	uint8	PressTagID;			/*外给定温度通道时所对应的RDB变量ID*/
	uint8	Flux_KPoint;			/*流量系数K的小数 位数*/
	uint8	RatedPress_Point;	/*额定压力小数点*/
	uint8	Flux_MPoint;		/*流体密度小数位数*/
	
	int16	Flux_M;				/*流体密度 */
	int16	Flux_K;	     			/*流量系数 K*/
	int16	RatedPress;			/*额定压力*/
	int16	RetadTemp;			/*额定温度*/
	
	uint8	RetadTemp_Point;	/*额定温度小数点*/	
	uint8	FixedPress_Point;	/*压力定值小数*/
	uint8	CompressZf_Point;	/*压缩系数Zf 小数位数 */
	uint8	FixedTemp_Point;	/*温度定值 */
	
	int16	FixedPress;			/*压力定值*/
	int16	FixedTemp;			/*温度定值 */

	int16	CompressZf;			/*压缩系数Zf */
	int16	CompressZn;		/*压缩系数Zn*/

	uint8	CompressZn_Point;	/*压缩系数Zn小数位数 */	
	uint8	FluxType; 			/*介质类型*/
	uint8	Flux_Point;			/*流量量程小数点*/
	uint8	FluxUnit;			/*流量单位*/

       uint8       SquareEnable;		/*是否开方*/
	uint8	OutPut_Tagid;		/*输出RDB 的索引*/
	uint8	Reserve[2];

	int16	FluxRangeh;			/*流量量程上限*/
	int16	FluxRangel;			/*流量量程下限*/

}_FluxMod;
/*PID 控制算法结构定义*/
typedef struct 			/*36Byte*/
{	
	uint16	KP;					/*比例系数*/	
	uint16	TI;					/*积分时间*/
	
	uint8	KP_Point;			/* 比例系数小数点位数 ，1~5*/
	uint8	TI_Point;			/*积分时间小数位数*/
	uint8	IS_Point;			/*积分分离的小数点位数，3  */
	uint8	TD_Point;			/*微分时间小数位数*/
	
	uint16	TD;					/*微分时间*/
	uint16	IS;					/*积分分离,0~100 百分数*/
		
	int16	SV;					/*设定值,工程量*/
	uint16	DV;					/*死区,0~1百分数*/
	
	uint8	SV_Point;			/*设定值的小数点位数，3  */
	uint8	DV_Point;			/* 死区的小数点位数，3 */
	uint8	Outrange_Point;		/* 输出小数点位数，3 */
	uint8	OriginalPos;			/*阀位初值 0：保持  1：固定值  */
	
	uint16	Outrangeh;			/* 输出上限 0~100*/	
	uint16	Outrangel;			/*输出下限，0~100  */
	

	uint16	ConstPos;			/*固定值，0~1  */
	uint16	SetPos;				/*输出值  */
	
	uint8	ConstPos_P;			/*固定值的小数点位数，3  */
	uint8	PidMode;			/*手操器的方式 0：手动 1：自动  */
	uint8	PosorNeg;			/*正反作用，0：反作用 1：正作用  */
	uint8	SV_Mode;			/* 给定模式，0：内给定，1：外给定 */

	int16	SV_K;
	uint8	SV_K_P;
	uint8	Reserve1;
	
	int16 	SV_b;
	uint8	SV_b_P;
	uint8	Reserve2;
	
	uint8	SV_TagID;			/*外给定通道   */
	uint8	PV_TagID;			/*采样通道   */
	uint8	Enable;
	uint8	MV_TagID;			/*输出通道*/

	uint8	Name[11];			/*回路名称*/
	uint8       Reserve3[5];
}_PidMod;
typedef struct
{
	uint8	PV_TagID;			/*采样通道*/
	uint8	MV_TagID;			/*输出通道*/
	int16	Rangeh;				/*变送采样通道的量程*/
	int16	Rangel;				/*变送采样通道的量程*/
	uint8	PosorNeg;			/*正向或反向变送*/
	uint8	Enable;	
}_TransformMod;
typedef	struct 
{
	float   	f_KP;		//比例度浮点
	float		f_TI;
	float		f_TD;
	float		f_IS;
	float		f_SV;
	float		f_DV;
	float		f_Outrangeh;
	float		f_Outrangel;
	float		f_ConstPos;
	float		f_SetPos;
	float		f_SV_K;
	float		f_SV_b;
	
	float	f_EV;		// 偏差 
	float	f_EV1;		// 上次偏差 
	float	f_EV2;		// 上上次偏差 
	float	f_UV;		// 增量临时值 
	float	f_SM;		// 积分累积址 
	float	f_AV;		// 增量
	float f_PV;		//采样值
}_PIDApp;
 typedef struct			/*12byte*/
{	uint8	TagID;
	uint8	FilterTime;			/*滤波时间*/
	uint16	OldValue;			/*滤波前一次采样值*/
	uint8	SmallSignal;			/*小信号切除*/
	uint8       reveserve;
	int16	ZeroOffset_K;		/*零点迁移K*/
	int16	ZeroOffset_B;		/*零点迁移b*/
	uint8	Reserve[2];	
}_FilterMod;

//=============================================================================

extern	void RDBInit(uint8 *baseaddr);

#endif
