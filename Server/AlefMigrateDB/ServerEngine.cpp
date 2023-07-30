#include "ApBase.h"

#include <stdio.h>
#include "ServerEngine.h"

#define	__DB2_VERSION__

AuOLEDBManager*		g_pcsAuOLEDBManager;
ApmMap*				g_pcsApmMap;
ApmObject*			g_pcsApmObject;
AgpmFactors*		g_pcsAgpmFactors;
AgpmCharacter*		g_pcsAgpmCharacter;
AgpmUnion*			g_pcsAgpmUnion;
AgpmGrid*			g_pcsAgpmGrid;
AgpmItem*			g_pcsAgpmItem;
AgpmCombat*			g_pcsAgpmCombat;
AgpmItemLog*		g_pcsAgpmItemLog;
AgpmParty*			g_pcsAgpmParty;
AgpmTimer*			g_pcsAgpmTimer;

ApmEventManager*	g_pcsApmEventManager;
AgpmEventNature*	g_pcsAgpmEventNature;
AgpmEventSpawn*		g_pcsAgpmEventSpawn;
AgpmSkill*			g_pcsAgpmSkill;
AgpmShrine*			g_pcsAgpmShrine;
AgpmEventBinding*	g_pcsAgpmEventBinding;
AgpmAdmin*			g_pcsAgpmAdmin;

AgsmDBStream*		g_pcsAgsmDBStream;
AgsmMap*			g_pcsAgsmMap;
AgsmAOIFilter*		g_pcsAgsmAOIFilter;
AgsmServerManager*	g_pcsAgsmServerManager;
AgsmCharacter*		g_pcsAgsmCharacter;
AgsmZoning*			g_pcsAgsmZoning;
AgsmItem*			g_pcsAgsmItem;
AgsmItemManager*	g_pcsAgsmItemManager;
AgsmItemLog*		g_pcsAgsmItemLog;
AgsmParty*			g_pcsAgsmParty;
AgsmCombat*			g_pcsAgsmCombat;
AgsmSkill*			g_pcsAgsmSkill;
AgsmSkillManager*	g_pcsAgsmSkillManager;
AgsmSystemInfo*		g_pcsAgsmSystemInfo;
AgsmFactors*		g_pcsAgsmFactors;
AgsmAccountManager*	g_pcsAgsmAccountManager;
AgsmCharManager*	g_pcsAgsmCharManager;

ServerEngine::ServerEngine(GUID guidApp)
{
//	SetGUID(guidApp);
}

ServerEngine::~ServerEngine()
{
}

