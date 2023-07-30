
#include <AgpmEventNPCTrade/AgpmEventNPCTrade.h>
#include <ApMemoryTracker/ApMemoryTracker.h>
#include <ApModule/ApModuleStream.h>
#include <AgpmGrid/AgpmGrid.h>
#include <ApmEventManager/ApmEventManager.h>

AgpmEventNPCTrade::AgpmEventNPCTrade()
{
	SetModuleName("AgpmEventNPCTrade");
	
	m_pcsApmEventManager	= NULL;
	m_pcsAgpmGrid			= NULL;
}

AgpmEventNPCTrade::~AgpmEventNPCTrade()
{
	//���ø��� �����Ѵ�.
	AgpdEventNPCTradeTemplate	**ppcsTemplate = NULL;

	INT32			lIndex = 0;

	// ��ϵ� ��� NPCTrade Template�� ���ؼ�...
	for( ppcsTemplate = (AgpdEventNPCTradeTemplate **) m_csNPCTradeTemplate.GetObjectSequence(&lIndex); ppcsTemplate; ppcsTemplate = (AgpdEventNPCTradeTemplate **) m_csNPCTradeTemplate.GetObjectSequence(&lIndex))
	{
		if( (*ppcsTemplate) != NULL )
		{
			(*ppcsTemplate)->m_csItemList.Destroy();

			// ������ (2004-06-17 ���� 12:35:08) : �̰� �־�������� �ʳ����~?
			delete (*ppcsTemplate);
		}
	}
}

BOOL AgpmEventNPCTrade::OnAddModule()
{
	m_pcsApmEventManager = (ApmEventManager *) GetModule("ApmEventManager");
	m_pcsAgpmGrid = (AgpmGrid *) GetModule("AgpmGrid");

	if( !m_pcsApmEventManager || !m_pcsAgpmGrid )
	{
		return FALSE;
	}

	if( !m_pcsApmEventManager->RegisterEvent(APDEVENT_FUNCTION_NPCTRADE, CBEventConstructor, CBEventDestructor, NULL, CBStreamWriteEvent, CBStreamReadEvent, this))
	{
		return FALSE;
	}

	if( !m_csNPCTradeTemplate.InitializeObject(sizeof(AgpdEventNPCTradeData *) , AGPMEVENT_NPCTRADE_MAX_NPC ) )
	{
		return FALSE;
	}

	if( !m_csNPCTradeItemGroup.InitializeObject( sizeof(AgpdEventNPCTradeItemGroupList *), AGPMEVENT_NPCTRADE_MAX_NPC ) )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL	AgpmEventNPCTrade::CBEventConstructor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpmEventNPCTrade		*pThis;
	ApdEvent				*pcsNPCTrade;
	BOOL			bResult;

	pThis = (AgpmEventNPCTrade *)pClass;
		
	bResult = FALSE;

	if( pThis->m_pcsAgpmGrid != NULL && pThis != NULL )
	{
		pcsNPCTrade = (ApdEvent	*)pData;			//���� �����͸� ���� Object

		pcsNPCTrade->m_pvData = new AgpdEventNPCTradeData;
		((AgpdEventNPCTradeData *)pcsNPCTrade->m_pvData)->m_pcsGrid = new AgpdGrid;

		ZeroMemory(((AgpdEventNPCTradeData *)pcsNPCTrade->m_pvData)->m_ppcInitGridData, sizeof(((AgpdEventNPCTradeData *)pcsNPCTrade->m_pvData)->m_ppcInitGridData));
		ZeroMemory(((AgpdEventNPCTradeData *)pcsNPCTrade->m_pvData)->m_alItemID, sizeof(((AgpdEventNPCTradeData *)pcsNPCTrade->m_pvData)->m_alItemID));
		ZeroMemory(((AgpdEventNPCTradeData *)pcsNPCTrade->m_pvData)->m_ppcItemData, sizeof(((AgpdEventNPCTradeData *)pcsNPCTrade->m_pvData)->m_ppcItemData));

		if( pcsNPCTrade->m_pvData != NULL )
		{
			if( ((AgpdEventNPCTradeData *)pcsNPCTrade->m_pvData)->m_pcsGrid != NULL )
			{
				bResult = pThis->m_pcsAgpmGrid->Init( ((AgpdEventNPCTradeData *)pcsNPCTrade->m_pvData)->m_pcsGrid, AGPDGRID_TYPE_NPCTRADEBOX );
			}
		}
	}

	return bResult;
}

