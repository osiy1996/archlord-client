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
	
	CDC*			m_pTarget;							//��� ������ DC�� ���� ������
	PAINTSTRUCT		m_PaintStruct;
	CRect			m_RcClient, m_RcWindow;				//��� �������� ũ�� ����

	CDC				m_MemoryDC;							//���� DC
	CBitmap			m_MemoryBmp, *m_pOldMemoryBmp;		//���۸��� ���� ��Ʈ��

public:
	CWnd*			m_pParent;							//��� �����쿡 ���� ������
	
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
