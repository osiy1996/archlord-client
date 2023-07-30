/******************************************************************************
Module:  ApModule.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 04. 02
******************************************************************************/

#include <ApModule/ApModule.h>
#include "ApModuleManager.h"
#include <ApMemoryTracker/ApMemoryTracker.h>

// constructor
///////////////////////////////////////////////////////////////////////////////
ApModule::ApModule()
{
	m_nAttachedDataNumber = 0;
	m_stAttachedData = new stATTACHEDDATA[APMODULE_MAX_MODULE_NUMBER];
	memset(m_stAttachedData, 0, sizeof(stATTACHEDDATA) * ( APMODULE_MAX_MODULE_NUMBER ));
	memset(m_nDataSize, 0, sizeof(INT32) * APMODULE_MAX_MODULE_NUMBER);

	for(int i=0;i<APMODULE_MAX_MODULE_NUMBER;++i)
	{
		m_listConstructor[i] = NULL;
		m_listDestructor[i] = NULL;
		m_listStream[i] = NULL;
	}

	for(int i = 0;i<APMODULE_MAX_CALLBACK_POINT;++i)
	{
		m_listCallback[i] = NULL;
	}

	m_listPreRecieveModule  = NULL;

	ZeroMemory(m_szName, sizeof(CHAR) * APMODULE_MAX_MODULE_NAME);

	//m_ulPacketType	= 0						;

	ZeroMemory(m_ulPacketType, sizeof(UINT32) * APMODULE_MAX_MODULEPACKET_TYPE);

	m_nIdleEvent	= 0						;
	m_nIdleEvent2	= 0						;
	m_nIdleEvent3	= 0						;

	m_nModuleType	= APMODULE_TYPE_PUBLIC	;
	m_bUseIdle		= FALSE					;
	m_bUseIdle2		= FALSE					;
	m_bUseIdle3		= FALSE					;
	m_pModuleManager= NULL					;
	m_bDynamicAllocated	= FALSE				;

	m_eModuleStatus	= APMODULE_STATUS_INIT	;
}

// destructor
///////////////////////////////////////////////////////////////////////////////
ApModule::~ApModule()
{
	delete []m_stAttachedData;

	for(int i=0;i<APMODULE_MAX_MODULE_NUMBER;++i)
	{
		ReleaseCallBackDataList(m_listConstructor[i]);
		m_listConstructor[i] = NULL;

		ReleaseCallBackDataList(m_listDestructor[i]);
		m_listDestructor[i] = NULL;

		ReleaseStreamDataList(m_listStream[i]);
		m_listStream[i] = NULL;
	}

	for(int i = 0;i<APMODULE_MAX_CALLBACK_POINT;++i)
	{
		ReleaseCallBackDataList(m_listCallback[i]);
		m_listCallback[i] = NULL;
	}

	PreProcessPacket*	cur_data = m_listPreRecieveModule;
	PreProcessPacket*	remove_data;
	while(cur_data)
	{
		remove_data = cur_data;
		cur_data = cur_data->next;

		delete remove_data;
	}
}

void ApModule::ReleaseCallBackDataList(CallBackData* node)
{
	if(node)
	{
		CallBackData*	cur_data = node;
		CallBackData*	remove_data;
		while(cur_data)
		{
			remove_data = cur_data;
			cur_data = cur_data->next;

			delete remove_data;
		}
	}
}

void ApModule::ReleaseStreamDataList(StreamData* node)
{
	if(node)
	{
		StreamData*		cur_data = node;
		StreamData*		remove_data;
		while(cur_data)
		{
			remove_data = cur_data;
			cur_data = cur_data->next;

			delete remove_data;
		}
	}
}

//		AddModule
//	Functions
//		- module add in global module pool
//	Arguments
//		- none
//	Return value
//		- BOOL : success or fail
///////////////////////////////////////////////////////////////////////////////
BOOL ApModule::AddModule(PVOID pModuleManager, BOOL bDynamicAllocated)
{
	if (!m_szName[0])
	{
		MessageBox(NULL, GetModuleName(), "ApModyle::AddModule Name Error", MB_OK);
		return FALSE;
	}

	m_pModuleManager = pModuleManager;
	m_bDynamicAllocated = bDynamicAllocated;

	if( ((ApModuleManager *) m_pModuleManager)->AddModule(this) )
	{
		// call OnAddModule()
		if (!OnAddModule())
		{
			// �׷��� �����ڿ� ���� �ƴ��̻� �޽����ڽ��� �߸� �ȵŴµ�..
			// �ڵ�ȭ ������ �ְ��ϴ� ���ϸ� ����
			//MessageBox(NULL, GetModuleName(), "ApModyle::AddModule Add Error", MB_OK);
			TRACE("ApModule::AddModule() OnAddModule Failed - %s\n", GetModuleName());

			// ���� ���� ���� ���� ���x��
			// �����忡 ���λ����� ���ʵ� �ϴ� �ϴ� ��ġ..
			// return FALSE;
		}

		return TRUE;
	}

	TRACE("ApModule::AddModule() Add Failed - %s\n", GetModuleName());

	return FALSE;
}

