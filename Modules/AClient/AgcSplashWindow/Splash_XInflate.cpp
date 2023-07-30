#define _WINSOCKAPI_
#include "Splash_XInflate.h"

/**********************************************************************************

                                    -= history =-

* 2010/4/5 코딩 시작
* 2010/4/9 1차 완성
	- 속도 비교 (34.5MB 짜리 gzip 파일, Q9400 CPU)
		. zlib : 640ms
		. xinflate : 1900ms

* 2010/4/29 
	- 속도 향상 최적화 
		. 일부 함수 호출을 매크로화 시키기 : 1900ms
		. 비트 스트림을 로컬 변수화 시키기: 1800ms
		. Write 함수 매크로화: 1750ms
		. Window 멤버 변수화: 1680ms
		. Window 매크로화 + 로컬 변수화 : 1620ms
		. InputBuffer 로컬 변수화 : 1610ms
		. 출력 윈도우 로컬 변수화 : 1500ms;
		. while() 루프에서 m_bCompleted 비교 제거 : 1470ms;
		. while() 루프내에서 m_error 비교 제거 : 1450ms
		. m_state 로컬 변수화 : 1450ms
		. m_windowLen, m_windowDistCode 로컬 변수화 : 1390ms

	- 나름 최적화후 속도 차이 비교 (34.5MB 짜리 gzip 파일, Q9400 CPU)
		. zlib : 640MS
		. Halibut : 1750ms
		. XInflate : 1370ms

* 2010/4/30
	- 코드 정리
	- v1.0 최초 공개

* 2010/5/11
	- static table 이 필요할때만 생성하도록 수정
	- 최적화
		. 최적화 전 : 1340ms
		. CHECK_TABLE 제거 : 1330ms
		. FILLBUFFER() 개선: 1320ms
		. table을 링크드 리스트에서 배열로 수정 : 1250ms

* 2010/5/12
	- 최적화 계속
		. STATE_GET_LEN 의 break 제거 : 1220ms
		. STATE_GET_DISTCODE 의 break 제거 : 1200ms
		. STATE_GET_DIST 의 break 제거 : 1170ms

* 2010/5/13
	- 최적화 계속
		. FastInflate() 로 분리후 FILLBUFFER_FAST, HUFFLOOKUP_FAST 적용 : 1200ms
		. 허프만 트리 처리 방식 개선(단일 테이블 참조로 트리 탐색은 없애고 메모리 사용량도 줄이고) : 900ms
		. HUFFLOOKUP_FAST 다시 적용 : 890ms
		. WRITE_FAST 적용 : 810ms
		. lz77 윈도우 출력시 while() 을 do-while 로 변경 : 800ms

* 2010/5/19
	- 출력 버퍼를 내부 alloc 대신 외부 버퍼를 이용할 수 있도록 기능 추가
	- m_error 변수 제거

* 2010/5/25
	- 외부버퍼 출력 기능쪽 버그 수정
	- direct copy 쪽 약간 개선

***********************************************************************************/



// DEFLATE 의 압축 타입
static enum BTYPE
{
	BTYPE_NOCOMP			= 0,
	BTYPE_FIXEDHUFFMAN		= 1,
	BTYPE_DYNAMICHUFFMAN	= 2,
	BTYPE_RESERVED			= 3		// ERROR
};

// len, dist 테이블 - halibut 의 deflate.c 에서 가져옴
struct coderecord
{
    short code, extrabits;
    int min, max;
} ;

static const coderecord lencodes[] = 
{
    {257, 0, 3, 3},		{258, 0, 4, 4},		{259, 0, 5, 5},		{260, 0, 6, 6},		{261, 0, 7, 7},		{262, 0, 8, 8},		{263, 0, 9, 9},		{264, 0, 10, 10},	{265, 1, 11, 12},	{266, 1, 13, 14},	{267, 1, 15, 16},	{268, 1, 17, 18},
    {269, 2, 19, 22},	{270, 2, 23, 26},	{271, 2, 27, 30},	{272, 2, 31, 34},	{273, 3, 35, 42},	{274, 3, 43, 50},	{275, 3, 51, 58},	{276, 3, 59, 66},	{277, 4, 67, 82},	{278, 4, 83, 98},	{279, 4, 99, 114},	{280, 4, 115, 130},
    {281, 5, 131, 162},	{282, 5, 163, 194},	{283, 5, 195, 226},	{284, 5, 227, 257},	{285, 0, 258, 258},	
};

static const coderecord distcodes[] = 
{
    {0, 0, 1, 1},			{1, 0, 2, 2},			{2, 0, 3, 3},			{3, 0, 4, 4},			{4, 1, 5, 6},			{5, 1, 7, 8},			{6, 2, 9, 12},			{7, 2, 13, 16},			{8, 3, 17, 24},			{9, 3, 25, 32},			{10, 4, 33, 48},		{11, 4, 49, 64},		{12, 5, 65, 96},		{13, 5, 97, 128},		{14, 6, 129, 192},		{15, 6, 193, 256},
    {16, 7, 257, 384},		{17, 7, 385, 512},		{18, 8, 513, 768},		{19, 8, 769, 1024},		{20, 9, 1025, 1536},	{21, 9, 1537, 2048},	{22, 10, 2049, 3072},	{23, 10, 3073, 4096},	{24, 11, 4097, 6144},	{25, 11, 6145, 8192},	{26, 12, 8193, 12288},	{27, 12, 12289, 16384},	{28, 13, 16385, 24576},	{29, 13, 24577, 32768},
};

