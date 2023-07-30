#pragma once
#pragma pack(1)

/**************************************************************************************************

�ۼ���: 2009-08-14
�ۼ���: ������ (youngmoon@webzen.co.kr)

����: �Ϲ� �佽�� �������� �������� ����

**************************************************************************************************/

#include "ShopDefine.h"


namespace HxEnShopProtocol
{

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
	//const char *id, const char *idno, int *rtn, char *msg, int *totbal, int *chrgbal, int *inevtbal, int *outevtbal, int *giftbal, int *prmrbal
	char* id;			//����� ���̵�
	char* idno;			//����� ��ȣ
};

class MSG_SERVER_INQURE_CASH : public MSG_BASE<MSG_SERVER_INQURE_CASH>	//(S->C) ����� ĳ�� ��ȸ
{
public:
	MSG_SERVER_INQURE_CASH() : MSG_BASE<MSG_SERVER_INQURE_CASH>(PROTOCOL_SERVER_INQURE_CASH) 	  
	{}

public:
	int rtn;				//ó����� �ڵ�
	char msg[1024];			//���� �޽���
	int totbal;				//���ܾ�
	int chrgbal;			//���� ���� �ܾ�
	int inevtbal;			//�����̺�Ʈ �ܾ�
	int outevtbal;			//�ܺ� �̺�Ʈ �ܾ�
	int giftbal;			//�������� �ܾ�
	int prmrbal;			//�����̾� �ܾ�
};	

class MSG_CLIENT_BUYPRODUCT : public MSG_BASE<MSG_CLIENT_BUYPRODUCT>	//(C->S)��ǰ ����
{
public:
	MSG_CLIENT_BUYPRODUCT() : MSG_BASE<MSG_CLIENT_BUYPRODUCT>(PROTOCOL_CLIENT_BUYPRODUCT)
	{}

public:
	//virtual RET_CODE useCoin(const char *id, const char *idno, const char *prodkind, const char *prodcode, const char *snm, int samt, const char *refkey, const char *ip, char *sid, int *rtn, int *bal, char *msg);
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
	//virtual RET_CODE useCoinGift(const char *id, const char *idno, const char *prodkind, const char *prodcode, const char *snm, int samt, const char *refkey, const char *ip, const char *receiverid, char *sid, int *rtn, int *bal, char *msg);
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

}


#pragma pack()