//		OnInit (virtual function)
//	Functions
//		- module specific start routine
//	Arguments
//		- none
//	Return value
//		- BOOL : module specific
//  Modify
//      - 120502(Bob, �����Լ�)
///////////////////////////////////////////////////////////////////////////////
//BOOL ApModule::OnInit()
//{
//return TRUE;
//}

//		OnDestroy() (virtual function)
//	Functions
//		- module specific destroy routine
//	Arguments
//		- none
//	Return value
//		- BOOL : module specific
///////////////////////////////////////////////////////////////////////////////
BOOL ApModule::OnPreDestroy()
{
	return TRUE;
}

BOOL ApModule::OnDestroy()
{
	return TRUE;
}

//		GetModuleName
//	Functions
//		- get (this)module name
//	Arguments
//		- none
//	Return value
//		- CHAR* : module name string pointer
///////////////////////////////////////////////////////////////////////////////
CHAR* ApModule::GetModuleName()
{
	return m_szName;
}

//		EnumCallback
//	Functions
//		- call registered callback function
//	Arguments
//		- nCallbackID : callback id for function call
//		- pData : callback function parameter
//	Return value
//		- BOOL : success or fail
///////////////////////////////////////////////////////////////////////////////
BOOL ApModule::EnumCallback(UINT32 nCallbackID, PVOID pData, PVOID pCustData)
{
	//PROFILE("ApModule::EnumCallback");

	if (nCallbackID >= (UINT32) APMODULE_MAX_CALLBACK_POINT)
	{
		// error. callback id is not exist...
		return FALSE;
	}

	BOOL	bRet = TRUE;

	CallBackData*	pCB = m_listCallback[nCallbackID];
	while(pCB)
	{
		//@{ Jaewon 20050428
		// A temporary expedient...--
		if(pCB->pFunc != NULL && !IsBadCodePtr((FARPROC)(pCB->pFunc)))
		//@} Jaewon
		{
			bRet &= pCB->pFunc(pData,pCB->pClass,pCustData);
		}

		pCB = pCB->next;
	}

	// ������ (2003-10-21 ���� 9:09:42) : ������ ����.
	// �޺��� ��� ������ �ִ��� , �ٸ� �ݺ����� ���������� �����´�.
	// ������ ������ ������ ��.
	return bRet;
}

//		GetParentModule
//	Functions
//		- get parent module (stATTACHEDDATA.pParentModule)
//	Arguments
//		- nIndex : index of stATTACHEDDATA array
//	Return value
//		- ApModule* : parent module
//		- NULL : search failed....
///////////////////////////////////////////////////////////////////////////////

//ApModule:* ApModule::GetParentModule(INT16 nIndex)
//{
//	if (nIndex >= APMODULE_MAX_MODULE_NUMBER)
//		return NULL;
//
//	return (ApModule: *) m_stAttachedData[nIndex].pParentModule;
//}


//		SetAttachedModuleData
//	Functions
//		- get parameter (attached module data)
//		- attach module data
//	Arguments
//			pClass : class pointer
//			nDataIndex : module data index
//			nDataSize : attached data size
//			pfConstructor : constructor function
//			pfDestructor : destructor function
//	Return value
//		- INT16 : index number of attached module list
///////////////////////////////////////////////////////////////////////////////
INT16 ApModule::SetAttachedModuleData(PVOID pClass, INT16 nDataIndex, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor)
{
	if( nDataIndex >= APMODULE_MAX_MODULE_NUMBER || nDataIndex < 0 )
		return -1;

#ifndef _WIN64
	nDataSize = APMODULE_ALIGN_4BYTE(nDataSize);
#else
	nDataSize = nDataSize;
#endif

	if (m_nAttachedDataNumber >= APMODULE_MAX_MODULE_NUMBER || m_nAttachedDataNumber < 0
		|| nDataIndex >= APMODULE_MAX_MODULE_NUMBER || nDataIndex < 0)
		return -1;

	m_stAttachedData[m_nAttachedDataNumber].pAttachedModule = pClass;
	m_stAttachedData[m_nAttachedDataNumber].nDataOffset = m_nDataSize[nDataIndex];
	m_stAttachedData[m_nAttachedDataNumber].nDataIndex = nDataIndex;
	m_stAttachedData[m_nAttachedDataNumber].nDataSize = nDataSize;

	AddDataSize(m_stAttachedData[m_nAttachedDataNumber].nDataSize, nDataIndex);

	CallBackData*	find_data;

	CallBackData*	nw_Constructor = new CallBackData;
	nw_Constructor->pClass = pClass;
	nw_Constructor->pFunc = pfConstructor;
	nw_Constructor->next = NULL;

	if(m_listConstructor[nDataIndex] == NULL)
	{
		m_listConstructor[nDataIndex] = nw_Constructor;
	}
	else		// �ǵ� add
	{
		find_data = m_listConstructor[nDataIndex];
		while(find_data->next)
		{
			find_data = find_data->next;
		}
	
		find_data->next = nw_Constructor;
	}

	CallBackData*	nw_Destructor = new CallBackData;
	nw_Destructor->pClass = pClass;
	nw_Destructor->pFunc = pfDestructor;
	nw_Destructor->next = NULL;

	if(m_listDestructor[nDataIndex] == NULL)
	{
		m_listDestructor[nDataIndex] = nw_Destructor;
	}
	else		// �ǵ� add
	{
		find_data = m_listDestructor[nDataIndex];
		while(find_data->next)
		{
			find_data = find_data->next;
		}
	
		find_data->next = nw_Destructor;
	}

	++m_nAttachedDataNumber;

	return (m_nAttachedDataNumber - 1);
}

