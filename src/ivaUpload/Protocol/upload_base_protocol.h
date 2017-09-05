#ifndef __UPLOAD_BASE_H__
#define __UPLOAD_BASE_H__
#include "../QueryDb.h"

enum {
	UpData_None = 0, // NONE：不需要上传数据
	UpData_Url  = 1, // URL ：上传文件的下载地址，由平台自行下载
	UpData_Data = 2, // DATA：将文件base64打包到JSON中，与结构化信息一并上传
	UpData_Ftp  = 3  // FTP ：通过FTP将文件上传到指定位置，然后将相对路径提供给平台
};

typedef struct _DataUploadOption
{
	int type;
	FtpSvr tFtpSvr;
	bool bUploadImg;
	bool bUploadRec;
}DataUploadOption;

#define UPLOAD_RET_OK			0	// 上传成功
#define UPLOAD_RET_NO_FILE		1	// 上传失败，因为缺少文件
#define UPLOAD_RET_ERROR		2	// 上传失败，因为服务器未成功接收

#define NMS_HEART_CYCLE 60	// 心跳周期60秒


// 上传的基类，以后各个项目的不同上传方式从此派生
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

