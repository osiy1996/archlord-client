#pragma once

#ifdef _WEBZEN_BILLING_

#ifdef _WIN64
#ifdef _DEBUG
#pragma  comment(lib, "ClientStub_VS2005_NOSP_64_MB_d.lib")
#else
#pragma  comment(lib, "ClientStub_VS2005_NOSP_64_MB.lib")
#endif
#else
#ifdef _DEBUG
#pragma  comment(lib, "ClientStub_VS2005_NOSP_MB_d.lib")
#else
#pragma  comment(lib, "ClientStub_VS2005_NOSP_MB.lib")
#endif
#endif


//#pragma comment(lib, "atls.lib")
//#include "ApBase.h"

//#include "Base.h"
#include "WBANetwork.h"
#include "util/Stream.h"
#include "ShopEventHandler.h"


//class CUser;
//class CPlayer;

class AgsmBillingWebzen : public CShopEventHandler
{
public:
	AgsmBillingWebzen(void);
	~AgsmBillingWebzen(void);

	void Start( BOOL bFirstStarUp = TRUE );
	void Stop();

	inline int		GetYear();
	inline int		GetYearIdentity();

	inline BOOL		IsNumberFromItemID( char* pData, int size );
	inline void		SetConnect(BOOL bConnect);
	inline BOOL		IsConnect();

protected:
	//���������� �α׸� �ۼ� �̺�Ʈ �̴�.
	//�Լ����ο��� �α׸� ����� �ֵ��� �ؾ��Ѵ�.
	void WriteLog(char* szMsg);

	//���������� ���� ���� ���� (���� ����Ǹ� ���� ������ �ʿ��� ��� ������ ������ �ٽ� �α����ؾ��Ѵ�.)
	//		success: ���� ���� ����
	//		error: �����ڵ�
	void OnNetConnect(bool success, DWORD error);

	//���������� ���� ����Ÿ ���� ����
	//		size: ���۵� ��Ŷ ������
	void OnNetSend( int size );

	//���������� ������ �������� (���������� ���������� ó�� �ؾ��Ѵ�.)	
	//		error: �����ڵ�
	void OnNetClose( DWORD error );	



	// ��ǰ �� ��ũ��Ʈ �ֽ� ���� ���� ��ȸ
	virtual void OnInquireSalesZoneScriptVersion(unsigned short SalesZone, 
		unsigned short Year, 
		unsigned short YearIdentity,
		long ResultCode);

	// ��� ��ũ��Ʈ �ֽ� ���� ���� ��ȸ
	virtual void OnInquireBannerZoneScriptVersion(unsigned short BannerZone, 
		unsigned short Year, 
		unsigned short YearIdentity,
		long ResultCode);

	// ĳ�� ��ȸ�� ���
	// MU
	virtual void OnInquireCash(DWORD AccountSeq, 
		double CashSum, 
		double PointSum, 
		double MileageSum, 
		int DetailCount, 
		ShopProtocol::STCashDetail Detail[], 
		long ResultCode);

	// MU Global
	virtual void OnInquireCash(DWORD AccountSeq, 
		double CashSum, 
		double PointSum, 
		double MileageSum, 
		int ListCount, 
		ShopProtocol::STCashDetail_GB Detail[], 
		long ResultCode);

	// MU JP - GameChu, GameOn 
	virtual void OnInquireCash(DWORD AccountSeq, 
		long ResultCode,
		long OutBoundResultCode,
		double CashSum, 
		double MileageSum);

	// Hx
	virtual void OnInquireCash(DWORD AccountSeq, 
		char AccountID[MAX_ACCOUNTID_LENGTH], 
		double CashSum, 
		double PointSum, 
		double MileageSum, 
		int DetailCount, 
		ShopProtocol::STCashDetail Detail[], 
		long ResultCode,
		long OutBoundResultCode);

	// ��ǰ ������ ���
	// MU
	virtual void OnBuyProduct(DWORD AccountSeq, 
		long ResultCode, 
		long LeftProductCount);

	// MU JP - GameChu, GameOn 
	virtual void OnBuyProduct(DWORD AccountSeq, 
		long ResultCode,
		long OutBoundResultCode,
		long LeftProductCount);

	// Hx
	virtual void OnBuyProduct(DWORD AccountSeq, 
		char AccountID[MAX_ACCOUNTID_LENGTH], 
		long LeftProductCount, 
		long ResultCode,
		long OutBoundResultCode);

	// ��ǰ ������ ���
	// MU
	virtual void OnGiftProduct(DWORD SenderSeq, 
		DWORD ReceiverSeq, 
		double LimitedCash, 
		long LeftProductCount, 
		long ResultCode);

	// MU JP - GameChu, GameOn 
	virtual void OnGiftProduct(DWORD  SenderAccountSeq, 
		long   ResultCode,
		long   OutBoundResultCode,
		DWORD  ReceiverAccountSeq, 
		long   LeftProductCount);

	// Hx
	virtual void OnGiftProduct(DWORD  SenderAccountSeq, 
		char   SenderAccountID[MAX_ACCOUNTID_LENGTH], 
		WCHAR  SenderCharName[MAX_CHARACTERID_LENGTH], 
		DWORD  ReceiverAccountSeq, 
		char   ReceiverAccountID[MAX_ACCOUNTID_LENGTH], 
		WCHAR  ReceiverCharName[MAX_CHARACTERID_LENGTH], 
		WCHAR  Message[MAX_MESSAGE_LENGTH], 
		double LimitedCash, 
		long   LeftProductCount, 
		long   ResultCode,
		long   OutBoundResultCode);

	// ĳ�� ���� ���
	virtual void OnGiftCash(DWORD SenderSeq,
		DWORD ReceiverSeq, 
		long ResultCode, 
		double GiftSendLimit);

