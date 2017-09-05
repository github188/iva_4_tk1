#ifndef __UPLOAD_HTTP_COM_H__
#define __UPLOAD_HTTP_COM_H__

#include "upload_base_protocol.h"
#include <string>

// HTTP协议上传到新汇聚平台
class UploadByHttpCommon : public UploadBaseProtocol
{
private:
	UploadByHttpCommon(void);
public:
	virtual ~UploadByHttpCommon(void);
	static ProtocolHandle m_pInstance;
	static ProtocolHandle GetInstance();
	
public:
	int PostSomething(std::string &szUrl, std::string &szContent, std::string &szResp);

	virtual int InitSever();
	virtual int UploadData(const AnalyDbRecord *pData);
	virtual int UploadAlarm(const AlarmDbRecord *pData);

	virtual bool CanUploadData();
	virtual bool CanUploadAlarm();
	virtual bool CanRemoveFile();
private:
	int DoUploadResp( std::string szResp );
	int PackeUploadDataJson(const AnalyDbRecord *pData, std::string &szContent);

public:
	void TurnToUregistered();

	int CheckRegister();

private:
	int PostRegister();
	int DoRegisterResp(std::string szResp);
	time_t m_last_post_time;
	unsigned int m_need_wait_secs;

	bool m_bNeedUploadAlarm;
	DataUploadOption m_tUpOption;

public:
	int CheckStatus();
private:
	int PostStatus();
	int DoStatusResp(std::string szResp);

public:
	bool m_bExitThread;
	bool m_bRegistedOk;

private:
	pthread_t m_heart_thread;
};

#endif // __UPLOAD_HTTP_COM_H__

