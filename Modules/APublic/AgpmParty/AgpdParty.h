/******************************************************************************
Module:  AgpdParty.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2003. 01. 27
******************************************************************************/

#if !defined(__AGPDPARTY_H__)
#define __AGPDPARTY_H__

#include <ApBase/ApBase.h>
#include <AgpmFactors/AgpdFactors.h>

typedef enum _eAGPMPARTY_DATA_TYPE {
	AGPMPARTY_DATA_TYPE_PARTY			= 0,
} eAGPMPARTY_DATA_TYPE;

enum	AgpmPartyCalcExpType {
//	AGPMPARTY_EXP_TYPE_BY_NONE				= 0,
	AGPMPARTY_EXP_TYPE_BY_DAMAGE			=	0,
	AGPMPARTY_EXP_TYPE_BY_LEVEL,
//	AGPMPARTY_EXP_TYPE_BY_COMPLEX,
};

//	2005.04.15	Start By SungHoon
//	��Ƽ�ɼ��� ������ȹ�濡 ���� Option Enum
enum	AgpmPartyOptionDivisionItem {
//	AGPMPARTY_DIVISION_ITEM_NONE			= 0,
	AGPMPARTY_DIVISION_ITEM_SEQUENCE		=	0,				// ������ ȹ��
	AGPMPARTY_DIVISION_ITEM_FREE,								// ���� ȹ��
	AGPMPARTY_DIVISION_ITEM_DAMAGE								// ���� ȹ��
} ;
//	2005.04.15	Finish By SungHoon

//	AgpdParty class (data only class)
///////////////////////////////////////////////////////////////////////////////
class AgpdParty : public ApBase {
public:
	INT16					m_nMaxMember;			// �ִ� ��Ƽ ���� ���� �ο�
	INT16					m_nCurrentMember;		// ���� ��Ƽ ���� �ο�

	// ���� �տ� �ִ� ���� ������ �ȴ�.
//	INT32					m_lMemberListID[AGPMPARTY_MAX_PARTY_MEMBER];
//	AgpdCharacter*			m_pcsMemberList[AGPMPARTY_MAX_PARTY_MEMBER];

	ApSafeArray<INT32, AGPMPARTY_MAX_PARTY_MEMBER>				m_lMemberListID;
	//ApSafeArray<AgpdCharacter*, AGPMPARTY_MAX_PARTY_MEMBER>		m_pcsMemberList;

	INT32					m_lHighestMemberLevel;	// �� ��Ƽ ����� ������ �� �������� ����
	INT32					m_lLowerMemberLevel;	// �� ��Ƽ ����� ������ �� �������� ����
	INT32					m_lTotalMemberLevel;	// �� ��Ƽ ������� ���� ����

	INT32					m_lHighestMemberUnionRank;	// �� ��Ƽ ����� ���Ͽ·�ũ�� �� �������� ��ũ

	UINT32					m_ulRemoveTimeMSec;

	//BOOL					m_bIsInEffectArea[AGPMPARTY_MAX_PARTY_MEMBER];
	ApSafeArray<BOOL, AGPMPARTY_MAX_PARTY_MEMBER>				m_bIsInEffectArea;

	AgpmPartyCalcExpType	m_eCalcExpType;

//	2005.04.15	Start By SungHoon
//	������ ������ ȹ�濡 �ʿ��� ����
	INT32					m_lCurrentGetItemCID;
	INT32					m_lFinishGetItemCID;

	AgpmPartyOptionDivisionItem	m_eDivisionItem;
//	2005.04.15	Finish
//
};

//	AgpdPartyADChar class (data class)
//		- character ��⿡ �ٿ� ������ ����Ÿ
///////////////////////////////////////////////////////////////////////////////
class AgpdPartyADChar {
public:
	INT32		lPID;								// party id
	AgpdParty	*pcsParty;							// party data pointer

	AgpdFactor	m_csFactorPoint;
};

#endif //__AGPDPARTY_H__