PVOID ApModule::GetAttachedModuleData(INT16 nIndex, PVOID pData)
{
	// pData �� NULL �̸� return NULL �߰� 2007.07.20. steeple
	if (!pData || nIndex >= APMODULE_MAX_MODULE_NUMBER || nIndex < 0)
		return NULL;

	return (((CHAR *) pData) + m_stAttachedData[nIndex].nDataOffset);
}


//		GetAttachedModuleData
//	Functions
//		- get attached module data (in parent module)
//	Arguments
//		- nIndex : m_stAttachedData index
//		- pData : ���� ����Ÿ ������
//		- nDataIndex : parent module data indexs
//	Return value
//		- PVOID : attached module data pointer
//		- NULL : get failed...
///////////////////////////////////////////////////////////////////////////////

// ������ (2003-11-14 ���� 11:01:18) : �ζ��� ������� �����ؼ� ����� �̵�!..

//PVOID ApModule::GetAttachedModuleData(INT16 nIndex, PVOID pData)
//{
//	if (nIndex >= APMODULE_MAX_MODULE_NUMBER)
//		return NULL;
//
//	return (((CHAR *) pData) + m_stAttachedData[nIndex].nDataOffset);
//}

//		GetParentModuleData
//	Functions
//		- get parent module data (in child module)
//	Arguments
//		- nIndex : m_stAttachedData index
//		- pData : �ٿ��� ����Ÿ ������
//		- nDataIndex : parent module data indexs
//	Return value
//		- PVOID : child module data pointer
//		- NULL : get failed...
///////////////////////////////////////////////////////////////////////////////
PVOID ApModule::GetParentModuleData(INT16 nIndex, PVOID pData)
{
	if (nIndex >= APMODULE_MAX_MODULE_NUMBER || nIndex < 0)
		return NULL;

	return (((CHAR *) pData) - m_stAttachedData[nIndex].nDataOffset);
}

INT32 ApModule::GetDataSize(INT16 nDataIndex)
{
	if( nDataIndex >= APMODULE_MAX_MODULE_NUMBER || nDataIndex < 0 )
		return 0;

	return m_nDataSize[ nDataIndex ];
}

INT32 ApModule::AddDataSize(INT32 nDataSize, INT16 nDataIndex)
{
	if( nDataIndex >= APMODULE_MAX_MODULE_NUMBER || nDataIndex < 0 )
		return 0;

	m_nDataSize[ nDataIndex ] += nDataSize;
	return m_nDataSize[ nDataIndex ];
}

//		SetModuleData
//	Functions
//		- set default module data size
//	Arguments
//		- nSize : data size
//		- nDataIndex : data index
//	Return value
//		- BOOL : success or fail
///////////////////////////////////////////////////////////////////////////////
BOOL ApModule::SetModuleData(INT32 nSize, INT16 nDataIndex)
{
	if( nDataIndex >= APMODULE_MAX_MODULE_NUMBER || nDataIndex < 0 )
		return FALSE;

#ifndef _WIN64
	m_nDataSize[nDataIndex] = APMODULE_ALIGN_4BYTE(nSize);
#else
	m_nDataSize[nDataIndex] = nSize;
#endif

	return TRUE;
}

//		SetCallback
//	Functions
//		- register callback function
//	Arguments
//		- nCallbackID : callback id
//		- pfCallback : callback function pointer
//		- pClass : callback owner class pointer
//	Return value
//		- BOOL : success or fail
///////////////////////////////////////////////////////////////////////////////
BOOL ApModule::SetCallback(INT16 nCallbackID, ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	if (nCallbackID >= APMODULE_MAX_CALLBACK_POINT || nCallbackID < 0)
		return FALSE;

	CallBackData*	nw_CB = new CallBackData;
	nw_CB->pFunc = pfCallback;
	nw_CB->pClass = pClass;
	nw_CB->next = NULL;

	if(m_listCallback[nCallbackID] == NULL)
	{
		m_listCallback[nCallbackID] = nw_CB;
	}
	else		// �ǵ� add
	{
		CallBackData* find_data = m_listCallback[nCallbackID];
		while(find_data->next)
		{
			find_data = find_data->next;
		}

		find_data->next = nw_CB;
	}
	
	return TRUE;
}

BOOL ApModule::AddStreamCallback(INT16 nDataIndex, ApModuleStreamReadCallBack pfReadCallback, ApModuleStreamWriteCallBack pfWriteCallback, ApModule *pClass)
{
	if (nDataIndex >= APMODULE_MAX_MODULE_NUMBER || nDataIndex < 0)
		return FALSE;

	StreamData*	nw_SD = new StreamData;
	nw_SD->pReadFunc = pfReadCallback;
	nw_SD->pWriteFunc = pfWriteCallback;
	nw_SD->pModule = pClass;
	nw_SD->next = NULL;

	if(m_listStream[nDataIndex] == NULL)
	{
		m_listStream[nDataIndex] = nw_SD;
	}
	else		// �ǵ� add
	{
		StreamData* find_data = m_listStream[nDataIndex];
		while(find_data->next)
		{
			find_data = find_data->next;
		}
		
		find_data->next = nw_SD;
	}

	return TRUE;
}

