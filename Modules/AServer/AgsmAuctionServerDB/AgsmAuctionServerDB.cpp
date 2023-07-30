#include <stdio.h>
#include "AgsmAuctionServerDB.h"

AgsmAuctionServerDB::AgsmAuctionServerDB()
{
	SetModuleName( "AgsmAuctionServerDB" );
}

AgsmAuctionServerDB::~AgsmAuctionServerDB()
{
}

//��𼭳� �� �� �ִ� OnAddModule()
BOOL AgsmAuctionServerDB::OnAddModule()
{
	m_paAuOLEDBManager = (AuOLEDBManager *)GetModule( "AuOLEDBManager" );
	m_pagsmAuctionServer = (AgsmAuctionServer *)GetModule( "AgsmAuctionServer" );
	m_pagsmAuctionClient = (AgsmAuctionClient *)GetModule( "AgsmAuctionClient" );

	if( !m_paAuOLEDBManager || !m_pagsmAuctionServer || !m_pagsmAuctionServer )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL AgsmAuctionServerDB::ProcessDBOperation( COLEDB *pcOLEDB, INT8 nOperation, INT32 lSellerID, INT32 lBuyerID, INT32 lErrorCode )
{
	//������ �����Ѵ�.
	pcOLEDB->StartTranaction();

	//1. �����Ѱ��.
	if( pcOLEDB->ExecuteQuery() == OLEDB_SQL_SUCCESS )
	{
		//�ȶ�.......
		if( nOperation == AGPMAUCTION_SELL_ITEM )
		{
			m_pagsmAuctionServer->EnumCallback( AUCTION_CB_ID_SELL, &lSellerID, NULL );
		}
		//�ȷ�����.......
		else if( nOperation == AGPMAUCTION_SOLD_ITEM )
		{
			m_pagsmAuctionServer->EnumCallback( AUCTION_CB_ID_SOLD, &lSellerID, NULL );
		}
		//�춧~
		else if( nOperation == AGPMAUCTION_BUY_ITEM )
		{
			//���⼭�� �ϴ��� ����~ AgsmAuctionServer���� �̹� CallBack SendBuyItem�� ������.
		}
		else if( nOperation == AGPMAUCTION_BIDDING_ITEM )
		{
			//2�����Ŀ��� ���δ�.
		}
		else if( nOperation == AGPMAUCTION_BACK_OUT_TO_INVENTORY )
		{
			m_pagsmAuctionServer->EnumCallback( AUCTION_CB_ID_BACKOUT_TO_INVENTORY, &lSellerID, NULL );
		}
		else if( nOperation == AGPMAUCTION_BACK_OUT_TO_SALESBOX )
		{
			m_pagsmAuctionServer->EnumCallback( AUCTION_CB_ID_BACKOUT_TO_SALESBOX, &lSellerID, NULL );
		}
		else if( nOperation == AGPMAUCTION_SEARCH_BY_PAGE )
		{
		}
		//Error~
		else
		{
		}

		pcOLEDB->EndQuery();
	}
	//2.������ ���.
	else
	{
	}

	pcOLEDB->CommitTransaction();

	return FALSE;
}

unsigned int WINAPI AgsmAuctionServerDB::AuctionDBProcessThread( void *pvArg )
{
	COLEDBManagerArg	*pcsOLEDBManagerArg;

	AuOLEDBManager		*pcOLEDBManager;
	AgsmAuctionServerDB	*pThis;
	COLEDB				*pcOLEDB;
	void				*pvQuery;

	INT32				lSellerID;
	INT32				lBuyerID;
	INT32				lErrorCode;
	INT8				nOperation;
	char				*pstrQuery;

	int				iIndex;

	//ĳ����.
	pcsOLEDBManagerArg = (COLEDBManagerArg *)pvArg;

	//�ʿ��� ������ ��´�.
	pcOLEDBManager = pcsOLEDBManagerArg->m_pcOLEDBManager;
	pThis = (AgsmAuctionServerDB *)pcsOLEDBManagerArg->pvClassPointer;
	iIndex = pcsOLEDBManagerArg->m_iIndex;

	//�ʿ��� ������ ������� �����.
	delete pvArg;

	//DB������ �����Ѵ�.
	pcOLEDB = pcOLEDBManager->GetOLEDB( iIndex );

	if( pcOLEDB->Initialize() == true )
	{
		printf( "OLEDB Init succeeded!!\n" );
	}
	else
	{
		printf( "OLEDBInit Failed!!\n" );
	}

	if( pcOLEDB->ConnectToDB( (LPOLESTR)L"scott", (LPOLESTR)L"tiger", (LPOLESTR)L"ora81" ) == true )
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
			AgsmAuctionQueueInfo		*pcsQueryInfo;
			pcsQueryInfo = (AgsmAuctionQueueInfo *)pvQuery;

			//CID, Operation, Query�� ��´�.
			nOperation = pcsQueryInfo->m_nOperation;
			lBuyerID = pcsQueryInfo->m_lBuyerID;
			lSellerID = pcsQueryInfo->m_lSellerID;
			lErrorCode = pcsQueryInfo->m_lErrorCode;
			pstrQuery = pcsQueryInfo->m_pstrQueury;

			if( pstrQuery != NULL )
			{
				//������ UniCode�� �����.
				pcOLEDB->SetQueryText( pstrQuery );

				pThis->ProcessDBOperation( pcOLEDB, nOperation, lBuyerID, lSellerID, lErrorCode );
			}

			//������ ���������� ������~ ���d�d~
			delete pcsQueryInfo;
		}
		else
		{
			//1ms�� ����.
			Sleep( 1 );
		}
	}

	return 1;
}
