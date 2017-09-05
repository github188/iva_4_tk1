#include "AnalyXml.h"
#include "oal_regexp.h"
#include "oal_log.h"

XmlParser::XmlParser( void )
{
	;//do nothing
}

XmlParser::~XmlParser( void )
{
	;//do nothing
}

AnalySet * XmlParser::XmlParseASFile(const char * xmlpath)
{
	int iRet = -1;
	xmlDocPtr doc;
	xmlNodePtr root;
	AnalySet *pNewAS = NULL;

	if (xmlpath == NULL)
	{
		LOG_ERROR_FMT("[AS] xmlpath is NULL\n");
		return NULL;
	}

	// 解析XML
	doc = xmlParseFile(xmlpath);
	if(doc == NULL)
	{
		LOG_ERROR_FMT("[AS] xmlParseFile %s failed\n",xmlpath);
		return NULL;
	}

	// 获得根节点jayawh
	root = xmlDocGetRootElement(doc);
	if(root == NULL)
	{
		LOG_ERROR_FMT("[AS] root is null\n");
		goto ErrOut;
	}

	if(xmlStrcasecmp(root->name, TO_CAST(AS_XML_ROOT))!=0)
	{
		LOG_ERROR_FMT("[AS] root error,need %s,is %s\n",AS_XML_ROOT,root->name);
		goto ErrOut;
	}

	// 解析Channel集合，便可以得到AnalySet
	iRet = XmlParseChannelSet(&pNewAS,root);
	
	
ErrOut:

	// 释放XmlDOC
	if (doc)
	{
		xmlFreeDoc(doc);
		doc = NULL;
	}

	return pNewAS;
}

int XmlParser::XmlSaveAsFile(AnalySet * as,const char * xmlpath)
{
	if (as == NULL || xmlpath == NULL)
	{
		LOG_ERROR_FMT("[AS] input is NULL\n");
		return -1;
	}

	if (as->GetCnt() < 1)
	{
		LOG_ERROR_FMT("[AS] No data\n");
		return -1;
	}

	xmlDocPtr  doc = NULL;
	xmlNodePtr root;

	doc = xmlNewDoc(TO_CAST("1.0"));
	if (doc == NULL)
	{
		LOG_ERROR_FMT("[AS] xmlNewDoc Failed\n");
		return -1;
	}

	// 创建根节点
	root = xmlNewNode(NULL, TO_CAST(AS_XML_ROOT));
	xmlDocSetRootElement(doc, root);

	// 设置跟节点属性
	xmlNewPropInt(root,"Cnt",as->GetCnt());

	// 添加所有的通道节点
	XmlSaveSet(as,root,CHN_SET);

	// 保存
	xmlSaveFormatFileEnc(xmlpath, doc, "UTF-8", 1);

	// 释放
	xmlFreeDoc(doc);

	xmlCleanupParser();

	xmlMemoryDump();//debug memory for regression tests

	return(0);
}

int XmlParser::XmlParseChannelSet(AnalySet ** pptSet,xmlNodePtr root )
{
	int iRet = -1;
	AnalySet * ptSet = NULL;
	xmlNodePtr node;
	const char * child_name = "Channel";

	// 获取数量
	int iCnt = XmlParseSetChildCnt(root, child_name);
	if (iCnt < 1)
	{
		return -1;
	}

	// 创建分析规则对象
	ptSet = new AnalySet();
	assert(ptSet);

	// 遍历CHN
	int iOkCnt = 0;
	for(node = root->children; node && iOkCnt < iCnt; node = node->next)
	{
		if(xmlStrcasecmp(node->name, TO_CAST(child_name))== 0)
		{
			// 解析集合信息
			ASChannel * ptT = NULL;
			iRet = XmlParseChannel(&ptT, node);
			if (iRet == 0)
			{
				// 记录解析成功的子节点
				iOkCnt++;
				ptSet->Add(ptT);
			}
		}
		else
		{
			;//printf("[AS] Unknown node [%s] in [%s]\n",BK_CAST(node->name),BK_CAST(root->name));
		}
	}

	if (iOkCnt < 1)
	{
		LOG_ERROR_FMT("[AS] No valid [%s] node in [%s]\n",child_name,BK_CAST(root->name));
		SafeDeleteS(ptSet);
		iRet = 0;//-1;
	}
	else if (iOkCnt <= iCnt)
	{
		iRet = 0;//OK
		*pptSet = ptSet;
		//printf("[AS] %d/%d [%s] node in [%s]\n",iOkCnt,iCnt,child_name,BK_CAST(root->name));
	}

	return iRet;
}

