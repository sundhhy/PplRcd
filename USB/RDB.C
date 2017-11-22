

#include "rdb.h"
#include "ConfigInfo.h"
#include <stdio.h>




void	RDBInit(uint8	*baseaddr)
{
	uint8	RdbBaseNum,RdbSampleNum,RdbRecordNum,RdbAlarmNum;
	uint32	base;
	uint8	i;
	
	SPIRead(CONFIGURATIONADDR, (uint8 *)&ConfigPara,SIZE_OF_CONFIGURATION);
								
	RdbBaseNum=ConfigPara.RdbFile_Head.RdbNumber;
	RdbSampleNum=ConfigPara.RdbFile_Head.SampleNumber;
	RdbRecordNum=ConfigPara.RdbFile_Head.RecordNumber;
	RdbAlarmNum=ConfigPara.RdbFile_Head.AlarmNumber;
	base=(uint32)&ConfigPara+SIZE_OF_FILEHEAD;
	for(i=0;i<RdbBaseNum;i++)
		RdbBase_Record[i]=(_RdbBase_Record *)(base+i*SIZE_OF_RDBBASE);
	base+=RdbBaseNum*SIZE_OF_RDBBASE;
	for(i=0;i<RdbSampleNum;i++)
		RdbSample_Record[i]=(_RdbSample_Record *)(base+i*SIZE_OF_RDBSAMPLE);
	base+=RdbSampleNum*SIZE_OF_RDBSAMPLE;
	for(i=0;i<RdbRecordNum;i++)
		RdbRecord_Record[i]=(_RdbRecord_Record *)(base+i*SIZE_OF_RDBRECORD);
	base+=RdbRecordNum*SIZE_OF_RDBRECORD;
	for(i=0;i<RdbAlarmNum;i++)
		RdbAlarm_Record[i]=(_RdbAlarm_Record *)(base+i*SIZE_OF_RDBALARM);
	base+=RdbAlarmNum*SIZE_OF_RDBALARM;
	for(i=0;i<FUN_SUMMOD_MAXNUM;i++)
		SumMod[i]=(_SumMod *)(base+i*SIZE_OF_SUMMOD);
	base+=(FUN_SUMMOD_MAXNUM*SIZE_OF_SUMMOD);
	for(i=0;i<FUN_FLUXMOD_MAXNUM;i++)
		FluxMod[i]=(_FluxMod *)(base+i*SIZE_OF_FLUXMOD);
	base+=FUN_FLUXMOD_MAXNUM*SIZE_OF_FLUXMOD;
	for(i=0;i<FUN_PIDMOD_MAXNUM;i++)
		PidMod[i]=(_PidMod *)(base+i*SIZE_OF_PIDMOD);
	base+=FUN_PIDMOD_MAXNUM*SIZE_OF_PIDMOD;
	
	for(i=0;i<FUN_FILTERMOD_MAXNUM;i++)
		FilterMod[i]=(_FilterMod *)(base+i*SIZE_OF_FILTERMOD);
	base+=FUN_FILTERMOD_MAXNUM*SIZE_OF_FILTERMOD;
	for(i=0;i<FUN_TRANFORMMOD_MAXNUM;i++)
		TranformMod[i]=(_TransformMod *)(base+i*SIZE_OF_TRANSFORMMOD);
	
}

