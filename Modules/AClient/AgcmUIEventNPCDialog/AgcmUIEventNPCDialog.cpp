#include <AgcmUIEventNPCDialog/AgcmUIEventNPCDialog.h>
#include <AgcmTargeting/AgcmTargeting.h>
#include <AgcmSound/AgcmSound.h>
#include <AgpmEventNPCDialog/AgpmEventNPCDialog.h>
#include <AgcmEventNPCDialog/AgcmEventNPCDialog.h>
#include <AuStrTable/AuStrTable.h>
#include <AgpmCharacter/AgpmCharacter.h>
#include <AgcmCharacter/AgcmCharacter.h>
#include <AgcmUIManager2/AgcmUIManager2.h>
#include <ApmEventManager/ApmEventManager.h>
#include <AgcmEventManager/AgcmEventManager.h>
#include <AgcModule/AgcEngine.h>

void AgcmUIEventNPCDialogButton::Clear()
{
	for( int i=0; i<AGCMUI_EVENT_MAX_NPCDIALOG; ++i )
	{
		m_csButton[i].Reset();
	}
}

INT32 AgcmUIEventNPCDialogButton::SetData(ApdEventFunction eFunctionType, char *pstrDisplay, bool bExit )
{
	INT32			lIndex;

	lIndex = -1;

	for( int i=0; i<AGCMUI_EVENT_MAX_NPCDIALOG; ++i )
	{
		if( m_csButton[i].m_eFunctionType == APDEVENT_FUNCTION_NONE )
		{
			lIndex = i;
			break;
		}
	}

	if( lIndex != -1 )
	{
		m_csButton[lIndex].m_eFunctionType = eFunctionType;
		strcpy( m_csButton[lIndex].m_strDisplayString, pstrDisplay );
		m_csButton[lIndex].m_bExit = bExit;
	}

	return lIndex;
}

AgcdUIEventNPCDialogButtonInfo *AgcmUIEventNPCDialogButton::GetData( INT32 lIndex )
{
	return &m_csButton[lIndex];
}

AgcmUIEventNPCDialog::AgcmUIEventNPCDialog()
{
	SetModuleName("AgcmUIEventDialog");

	m_pcsApBase				= NULL;
	m_pcsGenerator			= NULL;

	m_pcsAgpmCharacter		= NULL;
	m_pcsAgpmItem			= NULL;

	m_pcsAgcmCharacter		= NULL;
	m_pcsAgcmItem			= NULL;
	m_pcsAgcmEventNPCDialog		= NULL;
	m_pcsAgcmUIManager2		= NULL;

	m_lEventOpenNPCDialogUI = 0;
	m_lEventCloseNPCDialogUI = 0;

	ZeroMemory(m_apcsDialog, sizeof(AgcdUIEventNPCDialogButtonInfo *) * AGCMUI_EVENT_MAX_NPCDIALOG);

	ZeroMemory(&m_stEventPos, sizeof(AuPOS));

	m_bIsNPCDialogUIOpen	= FALSE;
	m_bNoNeedCloseSound		= FALSE;

	m_lLastOpenDialogNPCID	= AP_INVALID_CID;
}

AgcmUIEventNPCDialog::~AgcmUIEventNPCDialog()
{
}

BOOL AgcmUIEventNPCDialog::OnAddModule()
{
	m_pcsApmEventManager	= (ApmEventManager *)	GetModule("ApmEventManager");
	m_pcsAgpmCharacter		= (AgpmCharacter *)		GetModule("AgpmCharacter");
	m_pcsAgpmItem			= (AgpmItem *)			GetModule("AgpmItem");
	m_pcsAgpmEventNPCDialog = (AgpmEventNPCDialog *)GetModule("AgpmEventNPCDialog" );
	m_pcsAgcmCharacter		= (AgcmCharacter *)		GetModule("AgcmCharacter");
	m_pcsAgcmItem			= (AgcmItem *)			GetModule("AgcmItem");
	m_pcsAgcmEventManager	= (AgcmEventManager *)  GetModule("AgcmEventManager");
	m_pcsAgcmEventNPCDialog		= (AgcmEventNPCDialog *)		GetModule("AgcmEventNPCDialog");
	m_pcsAgcmUIManager2		= (AgcmUIManager2 *)	GetModule("AgcmUIManager2");

	if (!m_pcsAgpmCharacter ||
		!m_pcsAgpmItem ||
		!m_pcsAgpmEventNPCDialog ||
		!m_pcsAgcmCharacter ||
		!m_pcsAgcmItem ||
		!m_pcsAgcmEventManager ||
		!m_pcsAgcmEventNPCDialog ||
		!m_pcsAgcmUIManager2)
		return FALSE;

	if (!m_pcsAgcmEventNPCDialog->SetCallbackAriseNPCDialogEvent(CBAriseNPCDialogUI, this))
		return FALSE;

	if (!m_pcsAgcmCharacter->SetCallbackSelfUpdatePosition(CBSelfUpdatePosition, this))
		return FALSE;

	if (!AddEvent())
		return FALSE;
	if (!AddFunction())
		return FALSE;
	if (!AddDisplay())
		return FALSE;
	if (!AddUserData())
		return FALSE;

	return TRUE;
}

