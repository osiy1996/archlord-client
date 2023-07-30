#include "AgsmEventGacha.h"

static  AgpmItem	* _spcsAgpmItem = NULL;
char * GetItemTemplateName( INT32 nTID )
{
	AgpdItemTemplate * pcsTemplate = _spcsAgpmItem->GetItemTemplate( nTID );
	
	if( pcsTemplate ) return pcsTemplate->m_szName;
	else
	{
		static	char strError[] = "<Error>";
		return strError;
	}
}

AgsmEventGacha::AgsmEventGacha():
	m_pcsAgpmcharacter( NULL ),
	m_pcsAgpmItem( NULL ),
	m_pcsAgpmDropItem2( NULL ),
	m_pcsAgpmEventGacha( NULL ) ,
	m_pcsAgsmCharacter( NULL ),
	m_pcsAgsmItem( NULL ),
	m_pcsAgsmItemManager( NULL )
{
	SetModuleName("AgsmEventGacha");
}

AgsmEventGacha::~AgsmEventGacha()
{
}

	// ... ApModule inherited		
BOOL AgsmEventGacha::OnAddModule()
{
	m_pcsAgpmcharacter = ( AgpmCharacter * ) GetModule( "AgpmCharacter" );
	m_pcsAgpmItem = ( AgpmItem * ) GetModule( "AgpmItem" );
	m_pcsAgpmDropItem2 = ( AgpmDropItem2 * ) GetModule( "AgpmDropItem2" );
	m_pcsAgpmEventGacha = ( AgpmEventGacha * ) GetModule( "AgpmEventGacha" );

	m_pcsAgsmCharacter = ( AgsmCharacter * ) GetModule( "AgsmCharacter" );
	m_pcsAgsmItem = ( AgsmItem * ) GetModule( "AgsmItem" );
	m_pcsAgsmItemManager = ( AgsmItemManager * ) GetModule( "AgsmItemManager" );

	if( !m_pcsAgpmcharacter || !m_pcsAgpmItem || !m_pcsAgpmDropItem2 || !m_pcsAgpmEventGacha ||
		!m_pcsAgsmCharacter || !m_pcsAgsmItem || !m_pcsAgsmItemManager )
	{
		return FALSE;
	}

	_spcsAgpmItem = m_pcsAgpmItem;

	m_pcsAgpmEventGacha->SetCallbackRequest( CBEventRequest , this );
	m_pcsAgpmEventGacha->SetCallbackGacha( CBEventGacha , this );

	m_pcsAgpmcharacter->SetCallbackCheckAllBlockStatus(CBCheckAllBlock, this);
	
	return TRUE;
}

	// ... Callback
BOOL AgsmEventGacha::CBEventRequest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmEventGacha *pThis = (AgsmEventGacha *)pClass;
	AgpmEventGacha::GachaInfo	*pstGachaInfo = ( AgpmEventGacha::GachaInfo * ) pData ;
	//ApdEvent *pApdEvent = (ApdEvent *) pData;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pCustData;
	INT32	nGachaType = PtrToInt( pstGachaInfo->pcsEvent->m_pvData );

	// ������ TID ������..
	vector< INT32 >	vecTID;

	AgpmEventGacha::ERROR_CODE eErrorCode = pThis->m_pcsAgpmEventGacha->CheckProperGacha( pAgpdCharacter , nGachaType , &vecTID );

	// ��Ŷ�� vecTID�� �߰��ؾ��Ѵ�.
	INT16 nPacketLength = 0;
	PVOID pvPacket = pThis->m_pcsAgpmEventGacha->MakePacketEventRequestGranted(pstGachaInfo->pcsEvent, pAgpdCharacter->m_lID, ( INT32 ) eErrorCode , &vecTID ,  &nPacketLength);

	BOOL bResult = pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pcsAgsmCharacter->GetCharDPNID(pAgpdCharacter));
	pThis->m_pcsAgpmEventGacha->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmEventGacha::CBEventGacha(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmEventGacha *pThis = (AgsmEventGacha *)pClass;
	AgpmEventGacha::GachaInfo	*pstGachaInfo = ( AgpmEventGacha::GachaInfo * ) pData ;
	//ApdEvent *pApdEvent = (ApdEvent *) pData;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pCustData;
	INT32	nGachaType = PtrToInt( pstGachaInfo->pcsEvent->m_pvData );

	// ���⼭ ������ ��󳻰� ,
	// ��� TID�� ���ִ� �۾����Ѵ�.
	// �׸��� ������ �� �ɼ� �����ϰ� , �κ��� �־��ش�.

	AgpmEventGacha::ERROR_CODE eAccepted = pThis->m_pcsAgpmEventGacha->CheckProperGacha( pAgpdCharacter , nGachaType , NULL );

	INT32	nTID = 0;

	if( eAccepted == AgpmEventGacha::EC_NOERROR )
	{
		nTID = pThis->ProcessGacha( pAgpdCharacter , nGachaType );

		if(nTID == 0)
			eAccepted = AgpmEventGacha::EC_GACHA_ERROR;
	}
	else
	{
	}

	INT16 nPacketLength = 0;
	PVOID pvPacket = pThis->m_pcsAgpmEventGacha->MakePacketEventResult(
		pstGachaInfo->pcsEvent, 
		pAgpdCharacter->m_lID, 
		(INT32 ) eAccepted ,	// �����ڵ�
		nTID ,	// ȹ�� ������..
		&nPacketLength);

	BOOL bResult = pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pcsAgsmCharacter->GetCharDPNID(pAgpdCharacter));
	pThis->m_pcsAgpmEventGacha->m_csPacket.FreePacket(pvPacket);
	
	return bResult;
}

