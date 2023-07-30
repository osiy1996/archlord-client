#include <AgpmEventNPCDialog/AgpmEventNPCDialog.h>

AgpdEventMobDialogKeyTemplate *AgpmEventNPCDialog::GetMobKeyDialogTemplate( INT32 lMobKeyTID )
{
	AgpdEventMobDialogKeyTemplate	**ppcsKeyTemplate;

	ppcsKeyTemplate = (AgpdEventMobDialogKeyTemplate	**)m_csMobDialogKeyTemplate.GetObject( lMobKeyTID );

	if( ppcsKeyTemplate != NULL )
	{
		return *ppcsKeyTemplate;
	}
	else
	{
		return NULL;
	}
}

AgpdEventMobDialogTemplate *AgpmEventNPCDialog::GetMobDialogTemplate( INT32 lDialogID )
{
	//�� TID���� ��� Key ID�� ����.
	AgpdEventMobDialogTemplate		*pcsTemplate;
	AgpdEventMobDialogTemplate		**ppcsMobDialogTemplate;

	pcsTemplate = NULL;

	ppcsMobDialogTemplate = (AgpdEventMobDialogTemplate **)m_csMobDialogTemplate.GetObject( lDialogID );

	if( ppcsMobDialogTemplate != NULL )
	{
		pcsTemplate = *ppcsMobDialogTemplate;
	}
	else
	{
		pcsTemplate = NULL;
	}

	return pcsTemplate;
}
