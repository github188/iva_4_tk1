#include <stdio.h>
#include <stdlib.h>

#include <locale.h>
#include <iconv.h>
#include "cvxText.h" 
#include "oal_unicode.h"
#include "opencv2/core/core_c.h"

#define OSD_FONT "simhei.ttf"


static freetype_convert_t freetype;
static int s_space_width = 8;// ¿Õ¸ñµÄ¿í´ø


void resetFont(int size)
{
	FT_Set_Pixel_Sizes(freetype.m_face, size, size);
	s_space_width = size/4;
}

void setFont(freetype_convert_t* convert,int *type, CvScalar *size, bool *underline, float *diaphaneity)
{
    if (type)
    {
       if (type  >= 0)
           convert->m_fontType  = *type;
    }

    if (size)
    {
       convert->m_fontSize.val[0] = fabs(size->val[0]);
       convert->m_fontSize.val[1] = fabs(size->val[1]);
       convert->m_fontSize.val[2] = fabs(size->val[2]);
       convert->m_fontSize.val[3] = fabs(size->val[3]);
    }

    if (underline)
    {
       convert->m_fontUnderline = *underline;
    }

    if (diaphaneity)
    {
       convert->m_fontDiaphaneity = *diaphaneity;
    }

    FT_Set_Pixel_Sizes(convert->m_face, (int)convert->m_fontSize.val[0], 0);
	
}

void LoadFontLib(const char *freeType,freetype_convert_t* convert)
{
	float fp = 1;
	int iScale = 10;
	CvScalar size = cvScalar(4 * iScale, 0.5 * iScale, 0.1, 0);
	
	assert(freeType  != NULL && convert != NULL);
	
	if(FT_Init_FreeType(&convert->m_library))
	{
	    convert->m_bLibCreated = false;
	    return;
	}
	else
	{
	    convert->m_bLibCreated  = true;
	}
	if(FT_New_Face(convert->m_library, freeType, 0, &convert->m_face))
	{
	   convert->m_bFaceCreated  = false;
	   return;
	}
	else
	{
	   convert->m_bFaceCreated  = true;
	}
	

	setFont(convert, NULL, &size, NULL, &fp);
	
	if(setlocale(LC_CTYPE,"zh_CN.UTF8") == NULL)
	{
		printf("setlocale return NULL, please do sudo apt-get install language-pack-zh-hant-base language-pack-zh-hans-base\n");
	}

	convert->m_bCreated  = true;
}

void putWChar(freetype_convert_t* convert,IplImage *img, wchar_t wc, CvPoint *pos, CvScalar stColor)
{
    FT_UInt glyph_index  = FT_Get_Char_Index(convert->m_face, wc);
    FT_Load_Glyph(convert->m_face, glyph_index, FT_LOAD_DEFAULT);
    //FT_Render_Glyph(convert->m_face->glyph, FT_RENDER_MODE_MONO);
	FT_Render_Glyph(convert->m_face->glyph, FT_RENDER_MODE_NORMAL);

    FT_GlyphSlot slot  = convert->m_face->glyph;

    int rows  = slot->bitmap.rows;
    int cols  = slot->bitmap.width;
    int width = cols;

    int i, j, k;
	
	int x0 = 0;//slot->bitmap_left;
	int y0 = slot->bitmap_top;


	//printf( "-------------------------------------------------------------------------\n");
    for (i  = 0; i < rows; ++i)
    {
        for (j  = 0; j < cols; ++j)
        {
			//printf( "%d " , slot->bitmap.buffer[i * slot->bitmap.pitch + j] ? 1 : 0 );
			//printf( "%d " , ((slot->bitmap.buffer[i * slot->bitmap.pitch + (j>>3)]<<(j%8))&0x80)?1:0 ); 

            int off   = i * slot->bitmap.pitch + j;//((img->origin  == 0)? i: (rows-1-i))* slot->bitmap.pitch + j/8;

            //if (slot->bitmap.buffer[off] & (0xC0 >> (j%8)))
            if (slot->bitmap.buffer[off])
            {
				int r = pos->y - y0 + i;
				int c = pos->x + x0 + j;
				
				if(img->origin  == 1)
				{
					r = pos->y + y0 + i;
				}
                //int r  = (img->origin  == 0)? pos->y - (rows-1-i)-(slot->bitmap_top-slot->bitmap.rows): pos->y + i + (slot->bitmap_top-slot->bitmap.rows);
				
				//if(rows < 5)
				//{
				//	r = r - 12;
				//}
				
                //int c  = pos->x + j+slot->bitmap_left;

                if (r  >= 0  &&  r < img->height
                     &&  c  >= 0  &&  c < img->width)
                {
                    CvScalar scalar  = cvGet2D(img, r, c);


                    float p  = convert->m_fontDiaphaneity;
                    for (k  = 0; k < 4; ++k)
                    {
                        scalar.val[k]  = scalar.val[k]*(1-p) + stColor.val[k]*p;
                    }
					
                    cvSet2D(img, r, c, scalar);
                }
            }
        } // end for
       //printf("\n");
    } // end for

	//printf("img->origin %d left %d top %d width %d height %d x0 %d y0 %d x %d y %d\n",img->origin,slot->bitmap_left,slot->bitmap_top,slot->bitmap.width,slot->bitmap.rows,x0,y0,pos->x,pos->y);

    pos->x += ((width ? width: s_space_width) + s_space_width);
}

