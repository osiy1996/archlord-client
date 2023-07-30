#pragma once
#pragma pack(1)

/**************************************************************************************************

�ۼ���: 2008-07-18
�ۼ���: ������ (youngmoon@webzen.co.kr)

����: ���� �� �������� �������� ����

**************************************************************************************************/

#include "ShopDefine.h"

/*
#define PROTOCOL_CLIENT_INQURE_CASH			0x71
#define PROTOCOL_SERVER_INQURE_CASH			0x72
#define PROTOCOL_CLIENT_BUYPRODUCT			0x73
#define PROTOCOL_SERVER_BUYPRODUCT			0x74
#define PROTOCOL_CLIENT_GIFTPRODUCT			0x75
#define PROTOCOL_SERVER_GIFTPRODUCT			0x76
#define PROTOCOL_CLIENT_STORAGELIST			0x77
#define PROTOCOL_SERVER_STORAGELIST			0x78
#define PROTOCOL_CLIENT_GIFTCASH			0x79
#define PROTOCOL_SERVER_GIFTCASH			0x7A
#define PROTOCOL_CLIENT_PRODUCTBUYGIFTCHECK	0x7B
#define PROTOCOL_SERVER_PRODUCTBUYGIFTCHECK	0x7C
#define PROTOCOL_CLIENT_STORAGEUSE			0x7D
#define PROTOCOL_SERVER_STORAGEUSE			0x7E
#define PROTOCOL_CLIENT_STORAGETHROW		0x7F
#define PROTOCOL_SERVER_STORAGETHROW		0x80
#define PROTOCOL_CLIENT_PRODUCTLIST			0x81
#define PROTOCOL_SERVER_PRODUCTLIST			0x82
#define PROTOCOL_CLIENT_PRODUCTEVENTLIST	0x83
#define PROTOCOL_SERVER_PRODUCTEVENTLIST	0x84
#define PROTOCOL_CLIENT_UPDATELIST			0x85
#define PROTOCOL_SERVER_UPDATELIST			0x86
#define PROTOCOL_SERVER_UPDATECATEGORYLIST  0x87
#define PROTOCOL_SERVER_UPDATEPRODUCTLIST	0x88
*/
namespace MuShopProtocol
{


/////////////////////////////////////////

struct STItemProperty
{
	long PropertySeq;
	long Value;
};

struct STDetail //ĳ�� ��ȸ���� ���
{
	STDetail()
	{
		ZeroMemory(Name, 20 * sizeof(WCHAR));
		Value = 0;			
		Type = 0;			
	};
	
	WCHAR Name[20];						//ĳ������ �̳� ����Ʈ���� �̸�
	double Value;						//ĳ���� ����Ʈ ��
	char Type;							//ĳ������ ����Ʈ ���� ���� (C:ĳ��, P:����Ʈ 0:����)
};

struct STStorage //������ ����
{
	long Seq;					//������ ����
	long ItemSeq;				//������ ��ǰ or ĳ�� �׸� ����
	long GroupCode;				//������ �׷��ڵ�
	long ProductSeq;			//������ǰ �ڵ�

