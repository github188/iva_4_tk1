#include "Profile4Ini.h"
#include "oal_typedef.h"
#ifndef WIN32
#include <unistd.h>
#endif
#include "oal_log.h"
#include "pthread.h"
#include "oal_file.h"

string & string_trim_left(string & str)
{
	if (str.empty())
	{
		return str;
	}
	str.erase(0, str.find_first_not_of(" "));
	str.erase(0, str.find_first_not_of("\t"));
	str.erase(0, str.find_first_not_of("\r"));
	str.erase(0, str.find_first_not_of("\n"));
	return str;  
}

string & string_trim_right(string & str)
{
	if (str.empty())   
	{  
		return str;  
	}  
	str.erase(str.find_last_not_of(" ") + 1);  
	str.erase(str.find_last_not_of("\t") + 1);  
	str.erase(str.find_last_not_of("\r") + 1);  
	str.erase(str.find_last_not_of("\n") + 1);  
	return str;  
}

string & string_trim(string & str)
{
	string_trim_left(str);
	string_trim_right(str);
	return str;  
}

CProfileIni::CProfileIni(const string& strDir,const string& strFileName): CBaseProfile()
{
	m_strDir = strDir;
	m_strFileName = strFileName;
}

int CProfileIni::DoReadProfile()
{
	string strFile = "";
	string strFile_temp = "";
	string strPrefix = "~";

	strFile = m_strDir + m_strFileName;
	strFile_temp = m_strDir + strPrefix + m_strFileName;

	
	//检查配置文件是否存在,若配置文件不存在，查看相应的临时配置是否存在，若存在，则采用临时文件替换
	if(access(strFile.c_str(),R_OK|W_OK)!=0)
	{
		if(access(strFile_temp.c_str(),R_OK|W_OK)!=0)
		{
			LOG_ERROR_FMT("Config file %s not exist\n",strFile.c_str());
			return 0;
		}
		rename(strFile_temp.c_str(),strFile.c_str());
	}

	m_cfgCache.clear();

	FILE *fp = fopen(strFile.c_str(),"r");
	if(NULL == fp)
	{
		LOG_ERROR_FMT("Open %s failed,read config to cache failed",strFile.c_str());
		return -1;
	}

	printf("read config %s to cache\n",strFile.c_str());

	string strSegment = "unknown";// 默认分组

	while (1)
	{
		// 从文件读出一行
		char szBuffer[1024] = {0};
		char* pBuffer = fgets(szBuffer, sizeof(szBuffer), fp);
		if(pBuffer == NULL)
		{
			break;
		}

		// 去除首尾空格、tab、换行
		string content = szBuffer;
		string_trim(content);

		// 忽略空行
		if(content.empty())
		{
			continue;
		}

		// 忽略注释行#;
		if ('#' == content[0] || ';' == content[0])
		{
			continue;
		}

		// 查找[segment]
		if ('[' == content[0])
		{
			string::size_type pos = content.find_first_of(']');
			if(pos != string::npos)
			{
				strSegment = content.substr(1, pos-1);
				string_trim(strSegment);
				//printf("[%s]\n",strSegment.c_str());
			}
		}

		// 查找key=value
		string::size_type pos = content.find_first_of('=');
		if(pos != string::npos)
		{
			string strKey   = content.substr(0, pos);
			string strValue = content.substr(pos+1);
			
			string_trim(strKey);
			string_trim(strValue);
			
			//printf("%s=%s\n",strKey.c_str(), strValue.c_str());

			SegmentMapIter iter = m_cfgCache.find(strSegment);
			if(iter != m_cfgCache.end())
			{
				iter->second.push_back(make_pair(strKey, strValue));
			}
			else
			{
				KeyValueList listCache;
				listCache.clear();
				listCache.push_back(make_pair(strKey, strValue));
				m_cfgCache.insert(SegmentMap::value_type(strSegment,listCache));
			}
		}
	}
	
	fclose(fp);
	return 0;
}

int CProfileIni::DoWriteProfile()
{
	string strFile = m_strDir + m_strFileName;

	OAL_MakeDirByPath(strFile.c_str());

	FILE* fp = fopen(strFile.c_str(), "w");
	if (NULL == fp)
	{
		LOG_ERROR_FMT("fopen file (\"%s\") fail, error=(%d, \"%s\")", strFile.c_str(), errno, strerror(errno));
		return -1;
	}

	string strContent = "";

	SegmentMapIter smIter = m_cfgCache.begin();
	for(; smIter != m_cfgCache.end(); smIter++)
	{
		if(smIter->second.size()<=0)
		{
			continue;
		}
		
		strContent += "[" + smIter->first+ "]\n";
		
		KeyValueListIter kvIter = smIter->second.begin();
		for (;kvIter != smIter->second.end(); ++kvIter)
		{
			strContent += kvIter->first+ " = "+kvIter->second + "\n";
		}
		
		strContent += "\n";
	}
	
	fseek(fp,0,SEEK_SET);
	fputs(strContent.c_str(),fp);
	fflush(fp);
	fclose(fp);
	return 0;
}

int CProfileIni::DoDeleteProfile()
{
	string strFile = m_strDir+m_strFileName;
#ifdef WIN32
	_unlink(strFile.c_str());
#else
	unlink(strFile.c_str());
#endif
	return 0;
}

const char *iniGetString(const char *title, const char *key, const char *defaultValue, const char *fileName)
{
	CProfileIni tProfile(fileName,"");
	if(tProfile.ReadProfile() != 0)
	{
		return defaultValue;
	}

	static char sbuffer[1024] = {0};
	memset(sbuffer, 0, sizeof(sbuffer));
	if(tProfile.GetProfileValueString(title, key, sbuffer, defaultValue)!=0)
	{
		return defaultValue;
	}

	return sbuffer;
}

int iniGetInt(const char *title, const char *key, int defaultValue, const char *fileName)
{
	CProfileIni tProfile(fileName,"");
	if(tProfile.ReadProfile() != 0)
	{
		return defaultValue;
	}

	int iRet = 0;
	if(tProfile.GetProfileValueNumber(title, key, iRet, defaultValue)!=0)
	{
		return defaultValue;
	}

	return iRet;
}