int XmlParser::XmlParseChnSceneSet(ASSceneSet ** pptSet,xmlNodePtr root )
{
	int iRet = -1;
	ASSceneSet * ptSet = NULL;
	xmlNodePtr node;
	const char * child_name = "Scene";

	// 获取数量
	int iCnt = XmlParseSetChildCnt(root, child_name);
	if (iCnt < 1)
	{
		return -1;
	}

	// 创建分析规则对象
	ptSet = new ASSceneSet();
	assert(ptSet);

	// 遍历CHN
	int iOkCnt = 0;
	for(node = root->children; node && iOkCnt < iCnt; node = node->next)
	{
		if(xmlStrcasecmp(node->name, TO_CAST(child_name))== 0)
		{
			// 解析集合信息
			ASScene * ptT = NULL;
			iRet = XmlParseChnScene(&ptT, node);
			if (iRet == 0)
			{
				// 记录解析成功的子节点
				iOkCnt++;
				ptSet->Add(ptT);
			}
		}
		else
		{
			;//printf("[AS] Unknown node [%s] in [%s]\n",BK_CAST(node->name),BK_CAST(root->name));
		}
	}

	if (iOkCnt < 1)
	{
		printf("[AS] No valid [%s] node in [%s]\n",child_name,BK_CAST(root->name));
		SafeDeleteS(ptSet);
		iRet = 0;//-1;
	}
	else if (iOkCnt <= iCnt)
	{
		iRet = 0;//OK
		*pptSet = ptSet;
		//printf("[AS] %d/%d [%s] node in [%s]\n",iOkCnt,iCnt,child_name,BK_CAST(root->name));
	}

	return iRet;
}

int XmlParser::XmlParseChnSceneMutiPointSet(ASMutiPointSet ** pptSet,xmlNodePtr root ,const char *child_name)
{
	int iRet = -1;
	ASMutiPointSet * ptSet = NULL;
	xmlNodePtr node;

	// 获取数量
	int iCnt = XmlParseSetChildCnt(root, child_name);
	if (iCnt < 1)
	{
		return -1;
	}

	// 创建分析规则对象
	ptSet = new ASMutiPointSet();
	assert(ptSet);

	// 遍历CHN
	int iOkCnt = 0;
	for(node = root->children; node && iOkCnt < iCnt; node = node->next)
	{
		if(xmlStrcasecmp(node->name, TO_CAST(child_name))== 0)
		{
			// 解析集合信息
			ASMutiPoint * ptT = NULL;
			iRet = XmlParseChnScenePointSet(&ptT, node);
			if (iRet == 0)
			{
				// 记录解析成功的子节点
				iOkCnt++;
				ptSet->Add(ptT);
			}
		}
		else
		{
			;//printf("[AS] Unknown node [%s] in [%s]\n",BK_CAST(node->name),BK_CAST(root->name));
		}
	}

	if (iOkCnt < 1)
	{
		printf("[AS] No valid [%s] node in [%s]\n",child_name,BK_CAST(root->name));
		SafeDeleteS(ptSet);
		iRet = 0;//-1;
	}
	else if (iOkCnt <= iCnt)
	{
		iRet = 0;//OK
		*pptSet = ptSet;
		//printf("[AS] %d/%d [%s] node in [%s]\n",iOkCnt,iCnt,child_name,BK_CAST(root->name));
	}

	return iRet;
}

