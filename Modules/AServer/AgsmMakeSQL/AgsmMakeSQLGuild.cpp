// AgsmMakeSQLGuild.cpp
// 2004.07.02. steeple
// Guild ���� Query

#include "AgsmMakeSQL.h"

BOOL AgsmMakeSQL::GuildMasterInsert2(AgsdRelayGuildMaster2* pcsGuildMaster)
{
	if(!pcsGuildMaster)
		return FALSE;

	// ��ȿ�� �˻�
	if(!pcsGuildMaster->m_szGuildID || strlen(pcsGuildMaster->m_szGuildID) > AGPMGUILD_MAX_GUILD_ID_LENGTH)
		return FALSE;
	if(!pcsGuildMaster->m_szMasterID || strlen(pcsGuildMaster->m_szMasterID) > AGPACHARACTER_MAX_ID_STRING)
		return FALSE;

	SQLBuffer* pSQLBuffer = new SQLBuffer;

	sprintf(pSQLBuffer->Buffer, "INSERT INTO guildmaster(guildid, guildtid, gmcharid, maxmember, creationdate, unionid, guildrank, password)"
												"VALUES('%s', %d, '%s', %d, %d, %d, %d, '%s')",
												pcsGuildMaster->m_szGuildID,
												pcsGuildMaster->m_lGuildTID,
												pcsGuildMaster->m_szMasterID,
												pcsGuildMaster->m_lMaxMemberCount,
												pcsGuildMaster->m_lCreationDate,
												pcsGuildMaster->m_lUnionID,
												pcsGuildMaster->m_lGuildRank,
												pcsGuildMaster->m_szPassword);
	
	ASSERT(strlen(pSQLBuffer->Buffer) < sizeof(pSQLBuffer->Buffer));
	
	return m_pAgsmDBPool->ExecuteSQL(pSQLBuffer);
}

BOOL AgsmMakeSQL::GuildMasterUpdate2(AgsdRelayGuildMaster2* pcsGuildMaster)
{
	if(!pcsGuildMaster)
		return FALSE;

	// ��ȿ�� �˻�
	if(!pcsGuildMaster->m_szGuildID || strlen(pcsGuildMaster->m_szGuildID) > AGPMGUILD_MAX_GUILD_ID_LENGTH)
		return FALSE;

	SQLBuffer* pSQLBuffer = new SQLBuffer;

	sprintf(pSQLBuffer->Buffer, "UPDATE guildmaster SET gmcharid='%s', maxmember=%d, guildrank=%d "
												"WHERE guildid='%s'",
												pcsGuildMaster->m_szMasterID,
												pcsGuildMaster->m_lMaxMemberCount,
												pcsGuildMaster->m_lGuildRank,
												pcsGuildMaster->m_szGuildID);
	
	ASSERT(strlen(pSQLBuffer->Buffer) < sizeof(pSQLBuffer->Buffer));
	
	return m_pAgsmDBPool->ExecuteSQL(pSQLBuffer);
}

BOOL AgsmMakeSQL::GuildMasterDelete2(AgsdRelayGuildMaster2* pcsGuildMaster)
{
	if(!pcsGuildMaster)
		return FALSE;

	// ��ȿ�� �˻�
	if(!pcsGuildMaster->m_szGuildID || strlen(pcsGuildMaster->m_szGuildID) > AGPMGUILD_MAX_GUILD_ID_LENGTH)
		return FALSE;

	SQLBuffer* pSQLBuffer = new SQLBuffer;
	sprintf(pSQLBuffer->Buffer, "DELETE FROM guildmaster WHERE guildid='%s'", pcsGuildMaster->m_szGuildID);
	ASSERT(strlen(pSQLBuffer->Buffer) < sizeof(pSQLBuffer->Buffer));
	if(!m_pAgsmDBPool->ExecuteSQL(pSQLBuffer))
		return FALSE;

	// Member Table ������ �����.
	SQLBuffer* pSQLBuffer2 = new SQLBuffer;
	sprintf(pSQLBuffer2->Buffer, "DELETE FrOM guildmember WHERE guildid='%s'", pcsGuildMaster->m_szGuildID);
	ASSERT(strlen(pSQLBuffer2->Buffer) < sizeof(pSQLBuffer2->Buffer));
	return m_pAgsmDBPool->ExecuteSQL(pSQLBuffer2);
}

