#ifndef __AGCM_RECRUIT_H__
#define __AGCM_RECRUIT_H__

#include <AgcModule/AgcModule.h>
#include <ApBase/ApBase.h>
#include <AgpmCharacter/AgpdCharacter.h>
#include <AgpmItem/AgpdItem.h>
#include <AgcmRecruit/AcUIRecruit.h>
#include <AgcmRecruit/AcUIRecruitWrite.h>

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmRecruitD" )
#else
#pragma comment ( lib , "AgcmRecruit" )
#endif
#endif

class AgcmRecruit : public AgcModule
{
	INT32				m_lControlServerID;
	INT16				m_nControlServerNID;
	INT16				m_nRecruitServerNID;

	class ApmMap				*m_papmMap;
	class AgpmCharacter			*m_pagpmCharacter;
	class AgcmCharacter			*m_pagcmCharacter;
	class AgpmFactors			*m_pagpmFactors;
	class AgpmItem				*m_pagpmItem;
	class AgcmConnectManager	*m_pagcmConnectManager;

	AuPacket			m_csPacket;   //Recruit ��Ŷ.
	AuPacket			m_csLFPInfo;  //Looking for Party
	AuPacket			m_csLFMInfo;  //Looking for Member
	AuPacket			m_csSearchInfo; //ã�⸦ �Ҷ� ���Ǵ� ��Ŷ.

	//AcUIBbs				m_clBbs;
	BOOL				m_bBBSWindowShow;
	bool				m_bSearchByMyLevel;			// ã�⸦ ������ ���� ���ŵȴ� - ���� �˻� ���� ����
	INT32				m_lSelectedClass;			// ã�⸦ ������ ���� ���ŵȴ� - ���� �˻� ���� ����
	
public:

	AgcmRecruit();
	~AgcmRecruit();

	AcUIRecruit			m_clRecruitWindow			;
	AcUIRecruitWrite	m_clRecruitWriteWindow		;
	
	INT32 GetControlServerID();
	INT16 GetControlServerNID();
	INT16 GetRecruitServerNID();

	void SetControlServerID( INT32 nControlServerID );
	void SetControlServerNID( INT16 nControlServerNID );
	void SetRecruitServerNID( INT16 nRecruitServerNID );

	BOOL SendAddToRecruitServer( INT32 lCID );
	BOOL SendRegisterToLFP( INT32 lCID, char *pstrCharName );
	BOOL SendRegisterToLFM( INT32 lCID, char *pstrCharName, char *pstrPurpose, INT32 lRequireMember, INT32 lMinLV, INT32 lMaxLV, INT32 lClass );
	BOOL SendCancelFromLFP( INT32 lCID, char *pstrCharName );
	BOOL SendCancelFromLFM( INT32 lCID, char *pstrCharName );
	BOOL SendSearchLFPByPage( INT32 lCID, bool bSearchByMyLevel, INT32 lPage, INT32 lClass );
	BOOL SendSearchLFMByPage( INT32 lCID, bool bSearchByMyLevel, INT32 lPage, INT32 lClass );

	BOOL ProcessSearchLFPResult( void *pvPlayerInfo );
	BOOL ProcessSearchLFMResult( void *pvPlayerInfo );

	//��𿡳� �ִ� OnAddModule�̴�. Ư���Ұ� ����. Ư���ϸ� �ȵǳ�? ���d�d~
	BOOL OnAddModule();
	//��Ŷ�� �ް� �Ľ�����~
	BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	//2002_03_14 For BBS
	BOOL	OnInit();
	void	ShowBbsWindow( BOOL bShow );

	static
	BOOL	CBBbsClickContent( PVOID pClass, PVOID pData_1, PVOID pData_2, PVOID pData_3 );
	static
	BOOL	CBBbsChangePage( PVOID pClass, PVOID pData_1, PVOID pData_2, PVOID pData_3 );
	static
	BOOL	CBBbsWindowClose( PVOID pClass, PVOID pData_1, PVOID pData_2, PVOID pData_3 );

	
public:		// BBS Manage function
	void	ClickMercenaryBbsContent( UINT32 lIndex, UINT8 cRow, UINT8 cColumn );
	void	ClickPartyBbsContent( UINT32 lIndex, UINT8 cRow, UINT8 cColumn );

	// UI���� Function
	void	UIClickRegisterLFP()	;		// LFP ��� ��û
	void	UIClickDeleteLFP()		;		// LFP ���� ��û
	void	UIClickRegisterLFM( char *pstrPurpose, INT32 lRequireMember, INT32 lMinLV, INT32 lMaxLV, INT32 lClass );	// LFM ��� ��û 
	void	UIClickDeleteLFM()		;
	void	UILFPSearch( bool bSearchByMyLevel, INT32 lPage, INT32 lClass); 
	void	UILFMSearch( bool bSearchByMyLevel, INT32 lPage, INT32 lClass); 
	void	UILFPPageChange( INT32 lPage );
	void	UILFMPageChange( INT32 lPage );
};

#endif