int XmlParser::XmlParseChnScenePointSet(ASMutiPoint ** pptSet,xmlNodePtr root)
{
	int iRet = -1;
	ASMutiPoint * ptSet = NULL;
	xmlNodePtr node;
	const char * child_name = "point";

	// 获取数量
	int iCnt = XmlParseSetChildCnt(root, child_name);
	if (iCnt < 1)
	{
		return -1;
	}

	// 创建分析规则对象
	ptSet = new ASMutiPoint();
	assert(ptSet);

	// 遍历CHN
	int iOkCnt = 0;
	for(node = root->children; node && iOkCnt < iCnt; node = node->next)
	{
		if(xmlStrcasecmp(node->name, TO_CAST(child_name))== 0)
		{
			// 解析集合信息
			ASPoint * ptT = NULL;
			iRet = XmlParseChnScenePoint(&ptT, node);
			if (iRet == 0)
			{
				// 记录解析成功的子节点
				iOkCnt++;
				ptSet->Add(ptT);
			}
		}
		else
		{
			;//printf("[AS] Unknown node [%s] in [%s]\n",BK_CAST(node->name),BK_CAST(root->name));
		}
	}

	if (iOkCnt < 1)
	{
		printf("[AS] No valid [%s] node in [%s]\n",child_name,BK_CAST(root->name));
		SafeDeleteS(ptSet);
		iRet = 0;//-1;
	}
	else if (iOkCnt <= iCnt)
	{
		iRet = 0;//OK
		*pptSet = ptSet;
		//printf("[AS] %d/%d [%s] node in [%s]\n",iOkCnt,iCnt,child_name,BK_CAST(root->name));
	}

	return iRet;
}

int XmlParser::XmlParseSetChildCnt( xmlNodePtr root, const char * child_name )
{
	if (root == NULL || child_name == NULL)
	{
		return 0;
	}

	// 获取实际的数量
	int iCnt = 0;
	xmlNodePtr child;
	for(child = root->children; child; child = child->next)
	{
		if(xmlStrcasecmp(child->name, TO_CAST(child_name))==0)
		{
			iCnt++;
		}
	}
	//printf("[AS] Found %d [%s] Node in [%s]\n",iCnt, child_name, TO_CAST(root->name));
	if (iCnt < 1)
	{
		return 0;
	}

	// 判断是否有属性控制
	int iChnCnt_Prop = xmlGetPropInt(root,"Cnt");
	if (iChnCnt_Prop > 0)
	{
		iCnt = MIN(iCnt,iChnCnt_Prop);
	}

	return iCnt;
}

int XmlParser::XmlParseChannel( ASChannel ** pptChn,xmlNodePtr root )
{
	int iRet = -1;
	xmlNodePtr node;

	if (pptChn == NULL || root == NULL)
	{
		return -1;
	}

	ASChannel * pt = new ASChannel();
	assert(pt);

	// 通道编号属性
	pt->m_iChnID = xmlGetPropInt(root,"ID");
	pt->m_bOpenAlg = (xmlGetPropInt(root,"OpenAlg") == 1 ? true: false);

	// 遍历通道子节点
	for(node = root->children; node; node=node->next)
	{
		if (xmlStrcasecmp(node->name,BAD_CAST("SceneInfo"))==0)
		{
			if (pt->m_ptSceneSet == NULL)
			{
				XmlParseChnSceneSet(&(pt->m_ptSceneSet),node);	
			}
			else
			{
				printf("[AS] Redundancy [%s] Node in [%s]\n",node->name, root->name);
			}
		}
		else
		{
			;//printf("[AS] Unknown node [%s] in [%s]\n",BK_CAST(node->name),BK_CAST(root->name));
		}

	}

	if (pt->m_ptSceneSet == NULL)
	{
		printf("[AS] Missing node [SceneInfo] in [%s]\n",BK_CAST(root->name));
	}

	*pptChn = pt;
	iRet = 0;
	return iRet;
}

