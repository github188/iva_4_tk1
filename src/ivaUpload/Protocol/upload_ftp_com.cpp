#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "oal_log.h"
#include "oal_file.h"
#include "oal_unicode.h"
#include "ftp.h"
#include "xml_com.h"
#include "upload_ftp_com.h"
#include "../UploadMan.h"


ProtocolHandle UploadByFtpCommon::m_pInstance = NULL;

ProtocolHandle UploadByFtpCommon::GetInstance()
{
	if(m_pInstance == NULL)
	{
		m_pInstance =  new UploadByFtpCommon();
		if (m_pInstance)
		{
			m_pInstance->InitSever();
		}
	}

	return m_pInstance;
}

int UploadByFtpCommon::InitSever()
{
	return 0;
}

UploadByFtpCommon::UploadByFtpCommon(void)
{
}

UploadByFtpCommon::~UploadByFtpCommon(void)
{
}
	
int UploadByFtpCommon::UploadData(const AnalyDbRecord *pData)
{
	int ret = 0;
	if(NULL == pData)
	{
		LOG_DEBUG_FMT("input error!");
		return -1;
	}

	if(pData->iImgNum < 1)
	{
		LOG_DEBUG_FMT("no Pic1!");
		return 0;
	}

	
	// 获得上传服务器地址
	FtpSvr tFTP = {0};
	int iRet = UploadMan::GetInstance()->GetUploadServerFtp(&tFTP);
	if(iRet != 0 || strlen(tFTP.szAddr) < 1)
	{
		return -1;
	}

	// 远程目录
	char dst_dir_grammar_gbk[512] = {0};
	UploadMan::GetInstance()->GetFtpDirGrammar(pData->iDataType, dst_dir_grammar_gbk);
	char dst_dir_grammar_utf8[512] = {0};
	GB2312_To_UTF8(dst_dir_grammar_gbk, strlen(dst_dir_grammar_gbk), dst_dir_grammar_utf8, sizeof(dst_dir_grammar_utf8));
	char dst_dir_name_utf8[512] = {0};
	FtpSvrSubDir(pData, dst_dir_grammar_utf8, dst_dir_name_utf8);
	char dst_dir_name_gbk[512] = {0};
	UTF8_To_GB2312(dst_dir_name_utf8, strlen(dst_dir_name_utf8), dst_dir_name_gbk, sizeof(dst_dir_name_gbk));

	bool bUseUft8 = UploadMan::GetInstance()->FtpUploadFileCharsetByUtf8();
	bool bNeedUploadText = UploadMan::GetInstance()->FtpEnableUploadFile(FTP_FLAG_XML);
	bool bNeedUploadImage = UploadMan::GetInstance()->FtpEnableUploadFile(FTP_FLAG_IMG);
	bool bNeedUploadRec = UploadMan::GetInstance()->FtpEnableUploadFile(FTP_FLAG_REC);

	char szSrcFileUtf8[512] = {0};
	char szSrcFileGbk[512] = {0};
	char szDstFileUtf8[512] = {0};
	char szDstFileGbk[512] = {0};

	// 上传文本文件
	if (bNeedUploadText)
	{
		sprintf(szSrcFileUtf8, "%s%s", pData->szImgRoot, pData->szImgPath[0]);
		sprintf(szSrcFileUtf8+(strlen(szSrcFileUtf8)-3), "xml");
		UTF8_To_GB2312(szSrcFileUtf8, strlen(szSrcFileUtf8), szSrcFileGbk, sizeof(szSrcFileGbk));

		strcpy(szDstFileUtf8, pData->szImgPath[0]);
		sprintf(szDstFileUtf8+(strlen(szDstFileUtf8)-3), "xml");
		UTF8_To_GB2312(szDstFileUtf8, strlen(szDstFileUtf8), szDstFileGbk, sizeof(szDstFileGbk));

		if(access(szSrcFileGbk,0))
		{
			if (create_xml_file(szSrcFileGbk, pData) != 0)
			{
				LOG_DEBUG_FMT("create_xml_file %s ret = %d!!!", szSrcFileGbk);
				goto UPLOAD_ERR;
			}

			if(access(szSrcFileGbk,0))
			{
				ret = FTP_ERROR_FILE;
				LOG_DEBUG_FMT("%s is missing!", szSrcFileGbk);
				goto UPLOAD_ERR;
			}
		}

		ret = FTP_UploadFileEntire(tFTP.szAddr, tFTP.iPort, tFTP.szUser, tFTP.szPass,
			bUseUft8?dst_dir_name_utf8:dst_dir_name_gbk, bUseUft8?szDstFileUtf8:szDstFileGbk, szSrcFileGbk);

		unlink(szSrcFileGbk);

		if(0 != ret)
		{
			LOG_DEBUG_FMT("FTP_UploadFileEntire %s:%d@%s:%s, %s->%s/%s ret = %d!!!",tFTP.szAddr, tFTP.iPort, tFTP.szUser, tFTP.szPass,
				szSrcFileGbk, dst_dir_name_gbk, szDstFileGbk, ret);
			goto UPLOAD_ERR;
		}
	}

	// 上传图片文件
	for (int i = 0; i < pData->iImgNum && bNeedUploadImage; i++)
	{
		sprintf(szSrcFileUtf8, "%s%s", pData->szImgRoot, pData->szImgPath[i]);
		UTF8_To_GB2312(szSrcFileUtf8, strlen(szSrcFileUtf8), szSrcFileGbk, sizeof(szSrcFileGbk));

		strcpy(szDstFileUtf8, pData->szImgPath[i]);
		UTF8_To_GB2312(szDstFileUtf8, strlen(szDstFileUtf8), szDstFileGbk, sizeof(szDstFileGbk));

		if(access(szSrcFileGbk,0))
		{
			ret = FTP_ERROR_FILE;
			LOG_DEBUG_FMT("%s is missing!", szSrcFileGbk);
			goto UPLOAD_ERR;
		}

		ret = FTP_UploadFileEntire(tFTP.szAddr, tFTP.iPort, tFTP.szUser, tFTP.szPass,
			bUseUft8?dst_dir_name_utf8:dst_dir_name_gbk, bUseUft8?szDstFileUtf8:szDstFileGbk, szSrcFileGbk);

		if(0 != ret)
		{
			LOG_DEBUG_FMT("FTP_UploadFileEntire %s:%d@%s:%s, %s->%s/%s ret = %d!!!",tFTP.szAddr, tFTP.iPort, tFTP.szUser, tFTP.szPass,
				szSrcFileGbk, dst_dir_name_gbk, szDstFileGbk, ret);
			goto UPLOAD_ERR;
		}
	}

	// 上传录像文件
	for (int i = 0; i < pData->iRecNum && bNeedUploadRec; i++)
	{
		sprintf(szSrcFileUtf8, "%s%s", pData->szRecRoot, pData->szRecPath[i]);
		UTF8_To_GB2312(szSrcFileUtf8, strlen(szSrcFileUtf8), szSrcFileGbk, sizeof(szSrcFileGbk));

		strcpy(szDstFileUtf8, pData->szRecPath[i]);
		UTF8_To_GB2312(szDstFileUtf8, strlen(szDstFileUtf8), szDstFileGbk, sizeof(szDstFileGbk));

		if(access(szSrcFileGbk,0))
		{
			ret = FTP_ERROR_FILE;
			LOG_DEBUG_FMT("%s is missing!", szSrcFileGbk);
			goto UPLOAD_ERR;
		}

		ret = FTP_UploadFileEntire(tFTP.szAddr, tFTP.iPort, tFTP.szUser, tFTP.szPass,
			bUseUft8?dst_dir_name_utf8:dst_dir_name_gbk, bUseUft8?szDstFileUtf8:szDstFileGbk, szSrcFileGbk);

		if(0 != ret)
		{
			LOG_DEBUG_FMT("FTP_UploadFileEntire %s:%d@%s:%s, %s->%s/%s ret = %d!!!",tFTP.szAddr, tFTP.iPort, tFTP.szUser, tFTP.szPass,
				szSrcFileGbk, dst_dir_name_gbk, szDstFileGbk, ret);
			goto UPLOAD_ERR;
		}
	}

UPLOAD_ERR:

	if(ret == FTP_SUCCESS)
		return UPLOAD_RET_OK;
	else if(ret == FTP_ERROR_FILE)
		return UPLOAD_RET_NO_FILE;
	else
		return UPLOAD_RET_ERROR;
}

