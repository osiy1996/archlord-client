// AgsmMakeSQL.h: interface for the AgsmMakeSQL class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGSMMAKESQL_H__3B98E9D0_C704_4334_8F65_7967158CF58D__INCLUDED_)
#define AFX_AGSMMAKESQL_H__3B98E9D0_C704_4334_8F65_7967158CF58D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApBase.h"
#include "AgsEngine.h"

#include "AgpmAdmin.h"	// 2004.03.30. steeple

#include "AgsmLog.h"
#include "AgsmAdmin.h"	// 2004.03.30. steeple
#include "AgsmDBPool.h"
#include "AgsmRelay.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmMakeSQLD" )
#else
#pragma comment ( lib , "AgsmMakeSQL" )
#endif
#endif

class AgsmMakeSQL : public AgsModule
{
private:
	AgsmLog*		m_pAgsmLog;
	AgsmAdmin*		m_pAgsmAdmin;
	AgsmDBPool*		m_pAgsmDBPool;
	AgsmRelay*		m_pAgsmRelay;

public:
	AgsmMakeSQL();
	virtual ~AgsmMakeSQL();

	BOOL OnAddModule();
	BOOL OnInit();
	BOOL OnDestroy();

	/*
	// Relay Server���� ������ ���� SQL�� ������ DB ��������
	BOOL ItemInsert(AgsdRelayItem *pAgsdRelayItem);
	BOOL ItemUpdate(AgsdRelayItem *pAgsdRelayItem);
	BOOL ItemDelete(AgsdRelayItem *pAgsdRelayItem);

	BOOL ItemConvertHistoryInsert(AgsdRelayItemConvertHistory *pAgsdRelayItemConvertHistory);
	BOOL ItemConvertHistoryDelete(AgsdRelayItemConvertHistory *pAgsdRelayItemConvertHistory);

	// Relay Server���� ĳ���� ���� SQL�� ������ DB ��������
	BOOL CharacterInsert(AgsdRelayCharacter* pAgsdRelayCharacter);
	BOOL CharacterUpdate(AgsdRelayCharacter* pAgsdRelayCharacter);
	BOOL CharacterDelete(AgsdRelayCharacter* pAgsdRelayCharacter);
	BOOL CharacterSelect(AgsdRelayCharacter* pAgsdRelayCharacter);
	
	// Relay Server���� ĳ���� Ban ���� SQL�� ������ DB �������� - 2004.03.30. 
	BOOL CharacterBanUpdate(stAgpdAdminBan* pstBan);

	// Relay Server���� ��ų ���� SQL�� ������ DB ��������
	BOOL SkillInsert(AgsdRelaySkill* pAgsdRelaySkill);
	BOOL SkillUpdate(AgsdRelaySkill* pAgsdRelaySkill);
	BOOL SkillDelete(AgsdRelaySkill* pAgsdRelaySkill);
	*/

	BOOL ItemInsert3(AgsdRelayItem2 *pAgsdRelayItem);
	BOOL ItemUpdate3(AgsdRelayItem2 *pAgsdRelayItem);
	BOOL ItemDelete3(AgsdRelayItem2 *pAgsdRelayItem);

	BOOL ItemConvertHistoryUpdate(AgsdRelayItemConvertHistory2 *pAgsdRelayItemConvertHistory);

	// Relay Server���� ĳ���� ���� SQL�� ������ DB ��������
	BOOL CharacterInsert3(AgsdRelayCharacter2* pAgsdRelayCharacter);
	BOOL CharacterUpdate3(AgsdRelayCharacter2* pAgsdRelayCharacter);
	BOOL CharacterDelete3(AgsdRelayCharacter2* pAgsdRelayCharacter);
	BOOL CharacterSelect3(AgsdRelayCharacter2* pAgsdRelayCharacter);

	BOOL AccountWorldUpdate(AgsdRelayAccountWorld* pstAccountWorld);
	
	// Relay Server���� ĳ���� Ban ���� SQL�� ������ DB �������� - 2004.03.30. 
	BOOL CharacterBanUpdate3(stAgpdAdminBan* pstBan);

	// Relay Server���� ��ų ���� SQL�� ������ DB ��������
	BOOL SkillInsert3(AgsdRelaySkill2* pAgsdRelaySkill);
	BOOL SkillUpdate3(AgsdRelaySkill2* pAgsdRelaySkill);
	BOOL SkillDelete3(AgsdRelaySkill2* pAgsdRelaySkill);

