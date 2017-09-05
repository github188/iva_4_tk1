#include "WriteExcel.h"
#include "xlslib/xlslib.h"
#include "oal_unicode.h"
#include <string.h>
#include "oal_time.h"
#include "oal_log.h"

using namespace std;
using namespace xlslib_core;

//#define TEST_GB_TO_UCS2

/**************************************************************************
�������ƣ�UTF8_To_UCS4
����˵����ת��UTF8���뵽UCS4����
���������pbUTF8
���������
�� �� ֵ���ɹ����ֽ�����ʧ�ܣ�0
***************************************************************************/
int UTF8_To_UCS4(unsigned char* pUtf8Str, unsigned int* pUCS4Char)
{
	int i, iLen;
	unsigned char  b;

	if (pUtf8Str == NULL)
	{
		return 0;
	}

	b = *pUtf8Str++;
	if (b < 0x80)
	{
		*pUCS4Char = b;
		return 1;
	}

	if (b < 0xC0 || b > 0xFD)
	{
		// �Ƿ�UTF8
		return 0;
	}

	if (b < 0xE0)
	{
		*pUCS4Char = b & 0x1F;
		iLen = 2;
	}
	else if (b < 0xF0)
	{
		*pUCS4Char = b & 0x0F;
		iLen = 3;
	}
	else if (b < 0xF8)
	{
		*pUCS4Char = b & 7;
		iLen = 4;
	}
	else if (b < 0xFC)
	{
		*pUCS4Char = b & 3;
		iLen = 5;
	}
	else
	{
		*pUCS4Char = b & 1;
		iLen = 6;
	}

	for (i = 1;i < iLen;i++)
	{
		b = *pUtf8Str++;
		if (b < 0x80 || b > 0xBF)
		{
			// �Ƿ�UTF8
			break;
		}

		*pUCS4Char = (*pUCS4Char << 6) + (b & 0x3F);
	}

	if (i < iLen)
	{
		return 0;
	}
	else
	{
		return iLen;
	}
}

int UCS4_To_UCS2(unsigned int dwUCS4, unsigned short* pUCS2Char )
{
	if (dwUCS4 <= 0xFFFF)
	{
		if (pUCS2Char != NULL)
		{
			*pUCS2Char = (u16)(dwUCS4);
		}

		return 1;
	}
	else if (dwUCS4 <= 0xEFFFF)
	{
		if (pUCS2Char != NULL)
		{
			pUCS2Char[0] = (u16)(0xD800 + (dwUCS4 >> 10) - 0x40);// ��10λ
			pUCS2Char[1] = (u16)(0xDC00 + (dwUCS4 & 0x03FF));// ��10λ
		}
		return 2;
	}
	else
	{
		return 0;
	}
}

