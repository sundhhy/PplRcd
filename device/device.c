#include "device.h"
#include "deviceId.h"

#include "basis/sdhDebug.h"
#include "sdhDef.h"
#include "basis/sdhError.h"

devId_t Dev_TYPE( devId_t devid)
{
	int type = 0;
	type = devid >> ( DEVID_MINOR_BITS + DEVID_MINOR_BITS);
	type = type & ((1 << DEVID_TYPE_BITS) - 1);
	return type;
}

devId_t Dev_MAJORID( devId_t devid)
{
	int major = 0;
	major = devid >> DEVID_MINOR_BITS;
	major = major & ((1 << DEVID_MAJOR_BITS) - 1);
	return major;
}

devId_t Dev_MINORID( devId_t devid)
{
	int minor = 0;

	minor = devid & ((1 << DEVID_MINOR_BITS) - 1);
	return minor;
}

int Dev_open( devId_t devId, void **dev)
{
	int type = Dev_TYPE( devId);
	int majorId = Dev_MAJORID( devId);
	int minorId = Dev_MINORID( devId);
	int ret = 0;
	
	switch( type)
	{
		case DEVTYPE_CHAR:
			ret = DevChar_open( majorId, minorId, dev);
			break;
		case DEVTYPE_DISPLAY:
			ret = DevLCD_open( majorId, minorId, dev);
			break;
		default:
			ret = ERR_BAD_PARAMETER;
			break;
		
	}
	
	
	return ret;
	
}

