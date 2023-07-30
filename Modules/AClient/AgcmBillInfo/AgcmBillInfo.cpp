#include "AgcmBillInfo.h"
#include <AgcmUIControl/AgcmUIControl.h>
#include <AgcmUIMain/AgcmUIMain.h>
#include <AgpmGrid/AgpdGrid.h>
#include <AgpmGrid/AgpmGrid.h>
#include <AgpmBillInfo/AgpmBillInfo.h>

AgcmBillInfo::AgcmBillInfo()
:	m_pcsAgcmUIControl(NULL)
#ifdef _AREA_GLOBAL_
,	m_pGridPCRoom(NULL)
#elif !defined(_AREA_CHINA_)
,	m_pGridHangameS(NULL)
,	m_pGridHangameTPack(NULL)
#endif
,	m_pcsAgpmGrid(NULL)
,	m_pcsAgcmUIMain(NULL)
{
	SetModuleName("AgcmBillInfo");
}

AgcmBillInfo::~AgcmBillInfo()
{
}

BOOL AgcmBillInfo::OnAddModule()
{
	m_pcsAgpmBillInfo	= (AgpmBillInfo *)	GetModule("AgpmBillInfo");

	if (!m_pcsAgpmBillInfo)
		return FALSE;

	if (!m_pcsAgpmBillInfo->SetCallbackUpdateBillInfo(CBUpdateBillInfo, this))
		return FALSE;

	return TRUE;
}

BOOL	AgcmBillInfo::OnInit( VOID )
{

	m_pcsAgcmUIControl		=	static_cast< AgcmUIControl* >( GetModule( "AgcmUIControl" ) );
	m_pcsAgpmGrid			=	static_cast< AgpmGrid* >( GetModule( "AgpmGrid") );
	m_pcsAgcmUIMain			=	static_cast< AgcmUIMain* >( GetModule( "AgcmUIMain" ) );

	return TRUE;
}

BOOL AgcmBillInfo::CBUpdateBillInfo(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmBillInfo	*pThis			= (AgcmBillInfo *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	AgpdBillInfo	*pcsAttachData	= pThis->m_pcsAgpmBillInfo->GetADCharacter(pcsCharacter);

#ifdef _AREA_GLOBAL_
	if( pcsAttachData && pcsAttachData->m_bIsPCRoom )
	{
		return pThis->AddSystemGridIcon_PCRoom( pcsAttachData->m_ulRemainCouponPlayTime );
	}
	
	return pThis->m_pcsAgcmUIMain->RemoveSystemMessageGridItem( pThis->m_pGridPCRoom );
#elif !defined(_AREA_CHINA_)
	if( DEF_FLAG_CHK( pcsAttachData->m_ulPCRoomType , AGPDPCROOMTYPE_HANGAME_S ) )
	{
		pThis->_AddSystemGridHangameS();
	}

	else
	{
		if( pThis->m_pGridHangameS )
			pThis->m_pcsAgcmUIMain->RemoveSystemMessageGridItem( pThis->m_pGridHangameS );
	}

	if( DEF_FLAG_CHK( pcsAttachData->m_ulPCRoomType , AGPDPCROOMTYPE_HANGAME_TPACK) )
	{
		pThis->_AddSystemGridHangameTPack();
	}

	else
	{
		if( pThis->m_pGridHangameTPack )
			pThis->m_pcsAgcmUIMain->RemoveSystemMessageGridItem( pThis->m_pGridHangameTPack );
	}

	return TRUE;
#endif
}

#ifdef _AREA_GLOBAL_
BOOL AgcmBillInfo::AddSystemGridIcon_PCRoom( UINT32 nExpireTime )
{
	if( !m_pGridPCRoom )
	{
		m_pGridPCRoom =	m_pcsAgpmGrid->CreateGridItem();
		if( !m_pGridPCRoom ) return FALSE;
	}

	m_pGridPCRoom->m_eType = AGPDGRID_ITEM_TYPE_ITEM;

	RwTexture** ppGridTexture = m_pcsAgcmUIControl->GetAttachGridItemTextureData( m_pGridPCRoom );
	if( !ppGridTexture ) return FALSE;

	*ppGridTexture = m_pcsAgcmUIControl->m_pTexturePCRoom;
	m_pGridPCRoom->SetTooltip( "PCRoom" );

	return m_pcsAgcmUIMain->AddSystemMessageGridItem( m_pGridPCRoom, NULL, NULL, FALSE, nExpireTime );
}
#elif !defined(_AREA_CHINA_)
VOID	AgcmBillInfo::_AddSystemGridHangameS( VOID )
{

	if( !m_pGridHangameS )
		m_pGridHangameS		=	m_pcsAgpmGrid->CreateGridItem();

	if( !m_pGridHangameS )
		return;

	m_pGridHangameS->m_eType		= AGPDGRID_ITEM_TYPE_ITEM;

	RwTexture **ppGridTexture	= m_pcsAgcmUIControl->GetAttachGridItemTextureData( m_pGridHangameS );
	if (ppGridTexture)
		*ppGridTexture	= m_pcsAgcmUIControl->m_pTextureHangameS;

	m_pGridHangameS->SetTooltip( "S PC" );

	m_pcsAgcmUIMain->AddSystemMessageGridItem( m_pGridHangameS , NULL , NULL , TRUE );
}

VOID	AgcmBillInfo::_AddSystemGridHangameTPack( VOID )
{
	if( !m_pGridHangameTPack )
		m_pGridHangameTPack	=	m_pcsAgpmGrid->CreateGridItem();

	if( !m_pGridHangameTPack )
		return;

	m_pGridHangameTPack->m_eType	=	AGPDGRID_ITEM_TYPE_ITEM;

	RwTexture	**ppGridTexture	=	m_pcsAgcmUIControl->GetAttachGridItemTextureData( m_pGridHangameTPack );
	if( ppGridTexture )
		*ppGridTexture	=	m_pcsAgcmUIControl->m_pTextureHangameT;

	m_pGridHangameTPack->SetTooltip( "T Pack" );
	
	m_pcsAgcmUIMain->AddSystemMessageGridItem( m_pGridHangameTPack , NULL , NULL , TRUE );
}
#endif