//		GetModule
//	Functions
//		- get module pointer
//	Arguments
//		- szModuleName : search module name
//	Return value
//		- NULL : search failed
//		- ApModule:* : searched module pointer
///////////////////////////////////////////////////////////////////////////////
ApModule* ApModule::GetModule(CHAR* szModuleName)
{
	
//	for (int i = 0; i < APMODULE_MAX_MODULE_NUMBER; ++i)
//	{
//		if (g_pModulePool[i] && !strcmp(szModuleName, g_pModulePool[i]->GetModuleName()))
//		{
//			return g_pModulePool[i];
//		}
//	}
	

	return ((ApModuleManager *) m_pModuleManager)->GetModule(szModuleName);
}

BOOL ApModule::Idle(UINT32 ulClockCount)
{
	INT32	lIndex = 0;

	m_MutexIdleEvent.WLock();
	pstMODULEIDLEEVENT pstIdleEvent = GetIdleEvent(ulClockCount, &lIndex);
	while (pstIdleEvent)
	{
		INT32	lCID	= pstIdleEvent->lCID;
		PVOID	pClass	= pstIdleEvent->pClass;
		PVOID	pvData	= pstIdleEvent->pvData;
		BOOL	(*pfnProcessFunc) (INT32, PVOID, UINT32, PVOID)	= pstIdleEvent->pfnProcessFunc;

		m_csIdleEvent.DeletePrev(lIndex);

		m_MutexIdleEvent.Release();
		
		// call Idle event process function
		pfnProcessFunc(lCID, pClass, ulClockCount, pvData);

		m_MutexIdleEvent.WLock();

		pstIdleEvent = GetIdleEvent(ulClockCount, &lIndex);
	}

	m_MutexIdleEvent.Release();

	return OnIdle(ulClockCount);
}

BOOL ApModule::Idle2(UINT32 ulClockCount)
{
	INT32	lIndex = 0;

	m_MutexIdleEvent2.WLock();
	pstMODULEIDLEEVENT pstIdleEvent = GetIdleEvent2(ulClockCount, &lIndex);
	while (pstIdleEvent)
	{
		INT32	lCID	= pstIdleEvent->lCID;
		PVOID	pClass	= pstIdleEvent->pClass;
		PVOID	pvData	= pstIdleEvent->pvData;
		BOOL	(*pfnProcessFunc) (INT32, PVOID, UINT32, PVOID)	= pstIdleEvent->pfnProcessFunc;

		m_csIdleEvent2.DeletePrev(lIndex);

		m_MutexIdleEvent2.Release();

		// call Idle event process function
		pfnProcessFunc(lCID, pClass, ulClockCount, pvData);

		m_MutexIdleEvent2.WLock();

		pstIdleEvent = GetIdleEvent2(ulClockCount, &lIndex);
	}

	m_MutexIdleEvent2.Release();

	return OnIdle2(ulClockCount);
}

BOOL ApModule::Idle3(UINT32 ulClockCount)
{
	INT32	lIndex = 0;

	m_MutexIdleEvent3.WLock();
	pstMODULEIDLEEVENT pstIdleEvent = GetIdleEvent2(ulClockCount, &lIndex);
	while (pstIdleEvent)
	{
		INT32	lCID	= pstIdleEvent->lCID;
		PVOID	pClass	= pstIdleEvent->pClass;
		PVOID	pvData	= pstIdleEvent->pvData;
		BOOL	(*pfnProcessFunc) (INT32, PVOID, UINT32, PVOID)	= pstIdleEvent->pfnProcessFunc;

		m_csIdleEvent3.DeletePrev(lIndex);

		m_MutexIdleEvent3.Release();

		// call Idle event process function
		pfnProcessFunc(lCID, pClass, ulClockCount, pvData);

		m_MutexIdleEvent3.WLock();

		pstIdleEvent = GetIdleEvent2(ulClockCount, &lIndex);
	}

	m_MutexIdleEvent3.Release();

	return OnIdle3(ulClockCount);
}

//		AddIdleEvent
//	Functions
//		- add idle event
//	Arguments
//		- ulClockCount : �� �̺�Ʈ�� ó���� �ð�
//		- lCID : ó���ؾ��� ĳ����
//		- pClass : ó���� Ŭ����
//		- pfnProcessFunc : ���μ��� �Լ� ������
//	Return value
//		- BOOL : ��������
///////////////////////////////////////////////////////////////////////////////
BOOL ApModule::AddIdleEvent(UINT32 ulClockCount, INT32 lCID, PVOID pClass, 
							BOOL (*pfnProcessFunc) (INT32 lCID, PVOID pClass, UINT32 ulClockCount, PVOID pvData),
							PVOID pvData)
{
	stMODULEIDLEEVENT	stIdleEvent;

	stIdleEvent.ulClockCount = ulClockCount;
	stIdleEvent.lCID = lCID;
	stIdleEvent.pClass = pClass;
	stIdleEvent.pfnProcessFunc = pfnProcessFunc;
	stIdleEvent.pvData = pvData;

	m_MutexIdleEvent.WLock();

	INT16	nRetval = m_csIdleEvent.Add(&stIdleEvent);

	m_MutexIdleEvent.Release();

	if (nRetval < 0)
		return FALSE;

	return TRUE;
}

