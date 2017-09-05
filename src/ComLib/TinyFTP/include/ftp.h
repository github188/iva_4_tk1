#ifndef __TINY_FTP_H__
#define __TINY_FTP_H__

#ifndef FTP_API
	#ifndef WIN32
		#ifdef __cplusplus
			#define FTP_API extern "C"
		#else
			#define FTP_API
		#endif
	#else
		#ifdef TINYFTP_EXPORTS
			#define FTP_API extern  "C"   __declspec(dllexport)
		#else
			#define FTP_API extern  "C"   __declspec(dllimport)
		#endif
	#endif
#endif

#define FTP_SUCCESS             0   // �ɹ�
#define FTP_ERROR_PARAM         1   // ��������
#define FTP_ERROR_SERVER        2   // ������������ 
#define FTP_ERROR_FILE          3   // �ļ������� 
#define FTP_ERROR_USER          4   // �û��������� 
#define FTP_ERROR_PASSWD        5   // ������� 


typedef void *  FtpHanlde;


FTP_API int FTP_UploadFileEntire(const char *addr, int port, 
					const char *user, const char *pass,
					const char *dst_dir, const char *dst_file,
					const char *src_file);


FTP_API FtpHanlde FTP_Open(const char *addr, int port, const char *user, const char *pass);

FTP_API int FTP_UploadFile(FtpHanlde tHandle, const char *dst_dir, const char *dst_file, const char *src_file);

FTP_API int FTP_SendCmd(FtpHanlde tHandle, const char *cmd, const char *params);

FTP_API int Ftp_Close(FtpHanlde tHandle);

#endif//__TINY_FTP_H__