int XmlParser::XmlParseChnScene( ASScene ** pptScene,xmlNodePtr root )
{
	if (pptScene == NULL || root == NULL)
	{
		return -1;
	}

	xmlChar * prop = NULL;
	ASScene * pt = new ASScene();
	assert(pt);

	// 属性
	pt->m_iScnID = xmlGetPropInt(root,"ID");

	// 解析子节点
	xmlNodePtr node;
	for (node = root->children; node; node = node->next)
	{
		if(xmlStrcasecmp(node->name,TO_CAST("TargetFilter"))==0)
		{
			if (pt->m_pTargetFilter == NULL)
			{
				XmlParseChnSceneTargetFilter(&(pt->m_pTargetFilter),node);
			}
			else
			{
				LOG_ERROR_FMT("[AS] Redundancy [%s] Node in [%s]\n",node->name, root->name);
			}
		}
		else if(xmlStrcasecmp(node->name,TO_CAST("Calibration"))==0)
		{
			if (pt->m_ptCalibration == NULL)
			{
				XmlParseChnSceneCalibration(&(pt->m_ptCalibration),node);
			}
			else
			{
				LOG_ERROR_FMT("[AS] Redundancy [%s] Node in [%s]\n",node->name, root->name);
			}
		}
		else if(xmlStrcasecmp(node->name,TO_CAST("SheildArea"))==0)
		{
			if (pt->m_ptShieldAreaSet == NULL)
			{
				XmlParseChnSceneMutiPointSet(&(pt->m_ptShieldAreaSet),node,"Area");
			}
			else
			{
				printf("[AS] Redundancy [%s] Node in [%s]\n",node->name, root->name);
			}
		}
		else if(xmlStrcasecmp(node->name,TO_CAST("AnalyArea"))==0)
		{
			if (pt->m_ptAnalyAreaSet == NULL)
			{
				XmlParseChnSceneMutiPointSet(&(pt->m_ptAnalyAreaSet),node,"Area");
			}
			else
			{
				printf("[AS] Redundancy [%s] Node in [%s]\n",node->name, root->name);
			}
		}
		else if(xmlStrcasecmp(node->name,TO_CAST("JudgeLine"))==0)
		{
			if (pt->m_ptJudgeLineSet == NULL)
			{
				XmlParseChnSceneMutiPointSet(&(pt->m_ptJudgeLineSet),node,"Area");
			}
			else
			{
				LOG_ERROR_FMT("[AS] Redundancy [%s] Node in [%s]\n",node->name, root->name);
			}
		}
		else
		{
			;//printf("[AS] Unknown node [%s] in [%s]\n",BK_CAST(node->name),BK_CAST(root->name));
		}
	}

	*pptScene = pt;
	return 0;
}

