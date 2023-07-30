#include <AgcmUIPartyOption/AgcmUIPartyOption.h>
#include <AgcmUIItem/AgcmUIItem.h>
#include <AuStrTable/AuStrTable.h>
#include <AgpmParty/AgpmParty.h>
#include <AgcmParty/AgcmParty.h>
#include <AgpmCharacter/AgpmCharacter.h>
#include <AgcmCharacter/AgcmCharacter.h>
#include <AgcmUIManager2/AgcmUIManager2.h>
#include <AgcmChatting2/AgcmChatting2.h>
#include <AgpmItem/AgpmItem.h>
#include <AgcmUIControl/AcUIEdit.h>
#include <AgcmUIControl/AcUICombo.h>

/*
	�⺻ ������
	����� �̸��� �����ϰ� ��⿡�� ���Ǵ� �������� �ʱ�ȭ�Ѵ�.
	���Ǵ� Packet Ÿ���� �����Ѵ�.
	���Ǵ� ���� ����� �����͸� NULL�� �ʱ�ȭ �Ѵ�.
*/
AgcmUIPartyOption::AgcmUIPartyOption()
{

	SetModuleName("AgcmUIPartyOption");
	SetPacketType(AGPMPARTYITEM_PACKET_TYPE);

	m_aszDivisionItem[AGCMUI_PARTYOPTION_DIVISION_ITEM_SEQUENCE]	=	UI_MESSAGE_PARTYOPTION_DIVISION_ITEM_SEQUENCE;
	m_aszDivisionItem[AGCMUI_PARTYOPTION_DIVISION_ITEM_FREE]		=	UI_MESSAGE_PARTYOPTION_DIVISION_ITEM_FREE;
	m_aszDivisionItem[AGCMUI_PARTYOPTION_DIVISION_ITEM_DAMAGE]		=	UI_MESSAGE_PARTYOPTION_DIVISION_ITEM_DAMAGE;

	m_aszDivisionExp[AGCMUI_PARTYOPTION_DIVISION_EXP_DAMAGE]		=	UI_MESSAGE_PARTYOPTION_DIVISION_EXP_DAMAGE;
	m_aszDivisionExp[AGCMUI_PARTYOPTION_DIVISION_EXP_LEVEL]			=	UI_MESSAGE_PARTYOPTION_DIVISION_EXP_LEVEL;

	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
							AUTYPE_INT32,          1,				// Item Template ID
							AUTYPE_INT32,		   1,				// Item Owner CID
							AUTYPE_INT32,		   1,				// Item Count
							AUTYPE_INT8,		   1,				// # of physical convert
							AUTYPE_INT8,		   1,				// # of socket
							AUTYPE_INT8,		   1,				// # of converted socket
							AUTYPE_INT8,		   1,				// option Count
		                    AUTYPE_END,            0
							);
	ZeroMemory(&m_stMessageData, sizeof(AgcmUIPartyOptionGetItemPartyMemberMessageData));

	m_pcsAgpmFactors		=		NULL;
	m_pcsAgpmCharacter		=		NULL;
	m_pcsAgcmCharacter		=		NULL;
	m_pcsAgpmParty			=		NULL;
	m_pcsAgcmParty			=		NULL;
	m_pcsAgcmUIManager2		=		NULL;
	m_pcsAgcmChatting		=		NULL;
	m_pcsAgpmItem			=		NULL;

	m_pcsUserDivisionItem	=		NULL;
	m_pcsUserDivisionExp	=		NULL;
	m_pcsMessageData		=		NULL;

	m_lEventPartyOptionItemGetMemeber		=	0;
	m_lEventPartyOptionRecvNoLoginMember	=	0;
	m_lEventPartyOptionDivisionItemUpdate	=	0;
	m_lEventPartyOptionUIOpen				=	0;
	m_lEventPartyOptionUIOpenClick			=	0;
	m_lEventPartyOptionExpItemUpdate		=	0;
	m_bEnableOptionForPartyLeader			=	FALSE;
	m_pszNoLoginMemberID					=	NULL;
	m_bEnableOptionInviteMember				=	TRUE;

	memset(m_szDefaultID, 0, AGPACHARACTER_MAX_ID_STRING+1);
}

AgcmUIPartyOption::~AgcmUIPartyOption()
{
}

