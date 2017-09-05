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
函数名称：UTF8_To_UCS4
函数说明：转换UTF8编码到UCS4编码
输入参数：pbUTF8
输出参数：
返 回 值：成功，字节数，失败，0
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
		// 非法UTF8
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
			// 非法UTF8
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
			pUCS2Char[0] = (u16)(0xD800 + (dwUCS4 >> 10) - 0x40);// 高10位
			pUCS2Char[1] = (u16)(0xDC00 + (dwUCS4 & 0x03FF));// 低10位
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

	// 第一步 GB2312转UTF8
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

	// 第二步 UTF8转UCS4 UCS4转UCS2
	s32 iNum, iLen;
	u32 dwUCS4;

	iNum = 0;
	s8  *pUtf8char = pUTF8Str;
	u16 *pUTF16Char = pUCS2Str;
	while( *pUtf8char )
	{
		// UTF8转UCS4
		iLen = UTF8_To_UCS4((u8*)pUtf8char, &dwUCS4);
		if ( iLen == 0 )
		{
			// 非法的UTF8编码
			printf("x%04x 非法的UTF8编码",*pUtf8char);
			pUtf8char += 1;
			continue;
		}

		pUtf8char += iLen;

		// UCS4转UTF16
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
		*pUTF16Char = 0;// 写入字符串结束标记
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
函数名称：GetExcelXFormat
函数说明：获得Excel格式化对象
输入参数：wb		    - Workbook对象
            font_name   - 字体名称
            font_clr    - 字体颜色
            font_size   - 字体大小
            font_bold   - 字体黑体
            h_align	    - 水平对齐方式
            v_align	    - 垂直对齐方式
            border_style- 边框效果
            border_clr  - 边框颜色
            fill_style  - 填充方式
            fill_clr    - 填充颜色
输出参数：
返 回 值：  成功，则返回xf指针，失败，则返回NULL
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

	// 创建XFormat
	xf = wb->xformat();
	if(xf == NULL)
	{
		return NULL;
	}

	// 设置字体属性
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

	// 设置对齐属性
	xf->SetHAlign(h_align);
	xf->SetVAlign(v_align);

	// 设置边框属性
	xf->SetBorderStyle(BORDER_TOP,   border_style);
	xf->SetBorderStyle(BORDER_BOTTOM,border_style);
	xf->SetBorderStyle(BORDER_LEFT,  border_style);
	xf->SetBorderStyle(BORDER_RIGHT, border_style);

	xf->SetBorderColor(BORDER_TOP,	 border_clr);
	xf->SetBorderColor(BORDER_BOTTOM,border_clr);
	xf->SetBorderColor(BORDER_LEFT,  border_clr);
	xf->SetBorderColor(BORDER_RIGHT, border_clr);

	// 填充
	xf->SetFillStyle(fill_style);
	xf->SetFillFGColor(fill_clr);
	return xf;
}


