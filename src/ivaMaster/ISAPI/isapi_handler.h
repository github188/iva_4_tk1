#ifndef __ISAPI_HANDLER_H__
#define __ISAPI_HANDLER_H__
#include "isapi_json.h"
#include "mq_master.h"
#include "oal_typedef.h"

typedef void (*HANDLE_FUNC)(const ISAPI_REQ & tReqHead, const JsonElement& jReqRoot, JsonElement& jRespRoot);

typedef struct _ISAPI_HANDLE
{
	const char *uri;
	HANDLE_FUNC get_handler;
	HANDLE_FUNC put_handler;
	HANDLE_FUNC post_handler;
	HANDLE_FUNC delete_handler;
} ISAPI_HANDLE;

typedef struct _ISAPI_NOAuthorization
{
	const char *uri;
	bool bGet;
	bool bPut;
	bool bPost;
	bool bDelete;
} ISAPI_NOAuthorization;

ISAPI_HANDLE * GetISAPIHandle(const char * uri);

HANDLE_FUNC GetISAPIHandleFunc(ISAPI_HANDLE * handle, int method);

const char * GetISAPIMethod(int method);

bool IsNeedCheckAuthorization(const char * uri, int method);
bool AuthorizationParse(const char * content, char *name, char *password);
bool QueryGetStringValue(const char* content,const char* key, char * value, int len, JsonElement &jError);
bool QueryGetNumberValue(const char* content,const char* key, int & value, JsonElement &jError);
bool QueryGetNumberValueWithBound(const char* content,const char* key, int & value, int nLower, int nUpper, JsonElement &jError);
int  RequestUserRole(const ISAPI_REQ & tReqHead);

int WriteDbLog(const ISAPI_REQ & tReqHead, int iType, const char *format, ...);
int WriteAlarm(const char *format, ...);
int File2HttpUrl(const char * file_path, char *url, bool bUploaded);

#endif // __ISAPI_HANDLER_H__