int Gb2312_TO_UCS2(char* GB2312Str, wchar_t *USC2Str)
{
	int s32Ret = SUCCESS;
	if(GB2312Str == NULL || USC2Str == NULL)
	{
		return FAILURE;
	}
	int nCnt = strlen(GB2312Str)+1;
	s8  *pUTF8Str = NULL;
	u16 *pUCS2Str = NULL;

	pUTF8Str = (s8 *)malloc(8*nCnt*sizeof(s8));
	if(pUTF8Str == NULL)
	{
		return FAILURE;
	}

	pUCS2Str  = (u16*)malloc(2*nCnt*sizeof(u16));
	if(pUCS2Str == NULL)
	{
		free(pUTF8Str);
		return FAILURE;
	}

	memset(pUTF8Str,0,nCnt*sizeof(s8));
	memset(pUCS2Str,0,nCnt*sizeof(u16));


#ifdef TEST_GB_TO_UCS2
	printf("\n========================Gb2312 TO UCS2========================\n");
	printf("==GB2312:%s\n",GB2312Str);
#endif

	// ��һ�� GB2312תUTF8
	GB2312_To_UTF8(GB2312Str,strlen(GB2312Str), pUTF8Str,8*nCnt*sizeof(s8));
	//strcpy(pUTF8Str,GB2312Str);
	//StrGB2312ToUTF8(pUTF8Str,strlen(pUTF8Str));

#ifdef TEST_GB_TO_UCS2
	printf("==UTF8:");
	for(s32 pp=0;pp < strlen(pUTF8Str);pp++)
	{
		printf("x%02x ",pUTF8Str[pp]);
	}
	printf("\n");
#endif

	// �ڶ��� UTF8תUCS4 UCS4תUCS2
	s32 iNum, iLen;
	u32 dwUCS4;

	iNum = 0;
	s8  *pUtf8char = pUTF8Str;
	u16 *pUTF16Char = pUCS2Str;
	while( *pUtf8char )
	{
		// UTF8תUCS4
		iLen = UTF8_To_UCS4((u8*)pUtf8char, &dwUCS4);
		if ( iLen == 0 )
		{
			// �Ƿ���UTF8����
			printf("x%04x �Ƿ���UTF8����",*pUtf8char);
			pUtf8char += 1;
			continue;
		}

		pUtf8char += iLen;

		// UCS4תUTF16
		iLen = UCS4_To_UCS2(dwUCS4, pUTF16Char);
		if( iLen == 0 )
		{
			continue;
		}

		if( pUTF16Char != NULL )
		{
			pUTF16Char += iLen;
		}

		iNum += iLen;
	}

	if( pUTF16Char != NULL )
	{
		*pUTF16Char = 0;// д���ַ����������
	}

#ifdef TEST_GB_TO_UCS2
	printf("==UCS2:");
	for(s32 qq=0;qq < iNum;qq++)
	{
		printf("x%04x ",pUCS2Str[qq]);
	}
	printf("\n");
	printf("========================Gb2312 TO UCS2========================\n\n");
#endif

	for(s32 i=0;i < iNum;i++)
	{
		*(USC2Str+i) = (wchar_t)(pUCS2Str[i]);
	}

	if(pUTF8Str)
	{
		free(pUTF8Str);
		pUTF8Str = NULL;
	}
	if(pUCS2Str)
	{
		free(pUCS2Str);
		pUCS2Str = NULL;
	}

	return s32Ret;
}


/**************************************************************************
�������ƣ�GetExcelXFormat
����˵�������Excel��ʽ������
���������wb		    - Workbook����
            font_name   - ��������
            font_clr    - ������ɫ
            font_size   - �����С
            font_bold   - �������
            h_align	    - ˮƽ���뷽ʽ
            v_align	    - ��ֱ���뷽ʽ
            border_style- �߿�Ч��
            border_clr  - �߿���ɫ
            fill_style  - ��䷽ʽ
            fill_clr    - �����ɫ
���������
�� �� ֵ��  �ɹ����򷵻�xfָ�룬ʧ�ܣ��򷵻�NULL
***************************************************************************/
xf_t* ExcelGetXFormat(workbook* wb,
					  const s8 * font_name, color_name_t font_clr, u32 font_size, boldness_option_t font_bold,
					  halign_option_t h_align,valign_option_t v_align,
					  border_style_t border_style,color_name_t border_clr,
					  fill_option_t fill_style,color_name_t fill_clr)
{
	xf_t   * xf	  = NULL;
	font_t * font = NULL;

	if(wb == NULL)
	{
		return NULL;
	}

	// ����XFormat
	xf = wb->xformat();
	if(xf == NULL)
	{
		return NULL;
	}

	// ������������
	if(font_name != NULL)
	{
		font = wb->font(font_name);
		if(font == NULL)
		{
			return NULL;
		}

		font->SetColor(font_clr);
		font->SetHeight(font_size);
		font->SetBoldStyle(font_bold);
		xf->SetFont(font);
	}

	// ���ö�������
	xf->SetHAlign(h_align);
	xf->SetVAlign(v_align);

	// ���ñ߿�����
	xf->SetBorderStyle(BORDER_TOP,   border_style);
	xf->SetBorderStyle(BORDER_BOTTOM,border_style);
	xf->SetBorderStyle(BORDER_LEFT,  border_style);
	xf->SetBorderStyle(BORDER_RIGHT, border_style);

	xf->SetBorderColor(BORDER_TOP,	 border_clr);
	xf->SetBorderColor(BORDER_BOTTOM,border_clr);
	xf->SetBorderColor(BORDER_LEFT,  border_clr);
	xf->SetBorderColor(BORDER_RIGHT, border_clr);

	// ���
	xf->SetFillStyle(fill_style);
	xf->SetFillFGColor(fill_clr);
	return xf;
}


