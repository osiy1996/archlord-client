#pragma once
#pragma pack(1)

/**************************************************************************************************

�ۼ���: 2010.03.25
�ۼ���: ������

��  ��: ������ �������� ����

**************************************************************************************************/

#include "ShopDefine.h"

namespace ShopProtocol
{

//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// ����ü ///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

// ���� ĳ�� ���� ���� ����
struct STCashDetail 
{
	STCashDetail()
	{
		ZeroMemory(Name, MAX_TYPENAME_LENGTH * sizeof(WCHAR));
		Value = 0;			
		Type = 0;			
	};
	
	WCHAR	Name[MAX_TYPENAME_LENGTH];	// ĳ������ �̳� ����Ʈ���� �̸�
	double	Value;						// ĳ���� ����Ʈ ��
	char	Type;						// ĳ������ ����Ʈ ���� ���� (C:ĳ��, P:����Ʈ 0:����)
};

// �۷ι� ĳ�� ���� ���� ����
struct STCashDetail_GB
{
	STCashDetail_GB()
	{
		ZeroMemory(Name, MAX_TYPENAME_LENGTH * sizeof(WCHAR));	
		Value = 0;			
		Type = 0;			
		CashTypeCode = 0;
	};
	
	WCHAR	Name[MAX_TYPENAME_LENGTH];	// ĳ������ �̳� ����Ʈ���� �̸�
	double	Value;						// ĳ���� ����Ʈ ��
	char	Type;						// ĳ������ ����Ʈ ���� ���� (C:ĳ��, P:����Ʈ 0:����)
	long	CashTypeCode;				// ĳ�� Ÿ�� �ڵ�
};

// ����Ʈ ���� ���� ����
struct STPointDetail
{
	STPointDetail()
	{
		ZeroMemory(PointTypeName, MAX_POINTNAME_LENGTH);
		PointType = PointValue = 0;
	};

	char	PointTypeName[MAX_POINTNAME_LENGTH];
	long	PointType;
	long	PointValue;
};

// ������ �Ӽ�
struct STItemProperty
{
	STItemProperty()
	{
		PropertySeq = 0;
		Value = 0;
	};

	long	PropertySeq;
	long	Value;
};

//������ ���� ������ ����
struct STStorage 
{
	STStorage()
	{
		Seq = ItemSeq = GroupCode = ShareFlag = ProductSeq = PriceSeq = ProductType = 0;
		CashPoint = 0;
		ItemType = 0;
		RelationType = 0;
		ZeroMemory(CashName, MAX_TYPENAME_LENGTH * sizeof(WCHAR));
		ZeroMemory(SendAccountID, MAX_ACCOUNTID_LENGTH * sizeof(WCHAR));
		ZeroMemory(SendMessage, MAX_MESSAGE_LENGTH * sizeof(WCHAR));
	};

	long	Seq;								// ������ ����
	long	ItemSeq;							// ������ ��ǰ or ĳ�� �׸� ����
	long	GroupCode;							// ������ �׷��ڵ�
	long	ShareFlag;							// ServerType(��������) �� ������ �׸� ��ȸ ���� �׸� ����
												/*	435	Ŭ���� ���̺� (��������-���Ἥ��)
													436	Ŭ���� �׽�Ʈ (��������-�׽�Ʈ����)
													470	ī���� ���̺� (�κ�����-���Ἥ��)
													471	ī���� �׽�Ʈ (�κ�����-�׽�Ʈ����)
													550	�佽�� ���̺�*/

	long	ProductSeq;							// ������ǰ �ڵ�
	WCHAR	CashName[MAX_TYPENAME_LENGTH];		// ĳ�� ��
	double	CashPoint;							// ���� ĳ�� ����Ʈ
	WCHAR	SendAccountID[MAX_ACCOUNTID_LENGTH];// �߽��� ȸ�� ���̵�
	WCHAR	SendMessage[MAX_MESSAGE_LENGTH];	// �߽� �޽��� - ������ ��� ����
	char	ItemType;							// ��ǰ ĳ�� ���� (P:��ǰ, C:ĳ��)
	BYTE	RelationType;						// ������ ���� ���� (1:����, �ø���, �������� , 2:����)
	long	PriceSeq;
	long	ProductType;						// ���� ��ǰ ���� - 406�� ��� ���� ������ ������ ����ϱ� �Ұ�
};

//������ ���� ������ ����
struct STStorageNoGiftMessage
{
	STStorageNoGiftMessage()
	{
		Seq = ItemSeq = GroupCode = ShareFlag = ProductSeq = PriceSeq = ProductType = 0;
		CashPoint = 0;
		ItemType = 0;
		RelationType = 0;
		ZeroMemory(CashName, MAX_TYPENAME_LENGTH * sizeof(WCHAR));
		ZeroMemory(SendAccountID, MAX_ACCOUNTID_LENGTH * sizeof(WCHAR));
	};

	long	Seq;								// ������ ����
	long	ItemSeq;							// ������ ��ǰ or ĳ�� �׸� ����
	long	GroupCode;							// ������ �׷��ڵ�
	long	ShareFlag;							// ServerType(��������) �� ������ �׸� ��ȸ ���� �׸� ����
	long	ProductSeq;							// ������ǰ �ڵ�
	WCHAR	CashName[MAX_TYPENAME_LENGTH];		// ĳ�� ��
	double	CashPoint;							// ���� ĳ�� ����Ʈ
	WCHAR	SendAccountID[MAX_ACCOUNTID_LENGTH];// �߽��� ȸ�� ���̵�
	char	ItemType;							// ��ǰ ĳ�� ���� (P:��ǰ, C:ĳ��)
	BYTE	RelationType;						// ������ ���� ���� (1:����, �ø���, �������� , 2:����)
	long	PriceSeq;
	long	ProductType;						// ���� ��ǰ ���� - 406�� ��� ���� ������ ������ ����ϱ� �Ұ�
};

// ���� ���� Ÿ�� - ��ü����, ������, ijji.com ...
enum PaymentType
{
	Inbound,
	Outbound_HanCoin,
	Outbound_Ijji,
};


//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// �޽��� ���̽� ///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

// �޽��� ���̽�
template <class _subcls>
class MSG_REQUEST_BASE
{
public:
	MSG_REQUEST_BASE() {}
	MSG_REQUEST_BASE(DWORD dwID) 
		: dwProtocolID(dwID),
		  GameCode(0),
		  dwPacketSize(sizeof( _subcls ))
	{}

	long	GetGameCode()	{ return GameCode; }
	
