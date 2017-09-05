#ifndef __PROFILE_PROC_H__
#define __PROFILE_PROC_H__
#include "oal_typedef.h"
#include "pthread.h"
#include <list>
#include <map>
#include <string>
using namespace std;

#define CFG_MISS_SEGMENT	-2	// 没有对应的段
#define CFG_MISS_KEY		-3	// 没有对应的关键字

typedef pair<string, string>		KeyValuePair;
typedef list<KeyValuePair>			KeyValueList;
typedef KeyValueList::iterator		KeyValueListIter;
typedef map<string,KeyValueList>	SegmentMap;
typedef SegmentMap::iterator		SegmentMapIter;

class CBaseProfile
{
public:
	CBaseProfile();
	~CBaseProfile();

	int ReadProfile();
	int WriteProfile();
	int DeleteProfile();

	int GetProfileValueString(const char * segment, const char * key, char *  value, const char * defualt = NULL);

	int SetProfileValueString(const char * segment, const char * key, const char *  value);

	int DelProfileSegment(const char * segment);

	int DelProfileSegmentKey(const char * segment, const char * key);

	int GetProfileValueNumber(const char * segment, const char * key, s8 & value, s8 defualt = (s8)0);
	int GetProfileValueNumber(const char * segment, const char * key, s16 & value, s16 defualt = (s16)0);
	int GetProfileValueNumber(const char * segment, const char * key, s32 & value, s32 defualt = (s32)0);
	int GetProfileValueNumber(const char * segment, const char * key, u8 & value, u8 defualt = (u8)0);
	int GetProfileValueNumber(const char * segment, const char * key, u16 & value, u16 defualt = (u16)0);
	int GetProfileValueNumber(const char * segment, const char * key, u32 & value, u32 defualt = (u32)0);
	int GetProfileValueNumber(const char * segment, const char * key, bool & value, bool defualt = false);
	int GetProfileValueNumber(const char * segment, const char * key, float & value, float defualt = (float)0);
	int GetProfileValueNumber(const char * segment, const char * key, double & value, double defualt = (double)0);
	
	int SetProfileValueNumber(const char * segment, const char * key, s8 value);
	int SetProfileValueNumber(const char * segment, const char * key, s16 value);
	int SetProfileValueNumber(const char * segment, const char * key, s32 value);
	int SetProfileValueNumber(const char * segment, const char * key, u8 value);
	int SetProfileValueNumber(const char * segment, const char * key, u16 value);
	int SetProfileValueNumber(const char * segment, const char * key, u32 value);
	int SetProfileValueNumber(const char * segment, const char * key, bool value);
	int SetProfileValueNumber(const char * segment, const char * key, float value);
	int SetProfileValueNumber(const char * segment, const char * key, double value);

private:
	int GetProfileValue(string segment, string key, string & value, string defualt = "");
	int SetProfileValue(string segment, string key, string value);

//内部调用
protected:

	virtual int DoReadProfile() = 0;
	virtual int DoWriteProfile() = 0;
	virtual int DoDeleteProfile() = 0;
	
    //m_cfgCache读写锁
    pthread_rwlock_t m_rwlock;
	SegmentMap m_cfgCache;
};



#endif //__PROFILE_PROC_H__