BOOL AgcmUIEventNPCDialog::OnDestroy()
{
	OnClearNpcDialogText();
	return ApModule::OnDestroy();
}

BOOL AgcmUIEventNPCDialog::AddEvent()
{
	m_lEventOpenNPCDialogUI = m_pcsAgcmUIManager2->AddEvent("NPCDialog_OpenUI");
	if (m_lEventOpenNPCDialogUI < 0)
		return FALSE;

	m_lEventCloseNPCDialogUI = m_pcsAgcmUIManager2->AddEvent("NPCDialog_CloseUI");
	if (m_lEventCloseNPCDialogUI < 0)
		return FALSE;

	return TRUE;
}

BOOL AgcmUIEventNPCDialog::AddFunction()
{
	if (!m_pcsAgcmUIManager2->AddFunction(this, "NPCDialog_DisplayDialog", CBNPCDialogDisplayDialog, 1 ))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "NPCDialog_SelectButton", CBNPCDailogSelectButton, 1))
		return FALSE;

	return TRUE;
}

BOOL AgcmUIEventNPCDialog::AddDisplay()
{
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "NPCDialog_Button", 0, CBDisplayNPCDialog, AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;

	return TRUE;
}

BOOL AgcmUIEventNPCDialog::AddUserData()
{
	m_pstDialogUD = m_pcsAgcmUIManager2->AddUserData("NPCDialog_Buttons", m_apcsDialog, sizeof(AgcdUIEventNPCDialogButtonInfo *), AGCMUI_EVENT_MAX_NPCDIALOG, AGCDUI_USERDATA_TYPE_STRING );

	return TRUE;
}