	DWORD 	dwPacketSize; 					// ��Ŷ�� ��ü ������
	DWORD	dwProtocolID;					// ��Ŷ ���̵�
	long	GameCode;						// ���� �ڵ�
};

// ���� �޽��� ���̽�
template <class _subcls>
class MSG_RESPONSE_BASE : public MSG_REQUEST_BASE<_subcls>	
{
public:
	MSG_RESPONSE_BASE(DWORD dwProtocol) 
		: MSG_REQUEST_BASE<_subcls>(dwProtocol)
	{
		ResultCode = 0;
	}
	
public:
	long ResultCode;
};


//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// �۷ι� - ��ũ�ε� ////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////// ĳ�� ��ȸ /////////////////////////////////

// C->S : �۷ι� - ��ũ�ε�
class MSG_CLIENT_INQUIRE_CASH_AR : public MSG_REQUEST_BASE<MSG_CLIENT_INQUIRE_CASH_AR>
{
public:
	MSG_CLIENT_INQUIRE_CASH_AR(long nGameCode)
		: MSG_REQUEST_BASE<MSG_CLIENT_INQUIRE_CASH_AR>(PROTOCOL_CLIENT_INQUIRE_CASH)
	{
		GameCode = nGameCode;
		ViewType = 0;
		AccountSeq = 0;
		SumOnly = false;
		ZeroMemory(AccountID, MAX_ACCOUNT_LENGTH);
	}

public:
	BYTE	ViewType;							//��ȸ ���� (0:��ü(ĳ��+����Ʈ), 1:ĳ��(�Ϲ�+�̺�Ʈ), 2:����Ʈ(���ʽ�+�Ϲ�), 3:���ʽ�����Ʈ, 4:��������Ʈ, 5:ĳ��(�Ϲ�))
	char	AccountID[MAX_ACCOUNT_LENGTH];
	DWORD	AccountSeq;							//��������
	bool	SumOnly;							//�� �հ踸 ��ȸ ���� (true: ������ ���� ����, false: ������ ���� + ĳ�� ������ ���(STDetail �� ���))
};

// S->C : �۷ι� - ��ũ�ε�
class MSG_SERVER_INQUIRE_CASH_AR : public MSG_RESPONSE_BASE<MSG_SERVER_INQUIRE_CASH_AR>
{
public:
	MSG_SERVER_INQUIRE_CASH_AR(long nGameCode)
		: MSG_RESPONSE_BASE<MSG_SERVER_INQUIRE_CASH_AR>(PROTOCOL_SERVER_INQUIRE_CASH)	  
	{
		GameCode = nGameCode;
		PackHeader = dwPacketSize;
		AccountSeq = 0;
		sum[0] = sum[1] = 0;
		ListCount = 0;
		ZeroMemory(AccountID, MAX_ACCOUNT_LENGTH);
	}

public:
	long	PackHeader;
	char	AccountID[MAX_ACCOUNT_LENGTH];
	DWORD	AccountSeq;							// ��������
	double	sum[2];								// ������ ���� ���� (0: ĳ�� ����, 1: ����Ʈ ����)
	int		ListCount;
};	

///////////////////////////////// ��ǰ ���� /////////////////////////////////

// C->S : �۷ι� - ��ũ�ε�
class MSG_CLIENT_BUYPRODUCT_AR : public MSG_REQUEST_BASE<MSG_CLIENT_BUYPRODUCT_AR>
{
public:
	MSG_CLIENT_BUYPRODUCT_AR(long nGameCode)
		: MSG_REQUEST_BASE<MSG_CLIENT_BUYPRODUCT_AR>(PROTOCOL_CLIENT_BUYPRODUCT)
	{
		GameCode = nGameCode;
		AccountSeq = 0;
		ProductSeq = Class = Level = ServerIndex = SalesZone = CashTypeCode = 0;
		DeductPrice = 0;
		ZeroMemory(AccountID, MAX_ACCOUNT_LENGTH);
		ZeroMemory(CharName, MAX_CHARNAME_LENGTH * sizeof(WCHAR));	
		ZeroMemory(ProductName, MAX_PRODUCTNAME_LENGTH * sizeof(WCHAR));	
	}

public:
	char	AccountID[MAX_ACCOUNT_LENGTH];
	DWORD	AccountSeq;								// ���� ����
	int		ProductSeq;								// ��ǰ ����
	WCHAR	ProductName[MAX_PRODUCTNAME_LENGTH];	// ��ǰ �̸�
	int		InGamePurchaseSeq;						// ���� �� ��ǰ ���Ź�ȣ
	int		Class;									// Ŭ���� �ڵ�
	int		Level;									// ���� �ڵ�
	WCHAR	CharName[MAX_CHARNAME_LENGTH];			// ĳ���� ��
	int		ServerIndex;							// ���� �ε���
	int		SalesZone;								// �Ǹ� ����
	double	DeductPrice;							// ���� ĳ�� ����
	char	DeductType;								// ���� ����
	int		CashTypeCode;							// ĳ�� ���� �ڵ�
};

// S->C : �۷ι� - ��ũ�ε�
class MSG_SERVER_BUYPRODUCT_AR : public MSG_RESPONSE_BASE<MSG_SERVER_BUYPRODUCT_AR>
{
public:
	MSG_SERVER_BUYPRODUCT_AR(long nGameCode)
		: MSG_RESPONSE_BASE<MSG_SERVER_BUYPRODUCT_AR>(PROTOCOL_SERVER_BUYPRODUCT)
	{
		GameCode = nGameCode;
		AccountSeq = 0;
		DeductCashSeq = 0;
		InGamePurchaseSeq = 0;
		ZeroMemory(AccountID, MAX_ACCOUNT_LENGTH);
	}

public:
	char	AccountID[MAX_ACCOUNT_LENGTH];
	DWORD	AccountSeq;							// ��������
	long	DeductCashSeq;						// �� ��ǰ ���ſ� ���� ���� ��ȣ (ResultCode != �̸� -1 �� ����)	
	int		InGamePurchaseSeq;					// ���� �� ��ǰ ���Ź�ȣ
};

///////////////////////////////// ��ǰ ���� /////////////////////////////////

// C->S : �۷ι� - ��ũ�ε�
class MSG_CLIENT_GIFTPRODUCT_AR : public MSG_REQUEST_BASE<MSG_CLIENT_GIFTPRODUCT_AR>
{
public:
	MSG_CLIENT_GIFTPRODUCT_AR(long nGameCode) 
		: MSG_REQUEST_BASE<MSG_CLIENT_GIFTPRODUCT_AR>(PROTOCOL_CLIENT_GIFTPRODUCT)		
	{
		GameCode = nGameCode;
		SenderSeq = ReceiverSeq = ProductSeq = InGamePurchaseSeq = SalesZone = 0;
		SendClass = SendLevel = SendServerIndex = RevClass = RevLevel = RevServerIndex = CashTypeCode = 0;
		DeductPrice = 0;
		DeductType = 0;
		ZeroMemory(SenderID, MAX_ACCOUNT_LENGTH); 
		ZeroMemory(ReceiverID, MAX_ACCOUNT_LENGTH);
		ZeroMemory(Message, MAX_MESSAGE_LENGTH * sizeof(WCHAR));
		ZeroMemory(SendCharName, MAX_CHARNAME_LENGTH * sizeof(WCHAR));
		ZeroMemory(RevCharName, MAX_CHARNAME_LENGTH * sizeof(WCHAR));
		ZeroMemory(ProductName, MAX_PRODUCTNAME_LENGTH * sizeof(WCHAR));
	}

public:
	char	SenderID[MAX_ACCOUNT_LENGTH];
	DWORD	SenderSeq;								// �߽��� ����
	char	ReceiverID[MAX_ACCOUNT_LENGTH];
	DWORD	ReceiverSeq;							// ������ ����
	WCHAR	Message[MAX_MESSAGE_LENGTH];			// �޽���
	DWORD	ProductSeq;								// ��ǰ ����
	WCHAR	ProductName[MAX_PRODUCTNAME_LENGTH];	// ��ǰ ��
	DWORD	InGamePurchaseSeq;						// ���� �� ��ǰ ���Ź�ȣ
	int		SendClass;								// �߽��� Ŭ���� �ڵ�
	int		SendLevel;								// �߽��� ���� �ڵ�
	WCHAR	SendCharName[MAX_CHARNAME_LENGTH];		// �߽��� ĳ���͸�
	int		SendServerIndex;						// ���� ��� ���� �ε���
	int		RevClass;								// ������ Ŭ���� �ڵ�
	int		RevLevel;								// ������ ���� �ڵ�
	WCHAR	RevCharName[MAX_CHARNAME_LENGTH];		// ������ ĳ���͸�
	int		RevServerIndex;							// �޴� ��� ���� �ε���
	DWORD	SalesZone;								// �Ǹ� ����
	double	DeductPrice;							// ���� ĳ�� ����
	char	DeductType;								// ���� ����
	int		CashTypeCode;							// ĳ�� ���� �ڵ�
};

// S->C : �۷ι� - ��ũ�ε�
class MSG_SERVER_GIFTPRODUCT_AR : public MSG_RESPONSE_BASE<MSG_SERVER_GIFTPRODUCT_AR>
{
public:
	MSG_SERVER_GIFTPRODUCT_AR(long nGameCode)
		: MSG_RESPONSE_BASE<MSG_SERVER_GIFTPRODUCT_AR>(PROTOCOL_SERVER_GIFTPRODUCT)
	{
		GameCode = nGameCode;
		SenderSeq = ReceiverSeq = 0;
		DeductCashSeq = 0;
		ZeroMemory(SenderID, MAX_ACCOUNT_LENGTH); 
		ZeroMemory(ReceiverID, MAX_ACCOUNT_LENGTH);
	}

public:
	char	SenderID[MAX_ACCOUNT_LENGTH];
	char	ReceiverID[MAX_ACCOUNT_LENGTH];
	DWORD	SenderSeq;							// ��������
	DWORD	ReceiverSeq;						// ��������
	long	DeductCashSeq;						// �� ��ǰ ���ſ� ���� ���� ��ȣ (ResultCode != �̸� -1 �� ����)
};


//////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// ���� - �� /////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////// ĳ�� ��ȸ /////////////////////////////////

// C->S : ���� - ��
class MSG_CLIENT_INQUIRE_CASH_KR : public MSG_REQUEST_BASE<MSG_CLIENT_INQUIRE_CASH_KR>	
{
public:
	MSG_CLIENT_INQUIRE_CASH_KR(long nGameCode) 
		: MSG_REQUEST_BASE<MSG_CLIENT_INQUIRE_CASH_KR>(PROTOCOL_CLIENT_INQUIRE_CASH)
	{ 
		GameCode = nGameCode; 
		ViewType = 0;
		AccountSeq = 0;
		SumOnly = false;
		MileageSection = 0;
	}

public:
	BYTE	ViewType;					// ��ȸ ���� (	0:��ü(ĳ��+����Ʈ), 
										//				1:ĳ��(�Ϲ�+�̺�Ʈ), 
										//				2:����Ʈ(���ʽ�+����), 
										//				3:���ʽ�����Ʈ, 
										//				4:��������Ʈ, 
										//				5:ĳ��(�Ϲ�), 
										//				6:ĳ��(�̺�Ʈ),
										//				7:���ϸ���)
	DWORD	AccountSeq;					// ��������	
	bool	SumOnly;					// �� �հ踸 ��ȸ ���� (	true  : ������ ���� ����, 
										//							false : ������ ���� + ĳ�� ������ ���(STDetail �� ���))
	long	MileageSection;				// ���ϸ��� ���� (438:���, 439:QT)
};

// S->C : ���� - ��
class MSG_SERVER_INQUIRE_CASH_KR : public MSG_RESPONSE_BASE<MSG_SERVER_INQUIRE_CASH_KR>
{
public:
	MSG_SERVER_INQUIRE_CASH_KR(long nGameCode) 
		: MSG_RESPONSE_BASE<MSG_SERVER_INQUIRE_CASH_KR>(PROTOCOL_SERVER_INQUIRE_CASH)
	{
		GameCode = nGameCode;
		PackHeader = dwPacketSize;
		AccountSeq = 0;
		ListCount = 0;
		sum[0] = 0;					// ĳ��(�Ϲ�, �̺�Ʈ) ����
		sum[1] = 0;					// ����Ʈ(���ʽ�, ����) ����
		sum[2] = 0;					// ���ϸ���(���, QT) ����
	}

public:
	DWORD	AccountSeq;				// ��������
	long	PackHeader;	
	int		ListCount;
	double	sum[3];					// ������ ���� ���� (0: ĳ�� ����, 1: ����Ʈ ����, 2: ���ϸ��� ����)	
};	


///////////////////////////////// ��ǰ ���� /////////////////////////////////

// C->S : ���� - ��
class MSG_CLIENT_BUYPRODUCT_KR : public MSG_REQUEST_BASE<MSG_CLIENT_BUYPRODUCT_KR>
{
public:
	MSG_CLIENT_BUYPRODUCT_KR(long nGameCode) 
		: MSG_REQUEST_BASE<MSG_CLIENT_BUYPRODUCT_KR>(PROTOCOL_CLIENT_BUYPRODUCT)
	{
		GameCode = nGameCode;
		AccountSeq = 0;
		PackageSeq = PriceSeq = SalesZone = DisplaySeq = Class = Level = ServerIndex = 0;
		ZeroMemory(CharName, MAX_CHARACTERID_LENGTH * sizeof(WCHAR));
		ZeroMemory(Rank, MAX_RANK_LENGTH * sizeof(WCHAR));
	}

public:
	DWORD	AccountSeq;							// ��������	
	long	PackageSeq;							// ���� ��ǰ ����
	long	PriceSeq;							// ���� ����	
	long	SalesZone;							// �Ǹſ���
	long	DisplaySeq;							// �������� ����
	long	Class;								// Ŭ����
	long	Level;								// ����
	WCHAR	CharName[MAX_CHARACTERID_LENGTH];	// ĳ���� ��
	WCHAR	Rank[MAX_RANK_LENGTH];				// ��ũ
	long	ServerIndex;						// ���� �ε���
};

// S->C : ���� - ��
class MSG_SERVER_BUYPRODUCT_KR : public MSG_RESPONSE_BASE<MSG_SERVER_BUYPRODUCT_KR>
{
public:
	MSG_SERVER_BUYPRODUCT_KR(long nGameCode)
		: MSG_RESPONSE_BASE<MSG_SERVER_BUYPRODUCT_KR>(PROTOCOL_SERVER_BUYPRODUCT)
	{
		GameCode = nGameCode;
		AccountSeq = 0;
		LeftCount = 0;
	}

public:
	DWORD	AccountSeq;					// ��������
	long	LeftCount;					// ��ǰ �Ǹ� ���� �ܿ����� (-1: ������) ��ǰ ���� ������ �����ؾ��Ѵ�.
	
