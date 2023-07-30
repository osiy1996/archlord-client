#pragma once

/**************************************************************************************************

�ۼ���: 2008-07-08
�ۼ���: ������ (youngmoon@webzen.co.kr)

����: ���������� ����ϴ� Ŭ���̾�Ʈ���� ���ŵ� ��Ŷ�� ������ �ֱ� ���� �̺�Ʈ ��ü 
      �̺�Ʈ�� �޴���(����Ŭ���̾�Ʈ)���� ��ӹ޾� ����ؾ� �Ѵ�.

**************************************************************************************************/

#include "ClientSession.h"
#include "protocol_billing.h"


class CBillEventHandler : public CClientSession
{
public:
	CBillEventHandler(void);
	virtual ~CBillEventHandler(void);

	//�ʱ�ȭ �Լ� (���α׷� �����ϰ� �ѹ��� ����ؾ� �Ѵ�.)
	bool CreateSession(WBANetwork::LPFN_ErrorHandler errhandler = NULL);

	//���� ������ �α��� �Ѵ�.
	bool UserLogin(DWORD dwAccountGUID, DWORD dwIPAddress, DWORD dwRoomGUID, DWORD dwGameCode, DWORD dwServerType);

	//���� ������ �α׾ƿ� �Ѵ�.
	bool UserLogout(long dwBillingGUID);

	//����� ������ ��ȸ�Ѵ�.
	bool InquireUser(long dwBillingGUID);	

	//PC ���� ��Ƽ ������ �����ϴ��� ��ȸ�Ѵ�.
	bool InquireMultiUser(long AccountGUID, long RoomGUID);	

	// ������ ���� �� ����, ���� ���� ���θ� ��ȸ�Ѵ�.
	bool InquirePersonDeduct(long AccountGUID, long GameCode);

	// Ư�� PC���� ���� �� ���� ���� ���θ� ��ȸ�Ѵ�.
	bool InquirePCRoomPoint(long AccountGUID, long RoomGUID, long GameCode);

protected:	
	void OnReceive( PBYTE buffer, int size );	

	//���������� ���� ���� ���� (���� ����Ǹ� ���� ������ �ʿ��� ��� ������ ������ �ٽ� �α����ؾ��Ѵ�.)
	//		success: ���� ���� ����
	//		error: �����ڵ�
	void OnConnect(bool success, DWORD error);

	//���������� ���� ����Ÿ ���� ����
	//		size: ���۵� ��Ŷ ������
	void OnSend( int size );

	//���������� ������ �������� (���������� ���������� ó�� �ؾ��Ѵ�.)	
	//		error: �����ڵ�
	void OnClose( DWORD error );	


protected:
	
	// ������ ���� �� ����, ���� ���� ���θ� ��ȸ�Ѵ�.
	virtual void OnInquirePersonDeduct(long AccountGUID, 
									   long GameCode, 
									   long ResultCode) = 0;

	// Ư�� PC���� ���� �� ���� ���� ���θ� ��ȸ�Ѵ�.
	virtual void OnInquirePCRoomPoint(long AccountGUID, 
									  long RoomGUID, 
									  long GameCode, 
									  long ResultCode) = 0;

	//PC ���� ��Ƽ ������ �����ϴ��� ��ȸ�Ѵ�.
	virtual void OnInquireMultiUser(long AccountID, long RoomGUID, long Result) = 0;	

	//������ ���� ������ ����Ǿ��� ��
			//dwAccountGUID: �α��� �Ҷ� ������ ���� GUID
			//dwBillingGUID: ���� ����ũ ���̵�
			//RealEndDate: ���� ��� ���� �Ͻ� (__time32_t) 
			//EndDate: ���� ���� ���� �Ͻ�
			//dwRestPoint: ���� �ܿ� ����Ʈ 
			//dRestTime; ���� �ܿ� �ð�(��)
			//nDeductType: ����� ���� ����
			//nAccessCheck: ���� üũ ���
			//nResultCode: ó�����
	virtual void OnUserStatus(DWORD dwAccountGUID, 
							  long dwBillingGUID, 
							  DWORD RealEndDate, 
							  DWORD EndDate, 
							  double dRestPoint, 
							  double dRestTime, 
							  short nDeductType, 
							  short nAccessCheck, 
							  short nResultCode) = 0;

	//������ ������ ����� �˷��ݴϴ�.
	virtual void OnNetConnect(bool success, DWORD error) = 0;
	
	//������ ����Ÿ�� ������ ����� �˷��ݴϴ�.
	virtual void OnNetSend( int size ) = 0;
	
	//������ ������ ����Ǿ����� �߻��մϴ�.
	virtual void OnNetClose( DWORD error ) = 0;	
	
	//�α׸� ����Ҷ� ������ �̺�Ʈ �޼ҵ�
	virtual void WriteLog(char* szMsg) = 0;

private:
	//�α׸� ����Ѵ�.
	void WriteLog(const char* szFormat, ...);
};