/**************************************************************************
函数名称：  ExcelMergeArea
函数说明：  合并单元格
输入参数：  ws		         - Worksheet对象
            row_from,row_to  - 行范围
            col_from,col_to  - 列范围
            col_to	         - 单元格格式
输出参数：
返 回 值：  void
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

		// 格式化
		for(row = row_from;row <= row_to;row++)
		{
			for(col = col_from;col <= col_to;col++)
			{
				ws->blank(row,col,xf);
			}
		}

		// 合并
		ws->merge(row_from,col_from,row_to,col_to);
	}
}

/**************************************************************************
函数名称：ExcelSetText
函数说明：设置一个单元格文本
输入参数：ws		   - Worksheet对象
          row,col    - 行列
          content    - 内容
          xf         - 单元格格式
输出参数：
返 回 值：void
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
函数名称：ExcelSetHyperlink
函数说明：设置一个单元格超链接
输入参数：ws		   - Worksheet对象
          row,col    - 行列
          content    - 内容
输出参数：
返 回 值：void
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

	// 设置列宽 基本单位是字符'0'的宽度的1/256
	//ws->defaultColwidth(256*10);
	//ws->colwidth(1,256*20);
	//ws->colwidth(4,256*16);
	//ws->colwidth(5,256*60);

	// 获得格式
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

	// 第1行 记录TITLE
	sprintf(s8Gb2312Str,"%s","日志记录");
	ExcelMergeArea(ws,row,0,0,iTotalCol,xf);
	ExcelSetText(ws,row++,0,s8Gb2312Str,xf);

	// 第2行 创建时间:yyyy-mm-dd hh:mm:ss
	s8 s8TimeNow[36] = {0};
	TimeFormatString(time(NULL),s8TimeNow,sizeof(s8TimeNow),eYMDHMS1);
	sprintf(s8Gb2312Str,"创建时间:%s",s8TimeNow);
	ExcelMergeArea(ws,row,0,row,iTotalCol,xf1);
	ExcelSetText(ws,row++,0,s8Gb2312Str,xf1);

	// 第3行 查询结果
	sprintf(s8Gb2312Str,"查询结果:共%d条记录",iCnt);
	ExcelMergeArea(ws,row,0,row,iTotalCol,xf1);
	ExcelSetText(ws,row++,0,s8Gb2312Str,xf1);

	// 第4行 标题
	sprintf(s8Gb2312Str,"%s","序号");
	ExcelSetText(ws,row,0,s8Gb2312Str,xf_title);

	sprintf(s8Gb2312Str,"%s","时间");
	ExcelMergeArea(ws,row,1,row,2,xf_title);
	ExcelSetText(ws,row,1,s8Gb2312Str,xf_title);

	sprintf(s8Gb2312Str,"%s","类型");
	ExcelSetText(ws,row,3,s8Gb2312Str,xf_title);

	sprintf(s8Gb2312Str,"%s","用户");
	ExcelSetText(ws,row,4,s8Gb2312Str,xf_title);

	sprintf(s8Gb2312Str,"%s","操作地址");
	ExcelMergeArea(ws,row,5,row,6,xf_title);
	ExcelSetText(ws,row,5,s8Gb2312Str,xf_title);

	sprintf(s8Gb2312Str,"%s","内容");
	ExcelMergeArea(ws,row,7,row,iTotalCol,xf_title);
	ExcelSetText(ws,row,7,s8Gb2312Str,xf_title);
	row++;

	// 正文
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

		// 序号
		sprintf(s8Gb2312Str,"%d", i+1);
		ExcelSetText(ws,row,0,s8Gb2312Str,xf_row);
		//printf("%s ",s8Gb2312Str);

		// 时间
		char szTime[40] = {0};
		TimeFormatString(pRowNode->tCreate,szTime,sizeof(szTime), eYMDHMS1);
		ExcelMergeArea(ws,row,1,row,2,xf_row);
		ExcelSetText(ws,row,1,szTime,xf_row);
		//printf("%s ",szTime);

		// 类型
		switch (pRowNode->iType)
		{
		case OP_LOG_USR:
			sprintf(s8Gb2312Str,"%s","用户管理");
			break;
		case OP_LOG_MTN:
			sprintf(s8Gb2312Str,"%s","系统维护");
			break;
		case OP_LOG_SYS:
			sprintf(s8Gb2312Str,"%s","系统配置");
			break;
		case OP_LOG_VIN :
			sprintf(s8Gb2312Str,"%s","视频输入");
			break;
		case OP_LOG_REC :
			sprintf(s8Gb2312Str,"%s","视频录像");
			break;
		case OP_LOG_ALG:
			sprintf(s8Gb2312Str,"%s","智能算法");
			break;
		default:
			sprintf(s8Gb2312Str,"%s","未知操作");
			break;
		}
		ExcelSetText(ws,row,3,s8Gb2312Str,xf_row);
		//printf("%s ",s8Gb2312Str);

		// 用户
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

	// 保存文件
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

	// 设置列宽 基本单位是字符'0'的宽度的1/256
	//ws->colwidth(0,1,NULL);
	//ws->colwidth(1,2,NULL);

	// 获得格式
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

	// 第1行 记录TITLE
	sprintf(s8Gb2312Str,"%s","日志记录");
	ExcelMergeArea(ws,row,0,0,iTotalCol,xf);
	ExcelSetText(ws,row++,0,s8Gb2312Str,xf);

	// 第2行 创建时间:yyyy-mm-dd hh:mm:ss
	s8 s8TimeNow[36] = {0};
	TimeFormatString(time(NULL),s8TimeNow,sizeof(s8TimeNow),eYMDHMS1);
	sprintf(s8Gb2312Str,"创建时间:%s",s8TimeNow);
	ExcelMergeArea(ws,row,0,row,iTotalCol,xf1);
	ExcelSetText(ws,row++,0,s8Gb2312Str,xf1);

	// 第3行 查询结果
	sprintf(s8Gb2312Str,"查询结果:共%d条记录",iCnt);
	ExcelMergeArea(ws,row,0,row,iTotalCol,xf1);
	ExcelSetText(ws,row++,0,s8Gb2312Str,xf1);

	// 第4行 标题
	sprintf(s8Gb2312Str,"%s","序号");
	ExcelSetText(ws,row,0,s8Gb2312Str,xf_title);
	sprintf(s8Gb2312Str,"%s","时间");
	ExcelMergeArea(ws,row,1,row,2,xf);
	ExcelSetText(ws,row,1,s8Gb2312Str,xf_title);
	sprintf(s8Gb2312Str,"%s","内容");
	ExcelMergeArea(ws,row,3,row,iTotalCol,xf);
	ExcelSetText(ws,row,3,s8Gb2312Str,xf_title);
	row++;

	// 正文
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

		// 序号
		sprintf(s8Gb2312Str,"%d", i+1);
		ExcelSetText(ws,row,0,s8Gb2312Str,xf_row);

		// 时间
		TimeFormatString(pRowNode->tCreate,s8Gb2312Str,sizeof(s8Gb2312Str), eYMDHMS1);
		ExcelMergeArea(ws,row,1,row,2,xf);
		ExcelSetText(ws,row,1,s8Gb2312Str,xf_row);

		// Content
		UTF8_To_GB2312((char *)pRowNode->szContent,strlen(pRowNode->szContent),s8Gb2312Str,sizeof(s8Gb2312Str));
		ExcelMergeArea(ws,row,3,row,iTotalCol,xf_row);
		ExcelSetText(ws,row,3,s8Gb2312Str,xf_row);

		row++;
	}

	// 保存文件
	ret = wb.Dump(ps8SavePath);
	if(ret != 0)
	{
		LOG_ERROR_FMT("xlsWorkbookDump %s Failed",ps8SavePath);
		return FAILURE;
	}
	return SUCCESS;
}
