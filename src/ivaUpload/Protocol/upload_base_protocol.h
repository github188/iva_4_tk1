#ifndef __UPLOAD_BASE_H__
#define __UPLOAD_BASE_H__
#include "../QueryDb.h"

enum {
	UpData_None = 0, // NONE������Ҫ�ϴ�����
	UpData_Url  = 1, // URL ���ϴ��ļ������ص�ַ����ƽ̨��������
	UpData_Data = 2, // DATA�����ļ�base64�����JSON�У���ṹ����Ϣһ���ϴ�
	UpData_Ftp  = 3  // FTP ��ͨ��FTP���ļ��ϴ���ָ��λ�ã�Ȼ�����·���ṩ��ƽ̨
};

typedef struct _DataUploadOption
{
	int type;
	FtpSvr tFtpSvr;
	bool bUploadImg;
	bool bUploadRec;
}DataUploadOption;

#define UPLOAD_RET_OK			0	// �ϴ��ɹ�
#define UPLOAD_RET_NO_FILE		1	// �ϴ�ʧ�ܣ���Ϊȱ���ļ�
#define UPLOAD_RET_ERROR		2	// �ϴ�ʧ�ܣ���Ϊ������δ�ɹ�����

#define NMS_HEART_CYCLE 60	// ��������60��


// �ϴ��Ļ��࣬�Ժ������Ŀ�Ĳ�ͬ�ϴ���ʽ�Ӵ�����
class UploadBaseProtocol
{
public:
	UploadBaseProtocol();
	~UploadBaseProtocol();
	
public:
	virtual int InitSever() = 0;
	virtual int UploadData(const AnalyDbRecord *pData) = 0;
	virtual int UploadAlarm(const AlarmDbRecord *pData) = 0;

	virtual bool CanUploadData() = 0;
	virtual bool CanUploadAlarm() = 0;
	virtual bool CanRemoveFile() = 0;

	virtual int GenXmlOne(const char *file_name, const AnalyDbRecord *pData);
	virtual int ShutDown();
};

typedef UploadBaseProtocol * ProtocolHandle;

size_t http_resp_data_writer(void* data, size_t size, size_t nmemb, void* content);
const char * LocalIPAddr();
int File2HttpBoaUrl(const char * file_path, char *url);
int PackageFile2Base64Buf(const char * file_path, char ** base64_str);
int FtpSvrSubDir(const AnalyDbRecord *data, const char * grammar, char *sub_dir);
//int read_xml_to_buf(const char *xml_name, char *out_buf, size_t size);


#endif // __UPLOAD_BASE_H__

