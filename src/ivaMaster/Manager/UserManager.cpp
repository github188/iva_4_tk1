#include "UserManager.h"
#include "oal_log.h"
#include "../Profile/SystemProfile.h"
#include "oal_base64.h"


UserManager* UserManager::m_pInstance = NULL;

UserManager::UserManager()
{
	pthread_mutex_init(&m_mutex, NULL);
	pthread_mutex_lock(&m_mutex);
	memset(m_AllUserInfo, 0, sizeof(USER_INFO)*MAX_USER_NUM);
	pthread_mutex_unlock(&m_mutex);
}

UserManager::~UserManager()
{
	pthread_mutex_lock(&m_mutex);
	pthread_mutex_unlock(&m_mutex);
	pthread_mutex_destroy(&m_mutex);
}

UserManager* UserManager::GetInstance()
{
	return m_pInstance;
}

int UserManager::Initialize()
{
	int iRet = -1;

	if(NULL == m_pInstance)
	{
		m_pInstance = new UserManager();
	}

	bool bAdmin = false;
	for(int i = 0; i < MAX_USER_NUM; i++)
	{
		USER_INFO tUser = {0};
		iRet = SystemProfile::GetInstance()->GetUserInfo(i, &tUser);
		if(iRet == 0)
		{
			if(tUser.enable)
			{
				USER_INFO tFindUser;
				if(m_pInstance->FindUser(tUser.szName,tFindUser) == 0)
				{
					LOG_DEBUG_FMT("User %s is exist");
					continue;;
				}

				if(m_pInstance->RuleCheck(tUser))
				{
					if(strcmp(tUser.szName, "admin") == 0)
					{
						bAdmin = true;
					}

					m_pInstance->InitUser(i, tUser);
				}
			}		
		}
	}

	if(bAdmin == false)
	{
		USER_INFO tAdmin = {0};
		tAdmin.enable = true;
		strcpy(tAdmin.szName,"admin");
		Base64encode(tAdmin.szPass, "admin",strlen("admin"));
		//strcpy(tAdmin.szPass,"YWRtaW4");//admin的base64
		tAdmin.iRole = USER_ROLE_ADMIN;
		iRet = m_pInstance->AddNewUser(tAdmin);
		if (iRet != 0)
		{
			LOG_ERROR_FMT("Add admin user failed");
			return -1;
		}
	}

	return 0;
}

void UserManager::UnInitialize()
{
	if(m_pInstance)
	{
		delete m_pInstance;
		m_pInstance=NULL;
	}
}

int UserManager::InitUser( int iUserID, const USER_INFO& stUserInfo )
{
	if (iUserID < 0 || iUserID > MAX_USER_NUM-1)
	{
		return -1;
	}
	pthread_mutex_lock(&m_mutex);
	memcpy(m_AllUserInfo+iUserID, &stUserInfo, sizeof(USER_INFO));
	pthread_mutex_unlock(&m_mutex);
	return 0;
}

int UserManager::CheckUser( const char * szName, const char *szPassWord )
{
	if (szName == NULL || szPassWord == NULL)
	{
		return -1;
	}

	USER_INFO tUserInfo = {0};
	int iRet = FindUser(szName, tUserInfo);
	if (iRet != 0)
	{
		LOG_DEBUG_FMT("No User:%s",szName);
		return -2;
	}

	if(strcmp(szPassWord, tUserInfo.szPass) == 0)
	{
		return 0;
	}
	else
	{
		LOG_DEBUG_FMT("User:%s Password is error",szName);
		return -3;
	}
}

int UserManager::FindUser(const char * szName, USER_INFO& stUserInfo)
{
	if(szName == NULL)
	{
		return -1;
	}

	bool bFind = false;
	pthread_mutex_lock(&m_mutex);
	for(int i = 0; i < MAX_USER_NUM; i++)
	{
		if(m_AllUserInfo[i].enable)
		{
			if (strcmp(szName, m_AllUserInfo[i].szName)==0)
			{
				memcpy(&stUserInfo, m_AllUserInfo+i, sizeof(USER_INFO));
				bFind = true;
				break;
			}
		}
	}
	pthread_mutex_unlock(&m_mutex);
	return bFind?0:-1;
}

