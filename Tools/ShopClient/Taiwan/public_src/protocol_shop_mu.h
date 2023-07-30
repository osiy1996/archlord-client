#pragma once
#pragma pack(1)

/**************************************************************************************************

작성일: 2008-07-18
작성자: 문상현 (youngmoon@webzen.co.kr)

설명: 국내 뮤 빌링서버 프로토콜 정의

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

struct STDetail //캐쉬 조회에서 사용
{
	STDetail()
	{
		ZeroMemory(Name, 20 * sizeof(WCHAR));
		Value = 0;			
		Type = 0;			
	};
	
	WCHAR Name[20];						//캐쉬유형 이나 포인트유형 이름
	double Value;						//캐쉬나 포인트 값
	char Type;							//캐쉬인지 포인트 인지 구분 (C:캐쉬, P:포인트 0:없음)
};

struct STStorage //보관함 정보
{
	long Seq;					//보관함 순번
	long ItemSeq;				//보관함 상품 or 캐시 항목 순번
	long GroupCode;				//보관함 그룹코드
	long ShareFlag;				//ServerType(서버유형) 간 보관함 항목 조회 공유 항목 여부
								/*435	클래식 라이브 (정액정량-유료서버)
								436	클래식 테스트 (정액정량-테스트서버)
								470	카오스 라이브 (부분유료-유료서버)
								471	카오스 테스트 (부분유료-테스트서버)
								550	헉슬리 라이브*/

	long ProductSeq;			//단위상품 코드
	WCHAR CashName[20];			//캐쉬 명
	double CashPoint;			//웹젠 캐시 포인트
	WCHAR SendAccountID[50];	//발신자 회원 아이디
	WCHAR SendMessage[200];		//발신 메시지 - 선물일 경우 존재
	char ItemType;				//상품 캐시 구분 (P:상품, C:캐시)
	BYTE RelationType;			//보관함 유형 구분 (1:구매,시리얼,무료지급 , 2:선물)
};

struct STProductList_PRODUCT	//상품 목록 리스트 단위상품의 가격
{
	long Seq;					//단위 상품 순번
	WCHAR Name[50];				//단위 상품 명
	WCHAR PropertyName[50];		//속성 명
	WCHAR Value[255];			//속성 값
	WCHAR UnitName[20];			//속성 단위 명
	long Price;					//단위 상품 가격
	long PriceSeq;				//단위 상품 가격 순번
	long PropertyType;			//속성 유형 (141:아이템 속성, 142:가격 속성)
	long MustFlag;				//필수 여부 (145:필수, 146:선택)
	long vOrder;				//메인 속성 구분(1: 메인속성, 9:서브속성)
	long Deleteflag;			//삭제 플러그 (144: 활성, 143: 삭제)
};


////////////////////////////////////////


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

template <class _subcls>
class MSG_GAMECODE_BASE : public MSG_BASE<_subcls>	//게임코드로 분기 되야 하는 녀석들은 이것을 상속받는다.
{
public:	
	MSG_GAMECODE_BASE(DWORD dwProtocol) : MSG_BASE<_subcls>(dwProtocol) {}

protected:	
	long GameCode;						//게임 코드	

public:
	long GetGameCode() {return GameCode;};
};

class MSG_CLIENT_INQURE_CASH_MU : public MSG_GAMECODE_BASE<MSG_CLIENT_INQURE_CASH_MU>	//(C->S) 사용자 캐쉬 조회 (뮤)
{
public:
	MSG_CLIENT_INQURE_CASH_MU() : MSG_GAMECODE_BASE<MSG_CLIENT_INQURE_CASH_MU>(PROTOCOL_CLIENT_INQURE_CASH)
		{GameCode = 112;}

public:
	BYTE ViewType;						//조회 유형 (0:전체(캐쉬+포인트), 1:캐쉬(일반+이벤트), 2:포인트(보너스+게임), 3:보너스포인트, 4:게임포인트, 5:캐쉬(일반), 6:캐시(일반))
	DWORD AccountSeq;					//계정순번	
	bool SumOnly;						//총 합계만 조회 여부 (true: 유형별 종합 값만, false: 유형별 종합 + 캐시 유형별 결과(STDetail 값 출력))
	long MileageSection;					//마일리지 유형 (438:고블린 ,439:QT)
};