//		AddIdleEvent2
//	Functions
//		- add idle event
//	Arguments
//		- ulClockCount : �� �̺�Ʈ�� ó���� �ð�
//		- lCID : ó���ؾ��� ĳ����
//		- pClass : ó���� Ŭ����
//		- pfnProcessFunc : ���μ��� �Լ� ������
//	Return value
//		- BOOL : ��������
///////////////////////////////////////////////////////////////////////////////
BOOL ApModule::AddIdleEvent2(UINT32 ulClockCount, INT32 lCID, PVOID pClass, 
							BOOL (*pfnProcessFunc) (INT32 lCID, PVOID pClass, UINT32 ulClockCount, PVOID pvData),
							PVOID pvData)
{
	stMODULEIDLEEVENT	stIdleEvent;

	stIdleEvent.ulClockCount = ulClockCount;
	stIdleEvent.lCID = lCID;
	stIdleEvent.pClass = pClass;
	stIdleEvent.pfnProcessFunc = pfnProcessFunc;
	stIdleEvent.pvData = pvData;

	m_MutexIdleEvent2.WLock();

	INT16	nRetval = m_csIdleEvent2.Add(&stIdleEvent);

	m_MutexIdleEvent2.Release();

	if (nRetval < 0)
		return FALSE;

	return TRUE;
}

//		AddIdleEvent3
//	Functions
//		- add idle event
//	Arguments
//		- ulClockCount : �� �̺�Ʈ�� ó���� �ð�
//		- lCID : ó���ؾ��� ĳ����
//		- pClass : ó���� Ŭ����
//		- pfnProcessFunc : ���μ��� �Լ� ������
//	Return value
//		- BOOL : ��������
///////////////////////////////////////////////////////////////////////////////
BOOL ApModule::AddIdleEvent3(UINT32 ulClockCount, INT32 lCID, PVOID pClass, 
							BOOL (*pfnProcessFunc) (INT32 lCID, PVOID pClass, UINT32 ulClockCount, PVOID pvData),
							PVOID pvData)
{
	stMODULEIDLEEVENT	stIdleEvent;

	stIdleEvent.ulClockCount = ulClockCount;
	stIdleEvent.lCID = lCID;
	stIdleEvent.pClass = pClass;
	stIdleEvent.pfnProcessFunc = pfnProcessFunc;
	stIdleEvent.pvData = pvData;

	m_MutexIdleEvent3.WLock();

	INT16	nRetval = m_csIdleEvent3.Add(&stIdleEvent);

	m_MutexIdleEvent3.Release();

	if (nRetval < 0)
		return FALSE;

	return TRUE;
}

//		RemoveIdleEvent
//	Functions
//		- remove idle event
//	Arguments
//		- ulCID : ���� ĳ���� �Ƶ�
//	Return value
//		- BOOL : ��������
///////////////////////////////////////////////////////////////////////////////
BOOL ApModule::RemoveIdleEvent(INT32 lCID)
{
	INT32	lIndex = 0;

	BOOL	bRetval = FALSE;

	HANDLE	hCurrentThread = GetCurrentThread();

	m_MutexIdleEvent.WLock();

	pstMODULEIDLEEVENT pstIdleEvent = (pstMODULEIDLEEVENT) m_csIdleEvent.GetSequence(&lIndex);
	while (pstIdleEvent)
	{
		if (pstIdleEvent->lCID == lCID)
		{
			bRetval = m_csIdleEvent.Delete(lIndex);
			break;
		}

		pstIdleEvent = (pstMODULEIDLEEVENT) m_csIdleEvent.GetSequence(&lIndex);
	}

	m_MutexIdleEvent.Release();

	return bRetval;
}

//		RemoveIdleEvent2
//	Functions
//		- remove idle event
//	Arguments
//		- ulCID : ���� ĳ���� �Ƶ�
//	Return value
//		- BOOL : ��������
///////////////////////////////////////////////////////////////////////////////
BOOL ApModule::RemoveIdleEvent2(INT32 lCID)
{
	INT32	lIndex = 0;

	BOOL	bRetval = FALSE;

	HANDLE	hCurrentThread = GetCurrentThread();

	m_MutexIdleEvent2.WLock();

	pstMODULEIDLEEVENT pstIdleEvent = (pstMODULEIDLEEVENT) m_csIdleEvent2.GetSequence(&lIndex);
	while (pstIdleEvent)
	{
		if (pstIdleEvent->lCID == lCID)
		{
			bRetval = m_csIdleEvent2.Delete(lIndex);
			break;
		}

		pstIdleEvent = (pstMODULEIDLEEVENT) m_csIdleEvent2.GetSequence(&lIndex);
	}

	m_MutexIdleEvent2.Release();

	return bRetval;
}

