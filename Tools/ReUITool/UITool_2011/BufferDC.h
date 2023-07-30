// BufferDC.h: interface for the CBufferDC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUFFERDC_H__F631A3F3_9053_406F_A147_63DB4CFDA08B__INCLUDED_)
#define AFX_BUFFERDC_H__F631A3F3_9053_406F_A147_63DB4CFDA08B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBufferDC : public CDC  
{

private:
	
	CBufferDC(const CBufferDC &src) { }
	CBufferDC& operator=(const CBufferDC &src) { }

protected:
	BOOL Attach(HDC hDC);
	HDC Detach();

private:
	
	CDC*			m_pTarget;							//대상 윈도우 DC에 대한 포인터
	PAINTSTRUCT		m_PaintStruct;
	CRect			m_RcClient, m_RcWindow;				//대상 윈도우의 크기 정보

	CDC				m_MemoryDC;							//버퍼 DC
	CBitmap			m_MemoryBmp, *m_pOldMemoryBmp;		//버퍼링을 위한 비트맵

public:
	CWnd*			m_pParent;							//대상 윈도우에 대한 포인터
	
	CBufferDC() { }
	CBufferDC(CWnd *pParent);
	~CBufferDC();

public:
	inline CRect ClientRect() const { return m_RcClient; }
	inline CRect WindowRect() const { return m_RcWindow; }
	inline CRect UpdateRect() const { return m_PaintStruct.rcPaint; }

	operator HDC() const { return m_MemoryDC.m_hDC; }       //  DC handle for API functions
};

#endif // !defined(AFX_BUFFERDC_H__F631A3F3_9053_406F_A147_63DB4CFDA08B__INCLUDED_)
