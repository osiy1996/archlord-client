
#include <AgcmRecruit/AcUIRecruitWrite.h>
#include <AgcmRecruit/AgcmRecruit.h>
#include <AgcModule/AgcEngine.h>
#include <AgpmRecruit/AgpmRecruit.h>

AcUIRecruitWrite::AcUIRecruitWrite()
{
	m_pAgcmRecruit				=	NULL	;
	m_clPersonNum.m_bNumberOnly =	TRUE	;
	m_clLevelMin.m_bNumberOnly	=	TRUE	;
	m_clLevelMax.m_bNumberOnly	=	TRUE	;
}

/*****************************************************************
*   Function : SetWriteWindowInit
*   Comment  : Window���� ��� Control�� �ʱ�ȭ�Ѵ�
*   Date&Time : 2003-06-26, ���� 4:04
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
void	AcUIRecruitWrite::SetWriteWindowInit()
{
	m_clPurpose.ClearText();
	m_clPersonNum.ClearText();
	m_clLevelMin.ClearText();
	m_clLevelMax.ClearText();

	m_clPersonNum.SetText( "1" );
	m_clLevelMin.SetText( "1" );
	m_clLevelMax.SetText( "1" );
	
	for ( int i = 0 ; i < ACUIRECRUITWRITE_CLASS_NUM ; ++i )
		m_clClassPush[i].SetButtonMode( 0 );

	if( !g_pEngine->GetCharCheckState() )
	{
		m_clPurpose.SetMeActiveEdit();
	}	

	return;
}

/*****************************************************************
*   Function : SetWriteWindowClose
*   Comment  : // Write Window ���� Control���� ���� ó�� �Ѵ� - Edit
*   Date&Time : 2003-06-26, ���� 4:04
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
void	AcUIRecruitWrite::SetWriteWindowClose()
{
	// Eidt Control ���� 
	/*
	if( g_pEngine->GetCharCheckState() )	g_pEngine->SetCharCheckIn();
	m_clPurpose.m_bActive	= false;
	m_clPersonNum.m_bActive = false;
	m_clLevelMin.m_bActive	= false;
	m_clLevelMax.m_bActive	= false;
	*/
}

