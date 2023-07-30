//2003.02.27 -Ashulam-

#include "AgcmEventNPCTrade.h"
#include <stdio.h>
#include <ApMemoryTracker/ApMemoryTracker.h>
#include <ApmEventManager/ApmEventManager.h>
#include <AgcmEventManager/AgcmEventManager.h>
#include <AgpmItem/AgpmItem.h>
#include <AgpmCharacter/AgpmCharacter.h>
#include <AgcmCharacter/AgcmCharacter.h>
#include <AgpmGrid/AgpmGrid.h>
#include <AgpmEventNPCTrade/AgpmEventNPCTrade.h>
#include <ApmObject/ApmObject.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AgcmEventNPCTrade::AgcmEventNPCTrade()
{
	SetModuleName("AgcmEventNPCTrade");

	SetPacketType(AGPMEVENT_NPCTRADE_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT16));
	m_csPacket.SetFieldType(AUTYPE_PACKET,	1,		// Event Base
							AUTYPE_INT8,	1,		// nOperation	: �ŷ� ����     
							AUTYPE_INT32,	1,		// RequesterCID	: �ŷ� ��û�� ID
							AUTYPE_INT32,	1,		// lIID			: �ŷ� ������ ID
							AUTYPE_PACKET,	1,		// DetailInfo	: ������ ���� ����.
							AUTYPE_INT32,	1,		// lTimeStamp	; ���ݺ����� �Ĵ� ���ǿ� ��ȭ�� ������ �̰��� �����ȴ�.
													//                �� ������ ������ ������ ���� ������ �����Ѵ�.
							AUTYPE_INT32,	1,      // lResult		; ������� ������.
							AUTYPE_INT8,	1,		// Layer		; �κ��� ��� ��ġ�� ����
							AUTYPE_INT8,	1,		// Row
							AUTYPE_INT8,	1,		// Column
							AUTYPE_END,		0
							);

	m_csDetailInfo.SetFlagLength(sizeof(INT16));
	m_csDetailInfo.SetFieldType(AUTYPE_INT32,	1,		// Item TID		: TemplateID
							AUTYPE_INT32,	1,		// Item Price	: ������ ����.
							AUTYPE_INT32,	1,		// Item Count	: ������ ����.
							AUTYPE_INT16,	1,		// Item status	: item status
							AUTYPE_INT16,	1,		// Layer		: Layer
							AUTYPE_INT16,	1,		// Row			: Row
							AUTYPE_INT16,	1,		// Column		: Column
							AUTYPE_END,		0
							);

	m_lNPCID		= 0;
	m_eNPCType		= APBASE_TYPE_NONE;
}

AgcmEventNPCTrade::~AgcmEventNPCTrade()
{
	;
}