/*
	���� ��⿡�� ���Ǵ� ���� ����� �����ϱ� ���� ����Ѵ�.
	�� ��⿡�� Call-Back���� �Ҹ����� static �迭�� �Լ��� ����Ѵ�.
	UI���� ó���� Event�� ����Ѵ�.
	UI������ ���� ��ϵ� �Լ��� ����Ѵ�.
	UI ��¿� �ʿ��� �Լ��� ����Ѵ�.
	���뵥��Ÿ�� ����Ѵ�.
*/
BOOL AgcmUIPartyOption::OnAddModule()
{
	m_pcsAgpmFactors		= (AgpmFactors *)		GetModule("AgpmFactors");
	m_pcsAgpmCharacter		= (AgpmCharacter *)		GetModule("AgpmCharacter");
	m_pcsAgcmCharacter		= (AgcmCharacter *)		GetModule("AgcmCharacter");
	m_pcsAgpmItem			= (AgpmItem*)			GetModule("AgpmItem");
	m_pcsAgpmParty			= (AgpmParty *)			GetModule("AgpmParty");
	m_pcsAgcmParty			= (AgcmParty *)			GetModule("AgcmParty");
	m_pcsAgcmUIManager2		= (AgcmUIManager2 *)	GetModule("AgcmUIManager2");
	m_pcsAgcmChatting		= (AgcmChatting2 *)		GetModule("AgcmChatting2");
	
	if (!m_pcsAgpmFactors	||
		!m_pcsAgpmCharacter ||
		!m_pcsAgcmCharacter ||
		!m_pcsAgpmItem	||
		!m_pcsAgpmParty ||
		!m_pcsAgcmParty ||
		!m_pcsAgcmUIManager2 ||
		!m_pcsAgcmChatting)
		return FALSE;

	if (!m_pcsAgpmParty->SetCBUpdateExpType(CBUpdateExpDivision, this))				//	2005.05.17 By SungHoon
		return FALSE;

	if (!m_pcsAgpmParty->SetCBUpdateItemDivision(CBUpdateItemDivision, this))		// 2005.04.19 By SungHoon
		return FALSE;

	if (!m_pcsAgpmParty->SetCBAddParty(CBAddParty, this))							//	2005.05.10	By SungHoon
		return FALSE;
	if (!m_pcsAgpmParty->SetCBRemoveParty(CBRemoveParty, this))							//	2005.05.10	By SungHoon
		return FALSE;
	if (!m_pcsAgpmParty->SetCBChangeLeader(CBUpdateParty, this))
		return FALSE;

	if (!m_pcsAgcmParty->SetCallbackReceiveInviteFailedNoLoginMember(CBReceiveInviteFailedNoLoginMember, this))		// 2005.04.21 By SungHoon
		return FALSE;

	if (!AddEvent())
		return FALSE;

	if (!AddFunction())
		return FALSE;

	if (!AddDisplay())
		return FALSE;

	if (!AddUserData())
		return FALSE;

	if (!AddBoolean())
		return FALSE;

	return TRUE;
}

/*
	�ʱ�ȭ ���� �۾��� �Ѵ�.
*/
BOOL AgcmUIPartyOption::OnInit()
{
	AgpdCharacter			*pcsSelfCharacter	= m_pcsAgcmCharacter->GetSelfCharacter();
	AgpdParty				*pcsSelfParty		= NULL;
	if (pcsSelfCharacter)	 pcsSelfParty		= m_pcsAgpmParty->GetParty(pcsSelfCharacter);

	m_pcsAgcmUIItem		=	static_cast< AgcmUIItem* >(GetModule( "AgcmUIItem" ));

	SetPartyOption(pcsSelfParty);
	return TRUE;
}

/*
	���� ���� �۾��� �Ѵ�.
*/
BOOL AgcmUIPartyOption::OnDestroy()
{
	return TRUE;
}