/*****************************************************************
*   Function : OnInit
*   Comment  : OnInit
*   Date&Time : 2003-06-26, ���� 4:04
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
BOOL	AcUIRecruitWrite::OnInit			(					)
{
	// Purpose Edit
	m_clPurpose.MoveWindow( ACUIRECRUITWRITE_PURPOSTEDIT_X, ACUIRECRUITWRITE_PURPOSTEDIT_Y, 
		ACUIRECRUITWRITE_PURPOSTEDIT_WIDTH, ACUIRECRUITWRITE_PURPOSTEDIT_HEIGHT );
	AddChild( &m_clPurpose, ACUIRECRUITWRITE_CONTROLID_PURPOSE_EDIT );		
	
	// Edit1 ( �ʿ��ο� Edit )
	m_clPersonNum.MoveWindow( ACUIRECRUITWRITE_PERSONNUMEDIT_X, ACUIRECRUITWRITE_PERSONNUMEDIT_Y, 
		ACUIRECRUITWRITE_PERSONNUMEDIT_WIDTH, ACUIRECRUITWRITE_PERSONNUMEDIT_HEIGHT );
	AddChild( &m_clPersonNum, ACUIRECRUITWRITE_CONTROLID_PERSONNUM_EDIT );

	// Edit2 ( �ʿ䷹�� Edit )
	m_clLevelMin.MoveWindow( ACUIRECRUITWRITE_LEVELMIN_X, ACUIRECRUITWRITE_LEVELMIN_Y, 
		ACUIRECRUITWRITE_LEVELEDIT_WIDTH, ACUIRECRUITWRITE_LEVELEDIT_HEIGHT );
	AddChild( &m_clLevelMin, ACUIRECRUITWRITE_CONTROLID_LEVEL_MIN );
	m_clLevelMax.MoveWindow( ACUIRECRUITWRITE_LEVELMAX_X, ACUIRECRUITWRITE_LEVELMAX_Y, 
		ACUIRECRUITWRITE_LEVELEDIT_WIDTH, ACUIRECRUITWRITE_LEVELEDIT_HEIGHT );
	AddChild( &m_clLevelMax, ACUIRECRUITWRITE_CONTROLID_LEVEL_MAX );

	// Class ���� Push��ư 
	m_clClassPush[0].MoveWindow( ACUIRECRUITWRITE_CLASSPUSH_X, 
			ACUIRECRUITWRITE_CLASSPUSH_Y, 
			ACUIRECRUITWRITE_CLASSPUSH_WIDTH, ACUIRECRUITWRITE_CLASSPUSH_HEIGHT );

	m_clClassPush[1].MoveWindow( ACUIRECRUITWRITE_CLASSPUSH_X + ACUIRECRUITWRITE_CLASSPUSH_X_GAP, 
			ACUIRECRUITWRITE_CLASSPUSH_Y, 
			ACUIRECRUITWRITE_CLASSPUSH_WIDTH, ACUIRECRUITWRITE_CLASSPUSH_HEIGHT );

	m_clClassPush[2].MoveWindow( ACUIRECRUITWRITE_CLASSPUSH_X, 
			ACUIRECRUITWRITE_CLASSPUSH_Y + ACUIRECRUITWRITE_CLASSPUSH_Y_GAP, 
			ACUIRECRUITWRITE_CLASSPUSH_WIDTH, ACUIRECRUITWRITE_CLASSPUSH_HEIGHT );

	m_clClassPush[3].MoveWindow( ACUIRECRUITWRITE_CLASSPUSH_X + ACUIRECRUITWRITE_CLASSPUSH_X_GAP, 
			ACUIRECRUITWRITE_CLASSPUSH_Y + ACUIRECRUITWRITE_CLASSPUSH_Y_GAP, 
			ACUIRECRUITWRITE_CLASSPUSH_WIDTH, ACUIRECRUITWRITE_CLASSPUSH_HEIGHT );
			
	for ( int i = 0 ; i < ACUIRECRUITWRITE_CLASS_NUM ; ++i )
		AddChild( &m_clClassPush[i] );

	// Write Button
	m_clWrite.MoveWindow( ACUIRECRUITWRITE_WRITE_BUTTON_X, ACUIRECRUITWRITE_WRITE_BUTTON_Y, 
		ACUIRECRUITWRITE_BUTTON_WIDTH, ACUIRECRUITWRITE_BUTTON_HEIGHT );
	AddChild( &m_clWrite, ACUIRECRUITWRITE_CONTROLID_WRITE );
	
	// Cancel Button
	m_clCancel.MoveWindow( ACUIRECRUITWRITE_CANCEL_BUTTON_X, ACUIRECRUITWRITE_CANCEL_BUTTON_Y, 
		ACUIRECRUITWRITE_BUTTON_WIDTH, ACUIRECRUITWRITE_BUTTON_HEIGHT );
	AddChild( &m_clCancel, ACUIRECRUITWRITE_CONTROLID_CANCEL );

	// Modify Button
	m_clModify.MoveWindow( ACUIRECRUITWRITE_MODIFY_BUTTON_X, ACUIRECRUITWRITE_MODIFY_BUTTON_Y, 
		ACUIRECRUITWRITE_BUTTON_WIDTH, ACUIRECRUITWRITE_BUTTON_HEIGHT );
	AddChild( &m_clModify, ACUIRECRUITWRITE_CONTROLID_MODIFY );

	// Delete Button
	m_clDelete.MoveWindow( ACUIRECRUITWRITE_DELETE_BUTTON_X, ACUIRECRUITWRITE_DELETE_BUTTON_Y, 
		ACUIRECRUITWRITE_BUTTON_WIDTH, ACUIRECRUITWRITE_BUTTON_HEIGHT );
	AddChild( &m_clDelete, ACUIRECRUITWRITE_CONTROLID_DELETE );

	return TRUE;
}

BOOL AcUIRecruitWrite::OnLButtonDown	( RsMouseStatus* ms )
{
	return TRUE;
}

void	AcUIRecruitWrite::SetNextEdit()
{
	/*
	if ( m_clPurpose.m_bActive )
	{
		m_clPurpose.m_bActive = FALSE;
		m_clPersonNum.m_bActive = TRUE;
	}
	else if ( m_clPersonNum.m_bActive )
	{	
		m_clPersonNum.m_bActive = FALSE;
		m_clLevelMin.m_bActive = TRUE;
	}
	else if( m_clLevelMin.m_bActive )
	{
		m_clLevelMin.m_bActive = FALSE;
		m_clLevelMax.m_bActive = TRUE;
	}
	else if ( m_clLevelMax.m_bActive )
	{
		m_clLevelMax.m_bActive = FALSE;
		m_clPurpose.m_bActive = TRUE;
	}
	*/
}