BOOL AgsmMakeSQL::GuildMasterSelect2(AgsdRelayGuildMaster2* pcsGuildMaster)
{
	if(!pcsGuildMaster)
		return FALSE;

	// Select ������ ��ȿ�� �˻簡 �ʿ����.
	SQLBuffer* pSQLBuffer = new SQLBuffer;
	sprintf(pSQLBuffer->Buffer, "SELECT guildid, guildtid, gmcharid, maxmember, creationdate, unionid, guildrank, password "
												"FROM guildmaster");
	ASSERT(strlen(pSQLBuffer->Buffer) < sizeof(pSQLBuffer->Buffer));

	// �޸� ���� �Ҵ��ؼ� �־��ش�.
	AgsdRelayGuildMaster2* pcsNewGuildMaster = new AgsdRelayGuildMaster2;
	memcpy(pcsNewGuildMaster, pcsGuildMaster, sizeof(AgsdRelayGuildMaster2));

	// Callback Setting
	pSQLBuffer->SetCallbackParam(CBGuildMasterSelectResult2, this, pcsNewGuildMaster);

	return m_pAgsmDBPool->ExecuteSQL(pSQLBuffer);
}

BOOL AgsmMakeSQL::GuildMemberInsert2(AgsdRelayGuildMember2* pcsGuildMember)
{
	if(!pcsGuildMember)
		return FALSE;

	// ��ȿ�� �˻�
	if(!pcsGuildMember->m_szGuildID || strlen(pcsGuildMember->m_szGuildID) > AGPMGUILD_MAX_GUILD_ID_LENGTH)
		return FALSE;
	if(!pcsGuildMember->m_szMemberID || strlen(pcsGuildMember->m_szMemberID) > AGPACHARACTER_MAX_ID_STRING)
		return FALSE;

	SQLBuffer* pSQLBuffer = new SQLBuffer;
	sprintf(pSQLBuffer->Buffer, "INSERT INTO guildmember(charid, guildid, memberrank, joindate) "
												"VALUES('%s', '%s', %d, %d)",
												pcsGuildMember->m_szMemberID,
												pcsGuildMember->m_szGuildID,
												pcsGuildMember->m_lMemberRank,
												pcsGuildMember->m_lJoinDate);
	ASSERT(strlen(pSQLBuffer->Buffer) < sizeof(pSQLBuffer->Buffer));

	return m_pAgsmDBPool->ExecuteSQL(pSQLBuffer);
}

BOOL AgsmMakeSQL::GuildMemberUpdate2(AgsdRelayGuildMember2* pcsGuildMember)
{
	if(!pcsGuildMember)
		return FALSE;

	// ��ȿ�� �˻�
	if(!pcsGuildMember->m_szMemberID || strlen(pcsGuildMember->m_szMemberID) > AGPACHARACTER_MAX_ID_STRING)
		return FALSE;

	SQLBuffer* pSQLBuffer = new SQLBuffer;
	sprintf(pSQLBuffer->Buffer, "UPDATE guildmember SET memberrank=%d WHERE charid='%s'",
												pcsGuildMember->m_lMemberRank,
												pcsGuildMember->m_szMemberID);
	ASSERT(strlen(pSQLBuffer->Buffer) < sizeof(pSQLBuffer->Buffer));

	return m_pAgsmDBPool->ExecuteSQL(pSQLBuffer);
}

BOOL AgsmMakeSQL::GuildMemberDelete2(AgsdRelayGuildMember2* pcsGuildMember)
{
	if(!pcsGuildMember)
		return FALSE;

	// ��ȿ�� �˻�
	if(!pcsGuildMember->m_szMemberID || strlen(pcsGuildMember->m_szMemberID) > AGPACHARACTER_MAX_ID_STRING)
		return FALSE;

	SQLBuffer* pSQLBuffer = new SQLBuffer;
	sprintf(pSQLBuffer->Buffer, "DELETE FROM guildmember WHERE charid='%s'", pcsGuildMember->m_szMemberID);
	ASSERT(strlen(pSQLBuffer->Buffer) < sizeof(pSQLBuffer->Buffer));

	return m_pAgsmDBPool->ExecuteSQL(pSQLBuffer);
}