/*
	��� ���뵥���͸� ����Ѵ�.
*/
BOOL AgcmUIPartyOption::AddUserData()
{
	m_pcsUserDivisionItem		= m_pcsAgcmUIManager2->AddUserData(AGCMUI_PARTYOPTION_DIVISION_ITEM,
																	m_aszDivisionItem,
																   sizeof(CHAR *),
																   AGCMUI_MAX_PARTYOPTION_DIVISION_ITEM,
																   AGCDUI_USERDATA_TYPE_STRING);
	if (!m_pcsUserDivisionItem)
		return FALSE;

	m_pcsUserDivisionExp		= m_pcsAgcmUIManager2->AddUserData(AGCMUI_PARTYOPTION_DIVISION_EXP,
																	m_aszDivisionExp,
																   sizeof(CHAR *),
																   AGCMUI_MAX_PARTYOPTION_DIVISION_EXP,
																   AGCDUI_USERDATA_TYPE_STRING);
	if (!m_pcsUserDivisionExp)
		return FALSE;

	m_pcsMessageData			= m_pcsAgcmUIManager2->AddUserData(AGCMUI_PARTYOPTION_USERDATA_ITEM_GET_PARTY_MEMBER,
																	&m_stMessageData,
																	sizeof(AgcmUIPartyOptionGetItemPartyMemberMessageData),
																   1,
																   AGCDUI_USERDATA_TYPE_STRING);
	if (!m_pcsMessageData)
		return FALSE;

	m_pcsEnableDivisionItem = m_pcsAgcmUIManager2->AddUserData(AGCMUI_PARTYOPTION_USERDATA_ENABLE_DIVISION_ITEM, &m_bEnableOptionForPartyLeader, sizeof(m_bEnableOptionForPartyLeader), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if (!m_pcsEnableDivisionItem)
		return FALSE;

	m_pcsEnableDivisionExp = m_pcsAgcmUIManager2->AddUserData(AGCMUI_PARTYOPTION_USERDATA_ENABLE_DIVISION_EXP, &m_bEnableOptionForPartyLeader, sizeof(m_bEnableOptionForPartyLeader), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if (!m_pcsEnableDivisionExp)
		return FALSE;

	m_pcsEnableOptionAply = m_pcsAgcmUIManager2->AddUserData(AGCMUI_PARTYOPTION_USERDATA_ENABLE_OPTION_APLY, &m_bEnableOptionForPartyLeader, sizeof(m_bEnableOptionForPartyLeader), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if (!m_pcsEnableOptionAply)
		return FALSE;

	m_pcsEnableOptionInviteMember = m_pcsAgcmUIManager2->AddUserData(AGCMUI_PARTYOPTION_USERDATA_ENABLE_INVITE_MEMBER, &m_bEnableOptionInviteMember, sizeof(m_bEnableOptionInviteMember), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if (!m_pcsEnableOptionInviteMember)
		return FALSE;

	return TRUE;
}

/*
	UI ����(Ŭ����)�� ���� �Լ��� UITool�� ����ϱ� ���� �̸� ��� �Ѵ�.
*/
BOOL AgcmUIPartyOption::AddFunction()
{
//	Begin 2005.04.13 by SungHoon
//	Party Option CallBack Funtion Register
	if (!m_pcsAgcmUIManager2->AddFunction(this, AGCMUI_PARTYOPTION_FUNCTION_APLY, CBPartyOptionAply, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, AGCMUI_PARTYOPTION_FUNCTION_INVITE_MEMBER, CBPartyOptionInviteMember, 0))
		return FALSE;
//	Finish 2005.04.13 by SungHoon	

	if (!m_pcsAgcmUIManager2->AddFunction(this, AGCMUI_PARTYOPTION_FUNCTION_OPEN_UI, CBPartyOptionOpenUI, 0))
		return FALSE;		//	2005.05.06	By SungHoon

	if (!m_pcsAgcmUIManager2->AddFunction(this, AGCMUI_PARTYOPTION_FUNCTION_OPEN_UI_CLICK, CBPartyOptionOpenUIClick, 0))
		return FALSE;		//	2005.05.06	By SungHoon

	return TRUE;
}

/*
	UITool�� ��ϵ� �̺�Ʈ�� ���� Ư�� ������ ���� ������ �����ϴ� �Լ��� ����Ѵ�.
*/
BOOL AgcmUIPartyOption::AddDisplay()
{
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "PartyOptionItemPickupName", AGCMUI_PARTYOPTION_DISPLAY_ID_ITEM_PICKUP_NAME, CBDisplayPartyOptionGetItemMember, AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "PartyOptionPartyMemeber", AGCMUI_PARTYOPTION_DISPLAY_ID_PARTY_MEMBER, CBDisplayPartyOptionGetItemMember, AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "PartyOptionDivisionItem", AGCMUI_PARTYOPTION_DISPLAY_ID_DIVISION_ITEM_UPDATE, CBDisplayPartyOptionDivisionItemUpdate, AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "PartyOptionDivisionExp", AGCMUI_PARTYOPTION_DISPLAY_ID_DIVISION_EXP_UPDATE, CBDisplayPartyOptionDivisionItemUpdate, AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, "PartyOptionNoLoginMember", AGCMUI_PARTYOPTION_DISPLAY_ID_NO_LOGIN_MEMBER, CBDisplayPartyOptionNoLoginMember, AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;


	return TRUE;
}

/*
	UITool�� ��ϵ� �̺�Ʈ�� ������ �߰��Ѵ�.
*/
BOOL AgcmUIPartyOption::AddEvent()
{
	m_lEventPartyOptionItemGetMemeber		=	m_pcsAgcmUIManager2->AddEvent(AGCMUI_PARTYOPTION_EVENT_RECEIVE_GET_ITEM_MEMBER);
	m_lEventPartyOptionRecvNoLoginMember	=	m_pcsAgcmUIManager2->AddEvent(AGCMUI_PARTYOPTION_EVENT_RECEIVE_INVITE_NO_LOGIN_MEMBER);
	m_lEventPartyOptionDivisionItemUpdate	=	m_pcsAgcmUIManager2->AddEvent(AGCMUI_PARTYOPTION_EVENT_RECEIVE_DIVISION_ITEM_UPDATE);
	m_lEventPartyOptionExpItemUpdate		=	m_pcsAgcmUIManager2->AddEvent(AGCMUI_PARTYOPTION_EVENT_RECEIVE_DIVISION_EXP_UPDATE);
	m_lEventPartyOptionUIOpen				=	m_pcsAgcmUIManager2->AddEvent(AGCMUI_PARTYOPTION_EVENT_OPEN_UI);
	m_lEventPartyOptionUIOpenClick			=	m_pcsAgcmUIManager2->AddEvent(AGCMUI_PARTYOPTION_EVENT_OPEN_UI_CLICK);

	return TRUE;
}

BOOL AgcmUIPartyOption::AddBoolean()
{
	if (!m_pcsAgcmUIManager2->AddBoolean(this, "IsActivePO_InviteMemberID", CBIsActivePartyOptionUIInviteMemberID, AGCDUI_USERDATA_TYPE_CHARACTER))
		return FALSE;

	return TRUE;
}

/*
	UI�� ��µǴ� �ؽ�Ʈ�� UIMessage.txt ���� �ҷ��´�.
*/
void AgcmUIPartyOption::InitUIMessage()
{
	if (m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_PARTYOPTION_DIVISION_ITEM_SEQUENCE))
		m_aszDivisionItem[AGCMUI_PARTYOPTION_DIVISION_ITEM_SEQUENCE]	=	m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_PARTYOPTION_DIVISION_ITEM_SEQUENCE);
	if (m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_PARTYOPTION_DIVISION_ITEM_FREE))
		m_aszDivisionItem[AGCMUI_PARTYOPTION_DIVISION_ITEM_FREE]		=	m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_PARTYOPTION_DIVISION_ITEM_FREE);
	if (m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_PARTYOPTION_DIVISION_ITEM_DAMAGE))
		m_aszDivisionItem[AGCMUI_PARTYOPTION_DIVISION_ITEM_DAMAGE]		=	m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_PARTYOPTION_DIVISION_ITEM_DAMAGE);
	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDivisionItem);

	if (m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_PARTYOPTION_DIVISION_EXP_DAMAGE))
		m_aszDivisionExp[AGCMUI_PARTYOPTION_DIVISION_EXP_DAMAGE]		=	m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_PARTYOPTION_DIVISION_EXP_DAMAGE);
	if (m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_PARTYOPTION_DIVISION_EXP_LEVEL))
		m_aszDivisionExp[AGCMUI_PARTYOPTION_DIVISION_EXP_LEVEL]			=	m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_PARTYOPTION_DIVISION_EXP_LEVEL);
	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDivisionExp);



}