BOOL ServerEngine::OnRegisterModule()
{
	REGISTER_MODULE(g_pcsAuOLEDBManager, AuOLEDBManager);
	REGISTER_MODULE(g_pcsAgpmItemLog, AgpmItemLog);
	REGISTER_MODULE(g_pcsApmMap, ApmMap);
	REGISTER_MODULE(g_pcsApmObject, ApmObject);
	REGISTER_MODULE(g_pcsAgpmFactors, AgpmFactors);
	REGISTER_MODULE(g_pcsAgpmCharacter, AgpmCharacter);
	REGISTER_MODULE(g_pcsAgpmUnion, AgpmUnion);
	REGISTER_MODULE(g_pcsAgpmGrid, AgpmGrid);
	REGISTER_MODULE(g_pcsAgpmItem, AgpmItem);
	REGISTER_MODULE(g_pcsAgpmCombat, AgpmCombat);
	REGISTER_MODULE(g_pcsAgpmParty, AgpmParty);
	REGISTER_MODULE(g_pcsAgpmTimer, AgpmTimer);
	REGISTER_MODULE(g_pcsApmEventManager, ApmEventManager);
	REGISTER_MODULE(g_pcsAgpmEventNature, AgpmEventNature);
	REGISTER_MODULE(g_pcsAgpmEventSpawn, AgpmEventSpawn);
	REGISTER_MODULE(g_pcsAgpmSkill, AgpmSkill);
	REGISTER_MODULE(g_pcsAgpmShrine, AgpmShrine);
	REGISTER_MODULE(g_pcsAgpmEventBinding, AgpmEventBinding);
	REGISTER_MODULE(g_pcsAgpmAdmin, AgpmAdmin);
	REGISTER_MODULE(g_pcsAgsmDBStream, AgsmDBStream);
	REGISTER_MODULE(g_pcsAgsmAOIFilter, AgsmAOIFilter);
	REGISTER_MODULE(g_pcsAgsmAOIFilter, AgsmAOIFilter);
	REGISTER_MODULE(g_pcsAgsmFactors, AgsmFactors);
	REGISTER_MODULE(g_pcsAgsmServerManager, AgsmServerManager);
	REGISTER_MODULE(g_pcsAgsmMap, AgsmMap);
	REGISTER_MODULE(g_pcsAgsmAccountManager, AgsmAccountManager);
	REGISTER_MODULE(g_pcsAgsmCharacter, AgsmCharacter);
	REGISTER_MODULE(g_pcsAgsmZoning, AgsmZoning);
	REGISTER_MODULE(g_pcsAgsmSystemInfo, AgsmSystemInfo);
	REGISTER_MODULE(g_pcsAgsmParty, AgsmParty);
	REGISTER_MODULE(g_pcsAgsmItem, AgsmItem);
	REGISTER_MODULE(g_pcsAgsmCharManager, AgsmCharManager);
	REGISTER_MODULE(g_pcsAgsmItemManager, AgsmItemManager);
	REGISTER_MODULE(g_pcsAgsmItemLog, AgsmItemLog);
	REGISTER_MODULE(g_pcsAgsmCombat, AgsmCombat);
	REGISTER_MODULE(g_pcsAgsmSkill, AgsmSkill);
	REGISTER_MODULE(g_pcsAgsmSkillManager, AgsmSkillManager);

	// disable idle events
	g_pcsAgpmCharacter->EnableIdle(FALSE);

	g_pcsAgpmCharacter->	SetMaxCharacterTemplate	( 200	);
	g_pcsAgpmCharacter->	SetMaxCharacter			( 4000	);
	g_pcsAgpmCharacter->	SetMaxCharacterRemove	( 2000	);
	g_pcsAgpmCharacter->	SetMaxIdleEvent			( 100	);

	g_pcsAgpmItem->		SetMaxItemTemplate		( 2000	);
	g_pcsAgpmItem->		SetMaxItem				( 20000	);

	g_pcsAgpmSkill->		SetMaxSkill				( 10000	);
	g_pcsAgpmSkill->		SetMaxSkillTemplate		( 200	);

	g_pcsAgsmAccountManager->	SetMaxAccount		( 4000	);

	g_pcsAgsmCharacter->	SetMaxCheckRecvChar		( 100	);
	g_pcsAgsmCharacter->	SetMaxWaitForRemoveChar	( 100	);

	g_pcsAgsmServerManager->	SetMaxServer	( 20		);
	g_pcsAgsmServerManager->	SetMaxTemplate	( 100		);
	g_pcsAgsmServerManager->	SetMaxIdleEvent	( 100		);

	// module start
	if (!Initialize())
		return FALSE;

/*	if( !g_csAgsmLoginClient.LoadCharNameFromExcel( "CharName.txt" ) )
	{
		return FALSE;
	}*/

	if (!g_pcsAgpmFactors->		CharacterTypeStreamRead("Ini\\CharType.ini"			)	)
	{
		ASSERT( !"g_csAgpmFactors CharacterTypeStreamRead ����" );
		return FALSE;
	}

	if (!g_pcsAgpmSkill->			StreamReadTemplate("Ini\\SkillTemplate.ini"			)	)
	{
		ASSERT( !"g_csAgpmSkill StreamReadTemplate ����" );
		return FALSE;
	}

	if (!g_pcsAgpmCharacter->		StreamReadTemplate("Ini\\CharacterTemplate.ini"		)	)
	{
		ASSERT( !"g_csAgpmCharacter StreamReadTemplate ����" );
		return FALSE;
	}

	if (!g_pcsAgpmCharacter->		StreamReadLevelUpExpTxt("Ini\\LevelUpExp.txt"))
	{
		ASSERT( !"g_csAgpmCharacter StreamReadLevelUpExpTxt ����" );
		return FALSE;
	}

	if (!g_pcsAgpmCharacter->		StreamReadCharGrowUpTxt("Ini\\GrowUpFactor.txt"))
	{
		ASSERT( !"g_csAgpmCharacter StreamReadCharGrowUpTxt ����" );
		return FALSE;
	}

	if (!g_pcsAgpmCharacter->		StreamReadCharKind("Ini\\CharKind.ini"				)	)
	{
		ASSERT( !"g_csAgpmCharacter StreamReadCharKind ����" );
		return FALSE;
	}

	if (!g_pcsAgpmItem->			StreamReadTemplate("Ini\\ItemTemplate.ini"			)	)
	{
		ASSERT( !"g_csAgpmItem StreamReadTemplate ����" );
		return FALSE;
	}

	CHAR	szBuffer[512];
	if (!g_pcsAgpmCharacter->		StreamReadImportData("Ini\\CharacterDataTable.txt", szBuffer))
	{
		ASSERT( !"g_csAgpmCharacter StreamReadImportData ����" );
		return FALSE;
	}

	if (!g_pcsAgpmItem->			StreamReadImportData("Ini\\ItemDataTable.txt", szBuffer))
	{
		ASSERT( !"g_csAgpmCharacter StreamReadImportData ����" );
		return FALSE;
	}

	if (!g_pcsAgpmSkill->			ReadSkillSpecTxt("Ini\\Skill_Spec.txt"				)	)
	{
		ASSERT( !"g_csAgpmSkill ReadSkillSpecTxt ����" );
		return FALSE;
	}

	if (!g_pcsAgpmSkill->			ReadSkillConstTxt("Ini\\Skill_Const.txt"			)	)
	{
		ASSERT( !"g_csAgpmSkill ReadSkillConstTxt ����" );
		return FALSE;
	}

	if (!g_pcsAgpmSkill->			ReadSkillMasteryTxt("Ini\\Skill_Mastery.txt"		)	)
	{
		ASSERT( !"g_csAgpmSkill ReadSkillMasteryTxt ����" );
		return FALSE;
	}

	//OLEDBManager�� �ʱ�ȭ���ش�.
	g_pcsAuOLEDBManager->Init( 2, 10000, 2 );

	g_pcsAuOLEDBManager->SetStatus( DBTHREAD_ACTIVE );

	if (!ConnectToDB())
		return FALSE;

	WriteLog(AS_LOG_RELEASE, "ServerEngine : module is started");
	//MessageBox(NULL, "Module is started", "AlefLoginServer", 1);

	return TRUE;
}

