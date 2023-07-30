#define _WINSOCKAPI_
#include "Splash_XBitmap.h"

#ifndef _XBITMAP_SUPPORT_PNG
#	include "Splash_XPng.h"
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ETC 
//

// 파일과 메모리에서 데이터를 읽기위한 공통 인터페이스
class XBitmapStream
{
public:
	XBitmapStream(){}
	virtual ~XBitmapStream(){}

	virtual BOOL	Read(LPVOID lpBuffer, DWORD nNumberOfBytesToRead) PURE;
	virtual BOOL	Write(LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite) PURE;
	virtual int		SetPointer(LONG lDistanceToMove, DWORD dwMoveMethod) PURE;
	virtual int		GetCurrentPosition() PURE;
	virtual int		GetSize() PURE;
};

class XBitmapFileStream : public XBitmapStream
{
public :
	XBitmapFileStream()
	{
		m_nBufLen = 0;
		m_nBufPos = 0;
		m_nFilePos = 0;
		m_hFile = INVALID_HANDLE_VALUE;
		m_nFileSize = -1;
		m_bAutoCloseHandle = FALSE;
	}
	virtual ~XBitmapFileStream()
	{
		if(m_bAutoCloseHandle && m_hFile != INVALID_HANDLE_VALUE) { CloseHandle(m_hFile); m_hFile = INVALID_HANDLE_VALUE; }
	}
	virtual BOOL	Read(LPVOID lpBuffer, DWORD nNumberOfBytesToRead);
	virtual BOOL	Write(LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite)
	{
		DWORD dwWritten;
		return ::WriteFile(m_hFile, lpBuffer, nNumberOfBytesToWrite, &dwWritten, NULL);
	}
	virtual int	SetPointer(LONG lDistanceToMove, DWORD dwMoveMethod) 
	{
		m_nBufLen = 0;		// 버퍼 비우기.
		m_nBufPos = 0;

		switch(dwMoveMethod)	// 가상 위치(m_nFilePos)를 기준으로 이동해야 한다.
		{
		case FILE_BEGIN   : m_nFilePos = lDistanceToMove; break;
		case FILE_CURRENT : m_nFilePos = m_nFilePos + lDistanceToMove; break;
		case FILE_END     : m_nFilePos = GetSize() - lDistanceToMove; break;
		}
		m_nFilePos = ::SetFilePointer(m_hFile, m_nFilePos, NULL, FILE_BEGIN);
		return m_nFilePos;
	}
	virtual int	GetSize() 
	{
		//if(m_nFileSize<0)
			m_nFileSize = ::GetFileSize(m_hFile, NULL);
		return m_nFileSize;
	}
	virtual int	GetCurrentPosition()
	{
		return m_nFilePos;			// 가상 위치를 알려준다.
	}
public :
	enum { LEN_BUF = 4096*4 };
	HANDLE	m_hFile;
	BYTE	m_buf[LEN_BUF];
	int		m_nBufLen;				// 버퍼에 들어있는 유효 데이타의 크기. m_nBufPos 와 무관
	int		m_nBufPos;				// 버퍼에서 사용한 위치
	int		m_nFilePos;				// 가상의 파일 포인터
	int		m_nFileSize;
	BOOL	m_bAutoCloseHandle;		// 종료시 자동으로 핸들 닫을까 말까.
};

