// UIToolBaseObject.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "UITool_2011.h"
#include "UIToolBaseObject.h"
#include "MainFrm.h"
#include "UITool_2011Doc.h"
#include "UITool_2011View.h"
#include "DerivedPropertyGridFontProperty.h"


// CUIToolBaseObject

CUIToolBaseObject::CUIToolBaseObject()
:m_parent(NULL), m_bVisible(true), m_Color(RGB(0, 0, 0)), m_ObjState(EOS_DEFAULT), m_eMousepos(E_NTNT),
m_DefaultImg(_T("")), m_MouseOnImg(_T("")), m_ClickImg(_T("")), m_DisableImg(_T("")), m_PositionType(_T("Absolute")),
m_bUserInput(true), m_bMovalbe(true), m_bModal(false), m_strToolTip(_T("")), m_strCaption(_T("")), m_pCwndObj(NULL)
{
	LOGFONT lf;
	CFont* FONT = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	FONT->GetLogFont(&lf);
	lstrcpy(lf.lfFaceName, _T("맑은 고딕"));
	m_logfont = lf;

	m_FontColor = RGB(0, 0, 0);
}

CUIToolBaseObject::~CUIToolBaseObject()
{
}

void CUIToolBaseObject::Create(CString pID, CPoint pStart, CPoint pEnd)
{
	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	if(m_parent != NULL && m_PositionType.Compare(_T("Absolute")) == 0)
	{
		m_Absolute_Position.x = pStart.x - m_parent->m_Position.x;
		m_Absolute_Position.y = pStart.y - m_parent->m_Position.y;
	}
	else
	{
		m_Absolute_Position = CPoint(0, 0);
	}

	m_Position = CPoint(pStart);
	m_Size = CPoint(pEnd.x - pStart.x, pEnd.y - pStart.y);

	CMainFrame* mFrm = (CMainFrame*)AfxGetMainWnd();

	m_ID = pID;

	HTREEITEM hItem = mFrm->m_wndFileView.GetTreeItemFromItemText(m_ID, NULL, TRUE);
	if(hItem == NULL)
	{
		if(mFrm->m_wndFileView.AddFile(this))
		{			
			CUITool_2011Doc* pDoc = (CUITool_2011Doc*)mFrm->GetActiveDocument();

			if(m_parent != NULL)
			{
				m_parent->m_ChildList.push_back(this);
			}
			else
			{
				pDoc->m_ObjectList.push_back(this);
			}

			pDoc->CurrentPickUpObjUpdate(this);
		}
	}
	else
	{
		AfxMessageBox(_T("같은 이름이 존재합니다."));
	}
}
void CUIToolBaseObject::Render(CBufferDC* pDC)
{
	if(!m_bVisible)
		return;

	Graphics gdi(pDC->m_hDC); // gdi 객체 선언

	CString str_img = m_DefaultImg;

	if(m_ObjState == EOS_MOUSEON && m_MouseOnImg != "")
		str_img = m_MouseOnImg;
	else if(m_ObjState == EOS_CLICK && m_ClickImg != "")
		str_img = m_ClickImg;
	else if(m_ObjState == EOS_DISABLE && m_DisableImg != "")
		str_img = m_DisableImg;

	SetImage(str_img);

	if(str_img != "")
	{	
		Image img(str_img);
		gdi.DrawImage(&img, m_Position.x, m_Position.y, m_Size.x, m_Size.y);
	}
	else
	{
		int bitmask_red=0x0000ff;  //255
		int bitmask_green=0x00ff00; //65280
		int bitmask_blue=0xff0000;  //16711680

		int red =  m_Color & bitmask_red;
		int green = (m_Color & bitmask_green)>>8;
		int blue = (m_Color & bitmask_blue)>>16;

		Color PenColor = Color(255, red, green, blue);
		Pen BlackPen(PenColor, 2.0f);

		gdi.DrawRectangle(&BlackPen, m_Position.x, m_Position.y, m_Size.x, m_Size.y);

	}

	/*if(m_strCaption.Compare(_T("")) != 0)
	{
		CFont font;
		font.CreateFontIndirect(&m_logfont);

		pDC->SelectObject(font);
		pDC->SetBkMode(TRANSPARENT);

		pDC->SetTextColor(m_FontColor);
		pDC->DrawText(m_strCaption, CRect(m_Position.x, m_Position.y, m_Position.x + m_Size.x, m_Position.y + m_Size.y), DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	}*/

	for(std::list<CUIToolBaseObject*>::iterator iter = m_ChildList.begin(); iter != m_ChildList.end(); iter++)
	{
		(*iter)->Render(pDC);
	}
}

