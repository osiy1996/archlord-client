#if !defined(AFX_KBCBMP_H__3B18A7EA_F8CB_4410_8E1E_AA0B72F039BB__INCLUDED_)
#define AFX_KBCBMP_H__3B18A7EA_F8CB_4410_8E1E_AA0B72F039BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KbcBmp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CKbcBmp view
//
// ���� - ������(kbckbc@postech.ac.kr | http://kbckbc.com)
// Ȥ�ö� ����Ͻô� �в��� Ŭ���� �̸��̳��� �������ֽñ� �ٶ��ϴ�.
// �׷��� ���� ����� ��� �Ŷ��� �������ֽñ� �ٶ��ϴ�.

class CKbcBmp : public CView
{
public:
	void				LoadBitmap(CString);	// ���� �̸��� ���� ���� ��..
	void				LoadBitmap(UINT);		// ���ҽ��� ���� ��..

	// Draw - ��Ʈ�� �׷��ִ� �Լ��̴�. ���� �˰���?
	// Draw(CDC *pDC,int nX,int nY)						- �׳� ��Ʈ�� �׷��� ��..
	// Draw(CDC *pDC,int nX,int nY,UINT nMask)			- ���� �Ⱥ��̰� ��Ʈ�� �׷��� ��..
	// Draw(CDC *pDC,int nSeq,int nX,int nY,UINT nMask)	- KbcButton ���� ȣ��Ǵ� �Լ�..
	void				Draw(CDC *pDC,int nX,int nY);	
	void				Draw(CDC *pDC,int nX,int nY,UINT nMask);	
	void				Draw(CDC *pDC,int nSeq,int nX,int nY,UINT nMask, char *pstrText);	
	/////////////////////////////////////////////////// 

	int					GetWidth()		 { return m_nWidth; }
	int					GetSliceWidth()	 { return m_nSliceWidth; }
	int					GetHeight()		 { return m_nHeight; }
	void				SetFontColor(COLORREF color){ m_FontColor = color; }

protected:
	HBITMAP				m_hBitmap; 
	BITMAP				m_BmpInfo; 
	CBitmap				m_Bitmap;

	int					m_nWidth;
	int					m_nSliceWidth;
	int					m_nHeight;

	int					m_nMode;

	COLORREF			m_FontColor;

protected:
	
	DECLARE_DYNCREATE(CKbcBmp)
	
// Attributes
public:
	CKbcBmp();           // protected constructor used by dynamic creation
	virtual ~CKbcBmp();
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKbcBmp)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CKbcBmp)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KBCBMP_H__3B18A7EA_F8CB_4410_8E1E_AA0B72F039BB__INCLUDED_)