BOOL AgcmUIEventNPCDialog::CBAriseNPCDialogUI(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIEventNPCDialog *pThis	= (AgcmUIEventNPCDialog *)	pClass;
	ApdEvent			*pcsEvent = (ApdEvent *)pData;
	ApBase				*pcsGenerator = (ApBase *)pCustData;

	// ������ (2005-05-27 ���� 11:40:29) : 
	// ����� ������� �ڵ�..
	pThis->m_bNoNeedCloseSound = FALSE;

	//pThis�� �翬�� �ְ���?
	if( pThis && pcsEvent )
	{
		ApdEventAttachData	*pcsAttachData = pThis->m_pcsApmEventManager->GetEventData(pcsEvent->m_pcsSource);

		if (pcsAttachData)
		{
			INT32			lIndex;
			INT32			lDisplayButtonCount;

			lDisplayButtonCount = 0;
			pThis->m_pcsApBase = pcsEvent->m_pcsSource;
			pThis->m_pcsGenerator = pcsGenerator;
			pThis->m_csDialogButtons.Clear();

			//�ش� ������Ʈ�� �������ִ� �̺�Ʈ�� �޿�~ ����.
			for (int i = 0; i < pcsAttachData->m_unFunction; ++i)
			{
				//���� �̺�Ʈ�� ������ �ִ°��.
				if (pcsAttachData->m_astEvent[i].m_eFunction > APDEVENT_FUNCTION_NONE && pcsAttachData->m_astEvent[i].m_eFunction < APDEVENT_MAX_FUNCTION)
				{
					//NPC��� �̺�Ʈ�� ����.
					//					if( pcsAttachData->m_astEvent[i].m_eFunction != APDEVENT_FUNCTION_NPCDAILOG &&
					//						pcsAttachData->m_astEvent[i].m_eFunction != APDEVENT_FUNCTION_AUCTION)
					if( pcsAttachData->m_astEvent[i].m_eFunction != APDEVENT_FUNCTION_NPCDAILOG )
					{
						char			strDisplayData[80];

						sprintf( strDisplayData, "%s", ClientStr().GetStr(STI_CANT_USE) );

						if( pcsAttachData->m_astEvent[i].m_eFunction == APDEVENT_FUNCTION_NPCTRADE )
						{
							sprintf( strDisplayData, "%s", ClientStr().GetStr(STI_BUY_SELL) );
						}
						else if( pcsAttachData->m_astEvent[i].m_eFunction == APDEVENT_FUNCTION_ITEM_REPAIR )
						{
							sprintf( strDisplayData, "%s", ClientStr().GetStr(STI_REPAIR) );
						}
						else if( pcsAttachData->m_astEvent[i].m_eFunction == APDEVENT_FUNCTION_BANK )
						{
							sprintf( strDisplayData, "%s", ClientStr().GetStr(STI_WAREHOUSE) );
						}
						else if( pcsAttachData->m_astEvent[i].m_eFunction == APDEVENT_FUNCTION_ITEMCONVERT )
						{
							sprintf( strDisplayData, "%s", ClientStr().GetStr(STI_ENHANCE) );
						}
						else if( pcsAttachData->m_astEvent[i].m_eFunction == APDEVENT_FUNCTION_GUILD )
						{
							sprintf( strDisplayData, "%s", ClientStr().GetStr(STI_GUILD) );
						}
						else if( pcsAttachData->m_astEvent[i].m_eFunction == APDEVENT_FUNCTION_PRODUCT )
						{
							sprintf( strDisplayData, "%s", ClientStr().GetStr(STI_PRODUCE) );
						}
						else if( pcsAttachData->m_astEvent[i].m_eFunction == APDEVENT_FUNCTION_TELEPORT )
						{
							sprintf( strDisplayData, "%s", ClientStr().GetStr(STI_TELEPORT) );
						}
						else if( pcsAttachData->m_astEvent[i].m_eFunction == APDEVENT_FUNCTION_QUEST )
						{
							sprintf( strDisplayData, "%s", ClientStr().GetStr(STI_QUEST) );
						}						
						else if( pcsAttachData->m_astEvent[i].m_eFunction == APDEVENT_FUNCTION_SKILLMASTER )
						{
							sprintf( strDisplayData, "%s", ClientStr().GetStr(STI_SKILL) );
						}
						else if( pcsAttachData->m_astEvent[i].m_eFunction == APDEVENT_FUNCTION_AUCTION )
						{
							sprintf( strDisplayData, "%s", ClientStr().GetStr(STI_AUCTION) );
						}
						else if( pcsAttachData->m_astEvent[i].m_eFunction == APDEVENT_FUNCTION_REFINERY )
						{
							sprintf( strDisplayData, "%s", ClientStr().GetStr(STI_REFINE) );
						}
						else if( pcsAttachData->m_astEvent[i].m_eFunction == APDEVENT_FUNCTION_CHAR_CUSTOMIZE )
						{
							sprintf( strDisplayData, "%s", ClientStr().GetStr(STI_CUSTOMIZE) );
						}
						else if( pcsAttachData->m_astEvent[i].m_eFunction == APDEVENT_FUNCTION_REMISSION )
						{
							sprintf( strDisplayData, "%s", ClientStr().GetStr(STI_FORGIVE) );
						}
						else if( pcsAttachData->m_astEvent[i].m_eFunction == APDEVENT_FUNCTION_WANTEDCRIMINAL )
						{
							sprintf( strDisplayData, "%s",  ClientStr().GetStr(STI_WANTED));
						}
						else if( pcsAttachData->m_astEvent[i].m_eFunction == APDEVENT_FUNCTION_SIEGEWAR_NPC )
						{
							sprintf( strDisplayData, "%s", ClientStr().GetStr(STI_SIEGEWAR_NPC));
						}
						else if( pcsAttachData->m_astEvent[i].m_eFunction == APDEVENT_FUNCTION_TAX )
						{
							sprintf( strDisplayData, "%s", ClientStr().GetStr(STI_TAX));
						}
						else if( pcsAttachData->m_astEvent[i].m_eFunction == APDEVENT_FUNCTION_GUILD_WAREHOUSE )
						{
							sprintf( strDisplayData, "%s", ClientStr().GetStr(STI_GUILD_WAREHOUSE) );
						}
						else if( pcsAttachData->m_astEvent[i].m_eFunction == APDEVENT_FUNCTION_ARCHLORD )
						{
							sprintf( strDisplayData, "%s", ClientStr().GetStr(STI_EVENT_ARCHLORD) );
						}
						else if( pcsAttachData->m_astEvent[i].m_eFunction == APDEVENT_FUNCTION_GAMBLE )
						{
							sprintf( strDisplayData, "%s", ClientStr().GetStr(STI_EVENT_GAMBLE) );
						}
						else if( pcsAttachData->m_astEvent[i].m_eFunction == APDEVENT_FUNCTION_WORLD_CHAMPIONSHIP )
						{
							sprintf( strDisplayData, "%s", ClientStr().GetStr(STI_EVENT_WORLD_CHAMPIONSHIP) );
						}

						//NPC��� �̺�Ʈ���� �̺�Ʈ���� �����ش�.
						lIndex = pThis->m_csDialogButtons.SetData( pcsAttachData->m_astEvent[i].m_eFunction, strDisplayData );

						if( lIndex != -1 )
						{
							pThis->m_apcsDialog[lIndex] = pThis->m_csDialogButtons.GetData(lIndex);
							lDisplayButtonCount++;
						}
					}
				}
			}

			//�������� Exit�� �ٿ��ش�.
			char* pTextExit = pThis->m_pcsAgcmUIManager2->GetUIMessage( "StaticText_Exit" );
			if(pTextExit)
				lIndex = pThis->m_csDialogButtons.SetData( APDEVENT_MAX_FUNCTION, pTextExit, true );
			else
				lIndex = pThis->m_csDialogButtons.SetData( APDEVENT_MAX_FUNCTION, "Exit", true );
			if( lIndex != -1 )
			{
				pThis->m_apcsDialog[lIndex] = pThis->m_csDialogButtons.GetData(lIndex);
				++lDisplayButtonCount;
			}

			pThis->m_pstDialogUD->m_stUserData.m_lCount = lDisplayButtonCount;

			pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pstDialogUD);

			pThis->m_lNPCDailogTextID = ((AgpdEventNPCDialogData *)(pcsEvent->m_pvData))->m_lNPCDialogTextID;

			pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventOpenNPCDialogUI , 0 , FALSE , pcsEvent->m_pcsSource );

			if (pcsGenerator)
				pThis->m_stEventPos	= ((AgpdCharacter *) pcsGenerator)->m_stPos;

			// ������ (2005-05-17 ���� 7:07:33) : 
			// ���� ���
			if (!pThis->m_bIsNPCDialogUIOpen)
				pThis->PlayNPCSound( NS_WELCOME , pcsEvent->m_pcsSource );

			pThis->m_bIsNPCDialogUIOpen	= TRUE;

			pThis->m_lLastOpenDialogNPCID	= pThis->m_pcsApBase->m_lID;
		}
	}

	return TRUE;
}

