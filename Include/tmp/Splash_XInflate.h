////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// DEFLATE 알고리즘의 압축 해제(inflate) 클래스
///
/// - 버전 : v 1.1.0 (2010/5/25)
///
/// - 라이선스 : zlib license (http://www.kippler.com/etc/zlib_license/ 참고)
///
/// - 참고 자료
///  . RFC 1951 (http://www.http-compression.com/rfc1951.pdf)
///  . Halibut(http://www.chiark.greenend.org.uk/~sgtatham/halibut/)의 deflate.c 를 일부 참고하였음
/// 
/// @author   kippler@gmail.com ( http://www.kippler.com )
/// @date     Monday, April 05, 2010  5:59:34 PM
/// 
/// Copyright(C) 2010 Bandisoft, All rights reserved.
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <windows.h>


// 에러 코드
enum XINFLATE_ERR
{
	XINFLATE_ERR_OK,						// 성공
	XINFLATE_ERR_NOT_INITIALIZED,			// Init() 을 호출하지 않았음.
	XINFLATE_ERR_HEADER,					// 헤더에 문제 있음
	XINFLATE_ERR_INVALID_NOCOMP_LEN,		// no compressio 헤더에 문제 있음
	XINFLATE_ERR_ALLOC_FAIL,				// 메모리 ALLOC 실패
	XINFLATE_ERR_INVALID_DIST,				// DIST 정보에서 에러 발생
	XINFLATE_ERR_INVALID_SYMBOL,			// SYMBOL 정보에서 에러 발생
	XINFLATE_ERR_BLOCK_COMPLETED,			// 압축 해제가 이미 완료되었음.
	XINFLATE_ERR_CREATE_CODES,				// code 생성중 에러 발생
	XINFLATE_ERR_CREATE_TABLE,				// table 생성중 에러 발생
	XINFLATE_ERR_INVALID_LEN,				// LEN 정보에 문제 있음
	XINFLATE_ERR_INSUFFICIENT_OUT_BUFFER,	// 외부 출력 버퍼가 모자라다.
};


class XFastHuffTable;
class XInflate
{
public :
	XInflate();
	~XInflate();
	void					Init();													// 초기화: 매번 압축 해제를 시작할때마다 호출해 줘야 한다.
	void					Free();													// 내부에 사용된 모든 객체를 해제한다. 더이상 이 클래스를 사용하지 않을때 호출하면 된다.
	XINFLATE_ERR			Inflate(const BYTE* inBuffer, int inBufferLen);			// 압축해제 작업. 큰 메모리 블럭 압축 해제시 여러번 계속 호출하면 된다.
	int						GetOutBufferLen() const { return m_outBufferPos; }		// Inflate() 호출의 결과에 접근하기.
	const BYTE*				GetOutBuffer() const { return m_outBuffer; }
	BOOL					IsCompleted() { return m_bCompleted; }					// 압축 데이타의 마지막 블럭까지 도달했는지 여부

	void					SetOutBuffer(BYTE* outBuf, int outBufLen);				// 출력 버퍼를 직접 지정하기.

private :
	void					CreateStaticTable();
	BOOL					CreateCodes(BYTE* lengths, int numSymbols, int* codes);
	BOOL					CreateTable(BYTE* lengths, int numSymbols, XFastHuffTable*& pTable, XINFLATE_ERR& err);
	BOOL					_CreateTable(int* codes, BYTE* lengths, int numSymbols, XFastHuffTable*& pTable);

private :
	XINFLATE_ERR			FastInflate(const BYTE** ppinBuffer, int* pinBufferRemain);

	enum	STATE						// 내부 상태
	{
		STATE_START,
		
		STATE_NO_COMPRESSION,
		STATE_NO_COMPRESSION_LEN,
		STATE_NO_COMPRESSION_NLEN,
		STATE_NO_COMPRESSION_BYPASS,

		STATE_FIXED_HUFFMAN,

		STATE_GET_SYMBOL_ONLY,
		STATE_GET_LEN,
		STATE_GET_DISTCODE,
		STATE_GET_DIST,
		STATE_GET_SYMBOL,

		STATE_DYNAMIC_HUFFMAN,
		STATE_DYNAMIC_HUFFMAN_LENLEN,
		STATE_DYNAMIC_HUFFMAN_LEN,
		STATE_DYNAMIC_HUFFMAN_LENREP,

		STATE_COMPLETED,
	};

private :
	STATE					m_state;				// 현재 압축 해제 상태

	XFastHuffTable*			m_pStaticInfTable;		// static huffman table
	XFastHuffTable*			m_pStaticDistTable;		// static huffman table (dist)
	XFastHuffTable*			m_pDynamicInfTable;		// 
	XFastHuffTable*			m_pDynamicDistTable;	// 
	XFastHuffTable*			m_pCurrentTable;		// 
	XFastHuffTable*			m_pCurrentDistTable;	// 
	int						m_symbol;				// current huffman symbol

	int						m_windowLen;			// LZ77 길이
	int						m_windowDistCode;		// dist 를 얻기 위한 코드

	BYTE*					m_outBuffer;			// 출력 버퍼 (내부에서 alloc 한다)
	int						m_outBufferAlloc;		// 출력 버퍼 크기
	int						m_outBufferPos;			// 출력 버퍼 쓰는 위치
	BOOL					m_isExternalOutBuffer;	// 출력 버퍼를 외부 버퍼를 사용하남?

	UINT32					m_uncompRemain;			// no compression 인 경우 블럭의 남은 데이타
	BOOL					m_bFinalBlock;			// 현재 마지막 블럭을 처리중인가?
	BOOL					m_bCompleted;			// 완료 되었는가?

	int						m_literalsNum;			// dynamic huffman - # of literal/length codes   (267~286)
	int						m_distancesNum;			// "               - # of distance codes         (1~32)
	int						m_lenghtsNum;			//                 - # of code length codes      (4~19)
	BYTE					m_lengths[286+32];		// literal + distance 의 length 가 같이 들어간다.
	int						m_lenghtsPtr;			// m_lengths 에서 현재 위치
	int						m_lenExtraBits;
	int						m_lenAddOn;
	int						m_lenRep;
	XFastHuffTable*			m_pLenLenTable;
	const char*				m_copyright;

private :
	enum { DEFLATE_WINDOW_SIZE = 32768 };			// RFC 에 정의된 WINDOW 크기
	enum { DEFAULT_ALLOC_SIZE = 1024*256 };			// 기본 출력 버퍼 크기
	enum { MAX_SYMBOLS = 288 };						// deflate 에 정의된 심볼 수
	enum { MAX_CODELEN = 16 };						// deflate 허프만 트리 최대 코드 길이
	enum { FASTINFLATE_MIN_BUFFER_NEED = 6 };		// FastInflate() 호출시 필요한 최소 입력 버퍼수

private :											// 비트 스트림 처리 부분
	void					BS_Init();
	UINT					m_bits;					// 32bit 비트 스트림
	int						m_bitLen;				// m_bits 에서 유효한 bit count

private :											// lz77 window 처리 부분
	void					Win_Init();
	BYTE*					m_windowBuf;
	int						m_windowPos;			// 현재 쓰고자 하는 위치

#ifdef _DEBUG
	int						m_inputCount;
	int						m_outputCount;
#endif

};