BOOL AgcmEventNPCTrade::OnAddModule()
{
	// ���� Module�� �����´�.
	m_pcsApmObject = (ApmObject *) GetModule("ApmObject");
	m_pcsAgpmFactors = (AgpmFactors *) GetModule("AgpmFactors");
	m_pcsAgpmCharacter = (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pcsAgpmItem = (AgpmItem *) GetModule("AgpmItem");
	m_pcsApmEventManager = (ApmEventManager *) GetModule("ApmEventManager");
	m_pcsAgpmGrid = ( AgpmGrid* ) GetModule("AgpmGrid");
	m_pcsAgcmCharacter = ( AgcmCharacter* ) GetModule("AgcmCharacter");
	m_pcsAgcmEventManager = ( AgcmEventManager* ) GetModule("AgcmEventManager");

	if ( !m_pcsApmEventManager || !m_pcsAgpmCharacter || !m_pcsAgpmItem || !m_pcsAgpmGrid || !m_pcsAgcmEventManager )
		return FALSE;

	if (!m_pcsAgcmEventManager->RegisterEventCallback(APDEVENT_FUNCTION_NPCTRADE, CBRequestNPCItemList, this))
		return FALSE;

	return TRUE;
}

BOOL AgcmEventNPCTrade::SetCallbackUpdateNPCTradeGrid(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback( NPCTRADE_CB_ID_UPDATE_NPCTRADE_GRID, pfCallback, pClass);
}

BOOL AgcmEventNPCTrade::SetCallbackUpdateInventoryGrid(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback( NPCTRADE_CB_ID_UPDATE_INVENTORY_GRID, pfCallback, pClass);
}

BOOL AgcmEventNPCTrade::SetCallbackOpenUINPCTrade(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback( NPCTRADE_CB_ID_OPEN_UI_NPCTRADE, pfCallback, pClass);
}

BOOL AgcmEventNPCTrade::SetCallbackAriseNPCTradeEvent(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback( NPCTRADE_CB_ID_ARISE_NPCTRADE_EVENT, pfCallback, pClass);
}

BOOL AgcmEventNPCTrade::SetCallbackInvenFull(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback( NPCTRADE_CB_MSG_INVEN_FULL, pfCallback, pClass);
}

BOOL AgcmEventNPCTrade::SetCallbackBuyItem(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback( NPCTRADE_CB_MSG_BUY_ITEM, pfCallback, pClass);
}

BOOL AgcmEventNPCTrade::SetCallbackSellItem(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback( NPCTRADE_CB_MSG_SELL_ITEM, pfCallback, pClass);
}

BOOL AgcmEventNPCTrade::SetCallbackNotEnoughGhelld(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback( NPCTRADE_CB_MSG_NOT_ENOUGH_GHELLD, pfCallback, pClass);
}

BOOL AgcmEventNPCTrade::SetCallbackNotEnoughSpaceToReturnGhelld(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback( NPCTRADE_CB_MSG_NOT_ENOUGH_SPACE_TO_RETURN_GHELLD, pfCallback, pClass);
}

void AgcmEventNPCTrade::SetNPCInfo( INT32 lNPCID, ApBaseType eNPCType )
{
	m_lNPCID = lNPCID;
	m_eNPCType = eNPCType;
}

void AgcmEventNPCTrade::ResetNPCInfo()
{
	m_lNPCID = 0;
	m_eNPCType = APBASE_TYPE_NONE;
}

//������ ����Ʈ�� �׸���.
BOOL AgcmEventNPCTrade::ProcessItemList( ApdEvent *pstEvent, INT32 lIID, void *pvDetailInfo )
{
	BOOL			bResult;

	bResult = FALSE;
	AgpdItem* pagpdItem = m_pcsAgpmItem->GetItem( lIID );

	if( (pstEvent != NULL) && (pvDetailInfo != NULL) && (pagpdItem != NULL) )
	{
		INT16 nStatus, nLayer, nRow, nColumn;
		INT32 lItemCount = -1;
		float fPrice;

		m_csDetailInfo.GetField(FALSE, pvDetailInfo, 0,
					NULL,				//Item TID
					&fPrice,			//ItemPrice
					&lItemCount,		//ItemCount
					&nStatus,			//Status
					&nLayer,			//Layer
					&nRow,				//Row
					&nColumn);			//Column

		bResult = m_pcsAgpmGrid->Add( ((AgpdEventNPCTradeData *)pstEvent->m_pvData)->m_pcsGrid, nLayer, nRow, nColumn, pagpdItem->m_pcsGridItem, 1, 1 );
		
		// edited by netong	- 2003. 11. 28
		// �ι��� ����� ������ m_pcsAgpmGrid->Add() �� FALSE�� �����Ѵ�. (�Ʒ� NPCTRADE_CB_ID_UPDATE_NPCTRADE_GRID �ݹ��� �ȺҸ���.)
		// �̺κ�.. �ѹ� ����Ʈ�� ���� NPC�� ��� �� �������� ���� �ʴ°ɷ� �Ѵ�. (�������� �ʴ°�쿡...)
		//if( bResult )
		//{
			AgpdItem		*pcsAgpdItem;

			pcsAgpdItem = m_pcsAgpmItem->GetItem( lIID );

			if( pcsAgpdItem != NULL )
			{
				if (pcsAgpdItem->m_pcsItemTemplate &&
					((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_bStackable)
				{
//					m_pcsAgpmItem->SetItemStackCount(pcsAgpdItem, ((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_lMaxStackableCount);
					m_pcsAgpmItem->SetItemStackCount(pcsAgpdItem, lItemCount);

					pcsAgpdItem->m_pcsGridItem->m_bWriteRightBottom	= TRUE;
//					sprintf(pcsAgpdItem->m_pcsGridItem->m_strRightBottom, "%d", ((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_lMaxStackableCount);
					sprintf(pcsAgpdItem->m_pcsGridItem->m_strRightBottom, "%d", lItemCount );
				}

				pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_TAB] = nLayer;
				pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_ROW] = nRow;
				pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_COLUMN] = nColumn;

				pcsAgpdItem->m_eStatus	= AGPDITEM_STATUS_NPC_TRADE;

				pcsAgpdItem->m_anPrevGridPos	= pcsAgpdItem->m_anGridPos;
				m_pcsAgpmItem->SetNPCPrice(pcsAgpdItem, fPrice);

				EnumCallback(NPCTRADE_CB_ID_UPDATE_NPCTRADE_GRID, (void *)pstEvent, NULL );
			}
		//}
	}

	return bResult;
}