//	Begin 2005.04.13 by SungHoon
//	Party Option CallBack Funtion	Ȯ�� ��ư�� �������� �ʿ������� �Բ� ȣ��ȴ�.
//	pData1 ������ ȹ����(Combo)
//	pData2 ����ġ ȹ����(Combo)
BOOL AgcmUIPartyOption::CBPartyOptionAply(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl || !pData1 || !pData2)
		return FALSE;

	AgcmUIPartyOption				*pThis					= (AgcmUIPartyOption *)			pClass;
	AgcdUIControl	*pcsUIDivisionItemControl	= (AgcdUIControl *)	pData1;
	AgcdUIControl	*pcsUIDivisionExpControl	= (AgcdUIControl *)	pData2;

	if ((pcsUIDivisionItemControl->m_lType != AcUIBase::TYPE_COMBO) ||
		(pcsUIDivisionExpControl->m_lType != AcUIBase::TYPE_COMBO)) return FALSE;

	AgpdCharacter	*pcsSelfCharacter		= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter) return FALSE;
	AgpdParty		*pcsSelfParty			= pThis->m_pcsAgpmParty->GetParty(pcsSelfCharacter);
	if (!pcsSelfParty) return FALSE;

	AcUICombo *	pcsDivisionItemCombo = (AcUICombo *) pcsUIDivisionItemControl->m_pcsBase;
	AcUICombo *	pcsDivisionExpCombo = (AcUICombo *) pcsUIDivisionExpControl->m_pcsBase;

	AgpmPartyOptionDivisionItem eDivisionItem = AGPMPARTY_DIVISION_ITEM_DAMAGE;
	if (!strcmp(pcsDivisionItemCombo->GetSelectedString(),pThis->m_aszDivisionItem[AGCMUI_PARTYOPTION_DIVISION_ITEM_FREE]))
		eDivisionItem = AGPMPARTY_DIVISION_ITEM_FREE;
	if (!strcmp(pcsDivisionItemCombo->GetSelectedString(),pThis->m_aszDivisionItem[AGCMUI_PARTYOPTION_DIVISION_ITEM_SEQUENCE]))
		eDivisionItem = AGPMPARTY_DIVISION_ITEM_SEQUENCE;

	AgpmPartyCalcExpType eExpCalcType = AGPMPARTY_EXP_TYPE_BY_DAMAGE;
	if (!strcmp(pcsDivisionExpCombo->GetSelectedString(),pThis->m_aszDivisionExp[AGCMUI_PARTYOPTION_DIVISION_EXP_LEVEL]))
		eExpCalcType = AGPMPARTY_EXP_TYPE_BY_LEVEL;

	pThis->m_pcsAgcmParty->SendPacketUpdateItemDivision(pcsSelfParty->m_lID, pcsSelfCharacter->m_lID, eDivisionItem);
	pThis->m_pcsAgcmParty->SendPacketUpdateExpType(pcsSelfParty->m_lID, pcsSelfCharacter->m_lID, eExpCalcType);

	return ( TRUE );
}

