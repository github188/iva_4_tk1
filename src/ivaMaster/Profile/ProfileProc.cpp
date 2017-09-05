#include "ProfileProc.h"
#include <sstream>
using namespace std;

CBaseProfile::CBaseProfile()
{
	m_cfgCache.clear();
	int ret = pthread_rwlock_init(&m_rwlock,NULL);
}

CBaseProfile::~CBaseProfile()
{
	pthread_rwlock_wrlock(&m_rwlock);
	m_cfgCache.clear();
	pthread_rwlock_unlock(&m_rwlock);
	pthread_rwlock_destroy(&m_rwlock);
}

int CBaseProfile::ReadProfile()
{
	int iRet = 0;
	pthread_rwlock_wrlock(&m_rwlock);
	m_cfgCache.clear();
	iRet = DoReadProfile();
	pthread_rwlock_unlock(&m_rwlock);   
	return iRet;
}

int CBaseProfile::WriteProfile()
{
	int iRet = 0;
	pthread_rwlock_wrlock(&m_rwlock);
	iRet = DoWriteProfile();
	pthread_rwlock_unlock(&m_rwlock);   
	return iRet;
}

int CBaseProfile::DeleteProfile()
{
	int iRet = 0;
	pthread_rwlock_wrlock(&m_rwlock);
	m_cfgCache.clear();
	iRet = DoDeleteProfile();
	pthread_rwlock_unlock(&m_rwlock);   
	return iRet;
}

int CBaseProfile::GetProfileValue(string segment, string key, string & value, string defualt)
{
	int iRet = CFG_MISS_SEGMENT;
	
	pthread_rwlock_rdlock(&m_rwlock);

	for (SegmentMapIter smIter = m_cfgCache.begin(); smIter != m_cfgCache.end(); ++smIter)
	{
		if(smIter->first == segment)
		{
			iRet = CFG_MISS_KEY;
			for (KeyValueListIter kvIter = smIter->second.begin(); kvIter != smIter->second.end(); ++kvIter)
			{
				if(kvIter->first == key)
				{
					iRet = 0;
					value = kvIter->second;
					//printf("Read [%s]->[%s] = %s\n", segment.c_str(), key.c_str(), value.c_str());
					break;
				}
			}

			break;
		}
	}

	pthread_rwlock_unlock(&m_rwlock);

	if(iRet != 0)
	{
		value = defualt;
		//printf("Read [%s]->[%s] = %s (Defualt)\n", segment.c_str(), key.c_str(), value.c_str());
	}

	return iRet;
}

int CBaseProfile::SetProfileValue(string segment, string key, string value)
{
	pthread_rwlock_wrlock(&m_rwlock);
	
	// SEGMENT
	SegmentMapIter smIter = m_cfgCache.begin();
	for (; smIter != m_cfgCache.end(); ++smIter)
	{
		if(smIter->first == segment)
		{
			break;
		}
	}

	// 没有对应的Segment
	if(smIter == m_cfgCache.end())
	{
		KeyValueList listCache;
		listCache.clear();
		m_cfgCache.insert(SegmentMap::value_type(segment,listCache));
		
		for (smIter = m_cfgCache.begin(); smIter != m_cfgCache.end(); ++smIter)
		{
			if(smIter->first == segment)
			{
				break;
			}
		}
	}

	KeyValueListIter kvIter = smIter->second.begin();
	for (; kvIter != smIter->second.end(); ++kvIter)
	{
		if(kvIter->first == key)
		{
			kvIter->second = value;
			break;
		}
	}

	if(kvIter == smIter->second.end())
	{
		smIter->second.push_back(make_pair(key, value));
	}
	
	//printf("Write [%s]->[%s] = %s success\n",segment.c_str(),key.c_str(), value.c_str());

	pthread_rwlock_unlock(&m_rwlock);

	return 0;
}

