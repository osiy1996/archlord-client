/******************************************************************************
Module:  AgsdFactors.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 10. 16
******************************************************************************/

#if !defined(__AGSDFACTORS_H__)
#define __AGSDFACTORS_H__

#include "ApBase.h"
#include "AsDefine.h"

#include "AgpmFactors.h"

class AgsdFactors {
public:
	UINT32				m_ulDeadTime;		// ���� �ð� (game clock count ����)
											// (�ױ� ���ð�, ������ �����ð� : m_fCharStatus�� ���� �����ϴ� ���� �޶�����.)

	INT16				m_nLastLoseExp;		// ���������� ���� Exp (������ ���� �������� Exp ���� ������ ���ȴ�.)

	pstHITHISTORY		m_pstHitHistory;	// Hit History
};

#endif //__AGSDFACTORS_H__