// code length 
static const unsigned char lenlenmap[] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};


// 유틸 매크로
#define HUFFTABLE_VALUE_NOT_EXIST	-1
#ifndef ASSERT
#	define ASSERT(x) {}
#endif
#define SAFE_DEL(x) if(x) {delete x; x=NULL;}
#define SAFE_FREE(x) if(x) {free(x); x=NULL;}



////////////////////////////////////////////////
//
// 비트 스트림 매크로화
//
#define BS_EATBIT()								\
	bits & 0x01;								\
	bitLen --;									\
	bits >>= 1;									\
	if(bitLen<0) {}

#define BS_EATBITS(count)						\
	(bits & ((1<<(count))-1));					\
	bitLen -= count;							\
	bits >>= count;								\
	if(bitLen<0) {}

#define BS_REMOVEBITS(count)					\
	bits >>= (count);							\
	bitLen -= (count);							\
	if(bitLen<0) {} 

#define BS_MOVETONEXTBYTE						\
	BS_REMOVEBITS((bitLen % 8));

#define BS_ADDBYTE(byte)						\
	bits |= (byte) << bitLen;					\
	bitLen += 8;
//
// 비트 스트림 매크로 처리
//
////////////////////////////////////////////////


////////////////////////////////////////////////
//
// 디버깅용 매크로
//
#ifdef _DEBUG
#	define ADD_INPUT_COUNT		m_inputCount ++
#	define ADD_OUTPUT_COUNT		m_outputCount ++
#else
#	define ADD_INPUT_COUNT	
#	define ADD_OUTPUT_COUNT
#endif


////////////////////////////////////////////////
//
// 반복 작업 매크로화
//
#define FILLBUFFER()							\
	while(bitLen<=24)							\
	{											\
		if(inBufferRemain==0)					\
			break;								\
		BS_ADDBYTE(*inBuffer);					\
		inBufferRemain--;						\
		inBuffer++;								\
		ADD_INPUT_COUNT;						\
	}											\
	if(bitLen<=0)								\
		goto END;


#define HUFFLOOKUP(result, pTable)								\
		/* 데이타 부족 */										\
		if(pTable->bitLenMin > bitLen)							\
		{														\
			result = -1;										\
		}														\
		else													\
		{														\
			pItem = &(pTable->pItem[pTable->mask & bits]);		\
			/* 데이타 부족 */									\
			if(pItem->bitLen > bitLen)							\
			{													\
				result = -1;									\
			}													\
			else												\
			{													\
				result = pItem->symbol;							\
				BS_REMOVEBITS(pItem->bitLen);					\
			}													\
		}


#define WRITE(byte)												\
	ADD_OUTPUT_COUNT;											\
	WIN_ADD(byte);												\
	CHECK_AND_INCREATE_OUT_BUFFER								\
	outBuffer[outBufferPos] = byte;								\
	outBufferPos++;												

#define CHECK_AND_INCREATE_OUT_BUFFER							\
	if(outBufferPos>=outBufferAlloc)							\
	{															\
		if(m_isExternalOutBuffer)/* 버퍼를 늘릴수가 없는 상황*/	\
			DOERR(XINFLATE_ERR_INSUFFICIENT_OUT_BUFFER);		\
																\
		/* 버퍼 늘리기 */										\
		outBufferAlloc = outBufferAlloc * 2;					\
		BYTE* temp = outBuffer;									\
		outBuffer = (BYTE*)realloc(outBuffer, outBufferAlloc);	\
		/* alloc 실패 처리 */									\
		if(outBuffer==NULL)										\
		{														\
			{}											\
			ret = XINFLATE_ERR_ALLOC_FAIL;						\
			outBuffer = temp;									\
			outBufferPos = 0; /* 그냥 처음 위치로 되돌린다.*/	\
			outBufferAlloc /= 2;								\
			goto END;											\
		}														\
	}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              FastInflate
//


// 입력 버퍼가 충분한지 체크하지 않는다.
#define FILLBUFFER_FAST()								\
	while(bitLen<=24)									\
	{													\
		BS_ADDBYTE(*inBuffer);							\
		inBufferRemain--;								\
		inBuffer++;										\
		ADD_INPUT_COUNT;								\
	}													\

// 비트스트림 데이타(bits + bitLen)가 충분한지 체크하지 않는다.
#define HUFFLOOKUP_FAST(result, pTable)					\
		pItem = &(pTable->pItem[pTable->mask & bits]);	\
		result = pItem->symbol;							\
		BS_REMOVEBITS(pItem->bitLen);					


