// ApmEventManager.cpp: implementation of the ApmEventManager class.
//
//////////////////////////////////////////////////////////////////////

#include <ApmEventManager/ApmEventManager.h>
#include <ApModule/ApModuleStream.h>
#include <AgpmEventQuest/AgpmEventQuest.h>
#include <ApmObject/ApmObject.h>
#include <AgpmCharacter/AgpmCharacter.h>
#include <AgpmItem/AgpmItem.h>
#include <AgpmParty/AgpmParty.h>
#include <AgpmFactors/AgpmFactors.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ApmEventManager::ApmEventManager()
{
	SetModuleName("ApmEventManager");

	//EnableIdle(TRUE);

	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(AUTYPE_INT8, 1,			// Source Type
							AUTYPE_INT32, 1,		// Source ID (Object/Item/Character)
							AUTYPE_INT32, 1,		// Effect ID
							AUTYPE_INT32, 1,		// Function ID
							AUTYPE_END, 0
							);

	m_csPacketEvent.SetFlagLength(sizeof(INT8));
	m_csPacketEvent.SetFieldType(
							AUTYPE_PACKET,			1,			// event base packet
							AUTYPE_PACKET,			1,			// event custom data
							AUTYPE_END,				0
							);
							

	m_lMaxEvent						= 0;
	m_nMapAttachIndex				= 0;
	m_nObjectTemplateAttachIndex	= 0;
	m_nObjectAttachIndex			= 0;
	m_nCharacterTemplateAttachIndex	= 0;
	m_nCharacterAttachIndex			= 0;
	m_nItemTemplateAttachIndex		= 0;
	m_nItemAttachIndex				= 0;

	m_pcsApmMap				= NULL;
	m_pcsApmObject			= NULL;
	m_pcsAgpmGrid			= NULL;
	m_pcsAgpmFactors		= NULL;
	m_pcsAgpmCharacter		= NULL;
	m_pcsAgpmItem			= NULL;
	m_pcsAgpmParty			= NULL;

	for( int i = 0 ; i < APDEVENT_MAX_FUNCTION ; i ++ )
	{
		m_fnEventStreamWriter	[ i ] = NULL;
		m_fnEventStreamReader	[ i ] = NULL;
		m_fnEventConstructor	[ i ] = NULL;
		m_fnEventDestructor		[ i ] = NULL;
		m_pacsFunctionModule	[ i ] = NULL;
		m_fnEventIdleCallback	[ i ] = NULL;
	}

	SetPacketType(AGPMEVENT_MANAGER);

	m_fpMakePacketCallback.MemSetAll();
	m_fpReceivePacketCallback.MemSetAll();
	m_pcsModule.MemSetAll();
}

ApmEventManager::~ApmEventManager()
{
}

