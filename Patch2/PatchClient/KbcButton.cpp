// KbcButton.cpp : implementation file
//

#include "stdafx.h"
#include "KbcButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKbcButton

CKbcButton::CKbcButton()
{	
	m_bDisable			= FALSE;
	m_bCursorOnButton	= FALSE;
	m_bHover			= FALSE;
	m_bToggle			= FALSE;
	m_bClicked			= FALSE;
	m_bLButtonDown		= FALSE;

	memset( m_strButtonText, 0, sizeof(m_strButtonText) );
}

CKbcButton::~CKbcButton()
{
}

void CKbcButton::SetButtonText( char *pstrText )
{
	if( pstrText != NULL )
	{
		sprintf( m_strButtonText, "%s", pstrText );
	}
}

char *CKbcButton::GetButtonText()
{
	return m_strButtonText;
}

BEGIN_MESSAGE_MAP(CKbcButton, CButton)
	//{{AFX_MSG_MAP(CKbcButton)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_MESSAGE(WM_MOUSEHOVER,OnMouseHover)
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKbcButton message handlers

void CKbcButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// LPDRAWITEMSTRUCT �� ���� DC ������...
	CWnd			*pcWnd;
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	// ��Ȱ��ȭ �����̸� ��Ȱ��ȭ�� ���õ� ��ư�� �׷��ش�.
	pcWnd = pDC->GetWindow();

	if( !pcWnd->IsWindowEnabled() )
	{
		m_bmpImage.Draw(pDC,3,0,0,m_nMask, GetButtonText() );
	}

	// ��Ȱ��ȭ ���°� �ƴ϶��..
	else
	{	
		//. fake toggle.
		if(m_bToggle)
		{
			if(m_bClicked)
			{
				m_bmpImage.Draw(pDC,2,0,0,m_nMask, GetButtonText());
			}
			else
			{
				m_bmpImage.Draw(pDC,0,0,0,m_nMask, GetButtonText());
			}
		}
		else
		{
			// ��ư�� Ŭ�� �Ǿ����� �׸�
			if( lpDrawItemStruct->itemState & ODS_SELECTED )
			{
				m_bLButtonDown = TRUE;
				m_bmpImage.Draw(pDC,2,0,0,m_nMask, GetButtonText());
			}

			// ��ư�� Ŀ���� �ö� ���� �� �׸�.
			else if ( m_bHover)
				m_bmpImage.Draw(pDC,1,0,0,m_nMask, GetButtonText());

			// �ƹ� ���µ� �ƴҶ� �׸�..
			else
			{
				m_bLButtonDown = FALSE;
				m_bmpImage.Draw(pDC,0,0,0,m_nMask, GetButtonText());
			}
		}
	}
}

void CKbcButton::SetBmpButtonImage(CString strFileName,UINT nMask)
{
	m_bmpImage.LoadBitmap(strFileName);
	m_nMask = nMask;
	

	// �켱 ��ư�� ũ�⸦ ����� ���´�.
	// �׸��� ���̿� ������ ��ư�� ũ�⸦ ���Դ�.
	m_rectButton.left	= 0;
	m_rectButton.top	= 0;
	m_rectButton.right	= m_bmpImage.GetSliceWidth();
	m_rectButton.bottom	= m_bmpImage.GetHeight();


	// ��ư�� ũ�⸦ �׸����ٰ� �����־�� �Ѵ�. 
	// ���̿� ���� �׸����� �����Ƿ� ���ϱ� ����.
	// ��ư�� left �� top �� �˾ƾ� �ϴµ� �ؿ� ó���ϸ� ��������.
	CWnd *pWnd = this->GetParent();
	GetWindowRect(&m_rectButtonPos);
	pWnd->ScreenToClient(m_rectButtonPos);
	m_rectButtonPos.right	= m_rectButtonPos.left + m_bmpImage.GetSliceWidth();
	m_rectButtonPos.bottom	= m_rectButtonPos.top  + m_bmpImage.GetHeight();

	MoveWindow(m_rectButtonPos);
}


void CKbcButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	// WM_MOUSEHOVE �� WM_MOUSELEAVE�� �߻���Ű�� ���ؼ� 
	// _TrackMouseEvent�� ȣ�����ش�.
	// m_bCursorOnWindow �� üũ�� ���� ���� ���콺�� ��ư �ۿ� �ִٰ�
	// ��ư ���� �����Ͱ� �������� ��� �ѹ��� WM_MOUSEHOVER �޼����� �߻���Ű�� �����̴�.
	// �����Ͱ� ��ư�� ������ ��쿡�� �ѹ��� WM_MOUSELEAVE �޼����� �߻���Ű�� ����.
	if( m_bCursorOnButton == FALSE )
	{
		TRACKMOUSEEVENT tme;
		ZeroMemory(&tme,sizeof(TRACKMOUSEEVENT));
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE|TME_HOVER;
		tme.dwHoverTime = 1;
		m_bCursorOnButton = _TrackMouseEvent(&tme);
	}
	
	CButton::OnMouseMove(nFlags, point);
}

