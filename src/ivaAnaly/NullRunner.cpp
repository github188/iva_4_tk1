#include "NullRunner.h"
#include "oal_log.h"

NullRunner::NullRunner(int iChnID):BaseRunner(iChnID)
{
}

NullRunner::~NullRunner()
{
}

void NullRunner::CheckPreset()
{
	printf("NullRunner::CheckPreset()\n");
	return;
}

void NullRunner::ProcessFrame( YuvCacheNode * ptYuvNode )
{
	usleep(40*1000);
	return;
}

void NullRunner::ResetAnaly()
{
	printf("NullRunner::ResetAnaly()\n");
	return;
}

int NullRunner::CollectAlgResult( int iChnID, const void * ptAlgRes, AnalyData *ptData )
{
	printf("NullRunner::CollectAlgResult()\n");
	return 0;
}

void NullRunner::ManualSnap( ManualSnapT *ptInfo )
{
	printf("NullRunner::ManualSnap()\n");
	return;
}

void NullRunner::CreateAlgChannel()
{
	printf("NullRunner::CreateAlgChannel()\n");
	return;
}

void NullRunner::DestroyAlgChannel()
{
	printf("NullRunner::DestroyAlgChannel()\n");
	return;
}