int CBaseProfile::GetProfileValueString(const char * segment, const char * key, char *	value, const char * defualt)
{
	if(segment == NULL || key == NULL || value == NULL)
	{
		return -1;
	}
	
	string strSegment = segment;
	string strKey = key;
	string strValue = "";
	string strDefualt = defualt ? defualt : "";
	
	int ret = GetProfileValue(strSegment, strKey, strValue, strDefualt);

	strcpy(value, strValue.c_str());
	
	return ret;
}

int CBaseProfile::SetProfileValueString(const char * segment, const char * key, const char *  value)
{
	if(segment == NULL || key == NULL || value == NULL)
	{
		return -1;
	}
	
	string strSegment = segment;
	string strKey = key;
	string strValue =value;
	return SetProfileValue(segment, key, strValue);
}

int CBaseProfile::DelProfileSegment(const char * segment)
{
	if(segment == NULL)
	{
		return -1;
	}
	
	
	pthread_rwlock_wrlock(&m_rwlock);
	
	// SEGMENT
	for (SegmentMapIter smIter = m_cfgCache.begin(); smIter != m_cfgCache.end(); ++smIter)
	{
		if(smIter->first == segment)
		{
			//printf("Delete [%s] success\n",segment);
			m_cfgCache.erase(smIter);
			break;
		}
	}

	pthread_rwlock_unlock(&m_rwlock);

	return 0;
}
	
int CBaseProfile::DelProfileSegmentKey(const char * segment, const char * key)
{
	if(segment == NULL || key == NULL)
	{
		return -1;
	}
	
	pthread_rwlock_wrlock(&m_rwlock);
	
	// SEGMENT
	SegmentMapIter smIter = m_cfgCache.begin();
	for (; smIter != m_cfgCache.end(); ++smIter)
	{
		if(smIter->first == segment)
		{
			break;
		}
	}

	// 没有对应的Segment
	if(smIter == m_cfgCache.end())
	{
		return 0;
	}

	KeyValueListIter kvIter = smIter->second.begin();
	for (; kvIter != smIter->second.end(); ++kvIter)
	{
		if(kvIter->first == key)
		{
			smIter->second.erase(kvIter);
			//printf("Delete [%s]->[%s] success\n",segment,key);
			break;
		}
	}

	pthread_rwlock_unlock(&m_rwlock);

	return 0;
}

int CBaseProfile::GetProfileValueNumber(const char * segment, const char * key, s8 & value, s8 defualt)
{
	s32 s32Vaue = 0;
	s32 s32Defualt = (s32)defualt;
	int ret = GetProfileValueNumber(segment, key, s32Vaue, s32Defualt);
	value = (s8)(s32Vaue);
	return ret;
}

int CBaseProfile::GetProfileValueNumber(const char * segment, const char * key, s16 & value, s16 defualt)
{
	s32 s32Vaue = 0;
	s32 s32Defualt = (s32)defualt;
	int ret = GetProfileValueNumber(segment, key, s32Vaue, s32Defualt);
	value = (s16)(s32Vaue);
	return ret;
}

int CBaseProfile::GetProfileValueNumber(const char * segment, const char * key, s32 & value, s32 defualt)
{
	if(segment == NULL || key == NULL)
	{
		return -1;
	}

	stringstream ss;
	ss.clear();
	ss << defualt;
	
	string strSegment = segment;
	string strKey	  = key;
	string strValue   = "";
	string strDefualt = ss.str();

	int ret = GetProfileValue(strSegment, strKey, strValue, strDefualt);

	value = atoi(strValue.c_str());
	
	return ret;
}

int CBaseProfile::GetProfileValueNumber(const char * segment, const char * key, u8 & value, u8 defualt)
{
	s32 s32Vaue = 0;
	s32 s32Defualt = (s32)defualt;
	int ret = GetProfileValueNumber(segment, key, s32Vaue, s32Defualt);
	value = (u8)(s32Vaue);
	return ret;
}