BOOL ServerEngine::ConnectToDB()
{
	for (int i = 0; i < 2; ++i)
	{
		COLEDB				*pcOLEDB = *(COLEDB **) g_pcsAuOLEDBManager->GetOLEDB( i );

		BOOL	bConnectDB	= FALSE;

		while (!bConnectDB)
		{
			if( pcOLEDB[AGSMLOGIN_DS_ARCHLORD].Initialize() == true )
			{
				printf( "OLEDB Init succeeded!!\n" );
				bConnectDB = TRUE;
			}
			else
			{
				printf( "OLEDBInit Failed!!\n" );
				bConnectDB = FALSE;
				continue;
			}

			//if( pcOLEDB->ConnectToDB((LPOLESTR)wszID, (LPOLESTR)wszPasswd, (LPOLESTR)wszDataSource) == true )
			bConnectDB = pcOLEDB[AGSMLOGIN_DS_ARCHLORD].ConnectToDB("alef1", "alef123", "archlord");

			if (!bConnectDB)
			{
				printf( "DB Connection Failed!\n" );

				Sleep(1000);
			}
		}
	}

	printf( "DB Connetion Succeeded!\n" );

	return TRUE;
}

BOOL ServerEngine::OnTerminate()
{
	return Destroy();
	//return TRUE;
}

//DB�������� ���¸� �ٲ۴�. DBó���߿� ������ �ʵ���.....
BOOL ServerEngine::SetDBThreadStatus( int iStatus )
{
	g_pcsAuOLEDBManager->SetStatus( iStatus );

	return TRUE;
}

//DB ��� �����尡 ������ ��ٸ���.
BOOL ServerEngine::WaitDBThreads()
{
	g_pcsAuOLEDBManager->WaitThreads();

	return TRUE;
}