	/*	��� �ڵ� 
		-2 : DB ó�� ����(���������� �������� ����)
		 0 : ����
		 1 : ����ĳ�ú���
		 2 : ���� ��ǰ �����ϱ� �Ұ���
		 3 : ���� �����ϱ� �ѵ� �ʰ�
		 4 : �ܿ���������
		 5 : �ǸűⰣ����
		 6 : �Ǹ�����(��ǰ������ ���� ���)
		 7 : ��ǰ���� �Ұ���
		 8 : �̺�Ʈ��ǰ ���� �Ұ�
		 9 : �̺�Ʈ ��ǰ ���� ���� Ƚ�� �ʰ�
		-1 : ���� �߻�
	*/
};

///////////////////////////////// ��ǰ ���� /////////////////////////////////

// C->S : ���� - ��
class MSG_CLIENT_GIFTPRODUCT_KR : public MSG_REQUEST_BASE<MSG_CLIENT_GIFTPRODUCT_KR>
{
public:
	MSG_CLIENT_GIFTPRODUCT_KR(long nGameCode)
		: MSG_REQUEST_BASE<MSG_CLIENT_GIFTPRODUCT_KR>(PROTOCOL_CLIENT_GIFTPRODUCT)
	{
		GameCode = nGameCode; 
		SenderSeq = ReceiverSeq = 0;
		SenderServerIndex = ReceiverServerIndex = PackageSeq = PriceSeq = SalesZone = DisplaySeq = 0;
		ZeroMemory(Message, MAX_MESSAGE_LENGTH * sizeof(WCHAR));
		ZeroMemory(SenderCharName, MAX_CHARACTERID_LENGTH * sizeof(WCHAR));
		ZeroMemory(ReceiverCharName, MAX_CHARACTERID_LENGTH * sizeof(WCHAR));
	}

public:
	DWORD	SenderSeq;									// �߽��� ����
	long	SenderServerIndex;							// �߽��� ���� ��ȣ
	WCHAR	SenderCharName[MAX_CHARACTERID_LENGTH];		// �߽��� ĳ���� ��
	DWORD	ReceiverSeq;								// ������ ����
	long	ReceiverServerIndex;						// ������ ���� ��ȣ
	WCHAR	ReceiverCharName[MAX_CHARACTERID_LENGTH];	// ������ ĳ���� ��
	WCHAR	Message[MAX_MESSAGE_LENGTH];				// �޽���	
	long	PackageSeq;									// ���� ��ǰ ����
	long	PriceSeq;									// ���� ����	
	long	SalesZone;									// �Ǹſ���
	long	DisplaySeq;									// �������� ����
};

// S->C : ���� - ��
class MSG_SERVER_GIFTPRODUCT_KR : public MSG_RESPONSE_BASE<MSG_SERVER_GIFTPRODUCT_KR>
{
public:
	MSG_SERVER_GIFTPRODUCT_KR(long nGameCode) 
		: MSG_RESPONSE_BASE<MSG_SERVER_GIFTPRODUCT_KR>(PROTOCOL_SERVER_GIFTPRODUCT)
	{
		GameCode = nGameCode;
		SenderSeq = ReceiverSeq = 0;
		LeftProductCount = 0;
		LimitedCash = 0;		
	}

public:
	DWORD	SenderSeq;					// ���� ����
	DWORD	ReceiverSeq;				// ���� ����
	double	LimitedCash;				// ���� ���� ĳ��
	long	LeftProductCount;
};


//////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// ����- �佽�� ///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////// ĳ�� ��ȸ /////////////////////////////////

// C->S : ����- �佽��
class MSG_CLIENT_INQUIRE_CASH_OB : public MSG_REQUEST_BASE<MSG_CLIENT_INQUIRE_CASH_OB>	
{
public:
	MSG_CLIENT_INQUIRE_CASH_OB(long nGameCode) 
		: MSG_REQUEST_BASE<MSG_CLIENT_INQUIRE_CASH_OB>(PROTOCOL_CLIENT_INQUIRE_CASH)
	{
		GameCode = nGameCode;
		AccountSeq = 0;
		SumOnly = true;
		PaymentType = Inbound;
		MileageSection = 0;
		ZeroMemory(AccountID, MAX_ACCOUNTID_LENGTH);
	}

public:
	PaymentType	PaymentType;						// ���� Ÿ��
	DWORD		AccountSeq;							// ��������	
	char		AccountID[MAX_ACCOUNTID_LENGTH];	// ���� ����
	BYTE		ViewType;							// 7:���ϸ����� ����
	long		MileageSection;						// ���ϸ��� ���� (438:���, 439:QT)
	bool		SumOnly;							// true�� ����
};

// S->C : ����- �佽��
class MSG_SERVER_INQUIRE_CASH_OB : public MSG_RESPONSE_BASE<MSG_SERVER_INQUIRE_CASH_OB>
{
public:
	MSG_SERVER_INQUIRE_CASH_OB(long nGameCode) 
		: MSG_RESPONSE_BASE<MSG_SERVER_INQUIRE_CASH_OB>(PROTOCOL_SERVER_INQUIRE_CASH)
	{	
		GameCode = nGameCode;
		PackHeader = dwPacketSize;
		AccountSeq = 0;
		ListCount = 0;
		sum[0] = 0;								// ĳ��(�Ϲ�, �̺�Ʈ) ����
		sum[1] = 0;								// ����Ʈ(���ʽ�, ����) ����
		sum[2] = 0;								// ���ϸ���(���, QT) ����
		ZeroMemory(AccountID, MAX_ACCOUNTID_LENGTH);
		OutBoundResultCode = 0;
	}

public:
	DWORD	AccountSeq;							// ��������
	char	AccountID[MAX_ACCOUNTID_LENGTH];	// ���� ����
	long	PackHeader;	
	int		ListCount;	
	double	sum[3];								// ������ ���� ���� (0: ĳ�� ����, 1: ����Ʈ ����)	
	long	OutBoundResultCode;					// �ܺ� ��� ��� �ڵ�
};	


///////////////////////////////// ��ǰ ���� /////////////////////////////////

// C->S : ����- �佽��
class MSG_CLIENT_BUYPRODUCT_OB : public MSG_REQUEST_BASE<MSG_CLIENT_BUYPRODUCT_OB>
{
public:
	MSG_CLIENT_BUYPRODUCT_OB(long nGameCode)
		: MSG_REQUEST_BASE<MSG_CLIENT_BUYPRODUCT_OB>(PROTOCOL_CLIENT_BUYPRODUCT)
	{
		GameCode = nGameCode;
		AccountSeq = dwIPAddress = 0;
		PaymentType = Inbound;
		PackageSeq = PriceSeq = SalesZone = ProductDisplaySeq = Class = Level = ServerIndex = 0;
		ZeroMemory(AccountID, MAX_ACCOUNTID_LENGTH);
		ZeroMemory(RefKey, MAX_TYPENAME_LENGTH);
		ZeroMemory(CharName, MAX_CHARACTERID_LENGTH * sizeof(WCHAR));
		ZeroMemory(Rank, MAX_RANK_LENGTH * sizeof(WCHAR));
		DeductMileageFlag = false;
	}

public:
	PaymentType	PaymentType;						// ���� Ÿ��
	DWORD		AccountSeq;							// ���� ����	
	char		AccountID[MAX_ACCOUNTID_LENGTH];	// ���� ����
	WCHAR		CharName[MAX_CHARACTERID_LENGTH];	// ĳ���� ��
	long		ServerIndex;						// ���� �ε���
	DWORD		dwIPAddress;						// ������ ����
	long		PackageSeq;							// ���� ��ǰ ����
	long		PriceSeq;							// ���� ����	
	long		SalesZone;							// �Ǹ� ����	
	long		ProductDisplaySeq;
	long		Class;								// Ŭ����
	long		Level;								// ����
	WCHAR		Rank[MAX_RANK_LENGTH];				// ��ũ
	char		RefKey[MAX_TYPENAME_LENGTH];		// ���� �ý��� �߱� �ֹ���ȣ
	bool		DeductMileageFlag;					// ���ϸ����� �����ϴ� ��ǰ���� ���� (false : �Ϲ�, true : ���ϸ��� ����)
};

// S->C : ����- �佽��
class MSG_SERVER_BUYPRODUCT_OB : public MSG_RESPONSE_BASE<MSG_SERVER_BUYPRODUCT_OB>
{
public:
	MSG_SERVER_BUYPRODUCT_OB(long nGameCode)
		: MSG_RESPONSE_BASE<MSG_SERVER_BUYPRODUCT_OB>(PROTOCOL_SERVER_BUYPRODUCT)
	{
		GameCode = nGameCode;
		AccountSeq = 0;
		LeftCount = 0;
		ZeroMemory(AccountID, MAX_ACCOUNTID_LENGTH);
		OutBoundResultCode = 0;
	}

public:
	DWORD	AccountSeq;						// ��������
	char	AccountID[MAX_ACCOUNTID_LENGTH];// ���� ����
	long	LeftCount;						// ��ǰ �Ǹ� ���� �ܿ����� (-1: ������) ��ǰ ���� ������ �����ؾ��Ѵ�.
	long	OutBoundResultCode;				// �ܺ� ��� ��� �ڵ�
	
