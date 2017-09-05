#ifndef __MQ_DATABASE_H__
#define __MQ_DATABASE_H__

#include "mq_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
	DATABASE_CMD_START = DATABASE_CMD,
	DATABASE_CMD_INSERT_DELETE_UPDATE,
	DATABASE_CMD_SELECT,
	DATABASE_CMD_SELECT_COUNT,
	DATABASE_MSG_END
};

typedef struct
{
	int iRow;
	int iCol;
	int iLen;
	unsigned char data[8*1024];
}DbQueryResult;

enum
{
	eUnUp  = 0, // 还未上传
	eUpOk  = 1, // 已上传
	eUpErr = 2  // 无法上传
};

MQ_API int MQ_Database_Init();
MQ_API int MQ_DataBase_Close();
MQ_API int MQ_DataBase_Update(long lSrc, const char *sql);
MQ_API int MQ_DataBase_Insert(long lSrc, const char *sql);
MQ_API int MQ_DataBase_Delete(long lSrc, const char *sql);
MQ_API int MQ_DataBase_Select_Count(long lSrc, const char *sql);
MQ_API int MQ_DataBase_Select(long lSrc, const char *sql, DbQueryResult *ptResult);

MQ_API int MQ_DataBase_Answer_Count(long lDes, int iCnt);
MQ_API int MQ_DataBase_Answer_Select(long lDes, const DbQueryResult *ptResult);

#ifdef __cplusplus
};
#endif

#endif//__MQ_DATABASE_H__