bool UserManager::RuleCheck(USER_INFO& stUserInfo)
{
	if (stUserInfo.enable == false ||
		strlen(stUserInfo.szName) < 1 ||
		strlen(stUserInfo.szPass) < 1)
	{
		return false;
	}

	if(strcmp(stUserInfo.szName, "admin") == 0)
	{
		if(stUserInfo.iRole != USER_ROLE_ADMIN)
		{
			stUserInfo.iRole = USER_ROLE_ADMIN;// admin必须为ADMIN
		}
	}
	else
	{
		if (stUserInfo.iRole == USER_ROLE_ADMIN)
		{
			stUserInfo.iRole = USER_ROLE_VIEWER;// 只有admin必须为ADMIN
		}
	}
	return true;
}

int UserManager::AddNewUser(const USER_INFO & stUserInfo)
{
	bool bFind = false;
	pthread_mutex_lock(&m_mutex);
	for(int i = 0; i < MAX_USER_NUM; i++)
	{
		if(m_AllUserInfo[i].enable == false)
		{
			int iRet = SystemProfile::GetInstance()->SetUserInfo(i, &stUserInfo);
			if(iRet == 0)
			{
				memcpy(m_AllUserInfo+i, &stUserInfo, sizeof(USER_INFO));
				bFind = true;
			}
			break;			
		}
	}
	pthread_mutex_unlock(&m_mutex);
	
	return bFind?0:-1;
}

int UserManager::DeleteUser(const char * szName)
{
	if(szName == NULL)
	{
		return -1;
	}

	bool bFind = false;
	pthread_mutex_lock(&m_mutex);
	for(int i = 0; i < MAX_USER_NUM; i++)
	{
		if(strcmp(m_AllUserInfo[i].szName,szName) == 0)
		{
			m_AllUserInfo[i].enable = false;
			int iRet = SystemProfile::GetInstance()->SetUserInfo(i, m_AllUserInfo+i);
			if(iRet == 0)
			{
				bFind = true;
			}
			break;			
		}
	}
	pthread_mutex_unlock(&m_mutex);
	return bFind?0:-1;
}

int UserManager::UpdateUser(const USER_INFO& stUserInfo)
{
	bool bFind = false;
	pthread_mutex_lock(&m_mutex);
	for(int i = 0; i < MAX_USER_NUM; i++)
	{
		if(strcmp(m_AllUserInfo[i].szName,stUserInfo.szName) == 0)
		{
			int iRet = SystemProfile::GetInstance()->SetUserInfo(i, &stUserInfo);
			if(iRet == 0)
			{
				memcpy(m_AllUserInfo+i, &stUserInfo, sizeof(USER_INFO));
				bFind = true;
			}
			break;			
		}
	}
	pthread_mutex_unlock(&m_mutex);
	return bFind?0:-1;
}

int UserManager::GetAllEnableUser( USER_INFO *pstUserInfo, int iCnt )
{
	int iFind = 0;
	pthread_mutex_lock(&m_mutex);
	for(int i = 0; i < MAX_USER_NUM && iFind < iCnt; i++)
	{
		if(m_AllUserInfo[i].enable)
		{
			(pstUserInfo+iFind)->enable = true;
			(pstUserInfo+iFind)->iRole = m_AllUserInfo[i].iRole;
			strcpy((pstUserInfo+iFind)->szName,m_AllUserInfo[i].szName);
			strcpy((pstUserInfo+iFind)->szPass,m_AllUserInfo[i].szPass);
			strcpy((pstUserInfo+iFind)->szRemark,m_AllUserInfo[i].szRemark);
			iFind++;
		}
	}
	pthread_mutex_unlock(&m_mutex);
	return iFind;
}