	/*	��� �ڵ�
		-1 ~ -100   : ������ ��� ���� �ڵ�
		-996 ~ -999 : DB ó�� ����

		0 : ����
		1 : ���� ĳ�� ����
		2 : (����)��ǰ ���� �Ұ���
		3 : ���Ǽ����ϱ��ѵ��ʰ�
		4 : �ܿ���������
		5 : �ǸűⰣ����
		6 : �Ǹ�����(��ǰ���������°��)
		7 : ��ǰ�����Ұ���
		8 : �̺�Ʈ��ǰ�����Ұ�
		9 : �̺�Ʈ��ǰ��������Ƚ���ʰ�

		***** ���� ������ ��� ��� *****
		101	: ��ȿ���� ���� �Ķ����
		102	: ��ȿ���� ���� ����� ����
		103	: ��ȿ���� ���� PG(Payment Gateway)
		104	: ��ȿ���� ���� ������
		105	: ������ �ݾ��� 0���� ũ�� ����
		106	: ���°� �������� ����
		107	: �ܾ� ����
		108	: �ܾ� ���� ����
		199	: ������ DB ���� 
	*/
};


///////////////////////////////// ��ǰ ���� /////////////////////////////////

// C->S : ����- �佽��

class MSG_CLIENT_GIFTPRODUCT_OB : public MSG_REQUEST_BASE<MSG_CLIENT_GIFTPRODUCT_OB>
{
public:
	MSG_CLIENT_GIFTPRODUCT_OB(long nGameCode)
		: MSG_REQUEST_BASE<MSG_CLIENT_GIFTPRODUCT_OB>(PROTOCOL_CLIENT_GIFTPRODUCT)
	{
		GameCode = nGameCode; 
		AccountSeq = ReceiverAccountSeq = 0;
		PaymentType = Inbound;
		ServerIndex = dwIPAddress = ReceiverServerIndex = PackageSeq = PriceSeq = SalesZone = ProductDisplaySeq = 0;
		ZeroMemory(AccountID, MAX_ACCOUNTID_LENGTH);
		ZeroMemory(ReceiverAccountID, MAX_ACCOUNTID_LENGTH);
		ZeroMemory(RefKey, MAX_TYPENAME_LENGTH);
		ZeroMemory(CharName, MAX_CHARACTERID_LENGTH * sizeof(WCHAR));
		ZeroMemory(ReceiverCharName, MAX_CHARACTERID_LENGTH * sizeof(WCHAR));
		ZeroMemory(Message, MAX_MESSAGE_LENGTH * sizeof(WCHAR));
		DeductMileageFlag = false;
	}

public:
	PaymentType	PaymentType;								// ���� Ÿ��
	DWORD		AccountSeq;									// �߽��� ����
	char		AccountID[MAX_ACCOUNTID_LENGTH];			// ���� ����
	WCHAR		CharName[MAX_CHARACTERID_LENGTH];			// �߽��� ĳ���� ��
	long		ServerIndex;								// �߽��� ���� ��ȣ
	DWORD		ReceiverAccountSeq;							// ������ ����
	char		ReceiverAccountID[MAX_ACCOUNTID_LENGTH];	// ������ ���� ��
	WCHAR		ReceiverCharName[MAX_CHARACTERID_LENGTH];	// ������ ĳ���� ��
	long		ReceiverServerIndex;						// ������ ���� ��ȣ
	DWORD		dwIPAddress;								// ����������
	long		PackageSeq;									// ���� ��ǰ ����
	long		PriceSeq;									// ���� ����	
	long		SalesZone;									// �Ǹſ���	
	long		ProductDisplaySeq;
	WCHAR		Message[MAX_MESSAGE_LENGTH];				// �޽���	
	char		RefKey[MAX_TYPENAME_LENGTH];				// ���� �ý��� �߱� �ֹ���ȣ
	bool		DeductMileageFlag;							// ���ϸ����� �����ϴ� ��ǰ���� ���� (false : �Ϲ�, true : ���ϸ��� ����)
};
// S->C : ����- �佽��

class MSG_SERVER_GIFTPRODUCT_OB : public MSG_RESPONSE_BASE<MSG_SERVER_GIFTPRODUCT_OB>
{
public:
	MSG_SERVER_GIFTPRODUCT_OB(long nGameCode) 
		: MSG_RESPONSE_BASE<MSG_SERVER_GIFTPRODUCT_OB>(PROTOCOL_SERVER_GIFTPRODUCT)
	{
		GameCode = nGameCode;
		AccountSeq = ReceiverAccountSeq = 0;
		LeftProductCount = 0;
		LimitedCash = 0;
		ZeroMemory(AccountID, MAX_ACCOUNTID_LENGTH);
		OutBoundResultCode = 0;
	}

public:
	DWORD	AccountSeq;						// ��������
	char	AccountID[MAX_ACCOUNTID_LENGTH];// ���� ����
	DWORD	ReceiverAccountSeq;				// ��������
	double	LimitedCash;					// ���� ���� ĳ��
	long	LeftProductCount;
	long	OutBoundResultCode;				// �ܺ� ��� ��� �ڵ�
};


//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// �۷ι� - �� ////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////// ĳ�� ��ȸ /////////////////////////////////

// C->S : �۷ι�- ��
class MSG_CLIENT_INQUIRE_CASH_GB : public MSG_REQUEST_BASE<MSG_CLIENT_INQUIRE_CASH_GB>
{
public:
	MSG_CLIENT_INQUIRE_CASH_GB(long nGameCode)
		: MSG_REQUEST_BASE<MSG_CLIENT_INQUIRE_CASH_GB>(PROTOCOL_CLIENT_INQUIRE_CASH)
	{ 
		GameCode = nGameCode; 
		ViewType = 0;
		AccountSeq = 0;
		SumOnly = false;
	}

public:
	BYTE	ViewType;					// ��ȸ ���� (	0:��ü(ĳ��+����Ʈ), 
										//				1:ĳ��(�Ϲ�+�̺�Ʈ), 
										//				2:����Ʈ(���ʽ�+����), 
										//				3:���ʽ�����Ʈ, 
										//				4:��������Ʈ, 
										//				5:ĳ��(�Ϲ�), 
										//				6:ĳ��(�̺�Ʈ),
										//				7:���ϸ���)
	DWORD	AccountSeq;					// ��������	
	bool	SumOnly;					// �� �հ踸 ��ȸ ���� (	true  : ������ ���� ����, 
										//							false : ������ ���� + ĳ�� ������ ���(STDetail �� ���))
};

// S->C : �۷ι�- ��
class MSG_SERVER_INQUIRE_CASH_GB : public MSG_RESPONSE_BASE<MSG_SERVER_INQUIRE_CASH_GB>
{
public:
	MSG_SERVER_INQUIRE_CASH_GB(long nGameCode) 
		: MSG_RESPONSE_BASE<MSG_SERVER_INQUIRE_CASH_GB>(PROTOCOL_SERVER_INQUIRE_CASH)
	{
		GameCode = nGameCode;
		PackHeader = dwPacketSize;
		AccountSeq = 0;
		ListCount = 0;
		sum[0] = 0;					// ĳ��(�Ϲ�, �̺�Ʈ) ����
		sum[1] = 0;					// ����Ʈ(���ʽ�, ����) ����
	}

public:
	DWORD	AccountSeq;				// ��������
	long	PackHeader;	
	long	ListCount;	
	double	sum[2];					// ������ ���� ���� (0: ĳ�� ����, 1: ����Ʈ ����, 2: ���ϸ��� ����)	
};

///////////////////////////////// ��ǰ ���� /////////////////////////////////

// C->S : �۷ι�- ��
class MSG_CLIENT_BUYPRODUCT_GB : public MSG_REQUEST_BASE<MSG_CLIENT_BUYPRODUCT_GB>
{
public:
	MSG_CLIENT_BUYPRODUCT_GB(long nGameCode)
		: MSG_REQUEST_BASE<MSG_CLIENT_BUYPRODUCT_GB>(PROTOCOL_CLIENT_BUYPRODUCT)
	{
		GameCode = nGameCode;
		AccountSeq = 0;
		PackageSeq = PriceSeq = SalesZone = DisplaySeq = Class = Level = ServerIndex = CashTypeCode = 0;
		ZeroMemory(CharName, MAX_CHARACTERID_LENGTH * sizeof(WCHAR));
		ZeroMemory(Rank, MAX_RANK_LENGTH * sizeof(WCHAR));
	}

public:
	DWORD	AccountSeq;							// ��������	
	long	PackageSeq;							// ���� ��ǰ ����
	long	PriceSeq;							// ���� ����	
	long	SalesZone;							// �Ǹſ���
	long	DisplaySeq;							// �������� ����
	long	Class;								// Ŭ����
	long	Level;								// ����
	WCHAR	CharName[MAX_CHARACTERID_LENGTH];	// ĳ���� ��
	WCHAR	Rank[MAX_RANK_LENGTH];				// ��ũ
	long	ServerIndex;						// ���� �ε���
	long	CashTypeCode;						// ĳ�� ���� �ڵ�
};

// S->C : �۷ι�- ��
class MSG_SERVER_BUYPRODUCT_GB : public MSG_RESPONSE_BASE<MSG_SERVER_BUYPRODUCT_GB>
{
public:
	MSG_SERVER_BUYPRODUCT_GB(long nGameCode)
		: MSG_RESPONSE_BASE<MSG_SERVER_BUYPRODUCT_GB>(PROTOCOL_SERVER_BUYPRODUCT)
	{
		GameCode = nGameCode;
		AccountSeq = 0;
		LeftProductCount = 0;
	}

public:
	DWORD	AccountSeq;					// ��������
	long	LeftProductCount;			// ��ǰ �Ǹ� ���� �ܿ����� (-1: ������) ��ǰ ���� ������ �����ؾ��Ѵ�.
	