// 출력 버퍼가 충분한지 체크하지 않는다.
#define WRITE_FAST(byte)										\
	ADD_OUTPUT_COUNT;											\
	WIN_ADD(byte);												\
	outBuffer[outBufferPos] = byte;								\
	outBufferPos++;												


////////////////////////////////////////////////
//
// 윈도우 매크로화
//

#define WIN_ADD(b)												\
	windowBuf[windowPos] = b;									\
	windowPos = (windowPos+1) & (DEFLATE_WINDOW_SIZE-1);

#define WIN_GETBUF(dist)										\
	(windowBuf +  ((windowPos - dist) & (DEFLATE_WINDOW_SIZE-1)) )

//
// 윈도우 매크로화
//
////////////////////////////////////////////////



/////////////////////////////////////////////////////
//
// 허프만 트리를 빨리 탐색할 수 있게 하기 위해서
// 트리 전체를 하나의 배열로 만들어 버린다.
//   
//

struct XFastHuffItem									// XFastHuffTable 의 아이템
{
#ifdef _DEBUG
	XFastHuffItem() 
	{
		bitLen = 0;
		symbol = HUFFTABLE_VALUE_NOT_EXIST;
	}
#endif
	int		bitLen;										// 유효한 비트수
	int		symbol;										// 유효한 심볼
};

class XFastHuffTable
{
public :
	XFastHuffTable()
	{
		pItem = NULL;
	}
	~XFastHuffTable()
	{
		if(pItem){ delete[] pItem; pItem=NULL;}
	}
	// 배열 생성
	void	Create(int _bitLenMin, int _bitLenMax)
	{
		if(pItem) {}							// 발생 불가

		bitLenMin = _bitLenMin;
		bitLenMax = _bitLenMax;
		mask	  = (1<<bitLenMax) - 1;					// 마스크
		itemCount = 1<<bitLenMax;						// 조합 가능한 최대 테이블 크기
		pItem     = new XFastHuffItem[itemCount];		// 2^maxBitLen 만큼 배열을 만든다.
	}
	// 심볼 데이타를 가지고 전체 배열을 채운다.
	void	SetValue(int symbol, int bitLen, UINT bitCode)
	{
		/*
			만일 허프만 트리 노드에서 0->1->1 이라는 데이타가 'A' 라면
			symbol = 'A',   bitLen = 3,   bitCode = 3  이 파라메터로 전달된다.

			* 실제 bitstream 은 뒤집어져 들어오기 때문에 나중에 참조를 빨리 하기 위해서
			  0->1->1 을 1<-1<-0 으로 뒤집는다.

			* 만일 bitLenMax 가 5 라면 뒤집어진 1<-1<-0 에 의 앞 2bit 로 조합 가능한
			  00110, 01110, 10110, 11110 에 대해서도 동일한 심볼을 참조할 수 있도록 만든다.
		*/

		UINT revBitCode = 0;
		// 뒤집기
		int i;
		for(i=0;i<bitLen;i++)
		{
			revBitCode <<= 1;
			revBitCode |= (bitCode & 0x01);
			bitCode >>= 1;
		}

		int		add2code = (1<<bitLen);		// bitLen 이 3 이라면 add2code 에는 1000(bin) 이 들어간다

		// 배열 채우기
		for(;;)
		{
#ifdef _DEBUG
			if(pItem[revBitCode].symbol!=  HUFFTABLE_VALUE_NOT_EXIST) 
				{}
#endif
			pItem[revBitCode].bitLen = bitLen;
			pItem[revBitCode].symbol = symbol;

			// 조합 가능한 bit code 를 처리하기 위해서 값을 계속 더한다.
			revBitCode += add2code;

			// 조합 가능한 범위가 벗어난 경우 끝낸다
			if(revBitCode >= itemCount)
				break;
		}
	}

	XFastHuffItem*	pItem;							// (huff) code 2 symbol 아이템, code가 배열의 위치 정보가 된다.
	int				bitLenMin;						// 유효한 최소 비트수
	int				bitLenMax;						// 유효한 최대 비트수
	UINT			mask;							// bitLenMax 에 대한 bit mask
	UINT			itemCount;						// bitLenMax 로 생성 가능한 최대 아이템 숫자
};



// 멤버 변수를 로컬로 복사하고 로컬 변수를 멤버 변수로 반환하기

#define	SAVE										\
	bits = m_bits;									\
	bitLen = m_bitLen;								\
	windowBuf = m_windowBuf;						\
	windowPos = m_windowPos;						\
	outBuffer = m_outBuffer;						\
	outBufferAlloc = m_outBufferAlloc;				\
	outBufferPos = m_outBufferPos;					\
	state = m_state;								\
	windowLen = m_windowLen;						\
	windowDistCode = m_windowDistCode;				\
	symbol = m_symbol;

