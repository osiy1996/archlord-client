#ifndef	__AGPMBILLINFO_H__
#define	__AGPMBILLINFO_H__

#include <ApModule/ApModule.h>
#include <AgpmBillInfo/AgpdBillInfo.h>
#include <AuPacket/AuPacket.h>


#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgpmBillInfoD" )
#else
#pragma comment ( lib , "AgpmBillInfo" )
#endif
#endif


typedef enum	_AgpmBillInfoOperation {
	AGPMBILL_OPERATION_ADD					= 0,
	AGPMBILL_OPERATION_CASHINFO,
	AGPMBILL_OPERATION_GUID,
} AgpmBillInfoOperation;

typedef enum	_AgpmBillInfoCBID {
	AGPMBILL_UPDATE_BIIL_INFO				= 0,
} AgpmBillInfoCBID;

class AgpdCharacter;

class AgpmBillInfo : public ApModule {
private:
	class AgpmCharacter	*m_pcsAgpmCharacter;

	INT32				m_lIndexADCharacter;

public:
	AuPacket		m_csPacket;

public:
	AgpmBillInfo();
	virtual ~AgpmBillInfo();

	BOOL	OnAddModule();
	BOOL	OnInit();
	BOOL	OnDestroy();

	BOOL	OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	AgpdBillInfo	*GetADCharacter(AgpdCharacter *pcsCharacter);

	PVOID	MakePacketBillInfo(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength);

	BOOL	SetCallbackUpdateBillInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	UpdateIsPCRoom(AgpdCharacter *pcsCharacter, BOOL bIsPCRoom);

	BOOL	IsPCBang(AgpdCharacter *pcsCharacter);
	BOOL	SetCashGlobal(AgpdCharacter* pcsCharacter, double WCoin, double PCoin);
	BOOL	GetCashGlobal(AgpdCharacter* pcsCharacter, double& WCoin, double& PCoin);

	BOOL	SetGUIDGlobal(AgpdCharacter* pcsCharacter, DWORD AccountGUID);
	DWORD	GetGUIDGlobal(AgpdCharacter* pcsCharacter);

	BOOL	SetCashTaiwan(AgpdCharacter* pcsCharacter, double CoinSum);
	BOOL	GetCashTaiwna(AgpdCharacter* pcsCharacter, double& CoinSum);
};

#endif	//__AGPMBILLINFO_H__