//�������� �ȾҴٰ� �޽����� ���δ�.
BOOL AgcmEventNPCTrade::ProcessSellItem( INT32 lResult )
{
	BOOL			bResult;

	bResult = FALSE;

	if( lResult == AGPDEVENTNPCTRADE_RESULT_SELL_SUCCEEDED )
	{
		//�������� �� �ȾҴ�.
		bResult = EnumCallback(NPCTRADE_CB_MSG_SELL_ITEM, NULL, NULL);
	}
	else if( lResult == AGPDEVENTNPCTRADE_RESULT_SELL_EXCEED_MONEY )
	{
		//���� ���� ������ ����. ���� �ʹ� ����~~
		bResult = EnumCallback( NPCTRADE_CB_MSG_NOT_ENOUGH_SPACE_TO_RETURN_GHELLD, NULL, NULL );
	}
	else if( lResult == AGPDEVENTNPCTRADE_RESULT_INVALID_ITEMID )
	{
		//���������ʴ� �������� �ȷ��Ѵ�.
	}
	//�̰� �߸��� ��Ŷ�� �����ǵ�.....
	else
	{
	}

	return bResult;
}

//�������� ��ٰ� �޽����� ���δ�.
BOOL AgcmEventNPCTrade::ProcessBuyItem( INT32 lResult )
{
	BOOL			bResult;

	bResult = FALSE;

	if( lResult == AGPDEVENTNPCTRADE_RESULT_BUY_SUCCEEDED )
	{
		//������ ���Լ���!
		bResult = EnumCallback( NPCTRADE_CB_MSG_BUY_ITEM, NULL, NULL );
	}
	else if( lResult == AGPDEVENTNPCTRADE_RESULT_BUY_NOT_ENOUGH_MONEY )
	{
		//���� ������� �ʴ�.
		bResult = EnumCallback( NPCTRADE_CB_MSG_NOT_ENOUGH_GHELLD, NULL, NULL );
	}
	else if( lResult == AGPDEVENTNPCTRADE_RESULT_BUY_FULL_INVENTORY )
	{
		//�κ��� ��! ���� ���̻� ����� ����.
		bResult = EnumCallback( NPCTRADE_CB_MSG_INVEN_FULL, NULL, NULL );
	}
	else if( lResult == AGPDEVENTNPCTRADE_RESULT_INVALID_ITEMID )
	{
		//�������� �ʴ� �������� ����Ѵ�.
	}
	else if( lResult == AGPDEVENTNPCTRADE_RESULT_INVALID_ITEM_COUNT )
	{
		//������ �����Է��� �߸��Ǿ���.
	}
	else if( lResult == AGPDEVENTNPCTRADE_RESULT_INVALID_USE_CLASS )
	{
		// ĳ���Ͱ� �� ������ ����� �� (���� ��) ���� Ŭ������.
	}
	else if( lResult == AGPDEVENTNPCTRADE_RESULT_NO_SELL_TO_MURDERER )
	{
		// ������ ���� 2�̻󿡰Դ� ������ �Ǹ����� �ʴ´�.
	}
	//�̰� �߸��� ��Ŷ�� �����ǵ�.....
	else
	{
	}

	return bResult;
}