/**************************************************************************
�������ƣ�  ExcelMergeArea
����˵����  �ϲ���Ԫ��
���������  ws		         - Worksheet����
            row_from,row_to  - �з�Χ
            col_from,col_to  - �з�Χ
            col_to	         - ��Ԫ���ʽ
���������
�� �� ֵ��  void
***************************************************************************/
void ExcelMergeArea(worksheet* ws,u32 row_from, u32 col_from, u32 row_to, u32 col_to,xf_t * xf)
{
	u32 u32Tmp = 0;
	u32 row = 0;
	u32 col = 0;

	if(ws)
	{
		if(row_from > row_to)
		{
			u32Tmp = row_from;
			row_from = row_to;
			row_to = u32Tmp;
		}

		if(col_from > col_to)
		{
			u32Tmp = col_from;
			col_from = col_to;
			col_to = u32Tmp;
		}

		// ��ʽ��
		for(row = row_from;row <= row_to;row++)
		{
			for(col = col_from;col <= col_to;col++)
			{
				ws->blank(row,col,xf);
			}
		}

		// �ϲ�
		ws->merge(row_from,col_from,row_to,col_to);
	}
}

/**************************************************************************
�������ƣ�ExcelSetText
����˵��������һ����Ԫ���ı�
���������ws		   - Worksheet����
          row,col    - ����
          content    - ����
          xf         - ��Ԫ���ʽ
���������
�� �� ֵ��void
***************************************************************************/
void ExcelSetText(worksheet* ws,u32 row, u32 col, const s8 * content, xf_t * xf)
{
	wchar_t *pwstr = NULL;
	if(ws && content && xf)
	{
		pwstr = (wchar_t *)malloc((strlen(content)+1)*sizeof(wchar_t));
		if(pwstr == NULL)
		{
			LOG_ERROR_FMT("Malloc failed");
			return;
		}
		memset(pwstr,0,(strlen(content)+1)*sizeof(wchar_t));

		Gb2312_TO_UCS2((char *)content,pwstr);
		ws->label(row,col,pwstr,xf);

		free(pwstr);
		pwstr = NULL;
	}
}

/**************************************************************************
�������ƣ�ExcelSetHyperlink
����˵��������һ����Ԫ������
���������ws		   - Worksheet����
          row,col    - ����
          content    - ����
���������
�� �� ֵ��void
***************************************************************************/
void ExcelSetHyperlink(worksheet* ws,u32 row, u32 col, const s8 * content)
{
	cell_t *cell = NULL;
	wchar_t *pwstr = NULL;
	if(ws && content)
	{
		pwstr = (wchar_t *)malloc((strlen(content)+1)*sizeof(wchar_t));
		if(pwstr == NULL)
		{
			LOG_ERROR_FMT("Malloc failed");
			return;
		}
		memset(pwstr,0,(strlen(content)+1)*sizeof(wchar_t));

		cell = ws->FindCell(row,col);
		if(cell)
		{
			Gb2312_TO_UCS2((char *)content,pwstr);
			ws->hyperLink(cell,pwstr);
		}

		free(pwstr);
		pwstr = NULL;
	}
}

