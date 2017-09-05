#ifndef   __CVX_TEXT_H__
#define   __CVX_TEXT_H__

#include <ft2build.h>
#include "opencv2/core/types_c.h"
#include FT_FREETYPE_H

//#define OSD_HEIGHT			36		// 文字高度
//#define OSD_CHAR_GAP		(OSD_HEIGHT/4)		// 文字间距

#ifdef __cplusplus
extern "C" {
#endif

typedef struct freetype_convert
{
    FT_Library		m_library;   
    FT_Face		m_face;      
    bool			m_bCreated;  
    bool			m_bLibCreated;
    bool			m_bFaceCreated;
    int			m_fontType;
    CvScalar		m_fontSize;
    bool			m_fontUnderline;
    float			m_fontDiaphaneity;
}freetype_convert_t;

void initFont();
void resetFont(int size);
void destoryFont();
int PutOsdText(IplImage* pImage, char* text, int x, int y,CvScalar txtColor);
int GetOsdTextWidth(char* text);

#ifdef __cplusplus
}
#endif

#endif