BOOL	AgpmEventNPCTrade::CBEventDestructor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpmEventNPCTrade		*pThis;
	ApdEvent				*pcsNPCTrade;
	BOOL			bResult;
		
	bResult = FALSE;

	pThis = (AgpmEventNPCTrade *)pClass;

	if( pThis->m_pcsAgpmGrid != NULL && pThis != NULL )
	{
		pcsNPCTrade = (ApdEvent	*)pData;			//���� �����͸� ���� Object

		if( (AgpdEventNPCTradeData *)pcsNPCTrade->m_pvData != NULL )
		{
			pThis->EnumCallback(AGPMEVENT_NPCTRADE_CB_ID_DELETE_GRID, ((AgpdEventNPCTradeData *)pcsNPCTrade->m_pvData)->m_pcsGrid, NULL);

			pThis->m_pcsAgpmGrid->Remove( ((AgpdEventNPCTradeData *)pcsNPCTrade->m_pvData)->m_pcsGrid );

			delete ((AgpdEventNPCTradeData *)pcsNPCTrade->m_pvData)->m_pcsGrid;
			delete pcsNPCTrade->m_pvData;
		}
	}

	return TRUE;
}

BOOL	AgpmEventNPCTrade::CBStreamWriteEvent(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpmEventNPCTrade *		pThis		= (AgpmEventNPCTrade *) pClass;
	ApdEvent *				pstEvent	= (ApdEvent *) pData;
	AgpdEventNPCTradeData*	pstNPCTrade	= (AgpdEventNPCTradeData *) pstEvent->m_pvData;
	ApModuleStream *		pstStream	= (ApModuleStream *) pCustData;

	//����~
	if (!pstStream->WriteValue(AGPMEVENT_NPCTRADE_INI_NAME_START, 0))
		return FALSE;

	//���ø��̸� ����.
	if (!pstStream->WriteValue(AGPMEVENT_NPCTRADE_INI_TEMPLATE, pstNPCTrade->m_lNPCTradeTemplateID ))
		return FALSE;

	//��~
	if (!pstStream->WriteValue(AGPMEVENT_NPCTRADE_INI_NAME_END, 0))
		return FALSE;

	return TRUE;
}

BOOL	AgpmEventNPCTrade::CBStreamReadEvent(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpmEventNPCTrade *		pThis		= (AgpmEventNPCTrade *) pClass;
	ApdEvent *				pstEvent	= (ApdEvent *) pData;
	AgpdEventNPCTradeData*	pstNPCTrade	= (AgpdEventNPCTradeData *) pstEvent->m_pvData;
	ApModuleStream *		pstStream	= (ApModuleStream *) pCustData;
	const CHAR *			szValueName;

	if (!pstStream->ReadNextValue())
		return TRUE;

	szValueName = pstStream->GetValueName();
	if (_stricmp(szValueName, AGPMEVENT_NPCTRADE_INI_NAME_START))
		return TRUE;

	while (pstStream->ReadNextValue())
	{
		szValueName = pstStream->GetValueName();

		if( !_stricmp(szValueName, AGPMEVENT_NPCTRADE_INI_TEMPLATE) )
		{
			pstStream->GetValue( &pstNPCTrade->m_lNPCTradeTemplateID );
		}
		else if(!_stricmp(szValueName, AGPMEVENT_NPCTRADE_INI_NAME_END))
		{
			break;
		}
	}

	return TRUE;
}

