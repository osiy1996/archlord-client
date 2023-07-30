#pragma once
#pragma pack(1)

/**************************************************************************************************

�ۼ���: 2008-07-18
�ۼ���: ������ (youngmoon@webzen.co.kr)

����: �������� �������� ����

**************************************************************************************************/

#define MAX_ACCOUNTID							51
#define MAX_QUREY_LENGTH						1024
#define MAX_PCROOM_GAMECODE_LENGTH				3

#define PROTOCOL_CLIENT_LOGIN					0x01
#define PROTOCOL_SERVER_USERSTATUS				0x02
#define PROTOCOL_CLIENT_LOGOUT					0x03
#define PROTOCOL_CLIENT_INQUIRE					0x04
#define PROTOCOL_CLIENT_INQUIRE_MULTIUSER		0x05
#define PROTOCOL_SERVER_INQUIRE_MULTIUSER		0x06
#define PROTOCOL_CLIENT_INQUIRE_PERSON_DEDUCT	0x07
#define PROTOCOL_SERVER_INQUIRE_PERSON_DEDUCT	0x08
#define PROTOCOL_CLIENT_INQUIRE_PCROOM_DEDUCT	0x09
#define PROTOCOL_SERVER_INQUIRE_PCROOM_DEDUCT	0x0A