	BOOL UIStatusUpdate3(AgsdRelayUIStatus2* pAgsdRelayUIStatus);

	// AgsmDBPool�� ���ؼ� Select ����� ���� callback �Լ���
	static BOOL CBCharacterSelect(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBItemSelect(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBSkillSelect(PVOID pData, PVOID pClass, PVOID pCustData);
	
	/*
	// AgsmRelay�� ���� ���ŵ� �����͸� ���� callback �Լ���
	static BOOL CBCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBItem(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBItemConvertHistory(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBSkill(PVOID pData, PVOID pClass, PVOID pCustData);
	*/

	static BOOL CBCharacter2(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBItem2(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBItemConvertHistory2(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBSkill2(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBUIStatus2(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBAccountWorld2(PVOID pData, PVOID pClass, PVOID pCustData);

	// AgsmAdmin�� ���� ���ŵ� �����͸� ���� Callback �Լ��� - 2004.03.30. steeple
	static BOOL CBCharacterBan(PVOID pData, PVOID pClass, PVOID pCustData);
	

	

	//////////////////////////////////////////////////////////////////////////
	// Guild ����
	BOOL GuildMasterInsert2(AgsdRelayGuildMaster2* pcsGuildMaster);
	BOOL GuildMasterUpdate2(AgsdRelayGuildMaster2* pcsGuildMaster);
	BOOL GuildMasterDelete2(AgsdRelayGuildMaster2* pcsGuildMaster);
	BOOL GuildMasterSelect2(AgsdRelayGuildMaster2* pcsGuildMaster);
	BOOL GuildMemberInsert2(AgsdRelayGuildMember2* pcsGuildMember);
	BOOL GuildMemberUpdate2(AgsdRelayGuildMember2* pcsGuildMember);
	BOOL GuildMemberDelete2(AgsdRelayGuildMember2* pcsGuildMember);
	BOOL GuildMemberSelect2(AgsdRelayGuildMember2* pcsGuildMember);

	// From AgsmRelay
	static BOOL CBGuildMaster2(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBGuildMember2(PVOID pData, PVOID pClass, PVOID pCustData);

	// From Database Manager
	static BOOL CBGuildMasterSelectResult2(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBGuildMemberSelectResult2(PVOID pData, PVOID pClass, PVOID pCustData);

	

	//////////////////////////////////////////////////////////////////////////
	// Admin ����
	BOOL SearchCharacterByAccount(AgsdRelaySearchCharacter* pcsSearchCharacter);
	BOOL SearchCharacterByCharName(AgsdRelaySearchCharacter* pcsSearchCharacter);
	
	// From AgsmRelay
	static BOOL CBSearchCharacter(PVOID pData, PVOID pClass, PVOID pCustData);

	// From Database Manager
	static BOOL CBSearchCharacterSelectResult(PVOID pData, PVOID pClass, PVOID pCustData);


	
	//////////////////////////////////////////////////////////////////////////
	// Log ����
	BOOL Log_Loginout(AgpdLog_Loginout* pcsAgpdLog);
	BOOL Log_Combat(AgpdLog_Combat* pcsAgpdLog);
	BOOL Log_SkillPoint(AgpdLog_SkillPoint* pcsAgpdLog);
	BOOL Log_SkillUse(AgpdLog_SkillUse* pcsAgpdLog);
	BOOL Log_ItemPickUse(AgpdLog_ItemPickUse* pcsAgpdLog);
	BOOL Log_ItemTrade(AgpdLog_ItemTrade* pcsAgpdLog);
	BOOL Log_ItemConvert(AgpdLog_ItemConvert* pcsAgpdLog);
	BOOL Log_ItemRepair(AgpdLog_ItemRepair* pcsAgpdLog);
	BOOL Log_LevelUp(AgpdLog_LevelUp* pcsAgpdLog);
	BOOL Log_ItemOwnerChange(AgpdLog_ItemOwnerChange* pcsAgpdLog);

	static BOOL CBLog(PVOID pData, PVOID pClass, PVOID pCustData);
};

#endif // !defined(AFX_AGSMMAKESQL_H__3B98E9D0_C704_4334_8F65_7967158CF58D__INCLUDED_)