/*
	UI�󿡼� �ʴ� ��ư�� ������ ��� ĳ���Ͱ� ������ ������ ������ �������
	�þ߹ۿ� ���� ��� ���ο� ������� ��Ƽ ����� �ʴ��Ѵ�.
*/
BOOL AgcmUIPartyOption::CBPartyOptionInviteMember(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl || !pData1 )
		return FALSE;

	AgcmUIPartyOption				*pThis					= (AgcmUIPartyOption *)			pClass;
	AgcdUIControl	*pcsUIInvitedPlayerControl				= (AgcdUIControl *)	pData1;

	if (pcsUIInvitedPlayerControl->m_lType != AcUIBase::TYPE_EDIT) return FALSE;

	AcUIEdit *	pcsUIInvitedPlayerEdit = (AcUIEdit *) pcsUIInvitedPlayerControl->m_pcsBase;

	AgpdCharacter			*pcsSelfCharacter		= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter) return FALSE;
	AgpdParty *pcsSelfParty = pThis->m_pcsAgpmParty->GetParty(pcsSelfCharacter);

	CHAR *szMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_INVITE_FAILED_COLOR);
	DWORD dwColor = 0x00FF0000;
	if(szMessage) dwColor = atoi(szMessage);

	if (pcsSelfParty && pThis->m_pcsAgpmParty->GetLeaderCID(pcsSelfParty) != pcsSelfCharacter->m_lID)
	{
		CHAR *szBuffer = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_INVITE_FAILED_NO_LEADER);
		if (szBuffer) pThis->AddSystemMessage(szBuffer, dwColor);
		return FALSE;
	}

	CHAR szTargetID[ AGPACHARACTER_MAX_ID_STRING + 1 ] = { 0 };
	strncpy( szTargetID, (CHAR*)pcsUIInvitedPlayerEdit->GetText() ,AGPACHARACTER_MAX_ID_STRING );
	if ( strlen( szTargetID ) <= 0 ) return FALSE;

	//pThis->m_pcsAgpmCharacter->MakeGameID( szTargetID, szTargetID);


	AgpdCharacter			*pcsTargetCharacter		= pThis->m_pcsAgpmCharacter->GetCharacter(szTargetID);

	pcsUIInvitedPlayerEdit->SetMeActiveEdit();
	pcsUIInvitedPlayerEdit->ClearText();

	if (pThis->m_pcsAgpmCharacter->CheckCharName(szTargetID, FALSE) == FALSE) 
	{
		CHAR *szBuffer = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_INVITE_FAILED_SELF);
		if (szBuffer) pThis->AddSystemMessage(szBuffer, dwColor);
		return FALSE;
	}
	if (pcsTargetCharacter)
	{
		if (pcsSelfCharacter->m_lID == pcsTargetCharacter->m_lID)
		{
			CHAR *szBuffer = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_INVITE_FAILED_SELF);
			if (szBuffer) pThis->AddSystemMessage(szBuffer, dwColor);
			return FALSE;
		}
		else return pThis->m_pcsAgcmParty->SendPacketInvite(pcsSelfCharacter->m_lID, pcsTargetCharacter->m_lID);
	}
	else
		return pThis->m_pcsAgcmParty->SendPacketInviteByCharID(pcsSelfCharacter->m_lID, szTargetID);

	return ( TRUE );
}

/*
	2005.04.19	Start By SungHoon
	������ �й� ��� ������ ������ ���� ���� �޾��� ��� System �޼����� �ѷ����� �����Ѵ�.
*/
BOOL AgcmUIPartyOption::CBUpdateItemDivision(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIPartyOption		*pThis				= (AgcmUIPartyOption *)	pClass;

	AgpdCharacter			*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter) return FALSE;
	AgpdParty	*pcsSelfParty					= pThis->m_pcsAgpmParty->GetParty(pcsSelfCharacter);
	if (!pcsSelfParty) return FALSE;

	pThis->m_ePartyOptionDivisionItem	= pcsSelfParty->m_eDivisionItem;
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsUserDivisionItem);
	
	pThis->SetPartyOption(pcsSelfParty);
	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventPartyOptionDivisionItemUpdate);
}

/*
	2005.05.17	Start By SungHoon
	����ġ �й� ��� ������ ������ ���� ���� �޾��� ��� System �޼����� �ѷ����� �����Ѵ�.
*/
BOOL AgcmUIPartyOption::CBUpdateExpDivision(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIPartyOption		*pThis				= (AgcmUIPartyOption *)	pClass;

	AgpdCharacter			*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter) return FALSE;
	AgpdParty	*pcsSelfParty					= pThis->m_pcsAgpmParty->GetParty(pcsSelfCharacter);
	if (!pcsSelfParty) return FALSE;

	pThis->m_ePartyCalExpType	= pcsSelfParty->m_eCalcExpType;
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsUserDivisionExp);
	
	pThis->SetPartyOption(pcsSelfParty);
	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventPartyOptionExpItemUpdate);
}

