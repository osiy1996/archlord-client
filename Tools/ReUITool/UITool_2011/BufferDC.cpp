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

	//��� �����쿡 ���� ������ �����Ѵ�.
	m_pTarget = m_pParent->BeginPaint(&m_PaintStruct);
	m_pParent->GetClientRect(&m_RcClient);
	m_pParent->GetWindowRect(&m_RcWindow);

	//��� �����쿡 ���� DC�� �����Ѵ�.
	m_MemoryDC.CreateCompatibleDC(m_pTarget);
	//��� DC�� ���� �޸� ��Ʈ���� �����Ͽ� Select �Ѵ�.
	m_MemoryBmp.CreateBitmap(m_RcClient.Width(), m_RcClient.Height(), 
		m_MemoryDC.GetDeviceCaps(PLANES),
		m_MemoryDC.GetDeviceCaps(BITSPIXEL), 0);
	m_pOldMemoryBmp = m_MemoryDC.SelectObject(&m_MemoryBmp);

	//�޸� ���ۿ� Attach�Ѵ�.
	Attach(m_MemoryDC);
}

//////////////////////////////////////////////////////////////////////
CBufferDC::~CBufferDC()
{
	//�޸� DC�� ������ ��� �����쿡 ����Ѵ�.
	//���������� ��Ʈ�ʿ� ����� ���̹Ƿ� �ش� ��Ʈ���� 1:1�� �����Ѵ�.
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