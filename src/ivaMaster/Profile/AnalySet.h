/**************************************************************************
# Copyright : JAYA	
# Name	    : AnalySet.h
# Describe  : ������������
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

		SafeDeleteS(DelNode);// ��node����ʱ���ͷ�pt
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
	int m_iX;         //x����
	int m_iY;         //y����
};


// ���
typedef ASSet<ASPoint> ASMutiPoint;

// ��㼯��
typedef ASSet<ASMutiPoint> ASMutiPointSet;

// �����궨��Ϣ
class ASCalibration
{
public:
	ASCalibration(void);
	~ASCalibration(void);
public:
	float m_fLength;       // �����궨ʵ�ʳ���
	float m_fWidth;        // �����궨ʵ�ʿ��
	ASMutiPoint * m_ptPoints;  // �����궨��������
};

// Ŀ�����
class ASTargetFilter
{
public:
	ASTargetFilter(void);
	~ASTargetFilter(void);
public:
	int m_bEnable;
	ASMutiPoint * m_ptMinPoints;  // ��СĿ�����������
	ASMutiPoint * m_ptMaxPoints;  // ���Ŀ�����������
};

// ����
class ASScene
{
public:
	ASScene(void);
	~ASScene(void);
public:

	// ������Ϣ
	int m_iScnID;		// �������	

	// Ŀ�����
	ASTargetFilter * m_pTargetFilter;

	// �궨����
	ASCalibration  * m_ptCalibration;

	// ��������
	ASMutiPointSet * m_ptShieldAreaSet;

	// ��������
	ASMutiPointSet * m_ptAnalyAreaSet;

	// �ж���
	ASMutiPointSet * m_ptJudgeLineSet;
};

// ��������
typedef ASSet<ASScene> ASSceneSet;


// ͨ��
class ASChannel
{
public:
	ASChannel(void);
	~ASChannel(void);
public:
	// ͨ�����
	int m_iChnID;

	// �����㷨
	bool m_bOpenAlg;

	// ����
	ASSceneSet * m_ptSceneSet;
};

// ��������=ͨ������
typedef ASSet<ASChannel> AnalySet;


#endif //__ANALYSET_H__
