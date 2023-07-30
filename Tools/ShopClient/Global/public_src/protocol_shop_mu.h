#pragma once
#pragma pack(1)

/**************************************************************************************************

�ۼ���: 2008-07-18
�ۼ���: ������ (youngmoon@webzen.co.kr)

����: ���� �� �������� �������� ����

**************************************************************************************************/

#include "ShopDefine.h"

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
	long ShareFlag;				//ServerType(��������) �� ������ �׸� ��ȸ ���� �׸� ����
								/*435	Ŭ���� ���̺� (��������-���Ἥ��)
								436	Ŭ���� �׽�Ʈ (��������-�׽�Ʈ����)
								470	ī���� ���̺� (�κ�����-���Ἥ��)
								471	ī���� �׽�Ʈ (�κ�����-�׽�Ʈ����)
								550	�佽�� ���̺�*/

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

template <class _subcls>
class MSG_GAMECODE_BASE : public MSG_BASE<_subcls>	//�����ڵ�� �б� �Ǿ� �ϴ� �༮���� �̰��� ��ӹ޴´�.
{
public:	
	MSG_GAMECODE_BASE(DWORD dwProtocol) : MSG_BASE<_subcls>(dwProtocol) {}

protected:	
	long GameCode;						//���� �ڵ�	

public:
	long GetGameCode() {return GameCode;};
};

class MSG_CLIENT_INQURE_CASH_MU : public MSG_GAMECODE_BASE<MSG_CLIENT_INQURE_CASH_MU>	//(C->S) ����� ĳ�� ��ȸ (��)
{
public:
	MSG_CLIENT_INQURE_CASH_MU() : MSG_GAMECODE_BASE<MSG_CLIENT_INQURE_CASH_MU>(PROTOCOL_CLIENT_INQURE_CASH)
		{GameCode = 112;}

public:
	BYTE ViewType;						//��ȸ ���� (0:��ü(ĳ��+����Ʈ), 1:ĳ��(�Ϲ�+�̺�Ʈ), 2:����Ʈ(���ʽ�+����), 3:���ʽ�����Ʈ, 4:��������Ʈ, 5:ĳ��(�Ϲ�), 6:ĳ��(�Ϲ�))
	DWORD AccountSeq;					//��������	
	bool SumOnly;						//�� �հ踸 ��ȸ ���� (true: ������ ���� ����, false: ������ ���� + ĳ�� ������ ���(STDetail �� ���))
	long MileageSection;					//���ϸ��� ���� (438:��� ,439:QT)
};


class MSG_CLIENT_INQURE_CASH_HX_KR : public MSG_GAMECODE_BASE<MSG_CLIENT_INQURE_CASH_HX_KR>	//(C->S) ����� ĳ�� ��ȸ (�佽�� ����)
{
public:
	MSG_CLIENT_INQURE_CASH_HX_KR() : MSG_GAMECODE_BASE<MSG_CLIENT_INQURE_CASH_HX_KR>(PROTOCOL_CLIENT_INQURE_CASH)
		{GameCode = 115;}

public:
	char AccoundID[20]; //���� ����
};



class MSG_SERVER_INQURE_CASH_MU : public MSG_GAMECODE_BASE<MSG_SERVER_INQURE_CASH_MU>	//(S->C) ����� ĳ�� ��ȸ (��)
{
public:
	MSG_SERVER_INQURE_CASH_MU() : MSG_GAMECODE_BASE<MSG_SERVER_INQURE_CASH_MU>(PROTOCOL_SERVER_INQURE_CASH)
	{
		GameCode = 112;
		PackHeader = dwPacketSize;
		sum[0] = 0;//ĳ��(�Ϲ�, �̺�Ʈ) ����
		sum[1] = 0;//����Ʈ(���ʽ�, ����) ����
		sum[2] = 0;//���ϸ���(���, QT) ����
	}

public:
	DWORD AccountSeq;					//��������
	long ResultCode;					//��� �ڵ�
	long PackHeader;	
	long ListCount;	
	double sum[3];						//������ ���� ���� (0: ĳ�� ����, 1: ����Ʈ ����)	
};	

class MSG_SERVER_INQURE_CASH_HX_KR : public MSG_GAMECODE_BASE<MSG_SERVER_INQURE_CASH_HX_KR>	//(S->C) ����� ĳ�� ��ȸ (�佽�� ����)
{
public:
	MSG_SERVER_INQURE_CASH_HX_KR() : MSG_GAMECODE_BASE<MSG_SERVER_INQURE_CASH_HX_KR>(PROTOCOL_SERVER_INQURE_CASH)
	{	
		GameCode = 115;
	}

public:
	char AccoundID[20];					//���� ����
	long ResultCode;					//��� �ڵ�
	long HanCoin;						//�Ѱ��� ������ ����
};	


