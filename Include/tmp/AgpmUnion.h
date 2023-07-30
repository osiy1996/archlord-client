#ifndef	__AGPMUNION_H__
#define	__AGPMUNION_H__

#include "ApBase.h"
#include "ApModule.h"

#include "AgpmFactors.h"
#include "AgpmCharacter.h"


#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgpmUnionD" )
#else
#pragma comment ( lib , "AgpmUnion" )
#endif
#endif


typedef enum	_AgpmUnionCB {
	AGPMUNION_CB_UPDATE_UNION_RANK				= 0,
	AGPMUNION_CB_UPDATE_UNION_RANK_POINT
} AgpmUnionCB;

// union rank
const int	AGPMUNION_MAX_UNION_TITLE			= 31;
const int	AGPMUNION_MAX_UNION_RANK			= 8;

class AgpdUnionRankTable
{
public:
	// ���� ����
	INT32					m_lRank;									// �ܰ�
	CHAR					m_szTitle[AGPMUNION_MAX_UNION_TITLE + 1];	// ��Ī
	INT32					m_lBonusCharisma;							// �ܰ迡 ���� ���ʽ� ī������
	INT32					m_lNeedRankPoint;							// �ʿ� ��ġ

	// ȹ�� ��ġ
	//INT32					m_lBonusRankPoint[6];						// UvU�� ȹ���ϴ� rank point
	ApSafeArray<INT32, 6>	m_lBonusRankPoint;

	// ��ġ ���
	INT32					m_lLoseRankPointByUvU;			// UvU���� �׾����� ���̴� ��ġ
	INT32					m_lLoseRankPointByNormal;		// �Ϲ����ΰ�� �׾����� ���̴� ��ġ

	INT32					m_lRestoreExpBySkull;			// ���� ���� ���� Exp
};

class AgpdUnionRank
{
public:
	//AgpdUnionRankTable		m_csUnionRankTable[AGPMUNION_MAX_UNION_RANK];
	ApSafeArray<AgpdUnionRankTable, AGPMUNION_MAX_UNION_RANK>	m_csUnionRankTable;
};

class AgpmUnion : public ApModule {
private:
	AgpmFactors			*m_pcsAgpmFactors;
	AgpmCharacter		*m_pcsAgpmCharacter;

public:
	AgpmUnion();
	~AgpmUnion();

	BOOL				OnAddModule();
	BOOL				OnInit();
	BOOL				OnDestroy();

	BOOL				StreamReadUnionRankTable(CHAR *szFile);

	//////////////////////////////////////////////////////////////////////
	// Union Rank (point) ó�� �Լ���
	//////////////////////////////////////////////////////////////////////
	INT32				GetUnionRankPoint(AgpdCharacter *pcsCharacter);
	BOOL				SetUnionRankPoint(AgpdCharacter *pcsCharacter, INT32 lNewUnionRank);
	BOOL				UpdateUnionRankPoint(AgpdCharacter *pcsCharacter, INT32 lUpdateRankPoint);

	INT32				GetUnionRank(AgpdCharacter *pcsCharacter);

	BOOL				SetCallbackUpdateUnionRank(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackUpdateUnionRankPoint(ApModuleDefaultCallBack pfCallback, PVOID pClass);
};

#endif	//__AGPMUNION_H__