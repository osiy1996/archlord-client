////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// simple bitmap wrapper
///
/// - ���̼��� : zlib license (http://www.kippler.com/etc/zlib_license/ ����)
/// 
/// @author   parkkh
/// @date     Thursday, November 11, 2010  10:01:03 AM
/// 
/// Copyright(C) 2010, 2011 Bandisoft, All rights reserved.
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////


/*
                 -= History =-

* 2010/11/11 - �۾� ����
* 2011/01/03 - PNG �ε� ��� �߰�
* 2011/02/09 - Clear() �߰�

*/

/*
predefines --

#define _XBITMAP_SUPPORT_PNG		// png ���� ����

*/


#pragma once
#include <Windows.h>

enum PIXELFORMAT
{
	PIXELFORMAT_24 = 24,		// 24��Ʈ �÷�
	PIXELFORMAT_32 = 32,		// 32��Ʈ �÷�(���� ����)
};

#define PIXEL_BYTE_24BIT	3
#define PIXEL_BYTE_32BIT	4


#ifndef RGBA
#	define RGBA(r,g,b,a)       ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))| (((DWORD)(BYTE)(b))<<16) | (((DWORD)(BYTE)(a))<<24) ) )
#endif 
#ifndef GetAValue
#	define GetAValue(rgba)		(LOBYTE((rgba)>>24))
#endif


class XBitmapStream;
class XBitmap
{
public :
	XBitmap();
	~XBitmap();

public :
	BOOL		CreateDib(int width, int height, PIXELFORMAT pixelFormat, BOOL bBottomUpDib=FALSE);
	void		Delete();

public :
	BOOL		Clear(COLORREF clr);

public :
	HDC			GetDC();
	void		ReleaseDC();

	HBITMAP		GetBitmap() const { return m_hBitmap; }
	int			Width() const { return m_width; }
	int			Height() const { return m_height; }
	int			GetBytePerPixel() const { return m_pixelFormat/8; }
	BOOL		FillRect(const RECT& rect, COLORREF clr);
	PIXELFORMAT	GetPixelFormat() { return m_pixelFormat; }
	BOOL		ApplyAlpha2RGB4LayerdWindow();

public :
	__forceinline LPBYTE GetXYPos_Fast(int x, int y)	// ���� üũ�� ���� �ʾ� GetXYPos() ���� �ణ ������.
	{	
		//ASSERT(x < m_width && y < m_height);
		return m_pixel + m_stride * y + m_bytePerPixel * x;
	}


public :
	BOOL		DrawTo(HWND hWnd, int dx = 0, int dy = 0, int sx = 0, int sy = 0, int sw = -1, int sh = -1, DWORD rop = SRCCOPY);
	BOOL		DrawTo(HDC hdc, int dx = 0, int dy = 0, int sx = 0, int sy = 0, int sw = -1, int sh = -1, DWORD rop = SRCCOPY);
	BOOL		DrawTo(XBitmap* pDib, int dx = 0, int dy = 0, int sx = 0, int sy = 0, int sw = -1, int sh = -1, BOOL bProcess32bitAlpha=FALSE);

public :
	BOOL		LoadPNG(LPCTSTR szFilePathName);
	BOOL		LoadPNG(HANDLE hFile);
	BOOL		LoadPNG(LPBYTE data, int length);
	BOOL		LoadPNG(XBitmapStream* pInput);


private :
	void		Init();
	void		DeleteDC();

	SIZE		GetSize() const { SIZE s; s.cx = m_width; s.cy = m_height; return s; }
	BOOL		ChecknRecalulateParams(SIZE* sizeDest, int& dx, int& dy, int& sx, int& sy, int& sw, int& sh);
	BOOL		ChecknRecalulateParams(XBitmap* pDibDest, int& dx, int& dy, int& sx, int& sy, int& sw, int& sh);

private :
	HBITMAP		m_hBitmap;					// bitmap �ڵ�
	int			m_stride;					// ���ٴ� ����Ʈ��
	int			m_paddingByte;				// ���ٴ� padding ����Ʈ ��
	int			m_width;					// �� pixel
	int			m_height;					// ���� pixel
	PIXELFORMAT	m_pixelFormat;				// �ȼ��� ��Ʈ �� (24, 32)
	int			m_bytePerPixel;				// == m_pixelFormat/8
	LPBYTE		m_pixel;					// �ȼ� �޸� ���� ��

private :
	HDC			m_hDC;
	HBITMAP		m_hOldBitmap;
	int			m_dcLockCount;				// GetDC() ȣ�� ī��Ʈ
};

