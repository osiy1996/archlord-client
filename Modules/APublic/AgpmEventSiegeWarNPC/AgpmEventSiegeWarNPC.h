#pragma once

#include <ApModule/ApModule.h>
#include <AuPacket/AuPacket.h>

typedef enum AgpmEventSiegeWarNPCOperation {
	AGPMEVENTSIEGEWAR_EVENT_REQUEST				= 1,
	AGPMEVENTSIEGEWAR_EVENT_GRANT,
};

typedef enum AgpmEventSiegeWarNPCCBID {
	AGPMEVENTSIEGEWAR_CB_EVENT_GRANT			= 0,
};

struct ApdEvent;
class AgpdCharacter;
class AgpdSiegeWar;

class AgpmEventSiegeWarNPC : public ApModule {
public:
	AgpmEventSiegeWarNPC();
	virtual ~AgpmEventSiegeWarNPC();

	BOOL	OnAddModule();

	AuPacket	m_csPacket;
	AuPacket	m_csPacketEventData;

public:
	class ApmEventManager	*m_pcsApmEventManager;
	class AgpmCharacter		*m_pcsAgpmCharacter;
	class AgpmSiegeWar		*m_pcsAgpmSiegeWar;

public:
	BOOL				OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

private:
	BOOL				OnOperationEventRequest(ApdEvent *pcsEvent, AgpdCharacter *pcsCharacter);
	BOOL				OnOperationEventGrant(ApdEvent *pcsEvent);

public:
	static BOOL			CBActionSiegeWar(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL				SetCallbackEventGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	PVOID				MakePacketEventRequest(ApdEvent *pcsEvent, INT32 lCID, INT16 *pnPacketLength);
	PVOID				MakePacketEventGrant(ApdEvent *pcsEvent, INT16 *pnPacketLength);

	AgpdSiegeWar*		GetSiegeWarInfo(ApdEvent *pcsEvent);
};