/*BOOL AgpmEventNPCTrade::LoadNPCTradeItemGroupTemplate( char *pstrFileName )
{
	BOOL				bResult;

	bResult = FALSE;

	if( m_csExcelTxtLib.OpenExcelFile( pstrFileName, true ) )
	{
		INT32			lRow, lColumn;

		lRow = m_csExcelTxtLib.GetRow();
		lColumn = m_csExcelTxtLib.GetColumn();

		for( int lTempColumn=0; lTempColumn<lColumn; lTempColumn++ )
		{
			AgpdEventNPCTradeItemGroupList *pcsItemGroup;

			char			*pstrGroupName;

			pcsItemGroup = new AgpdEventNPCTradeItemGroupList;

			//2��°�� �׷� ������ �̸�.
			pstrGroupName = m_csExcelTxtLib.GetData( lTempColumn, 2 );

			strcat( pcsItemGroup->m_strGroupName, pstrGroupName );

			//������� �������̸�.
			for( int lTempRow=3; lTempRow<lRow; lTempRow++ )
			{
				char			*pstrTempItemTID;

				pstrTempItemTID = m_csExcelTxtLib.GetData( lTempColumn, lTempRow );

				if( pstrTempItemTID != NULL )
				{
					INT32			*plItemTID;

					plItemTID = new INT32;
					
					*plItemTID = atoi( pstrTempItemTID );

					pcsItemGroup->m_csNPCTradeItemList.Add( plItemTID );
				}
			}

			m_csNPCTradeItemGroup.AddObject( (void *)&pcsItemGroup, pstrGroupName );
		}

		bResult = TRUE;
	}

	return bResult;
}

BOOL AgpmEventNPCTrade::LoadNPCTradeTemplate( char *pstrFileName )
{
	BOOL				bResult;

	bResult = FALSE;

	if( m_csExcelTxtLib.OpenExcelFile( pstrFileName, true ) )
	{
		INT32			lRow, lColumn;

		lRow = m_csExcelTxtLib.GetRow();
		lColumn = m_csExcelTxtLib.GetColumn();

		for( int lTempRow=3; lTempRow<lRow; lTempRow++ )
		{
			AgpdEventNPCTradeTeamplate	*pcsTemplate;
			char			*pstrData;

			pcsTemplate = new AgpdEventNPCTradeTeamplate;

			//Template �̸�����.
			pstrData = m_csExcelTxtLib.GetData( 0, lTempRow );
			pcsTemplate->m_lNPCTID = atoi( pstrData );

			//Town �̸�����
			pstrData = m_csExcelTxtLib.GetData( 1, lTempRow );
			strcat( pcsTemplate->m_strTownName, pstrData );

			//�ŵ��Լ�.
			pstrData = m_csExcelTxtLib.GetData( 2, lTempRow );
			pcsTemplate->m_fSellFunc = atof( pstrData );

			//�����Լ�
			pstrData = m_csExcelTxtLib.GetData( 3, lTempRow );
			pcsTemplate->m_BuyFunc = atof( pstrData );

			//�����Լ�B(Other)
			pstrData = m_csExcelTxtLib.GetData( 4, lTempRow );
			pcsTemplate->m_BuyOtherFunc = atof( pstrData );

			for( int lTempColumn=5; lTempColumn<lColumn; lTempColumn++ )
			{
				pstrData = m_csExcelTxtLib.GetData( lTempColumn, lTempRow );

				if( pstrData != NULL )
				{
					AgpdEventNPCTradeItemGroupList		**ppcsGroupList;
					
					ppcsGroupList = (AgpdEventNPCTradeItemGroupList **)m_csNPCTradeItemGroup.GetObject( pstrData );

					if( ppcsGroupList != NULL )
					{
						pcsTemplate->m_csGroupList.Add( (*ppcsGroupList) );
					}
				}
			}

			m_csNPCTradeTemplate.AddObject( (void *)&pcsTemplate, pcsTemplate->m_lNPCTID );
		}

		bResult = TRUE;
	}

	return bResult;
}*/

