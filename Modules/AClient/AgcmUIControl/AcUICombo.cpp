
#include <AgcmUIControl/AcUICombo.h>
#include <ApMemoryTracker/ApMemoryTracker.h>
#include <AgcmFont/AgcmFont.h>
#include <AuProfileManager/AuProfileManager.h>
#include <AgcmUIControl/AcUIButton.h>
#include <AgcModule/AgcEngine.h>

extern AgcEngine *	g_pEngine;

#define ACUICOMBO_STRING_COLOR_ENABLE					0xffffffff
#define ACUICOMBO_STRING_COLOR_DISABLE					0xff777777

AcUICombo::AcUICombo( void )
{
	m_nType						= TYPE_COMBO;
	m_listComboItem				= NULL;
	m_lListItemNum				= 0;
	w							= ACUICOMBO_MIN_WIDTH;
	m_bComboOpenMode			= FALSE;
	m_lSelectedIndex			= -1;
	m_pSelectedItem				= NULL;
	m_pcsComboButton			= NULL;
	m_bDrawStaticString			= FALSE;
	m_lMouseOnIndex				= -1;
	m_stRenderInfo.lRenderID	= 0;

	memset( m_lDropTextureID, 0, sizeof( INT32 ) * ACUICOMBO_MAX_TEXTURE );
}

AcUICombo::~AcUICombo( void )
{
	ClearAllString();
}

