////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// PNG �̹��� ���ڴ� Ŭ����
///
/// - ���̼��� : zlib license (http://www.kippler.com/etc/zlib_license/ ����)
///
/// - ���� �ڷ�
///   . RFC : http://www.faqs.org/rfcs/rfc2083.html
///   . TR : http://www.w3.org/TR/PNG/
/// 
/// @author   kippler@gmail.com
/// @date     Thursday, May 06, 2010  10:09:06 AM
/// 
/// Copyright(C) 2010 Bandisoft, All rights reserved.
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////
/* 
	�����ϴ� ����
		- RGB24, RGBA32 �÷� ����
		- RGB24 �� BGR ����, RGBA32 �� BGRA ���� ���(HBITMAP �� ����)

	�������� �ʴ� ����
		- �÷� �ɵ��� 8BIT�� ���� (1,2,4,16 ���� ����)
		- ���ͷ��̽� �̹��� ���� ����
		- ��� �̹��� ���� ����

	PREDEFINE
		#define		_XPNG_NO_CRC		// <- �̰� �����ϸ� crc üũ�� ���Ѵ�. (�ӵ� ���)
*/



#pragma once
#include <windows.h>

#define _XBITMAP_SUPPORT_PNG

struct XPngStream
{
	virtual BOOL	Read(BYTE* data, UINT32 len) = 0;
	virtual BOOL	SkipData(UINT32 len) = 0;
	virtual BYTE*	GetBuffer(UINT32 len) = 0;			// ���� ��ġ���� len ��ŭ ��ȿ�� ����Ÿ�� ���۸� �����Ѵ�.
														// ���� ������ (���ʿ���) malloc �� ���ϱ� ���ؼ� Read() ��� ����Ѵ�.
};


enum XPNG_ERR
{
	XPNG_ERR_OK,
	XPNG_ERR_NOT_PNG_FILE,					// PNG ������ �ƴ�
	XPNG_ERR_CANT_READ_HEADER,				// ��� �д��� ���� �߻�
	XPNG_ERR_UNSUPPORTED_FORMAT,			// ���� �������� �ʴ� ����
	XPNG_ERR_CANT_READ,						// ����Ÿ�� �д��� ���� �߻� (���� �޺κ��� �߷ȴٴ���..)
	XPNG_ERR_INVALID_CRC,					// crc ���� �߻�.
	XPNG_ERR_INVALID_DATA,					// �߸��� ������ ������ ����
};


struct XPngImageHeader						// �̹��� ���� (IHDR ����)
{
	UINT32	width;
	UINT32	height;
	BYTE	bitDepth;						// Bit depth is a single-byte integer giving the number of bits per sample or 
											// per palette index (not per pixel).
	BYTE	colorType;						// COLORTYPE_COLOR_ALPHA Ȥ�� COLORTYPE_COLOR
	BYTE	compressionMethod;
	BYTE	filterMode;
	BYTE	interlaceMethod;
};


class XInflate;
struct XPngChunkHeader;

class XPng
{
public :
	XPng();
	~XPng();

	BOOL					Open(XPngStream* pStream);
	const XPngImageHeader&	GetImageHeader() { return m_header; }
	XPNG_ERR				GetErr() { return m_err; }

	void					SetDecodeData(BYTE* imageBuffer, int stride, int paddingByte, int bytePerPixel);
	BOOL					Decode();

	void					Close();
	BOOL					IsEnd() { return m_bEnd; }

private :
	void					Init();
	BOOL					ReadChunkHeader(XPngChunkHeader* chunkHeader);
	BOOL					ReadUINT32(UINT32& value);
	BOOL					ReadUINT8(BYTE& value);
	void					CRCInit();
	BOOL					CheckCRC();
	BOOL					ProcessIDATChunk(int dataLen);
	BOOL					ProcessPalette(int dataLen);
	BOOL					ProcessTrans(int dataLen);

public :
	enum {	LEN_PNG_SIGNATURE = 8 };
	enum {	RGBA = 4 };

			// �÷�	Ÿ�� - ������ ��Ʈ �ǹ� [ 1:palette used , 2:color used , 4:alpha channel used ]
	enum {	COLORTYPE_BW = 0,				
			COLORTYPE_COLOR = 2,			// RGB, no alpha, no pal, 
			COLORTYPE_COLOR_PAL = 3,		// palette used + color used
			COLORTYPE_BW_ALPHA = 4,			// no color, alpha channel
			COLORTYPE_COLOR_ALPHA = 6};		// RGBA

	enum {	FILTER_NONE		= 0, 
			FILTER_SUB		= 1,
			FILTER_UP		= 2,
			FILTER_AVERAGE	= 3,
			FILTER_PAETH	= 4 };

	enum {	LEN_PALETTE		= 256*3 };
	enum {	LEN_TRANS		= 256 };

private :
	XPngStream*			m_pStream;
	XPNG_ERR			m_err;
	XPngImageHeader		m_header;
#ifndef _XPNG_NO_CRC
	DWORD				m_crc32;
#endif
	XInflate*			m_pInflate;
	BOOL				m_bEnd;

	BYTE*				m_imageBuffer;
	int					m_imageStride;						// �� ���ΰ� ���� ������ �޸� ����
	int					m_imageBytePerPixel;
	int					m_imageWidthByte;					// �� ������ �����ϴ� ���� ����Ʈ �� == (m_imageStride - m_imagePaddingByte)
	int					m_imagePaddingByte;

	BYTE*				m_imageCurBuffer;
	int					m_imageCurWidthByte;

	BOOL				m_bFirst;
	int					m_nBytePerPixel;					// �ȼ��� ����Ʈ��

	const int*			m_ColorMapTableCur;
	int					m_ColorMapTablePos;					// m_ColorMapTableCur �迭�� ���� ��ġ - 24��Ʈ�� ��� 0~2 , 32��Ʈ�� ��� 0~3
	int					m_ColorMapTableLen;

	BYTE*				m_scanline_before;
	BYTE*				m_scanline_current;

	// ����
	BYTE				m_filter;				// ���� ������ ���� ����
	BYTE				m_filter_a[RGBA];		// ���� �ȼ� ����
	BYTE*				m_filter_b;				// ���� ��ĵ���� ������
	BYTE				m_filter_c[RGBA];		// �»�� �ȼ� ����
	BYTE*				m_filter_b_dummy;		// �ӽ� ���� - 
												// �� ó�� ���۵Ǵ� ���Ͱ� FILTER_NONE �� �ƴϰ�, m_filter_b �� �����ϴ� ����
												// (FILTER_PAETH�� ����)�� ���, ���� ���� ������ ������ �߻��ϴ°��� ���� ���ؼ�
												// �ӽ÷� 0 ���� ä���� ����Ѵ�.

	BOOL				m_bPaletted;			// �ȷ�Ʈ ����
	BYTE*				m_palette;				// palette ���̺� (256*3)
	BYTE*				m_trans;				// alpha ���̺� (256)

};

