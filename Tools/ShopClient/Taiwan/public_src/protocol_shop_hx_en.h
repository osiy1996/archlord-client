#pragma once
#pragma pack(1)

/**************************************************************************************************

작성일: 2009-08-14
작성자: 문상현 (youngmoon@webzen.co.kr)

설명: 북미 헉슬리 빌링서버 프로토콜 정의

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
	DWORD 	dwPacketSize; 					//패킷의 전체 사이즈
	DWORD	dwProtocolID;					//패킷 아이디
};

/*
////////////////////////////////////////////////////////////////////////////////////////////////////////
샵 서버 프로토콜
////////////////////////////////////////////////////////////////////////////////////////////////////////
*/


class MSG_CLIENT_INQURE_CASH : public MSG_BASE<MSG_CLIENT_INQURE_CASH>	//(C->S) 사용자 캐쉬 조회
{
public:
	MSG_CLIENT_INQURE_CASH() : MSG_BASE<MSG_CLIENT_INQURE_CASH>(PROTOCOL_CLIENT_INQURE_CASH)
		{}

public:
	//const char *id, const char *idno, int *rtn, char *msg, int *totbal, int *chrgbal, int *inevtbal, int *outevtbal, int *giftbal, int *prmrbal
	char* id;			//사용자 아이디
	char* idno;			//사용자 번호
};

class MSG_SERVER_INQURE_CASH : public MSG_BASE<MSG_SERVER_INQURE_CASH>	//(S->C) 사용자 캐쉬 조회
{
public:
	MSG_SERVER_INQURE_CASH() : MSG_BASE<MSG_SERVER_INQURE_CASH>(PROTOCOL_SERVER_INQURE_CASH) 	  
	{}

public:
	int rtn;				//처리결과 코드
	char msg[1024];			//에러 메시지
	int totbal;				//총잔액
	int chrgbal;			//유료 충전 잔액
	int inevtbal;			//내부이벤트 잔액
	int outevtbal;			//외부 이벤트 잔액
	int giftbal;			//선물받은 잔액
	int prmrbal;			//프리미어 잔액
};	

class MSG_CLIENT_BUYPRODUCT : public MSG_BASE<MSG_CLIENT_BUYPRODUCT>	//(C->S)상품 구매
{
public:
	MSG_CLIENT_BUYPRODUCT() : MSG_BASE<MSG_CLIENT_BUYPRODUCT>(PROTOCOL_CLIENT_BUYPRODUCT)
	{}

public:
	//virtual RET_CODE useCoin(const char *id, const char *idno, const char *prodkind, const char *prodcode, const char *snm, int samt, const char *refkey, const char *ip, char *sid, int *rtn, int *bal, char *msg);
};

class MSG_SERVER_BUYPRODUCT : public MSG_BASE<MSG_SERVER_BUYPRODUCT>	//(S->C) 상품 구매
{
public:
	MSG_SERVER_BUYPRODUCT() : MSG_BASE<MSG_SERVER_BUYPRODUCT>(PROTOCOL_SERVER_BUYPRODUCT)
		{}

public:
	DWORD AccountSeq;					//계정순번
	long LeftCount;						//상품 판매 가능 잔여수량 (-1: 무제한) 상품 개수 정보를 갱신해야한다.
	long ResultCode;					/*결과 코드 
										-2: DB 오류(정상적으로 수행하지 못함)
										0: 성공
										1: 보유캐시부족
										2: 나의 상품 선물하기 불가능
										3: 나의 선물하기 한도 초과
										4: 잔여수량부족
										5: 판매기간종료
										6: 판매종료(사품정보가 없는 경우)
										7: 상품선물 불가능
										8: 이벤트상품 선물 불가
										9: 이벤트 상품 선물 가능 횟수 초과
										-1: 오류 발생*/
};

class MSG_CLIENT_GIFTPRODUCT : public MSG_BASE<MSG_CLIENT_GIFTPRODUCT>	//(C->S)상품 선물
{
public:
	MSG_CLIENT_GIFTPRODUCT() : MSG_BASE<MSG_CLIENT_GIFTPRODUCT>(PROTOCOL_CLIENT_GIFTPRODUCT)
		{}

public:
	//virtual RET_CODE useCoinGift(const char *id, const char *idno, const char *prodkind, const char *prodcode, const char *snm, int samt, const char *refkey, const char *ip, const char *receiverid, char *sid, int *rtn, int *bal, char *msg);
};

class MSG_SERVER_GIFTPRODUCT : public MSG_BASE<MSG_SERVER_GIFTPRODUCT>	//(S->C) 상품 선물
{
public:
	MSG_SERVER_GIFTPRODUCT() : MSG_BASE<MSG_SERVER_GIFTPRODUCT>(PROTOCOL_SERVER_GIFTPRODUCT),
		LimitedCash(0)
		{}

public:
	long SenderSeq;					//계정순번
	long ReceiverSeq;					//계정순번
	long ResultCode;					//결과 코드
	double LimitedCash;					//선물 가능 캐쉬
	long LeftProductCount;
};

}


#pragma pack()