void *AgcmEventNPCTrade::MakeBasePacket( INT32 lOBJID, ApBaseType eType, ApdEventFunction eEventFunction )
{
	ApBase				*pcsApBase = NULL;
	void				*pvResult;

	pcsApBase = NULL;
	pvResult = NULL;

	if( lOBJID != 0 )
	{
		if( eType == APBASE_TYPE_OBJECT && m_pcsApmObject )
		{
			ApdObject			*pcsApdObject;

			pcsApdObject = m_pcsApmObject->GetObject( lOBJID );

			pcsApBase = (ApBase *)pcsApdObject;
		}
		else if( eType == APBASE_TYPE_CHARACTER )
		{
			AgpdCharacter		*pcsCharacter;

			pcsCharacter = m_pcsAgpmCharacter->GetCharacter( lOBJID );
			pcsApBase = (ApBase *)pcsCharacter;
		}
		else if( eType == APBASE_TYPE_ITEM )
		{
			AgpdItem			*pcsAgpdItem;

			pcsAgpdItem = m_pcsAgpmItem->GetItem( lOBJID );
			pcsApBase = (ApBase *)pcsAgpdItem;
		}
	}

	if( pcsApBase != NULL )
	{
		ApdEvent		csEvent;

		memset( &csEvent, 0, sizeof( csEvent ) );

		csEvent.m_eFunction = eEventFunction;
		csEvent.m_pcsSource = pcsApBase;
		csEvent.m_lEID = APDEVENT_EID_FLAG_LOCAL;

		pvResult = m_pcsApmEventManager->MakeBasePacket( &csEvent );
	}

	return pvResult;
}

BOOL AgcmEventNPCTrade::SendBuy( INT32 lCID, INT32 lIID, INT32 lItemCount, INT16 nStatus, INT8 lLayer, INT8 lRow, INT8 lColumn )
{
	BOOL			bResult;

	bResult = FALSE;

	if( (lCID != 0) && (m_lNPCID != 0 ) )
	{
		AgpdCharacter		*pcsAgpdCharacter;

		pcsAgpdCharacter = m_pcsAgpmCharacter->GetCharacter( lCID );

		if( pcsAgpdCharacter != NULL )
		{
			INT8				nOperation;
			INT16				nPacketLength;

			AgpdItem			*pcsAgpdItem;
			void				*pvEventBase;
			void				*pvDetailInfo;
			void				*pvPacket;

			pvEventBase = NULL;
			nOperation = AGPDEVENTNPCTRADE_BUYITEM;

			pvEventBase = MakeBasePacket( m_lNPCID, m_eNPCType, APDEVENT_FUNCTION_NPCTRADE );
			pcsAgpdItem = m_pcsAgpmItem->GetItem( lIID );

			if( (pvEventBase != NULL) && (pcsAgpdItem != NULL) )
			{
				pvDetailInfo = m_csDetailInfo.MakePacket( FALSE, &nPacketLength, AGPMEVENT_NPCTRADE_PACKET_TYPE,
															NULL,
															NULL,
															&lItemCount,
															&nStatus,
															&pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_TAB],
															&pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_ROW],
															&pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_COLUMN]
														);

				pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMEVENT_NPCTRADE_PACKET_TYPE,
															pvEventBase,    //pvEventBase
															&nOperation,    //nOperation
															&lCID,			//lCID
															&lIID,           //IID
															pvDetailInfo,	//pvDetailInfo
															NULL,			//lTimeStamp
															NULL,			//lResult
															(lLayer > (-1)) ? &lLayer : NULL,
															(lRow > (-1)) ? &lRow : NULL,
															(lColumn > (-1)) ? &lColumn : NULL
															);

				m_csPacket.FreePacket(pvEventBase);

				if (pvDetailInfo)
					m_csPacket.FreePacket(pvDetailInfo);

				if( pvPacket != NULL )
				{
					bResult = SendPacket( pvPacket, nPacketLength );
					
					m_csPacket.FreePacket(pvPacket);
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

	return bResult;
}

