#ifndef _AGSMBILLINGGLOBAL_H
#define _AGSMBILLINGGLOBAL_H

#include "ApDefine.h"
#include "AgsmBilling.h"


struct AgpdBillingGlobal : public ApMemory<AgpdBillingGlobal, 3000>
{
	DWORD AccountGUID;
	AgpdCharacter* pcsCharacter;
	DWORD dwBillingGUID;
	//JK_PC����ݻ���Ȯ��
	DWORD dwPCRoomGUID;

	AgpdBillingGlobal()
	{
		AccountGUID = 0;
		pcsCharacter = NULL;
		dwBillingGUID = 0;

		dwPCRoomGUID = 0;//JK_PC����ݻ���Ȯ��
	};
};

class AgsmBillingGlobal
{
public:
	AgsmBillingGlobal();
	virtual ~AgsmBillingGlobal();

	ApAdmin m_AdminGUID;

	BOOL	Initialize();
	BOOL	OnIdle();
	
	BOOL ConnectShop();
	BOOL ConnectBilling();

	static void OnLog(char* strLog);
	static void OnInquireCash(PVOID pInquire);
	static void	OnBuyProduct(PVOID pBuyProduct);

	static void OnUserStatus(PVOID UserStatus);
	static void OnInquirePersonDeduct(PVOID InquirePersonDeduct);
	//JK_��������
	static void OnInquireSalesZoneScriptVersion( PVOID pInquire );
	//JK_��������
	static void OnUpdateVersion( PVOID pInquire );
	//JK_��������
	static void OnUseStorage( PVOID pUseStorage);
	//JK_��������
	static void OnInquireStorageListPageNoGiftMessage( PVOID pStorageList );
	//JK_��������
	static void OnRollbackUseStorage( PVOID pRollbackUseStorage );
	//JK_PC����ݻ���Ȯ��
	static void	OnInquirePCRoomPoint( PVOID pInquirePCRoomPoint );



	//JK_��������
	static void OnNetConnectShop(PVOID pOnNetConnect);
		

	BOOL	InquireCash(CHAR* szAccountID);
	//JK_��������
	BOOL	InquireCash( DWORD dwAccountGUID );

	BOOL	BuyProduct(CHAR* AccountID, INT32 ProductID, CHAR* ProductName, UINT64 ListSeq, INT32 Class, INT32 Level, CHAR* szCharName, INT32 ServerIndex, double Price, INT32 lType );
	//JK_��������
	BOOL	BuyProduct(DWORD dwAccountSeq, INT32 ProductSeq, INT32 DisplaySeq, INT32 PriceSeq, INT32 Class, INT32 Level, CHAR* szCharName, CHAR* szRank,  INT32 ServerIndex);
	//JK_��������
	BOOL	UseStorage(DWORD AccountSeq, CHAR* szIP, DWORD StorageSeq, DWORD StorageItemSeq, INT32 Class, INT32 Level, CHAR* CharName, CHAR* Rank, INT32 ServerIndex);
	//JK_��������
	BOOL	InquireStorageList(DWORD AccountSeq, INT32 NowPage);
	//JK_�������� 
	BOOL	RollbackUseStorage(DWORD AccountSeq, DWORD StorageSeq, DWORD StorageItemSeq);






	BOOL	InquirePersonDeduct(DWORD AccountGUID);

	BOOL	AddUser(AgpdCharacter* pcsCharacter, DWORD AccountGUID);
	BOOL	RemoveUser(DWORD AccountGUID);
	AgpdCharacter* GetCharacter(DWORD AccountGUID);
	BOOL	SetBillingGUID(DWORD AccountGUID, DWORD BillingGUID);
	//JK_��������
	DWORD	GetBillingGUID(DWORD AccountGUID);


	BOOL	Login(DWORD AccountGUID, CHAR* szIP);
	void	LogOut(DWORD AccountGUID);

	BOOL	LoginWebzen( DWORD AccountGUID, CHAR* szIP, DWORD PCRoomGUID);
	//JK_�������� : ������ ���ڿ��ΰ�?
	BOOL	IsNumberFromItemID( char* pData, int nSize );
	//JK_PC����ݻ���Ȯ��
	BOOL	SetPCRoomGUID(DWORD AccountGUID, DWORD PCRoomGUID);
	DWORD	GetPCRoomGUID(DWORD AccountGUID);
	void	CheckPCRoomPointAllUser();





};


#endif //_AGSMBILLINGGLOBAL_H