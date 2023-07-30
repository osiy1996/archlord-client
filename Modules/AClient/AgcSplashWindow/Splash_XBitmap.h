////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// simple bitmap wrapper
///
/// - 라이선스 : zlib license (http://www.kippler.com/etc/zlib_license/ 참고)
/// 
/// @author   parkkh
/// @date     Thursday, November 11, 2010  10:01:03 AM
/// 
/// Copyright(C) 2010, 2011 Bandisoft, All rights reserved.
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////


/*
                 -= History =-

* 2010/11/11 - 작업 시작
* 2011/01/03 - PNG 로드 기능 추가
* 2011/02/09 - Clear() 추가

*/

/*
predefines --

#define _XBITMAP_SUPPORT_PNG		// png 파일 지원

*/


#pragma once
#include <Windows.h>

enum PIXELFORMAT
{
	PIXELFORMAT_24 = 24,		// 24비트 컬러
	PIXELFORMAT_32 = 32,		// 32비트 컬러(알파 포함)
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
	__forceinline LPBYTE GetXYPos_Fast(int x, int y)	// 범위 체크를 하지 않아 GetXYPos() 보다 약간 빠르다.
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
	HBITMAP		m_hBitmap;					// bitmap 핸들
	int			m_stride;					// 한줄당 바이트수
	int			m_paddingByte;				// 한줄당 padding 바이트 수
	int			m_width;					// 폭 pixel
	int			m_height;					// 높이 pixel
	PIXELFORMAT	m_pixelFormat;				// 픽셀당 비트 수 (24, 32)
	int			m_bytePerPixel;				// == m_pixelFormat/8
	LPBYTE		m_pixel;					// 픽셀 메모리 시작 값

private :
	HDC			m_hDC;
	HBITMAP		m_hOldBitmap;
	int			m_dcLockCount;				// GetDC() 호출 카운트
};