BOOL AgcmUIPartyOption::CBAddParty(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIPartyOption		*pThis				= (AgcmUIPartyOption *)	pClass;
	AgpdParty				*pcsParty			= (AgpdParty		 *) pData;

	pThis->m_ePartyOptionDivisionItem	= pcsParty->m_eDivisionItem;
	pThis->m_ePartyCalExpType	= pcsParty->m_eCalcExpType;
	pThis->SetPartyOption(pcsParty);

	pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventPartyOptionDivisionItemUpdate );
	pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventPartyOptionExpItemUpdate );

	return TRUE;
}


BOOL AgcmUIPartyOption::CBUpdateParty(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIPartyOption		*pThis				= (AgcmUIPartyOption *)	pClass;
	AgpdParty				*pcsParty			= (AgpdParty		 *) pData;
	pThis->SetPartyOption(pcsParty);

	return TRUE;
}

BOOL AgcmUIPartyOption::CBRemoveParty(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData)
		return FALSE;

	AgcmUIPartyOption		*pThis				= (AgcmUIPartyOption *)	pClass;
	pThis->SetPartyOption(NULL);
	return TRUE;
}

//	Begin 2005.04.14 by SungHoon
//	Party Option Control �ʱ�ȭ.
void AgcmUIPartyOption::SetPartyOption(AgpdParty *pcsParty)
{
//	������ �й� ��� �ʱⰪ ����
	if (pcsParty)
	{
		m_pcsUserDivisionItem->m_lSelectedIndex = 0;
		switch (pcsParty->m_eDivisionItem )
		{
		case AGPMPARTY_DIVISION_ITEM_SEQUENCE :
			m_pcsUserDivisionItem->m_lSelectedIndex = 1;
			break;
		case AGPMPARTY_DIVISION_ITEM_FREE :
			m_pcsUserDivisionItem->m_lSelectedIndex = 2;
			break;
		}
		m_pcsUserDivisionExp->m_lSelectedIndex = 0;
		switch (pcsParty->m_eCalcExpType )
		{
		case AGPMPARTY_EXP_TYPE_BY_LEVEL :
			m_pcsUserDivisionExp->m_lSelectedIndex = 1;
			break;
		}

	}
	else
	{
		m_pcsUserDivisionItem->m_lSelectedIndex = -1;
		m_pcsUserDivisionExp->m_lSelectedIndex = -1;
	}
	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDivisionItem);
	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDivisionExp);

	EnableUIForPartyLeader(pcsParty);
}
//	Finish 2005.04.13 by SungHoon


BOOL AgcmUIPartyOption::AddSystemMessage(CHAR* szMsg, DWORD dwColor)
{
	if(!szMsg)
		return FALSE;

	m_pcsAgcmChatting->AddSystemMessage(szMsg, dwColor);
	m_pcsAgcmChatting->EnumCallback(AGCMCHATTING2_CB_INPUT_MESSAGE, NULL, NULL);

	return TRUE;
}

/*
	�ش� ��⿡ ��ϵ� ��Ŷ�� �������� ��� ȣ��ȴ�.
*/
BOOL AgcmUIPartyOption::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pvPacket || !nSize)
		return FALSE;

	ZeroMemory(&m_stMessageData, sizeof(AgcmUIPartyOptionGetItemPartyMemberMessageData));

	m_csPacket.GetField(TRUE, pvPacket, nSize,
							&m_stMessageData.lTID,	        				// Item Template ID
							&m_stMessageData.lCID,			   				// Item Owner CID
							&m_stMessageData.lCount,		 				// Item Count
							&m_stMessageData.cNumPhysicalConvert,			// # of physical convert
							&m_stMessageData.cNumSocket,					// # of socket
							&m_stMessageData.cNumConvertedSocket,			// # of converted socket
							&m_stMessageData.cOptionCount					// option packet
							);

	m_pcsAgcmUIManager2->ThrowEvent( m_lEventPartyOptionItemGetMemeber );
	return TRUE;
}