INT32	AgsmEventGacha::ProcessGacha( AgpdCharacter * pcsCharacter , INT32 nGachaType )
{
	CHAR szFile[MAX_PATH];
	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		sprintf(szFile, "LOG\\GACHA_%04d%02d%02d.log", st.wYear, st.wMonth, st.wDay);
	}

	if( this->m_pcsAgpmEventGacha->m_pcsAgpmCharacter->IsAllBlockStatus( pcsCharacter ) ) return FALSE;

	// ���� ��� ������. ���� �Լ��� �������� �˻縦��.
	INT32	nJibulItemTID , nJibulItemCount , nJibulMoneyCount;
	INT32	nNeedCharismaPoint;

	AgpdGachaType * pGachaType = this->m_pcsAgpmEventGacha->GetGachaTypeInfo( nGachaType );
	{
		AuRace stRace = this->m_pcsAgpmEventGacha->m_pcsAgpmCharacter->GetCharacterRace( pcsCharacter );

		ASSERT( pGachaType );
		if( !pGachaType ) return FALSE;

		INT32 nLevel = this->m_pcsAgpmEventGacha->m_pcsAgpmCharacter->GetLevel( pcsCharacter );

		// Ȯ�� ����
		AgpdGachaDropTable::DropInfo * pDropInfo = &pGachaType->stDropTable.mapGacha[ nLevel ];

		nJibulItemTID		= pDropInfo->nItemTID		;
		nJibulItemCount		= pDropInfo->nItemNeeded	;
		nJibulMoneyCount	= pDropInfo->nCost			;
		nNeedCharismaPoint	= pDropInfo->nNeedCharisma;
	}

	INT32	nTID	= m_pcsAgpmEventGacha->GetGachaItem( pcsCharacter , nGachaType );

	static ApmMap	*pcsApmMap	= ( ApmMap * ) GetModule( "ApmMap" );
	INT32	nRegion = pcsCharacter->m_nBindingRegionIndex;
	ApmMap::RegionTemplate * pcsRegionTemplate = pcsApmMap->GetTemplate( nRegion );

	if( !nTID )
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), // [�ð�:��] 
			"%s\t"	// �ɸ����̸�
			"No Item\t"
			, pcsCharacter->m_szID);
		AuLogFile_s(szFile, strCharBuff);

		return 0;
	}

	BOOL bResult = FALSE;
	BOOL bResultCharisma = TRUE;
	INT64	lTotalMoney;
	m_pcsAgpmcharacter->GetMoney( pcsCharacter , & lTotalMoney );

	INT64	lFee	= ( INT64 ) nJibulMoneyCount;

	if( lTotalMoney >= lFee)
	{
		if( nJibulItemTID && nJibulItemCount > 0 )
		{
			if(nJibulItemTID == m_pcsAgpmItem->GetMoneyTID())
			{
				INT64 llMoney = (INT64)nJibulItemCount;
				bResult = m_pcsAgpmcharacter->SubMoney(pcsCharacter, llMoney);
			}
			else
			{
				AgpdItemStatus eItemStatus = AGPDITEM_STATUS_INVENTORY;
				
				if(g_eServiceArea == AP_SERVICE_AREA_GLOBAL || g_eServiceArea == AP_SERVICE_AREA_CHINA)
					eItemStatus = AGPDITEM_STATUS_CASH_INVENTORY;

				bResult = m_pcsAgpmItem->RemoveItemCompleteFromGrid(pcsCharacter, nJibulItemTID, nJibulItemCount, eItemStatus);
			}
		}
		else
		{
			bResult = TRUE;
		}
		
		if( nNeedCharismaPoint > 0 )
		{
			if(nNeedCharismaPoint > m_pcsAgpmcharacter->GetCharismaPoint(pcsCharacter))
				bResultCharisma = FALSE;
		}

		if( bResult && bResultCharisma)
		{
			if(lFee>0)
				bResult = m_pcsAgpmcharacter->SubMoney(pcsCharacter, lFee);
			
			if(nNeedCharismaPoint>0)
				bResultCharisma = m_pcsAgpmcharacter->SubCharismaPoint(pcsCharacter, nNeedCharismaPoint);
		}

		if(!bResult || !bResultCharisma)
		{
			// ������ ���� ����..
			// ����ó�� -��-;
			return FALSE;
		}
	}
	else
	{
		// �̰� �����ִ�.
		ASSERT( !"�� ��ġ �̻�" );
		return FALSE;
	}

	// pcsCharacter�� Lock�� ���¿��� �����Ұű� ������
	// ������ ���ܼ� �ȵȴ�.
	// ���������� �� �Լ����� �� �ľ��Ѵ�.

	{
		// �α׸� ����..

		char strCharBuff[1024] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), 
			// [�ð�:��] �̸� ��íŸ�� �������� ���� �� ���������"
			"%s\t" //�̸�
			"Gacha Start\t"
			"%s\t" // Region Name
			"%s\t" // ��íŸ�� 
			"%d\t" // ��� ������ TID
			"%d\t" // ��� ������ ����
			"%d\t" // ��
			"%d\t" // ��� ������ TID
			"%s"	// ������ �̸�

			, pcsCharacter->m_szID
			, pcsRegionTemplate ? pcsRegionTemplate->pStrName : "<noregion>"
			, pGachaType->strName.c_str()
			, nJibulItemTID
			, nJibulItemCount 
			, lFee 
			, nTID 
			, GetItemTemplateName( nTID ));
		AuLogFile_s(szFile, strCharBuff);
	}

	// �������� �־��ִ� ����

	// Ÿ�̸� ���.. cGacha_Rolling_Time �ڿ� Ŭ���̾�Ʈ�� �˷���.	
	AgsdCharacter *pAgsdCharacter = m_pcsAgsmCharacter->GetADCharacter(pcsCharacter);
	pAgsdCharacter->SetGachaBlock( TRUE );

	AgpdItem* pcsItem = m_pcsAgsmItemManager->CreateItem(nTID, NULL, 1, TRUE);
	
	ASSERT(pcsItem);
	if(!pcsItem)
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), 
			// [�ð�:��] 
			"%s\t"	// �ɸ����̸�
			"%d\t"	// nTID
			"Item Creation Failed\t"
			"%s"
			, pcsCharacter->m_szID
			, nTID
			, GetItemTemplateName( nTID ));
		AuLogFile_s(szFile, strCharBuff);

		return FALSE;
	}

	//m_pcsAgpmDropItem2->ProcessConvertItem(pcsItem, pcsCharacter);
	m_pcsAgpmDropItem2->ProcessConvertItemOptionGacha(pcsItem, pcsCharacter,
		pcsItem->m_pcsItemTemplate->m_lMinOptionNum ,
		pcsItem->m_pcsItemTemplate->m_lMaxOptionNum );

	m_pcsAgpmDropItem2->ProcessConvertItemSocket(pcsItem, 
		pcsItem->m_pcsItemTemplate->m_lMinSocketNum ,
		pcsItem->m_pcsItemTemplate->m_lMaxSocketNum );

	m_pcsAgsmItem->ProcessItemSkillPlus(pcsItem, pcsCharacter);

	if(m_pcsAgpmItem->AddItemToInventory(pcsCharacter, pcsItem) != TRUE)
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), 
			// [�ð�:��] 
			"%s\t"	// �ɸ����̸�
			"%d\t"	// nTID
			"Add Inventory Failed\t"
			"%s"
			, pcsCharacter->m_szID
			, nTID
			, GetItemTemplateName( nTID ));
		AuLogFile_s(szFile, strCharBuff);

		// ������ �����.
		m_pcsAgpmItem->RemoveItem(pcsItem);
		return FALSE;
	}

	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), 
		// [�ð�:��] 
		"%s\t"	// �ɸ����̸�
		"%d\t"	// nTID
		"Add Item Success\t"
		"%s"
		, pcsCharacter->m_szID
		, nTID
		, GetItemTemplateName( nTID ));
	AuLogFile_s(szFile, strCharBuff);

	if(m_pcsAgpmItem->GetBoundType(pcsItem) == E_AGPMITEM_BIND_ON_ACQUIRE)
		m_pcsAgpmItem->SetBoundOnOwner(pcsItem, pcsCharacter);

	AddTimer( cGacha_Rolling_Time , pcsCharacter->m_lID, this, CBGachaItemUpdate, pcsItem );

	return nTID;
}

