#ifndef __USER_MANAGER_H__
#define __USER_MANAGER_H__
#include <string>
#include "pthread.h"
using namespace std;

#define MAX_USER_NUM	125	// ����û�����

#define USER_ROLE_ADMIN			0
#define USER_ROLE_OPERATOR		1
#define USER_ROLE_VIEWER		2

//�����û���Ϣ
typedef struct _tagUserInfo
{
	bool enable;		  // ���
	char szName[32];      // �û���
	char szPass[32];      // �û�����
	char szRemark[64];    // �û���ע
	int  iRole;			  // 0-admin 1-operater 2-viewer
}USER_INFO;


class UserManager
{
public:
	static UserManager* GetInstance();
	static int Initialize();
	static void UnInitialize();

	int CheckUser( const char * szName, const char *szPassWord );

	int FindUser( const char * szName, USER_INFO& stUserInfo );
	bool RuleCheck( USER_INFO& stUserInfo );
	int AddNewUser( const USER_INFO& stUserInfo );
	int DeleteUser( const char * szName );
	int UpdateUser( const USER_INFO& stUserInfo );             
	int GetAllEnableUser( USER_INFO *pstUserInfo, int iCnt );
private:
	static UserManager* m_pInstance;

private:
	UserManager();
	~UserManager();

public:
	int InitUser(int iUserID, const USER_INFO& stUserInfo);             

private:
	USER_INFO m_AllUserInfo[MAX_USER_NUM];
	pthread_mutex_t  m_mutex;
};

#endif//__USER_MANAGER_H__
