#include "isapi_security.h"
#include "../Manager/UserManager.h"
#include "../Manager/LogManager.h"

namespace ISAPI
{
	namespace Security
	{
		void Capabilities_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			JsonElement jSecCap;
			jSecCap["maxUserNum"] = MAX_USER_NUM;
			jRespRoot["securityCap"] = jSecCap;

			WriteJsonResultSuccess(jRespRoot);
		}

		void Users_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			USER_INFO tAllUser[MAX_USER_NUM] = {{0}};
			int iCnt = UserManager::GetInstance()->GetAllEnableUser(tAllUser, MAX_USER_NUM);
			if( iCnt < 1 )
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE,jRespRoot);
			}
			else
			{
				JsonElement jUserArray;
				for (int i = 0; i < iCnt; i++)
				{
					JsonElement jUser;
					jUser["name"] = tAllUser[i].szName;
					jUser["remark"] = tAllUser[i].szRemark;
					jUser["userLevel"] = tAllUser[i].iRole;
					jUserArray[i] = jUser;
				}
				jRespRoot["userlist"] = jUserArray;
			
				WriteJsonResultSuccess(jRespRoot);
			}
		}

		void User_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			char szName[32] = {0};
			JsonElement jError;
			if (!QueryGetStringValue(tReqHead.szQuery, "name",szName,sizeof(szName),jError))
			{
				WriteJsonResultError(ISAPI_FAILED_PARAMS,jRespRoot);
				return;
			}
			
			USER_INFO tUser = {0};
			int iRet = UserManager::GetInstance()->FindUser(szName,tUser);
			if(iRet != 0)
			{
				char message[128] = {0};
				sprintf(message, "no user is %s", szName);
				WriteJsonResult(ISAPI_FAILED_NOCFG, message, jRespRoot);
			}
			else
			{
				JsonElement jUser;
				jUser["name"] = tUser.szName;
				if (RequestUserRole(tReqHead) == USER_ROLE_ADMIN)
				{
					jUser["password"] = tUser.szPass;//ֻ��Admin���ܲ�ѯ������
				}
				jUser["remark"] = tUser.szRemark;
				jUser["userLevel"] = tUser.iRole;
				jRespRoot["user"] = jUser;
				WriteJsonResultSuccess(jRespRoot);
			}
		}

		void User_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			JsonElement jUser;
			if(!JsonChildAsObj(jReqRoot, "user", jUser, jRespRoot))return;

			USER_INFO tUserInfo = {0};
			tUserInfo.enable = true;
			if(!JsonChildAsBuffer(jUser,"name",		tUserInfo.szName,  sizeof(tUserInfo.szName),  jRespRoot))return;
			if(!JsonChildAsBuffer(jUser,"password", tUserInfo.szPass,  sizeof(tUserInfo.szPass),  jRespRoot))return;
			if(!JsonChildAsBuffer(jUser,"remark",   tUserInfo.szRemark,sizeof(tUserInfo.szRemark),jRespRoot))return;
			if(!JsonChildAsIntWithBound(jUser,"userLevel", tUserInfo.iRole, USER_ROLE_ADMIN, USER_ROLE_VIEWER, jRespRoot))return;

			// �û���-���� ����Ϊ��
			if (strlen(tUserInfo.szName) < 1 || strlen(tUserInfo.szPass) < 1)
			{
				WriteJsonResultError(ISAPI_FAILED_PARAMS, jRespRoot);
				return;
			}

			USER_INFO tOldUser = {0};
			int iRet = UserManager::GetInstance()->FindUser(tUserInfo.szName, tOldUser);
			if(iRet != 0)
			{
				char message[128] = {0};
				sprintf(message, "no user is %s", tUserInfo.szName);
				WriteJsonResult(ISAPI_FAILED_PARAMS, message, jRespRoot);
				return;
			}
			
			if (RequestUserRole(tReqHead) == USER_ROLE_ADMIN)
			{
				// ����Ա�������޸������û������롢��������ɫ������Ա/�۲�Ա��
				if (strcmp(tUserInfo.szName, "admin") == 0)
				{
					// ����Ա�Ľ�ɫ���ܱ�
					if (tUserInfo.iRole != USER_ROLE_ADMIN)
					{
						WriteJsonResultError(ISAPI_FAILED_PARAMS, jRespRoot);
						return;
					}
				}
				else
				{
					// ����Ա/�۲�Ա�Ľ�ɫ���ܱ�ɹ���Ա
					if (tUserInfo.iRole == USER_ROLE_ADMIN)
					{
						WriteJsonResultError(ISAPI_FAILED_PARAMS, jRespRoot);
						return;
					}
				}
			}
			else
			{
				// ����Ա/�۲�Ա��ֻ�����޸��Լ������롢����
				char szUserName[32] = {0};
				char szPassword[32] = {0};
				AuthorizationParse(tReqHead.szAuth, szUserName, szPassword);
				if (strcmp(szUserName, tUserInfo.szName) != 0||
					tUserInfo.iRole != tOldUser.iRole)
				{
					WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
					return;
				}
			}

			iRet = UserManager::GetInstance()->UpdateUser(tUserInfo);
			if(iRet == 0)
			{
				WriteJsonResultSuccess(jRespRoot);

				WriteDbLog(tReqHead, OP_LOG_USR, "�޸��û�:%s",tUserInfo.szName);
			}
			else
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE,jRespRoot);
			}
		}

		void User_POST(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			/*����Ա�������½�����Ա/�۲�Ա�û��������½������û�����������maxUserNum	*/
			if (RequestUserRole(tReqHead) != USER_ROLE_ADMIN)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION, jRespRoot);
				return;
			}

			JsonElement jUser;
			if(!JsonChildAsObj(jReqRoot, "user", jUser, jRespRoot))return;

			USER_INFO tUserInfo = {0};
			tUserInfo.enable = true;
			if(!JsonChildAsBuffer(jUser,"name",		tUserInfo.szName,  sizeof(tUserInfo.szName),  jRespRoot))return;
			if(!JsonChildAsBuffer(jUser,"password", tUserInfo.szPass,  sizeof(tUserInfo.szPass),  jRespRoot))return;
			if(!JsonChildAsBuffer(jUser,"remark",   tUserInfo.szRemark,sizeof(tUserInfo.szRemark),jRespRoot))return;
			if(!JsonChildAsIntWithBound(jUser,"userLevel", tUserInfo.iRole, USER_ROLE_ADMIN, USER_ROLE_VIEWER, jRespRoot))return;

			if(!UserManager::GetInstance()->RuleCheck(tUserInfo))
			{
				WriteJsonResultError(ISAPI_FAILED_PARAMS, jRespRoot);
				return;
			}

			USER_INFO tFindUser;
			if(UserManager::GetInstance()->FindUser(tUserInfo.szName,tFindUser) == 0)
			{
				WriteJsonResultError(ISAPI_FAILED_EXISTING, jRespRoot);
				return;
			}

			int iRet = UserManager::GetInstance()->AddNewUser(tUserInfo);
			if(iRet == 0)
			{
				WriteJsonResultSuccess(jRespRoot);

				WriteDbLog(tReqHead, OP_LOG_USR, "�½��û�:%s",tUserInfo.szName);
			}
			else
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE,jRespRoot);
			}
		}

		void User_DELETE(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			/*����Ա������ɾ������Ա/�۲�Ա�û�*/
			if (RequestUserRole(tReqHead) != USER_ROLE_ADMIN)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION, jRespRoot);
				return;
			}

			char szName[32] = {0};
			JsonElement jError;
			if (!QueryGetStringValue(tReqHead.szQuery, "name",szName,sizeof(szName),jError))
			{
				WriteJsonResultError(ISAPI_FAILED_PARAMS,jRespRoot);
				return;
			}

			// admin����ɾ��
			if (strcmp(szName, "admin") == 0)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}
			
			int iRet = UserManager::GetInstance()->DeleteUser(szName);
			if(iRet == 0)
			{
				WriteJsonResultSuccess(jRespRoot);

				WriteDbLog(tReqHead, OP_LOG_USR, "ɾ���û�:%s",szName);
			}
			else
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE,jRespRoot);
			}
		}

		void UserCheck_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			JsonElement jUserCheck;
			if(!JsonChildAsObj(jReqRoot, "userCheck", jUserCheck, jRespRoot))return;

			char szName[32] = {0};
			char szPass[32] = {0};
			if(!JsonChildAsBuffer(jUserCheck,"name",	szName,  sizeof(szName),  jRespRoot))return;
			if(!JsonChildAsBuffer(jUserCheck,"password",szPass,  sizeof(szPass),  jRespRoot))return;
			
			int iRet = UserManager::GetInstance()->CheckUser(szName,szPass);
			if(iRet == 0)
			{
				WriteJsonResultSuccess(jRespRoot);
			}
			else if(iRet == -2)
			{
				WriteJsonResult(401,"no user",jRespRoot);
			}
			else if(iRet == -3)
			{
				WriteJsonResult(401,"wrong password",jRespRoot);
			}
			else
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE,jRespRoot);
			}
		}
	}
}

