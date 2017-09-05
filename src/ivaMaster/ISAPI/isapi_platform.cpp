#include "isapi_log.h"
#include "oal_time.h"
#include "capacity.h"
#include "../Manager/DeviceManager.h"
#include "../Manager/SystemStatusMonitor.h"
#include "../Manager/VideoInManager.h"
#include "../Manager/AnalyManager.h"
#include "../Manager/RecManager.h"


namespace ISAPI
{
	namespace PlatformNS
	{
		void AllStat_GET( JsonElement& jRespRoot )
		{
			jRespRoot["model"] = Global_DeviceModel();
			jRespRoot["hardware"] = Global_HardVersion();
			jRespRoot["software"] = Global_SoftVersion();

			jRespRoot["uptime"] = SystemStatusMonitor::GetInstance()->GetUptime();

			JsonElement jGps;
			jGps["longitude"] = 0;
			jGps["latitude"] = 0;
			jRespRoot["gps"] = jGps;

			AllDiskShow tDisk;
			memset(&tDisk, 0, sizeof(AllDiskShow));
			DeviceManager::GetInstance()->GetAllDiskShow(&tDisk);

			u64 uAllTotalSize = tDisk.tRom.uTotalSize;
			u64 uAllFreeSize = tDisk.tRom.uFreeSize;
			uAllTotalSize += tDisk.tSD.uTotalSize;
			uAllFreeSize += tDisk.tSD.uFreeSize;
			uAllTotalSize += tDisk.tUSB.uTotalSize;
			uAllFreeSize += tDisk.tUSB.uFreeSize;
			uAllTotalSize += tDisk.tHDD.uTotalSize;
			uAllFreeSize += tDisk.tHDD.uFreeSize;

			int iRate = (uAllTotalSize > 0 ? (int)(100*uAllFreeSize/uAllTotalSize) : 0);
			JsonElement jStoreStatus;
			jStoreStatus["total"] = PrintSize(uAllTotalSize);
			jStoreStatus["usage"]  = 100-iRate;
			jRespRoot["storage"] = jStoreStatus;

			SysStatus tStatus = {0};
			SystemStatusMonitor::GetInstance()->GetStatus(tStatus);
			JsonElement jChns;
			for (int i = 0; i < MAX_CHANNEL_NUM; i++)
			{
				JsonElement jChni;
				jChni["id"] = i+1;
				jChni["video"] = (VideoInManager::GetInstance()->GetChnStatus(i)==VIS_NORMAL);;
				jChni["analyze"] = (AnalyManager::GetInstance()->GetAnalyStatus(i)==ANALY_RUNNING);
				jChni["record"] = (RecManager::GetInstance()->GetChnStatus(i)==REC_RUNNING);

				RtspInfo tMainRtsp = {0};
				RtspInfo tSubRtsp = {0};
				VideoInManager::GetInstance()->GetChnRtsp(i, STREAM_TYPE_MAIN, tMainRtsp);
				VideoInManager::GetInstance()->GetChnRtsp(i, STREAM_TYPE_SUB, tSubRtsp);

				jChni["mainstream"] = tMainRtsp.szUrl;
				jChni["substream"] = tSubRtsp.szUrl;

				jChns.append(jChni);
			}

			jRespRoot["channel"] = jChns;
		}
	}

}
