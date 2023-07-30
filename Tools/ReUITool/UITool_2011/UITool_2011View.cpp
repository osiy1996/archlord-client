
// UITool_2011View.cpp : CUITool_2011View 클래스의 구현
//

#include "stdafx.h"
#include "UITool_2011.h"

#include "UITool_2011Doc.h"
#include "UITool_2011View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CUITool_2011View

IMPLEMENT_DYNCREATE(CUITool_2011View, CView)

BEGIN_MESSAGE_MAP(CUITool_2011View, CView)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
	ON_WM_SETCURSOR()
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CUITool_2011View 생성/소멸

CUITool_2011View::CUITool_2011View()
:m_MouseOnObj(NULL), m_eMouseState(EMS_NORMAL), m_eMousePosition(E_NTNT)
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CUITool_2011View::~CUITool_2011View()
{
}

BOOL CUITool_2011View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CUITool_2011View 그리기

void CUITool_2011View::OnDraw(CDC* /*pDC*/)
{
	CUITool_2011Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}

void CUITool_2011View::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CUITool_2011View::OnContextMenu(CWnd* pWnd, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}


// CUITool_2011View 진단

#ifdef _DEBUG
void CUITool_2011View::AssertValid() const
{
	CView::AssertValid();
}

void CUITool_2011View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CUITool_2011Doc* CUITool_2011View::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CUITool_2011Doc)));
	return (CUITool_2011Doc*)m_pDocument;
}
#endif //_DEBUG


// CUITool_2011View 메시지 처리기

void CUITool_2011View::OnPaint()
{
	//CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CView::OnPaint()을(를) 호출하지 마십시오.
	
	CBufferDC dc(this);
	CRect Rect;
	GetClientRect(&Rect);
	dc.FillSolidRect(&Rect, RGB(125, 125, 125));
		
	GetDocument()->Render(&dc);	
}

void CUITool_2011View::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_StartMousePoint = point;

	SetCursor(m_hCurrentCursor);

	if(m_eMouseState == EMS_MOUSEON)
	{
		m_eMouseState = EMS_CLICK;
		
		if(m_MouseOnObj != NULL && m_MouseOnObj->m_ObjState == EOS_MOUSEON)
			GetDocument()->CurrentPickUpObjUpdate(m_MouseOnObj);
	}
	
	if(GetDocument()->m_bTrack)
	{
		m_eMouseState = EMS_CREATEDRAG;
		GetDocument()->m_bTrack = false;
	}
	if(m_eMousePosition != E_NTNT)
	{
		m_eMouseState = EMS_RESIZE;
	}

	CView::OnLButtonDown(nFlags, point);
}

void CUITool_2011View::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	SetCursor(m_hCurrentCursor);

	if(m_eMouseState == EMS_NORMAL || m_eMouseState == EMS_MOUSEON)
	{
		int size = GetDocument()->m_ObjectList.size();

		//for(std::list<CUIToolBaseObject*>::iterator iter = GetDocument()->m_ObjectList.begin(); iter != GetDocument()->m_ObjectList.end(); iter++)
		while(size > 0)
		{
			std::list<CUIToolBaseObject*>::iterator iter = GetDocument()->m_ObjectList.begin();
			for(int i = 1; i < size; i++)
			{
				iter++;
			}
			size--;

			m_MouseOnObj = (*iter)->AllItemCheckPos(point);

			if(m_MouseOnObj != NULL)
			{
				m_eMouseState = EMS_MOUSEON;
				break;
			}

			if((*iter)->IsMouseOn(point))
			{
				(*iter)->m_ObjState = EOS_MOUSEON;
				m_MouseOnObj = (*iter);
				m_eMouseState = EMS_MOUSEON;
				break;
			}
			else
				(*iter)->m_ObjState = EOS_DEFAULT;			
		}

		SetMouseCursor(point);
	}
	else if(m_eMouseState == EMS_CLICK)
	{
		if(m_MouseOnObj != NULL)
		{
			m_MouseOnObj->MovePosition(point.x - m_StartMousePoint.x, point.y - m_StartMousePoint.y);
			m_StartMousePoint = point;
		}
	}

	Invalidate(TRUE);
	CView::OnMouseMove(nFlags, point);
}