void	AgcmUIEventNPCDialog::PlayNPCSound( NS_TYPE eType , ApBase * pcsOwnerBase )
{
	if ( g_eServiceArea == AP_SERVICE_AREA_CHINA || g_eServiceArea == AP_SERVICE_AREA_JAPAN )
		return;		// �߱�/�Ϻ��� NPC ������ �ȵ���ش�

	AgcmTargeting* pcsAgcmTargeting = ( AgcmTargeting* ) GetModule( "AgcmTargeting"	);
	AgcmSound* pcsAgcmSound = ( AgcmSound* ) GetModule( "AgcmSound" );
	ASSERT( pcsAgcmTargeting	);
	ASSERT( pcsAgcmSound		);
	if( !pcsAgcmTargeting	) return;
	if( !pcsAgcmSound		) return;

	AgpdCharacter* pcsCharacter = m_pcsAgpmCharacter->GetCharacter( pcsOwnerBase ? pcsOwnerBase->m_lID : ( eType == NS_FAREWELL ? m_lLastOpenDialogNPCID : pcsAgcmTargeting->GetLastNPCID() ) );
	if( !pcsCharacter )		return;

	// ������ �÷����������� ������
	static	INT32	_snCID		= -1;
	static	INT32	_snType		= -1;
	static	UINT32	_suEndtime	= GetTickCount();
	if( pcsCharacter->m_pcsCharacterTemplate->m_lID	== _snCID && _snType == eType && _suEndtime > GetTickCount() )
		return;

	char	strFileName[ 1024 ];
	if( pcsCharacter->m_pcsCharacterTemplate->m_lID < 1000 )	//4�ڸ��� �̸�
	{
		sprintf( strFileName, "Sound\\Effect\\NP%03d%d.wav", pcsCharacter->m_pcsCharacterTemplate->m_lID, (int)eType );
	}
	else
	{
		char szID[16];
		itoa( pcsCharacter->m_pcsCharacterTemplate->m_lID, szID, 10 );
		sprintf( strFileName, "Sound\\Effect\\NP%s%d.wav", szID, (int)eType );
	}

	pcsAgcmSound->PlaySampleSound( strFileName );

	_snCID		= pcsCharacter->m_pcsCharacterTemplate->m_lID;
	_snType		= eType;
	_suEndtime	= GetTickCount() + pcsAgcmSound->GetSampleSoundLength( SOUND_TYPE_3D_SOUND, strFileName );
}


