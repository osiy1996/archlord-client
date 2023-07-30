// ApmEventManager.h: interface for the ApmEventManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APMEVENTMANAGER_H__999BBC3E_ED58_4F45_94D4_D2691D7E0226__INCLUDED_)
#define AFX_APMEVENTMANAGER_H__999BBC3E_ED58_4F45_94D4_D2691D7E0226__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApModule.h"

#include "ApaEvent.h"
#include "ApdEvent.h"

#include "ApmMap.h"
#include "ApmObject.h"
#include "AgpmCharacter.h"
#include "AgpmItem.h"
#include "AgpmParty.h"

#include "AuPacket.h"
#include "AuGenerateID.h"

//@{ Jaewon 20041118
#if _MSC_VER < 1300
#pragma message( "ApmEventManager : Visual Studio 6.0 build" )
#ifdef	_DEBUG
#pragma comment ( lib , "ApmEventManagerD" )
#else
#pragma comment ( lib , "ApmEventManager" )
#endif
#endif
//@} Jaewon

typedef BOOL	(*ApmEventConditionProcessor) (PVOID pData, PVOID pvClass, PVOID pvUpdateData);

#define APMEVENT_CONDITION_MAX_TRY	10

#define APMEVENT_FLAG_PACKET_TYPE	0x80

#define APMEVENT_STREAM_NAME_FUNCTION		"Function"
#define APMEVENT_STREAM_NAME_EID			"EventID"

#define APMEVENT_STREAM_NAME_COND_START		"CondStart"
#define APMEVENT_STREAM_NAME_COND_END		"CondEnd"

#define APMEVENT_STREAM_NAME_COND_TYPE		"CondType"

#define APMEVENT_STREAM_NAME_TARGET_ITID	"CondTargetItemTID"
#define APMEVENT_STREAM_NAME_AREA_TYPE			"CondAreaType"
#define APMEVENT_STREAM_NAME_AREA_SPHERE_RADIUS	"CondAreaSphereRadius"
#define APMEVENT_STREAM_NAME_AREA_FAN_RADIUS	"CondAreaFanRadius"
#define APMEVENT_STREAM_NAME_AREA_ANGLE			"CondAreaAngle"
#define APMEVENT_STREAM_NAME_AREA_BOX_INF		"CondAreaBoxInf"
#define APMEVENT_STREAM_NAME_AREA_BOX_SUP		"CondAreaBoxSup"

#define	APMEVENT_MAX_RANGE_TO_ARISE_EVENT	600

typedef enum
{
	APMEVENT_CB_INIT_OBJECT	= 0,
	APMEVENT_CB_ADD_EVENT,
} ApmEventCBPoint;

class ApmEventManager : public ApModule	
{
private:

	ApmMap *				m_pcsApmMap;
	ApmObject *				m_pcsApmObject;
	AgpmGrid *				m_pcsAgpmGrid;
	AgpmFactors *			m_pcsAgpmFactors;
	AgpmCharacter *			m_pcsAgpmCharacter;
	AgpmItem *				m_pcsAgpmItem;
	AgpmParty *				m_pcsAgpmParty;

	ApaEvent				m_csEvents;

	INT32					m_lMaxEvent;

	INT16					m_nMapAttachIndex;
	INT16					m_nObjectTemplateAttachIndex;
	INT16					m_nObjectAttachIndex;
	INT16					m_nCharacterTemplateAttachIndex;
	INT16					m_nCharacterAttachIndex;
	INT16					m_nItemTemplateAttachIndex;
	INT16					m_nItemAttachIndex;

	//INT16					m_nEventDataSize[APDEVENT_MAX_FUNCTION];
//	ApModuleDefaultCallBack	m_fnEventStreamWriter[APDEVENT_MAX_FUNCTION];
//	ApModuleDefaultCallBack	m_fnEventStreamReader[APDEVENT_MAX_FUNCTION];
//	ApModuleDefaultCallBack	m_fnEventConstructor[APDEVENT_MAX_FUNCTION];
//	ApModuleDefaultCallBack	m_fnEventDestructor[APDEVENT_MAX_FUNCTION];
//	ApModule *				m_pacsFunctionModule[APDEVENT_MAX_FUNCTION];
//	ApModuleDefaultCallBack	m_fnEventIdleCallback[APDEVENT_MAX_FUNCTION];

	ApSafeArray<ApModuleDefaultCallBack, APDEVENT_MAX_FUNCTION>		m_fnEventStreamWriter;
	ApSafeArray<ApModuleDefaultCallBack, APDEVENT_MAX_FUNCTION>		m_fnEventStreamReader;
	ApSafeArray<ApModuleDefaultCallBack, APDEVENT_MAX_FUNCTION>		m_fnEventConstructor;
	ApSafeArray<ApModuleDefaultCallBack, APDEVENT_MAX_FUNCTION>		m_fnEventDestructor;
	ApSafeArray<ApModule *, APDEVENT_MAX_FUNCTION>					m_pacsFunctionModule;
	ApSafeArray<ApModuleDefaultCallBack, APDEVENT_MAX_FUNCTION>		m_fnEventIdleCallback;