/*
	�������� ȹ�� ���� ��� ������ �̸��� �����̸��� �������� �����ֱ� ���� Custom-Display �Լ�
*/
BOOL AgcmUIPartyOption::CBDisplayPartyOptionGetItemMember(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pClass ||
		!szDisplay)
		return FALSE;

	AgcmUIPartyOption		*pThis			= (AgcmUIPartyOption *)		pClass;
	AgcmUIPartyOptionGetItemPartyMemberMessageData *pcsMessagData	=	(AgcmUIPartyOptionGetItemPartyMemberMessageData *) pData;

	switch (lID) {
	case AGCMUI_PARTYOPTION_DISPLAY_ID_ITEM_PICKUP_NAME:
		{
			AgpdItemTemplate		*pcsItemTemplate		= pThis->m_pcsAgpmItem->GetItemTemplate(pcsMessagData->lTID);
			if (pcsItemTemplate)
			{
				if (pcsItemTemplate->m_lID == pThis->m_pcsAgpmItem->GetMoneyTID())
				{
					wsprintf(szDisplay, "%d %s", pcsMessagData->lCount, ClientStr().GetStr(STI_GELD_POST));
				}
				else if (pcsItemTemplate->m_bStackable)
				{
					wsprintf(szDisplay, "%s %d %s", pcsItemTemplate->m_szName, pcsMessagData->lCount, ClientStr().GetStr(STI_UNIT_POST));
				}
				else
				{
					DWORD	dwColor	= pThis->m_pcsAgcmUIItem->GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL);

					INT32	lItemRank	= 0;
					pThis->m_pcsAgpmFactors->GetValue(&pcsItemTemplate->m_csFactor, &lItemRank, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_RANK);

					if (pcsItemTemplate->m_ulTitleFontColor > 0)
						dwColor	= pcsItemTemplate->m_ulTitleFontColor;
					else if (pcsItemTemplate->m_nType == AGPMITEM_TYPE_EQUIP && lItemRank == 2)
						dwColor	= pThis->m_pcsAgcmUIItem->GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_RARE);
					else if (pcsItemTemplate->m_nType == AGPMITEM_TYPE_EQUIP && lItemRank == 3)
						dwColor = pThis->m_pcsAgcmUIItem->GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_UNIQUE);
					else if (pcsMessagData->cOptionCount)
						//dwColor = AGCMUIITEM_TOOLTIP_COLOR_OPTION;
						dwColor	=	pThis->m_pcsAgcmUIItem->GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL);
//					else if (pThis->m_pcsAgpmItemConvert->IsEgoItem(pcsItem))
//						dwColor	= AGCMUIITEM_TOOLTIP_COLOR_EGO;
					else if (pcsMessagData->cNumConvertedSocket > 0)
						dwColor	= pThis->m_pcsAgcmUIItem->GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_SPIRIT);

					// Item Name
					wsprintf(szDisplay, "<C%d>%s", dwColor, pcsItemTemplate->m_szName);

					// Physical Convert
					if ( pcsMessagData->cNumPhysicalConvert > 0 )
						wsprintf( szDisplay + strlen(szDisplay), " + %d", pcsMessagData->cNumPhysicalConvert);

					// Socket Convert
					if ( pcsMessagData->cNumSocket > 0 )
						wsprintf( szDisplay + strlen(szDisplay), " [%d/%d]", pcsMessagData->cNumConvertedSocket, pcsMessagData->cNumSocket );

					wsprintf(szDisplay + strlen(szDisplay), " <HFFFFFF>%s", ClientStr().GetStr(STI_ITEM_POST));
				}
			}
		}
		break;
	case AGCMUI_PARTYOPTION_DISPLAY_ID_PARTY_MEMBER :
		{
			AgpdCharacter			*pcsTargetCharacter		= pThis->m_pcsAgpmCharacter->GetCharacter(pcsMessagData->lCID);
			if (pcsTargetCharacter != NULL) wsprintf(szDisplay, pcsTargetCharacter->m_szID);
		}
		break;
	}

	return TRUE;
}

/*
	������ �й����� ����Ǿ��� ��� �������� �����ֱ� ���� Custom-Display �Լ�
*/
BOOL AgcmUIPartyOption::CBDisplayPartyOptionDivisionItemUpdate(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pClass ||
		!szDisplay)
		return FALSE;

	AgcmUIPartyOption		*pThis			= (AgcmUIPartyOption *)		pClass;
	
	switch (lID) {
	case AGCMUI_PARTYOPTION_DISPLAY_ID_DIVISION_ITEM_UPDATE:
		{
			AgpmPartyOptionDivisionItem eDivisionItem = pThis->m_ePartyOptionDivisionItem;

			if (eDivisionItem == AGPMPARTY_DIVISION_ITEM_SEQUENCE)
				wsprintf(szDisplay,"%s", pThis->m_aszDivisionItem[AGCMUI_PARTYOPTION_DIVISION_ITEM_SEQUENCE]);
			if (eDivisionItem == AGPMPARTY_DIVISION_ITEM_FREE)
				wsprintf(szDisplay,"%s", pThis->m_aszDivisionItem[AGCMUI_PARTYOPTION_DIVISION_ITEM_FREE]);
			if (eDivisionItem == AGPMPARTY_DIVISION_ITEM_DAMAGE)
				wsprintf(szDisplay,"%s", pThis->m_aszDivisionItem[AGCMUI_PARTYOPTION_DIVISION_ITEM_DAMAGE]);

		}
		break;
	case AGCMUI_PARTYOPTION_DISPLAY_ID_DIVISION_EXP_UPDATE:
		{
			AgpmPartyCalcExpType eCalcExpType = pThis->m_ePartyCalExpType;

			if (eCalcExpType == AGPMPARTY_EXP_TYPE_BY_DAMAGE)
				wsprintf(szDisplay,"%s", pThis->m_aszDivisionExp[AGCMUI_PARTYOPTION_DIVISION_EXP_DAMAGE]);
			if (eCalcExpType == AGPMPARTY_EXP_TYPE_BY_LEVEL)
				wsprintf(szDisplay,"%s", pThis->m_aszDivisionExp[AGCMUI_PARTYOPTION_DIVISION_EXP_LEVEL]);
		}
		break;
	}

	return TRUE;
}

