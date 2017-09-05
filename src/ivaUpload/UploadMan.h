#ifndef __UPLOAD_CFG_MAN_H__
#define __UPLOAD_CFG_MAN_H__
#include "pthread.h"
#include "mq_upload.h"
#include "Protocol/upload_base_protocol.h"

class UploadMan
{
public:
	static UploadMan* GetInstance();
	static int Initialize();
	static void UnInitialize();
private:
	UploadMan();
	~UploadMan();
public:
	int Run();

public:
	int SetUploadStrategy(const UploadStrategy* ptInfo);
	int SetUploadEnable(bool bEnable);
	int SetUploadFtpAdv(const FtpAdvance* ptInfo);
	int GetFtpDirGrammar(int iDataType, char * pszDir);
	bool FtpEnableUploadFile(int iFlag);
	bool FtpUploadFileCharsetByUtf8();

	ProtocolHandle GetUploadProtocolHandle();
	bool NeedRemoveFileAfterUpload();
	int GetUploadServerFtp(FtpSvr* ptFtp);

	bool GetPlatformAndEnable(Platform *ptSvr);
	bool GetMqttAndEnable( Platform *ptSvr );

	bool m_bExitThread;
	pthread_t m_tUploadTread;
	pthread_t m_tPerviewThread;

private:
	pthread_mutex_t m_tMutex;
	bool			m_bEnable;
	UploadStrategy  m_tStrategy;
	FtpAdvance      m_tFtpAdvance;

private:
	static UploadMan* m_pInstance;
};



#endif //__UPLOAD_CFG_MAN_H__