/*****************************************************************
*   Function : OnInit
*   Comment  : Init virtual function 
*   Date&Time : 2003-07-25, ���� 3:54
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
BOOL AcUICombo::OnInit( void )
{
	AcUIBase::OnInit();
	return TRUE;
}

/*****************************************************************
*   Function : AddString
*   Comment  : String�� Set �Ѵ�. 
*			   lComboData : Combo User Data
*			   bFirst : TRUE��� ���� Data�� ��� �������� 	
*	Return	 : Index�� Return �ȴ�. ���нÿ��� -1 Return 
*   Date&Time : 2003-07-25, ���� 3:21
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
INT32 AcUICombo::AddString( CHAR* szString, INT32 lComboData, BOOL bFirst )
{
	// �켱 bFirstó��
	if( bFirst )
	{
		ClearAllString( FALSE );
	}
	
	INT32 lStringLength = strlen( szString );
	if( 0 >= lStringLength ) return -1;

	AcUIComboItem* pNewItem	= new AcUIComboItem;
	memset( pNewItem->szComboString, 0, sizeof( pNewItem->szComboString ) );
	strncpy( pNewItem->szComboString, szString, ( ACUICOMBO_STRING_MAX_LENGTH - 1 ) );

	pNewItem->lComboData = lComboData;
	pNewItem->bEnable =	TRUE;
	pNewItem->pcsNext =	NULL;

	AcUIComboItem* pList = m_listComboItem;
	if( pList )
	{
		// ����Ʈ�� �� �������� �� ������ �߰�
		while( pList->pcsNext )
		{
			pList = pList->pcsNext;
		}

		pList->pcsNext = pNewItem;
	}
	else
	{
		// ����Ʈ�� ó������ ���� �Ÿ� �� ó���� �߰�
		m_listComboItem = pNewItem;
	}

	if( m_lListItemNum == m_lSelectedIndex )
	{
		m_pSelectedItem = pNewItem;
	}
	else if( m_lSelectedIndex == -1 )
	{
		m_lSelectedIndex = m_lListItemNum;
		m_pSelectedItem = pNewItem;
	}
	
	++m_lListItemNum;
	return ( m_lListItemNum - 1 );
}

/*****************************************************************
*   Function : ClearAllString
*   Comment  : ClearAllString
*   Date&Time : 2003-07-25, ���� 3:35
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
void AcUICombo::ClearAllString( BOOL bResetSelection )
{
	AcUIComboItem* pList = m_listComboItem;
	AcUIComboItem* temp = NULL;
	
	while( pList )
	{
		temp = pList->pcsNext;
		delete pList;
		pList = temp;
	}

	m_listComboItem	= NULL;
	m_lListItemNum = 0;

	m_pSelectedItem = NULL;

	if( bResetSelection )
	{
		m_lSelectedIndex = -1;
	}
}

/*****************************************************************
*   Function : OnMoveWindow
*   Comment  : Move Window virtual function 
*   Date&Time : 2003-07-25, ���� 3:44
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
void AcUICombo::OnMoveWindow( void )
{
	AcUIBase::OnMoveWindow();

	// Combo Control�� Min Widht�� �ִ�.
	if( w < ACUICOMBO_MIN_WIDTH )
	{
		w = ACUICOMBO_MIN_WIDTH;
	}
}

/*****************************************************************
*   Function : OnClose
*   Comment  : Close virtual function 
*   Date&Time : 2003-07-25, ���� 4:01
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
void AcUICombo::OnClose( void )
{
	AcUIBase::OnClose();

	// List ���� 
	ClearAllString();
}

/*****************************************************************
*   Function : OnCommand
*   Comment  : Command virtual function 
*   Date&Time : 2003-07-25, ���� 4:15
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
BOOL AcUICombo::OnCommand( INT32 nID , PVOID pParam	)
{
	if( m_pcsComboButton && *( INT32* )pParam == m_pcsComboButton->m_lControlID )
	{
		switch( nID )
		{
		case UICM_BUTTON_MESSAGE_CLICK :		// �޺���ư�� Ŭ���� ���
			{
				// ���� ������ �ݰ� ���� ������ ����.
				if( m_bComboOpenMode )
				{
					CloseComboControl();
				}
				else
				{
					OpenComboControl();
				}

				return TRUE;
			}
			break;

		case UICM_BASE_MESSAGE_CLOSE :			// �ݾƹ��� ���
			{
				// �޺���ư ���� ����
				this->SetComboButton( NULL );
			}
			break;
		}
	}
		
	return AcUIBase::OnCommand( nID, pParam );
}

/*****************************************************************
*   Function : OnWindowRender
*   Comment  : Window Render virtulal function 
*   Date&Time : 2003-07-25, ���� 4:22
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
void AcUICombo::OnWindowRender( void )
{
	PROFILE("AcUICombo::OnWindowRender");
	AcUIBase::OnWindowRender();

	// �׸��� 
	INT32	lFontHeight = m_pAgcmFont->m_astFonts[ m_lFontType ].lSize;
	INT32	lAbsX = 0;
	INT32	lAbsY = 0;
	ClientToScreen( &lAbsX, &lAbsY );

	// ��Ʈ ��� ����
	m_pAgcmFont->FontDrawStart( m_lFontType );

	// �������� �������� ������ �ؽ�Ʈ�� ��´�.
	_DrawSelectedItemText( m_pSelectedItem, lAbsX, lAbsY, lFontHeight );

	// �޺�����Ʈ�� ���� ���¸� ����Ʈ �׸���� ����ش�.
	_DrawComboItemList( m_listComboItem, lAbsX, lAbsY, lFontHeight );

	// ��Ʈ ��� ��
	m_pAgcmFont->FontDrawEnd();
}

void AcUICombo::OpenComboControl( void )
{
	if( m_bComboOpenMode ) return;

	SetModal();
	m_bComboOpenMode = TRUE			;

	// Windowũ�⸦ �ø���
	m_lOriginalHeight = h;
	h += m_lLineHeight * m_lListItemNum;
}

void AcUICombo::CloseComboControl( void )
{
	ReleaseModal();
	m_bComboOpenMode = FALSE		;

	// Windowũ�⸦ ���󺹱� �Ѵ� 
	h = m_lOriginalHeight;
	if( m_pcsComboButton )
	{
		m_pcsComboButton->SetButtonMode( ACUIBUTTON_MODE_NORMAL, FALSE );
	}
}

BOOL AcUICombo::OnLButtonDown( RsMouseStatus* ms )
{
	// Open Mode �϶��� LButton Down�� �ʿ��ϴ� 
	if( m_bComboOpenMode )
	{
		AcUIComboItem* pList = m_listComboItem;
		INT32 lSelectedIndex = 0;

		while( pList )
		{
			// ���õǾ���
			if( ms->pos.y >= ( FLOAT )( m_lOriginalHeight + m_lLineHeight * lSelectedIndex ) &&
				ms->pos.y < ( FLOAT )( m_lOriginalHeight + m_lLineHeight * ( lSelectedIndex + 1 ) ) )
			{
				SelectIndex( lSelectedIndex );
				break;
			}

			++lSelectedIndex;
			pList = pList->pcsNext;
		}

		CloseComboControl();
		return TRUE;
	}
	else if( m_pcsComboButton )
	{
		m_pcsComboButton->OnLButtonDown( ms );
	}
	else
	{
		return AcUIBase::OnLButtonDown( ms );
	}

	return TRUE;
}

AcUIComboItem* AcUICombo::GetComboItem( CHAR *szString, INT32 *plIndex )
{
	AcUIComboItem* pList = m_listComboItem;
	if( plIndex )
	{
		*plIndex = 0;
	}
		
	while( pList )
	{
		// Find!
		if( !strcmp( pList->szComboString, szString ) )
		{
			return pList;
		}

		pList = pList->pcsNext;
		if( plIndex )
		{
			++( *plIndex );
		}
	}

	return NULL;
}

AcUIComboItem* AcUICombo::GetComboItem( INT32 lIndex )
{
	AcUIComboItem* pList = m_listComboItem;
	INT32 lHereIndex = 0;
		
	while( pList )
	{
		// Find!
		if( lHereIndex == lIndex )
		{
			return pList;
		}

		pList = pList->pcsNext;
		++lHereIndex;
	}

	return NULL;	
}

/*****************************************************************
*   Function : GetItemData
*   Comment  : szString�� �ش��ϴ� User Data �� lUserData�� Return �Ѵ�
*			   Get ������ TRUE�� Return, ���н� FALSE�� Return �Ѵ� 
*   Date&Time : 2003-08-12, ���� 10:32
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
BOOL AcUICombo::GetItemData( CHAR* szString , INT32* lComboData )
{
	AcUIComboItem* pList = GetComboItem( szString );
	if( pList )
	{
		( *lComboData ) = pList->lComboData;
		return TRUE;
	}

	return FALSE;
}

/*****************************************************************
*   Function : GetItemData
*   Comment  : lIndex�� �ش��ϴ� User Data �� lUserData�� Return �Ѵ�
*			   Get ������ TRUE�� Return, ���н� FALSE�� Return �Ѵ� 
*   Date&Time : 2003-08-12, ���� 10:36
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
BOOL AcUICombo::GetItemData( INT32 lIndex, INT32* lComboData )
{
	AcUIComboItem* pList = GetComboItem( lIndex );
	if( pList )
	{
		( *lComboData ) = pList->lComboData;
		return TRUE;
	}

	return FALSE;
}

/*****************************************************************
*   Function : DeleteString
*   Comment  : szString�� �ش��ϴ� Combo Data�� �����Ѵ� 
*   Date&Time : 2003-08-12, ���� 10:40
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
BOOL AcUICombo::DeleteString( CHAR* szString )
{
	AcUIComboItem* pList = m_listComboItem;
	AcUIComboItem* temp = NULL;

	while( pList )
	{
		// Find!
		if( !strcmp( pList->szComboString, szString ) )
		{
			if( pList == m_pSelectedItem )
			{
				m_pSelectedItem	= NULL;
				m_lSelectedIndex = -1;
			}

			// First List Index 
			if( NULL == temp )
			{
				m_listComboItem = pList->pcsNext ;
			}
			// Last List Index
			else if( pList->pcsNext == NULL )
			{
				temp->pcsNext = NULL;
			}
			// ETC
			else 
			{
				temp->pcsNext = pList->pcsNext;
			}
			
			delete pList;	
			--m_lListItemNum;
			return TRUE;
		}

		temp = pList;
		pList = pList->pcsNext;
	}

	return FALSE;		
}

/*****************************************************************
*   Function : DeleteString
*   Comment  : lIndex�� �ش��ϴ� Combo Data�� �����Ѵ� 
*   Date&Time : 2003-08-12, ���� 10:40
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
BOOL AcUICombo::DeleteString( INT32 lIndex )
{
	AcUIComboItem* pList = m_listComboItem;
	AcUIComboItem* temp = NULL;
	INT32 lHereIndex = 0;

	while( pList )
	{
		// Find!
		if( lHereIndex == lIndex )
		{
			if( pList == m_pSelectedItem )
			{
				m_pSelectedItem	= NULL;
				m_lSelectedIndex = -1;
			}

			// First List Index 
			if( NULL == temp )
			{
				m_listComboItem = pList->pcsNext;
			}
			// Last List Index
			else if( pList->pcsNext == NULL )
			{
				temp->pcsNext = NULL;
			}
			// ETC
			else
			{
				temp->pcsNext = pList->pcsNext;
			}
			
			delete pList;
			--m_lListItemNum;
			return TRUE;			
		}

		temp = pList;
		pList = pList->pcsNext;
		++lHereIndex;
	}

	return FALSE;	
}

/*****************************************************************
*   Function : SetItemData
*   Comment  : szString�� �´� Combo Data�� �����Ѵ� 
*   Date&Time : 2003-08-12, ���� 10:57
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
BOOL AcUICombo::SetItemData( CHAR* szString, INT32 lNewComboData )
{
	AcUIComboItem* pList = GetComboItem( szString );
	if( pList )
	{
		pList->lComboData = lNewComboData;
		return TRUE;
	}

	return FALSE;
}

/*****************************************************************
*   Function : SetItemData
*   Comment  : lIndex�� �´� Combo Data�� �����Ѵ� 
*   Date&Time : 2003-08-12, ���� 10:57
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
BOOL AcUICombo::SetItemData( INT32 lIndex, INT32 lNewComboData )
{
	AcUIComboItem* pList = GetComboItem( lIndex );
	if( pList )
	{
		pList->lComboData = lNewComboData;
		return TRUE;
	}

	return FALSE;
}

CHAR* AcUICombo::GetSelectedString( void )
{
	if( !m_pSelectedItem ) return NULL;
	return m_pSelectedItem->szComboString;
}

INT32 AcUICombo::GetSelectedIndex( void )
{
	if( !m_pSelectedItem ) return -1;
	return m_lSelectedIndex;
}

BOOL AcUICombo::SelectString( CHAR* szString )
{
	if( m_pSelectedItem && szString && !strcmp( m_pSelectedItem->szComboString, szString ) ) return TRUE;

	INT32 lIndex = 0;
	
	AcUIComboItem* pList = GetComboItem( szString, &lIndex );
	if( pList && !pList->bEnable ) return FALSE;

	m_pSelectedItem	= NULL;
	if( pList )
	{
		m_pSelectedItem = pList;
		m_lSelectedIndex = lIndex;
	}

	if( !m_pSelectedItem )
	{
		m_lSelectedIndex = -1;
	}

	if( NULL != pParent )
	{
		pParent->SendMessage( MESSAGE_COMMAND, ( PVOID ) UICM_COMBO_MESSAGE_SELECT_CHANGED, ( PVOID ) &m_lControlID );
	}

	return TRUE;
}

BOOL AcUICombo::SelectIndex( INT32 lIndex )
{
	if( lIndex >= m_lListItemNum ) lIndex = -1;
	if( m_lSelectedIndex == lIndex ) return TRUE;

	AcUIComboItem* pList = GetComboItem( lIndex );
	if( pList && !pList->bEnable ) return FALSE;

	m_pSelectedItem	= NULL;
	if( pList )
	{
		m_pSelectedItem = pList;
		m_lSelectedIndex = lIndex;
	}

	if( !m_pSelectedItem ) m_lSelectedIndex = -1;
	if( NULL != pParent )
	{
		pParent->SendMessage( MESSAGE_COMMAND, ( PVOID )UICM_COMBO_MESSAGE_SELECT_CHANGED, ( PVOID )&m_lControlID );
	}

	return TRUE;
}

BOOL AcUICombo::EnableString( CHAR* szString, BOOL bEnable )
{
	AcUIComboItem* pList = GetComboItem( szString );
	if( pList )
	{
		pList->bEnable = bEnable;
		return TRUE;
	}

	return FALSE;
}

BOOL AcUICombo::EnableString( INT32 lIndex, BOOL bEnable )
{
	AcUIComboItem* pList = GetComboItem( lIndex ) ;
	if( pList )
	{
		pList->bEnable = bEnable;
		return TRUE;
	}

	return FALSE;
}

VOID AcUICombo::SetComboButton( AcUIButton *pcsButton )
{
	if( m_pcsComboButton )
	{
		m_pcsComboButton->m_stProperty.m_bClickOnButtonDown	= FALSE;
	}

	m_pcsComboButton = pcsButton;
	if( m_pcsComboButton )
	{
		m_pcsComboButton->MoveWindow( w - m_pcsComboButton->w, 0, m_pcsComboButton->w, h );
		m_pcsComboButton->m_stProperty.m_bClickOnButtonDown	= TRUE;
	}
}

void AcUICombo::WindowRender( void )
{
	if( !m_Property.bVisible ) return;

	AgcWindowNode* cur_node = m_listChild.head;
	while( cur_node )
	{
		cur_node->pWindow->WindowRender();
		cur_node = cur_node->next;
	}

	// ��� ���ϵ带 ���� �����Ѵ�.
	OnWindowRender();
	return;	
}

BOOL AcUICombo::OnMouseMove( RsMouseStatus *ms )
{
	if( AcUIBase::OnMouseMove( ms ) ) return TRUE;
	m_lMouseOnIndex = -1;

	if( !m_bComboOpenMode ) return FALSE;
	if( ms->pos.y < m_lOriginalHeight ) return FALSE;

	m_lMouseOnIndex = ( ( int )ms->pos.y - m_lOriginalHeight ) / m_lLineHeight;
	return FALSE;
}

VOID AcUICombo::SetDropDownTexture( INT32 lMode, INT32 lTextureID )
{
	if( lMode < 0 || lMode >= ACUICOMBO_MAX_TEXTURE ) return;
	m_lDropTextureID[ lMode ] = lTextureID;
}

RwTexture* AcUICombo::_GetTextureNormal( void )
{
	return m_csTextureList.GetImage_ID( m_lDropTextureID[ ACUICOMBO_TEXTURE_NORMAL ] );
}

RwTexture* AcUICombo::_GetTextureOn( void )
{
	return m_csTextureList.GetImage_ID( m_lDropTextureID[ ACUICOMBO_TEXTURE_ONMOUSE ] );
}

RwTexture* AcUICombo::_GetTextureBottom( void )
{
	return m_csTextureList.GetImage_ID( m_lDropTextureID[ ACUICOMBO_TEXTURE_BOTTOM ] );
}

INT32 AcUICombo::_GetTextureWidth( RwTexture* pTexture )
{
	if( !pTexture ) return 0;

	RwRaster* pRaster = RwTextureGetRaster( pTexture );
	if( !pRaster ) return 0;

	return RwRasterGetWidth( pRaster );
}

INT32 AcUICombo::_GetTextureHeight( RwTexture* pTexture )
{
	if( !pTexture ) return 0;

	RwRaster* pRaster = RwTextureGetRaster( pTexture );
	if( !pRaster ) return 0;

	return RwRasterGetHeight( pRaster );
}

void AcUICombo::_DrawComboTexture( RwTexture* pTexture, float fPosX, float fPosY, float fWidth, float fHeight, float fUStart, float fVStart, float fUEnd, float fVEnd )
{
	if( !pTexture ) return;
	g_pEngine->DrawIm2D( pTexture, fPosX, fPosY, fWidth, fHeight, fUStart, fVStart, fUEnd, fVEnd );
}

void AcUICombo::_DrawComboText( char* pString, float fPosX, float fPosY, UINT32 nFontType, UINT8 nAlpha, DWORD dwColor )
{
	if( !m_pAgcmFont ) return;
	if( !pString || strlen( pString ) <= 0 ) return;

	m_pAgcmFont->DrawTextIM2D( fPosX, fPosY, pString, nFontType, nAlpha, dwColor );	
}

void AcUICombo::_DrawSelectedItemText( AcUIComboItem* pItem, INT32 nAbsX, INT32 nAbsY, INT32 nFontHeight )
{
	if( !pItem ) return;

	AgcdFontClippingBox	stBox = { ( float )nAbsX, ( float )nAbsY, w, h };
	if( m_pcsComboButton )
	{
		stBox.w -= m_pcsComboButton->w;
	}

	INT32 lIndex = m_bComboOpenMode ? m_lOriginalHeight : h;
	UINT8 nAlpha = ( UINT8 )( 255.0f * ( m_pfAlpha ? *m_pfAlpha : 1 ) );

	m_pAgcmFont->SetClippingArea( &stBox );

	_DrawComboText( pItem->szComboString, nAbsX + 3.0f, nAbsY + ( lIndex - nFontHeight ) / 2.0f, m_lFontType, nAlpha, 
					m_pSelectedItem->bEnable ? ACUICOMBO_STRING_COLOR_ENABLE : ACUICOMBO_STRING_COLOR_DISABLE );

	m_pAgcmFont->SetClippingArea( NULL );
}

void AcUICombo::_DrawComboItemList( AcUIComboItem* pItem, INT32 nAbsX, INT32 nAbsY, INT32 nFontHeight )
{
	if( !m_bComboOpenMode ) return;

	RwTexture* pTextureNormal	= _GetTextureNormal();
	RwTexture* pTextureOn		= _GetTextureOn();
	RwTexture* pTextureBottom	= _GetTextureBottom();

	INT32 lImageWidth	= _GetTextureWidth( pTextureNormal );
	INT32 lImageHeight	= _GetTextureHeight( pTextureNormal );
	INT32 lStartY		= nAbsY + m_lOriginalHeight;
	INT32 lFontOffsetY	= ( m_lLineHeight - nFontHeight ) / 2;
	UINT8 nAlpha = ( UINT8 )( 255.0f * ( m_pfAlpha ? *m_pfAlpha : 1 ) );

	INT32 lIndex = 0;
	while( pItem )
	{
		// ������ �ٴ��� ���..
		_DrawComboTexture( lIndex == m_lMouseOnIndex ? pTextureOn : pTextureNormal,
							( float )nAbsX, ( float )lStartY, ( float )lImageWidth, ( float )lImageHeight,
							0.0f, 0.0f, 1.0f, 1.0f );

		// ������ �ؽ�Ʈ�� ���..
		_DrawComboText( pItem->szComboString, ( float )nAbsX + 3.0f, ( float )lStartY + lFontOffsetY, m_lFontType, nAlpha, 
						pItem->bEnable ? ACUICOMBO_STRING_COLOR_ENABLE : ACUICOMBO_STRING_COLOR_DISABLE );

		// ������ ���� �غ��ϰ�..
		pItem = pItem->pcsNext ;
		lStartY += m_lLineHeight;
		++lIndex;
	}

	// �������� �عٴ��� ��´�.
	if( pTextureBottom )
	{
		_DrawComboTexture( pTextureBottom, ( float )nAbsX, ( float )lStartY,
							( float )_GetTextureWidth( pTextureBottom ), ( float )_GetTextureHeight( pTextureBottom ),
							0.0f, 0.0f, 1.0f, 1.0f );
	}
}