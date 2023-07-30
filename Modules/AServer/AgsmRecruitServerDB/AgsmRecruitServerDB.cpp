#include <stdio.h>
#include "AgsmRecruitServerDB.h"

AgsmRecruitServerDB::AgsmRecruitServerDB()
{
	SetModuleName( "AgsmRecruitServerDB" );
}

AgsmRecruitServerDB::~AgsmRecruitServerDB()
{
}

//��𼭳� �� �� �ִ� OnAddModule()
BOOL AgsmRecruitServerDB::OnAddModule()
{
	m_paAuOLEDBManager = (AuOLEDBManager *)GetModule( "AuOLEDBManager" );
	m_pagsmRecruitServer = (AgsmRecruitServer *)GetModule( "AgsmRecruitServer" );

	if( !m_paAuOLEDBManager || !m_pagsmRecruitServer )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL AgsmRecruitServerDB::ProcessDBInsertIntoLFP( COLEDB *pcOLEDB, AgsmRecruitQueueInfo *pcsQueryInfo )
{
	BOOL			bResult;
	char			strQuery[1024];
	char			*pstrCharName;
	INT32			lCID;
	INT32			lClass;
	INT32			lLevel;

	bResult = FALSE;

	if( !pcsQueryInfo )
	{
		return bResult;
	}

	pstrCharName = pcsQueryInfo->m_pstrCharName;
	lCID = pcsQueryInfo->m_lCID;
	lClass = pcsQueryInfo->m_lClass;
	lLevel = pcsQueryInfo->m_lLV;

	if( pstrCharName != NULL )
	{
		//������ �����Ѵ�.
		pcOLEDB->StartTranaction();

		sprintf( strQuery, "insert into BOARD_LFP values( '%s', %d, %d)", pstrCharName, lClass, lLevel );
		ASSERT(strlen(strQuery) < 1024);

		//Recruit �Խ��ǿ� �߰��Ϸ��� �������� �ִ´�.
		pcOLEDB->SetQueryText( strQuery );

		//����~
		if( pcOLEDB->ExecuteQuery() == OLEDB_SQL_SUCCESS )
		{
			INT32				lResult;

			lResult = AGPMRECRUIT_RESULT_LFP_REGISTER_SUCCEEDED;

			bResult = m_pagsmRecruitServer->EnumCallback(RECRUIT_CB_ID_REGISTER_LFP, &lCID, &lResult );
		}

		pcOLEDB->EndQuery();

		if( bResult )
		{
			pcOLEDB->CommitTransaction();
		}
		else
		{
			pcOLEDB->AbortTransaction();
		}
	}

	return bResult;
}

BOOL AgsmRecruitServerDB::ProcessDBInsertIntoLFM( COLEDB *pcOLEDB, AgsmRecruitQueueInfo *pcsQueryInfo )
{
	BOOL			bResult;
	char			strQuery[1024];
	char			*pstrCharName;
	char			*pstrPurpose;
	INT32			lCID;
	INT32			lRequireMember;
	INT32			lLevel;
	INT32			lMinLV;
	INT32			lMaxLV;
	INT32			lClass;
	INT32			lLeaderCID;

	bResult = FALSE;

	if( !pcsQueryInfo )
	{
		return bResult;
	}

	pstrCharName = pcsQueryInfo->m_pstrCharName;
	pstrPurpose = pcsQueryInfo->m_pstrPurpose;
	lCID = pcsQueryInfo->m_lCID;
	lRequireMember = pcsQueryInfo->m_lRequireMember;
	lLevel = pcsQueryInfo->m_lLV;
	lMinLV = pcsQueryInfo->m_lMinLV;
	lMaxLV = pcsQueryInfo->m_lMaxLV;
	lClass = pcsQueryInfo->m_lClass;
	lLeaderCID = pcsQueryInfo->m_lLeaderID;

	if( pstrCharName != NULL )
	{
		//������ �����Ѵ�.
		pcOLEDB->StartTranaction();

		sprintf( strQuery, "insert into BOARD_LFM (charname, purpose, requiremember, minlv, maxlv, class, lv, leaderid ) values( '%s', '%s', %d, %d, %d, %d, %d, %d)", pstrCharName, pstrPurpose, lRequireMember, lMinLV, lMaxLV, lClass, lLevel, lLeaderCID );
		ASSERT(strlen(strQuery) < 1024);

		//Recruit �Խ��ǿ� �߰��Ϸ��� �������� �ִ´�.
		pcOLEDB->SetQueryText( strQuery );

		//����~
		if( pcOLEDB->ExecuteQuery() == OLEDB_SQL_SUCCESS )
		{
			INT32				lResult;

			lResult = AGPMRECRUIT_RESULT_LFM_REGISTER_SUCCEEDED;

			bResult = m_pagsmRecruitServer->EnumCallback(RECRUIT_CB_ID_REGISTER_LFP, &lCID, &lResult );
		}

		pcOLEDB->EndQuery();

		if( bResult )
		{
			pcOLEDB->CommitTransaction();
		}
		else
		{
			pcOLEDB->AbortTransaction();
		}
	}

	return bResult;
}

BOOL AgsmRecruitServerDB::ProcessDBUpdateLFP( COLEDB *pcOLEDB, AgsmRecruitQueueInfo *pcsQueryInfo )
{
	BOOL			bResult;

	bResult = FALSE;

	return bResult;
}

BOOL AgsmRecruitServerDB::ProcessDBUpdateLFM( COLEDB *pcOLEDB, AgsmRecruitQueueInfo *pcsQueryInfo )
{
	BOOL			bResult;

	bResult = FALSE;

	return bResult;
}

BOOL AgsmRecruitServerDB::ProcessDBDeleteFromLFP( COLEDB *pcOLEDB, AgsmRecruitQueueInfo	*pcsQueryInfo )
{
	BOOL				bResult;
	char				strQuery[1024];
	char				*pstrCharName;
	INT32				lCID;

	bResult = FALSE;
	pstrCharName = pcsQueryInfo->m_pstrCharName;
	lCID = pcsQueryInfo->m_lCID;

	if( pstrCharName != NULL )
	{
		sprintf( strQuery, "delete from board_lfp where charname = '%s'", pstrCharName );
		ASSERT(strlen(strQuery) < 1024);

		pcOLEDB->StartTranaction();

		//Recruit �Խ��ǿ��� ������� �������� �ִ´�.
		pcOLEDB->SetQueryText( strQuery );

		//����~
		if( pcOLEDB->ExecuteQuery() == OLEDB_SQL_SUCCESS )
		{
			INT32			lResult;

			lResult = AGPMRECRUIT_RESULT_LFP_CANCEL_SUCCEEDED;

			//CallBack ȣ��
			bResult = m_pagsmRecruitServer->EnumCallback(RECRUIT_CB_ID_CANCEL_LFP, &lCID, &lResult);
		}
		//����~
		else
		{
			//CallBackȣ��
		}

		pcOLEDB->EndQuery();

		if( bResult )
		{
			pcOLEDB->CommitTransaction();
		}
		else
		{
			pcOLEDB->AbortTransaction();
		}
	}

	return bResult;
}

BOOL AgsmRecruitServerDB::ProcessDBDeleteFromLFM( COLEDB *pcOLEDB, AgsmRecruitQueueInfo	*pcsQueryInfo )
{
	BOOL				bResult;
	char				strQuery[1024];
	char				*pstrCharName;
	INT32				lCID;

	bResult = FALSE;

	lCID = pcsQueryInfo->m_lCID;
	pstrCharName = pcsQueryInfo->m_pstrCharName;

	if( pstrCharName != NULL )
	{
		sprintf( strQuery, "delete from board_lfm where charname = '%s'", pstrCharName );
		ASSERT(strlen(strQuery) < 1024);

		pcOLEDB->StartTranaction();

		//Recruit �Խ��ǿ��� ������� �������� �ִ´�.
		pcOLEDB->SetQueryText( strQuery );

		//����~
		if( pcOLEDB->ExecuteQuery() == OLEDB_SQL_SUCCESS )
		{
			INT32			lResult;

			lResult = AGPMRECRUIT_RESULT_LFM_CANCEL_SUCCEEDED;

			//CallBack ȣ��
			bResult = m_pagsmRecruitServer->EnumCallback(RECRUIT_CB_ID_CANCEL_LFM, &lCID, &lResult );
		}
		//����~
		else
		{
			//CallBackȣ��
		}

		pcOLEDB->EndQuery();

		if( bResult )
		{
			pcOLEDB->CommitTransaction();
		}
		else
		{
			pcOLEDB->AbortTransaction();
		}
	}

	return bResult;
}

INT32 AgsmRecruitServerDB::GetAllClassFlag()
{
	INT32			lClass;

	lClass = 0;

	lClass |= AGPMRECRUIT_CLASS_FIGHTER;
	lClass |= AGPMRECRUIT_CLASS_RANGER;
	lClass |= AGPMRECRUIT_CLASS_MONK;
	lClass |= AGPMRECRUIT_CLASS_MAGE;

	return lClass;
}

BOOL AgsmRecruitServerDB::GetClassSearchQuery( char *pstrString, INT32 lStringSize, INT32 lClass )
{
	BOOL				bResult;

	bResult = FALSE;

	if( pstrString )
	{
		memset( pstrString, 0, lStringSize );

		if( lClass == GetAllClassFlag() )
		{
			//������ �߰��Ѵ�. �̰��ϳ��� ok!
			strcat( pstrString, "" );
			bResult = TRUE;
		}
		else
		{
			bool			bPrevious;
			bool			bWhere;
			INT32			lIndex;

			bWhere = false;
			bPrevious = false;

			if( lClass & AGPMRECRUIT_CLASS_FIGHTER )
			{
				if( bWhere == false )
				{
					strcat( pstrString, "where " );
					bWhere = true;
				}

				lIndex = strlen( pstrString );				
				sprintf( &pstrString[lIndex], "Class = %d ", AGPMRECRUIT_CLASS_FIGHTER ); 
				bPrevious = true;
			}
			if( lClass & AGPMRECRUIT_CLASS_RANGER )
			{
				if( bPrevious )
				{
					strcat( pstrString, "or " );

					lIndex = strlen( pstrString );
					sprintf( &pstrString[lIndex], "Class = %d ", AGPMRECRUIT_CLASS_RANGER ); 
				}
				else
				{
					if( bWhere == false )
					{
						strcat( pstrString, "where " );
						bWhere = true;
					}

					lIndex = strlen( pstrString );				
					sprintf( &pstrString[lIndex], "Class = %d ", AGPMRECRUIT_CLASS_RANGER ); 
					bPrevious = true;
				}
			}
			if( lClass & AGPMRECRUIT_CLASS_MONK )
			{
				if( bPrevious )
				{
					strcat( pstrString, "or " );

					lIndex = strlen( pstrString );
					sprintf( &pstrString[lIndex], "Class = %d ", AGPMRECRUIT_CLASS_MONK ); 
				}
				else
				{
					if( bWhere == false )
					{
						strcat( pstrString, "where " );
						bWhere = true;
					}

					lIndex = strlen( pstrString );				
					sprintf( &pstrString[lIndex], "Class = %d ", AGPMRECRUIT_CLASS_MONK ); 
					bPrevious = true;
				}
			}
			if( lClass & AGPMRECRUIT_CLASS_MAGE )
			{
				if( bPrevious )
				{
					strcat( pstrString, "or " );

					lIndex = strlen( pstrString );
					sprintf( &pstrString[lIndex], "Class = %d ", AGPMRECRUIT_CLASS_MAGE ); 
				}
				else
				{
					if( bWhere == false )
					{
						strcat( pstrString, "where " );
						bWhere = true;
					}

					lIndex = strlen( pstrString );				
					sprintf( &pstrString[lIndex], "Class = %d ", AGPMRECRUIT_CLASS_MAGE ); 
					bPrevious = true;
				}
			}

			if( strlen(pstrString) )
				bResult = TRUE;
		}
	}

	return bResult;
}

BOOL AgsmRecruitServerDB::ProcessDBSearchFromLFP( COLEDB *pcOLEDB, AgsmRecruitQueueInfo *pcsQueryInfo )
{
	BOOL			bResult;
	bool			bSearchMyLevel;
	char			strQuery[1024];
	char			strClassSearchQuery[256];
	INT32			iTotalCount;		//��ü �Խù�����.
	INT32			lPage;
	INT32			lCID;
	INT32			lLevel;
	INT32			lClass;

	bResult = FALSE;

	bSearchMyLevel = pcsQueryInfo->m_bSearchByLevel;
	lCID = pcsQueryInfo->m_lCID;
	lPage = pcsQueryInfo->m_lPage;
	lLevel = pcsQueryInfo->m_lLV;
	lClass = pcsQueryInfo->m_lClass;

	if( lClass == 0 )
		return FALSE;

	if( !GetClassSearchQuery( strClassSearchQuery, sizeof(strClassSearchQuery), lClass ) )
	{
		return FALSE;
	}

	pcOLEDB->StartTranaction();

	sprintf( strQuery, "select count(*) from board_lfp %s", strClassSearchQuery );

	//Recruit �Խ��ǿ� �ö�� �Խù��� ��� �������� �ִ´�.
	pcOLEDB->SetQueryText( strQuery );

	//���� ��ü Count�� ����.
	if( pcOLEDB->ExecuteQuery() == OLEDB_SQL_SUCCESS )
	{
		//Select���� Ǯ���.
		iTotalCount = atoi(pcOLEDB->GetQueryResult( 0 ));
		bResult = TRUE;
	}

	pcOLEDB->EndQuery();

	if( bResult )
	{
		//�ϳ��� ���°��~
		if( iTotalCount == 0 )
		{
		}
		else
		{
			INT8			nQueryResult;
			INT32			lLastPage;
			INT32			lRest;

			//Page�� ��´�.
			if( bSearchMyLevel )
			{
				INT32			lNowPage;
				INT32			lTempCount;

				char			strQueryText[256];

				lNowPage = 0;

				sprintf( strQueryText, "select count(*) from (select * from board_lfp where lv <= %d) %s", lLevel, strClassSearchQuery );
				
				pcOLEDB->SetQueryText( strQueryText );

				//���� ��ü Count�� ����.
				if( pcOLEDB->ExecuteQuery() == OLEDB_SQL_SUCCESS )
				{
					//Select���� Ǯ���.
					lTempCount = atoi(pcOLEDB->GetQueryResult( 0 ));

					if( iTotalCount <= MAX_RECRUIT_ROW )
					{
						lNowPage = 1;
					}
					else if( lTempCount <= MAX_RECRUIT_ROW )
					{
						lNowPage = 1;
					}
					else
					{
						lNowPage = lTempCount/MAX_RECRUIT_ROW;

						if( lNowPage%MAX_RECRUIT_ROW )
							lNowPage++;
					}

					lPage = lNowPage;
					bResult = TRUE;
				}
				else
				{
					bResult = FALSE;
				}

				pcOLEDB->EndQuery();
			}

			if( bResult == TRUE )
			{
				//������ �������� ���Ѵ�.
				lLastPage = iTotalCount/MAX_RECRUIT_ROW;
				lRest = MAX_RECRUIT_ROW;

				if( iTotalCount%MAX_RECRUIT_ROW )
					lLastPage++;

				if( lPage >= lLastPage )
				{
					lRest = iTotalCount%MAX_RECRUIT_ROW;

					if( lRest == 0 )
						lRest = MAX_RECRUIT_ROW;
				}

				sprintf( strQuery, "select * from (select * from (select * from (select * from (select * from board_lfp %s order by lv, charname ) where rownum <= %d ) order by lv desc, charname desc ) where rownum <= %d) order by lv", strClassSearchQuery, MAX_RECRUIT_ROW * lPage, lRest );

				//�������� �����Ѵ�.
				pcOLEDB->SetQueryText( strQuery );

				nQueryResult = pcOLEDB->ExecuteQuery();

				//����
				if( nQueryResult == OLEDB_SQL_SUCCESS )
				{
					int				iIndex = 0;

					//Select���� Ǯ���.
					do	
					{
						AgsmRecruitSearchInfo	*pstrRecruitInfo;
						int				iStrLen;

						pstrRecruitInfo = new AgsmRecruitSearchInfo;

						iStrLen = strlen( pcOLEDB->GetQueryResult( 0 ) ) + 1;

						pstrRecruitInfo->m_lTotalCount = iTotalCount;
						pstrRecruitInfo->m_lIndex = iIndex;
						//CharName ����
						pstrRecruitInfo->m_pstrCharName = new char[iStrLen];
						memset( pstrRecruitInfo->m_pstrCharName,  0, iStrLen );
						strcat( pstrRecruitInfo->m_pstrCharName, pcOLEDB->GetQueryResult( 0 ) );
						//Class ����
						pstrRecruitInfo->m_lClass = atoi(pcOLEDB->GetQueryResult( 1 ));
						//Level ����
						pstrRecruitInfo->m_lLevel = atoi(pcOLEDB->GetQueryResult( 2 ));

						m_pagsmRecruitServer->EnumCallback(RECRUIT_CB_ID_SEARCH_LFP, &lCID, pstrRecruitInfo );

						delete pstrRecruitInfo;

						iIndex++;

					} while( pcOLEDB->GetNextRow() );
				}
				else if( nQueryResult == OLEDB_SQL_NO_RESULT )
				{
					INT32				lResult;

					lResult = AGPMRECRUIT_RESULT_NO_SEARCH_RESULT;
					bResult = m_pagsmRecruitServer->EnumCallback(RECRUIT_CB_ID_RESULT, &lCID, &lResult );
				}
				else
				{
					bResult = FALSE;
				}

				pcOLEDB->EndQuery();
			}
		}
	}
	//���� ��Ȳ�̱�~ ����~
	else
	{
	}

	if( bResult )
	{
		pcOLEDB->CommitTransaction();
	}
	else
	{
		pcOLEDB->AbortTransaction();
	}

	return bResult;
}

BOOL AgsmRecruitServerDB::ProcessDBSearchFromLFM( COLEDB *pcOLEDB, AgsmRecruitQueueInfo *pcsQueryInfo )
{
	BOOL			bResult;
	bool			bSearchMyLevel;
	char			strQuery[1024];
	char			strClassSearchQuery[256];
	INT32			iTotalCount;		//��ü �Խù�����.
	INT32			lPage;
	INT32			lCID;
	INT32			lLevel;
	INT32			lClass;

	bResult = FALSE;

	bSearchMyLevel = pcsQueryInfo->m_bSearchByLevel;
	lCID = pcsQueryInfo->m_lCID;
	lPage = pcsQueryInfo->m_lPage;
	lLevel = pcsQueryInfo->m_lLV;
	lClass = pcsQueryInfo->m_lClass;

	if( lClass == 0 )
		return FALSE;

	if( !GetClassSearchQuery( strClassSearchQuery, sizeof(strClassSearchQuery), lClass ) )
	{
		return FALSE;
	}

	pcOLEDB->StartTranaction();

	sprintf( strQuery, "select count(*) from board_lfm %s", strClassSearchQuery );

	//Recruit �Խ��ǿ� �ö�� �Խù��� ��� �������� �ִ´�.
	pcOLEDB->SetQueryText( strQuery );

	//���� ��ü Count�� ����.
	if( pcOLEDB->ExecuteQuery() == OLEDB_SQL_SUCCESS )
	{
		//Select���� Ǯ���.
		iTotalCount = atoi(pcOLEDB->GetQueryResult( 0 ));
		bResult = TRUE;
	}

	pcOLEDB->EndQuery();

	if( bResult )
	{
		//�ϳ��� ���°��~
		if( iTotalCount == 0 )
		{
		}
		else
		{
			INT8			nQueryResult;
			INT32			lLastPage;
			INT32			lRest;

			//Page�� ��´�.
			if( bSearchMyLevel )
			{
				INT32			lNowPage;
				INT32			lTempCount;

				char			strQueryText[256];

				lNowPage = 0;

				sprintf( strQueryText, "select count(*) from (select * from board_lfm where minlv <= %d) %s", lLevel, strClassSearchQuery );
				
				pcOLEDB->SetQueryText( strQueryText );

				//���� ��ü Count�� ����.
				if( pcOLEDB->ExecuteQuery() == OLEDB_SQL_SUCCESS )
				{
					//Select���� Ǯ���.
					lTempCount = atoi(pcOLEDB->GetQueryResult( 0 ));

					if( iTotalCount <= MAX_RECRUIT_ROW )
					{
						lNowPage = 1;
					}
					else if( lTempCount <= MAX_RECRUIT_ROW )
					{
						lNowPage = 1;
					}
					else
					{
						lNowPage = lTempCount/MAX_RECRUIT_ROW;

						if( lNowPage%MAX_RECRUIT_ROW )
							lNowPage++;
					}

					lPage = lNowPage;
					bResult = TRUE;
				}
				else
				{
					bResult = FALSE;
				}

				pcOLEDB->EndQuery();
			}

			if( bResult == TRUE )
			{
				//������ �������� ���Ѵ�.
				lLastPage = iTotalCount/MAX_RECRUIT_ROW;
				lRest = MAX_RECRUIT_ROW;

				if( iTotalCount%MAX_RECRUIT_ROW )
					lLastPage++;

				if( lPage >= lLastPage )
				{
					lRest = iTotalCount%MAX_RECRUIT_ROW;

					if( lRest == 0 )
						lRest = MAX_RECRUIT_ROW;
				}

				sprintf( strQuery, "select * from (select * from (select * from (select * from (select * from board_lfm %s order by minlv, charname ) where rownum <= %d ) order by minlv desc, charname desc ) where rownum <= %d) order by minlv", strClassSearchQuery, MAX_RECRUIT_ROW*lPage, lRest );

				//�������� �����Ѵ�.
				pcOLEDB->SetQueryText( strQuery );

				nQueryResult = pcOLEDB->ExecuteQuery();

				//����
				if( nQueryResult == OLEDB_SQL_SUCCESS )
				{
					int				iIndex = 0;

					//Select���� Ǯ���.
					do	
					{
						AgsmRecruitSearchInfo	*pstrRecruitInfo;
						
						int				iStrLen;

						pstrRecruitInfo = new AgsmRecruitSearchInfo;

						pstrRecruitInfo->m_lTotalCount = iTotalCount;
						pstrRecruitInfo->m_lIndex = iIndex;
						//CharName ����
						iStrLen = strlen( pcOLEDB->GetQueryResult( 0 ) ) + 1;
						pstrRecruitInfo->m_pstrCharName = new char[iStrLen];
						memset( pstrRecruitInfo->m_pstrCharName,  0, iStrLen );
						strcat( pstrRecruitInfo->m_pstrCharName, pcOLEDB->GetQueryResult( 0 ) );
						//Purpose����
						iStrLen = strlen( pcOLEDB->GetQueryResult( 1 ) ) + 1;
						pstrRecruitInfo->m_pstrPurpose = new char[iStrLen];
						memset( pstrRecruitInfo->m_pstrPurpose,  0, iStrLen );
						strcat( pstrRecruitInfo->m_pstrPurpose, pcOLEDB->GetQueryResult( 1 ) );
						//Require Member
						pstrRecruitInfo->m_lRequireMember = atoi(pcOLEDB->GetQueryResult( 2 ));
						//Min LV����
						pstrRecruitInfo->m_lMinLV = atoi(pcOLEDB->GetQueryResult( 3 ));
						//Max LV����
						pstrRecruitInfo->m_lMaxLV = atoi(pcOLEDB->GetQueryResult( 4 ));
						//Class ����
						pstrRecruitInfo->m_lClass = atoi(pcOLEDB->GetQueryResult( 5 ));
						//Level ����
						pstrRecruitInfo->m_lLevel = atoi(pcOLEDB->GetQueryResult( 6 ));
						//LeaderID ����
						pstrRecruitInfo->m_lLeaderID = atoi(pcOLEDB->GetQueryResult( 7 ));

						m_pagsmRecruitServer->EnumCallback(RECRUIT_CB_ID_SEARCH_LFM, &lCID, pstrRecruitInfo );

						delete pstrRecruitInfo;

						iIndex++;
					} while( pcOLEDB->GetNextRow() );
				}
				else if( nQueryResult == OLEDB_SQL_NO_RESULT )
				{
					INT32				lResult;

					lResult = AGPMRECRUIT_RESULT_NO_SEARCH_RESULT;
					bResult = m_pagsmRecruitServer->EnumCallback(RECRUIT_CB_ID_RESULT, &lCID, &lResult );
				}
				else
				{
					bResult = FALSE;
				}

				pcOLEDB->EndQuery();
			}
		}
	}
	//���� ��Ȳ�̱�~ ����~
	else
	{
	}

	if( bResult )
	{
		pcOLEDB->CommitTransaction();
	}
	else
	{
		pcOLEDB->AbortTransaction();
	}

	return bResult;
}

BOOL AgsmRecruitServerDB::ProcessDBOperation( COLEDB *pcOLEDB, AgsmRecruitQueueInfo	*pcsQueryInfo  )
{
	BOOL				bResult;
	INT8				nOperation;

	bResult = FALSE;

	nOperation = pcsQueryInfo->m_nOperation;

	if( nOperation == AGSM_RECRUIT_OP_INSERT_LFP )
	{
		bResult = ProcessDBInsertIntoLFP( pcOLEDB, pcsQueryInfo );
	}
	else if( nOperation == AGSM_RECRUIT_OP_UPDATE_LFP )
	{
	}
	else if( nOperation == AGSM_RECRUIT_OP_DELETE_LFP )
	{
		bResult = ProcessDBDeleteFromLFP( pcOLEDB, pcsQueryInfo );
	}
	else if( nOperation == AGSM_RECRUIT_OP_SEARCH_LFP )
	{
		bResult = ProcessDBSearchFromLFP( pcOLEDB, pcsQueryInfo );
	}
	else if( nOperation == AGSM_RECRUIT_OP_INSERT_LFM )
	{
		bResult = ProcessDBInsertIntoLFM( pcOLEDB, pcsQueryInfo );
	}
	else if( nOperation == AGSM_RECRUIT_OP_UPDATE_LFM )
	{
	}
	else if( nOperation == AGSM_RECRUIT_OP_DELETE_LFM )
	{
		bResult = ProcessDBDeleteFromLFM( pcOLEDB, pcsQueryInfo );
	}
	else if( nOperation == AGSM_RECRUIT_OP_SEARCH_LFM )
	{
		bResult = ProcessDBSearchFromLFM( pcOLEDB, pcsQueryInfo );
	}

	return bResult;
}

unsigned int WINAPI AgsmRecruitServerDB::RecruitDBProcessThread( void *pvArg )
{
	COLEDBManagerArg	*pcsOLEDBManagerArg;

	AuOLEDBManager		*pcOLEDBManager;
	AgsmRecruitServerDB	*pThis;
	COLEDB				*pcOLEDB;
	void				*pvQuery;

	int				iIndex;

	//ĳ����.
	pcsOLEDBManagerArg = (COLEDBManagerArg *)pvArg;

	//�ʿ��� ������ ��´�.
	pcOLEDBManager = pcsOLEDBManagerArg->m_pcOLEDBManager;
	pThis = (AgsmRecruitServerDB *)pcsOLEDBManagerArg->pvClassPointer;
	iIndex = pcsOLEDBManagerArg->m_iIndex;

	//�ʿ��� ������ ������� �����.
	delete pvArg;

	//DB������ �����Ѵ�.
	pcOLEDB = pcOLEDBManager->GetOLEDB( iIndex );

	if( pcOLEDB->Initialize() == true )
	{
		printf( "OLEDB Init succeeded!!\n" );

		if( pcOLEDB->ConnectToDB( "alef", "alef", "tak" ) == true )
		{
			printf( "DB Connetion Succeeded!\n" );
		}
		else
		{
			printf( "DB Connection Failed!\n" );
		}

		while( 1 )
		{
			//Deactive�� Break!!
			if( pcOLEDBManager->GetStatus() == DBTHREAD_DEACTIVE )
			{
				break;
			}

			pvQuery = pcOLEDBManager->PopFromQueue();

			if( pvQuery != NULL )
			{
				AgsmRecruitQueueInfo		*pcsQueryInfo;
				pcsQueryInfo = (AgsmRecruitQueueInfo *)pvQuery;

				pThis->ProcessDBOperation( pcOLEDB, pcsQueryInfo );

				delete pcsQueryInfo;
			}
			else
			{
				//1ms�� ����.
				Sleep( 1 );
			}
		}
	}
	else
	{
		printf( "OLEDBInit Failed!!\n" );
	}


	return 1;
}
