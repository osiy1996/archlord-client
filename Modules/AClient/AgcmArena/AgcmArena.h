#ifndef _AGCM_ARENA_H_
#define _AGCM_ARENA_H_

#include <AgcModule/AgcModule.h>
#include <AuPacket/AuPacket.h>

class AgcmUIManager2;
struct AgcdUIUserData;
class AgcdUIControl;
enum AgcdUIDataType;

struct AgcdArenaLeaderboardEntry {
	UINT32	ulRank;
	char	szName[32];
	UINT32	ulPoints;
	UINT32	ulWin;
	UINT32	ulLose;
};

class AgcmArena : public AgcModule
{
private:
	AgcmUIManager2 *			m_pcsAgcmUIManager2;
	AuPacket					m_csPacket;
	AuPacket					m_csLeaderboardPacket;
	AuPacket					m_csEntryPacket;
	UINT32						m_ulArenaPoints;
	UINT32						m_ulArenaCoins;
	AgcdUIUserData *			m_pstArenaUD;
	AgcdUIUserData *			m_pstLeaderboardUD;
	AgcdArenaLeaderboardEntry	m_stLeaderboard[16];
	UINT32						m_ulLeaderboardIndex;
	UINT32						m_ulLeaderboardCount;
	int							m_bArenaRegisteredNormal;
	int							m_bArenaRegisteredRanked;
	char						m_szLBPageBuffer[30000];
	UINT32						m_ulInQueueCount;

public:
	AgcmArena();
	virtual ~AgcmArena();

	BOOL OnAddModule();
	BOOL OnInit();
	BOOL OnDestroy();
	BOOL OnIdle( UINT32 ulClockCount );

private:
	BOOL OnReceive(
		UINT32			ulType, 
		PVOID			pvPacket, 
		INT16			nSize, 
		UINT32			ulNID, 
		DispatchArg *	pstCheckArg);

	void RefreshUD();
	void QueryPage(UINT32 ulPageIndex, BOOL bSearchSelf);

	static BOOL CBDisplayLBRank(
		PVOID			pClass,
		PVOID			pData,
		AgcdUIDataType	eType,
		INT32			lID,
		CHAR *			szDisplay,
		INT32 *			plValue,
		AgcdUIControl *	pcsSourceControl);
	
	static BOOL CBDisplayLBName(
		PVOID			pClass,
		PVOID			pData,
		AgcdUIDataType	eType,
		INT32			lID,
		CHAR *			szDisplay,
		INT32 *			plValue,
		AgcdUIControl *	pcsSourceControl);

	static BOOL CBDisplayLBWinLose(
		PVOID			pClass,
		PVOID			pData,
		AgcdUIDataType	eType,
		INT32			lID,
		CHAR *			szDisplay,
		INT32 *			plValue,
		AgcdUIControl *	pcsSourceControl);

	static BOOL CBDisplayLBPoints(
		PVOID			pClass,
		PVOID			pData,
		AgcdUIDataType	eType,
		INT32			lID,
		CHAR *			szDisplay,
		INT32 *			plValue,
		AgcdUIControl *	pcsSourceControl);

	static BOOL CBDisplayLBPage(
		PVOID			pClass,
		PVOID			pData,
		AgcdUIDataType	eType,
		INT32			lID,
		CHAR *			szDisplay,
		INT32 *			plValue,
		AgcdUIControl *	pcsSourceControl);

	static BOOL CBDisplayLBPageMax(
		PVOID			pClass,
		PVOID			pData,
		AgcdUIDataType	eType,
		INT32			lID,
		CHAR *			szDisplay,
		INT32 *			plValue,
		AgcdUIControl *	pcsSourceControl);

	static BOOL CBDisplayInQueueCount(
		PVOID			pClass,
		PVOID			pData,
		AgcdUIDataType	eType,
		INT32			lID,
		CHAR *			szDisplay,
		INT32 *			plValue,
		AgcdUIControl *	pcsSourceControl);

	static BOOL CBDisplayPoints(
		PVOID			pClass,
		PVOID			pData,
		AgcdUIDataType	eType,
		INT32			lID,
		CHAR *			szDisplay,
		INT32 *			plValue,
		AgcdUIControl *	pcsSourceControl);

	static BOOL CBDisplayCoins(
		PVOID			pClass,
		PVOID			pData,
		AgcdUIDataType	eType,
		INT32			lID,
		CHAR *			szDisplay,
		INT32 *			plValue,
		AgcdUIControl *	pcsSourceControl);

	static BOOL CBDisplayRegisterNormal(
		PVOID			pClass,
		PVOID			pData,
		AgcdUIDataType	eType,
		INT32			lID,
		CHAR *			szDisplay,
		INT32 *			plValue,
		AgcdUIControl *	pcsSourceControl);

	static BOOL CBDisplayRegisterRanked(
		PVOID			pClass,
		PVOID			pData,
		AgcdUIDataType	eType,
		INT32			lID,
		CHAR *			szDisplay,
		INT32 *			plValue,
		AgcdUIControl *	pcsSourceControl);

	static BOOL CBHomePage(
		PVOID			pClass, 
		PVOID			pData1, 
		PVOID			pData2, 
		PVOID			pData3, 
		PVOID			pData4, 
		PVOID			pData5, 
		ApBase *		pcsTarget, 
		AgcdUIControl *	pcsSourceControl);

	static BOOL CBPreviousPage(
		PVOID			pClass, 
		PVOID			pData1, 
		PVOID			pData2, 
		PVOID			pData3, 
		PVOID			pData4, 
		PVOID			pData5, 
		ApBase *		pcsTarget, 
		AgcdUIControl *	pcsSourceControl);

	static BOOL CBNextPage(
		PVOID			pClass, 
		PVOID			pData1, 
		PVOID			pData2, 
		PVOID			pData3, 
		PVOID			pData4, 
		PVOID			pData5, 
		ApBase *		pcsTarget, 
		AgcdUIControl *	pcsSourceControl);

	static BOOL CBMyRank(
		PVOID			pClass, 
		PVOID			pData1, 
		PVOID			pData2, 
		PVOID			pData3, 
		PVOID			pData4, 
		PVOID			pData5, 
		ApBase *		pcsTarget, 
		AgcdUIControl *	pcsSourceControl);

	static BOOL CBRegisterNormal(
		PVOID			pClass, 
		PVOID			pData1, 
		PVOID			pData2, 
		PVOID			pData3, 
		PVOID			pData4, 
		PVOID			pData5, 
		ApBase *		pcsTarget, 
		AgcdUIControl *	pcsSourceControl);

	static BOOL CBRegisterRanked(
		PVOID			pClass, 
		PVOID			pData1, 
		PVOID			pData2, 
		PVOID			pData3, 
		PVOID			pData4, 
		PVOID			pData5, 
		ApBase *		pcsTarget, 
		AgcdUIControl *	pcsSourceControl);
};

#endif /* _AGCM_ARENA_H_ */