	// ��ǰ ����/���� ���� ��ȸ ���
	virtual void OnInquireBuyGiftPossibility(DWORD AccountID,
		long ResultCode, 
		long ItemBuy, 
		long Present, 
		double MyPresentCash, 
		double MyPresentSendLimit);

	// �̺�Ʈ ��ǰ ����Ʈ ��ȸ ���
	virtual void OnInquireEventProductList(DWORD AccountSeq, 
		long ProductDisplaySeq, 
		int PackagesCount, 
		long Packages[]);

	// ���� ��ǰ �ܿ� ���� ��ȸ ���
	virtual void OnInquireProductLeftCount(long PackageSeq, long LeftCount);

	// ������ ��ȸ ���
	virtual void OnInquireStorageList(DWORD AccountID,
		long ListCount, 
		long ResultCode, 
		ShopProtocol::STStorage StorageList[]);

	// ������ ������ ��ȸ ���
	virtual void OnInquireStorageListPage(DWORD AccountID, 
		int	ResultCode, 
		char  StorageType,
		int	NowPage,
		int   TotalPage,
		int   TotalCount,
		int   ListCount, 
		ShopProtocol::STStorage StorageList[]);

	// ������ ������ ��ȸ ��� - ���� �޽��� ����
	virtual void OnInquireStorageListPageNoGiftMessage(DWORD AccountID, 
		int   ResultCode, 
		char  StorageType,
		int   NowPage,
		int   TotalPage,
		int   TotalCount,
		int   ListCount, 
		ShopProtocol::STStorageNoGiftMessage StorageList[]);

	// ������ ��ǰ ����ϱ� ���
	virtual void OnUseStorage(DWORD AccountSeq,
		long ResultCode, 
		char InGameProductID[MAX_TYPENAME_LENGTH], 
		BYTE PropertyCount, 
		long ProductSeq, 
		long StorageSeq,
		long StorageItemSeq,
		ShopProtocol::STItemProperty PropertyList[]);

	// ������ ��ǰ ����ϱ� �ѹ�
	virtual void OnRollbackUseStorage(DWORD AccountSeq, long ResultCode);

	// ������ ��ǰ ������ ���
	virtual void OnThrowStorage(DWORD AccountSeq, long ResultCode);

	// ���ϸ��� ����
	virtual void OnMileageDeduct(DWORD AccountSeq, long ResultCode);

	// ���ϸ��� ����
	virtual void OnMileageSave(DWORD AccountSeq, long ResultCode);

	// �ǽð� ���ϸ��� ����
	virtual void OnMileageLiveSaveUp(DWORD AccountSeq, long ResultCode);

	// ������ �ø��� �ڵ� ������Ʈ
	virtual void OnItemSerialUpdate(DWORD AccountSeq, long ResultCode);

	// ��ǰ �� ��ũ��Ʈ ���� ���� ������Ʈ �˸�
	virtual void OnUpdateVersion(long GameCode, 
		unsigned short SalesZone, 
		unsigned short year, 
		unsigned short yearIdentity);

	// ��� ��ũ��Ʈ ���� ���� ������Ʈ �˸�
	virtual void OnUpdateBannerVersion(long GameCode, 
		unsigned short BannerZone, 
		unsigned short year, 
		unsigned short yearIdentity);

	// �ΰ��� ����Ʈ ������ ���� �ۼ�Ʈ ��ȸ
	virtual void OnInquireInGamePointValue(long ResultCode,
		long PointCount,
		ShopProtocol::STPointDetail PointList[]);


	// Ÿ�̸�
//	void			OnTimer( int nId );

private:
	// ������
//	CCriticalSection					m_BillingLock;			

	int									m_nYear;
	int									m_nYearIdentity;
	BOOL								m_bConnect;
};

inline void	AgsmBillingWebzen::SetConnect(BOOL bConnect) {	/*CCriticalSectionLock theLock( &m_BillingLock ); */m_bConnect = bConnect;}
inline BOOL	AgsmBillingWebzen::IsConnect() { /*CCriticalSectionLock theLock( &m_BillingLock );*/ return m_bConnect; }

inline BOOL	AgsmBillingWebzen::IsNumberFromItemID( char* pData, int nSize )
{
	for(int i = 0 ; i < nSize; i++)
	{
		if(isdigit(pData[i]) == false)
			return FALSE;
	}
	return TRUE;
}

inline int AgsmBillingWebzen::GetYear()  { /*CCriticalSectionLock theLock( &m_BillingLock );*/ return m_nYear; }
inline int AgsmBillingWebzen::GetYearIdentity()  {	/*CCriticalSectionLock theLock( &m_BillingLock );*/	return m_nYearIdentity;}

/*
//////////////////////////////////////////////////////////////////////
//	GLOBAL VARIABLE.
extern	AgsmBillingWebzen	g_cBillingSystem;

//////////////////////////////////////////////////////////////////////
//	Nation Code For Billing

inline int GetSalesZoneBilling()
{
	// Nation Billing Code for USA 
	if(IsNation( SERVICE_USA )) 
		return PAYITEM_SALEZONE_USA;

	// Nation Billing Code for Korea 
	if(IsNation( SERVICE_KOR )) 
		return PAYITEM_SALEZONE_KOREA;
	return 0;
}

inline int GetGameCodeBilling()
{
	// Nation Billing Code for USA 
	if(IsNation( SERVICE_USA )) 
		return PAYITEM_GAMECODE_USA;

	// Nation Billing Code for Korea 
	if(IsNation( SERVICE_KOR )) 
		return PAYITEM_GAMECODE_KOREA;
	return 0;
}
*/


#endif //_WEBZEN_BILLING_