class MSG_CLIENT_BUYPRODUCT_MU : public MSG_GAMECODE_BASE<MSG_CLIENT_BUYPRODUCT_MU>	//(C->S)��ǰ ���� (��)
{
public:
	MSG_CLIENT_BUYPRODUCT_MU() : MSG_GAMECODE_BASE<MSG_CLIENT_BUYPRODUCT_MU>(PROTOCOL_CLIENT_BUYPRODUCT)
	{GameCode = 112;}

public:
	DWORD AccountSeq;					//��������	
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

class MSG_CLIENT_BUYPRODUCT_HX_KR : public MSG_GAMECODE_BASE<MSG_CLIENT_BUYPRODUCT_HX_KR>	//(C->S)��ǰ ���� (�佽�� ����)
{
public:
	MSG_CLIENT_BUYPRODUCT_HX_KR() : MSG_GAMECODE_BASE<MSG_CLIENT_BUYPRODUCT_HX_KR>(PROTOCOL_CLIENT_BUYPRODUCT)
	{GameCode = 115;}

public:
	char AccoundID[20];					//���� ����
	DWORD AccountSeq;					//��������	
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

class MSG_SERVER_BUYPRODUCT_MU : public MSG_GAMECODE_BASE<MSG_SERVER_BUYPRODUCT_MU>	//(S->C) ��ǰ ���� (��)
{
public:
	MSG_SERVER_BUYPRODUCT_MU() : MSG_GAMECODE_BASE<MSG_SERVER_BUYPRODUCT_MU>(PROTOCOL_SERVER_BUYPRODUCT)
		{LeftCount = 0;GameCode = 112;}

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

class MSG_SERVER_BUYPRODUCT_HX_KR : public MSG_GAMECODE_BASE<MSG_SERVER_BUYPRODUCT_HX_KR>	//(S->C) ��ǰ ���� (�佽�� ����)
{
public:
	MSG_SERVER_BUYPRODUCT_HX_KR() : MSG_GAMECODE_BASE<MSG_SERVER_BUYPRODUCT_HX_KR>(PROTOCOL_SERVER_BUYPRODUCT)
		{LeftCount = 0;GameCode = 115;}

public:
	char AccoundID[20];					//���� ����
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

class MSG_CLIENT_GIFTPRODUCT_MU : public MSG_GAMECODE_BASE<MSG_CLIENT_GIFTPRODUCT_MU>	//(C->S)��ǰ ���� (��)
{
public:
	MSG_CLIENT_GIFTPRODUCT_MU() : MSG_GAMECODE_BASE<MSG_CLIENT_GIFTPRODUCT_MU>(PROTOCOL_CLIENT_GIFTPRODUCT)
		{GameCode = 112;}

public:
	DWORD SenderSeq;					//�߽��� ����
	long SenderServerIndex;			//�߽��� ���� ��ȣ
	WCHAR SenderCharName[30];		//�߽��� ĳ���� ��
	DWORD ReceiverSeq;				//������ ����
	long ReceiverServerIndex;		//������ ���� ��ȣ
	WCHAR ReceiverCharName[30];		//������ ĳ���� ��
	WCHAR Message[200];				//�޽���	
	long PackageSeq;				//���� ��ǰ ����
	long PriceSeq;					//���� ����	
	long SalesZone;					//�Ǹſ���
	long DisplaySeq;				//�������� ����
};

class MSG_CLIENT_GIFTPRODUCT_HX_KR : public MSG_GAMECODE_BASE<MSG_CLIENT_GIFTPRODUCT_HX_KR>	//(C->S)��ǰ ���� (�佽�� ����)
{
public:
	MSG_CLIENT_GIFTPRODUCT_HX_KR() : MSG_GAMECODE_BASE<MSG_CLIENT_GIFTPRODUCT_HX_KR>(PROTOCOL_CLIENT_GIFTPRODUCT)
		{GameCode = 115;}

public:
	char AccoundID[20];					//���� ����
	DWORD SenderSeq;					//�߽��� ����
	long SenderServerIndex;			//�߽��� ���� ��ȣ
	WCHAR SenderCharName[30];		//�߽��� ĳ���� ��
	DWORD ReceiverSeq;				//������ ����
	long ReceiverServerIndex;		//������ ���� ��ȣ
	WCHAR ReceiverCharName[30];		//������ ĳ���� ��
	WCHAR Message[200];				//�޽���	
	long PackageSeq;				//���� ��ǰ ����
	long PriceSeq;					//���� ����	
	long SalesZone;					//�Ǹſ���
	long DisplaySeq;				//�������� ����
};

class MSG_SERVER_GIFTPRODUCT_MU : public MSG_GAMECODE_BASE<MSG_SERVER_GIFTPRODUCT_MU>	//(S->C) ��ǰ ���� (��)
{
public:
	MSG_SERVER_GIFTPRODUCT_MU() : MSG_GAMECODE_BASE<MSG_SERVER_GIFTPRODUCT_MU>(PROTOCOL_SERVER_GIFTPRODUCT),
		LimitedCash(0)
		{GameCode = 112;}

public:
	DWORD SenderSeq;					//��������
	DWORD ReceiverSeq;					//��������
	long ResultCode;					//��� �ڵ�
	double LimitedCash;					//���� ���� ĳ��
	long LeftProductCount;
};

class MSG_SERVER_GIFTPRODUCT_HX_KR : public MSG_GAMECODE_BASE<MSG_SERVER_GIFTPRODUCT_HX_KR>	//(S->C) ��ǰ ���� (�佽�� ����)
{
public:
	MSG_SERVER_GIFTPRODUCT_HX_KR() : MSG_GAMECODE_BASE<MSG_SERVER_GIFTPRODUCT_HX_KR>(PROTOCOL_SERVER_GIFTPRODUCT),
		LimitedCash(0)
		{GameCode = 115;}

public:
	char AccoundID[20];					//���� ����
	DWORD SenderSeq;					//��������
	DWORD ReceiverSeq;					//��������
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
	DWORD AccountID;						//���� ����
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
	DWORD AccountID;						//���� ����
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
	DWORD SenderAccountID;				//������ ���� ����
	WCHAR SendCharName[30];				//�߽��� ĳ���� ��
	long SendServerIndex;				//�߽��� ���� ��ȣ
	DWORD ReceiverAccountID;				//�޴� ���� ����
	WCHAR ReceiverCharName[30];				//������ ĳ���� ��
	long ReceiverServerIndex;				//������ ���� ��ȣ
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
	DWORD SenderSeq;					//��������
	DWORD ReceiverSeq;					//��������
	long ResultCode;					//���� ����
	double GiftSendLimit;					//������ ��� ����	
};

class MSG_CLIENT_PRODUCTBUYGIFTCHECK : public MSG_BASE<MSG_CLIENT_PRODUCTBUYGIFTCHECK>	//(C->S) ��ǰ ����/���� ���ɿ��� ��ȸ
{
public:
	MSG_CLIENT_PRODUCTBUYGIFTCHECK() : MSG_BASE<MSG_CLIENT_PRODUCTBUYGIFTCHECK>(PROTOCOL_CLIENT_PRODUCTBUYGIFTCHECK)
		{}

public:
	DWORD AccountID;					//���� ����	
	long GameCode;					//�������	
};

class MSG_SERVER_PRODUCTBUYGIFTCHECK : public MSG_BASE<MSG_SERVER_PRODUCTBUYGIFTCHECK>	//(S->C) ��ǰ ����/���� ���ɿ��� ��ȸ
{
public:
	MSG_SERVER_PRODUCTBUYGIFTCHECK() : MSG_BASE<MSG_SERVER_PRODUCTBUYGIFTCHECK>(PROTOCOL_SERVER_PRODUCTBUYGIFTCHECK),
		ItemBuy(0), Present(0), MyPresentCash(0), MyPresentSendLimit(0)
		{}

public:
	DWORD AccountID;						//��������
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
	DWORD AccountSeq;					//���� ����
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
	DWORD AccountSeq;					//���� ����
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
	DWORD AccountSeq;					//���� ����
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
	DWORD AccountSeq;					//���� ����
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
	DWORD AccountSeq;					//���� ����
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
	DWORD AccountSeq;					//���� ����
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


class MSG_CLIENT_MILEAGESAVE : public MSG_BASE<MSG_CLIENT_MILEAGESAVE>	//(C->S) ���ϸ��� ����
{
public:
	MSG_CLIENT_MILEAGESAVE() : MSG_BASE<MSG_CLIENT_MILEAGESAVE>(PROTOCOL_CLIENT_MILEAGESAVE)
	{}

public:		
	DWORD AccountSeq;		//���� ȸ�� ���� ����
	long GameCode;			//���� �ڵ�
	long MileageType;		//���ϸ�����������
	long MileageSection;	//���ϸ�������
	long MileagePoint;		//���ϸ��� ���� ����Ʈ

};

class MSG_SERVER_MILEAGESAVE : public MSG_BASE<MSG_SERVER_MILEAGESAVE>	//(S->C) ���ϸ��� ����
{
public:
	MSG_SERVER_MILEAGESAVE() : MSG_BASE<MSG_SERVER_MILEAGESAVE>(PROTOCOL_SERVER_MILEAGESAVE)
		{}
		
public:
	DWORD AccountSeq;		//���� ȸ�� ���� ����
	long ResultCode;		//��ȸ ��� �ڵ� 	0 : ����, -1 :  �����߻�

};

class MSG_CLIENT_MILEAGEDEDUCT : public MSG_BASE<MSG_CLIENT_MILEAGEDEDUCT>	//(C->S) ���ϸ��� ����
{
public:
	MSG_CLIENT_MILEAGEDEDUCT() : MSG_BASE<MSG_CLIENT_MILEAGEDEDUCT>(PROTOCOL_CLIENT_MILEAGEDEDUCT)
	{}

public:		
	DWORD AccountSeq;//	���� ȸ�� ���� ����
	long GameCode;//	���� �ڵ�
	long DeductCategory;//	���ϸ�����������
	long MileageSection;//	���ϸ�������
	long MileageDeductPoint;//	���ϸ��� ���� ����Ʈ
	long Class;//	Ŭ����
	long Level;//	����
	WCHAR CharName[32];//	ĳ���͸�
	WCHAR Rank;//	���
	long ServerIndex;//	���� ��ȣ
};

class MSG_SERVER_MILEAGEDEDUCT : public MSG_BASE<MSG_SERVER_MILEAGEDEDUCT>	//(S->C) ���ϸ��� ����
{
public:
	MSG_SERVER_MILEAGEDEDUCT() : MSG_BASE<MSG_SERVER_MILEAGEDEDUCT>(PROTOCOL_SERVER_MILEAGEDEDUCT)
		{}
		
public:
	DWORD AccountSeq;		//���� ȸ�� ���� ����
	long ResultCode;		//��ȸ ��� �ڵ� 	0 : ����, -1 :  �����߻�
};

class MSG_CLIENT_MILEAGELIVESAVEUP : public MSG_BASE<MSG_CLIENT_MILEAGELIVESAVEUP>	//(C->S) �ǽð� ���ϸ��� ����
{
public:
	MSG_CLIENT_MILEAGELIVESAVEUP() : MSG_BASE<MSG_CLIENT_MILEAGELIVESAVEUP>(PROTOCOL_CLIENT_MILEAGELIVESAVEUP)
	{}

public:		
	DWORD AccountSeq;//	���� ȸ�� ���� ����
	long GameCode;//	���� �ڵ�	
	long MileageSection;//	���ϸ�������
	long SourceType;//����, PC�� ����
};

class MSG_SERVER_MILEAGELIVESAVEUP : public MSG_BASE<MSG_SERVER_MILEAGELIVESAVEUP>	//(S->C) �ǽð� ���ϸ��� ����
{
public:
	MSG_SERVER_MILEAGELIVESAVEUP() : MSG_BASE<MSG_SERVER_MILEAGELIVESAVEUP>(PROTOCOL_SERVER_MILEAGELIVESAVEUP)
		{}
		
public:
	DWORD AccountSeq;		//���� ȸ�� ���� ����
	long ResultCode;		//��ȸ ��� �ڵ� 	0 : ����, -1 :  �����߻�
};

class MSG_CLIENT_ITEMSERIALUPDATE : public MSG_BASE<MSG_CLIENT_ITEMSERIALUPDATE>	//(C->S) ������ �ø��� ������Ʈ
{
public:
	MSG_CLIENT_ITEMSERIALUPDATE() : MSG_BASE<MSG_CLIENT_ITEMSERIALUPDATE>(PROTOCOL_CLIENT_ITEMSERIALUPDATE)
	{}

public:	
	DWORD AccountSeq;		//ȸ�� ���� ����
	long GameCode;			//���� �ڵ�
	long StorageSeq;		//������ ����
	long StorageItemSeq;	//������ ĳ�� or ��ǰ ����
	INT64 InGameUseCode;	//������ �ø��� �ڵ�	
};

class MSG_SERVER_ITEMSERIALUPDATE : public MSG_BASE<MSG_SERVER_ITEMSERIALUPDATE>	//(S->C) ������ �ø��� ������Ʈ
{
public:
	MSG_SERVER_ITEMSERIALUPDATE() : MSG_BASE<MSG_SERVER_ITEMSERIALUPDATE>(PROTOCOL_SERVER_ITEMSERIALUPDATE)
		{}
		
public:
	DWORD AccountSeq;		//���� ȸ�� ���� ����
	long ResultCode;		//��ȸ ��� �ڵ� 	0 : ����, 1: ��� �׸� ����, -1 :  �����߻�
};

}


#pragma pack()