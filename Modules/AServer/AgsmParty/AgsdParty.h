/******************************************************************************
Module:  AgsdParty.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2003. 02. 07
******************************************************************************/

#if !defined(__AGSDPARTY_H__)
#define __AGSDPARTY_H__

#include "ApBase.h"
#include "AgpmParty.h"
#include "AsDefine.h"

class AgsdParty {
public:
	DPNID					m_dpnidParty;							// ��Ƽ�� group id

	DPNID					m_dpnidPartyServer;						// ��Ƽ���� �����ִ� �������� group id
};

#endif //__AGSDPARTY_H__