BOOL	ApmEventManager::OnAddModule()
{
	// ���� Module�� �����´�.
	m_pcsApmMap = (ApmMap *) GetModule("ApmMap");
	m_pcsApmObject = (ApmObject *) GetModule("ApmObject");
	m_pcsAgpmGrid = (AgpmGrid *) GetModule("AgpmGrid");
	m_pcsAgpmFactors = (AgpmFactors *) GetModule("AgpmFactors");
	m_pcsAgpmCharacter = (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pcsAgpmItem = (AgpmItem *) GetModule("AgpmItem");
	m_pcsAgpmParty = (AgpmParty *) GetModule("AgpmParty");

	/*
	if (!m_pcsApmMap || !m_pcsApmObject || !m_pcsAgpmCharacter || !m_pcsAgpmItem)
		return FALSE;
	*/

	if (m_pcsApmObject)
	{
		// Event Data�� Attach�Ѵ�.
		m_nObjectTemplateAttachIndex = m_pcsApmObject->AttachObjectTemplateData(this, sizeof(ApdEventAttachData), CBEventConstructorObjectTemplate, CBEventDestructor);
		m_nObjectAttachIndex = m_pcsApmObject->AttachObjectData(this, sizeof(ApdEventAttachData), CBEventConstructorObject, CBEventDestructor);

		if (m_nObjectTemplateAttachIndex < 0 || m_nObjectAttachIndex < 0)
			return FALSE;

		// Stream Callback ���
		if (!m_pcsApmObject->AddStreamCallback(APMOBJECT_DATA_OBJECT, CBStreamRead, CBStreamWrite, this))
			return FALSE;

		if (!m_pcsApmObject->AddStreamCallback(APMOBJECT_DATA_OBJECT_TEMPLATE, CBStreamRead, CBStreamWrite, this))
			return FALSE;

		// Init�� Remove Callback ���
		if (!m_pcsApmObject->SetCallbackInitObject(CBInitObject, this))
			return FALSE;
	}
	else
		return FALSE;

	if (m_pcsAgpmCharacter)
	{
		// Event Data�� Attach�Ѵ�.
		m_nCharacterTemplateAttachIndex = m_pcsAgpmCharacter->AttachCharacterTemplateData(this, sizeof(ApdEventAttachData), CBEventConstructorCharacterTemplate, CBEventDestructor);
		m_nCharacterAttachIndex = m_pcsAgpmCharacter->AttachCharacterData(this, sizeof(ApdEventAttachData), CBEventConstructorCharacter, CBEventDestructor);

		if (m_nCharacterTemplateAttachIndex < 0 || m_nCharacterAttachIndex < 0)
			return FALSE;

		// Stream Callback ���
		if (!m_pcsAgpmCharacter->AddStreamCallback(AGPMCHAR_DATA_TYPE_TEMPLATE, CBStreamRead, CBStreamWrite, this))
			return FALSE;

		if (!m_pcsAgpmCharacter->AddStreamCallback(AGPMCHAR_DATA_TYPE_CHAR, CBStreamRead, CBStreamWrite, this))
			return FALSE;

		if (!m_pcsAgpmCharacter->AddStreamCallback(AGPMCHAR_DATA_TYPE_STATIC, CBStreamRead, CBStreamWrite, this))
			return FALSE;
		
		// Init�� Remove Callback ���
		if (!m_pcsAgpmCharacter->SetCallbackInitChar(CBInitObject, this))
			return FALSE;
	}
	else
		return FALSE;

	if (m_pcsAgpmItem)
	{
		// Event Data�� Attach�Ѵ�.
		m_nItemTemplateAttachIndex = m_pcsAgpmItem->AttachItemTemplateData(this, sizeof(ApdEventAttachData), CBEventConstructorItemTemplate, CBEventDestructor);
		m_nItemAttachIndex = m_pcsAgpmItem->AttachItemData(this, sizeof(ApdEventAttachData), CBEventConstructorItem, CBEventDestructor);

		if (m_nItemTemplateAttachIndex < 0 || m_nItemAttachIndex < 0)
			return FALSE;

		// Stream Callback ���
		if (!m_pcsAgpmItem->AddStreamCallback(AGPMITEM_DATA_TYPE_TEMPLATE, CBStreamRead, CBStreamWrite, this))
			return FALSE;

		if (!m_pcsAgpmItem->AddStreamCallback(AGPMITEM_DATA_TYPE_ITEM, CBStreamRead, CBStreamWrite, this))
			return FALSE;

		// Init�� Remove Callback ���
		if (!m_pcsAgpmItem->SetCallbackInit(CBInitObject, this))
			return FALSE;
	}
	else
		return FALSE;

	return TRUE;
}	

BOOL	ApmEventManager::OnInit()
{
	//�޸� �Ҵ� 
	if (!m_csEvents.InitializeObject(sizeof(ApdEventAttachData *), m_lMaxEvent))
		return FALSE;
	
	return TRUE;
}

BOOL	ApmEventManager::OnIdle(UINT32 ulClockCount)
{
	PROFILE("ApmEventManager::OnIdle");

	/*
	INT32			lIndex			= 0;
	ApdEvent **		ppcsEvent;

	if (!m_csEvents.GlobalWLock())
		return FALSE;

	// Idle������, ��� Event�� �����ͼ� Enumerating �Ѵ�.
	for (ppcsEvent = (ApdEvent **) m_csEvents.m_csObject.GetSequence(&lIndex); ppcsEvent; ppcsEvent = (ApdEvent **) m_csEvents.m_csObject.GetSequence(&lIndex))
	{
		//if (!(*ppcsEvent)->m_Mutex.TryLock())
		//	continue;

		BOOL	bLockResult = TRUE;
		if ((*ppcsEvent)->m_pcsSource)
			bLockResult = (*ppcsEvent)->m_pcsSource->m_Mutex.WLock();

		if (!m_csEvents.GlobalRelease())
		{
			if ((*ppcsEvent)->m_pcsSource && bLockResult)
				(*ppcsEvent)->m_pcsSource->m_Mutex.Release();
			continue;
		}

		if ((*ppcsEvent)->m_pcsSource && !bLockResult)
			continue;

		if (APDEVENT_FUNCTION_VALID((*ppcsEvent)->m_eFunction) && (*ppcsEvent)->m_eFunction != APDEVENT_FUNCTION_NONE && m_fnEventIdleCallback[(*ppcsEvent)->m_eFunction])
			m_fnEventIdleCallback[(*ppcsEvent)->m_eFunction](*ppcsEvent, m_pacsFunctionModule[(*ppcsEvent)->m_eFunction], (PVOID) &ulClockCount);
//			EnumCallback((INT32) (*ppcsEvent)->m_eFunction, (*ppcsEvent), (PVOID) &ulClockCount);

		BOOL	bTimeOver = IsEndSkillTIme(*ppcsEvent, ulClockCount);
		INT32	lEID = (*ppcsEvent)->m_lEID;

		//(*ppcsEvent)->m_Mutex.Release();

		if ((*ppcsEvent)->m_pcsSource && bLockResult)
			(*ppcsEvent)->m_pcsSource->m_Mutex.Release();

		if (bTimeOver)
			RemoveEvent(lEID);

		if (!m_csEvents.GlobalWLock())
			return FALSE;
	}

	if (!m_csEvents.GlobalRelease())
		return FALSE;
	*/

	return TRUE;
}

BOOL	ApmEventManager::OnDestroy()
{
	return TRUE;
}

BOOL	ApmEventManager::EventConstructor(ApBase *pcsBase)
{
	return EventConstructor(GetEventData(pcsBase));
}

BOOL	ApmEventManager::EventConstructor(ApdEventAttachData *pcsEvents)
{
	if (!pcsEvents)
		return FALSE;

	pcsEvents->m_unFunction = 0;

	for (int i = 0; i < APDEVENT_MAX_EVENT; ++i)
	{
		pcsEvents->m_astEvent[i].m_eFunction = APDEVENT_FUNCTION_NONE;
		pcsEvents->m_astEvent[i].m_lEID = 0;
		pcsEvents->m_astEvent[i].m_pcsSource = NULL;
		pcsEvents->m_astEvent[i].m_pstCondition = NULL;
		pcsEvents->m_astEvent[i].m_pvData = NULL;
		pcsEvents->m_astEvent[i].m_ulEventStartTime = 0;
		pcsEvents->m_astEvent[i].m_Mutex.Init();
	}

	return TRUE;
}

BOOL	ApmEventManager::EventDestructor(ApdEventAttachData *pcsEvents)
{
	if(!pcsEvents)
		return FALSE;

	INT16					nIndex;

	for (nIndex = 0; nIndex < APDEVENT_MAX_EVENT; ++nIndex)
	{
		if (pcsEvents->m_astEvent[nIndex].m_eFunction != APDEVENT_FUNCTION_NONE)
			RemoveEvent(&pcsEvents->m_astEvent[nIndex]);

		pcsEvents->m_astEvent[nIndex].m_Mutex.Destroy();
	}

	return TRUE;
}

BOOL	ApmEventManager::CBEventConstructorCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	ApmEventManager *		pThis = (ApmEventManager *) pClass;
	ApBase *				pcsBase = (ApBase *) pData;

	pcsBase->m_eType = APBASE_TYPE_CHARACTER;

	return pThis->EventConstructor(pcsBase);
}

BOOL	ApmEventManager::CBEventConstructorCharacterTemplate(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	ApmEventManager *		pThis = (ApmEventManager *) pClass;
	ApBase *				pcsBase = (ApBase *) pData;

	pcsBase->m_eType = APBASE_TYPE_CHARACTER_TEMPLATE;

	return pThis->EventConstructor(pcsBase);
}