BOOL XBitmapFileStream::Read( LPVOID lpBuffer, DWORD nNumberOfBytesToRead )
{
	BYTE* pBuffer = (BYTE*)lpBuffer;
	int nBufRemain = m_nBufLen - m_nBufPos;		// 남아 있는 버퍼크기
	if((int)nNumberOfBytesToRead<=nBufRemain)							// 버퍼가 충분한가?
	{
		memcpy(pBuffer, m_buf + m_nBufPos, nNumberOfBytesToRead);		// 복사해 주고.
		m_nBufPos += nNumberOfBytesToRead;								// 버퍼유효 위치 이동
		m_nFilePos += nNumberOfBytesToRead;								// 가상 파일 포인터 이동
		return TRUE;
	}
	else
	{
		if(nBufRemain)			// 버퍼가 모자라기는 한데, 남아 있기는 하다면..
		{
			memcpy(pBuffer, m_buf + m_nBufPos, nBufRemain);		// 남아 있는 버퍼 다 복사해 주고...
			m_nBufLen = 0;											// 버퍼 다 비우고.
			m_nBufPos = 0;

			pBuffer += nBufRemain;						// 파라메터로 넘어온 정보 수정하기.
			nNumberOfBytesToRead -= nBufRemain;
			m_nFilePos += nBufRemain;						// 가상 파일 포인터 이동
		}

		// 읽어야할 내용이 내부 버퍼보다 크다면, 실제 파일에서 바로 읽어서 넣어 주기.
		if(nNumberOfBytesToRead >= LEN_BUF)
		{
			DWORD nNumberOfBytesRead;
			if(ReadFile(m_hFile, pBuffer, nNumberOfBytesToRead, &nNumberOfBytesRead, NULL)==FALSE ||
				nNumberOfBytesRead != nNumberOfBytesToRead) {return FALSE;}
			m_nFilePos = ::SetFilePointer(m_hFile, 0, NULL, FILE_CURRENT);	// 새로운 파일 위치.
			return TRUE;
		}
		else
		{
			// 내부 버퍼에 파일 내용 저장하기.
			m_nBufLen = 0;
			m_nBufPos = 0;
			if(ReadFile(m_hFile, m_buf, LEN_BUF, (DWORD*)&m_nBufLen, NULL)==FALSE) {return FALSE;}
			if(m_nBufLen==0) return FALSE;

			// 아직 읽을 내용이 있다면...
			if(nNumberOfBytesToRead)
				return Read(pBuffer, nNumberOfBytesToRead);		// 리커젼 하게 처리해 버린다.
			return TRUE;
		}
	}
}


// 메모리 읽기 인터페이스
class XBitmapMemoryStream : public XBitmapStream
{
public :		// XBitmapMemoryStream
	XBitmapMemoryStream() { m_pData = NULL; m_nDataLen = 0; m_nCurPos = 0;  m_bAutoFreeMemory= FALSE; }
	virtual ~XBitmapMemoryStream()
	{
		if(m_bAutoFreeMemory && m_pData != NULL) { free(m_pData); }
	}
	virtual BOOL	Read(LPVOID lpBuffer, DWORD nNumberOfBytesToRead)
	{
		if(m_nCurPos + (int)nNumberOfBytesToRead > m_nDataLen) {return FALSE;}
		memcpy(lpBuffer, m_pData + m_nCurPos, nNumberOfBytesToRead);
		m_nCurPos += nNumberOfBytesToRead;
		return TRUE;
	}
	virtual BOOL	Write(LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite)
	{
			// 미 테스트, 확인 필요
		if(m_nDataLen-m_nCurPos<(int)nNumberOfBytesToWrite){ return FALSE;}
		memcpy(m_pData+m_nCurPos, lpBuffer, nNumberOfBytesToWrite);
		m_nCurPos+=nNumberOfBytesToWrite;
		return TRUE;
	}
	virtual int	SetPointer(LONG lDistanceToMove, DWORD dwMoveMethod) 
	{
		if(m_nDataLen==0) return 0;
		switch(dwMoveMethod)
		{
		case FILE_BEGIN :
			m_nCurPos = lDistanceToMove;
			break;
		case FILE_CURRENT :
			m_nCurPos += lDistanceToMove;
			break;
		case FILE_END :
			m_nCurPos = m_nDataLen - lDistanceToMove;
			break;
		}
		if(m_nCurPos>m_nDataLen) {/**/ m_nCurPos = m_nDataLen;}
		return m_nCurPos;
	}
	virtual int	GetSize() 
	{
		return m_nDataLen;
	}
	virtual int	GetCurrentPosition()
	{
		return m_nCurPos;
	}
public :
	LPBYTE			m_pData;
	int				m_nDataLen;
	int				m_nCurPos;
	BOOL			m_bAutoFreeMemory;		// 종료시 자동으로 메모리 free
};



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// XBITMAP
//


