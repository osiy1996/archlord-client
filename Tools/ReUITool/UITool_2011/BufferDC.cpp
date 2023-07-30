// BufferDC.cpp: implementation of the CBufferDC class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BufferDC.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBufferDC::CBufferDC(CWnd *pParent)
: m_pParent(pParent)
{
	ASSERT(pParent);

	//대상 윈도우에 대한 정보를 수집한다.
	m_pTarget = m_pParent->BeginPaint(&m_PaintStruct);
	m_pParent->GetClientRect(&m_RcClient);
	m_pParent->GetWindowRect(&m_RcWindow);

	//대상 윈도우에 대한 DC를 생성한다.
	m_MemoryDC.CreateCompatibleDC(m_pTarget);
	//대상 DC에 대한 메모리 비트맵을 생성하여 Select 한다.
	m_MemoryBmp.CreateBitmap(m_RcClient.Width(), m_RcClient.Height(), 
		m_MemoryDC.GetDeviceCaps(PLANES),
		m_MemoryDC.GetDeviceCaps(BITSPIXEL), 0);
	m_pOldMemoryBmp = m_MemoryDC.SelectObject(&m_MemoryBmp);

	//메모리 버퍼에 Attach한다.
	Attach(m_MemoryDC);
}

//////////////////////////////////////////////////////////////////////
CBufferDC::~CBufferDC()
{
	//메모리 DC의 내용을 대상 윈도우에 출력한다.
	//내부적으로 비트맵에 출력한 것이므로 해당 비트맵을 1:1로 복사한다.
	m_pTarget->BitBlt(
		m_PaintStruct.rcPaint.left,
		m_PaintStruct.rcPaint.top, 
		m_PaintStruct.rcPaint.right - m_PaintStruct.rcPaint.left, 
		m_PaintStruct.rcPaint.bottom - m_PaintStruct.rcPaint.top, 
		&m_MemoryDC,
		m_PaintStruct.rcPaint.left,
		m_PaintStruct.rcPaint.top, SRCCOPY);

	m_MemoryDC.SelectObject(m_pOldMemoryBmp);
	m_pParent->EndPaint(&m_PaintStruct);

	Detach();
}

//////////////////////////////////////////////////////////////////////
BOOL CBufferDC::Attach(HDC hDC)
{
	return CDC::Attach(hDC);
}

//////////////////////////////////////////////////////////////////////
HDC CBufferDC::Detach()
{
	return CDC::Detach();
}