int XmlParser::XmlParseChnSceneTargetFilter( ASTargetFilter ** pptTargetFilter,xmlNodePtr root )
{
	int iRet = -1;
	if (pptTargetFilter == NULL || root == NULL)
	{
		return -1;
	}

	ASTargetFilter * pt = new ASTargetFilter();
	assert(pt);

	// 属性
	pt->m_bEnable = xmlGetPropInt(root,"Enable");

	// 解析子节点
	xmlNodePtr node;
	for (node = root->children; node; node = node->next)
	{
		if(xmlStrcasecmp(node->name,BAD_CAST"MinSize")==0)
		{
			if (pt->m_ptMinPoints == NULL)
			{
				XmlParseChnScenePointSet(&(pt->m_ptMinPoints),node);
				if (iRet == 0)
				{
					if (pt->m_ptMinPoints->GetCnt() != 2)
					{
						LOG_ERROR_FMT("[AS] [MinSize] node need 2 [point] child,but is %d\n",pt->m_ptMinPoints->GetCnt());
						SafeDeleteS(pt->m_ptMinPoints);
					}
				}
			}
			else
			{
				LOG_ERROR_FMT("[AS] Redundancy [%s] Node in [%s]\n",node->name, root->name);
			}
		}
		if(xmlStrcasecmp(node->name,BAD_CAST"MaxSize")==0)
		{
			if (pt->m_ptMaxPoints == NULL)
			{
				XmlParseChnScenePointSet(&(pt->m_ptMaxPoints),node);
				if (iRet == 0)
				{
					if (pt->m_ptMaxPoints->GetCnt() != 2)
					{
						LOG_ERROR_FMT("[AS] [MaxSize] node need 2 [point] child,but is %d\n",pt->m_ptMaxPoints->GetCnt());
						SafeDeleteS(pt->m_ptMaxPoints);
					}
				}
			}
			else
			{
				LOG_ERROR_FMT("[AS] Redundancy [%s] Node in [%s]\n",node->name, root->name);
			}
		}
		else
		{
			;//LOG_ERROR_FMT("[AS] Unknown node [%s] in [%s]\n",BK_CAST(node->name),BK_CAST(root->name));
		}
	}
	
	*pptTargetFilter = pt;
	return 0;
}

int XmlParser::XmlParseChnSceneCalibration( ASCalibration ** pptCalibration,xmlNodePtr root )
{
	int iRet = -1;
	if (pptCalibration == NULL || root == NULL)
	{
		return -1;
	}

	ASCalibration * pt = new ASCalibration();
	assert(pt);

	// 属性
	pt->m_fWidth = xmlGetPropFloat(root,"width");
	pt->m_fLength = xmlGetPropFloat(root,"length");

	// 解析子节点
	xmlNodePtr node;
	for (node = root->children; node; node = node->next)
	{
		if(xmlStrcasecmp(node->name,BAD_CAST"Coord")==0)
		{
			if (pt->m_ptPoints == NULL)
			{
				XmlParseChnScenePointSet(&(pt->m_ptPoints),node);
				if (iRet == 0)
				{
					if (pt->m_ptPoints->GetCnt() != 4)
					{
						LOG_ERROR_FMT("[AS] [Coord] node need 4 [point] child,but is %d\n",pt->m_ptPoints->GetCnt());
						SafeDeleteS(pt->m_ptPoints);
					}
				}
			}
			else
			{
				LOG_ERROR_FMT("[AS] Redundancy [%s] Node in [%s]\n",node->name, root->name);
			}
		}
		else
		{
			;//LOG_ERROR_FMT("[AS] Unknown node [%s] in [%s]\n",BK_CAST(node->name),BK_CAST(root->name));
		}
	}

	//if (pt->m_ptPoints)
	{
		*pptCalibration = pt;
		return 0;
	}
	//else
	//{
	//	SafeDeleteS(pt);
	//	return 0;//-1;
	//}
}

int XmlParser::XmlParseChnScenePoint( ASPoint ** pptPoint,xmlNodePtr root )
{
	if (pptPoint == NULL || root == NULL)
	{
		return -1;
	}

	ASPoint * pt = new ASPoint();
	assert(pt);

	// 属性
	pt->m_iX = xmlGetPropInt(root,"x");
	pt->m_iY = xmlGetPropInt(root,"y");

	*pptPoint = pt;
	return 0;
}