BOOL AgsmMakeSQL::GuildMemberSelect2(AgsdRelayGuildMember2* pcsGuildMember)
{
	if(!pcsGuildMember)
		return FALSE;

	// Select ������ ��ȿ���˻簡 �ʿ����.
	SQLBuffer* pSQLBuffer = new SQLBuffer;
	sprintf(pSQLBuffer->Buffer, "SELECT a.charid, a.guildid, a.memberrank, a.joindate, b.chartid, c.lv "
													"FROM guildmember a, charmaster b, chardetail c "
													"WHERE a.charid=b.charid AND a.charid=c.charid");
	ASSERT(strlen(pSQLBuffer->Buffer) < sizeof(pSQLBuffer->Buffer));

	// ���� �޸� �Ҵ��ؼ� ó��
	AgsdRelayGuildMember2* pcsNewGuildMember = new AgsdRelayGuildMember2;
	memcpy(pcsNewGuildMember, pcsGuildMember, sizeof(AgsdRelayGuildMember2));

	// Callback Setting
	pSQLBuffer->SetCallbackParam(CBGuildMemberSelectResult2, this, pcsNewGuildMember);

	return m_pAgsmDBPool->ExecuteSQL(pSQLBuffer);
}

BOOL AgsmMakeSQL::CBGuildMaster2(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	INT32 lDBOperation = (INT32)pCustData;
	AgsmMakeSQL* pThis = (AgsmMakeSQL*)pClass;
	AgsdRelayGuildMaster2* pcsGuildMaster = (AgsdRelayGuildMaster2*)pData;
	
	switch(lDBOperation)
	{
		case AGSMRELAY_DB_OPERATION_INSERT:
			pThis->GuildMasterInsert2(pcsGuildMaster);
			break;
			
		case AGSMRELAY_DB_OPERATION_UPDATE:
			pThis->GuildMasterUpdate2(pcsGuildMaster);
			break;

		case AGSMRELAY_DB_OPERATION_DELETE:
			pThis->GuildMasterDelete2(pcsGuildMaster);
			break;
			
		case AGSMRELAY_DB_OPERATION_SELECT:
			pThis->GuildMasterSelect2(pcsGuildMaster);
			break;
	}

	return TRUE;
}

BOOL AgsmMakeSQL::CBGuildMember2(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	INT32 lDBOperation = (INT32)pCustData;
	AgsmMakeSQL* pThis = (AgsmMakeSQL*)pClass;
	AgsdRelayGuildMember2* pcsGuildMember = (AgsdRelayGuildMember2*)pData;

	switch(lDBOperation)
	{
		case AGSMRELAY_DB_OPERATION_INSERT:
			pThis->GuildMemberInsert2(pcsGuildMember);
			break;
			
		case AGSMRELAY_DB_OPERATION_UPDATE:
			pThis->GuildMemberUpdate2(pcsGuildMember);
			break;

		case AGSMRELAY_DB_OPERATION_DELETE:
			pThis->GuildMemberDelete2(pcsGuildMember);
			break;

		case AGSMRELAY_DB_OPERATION_SELECT:
			pThis->GuildMemberSelect2(pcsGuildMember);
			break;
	}

	return TRUE;
}