class MSG_CLIENT_INQURE_CASH_HX_KR : public MSG_GAMECODE_BASE<MSG_CLIENT_INQURE_CASH_HX_KR>	//(C->S) 사용자 캐쉬 조회 (헉슬리 국내)
{
public:
	MSG_CLIENT_INQURE_CASH_HX_KR() : MSG_GAMECODE_BASE<MSG_CLIENT_INQURE_CASH_HX_KR>(PROTOCOL_CLIENT_INQURE_CASH)
		{GameCode = 115;}

public:
	char AccoundID[20]; //게임 계정
};



class MSG_SERVER_INQURE_CASH_MU : public MSG_GAMECODE_BASE<MSG_SERVER_INQURE_CASH_MU>	//(S->C) 사용자 캐쉬 조회 (뮤)
{
public:
	MSG_SERVER_INQURE_CASH_MU() : MSG_GAMECODE_BASE<MSG_SERVER_INQURE_CASH_MU>(PROTOCOL_SERVER_INQURE_CASH)
	{
		GameCode = 112;
		PackHeader = dwPacketSize;
		sum[0] = 0;//캐시(일반, 이벤트) 총합
		sum[1] = 0;//포인트(보너스, 게임) 총합
		sum[2] = 0;//마일리지(고블린, QT) 총합
	}

public:
	DWORD AccountSeq;					//계정순번
	long ResultCode;					//결과 코드
	long PackHeader;	
	long ListCount;	
	double sum[3];						//유형별 종합 정보 (0: 캐쉬 총합, 1: 포인트 총합)	
};	

class MSG_SERVER_INQURE_CASH_HX_KR : public MSG_GAMECODE_BASE<MSG_SERVER_INQURE_CASH_HX_KR>	//(S->C) 사용자 캐쉬 조회 (헉슬리 국내)
{
public:
	MSG_SERVER_INQURE_CASH_HX_KR() : MSG_GAMECODE_BASE<MSG_SERVER_INQURE_CASH_HX_KR>(PROTOCOL_SERVER_INQURE_CASH)
	{	
		GameCode = 115;
	}

public:
	char AccoundID[20];					//게임 계정
	long ResultCode;					//결과 코드
	long HanCoin;						//한게임 코인의 총합
};	


class MSG_CLIENT_BUYPRODUCT_MU : public MSG_GAMECODE_BASE<MSG_CLIENT_BUYPRODUCT_MU>	//(C->S)상품 구매 (뮤)
{
public:
	MSG_CLIENT_BUYPRODUCT_MU() : MSG_GAMECODE_BASE<MSG_CLIENT_BUYPRODUCT_MU>(PROTOCOL_CLIENT_BUYPRODUCT)
	{GameCode = 112;}

public:
	DWORD AccountSeq;					//계정순번	
	long PackageSeq;					//포장 상품 순번
	long PriceSeq;						//가격 순번	
	long SalesZone;						//판매영역
	long DisplaySeq;					//전시정보 순번
	long Class;							//클래스
	long Level;							//레벨
	WCHAR CharName[32];					//캐릭터 명
	WCHAR Rank;							//랭크
	long ServerIndex;					//서버인덱스
};

class MSG_CLIENT_BUYPRODUCT_HX_KR : public MSG_GAMECODE_BASE<MSG_CLIENT_BUYPRODUCT_HX_KR>	//(C->S)상품 구매 (헉슬리 국내)
{
public:
	MSG_CLIENT_BUYPRODUCT_HX_KR() : MSG_GAMECODE_BASE<MSG_CLIENT_BUYPRODUCT_HX_KR>(PROTOCOL_CLIENT_BUYPRODUCT)
	{GameCode = 115;}

public:
	char AccoundID[20];					//게임 계정
	DWORD AccountSeq;					//계정순번	
	long PackageSeq;					//포장 상품 순번
	long PriceSeq;						//가격 순번	
	long SalesZone;						//판매영역
	long DisplaySeq;					//전시정보 순번
	long Class;							//클래스
	long Level;							//레벨
	WCHAR CharName[32];					//캐릭터 명
	WCHAR Rank;							//랭크
	long ServerIndex;					//서버인덱스
};