/*
	�ʴ��� ����ڰ� �α��� ���� �ʾ��� ��� �Ҹ����� Custom-Display �Լ�
*/
BOOL AgcmUIPartyOption::CBDisplayPartyOptionNoLoginMember(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pClass ||
		!szDisplay)
		return FALSE;

	AgcmUIPartyOption		*pThis			= (AgcmUIPartyOption *)		pClass;
	AgpmPartyOptionDivisionItem eDivisionItem = pThis->m_ePartyOptionDivisionItem;
	
	switch (lID) {
	case AGCMUI_PARTYOPTION_DISPLAY_ID_NO_LOGIN_MEMBER:
		{
			if (pThis->m_pszNoLoginMemberID != NULL) wsprintf(szDisplay,"%s", pThis->m_pszNoLoginMemberID);
		}
		break;
	}

	return TRUE;
}


/*
	�α��� ���� ���� ����ڸ� ��Ƽ�� �ʴ����� ��� �Ҹ����� Call-Back �Լ�
*/
BOOL AgcmUIPartyOption::CBReceiveInviteFailedNoLoginMember(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIPartyOption		*pThis			= (AgcmUIPartyOption *)	pClass;
	INT32			 lOperatorID		= *(INT32 *)		pData;
	CHAR			*szTargetID			= ( CHAR *)			pCustData;

	if (lOperatorID != pThis->m_pcsAgcmCharacter->GetSelfCID() &&
		szTargetID == NULL)
		return TRUE;

	pThis->m_pszNoLoginMemberID = szTargetID;
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventPartyOptionRecvNoLoginMember);
	pThis->m_pszNoLoginMemberID	= NULL;

	return TRUE;
}



/*
	��Ƽ ������� �ɼǹ�ư���� Ȱ��ȭ�Ѵ�.
*/
void AgcmUIPartyOption::EnableUIForPartyLeader(AgpdParty *pcsParty)
{
	InitUIMessage();
	m_bEnableOptionForPartyLeader	= FALSE;
	m_bEnableOptionInviteMember		= FALSE;
	
	AgpdCharacter	*pcsSelfCharacter	= m_pcsAgcmCharacter->GetSelfCharacter();

	if (!pcsParty) m_bEnableOptionInviteMember = TRUE;
	if (pcsSelfCharacter && pcsParty)
	{
		if (pcsParty->m_lMemberListID[0] == pcsSelfCharacter->m_lID)
		{
			m_bEnableOptionForPartyLeader	= TRUE;
			m_bEnableOptionInviteMember		= TRUE;
		}
	}
	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsEnableDivisionItem);
	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsEnableDivisionExp);
	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsEnableOptionAply);
	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsEnableOptionInviteMember);

}
/*
	2005.05.06 by SungHoon
	��Ƽ�ɼ� �������̽��� ���°����ϸ� �����Ѵ�.
*/
BOOL AgcmUIPartyOption::CBPartyOptionOpenUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass ) return FALSE;

	AgcmUIPartyOption *pThis = (AgcmUIPartyOption *)	pClass;

	if ( AgpdCharacter *pcsSelfCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter() )
	{
		if ( AgcdUIControl *pcsUIInvitedPlayerControl = (AgcdUIControl *)pData1 )
		{
			if ( pcsUIInvitedPlayerControl->m_lType == AcUIBase::TYPE_EDIT )
			{
				AcUIEdit *pcsUIInvitedPlayerEdit = (AcUIEdit *)pcsUIInvitedPlayerControl->m_pcsBase;
				
				pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventPartyOptionUIOpen);		

				if (strlen(pThis->m_szDefaultID))
				{
					pcsUIInvitedPlayerEdit->SetText(pThis->m_szDefaultID);
					memset(pThis->m_szDefaultID, 0, AGPACHARACTER_MAX_ID_STRING+1);
				}
				else
				{
					pcsUIInvitedPlayerEdit->ClearText();
				}

				return TRUE;
			}
		}
	}

	return FALSE;
}

/*
	��Ƽ�ɼ��� ���̵��Է� ��Ʈ���� Ȱ��ȭ ���� �����Ѵ�.
*/
BOOL AgcmUIPartyOption::CBIsActivePartyOptionUIInviteMemberID(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
{
	if (!pClass ) return FALSE;

	AgcmUIPartyOption		*pThis				= (AgcmUIPartyOption *)	pClass;

	return ( pThis->m_bEnableOptionInviteMember );
}

/*
	2005.05.08 by SungHoon
	�޴� Ŭ������ ��Ƽ�ɼ��� ����.
*/
BOOL AgcmUIPartyOption::CBPartyOptionOpenUIClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass ) return FALSE;

	AgcmUIPartyOption		*pThis				= (AgcmUIPartyOption *)	pClass;
	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventPartyOptionUIOpenClick);		
}