#ifndef __SOFT_AUTHORIZATION_MANAGE__
#define __SOFT_AUTHORIZATION_MANAGE__
#include <time.h>
#include "mq_analysis.h"

typedef struct _SoftAuth
{
	char szLicense[40]; // ��Ȩ��
	AuthStatusT tStatus;
}SoftAuth;

class SoftAuthManager
{
public:
	static SoftAuthManager* GetInstance();
	static int Initialize();
	static void UnInitialize();

	// ������Ȩ
	int DoAuthorization(const char * pstrLicense);

	// ��ȡ��Ȩ��ϸ��Ϣ
	int GetAuthorization(SoftAuth * ptInfo);

	// ɾ����Ȩ
	int DelAuthorization();
	
private:

	// ����LICENSE
	int DecodeLicense(const char * pstrLicense, AuthStatusT * ptInfo);

	// ��ȡ�ļ�
	int ReadFile(const char * pstrFilePath, unsigned char * buffer);

	// ����LICENSE�ļ�
	int SaveFile(const char * pstrFilePath, const unsigned char * buffer, int flen);

	// ��ȡ����MAC��ַ
	int ReadMacAddr(unsigned char * pu8Mac);

private:
    SoftAuthManager();	
	~SoftAuthManager();
    static SoftAuthManager* m_pInstance;
	SoftAuth m_tSAInfo;
};
#endif // __SOFT_AUTHORIZATION_MANAGE__