//		RemoveIdleEvent3
//	Functions
//		- remove idle event
//	Arguments
//		- ulCID : ���� ĳ���� �Ƶ�
//	Return value
//		- BOOL : ��������
///////////////////////////////////////////////////////////////////////////////
BOOL ApModule::RemoveIdleEvent3(INT32 lCID)
{
	INT32	lIndex = 0;

	BOOL	bRetval = FALSE;

	HANDLE	hCurrentThread = GetCurrentThread();

	m_MutexIdleEvent3.WLock();

	pstMODULEIDLEEVENT pstIdleEvent = (pstMODULEIDLEEVENT) m_csIdleEvent3.GetSequence(&lIndex);
	while (pstIdleEvent)
	{
		if (pstIdleEvent->lCID == lCID)
		{
			bRetval = m_csIdleEvent3.Delete(lIndex);
			break;
		}

		pstIdleEvent = (pstMODULEIDLEEVENT) m_csIdleEvent3.GetSequence(&lIndex);
	}

	m_MutexIdleEvent3.Release();

	return bRetval;
}

//		GetIdleEvent
//	Functions
//		- get idle event (ulClockCount���� ���� ���� ���� �̺�Ʈ�� ��� �����´�)
//	Arguments
//		- ulClockCount : current clock count
//	Return value
//		- pstMODULEIDLEEVENT : ó���ؾ��� event
///////////////////////////////////////////////////////////////////////////////
pstMODULEIDLEEVENT ApModule::GetIdleEvent(UINT32 ulClockCount, INT32 *plIndex)
{
	pstMODULEIDLEEVENT pstIdleEvent = (pstMODULEIDLEEVENT) m_csIdleEvent.GetSequence(plIndex);
	while (pstIdleEvent)
	{
		if (ulClockCount >= pstIdleEvent->ulClockCount)
			return pstIdleEvent;

		pstIdleEvent = (pstMODULEIDLEEVENT) m_csIdleEvent.GetSequence(plIndex);
	}

	return NULL;
}

//		GetIdleEvent2
//	Functions
//		- get idle event (ulClockCount���� ���� ���� ���� �̺�Ʈ�� ��� �����´�)
//	Arguments
//		- ulClockCount : current clock count
//	Return value
//		- pstMODULEIDLEEVENT : ó���ؾ��� event
///////////////////////////////////////////////////////////////////////////////
pstMODULEIDLEEVENT ApModule::GetIdleEvent2(UINT32 ulClockCount, INT32 *plIndex)
{
	pstMODULEIDLEEVENT pstIdleEvent = (pstMODULEIDLEEVENT) m_csIdleEvent2.GetSequence(plIndex);
	while (pstIdleEvent)
	{
		if (ulClockCount >= pstIdleEvent->ulClockCount)
			return pstIdleEvent;

		pstIdleEvent = (pstMODULEIDLEEVENT) m_csIdleEvent2.GetSequence(plIndex);
	}

	return NULL;
}

//		GetIdleEvent3
//	Functions
//		- get idle event (ulClockCount���� ���� ���� ���� �̺�Ʈ�� ��� �����´�)
//	Arguments
//		- ulClockCount : current clock count
//	Return value
//		- pstMODULEIDLEEVENT : ó���ؾ��� event
///////////////////////////////////////////////////////////////////////////////
pstMODULEIDLEEVENT ApModule::GetIdleEvent3(UINT32 ulClockCount, INT32 *plIndex)
{
	pstMODULEIDLEEVENT pstIdleEvent = (pstMODULEIDLEEVENT) m_csIdleEvent3.GetSequence(plIndex);
	while (pstIdleEvent)
	{
		if (ulClockCount >= pstIdleEvent->ulClockCount)
			return pstIdleEvent;

		pstIdleEvent = (pstMODULEIDLEEVENT) m_csIdleEvent3.GetSequence(plIndex);
	}

	return NULL;
}

//		SetMaxIdleEvent
//	Functions
//		- idle event�� ������ �迭�� ũ�⸦ �����Ѵ�. (������ �迭�� �ʱ�ȭ�Ѵ�)
//	Arguments
//		- nCount : �迭 ũ��
//	Return value
//		- BOOL : ���� ���
///////////////////////////////////////////////////////////////////////////////
BOOL ApModule::SetMaxIdleEvent(INT16 nCount)
{
	m_nIdleEvent = nCount;

	return m_csIdleEvent.Initialize(nCount, sizeof(stMODULEIDLEEVENT));
}

//		SetMaxIdleEvent
//	Functions
//		- idle event�� ������ �迭�� ũ�⸦ �����Ѵ�. (������ �迭�� �ʱ�ȭ�Ѵ�)
//	Arguments
//		- nCount : �迭 ũ��
//	Return value
//		- BOOL : ���� ���
///////////////////////////////////////////////////////////////////////////////
BOOL ApModule::SetMaxIdleEvent2(INT16 nCount)
{
	m_nIdleEvent2 = nCount;

	return m_csIdleEvent2.Initialize(nCount, sizeof(stMODULEIDLEEVENT));
}

//		SetMaxIdleEvent3
//	Functions
//		- idle event�� ������ �迭�� ũ�⸦ �����Ѵ�. (������ �迭�� �ʱ�ȭ�Ѵ�)
//	Arguments
//		- nCount : �迭 ũ��
//	Return value
//		- BOOL : ���� ���
///////////////////////////////////////////////////////////////////////////////
BOOL ApModule::SetMaxIdleEvent3(INT16 nCount)
{
	m_nIdleEvent3 = nCount;

	return m_csIdleEvent3.Initialize(nCount, sizeof(stMODULEIDLEEVENT));
}