int XmlParser::XmlSaveSet( void * ptSet, xmlNodePtr root, int set )
{
	xmlNodePtr node = root;
	if (ptSet == NULL || root == NULL)
	{
		return -1;
	}

	AnalySet *pt1 = NULL;
	ASSceneSet *pt2 = NULL;
	ASMutiPointSet *pt3 = NULL;
	ASMutiPoint *pt4 = NULL;

	// 创建子节点  
	switch (set)
	{
	case CHN_SET:
		pt1 = (AnalySet *)ptSet;
		for (int i = 0; i < pt1->GetCnt(); i++)
		{
			XmlSaveChannel(pt1->GetAt(i),node);
		}
		break;
	case SCE_SET:
		pt2 = (ASSceneSet*)ptSet;
		for (int i = 0; i < pt2->GetCnt(); i++)
		{
			XmlSaveChnScene(pt2->GetAt(i),node);
		}
		break;
	case MUT_SET_AREA:
		pt3 = (ASMutiPointSet*)ptSet;
		for (int i = 0; i < pt3->GetCnt(); i++)
		{
			XmlSaveChnSceneMutiPoint(pt3->GetAt(i),node,set);
		}
		break;
	case POT_SET:
		pt4 = (ASMutiPoint*)ptSet;
		for (int i = 0; i < pt4->GetCnt(); i++)
		{
			XmlSaveChnScenePoint(pt4->GetAt(i),node);
		}
		break;
	default:
		break;
	}

		
	return 0;
}

int XmlParser::XmlSaveChannel( ASChannel * ptChn,xmlNodePtr root )
{
	xmlNodePtr node,child;

	if (ptChn == NULL || root == NULL)
	{
		return -1;
	}

	// 创建节点
	node = xmlNewNode(NULL, TO_CAST("Channel"));
	assert(node);
	xmlAddChild(root, node);

	// 设置属性
	xmlNewPropInt(node,"ID",ptChn->m_iChnID);
	xmlNewPropInt(node,"OpenAlg",ptChn->m_bOpenAlg?1:0);


	// 添加SceneInfo子节点
	if (ptChn->m_ptSceneSet)
	{
		child = xmlNewNode(NULL, BAD_CAST "SceneInfo");
		assert(child);
		xmlAddChild(node, child);

		xmlNewPropInt(child,"Cnt",ptChn->m_ptSceneSet->GetCnt());

		XmlSaveSet(ptChn->m_ptSceneSet,child,SCE_SET);
	}

	return 0;
}

int XmlParser::XmlSaveChnScene( ASScene * ptScene,xmlNodePtr root )
{
	xmlNodePtr node,child;

	if (ptScene == NULL || root == NULL)
	{
		return -1;
	}

	// 创建节点
	node = xmlNewNode(NULL, BAD_CAST "Scene");
	assert(node);
	xmlAddChild(root, node);

	// 设置属性
	xmlNewPropInt(node,"ID",ptScene->m_iScnID);

	// 添加TargetFilter子节点
	if (ptScene->m_pTargetFilter)
	{
		XmlSaveChnSceneTargetFilter(ptScene->m_pTargetFilter, node);
	}

	// 添加Calibration子节点
	if (ptScene->m_ptCalibration)
	{
		XmlSaveChnSceneCalibration(ptScene->m_ptCalibration,node);
	}

	// 添加SheildArea子节点
	if (ptScene->m_ptShieldAreaSet)
	{
		child = xmlNewNode(NULL, BAD_CAST "SheildArea");
		assert(child);
		xmlAddChild(node, child);

		xmlNewPropInt(child,"Cnt",ptScene->m_ptShieldAreaSet->GetCnt());

		XmlSaveSet(ptScene->m_ptShieldAreaSet,child,MUT_SET_AREA);
	}

	// 添加AnalyArea子节点
	if (ptScene->m_ptAnalyAreaSet)
	{
		child = xmlNewNode(NULL, BAD_CAST "AnalyArea");
		assert(child);
		xmlAddChild(node, child);

		xmlNewPropInt(child,"Cnt",ptScene->m_ptAnalyAreaSet->GetCnt());

		XmlSaveSet(ptScene->m_ptAnalyAreaSet,child,MUT_SET_AREA);
	}

	// 添加SolidLine子节点
	if (ptScene->m_ptJudgeLineSet)
	{
		child = xmlNewNode(NULL, BAD_CAST "JudgeLine");
		assert(child);
		xmlAddChild(node, child);

		xmlNewPropInt(child,"Cnt",ptScene->m_ptJudgeLineSet->GetCnt());

		XmlSaveSet(ptScene->m_ptJudgeLineSet,child,MUT_SET_AREA);
	}


	return 0;
}