namespace BillingProtocol
{

template <class _subcls>
class MSG_BASE
{
public:
	MSG_BASE() {}
	MSG_BASE(DWORD dwID) : dwProtocolID(dwID), dwPacketSize(sizeof( _subcls )) {}
	
public:
	DWORD 	dwPacketSize; 					//��Ŷ�� ��ü ������
	DWORD	dwProtocolID;					//��Ŷ ���̵�
};

//(C->S) �α��� �Ҷ� ������ ��Ŷ
class MSG_CLIENT_LOGIN : public MSG_BASE<MSG_CLIENT_LOGIN>	
{
public:
	MSG_CLIENT_LOGIN() : MSG_BASE<MSG_CLIENT_LOGIN>(PROTOCOL_CLIENT_LOGIN), 
		dwAccountGUID(0), 
		dwIPAddress(0), 
		dwRoomGUID(0), 
		dwGameCode(0), 
		dwServerType(0)
	{}

public:
	DWORD 	dwAccountGUID;				 	//���̵� GUID	
	DWORD	dwIPAddress;					//Ŭ���̾�Ʈ ������ �ּ�
	DWORD 	dwRoomGUID;						//PC �� GUID
	DWORD	dwGameCode;						//���� �ڵ�
	DWORD	dwServerType;					//�׼����� ���̺����� ���� �ڵ�	
};

//(S->C) ���� ���� ���� ����
class MSG_SERVER_USERSTATUS : public MSG_BASE<MSG_SERVER_USERSTATUS>	
{
public:
	MSG_SERVER_USERSTATUS() : MSG_BASE<MSG_SERVER_USERSTATUS>(PROTOCOL_SERVER_USERSTATUS), 	
		dwAccountGUID(0), 
		dwBillingGUID(0), 
		RealEndDate(0), 
		EndDate(0), 
		dRestPoint(0), 
		dRestTime(0), 
		sDeductType(0), 
		sAccessCheck(0), 
		sResultCode(0)
	{}

public:
	DWORD 	dwAccountGUID;				 	//���̵� GUID	
	long	dwBillingGUID;					//�������� ����ϴ� GUID (�α׾ƿ��� ������� �Ѵ�.)
	DWORD	RealEndDate;					//�����ϰ�� ���� �������� (Ư�� �ð� ������ ��� ����)
	DWORD	EndDate;						//�����ϰ�� ���� ��������
	double	dRestPoint;						//������ ��� �ܿ�����Ʈ
	double	dRestTime;						//������ ��� �ð� (Sec)
	short	sDeductType;					//����� ���� ����
	short	sAccessCheck;					//���� üũ ���
	short	sResultCode;					//ó�� ��� �ڵ�
};

//(C->S) �α׾ƿ� �Ҷ� ������ ��Ŷ
class MSG_CLIENT_LOGOUT : public MSG_BASE<MSG_CLIENT_LOGOUT>
{
public:
	MSG_CLIENT_LOGOUT() : MSG_BASE<MSG_CLIENT_LOGOUT>(PROTOCOL_CLIENT_LOGOUT), 
		dwBillingGUID(0)
	{}

public:
	long	dwBillingGUID;					//�α��� ����� ���� GUID
};

//(C->S) ����� ���� ��ȸ
class MSG_CLIENT_INQUIRE : public MSG_BASE<MSG_CLIENT_INQUIRE>
{
public:
	MSG_CLIENT_INQUIRE() : MSG_BASE<MSG_CLIENT_INQUIRE>(PROTOCOL_CLIENT_INQUIRE), 	
		dwBillingGUID(0)
		{}

public:
	long	dwBillingGUID;					//�α��� ����� ���� GUID
};

//(C->S) ��Ƽ���� �������� ��ȸ
class MSG_CLIENT_INQUIRE_MULTIUSER : public MSG_BASE<MSG_CLIENT_INQUIRE_MULTIUSER>	
{
public:
	MSG_CLIENT_INQUIRE_MULTIUSER() : MSG_BASE<MSG_CLIENT_INQUIRE_MULTIUSER>(PROTOCOL_CLIENT_INQUIRE_MULTIUSER), 	
		dwRoomGUID(0)
		{}

public:
	DWORD   dwAccountID;
	DWORD	dwRoomGUID;						//���ӹ� ���� ��ȣ
};

//(S->C) ��Ƽ���� �������� ��ȸ
class MSG_SERVER_INQUIRE_MULTIUSER : public MSG_BASE<MSG_SERVER_INQUIRE_MULTIUSER>	
{
public:
	MSG_SERVER_INQUIRE_MULTIUSER() : MSG_BASE<MSG_SERVER_INQUIRE_MULTIUSER>(PROTOCOL_SERVER_INQUIRE_MULTIUSER), 	
		dwRoomGUID(0)
		{}

public:
	DWORD   dwAccountID;
	DWORD	dwRoomGUID;						//���ӹ� ���� ��ȣ
	long	result;							//��ȸ ���
};

//(C->S) ������ ���� �� ��� ������ ����,���� ���� ���θ� ��ȸ�Ѵ�.
class MSG_CLIENT_INQUIRE_PERSON_DEDUCT : public MSG_BASE<MSG_CLIENT_INQUIRE_PERSON_DEDUCT>	
{
public:
	MSG_CLIENT_INQUIRE_PERSON_DEDUCT() 
		: MSG_BASE<MSG_CLIENT_INQUIRE_PERSON_DEDUCT>(PROTOCOL_CLIENT_INQUIRE_PERSON_DEDUCT), 
		dwAccountGUID(0), dwGameCode(0)
		{
		}

public:
	DWORD	dwAccountGUID;
	DWORD	dwGameCode;
};

//(S->C) ������ ���� �� ��� ������ ����,���� ���� ���θ� ��ȸ�Ѵ�.
class MSG_SERVER_INQUIRE_PERSON_DEDUCT : public MSG_BASE<MSG_SERVER_INQUIRE_PERSON_DEDUCT>	
{
public:
	MSG_SERVER_INQUIRE_PERSON_DEDUCT() 
		: MSG_BASE<MSG_SERVER_INQUIRE_PERSON_DEDUCT>(PROTOCOL_SERVER_INQUIRE_PERSON_DEDUCT), 	
		dwAccountGUID(0), dwGameCode(0)
		{
		}

public:
	DWORD	dwAccountGUID;
	DWORD	dwGameCode;
	long	ResultCode;
};

//(C->S) Ư�� PC���� ���� �� ���� ���� ���θ� ��ȸ�Ѵ�.
class MSG_CLIENT_INQUIRE_PCROOM_DEDUCT : public MSG_BASE<MSG_CLIENT_INQUIRE_PCROOM_DEDUCT>
{
public:
	MSG_CLIENT_INQUIRE_PCROOM_DEDUCT() 
		: MSG_BASE<MSG_CLIENT_INQUIRE_PCROOM_DEDUCT>(PROTOCOL_CLIENT_INQUIRE_PCROOM_DEDUCT), 
		dwAccountGUID(0), dwRoomGUID(0), dwGameCode(0)
		{
		}

public:
	DWORD	dwAccountGUID;
	DWORD   dwRoomGUID;
	DWORD	dwGameCode;
};

//(S->C) Ư�� PC���� ���� �� ���� ���� ���θ� ��ȸ�Ѵ�.
class MSG_SERVER_INQUIRE_PCROOM_DEDUCT: public MSG_BASE<MSG_SERVER_INQUIRE_PCROOM_DEDUCT>
{
public:
	MSG_SERVER_INQUIRE_PCROOM_DEDUCT() 
		: MSG_BASE<MSG_SERVER_INQUIRE_PCROOM_DEDUCT>(PROTOCOL_SERVER_INQUIRE_PCROOM_DEDUCT),
		dwAccountGUID(0), dwRoomGUID(0), dwGameCode(0)
		{
		}

public:
	DWORD	dwAccountGUID;
	DWORD   dwRoomGUID;
	DWORD	dwGameCode;
	long	ResultCode;
};

}

#pragma pack()