#ifndef __SOFT_AUTHORIZATION_MANAGE__
#define __SOFT_AUTHORIZATION_MANAGE__
#include <time.h>
#include "mq_analysis.h"

typedef struct _SoftAuth
{
	char szLicense[40]; // 授权码
	AuthStatusT tStatus;
}SoftAuth;

class SoftAuthManager
{
public:
	static SoftAuthManager* GetInstance();
	static int Initialize();
	static void UnInitialize();

	// 进行授权
	int DoAuthorization(const char * pstrLicense);

	// 获取授权详细信息
	int GetAuthorization(SoftAuth * ptInfo);

	// 删除授权
	int DelAuthorization();
	
private:

	// 解密LICENSE
	int DecodeLicense(const char * pstrLicense, AuthStatusT * ptInfo);

	// 读取文件
	int ReadFile(const char * pstrFilePath, unsigned char * buffer);

	// 保存LICENSE文件
	int SaveFile(const char * pstrFilePath, const unsigned char * buffer, int flen);

	// 读取本机MAC地址
	int ReadMacAddr(unsigned char * pu8Mac);

private:
    SoftAuthManager();	
	~SoftAuthManager();
    static SoftAuthManager* m_pInstance;
	SoftAuth m_tSAInfo;
};
#endif // __SOFT_AUTHORIZATION_MANAGE__