void CKbcButton::SetBmpButtonImage(UINT32 ulResourceID ,UINT nMask)
{
	m_bmpImage.LoadBitmap(ulResourceID);
	m_nMask = nMask;
	

	// �켱 ��ư�� ũ�⸦ ����� ���´�.
	// �׸��� ���̿� ������ ��ư�� ũ�⸦ ���Դ�.
	m_rectButton.left	= 0;
	m_rectButton.top	= 0;
	m_rectButton.right	= m_bmpImage.GetSliceWidth();
	m_rectButton.bottom	= m_bmpImage.GetHeight();


	// ��ư�� ũ�⸦ �׸����ٰ� �����־�� �Ѵ�. 
	// ���̿� ���� �׸����� �����Ƿ� ���ϱ� ����.
	// ��ư�� left �� top �� �˾ƾ� �ϴµ� �ؿ� ó���ϸ� ��������.
	CWnd *pWnd = this->GetParent();
	if( pWnd )
	{
		pWnd->GetWindowRect(&m_rectButtonPos);
		pWnd->ScreenToClient(m_rectButtonPos);
	}

	m_rectButtonPos.right	= m_rectButtonPos.left + m_bmpImage.GetSliceWidth();
	m_rectButtonPos.bottom	= m_rectButtonPos.top  + m_bmpImage.GetHeight();

	MoveWindow(m_rectButtonPos);
}

LRESULT CKbcButton::OnMouseHover(WPARAM wparam, LPARAM lparam)
{
	m_bHover = TRUE;
	// DrawItem�� ȣ��Ƕ�� Invalidate()�Լ� �θ�
	Invalidate();
	return 0L;
}

LRESULT CKbcButton::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	m_bCursorOnButton	= FALSE;
	m_bHover			= FALSE;
	// DrawItem�� ȣ��Ƕ�� Invalidate()�Լ� �θ�
	Invalidate();
	return 0L;
}

void CKbcButton::SetButtonEnable()
{
	m_bDisable = FALSE;
	// DrawItem�� ȣ��Ƕ�� Invalidate()�Լ� �θ�
	Invalidate();
}

void CKbcButton::SetButtonDisable()
{
	m_bDisable = TRUE;
	// DrawItem�� ȣ��Ƕ�� Invalidate()�Լ� �θ�
	Invalidate();
}

void CKbcButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// m_bDisable �� TRUE ��� 	CButton::OnLButtonDown(nFlags, point) ��
	// �������� �ʴ´�. �ֳ� �̰� ����Ǹ� �θ𿡰� Notify ���ֱ� ������
	// �θ� �� ��ư�� ���� ���ٴ°� �˰� �ȴ�. �𸣰� �ؾ� �ϹǷ� 
	// �Լ��� �ҷ����� �ʴ´�.
	if( !m_bDisable )
		CButton::OnLButtonDown(nFlags, point);
}

BOOL CKbcButton::PreTranslateMessage(MSG* pMsg) 
{
	// ������ ���� ���� RelayEvent�� ȣ�����ش�.
	// ������ ���� �������� �ʾҴٸ�..
	// �� SetToolTipText �Լ��� ȣ����� �ʾ� Create���� �ʾҴٸ�
	// �ǳʶڴ�.
	if( m_ToolTip.GetSafeHwnd() != NULL )
		m_ToolTip.RelayEvent(pMsg);
	
	return CButton::PreTranslateMessage(pMsg);
}

void CKbcButton::SetToolTipText(CString strText)
{
	m_ToolTip.Create(this);
	m_ToolTip.Activate(TRUE);
	m_ToolTip.AddTool(this,(LPCTSTR)strText);
}

BOOL CKbcButton::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
//	::SetCursor(m_hCursor);
	return TRUE;
}

void CKbcButton::SetCursor(HCURSOR hCursor)
{
//	m_hCursor = hCursor;
}

BOOL CKbcButton::OnEraseBkgnd(CDC* pDC) 
{
	// �� �Լ����� �� �ִ� �۾��� ��ư�ؿ� �ִ� ������ ��ư�������ٰ� �׷��ִ� ���̴�.
	// ���� ��ư�� �ö󰡴� ������ ���� �׸��� �׷��� �ִٸ� ��ư�� �׸� ������ �ڵ��� ������ ������
	// ��ư ������ ������ �����ϰ� �׷��൵ ������� ��ư ����� ��Ÿ���� �ȴ�.
	// �׷��Ƿ� ��ư�� �ö󰡴� ������, ��ư�� �ö󰡴� �� ��ġ�� �׸��� ���ͼ�
	// ��ư���ٰ� ������� �׷��ָ� ������ �� ó�� ���δ�. �� �Լ��� �����̴�..


	// ���⼭ ���� �θ� ������ DC�� ��� ���� ���� �� �� �ִ�.
	CWnd *pWnd		 = this->GetParent();
	CDC  *pParentDC	 = pWnd->GetDC();

	CDC					srcDC;

	CBitmap				srcBmp;
	CBitmap				*oldsrcBmp;

	
	srcDC.CreateCompatibleDC(pParentDC);
	srcBmp.CreateCompatibleBitmap(pParentDC,m_bmpImage.GetSliceWidth(),m_bmpImage.GetHeight());
	oldsrcBmp = srcDC.SelectObject(&srcBmp);

	// �ᱹ �θ� �������� ��ư�� �ö󰡸鼭 ���ԵǴ� �׸��� srcDC�� �����´�.
	srcDC.BitBlt(0,0,m_bmpImage.GetSliceWidth(),m_bmpImage.GetHeight(),
		pParentDC,m_rectButtonPos.left,m_rectButtonPos.top,SRCCOPY);


	pParentDC->SelectObject(oldsrcBmp);
	ReleaseDC(pParentDC);

	pDC->BitBlt(0,0,m_bmpImage.GetSliceWidth(),m_bmpImage.GetHeight(),&srcDC,0,0,SRCCOPY);

	return TRUE;
}