int CBaseProfile::GetProfileValueNumber(const char * segment, const char * key, u16 & value, u16 defualt)
{
	s32 s32Vaue = 0;
	s32 s32Defualt = (s32)defualt;
	int ret = GetProfileValueNumber(segment, key, s32Vaue, s32Defualt);
	value = (u16)(s32Vaue);
	//printf("[%s] [%s] s32 = %d u16=%u\n",segment,key,s32Vaue, value);
	return ret;
}

int CBaseProfile::GetProfileValueNumber(const char * segment, const char * key, u32 & value, u32 defualt)
{
	s32 s32Vaue = 0;
	s32 s32Defualt = (s32)defualt;
	int ret = GetProfileValueNumber(segment, key, s32Vaue, s32Defualt);
	value = (u32)(s32Vaue);
	return ret;
}

int CBaseProfile::GetProfileValueNumber(const char * segment, const char * key, bool & value, bool defualt)
{
	s32 s32Vaue = 0;
	s32 s32Defualt = (s32)defualt;
	int ret = GetProfileValueNumber(segment, key, s32Vaue, s32Defualt);
	value = s32Vaue==0?false:true;
	return ret;
}

int CBaseProfile::GetProfileValueNumber(const char * segment, const char * key, float & value, float defualt)
{
	double dbVaue = 0;
	double dbDefualt = (double)defualt;
	int ret = GetProfileValueNumber(segment, key, dbVaue, dbDefualt);
	value = (float)dbVaue;
	return ret;
}

int CBaseProfile::GetProfileValueNumber(const char * segment, const char * key, double & value, double defualt)
{
	if(segment == NULL || key == NULL)
	{
		return -1;
	}

	stringstream ss;
	ss.clear();
	ss << defualt;
	
	string strSegment = segment;
	string strKey	  = key;
	string strValue   = "";
	string strDefualt = ss.str();

	int ret = GetProfileValue(strSegment, strKey, strValue, strDefualt);

	value = atof(strValue.c_str());
	return ret;
}

int CBaseProfile::SetProfileValueNumber(const char * segment, const char * key, s8 value)
{
	return SetProfileValueNumber(segment, key, (s32)value);
}

int CBaseProfile::SetProfileValueNumber(const char * segment, const char * key, s16 value)
{
	return SetProfileValueNumber(segment, key, (s32)value);
}

int CBaseProfile::SetProfileValueNumber(const char * segment, const char * key, s32 value)
{
	if(segment == NULL || key == NULL)
	{
		return -1;
	}

	stringstream ss;
	ss.clear();
	ss << value;
	
	string strSegment = segment;
	string strKey = key;
	string strValue = ss.str();

	return SetProfileValue(strSegment, strKey, strValue);
}

int CBaseProfile::SetProfileValueNumber(const char * segment, const char * key, u8 value)
{
	return SetProfileValueNumber(segment, key, (s32)value);
}

int CBaseProfile::SetProfileValueNumber(const char * segment, const char * key, u16 value)
{
	return SetProfileValueNumber(segment, key, (s32)value);
}

int CBaseProfile::SetProfileValueNumber(const char * segment, const char * key, u32 value)
{
	return SetProfileValueNumber(segment, key, (s32)value);
}

int CBaseProfile::SetProfileValueNumber(const char * segment, const char * key, bool value)
{
	return SetProfileValueNumber(segment, key, value?1:0);
}

int CBaseProfile::SetProfileValueNumber(const char * segment, const char * key, float value)
{
	return SetProfileValueNumber(segment, key, (double)value);
}

int CBaseProfile::SetProfileValueNumber(const char * segment, const char * key, double value)
{
	if(segment == NULL || key == NULL)
	{
		return -1;
	}

	stringstream ss;
	ss.clear();
	ss << value;
	
	string strSegment = segment;
	string strKey = key;
	string strValue = ss.str();

	return SetProfileValue(strSegment, strKey, strValue);
}