BOOL CUIToolBaseObject::IsMouseOn(CPoint pPos)
{
	if( pPos.x >= m_Position.x && pPos.x <= m_Position.x + m_Size.x)
	{
		if(pPos.y >= m_Position.y && pPos.y <= m_Position.y + m_Size.y)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}

E_MousePosition CUIToolBaseObject::IsMouseSideOn(CPoint pPos)
{
	CPoint ept;
	ept.x = m_Position.x + m_Size.x;
	ept.y = m_Position.y + m_Size.y;

	if(pPos.x >= m_Position.x - 2 && pPos.x <= m_Position.x + 2 && pPos.y >= m_Position.y -2 && pPos.y <= m_Position.y + 2)
	{
		m_eMousepos = E_PTPT;
	}
	else if(pPos.x >= ept.x - 2 && pPos.x <= ept.x + 2 && pPos.y >= ept.y - 2 && pPos.y <= ept.y + 2)
	{
		m_eMousepos = E_STST;
	}
	else if(pPos.x >= ept.x - 2 && pPos.x <= ept.x + 2 && pPos.y >= m_Position.y - 2 && pPos.y <= m_Position.y + 2)
	{
		m_eMousepos = E_STPT;

	}
	else if(pPos.x >= m_Position.x -2 && pPos.x <= m_Position.x + 2 && pPos.y >= ept.y -2 && pPos.y <= ept.y + 2)
	{
		m_eMousepos = E_PTST;
	}
	else
	{
		m_eMousepos = E_NTNT;
	}

	return m_eMousepos;
}



void CUIToolBaseObject::SetID(CString NewID)
{
	if(NewID.Compare(_T("")) == 0)
	{
		AfxMessageBox(_T("오브젝트명을 다시 입력해요."));
		return;
	}

	m_ID = NewID;

	CMainFrame* mFrm = (CMainFrame*)AfxGetMainWnd();
	mFrm->m_wndFileView.SetSelectItemText(NewID);
}
void CUIToolBaseObject::SetParentID(CString ParentID)
{
	if(ParentID.Compare(_T("")) == 0)
	{
		AfxMessageBox(_T("오브젝트명을 다시 입력해요."));
		return;
	}

	CUITool_2011Doc* pDoc = (CUITool_2011Doc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();

	CUIToolBaseObject* NewParent = pDoc->GetItemInTheList(ParentID);
	if(NewParent != NULL)
	{	
		if(m_parent == NULL)
		{
			m_parent = NewParent;
			NewParent->m_ChildList.push_back(this);
		}
		else
		{
			for(std::list<CUIToolBaseObject*>::iterator iter = m_parent->m_ChildList.begin(); iter != m_parent->m_ChildList.end(); )
			{
				if((*iter) == this)
				{
					m_parent->m_ChildList.remove(*iter++);
					m_parent = NewParent;
					NewParent->m_ChildList.push_back(this);
					break;
				}
				else
					iter++;
			}
		}
		
	}

}
void CUIToolBaseObject::PropertyChange(CString pStr, CMFCPropertyGridProperty* pProp)
{
	if(pStr.Compare(TEXT("Caption")) == 0)
	{
		SetCaption(_variant_t(pProp->GetValue()));
	}
	else if(pStr.Compare(TEXT("글꼴")) == 0)
	{
		SetFontType(_variant_t(pProp->GetValue()));
	}
	else if(pStr.Compare(TEXT("텍스트색상")) == 0)
	{
		m_FontColor = _variant_t(pProp->GetValue());
		if(m_pCwndObj)
			SetFontColor(m_FontColor);
	}
	else if(pStr.Compare(TEXT("Parent")) == 0)
	{
		SetParentID(pProp->GetValue());
	}
	else if(pStr.Compare(TEXT("ID")) == 0)
	{
		SetID(pProp->GetValue());
	}
	else if(pStr.Compare(TEXT("Visible")) == 0)
	{
		SetEnableWindow(_variant_t(pProp->GetValue()));
	}
	else if(pStr.Compare(TEXT("좌표")) == 0)
	{
		m_PositionType = _variant_t(pProp->GetValue());
	}
	else if(pStr.Compare(TEXT("X")) == 0)
	{	
		if(m_PositionType.Compare(_T("Absolute")) == 0)
		{
			m_Absolute_Position.x = _variant_t(pProp->GetValue());
			m_Position.x = m_parent->m_Position.x + m_Absolute_Position.x;
		}
		else
			m_Position.x = _variant_t(pProp->GetValue());
	}
	else if(pStr.Compare(TEXT("Y")) == 0)
	{
		if(m_PositionType.Compare(_T("Absolute")) == 0)
		{
			m_Absolute_Position.y = _variant_t(pProp->GetValue());
			m_Position.y = m_parent->m_Position.y + m_Absolute_Position.y;
		}
		else
			m_Position.y = _variant_t(pProp->GetValue());
	}
	else if(pStr.Compare(TEXT("높이")) == 0)
	{
		m_Size.x = _variant_t(pProp->GetValue());
	}
	else if(pStr.Compare(TEXT("너비")) == 0)
	{
		m_Size.y = _variant_t(pProp->GetValue());
	}
	else if(pStr.Compare(TEXT("색상")) == 0)
	{
		m_Color = _variant_t(pProp->GetValue());
	}
	else if(pStr.Compare(TEXT("Default")) == 0)
	{
		m_DefaultImg = GetFilePath( _variant_t(pProp->GetValue()) );
	}
	else if(pStr.Compare(TEXT("Mouse On")) == 0)
	{
		m_MouseOnImg = GetFilePath( _variant_t(pProp->GetValue()) );
	}
	else if(pStr.Compare(TEXT("Click")) == 0)
	{
		m_ClickImg = GetFilePath( _variant_t(pProp->GetValue()) );
	}
	else if(pStr.Compare(TEXT("Disable")) == 0)
	{
		m_DisableImg = GetFilePath( _variant_t(pProp->GetValue()) );
	}
	else if(pStr.Compare(TEXT("UserInput")) == 0)
	{
		m_bUserInput = _variant_t(pProp->GetValue());
	}
	else if(pStr.Compare(TEXT("Movable")) == 0)
	{
		m_bMovalbe = _variant_t(pProp->GetValue());
	}
	else if(pStr.Compare(TEXT("Modal")) == 0)
	{
		m_bModal = _variant_t(pProp->GetValue());
	}
	else if(pStr.Compare(TEXT("ToolTip")) == 0)
	{
		m_strToolTip = _variant_t(pProp->GetValue());
	}
}

void CUIToolBaseObject::FindPropertyValueChange()
{
	CMFCPropertyGridCtrl* pGrid = ((CMainFrame*)AfxGetMainWnd())->m_wndProperties.GetPropertyList();

	if(!pGrid)
		return;

	CMFCPropertyGridProperty* pProp = (CMFCPropertyGridProperty*)pGrid->FindItemByData(12);
	if(pProp != NULL)
	{
		if(m_parent == NULL)
			pProp->SetValue(_T(""));
		else
			pProp->SetValue(_variant_t(m_parent->m_ID));
	}

	pProp = (CMFCPropertyGridProperty*)pGrid->FindItemByData(22);
	if(pProp != NULL)
		pProp->SetValue(_variant_t(m_ID));
	pProp = (CMFCPropertyGridProperty*)pGrid->FindItemByData(23);
	if(pProp != NULL)
		pProp->SetValue(_variant_t(m_bVisible));
	pProp = (CMFCPropertyGridProperty*)pGrid->FindItemByData(24);
	if(pProp != NULL)
		pProp->SetValue(_variant_t(m_bUserInput));
	pProp = (CMFCPropertyGridProperty*)pGrid->FindItemByData(25);
	if(pProp != NULL)
		pProp->SetValue(_variant_t(m_bMovalbe));
	pProp = (CMFCPropertyGridProperty*)pGrid->FindItemByData(26);
	if(pProp != NULL)
		pProp->SetValue(_variant_t(m_bModal));
	pProp = (CMFCPropertyGridProperty*)pGrid->FindItemByData(27);
	if(pProp != NULL)
		pProp->SetValue(_variant_t(m_strToolTip));

	pProp = (CMFCPropertyGridProperty*)pGrid->FindItemByData(32);
	if(pProp != NULL)
		pProp->SetValue(_variant_t(m_PositionType));
	pProp = (CMFCPropertyGridProperty*)pGrid->FindItemByData(33);
	if(pProp != NULL)
	{
		if(m_PositionType.Compare(_T("Absolute")) == 0)
		{
			pProp->SetValue(_variant_t(m_Absolute_Position.x));
		}
		else
			pProp->SetValue(_variant_t(m_Position.x));
	}
	pProp = (CMFCPropertyGridProperty*)pGrid->FindItemByData(34);
	if(pProp != NULL)
	{
		if(m_PositionType.Compare(_T("Absolute")) == 0)
		{
			pProp->SetValue(_variant_t(m_Absolute_Position.y));
		}
		else
			pProp->SetValue(_variant_t(m_Position.y));
	}
	pProp = (CMFCPropertyGridProperty*)pGrid->FindItemByData(35);
	if(pProp != NULL)
	{
		pProp->SetValue(_variant_t(m_Size.x));
	}
	pProp = (CMFCPropertyGridProperty*)pGrid->FindItemByData(36);
	if(pProp != NULL)
	{
		pProp->SetValue(_variant_t(m_Size.y));
	}

	CMFCPropertyGridColorProperty* pColor = (CMFCPropertyGridColorProperty*)pGrid->FindItemByData(42);
	if(pColor != NULL)
		pColor->SetColor( m_Color);

	CMFCPropertyGridFileProperty* pFile = (CMFCPropertyGridFileProperty*)pGrid->FindItemByData(43);
	if(pFile != NULL)
		pFile->SetValue(_variant_t(m_DefaultImg));

	pFile = (CMFCPropertyGridFileProperty*)pGrid->FindItemByData(44);
	if(pFile != NULL)
		pFile->SetValue(_variant_t(m_MouseOnImg));

	pFile = (CMFCPropertyGridFileProperty*)pGrid->FindItemByData(45);
	if(pFile != NULL)
		pFile->SetValue(_variant_t(m_ClickImg));

	pFile = (CMFCPropertyGridFileProperty*)pGrid->FindItemByData(46);
	if(pFile != NULL)
		pFile->SetValue(_variant_t(m_DisableImg));

	pProp = (CMFCPropertyGridProperty*)pGrid->FindItemByData(71);
	if(pProp != NULL)
	{
		pProp->SetValue((_variant_t)m_strCaption);
	}
	pProp = (CMFCPropertyGridProperty*)pGrid->FindItemByData(72);
	if(pProp != NULL)
	{
		pProp->SetValue((_variant_t)m_strFontType);
	}
	pColor = (CMFCPropertyGridColorProperty*)pGrid->FindItemByData(73);
	if(pColor != NULL)
	{
		pColor->SetColor(m_FontColor);
	}
}

CUIToolBaseObject* CUIToolBaseObject::GetItemInTheChildList(CString pItemID)
{
	for(std::list<CUIToolBaseObject*>::iterator iter = m_ChildList.begin(); iter != m_ChildList.end(); iter++)
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

CUIToolBaseObject* CUIToolBaseObject::AllItemCheckPos(CPoint pPoint)
{
	CUIToolBaseObject* retrunObj = NULL;

	for(std::list<CUIToolBaseObject*>::iterator iter = m_ChildList.begin(); iter != m_ChildList.end(); iter++)
	{
		if((*iter)->IsMouseOn(pPoint))
		{
			(*iter)->m_ObjState = EOS_MOUSEON;
			retrunObj = (*iter);
		}
		else
			(*iter)->m_ObjState = EOS_DEFAULT;

		CUIToolBaseObject* tmp = (*iter)->AllItemCheckPos(pPoint);
		if(tmp != NULL)
			retrunObj = tmp;
	}

	return retrunObj;
}

// CUIToolBaseObject 메시지 처리기입니다.
void CUIToolBaseObject::MovePosition(long x, long y)
{
	if(!m_bVisible)
		return;

	m_Position.x += x;
	m_Position.y += y;

	if(m_parent != NULL)
	{
		m_Absolute_Position.x = m_Position.x - m_parent->m_Position.x;
		m_Absolute_Position.y = m_Position.y - m_parent->m_Position.y;
	}

	if(m_pCwndObj != NULL)
	{
		m_pCwndObj->MoveWindow(m_Position.x, m_Position.y, m_Size.x, m_Size.y);
	}

	for(std::list<CUIToolBaseObject*>::iterator iter = m_ChildList.begin(); iter != m_ChildList.end(); iter++)
	{
		if((*iter)->m_PositionType.Compare(_T("Absolute")) == 0)
			(*iter)->MovePosition(x, y);
	}
}

CString CUIToolBaseObject::GetFilePath(CString pStr)
{
	CString CopyStr = pStr, ResultStr = _T("");
	int count = 0, index = 0;
	int size = pStr.GetLength();

	while(index >= 0)
	{
		index = CopyStr.Find('\\', 0);
		if(index  >= 0)
			ResultStr += CopyStr.Left(index+1) + _T('\\');
		else
			ResultStr += CopyStr;

		size -= index+1;

		CopyStr = CopyStr.Right(size);
		count++;
	}
	

	return ResultStr;
}

void CUIToolBaseObject::SetEnableWindow(bool p_bVisible)
{
	m_bVisible = p_bVisible;
	if(m_pCwndObj)
	{
		if(!m_bVisible)
		{
			SetImage(m_DisableImg);
		}
		else
		{
			SetImage(m_DefaultImg);
		}

		m_pCwndObj->EnableWindow(m_bVisible);
	}
}

void CUIToolBaseObject::SetCaption(CString p_strCaption)
{
	m_strCaption = p_strCaption;
	if(m_pCwndObj)
	{
		m_pCwndObj->SetWindowText(p_strCaption);
	}
}
void CUIToolBaseObject::SetFontType(CString p_strFontType)
{
	m_strFontType = p_strFontType;
	if(m_pCwndObj)
	{
		static CFont font;
		LOGFONT lf;
		if(m_pCwndObj->GetFont())
			m_pCwndObj->GetFont()->GetLogFont(&lf);
		lstrcpy(lf.lfFaceName, m_strFontType);
		font.CreateFontIndirect(&lf);

		m_pCwndObj->SetFont(&font);
	}
}