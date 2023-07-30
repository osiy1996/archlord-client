/*
#include <AgcmUICouponBox/AgcmUICouponBox.h>
#include <AgcmUIItem/AgcmUIItem.h>

//-----------------------------------------------------------------------
//

CouponTexture::CouponTexture( void )
{
	m_nItemID = 0;
	m_nItemTID = 0;
	m_nStackCount = 0;

	m_ppdItem = NULL;
	m_bIsEnableToolTip = TRUE;
}

CouponTexture::~CouponTexture( void )
{
}

void CouponTexture::OnSetFocus( void )
{
	// AcUIBase ����ó��
	AcUIBase::OnSetFocus();

	if( m_bIsEnableToolTip )
	{
		// ���� ����
		AgcmUIItem* pcmUIItem = ( AgcmUIItem* )g_pEngine->GetModule( "AgcmUIItem" );
		if( !pcmUIItem ) return;

		// ������ ���� �� ������ ���..
		// �� ��Ʈ���� ��ǥ�� �� ��Ʈ���� �� UI �� ��ǥ�� ���ϸ� �Ǳ�~

		int nPosX = m_lAbsolute_x;
		int nPosY = m_lAbsolute_y;

		pcmUIItem->OpenToolTip( m_ppdItem, nPosX, nPosY, FALSE, FALSE );
	}
}

void CouponTexture::OnKillFocus( void )
{
	// AcUIBase ����ó��
	AcUIBase::OnKillFocus();

	// ���� �ݱ�
	AgcmUIItem* pcmUIItem = ( AgcmUIItem* )g_pEngine->GetModule( "AgcmUIItem" );
	if( !pcmUIItem ) return;

	pcmUIItem->CloseToolTip();
}

void CouponTexture::OnWindowRender( void )
{
	AcUIBase::OnWindowRender();
}

void CouponTexture::SetItemTID( int nTID, int nStackCount )
{
	if( m_nItemTID != nTID )
	{
		AgpmItem* ppmItem = ( AgpmItem* )g_pEngine->GetModule( "AgpmItem" );
		AgcmCashMall* pcmCashMall = ( AgcmCashMall* )g_pEngine->GetModule( "AgcmCashMall" );
		if( !ppmItem || !pcmCashMall ) return;

		// ������ ������� ���̾������� ������ ����
		if( m_ppdItem )
		{
			pcmCashMall->DestroyDummyItem( m_ppdItem );
			m_nItemID = 0;
			m_ppdItem = NULL;
		}

		m_nItemTID = nTID;
		m_nStackCount = nStackCount;

		m_ppdItem = pcmCashMall->CreateDummyItem( m_nItemTID, m_nStackCount );
		if( m_ppdItem )
		{
			m_nItemID = m_ppdItem->m_lID;
		}
	}
}

void CouponTexture::SetTexture( RwTexture* pTexture )
{
	if( !pTexture )
	{
		SetRednerTexture( 0 );
		return;
	}

	int nTextureID = m_csTextureList.AddTexture( pTexture );
	SetRednerTexture( nTextureID );
}

void CouponTexture::SetTexture( char* pFileName )
{
	if( !pFileName || strlen( pFileName ) <= 0 )
	{
		SetRednerTexture( 0 );
		return;
	}

	int nTextureID = m_csTextureList.AddImage( pFileName );
	SetRednerTexture( nTextureID );
}

//-----------------------------------------------------------------------
//

//-----------------------------------------------------------------------*/