BOOL AgsmMakeSQL::CBGuildMasterSelectResult2(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AuRowset* pRowSet = (AuRowset*)pData;
	AgsmMakeSQL* pThis = (AgsmMakeSQL*)pClass;
	AgsdRelayGuildMaster2* pcsGuildMaster = (AgsdRelayGuildMaster2*)pCustData;

	// DB �����͸� ������ �ӽ� ����
	AgsdRelayGuildMaster2 csGuildMaster;

	// ������ ���鼭 ���� ������.
	INT32 i = 0;
	for(i = 0; i < pRowSet->GetRowCount(); i++)
	{
		memset(&csGuildMaster, 0, sizeof(AgsdRelayGuildMaster2));

		if(pRowSet->Get(i, 0))	// ��� ���̵�
			csGuildMaster.m_szGuildID = (CHAR*)pRowSet->Get(i, 0);
		if(pRowSet->Get(i, 1))	// ��� TID
			csGuildMaster.m_lGuildRank = atoi((const char*)pRowSet->Get(i, 1));
		if(pRowSet->Get(i, 2))	// ��帶���� ���̵�
			csGuildMaster.m_szMasterID = (CHAR*)pRowSet->Get(i, 2);
		if(pRowSet->Get(i, 3))	// Max Member Count
			csGuildMaster.m_lMaxMemberCount = atoi((const char*)pRowSet->Get(i, 3));
		if(pRowSet->Get(i, 4))	// ���� ��¥
			csGuildMaster.m_lCreationDate = atoi((const char*)pRowSet->Get(i, 4));
		if(pRowSet->Get(i, 5))	// ���Ͽ� ���̵�
			csGuildMaster.m_lUnionID = atoi((const char*)pRowSet->Get(i, 5));
		if(pRowSet->Get(i, 6))	// ��� ��ũ
			csGuildMaster.m_lGuildRank = atoi((const char*)pRowSet->Get(i, 6));
		if(pRowSet->Get(i, 7))	// ��� ��й�ȣ
			csGuildMaster.m_szPassword = (CHAR*)pRowSet->Get(i, 7);

		pThis->m_pAgsmRelay->SendGuildMasterSelectResult2(pcsGuildMaster->m_ulNID, &csGuildMaster);
	}

	// ��尡 �ϳ� �̻� ���� �Ѵٸ� ����� �ε��Ѵ�.
	if(i > 0)
	{
		AgsdRelayGuildMember2 csGuildMember;
		memset(&csGuildMember, 0, sizeof(AgsdRelayGuildMember2));

		csGuildMember.m_ulNID = pcsGuildMaster->m_ulNID;

		pThis->GuildMemberSelect2(&csGuildMember);
	}

	// ���� �� �� ���� �� ����
	delete pcsGuildMaster;

	return TRUE;
}

BOOL AgsmMakeSQL::CBGuildMemberSelectResult2(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AuRowset* pRowSet = (AuRowset*)pData;
	AgsmMakeSQL* pThis = (AgsmMakeSQL*)pClass;
	AgsdRelayGuildMember2* pcsGuildMember = (AgsdRelayGuildMember2*)pCustData;

	// DB �����͸� ������ �ӽ� ����
	AgsdRelayGuildMember2 csGuildMember;
	
	// ������ ���鼭 ���� ������.
	INT32 i = 0;
	for(i = 0; i < pRowSet->GetRowCount(); i++)
	{
		memset(&csGuildMember, 0, sizeof(AgsdRelayGuildMember2));

		if(pRowSet->Get(i, 0))	// Char ID
			csGuildMember.m_szMemberID = (CHAR*)pRowSet->Get(i, 0);
		if(pRowSet->Get(i, 1))	// Guild ID
			csGuildMember.m_szGuildID = (CHAR*)pRowSet->Get(i, 1);
		if(pRowSet->Get(i, 2))	// Member Rank
			csGuildMember.m_lMemberRank = atoi((const char*)pRowSet->Get(i, 2));
		if(pRowSet->Get(i, 3))	// Join Date
			csGuildMember.m_lJoinDate = atoi((const char*)pRowSet->Get(i, 3));
		if(pRowSet->Get(i, 4))	// Char TID
			csGuildMember.m_lTID = atoi((const char*)pRowSet->Get(i, 4));
		if(pRowSet->Get(i, 5))	// Level
			csGuildMember.m_lLevel = atoi((const char*)pRowSet->Get(i, 5));

		pThis->m_pAgsmRelay->SendGuildMemberSelectResult2(pcsGuildMember->m_ulNID, &csGuildMember);
	}

	delete pcsGuildMember;

	return TRUE;
}