BOOL	ApmEventManager::CBEventConstructorItem(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	ApmEventManager *		pThis = (ApmEventManager *) pClass;
	ApBase *				pcsBase = (ApBase *) pData;

	pcsBase->m_eType = APBASE_TYPE_ITEM;

	return pThis->EventConstructor(pcsBase);
}

BOOL	ApmEventManager::CBEventConstructorItemTemplate(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	ApmEventManager *		pThis = (ApmEventManager *) pClass;
	ApBase *				pcsBase = (ApBase *) pData;

	pcsBase->m_eType = APBASE_TYPE_ITEM_TEMPLATE;

	return pThis->EventConstructor(pcsBase);
}

BOOL	ApmEventManager::CBEventConstructorObject(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	ApmEventManager *		pThis = (ApmEventManager *) pClass;
	ApBase *				pcsBase = (ApBase *) pData;

	pcsBase->m_eType = APBASE_TYPE_OBJECT;

	return pThis->EventConstructor(pcsBase);
}

BOOL	ApmEventManager::CBEventConstructorObjectTemplate(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	ApmEventManager *		pThis = (ApmEventManager *) pClass;
	ApBase *				pcsBase = (ApBase *) pData;

	pcsBase->m_eType = APBASE_TYPE_OBJECT_TEMPLATE;

	return pThis->EventConstructor(pcsBase);
}

BOOL	ApmEventManager::CBEventDestructor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	ApmEventManager *		pThis = (ApmEventManager *) pClass;
	ApdEventAttachData *	pcsEvents = pThis->GetEventData((ApBase *) pData);

	return pThis->EventDestructor(pcsEvents);
}

void	ApmEventManager::SetMaxEvent(INT32 lEvent)
{
	m_lMaxEvent = lEvent;
}

BOOL			ApmEventManager::RegisterEvent(ApdEventFunction eFunction, ApModuleDefaultCallBack fnConstructor, ApModuleDefaultCallBack fnDestructor, 
											   ApModuleDefaultCallBack fnCallback, ApModuleDefaultCallBack fnStreamWriter, ApModuleDefaultCallBack fnStreamReader, PVOID pvClass)
{
	if (!APDEVENT_FUNCTION_VALID(eFunction))
	{
		OutputDebugString("ApmEventManager::RegisterEvent() Error(1) !!!\n");
		return FALSE;
	}

	// Data Size�� Enumeration Function ����Ѵ�.
	//m_nEventDataSize[eFunction] = nDataSize;
	m_fnEventConstructor[eFunction] = fnConstructor;
	m_fnEventDestructor[eFunction] = fnDestructor;
	m_fnEventStreamWriter[eFunction] = fnStreamWriter;
	m_fnEventStreamReader[eFunction] = fnStreamReader;
	m_pacsFunctionModule[eFunction] = (ApModule *) pvClass;
	m_fnEventIdleCallback[eFunction] = fnCallback;
	/*
	if (fnCallback)
		return SetCallback((INT16) eFunction, fnCallback, pvClass);
	*/

	return TRUE;
}

BOOL			ApmEventManager::IsRegistered(ApdEventFunction eFunction)
{
	if (!m_pacsFunctionModule[eFunction])
		return FALSE;

	return TRUE;
}

ApdEvent *		ApmEventManager::AddEvent(ApdEventAttachData * pcsEvents, ApdEventFunction eFunction, ApBase * pcsSource, BOOL bAddList, INT32 lEID, UINT32 ulStartTime)
{
	INT16					nIndex;

	// Validation Check
	if (!pcsEvents)
	{
		// ������ (2003-12-17 ���� 4:00:55) : ����ó���� ���ּ��� -_-;
		TRACE( "ApmEventManager::AddEvent �̺�Ʈ �Է��� ��\n" );
//		TRACEFILE(ALEF_ERROR_FILENAME, "ApmEventManager::AddEvent �̺�Ʈ �Է��� ��\n" );
		return NULL;
	}

	if (pcsEvents->m_unFunction > APDEVENT_MAX_EVENT)
	{
		// ������ (2003-12-17 ���� 4:00:55) : ����ó���� ���ּ��� -_-;
		TRACE( "ApmEventManager::AddEvent APDEVENT_MAX_EVENT�ε��� �ʰ�\n" );
//		TRACEFILE(ALEF_ERROR_FILENAME, "ApmEventManager::AddEvent APDEVENT_MAX_EVENT�ε��� �ʰ�\n" );
		return NULL;
	}

	// �̹� ����� �Ǿ� �ִ��� ����.
	for (nIndex = 0; nIndex < pcsEvents->m_unFunction; ++nIndex)
	{
		if (pcsEvents->m_astEvent[nIndex].m_eFunction == eFunction)
		{
			// ������ (2003-12-17 ���� 4:00:55) : ����ó���� ���ּ��� -_-;
			TRACE( "ApmEventManager::AddEvent �̵̹�ϵǾ�����.\n" );
//			TRACEFILE(ALEF_ERROR_FILENAME, "ApmEventManager::AddEvent �̵̹�ϵǾ�����.\n" );
			return NULL;
		}
	}

	// ���ʿ� ����� ������ �ִ´�.
	for (nIndex = 0; nIndex < pcsEvents->m_unFunction; ++nIndex)
	{
		if (pcsEvents->m_astEvent[nIndex].m_eFunction == APDEVENT_FUNCTION_NONE)
		{
			break;
		}
	}

	// Event�� �ִ� ������ŭ �̹� ��ϵǾ� ������, NULL�� return
	if (nIndex >= APDEVENT_MAX_EVENT)
	{
		// ������ (2003-12-17 ���� 4:00:55) : ����ó���� ���ּ��� -_-;
		TRACE( "ApmEventManager::AddEvent �̺�Ʈ �ִ� ���� �ʰ�..\n" );
//		TRACEFILE(ALEF_ERROR_FILENAME, "ApmEventManager::AddEvent �̺�Ʈ �ִ� ���� �ʰ�..\n" );
		return NULL;
	}

	// EID�� Set�Ѵ�.
	// EID�� 0�̰� List�� Add�� ���̶��, Local Generating�Ѵ�.
	// EID�� 0�� �ƴѵ�, APDEVENT_EID_FLAG_LOCAL�� set�Ǿ� ������ Error�̴�.
	if (!lEID && bAddList)
	{
		lEID = m_csGenerateEID.GetID();
		lEID |= APDEVENT_EID_FLAG_LOCAL;
	}

	// ������ (2003-12-17 ���� 4:01:28) : �м� �������� ���������.
//	else if (lEID & APDEVENT_EID_FLAG_LOCAL)
//	{
//		// ������ (2003-12-17 ���� 4:00:55) : ����ó���� ���ּ��� -_-;
//		TRACE( "ApmEventManager::AddEvent Local Generating ����..\n" );
//		return NULL;
//	}


	// ������ Event �������� Set�Ѵ�.
	pcsEvents->m_astEvent[nIndex].m_lEID				= lEID			;
	pcsEvents->m_astEvent[nIndex].m_pcsSource			= pcsSource		;
	pcsEvents->m_astEvent[nIndex].m_eFunction			= eFunction		;
	pcsEvents->m_astEvent[nIndex].m_pstCondition		= NULL			;
	pcsEvents->m_astEvent[nIndex].m_ulEventStartTime	= ulStartTime	;

	if (m_fnEventConstructor[eFunction] && !m_fnEventConstructor[eFunction](&pcsEvents->m_astEvent[nIndex], m_pacsFunctionModule[eFunction], NULL))
	{
		ASSERT( !"Event ��Ͻ���!.. �����ǰ� ��Ʈ���� Ȯ�ο��." );

		pcsEvents->m_astEvent.MemSet(nIndex, 1);

		return NULL;
	}

	// �տ� �� ���� ������ ��쿡�� Function ������ �ϳ� ����
	if (nIndex == pcsEvents->m_unFunction)
		pcsEvents->m_unFunction += 1;

	if (bAddList)
		m_csEvents.AddEvent(&pcsEvents->m_astEvent[pcsEvents->m_unFunction - 1], lEID);

	return &pcsEvents->m_astEvent[nIndex];
}

