#ifndef __XML_PACK_H__
#define __XML_PACK_H__
#include "../QueryDb.h"

int create_xml_file(const char *file_name, const AnalyDbRecord *pData);

int create_xml_buff(char *buff, int len, const AnalyDbRecord *pData);

#endif//__XML_PACK_H__
