////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// DEFLATE �˰����� ���� ����(inflate) Ŭ����
///
/// - ���� : v 1.1.0 (2010/5/25)
///
/// - ���̼��� : zlib license (http://www.kippler.com/etc/zlib_license/ ����)
///
/// - ���� �ڷ�
///  . RFC 1951 (http://www.http-compression.com/rfc1951.pdf)
///  . Halibut(http://www.chiark.greenend.org.uk/~sgtatham/halibut/)�� deflate.c �� �Ϻ� �����Ͽ���
/// 
/// @author   kippler@gmail.com ( http://www.kippler.com )
/// @date     Monday, April 05, 2010  5:59:34 PM
/// 
/// Copyright(C) 2010 Bandisoft, All rights reserved.
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <windows.h>


// ���� �ڵ�
enum XINFLATE_ERR
{
	XINFLATE_ERR_OK,						// ����
	XINFLATE_ERR_NOT_INITIALIZED,			// Init() �� ȣ������ �ʾ���.
	XINFLATE_ERR_HEADER,					// ����� ���� ����
	XINFLATE_ERR_INVALID_NOCOMP_LEN,		// no compressio ����� ���� ����
	XINFLATE_ERR_ALLOC_FAIL,				// �޸� ALLOC ����
	XINFLATE_ERR_INVALID_DIST,				// DIST �������� ���� �߻�
	XINFLATE_ERR_INVALID_SYMBOL,			// SYMBOL �������� ���� �߻�
	XINFLATE_ERR_BLOCK_COMPLETED,			// ���� ������ �̹� �Ϸ�Ǿ���.
	XINFLATE_ERR_CREATE_CODES,				// code ������ ���� �߻�
	XINFLATE_ERR_CREATE_TABLE,				// table ������ ���� �߻�
	XINFLATE_ERR_INVALID_LEN,				// LEN ������ ���� ����
	XINFLATE_ERR_INSUFFICIENT_OUT_BUFFER,	// �ܺ� ��� ���۰� ���ڶ��.
};


class XFastHuffTable;
class XInflate
{
public :
	XInflate();
	~XInflate();
	void					Init();													// �ʱ�ȭ: �Ź� ���� ������ �����Ҷ����� ȣ���� ��� �Ѵ�.
	void					Free();													// ���ο� ���� ��� ��ü�� �����Ѵ�. ���̻� �� Ŭ������ ������� ������ ȣ���ϸ� �ȴ�.
	XINFLATE_ERR			Inflate(const BYTE* inBuffer, int inBufferLen);			// �������� �۾�. ū �޸� �� ���� ������ ������ ��� ȣ���ϸ� �ȴ�.
	int						GetOutBufferLen() const { return m_outBufferPos; }		// Inflate() ȣ���� ����� �����ϱ�.
	const BYTE*				GetOutBuffer() const { return m_outBuffer; }
	BOOL					IsCompleted() { return m_bCompleted; }					// ���� ����Ÿ�� ������ ������ �����ߴ��� ����

	void					SetOutBuffer(BYTE* outBuf, int outBufLen);				// ��� ���۸� ���� �����ϱ�.

private :
	void					CreateStaticTable();
	BOOL					CreateCodes(BYTE* lengths, int numSymbols, int* codes);
	BOOL					CreateTable(BYTE* lengths, int numSymbols, XFastHuffTable*& pTable, XINFLATE_ERR& err);
	BOOL					_CreateTable(int* codes, BYTE* lengths, int numSymbols, XFastHuffTable*& pTable);

private :
	XINFLATE_ERR			FastInflate(const BYTE** ppinBuffer, int* pinBufferRemain);

	enum	STATE						// ���� ����
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
	STATE					m_state;				// ���� ���� ���� ����

	XFastHuffTable*			m_pStaticInfTable;		// static huffman table
	XFastHuffTable*			m_pStaticDistTable;		// static huffman table (dist)
	XFastHuffTable*			m_pDynamicInfTable;		// 
	XFastHuffTable*			m_pDynamicDistTable;	// 
	XFastHuffTable*			m_pCurrentTable;		// 
	XFastHuffTable*			m_pCurrentDistTable;	// 
	int						m_symbol;				// current huffman symbol

	int						m_windowLen;			// LZ77 ����
	int						m_windowDistCode;		// dist �� ��� ���� �ڵ�

	BYTE*					m_outBuffer;			// ��� ���� (���ο��� alloc �Ѵ�)
	int						m_outBufferAlloc;		// ��� ���� ũ��
	int						m_outBufferPos;			// ��� ���� ���� ��ġ
	BOOL					m_isExternalOutBuffer;	// ��� ���۸� �ܺ� ���۸� ����ϳ�?

	UINT32					m_uncompRemain;			// no compression �� ��� ���� ���� ����Ÿ
	BOOL					m_bFinalBlock;			// ���� ������ ���� ó�����ΰ�?
	BOOL					m_bCompleted;			// �Ϸ� �Ǿ��°�?

	int						m_literalsNum;			// dynamic huffman - # of literal/length codes   (267~286)
	int						m_distancesNum;			// "               - # of distance codes         (1~32)
	int						m_lenghtsNum;			//                 - # of code length codes      (4~19)
	BYTE					m_lengths[286+32];		// literal + distance �� length �� ���� ����.
	int						m_lenghtsPtr;			// m_lengths ���� ���� ��ġ
	int						m_lenExtraBits;
	int						m_lenAddOn;
	int						m_lenRep;
	XFastHuffTable*			m_pLenLenTable;
	const char*				m_copyright;

private :
	enum { DEFLATE_WINDOW_SIZE = 32768 };			// RFC �� ���ǵ� WINDOW ũ��
	enum { DEFAULT_ALLOC_SIZE = 1024*256 };			// �⺻ ��� ���� ũ��
	enum { MAX_SYMBOLS = 288 };						// deflate �� ���ǵ� �ɺ� ��
	enum { MAX_CODELEN = 16 };						// deflate ������ Ʈ�� �ִ� �ڵ� ����
	enum { FASTINFLATE_MIN_BUFFER_NEED = 6 };		// FastInflate() ȣ��� �ʿ��� �ּ� �Է� ���ۼ�

private :											// ��Ʈ ��Ʈ�� ó�� �κ�
	void					BS_Init();
	UINT					m_bits;					// 32bit ��Ʈ ��Ʈ��
	int						m_bitLen;				// m_bits ���� ��ȿ�� bit count

private :											// lz77 window ó�� �κ�
	void					Win_Init();
	BYTE*					m_windowBuf;
	int						m_windowPos;			// ���� ������ �ϴ� ��ġ

#ifdef _DEBUG
	int						m_inputCount;
	int						m_outputCount;
#endif

};