	AuGenerateID			m_csGenerateEID;

	MTRand					m_csRandom;

public:
	AuPacket				m_csPacket;
	AuPacket				m_csPacketEvent;

//	ApModule					*m_pcsModule[APDEVENT_MAX_FUNCTION];
//	ApModuleDefaultCallBack		m_fpMakePacketCallback[APDEVENT_MAX_FUNCTION];
//	ApModuleDefaultCallBack		m_fpReceivePacketCallback[APDEVENT_MAX_FUNCTION];

	ApSafeArray<ApModule *, APDEVENT_MAX_FUNCTION>					m_pcsModule;
	ApSafeArray<ApModuleDefaultCallBack, APDEVENT_MAX_FUNCTION>		m_fpMakePacketCallback;
	ApSafeArray<ApModuleDefaultCallBack, APDEVENT_MAX_FUNCTION>		m_fpReceivePacketCallback;

public:
	ApmEventManager();
	virtual ~ApmEventManager();

	// Virtual Function ��
	BOOL			OnAddModule();
	BOOL			OnInit();
	BOOL			OnIdle(UINT32 ulClockCount);
	BOOL			OnDestroy();

	BOOL			OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	// �ִ� Event ����
	void			SetMaxEvent(INT32 lEvent);

	// Event Function�� ����Ѵ�.
	BOOL			RegisterEvent(ApdEventFunction eFunction, ApModuleDefaultCallBack fnConstructor, ApModuleDefaultCallBack fnDestructor, 
								  ApModuleDefaultCallBack fnCallback, ApModuleDefaultCallBack fnStreamWriter, ApModuleDefaultCallBack fnStreamReader, PVOID pvClass);

	// Object/Character/Item � Event�� �߰��Ѵ�.
	ApdEvent *		AddEvent(ApdEventAttachData * pcsEvents, ApdEventFunction eFunction, ApBase * pcsSource, BOOL bAddList = TRUE, INT32 lEID = 0, UINT32 ulStartTime = 0);
	ApdEvent *		AddEvent(ApBase *pcsData, ApdEventFunction eFunction);

	BOOL			SetCondition(ApdEvent *pstEvent, ApdEventCondFlag eFlags);
	void			DestroyCondition(ApdEvent *pstEvent);
	BOOL			CopyCondition(ApdEvent *pstDstEvent, ApdEvent *pstSrcEvent);

	BOOL			IsRegistered(ApdEventFunction eFunction);

	// Object/Character/Item���� Event/Base�� �����´�.
	ApdEventAttachData *		GetEventData(ApBase *pcsData);

	ApBase *		GetBase(ApBaseType eBaseType, INT32 lID);
	ApdEvent *		GetEvent(INT32 lEID);
	ApdEvent *		GetEvent(ApBase *pcsBase, ApdEventFunction eFunction);
	ApdEvent *		GetEvent(ApdEventAttachData * pcsEvents, ApdEventFunction eFunction);
	ApdEvent *		GetEvent(ApBaseType eSourceType, INT32 lSourceID, ApdEventFunction eFunction);
	ApdEvent *		GetEventSquence(INT32 *plIndex);

	// ������ (2004-07-05 ���� 5:20:14) : �̺�Ʈ�� �پ� �ִ��� �� Ȯ���Ѵ�.
	BOOL			CheckEventAvailable( ApBase * pcsBase );

	// Event�� Remove�Ѵ�.
	BOOL			RemoveEvent(ApdEvent *pcsEvent);
	BOOL			RemoveEvent(INT32 lEID);

	// Event Source Position�� ���´�.
	AuPOS *			GetBasePos(ApBase *pcsBase, AuMATRIX **ppstDirection);
	AuPOS *			GetBasePos(ApdEvent *pstEvent, AuPOS *pstPos);

	// Event Condition Process functions

	// ���ǿ� �´� ���� �����Ѵ�. (Ȥ�� ���ǿ� �´� ���� �ϳ��� �Լ��� ���ڷ� �Ѱ��ش�.)