BOOL AgcmEventNPCTrade::SendSell( INT32 lCID, INT32 lIID, INT32 lItemCount )
{
	BOOL			bResult;

	bResult = FALSE;

	if( (lCID != 0) && (m_lNPCID != 0 ) )
	{
		AgpdCharacter		*pcsAgpdCharacter;

		pcsAgpdCharacter = m_pcsAgpmCharacter->GetCharacter( lCID );

		if( pcsAgpdCharacter != NULL )
		{
			INT8				nOperation;
			INT16				nPacketLength;

			AgpdItem			*pcsAgpdItem;
			void				*pvEventBase;
			void				*pvDetailInfo;
			void				*pvPacket;

			pvEventBase = NULL;
			nOperation = AGPDEVENTNPCTRADE_SELLITEM;

			pvEventBase = MakeBasePacket( m_lNPCID, m_eNPCType, APDEVENT_FUNCTION_NPCTRADE );
			pcsAgpdItem = m_pcsAgpmItem->GetItem( lIID );

			if( (pvEventBase != NULL) && (pcsAgpdItem != NULL) )
			{
				pvDetailInfo = m_csDetailInfo.MakePacket( FALSE, &nPacketLength, AGPMEVENT_NPCTRADE_PACKET_TYPE,
															NULL,
															NULL,
															&lItemCount,
															&pcsAgpdItem->m_eStatus,
															&pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_TAB],
															&pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_ROW],
															&pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_COLUMN]
														);

				pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMEVENT_NPCTRADE_PACKET_TYPE,
															pvEventBase,    //pvEventBase
															&nOperation,    //nOperation
															&lCID,			//lCID
															&lIID,          //IID
															pvDetailInfo,	//pvDetailInfo
															NULL,			//lTimeStamp
															NULL,			//lResult
															NULL,
															NULL,
															NULL
															);

				m_csPacket.FreePacket(pvEventBase);

				if (pvDetailInfo)
					m_csPacket.FreePacket(pvDetailInfo);

				if( pvPacket != NULL )
				{
					bResult = SendPacket( pvPacket, nPacketLength );
					
					m_csPacket.FreePacket(pvPacket);
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

	return bResult;
}

