#ifndef __UPLOAD_FTP_COM_H__
#define __UPLOAD_FTP_COM_H__

#include "upload_base_protocol.h"

// 通用FTP协议
class UploadByFtpCommon : public UploadBaseProtocol
{
private:
	UploadByFtpCommon(void);
public:
	virtual ~UploadByFtpCommon(void);
	static ProtocolHandle m_pInstance;
	static ProtocolHandle GetInstance();
	
public:
	virtual int InitSever();
	virtual int UploadData(const AnalyDbRecord *pData);
	virtual int UploadAlarm(const AlarmDbRecord *pData);

	virtual bool CanUploadData();
	virtual bool CanUploadAlarm();
	virtual bool CanRemoveFile();
};

int FtpUploadOneFile(FtpSvr * pFtpSvr, char * szSrcFileUtf8, char * szDirNameUtf8, char *szDstFileUtf8);
#endif // __UPLOAD_FTP_COM_H__

