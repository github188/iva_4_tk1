/**************************************************************************
# Copyright : JAYA	
# Name	    : AnalySet.cpp
# Describe  : 分析规则对象
# Date      : 12/2/2015
# Author    : libo
**************************************************************************/
#include "AnalySet.h"


ASPoint::ASPoint( int x /*= 0*/, int y /*= 0*/ )
{
	m_iX = x;
	m_iY = y;
}

ASPoint::~ASPoint( void )
{
	;// do nothing
}

ASCalibration::ASCalibration( void )
{
	m_fLength = 0.0f;
	m_fWidth = 0.0f;
	m_ptPoints = NULL;
}

ASCalibration::~ASCalibration( void )
{
	SafeDeleteS(m_ptPoints)
}

ASTargetFilter::ASTargetFilter( void )
{
	m_bEnable = false;
	m_ptMinPoints = NULL;
	m_ptMaxPoints = NULL;
}

ASTargetFilter::~ASTargetFilter( void )
{
	SafeDeleteS(m_ptMinPoints)
	SafeDeleteS(m_ptMaxPoints)
}
ASScene::ASScene( void )
{
	m_iScnID = -1;
	m_pTargetFilter = NULL;
	m_ptCalibration = NULL;
	m_ptShieldAreaSet = NULL;
	m_ptAnalyAreaSet = NULL;
	m_ptJudgeLineSet = NULL;
}

ASScene::~ASScene( void )
{
	SafeDeleteS(m_pTargetFilter);
	SafeDeleteS(m_ptCalibration);
	SafeDeleteS(m_ptShieldAreaSet);
	SafeDeleteS(m_ptAnalyAreaSet);
	SafeDeleteS(m_ptJudgeLineSet);
}

ASChannel::ASChannel( void )
{
	m_iChnID = -1;
	m_bOpenAlg = false;
	m_ptSceneSet = NULL;
}

ASChannel::~ASChannel( void )
{
	SafeDeleteS(m_ptSceneSet);
}