//������ ������ �����ش�.
BOOL AgcmEventNPCTrade::SendRequestItemList( INT32 lCID )
{
	BOOL			bResult;

	bResult = FALSE;

	if( (lCID != 0) && (m_lNPCID != 0) )
	{
		AgpdCharacter		*pcsAgpdCharacter;

		pcsAgpdCharacter = m_pcsAgpmCharacter->GetCharacter( lCID );

		if( pcsAgpdCharacter != NULL )
		{
			INT8				nOperation;
			void				*pvEventBase;
			void				*pvPacket;
			INT16				nPacketLength;
			INT32				lTimeStamp;

			pvEventBase = NULL;
			nOperation = AGPDEVENTNPCTRADE_REQUESTITEMLIST;
			lTimeStamp = 0;

			pvEventBase = MakeBasePacket( m_lNPCID, m_eNPCType, APDEVENT_FUNCTION_NPCTRADE );

			if( pvEventBase != NULL )
			{
				pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMEVENT_NPCTRADE_PACKET_TYPE,
															pvEventBase,    //pvEventBase
															&nOperation,    //nOperation
															&lCID,			//lCID
															NULL,           //IID
															NULL,			//pvDetailInfo
															&lTimeStamp,	//lTimeStamp
															NULL,			//lResult
															NULL,
															NULL,
															NULL
															);

				m_csPacket.FreePacket(pvEventBase);

				if( pvPacket != NULL )
				{
					bResult = SendPacket( pvPacket, nPacketLength );
					
					m_csPacket.FreePacket(pvPacket);
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

	return bResult;
}

BOOL AgcmEventNPCTrade::CBRequestNPCItemList(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmEventNPCTrade *	pThis = (AgcmEventNPCTrade *) pClass;
	ApdEvent *			pcsEvent = (ApdEvent *) pData;

	if (pThis->m_pcsAgpmCharacter->IsActionBlockCondition(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter))
		return FALSE;

	BOOL				bResult;

	bResult = FALSE;

	if( pcsEvent )
	{
		//��� NPC�� �ŷ��϶�� ������ �˸���.
		pThis->SetNPCInfo( pcsEvent->m_pcsSource->m_lID, pcsEvent->m_pcsSource->m_eType );

		bResult = pThis->SendRequestItemList( pThis->m_pcsAgcmCharacter->m_lSelfCID );
	}

	// pcsEvnet, pcsGenerator
	pThis->EnumCallback(NPCTRADE_CB_ID_ARISE_NPCTRADE_EVENT, pData, pCustData);

	return bResult;
}

BOOL AgcmEventNPCTrade::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	// m_csPacket
	void				*pvEventBase;
	INT8				nOperation;
	INT32				lRequesterCID;
	INT32				lIID;
	void				*pvDetailInfo;
//	void				*pstAttach;
	INT32				lTimeStamp;
	INT32				lResult;

	ApdEvent *			pstEvent = NULL;

	INT8				cLayer		= (-1);
	INT8				cRow		= (-1);
	INT8				cColumn		= (-1);


	m_csPacket.GetField(TRUE, pvPacket, nSize,  
						&pvEventBase, 
						&nOperation,
						&lRequesterCID,
						&lIID,
						&pvDetailInfo, 
						&lTimeStamp,
						&lResult,
						&cLayer,
						&cRow,
						&cColumn);

	if (pvEventBase)
		pstEvent = m_pcsApmEventManager->GetEventFromBasePacket(pvEventBase);

	if( nOperation == AGPDEVENTNPCTRADE_REQUESTITEMLIST )
	{
		//�����۸���Ʈ�� ����.
		ProcessItemList( pstEvent, lIID, pvDetailInfo );
	}
	else if( nOperation == AGPDEVENTNPCTRADE_SEND_ALL_ITEM_INFO )
	{
		EnumCallback( NPCTRADE_CB_ID_OPEN_UI_NPCTRADE, NULL, NULL );
	}
	else if( nOperation == AGPDEVENTNPCTRADE_SELLITEM )
	{
		//�������� �Ǵ�.
		ProcessSellItem( lResult );
		//������ �Ⱦ����� �κ��丮�� �����Ѵ�.
		EnumCallback( NPCTRADE_CB_ID_UPDATE_INVENTORY_GRID, NULL, NULL );
	}
	else if( nOperation == AGPDEVENTNPCTRADE_BUYITEM )
	{
		//�������� ���.
		ProcessBuyItem( lResult );
		//������ �Ⱦ����� �κ��丮�� �����Ѵ�.
		EnumCallback( NPCTRADE_CB_ID_UPDATE_INVENTORY_GRID, NULL, NULL );
	}
	//�߸��� ��Ŷ�̴�. ����� �ش�. ID�� ��Ÿ ���� ������ �ξ��ٰ� �α׸� ������~
	else
	{
		;
	}

	return TRUE;
}
