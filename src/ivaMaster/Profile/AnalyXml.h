/**************************************************************************
# Copyright : JAYA	
# Name	    : XmlParser.h
# Describe  : XML配置文件解析类
# Date      : 12/3/2015
# Author    : libo
**************************************************************************/
#ifndef __AS_XMLPARSE_H__
#define __AS_XMLPARSE_H__

#include "AnalySet.h"
#include <libxml/parser.h>
#include <libxml/tree.h>

#define AS_XML_ROOT	 "JOKER"
#define TO_CAST(str) (xmlChar *)(str)
#define BK_CAST(str) (char *)(str)

#define CHN_SET			1
#define SCE_SET			2
#define MUT_SET_AREA	3
#define POT_SET			4

// XML解析类
class XmlParser
{
public:
	XmlParser(void);
	~XmlParser(void);

public:// 接口函数
	static AnalySet * XmlParseASFile(const char * xmlpath);
	static int XmlSaveAsFile(AnalySet * as,const char * xmlpath);

private:// 解析函数
	static int XmlParseChannelSet( AnalySet ** pptSet,xmlNodePtr root );
	static int XmlParseChannel( ASChannel ** pptChn,xmlNodePtr root );
	static int XmlParseChnSceneSet( ASSceneSet ** pptSet,xmlNodePtr root );
	static int XmlParseChnScene( ASScene ** pptScene,xmlNodePtr root );
	static int XmlParseChnSceneTargetFilter( ASTargetFilter ** pptTargetFilter,xmlNodePtr root );
	static int XmlParseChnSceneCalibration( ASCalibration ** pptCalibration,xmlNodePtr root );
	static int XmlParseChnSceneMutiPointSet( ASMutiPointSet ** pptSet,xmlNodePtr root ,const char *child_name );
	static int XmlParseChnScenePointSet(ASMutiPoint ** pptSet,xmlNodePtr root );
	static int XmlParseChnScenePoint( ASPoint ** pptPoint,xmlNodePtr root );

	static int XmlParseSetChildCnt( xmlNodePtr root, const char * child_name);


private:// 保存函数
	static int XmlSaveSet( void * ptSet, xmlNodePtr root, int set );
	static int XmlSaveChannel( ASChannel * ptChn,xmlNodePtr root );
	static int XmlSaveChnScene( ASScene * ptScene,xmlNodePtr root );
	static int XmlSaveChnSceneTargetFilter( ASTargetFilter * ptTargetFilter,xmlNodePtr root );
	static int XmlSaveChnSceneCalibration( ASCalibration * ptCalibration,xmlNodePtr root );
	static int XmlSaveChnSceneMutiPoint( ASMutiPoint * ptMutiPoint, xmlNodePtr root, int set );
	static int XmlSaveChnScenePoint( ASPoint * ptPoint,xmlNodePtr root );

private:
	static int   xmlGetPropInt( xmlNodePtr node,const char *name, int defualt = -1);
	static float xmlGetPropFloat( xmlNodePtr node,const char *name, float defualt = 0.0f);
	static int   xmlGetElementValueInt( xmlNodePtr node, int defualt = -1);
	static float xmlGetElementValueFloat( xmlNodePtr node, float defualt = 0.0f);
	static void  xmlGetElementValueString( xmlNodePtr node, string & strValue);

	static xmlAttrPtr xmlNewPropInt( xmlNodePtr node, const char *name, int value );
	static xmlAttrPtr xmlNewPropFloat( xmlNodePtr node, const char *name, float value );
	static xmlNodePtr xmlNewChildInt( xmlNodePtr node, const char *name, int value );
	static xmlNodePtr xmlNewChildFloat( xmlNodePtr node, const char *name, float value );
};


#endif //__AS_XMLPARSE_H__