#define RESTORE										\
	m_bits = bits;									\
	m_bitLen = bitLen;								\
	m_windowBuf = windowBuf;						\
	m_windowPos = windowPos;						\
	m_outBuffer = outBuffer;						\
	m_outBufferAlloc = outBufferAlloc;				\
	m_outBufferPos = outBufferPos;					\
	m_state = state;								\
	m_windowLen = windowLen;						\
	m_windowDistCode = windowDistCode;				\
	m_symbol = symbol;


const char* xinflate_copyright = 
"[XInflate - Copyright(C) 2010, by kippler]";


XInflate::XInflate()
{
	m_pStaticInfTable = NULL;
	m_pStaticDistTable = NULL;
	m_pDynamicInfTable = NULL;
	m_pDynamicDistTable = NULL;
	m_pCurrentTable = NULL;
	m_pCurrentDistTable = NULL;

	m_pLenLenTable = NULL;
	m_outBuffer = NULL;
	m_windowBuf = NULL;
	m_copyright = NULL;
	m_isExternalOutBuffer = FALSE;
}

XInflate::~XInflate()
{
	Free();
}

// 내부 메모리 alloc 해제
void XInflate::Free()
{
	SAFE_DEL(m_pStaticInfTable);
	SAFE_DEL(m_pStaticDistTable);
	SAFE_DEL(m_pDynamicInfTable);
	SAFE_DEL(m_pDynamicDistTable);
	SAFE_DEL(m_pLenLenTable);

	SAFE_FREE(m_windowBuf);


	if(m_isExternalOutBuffer==FALSE)	// 내부 버퍼
	{
		SAFE_FREE(m_outBuffer);
	}
	else								// 외부 버퍼
		m_outBuffer = NULL;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///         내부 변수 초기화
/// @param  
/// @return 
/// @date   Thursday, April 08, 2010  4:17:46 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
void XInflate::Init()
{
	SAFE_DEL(m_pLenLenTable);
	SAFE_DEL(m_pDynamicInfTable);
	SAFE_DEL(m_pDynamicDistTable);

	m_pCurrentTable = NULL;
	m_pCurrentDistTable = NULL;

	m_state = STATE_START;
	m_uncompRemain = 0;
	m_bFinalBlock = FALSE;
	m_symbol = 0;
	m_windowLen = 0;
	m_windowDistCode = 0;
	m_bCompleted = FALSE;

	BS_Init();
	Win_Init();

	m_copyright = xinflate_copyright;

#ifdef _DEBUG
	m_inputCount = 0;
	m_outputCount = 0;
#endif
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///         출력 버퍼를 내부에서 alloc 하지를 않고, 외부의 버퍼를 사용한다.
/// @param  
/// @return 
/// @date   Wednesday, May 19, 2010  12:44:50 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
void XInflate::SetOutBuffer(BYTE* outBuf, int outBufLen)
{
	// 이전에 사용하던 버퍼 있으면 버퍼 지우기.
	if(m_isExternalOutBuffer==FALSE)
	{
		SAFE_FREE(m_outBuffer);
	}

	if(outBuf)
	{
		// 외부 버퍼를 출력 버퍼로 지정하기
		m_outBuffer = outBuf;
		m_outBufferAlloc = outBufLen;
		m_outBufferPos = 0;
		m_isExternalOutBuffer = TRUE;
	}
	else
	{
		// outBuf==NULL 이면 외부 버퍼를 더이상 사용 안하는 경우이다.
		m_outBuffer = outBuf;
		m_outBufferAlloc = 0;
		m_outBufferPos = 0;
		m_isExternalOutBuffer = FALSE;
	}

}


////////////////////////////////////////////////////////////////////////////////////////////////////
///         압축 해제 - 호출전 Init() 을 반드시 호출해 줘야 한다.
/// @param  
/// @return 
/// @date   Thursday, April 08, 2010  4:18:55 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
#define	DOERR(x) { {} ret = x; goto END; }
XINFLATE_ERR XInflate::Inflate(const BYTE* inBuffer, int inBufferRemain)
{
	// 출력 버퍼 체크
	if(m_outBuffer==NULL)
	{
		m_outBuffer = (BYTE*)malloc(DEFAULT_ALLOC_SIZE);
		m_outBufferAlloc = DEFAULT_ALLOC_SIZE;
	}


	XINFLATE_ERR	ret = XINFLATE_ERR_OK;

	// 출력 버퍼 위치 초기화
	m_outBufferPos = 0;

	// 속도 향상을 위해서 멤버 변수를 로컬로 복사
	UINT		bits;
	int			bitLen;
	BYTE*		windowBuf;
	int			windowPos;
	BYTE*		outBuffer;
	int			outBufferAlloc;
	int			outBufferPos;
	STATE		state;
	int			windowLen;
	int			windowDistCode;
	int			symbol;

	SAVE;


	// 로컬 변수
	const coderecord*	rec;
	XFastHuffItem*		pItem;			// HUFFLOOKUP() 에서 사용
	BYTE				byte;			// 임시 변수
	XINFLATE_ERR		err;


	// 이미 완료되었는지 검사
	if(m_bCompleted)
		DOERR(XINFLATE_ERR_BLOCK_COMPLETED);

	// 초기화 함수 호출 되었는지 검사
	if(m_copyright==NULL)
		DOERR(XINFLATE_ERR_NOT_INITIALIZED);


	// 루프 돌면서 압축 해제
	for(;;)
	{
		FILLBUFFER();

		switch(state)
		{
			// 헤더 분석 시작
		case STATE_START :
			if(bitLen<3) 
				goto END;

			// 마지막 블럭 여부
			m_bFinalBlock = BS_EATBIT();

			// 블럭타입 헤더는 2bit 
			{
				int bType = BS_EATBITS(2);

				if(bType==BTYPE_DYNAMICHUFFMAN)
					state = STATE_DYNAMIC_HUFFMAN;
				else if(bType==BTYPE_NOCOMP)
					state = STATE_NO_COMPRESSION;
				else if(bType==BTYPE_FIXEDHUFFMAN)
					state = STATE_FIXED_HUFFMAN;
				else
					DOERR(XINFLATE_ERR_HEADER);
			}
			break;

			// 압축 안됨
		case STATE_NO_COMPRESSION :
			BS_MOVETONEXTBYTE;
			state = STATE_NO_COMPRESSION_LEN;
			break;

		case STATE_NO_COMPRESSION_LEN :
			// LEN
			if(bitLen<16) goto END;
			m_uncompRemain = BS_EATBITS(16);
			state = STATE_NO_COMPRESSION_NLEN;

			break;

		case STATE_NO_COMPRESSION_NLEN :
			// NLEN
			if(bitLen<16) goto END;
			{
				UINT32 nlen = BS_EATBITS(16);
				// one's complement 
				if( (nlen^0xffff) != m_uncompRemain) 
					DOERR(XINFLATE_ERR_INVALID_NOCOMP_LEN);
			}
			state = STATE_NO_COMPRESSION_BYPASS;
			break;

		case STATE_NO_COMPRESSION_BYPASS :
			// 데이타 가져오기
			if(bitLen<8) goto END;

			//////////////////////////////////////////////
			//
			// 원래 코드
			//
			/*
			{
				byte = BS_EATBITS(8);
				WRITE(byte);
			}
			m_uncompRemain--;
			*/

			if(bitLen%8!=0) {}			// 발생불가, 그냥 확인용

			//////////////////////////////////////////////
			//
			// 아래는 개선된 코드. 그런데 별 차이가 없다 ㅠ.ㅠ
			//

			// 비트 스트림을 먼저 비우고
			while(bitLen && m_uncompRemain)
			{
				byte = BS_EATBITS(8);
				WRITE(byte);
				m_uncompRemain--;
			}

			// 나머지 데이타는 바이트 그대로 쓰기
			{
				int	toCopy, toCopy2;
				toCopy = toCopy2 = min((int)m_uncompRemain, inBufferRemain);

				/* 원래코드
				while(toCopy)
				{
					WRITE(*inBuffer);
					inBuffer++;
					toCopy--;
				}
				*/

				if(outBufferAlloc-outBufferPos > toCopy)
				{
					// 출력 버퍼가 충분한 경우 - 출력 버퍼가 충분한지 여부를 체크하지 않는다.
					while(toCopy)
					{
						WRITE_FAST(*inBuffer);
						inBuffer++;
						toCopy--;
					}
				}
				else
				{
					while(toCopy)
					{
						WRITE(*inBuffer);
						inBuffer++;
						toCopy--;
					}
				}


				m_uncompRemain-=toCopy2;
				inBufferRemain-=toCopy2;
			}
			//
			// 개선된 코드 끝
			//
			//////////////////////////////////////////////

			if(m_uncompRemain==0)
			{
				if(m_bFinalBlock)
					state = STATE_COMPLETED;
				else
					state = STATE_START;
			}
			break;

			// 고정 허프만
		case STATE_FIXED_HUFFMAN :
			if(m_pStaticInfTable==NULL)
				CreateStaticTable();

			m_pCurrentTable = m_pStaticInfTable;
			m_pCurrentDistTable = m_pStaticDistTable;
			state = STATE_GET_SYMBOL;
			break;

			// 길이 가져오기
		case STATE_GET_LEN :
			// zlib 의 inflate_fast 호출 조건 흉내내기
			if(inBufferRemain>FASTINFLATE_MIN_BUFFER_NEED)
			{
				if(symbol<=256)	{}

				RESTORE;
				XINFLATE_ERR result = FastInflate(&inBuffer, &inBufferRemain);
				if(result!=XINFLATE_ERR_OK)
					return result;
				SAVE;
				break;
			}

			rec = &lencodes[symbol - 257];
			if (bitLen < rec->extrabits) goto END;

			// RFC 1951 3.2.5
			// 기본 길이에 extrabit 만큼의 비트의 내용을 더하면 진짜 길이가 나온다
			{
				int extraLen = BS_EATBITS(rec->extrabits);
				windowLen = rec->min + extraLen;	
			}
			state = STATE_GET_DISTCODE;
			//break;	필요 없다..

			// 거리 코드 가져오기
		case STATE_GET_DISTCODE :
			HUFFLOOKUP(windowDistCode, m_pCurrentDistTable);

			if(windowDistCode<0)
				goto END;

			if(windowDistCode==30 || windowDistCode==31)			// 30 과 31은 생길 수 없다. RFC1951 3.2.6
				DOERR(XINFLATE_ERR_INVALID_DIST);

			state = STATE_GET_DIST;

			FILLBUFFER();
			//break;		// 필요없다

			// 거리 가져오기
		case STATE_GET_DIST:
			rec = &distcodes[windowDistCode];
			// DIST 구하기
			if(bitLen<rec->extrabits)
				goto END;
			{
				int dist = rec->min + BS_EATBITS(rec->extrabits);

				// lz77 출력
				while(windowLen)
				{
					byte = *WIN_GETBUF(dist);
					WRITE(byte);
					windowLen--;
				}
			}
	
			state = STATE_GET_SYMBOL;

			FILLBUFFER();
			//break;		// 필요 없다

			// 심볼 가져오기.
		case STATE_GET_SYMBOL :
			HUFFLOOKUP(symbol, m_pCurrentTable);

			if(symbol<0) 
				goto END;
			else if(symbol<256)
			{
				byte = (BYTE)symbol;
				WRITE(byte);
				break;
			}
			else if(symbol==256)	// END OF BLOCK
			{
				if(m_bFinalBlock)
					state = STATE_COMPLETED;
				else
					state = STATE_START;
				break;
			}
			else if(symbol<286)
			{
				state = STATE_GET_LEN;
			}
			else
				DOERR(XINFLATE_ERR_INVALID_SYMBOL);		// 발생 불가

			break;

			// 다이나믹 허프만 시작
		case STATE_DYNAMIC_HUFFMAN :
			if(bitLen<5+5+4) 
				goto END;

			// 테이블 초기화
			SAFE_DEL(m_pDynamicInfTable);
			SAFE_DEL(m_pDynamicDistTable);

			m_literalsNum  = 257 + BS_EATBITS(5);
			m_distancesNum = 1   + BS_EATBITS(5);
			m_lenghtsNum   = 4   + BS_EATBITS(4);

			if(m_literalsNum > 286 || m_distancesNum > 30)
				DOERR(XINFLATE_ERR_INVALID_LEN);

			memset(m_lengths, 0, sizeof(m_lengths));
			m_lenghtsPtr = 0;

			state = STATE_DYNAMIC_HUFFMAN_LENLEN ;
			break;

			// 길이의 길이 가져오기.
		case STATE_DYNAMIC_HUFFMAN_LENLEN :

			if(bitLen<3) 
				goto END;

			// 3bit 씩 코드 길이의 코드 길이 정보 가져오기.
			while (m_lenghtsPtr < m_lenghtsNum && bitLen >= 3) 
			{
				if(m_lenghtsPtr>sizeof(lenlenmap))						// 입력값 체크..
					DOERR(XINFLATE_ERR_INVALID_LEN);

				m_lengths[lenlenmap[m_lenghtsPtr]] = BS_EATBITS(3);
				m_lenghtsPtr++;
			}

			// 다 가져왔으면..
			if (m_lenghtsPtr == m_lenghtsNum)
			{
				// 길이에 대한 허프만 테이블 만들기
				if(CreateTable(m_lengths, 19, m_pLenLenTable, err)==FALSE)
					DOERR(err);
				state = STATE_DYNAMIC_HUFFMAN_LEN;
				m_lenghtsPtr = 0;
			}
			break;

			// 압축된 길이 정보를 m_pLenLenTable 를 거쳐서 가져오기.
		case STATE_DYNAMIC_HUFFMAN_LEN:

			// 다 가져왔으면
			if (m_lenghtsPtr >= m_literalsNum + m_distancesNum) 
			{
				// 최종 다이나믹 테이블 생성 (literal + distance)
				if(	CreateTable(m_lengths, m_literalsNum, m_pDynamicInfTable, err)==FALSE ||
					CreateTable(m_lengths + m_literalsNum, m_distancesNum, m_pDynamicDistTable, err)==FALSE)
					DOERR(err);

				// lenlen 테이블은 이제 더이상 필요없다.
				SAFE_DEL(m_pLenLenTable);

				// 테이블 세팅
				m_pCurrentTable = m_pDynamicInfTable;
				m_pCurrentDistTable = m_pDynamicDistTable;

				// 진짜 압축 해제 시작
				state = STATE_GET_SYMBOL;
				break;
			}

			{
				// 길이 정보 코드 가져오기
				int code=-1;
				HUFFLOOKUP(code, m_pLenLenTable);

				if (code == -1)
					goto END;

				if (code < 16) 
				{
					if(m_lenghtsPtr>sizeof(m_lengths))		// 값 체크
						DOERR(XINFLATE_ERR_INVALID_LEN);

					m_lengths[m_lenghtsPtr] = code;
					m_lenghtsPtr++;
				} 
				else 
				{
					m_lenExtraBits = (code == 16 ? 2 : code == 17 ? 3 : 7);
					m_lenAddOn = (code == 18 ? 11 : 3);
					m_lenRep = (code == 16 && m_lenghtsPtr > 0 ? m_lengths[m_lenghtsPtr - 1] : 0);
					state = STATE_DYNAMIC_HUFFMAN_LENREP;
				}
			}
			break;

		case STATE_DYNAMIC_HUFFMAN_LENREP:
			if (bitLen < m_lenExtraBits)
				goto END;

			{
				int repeat = m_lenAddOn + BS_EATBITS(m_lenExtraBits);

				while (repeat > 0 && m_lenghtsPtr < m_literalsNum + m_distancesNum) 
				{
					m_lengths[m_lenghtsPtr] = m_lenRep;
					m_lenghtsPtr++;
					repeat--;
				}
			}

			state = STATE_DYNAMIC_HUFFMAN_LEN;
			break;

		case STATE_COMPLETED :
			m_bCompleted = TRUE;
			goto END;
			break;

		default :
			{}
		}
	}

END :
	// 로컬 변수 멤버 변수로 다시 적용
	RESTORE;

	
	return ret;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///         lengh 정보만 가지고 code 를 생성한다. 
//          RFC 1951 3.2.2
/// @param  
/// @return 
/// @date   Wednesday, April 07, 2010  1:53:36 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XInflate::CreateCodes(BYTE* lengths, int numSymbols, int* codes)
{
	int		bits;
	int		code = 0;
	int		bitLenCount[MAX_CODELEN+1];
	int		next_code[MAX_CODELEN+1];
	int		i;
	int		n;
	int		len;

	memset(bitLenCount, 0, sizeof(bitLenCount));
	for(i=0;i<numSymbols;i++)
	{
		bitLenCount[lengths[i]] ++;
	}

	bitLenCount[0] = 0;

	for(bits=1;bits<=MAX_CODELEN;bits++)
	{
		code = (code + bitLenCount[bits-1]) << 1;
		next_code[bits] = code;
	}

	for(n=0; n<numSymbols; n++)
	{
		len = lengths[n];
		if(len!=0)
		{
			codes[n] = next_code[len];
			next_code[len]++;
		}
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///         codes + lengths 정보를 가지고 테이블을 만든다.
/// @param  
/// @return 
/// @date   Wednesday, April 07, 2010  3:43:21 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XInflate::CreateTable(BYTE* lengths, int numSymbols, XFastHuffTable*& pTable, XINFLATE_ERR& err)
{
	int			codes[MAX_SYMBOLS];
	// lengths 정보를 가지고 자동으로 codes 정보를 생성한다.
	if(CreateCodes(lengths, numSymbols, codes)==FALSE) 
	{	
		err = XINFLATE_ERR_CREATE_CODES;
		{} 
		return FALSE;
	}
	if(_CreateTable(codes, lengths, numSymbols, pTable)==FALSE)
	{
		err = XINFLATE_ERR_CREATE_TABLE;
		{} 
		return FALSE;
	}
	return TRUE;
}
BOOL XInflate::_CreateTable(int* codes, BYTE* lengths, int numSymbols, XFastHuffTable*& pTable)
{
	int		bitLenCount[MAX_CODELEN+1];
	int		symbol;
	int		bitLen;

	// bit length 구하기
	memset(bitLenCount, 0, sizeof(bitLenCount));
	for(symbol=0;symbol<numSymbols;symbol++)
		bitLenCount[lengths[symbol]] ++;


	// 허프만 트리에서 유효한 최소 bitlen 과 최대 bitlen 구하기
	int	bitLenMax = 0;
	int	bitLenMin = MAX_CODELEN;

	for(bitLen=1;bitLen<=MAX_CODELEN;bitLen++)
	{
		if(bitLenCount[bitLen])
		{
			bitLenMax = max(bitLenMax, bitLen);
			bitLenMin = min(bitLenMin, bitLen);
		}
	}

	// 테이블 생성
	pTable = new XFastHuffTable;
	if(pTable==0) {{} return FALSE;}			// 발생 불가.
	pTable->Create(bitLenMin, bitLenMax);


	// 테이블 채우기
	for(symbol=0;symbol<numSymbols;symbol++)
	{
		bitLen = lengths[symbol];
		if(bitLen)
			pTable->SetValue(symbol, bitLen, codes[symbol]);
	}

#ifdef _DEBUG
	for(UINT i=0;i<pTable->itemCount;i++)
	{
		if(pTable->pItem[i].symbol==-1)
			{}
	}
#endif

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///         static 허프만에 사용할 테이블 만들기
/// @param  
/// @return 
/// @date   Thursday, April 08, 2010  4:17:06 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
void XInflate::CreateStaticTable()
{
	BYTE		lengths[MAX_SYMBOLS];

	// static symbol 테이블 만들기
	// RFC1951 3.2.6
    memset(lengths, 8, 144);
    memset(lengths + 144, 9, 256 - 144);
    memset(lengths + 256, 7, 280 - 256);
    memset(lengths + 280, 8, 288 - 280);

	XINFLATE_ERR err;
	CreateTable(lengths, MAX_SYMBOLS, m_pStaticInfTable, err);

	// static dist 테이블 만들기
	// RFC1951 3.2.6
	memset(lengths, 5, 32);
	CreateTable(lengths, 32, m_pStaticDistTable, err);
}

// 비트스트림 초기화
void XInflate::BS_Init()
{
	m_bits = 0;
	m_bitLen = 0;
}


// lz77 윈도우 생성
void XInflate::Win_Init()
{
	if(m_windowBuf==NULL)
		m_windowBuf = (BYTE*)malloc(DEFLATE_WINDOW_SIZE);
	m_windowPos = 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///         입력 버퍼가 충분할 경우 빠른 디코딩을 수행한다.
/// @param  
/// @return 
/// @date   Thursday, May 13, 2010  1:43:34 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
XINFLATE_ERR XInflate::FastInflate(const BYTE** ppinBuffer, int* pinBufferRemain)
{
	XINFLATE_ERR	ret = XINFLATE_ERR_OK;

	// 파라메터 저장
	const BYTE* inBuffer = *ppinBuffer;
	int  inBufferRemain = *pinBufferRemain;


	// 속도 향상을 위해서 멤버 변수를 로컬로 복사
	UINT		bits;
	int			bitLen;
	BYTE*		windowBuf;
	int			windowPos;
	BYTE*		outBuffer;
	int			outBufferAlloc;
	int			outBufferPos;
	STATE		state;
	int			windowLen;
	int			windowDistCode;
	int			symbol;

	SAVE;


	// 로컬 변수
	const coderecord*	rec;
	XFastHuffItem*		pItem;			// HUFFLOOKUP() 에서 사용
	BYTE				byte;			// 임시 변수
	int					extraLen;


	// 루프 돌면서 압축 해제
	while(inBufferRemain>FASTINFLATE_MIN_BUFFER_NEED)
	{
		FILLBUFFER_FAST();

		/////////////////////////////////////
		// 길이 가져오기
		rec = &lencodes[symbol - 257];

		// RFC 1951 3.2.5
		// 기본 길이에 extrabit 만큼의 비트의 내용을 더하면 진짜 길이가 나온다
		extraLen = BS_EATBITS(rec->extrabits);
		windowLen = rec->min + extraLen;	

		
		/////////////////////////////////////
		// 거리 코드 가져오기
		HUFFLOOKUP(windowDistCode, m_pCurrentDistTable);

		if(windowDistCode==30 || windowDistCode==31)			// 30 과 31은 생길 수 없다. RFC1951 3.2.6
			DOERR(XINFLATE_ERR_INVALID_DIST);

		FILLBUFFER_FAST();


		/////////////////////////////////////
		// 거리 가져오기
		rec = &distcodes[windowDistCode];
		// DIST 구하기
		int dist = rec->min + BS_EATBITS(rec->extrabits);


		/////////////////////////////////////
		// lz77 출력
		if(outBufferAlloc-outBufferPos > windowLen)
		{
			// 출력 버퍼가 충분한 경우 - 출력 버퍼가 충분한지 여부를 체크하지 않는다.
			do
			{
				byte = *WIN_GETBUF(dist);
				WRITE_FAST(byte);
			}while(--windowLen);
		}
		else
		{
			// 출력 버퍼가 충분하지 않은 경우
			do
			{
				byte = *WIN_GETBUF(dist);
				WRITE(byte);
			}while(--windowLen);
		}


		/////////////////////////////////////
		// 심볼 가져오기.
		for(;;)
		{
			// 입력 버퍼 체크
			if(!(inBufferRemain>FASTINFLATE_MIN_BUFFER_NEED)) 
			{
				state = STATE_GET_SYMBOL;
				goto END;
			}

			FILLBUFFER_FAST();

			HUFFLOOKUP_FAST(symbol, m_pCurrentTable);

			if(symbol<0) 
			{{} goto END;}					// 발생 불가.
			else if(symbol<256)
			{
				byte = (BYTE)symbol;
				WRITE(byte);
			}
			else if(symbol==256)	// END OF BLOCK
			{
				if(m_bFinalBlock)
					state = STATE_COMPLETED;
				else
					state = STATE_START;
				// 함수 종료
				goto END;
			}
			else if(symbol<286)
			{
				// 길이 가져오기로 진행한다.
				break;
			}
			else
				DOERR(XINFLATE_ERR_INVALID_SYMBOL);		// 발생 불가
		}
	}

END :
	RESTORE;

	// 파라메터 복원
	*ppinBuffer = inBuffer;
	*pinBufferRemain = inBufferRemain;

	return ret;
}

