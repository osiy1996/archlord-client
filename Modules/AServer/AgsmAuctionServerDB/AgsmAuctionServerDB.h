/******************************************************************************
Module:  AgsmAuctionServerDB.h
Notices: Copyright (c) NHN Studio 2003 Ashulam
Purpose: 
Last Update: 2003. 2. 17
******************************************************************************/

#ifndef __AGSM_AUCTION_SERVERDB_H__
#define __AGSM_AUCTION_SERVERDB_H__

#include "AgsEngine.h"
#include "AuOLEDB.h"
#include "AgsmAuctionServer.h"
#include "AgsmAuctionClient.h"

#ifdef	_DEBUG
#pragma comment ( lib , "AgsmAuctionServerDBD" )
#else
#pragma comment ( lib , "AgsmAuctuonServerDB" )
#endif

class AgsmAuctionServerDB : public AgsModule
{
	AuOLEDBManager		*m_paAuOLEDBManager;
	AgsmAuctionServer	*m_pagsmAuctionServer;
	AgsmAuctionClient	*m_pagsmAuctionClient;

public:

	AgsmAuctionServerDB();
	~AgsmAuctionServerDB();

	//��𿡳� �ִ� OnAddModule�̴�. Ư���Ұ� ����. Ư���ϸ� �ȵǳ�? ���d�d~
	BOOL OnAddModule();

	BOOL ProcessDBOperation( COLEDB *pcOLEDB, INT8 nOperation, INT32 lSellerID, INT32 lBuyerID, INT32 lErrorCode );
	static unsigned WINAPI AuctionDBProcessThread( void *pvArg );
};

#endif
