// AgpaGuild.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2004. 06. 15.


#include <AgpmGuild/AgpaGuild.h>
#include <ApBase/ApMutualEx.h>
#include <AgpmGuild/AgpdGuild.h>

AgpdGuild* AgpaGuild::AddGuild(AgpdGuild* pcsGuild)
{
	if( !pcsGuild ) return NULL;

	char lwrGuildID[AGPMGUILD_MAX_GUILD_ID_LENGTH+1] = {0, };
	strncpy_s(lwrGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH, pcsGuild->m_szID, _TRUNCATE);

	// �Ϲ� �϶��� ��/�ҹ��� �����ϰ�
	if ( AP_SERVICE_AREA_GLOBAL == g_eServiceArea )
		_strlwr(lwrGuildID);

	if(!AddObject((PVOID)&pcsGuild, lwrGuildID))
		return NULL;

	m_ApGuildSortList.Insert( pcsGuild );
	return pcsGuild;
}


BOOL AgpaGuild::RemoveGuild(CHAR* szGuildID)
{
	if(!szGuildID) return FALSE;

	char lwrGuildID[AGPMGUILD_MAX_GUILD_ID_LENGTH+1] = {0, };
	strncpy_s(lwrGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH, szGuildID, _TRUNCATE);

	if ( AP_SERVICE_AREA_GLOBAL == g_eServiceArea )
		_strlwr(lwrGuildID);

	AgpdGuild** ppcsGuild = (AgpdGuild**)GetObject(lwrGuildID);
	if( ppcsGuild && *ppcsGuild )
		m_ApGuildSortList.Remove( *ppcsGuild );

	return RemoveObject(lwrGuildID);

}

AgpdGuild* AgpaGuild::GetGuild(CHAR* szGuildID)
{
	if(!szGuildID) return NULL;
	if ( strlen(szGuildID) <= 0 )
		return NULL;

	char lwrGuildID[AGPMGUILD_MAX_GUILD_ID_LENGTH+1] = {0, };
	strncpy_s(lwrGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH, szGuildID, _TRUNCATE);

	if ( AP_SERVICE_AREA_GLOBAL == g_eServiceArea )
		_strlwr(lwrGuildID);

	AgpdGuild** ppcsGuild = (AgpdGuild**)GetObject(lwrGuildID);
	return ppcsGuild ? *ppcsGuild : NULL;
}

BOOL AgpaGuild::SortGuildMemberDesc( BOOL bPriorityMember )
{
	if ( GetObjectCount() <= 1 ) return FALSE;		//	��ü�� �ϳ� ���ϸ� ������ �ʿ����.

	if( bPriorityMember )	m_ApGuildSortList.Sort( CompareGuildMemberDesc() );
	else					m_ApGuildSortList.Sort( CompareGuildPointDesc() );
	
	INT32 lPage = 0;
	INT16 nCount = 0;

	AuAutoLock Lock( m_ApGuildSortList.m_Mutex );
	if( !Lock.Result() )		return FALSE;

	for(AgpdGuild *pcsGuild = m_ApGuildSortList.GetHead();!m_ApGuildSortList.IsEnd();pcsGuild = m_ApGuildSortList.GetNext())
	{
		if( !pcsGuild )		break;

		if( pcsGuild->m_Mutex.WLock() ) 
		{
			if( pcsGuild->m_bRemove )
			{
				pcsGuild->m_Mutex.Release();
				continue;
			}

			pcsGuild->m_lPage = lPage;

			//JK_��� ã�� ����..�ο� 10�� �̸��� ���� ����Ʈ ���� �ʴ´�.
/////////////////////////////////////////////////////////////
			// ��ü ��� ������ ���鼭 ī��Ʈ�� ����.
			INT32 lCount = 0;

			AgpdGuildMember* pcsMember = NULL;
			INT32 lIndex = 0;
			for(AgpdGuildMember** ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
				ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
			{
				if(!ppcsMember)
					break;
				pcsMember = *ppcsMember;
				if(!pcsMember)
					break;

				if(pcsMember->m_lRank != AGPMGUILD_MEMBER_RANK_JOIN_REQUEST)
					lCount++;
			}

			if(lCount < 10) 
			{
				pcsGuild->m_Mutex.Release();
				continue;
			}
///////////////////////////////////////////////////////////
			pcsGuild->m_Mutex.Release();
		}
			

		nCount++;
		if( nCount >= AGPMGUILD_MAX_VISIBLE_GUILD_LIST )
		{
			lPage++;
			nCount = 0;
		}
	}
	return TRUE;
}