int XmlParser::XmlSaveChnSceneTargetFilter( ASTargetFilter * ptTargetFilter, xmlNodePtr root )
{
	xmlNodePtr node,child;

	if (ptTargetFilter == NULL || root == NULL)
	{
		return -1;
	}

	// 创建节点
	node = xmlNewNode(NULL, BAD_CAST "TargetFilter");
	assert(node);
	xmlAddChild(root, node);

	// 设置属性
	xmlNewPropInt(node,"Enable",ptTargetFilter->m_bEnable);

	// 添加MinSize子节点
	if (ptTargetFilter->m_ptMinPoints)
	{
		child = xmlNewNode(NULL, BAD_CAST "MinSize");
		assert(child);
		xmlAddChild(node, child);

		xmlNewPropInt(child,"Cnt",ptTargetFilter->m_ptMinPoints->GetCnt());

		XmlSaveSet(ptTargetFilter->m_ptMinPoints,child, POT_SET);
	}

	// 添加MaxSize子节点
	if (ptTargetFilter->m_ptMaxPoints)
	{
		child = xmlNewNode(NULL, BAD_CAST "MaxSize");
		assert(child);
		xmlAddChild(node, child);

		xmlNewPropInt(child,"Cnt",ptTargetFilter->m_ptMaxPoints->GetCnt());

		XmlSaveSet(ptTargetFilter->m_ptMaxPoints,child, POT_SET);
	}
	return 0;
}

int XmlParser::XmlSaveChnSceneCalibration( ASCalibration * ptCalibration,xmlNodePtr root )
{
	xmlNodePtr node,child;

	if (ptCalibration == NULL || root == NULL)
	{
		return -1;
	}

	// 创建节点
	node = xmlNewNode(NULL, BAD_CAST "Calibration");
	assert(node);
	xmlAddChild(root, node);

	// 设置属性
	xmlNewPropFloat(node,"width",ptCalibration->m_fWidth);
	xmlNewPropFloat(node,"length",ptCalibration->m_fLength);

	// 添加Coord子节点
	if (ptCalibration->m_ptPoints)
	{
		child = xmlNewNode(NULL, BAD_CAST "Coord");
		assert(child);
		xmlAddChild(node, child);

		xmlNewPropInt(child,"Cnt",ptCalibration->m_ptPoints->GetCnt());

		XmlSaveSet(ptCalibration->m_ptPoints,child, POT_SET);
	}
	return 0;
}

int XmlParser::XmlSaveChnSceneMutiPoint( ASMutiPoint * ptMutiPoint, xmlNodePtr root, int set )
{
	xmlNodePtr node;
	const char * name = NULL;
	if (ptMutiPoint == NULL || root == NULL)
	{
		return -1;
	}

	switch (set)
	{
	case MUT_SET_AREA:
		name = "Area";
		break;
	default:
		break;
	}

	// 创建节点
	node = xmlNewNode(NULL, BAD_CAST name);
	assert(node);
	xmlAddChild(root, node);

	// 设置属性
	xmlNewPropInt(node,"Cnt",ptMutiPoint->GetCnt());

	// 添加point子节点
	XmlSaveSet(ptMutiPoint,node,POT_SET);
	return 0;
}

int XmlParser::XmlSaveChnScenePoint( ASPoint * ptPoint,xmlNodePtr root )
{
	xmlNodePtr node;

	if (ptPoint == NULL || root == NULL)
	{
		return -1;
	}

	// 创建节点
	node = xmlNewNode(NULL, BAD_CAST "point");
	assert(node);
	xmlAddChild(root, node);

	// 设置属性
	char cvalueX[128] = {0};
	sprintf(cvalueX,"%d",ptPoint->m_iX);
	xmlNewProp(node,TO_CAST("x"),TO_CAST(cvalueX));

	char cvalueY[128] = {0};
	sprintf(cvalueY,"%d",ptPoint->m_iY);
	xmlNewProp(node,TO_CAST("y"),TO_CAST(cvalueY));

	return 0;
}