BOOL AgcmUIEventNPCDialog::CBNPCDialogDisplayDialog(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIEventNPCDialog *pThis = (AgcmUIEventNPCDialog *) pClass;
	AgcdUIControl *pcsControl = (AgcdUIControl *) pData1;
	BOOL				bResult;

	bResult = FALSE;

	if( pcsControl != NULL )
	{
		AgcmCharacter* pcmCharacter = ( AgcmCharacter* )g_pEngine->GetModule( "AgcmCharacter" );
		if( !pcmCharacter ) return FALSE;

		AgpdCharacter* ppdSelfCharacter = pcmCharacter->GetSelfCharacter();
		if( !ppdSelfCharacter ) return FALSE;

		char* pText = pThis->GetNpcDialogText( pThis->m_lNPCDailogTextID, ppdSelfCharacter );
		if( pText )
		{
			((AcUIEdit *)pcsControl->m_pcsBase)->SetLineDelimiter( "\\n" );
			bResult = ((AcUIEdit *)pcsControl->m_pcsBase)->SetText( pText );
		}
	}

	return bResult;
}

BOOL AgcmUIEventNPCDialog::CBDisplayNPCDialog(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pData || !pClass || eType != AGCDUI_USERDATA_TYPE_STRING)
		return FALSE;
	AgcdUIEventNPCDialogButtonInfo		**ppcsButton;

	ppcsButton = (AgcdUIEventNPCDialogButtonInfo **)pData;

	if( (*ppcsButton) == NULL )
		return FALSE;

	AgcmUIEventNPCDialog	*pThis	= (AgcmUIEventNPCDialog *) pClass;

	sprintf(szDisplay, "%s", (*ppcsButton)->m_strDisplayString );

	return TRUE;
}

BOOL AgcmUIEventNPCDialog::CBNPCDailogSelectButton(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIEventNPCDialog	*pThis			= (AgcmUIEventNPCDialog *)	pClass;

	AcUIButton				*pcsUIButton	= (AcUIButton *)			pcsSourceControl->m_pcsBase;
	if (!pcsUIButton)
		return FALSE;

	AgcdUIEventNPCDialogButtonInfo	*pcsButtonInfo;

	pcsButtonInfo = pThis->m_csDialogButtons.GetData(pcsSourceControl->m_lUserDataIndex);

	if( pcsButtonInfo )
	{
		//���� ���� ��ư�̸�~
		if( pcsButtonInfo->m_bExit )
		{
			pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventCloseNPCDialogUI );

			pThis->m_bIsNPCDialogUIOpen	= FALSE;
		}
		else
		{
			// ������ (2005-05-27 ���� 11:40:29) : 
			// ����� ������� �ڵ�..
			pThis->m_bNoNeedCloseSound = TRUE;

			//����� ��ȭâ�� ������ �ٸ� UI�� ������.
			//���� Function ID�� ���� ��ȭ UI�� �������� �������� �ִ�.
			pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventCloseNPCDialogUI );

			pThis->m_bIsNPCDialogUIOpen	= FALSE;

			pThis->m_pcsAgcmEventManager->CheckEvent( pThis->m_pcsApBase, pThis->m_pcsGenerator, pcsButtonInfo->m_eFunctionType );
		}
	}

	return TRUE;
}

BOOL AgcmUIEventNPCDialog::CBSelfUpdatePosition(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIEventNPCDialog	*pThis				= (AgcmUIEventNPCDialog *)		pClass;
	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)				pData;

	if (!pThis->m_bIsNPCDialogUIOpen)
		return TRUE;

	FLOAT	fDistance = AUPOS_DISTANCE_XZ(pcsCharacter->m_stPos, pThis->m_stEventPos);

	if ((INT32) fDistance < AGCMUIEVENTNPCDIALOG_CLOSE_UI_DISTANCE)
		return TRUE;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseNPCDialogUI);

	pThis->m_bIsNPCDialogUIOpen	= FALSE;

	return TRUE;
}

void AgcmUIEventNPCDialog::CloseNPCDialog()
{
	if (!m_pcsApBase || !m_bIsNPCDialogUIOpen)
		return;

	PlayNPCSound( NS_FAREWELL , m_pcsApBase );
	m_bIsNPCDialogUIOpen	= FALSE;
}
