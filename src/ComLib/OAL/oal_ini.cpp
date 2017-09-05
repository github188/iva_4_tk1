#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/oal_ini.h"


OAL_API const char * OAL_ReadIniString( const char *title, const char *key, const char *defaultValue, const char *fileName )
{
    FILE *fp; 
    char szLine[1024];
    static char tmpstr[1024] = {0};
    int rtnval;
    int i = 0; 
    int flag = 0; 
    char *tmp;
 
    if ((fp = fopen(fileName, "r")) == NULL) 
        return defaultValue;
	
    while (!feof(fp)) 
    { 
        rtnval = fgetc(fp); 
        if (rtnval == EOF) 
        {
        	if (i > 0)
        	{
        		szLine[i++] = '\r';
				szLine[i++] = '\n';
				rtnval = '\n';
        	}
			else
            	break; 
        } 
        else
        { 
            szLine[i++] = (char)rtnval; 
        } 
        if (rtnval == '\n') 
        { 
            szLine[--i] = '\0';
			if (i > 1 && szLine[i - 1] == '\r')
				szLine[--i] = '\0';
			
            i = 0; 
            tmp = strchr(szLine, '='); 
 
            if (tmp != NULL && flag == 1) 
            { 
                if (strstr(szLine, key) != NULL) 
                { 
                     if ('#' == szLine[0])
                    {
                    }
                    else
                    {
                        strcpy(tmpstr, tmp + 1); 
                        fclose(fp);
                        return tmpstr;
                    }
                } 
            }
            else
            { 
                strcpy(tmpstr, "["); 
                strcat(tmpstr, title); 
                strcat(tmpstr, "]");
                if (strncmp(tmpstr, szLine, strlen(tmpstr)) == 0) 
                {
                    // found title
                    flag = 1; 
                }
            }
        }
    }
	
    fclose(fp); 

	return defaultValue; 
}

OAL_API int OAL_ReadIniNumber( const char *title, const char *key, int defaultValue, const char *fileName )
{
	char buf[64];
	sprintf(buf, "%d", defaultValue);
	return atoi(OAL_ReadIniString(title, key, buf, fileName));
}