ApdEvent *		ApmEventManager::AddEvent(ApBase *pcsData, ApdEventFunction eFunction)
{
	return AddEvent(GetEventData(pcsData), eFunction, pcsData, FALSE);
}

ApdEvent *		ApmEventManager::GetEventSquence(INT32 *plIndex)
{
	return m_csEvents.GetSequence( plIndex );
}


ApdEventAttachData *		ApmEventManager::GetEventData(ApBase *pcsData)
{
	if (!pcsData)
		return NULL;

	switch (pcsData->m_eType)
	{
	case APBASE_TYPE_OBJECT:
		return (ApdEventAttachData *) (m_pcsApmObject ? m_pcsApmObject->GetAttachedModuleData(m_nObjectAttachIndex, pcsData) : NULL);
	case APBASE_TYPE_OBJECT_TEMPLATE:
		return (ApdEventAttachData *) (m_pcsApmObject ? m_pcsApmObject->GetAttachedModuleData(m_nObjectTemplateAttachIndex, pcsData) : NULL);
	case APBASE_TYPE_CHARACTER:
		return (ApdEventAttachData *) (m_pcsAgpmCharacter ? m_pcsAgpmCharacter->GetAttachedModuleData(m_nCharacterAttachIndex, pcsData) : NULL);
	case APBASE_TYPE_CHARACTER_TEMPLATE:
		return (ApdEventAttachData *) (m_pcsAgpmCharacter ? m_pcsAgpmCharacter->GetAttachedModuleData(m_nCharacterTemplateAttachIndex, pcsData) : NULL);
	case APBASE_TYPE_ITEM:
		{
			ApdEventAttachData * pstEventAD = (ApdEventAttachData *) (m_pcsAgpmItem ? m_pcsAgpmItem->GetAttachedModuleData(m_nItemAttachIndex, pcsData) : NULL);
			// ���⼭ �̺�Ʈ ���̴� �κ���
			// AgpmItem.cpp Line 234 �� ���ø� Ȯ���Ŀ� �ű⵵�� �Ѵ�.
			AgpdItem	* pcsItem = ( AgpdItem * ) pcsData;

			if( pstEventAD && pcsItem->m_pcsItemTemplate && pcsItem->m_pcsItemTemplate->m_nQuestGroup )
			{
				// �Ⱥپ�������...

				int lIndex;
				for( lIndex = 0 ; lIndex < pstEventAD->m_unFunction; lIndex++ )
				{
					if( pstEventAD->m_astEvent[ lIndex ].m_eFunction == APDEVENT_FUNCTION_QUEST )
						break;		
				}

				if( lIndex == pstEventAD->m_unFunction )
				{
					// ����Ʈ�� �ٿ���� �Ѵ�.
					// ������

					ApdEvent * pstEvent;
					pstEvent = AddEvent( pstEventAD , APDEVENT_FUNCTION_QUEST , pcsItem , FALSE );

					AgpdQuestEventAttachData * pcsAttachData = ( AgpdQuestEventAttachData * ) pstEvent->m_pvData;

					if( pcsAttachData )
					{
						pcsAttachData->lQuestGroupID = pcsItem->m_pcsItemTemplate->m_nQuestGroup;
					}
					else
					{
						ASSERT( !"����ġ ����Ÿ�� ���Դϴ�. ����Ʈ �̺�Ʈ�� �����ڸ� Ȯ���ϼ���" );
					}
				}
			}
			return pstEventAD;
		}
	case APBASE_TYPE_ITEM_TEMPLATE:
		return (ApdEventAttachData *) (m_pcsAgpmItem ? m_pcsAgpmItem->GetAttachedModuleData(m_nItemTemplateAttachIndex, pcsData) : NULL);
	}

	return NULL;
}