BOOL AgpmEventNPCTrade::LoadNPCTradeRes( char *pstrFileName , BOOL bDecryption )
{
	BOOL				bResult;

	bResult = FALSE;

	if( m_csExcelTxtLib.OpenExcelFile( pstrFileName, TRUE , bDecryption ) )
	{
		AgpdEventNPCTradeTemplate	*pcsTemplate;

		INT32			lRow, lColumn;

		pcsTemplate = NULL;
		lRow = m_csExcelTxtLib.GetRow();
		lColumn = m_csExcelTxtLib.GetColumn();

		for( int lTempRow=0; lTempRow<lRow; lTempRow++ )
		{
			char			*pstrData;

			//Template �̸�����.
			pstrData = m_csExcelTxtLib.GetData( 0, lTempRow );

			if( pstrData != NULL )
			{
				if( !_stricmp( pstrData, "Npc" ) )
				{
					pcsTemplate = new AgpdEventNPCTradeTemplate;
					pstrData = m_csExcelTxtLib.GetData( 1, lTempRow );

					if( pstrData )
						pcsTemplate->m_lNPCTID = atoi(pstrData);
				}
				else if( !_stricmp( pstrData, "Sell" ) )
				{
					//�ŵ��Լ�.
					pstrData = m_csExcelTxtLib.GetData( 1, lTempRow );

					if( pstrData )
						pcsTemplate->m_fSellFunc = (float)atof( pstrData );
				}
				else if( !_stricmp( pstrData, "Buy" ) )
				{
					//�����Լ�
					pstrData = m_csExcelTxtLib.GetData( 1, lTempRow );
					
					if( pstrData )
						pcsTemplate->m_fBuyFunc = (float)atof( pstrData );
				}
				else if( !_stricmp( pstrData, "BuyOther" ) )
				{
					//�����Լ�B(Other)
					pstrData = m_csExcelTxtLib.GetData( 1, lTempRow );

					if( pstrData )
						pcsTemplate->m_fBuyOtherFunc = (float)atof( pstrData );
				}
				else if( !_stricmp( pstrData, "Item" ) )
				{
					//pcsTemplate�� ����Ʈ�� ���� �ִ´�.
					AgpdEventNPCTradeItemListData		*pcsItemListData;

					pcsItemListData = new AgpdEventNPCTradeItemListData;

					if( pcsItemListData )
					{
						pstrData = m_csExcelTxtLib.GetData( 1, lTempRow );

						if( pstrData )
							pcsItemListData->m_lItemTID = atoi(pstrData);

						pstrData = m_csExcelTxtLib.GetData( 2, lTempRow );

						if( pstrData )
							pcsItemListData->m_lItemCount = atoi(pstrData);

						pcsTemplate->m_csItemList.Add( pcsItemListData );
					}
				}
				else if( !_stricmp( pstrData, "-End" ) )
				{
					m_csNPCTradeTemplate.AddObject( (void *)&pcsTemplate, pcsTemplate->m_lNPCTID );
				}
			}
		}

		bResult = TRUE;

		//�������� �ݴ´�.
		m_csExcelTxtLib.CloseFile();
	}

	return bResult;
}

BOOL AgpmEventNPCTrade::IsItemInGrid( AgpdEventNPCTradeTemplate *pcsTemplate, INT32 lItemTID )
{
	CListNode< AgpdEventNPCTradeItemListData * >		*pcsNode;
	BOOL				bResult;

	bResult = FALSE;

	for( pcsNode = pcsTemplate->m_csItemList.GetStartNode(); pcsNode; pcsNode=pcsNode->m_pcNextNode )
	{
		if( lItemTID == ((AgpdEventNPCTradeItemListData *)pcsNode->m_tData)->m_lItemTID )
		{
			bResult = TRUE;
			break;
		}
	}

	return bResult;
}

BOOL AgpmEventNPCTrade::SetCallbackDeleteGrid(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_NPCTRADE_CB_ID_DELETE_GRID, pfCallback, pClass);
}
