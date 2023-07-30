#include "stdafx.h"
#include "../resource.h"		// main symbols
#include "AgcmEventGachaDlg.h"
#include "GachaSelectDlg.h"

AgcmEventGachaDlg::AgcmEventGachaDlg():
	m_pcsAgpmEventGacha( NULL ) ,
	m_pcsApmEventManager( NULL ) ,
	m_pcsApmEventManagerDlg( NULL ) 
{
	SetModuleName("AgcmEventGachaDlg");
}

AgcmEventGachaDlg::~AgcmEventGachaDlg()
{
}


// Virtual Function ��
BOOL	AgcmEventGachaDlg::OnAddModule()
{
	m_pcsAgpmEventGacha		= (AgpmEventGacha *)		GetModule("AgpmEventGacha");
	m_pcsApmEventManager	= (ApmEventManager *)		GetModule("ApmEventManager");
	m_pcsApmEventManagerDlg	= (ApmEventManagerDlg *)	GetModule("ApmEventManagerDlg");

	if (!m_pcsAgpmEventGacha ||
		!m_pcsApmEventManager ||
		!m_pcsApmEventManagerDlg)
		return FALSE;

	if (!m_pcsApmEventManagerDlg->AddEventDialog((ApmEventDialog *) this, APDEVENT_FUNCTION_GACHA))
		return FALSE;

	//if (!m_pcsAgpmShrine->SetCallbackAddShrineEvent(CBAddGachaEvent, this))
	//	return FALSE;

	return TRUE;
}

BOOL	AgcmEventGachaDlg::Open(ApdEvent *pcsEvent)
{
	// �̺�Ʈ ����Ʈ�� �Ұ�� �Ϸ� ȣ���̵�.

	INT32	nGachaIndex = PtrToInt( pcsEvent->m_pvData );

	CGachaSelectDlg	dlg( this , nGachaIndex );
	
	if( dlg.DoModal() == IDOK)
	{
		pcsEvent->m_pvData	= IntToPtr( dlg.m_nIndex );
	}
	
	return TRUE;
}

BOOL	AgcmEventGachaDlg::Close()
{
	return TRUE;
}

BOOL	AgcmEventGachaDlg::CBAddGachaEvent(PVOID pData, PVOID pClass, PVOID pCustData)
{
	/*
	// Dialog �߸鼭 � ��í�� ��������
	// ���� ���� �ֶǷ� �Ѵ�.
	ApdEvent			*pstEvent		= (ApdEvent *)				pData;
	AgcmEventGachaDlg	*pThis			= (AgcmEventGachaDlg *)		pClass;
	AgpdShrineTemplate	*pcsTemplate	= (AgpdShrineTemplate *)	pCustData;

	// shrine�� �߰��Ѵ�.
	AgpdShrine			*pcsShrine		= pThis->m_pcsAgpmShrine->AddShrine(pcsTemplate->m_lID, pcsTemplate->m_lID);
	if (!pcsShrine)
		return FALSE;

	pstEvent->m_pvData = pcsShrine;
	*/
	return TRUE;
}