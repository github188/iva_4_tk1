#include "include/capacity.h"
#include <stdio.h>
#include <string.h>

#ifndef BUILD_EDITION
#error no define BUILD_EDITION
#endif

#if (BUILD_EDITION == PAS_EDITION)
#define DEVICE_MODEL_STRING "IVA-PMS"
#elif (BUILD_EDITION == FSS_EDITION)
#define DEVICE_MODEL_STRING "IVA-FSS"
#else
#define DEVICE_MODEL_STRING "IVA_COM"
#endif

#define HARDWARE_VERSION "1.0.0.1"

#ifndef SOFT_VERSION
#error no define SOFT_VERSION
#endif


GLOBAL_API int Global_BuilldEdition()
{
	return BUILD_EDITION;
}

GLOBAL_API const char * Global_DeviceModel()
{
	return DEVICE_MODEL_STRING;
}

GLOBAL_API const char * Global_HardVersion()
{
	return HARDWARE_VERSION;
}

GLOBAL_API const char * Global_SoftVersion()
{
	return SOFT_VERSION;
}

GLOBAL_API int Global_EventDef( int iEvent, EventDef *ptDef )
{
	if (ptDef == NULL)
	{
		return -1;
	}

	int iRet = 0;
	switch( iEvent )
	{
	case eIT_Illegal_Park:
		strcpy(ptDef->szName, "违法停车");
		ptDef->bSupportEvent = true;
		ptDef->tEventImageNum.iMin = 1;
		ptDef->tEventImageNum.iMax = 2;

		ptDef->bSupportIllegal = true;
		ptDef->iEviModelNum = 2;
		strcpy(ptDef->tIllegalModelDef[0].szName,"即停即走");
		ptDef->tIllegalModelDef[0].tImageNumScope.iMin = 2;
		ptDef->tIllegalModelDef[0].tImageNumScope.iMax = 4;
		strcpy(ptDef->tIllegalModelDef[1].szName,"二次抓拍");
		ptDef->tIllegalModelDef[1].tImageNumScope.iMin = 2;
		ptDef->tIllegalModelDef[1].tImageNumScope.iMax = 4;

		ptDef->iThresholdNum = 1;
		ptDef->tThresholdDef[0].iWhich = 0;
		strcpy(ptDef->tThresholdDef[0].szName,"违停判定时间");
		strcpy(ptDef->tThresholdDef[0].szUnit,"秒");
		ptDef->tThresholdDef[0].tScope.iMin = 2;
		ptDef->tThresholdDef[0].tScope.iMax = 600;

		ptDef->bSupportCalibrate  = false;
		ptDef->bSupportShieldArea = true;
		ptDef->bSupportAnalyArea  = true;
		ptDef->bSupportPolyline   = false;	
		break;
	default:
		iRet = -1;
		break;
	};
	return iRet;
}

GLOBAL_API void Global_DefualtThreshold( int iEvent, int & iValue1, int & iValue2, double & fValue1, double & fValue2 )
{
	switch( iEvent )
	{
	case eIT_Illegal_Park:
		iValue1 = 2;	// 允许停留时间
		iValue2 = 0;	// 无意义
		fValue1 = 0;	// 无意义
		fValue2 = 0;	// 无意义
		break;
	default:
		iValue1 = 0;	// 无意义
		iValue2 = 0;	// 无意义
		fValue1 = 0;	// 无意义
		fValue2 = 0;	// 无意义
		break;
	};
}

GLOBAL_API const char * Global_DefualtCode(int iEvent)
{
	const char * pCode = NULL;
	
	switch( iEvent )
	{
	case eIT_Illegal_Park:
		pCode = "10390";
		break;
	default:
		pCode = "";
		break;
	};
	
	return pCode;
}

GLOBAL_API const char * Global_DefualtDesc(int iEvent)
{
	const char * pDesc = NULL;

	switch(iEvent)
	{
	case eIT_Illegal_Park://10390
		//pDesc="机动车违反规定停放、临时停车且驾驶人不在现场或者虽在现场但驾驶人拒绝立即驶离，妨碍其它车辆、行人通行的";
		pDesc="违法停车";
		break;
	default:
		pDesc = "";
		break;
	};
	//12081
	//通过有交通信号灯控制路口时，不按所需行进方向驶入导向车道的

	return pDesc;
}


