// AgsdGuild.h
// (C) NHN - ArchLord Development Team
// steeple, 2004. 06. 20.

#ifndef _AGSDGUILD_H_
#define _AGSDGUILD_H_

#include "ApBase.h"
#include "AsDefine.h"

class AgsdGuildMember
{
public:
	INT32 m_lServerID;
	UINT32 m_ulNID;

	AgpdCharacter* m_pcsCharacter;	// ulNID �� ���� �����ؼ� ĳ���� �����͸� ��� �ִ� ������ ����. 2005.04.27.
};

// 2005.04.17. steeple
// ��Ʋ ���¿� �� �ð��� �����Ѵ�. ���� ������ ���� �ʴ´�.
class AgsdGuildBattleProcess
{
public:
	eGuildBattleType	m_eType;
	UINT32				m_ulPerson;

	INT8				m_cStatus;

	UINT32				m_ulLastCheckTime;

	UINT32				m_ulAcceptTime;
	UINT32				m_ulReadyTime;
	UINT32				m_ulStartTime;
	UINT32				m_ulDuration;

	CHAR				m_szGuildID1[AGPMGUILD_MAX_GUILD_ID_LENGTH + 1];
	CHAR				m_szGuildID2[AGPMGUILD_MAX_GUILD_ID_LENGTH + 1];
};
typedef list<AgsdGuildBattleProcess*>	GuildBattleList;
typedef GuildBattleList::iterator		GuildBattleListItr;

#endif	//_AGSDGUILD_H_