class MSG_SERVER_BUYPRODUCT_MU : public MSG_GAMECODE_BASE<MSG_SERVER_BUYPRODUCT_MU>	//(S->C) 상품 구매 (뮤)
{
public:
	MSG_SERVER_BUYPRODUCT_MU() : MSG_GAMECODE_BASE<MSG_SERVER_BUYPRODUCT_MU>(PROTOCOL_SERVER_BUYPRODUCT)
		{LeftCount = 0;GameCode = 112;}

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

class MSG_SERVER_BUYPRODUCT_HX_KR : public MSG_GAMECODE_BASE<MSG_SERVER_BUYPRODUCT_HX_KR>	//(S->C) 상품 구매 (헉슬리 국내)
{
public:
	MSG_SERVER_BUYPRODUCT_HX_KR() : MSG_GAMECODE_BASE<MSG_SERVER_BUYPRODUCT_HX_KR>(PROTOCOL_SERVER_BUYPRODUCT)
		{LeftCount = 0;GameCode = 115;}

public:
	char AccoundID[20];					//게임 계정
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

class MSG_CLIENT_GIFTPRODUCT_MU : public MSG_GAMECODE_BASE<MSG_CLIENT_GIFTPRODUCT_MU>	//(C->S)상품 선물 (뮤)
{
public:
	MSG_CLIENT_GIFTPRODUCT_MU() : MSG_GAMECODE_BASE<MSG_CLIENT_GIFTPRODUCT_MU>(PROTOCOL_CLIENT_GIFTPRODUCT)
		{GameCode = 112;}

public:
	DWORD SenderSeq;					//발신자 순번
	long SenderServerIndex;			//발신자 서버 번호
	WCHAR SenderCharName[30];		//발신자 캐릭터 명
	DWORD ReceiverSeq;				//수신자 순번
	long ReceiverServerIndex;		//수신자 서버 번호
	WCHAR ReceiverCharName[30];		//수신자 캐릭터 명
	WCHAR Message[200];				//메시지	
	long PackageSeq;				//포장 상품 순번
	long PriceSeq;					//가격 순번	
	long SalesZone;					//판매영역
	long DisplaySeq;				//전시정보 순번
};

class MSG_CLIENT_GIFTPRODUCT_HX_KR : public MSG_GAMECODE_BASE<MSG_CLIENT_GIFTPRODUCT_HX_KR>	//(C->S)상품 선물 (헉슬리 국내)
{
public:
	MSG_CLIENT_GIFTPRODUCT_HX_KR() : MSG_GAMECODE_BASE<MSG_CLIENT_GIFTPRODUCT_HX_KR>(PROTOCOL_CLIENT_GIFTPRODUCT)
		{GameCode = 115;}

public:
	char AccoundID[20];					//게임 계정
	DWORD SenderSeq;					//발신자 순번
	long SenderServerIndex;			//발신자 서버 번호
	WCHAR SenderCharName[30];		//발신자 캐릭터 명
	DWORD ReceiverSeq;				//수신자 순번
	long ReceiverServerIndex;		//수신자 서버 번호
	WCHAR ReceiverCharName[30];		//수신자 캐릭터 명
	WCHAR Message[200];				//메시지	
	long PackageSeq;				//포장 상품 순번
	long PriceSeq;					//가격 순번	
	long SalesZone;					//판매영역
	long DisplaySeq;				//전시정보 순번
};

class MSG_SERVER_GIFTPRODUCT_MU : public MSG_GAMECODE_BASE<MSG_SERVER_GIFTPRODUCT_MU>	//(S->C) 상품 선물 (뮤)
{
public:
	MSG_SERVER_GIFTPRODUCT_MU() : MSG_GAMECODE_BASE<MSG_SERVER_GIFTPRODUCT_MU>(PROTOCOL_SERVER_GIFTPRODUCT),
		LimitedCash(0)
		{GameCode = 112;}

public:
	DWORD SenderSeq;					//계정순번
	DWORD ReceiverSeq;					//계정순번
	long ResultCode;					//결과 코드
	double LimitedCash;					//선물 가능 캐쉬
	long LeftProductCount;
};

class MSG_SERVER_GIFTPRODUCT_HX_KR : public MSG_GAMECODE_BASE<MSG_SERVER_GIFTPRODUCT_HX_KR>	//(S->C) 상품 선물 (헉슬리 국내)
{
public:
	MSG_SERVER_GIFTPRODUCT_HX_KR() : MSG_GAMECODE_BASE<MSG_SERVER_GIFTPRODUCT_HX_KR>(PROTOCOL_SERVER_GIFTPRODUCT),
		LimitedCash(0)
		{GameCode = 115;}

public:
	char AccoundID[20];					//게임 계정
	DWORD SenderSeq;					//계정순번
	DWORD ReceiverSeq;					//계정순번
	long ResultCode;					//결과 코드
	double LimitedCash;					//선물 가능 캐쉬
	long LeftProductCount;
};

class MSG_CLIENT_STORAGELIST : public MSG_BASE<MSG_CLIENT_STORAGELIST>	//(C->S) 보관함 조회
{
public:
	MSG_CLIENT_STORAGELIST() : MSG_BASE<MSG_CLIENT_STORAGELIST>(PROTOCOL_CLIENT_STORAGELIST),
		AccountID(0), GameCode(0), SalesZone(0)
		{}

public:
	DWORD AccountID;						//계정 순번
	long GameCode;						//게임토드
	int SalesZone;						//판매영역
	
};

class MSG_SERVER_STORAGELIST : public MSG_BASE<MSG_SERVER_STORAGELIST>	//(S->C) 보관함 조회
{
public:
	MSG_SERVER_STORAGELIST() : MSG_BASE<MSG_SERVER_STORAGELIST>(PROTOCOL_SERVER_STORAGELIST),
		AccountID(0), ListCount(0)
	{PackHeader = dwPacketSize;}	

public:
	long PackHeader;
	DWORD AccountID;						//계정 순번
	long ListCount;						//보관함 목록 개수
	long ResultCode;					//계정 순번
};

class MSG_CLIENT_GIFTCASH : public MSG_BASE<MSG_CLIENT_GIFTCASH>	//(C->S) 캐쉬선물
{
public:
	MSG_CLIENT_GIFTCASH() : MSG_BASE<MSG_CLIENT_GIFTCASH>(PROTOCOL_CLIENT_GIFTCASH),
		SenderAccountID(0), ReceiverAccountID(0), CashValue(0), GameCode(0)
		{}

public:
	DWORD SenderAccountID;				//보내는 계정 순번
	WCHAR SendCharName[30];				//발신자 캐릭터 명
	long SendServerIndex;				//발신자 서버 번호
	DWORD ReceiverAccountID;				//받는 계정 순번
	WCHAR ReceiverCharName[30];				//수신자 캐릭터 명
	long ReceiverServerIndex;				//수신자 서버 번호
	WCHAR Message[200];					//선물 메시지
	double CashValue;					//선물 캐쉬 값
	long GameCode;						//게임토드	
};

class MSG_SERVER_GIFTCASH : public MSG_BASE<MSG_SERVER_GIFTCASH>	//(S->C) 캐쉬선물
{
public:
	MSG_SERVER_GIFTCASH() : MSG_BASE<MSG_SERVER_GIFTCASH>(PROTOCOL_SERVER_GIFTCASH),
		SenderSeq(0), ReceiverSeq(0), ResultCode(0), GiftSendLimit(0)
		{}

public:
	DWORD SenderSeq;					//계정순번
	DWORD ReceiverSeq;					//계정순번
	long ResultCode;					//계정 순번
	double GiftSendLimit;					//보관함 목록 개수	
};

class MSG_CLIENT_PRODUCTBUYGIFTCHECK : public MSG_BASE<MSG_CLIENT_PRODUCTBUYGIFTCHECK>	//(C->S) 상품 구매/선물 가능여부 조회
{
public:
	MSG_CLIENT_PRODUCTBUYGIFTCHECK() : MSG_BASE<MSG_CLIENT_PRODUCTBUYGIFTCHECK>(PROTOCOL_CLIENT_PRODUCTBUYGIFTCHECK)
		{}

public:
	DWORD AccountID;					//계정 순번	
	long GameCode;					//게임토드	
};

class MSG_SERVER_PRODUCTBUYGIFTCHECK : public MSG_BASE<MSG_SERVER_PRODUCTBUYGIFTCHECK>	//(S->C) 상품 구매/선물 가능여부 조회
{
public:
	MSG_SERVER_PRODUCTBUYGIFTCHECK() : MSG_BASE<MSG_SERVER_PRODUCTBUYGIFTCHECK>(PROTOCOL_SERVER_PRODUCTBUYGIFTCHECK),
		ItemBuy(0), Present(0), MyPresentCash(0), MyPresentSendLimit(0)
		{}

public:
	DWORD AccountID;						//계정순번
	long ResultCode;					//상품구매, 상품/캐시 선물 가능여부 결과 (0: 불가, 1: 가능, -1: DB 처리 실패)
	long ItemBuy;						//상품 구매 가능 여부 결과 (0: 불가, 1: 가능)
	long Present;						//상품/캐시 선물 가능 여부 결과 (1: 가능, 2:선물하기 불가, 3: 선물한도 초과)
	double MyPresentCash;				//한달 동안 내가 선물한 캐시 합계
	double MyPresentSendLimit;			//한달 동안 선물하기 캐시 한도

};

class MSG_CLIENT_STORAGEUSE : public MSG_BASE<MSG_CLIENT_STORAGEUSE>	//(C->S) 보관함 아이템 사용하기
{
public:
	MSG_CLIENT_STORAGEUSE() : MSG_BASE<MSG_CLIENT_STORAGEUSE>(PROTOCOL_CLIENT_STORAGEUSE)
		{}

public:	
	DWORD AccountSeq;					//계정 순번
	long GameCode;						//게임코드
	DWORD IPAddress;					//접근 IP
	long StorageSeq;					//보관함 순번
	long StorageItemSeq;				//보관함 Or 상품 순번
	char StorageItemType;				//캐쉬 Or 상품 구분 (C: 캐시, P: 상품)
	long Class;							//클래스
	long Level;							//레벨
	WCHAR CharName[32];					//캐릭터 명
	WCHAR Rank;							//랭크
	long ServerIndex;					//서버인덱스	
};

class MSG_SERVER_STORAGEUSE : public MSG_BASE<MSG_SERVER_STORAGEUSE>	//(S->C) 보관함 아이템 사용하기
{
public:
	MSG_SERVER_STORAGEUSE() : MSG_BASE<MSG_SERVER_STORAGEUSE>(PROTOCOL_SERVER_STORAGEUSE)	
		{PackHeader = dwPacketSize;ProductSeq = 0;}

public:
	DWORD AccountSeq;					//계정 순번
	long ResultCode;					/*처리결과 
										0:성공
										1:보관함 사용하기 항목이 없음
										2:PC방에서만 받기 가능한 아이템
										3:기간 내 사용중인 컬러 요금이 있는 경우
										4:기간 내 사용중인 개인 정액 상품이 있는경우
										-1:오류발생
										-2:DB 접근 오류*/