//		GetMaxIdleEvent
//	Functions
//		- idle event�� �����ϴ� �迭�� ũ�⸦ �����´�.
//	Arguments
//		- none
//	Return value
//		- INT16 : �迭 ũ��
///////////////////////////////////////////////////////////////////////////////
INT16 ApModule::GetMaxIdleEvent()
{
	return m_nIdleEvent;
}

//		GetMaxIdleEvent2
//	Functions
//		- idle event�� �����ϴ� �迭�� ũ�⸦ �����´�.
//	Arguments
//		- none
//	Return value
//		- INT16 : �迭 ũ��
///////////////////////////////////////////////////////////////////////////////
INT16 ApModule::GetMaxIdleEvent2()
{
	return m_nIdleEvent2;
}

//		GetMaxIdleEvent3
//	Functions
//		- idle event�� �����ϴ� �迭�� ũ�⸦ �����´�.
//	Arguments
//		- none
//	Return value
//		- INT16 : �迭 ũ��
///////////////////////////////////////////////////////////////////////////////
INT16 ApModule::GetMaxIdleEvent3()
{
	return m_nIdleEvent2;
}

BOOL ApModule::m_fConstructorCB(PVOID pData, PVOID pClass, INT16 nDataIndex)
{
	ApModule *pThis = (ApModule *) pClass;

	return pThis->EnumConstructor(nDataIndex, pData, NULL);
}

BOOL ApModule::m_fDestructorCB(PVOID pData, PVOID pClass, INT16 nDataIndex)
{
	ApModule *pThis = (ApModule *) pClass;

	return pThis->EnumDestructor(nDataIndex, pData, NULL);
}

BOOL ApModule::EnumConstructor(INT16 nDataIndex, PVOID data, PVOID pCustData)
{
	if(nDataIndex < 0 || nDataIndex >= APMODULE_MAX_MODULE_NUMBER)
		return FALSE;

	CallBackData*	cur_data = m_listConstructor[nDataIndex]; 
	while(cur_data)
	{
		if(cur_data->pFunc)
		{
			cur_data->pFunc(data,cur_data->pClass,pCustData);
		}

		cur_data = cur_data->next;
	}

	return TRUE;
}

BOOL ApModule::EnumDestructor(INT16 nDataIndex, PVOID data, PVOID pCustData)
{
	if(nDataIndex < 0 || nDataIndex >= APMODULE_MAX_MODULE_NUMBER)
		return FALSE;

	CallBackData*	cur_data = m_listDestructor[nDataIndex]; 
	while(cur_data)
	{
		if(cur_data->pFunc)
		{
			cur_data->pFunc(data,cur_data->pClass,pCustData);
		}

		cur_data = cur_data->next;
	}
	
	return TRUE;
}

BOOL ApModule::InitMemoryPool(INT32 lDataSize, INT32 lCount, CHAR*	szName)
{
	// lDataSize�� 0�̸� ��ⵥ������ ����� �����ͼ� ����
	// 0�� �ƴϸ� �ο����� ���ڷ� ����� �����Ѵ�.
	if (0 == lDataSize)
	{
		#ifdef _CPPRTTI
			m_csMemoryPool.Initialize(m_nDataSize[0], lCount,szName);
		#else
			m_csMemoryPool.Initialize(m_nDataSize[0], lCount);
		#endif
	}
	else
	{
		#ifdef _CPPRTTI
			m_csMemoryPool.Initialize(lDataSize, lCount,szName);
		#else
			m_csMemoryPool.Initialize(lDataSize, lCount);
		#endif
	}
	
	if (-1 == m_csMemoryPool.GetTypeIndex())
		return FALSE;
	else 
		return TRUE;
}

BOOL ApModule::ExistCallback(INT16 nCallbackID)
{
	if( nCallbackID >= APMODULE_MAX_MODULE_NUMBER || nCallbackID < 0 ) return FALSE;
	return m_listCallback[nCallbackID] ? TRUE : FALSE;
}

PVOID ApModule::CreateModuleData(INT16 nDataIndex, INT16 nCount)
{
	if( nDataIndex >= APMODULE_MAX_MODULE_NUMBER || nDataIndex < 0 )
		return NULL;

	PVOID	pBuffer;
	INT16	nIndex;
	INT32	nDataSize = GetDataSize(nDataIndex);

	if (nDataSize < 1 || nCount <= 0)
		return NULL;

	// �޸� Ǯ���� ���� �Ͽ��ٸ� Ǯ���� ����Ѵ�.
	if ((-1 != m_csMemoryPool.GetTypeIndex()) && (0 == nDataIndex))
	{
		pBuffer = m_csMemoryPool.Alloc();
	}
	else
	{
		pBuffer = (PVOID) new BYTE[nDataSize * nCount];
#ifdef _AP_MEMORY_TRACKER_
		ApMemoryTracker::GetInstance().AddInformation(pBuffer, GetModuleName());
#endif
	}

	if (!pBuffer)
		return NULL;

	ZeroMemory(pBuffer, nDataSize * nCount);

	for (nIndex = 0; nIndex < nCount; ++nIndex)
	{
		EnumConstructor(nDataIndex, (CHAR *) pBuffer + nDataSize * nIndex, NULL);
	}

	return pBuffer;
}

