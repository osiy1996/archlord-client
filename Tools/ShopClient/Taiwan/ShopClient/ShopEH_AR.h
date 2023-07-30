#pragma once

/**************************************************************************************************

�ۼ���: 2008-07-10
�ۼ���: ������ (youngmoon@webzen.co.kr)

����: ���̺귯���� ���ǰ��� �۾��� �ϴ� ��ü

**************************************************************************************************/

#ifdef VERSION_VS60
	#ifdef _DEBUG
		#pragma  comment(lib, "ClientStub_VS60_d.lib")
	#else
		#pragma  comment(lib, "ClientStub_VS60.lib")
	#endif
#elif VERSION_VS2003
	#ifdef _UNICODE
		#ifdef _DEBUG
			#pragma  comment(lib, "ClientStub_VS2003_d.lib")
		#else
			#pragma  comment(lib, "ClientStub_VS2003.lib")
		#endif	
	#else
		#ifdef _DEBUG
			#pragma  comment(lib, "ClientStub_VS2003_MB_d.lib")
		#else
			#pragma  comment(lib, "ClientStub_VS2003_MB.lib")
		#endif	
	#endif
#elif VERSION_VS2005
	#ifdef _WIN64
		#ifdef _UNICODE
			#ifdef _DEBUG
				#pragma  comment(lib, "ClientStub_VS2005_64_d.lib")
			#else
				#pragma  comment(lib, "ClientStub_VS2005_64.lib")
			#endif	
		#else
			#ifdef _DEBUG
				#pragma  comment(lib, "ClientStub_VS2005_64_MB_d.lib")
			#else
				#pragma  comment(lib, "ClientStub_VS2005_64_MB.lib")
			#endif	
		#endif
	#else
		#ifdef _UNICODE
			#ifdef _DEBUG
				#pragma  comment(lib, "ClientStub_VS2005_d.lib")
			#else
				#pragma  comment(lib, "ClientStub_VS2005.lib")
			#endif	
		#else
			#ifdef _DEBUG
				#pragma  comment(lib, "ClientStub_VS2005_MB_d.lib")
			#else
				#pragma  comment(lib, "ClientStub_VS2005_MB.lib")
			#endif	
		#endif
	#endif
#elif VERSION_VS2005_NOSP
	#ifdef _WIN64
		#ifdef _UNICODE
			#ifdef _DEBUG
				#pragma  comment(lib, "ClientStub_VS2005_NOSP_64_d.lib")
			#else
				#pragma  comment(lib, "ClientStub_VS2005_NOSP_64.lib")
			#endif	
		#else
			#ifdef _DEBUG
				#pragma  comment(lib, "ClientStub_VS2005_NOSP_64_MB_d.lib")
			#else
				#pragma  comment(lib, "ClientStub_VS2005_NOSP_64_MB.lib")
			#endif	
		#endif
	#else
		#ifdef _UNICODE
			#ifdef _DEBUG
				#pragma  comment(lib, "ClientStub_VS2005_NOSP_d.lib")
			#else
				#pragma  comment(lib, "ClientStub_VS2005_NOSP.lib")
			#endif	
		#else
			#ifdef _DEBUG
				#pragma  comment(lib, "ClientStub_VS2005_NOSP_MB_d.lib")
			#else
				#pragma  comment(lib, "ClientStub_VS2005_NOSP_MB.lib")
			#endif	
		#endif
	#endif
#elif VERSION_VS2008
	#ifdef _WIN64
		#ifdef _R2_
			#ifdef _UNICODE
				#ifdef _DEBUG
					#pragma  comment(lib, "ClientStub_VS2008_R2_64_d.lib")
				#else
					#pragma  comment(lib, "ClientStub_VS2008_R2_64.lib")
				#endif	
			#else
			#endif
		#else
			#ifdef _UNICODE
				#ifdef _DEBUG
					#pragma  comment(lib, "ClientStub_VS2008_64_d.lib")
				#else
					#pragma  comment(lib, "ClientStub_VS2008_64.lib")
				#endif	
			#else
				#ifdef _DEBUG
					#pragma  comment(lib, "ClientStub_VS2008_64_MB_d.lib")
				#else
					#pragma  comment(lib, "ClientStub_VS2008_64_MB.lib")
				#endif	
			#endif
		#endif
	#else
		#ifdef _R2_
			#ifdef _UNICODE
				#ifdef _DEBUG
					#pragma  comment(lib, "ClientStub_VS2008_R2_d.lib")
				#else
					#pragma  comment(lib, "ClientStub_VS2008_R2.lib")
				#endif	
			#else
			#endif
		#else
			#ifdef _UNICODE
				#ifdef _DEBUG
					#pragma  comment(lib, "ClientStub_VS2008_d.lib")
				#else
					#pragma  comment(lib, "ClientStub_VS2008.lib")
				#endif	
			#else
				#ifdef _DEBUG
					#pragma  comment(lib, "ClientStub_VS2008_MB_d.lib")
				#else
					#pragma  comment(lib, "ClientStub_VS2008_MB.lib")
				#endif
			#endif
		#endif
	#endif
#elif VERSION_VS2005_MT
	#ifdef _WIN64
	#else
		#ifdef _UNICODE
		#else
			#ifdef _DEBUG
				#pragma  comment(lib, "ClientStub_VS2005_MB_MT_d.lib")
			#else
				#pragma  comment(lib, "ClientStub_VS2005_MB_MT.lib")
			#endif
		#endif
	#endif
#elif VERSION_VS2008_MT
	#ifdef _WIN64
	#else
		#ifdef _UNICODE
		#else
			#ifdef _DEBUG
				#pragma  comment(lib, "ClientStub_VS2008_MB_MT_d.lib")
			#else
				#pragma  comment(lib, "ClientStub_VS2008_MB_MT.lib")
			#endif
		#endif
	#endif
#endif

#include <Winsock2.h>
#include <WBANetwork.h>
#include "ShopEventHandler_AR.h"
#pragma warning(disable:4995)
#pragma warning(disable:4996)

class CShopEH_AR : public CShopEventHandler_AR
{
public:
	CShopEH_AR(void);
	virtual  ~CShopEH_AR(void);

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

	
	void OnGiftProduct(DWORD AccountSeq, 
					   DWORD ReceiverSeq, 
					   DWORD DeductCashSeq, 
					   long ResultCode);

	void OnInquireCash(DWORD AccountSeq, 
					   char AccountID[MAX_ACCOUNT_LENGTH], 
					   double CashSum, 
					   double PointSum, 
					   ShopProtocol::STCashDetail_GB Detail[], 
					   long nCashInfoCount, 
					   long ResultCode);

	void OnBuyProduct(DWORD AccountSeq, 
					  char AccountID[MAX_ACCOUNT_LENGTH], 
					  DWORD DeductCashSeq, 
					  DWORD InGamePurchaseSeq, 
					  long ResultCode);

	// AR_TW Start
	void OnInquireCash(char AccountID[MAX_ACCOUNT_LENGTH], 
					   double Cash,
					   long ResultCode);


	void OnBuyProduct(char	AccountID[MAX_ACCOUNT_LENGTH], 
					  INT64	ItemNo,
					  INT64 OrderNo,
					  int	OrderNumber,					
					  WCHAR	RetMessage[MAX_RETMESSAGE_LENGTH],
					  long	ResultCode);
	// AR_TW End



private:
	CRITICAL_SECTION m_cs;
};