	/*	��� �ڵ� 
		-2 : DB ó�� ����(���������� �������� ����)
		 0 : ����
		 1 : ����ĳ�ú���
		 2 : ���� ��ǰ �����ϱ� �Ұ���
		 3 : �ܿ���������
		 4 : �ǸűⰣ����
		 5 : �Ǹ�����
		 6 : ���� �Ұ���
		 7 : �̺�Ʈ��ǰ ���� �Ұ�
		 8 : �̺�Ʈ ��ǰ ���� ���� Ƚ�� �ʰ�
		-1 : ���� �߻�
	*/
};


///////////////////////////////// ��ǰ ���� /////////////////////////////////

// C->S : �۷ι�- ��
class MSG_CLIENT_GIFTPRODUCT_GB : public MSG_REQUEST_BASE<MSG_CLIENT_GIFTPRODUCT_GB>
{
public:
	MSG_CLIENT_GIFTPRODUCT_GB(long nGameCode)
		: MSG_REQUEST_BASE<MSG_CLIENT_GIFTPRODUCT_GB>(PROTOCOL_CLIENT_GIFTPRODUCT)
	{
		GameCode = nGameCode; 
		SenderSeq = ReceiverSeq = 0;
		SenderServerIndex = ReceiverServerIndex = PackageSeq = PriceSeq = SalesZone = DisplaySeq = CashTypeCode = 0;
		ZeroMemory(Message, MAX_MESSAGE_LENGTH * sizeof(WCHAR));
		ZeroMemory(SenderCharName, MAX_CHARACTERID_LENGTH * sizeof(WCHAR));
		ZeroMemory(ReceiverCharName, MAX_CHARACTERID_LENGTH * sizeof(WCHAR));
	}

public:
	DWORD	SenderSeq;									// �߽��� ����
	long	SenderServerIndex;							// �߽��� ���� ��ȣ
	WCHAR	SenderCharName[MAX_CHARACTERID_LENGTH];		// �߽��� ĳ���� ��
	DWORD	ReceiverSeq;								// ������ ����
	long	ReceiverServerIndex;						// ������ ���� ��ȣ
	WCHAR	ReceiverCharName[MAX_CHARACTERID_LENGTH];	// ������ ĳ���� ��
	WCHAR	Message[MAX_MESSAGE_LENGTH];				// �޽���	
	long	PackageSeq;									// ���� ��ǰ ����
	long	PriceSeq;									// ���� ����	
	long	SalesZone;									// �Ǹſ���
	long	DisplaySeq;									// �������� ����
	long	CashTypeCode;								// ĳ�� ���� �ڵ�
};

// S->C : �۷ι�- ��
class MSG_SERVER_GIFTPRODUCT_GB : public MSG_RESPONSE_BASE<MSG_SERVER_GIFTPRODUCT_GB>
{
public:
	MSG_SERVER_GIFTPRODUCT_GB(long nGameCode) 
		: MSG_RESPONSE_BASE<MSG_SERVER_GIFTPRODUCT_GB>(PROTOCOL_SERVER_GIFTPRODUCT)
	{
		GameCode = nGameCode;
		SenderSeq = ReceiverSeq = 0;
		LeftProductCount = 0;
		GiftSendLimit = 0;		
	}

public:
	DWORD	SenderSeq;					// ���� ����
	DWORD	ReceiverSeq;				// ���� ����
	long	LeftProductCount;			// �������� �ܿ� ����
	double	GiftSendLimit;				// �����ϱ� �ѵ� �ʰ��� �������� ĳ��
};


//////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// ���� ////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////// ĳ�� ���� /////////////////////////////////

// C->S : ĳ������
class MSG_CLIENT_GIFTCASH : public MSG_REQUEST_BASE<MSG_CLIENT_GIFTCASH>
{
public:
	MSG_CLIENT_GIFTCASH()
		: MSG_REQUEST_BASE<MSG_CLIENT_GIFTCASH>(PROTOCOL_CLIENT_GIFTCASH)
	{
		SenderAccountID = ReceiverAccountID = 0;
		SendServerIndex = ReceiverServerIndex = 0;
		CashValue = 0;
		ZeroMemory(Message, MAX_MESSAGE_LENGTH * sizeof(WCHAR));
		ZeroMemory(SendCharName, MAX_CHARACTERID_LENGTH * sizeof(WCHAR));
		ZeroMemory(ReceiverCharName, MAX_CHARACTERID_LENGTH * sizeof(WCHAR));
	}

public:
	DWORD	SenderAccountID;							// ������ ���� ����
	WCHAR	SendCharName[MAX_CHARACTERID_LENGTH];		// �߽��� ĳ���� ��
	long	SendServerIndex;							// �߽��� ���� ��ȣ
	DWORD	ReceiverAccountID;							// �޴� ���� ����
	WCHAR	ReceiverCharName[MAX_CHARACTERID_LENGTH];	// ������ ĳ���� ��
	long	ReceiverServerIndex;						// ������ ���� ��ȣ
	WCHAR	Message[MAX_MESSAGE_LENGTH];				// ���� �޽���
	double	CashValue;									// ���� ĳ�� ��
};

// S->C : ĳ������
class MSG_SERVER_GIFTCASH : public MSG_RESPONSE_BASE<MSG_SERVER_GIFTCASH>
{
public:
	MSG_SERVER_GIFTCASH()
		: MSG_RESPONSE_BASE<MSG_SERVER_GIFTCASH>(PROTOCOL_SERVER_GIFTCASH)
	{
		SenderSeq = ReceiverSeq = 0;
		GiftSendLimit = 0;
	}

public:
	DWORD	SenderSeq;					// ���� ����
	DWORD	ReceiverSeq;				// ���� ����
	double	GiftSendLimit;				// ������ ��� ����	
};


///////////////////////////////// ��ǰ ����/���� ���ɿ��� üũ /////////////////////////////////

// C->S : ��ǰ ����/���� ���ɿ��� ��ȸ
class MSG_CLIENT_PRODUCTBUYGIFTCHECK : public MSG_REQUEST_BASE<MSG_CLIENT_PRODUCTBUYGIFTCHECK>
{
public:
	MSG_CLIENT_PRODUCTBUYGIFTCHECK()
		: MSG_REQUEST_BASE<MSG_CLIENT_PRODUCTBUYGIFTCHECK>(PROTOCOL_CLIENT_PRODUCTBUYGIFTCHECK)
	{
		AccountID = 0;
	}

public:
	DWORD	AccountID;					// ���� ����
};

// S->C : ��ǰ ����/���� ���ɿ��� ��ȸ
class MSG_SERVER_PRODUCTBUYGIFTCHECK : public MSG_RESPONSE_BASE<MSG_SERVER_PRODUCTBUYGIFTCHECK>
{
public:
	MSG_SERVER_PRODUCTBUYGIFTCHECK() 
		: MSG_RESPONSE_BASE<MSG_SERVER_PRODUCTBUYGIFTCHECK>(PROTOCOL_SERVER_PRODUCTBUYGIFTCHECK)
	{
		AccountID = 0;
		ItemBuy = Present = 0;
		MyPresentCash = MyPresentSendLimit = 0;
	}

public:
	DWORD	AccountID;					// ��������
	long	ItemBuy;					// ��ǰ ���� ���� ���� ��� (0: �Ұ�, 1: ����)
	long	Present;					// ��ǰ/ĳ�� ���� ���� ���� ��� (1: ����, 2:�����ϱ� �Ұ�, 3: �����ѵ� �ʰ�)
	double	MyPresentCash;				// �Ѵ� ���� ���� ������ ĳ�� �հ�
	double	MyPresentSendLimit;			// �Ѵ� ���� �����ϱ� ĳ�� �ѵ�

	/*	ó�����
		 0 : �Ұ�
		 1 : ����
		-1 : DB ó�� ����
	*/
};


///////////////////////////////// �̺�Ʈ ��ǰ ��� /////////////////////////////////

// C->S : �̺�Ʈ ��ǰ ��� ��ȸ
class MSG_CLIENT_PRODUCTEVENTLIST : public MSG_REQUEST_BASE<MSG_CLIENT_PRODUCTEVENTLIST>
{
public:
	MSG_CLIENT_PRODUCTEVENTLIST()
		: MSG_REQUEST_BASE<MSG_CLIENT_PRODUCTEVENTLIST>(PROTOCOL_CLIENT_PRODUCTEVENTLIST)
	{
		AccountSeq = 0;
		SalesZone = ProductDisplaySeq = 0;
	}

public:	
	DWORD	AccountSeq;					// ���� ����
	long	SalesZone;					// �Ǹ� ����
	long	ProductDisplaySeq;			// ���� ��ǰ ī�װ� ����
};

// S->C : �̺�Ʈ ��ǰ ��� ��ȸ
class MSG_SERVER_PRODUCTEVENTLIST : public MSG_RESPONSE_BASE<MSG_SERVER_PRODUCTEVENTLIST>
{
public:
	MSG_SERVER_PRODUCTEVENTLIST()
		: MSG_RESPONSE_BASE<MSG_SERVER_PRODUCTEVENTLIST>(PROTOCOL_SERVER_PRODUCTEVENTLIST)
	{
		PackHeader = dwPacketSize;
		AccountSeq = 0;
		ProductDisplaySeq = PackagesCount = 0;
	}
		
public:
	long	PackHeader;
	DWORD	AccountSeq;					// ���� ����
	long	ProductDisplaySeq;			// ���� ��ǰ ī�װ� ����
	long	PackagesCount;		
};


///////////////////////////////// ��Ű�� �ܿ� ���� ��ȸ /////////////////////////////////

// C->S : ���� ��ǰ �ܿ� ���� ��ȸ
class MSG_CLIENT_PACKAGELEFTCOUNT : public MSG_REQUEST_BASE<MSG_CLIENT_PACKAGELEFTCOUNT>
{
public:
	MSG_CLIENT_PACKAGELEFTCOUNT()
		: MSG_REQUEST_BASE<MSG_CLIENT_PACKAGELEFTCOUNT>(PROTOCOL_CLIENT_PACKAGELEFTCOUNT)
	{
		PackageProductSeq = 0;
	}

public:	
	long	PackageProductSeq;
};

// S->C : ���� ��ǰ �ܿ� ���� ��ȸ
class MSG_SERVER_PACKAGELEFTCOUNT : public MSG_RESPONSE_BASE<MSG_SERVER_PACKAGELEFTCOUNT>
{
public:
	MSG_SERVER_PACKAGELEFTCOUNT() 
		: MSG_RESPONSE_BASE<MSG_SERVER_PACKAGELEFTCOUNT>(PROTOCOL_SERVER_PACKAGELEFTCOUNT)
	{
		PackageProductSeq = LeftCount = 0;
	}
		
public:
	long	PackageProductSeq;
	long	LeftCount;
};


///////////////////////////////// ��ü ������ ��ȸ /////////////////////////////////

// C->S : ��ü ������ ��ȸ
class MSG_CLIENT_STORAGELIST : public MSG_REQUEST_BASE<MSG_CLIENT_STORAGELIST>
{
public:
	MSG_CLIENT_STORAGELIST() 
		: MSG_REQUEST_BASE<MSG_CLIENT_STORAGELIST>(PROTOCOL_CLIENT_STORAGELIST)
	{
		AccountID = 0;
		SalesZone = 0;
	}

public:
	DWORD	AccountID;					// ���� ����
	int		SalesZone;					// �Ǹſ���	
};

// S->C : ��ü ������ ��ȸ
class MSG_SERVER_STORAGELIST : public MSG_RESPONSE_BASE<MSG_SERVER_STORAGELIST>
{
public:
	MSG_SERVER_STORAGELIST()
		: MSG_RESPONSE_BASE<MSG_SERVER_STORAGELIST>(PROTOCOL_SERVER_STORAGELIST)
	{
		PackHeader = dwPacketSize;
		AccountID = 0;
		ListCount = 0;
	}

public:
	long	PackHeader;
	DWORD	AccountID;					// ���� ����
	int		ListCount;					// ������ ��� ����
};


///////////////////////////////// �� ������ ������ ��ȸ /////////////////////////////////

// C->S : �� ������ ������ ��ȸ
class MSG_CLIENT_STORAGELIST_PAGE : public MSG_REQUEST_BASE<MSG_CLIENT_STORAGELIST_PAGE>
{
public:
	MSG_CLIENT_STORAGELIST_PAGE() 
		: MSG_REQUEST_BASE<MSG_CLIENT_STORAGELIST_PAGE>(PROTOCOL_CLIENT_STORAGELIST_PAGE)
	{
		AccountID = 0;
		SalesZone = PageSize = NowPage = 0;
		GiftMsgFlag = true;
		StorageType = 'A';
	}

public:
	DWORD	AccountID;					// ���� ����
	int		SalesZone;					// �Ǹſ���
	char	StorageType;				// A : ��ü, S : ������, G : ������
	bool	GiftMsgFlag;				// 0�̸� ���� �޽��� �Ⱥ�����.
	int		PageSize;					// �� �������� �� ������ ��
	int		NowPage;					// ��û�� ������ ��ȣ
};

// S->C : �� ������ ������ ��ȸ
class MSG_SERVER_STORAGELIST_PAGE : public MSG_RESPONSE_BASE<MSG_SERVER_STORAGELIST_PAGE>
{
public:
	MSG_SERVER_STORAGELIST_PAGE()
		: MSG_RESPONSE_BASE<MSG_SERVER_STORAGELIST_PAGE>(PROTOCOL_SERVER_STORAGELIST_PAGE)
	{
		PackHeader = dwPacketSize;
		AccountID = 0;
		ListCount = NowPage = TotalPage = TotalRecord = 0;
		GiftMsgFlag = true;
		StorageType = 'A';
	}

public:
	long	PackHeader;
	DWORD	AccountID;					// ���� ����
	int		ListCount;					// ������ ��� ����
	int		NowPage;					// ��û�� ������ ��ȣ
	char	StorageType;				// A : ��ü, S : ������, G : ������
	bool	GiftMsgFlag;				// 0�̸� ���� �޽��� �Ⱥ�����.
	int		TotalPage;					// ��ü ������ ��
	int		TotalRecord;				// ��ü ������ ��
};

///////////////////////////////// ������ ��� /////////////////////////////////

// C->S : ������ ������ ����ϱ�
class MSG_CLIENT_STORAGEUSE : public MSG_REQUEST_BASE<MSG_CLIENT_STORAGEUSE>
{
public:
	MSG_CLIENT_STORAGEUSE() 
		: MSG_REQUEST_BASE<MSG_CLIENT_STORAGEUSE>(PROTOCOL_CLIENT_STORAGEUSE)
	{
		AccountSeq = IPAddress = 0;
		StorageSeq = StorageItemSeq = Class = Level = ServerIndex = 0;
		StorageItemType = 0;
		ZeroMemory(CharName, MAX_CHARACTERID_LENGTH * sizeof(WCHAR));
		ZeroMemory(Rank, MAX_RANK_LENGTH * sizeof(WCHAR));
	}

public:	
	DWORD	AccountSeq;							// ���� ����
	DWORD	IPAddress;							// ���� IP
	long	StorageSeq;							// ������ ����
	long	StorageItemSeq;						// ������ Or ��ǰ ����
	char	StorageItemType;					// ĳ�� Or ��ǰ ���� (C: ĳ��, P: ��ǰ)
	long	Class;								// Ŭ����
	long	Level;								// ����
	WCHAR	CharName[MAX_CHARACTERID_LENGTH];	// ĳ���� ��
	WCHAR	Rank[MAX_RANK_LENGTH];				// ��ũ
	long	ServerIndex;						// �����ε���	
};

// S->C : ������ ������ ����ϱ�
class MSG_SERVER_STORAGEUSE : public MSG_RESPONSE_BASE<MSG_SERVER_STORAGEUSE>
{
public:
	MSG_SERVER_STORAGEUSE() 
		: MSG_RESPONSE_BASE<MSG_SERVER_STORAGEUSE>(PROTOCOL_SERVER_STORAGEUSE)
	{
		PackHeader = dwPacketSize;
		AccountSeq = 0;
		ProductSeq = StorageSeq = StorageItemSeq = 0;
		PropertyCount = 0;
		ZeroMemory(InGameProductID, MAX_TYPENAME_LENGTH);
	}

public:
	DWORD	AccountSeq;						//	���� ����
	char	InGameProductID[MAX_TYPENAME_LENGTH];
	long	PackHeader;
	long	ProductSeq;
	BYTE	PropertyCount;
	long	StorageSeq;							// ������ ����
	long	StorageItemSeq;						// ������ Or ��ǰ ����

	/*	ó�����
		 0 : ����
		 1 : ������ ����ϱ� �׸��� ����
		 2 : PC�濡���� �ޱ� ������ ������
		 3 : �Ⱓ �� ������� �÷� ����� �ִ� ���
		 4 : �Ⱓ �� ������� ���� ���� ��ǰ�� �ִ°��
		-1 : �����߻�
		-2 : DB ó�� ����(���������� �������� ����)
	*/
};


///////////////////////////////// ������ ��� �ѹ� /////////////////////////////////

// C->S : ������ ������ ��� �ѹ�
class MSG_CLIENT_STORAGEUSE_ROLLBACK : public MSG_REQUEST_BASE<MSG_CLIENT_STORAGEUSE_ROLLBACK>
{
public:
	MSG_CLIENT_STORAGEUSE_ROLLBACK() 
		: MSG_REQUEST_BASE<MSG_CLIENT_STORAGEUSE_ROLLBACK>(PROTOCOL_CLIENT_STORAGEUSE_ROLLBACK)
	{
		AccountSeq = 0;
		StorageSeq = StorageItemSeq = 0;
	}

public:	
	DWORD	AccountSeq;							// ���� ����
	long	StorageSeq;							// ������ ����
	long	StorageItemSeq;						// ������ ��ǰ ����
};

// S->C : ������ ������ ��� �ѹ�
class MSG_SERVER_STORAGEUSE_ROLLBACK : public MSG_RESPONSE_BASE<MSG_SERVER_STORAGEUSE_ROLLBACK>
{
public:
	MSG_SERVER_STORAGEUSE_ROLLBACK() 
		: MSG_RESPONSE_BASE<MSG_SERVER_STORAGEUSE_ROLLBACK>(PROTOCOL_SERVER_STORAGEUSE_ROLLBACK)
	{
		AccountSeq = 0;
	}

public:
	DWORD	AccountSeq;						//	���� ����

	/*	ó����� 
		 0 : ����
		 1 : Rollback ��� ����
		-1 : �����߻�
		-2 : DB ó�� ����(���������� �������� ����)
	*/
};


///////////////////////////////// ������ ������ /////////////////////////////////

// C->S : ������ ������ ������
class MSG_CLIENT_STORAGETHROW : public MSG_REQUEST_BASE<MSG_CLIENT_STORAGETHROW>
{
public:
	MSG_CLIENT_STORAGETHROW()
		: MSG_REQUEST_BASE<MSG_CLIENT_STORAGETHROW>(PROTOCOL_CLIENT_STORAGETHROW)
	{
		AccountSeq = 0;
		StorageSeq = StorageItemSeq = 0;
		StorageItemType = 0;
	}

public:
	DWORD	AccountSeq;					// ���� ����
	long	StorageSeq;					// ������ ����
	long	StorageItemSeq;				// ������ Or ��ǰ ����
	char	StorageItemType;			// ĳ�� Or ��ǰ ���� (C: ĳ��, P: ��ǰ)
};

// S->C : ������ ������ ������
class MSG_SERVER_STORAGETHROW : public MSG_RESPONSE_BASE<MSG_SERVER_STORAGETHROW>
{
public:
	MSG_SERVER_STORAGETHROW()
		: MSG_RESPONSE_BASE<MSG_SERVER_STORAGETHROW>(PROTOCOL_SERVER_STORAGETHROW)
	{
		AccountSeq = 0;
	}

public:
	DWORD	AccountSeq;					// ���� ����

	/*	ó����� 
		 0 : ����
		 1 : ������ ������ �׸��� ����
		-1 : �����߻�
		-2 : DB ó�� ����(���������� �������� ����)
	*/
};


///////////////////////////////// ���ϸ��� ���� /////////////////////////////////

// C->S : ���ϸ��� ����
class MSG_CLIENT_MILEAGEDEDUCT : public MSG_REQUEST_BASE<MSG_CLIENT_MILEAGEDEDUCT>
{
public:
	MSG_CLIENT_MILEAGEDEDUCT() 
		: MSG_REQUEST_BASE<MSG_CLIENT_MILEAGEDEDUCT>(PROTOCOL_CLIENT_MILEAGEDEDUCT)
	{
		AccountSeq = 0;
		DeductCategory = MileageSection = MileageDeductPoint = Class = Level = ServerIndex = 0;
		ZeroMemory(CharName, MAX_CHARACTERID_LENGTH * sizeof(WCHAR));
		ZeroMemory(Rank, MAX_RANK_LENGTH * sizeof(WCHAR));
	}

public:		
	DWORD	AccountSeq;							// ���� ȸ�� ���� ����
	long	DeductCategory;						// ���ϸ�����������
	long	MileageSection;						// ���ϸ�������
	long	MileageDeductPoint;					// ���ϸ��� ���� ����Ʈ
	long	Class;								// Ŭ����
	long	Level;								// ����
	WCHAR	CharName[MAX_CHARACTERID_LENGTH];	// ĳ���͸�
	WCHAR	Rank[MAX_RANK_LENGTH];				// ��ũ
	long	ServerIndex;						// ���� ��ȣ
};

// S->C : ���ϸ��� ����
class MSG_SERVER_MILEAGEDEDUCT : public MSG_RESPONSE_BASE<MSG_SERVER_MILEAGEDEDUCT>
{
public:
	MSG_SERVER_MILEAGEDEDUCT()
		: MSG_RESPONSE_BASE<MSG_SERVER_MILEAGEDEDUCT>(PROTOCOL_SERVER_MILEAGEDEDUCT)
	{
		AccountSeq = 0;
	}
		
public:
	DWORD	AccountSeq;				// ���� ȸ�� ���� ����
	
	// ��ȸ ��� �ڵ� 	0 : ����, -1 :  �����߻�
};


///////////////////////////////// ���ϸ��� ���� /////////////////////////////////

// C->S : ���ϸ��� ����
class MSG_CLIENT_MILEAGESAVE : public MSG_REQUEST_BASE<MSG_CLIENT_MILEAGESAVE>
{
public:
	MSG_CLIENT_MILEAGESAVE() 
		: MSG_REQUEST_BASE<MSG_CLIENT_MILEAGESAVE>(PROTOCOL_CLIENT_MILEAGESAVE)
	{
		AccountSeq = 0;
		MileageType = MileageSection = MileagePoint = Class = Level = ServerIndex = 0;
		ZeroMemory(CharName, MAX_CHARACTERID_LENGTH * sizeof(WCHAR));
		ZeroMemory(Rank, MAX_RANK_LENGTH * sizeof(WCHAR));
	}

public:		
	DWORD	AccountSeq;							// ���� ȸ�� ���� ����
	long	MileageType;						// ���ϸ�����������
	long	MileageSection;						// ���ϸ�������
	long	MileagePoint;						// ���ϸ��� ���� ����Ʈ
	long	Class;								// Ŭ����
	long	Level;								// ����
	WCHAR	CharName[MAX_CHARACTERID_LENGTH];	// ĳ���͸�
	WCHAR	Rank[MAX_RANK_LENGTH];				// ��ũ
	long	ServerIndex;						// ���� ��ȣ
};

// S->C : ���ϸ��� ����
class MSG_SERVER_MILEAGESAVE : public MSG_RESPONSE_BASE<MSG_SERVER_MILEAGESAVE>
{
public:
	MSG_SERVER_MILEAGESAVE()
		: MSG_RESPONSE_BASE<MSG_SERVER_MILEAGESAVE>(PROTOCOL_SERVER_MILEAGESAVE)
	{
		AccountSeq = 0;
	}
		
public:
	DWORD	AccountSeq;			// ���� ȸ�� ���� ����
	