int UploadByFtpCommon::UploadAlarm( const AlarmDbRecord *pData )
{
	return 0;
}

bool UploadByFtpCommon::CanUploadData()
{
	return true;
}

bool UploadByFtpCommon::CanUploadAlarm()
{
	return false;
}

bool UploadByFtpCommon::CanRemoveFile()
{
	return true;
}

int FtpUploadOneFile(FtpSvr * pFtpSvr, char * szSrcFileUtf8, char * szDirNameUtf8, char *szDstFileUtf8)
{
	bool bUseUft8 = UploadMan::GetInstance()->FtpUploadFileCharsetByUtf8();

	char szDirNameGbk[512] = {0};
	char szSrcFileGbk[512] = {0};
	char szDstFileGbk[512] = {0};
	UTF8_To_GB2312(szDirNameUtf8, strlen(szDirNameUtf8), szDirNameGbk, sizeof(szDirNameGbk));
	UTF8_To_GB2312(szSrcFileUtf8, strlen(szSrcFileUtf8), szSrcFileGbk, sizeof(szSrcFileGbk));
	UTF8_To_GB2312(szDstFileUtf8, strlen(szDstFileUtf8), szDstFileGbk, sizeof(szDstFileGbk));

	return FTP_UploadFileEntire(pFtpSvr->szAddr, pFtpSvr->iPort, pFtpSvr->szUser, pFtpSvr->szPass,
		bUseUft8?szDirNameUtf8:szDirNameGbk, 
		bUseUft8?szDstFileUtf8:szDstFileGbk, 
		szSrcFileGbk);
}