ApBase *		ApmEventManager::GetBase(ApBaseType eBaseType, INT32 lID)
{
	// Source�� Type�� ���� �˸��� Data������ Event Attache Data�� �����´�.
	switch (eBaseType)
	{
	case APBASE_TYPE_OBJECT:
		return m_pcsApmObject ? m_pcsApmObject->GetObject(lID) : NULL;

	case APBASE_TYPE_OBJECT_TEMPLATE:
		return m_pcsApmObject ? m_pcsApmObject->GetObjectTemplate(lID) : NULL;

	case APBASE_TYPE_CHARACTER:
		return m_pcsAgpmCharacter ? m_pcsAgpmCharacter->GetCharacter(lID) : NULL;

	case APBASE_TYPE_CHARACTER_TEMPLATE:
		return m_pcsAgpmCharacter ? m_pcsAgpmCharacter->GetCharacterTemplate(lID) : NULL;

	case APBASE_TYPE_ITEM:
		return m_pcsAgpmItem ? m_pcsAgpmItem->GetItem(lID) : NULL;

	case APBASE_TYPE_ITEM_TEMPLATE:
		return m_pcsAgpmItem ? m_pcsAgpmItem->GetItemTemplate(lID) : NULL;

	case APBASE_TYPE_PARTY:
		return m_pcsAgpmParty ? m_pcsAgpmParty->GetParty(lID) : NULL;
	}

	return NULL;
}

ApdEvent *		ApmEventManager::GetEvent(INT32 lEID)
{
	return m_csEvents.GetEvent(lEID);
}

ApdEvent *		ApmEventManager::GetEvent(ApBaseType eSourceType, INT32 lSourceID, ApdEventFunction eFunction)
{
	return GetEvent(GetBase(eSourceType, lSourceID), eFunction);
}

ApdEvent *		ApmEventManager::GetEvent(ApBase *pcsBase, ApdEventFunction eFunction)
{
	if (!pcsBase)
		return NULL;

	return GetEvent(GetEventData(pcsBase), eFunction);
}

ApdEvent *		ApmEventManager::GetEvent(ApdEventAttachData * pcsEvents, ApdEventFunction eFunction)
{
	ApdEvent *				pcsEvent = NULL;
	INT16					nIndex;

	if (!pcsEvents)
		return NULL;

	// Event Attach Data���� efunction�ΰ��� �����´�.
	if (pcsEvents)
	{
		for (nIndex = 0; nIndex < pcsEvents->m_unFunction; ++nIndex)
		{
			if (pcsEvents->m_astEvent[nIndex].m_eFunction == eFunction)
			{
				pcsEvent = &pcsEvents->m_astEvent[nIndex];
				break;
			}
		}
	}

	return pcsEvent;
}

BOOL		ApmEventManager::CheckEventAvailable( ApBase * pcsBase )
{
	if (!pcsBase)
		return FALSE;

	ApdEventAttachData * pcsEvent = GetEventData(pcsBase);

	if ( pcsEvent && pcsEvent->m_unFunction > 0 )
		return TRUE;
	else
		return FALSE;
}

BOOL			ApmEventManager::RemoveEvent(ApdEvent *pcsEvent)
{
	// Validation Check
	if (!pcsEvent)
		return FALSE;

	if (m_fnEventDestructor[pcsEvent->m_eFunction] && !m_fnEventDestructor[pcsEvent->m_eFunction](pcsEvent, m_pacsFunctionModule[pcsEvent->m_eFunction], NULL))
		return FALSE;

	//if (!pcsEvent->m_Mutex.RemoveLock())
	//	return FALSE;

	// EID�� ������, List���� �����Ѵ�.
	if (pcsEvent->m_lEID)
	{
		m_csEvents.GlobalWLock();
		m_csEvents.RemoveEvent(pcsEvent->m_lEID);
		m_csEvents.GlobalRelease();
	}

	pcsEvent->m_eFunction = APDEVENT_FUNCTION_NONE;

	// �Ҵ�� memory���� �����Ѵ�.
	DestroyCondition(pcsEvent);

	pcsEvent->m_pstCondition = NULL;

	/*
	if (pcsEvent->m_pvData)
		free(pcsEvent->m_pvData);
	*/

	pcsEvent->m_pvData = NULL;
	pcsEvent->m_pcsSource = NULL;
	pcsEvent->m_lEID = 0;

	//pcsEvent->m_Mutex.Release();

	return TRUE;
}

BOOL			ApmEventManager::RemoveEvent(INT32 lEID)
{
	return RemoveEvent(GetEvent(lEID));
}

AuPOS *			ApmEventManager::GetBasePos(ApBase *pcsBase, AuMATRIX **ppstDirection)
{
	if (!pcsBase)
		return NULL;

	switch (pcsBase->m_eType)
	{
	case APBASE_TYPE_OBJECT:
		{
			// Object�� ���, Object�� ��ġ�� ���� matrix�� return
			ApdObject *	pcsObject = (ApdObject *) pcsBase;

			if (ppstDirection)
				*ppstDirection = &pcsObject->m_stMatrix;

			return &pcsObject->m_stPosition;
		}
		break;

	case APBASE_TYPE_CHARACTER:
		{
			// Character�� ���, Character�� ��ġ�� ���� matrix�� return
			AgpdCharacter *	pcsCharacter = (AgpdCharacter *) pcsBase;

			if (ppstDirection)
				*ppstDirection = &pcsCharacter->m_stMatrix;

			return &pcsCharacter->m_stPos;
		}
		break;

	case APBASE_TYPE_ITEM:
		{
			// Item�� ���
			AgpdItem *	pcsItem = (AgpdItem *) pcsBase;

			switch (pcsItem->m_eStatus)
			{
			case AGPDITEM_STATUS_FIELD:
				{
					// Item�� Field�� ������, ���⼺ ���� ��ġ�� return
					if (ppstDirection)
						*ppstDirection = NULL;

					return &pcsItem->m_posItem;
				}
				break;
				
			case AGPDITEM_STATUS_INVENTORY:
			case AGPDITEM_STATUS_EQUIP:
				{
					// Inventory�� Equip Slot�� ������, Character�� ��ġ�� ������ return
					AgpdCharacter *	pcsCharacter = m_pcsAgpmCharacter ? m_pcsAgpmCharacter->GetCharacter(pcsItem->m_ulCID) : NULL;

					if (!pcsCharacter)
						return NULL;

					return GetBasePos(pcsCharacter, ppstDirection);
				}
				break;

			}
		}
		break;

	}

	return NULL;
}

AuPOS *			ApmEventManager::GetBasePos(ApdEvent *pstEvent, AuPOS *pstPos)
{
	AuMATRIX *	pstDirection;

	if( pstPos != NULL )
	{
		*pstPos = *GetBasePos(pstEvent->m_pcsSource, &pstDirection);
	}

	return pstPos;
}

