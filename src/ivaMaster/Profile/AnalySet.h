/**************************************************************************
# Copyright : JAYA	
# Name	    : AnalySet.h
# Describe  : 分析规则配置
# Date      : 12/2/2015
# Author    : libo
**************************************************************************/
#ifndef __ANALYSET_H__
#define __ANALYSET_H__

#include <string>
#include <assert.h>
using namespace std;


#define SafeDeleteS(p)	if (p)\
{\
	delete p;\
	p = NULL;\
}

#define SafeDeleteM(p)	if (p)\
{\
	delete [] p;\
	p = NULL;\
}


template <class T> 
class PtrNode
{
public:
	PtrNode<T>(){pt = NULL;	next = NULL;};
	~PtrNode<T>(void){	SafeDeleteS(pt);};

public:
	T * pt;
	PtrNode<T> * next;
};


template <class T> 
class ASSet
{
public:
	ASSet<T>(){m_list = NULL;};
	~ASSet<T>(void){DelAll();};
public:
	T * GetAt(int i)
	{
		T* pt = NULL;
		PtrNode<T> *node = m_list;
		int at = -1;
		while (node)
		{
			at++;		
			if (at == i)
			{
				pt = (T*)node->pt;
				break;
			}
			node = node->next;
		}

		return pt;
	};

	int Add(T *data)
	{

		PtrNode<T> *newnode = new PtrNode<T>();
		assert(newnode);

		newnode->pt = data;

		if (m_list == NULL)
		{
			m_list = newnode;
		}
		else
		{
			PtrNode<T> *node = m_list;
			while (node->next)
			{
				node = node->next;
			}

			node->next = newnode;
		}
		return 0;
	};
	int Del(int i)
	{
		if (i < 0 || i > GetCnt()-1)
		{
			return -1;
		}

		PtrNode<T> *node = m_list;
		PtrNode<T> *DelNode = NULL;
		PtrNode<T> *PreNode = NULL;
		int at = -1;
		while (node)
		{
			at++;		
			if (at == i)
			{
				DelNode = node;
				break;
			}
			else
			{
				PreNode = node;
			}
			node = node->next;
		}

		if (DelNode == NULL)
		{
			return -1;
		}

		if (PreNode == NULL)
		{
			m_list = m_list->next;
		}
		else
		{
			PreNode->next = DelNode->next;
		}

		SafeDeleteS(DelNode);// 在node析构时会释放pt
		return 0;
	};
	int GetCnt()
	{
		PtrNode<T> *node = m_list;
		int at = 0;
		while (node)
		{
			at++;		
			node = node->next;
		}
		return at;
	};
	void DelAll()
	{
		while (GetCnt() > 0)
		{
			Del(0);
		}
	};


private:
	PtrNode<T> * m_list;
};


class ASPoint
{
public:
	ASPoint(int x = 0, int y = 0);
	~ASPoint(void);
public:
	int m_iX;         //x坐标
	int m_iY;         //y坐标
};


// 多点
typedef ASSet<ASPoint> ASMutiPoint;

// 多点集合
typedef ASSet<ASMutiPoint> ASMutiPointSet;

// 场景标定信息
class ASCalibration
{
public:
	ASCalibration(void);
	~ASCalibration(void);
public:
	float m_fLength;       // 场景标定实际长度
	float m_fWidth;        // 场景标定实际宽度
	ASMutiPoint * m_ptPoints;  // 场景标定顶点坐标
};

// 目标过滤
class ASTargetFilter
{
public:
	ASTargetFilter(void);
	~ASTargetFilter(void);
public:
	int m_bEnable;
	ASMutiPoint * m_ptMinPoints;  // 最小目标的两个顶点
	ASMutiPoint * m_ptMaxPoints;  // 最大目标的两个顶点
};

// 场景
class ASScene
{
public:
	ASScene(void);
	~ASScene(void);
public:

	// 基本信息
	int m_iScnID;		// 场景编号	

	// 目标过滤
	ASTargetFilter * m_pTargetFilter;

	// 标定场景
	ASCalibration  * m_ptCalibration;

	// 屏蔽区域
	ASMutiPointSet * m_ptShieldAreaSet;

	// 分析区域
	ASMutiPointSet * m_ptAnalyAreaSet;

	// 判定线
	ASMutiPointSet * m_ptJudgeLineSet;
};

// 场景集合
typedef ASSet<ASScene> ASSceneSet;


// 通道
class ASChannel
{
public:
	ASChannel(void);
	~ASChannel(void);
public:
	// 通道编号
	int m_iChnID;

	// 开启算法
	bool m_bOpenAlg;

	// 场景
	ASSceneSet * m_ptSceneSet;
};

// 分析规则=通道集合
typedef ASSet<ASChannel> AnalySet;


#endif //__ANALYSET_H__