	WCHAR CashName[20];			//ĳ�� ��
	double CashPoint;			//���� ĳ�� ����Ʈ
	WCHAR SendAccountID[50];	//�߽��� ȸ�� ���̵�
	WCHAR SendMessage[200];		//�߽� �޽��� - ������ ��� ����
	char ItemType;				//��ǰ ĳ�� ���� (P:��ǰ, C:ĳ��)
	BYTE RelationType;			//������ ���� ���� (1:����,�ø���,�������� , 2:����)
};

struct STProductList_PRODUCT	//��ǰ ��� ����Ʈ ������ǰ�� ����
{
	long Seq;					//���� ��ǰ ����
	WCHAR Name[50];				//���� ��ǰ ��
	WCHAR PropertyName[50];		//�Ӽ� ��
	WCHAR Value[255];			//�Ӽ� ��
	WCHAR UnitName[20];			//�Ӽ� ���� ��
	long Price;					//���� ��ǰ ����
	long PriceSeq;				//���� ��ǰ ���� ����
	long PropertyType;			//�Ӽ� ���� (141:������ �Ӽ�, 142:���� �Ӽ�)
	long MustFlag;				//�ʼ� ���� (145:�ʼ�, 146:����)
	long vOrder;				//���� �Ӽ� ����(1: ���μӼ�, 9:����Ӽ�)
	long Deleteflag;			//���� �÷��� (144: Ȱ��, 143: ����)
};

/*
struct STProductList_PACKAGE	//��ǰ ��� ����Ʈ ��ǰ���
{	
	long ProductSeq;			//���� ��ǰ ī�װ� ����
	long ViewOrder;				//���� ����
	long Seq;					//���� ��ǰ ����
	WCHAR Name[255];			//���� ��ǰ ��
	long Type;					//���� ��ǰ ���� (170: �Ϲ� ��ǰ, 171: �̺�Ʈ ��ǰ)
	long Price;					//����
	WCHAR Description[2000];	//�󼼼���
	WCHAR Caution[1000];		//���ǻ���
	long SalesFlag;				//���Ű��� ����(182: ����, 183: �Ұ���)
	long GiftFlag;				//�������� ����(184: ����, 185: �Ұ���)
	long LeftCount;				//�ܿ� ���� (-1 �̸� ���Ѿ���)
	DWORD StartDate;			//�Ǹ� ������
	DWORD EndDate;				//�Ǹ� ������
	long CapsuleFlag;			//ĸ�� ��ǰ ���� (176:ĸ��, 177:�Ϲ�)
	long CapsuleCount;			//���� ��ǰ�� ���Ե� ���� ��ǰ ����
	long Deleteflag;			//���� �÷��� (144: Ȱ��, 143: ����)
	WCHAR CashName[100];		//���� ĳ�� ��
	char PriceUnitName[20];		//���� ���� ǥ�� ��	
	char ProductSeqInfo[100];	//���û�ǰ�� ���Ե� ���� ��ǰ ����

};

struct STProductList_CATEGORY	//��ǰ ��� ����Ʈ ī�װ�
{
	long ProductSeq;			//���� ��ǰ ī�װ� ����
	WCHAR Name[50];				//ī�װ� ��
	long EventFlag;				//�̺�Ʈ ī�װ� ����
	long StorageGroup;			//������ �׷� �ڵ�
	long OpenFlag;				//���� ���� (201: ����, 202:�����)
	long ParentSeq;				//�θ� ���� ��ǰ ī�װ� ����
	long DisplayOrder;			//�������
	long Root;					//�ֻ��� ���� (1: �ֻ���, 0:����)	
};
*/


////////////////////////////////////////


template <class _subcls>
class MSG_BASE
{
public:
	MSG_BASE() {}
	MSG_BASE(DWORD dwID) : dwProtocolID(dwID) ,dwPacketSize(sizeof( _subcls )) {}
	
public:
	DWORD 	dwPacketSize; 					//��Ŷ�� ��ü ������
	DWORD	dwProtocolID;					//��Ŷ ���̵�
};

/*
////////////////////////////////////////////////////////////////////////////////////////////////////////
�� ���� ��������
////////////////////////////////////////////////////////////////////////////////////////////////////////
*/


class MSG_CLIENT_INQURE_CASH : public MSG_BASE<MSG_CLIENT_INQURE_CASH>	//(C->S) ����� ĳ�� ��ȸ
{
public:
	MSG_CLIENT_INQURE_CASH() : MSG_BASE<MSG_CLIENT_INQURE_CASH>(PROTOCOL_CLIENT_INQURE_CASH)
		{}

public:
	BYTE ViewType;						//��ȸ ���� (0:��ü(ĳ��+����Ʈ), 1:ĳ��(�Ϲ�+�̺�Ʈ), 2:����Ʈ(���ʽ�+����), 3:���ʽ�����Ʈ, 4:��������Ʈ, 5:ĳ��(�Ϲ�), 6:ĳ��(�Ϲ�))
	long AccountSeq;					//��������
	long GameCode;						//���� �ڵ�
	bool SumOnly;						//�� �հ踸 ��ȸ ���� (true: ������ ���� ����, false: ������ ���� + ĳ�� ������ ���(STDetail �� ���))
};

class MSG_SERVER_INQURE_CASH : public MSG_BASE<MSG_SERVER_INQURE_CASH>	//(S->C) ����� ĳ�� ��ȸ
{
public:
	MSG_SERVER_INQURE_CASH() : MSG_BASE<MSG_SERVER_INQURE_CASH>(PROTOCOL_SERVER_INQURE_CASH) 	  
	{
		sum[0] = 0;
		sum[1] = 0;
	}

public:
	long ResultCode;					//��� �ڵ�
	long AccountSeq;					//��������
	double sum[2];						//������ ���� ���� (0: ĳ�� ����, 1: ����Ʈ ����)
	STDetail stdetail[3];				//ĳ�� ������ ��������	
};	

class MSG_CLIENT_BUYPRODUCT : public MSG_BASE<MSG_CLIENT_BUYPRODUCT>	//(C->S)��ǰ ����
{
public:
	MSG_CLIENT_BUYPRODUCT() : MSG_BASE<MSG_CLIENT_BUYPRODUCT>(PROTOCOL_CLIENT_BUYPRODUCT)
	{}

public:
	long AccountSeq;					//��������
	long GameCode;						//���� �ڵ�
	long PackageSeq;					//���� ��ǰ ����
	long PriceSeq;						//���� ����	
	long SalesZone;						//�Ǹſ���
	long DisplaySeq;					//�������� ����
	long Class;							//Ŭ����
	long Level;							//����
	WCHAR CharName[32];					//ĳ���� ��
	WCHAR Rank;							//��ũ
	long ServerIndex;					//�����ε���
};

class MSG_SERVER_BUYPRODUCT : public MSG_BASE<MSG_SERVER_BUYPRODUCT>	//(S->C) ��ǰ ����
{
public:
	MSG_SERVER_BUYPRODUCT() : MSG_BASE<MSG_SERVER_BUYPRODUCT>(PROTOCOL_SERVER_BUYPRODUCT)
		{}

public:
	DWORD AccountSeq;					//��������
	long LeftCount;						//��ǰ �Ǹ� ���� �ܿ����� (-1: ������) ��ǰ ���� ������ �����ؾ��Ѵ�.
	long ResultCode;					/*��� �ڵ� 
										-2: DB ����(���������� �������� ����)
										0: ����
										1: ����ĳ�ú���
										2: ���� ��ǰ �����ϱ� �Ұ���
										3: ���� �����ϱ� �ѵ� �ʰ�
										4: �ܿ���������
										5: �ǸűⰣ����
										6: �Ǹ�����(��ǰ������ ���� ���)
										7: ��ǰ���� �Ұ���
										8: �̺�Ʈ��ǰ ���� �Ұ�
										9: �̺�Ʈ ��ǰ ���� ���� Ƚ�� �ʰ�
										-1: ���� �߻�*/
};

class MSG_CLIENT_GIFTPRODUCT : public MSG_BASE<MSG_CLIENT_GIFTPRODUCT>	//(C->S)��ǰ ����
{
public:
	MSG_CLIENT_GIFTPRODUCT() : MSG_BASE<MSG_CLIENT_GIFTPRODUCT>(PROTOCOL_CLIENT_GIFTPRODUCT)
		{}

public:
	long SenderSeq;					//�߽��� ����
	long ReceiverSeq;					//������ ����
	WCHAR Message[200];					//�޽���
	long GameCode;						//�����ڵ�
	long PackageSeq;					//���� ��ǰ ����
	long PriceSeq;						//���� ����	
	long SalesZone;					//�Ǹſ���
	long DisplaySeq;					//�������� ����
};

class MSG_SERVER_GIFTPRODUCT : public MSG_BASE<MSG_SERVER_GIFTPRODUCT>	//(S->C) ��ǰ ����
{
public:
	MSG_SERVER_GIFTPRODUCT() : MSG_BASE<MSG_SERVER_GIFTPRODUCT>(PROTOCOL_SERVER_GIFTPRODUCT),
		LimitedCash(0)
		{}

public:
	long SenderSeq;					//��������
	long ReceiverSeq;					//��������
	long ResultCode;					//��� �ڵ�
	double LimitedCash;					//���� ���� ĳ��
	long LeftProductCount;
};

class MSG_CLIENT_STORAGELIST : public MSG_BASE<MSG_CLIENT_STORAGELIST>	//(C->S) ������ ��ȸ
{
public:
	MSG_CLIENT_STORAGELIST() : MSG_BASE<MSG_CLIENT_STORAGELIST>(PROTOCOL_CLIENT_STORAGELIST),
		AccountID(0), GameCode(0), SalesZone(0)
		{}

public:
	long AccountID;						//���� ����
	long GameCode;						//�������
	int SalesZone;						//�Ǹſ���
	
};

class MSG_SERVER_STORAGELIST : public MSG_BASE<MSG_SERVER_STORAGELIST>	//(S->C) ������ ��ȸ
{
public:
	MSG_SERVER_STORAGELIST() : MSG_BASE<MSG_SERVER_STORAGELIST>(PROTOCOL_SERVER_STORAGELIST),
		AccountID(0), ListCount(0)
	{PackHeader = dwPacketSize;}	

public:
	long PackHeader;
	long AccountID;						//���� ����
	long ListCount;						//������ ��� ����
	long ResultCode;					//���� ����
};

class MSG_CLIENT_GIFTCASH : public MSG_BASE<MSG_CLIENT_GIFTCASH>	//(C->S) ĳ������
{
public:
	MSG_CLIENT_GIFTCASH() : MSG_BASE<MSG_CLIENT_GIFTCASH>(PROTOCOL_CLIENT_GIFTCASH),
		SenderAccountID(0), ReceiverAccountID(0), CashValue(0), GameCode(0)
		{}

public:
	long SenderAccountID;				//������ ���� ����
	long ReceiverAccountID;				//�޴� ���� ����
	WCHAR Message[200];					//���� �޽���
	double CashValue;					//���� ĳ�� ��
	long GameCode;						//�������	
};

class MSG_SERVER_GIFTCASH : public MSG_BASE<MSG_SERVER_GIFTCASH>	//(S->C) ĳ������
{
public:
	MSG_SERVER_GIFTCASH() : MSG_BASE<MSG_SERVER_GIFTCASH>(PROTOCOL_SERVER_GIFTCASH),
		SenderSeq(0), ReceiverSeq(0), ResultCode(0), GiftSendLimit(0)
		{}

public:
	long SenderSeq;					//��������
	long ReceiverSeq;					//��������
	long ResultCode;					//���� ����
	double GiftSendLimit;					//������ ��� ����	
};

class MSG_CLIENT_PRODUCTBUYGIFTCHECK : public MSG_BASE<MSG_CLIENT_PRODUCTBUYGIFTCHECK>	//(C->S) ��ǰ ����/���� ���ɿ��� ��ȸ
{
public:
	MSG_CLIENT_PRODUCTBUYGIFTCHECK() : MSG_BASE<MSG_CLIENT_PRODUCTBUYGIFTCHECK>(PROTOCOL_CLIENT_PRODUCTBUYGIFTCHECK)
		{}

public:
	long AccountID;					//���� ����	
	long GameCode;					//�������	
};

class MSG_SERVER_PRODUCTBUYGIFTCHECK : public MSG_BASE<MSG_SERVER_PRODUCTBUYGIFTCHECK>	//(S->C) ��ǰ ����/���� ���ɿ��� ��ȸ
{
public:
	MSG_SERVER_PRODUCTBUYGIFTCHECK() : MSG_BASE<MSG_SERVER_PRODUCTBUYGIFTCHECK>(PROTOCOL_SERVER_PRODUCTBUYGIFTCHECK),
		ItemBuy(0), Present(0), MyPresentCash(0), MyPresentSendLimit(0)
		{}

public:
	long AccountID;						//��������
	long ResultCode;					//��ǰ����, ��ǰ/ĳ�� ���� ���ɿ��� ��� (0: �Ұ�, 1: ����, -1: DB ó�� ����)
	long ItemBuy;						//��ǰ ���� ���� ���� ��� (0: �Ұ�, 1: ����)
	long Present;						//��ǰ/ĳ�� ���� ���� ���� ��� (1: ����, 2:�����ϱ� �Ұ�, 3: �����ѵ� �ʰ�)
	double MyPresentCash;				//�Ѵ� ���� ���� ������ ĳ�� �հ�
	double MyPresentSendLimit;			//�Ѵ� ���� �����ϱ� ĳ�� �ѵ�

};

class MSG_CLIENT_STORAGEUSE : public MSG_BASE<MSG_CLIENT_STORAGEUSE>	//(C->S) ������ ������ ����ϱ�
{
public:
	MSG_CLIENT_STORAGEUSE() : MSG_BASE<MSG_CLIENT_STORAGEUSE>(PROTOCOL_CLIENT_STORAGEUSE)
		{}

public:	
	long AccountSeq;					//���� ����
	long GameCode;						//�����ڵ�
	DWORD IPAddress;					//���� IP
	long StorageSeq;					//������ ����
	long StorageItemSeq;				//������ Or ��ǰ ����
	char StorageItemType;				//ĳ�� Or ��ǰ ���� (C: ĳ��, P: ��ǰ)
	long Class;							//Ŭ����
	long Level;							//����
	WCHAR CharName[32];					//ĳ���� ��
	WCHAR Rank;							//��ũ
	long ServerIndex;					//�����ε���
};

class MSG_SERVER_STORAGEUSE : public MSG_BASE<MSG_SERVER_STORAGEUSE>	//(S->C) ������ ������ ����ϱ�
{
public:
	MSG_SERVER_STORAGEUSE() : MSG_BASE<MSG_SERVER_STORAGEUSE>(PROTOCOL_SERVER_STORAGEUSE)	
		{PackHeader = dwPacketSize;ProductSeq = 0;}

public:
	long AccountSeq;					//���� ����
	long ResultCode;					/*ó����� 
										0:����
										1:������ ����ϱ� �׸��� ����
										2:PC�濡���� �ޱ� ������ ������
										3:�Ⱓ �� ������� �÷� ����� �ִ� ���
										4:�Ⱓ �� ������� ���� ���� ��ǰ�� �ִ°��
										-1:�����߻�
										-2:DB ���� ����*/