int putText(freetype_convert_t* convert, IplImage *img, const wchar_t *text, CvPoint pos, CvScalar stColor)
{
	if (img == NULL)
	{
		//printf("%s %s %d \n", __FUNCTION__, __FILE__, __LINE__);
		return -1;
	}

	if (text == NULL)
	{
		//printf("%s %s %d \n", __FUNCTION__, __FILE__, __LINE__);
		return -1;
	}

	if(!convert->m_bCreated)
	{
		//printf("%s %s %d \n", __FUNCTION__, __FILE__, __LINE__);
		return -1;
	}

	for(int i = 0; text[i]  != '\0'; ++i)
	{
	    wchar_t wc = text[i];
	    putWChar(convert, img, wc, &pos, stColor);
	}

	return 0;
}

void ReleaseFont(freetype_convert_t* convert)
{
	if (convert->m_bFaceCreated)
	{
	   FT_Done_Face(convert->m_face);
	   convert->m_bFaceCreated  = false;
	}
	if (convert->m_bLibCreated)
	{
	   FT_Done_FreeType(convert->m_library);
	   convert->m_bLibCreated  = false;
	}

	convert->m_bCreated  = false;
}

void initFont()
{
	freetype.m_bCreated = false;
	freetype.m_bFaceCreated = false;
	freetype.m_bLibCreated = false;

	LoadFontLib(OSD_FONT, &freetype);
}

void destoryFont()
{
	ReleaseFont(&freetype);
}

int PutOsdText(IplImage* pImage, char* text, int x, int y,CvScalar txtColor)
{
	int ret = -1;
	wchar_t wtext[256];

	memset(wtext, '\0', sizeof(wtext));
	
	ret = mbstowcs(wtext, text, strlen(text));
	//printf("mbstowcs %s, ret = %d\n", text);

	putText(&freetype, pImage, wtext, cvPoint(x, y), txtColor);

	ret = 0;

	return ret;
}

int WCharWidth(freetype_convert_t* convert, wchar_t wc)
{
    FT_UInt glyph_index  = FT_Get_Char_Index(convert->m_face, wc);
    FT_Load_Glyph(convert->m_face, glyph_index, FT_LOAD_DEFAULT);
    FT_Render_Glyph(convert->m_face->glyph, FT_RENDER_MODE_MONO);

    FT_GlyphSlot slot  = convert->m_face->glyph;

    int rows  = slot->bitmap.rows;
    int cols  = slot->bitmap.width;
	
	int width = cols;//slot->bitmap_left+rows;
	
    return ((width? width: s_space_width) + s_space_width);
}

int TextWidth(freetype_convert_t* convert, const wchar_t  *text)
{
	int i;
	int iWidth = 0;
	wchar_t wc;
	

	if (text == NULL)
		return 0;

	if(!convert->m_bCreated)
		return 0;

	for(i  = 0; text[i]  != '\0'; ++i)
	{
	    wc  = text[i];
		
	    iWidth += WCharWidth(convert,wc);
	}
	//printf("TextWidth i = %d\n",i);

	return iWidth;
}

int GetOsdTextWidth(char* text)
{
	int ret = -1;
	wchar_t wtext[2048] = {0};
	char szUtf8Content[2048] = {0};
	GB2312_To_UTF8(text,strlen(text),szUtf8Content,2048);

	memset(wtext, '\0', sizeof(wtext));
	
	ret = mbstowcs(wtext, szUtf8Content, strlen(szUtf8Content));

	int iWidth = TextWidth(&freetype, wtext);
	
	//printf("%d ret %d== %s\n",iWidth,ret,text);

	return iWidth;
}
