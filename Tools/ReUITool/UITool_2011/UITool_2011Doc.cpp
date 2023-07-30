
// UITool_2011Doc.cpp : CUITool_2011Doc 클래스의 구현
//

#include "stdafx.h"
#include "UITool_2011.h"

#include "UITool_2011Doc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CUITool_2011Doc

IMPLEMENT_DYNCREATE(CUITool_2011Doc, CDocument)

BEGIN_MESSAGE_MAP(CUITool_2011Doc, CDocument)
END_MESSAGE_MAP()


// CUITool_2011Doc 생성/소멸

CUITool_2011Doc::CUITool_2011Doc()
: m_bTrack(FALSE), m_CurrentPickObj(NULL)
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.

}

CUITool_2011Doc::~CUITool_2011Doc()
{
}

BOOL CUITool_2011Doc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}




// CUITool_2011Doc serialization

void CUITool_2011Doc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}


// CUITool_2011Doc 진단

#ifdef _DEBUG
void CUITool_2011Doc::AssertValid() const
{
	CDocument::AssertValid();
}

void CUITool_2011Doc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CUITool_2011Doc 명령

void CUITool_2011Doc::AddObject(CPoint pStartpos, CPoint pEndpos)
{
	CMainFrame* MainFrm = (CMainFrame*)(AfxGetApp()->GetMainWnd());
	MainFrm->m_wndControlMenuView.m_CtrlMenu.SetCreate(pStartpos, pEndpos);
}

void CUITool_2011Doc::Render(CBufferDC* pDC)
{
	if(m_CurrentPickObj != NULL)
	{
		m_CurrentPickObj->FindPropertyValueChange();

		Border.InitBorder(m_CurrentPickObj->m_Position, m_CurrentPickObj->m_Size);
		Border.Render(pDC);
	}
	
	for(std::list<CUIToolBaseObject*>::iterator iter = m_ObjectList.begin(); iter != m_ObjectList.end(); iter++)
	{
		(*iter)->Render(pDC);
	}
}
void CUITool_2011Doc::CurrentPickUpObjUpdate(CUIToolBaseObject* pObj, BOOL bFileViewUpdate)
{
	if(pObj == NULL)
	{
		if(m_CurrentPickObj != NULL)
			m_CurrentPickObj->RemoveProperties();
		m_CurrentPickObj = NULL;
		return;
	}

	if(m_CurrentPickObj != pObj)
	{
		if(m_CurrentPickObj != NULL)
		{
			m_CurrentPickObj->RemoveProperties();
		}
		pObj->InitProperty();

		m_CurrentPickObj = pObj;
	}

	m_CurrentPickObj->m_ObjState = EOS_CLICK;

	if(bFileViewUpdate)
	{
		((CMainFrame*)AfxGetMainWnd())->m_wndFileView.SetSelectObj_Item(pObj);
	}

	((CMainFrame*)AfxGetMainWnd())->GetActiveWindow()->Invalidate(TRUE);
}

CUIToolBaseObject* CUITool_2011Doc::GetItemInTheList(CString pItemID)
{
	for(std::list<CUIToolBaseObject*>::iterator iter = m_ObjectList.begin(); iter != m_ObjectList.end(); iter++)
	{
		if(pItemID.Compare((*iter)->m_ID) == 0)
		{
			return (*iter);
		}
		else
		{
			CUIToolBaseObject* pObj = (*iter)->GetItemInTheChildList(pItemID);
			if(pObj != NULL)
				return pObj;
		}
	}
	
	return NULL;
}