XBitmap::XBitmap()
{
	Init();
}


XBitmap::~XBitmap()
{
	Delete();
}


void XBitmap::Delete()
{
	if(m_hDC) 
		DeleteDC();

	if(m_hBitmap)
		::DeleteObject(m_hBitmap);

	Init();
}

void XBitmap::Init()
{
	m_hBitmap = NULL;
	m_stride = 0;
	m_paddingByte = 0;
	m_width = 0;
	m_height = 0;
	m_pixelFormat = PIXELFORMAT(0);
	m_bytePerPixel = 0;
	m_pixel = NULL;

	m_hDC = NULL;
	m_hOldBitmap = NULL;
	m_dcLockCount = 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///         dib 생성
/// @param  
/// @return 
/// @date   Thursday, November 11, 2010  10:50:35 AM
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XBitmap::CreateDib(int width, int height, PIXELFORMAT pixelFormat, BOOL bBottomUpDib)
{
	Delete();

	m_stride = ((width * (DWORD)pixelFormat + 31) & ~31) >> 3;			// 8의 배수
	m_width = width;
	m_height = height;
	m_pixelFormat = pixelFormat;
	m_bytePerPixel = m_pixelFormat / 8;
	m_paddingByte = m_stride - (m_width * m_bytePerPixel);				// 패딩 바이트 계산

	BITMAPINFO	info;
	info.bmiHeader.biHeight = bBottomUpDib ? height : -height;
	info.bmiHeader.biWidth = width;
	info.bmiHeader.biBitCount = (WORD)pixelFormat;
	info.bmiHeader.biClrUsed = 0;
	info.bmiHeader.biClrImportant = 0;
	info.bmiHeader.biCompression = BI_RGB;
	info.bmiHeader.biXPelsPerMeter = 0;
	info.bmiHeader.biYPelsPerMeter = 0;
	info.bmiHeader.biPlanes = 1;
	info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	info.bmiHeader.biSizeImage = m_stride * height;

	m_hBitmap = CreateDIBSection(NULL, &info, DIB_RGB_COLORS, (LPVOID*)&m_pixel, NULL, 0);

	if(m_hBitmap==NULL)	// 생성 실패
	{Init();return FALSE;}

	return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///         윈도우에 그린다.
/// @param  
/// @return 
/// @date   Thursday, November 11, 2010  10:50:40 AM
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XBitmap::DrawTo(HWND hWnd, int dx, int dy, int sx, int sy, int sw, int sh, DWORD rop)
{
	HDC hDC = ::GetDC(hWnd);
	if(hDC==NULL) { return FALSE;}

	// 화면 밖으로 안나가게 하기..
	if(sw==-1 && sh==-1)
	{
		RECT	rClient;
		::GetClientRect(hWnd, &rClient);
		sw = (rClient.right-rClient.left) - dx - sx;
		sh = (rClient.bottom-rClient.top) - dy - sy;
	}

	BOOL ret = DrawTo(hDC, dx, dy, sx, sy, sw, sh, rop);
	::ReleaseDC(hWnd, hDC);

	return ret;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///         DC 에 그린다.
/// @param  
/// @return 
/// @date   Thursday, November 11, 2010  11:02:57 AM
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XBitmap::DrawTo(HDC hDC, int dx, int dy, int sx, int sy, int sw, int sh, DWORD rop)
{
	if (sw == -1)
		sw = Width();
	if (sh == -1)
		sh = Height();

	if(sw==0 || sh==0) return FALSE;			// 그릴것이 없다..

	//ASSERT(sw > 0 && sh > 0);

	HDC hdcSrc = CreateCompatibleDC(hDC);
	if(hdcSrc==NULL){ return FALSE;}

	HBITMAP hBitmapOld = (HBITMAP)SelectObject(hdcSrc, GetBitmap());

	BitBlt(hDC, dx, dy, sw, sh, hdcSrc, sx, sy, rop);

	SelectObject(hdcSrc, hBitmapOld);

	::DeleteDC(hdcSrc);

	return TRUE;
}


HDC XBitmap::GetDC()
{
	m_dcLockCount ++;

	if(m_hDC==NULL)
	{
		m_hDC = CreateCompatibleDC(NULL);
		m_hOldBitmap = (HBITMAP)SelectObject(m_hDC, m_hBitmap);
		//ASSERT(m_hDC);
		//ASSERT(m_hBitmap);
	}

	return m_hDC;
}

void XBitmap::ReleaseDC()
{
	m_dcLockCount = max(0, m_dcLockCount-1);

	if(m_dcLockCount==0 && m_hDC)
		DeleteDC();
}

void XBitmap::DeleteDC()
{
	//ASSERT(m_hDC);
	//ASSERT(m_hOldBitmap);

	SelectObject(m_hDC, m_hOldBitmap);
	::DeleteDC(m_hDC);

	m_hDC = NULL;
	m_hOldBitmap = NULL;

	m_dcLockCount = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///         PNG 파일 로드하기.
/// @param  
/// @return 
/// @date   Monday, January 03, 2011  10:20:54 AM
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _XBITMAP_SUPPORT_PNG

#include "splash_xpng.h"

class XPngStreamImpl : public XPngStream
{
public :
	XPngStreamImpl()
	{
		m_buf = NULL;
	}
	~XPngStreamImpl()
	{
		if(m_buf) free(m_buf);
	}
	BOOL	Read(BYTE* data, UINT32 len)
	{
		return pInput->Read(data, len);
	}
	BOOL	SkipData(UINT32 len)
	{
		pInput->SetPointer(len, FILE_CURRENT);
		return TRUE;
	}
	BYTE*	GetBuffer(UINT32 len)
	{
		if(m_buf) free(m_buf);
		m_buf = (BYTE*)malloc(len);

		if(m_buf==NULL){ return NULL;}
		if(pInput->Read(m_buf, len)==FALSE)
		{ return NULL;}

		return m_buf;
	}
	XBitmapStream*		pInput;
	BYTE*				m_buf;
};


BOOL XBitmap::LoadPNG(LPCTSTR szFilePathName)
{
	HANDLE hFile = CreateFile(szFilePathName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) { return FALSE; }
	BOOL ret = LoadPNG(hFile);
	CloseHandle(hFile);
	return ret;
}

BOOL XBitmap::LoadPNG(LPBYTE data, int length)
{
	XBitmapMemoryStream	memory;
	memory.m_pData = data;
	memory.m_nDataLen = length;
	memory.m_nCurPos = 0;
	return LoadPNG(&memory);
}

BOOL XBitmap::LoadPNG(HANDLE hFile)
{
	XBitmapFileStream file;
	file.m_hFile = hFile;
	return LoadPNG(&file);
}

BOOL XBitmap::LoadPNG(XBitmapStream* pInput)
{
	Delete();

	XPng			xpng;
	XPngStreamImpl	stream;
	stream.pInput = pInput;

	if(xpng.Open(&stream)==FALSE)
	{ return FALSE;}

	const XPngImageHeader &header = xpng.GetImageHeader();

	PIXELFORMAT		pixelFormat;

	if(header.colorType==XPng::COLORTYPE_COLOR_ALPHA)
		pixelFormat = PIXELFORMAT_32;
	else if(header.colorType==XPng::COLORTYPE_COLOR)
		pixelFormat = PIXELFORMAT_24;
	else if(header.colorType==XPng::COLORTYPE_COLOR_PAL)
		pixelFormat = PIXELFORMAT_32;		// 8 to 32bit
	else
	{ return FALSE;}				// 지원 안함.


	if(CreateDib(header.width, header.height, pixelFormat, FALSE)==FALSE)
	{ return FALSE;}

	xpng.SetDecodeData(m_pixel, m_stride, m_paddingByte, m_pixelFormat / 8);

	if(xpng.Decode()==FALSE)
	{ return FALSE;}

	return TRUE;
}


#endif // _XBITMAP_SUPPORT_PNG




////////////////////////////////////////////////////////////////////////////////////////////////////
///         단색으로 전체 다 채우기.
/// @param  
/// @return 
/// @date   Wednesday, February 09, 2011  1:17:29 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XBitmap::Clear(COLORREF clr)
{
	if(m_pixel==NULL) { return FALSE;}

	LPBYTE	src = (LPBYTE)&clr;
	int		bytePerPixel = GetBytePerPixel();
	LPBYTE	dst = m_pixel;

	// 한줄 채우기.
	for(int x=0;x<m_width;x++)
	{
		*(dst+0) = *(src+2);			// RGB -> BGR 로 뒤집기
		*(dst+1) = *(src+1);
		*(dst+2) = *(src+0);

		// 32bit alpha
		if(m_pixelFormat==PIXELFORMAT_32)
			*(dst+3) = *(src+3);

		// 다음 픽셀
		dst += bytePerPixel;
	}

	// 한줄을 나머지 줄에 복사
	LPBYTE	start = m_pixel;

	dst = start + m_stride;
	for(int y=1;y<m_height;y++)
	{
		memcpy(dst, start, m_stride);
		dst += m_stride;
	}
	return TRUE;
}


BOOL XBitmap::ChecknRecalulateParams(SIZE* sizeDest, int& dx, int& dy, int& sx, int& sy, int& sw, int& sh)
{
	if(sw <= 0 || sh <= 0) 
		return FALSE;

	sx = sx - min(0, dx);					// 음수 클리핑 ( dx 나 dy 가 0 보다 작을 경우 )
	sy = sy - min(0, dy);
	sw = sw + min(0, dx);					// 2008.2.9 빼기에서 덧셈으로 수정 -> 이게 맞는지 틀린지 모르겠다. 적어도 dx,dy가 음수일때는 맞다
	sh = sh + min(0, dy);
	dx = max(0, dx);
	dy = max(0, dy);

	dx = dx - min(0, sx);					// 음수 클리핑 ( sx 나 sy 가 0 보다 작을 경우 )
	dy = dy - min(0, sy);
	sw = sw + min(0, sx);
	sh = sh + min(0, sy);

	sx = max(0, sx);
	sy = max(0, sy);

	sw = min(sw, (int)Width() - sx);		// 넘어가는 값 클리핑( sw, sh 가 src 나 dest 의 범위를 벗어날 경우 )
	sh = min(sh, (int)Height() - sy);
	sw = min(sw, (int)sizeDest->cx - dx);
	sh = min(sh, (int)sizeDest->cy - dy);

	if(sw<=0 || sh<=0) return FALSE;		// 그릴 내용이 없다..

	return TRUE;
}

BOOL XBitmap::ChecknRecalulateParams(XBitmap* pDibDest, int& dx, int& dy, int& sx, int& sy, int& sw, int& sh)
{
	BOOL ret;
	ret = ChecknRecalulateParams(&(pDibDest->GetSize()), dx, dy, sx, sy, sw, sh);
	return ret;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///         UpdateLayeredWindow() 를 호출하기 전 알파채널 값을 rgb 에 적용한다.
/// @param  
/// @return 
/// @date   Friday, June 03, 2011  7:32:54 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XBitmap::ApplyAlpha2RGB4LayerdWindow()
{
	if(GetPixelFormat()!=PIXELFORMAT_32) { return FALSE;}

	int y,x;
	int h = Height();
	int w = Width();
	BYTE* ptr;
	int	alpha;

	for(y=0;y<h;y++)
	{
		ptr = GetXYPos_Fast(0, y);

		for(x=0;x<w;x++)
		{
			alpha = *(ptr+3);
			*(ptr+0) = *(ptr+0)*alpha/0xff;
			*(ptr+1) = *(ptr+1)*alpha/0xff;
			*(ptr+2) = *(ptr+2)*alpha/0xff;

			ptr+=4;
		}
	}

	return TRUE;
}