BOOL AgsmEventGacha::IsGachaBlocked(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgsdCharacter* pAgsdCharacter = m_pcsAgsmCharacter->GetADCharacter(pcsCharacter);
	if(!pAgsdCharacter)
		return FALSE;
	
	return pAgsdCharacter->m_bIsGachaBlock;
}


BOOL AgsmEventGacha::CBCheckAllBlock(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmEventGacha	*pThis	= (AgsmEventGacha *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	BOOL			*pbIsAllBlock	= (BOOL *)			pCustData;

	if (!*pbIsAllBlock)
        *pbIsAllBlock	|= pThis->IsGachaBlocked(pcsCharacter);

	return TRUE;
}


BOOL	AgsmEventGacha::CBGachaItemUpdate(INT32 lCID, PVOID pClass, UINT32 uDelay, PVOID pvData)
{
	AgsmEventGacha *pThis = (AgsmEventGacha *)pClass;
	AgpdItem* pcsItem	= ( AgpdItem * ) pvData;
	ASSERT( pThis );

	INT32 nTID = pcsItem->m_pcsItemTemplate->m_lID;
	AgpdCharacter		*pcsCharacter	= pThis->m_pcsAgpmcharacter->GetCharacter( lCID );

	if( pThis && nTID && pcsCharacter )
	{
		// �α׸� ����..
		CHAR szFile[MAX_PATH];
		SYSTEMTIME st;
		GetLocalTime(&st);
		sprintf(szFile, "LOG\\GACHA_%04d%02d%02d.log", st.wYear, st.wMonth, st.wDay);

		// ��� ������.
		AgsdCharacter *pAgsdCharacter = pThis->m_pcsAgsmCharacter->GetADCharacter(pcsCharacter);

		vector< AgsdCharacter::GachaDelayItemUpdateData >::iterator iter;
		for( iter = pAgsdCharacter->vecGachaItemQueue.begin();
			iter != pAgsdCharacter->vecGachaItemQueue.end();
			iter++)
		{
			AgsdCharacter::GachaDelayItemUpdateData	&data	= *iter;
			switch( data.eType )
			{
			case AgsdCharacter::GachaDelayItemUpdateData::NORMAL:
				pThis->m_pcsAgsmItem->SendPacketItem(data.pcsItem, pAgsdCharacter->m_dpnidCharacter );
				break;
			case AgsdCharacter::GachaDelayItemUpdateData::STACKABLE:
				pThis->m_pcsAgsmItem->SendPacketItemStackCount(data.pcsItem, pAgsdCharacter->m_dpnidCharacter );
				break;
			}
		}
		pAgsdCharacter->ClearGachaItemUpdate();

		pAgsdCharacter->SetGachaBlock( FALSE );
		{
			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), 
				// [�ð�:��] 
				"%s\t"	// �ɸ����̸�
				"Add Item Success\t"
				"%d\t%s\t"
				, pcsCharacter->m_szID
				, nTID , GetItemTemplateName( nTID ));
			AuLogFile_s(szFile, strCharBuff);
		}
	}

	return TRUE;
}
