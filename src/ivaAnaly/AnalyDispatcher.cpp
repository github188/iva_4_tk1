#ifdef WIN32
#include <winsock2.h>
#else
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#endif
#include "oal_time.h"
#include "oal_unicode.h"
#include "oal_file.h"
#include "mq_database.h"
#include "AnalyDispatcher.h"
#include "AnalyPtzCtrl.h"
#include "AnalyCfgMan.h"
#include "image/cvxText.h"
#include "image/alg_osd.h"
#include "image/image_stitch.h"
#include "AnalyYuvCacheMan.h"
#include "AnalyEvidenceMan.h"
#include "PASRunner.h"
#include "NullRunner.h"
#include <assert.h>

AnalyDispatcher* AnalyDispatcher::m_pInstance = NULL;

AnalyDispatcher* AnalyDispatcher::GetInstance()
{
	return m_pInstance;
}

int AnalyDispatcher::Initialize()
{
	if(NULL == m_pInstance)
	{
		m_pInstance = new AnalyDispatcher();
		assert(m_pInstance);
		m_pInstance->Run();
	}
	return 0;
}

void AnalyDispatcher::UnInitialize()
{
	if(m_pInstance)
	{
		delete m_pInstance;
		m_pInstance=NULL;
	}
}

AnalyDispatcher::AnalyDispatcher()
{
	initFont();
	AnalyCfgMan::Initialize();
	AnalyYuvMan::Initialize();
	AnalyPlanMan::Initialize();
	AnalyEvidenceMan::Initialize();
	InitPtzCtrler();
	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		m_ptRunner[i] = RunnerFactoryCreate(i);
		assert(m_ptRunner[i]);
	}
}

AnalyDispatcher::~AnalyDispatcher()
{
	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		delete m_ptRunner[i];
		m_ptRunner[i] = NULL;
	}

	AnalyEvidenceMan::UnInitialize();
	AnalyPlanMan::UnInitialize();
	AnalyYuvMan::UnInitialize();
	AnalyCfgMan::UnInitialize();
	destoryFont();
}

void AnalyDispatcher::Run()
{
	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		m_ptRunner[i]->Run();
	}
}

int AnalyDispatcher::ManualSnap( int iChnID, ManualSnapT *ptInfo )
{
	if (iChnID <=0 && iChnID < MAX_CHANNEL_NUM && ptInfo)
	{
		m_ptRunner[iChnID]->ManualSnap(ptInfo);
		return 0;
	}
	return 1;
}

BaseRunner * RunnerFactoryCreate( int iChnID )
{
	BaseRunner * ptNewRunner = NULL;
	int iEdition = Global_BuilldEdition();
	if (iEdition == PAS_EDITION)
	{
		ptNewRunner = new PASRunner(iChnID);
		LOG_DEBUG_FMT("====PASRunner====");
	}
	else
	{
		ptNewRunner = new NullRunner(iChnID);
		LOG_DEBUG_FMT("====NullRunner====");
	}
	return ptNewRunner;
}