BOOL ServerEngine::MigrateDB()
{
	COLEDB				*pcOLEDB0 = *(COLEDB **) g_pcsAuOLEDBManager->GetOLEDB( 0 );
	COLEDB				*pcOLEDB1 = *(COLEDB **) g_pcsAuOLEDBManager->GetOLEDB( 1 );

	AgpdCharacter	*pcsAgpdCharacter;
	AgpdFactor		*pcsAgpdFactor;
	AgpdItem		*paAgpdItem[512];
	AgpdSkill		*paAgpdSkill[AGPMSKILL_MAX_SKILL_OWN];

	BOOL			bResult;
	char			strQueryString[1024];
	char			strCharName[AGPACHARACTER_MAX_ID_STRING + 1];
	INT				iItemIndex;
	INT				iSkillIndex;
	INT32			lCID;

	pcsAgpdCharacter = NULL;
	pcsAgpdFactor = NULL;

	bResult = FALSE;
	lCID = 0;
	iItemIndex = 0;
	iSkillIndex = 0;

	ZeroMemory(strQueryString, sizeof(CHAR) * 1024);
	ZeroMemory(strCharName, sizeof(CHAR) * (AGPACHARACTER_MAX_ID_STRING + 1));

	memset( paAgpdItem, 0, sizeof(AgpdItem *)*512 );
	memset( paAgpdSkill, 0, sizeof(AgpdSkill *)*AGPMSKILL_MAX_SKILL_OWN );

	CHAR	*szBuffer	= NULL;

	pcOLEDB0[AGSMLOGIN_DS_ARCHLORD].StartTranaction();

	sprintf(strQueryString, "select Name from character");

	pcOLEDB0[AGSMLOGIN_DS_ARCHLORD].SetQueryText(strQueryString);

	if( pcOLEDB0[AGSMLOGIN_DS_ARCHLORD].ExecuteQuery() == OLEDB_SQL_SUCCESS )
	{
		do
		{
			if ((szBuffer = pcOLEDB0[AGSMLOGIN_DS_ARCHLORD].GetQueryResult(0)) != NULL)
			{
				strncpy(strCharName, szBuffer, AGSMDB_MAX_NAME);

				pcOLEDB1[AGSMLOGIN_DS_ARCHLORD].StartTranaction();

				INT32	lSlotIndex	= 0;

				INT32	lCID	= SelectCharacter(pcOLEDB1, strCharName, &lSlotIndex);

				InsertCharacter(pcOLEDB1, lCID, lSlotIndex);

				pcOLEDB1[AGSMLOGIN_DS_ARCHLORD].CommitTransaction();
			}
		} while( pcOLEDB0[AGSMLOGIN_DS_ARCHLORD].GetNextRow() );
	}

	pcOLEDB0[AGSMLOGIN_DS_ARCHLORD].EndQuery();

	pcOLEDB0[AGSMLOGIN_DS_ARCHLORD].CommitTransaction();

	return TRUE;
}