void CUITool_2011View::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_EndMousePoint = point;

	if(m_eMouseState == EMS_CLICK)
	{
		//m_eMouseState == EMS_NORMAL;
	}
	else if(m_eMouseState == EMS_CREATEDRAG)
	{
		GetDocument()->AddObject(m_StartMousePoint, m_EndMousePoint);
	}
	else if(m_eMouseState == EMS_RESIZE)
	{
		if(m_eMousePosition == E_PTPT)
		{
			GetDocument()->GetPickUpObject()->m_Position.x += point.x - m_StartMousePoint.x;
			GetDocument()->GetPickUpObject()->m_Position.y += point.y - m_StartMousePoint.y;

			GetDocument()->GetPickUpObject()->m_Size.x -= point.x - m_StartMousePoint.x;
			GetDocument()->GetPickUpObject()->m_Size.y -= point.y - m_StartMousePoint.y;
		}
		else if(m_eMousePosition == E_STPT)
		{
			GetDocument()->GetPickUpObject()->m_Position.y += point.y - m_StartMousePoint.y;
			GetDocument()->GetPickUpObject()->m_Size.x += point.x - m_StartMousePoint.x;
			GetDocument()->GetPickUpObject()->m_Size.y -= point.y - m_StartMousePoint.y;
		}
		else if(m_eMousePosition == E_PTST)
		{
			GetDocument()->GetPickUpObject()->m_Position.x += point.x - m_StartMousePoint.x;
			GetDocument()->GetPickUpObject()->m_Size.x -= point.x - m_StartMousePoint.x;
			GetDocument()->GetPickUpObject()->m_Size.y += point.y - m_StartMousePoint.y;
		}
		else if(m_eMousePosition == E_STST)
		{
			GetDocument()->GetPickUpObject()->m_Size.x += point.x - m_StartMousePoint.x;
			GetDocument()->GetPickUpObject()->m_Size.y += point.y - m_StartMousePoint.y;
		}

		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		m_eMousePosition = E_NTNT;
	}

	m_eMouseState = EMS_NORMAL;
	Invalidate(TRUE);

	CView::OnLButtonUp(nFlags, point);

}

BOOL CUITool_2011View::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	return TRUE;//CView::OnEraseBkgnd(pDC);
}
void CUITool_2011View::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	switch(nChar)
	{
	case VK_LEFT:
		GetDocument()->GetPickUpObject()->m_Position.x -= 1;
		break;
	case VK_RIGHT:
		GetDocument()->GetPickUpObject()->m_Position.x += 1;
		break;
	case VK_UP:
		GetDocument()->GetPickUpObject()->m_Position.y -= 1;
		break;
	case VK_DOWN:
		GetDocument()->GetPickUpObject()->m_Position.y += 1;
		break;
	case VK_SPACE:
		{
			CEdit* m_Edit = new CEdit;
			m_Edit->Create(WS_CHILD | WS_VISIBLE, CRect(0,0,100,100), this, 1233);
		}
		break;
	}
	Invalidate();

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CUITool_2011View::SetMouseCursor(CPoint point)
{
	BOOL bCursor = FALSE;

	if(!GetDocument()->GetPickUpObject())
		return bCursor;

	m_eMousePosition = GetDocument()->GetPickUpObject()->IsMouseSideOn(point);
	if(m_eMousePosition == E_PTPT || m_eMousePosition == E_STST)
	{
		bCursor = TRUE;
		m_hCurrentCursor = AfxGetApp()->LoadStandardCursor(IDC_SIZENWSE); 
		//AfxMessageBox(_T("IDC_SIZENWSE"));
	}
	else if(m_eMousePosition == E_PTST || m_eMousePosition == E_STPT)
	{
		bCursor = TRUE;
		m_hCurrentCursor = AfxGetApp()->LoadStandardCursor(IDC_SIZENESW); 
		//AfxMessageBox(_T("IDC_SIZENESW"));
	}
	else
	{
		bCursor = FALSE;
		m_hCurrentCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW); 
	}

	return bCursor;
}

int CUITool_2011View::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
 	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	return 0;
}