int WriteLog2Excel( const LogRecord* ptList, int iCnt, const char *ps8SavePath )
{
	if(NULL == ptList || NULL == ps8SavePath)
	{
		return FAILURE;
	}

	workbook   wb;
	worksheet *ws = wb.sheet("Log");
	if(ws == NULL)
	{
		LOG_ERROR_FMT("wb.sheet Failed");
		return FAILURE;
	}

	// �����п� ������λ���ַ�'0'�Ŀ�ȵ�1/256
	//ws->defaultColwidth(256*10);
	//ws->colwidth(1,256*20);
	//ws->colwidth(4,256*16);
	//ws->colwidth(5,256*60);

	// ��ø�ʽ
	xf_t *xf       = ExcelGetXFormat(&wb,"Arial",CLR_BLACK,360,BOLDNESS_BOLD,  HALIGN_CENTER,VALIGN_CENTER,BORDER_THIN,CLR_BLACK,FILL_NONE, CLR_WHITE);
	xf_t *xf1      = ExcelGetXFormat(&wb,"Arial",CLR_BLACK,240,BOLDNESS_NORMAL,HALIGN_LEFT,  VALIGN_CENTER,BORDER_THIN,CLR_BLACK,FILL_NONE, CLR_WHITE);
	xf_t *xf_title = ExcelGetXFormat(&wb,"Arial",CLR_BLACK,280,BOLDNESS_BOLD,  HALIGN_CENTER,VALIGN_CENTER,BORDER_THIN,CLR_BLACK,FILL_SOLID,CLR_TEAL);
	xf_t *xf_row1  = ExcelGetXFormat(&wb,"Arial",CLR_BLACK,240,BOLDNESS_NORMAL,HALIGN_CENTER,VALIGN_CENTER,BORDER_THIN,CLR_BLACK,FILL_SOLID,CLR_GRAY25);
	xf_t *xf_row2  = ExcelGetXFormat(&wb,"Arial",CLR_BLACK,240,BOLDNESS_NORMAL,HALIGN_CENTER,VALIGN_CENTER,BORDER_THIN,CLR_BLACK,FILL_NONE, CLR_WHITE);

	int row = 0;
	int ret = 0;
	int iTotalCol = 10;
	s8 s8Gb2312Str[1024] = {0};
	wchar_t swUcs2Str[2048] = {0};

	// ��1�� ��¼TITLE
	sprintf(s8Gb2312Str,"%s","��־��¼");
	ExcelMergeArea(ws,row,0,0,iTotalCol,xf);
	ExcelSetText(ws,row++,0,s8Gb2312Str,xf);

	// ��2�� ����ʱ��:yyyy-mm-dd hh:mm:ss
	s8 s8TimeNow[36] = {0};
	TimeFormatString(time(NULL),s8TimeNow,sizeof(s8TimeNow),eYMDHMS1);
	sprintf(s8Gb2312Str,"����ʱ��:%s",s8TimeNow);
	ExcelMergeArea(ws,row,0,row,iTotalCol,xf1);
	ExcelSetText(ws,row++,0,s8Gb2312Str,xf1);

	// ��3�� ��ѯ���
	sprintf(s8Gb2312Str,"��ѯ���:��%d����¼",iCnt);
	ExcelMergeArea(ws,row,0,row,iTotalCol,xf1);
	ExcelSetText(ws,row++,0,s8Gb2312Str,xf1);

	// ��4�� ����
	sprintf(s8Gb2312Str,"%s","���");
	ExcelSetText(ws,row,0,s8Gb2312Str,xf_title);

	sprintf(s8Gb2312Str,"%s","ʱ��");
	ExcelMergeArea(ws,row,1,row,2,xf_title);
	ExcelSetText(ws,row,1,s8Gb2312Str,xf_title);

	sprintf(s8Gb2312Str,"%s","����");
	ExcelSetText(ws,row,3,s8Gb2312Str,xf_title);

	sprintf(s8Gb2312Str,"%s","�û�");
	ExcelSetText(ws,row,4,s8Gb2312Str,xf_title);

	sprintf(s8Gb2312Str,"%s","������ַ");
	ExcelMergeArea(ws,row,5,row,6,xf_title);
	ExcelSetText(ws,row,5,s8Gb2312Str,xf_title);

	sprintf(s8Gb2312Str,"%s","����");
	ExcelMergeArea(ws,row,7,row,iTotalCol,xf_title);
	ExcelSetText(ws,row,7,s8Gb2312Str,xf_title);
	row++;

	// ����
	xf_t * xf_row = NULL;
	for(int i = 0; i < iCnt; i++)
	{
		if(row%2 == 0)
		{
			xf_row = xf_row1;
		}
		else
		{
			xf_row = xf_row2;
		}

		const LogRecord * pRowNode = ptList+i;

		// ���
		sprintf(s8Gb2312Str,"%d", i+1);
		ExcelSetText(ws,row,0,s8Gb2312Str,xf_row);
		//printf("%s ",s8Gb2312Str);

		// ʱ��
		char szTime[40] = {0};
		TimeFormatString(pRowNode->tCreate,szTime,sizeof(szTime), eYMDHMS1);
		ExcelMergeArea(ws,row,1,row,2,xf_row);
		ExcelSetText(ws,row,1,szTime,xf_row);
		//printf("%s ",szTime);

		// ����
		switch (pRowNode->iType)
		{
		case OP_LOG_USR:
			sprintf(s8Gb2312Str,"%s","�û�����");
			break;
		case OP_LOG_MTN:
			sprintf(s8Gb2312Str,"%s","ϵͳά��");
			break;
		case OP_LOG_SYS:
			sprintf(s8Gb2312Str,"%s","ϵͳ����");
			break;
		case OP_LOG_VIN :
			sprintf(s8Gb2312Str,"%s","��Ƶ����");
			break;
		case OP_LOG_REC :
			sprintf(s8Gb2312Str,"%s","��Ƶ¼��");
			break;
		case OP_LOG_ALG:
			sprintf(s8Gb2312Str,"%s","�����㷨");
			break;
		default:
			sprintf(s8Gb2312Str,"%s","δ֪����");
			break;
		}
		ExcelSetText(ws,row,3,s8Gb2312Str,xf_row);
		//printf("%s ",s8Gb2312Str);

		// �û�
		UTF8_To_GB2312((char *)pRowNode->szUser,strlen(pRowNode->szUser),s8Gb2312Str,sizeof(s8Gb2312Str));
		ExcelSetText(ws,row,4,s8Gb2312Str,xf_row);
		//printf("%s ",s8Gb2312Str);

		// IP
		UTF8_To_GB2312((char *)pRowNode->szFrom,strlen(pRowNode->szFrom),s8Gb2312Str,sizeof(s8Gb2312Str));
		ExcelMergeArea(ws,row,5,row,6,xf_row);
		ExcelSetText(ws,row,5,s8Gb2312Str,xf_row);
		//printf("%s ",s8Gb2312Str);

		// Content
		UTF8_To_GB2312((char *)pRowNode->szContent,strlen(pRowNode->szContent),s8Gb2312Str,sizeof(s8Gb2312Str));
		ExcelMergeArea(ws,row,7,row,iTotalCol,xf_row);
		ExcelSetText(ws,row,7,s8Gb2312Str,xf_row);
		//printf("%s \n",s8Gb2312Str);

		row++;
	}

	// �����ļ�
	ret = wb.Dump(ps8SavePath);
	if(ret != 0)
	{
		LOG_ERROR_FMT("xlsWorkbookDump %s Failed",ps8SavePath);
		return FAILURE;
	}
	return SUCCESS;
}