INT32 ServerEngine::SelectCharacter(COLEDB *pcOLEDB, CHAR *pstrCharName, INT32 *plSlotIndex)
{
	BOOL			bResult;
	char			strQueryString[1024];

	INT32			lCID				= 0;
	AgpdCharacter	*pcsAgpdCharacter	= NULL;

	AgpdItem		*paAgpdItem[512];
	AgpdSkill		*paAgpdSkill[AGPMSKILL_MAX_SKILL_OWN];

	INT				iItemIndex			= 0;
	INT				iSkillIndex			= 0;

	memset( paAgpdItem, 0, sizeof(AgpdItem *)*512 );
	memset( paAgpdSkill, 0, sizeof(AgpdSkill *)*AGPMSKILL_MAX_SKILL_OWN );

	CHAR			*szBuffer			= NULL;

	sprintf(strQueryString, "select slot from character where name='%s'", pstrCharName);
	pcOLEDB[AGSMLOGIN_DS_ARCHLORD].SetQueryText(strQueryString);
	pcOLEDB[AGSMLOGIN_DS_ARCHLORD].ExecuteQuery();

	szBuffer		= pcOLEDB[AGSMLOGIN_DS_ARCHLORD].GetQueryResult(0);
	*plSlotIndex	= atoi(szBuffer);

	pcOLEDB[AGSMLOGIN_DS_ARCHLORD].EndQuery();

	//ĳ�� �⺻ ������ ����.
	if( g_pcsAgsmCharacter->GetSelectQuery2( pstrCharName, strQueryString ) )
	{
		if( pcOLEDB[AGSMLOGIN_DS_ARCHLORD].SetQueryText( strQueryString ) )
		{
			if( pcOLEDB[AGSMLOGIN_DS_ARCHLORD].ExecuteQuery() == OLEDB_SQL_SUCCESS )
			{
				//���� ����� ����.
				lCID = g_pcsAgsmCharacter->GetSelectResult2( &pcOLEDB[AGSMLOGIN_DS_ARCHLORD] );

				if( lCID != 0 )
					//pcsAgpdCharacter = m_pagpmCharacter->GetCharacterLock( lCID );
					pcsAgpdCharacter = g_pcsAgpmCharacter->GetCharacter( lCID );

				if( pcsAgpdCharacter != NULL )
				{
					//m_pagsmCharManager->EnumCallback( AGSMCHARMM_CB_CREATECHAR, pcsAgpdCharacter, NULL );
					bResult = TRUE;
				}
				else
				{
					bResult = FALSE;
				}
			}
			else
			{
				bResult = FALSE;
			}

			pcOLEDB[AGSMLOGIN_DS_ARCHLORD].EndQuery();
		}
		else
		{
			bResult = FALSE;
		}
	}
	else
	{
		bResult = FALSE;
	}

	if( pcsAgpdCharacter != NULL )
	{
		//ĳ�� ���� ������( inven, Equip ��)�� ����.
		if( bResult )
		{
			if( g_pcsAgsmItem->GetSelectQuery2( pstrCharName, strQueryString ) )
			{
				if( pcOLEDB[AGSMLOGIN_DS_ARCHLORD].SetQueryText( strQueryString ) )
				{
					INT8			cResult;

					cResult = pcOLEDB[AGSMLOGIN_DS_ARCHLORD].ExecuteQuery();

					if( cResult == OLEDB_SQL_NO_RESULT )
					{
						//�ƹ������ ����. �׳� �Ѿ��.
					}
					else if( cResult == OLEDB_SQL_SUCCESS )
					{
						AgpdItem			*pcsTempAgpdItem;

						do
						{
							//���� ����� ����.
							pcsTempAgpdItem = g_pcsAgsmItemManager->GetSelectResult2( &pcOLEDB[AGSMLOGIN_DS_ARCHLORD] );

							if( pcsTempAgpdItem != NULL )
							{
								AgsdItem	*pcsAgsdItem	= g_pcsAgsmItem->GetADItem(pcsTempAgpdItem);
								pcsAgsdItem->m_bIsNeedInsertDB	= FALSE;

								paAgpdItem[iItemIndex] = pcsTempAgpdItem;
								iItemIndex++;
							}
						}while( pcOLEDB[AGSMLOGIN_DS_ARCHLORD].GetNextRow() );
					}
					else
					{
						bResult = FALSE;
					}

					pcOLEDB[AGSMLOGIN_DS_ARCHLORD].EndQuery();
				}
				else
				{
					bResult = FALSE;
				}
			}

		}

		//ĳ�� Equip Item Factor�� ����.
		if( bResult && (iItemIndex != 0) )
		{
			//���� Equip Item������ŭ Item�� Factor�� ����.
			for( int i=0; i<iItemIndex; ++i )
			{
				AgpdItem			*pcsItem;
				AgsdItem			*pcsAgsdItem;
				AgpdItemADChar		*pstItemADChar;

				BOOL				bAddItemSucceeded;

				bAddItemSucceeded = FALSE;
				pcsItem = paAgpdItem[i];

				pcsAgsdItem = g_pcsAgsmItem->GetADItem( pcsItem );
				pstItemADChar = g_pcsAgpmItem->GetADCharacter(pcsAgpdCharacter);

				//�κ��̸� �κ��� ����.
				if( pcsItem->m_eStatus == AGPDITEM_STATUS_INVENTORY )
				{
					INT16			nLayer;
					INT16			nRow;
					INT16			nColumn;

					nLayer = pcsItem->m_anGridPos[AGPDITEM_GRID_POS_TAB];
					nRow = pcsItem->m_anGridPos[AGPDITEM_GRID_POS_ROW];
					nColumn = pcsItem->m_anGridPos[AGPDITEM_GRID_POS_COLUMN];

					//DB���� ���� ��ġ�� �ִ´�.
					if( g_pcsAgpmItem->AddItemToInventory( pcsAgpdCharacter, pcsItem, &nLayer, &nRow, &nColumn ) == AGPMITEM_AddItemInventoryResult_TRUE )
					{
						bAddItemSucceeded = TRUE;
					}
					//�̹� �������� ���� ���� �ִ�. ��ĭ�� ã�Ƽ� �ִ´�.
					else
					{
						INT32			lResult;

						lResult = g_pcsAgpmItem->AddItemToInventory( pcsAgpdCharacter, pcsItem, NULL, NULL, NULL );

						if( lResult == AGPMITEM_AddItemInventoryResult_TRUE )
						{
							bAddItemSucceeded = TRUE;
						}
						//��ĭ��������. �����.
						else if( lResult == AGPMITEM_AddItemInventoryResult_FALSE )
						{
							g_pcsAgpmItem->RemoveItem(pcsItem->m_lID);
						}
						//���õǾ �ᱹ�� ��������.
						else //if( lResult == AGPMITEM_AddItemInventoryResult_RemoveByStack )
						{
							//���⼭�� �ƹ�ó�� �ʿ����.
						}
					}
				}
				//Equip�� Equip�� ����.
				else if( pcsItem->m_eStatus == AGPDITEM_STATUS_EQUIP )
				{
					if( !g_pcsAgpmItem->EquipItem(pcsAgpdCharacter, pstItemADChar, pcsItem ) )
					{
						g_pcsAgpmItem->RemoveItem(pcsItem->m_lID);
					}
					else
					{
						bAddItemSucceeded = TRUE;
					}
				}

				//�κ��̳�, Equip�� ���� ���ߴ�. �Ѿ�� Error�α� �����.
				if( bAddItemSucceeded == FALSE )
				{
					continue;
				}

				// convert history�� �����´�.
				if (((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_nType == AGPMITEM_TYPE_EQUIP)
				{
					INT32	lMaxConvert	= 0;

					if (((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON)
						lMaxConvert = AGPMITEM_MAX_CONVERT_WEAPON;
					else
						lMaxConvert = AGPMITEM_MAX_CONVERT_ARMOUR;

					if (g_pcsAgsmItem->GetSelectHistoryDB2(pcsItem, strQueryString))
					{
						if (pcOLEDB[AGSMLOGIN_DS_ARCHLORD].SetQueryText(strQueryString))
						{
							INT8	cResult	= pcOLEDB[AGSMLOGIN_DS_ARCHLORD].ExecuteQuery();

							if (cResult == OLEDB_SQL_NO_RESULT)
							{
							}
							else if (cResult == OLEDB_SQL_SUCCESS)
							{
								do {
									g_pcsAgsmItem->GetSelectConvertHistoryResult2(&pcOLEDB[AGSMLOGIN_DS_ARCHLORD], &pcsItem->m_stConvertHistory, pcsItem);
								} while (pcOLEDB[AGSMLOGIN_DS_ARCHLORD].GetNextRow());
							}
							else
							{
								bResult = FALSE;
							}

							pcOLEDB[AGSMLOGIN_DS_ARCHLORD].EndQuery();

							/*
							if (pcsItem->m_stConvertHistory.lConvertLevel == 0)
								break;
							*/
						}
					}
				}
			}
		}

		// get skill mastery information
		if (bResult)
		{
			if (g_pcsAgsmSkill->GetSelectQuery2(pstrCharName, strQueryString))
			{
				if (pcOLEDB[AGSMLOGIN_DS_ARCHLORD].SetQueryText(strQueryString))
				{
					INT8	cResult	= pcOLEDB[AGSMLOGIN_DS_ARCHLORD].ExecuteQuery();

					if (cResult == OLEDB_SQL_SUCCESS)
					{
						bResult	= g_pcsAgsmSkillManager->GetSelectResult2(&pcOLEDB[AGSMLOGIN_DS_ARCHLORD], pcsAgpdCharacter);
					}
					else
					{
						bResult = FALSE;
					}

					pcOLEDB[AGSMLOGIN_DS_ARCHLORD].EndQuery();

					if (cResult == OLEDB_SQL_NO_RESULT)
					{
						//�ƹ������ ����. �����͸� �ʱ� ����Ÿ���� ���� �ִ´�.
						//pcOLEDB[AGSMLOGIN_DS_ARCHLORD].StartTranaction();

						for (int i = 0; i < AGPMSKILL_MAX_MASTERY; ++i)
						{
							memset( strQueryString, 0, sizeof( strQueryString ) );

							if( g_pcsAgsmSkill->GetInsertQuery2(pcsAgpdCharacter, i, strQueryString) )
							{
								if( pcOLEDB[AGSMLOGIN_DS_ARCHLORD].SetQueryText( strQueryString ) )
								{
									pcOLEDB[AGSMLOGIN_DS_ARCHLORD].ExecuteQuery();
									pcOLEDB[AGSMLOGIN_DS_ARCHLORD].EndQuery();
								}
							}
						}

						//pcOLEDB[AGSMLOGIN_DS_ARCHLORD].CommitTransaction();
					}
				}
			}
		}
	}

	return lCID;
}

BOOL ServerEngine::InsertCharacter(COLEDB *pcOLEDB, INT32 lCID, INT32 lSlotIndex)
{
	BOOL			bResult				= TRUE;
	char			strQueryString[1024];

	AgpdItem		*paAgpdItem[512];
	AgpdSkill		*paAgpdSkill[AGPMSKILL_MAX_SKILL_OWN];

	INT				iItemIndex			= 0;
	INT				iSkillIndex			= 0;

	memset( paAgpdItem, 0, sizeof(AgpdItem *)*512 );
	memset( paAgpdSkill, 0, sizeof(AgpdSkill *)*AGPMSKILL_MAX_SKILL_OWN );

	AgpdCharacter	*pcsAgpdCharacter	= g_pcsAgpmCharacter->GetCharacter(lCID);
	if (!pcsAgpdCharacter)
		return FALSE;

	BOOL	bExist	= FALSE;

	AgsdCharacter	*pcsAgsdCharacter	= g_pcsAgsmCharacter->GetADCharacter(pcsAgpdCharacter);

	/*
	//������� ĳ���� �̸��� �̹� DB�� �ִ��� Ȯ���غ���.
	sprintf( strQueryString, "select charid from charmaster where charid = '%s'", pcsAgpdCharacter->m_szID );

	if( pcOLEDB[AGSMLOGIN_DS_ARCHLORD].SetQueryText( strQueryString ) )
	{
		if( pcOLEDB[AGSMLOGIN_DS_ARCHLORD].ExecuteQuery() == OLEDB_SQL_SUCCESS )
		{
			do
			{
				//�̹̰��� ĳ�� �̸��� �����Ѵ�.
				if( strcmp( pcOLEDB[AGSMLOGIN_DS_ARCHLORD].GetQueryResult(0), pcsAgpdCharacter->m_szID ) == 0 )
				{
					bExist = TRUE;
				}
			} while( pcOLEDB[AGSMLOGIN_DS_ARCHLORD].GetNextRow() );
		}

		pcOLEDB[AGSMLOGIN_DS_ARCHLORD].EndQuery();
	}

	if( bExist == FALSE )
	{
		//������ ����ִ� ĳ���� ���� Ȯ���Ѵ�.
		sprintf( strQueryString, "select charid, slot from charmaster where accountid = '%s'", pcsAgsdCharacter->m_szAccountID );

		if( pcOLEDB[AGSMLOGIN_DS_ARCHLORD].SetQueryText( strQueryString ) )
		{
			if( pcOLEDB[AGSMLOGIN_DS_ARCHLORD].ExecuteQuery() == OLEDB_SQL_SUCCESS )
			{
				char			*pstrAccountCharName;
				INT32			lAccountCharNameLen;
				INT32			lIndex;

				do
				{
					//���� ����� ����.
					pstrAccountCharName = pcOLEDB[AGSMLOGIN_DS_ARCHLORD].GetQueryResult(0);
					lIndex = atoi(pcOLEDB[AGSMLOGIN_DS_ARCHLORD].GetQueryResult(1));

					if( pstrAccountCharName != NULL )
					{
						lAccountCharNameLen = strlen(pstrAccountCharName) + 1;

						memset( cCreatedChar[lIndex].m_strCharName, 0, lAccountCharNameLen );
						strcat( cCreatedChar[lIndex].m_strCharName, pstrAccountCharName );

						cCreatedChar[lIndex].m_lIndex = lIndex;

						lCharCount++;
					}
				}while( pcOLEDB[AGSMLOGIN_DS_ARCHLORD].GetNextRow() );

				//�̹� AGPMLOGIN_MAX_CHAR_PER_ACCOUNT���� ĳ���͸� ������ �ִ�.
				if( lCharCount >= AGPMLOGIN_MAX_CHAR_PER_ACCOUNT )
				{
					bFull = TRUE;
				}
				else
				{
					for( int iCounter=0; iCounter<AGPMLOGIN_MAX_CHAR_PER_ACCOUNT; iCounter++ )
					{
						if( strlen( cCreatedChar[iCounter].m_strCharName ) == 0 )
						{
							lSlotIndex = iCounter;
							break;
						}
					}
				}
			}

			pcOLEDB[AGSMLOGIN_DS_ARCHLORD].EndQuery();
		}
	}

	//�̹� �����ϴ� �̸��̰ų� �������� ���� ���� ���� ���д�!
	if( bExist || bFull )
	{
		bResult = FALSE;
	}
	else
	{
		bResult = TRUE;
	}
	*/

	//ĳ�������� DB�� ����.
	// charmaster table
	if( bResult )
	{
		if( g_pcsAgsmCharacter->GetMasterInsertQuery3( pcsAgpdCharacter, lSlotIndex, strQueryString ) )
		{
			if( pcOLEDB[AGSMLOGIN_DS_ARCHLORD].SetQueryText( strQueryString ) )
			{
				if( pcOLEDB[AGSMLOGIN_DS_ARCHLORD].ExecuteQuery() == OLEDB_SQL_SUCCESS )
				{
					bResult = TRUE;
				}
				else
				{
					bResult = FALSE;
				}

				pcOLEDB[AGSMLOGIN_DS_ARCHLORD].EndQuery();
			}
			else
			{
				bResult = FALSE;
			}
		}
		else
		{
			bResult = FALSE;
		}
	}

	// chardetail table
	if (bResult)
	{
		if( g_pcsAgsmCharacter->GetDetailInsertQuery3( pcsAgpdCharacter, strQueryString ) )
		{
			if( pcOLEDB[AGSMLOGIN_DS_ARCHLORD].SetQueryText( strQueryString ) )
			{
				if( pcOLEDB[AGSMLOGIN_DS_ARCHLORD].ExecuteQuery() == OLEDB_SQL_SUCCESS )
				{
					bResult = TRUE;
				}
				else
				{
					bResult = FALSE;
				}

				pcOLEDB[AGSMLOGIN_DS_ARCHLORD].EndQuery();
			}
			else
			{
				bResult = FALSE;
			}
		}
		else
		{
			bResult = FALSE;
		}
	}

	//�⺻ Equip��� �ش�.
	if( bResult )
	{
		AgpdGridItem		*pcsAgpdGridItem;
		INT32				lIID;

		for( int i=0; i<AGPMITEM_PART_NUM; ++i )
		{
			pcsAgpdGridItem = g_pcsAgpmItem->GetEquipItem( pcsAgpdCharacter, i );

			if( pcsAgpdGridItem != NULL )
			{
				lIID = pcsAgpdGridItem->m_lItemID;
			}
			else
			{
				lIID = 0;
			}

			//DB�� �����Ѵ�.
			if( lIID != 0 )
			{
				AgpdItem			*pcsAgpdItem;
				AgsdItem			*pcsAgsdItem;

				pcsAgpdItem = g_pcsAgpmItem->GetItem( lIID );
				pcsAgsdItem = g_pcsAgsmItem->GetADItem( pcsAgpdItem );

				memset( strQueryString, 0, sizeof( strQueryString ) );

				if( g_pcsAgsmItem->GetInsertQuery3( pcsAgpdItem, strQueryString ) )
				{
					if( pcOLEDB[AGSMLOGIN_DS_ARCHLORD].SetQueryText( strQueryString ) )
					{
						if( pcOLEDB[AGSMLOGIN_DS_ARCHLORD].ExecuteQuery() == OLEDB_SQL_SUCCESS )
						{
							bResult = TRUE;
						}
						else
						{
							bResult = FALSE;
						}

						pcOLEDB[AGSMLOGIN_DS_ARCHLORD].EndQuery();
					}
					else
					{
						bResult = FALSE;
					}
				}
				else
				{
					bResult = FALSE;
				}
			}
		}
	}

	// �⺻���� �־����� �κ��丮 ���۵��� �����Ѵ�.
	if( bResult )
	{
		AgpdGrid		*pcsInventoryGrid	= g_pcsAgpmItem->GetInventory(pcsAgpdCharacter);
		if (pcsInventoryGrid)
		{
			INT32			lIndex	= 0;

			AgpdGridItem		*pcsAgpdGridItem = NULL;
			INT32				lIID;

			for (pcsAgpdGridItem = g_pcsAgpmGrid->GetItemSequence(pcsInventoryGrid, &lIndex);
				 pcsAgpdGridItem;
				 pcsAgpdGridItem = g_pcsAgpmGrid->GetItemSequence(pcsInventoryGrid, &lIndex))
			{
				if( pcsAgpdGridItem != NULL )
				{
					lIID = pcsAgpdGridItem->m_lItemID;
				}
				else
				{
					lIID = 0;
				}

				//DB�� �����Ѵ�.
				if( lIID != 0 )
				{
					AgpdItem			*pcsAgpdItem;
					AgsdItem			*pcsAgsdItem;

					pcsAgpdItem = g_pcsAgpmItem->GetItem( lIID );
					pcsAgsdItem = g_pcsAgsmItem->GetADItem( pcsAgpdItem );

					memset( strQueryString, 0, sizeof( strQueryString ) );

					if( g_pcsAgsmItem->GetInsertQuery3( pcsAgpdItem, strQueryString ) )
					{
						if( pcOLEDB[AGSMLOGIN_DS_ARCHLORD].SetQueryText( strQueryString ) )
						{
							if( pcOLEDB[AGSMLOGIN_DS_ARCHLORD].ExecuteQuery() == OLEDB_SQL_SUCCESS )
							{
								bResult = TRUE;
							}
							else
							{
								bResult = FALSE;
							}

							pcOLEDB[AGSMLOGIN_DS_ARCHLORD].EndQuery();
						}
						else
						{
							bResult = FALSE;
						}
					}
					else
					{
						bResult = FALSE;
					}
				}
			}
		}
	}

	// skill mastery �⺻ ������ DB�� �����Ѵ�
	if (bResult)
	{
		for (int i = 0; i < AGPMSKILL_MAX_MASTERY; ++i)
		{
			memset( strQueryString, 0, sizeof( strQueryString ) );

			if( g_pcsAgsmSkill->GetInsertQuery3(pcsAgpdCharacter, i, strQueryString) )
			{
				if( pcOLEDB[AGSMLOGIN_DS_ARCHLORD].SetQueryText( strQueryString ) )
				{
					if( pcOLEDB[AGSMLOGIN_DS_ARCHLORD].ExecuteQuery() == OLEDB_SQL_SUCCESS )
					{
						bResult = TRUE;
					}
					else
					{
						bResult = FALSE;
					}

					pcOLEDB[AGSMLOGIN_DS_ARCHLORD].EndQuery();
				}
				else
				{
					bResult = FALSE;
				}
			}
			else
			{
				bResult = FALSE;
			}

			if (!bResult)
				break;
		}
	}

	return TRUE;
}