	long PackHeader;
	long ProductSeq;
	BYTE PropertyCount;
};

class MSG_CLIENT_STORAGETHROW : public MSG_BASE<MSG_CLIENT_STORAGETHROW>	//(C->S) 보관함 아이템 버리기
{
public:
	MSG_CLIENT_STORAGETHROW() : MSG_BASE<MSG_CLIENT_STORAGETHROW>(PROTOCOL_CLIENT_STORAGETHROW)
		{}

public:
	DWORD AccountSeq;					//계정 순번
	long GameCode;						//게임코드
	long StorageSeq;					//보관함 순번
	long StorageItemSeq;				//보관함 Or 상품 순번
	char StorageItemType;				//캐쉬 Or 상품 구분 (C: 캐시, P: 상품)
};

class MSG_SERVER_STORAGETHROW : public MSG_BASE<MSG_SERVER_STORAGETHROW>	//(S->C) 보관함 아이템 버리기
{
public:
	MSG_SERVER_STORAGETHROW() : MSG_BASE<MSG_SERVER_STORAGETHROW>(PROTOCOL_SERVER_STORAGETHROW)
		{}

public:
	DWORD AccountSeq;					//계정 순번
	long ResultCode;					/*처리결과 
										0:성공
										1:보관함 버리기 항목이 없음										
										-1:오류발생
										-2:DB 접근 오류*/
};

class MSG_CLIENT_PRODUCTEVENTLIST : public MSG_BASE<MSG_CLIENT_PRODUCTEVENTLIST>	//(C->S) 이벤트 상품 목록 조회
{
public:
	MSG_CLIENT_PRODUCTEVENTLIST() : MSG_BASE<MSG_CLIENT_PRODUCTEVENTLIST>(PROTOCOL_CLIENT_PRODUCTEVENTLIST)
	{}

public:	
	DWORD AccountSeq;					//계정 순번
	long SalesZone;						//판매 영역
	long ProductDisplaySeq;				//전시 상품 카테고리 순번
};

class MSG_SERVER_PRODUCTEVENTLIST : public MSG_BASE<MSG_SERVER_PRODUCTEVENTLIST>	//(S->C) 이벤트 상품 목록 조회
{
public:
	MSG_SERVER_PRODUCTEVENTLIST() : MSG_BASE<MSG_SERVER_PRODUCTEVENTLIST>(PROTOCOL_SERVER_PRODUCTEVENTLIST),
		PackagesCount(0)
		{PackHeader = dwPacketSize;}
		
public:
	long PackHeader;
	DWORD AccountSeq;					//계정 순번
	long ProductDisplaySeq;				//전시 상품 카테고리 순번
	long PackagesCount;		
};

class MSG_CLIENT_PACKAGELEFTCOUNT : public MSG_BASE<MSG_CLIENT_PACKAGELEFTCOUNT>	//(C->S) 전시 상품 잔여 개수 조회
{
public:
	MSG_CLIENT_PACKAGELEFTCOUNT() : MSG_BASE<MSG_CLIENT_PACKAGELEFTCOUNT>(PROTOCOL_CLIENT_PACKAGELEFTCOUNT)
	{}

public:	
	long PackageProductSeq;
};

class MSG_SERVER_PACKAGELEFTCOUNT : public MSG_BASE<MSG_SERVER_PACKAGELEFTCOUNT>	//(S->C) 전시 상품 잔여 개수 조회
{
public:
	MSG_SERVER_PACKAGELEFTCOUNT() : MSG_BASE<MSG_SERVER_PACKAGELEFTCOUNT>(PROTOCOL_CLIENT_UPDATEVERSION)
		{}
		
public:
	long PackageProductSeq;
	long LeftCount;
};

class MSG_CLIENT_UPDATEVERSION : public MSG_BASE<MSG_CLIENT_UPDATEVERSION>	//(C->S) 버전 업데이트
{
public:
	MSG_CLIENT_UPDATEVERSION() : MSG_BASE<MSG_CLIENT_UPDATEVERSION>(PROTOCOL_CLIENT_UPDATEVERSION)
	{}

public:	
	unsigned short SaleZone;
	unsigned short year;
	unsigned short yearIdentity;
};

class MSG_SERVER_UPDATEVERSION : public MSG_BASE<MSG_SERVER_UPDATEVERSION>	//(S->C) 버전 업데이트
{
public:
	MSG_SERVER_UPDATEVERSION() : MSG_BASE<MSG_SERVER_UPDATEVERSION>(PROTOCOL_SERVER_UPDATEVERSION)
		{}
		
public:
	unsigned short SaleZone;
	unsigned short year;
	unsigned short yearIdentity;
};


class MSG_CLIENT_MILEAGESAVE : public MSG_BASE<MSG_CLIENT_MILEAGESAVE>	//(C->S) 마일리지 적립
{
public:
	MSG_CLIENT_MILEAGESAVE() : MSG_BASE<MSG_CLIENT_MILEAGESAVE>(PROTOCOL_CLIENT_MILEAGESAVE)
	{}

public:		
	DWORD AccountSeq;		//개인 회원 계정 순번
	long GameCode;			//게임 코드
	long MileageType;		//마일리지적립유형
	long MileageSection;	//마일리지유형
	long MileagePoint;		//마일리지 적립 포인트

};

class MSG_SERVER_MILEAGESAVE : public MSG_BASE<MSG_SERVER_MILEAGESAVE>	//(S->C) 마일리지 적립
{
public:
	MSG_SERVER_MILEAGESAVE() : MSG_BASE<MSG_SERVER_MILEAGESAVE>(PROTOCOL_SERVER_MILEAGESAVE)
		{}
		
public:
	DWORD AccountSeq;		//개인 회원 계정 순번
	long ResultCode;		//조회 결과 코드 	0 : 성공, -1 :  오류발생

};

class MSG_CLIENT_MILEAGEDEDUCT : public MSG_BASE<MSG_CLIENT_MILEAGEDEDUCT>	//(C->S) 마일리지 소진
{
public:
	MSG_CLIENT_MILEAGEDEDUCT() : MSG_BASE<MSG_CLIENT_MILEAGEDEDUCT>(PROTOCOL_CLIENT_MILEAGEDEDUCT)
	{}

public:		
	DWORD AccountSeq;//	개인 회원 계정 순번
	long GameCode;//	게임 코드
	long DeductCategory;//	마일리지소진유형
	long MileageSection;//	마일리지유형
	long MileageDeductPoint;//	마일리지 소진 포인트
	long Class;//	클래스
	long Level;//	레벨
	WCHAR CharName[32];//	캐릭터명
	WCHAR Rank;//	계급
	long ServerIndex;//	서버 번호
};

class MSG_SERVER_MILEAGEDEDUCT : public MSG_BASE<MSG_SERVER_MILEAGEDEDUCT>	//(S->C) 마일리지 소진
{
public:
	MSG_SERVER_MILEAGEDEDUCT() : MSG_BASE<MSG_SERVER_MILEAGEDEDUCT>(PROTOCOL_SERVER_MILEAGEDEDUCT)
		{}
		
public:
	DWORD AccountSeq;		//개인 회원 계정 순번
	long ResultCode;		//조회 결과 코드 	0 : 성공, -1 :  오류발생
};

class MSG_CLIENT_MILEAGELIVESAVEUP : public MSG_BASE<MSG_CLIENT_MILEAGELIVESAVEUP>	//(C->S) 실시간 마일리지 적립
{
public:
	MSG_CLIENT_MILEAGELIVESAVEUP() : MSG_BASE<MSG_CLIENT_MILEAGELIVESAVEUP>(PROTOCOL_CLIENT_MILEAGELIVESAVEUP)
	{}

public:		
	DWORD AccountSeq;//	개인 회원 계정 순번
	long GameCode;//	게임 코드	
	long MileageSection;//	마일리지유형
	long SourceType;//개인, PC방 구분
};

class MSG_SERVER_MILEAGELIVESAVEUP : public MSG_BASE<MSG_SERVER_MILEAGELIVESAVEUP>	//(S->C) 실시간 마일리지 적립
{
public:
	MSG_SERVER_MILEAGELIVESAVEUP() : MSG_BASE<MSG_SERVER_MILEAGELIVESAVEUP>(PROTOCOL_SERVER_MILEAGELIVESAVEUP)
		{}
		
public:
	DWORD AccountSeq;		//개인 회원 계정 순번
	long ResultCode;		//조회 결과 코드 	0 : 성공, -1 :  오류발생
};

class MSG_CLIENT_ITEMSERIALUPDATE : public MSG_BASE<MSG_CLIENT_ITEMSERIALUPDATE>	//(C->S) 아이템 시리얼 업데이트
{
public:
	MSG_CLIENT_ITEMSERIALUPDATE() : MSG_BASE<MSG_CLIENT_ITEMSERIALUPDATE>(PROTOCOL_CLIENT_ITEMSERIALUPDATE)
	{}

public:	
	DWORD AccountSeq;		//회원 계정 순번
	long GameCode;			//게임 코드
	long StorageSeq;		//보관함 순번
	long StorageItemSeq;	//보관함 캐시 or 상품 순번
	INT64 InGameUseCode;	//아이템 시리얼 코드	
};

class MSG_SERVER_ITEMSERIALUPDATE : public MSG_BASE<MSG_SERVER_ITEMSERIALUPDATE>	//(S->C) 아이템 시리얼 업데이트
{
public:
	MSG_SERVER_ITEMSERIALUPDATE() : MSG_BASE<MSG_SERVER_ITEMSERIALUPDATE>(PROTOCOL_SERVER_ITEMSERIALUPDATE)
		{}
		
public:
	DWORD AccountSeq;		//개인 회원 계정 순번
	long ResultCode;		//조회 결과 코드 	0 : 성공, 1: 대상 항목 없음, -1 :  오류발생
};

}


#pragma pack()