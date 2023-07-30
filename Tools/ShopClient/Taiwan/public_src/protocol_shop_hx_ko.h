#pragma once
#pragma pack(1)

/**************************************************************************************************

작성일: 2008-07-18
작성자: 문상현 (youngmoon@webzen.co.kr)

설명: 국내 뮤 빌링서버 프로토콜 정의

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

/*
struct STProductList_PACKAGE	//상품 목록 리스트 상품목록
{	
	long ProductSeq;			//전시 상품 카테고리 순번
	long ViewOrder;				//노출 순서
	long Seq;					//전시 상품 순번
	WCHAR Name[255];			//전시 상품 명
	long Type;					//전시 상품 유형 (170: 일반 상품, 171: 이벤트 상품)
	long Price;					//가격
	WCHAR Description[2000];	//상세설명
	WCHAR Caution[1000];		//주의사항
	long SalesFlag;				//구매가능 여부(182: 가능, 183: 불가능)
	long GiftFlag;				//선물가능 여부(184: 가능, 185: 불가능)
	long LeftCount;				//잔여 수량 (-1 이면 제한없음)
	DWORD StartDate;			//판매 시작일
	DWORD EndDate;				//판매 종료일
	long CapsuleFlag;			//캡슐 상품 구분 (176:캡슐, 177:일반)
	long CapsuleCount;			//전시 상품에 포함된 단위 상품 개수
	long Deleteflag;			//삭제 플러그 (144: 활성, 143: 삭제)
	WCHAR CashName[100];		//소진 캐시 명
	char PriceUnitName[20];		//가격 단위 표시 명	
	char ProductSeqInfo[100];	//전시상품에 포함된 단위 상품 순번

};

struct STProductList_CATEGORY	//상품 목록 리스트 카테고리
{
	long ProductSeq;			//전시 상품 카테고리 순번
	WCHAR Name[50];				//카테고리 명
	long EventFlag;				//이벤트 카테고리 여부
	long StorageGroup;			//보관함 그룹 코드
	long OpenFlag;				//공개 여부 (201: 공개, 202:비공개)
	long ParentSeq;				//부모 전시 상품 카테고리 순번
	long DisplayOrder;			//노출순서
	long Root;					//최상위 구분 (1: 최상위, 0:서브)	
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
	BYTE ViewType;						//조회 유형 (0:전체(캐쉬+포인트), 1:캐쉬(일반+이벤트), 2:포인트(보너스+게임), 3:보너스포인트, 4:게임포인트, 5:캐쉬(일반), 6:캐시(일반))
	long AccountSeq;					//계정순번
	long GameCode;						//게임 코드
	bool SumOnly;						//총 합계만 조회 여부 (true: 유형별 종합 값만, false: 유형별 종합 + 캐시 유형별 결과(STDetail 값 출력))
};

class MSG_SERVER_INQURE_CASH : public MSG_BASE<MSG_SERVER_INQURE_CASH>	//(S->C) 사용자 캐쉬 조회
{
public:
	MSG_SERVER_INQURE_CASH() : MSG_BASE<MSG_SERVER_INQURE_CASH>(PROTOCOL_SERVER_INQURE_CASH) 	  
	{
		sum[0] = 0;
		sum[1] = 0;
	}

public:
	long ResultCode;					//결과 코드
	long AccountSeq;					//계정순번
	double sum[2];						//유형별 종합 정보 (0: 캐쉬 총합, 1: 포인트 총합)
	STDetail stdetail[3];				//캐쉬 유형별 세부정보	
};	

class MSG_CLIENT_BUYPRODUCT : public MSG_BASE<MSG_CLIENT_BUYPRODUCT>	//(C->S)상품 구매
{
public:
	MSG_CLIENT_BUYPRODUCT() : MSG_BASE<MSG_CLIENT_BUYPRODUCT>(PROTOCOL_CLIENT_BUYPRODUCT)
	{}

public:
	long AccountSeq;					//계정순번
	long GameCode;						//게임 코드
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
	long SenderSeq;					//발신자 순번
	long ReceiverSeq;					//수신자 순번
	WCHAR Message[200];					//메시지
	long GameCode;						//게임코드
	long PackageSeq;					//포장 상품 순번
	long PriceSeq;						//가격 순번	
	long SalesZone;					//판매영역
	long DisplaySeq;					//전시정보 순번
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

class MSG_CLIENT_STORAGELIST : public MSG_BASE<MSG_CLIENT_STORAGELIST>	//(C->S) 보관함 조회
{
public:
	MSG_CLIENT_STORAGELIST() : MSG_BASE<MSG_CLIENT_STORAGELIST>(PROTOCOL_CLIENT_STORAGELIST),
		AccountID(0), GameCode(0), SalesZone(0)
		{}

public:
	long AccountID;						//계정 순번
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
	long AccountID;						//계정 순번
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
	long SenderAccountID;				//보내는 계정 순번
	long ReceiverAccountID;				//받는 계정 순번
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
	long SenderSeq;					//계정순번
	long ReceiverSeq;					//계정순번
	long ResultCode;					//계정 순번
	double GiftSendLimit;					//보관함 목록 개수	
};

class MSG_CLIENT_PRODUCTBUYGIFTCHECK : public MSG_BASE<MSG_CLIENT_PRODUCTBUYGIFTCHECK>	//(C->S) 상품 구매/선물 가능여부 조회
{
public:
	MSG_CLIENT_PRODUCTBUYGIFTCHECK() : MSG_BASE<MSG_CLIENT_PRODUCTBUYGIFTCHECK>(PROTOCOL_CLIENT_PRODUCTBUYGIFTCHECK)
		{}

public:
	long AccountID;					//계정 순번	
	long GameCode;					//게임토드	
};

class MSG_SERVER_PRODUCTBUYGIFTCHECK : public MSG_BASE<MSG_SERVER_PRODUCTBUYGIFTCHECK>	//(S->C) 상품 구매/선물 가능여부 조회
{
public:
	MSG_SERVER_PRODUCTBUYGIFTCHECK() : MSG_BASE<MSG_SERVER_PRODUCTBUYGIFTCHECK>(PROTOCOL_SERVER_PRODUCTBUYGIFTCHECK),
		ItemBuy(0), Present(0), MyPresentCash(0), MyPresentSendLimit(0)
		{}

public:
	long AccountID;						//계정순번
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
	long AccountSeq;					//계정 순번
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
	long AccountSeq;					//계정 순번
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
	long AccountSeq;					//계정 순번
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
	long AccountSeq;					//계정 순번
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
	long AccountSeq;					//계정 순번
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
	long AccountSeq;					//계정 순번
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

/*
class MSG_CLIENT_PRODUCTLIST : public MSG_BASE<MSG_CLIENT_PRODUCTLIST>	//(C->S) 상품 목록 조회
{
public:
	MSG_CLIENT_PRODUCTLIST() : MSG_BASE<MSG_CLIENT_PRODUCTLIST>(PROTOCOL_CLIENT_PRODUCTLIST)
		{}

public:
	long SalesZone;						//판매 영역
};

class MSG_SERVER_PRODUCTLIST : public MSG_BASE<MSG_SERVER_PRODUCTLIST>	//(S->C) 상품 목록 조회
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

class MSG_CLIENT_PRODUCTEVENTLIST : public MSG_BASE<MSG_CLIENT_PRODUCTEVENTLIST>	//(C->S) 이벤트 상품 목록 조회
{
public:
	MSG_CLIENT_PRODUCTEVENTLIST() : MSG_BASE<MSG_CLIENT_PRODUCTEVENTLIST>(PROTOCOL_CLIENT_PRODUCTEVENTLIST)
	{}

public:	
	long AccountSeq;					//계정 순번
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
	long AccountSeq;					//계정 순번
	long ProductDisplaySeq;				//전시 상품 카테고리 순번
	int PackagesCount;	
};


class MSG_CLIENT_UPDATELIST : public MSG_BASE<MSG_CLIENT_UPDATELIST>	//(C->S) 상품 정보를 업데이트 한다.
{
public:
	MSG_CLIENT_UPDATELIST() : MSG_BASE<MSG_CLIENT_UPDATELIST>(PROTOCOL_CLIENT_UPDATELIST)
	{}

public:	
	long UpdateType;					//업데이트 타입
	long ProcessType;					//프로세스 타입(440:등록, 441:수정, 442:삭제)
	long TypeValue;						//업데이트 타입에 의한 값
};

class MSG_SERVER_UPDATELIST : public MSG_BASE<MSG_SERVER_UPDATELIST>	//(S->C) 상품 정보를 업데이트 한다.
{
public:
	MSG_SERVER_UPDATELIST() : MSG_BASE<MSG_SERVER_UPDATELIST>(PROTOCOL_SERVER_UPDATELIST)
		{}
		
public:	
};

class MSG_SERVER_UPDATECATEGORYLIST : public MSG_BASE<MSG_SERVER_UPDATECATEGORYLIST>	//(S->C) 전시 상품 정보를 업데이트 한다.
{
public:
	MSG_SERVER_UPDATECATEGORYLIST() : MSG_BASE<MSG_SERVER_UPDATECATEGORYLIST>(PROTOCOL_SERVER_UPDATECATEGORYLIST)
		{PackHeader = dwPacketSize;}
		
public:	
	long PackHeader;
	int CategorysCount;
	int PackagesCount;	
	long ProcessType;					//프로세스 타입(440:등록, 441:수정, 442:삭제)
};

class MSG_SERVER_UPDATEPRODUCTLIST : public MSG_BASE<MSG_SERVER_UPDATEPRODUCTLIST>	//(S->C) 개별 상품 정보를 업데이트 한다.
{
public:
	MSG_SERVER_UPDATEPRODUCTLIST() : MSG_BASE<MSG_SERVER_UPDATEPRODUCTLIST>(PROTOCOL_SERVER_UPDATEPRODUCTLIST)
		{PackHeader = dwPacketSize;}
		
public:	
	long PackHeader;
	long ProcessType;					//프로세스 타입(440:등록, 441:수정, 442:삭제)
	int ProductCount;
};
*/


}


#pragma pack()