int WriteAlarm2Excel(const AlarmRecord* ptList, int iCnt, const char *ps8SavePath)
{
	if(NULL == ptList || NULL == ps8SavePath)
	{
		return FAILURE;
	}

	workbook   wb;
	worksheet *ws = wb.sheet("Log");
	if(ws == NULL)
	{
		LOG_ERROR_FMT("wb.sheet Failed");
		return FAILURE;
	}

	// �����п� ������λ���ַ�'0'�Ŀ�ȵ�1/256
	//ws->colwidth(0,1,NULL);
	//ws->colwidth(1,2,NULL);

	// ��ø�ʽ
	xf_t *xf       = ExcelGetXFormat(&wb,"Arial",CLR_BLACK,360,BOLDNESS_BOLD,  HALIGN_CENTER,VALIGN_CENTER,BORDER_THIN,CLR_BLACK,FILL_NONE, CLR_WHITE);
	xf_t *xf1      = ExcelGetXFormat(&wb,"Arial",CLR_BLACK,240,BOLDNESS_NORMAL,HALIGN_LEFT,  VALIGN_CENTER,BORDER_THIN,CLR_BLACK,FILL_NONE, CLR_WHITE);
	xf_t *xf_title = ExcelGetXFormat(&wb,"Arial",CLR_BLACK,280,BOLDNESS_BOLD,  HALIGN_CENTER,VALIGN_CENTER,BORDER_THIN,CLR_BLACK,FILL_SOLID,CLR_TEAL);
	xf_t *xf_row1  = ExcelGetXFormat(&wb,"Arial",CLR_BLACK,240,BOLDNESS_NORMAL,HALIGN_CENTER,VALIGN_CENTER,BORDER_THIN,CLR_BLACK,FILL_SOLID,CLR_GRAY25);
	xf_t *xf_row2  = ExcelGetXFormat(&wb,"Arial",CLR_BLACK,240,BOLDNESS_NORMAL,HALIGN_CENTER,VALIGN_CENTER,BORDER_THIN,CLR_BLACK,FILL_NONE, CLR_WHITE);

	int row = 0;
	int ret = 0;
	int iTotalCol = 5;
	s8 s8Gb2312Str[1024] = {0};
	wchar_t swUcs2Str[2048] = {0};

	// ��1�� ��¼TITLE
	sprintf(s8Gb2312Str,"%s","��־��¼");
	ExcelMergeArea(ws,row,0,0,iTotalCol,xf);
	ExcelSetText(ws,row++,0,s8Gb2312Str,xf);

	// ��2�� ����ʱ��:yyyy-mm-dd hh:mm:ss
	s8 s8TimeNow[36] = {0};
	TimeFormatString(time(NULL),s8TimeNow,sizeof(s8TimeNow),eYMDHMS1);
	sprintf(s8Gb2312Str,"����ʱ��:%s",s8TimeNow);
	ExcelMergeArea(ws,row,0,row,iTotalCol,xf1);
	ExcelSetText(ws,row++,0,s8Gb2312Str,xf1);

	// ��3�� ��ѯ���
	sprintf(s8Gb2312Str,"��ѯ���:��%d����¼",iCnt);
	ExcelMergeArea(ws,row,0,row,iTotalCol,xf1);
	ExcelSetText(ws,row++,0,s8Gb2312Str,xf1);

	// ��4�� ����
	sprintf(s8Gb2312Str,"%s","���");
	ExcelSetText(ws,row,0,s8Gb2312Str,xf_title);
	sprintf(s8Gb2312Str,"%s","ʱ��");
	ExcelMergeArea(ws,row,1,row,2,xf);
	ExcelSetText(ws,row,1,s8Gb2312Str,xf_title);
	sprintf(s8Gb2312Str,"%s","����");
	ExcelMergeArea(ws,row,3,row,iTotalCol,xf);
	ExcelSetText(ws,row,3,s8Gb2312Str,xf_title);
	row++;

	// ����
	xf_t * xf_row = NULL;
	for(int i = 0; i < iCnt; i++)
	{
		if(row%2 == 0)
		{
			xf_row = xf_row1;
		}
		else
		{
			xf_row = xf_row2;
		}

		const AlarmRecord * pRowNode = ptList+i;

		// ���
		sprintf(s8Gb2312Str,"%d", i+1);
		ExcelSetText(ws,row,0,s8Gb2312Str,xf_row);

		// ʱ��
		TimeFormatString(pRowNode->tCreate,s8Gb2312Str,sizeof(s8Gb2312Str), eYMDHMS1);
		ExcelMergeArea(ws,row,1,row,2,xf);
		ExcelSetText(ws,row,1,s8Gb2312Str,xf_row);

		// Content
		UTF8_To_GB2312((char *)pRowNode->szContent,strlen(pRowNode->szContent),s8Gb2312Str,sizeof(s8Gb2312Str));
		ExcelMergeArea(ws,row,3,row,iTotalCol,xf_row);
		ExcelSetText(ws,row,3,s8Gb2312Str,xf_row);

		row++;
	}

	// �����ļ�
	ret = wb.Dump(ps8SavePath);
	if(ret != 0)
	{
		LOG_ERROR_FMT("xlsWorkbookDump %s Failed",ps8SavePath);
		return FAILURE;
	}
	return SUCCESS;
}