BOOL	AcUIRecruitWrite::OnCommand		( INT32	nID , PVOID pParam	)	
{
	INT32* pUIID = (INT32*)pParam;
	UINT8 i = 0 ;

	// Control �߽��� Command Message
	switch( *pUIID )
	{
		case ACUIRECRUITWRITE_CONTROLID_WRITE:		// ��� ��ư 
			if ( UICM_BUTTON_MESSAGE_CLICK == nID )
			{
				OnClickWriteButton();
				EndDialog( 0 );
				return TRUE;
			}
			break;

		case ACUIRECRUITWRITE_CONTROLID_CANCEL:		// ��� ��ư 
			if ( UICM_BUTTON_MESSAGE_CLICK == nID )
			{
				EndDialog( 0 );
				return TRUE;
			}
			break;

		case ACUIRECRUITWRITE_CONTROLID_MODIFY:		// ���� ��ư 
			if ( UICM_BUTTON_MESSAGE_CLICK == nID )
			{
				EndDialog( 0 );
				return TRUE;
			}
			break;

		case ACUIRECRUITWRITE_CONTROLID_DELETE:		// ���� ��ư 
			if ( UICM_BUTTON_MESSAGE_CLICK == nID )
			{
				m_pAgcmRecruit->UIClickDeleteLFM();
				EndDialog( 0 );
				return TRUE;
			}
			break;
			
		case ACUIRECRUITWRITE_CONTROLID_PURPOSE_EDIT:
		case ACUIRECRUITWRITE_CONTROLID_PERSONNUM_EDIT:
		case ACUIRECRUITWRITE_CONTROLID_LEVEL_MIN:
		case ACUIRECRUITWRITE_CONTROLID_LEVEL_MAX:

			if ( UICM_EDIT_INPUT_TAB == nID )
			{
				SetNextEdit();
			}
			return TRUE;

			break;
	}
	
	return TRUE;
}

void	AcUIRecruitWrite::SetModulePointer( AgcmRecruit* pAgcmRecruit )
{
	m_pAgcmRecruit = pAgcmRecruit;
}

void	AcUIRecruitWrite::OnClickWriteButton()
{
	// Valid Check �Ӵ� �ǰ� �Ӵ� �ȵȴ� ��Ÿ���

	if ( strlen( m_clPersonNum.GetText() ) <= 0 ) return;
	else if ( strlen( m_clLevelMin.GetText() ) <= 0 ) return;
	else if ( strlen( m_clLevelMax.GetText() ) <= 0 ) return;
	
	INT32 lPersonNum = atoi( m_clPersonNum.GetText() );
	INT32 lLevelMax = atoi( m_clLevelMax.GetText() );
	INT32 lLevelMin = atoi( m_clLevelMin.GetText() );
	INT32 lClass	= 0x00000000;

	if ( lLevelMin > lLevelMax )
	{
		INT32 lTemp = lLevelMax;
		lLevelMax = lLevelMin;
		lLevelMin = lTemp;
	}

	// Class Setting
	if ( 2 == m_clClassPush[0].GetButtonMode() )					// ������ 
		lClass = lClass | AGPMRECRUIT_CLASS_MAGE;

	if ( 2 == m_clClassPush[1].GetButtonMode() )// ����
		lClass = lClass | AGPMRECRUIT_CLASS_FIGHTER;	
	
	if ( 2 == m_clClassPush[2].GetButtonMode() )					// �ü�
		lClass = lClass | AGPMRECRUIT_CLASS_RANGER;
	
	if ( 2 == m_clClassPush[3].GetButtonMode() )					// ������
		lClass = lClass | AGPMRECRUIT_CLASS_MONK;
	
	// ������
	m_pAgcmRecruit->UIClickRegisterLFM( (CHAR *) m_clPurpose.GetText(), lPersonNum, lLevelMin, lLevelMax, lClass );

	return;
}