	// Event Condition�� Target Factor�� �ִ� ��� Factor�� Ư������ ���� �ִ� Character�� ��� ó�����ش�.
	BOOL			GetTargetFactorCharacters(ApdEvent *pstEvent, ApmEventConditionProcessor fnProcessor, PVOID pvClass, PVOID pvUpdateData);
	// Event Condition�� Target Item�� �ִ� ��� Item�� ���� �ִ�(�����ϰ� �ִ�) Character�� ��� ó�����ش�.
	BOOL			GetTargetItemCharacters(ApdEvent *pstEvent, ApmEventConditionProcessor fnProcessor, PVOID pvClass, PVOID pvUpdateData);
	// Event Condition�� Area�� ���� ��� �ش�Ǵ� Area�� Character�� �����´�.
	INT32			GetAreaCharacters( INT32 nDimension , INT32	lCharType, ApdEvent *pstEvent, INT_PTR *alCID, INT16 nCount, INT32* alCID2, INT16 nCount2, AuPOS *pposBase = NULL);
	INT32			GetAreaCharacters( INT32 nDimension , INT32	lCharType, ApdEvent *pstEvent, INT_PTR *alCID, INT16 nCount, INT32* alCID2, INT16 nCount2, ApBase *pcsBase);
	/*
	INT32			GetAreaNPCs(ApdEvent *pstEvent, INT32 *alCID, INT16 nCount, AuPOS *pposBase = NULL);
	INT32			GetAreaNPCs(ApdEvent *pstEvent, INT32 *alCID, INT16 nCount, ApBase *pcsBase);
	INT32			GetAreaMonsters(ApdEvent *pstEvent, INT32 *alCID, INT16 nCount, AuPOS *pposBase = NULL);
	INT32			GetAreaMonsters(ApdEvent *pstEvent, INT32 *alCID, INT16 nCount, ApBase *pcsBase);
	*/

	// Event Condition�� �ش� ������ �ִ��� �˻��Ѵ�.
	BOOL			IsTargetFactorCharacter(ApdEvent *pstEvent, AgpdCharacter *pcsCharacter);
	BOOL			IsTargetItemCharacter(ApdEvent *pstEvent, AgpdCharacter *pcsCharacter);

	BOOL			IsActiveSkillTIme(ApdEvent *pstEvent, UINT32 ulClockCount);
	BOOL			IsEndSkillTIme(ApdEvent *pstEvent, UINT32 ulClockCount);

	BOOL			GetRandomPos(ApdEvent *pstEvent, AuPOS *pstPos);
	BOOL			GetRandomPos4MobWander(ApdEvent *pstEvent, AuPOS *pstObjCurrentPos, AuPOS *pstPos);
	BOOL			GetRandomPos(AuPOS *pstBasePos, AuPOS *pstRandomPos, FLOAT fRadiusMin, FLOAT fRadiusMax, BOOL bCheckBlocking);

	// Packet ����
	PVOID			MakeBasePacket(ApdEvent *pstEvent);
	ApdEvent *		GetEventFromBasePacket(PVOID pvPacket);

	PVOID			MakePacketEventData(ApdEvent *pcsEvent, ApdEventFunction eFunction, INT16 *pnPacketLength);

	BOOL			EventConstructor(ApBase *pcsBase);
	BOOL			EventConstructor(ApdEventAttachData *pcsEvents);

	BOOL			EventDestructor(ApdEventAttachData *pcsEvents);

	BOOL			InitObject(ApBase *pcsDstBase, ApdEventAttachData *pcsSrcEvents);

	BOOL			StreamWrite(ApdEventAttachData *pcsEvents, ApModuleStream *pcsStream);
	BOOL			StreamRead(ApBase *pcsBase, ApdEventAttachData *pcsEvents, ApModuleStream *pcsStream);

	static BOOL		CBEventConstructorCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBEventConstructorCharacterTemplate(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBEventConstructorItem(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBEventConstructorItemTemplate(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBEventConstructorObject(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBEventConstructorObjectTemplate(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL		CBEventDestructor(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL		CBStreamWrite(PVOID pData, ApModule *pClass, ApModuleStream *pcsStream);
	static BOOL		CBStreamRead(PVOID pData, ApModule *pClass, ApModuleStream *pcsStream);

	static BOOL		CBInitObject(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL			SetCallbackInitObject(ApModuleDefaultCallBack fnCallback, PVOID pvClass);
	BOOL			SetCallbackAddEvent(ApModuleDefaultCallBack fnCallback, PVOID pvClass);

	BOOL			CheckValidRange(ApdEvent *pcsEvent, AuPOS *pcsPos, INT32 lRange, AuPOS *pcsDestPos = NULL);

	BOOL			RegisterPacketFunction(ApModuleDefaultCallBack fpMakePacketCallback, ApModuleDefaultCallBack fpReceivePacketCallback, ApModule *pcsModule, ApdEventFunction eFunction);
};

#endif // !defined(AFX_APMEVENTMANAGER_H__999BBC3E_ED58_4F45_94D4_D2691D7E0226__INCLUDED_)