BOOL ApModule::DestroyModuleData(PVOID pData, INT16 nDataIndex)
{
	if( nDataIndex >= APMODULE_MAX_MODULE_NUMBER || nDataIndex < 0 )
		return NULL;

	INT16	nIndex;
	INT32	nDataSize = GetDataSize(nDataIndex);
//	INT32	nAllocatedSize;
	INT16	nCount;
	
	if (!pData || nDataSize < 0)
		return FALSE;

	//nAllocatedSize = (INT32) GlobalSize(pData);
//	nAllocatedSize = (INT32) _msize(pData);
//	if (nAllocatedSize <= 0)
//		return FALSE;
	nCount = 1;

//	nCount = nAllocatedSize / nDataSize;

	for (nIndex = 0; nIndex < nCount; ++nIndex)
	{
		EnumDestructor(nDataIndex, (CHAR *) pData + nDataSize * nIndex, NULL);
	}

	//GlobalFree(pData);
	//delete [] pData;
	// ������ (2004-05-29 ���� 3:40:47) : ��°�� malloc -> delete �ϱ -_-;;
	if ((-1 != m_csMemoryPool.GetTypeIndex()) && (0 == nDataIndex))
		m_csMemoryPool.Free(pData);
	else
		delete [] (BYTE*)( pData );

	return TRUE;
}

BOOL ApModule::SetPacketType(UINT32 ulType)
{
	for (int i = 0; i < APMODULE_MAX_MODULEPACKET_TYPE; ++i)
	{
		if (m_ulPacketType[i] == 0)
		{
			m_ulPacketType[i] = ulType;

			return TRUE;
		}
	}

	return FALSE;
}

/*
UINT32 ApModule::GetPacketType()
{
	return m_ulPacketType;
}
*/

BOOL ApModule::AddPreReceiveModule(ApModule *pcsModule)
{
	if (!pcsModule)
		return FALSE;

	PreProcessPacket*	nw_PP = new PreProcessPacket;
	nw_PP->pModule = pcsModule;
	nw_PP->next = NULL;

	if(m_listPreRecieveModule == NULL)
	{
		m_listPreRecieveModule = nw_PP;
	}
	else
	{
		PreProcessPacket*	find_node = m_listPreRecieveModule;

		while(find_node->next)
		{
			find_node = find_node->next;
		}

		find_node->next = nw_PP;
	}

	return TRUE;
}

BOOL ApModule::EnumPreRecieveModule(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	PreProcessPacket*	cur_data = m_listPreRecieveModule;
	while(cur_data)
	{
		if(cur_data->pModule)
		{
			if(!cur_data->pModule->OnPreReceive(ulType, pvPacket, nSize, ulNID, pstCheckArg))
				return FALSE;
		}
		cur_data = cur_data->next;
	}

	return TRUE;
}

BOOL ApModule::SendPacket(PVOID pvPacket, INT16 nLength, UINT32 ulNID, PACKET_PRIORITY ePriority, INT16 nFlag)
{
	if (!m_pModuleManager)
		return FALSE;

	return ((ApModuleManager *) m_pModuleManager)->SendPacket(pvPacket, nLength, ulNID, ePriority, nFlag);
}

PVOID ApModule::GetModuleManager()
{
	return m_pModuleManager;
}

VOID ApModule::EnableIdle(BOOL bUseIdle)
{
	((ApModuleManager *) m_pModuleManager)->EnableModuleIdle(this, bUseIdle);
}

VOID ApModule::EnableIdle2(BOOL bUseIdle)
{
	((ApModuleManager *) m_pModuleManager)->EnableModuleIdle2(this, bUseIdle);
}

VOID ApModule::EnableIdle3(BOOL bUseIdle)
{
	((ApModuleManager *) m_pModuleManager)->EnableModuleIdle3(this, bUseIdle);
}

BOOL ApModule::SetModuleType(INT16 nModuleType)
{
	m_nModuleType = nModuleType;

	return TRUE;
}

INT16 ApModule::GetModuleType()
{
	return m_nModuleType;
}

BOOL ApModule::SetSelfCID(INT32 lCID)
{
	if (!m_pModuleManager)
		return FALSE;

	return ((ApModuleManager *) m_pModuleManager)->SetSelfCID(lCID);
}

UINT32 ApModule::GetPrevClockCount()
{
	return ((ApModuleManager *) m_pModuleManager)->GetPrevClockCount();
}

UINT32 ApModule::GetClockCount()
{
	return ((ApModuleManager *) m_pModuleManager)->GetClockCount();
}

BOOL ApModule::Receive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	return OnReceive(ulType, pvPacket, nSize, ulNID, pstCheckArg);
}

void ApModule::Report(FILE *fp)
{
	if (fp)
		fprintf(fp, "\n None\n");

	return;
}

void ApModule::Report_Head(FILE *fp)
{
	if (fp)
	{
		fprintf(fp, "\n( -_-)�� (((((((((((((((((((((((((((((��~*\n"
					"\n Module : %s "
					"\n( -_-)�� (((((((((((((((((((((((((((((��~*\n", GetModuleName());
	}
}