int XmlParser::xmlGetPropInt(xmlNodePtr node,const char *name,int defualt)
{
	int iValue = defualt;
	if (node && name)
	{
		xmlChar * prop = xmlGetProp(node,TO_CAST(name));
		if (prop)
		{
			iValue = atoi(BK_CAST(prop));
			xmlFree(prop);
			prop = NULL;
		}
	}

	return iValue;
	
}

float XmlParser::xmlGetPropFloat( xmlNodePtr node,const char *name, float defualt )
{
	float fValue = defualt;
	if (node && name)
	{
		xmlChar * prop = xmlGetProp(node,TO_CAST(name));
		if (prop)
		{
			fValue = (float)atof(BK_CAST(prop));
			xmlFree(prop);
			prop = NULL;
		}
	}

	return fValue;
}

int XmlParser::xmlGetElementValueInt( xmlNodePtr node, int defualt )
{
	xmlNodePtr child;
	int iValue = defualt;
	if (node)
	{
		for (child = node->children;child;child = child->next)
		{
			if (xmlStrcasecmp(child->name,TO_CAST("text"))==0)
			{
				if (child->content)
				{
					iValue = atoi(BK_CAST(child->content));
					return iValue;
				}
			}
		}
	}
	return iValue;
}

float XmlParser::xmlGetElementValueFloat( xmlNodePtr node, float defualt )
{
	xmlNodePtr child;
	float fValue = defualt;
	if (node)
	{
		for (child = node->children;child;child = child->next)
		{
			if (xmlStrcasecmp(child->name,TO_CAST("text"))==0)
			{
				if (child->content)
				{
					fValue = (float)atof(BK_CAST(child->content));
					return fValue;
				}
			}
		}
	}
	return fValue;
}

void XmlParser::xmlGetElementValueString( xmlNodePtr node, string & strValue)
{
	xmlNodePtr child;
	strValue = "";
	if (node)
	{
		for (child = node->children;child;child = child->next)
		{
			if (xmlStrcasecmp(child->name,TO_CAST("text"))==0)
			{
				if (child->content)
				{
					strValue = BK_CAST(child->content);
					return;
				}
			}
		}
	}
}

xmlAttrPtr XmlParser::xmlNewPropInt( xmlNodePtr node, const char *name, int value )
{
	char cIvalue[128] = {0};
	if (node && name)
	{
		sprintf(cIvalue,"%d",value);
		return xmlNewProp(node,TO_CAST(name),TO_CAST(cIvalue));
	}

	return NULL;
}

xmlAttrPtr XmlParser::xmlNewPropFloat( xmlNodePtr node, const char *name, float value )
{
	char cIvalue[128] = {0};
	if (node && name)
	{
		sprintf(cIvalue,"%g",value);
		return xmlNewProp(node,TO_CAST(name),TO_CAST(cIvalue));
	}

	return NULL;
}

xmlNodePtr XmlParser::xmlNewChildInt( xmlNodePtr node, const char *name, int value )
{
	char cIvalue[128] = {0};
	if (node && name)
	{
		sprintf(cIvalue,"%d",value);
		return xmlNewChild(node,NULL,TO_CAST(name),TO_CAST(cIvalue));
	}

	return NULL;
}

xmlNodePtr XmlParser::xmlNewChildFloat( xmlNodePtr node, const char *name, float value )
{
	char cIvalue[128] = {0};
	if (node && name)
	{
		sprintf(cIvalue,"%g",value);
		return xmlNewChild(node,NULL,TO_CAST(name),TO_CAST(cIvalue));
	}

	return NULL;
}



