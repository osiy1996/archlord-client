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

#include <Winsock2.h>
#include <Windows.h>
#include <WBANetwork.h>
#include <util/Stream.h>
#include "BillEventHandler.h"

class CLibClientSession : public CBillEventHandler
{
public:
	CLibClientSession(void);
	virtual  ~CLibClientSession(void);

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

	//������ ���� ������ ����Ǿ��� ��
	//		dwBillingGUID: ���� ����ũ ���̵�
	//		RealEndDate: ���� ��� ���� �Ͻ� (__time32_t) 
	//		EndDate: ���� ���� ���� �Ͻ�
	//		dwRestPoint: ���� �ܿ� ����Ʈ 
	//		nDeductType: ����� ���� ����
	//		nAccessCheck: ���� üũ ���
	//		nResultCode: ó�����
	void OnUserStatus(DWORD dwAccountGUID, 
					  long dwBillingGUID, 
					  DWORD RealEndDate, 
					  DWORD EndDate, 
					  double dRestPoint, 
					  double dRestTime, 
					  short nDeductType, 
					  short nAccessCheck, 
					  short nResultCode);

	//PC �� ��Ƽ ���� ��ȸ ��� 
	//		��� �ڵ�
	//		1 : ����
	//		2 : �Ұ�
	//		-1 : PC�� ���� ���� 
	void OnInquireMultiUser(long AccountID, long RoomGUID, long Result);

	// Ư�� PC���� ���� �� ���� ���� ���θ� ��ȸ�Ѵ�.
	void OnInquirePCRoomPoint(long AccountID, 
							  long RoomGUID, 
							  long GameCode, 
							  long ResultCode);

	void OnInquirePersonDeduct(long AccountGUID, long GameCode, long ResultCode);

	//// ���� Ÿ�� ��ȸ
	//void OnInquireDeductType(long AccountID, long Result);

private:
	CRITICAL_SECTION m_cs;

};
