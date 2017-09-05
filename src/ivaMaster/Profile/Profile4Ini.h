#ifndef __PROFILE_INI_H__
#define __PROFILE_INI_H__

#include "ProfileProc.h"
#include <string>
using namespace std;


class CProfileIni : public CBaseProfile
{
public:
	CProfileIni(const string& strDir, const string& strFileName);
	~CProfileIni(){};

	
private:
	virtual int DoReadProfile();
	virtual int DoWriteProfile();
	virtual int DoDeleteProfile();

private:
	string m_strDir;
	string m_strFileName;
};

string & string_trim(string & str);

const char *iniGetString(const char *title, const char *key, const char *defaultValue, const char *fileName);
int iniGetInt(const char *title, const char *key, int defaultValue, const char *fileName);

#endif //__PROFILE_INI_H__