BOOL			ApmEventManager::StreamWrite(ApdEventAttachData *pcsEvents, ApModuleStream *pcsStream)
{
	ApdEvent *				pstEvent;
	INT16					nIndex;
	INT32					lIndex2;

	// ��ϵ� Function�� ������ŭ Loop
	for (nIndex = 0; nIndex < pcsEvents->m_unFunction; ++nIndex)
	{
		pstEvent = &pcsEvents->m_astEvent[nIndex];

		if (APDEVENT_FUNCTION_VALID(pstEvent->m_eFunction) && pstEvent->m_eFunction != APDEVENT_FUNCTION_NONE)
		{
			// Function ����
			if (!pcsStream->WriteValue(APMEVENT_STREAM_NAME_FUNCTION, pstEvent->m_eFunction))
				return FALSE;

			// Event ID
			if (!pcsStream->WriteValue(APMEVENT_STREAM_NAME_EID, pstEvent->m_lEID))
				return FALSE;

			if (!pcsStream->WriteValue(APMEVENT_STREAM_NAME_COND_START, 0))
				return FALSE;

			// Condition�� �ִٸ�, Write
			if (pstEvent->m_pstCondition)
			{
				if (pstEvent->m_pstCondition->m_pstTarget)
				{
					if (!pcsStream->WriteValue(APMEVENT_STREAM_NAME_COND_TYPE, APDEVENT_COND_TARGET))
						return FALSE;

					for (lIndex2 = 0; pstEvent->m_pstCondition->m_pstTarget->m_alItemTID[lIndex2]; ++lIndex2)
					{
						if (!pcsStream->WriteValue(APMEVENT_STREAM_NAME_TARGET_ITID, pstEvent->m_pstCondition->m_pstTarget->m_alItemTID[lIndex2]))
							return FALSE;
					}
				}

				if (pstEvent->m_pstCondition->m_pstArea)
				{
					if (!pcsStream->WriteValue(APMEVENT_STREAM_NAME_COND_TYPE, APDEVENT_COND_AREA))
						return FALSE;

					if (!pcsStream->WriteValue(APMEVENT_STREAM_NAME_AREA_TYPE, pstEvent->m_pstCondition->m_pstArea->m_eType))
						return FALSE;

					switch (pstEvent->m_pstCondition->m_pstArea->m_eType)
					{
					case APDEVENT_AREA_SPHERE:
						if (!pcsStream->WriteValue(APMEVENT_STREAM_NAME_AREA_SPHERE_RADIUS, pstEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius))
							return FALSE;

						break;

					case APDEVENT_AREA_FAN:
						if (!pcsStream->WriteValue(APMEVENT_STREAM_NAME_AREA_FAN_RADIUS, pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stPan.m_fRadius))
							return FALSE;
						if (!pcsStream->WriteValue(APMEVENT_STREAM_NAME_AREA_ANGLE, pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stPan.m_f2BySinThetaDiv2))
							return FALSE;

						break;

					case APDEVENT_AREA_BOX:
						if (!pcsStream->WriteValue(APMEVENT_STREAM_NAME_AREA_BOX_INF, &pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stBox.inf))
							return FALSE;
						if (!pcsStream->WriteValue(APMEVENT_STREAM_NAME_AREA_BOX_SUP, &pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stBox.sup))
							return FALSE;

						break;
					}
				}
			}

			if (!pcsStream->WriteValue(APMEVENT_STREAM_NAME_COND_END, 0))
				return FALSE;

			// �ش� Event Callback�� Call
			if (m_fnEventStreamWriter[pstEvent->m_eFunction])
			{
				if (!m_fnEventStreamWriter[pstEvent->m_eFunction](&pcsEvents->m_astEvent[nIndex], m_pacsFunctionModule[pstEvent->m_eFunction], pcsStream))
					return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL			ApmEventManager::StreamRead(ApBase *pcsBase, ApdEventAttachData *pcsEvents, ApModuleStream *pcsStream)
{
	const CHAR *			szValueName;
	ApdEventFunction		eFunction;
	INT32					lEID;
	ApdEvent *				pstEvent;
	INT32					lIndex;

	if (!pcsStream->ReadNextValue())
		return TRUE;

	pcsEvents->m_unFunction = 0;

	while (1)
	{
		// Function ���� �о�´�.
		szValueName = pcsStream->GetValueName();
		if (strncmp(szValueName, APMEVENT_STREAM_NAME_FUNCTION, strlen(APMEVENT_STREAM_NAME_FUNCTION)))
			return TRUE;

		pcsStream->GetValue((INT32 *) &eFunction);

		pcsEvents->m_astEvent[pcsEvents->m_unFunction].m_eFunction = eFunction;
		if (!APDEVENT_FUNCTION_VALID(eFunction))
			return FALSE;

		if (!pcsStream->ReadNextValue())
			return TRUE;

		// Event ID �о�´�.
		szValueName = pcsStream->GetValueName();
		if (strncmp(szValueName, APMEVENT_STREAM_NAME_EID, strlen(APMEVENT_STREAM_NAME_EID)))
			return FALSE;

		pcsStream->GetValue((INT32 *) &lEID);

		// ���� EID�� Local Generation�̸�, ������ Generating���ش�.
		if (lEID & APDEVENT_EID_FLAG_LOCAL)
			lEID = m_csGenerateEID.GetID() | APDEVENT_EID_FLAG_LOCAL;

		pstEvent = AddEvent(pcsEvents, eFunction, pcsBase, TRUE, lEID);
		if (!pstEvent)
			return FALSE;

		if (!pcsStream->ReadNextValue())
			return TRUE;

		szValueName = pcsStream->GetValueName();

		if (strcmp(szValueName, APMEVENT_STREAM_NAME_COND_START))
			return FALSE;

		lIndex = 0;
		while (strcmp(szValueName, APMEVENT_STREAM_NAME_COND_END))
		{
			ApdEventCondFlag	eFlag;

			while (1)
			{
				if (!pcsStream->ReadNextValue())
					return TRUE;

				szValueName = pcsStream->GetValueName();

				if (!strcmp(szValueName, APMEVENT_STREAM_NAME_COND_TYPE))
				{
					pcsStream->GetValue((INT32 *) &eFlag);
					SetCondition(pstEvent, eFlag);
				}
				else if (!strcmp(szValueName, APMEVENT_STREAM_NAME_TARGET_ITID))
				{
					pcsStream->GetValue(&pstEvent->m_pstCondition->m_pstTarget->m_alItemTID[lIndex]);
					++lIndex;
				}
				else if (!strcmp(szValueName, APMEVENT_STREAM_NAME_AREA_TYPE))
				{
					pcsStream->GetValue((INT32 *) &pstEvent->m_pstCondition->m_pstArea->m_eType);
				}
				else if (!strcmp(szValueName, APMEVENT_STREAM_NAME_AREA_SPHERE_RADIUS))
				{
					pcsStream->GetValue(&pstEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius);

					/*
					if( pstEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius == 0.0f )
					{
						pstEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius = 5000.0f;

						printf( "Sphere 5000���� ����!\n" );
						//getchar();
					}
					*/
				}
				else if (!strcmp(szValueName, APMEVENT_STREAM_NAME_AREA_FAN_RADIUS))
				{
					pcsStream->GetValue(&pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stPan.m_fRadius);
				}
				else if (!strcmp(szValueName, APMEVENT_STREAM_NAME_AREA_ANGLE))
				{
					pcsStream->GetValue(&pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stPan.m_f2BySinThetaDiv2);
				}
				else if (!strcmp(szValueName, APMEVENT_STREAM_NAME_AREA_BOX_INF))
				{
					pcsStream->GetValue(&pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stBox.inf);
				}
				else if (!strcmp(szValueName, APMEVENT_STREAM_NAME_AREA_BOX_SUP))
				{
					pcsStream->GetValue(&pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stBox.sup);
				}
				else if (!strcmp(szValueName, APMEVENT_STREAM_NAME_COND_END))
					break;
			}
		}

		/*
		pcsEvents->m_astEvent[pcsEvents->m_unFunction].m_lEID = lEID;
		pcsEvents->m_astEvent[pcsEvents->m_unFunction].m_pcsSource = (ApBase *)pData;
		pcsEvents->m_unFunction += 1;
		*/

		// �˸��� Function Callback�� Call���ش�.
		if (m_fnEventStreamReader[eFunction])
		{
			if (!m_fnEventStreamReader[eFunction](pstEvent, m_pacsFunctionModule[eFunction], pcsStream))
				return FALSE;
		}

		if (!pcsStream->ReadNextValue())
			return TRUE;
	}

	return TRUE;
}

BOOL			ApmEventManager::CBStreamWrite(PVOID pData, ApModule *pClass, ApModuleStream *pcsStream)
{
	ApmEventManager *		pThis = (ApmEventManager *) pClass;
	ApdEventAttachData *	pcsEvents = pThis->GetEventData((ApBase *) pData);

	if( pcsEvents )
		return pThis->StreamWrite(pcsEvents, pcsStream);
	else
		return TRUE;
}

BOOL			ApmEventManager::CBStreamRead(PVOID pData, ApModule *pClass, ApModuleStream *pcsStream)
{
	ApmEventManager *		pThis = (ApmEventManager *) pClass;
	ApdEventAttachData *	pcsEvents = pThis->GetEventData((ApBase *) pData);

	return pThis->StreamRead((ApBase *) pData, pcsEvents, pcsStream);
}

BOOL			ApmEventManager::CBInitObject(PVOID pData, PVOID pClass, PVOID pCustData)
{
	ApmEventManager *		pThis = (ApmEventManager *) pClass;

	return pThis->InitObject((ApBase *) pData, pCustData ? (pThis->GetEventData((ApBase *) pCustData)) : NULL);
}

BOOL			ApmEventManager::InitObject(ApBase *pcsDstBase, ApdEventAttachData *pcsSrcEvents)
{
	UINT32					unIndex;
	ApdEventAttachData *	pcsDstEvents = GetEventData(pcsDstBase);
	ApdEvent *				pstSrcEvent;
	ApdEvent *				pstDstEvent;

	// Source�� Event ������ŭ
	for (unIndex = 0; unIndex < pcsSrcEvents->m_unFunction; ++unIndex)
	{
		pstSrcEvent	= &pcsSrcEvents->m_astEvent[unIndex];
		pstDstEvent	= &pcsDstEvents->m_astEvent[pcsDstEvents->m_unFunction];

		// ��� Copy�� �ִ´�.
		pstDstEvent->m_eFunction	= pstSrcEvent->m_eFunction;
		pstDstEvent->m_lEID			= m_csGenerateEID.GetID() | APDEVENT_EID_FLAG_LOCAL;
		pstDstEvent->m_pcsSource	= pcsDstBase;

		// Condition�� �޸� �Ҵ��ؼ� Copy�ϰ�
		if (pstSrcEvent->m_pstCondition)
		{
			// Condition�� Copy�Ѵ�.
			CopyCondition(pstDstEvent, pstSrcEvent);
		}

		// ���� Callback�� �����ϸ�(OnIdle()�� �Ҹ��� �Լ��� ������), Event�� Add�Ѵ�.
		if (ExistCallback(pstDstEvent->m_eFunction))
		{
			m_csEvents.AddEvent(pstDstEvent, pstDstEvent->m_lEID);
		}

		// Constructor�� �ִٸ�, Callback���ش�.
		if (m_fnEventConstructor[pstSrcEvent->m_eFunction] && 
			!m_fnEventConstructor[pstSrcEvent->m_eFunction](pstDstEvent, m_pacsFunctionModule[pstSrcEvent->m_eFunction], pstSrcEvent))
		{
			memset(pstDstEvent, 0, sizeof(ApdEvent));

			return FALSE;
		}

		//�̺�Ʈ�� �ϳ� �þ���� ������ �ϳ� �����ؾ��Ѵ�. 2003-03-11 Ashulam�߰�
		pcsDstEvents->m_unFunction++;
		if (pcsDstEvents->m_unFunction >= APDEVENT_MAX_EVENT)
			break;
	}

	if (pcsDstEvents->m_unFunction)
		EnumCallback(APMEVENT_CB_INIT_OBJECT, pcsDstBase, pcsSrcEvents);

	return TRUE;
}

PVOID			ApmEventManager::MakeBasePacket(ApdEvent *pstEvent)
{
	INT16	nSize;
	PVOID	pvPacket;

	if (!pstEvent)
		return NULL;

	if (pstEvent->m_lEID == 0 || pstEvent->m_lEID & APDEVENT_EID_FLAG_LOCAL)
	{
		if (!pstEvent->m_pcsSource)
			return NULL;

		// Local Generated ID���, Source�� Type�� ID�� Packet�� �����.
		pvPacket = m_csPacket.MakePacket(FALSE, &nSize, 0,
			&pstEvent->m_pcsSource->m_eType,
			&pstEvent->m_pcsSource->m_lID,
			NULL,
			&pstEvent->m_eFunction );
	}
	else
	{
		// Global Generated ID���, EID�� Packet�� �����.
		pvPacket = m_csPacket.MakePacket(FALSE, &nSize, 0,
			NULL, 
			NULL, 
			&pstEvent->m_lEID,
			NULL );
	}

	return pvPacket;
}

ApdEvent *		ApmEventManager::GetEventFromBasePacket(PVOID pvPacket)
{
	INT8					cSourceType = -1;
	INT32					lSourceID = 0;
	INT32					lEID = 0;
	ApdEventFunction		lFunctionID;
	ApdEvent *				pstEvent = NULL;

	m_csPacket.GetField(FALSE, pvPacket, 0, 
						&cSourceType, 
						&lSourceID, 
						&lEID,
						&lFunctionID
						);

	if (lEID)
	{
		// EID�� �ִٸ�(Global ID���), ID�� Event�� ��������,
		pstEvent = GetEvent(lEID);
	}
	else if (cSourceType && lSourceID)
	{
		// �ƴϸ�, Source Type�� Source ID�� Event�� �����´�.
		pstEvent = GetEvent( (ApBaseType) cSourceType, lSourceID, lFunctionID );
	}

	return pstEvent;
}

PVOID			ApmEventManager::MakePacketEventData(ApdEvent *pcsEvent, ApdEventFunction eFunction, INT16 *pnPacketLength)
{
	if (!pcsEvent ||
		!pnPacketLength ||
		eFunction <= APDEVENT_FUNCTION_NONE ||
		eFunction >= APDEVENT_MAX_FUNCTION)
		return NULL;

	PVOID	pvPacketBase	= MakeBasePacket(pcsEvent);
	if (!pvPacketBase)
		return NULL;

	PVOID	pvPacketCustomData	= NULL;

	if (m_fpMakePacketCallback[eFunction])
	{
		m_fpMakePacketCallback[eFunction](pcsEvent, m_pcsModule[eFunction], &pvPacketCustomData);
	}

	PVOID	pvPacket		= m_csPacketEvent.MakePacket(TRUE, pnPacketLength, AGPMEVENT_MANAGER,
														pvPacketBase,
														pvPacketCustomData);

	m_csPacket.FreePacket(pvPacketCustomData);
	m_csPacket.FreePacket(pvPacketBase);

	return	pvPacket;
}

BOOL			ApmEventManager::SetCallbackInitObject(ApModuleDefaultCallBack fnCallback, PVOID pvClass)
{
	return SetCallback(APMEVENT_CB_INIT_OBJECT, fnCallback, pvClass);
}

BOOL			ApmEventManager::SetCallbackAddEvent(ApModuleDefaultCallBack fnCallback, PVOID pvClass)
{
	return SetCallback(APMEVENT_CB_ADD_EVENT, fnCallback, pvClass);
}

BOOL	ApmEventManager::CheckValidRange(ApdEvent *pcsEvent, AuPOS *pcsPos, INT32 lRange, AuPOS *pcsDestPos)
{
	if (!pcsEvent || !pcsPos)
		return FALSE;

	AuPOS	*pcsEventBasePos = GetBasePos(pcsEvent->m_pcsSource, NULL);
	if (!pcsEventBasePos)
		return FALSE;

	return m_pcsAgpmFactors->IsInRange(pcsPos, pcsEventBasePos, lRange, 0, pcsDestPos);
}

BOOL			ApmEventManager::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pvPacket || nSize < 1)
		return FALSE;

	PVOID	pvPacketBase		= NULL;
	PVOID	pvPacketCustomData	= NULL;

	m_csPacketEvent.GetField(TRUE, pvPacket, nSize,
								&pvPacketBase,
								&pvPacketCustomData);

	if (!pvPacketBase)
		return FALSE;

	INT8					cSourceType		= -1;
	INT32					lSourceID		= 0;
	INT32					lEID			= 0;
	ApdEventFunction		lFunctionID		= APDEVENT_FUNCTION_NONE;

	m_csPacket.GetField(FALSE, pvPacketBase, 0, 
						&cSourceType, 
						&lSourceID, 
						&lEID,
						&lFunctionID);

	ApBase		*pcsBase	= GetBase((ApBaseType) cSourceType, lSourceID);
	if (!pcsBase || !pcsBase->m_Mutex.WLock())
		return FALSE;

	ApdEvent	*pcsEvent	= AddEvent(pcsBase, (ApdEventFunction) lFunctionID);
	
	if (pcsEvent &&	pvPacketCustomData && m_fpReceivePacketCallback[lFunctionID])
	{
		m_fpReceivePacketCallback[lFunctionID](pcsEvent, m_pcsModule[lFunctionID], pvPacketCustomData);
	}

	if (pcsEvent)
		EnumCallback(APMEVENT_CB_ADD_EVENT, pcsEvent, pcsBase);

	pcsBase->m_Mutex.Release();

	return TRUE;
}

BOOL ApmEventManager::RegisterPacketFunction(ApModuleDefaultCallBack fpMakePacketCallback,
											 ApModuleDefaultCallBack fpReceivePacketCallback,
											 ApModule *pcsModule,
											 ApdEventFunction eFunction)
{
	if (eFunction <= APDEVENT_FUNCTION_NONE ||
		eFunction >= APDEVENT_MAX_FUNCTION)
		return FALSE;

	m_fpMakePacketCallback[eFunction]		= fpMakePacketCallback;
	m_fpReceivePacketCallback[eFunction]	= fpReceivePacketCallback;
	m_pcsModule[eFunction]	= pcsModule;

	return TRUE;
}