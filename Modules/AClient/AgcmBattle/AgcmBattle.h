#ifndef _AGCM_BATTLE_H_
#define _AGCM_BATTLE_H_

#include <AgcModule/AgcModule.h>
#include <AuPacket/AuPacket.h>

class AgcmUIManager2;
struct AgcdUIUserData;
class AgcdUIControl;
enum AgcdUIDataType;
class AgcmUISystemMessage;

class AgcmBattle : public AgcModule
{
private:
	AgcmUIManager2 *		m_pcsAgcmUIManager2;
	AgcmUISystemMessage *	m_pcsAgcmUISystemMessage;
	AuPacket				m_csPacket;
	time_t					m_tBegin;
	time_t					m_tUpdate;
	AgcdUIUserData *		m_pstTimerUD;
	INT32					m_lEventShowConfirm;
	INT32					m_lEventHideConfirm;

public:
	AgcmBattle();
	virtual ~AgcmBattle();

	BOOL OnAddModule();
	BOOL OnInit();
	BOOL OnDestroy();
	BOOL OnIdle(UINT32 ulClockCount);

private:
	BOOL OnReceive(
		UINT32			ulType, 
		PVOID			pvPacket, 
		INT16			nSize, 
		UINT32			ulNID, 
		DispatchArg *	pstCheckArg);

	static BOOL CBDisplayTimer(
		PVOID			pClass,
		PVOID			pData,
		AgcdUIDataType	eType,
		INT32			lID,
		CHAR *			szDisplay,
		INT32 *			plValue,
		AgcdUIControl *	pcsSourceControl);

	static BOOL CBAccept(
		PVOID			pClass, 
		PVOID			pData1, 
		PVOID			pData2, 
		PVOID			pData3, 
		PVOID			pData4, 
		PVOID			pData5, 
		ApBase *		pcsTarget, 
		AgcdUIControl *	pcsSourceControl);

	static BOOL CBDecline(
		PVOID			pClass, 
		PVOID			pData1, 
		PVOID			pData2, 
		PVOID			pData3, 
		PVOID			pData4, 
		PVOID			pData5, 
		ApBase *		pcsTarget, 
		AgcdUIControl *	pcsSourceControl);
};

#endif /* _AGCM_BATTLE_H_ */