	// ��ȸ ��� �ڵ� 	0 : ����, -1 :  �����߻�
};


///////////////////////////////// �ǽð� ���ϸ��� ���� /////////////////////////////////

// C->S : �ǽð� ���ϸ��� ����
class MSG_CLIENT_MILEAGELIVESAVEUP : public MSG_REQUEST_BASE<MSG_CLIENT_MILEAGELIVESAVEUP>
{
public:
	MSG_CLIENT_MILEAGELIVESAVEUP()
		: MSG_REQUEST_BASE<MSG_CLIENT_MILEAGELIVESAVEUP>(PROTOCOL_CLIENT_MILEAGELIVESAVEUP)
	{
		AccountSeq = 0;
		MileageSection = SourceType = 0;
	}

public:		
	DWORD	AccountSeq;			// ���� ȸ�� ���� ����
	long	MileageSection;		// ���ϸ�������
	long	SourceType;			// ����, PC�� ����
};

// S->C : �ǽð� ���ϸ��� ����
class MSG_SERVER_MILEAGELIVESAVEUP : public MSG_RESPONSE_BASE<MSG_SERVER_MILEAGELIVESAVEUP>
{
public:
	MSG_SERVER_MILEAGELIVESAVEUP()
		: MSG_RESPONSE_BASE<MSG_SERVER_MILEAGELIVESAVEUP>(PROTOCOL_SERVER_MILEAGELIVESAVEUP)
	{
		AccountSeq = 0;
	}
		
public:
	DWORD AccountSeq;			// ���� ȸ�� ���� ����
	
	// ��ȸ ��� �ڵ� 	0 : ����, -1 :  �����߻�
};


///////////////////////////////// ������ �����ڵ� ���� /////////////////////////////////

// C->S : ������ �ø��� ������Ʈ
class MSG_CLIENT_ITEMSERIALUPDATE : public MSG_REQUEST_BASE<MSG_CLIENT_ITEMSERIALUPDATE>
{
public:
	MSG_CLIENT_ITEMSERIALUPDATE()
		: MSG_REQUEST_BASE<MSG_CLIENT_ITEMSERIALUPDATE>(PROTOCOL_CLIENT_ITEMSERIALUPDATE)
	{
		AccountSeq = 0;
		StorageSeq = StorageItemSeq = 0;
		InGameUseCode = 0;
	}

public:	
	DWORD	AccountSeq;			// ȸ�� ���� ����
	long	StorageSeq;			// ������ ����
	long	StorageItemSeq;		// ������ ĳ�� or ��ǰ ����
	INT64	InGameUseCode;		// ������ �ø��� �ڵ�	
};

// S->C : ������ �ø��� ������Ʈ
class MSG_SERVER_ITEMSERIALUPDATE : public MSG_RESPONSE_BASE<MSG_SERVER_ITEMSERIALUPDATE>
{
public:
	MSG_SERVER_ITEMSERIALUPDATE() 
		: MSG_RESPONSE_BASE<MSG_SERVER_ITEMSERIALUPDATE>(PROTOCOL_SERVER_ITEMSERIALUPDATE)
	{
		AccountSeq = 0;
	}
		
public:
	DWORD	AccountSeq;			// ���� ȸ�� ���� ����
	
	// ��ȸ ��� �ڵ� 	0 : ����, 1: ��� �׸� ����, -1 :  �����߻�
};


/////////////////////// �ΰ��� ����Ʈ ������ ���� �ۼ�Ʈ ��ȸ ///////////////////////

// C->S
class MSG_CLIENT_INQUIRE_GAMEPOINTVALUE : public MSG_REQUEST_BASE<MSG_CLIENT_INQUIRE_GAMEPOINTVALUE>
{
public:
	MSG_CLIENT_INQUIRE_GAMEPOINTVALUE() 
		: MSG_REQUEST_BASE<MSG_CLIENT_INQUIRE_GAMEPOINTVALUE>(PROTOCOL_CLIENT_INQUIRE_GAMEPOINTVALUE)
	{
		ServerType = AccessType = 0;
	}

public:	
	long	ServerType;
	long	AccessType;
};

// S->C
class MSG_SERVER_INQUIRE_GAMEPOINTVALUE : public MSG_RESPONSE_BASE<MSG_SERVER_INQUIRE_GAMEPOINTVALUE>
{
public:
	MSG_SERVER_INQUIRE_GAMEPOINTVALUE() 
		: MSG_RESPONSE_BASE<MSG_SERVER_INQUIRE_GAMEPOINTVALUE>(PROTOCOL_SERVER_INQUIRE_GAMEPOINTVALUE)
	{
		PackHeader = dwPacketSize;
		ListCount = 0;
	}
		
public:
	long	PackHeader;
	long	ListCount;
};


///////////////////////////////// ��ǰ ���� ��ȸ /////////////////////////////////

// C->S : ��ǰ ���� ��ȸ
class MSG_CLIENT_INQUIRE_SALESVERSION : public MSG_REQUEST_BASE<MSG_CLIENT_INQUIRE_SALESVERSION>
{
public:
	MSG_CLIENT_INQUIRE_SALESVERSION()
		: MSG_REQUEST_BASE<MSG_CLIENT_INQUIRE_SALESVERSION>(PROTOCOL_CLIENT_INQUIRE_SALESVERSION)
	{
		SalesZone = 0;
	}

public:
	long	SalesZone;						// �Ǹ� ���� ��ȣ
};

// S->C : ��ǰ ���� ��ȸ
class MSG_SERVER_INQUIRE_SALESVERSION : public MSG_RESPONSE_BASE<MSG_SERVER_INQUIRE_SALESVERSION>
{
public:
	MSG_SERVER_INQUIRE_SALESVERSION() 
		: MSG_RESPONSE_BASE<MSG_SERVER_INQUIRE_SALESVERSION>(PROTOCOL_SERVER_INQUIRE_SALESVERSION)
	{
		SalesZone = Year = YearIdentity = 0;
	}

public:
	unsigned short	SalesZone;
	unsigned short	Year;
	unsigned short	YearIdentity;
};


///////////////////////////////// ��� ���� ��ȸ /////////////////////////////////

// C->S : ��� ���� ��ȸ
class MSG_CLIENT_INQUIRE_BANNERVERSION : public MSG_REQUEST_BASE<MSG_CLIENT_INQUIRE_BANNERVERSION>
{
public:
	MSG_CLIENT_INQUIRE_BANNERVERSION()
		: MSG_REQUEST_BASE<MSG_CLIENT_INQUIRE_BANNERVERSION>(PROTOCOL_CLIENT_INQUIRE_BANNERVERSION)
	{
		BannerZone = 0;
	}

public:
	long	BannerZone;						// ��� ���� ��ȣ
};	

// S->C : ��� ���� ��ȸ
class MSG_SERVER_INQUIRE_BANNERVERSION : public MSG_RESPONSE_BASE<MSG_SERVER_INQUIRE_BANNERVERSION>
{
public:
	MSG_SERVER_INQUIRE_BANNERVERSION() 
		: MSG_RESPONSE_BASE<MSG_SERVER_INQUIRE_BANNERVERSION>(PROTOCOL_SERVER_INQUIRE_BANNERVERSION)
	{
		BannerZone = Year = YearIdentity = 0;
	}

public:
	unsigned short	BannerZone;
	unsigned short	Year;
	unsigned short	YearIdentity;
};


///////////////////////////////// ���� ������Ʈ /////////////////////////////////

// C->S : ���� ������Ʈ
class MSG_CLIENT_UPDATEVERSION : public MSG_REQUEST_BASE<MSG_CLIENT_UPDATEVERSION>
{
public:
	MSG_CLIENT_UPDATEVERSION() 
		: MSG_REQUEST_BASE<MSG_CLIENT_UPDATEVERSION>(PROTOCOL_CLIENT_UPDATEVERSION)
	{
		SalesZone = Year = YearIdentity = 0;
	}

public:	
	unsigned short	SalesZone;
	unsigned short	Year;
	unsigned short	YearIdentity;
};

// S->C : ���� ������Ʈ
class MSG_SERVER_UPDATEVERSION : public MSG_RESPONSE_BASE<MSG_SERVER_UPDATEVERSION>
{
public:
	MSG_SERVER_UPDATEVERSION() 
		: MSG_RESPONSE_BASE<MSG_SERVER_UPDATEVERSION>(PROTOCOL_SERVER_UPDATEVERSION)
	{
		SalesZone = Year = YearIdentity = 0;
	}
		
public:
	unsigned short	SalesZone;
	unsigned short	Year;
	unsigned short	YearIdentity;
};


///////////////////////////////// ��� ���� ������Ʈ /////////////////////////////////

// C->S : ��� ���� ������Ʈ
class MSG_CLIENT_UPDATE_BANNERVERSION : public MSG_REQUEST_BASE<MSG_CLIENT_UPDATE_BANNERVERSION>
{
public:
	MSG_CLIENT_UPDATE_BANNERVERSION() 
		: MSG_REQUEST_BASE<MSG_CLIENT_UPDATE_BANNERVERSION>(PROTOCOL_CLIENT_UPDATE_BANNERVERSION)
	{
		BannerZone = Year = YearIdentity = 0;
	}

public:	
	unsigned short	BannerZone;
	unsigned short	Year;
	unsigned short	YearIdentity;
};

// S->C : ��� ���� ������Ʈ
class MSG_SERVER_UPDATE_BANNERVERSION : public MSG_RESPONSE_BASE<MSG_SERVER_UPDATE_BANNERVERSION>
{
public:
	MSG_SERVER_UPDATE_BANNERVERSION() 
		: MSG_RESPONSE_BASE<MSG_SERVER_UPDATE_BANNERVERSION>(PROTOCOL_SERVER_UPDATE_BANNERVERSION)
	{
		BannerZone = Year = YearIdentity = 0;
	}
		
public:
	unsigned short	BannerZone;
	unsigned short	Year;
	unsigned short	YearIdentity;
};

}

#pragma pack()