	long PackHeader;
	long ProductSeq;
	BYTE PropertyCount;
};

class MSG_CLIENT_STORAGETHROW : public MSG_BASE<MSG_CLIENT_STORAGETHROW>	//(C->S) ������ ������ ������
{
public:
	MSG_CLIENT_STORAGETHROW() : MSG_BASE<MSG_CLIENT_STORAGETHROW>(PROTOCOL_CLIENT_STORAGETHROW)
		{}

public:
	long AccountSeq;					//���� ����
	long GameCode;						//�����ڵ�
	long StorageSeq;					//������ ����
	long StorageItemSeq;				//������ Or ��ǰ ����
	char StorageItemType;				//ĳ�� Or ��ǰ ���� (C: ĳ��, P: ��ǰ)
};

class MSG_SERVER_STORAGETHROW : public MSG_BASE<MSG_SERVER_STORAGETHROW>	//(S->C) ������ ������ ������
{
public:
	MSG_SERVER_STORAGETHROW() : MSG_BASE<MSG_SERVER_STORAGETHROW>(PROTOCOL_SERVER_STORAGETHROW)
		{}

public:
	long AccountSeq;					//���� ����
	long ResultCode;					/*ó����� 
										0:����
										1:������ ������ �׸��� ����										
										-1:�����߻�
										-2:DB ���� ����*/
};

class MSG_CLIENT_PRODUCTEVENTLIST : public MSG_BASE<MSG_CLIENT_PRODUCTEVENTLIST>	//(C->S) �̺�Ʈ ��ǰ ��� ��ȸ
{
public:
	MSG_CLIENT_PRODUCTEVENTLIST() : MSG_BASE<MSG_CLIENT_PRODUCTEVENTLIST>(PROTOCOL_CLIENT_PRODUCTEVENTLIST)
	{}

public:	
	long AccountSeq;					//���� ����
	long SalesZone;						//�Ǹ� ����
	long ProductDisplaySeq;				//���� ��ǰ ī�װ� ����
};

class MSG_SERVER_PRODUCTEVENTLIST : public MSG_BASE<MSG_SERVER_PRODUCTEVENTLIST>	//(S->C) �̺�Ʈ ��ǰ ��� ��ȸ
{
public:
	MSG_SERVER_PRODUCTEVENTLIST() : MSG_BASE<MSG_SERVER_PRODUCTEVENTLIST>(PROTOCOL_SERVER_PRODUCTEVENTLIST),
		PackagesCount(0)
		{PackHeader = dwPacketSize;}
		
public:
	long PackHeader;
	long AccountSeq;					//���� ����
	long ProductDisplaySeq;				//���� ��ǰ ī�װ� ����
	long PackagesCount;		
};

class MSG_CLIENT_PACKAGELEFTCOUNT : public MSG_BASE<MSG_CLIENT_PACKAGELEFTCOUNT>	//(C->S) ���� ��ǰ �ܿ� ���� ��ȸ
{
public:
	MSG_CLIENT_PACKAGELEFTCOUNT() : MSG_BASE<MSG_CLIENT_PACKAGELEFTCOUNT>(PROTOCOL_CLIENT_PACKAGELEFTCOUNT)
	{}

public:	
	long PackageProductSeq;
};

class MSG_SERVER_PACKAGELEFTCOUNT : public MSG_BASE<MSG_SERVER_PACKAGELEFTCOUNT>	//(S->C) ���� ��ǰ �ܿ� ���� ��ȸ
{
public:
	MSG_SERVER_PACKAGELEFTCOUNT() : MSG_BASE<MSG_SERVER_PACKAGELEFTCOUNT>(PROTOCOL_CLIENT_UPDATEVERSION)
		{}
		
public:
	long PackageProductSeq;
	long LeftCount;
};

class MSG_CLIENT_UPDATEVERSION : public MSG_BASE<MSG_CLIENT_UPDATEVERSION>	//(C->S) ���� ������Ʈ
{
public:
	MSG_CLIENT_UPDATEVERSION() : MSG_BASE<MSG_CLIENT_UPDATEVERSION>(PROTOCOL_CLIENT_UPDATEVERSION)
	{}

public:	
	unsigned short SaleZone;
	unsigned short year;
	unsigned short yearIdentity;
};

class MSG_SERVER_UPDATEVERSION : public MSG_BASE<MSG_SERVER_UPDATEVERSION>	//(S->C) ���� ������Ʈ
{
public:
	MSG_SERVER_UPDATEVERSION() : MSG_BASE<MSG_SERVER_UPDATEVERSION>(PROTOCOL_SERVER_UPDATEVERSION)
		{}
		
public:
	unsigned short SaleZone;
	unsigned short year;
	unsigned short yearIdentity;
};

/*
class MSG_CLIENT_PRODUCTLIST : public MSG_BASE<MSG_CLIENT_PRODUCTLIST>	//(C->S) ��ǰ ��� ��ȸ
{
public:
	MSG_CLIENT_PRODUCTLIST() : MSG_BASE<MSG_CLIENT_PRODUCTLIST>(PROTOCOL_CLIENT_PRODUCTLIST)
		{}

public:
	long SalesZone;						//�Ǹ� ����
};

class MSG_SERVER_PRODUCTLIST : public MSG_BASE<MSG_SERVER_PRODUCTLIST>	//(S->C) ��ǰ ��� ��ȸ
{
public:
	MSG_SERVER_PRODUCTLIST() : MSG_BASE<MSG_SERVER_PRODUCTLIST>(PROTOCOL_SERVER_PRODUCTLIST),
		CategorysCount(0), PackagesCount(0), ProductCount(0)
		{PackHeader = dwPacketSize;}
	
public:
	long PackHeader;
	int CategorysCount;
	int PackagesCount;
	int ProductCount;	
};

class MSG_CLIENT_PRODUCTEVENTLIST : public MSG_BASE<MSG_CLIENT_PRODUCTEVENTLIST>	//(C->S) �̺�Ʈ ��ǰ ��� ��ȸ
{
public:
	MSG_CLIENT_PRODUCTEVENTLIST() : MSG_BASE<MSG_CLIENT_PRODUCTEVENTLIST>(PROTOCOL_CLIENT_PRODUCTEVENTLIST)
	{}

public:	
	long AccountSeq;					//���� ����
	long SalesZone;						//�Ǹ� ����
	long ProductDisplaySeq;				//���� ��ǰ ī�װ� ����
};

class MSG_SERVER_PRODUCTEVENTLIST : public MSG_BASE<MSG_SERVER_PRODUCTEVENTLIST>	//(S->C) �̺�Ʈ ��ǰ ��� ��ȸ
{
public:
	MSG_SERVER_PRODUCTEVENTLIST() : MSG_BASE<MSG_SERVER_PRODUCTEVENTLIST>(PROTOCOL_SERVER_PRODUCTEVENTLIST),
		PackagesCount(0)
		{PackHeader = dwPacketSize;}
		
public:
	long PackHeader;
	long AccountSeq;					//���� ����
	long ProductDisplaySeq;				//���� ��ǰ ī�װ� ����
	int PackagesCount;	
};


class MSG_CLIENT_UPDATELIST : public MSG_BASE<MSG_CLIENT_UPDATELIST>	//(C->S) ��ǰ ������ ������Ʈ �Ѵ�.
{
public:
	MSG_CLIENT_UPDATELIST() : MSG_BASE<MSG_CLIENT_UPDATELIST>(PROTOCOL_CLIENT_UPDATELIST)
	{}

public:	
	long UpdateType;					//������Ʈ Ÿ��
	long ProcessType;					//���μ��� Ÿ��(440:���, 441:����, 442:����)
	long TypeValue;						//������Ʈ Ÿ�Կ� ���� ��
};

class MSG_SERVER_UPDATELIST : public MSG_BASE<MSG_SERVER_UPDATELIST>	//(S->C) ��ǰ ������ ������Ʈ �Ѵ�.
{
public:
	MSG_SERVER_UPDATELIST() : MSG_BASE<MSG_SERVER_UPDATELIST>(PROTOCOL_SERVER_UPDATELIST)
		{}
		
public:	
};

class MSG_SERVER_UPDATECATEGORYLIST : public MSG_BASE<MSG_SERVER_UPDATECATEGORYLIST>	//(S->C) ���� ��ǰ ������ ������Ʈ �Ѵ�.
{
public:
	MSG_SERVER_UPDATECATEGORYLIST() : MSG_BASE<MSG_SERVER_UPDATECATEGORYLIST>(PROTOCOL_SERVER_UPDATECATEGORYLIST)
		{PackHeader = dwPacketSize;}
		
public:	
	long PackHeader;
	int CategorysCount;
	int PackagesCount;	
	long ProcessType;					//���μ��� Ÿ��(440:���, 441:����, 442:����)
};

class MSG_SERVER_UPDATEPRODUCTLIST : public MSG_BASE<MSG_SERVER_UPDATEPRODUCTLIST>	//(S->C) ���� ��ǰ ������ ������Ʈ �Ѵ�.
{
public:
	MSG_SERVER_UPDATEPRODUCTLIST() : MSG_BASE<MSG_SERVER_UPDATEPRODUCTLIST>(PROTOCOL_SERVER_UPDATEPRODUCTLIST)
		{PackHeader = dwPacketSize;}
		
public:	
	long PackHeader;
	long ProcessType;					//���μ��� Ÿ��(440:���, 441:����, 442:����)
	int ProductCount;
};
*/


}


#pragma pack()