#include "StdAfx.h"
#include "UIToolStatic.h"
#include "MainFrm.h"
#include "DerivedPropertyGridFontProperty.h"
#include "UITool_2011View.h"

CUIToolStatic::CUIToolStatic(void)
:m_strRowArray(_T("Center")), m_strColumnArray(_T("Center")), m_bNumberComma(false), m_bImgNumber(false), m_bShadow(false), m_Static(NULL)
{
}

CUIToolStatic::~CUIToolStatic(void)
{
}

void CUIToolStatic::InitProperty()
{
	CMFCPropertyGridCtrl* pGrid = ((CMainFrame*)AfxGetMainWnd())->m_wndProperties.GetPropertyList();

	if(!pGrid)
		return;

	CMFCPropertyGridProperty* pGroup = new CMFCPropertyGridProperty(_T("Ư��"));
	pGroup->SetData(200);

	CMFCPropertyGridProperty* pSub = new CMFCPropertyGridProperty(_T("��������"),_T("Center"), _T("Left, Center, Right �� �ϳ��� �����մϴ�."));
	pSub->AddOption(_T("Left"));
	pSub->AddOption(_T("Center"));
	pSub->AddOption(_T("Right"));
	pSub->AllowEdit(FALSE);
	pSub->SetData(203);
	pGroup->AddSubItem(pSub);

	pSub = new CMFCPropertyGridProperty(_T("��������"),_T("Center"), _T("Top, Center, Bottom �� �ϳ��� �����մϴ�."));
	pSub->AddOption(_T("Top"));
	pSub->AddOption(_T("Center"));
	pSub->AddOption(_T("Bottom"));
	pSub->AllowEdit(FALSE);
	pSub->SetData(204);
	pGroup->AddSubItem(pSub);

	pSub = new CMFCPropertyGridProperty(_T("õ����','ǥ��"), (_variant_t) m_bNumberComma, _T("õ ���� ǥ�� ��/���� �����մϴ�."));
	pSub->SetData(205);
	pGroup->AddSubItem(pSub);

	pSub = new CMFCPropertyGridProperty(_T("�׸���ȿ��"), (_variant_t) m_bShadow, _T("�ؽ�Ʈ�� �׸���ȿ���� ǥ�� ��/���� �����մϴ�."));
	pSub->SetData(206);
	pGroup->AddSubItem(pSub);

	pSub = new CMFCPropertyGridProperty(_T("�̹�����Ʈ"), (_variant_t) m_bImgNumber, _T("���ڸ� �̹�����Ʈ�� ����� ǥ���� ���� �����մϴ�."));
	pSub->SetData(208);
	pGroup->AddSubItem(pSub);

	pGrid->AddProperty(pGroup);

	pSub = (CMFCPropertyGridProperty*)pGrid->FindItemByData(71);
	if(pSub != NULL)
		pSub->Enable(TRUE);
	pSub = (CMFCPropertyGridProperty*)pGrid->FindItemByData(72);
	if(pSub != NULL)
		pSub->Enable(TRUE);
	CMFCPropertyGridColorProperty* pColor = (CMFCPropertyGridColorProperty*)pGrid->FindItemByData(73);
	if(pColor != NULL)
		pColor->Enable(TRUE);
	pGroup = (CMFCPropertyGridProperty*)pGrid->FindItemByData(7);
	if(pGroup != NULL)
		pGroup->Expand(TRUE);
}
void CUIToolStatic::RemoveProperties()
{
	CMFCPropertyGridCtrl* pGrid = ((CMainFrame*)AfxGetMainWnd())->m_wndProperties.GetPropertyList();

	CMFCPropertyGridProperty* pGroup = (CMFCPropertyGridProperty*)pGrid->FindItemByData(200);

	if(!pGrid || !pGroup)
		return;

	//CMFCPropertyGridProperty* pSub;
	/*for(int i = 203; i < 210; i++)
	{
		pSub = (CMFCPropertyGridProperty*)pGrid->FindItemByData(i);
		if(pSub != NULL)
			pGroup->RemoveSubItem(pSub);
	}*/

	if(pGroup != NULL)
		pGrid->DeleteProperty(pGroup);

	CMFCPropertyGridProperty* pSub = (CMFCPropertyGridProperty*)pGrid->FindItemByData(71);
	if(pSub != NULL)
		pSub->Enable(FALSE);
	pSub = (CMFCPropertyGridProperty*)pGrid->FindItemByData(72);
	if(pSub != NULL)
		pSub->Enable(FALSE);
	pSub = (CMFCPropertyGridProperty*)pGrid->FindItemByData(73);
	if(pSub != NULL)
		pSub->Enable(FALSE);

	pGroup = (CMFCPropertyGridProperty*)pGrid->FindItemByData(7);
	if(pGroup != NULL)
		pGroup->Expand(FALSE);
}

void CUIToolStatic::FindPropertyValueChange()
{
	CUIToolBaseObject::FindPropertyValueChange();

	CMFCPropertyGridCtrl* pGrid = ((CMainFrame*)AfxGetMainWnd())->m_wndProperties.GetPropertyList();

	if(!pGrid)
		return;

	CMFCPropertyGridProperty* pProp = (CMFCPropertyGridProperty*)pGrid->FindItemByData(203);
	if(pProp != NULL)
	{
		//LPCTSTR str = pFile->GetOption(m_strRowArray);
		pProp->SetValue(m_strRowArray);
	}

	pProp = (CMFCPropertyGridProperty*)pGrid->FindItemByData(204);
	if(pProp != NULL)
	{
// 		LPCTSTR str = pFile->GetOption(m_strColumnArray);
// 		if(strcmp((char*)str, "Left")== 0)
// 			str = _T("Top");
// 		else if(strcmp((char*)str, "Right") == 0)
// 			str = _T("Bottom");
		pProp->SetValue(m_strColumnArray);
	}

	pProp = (CMFCPropertyGridProperty*)pGrid->FindItemByData(205);
	if(pProp != NULL)
		pProp->SetValue(_variant_t(m_bNumberComma));

	pProp = (CMFCPropertyGridProperty*)pGrid->FindItemByData(206);
	if(pProp != NULL)
		pProp->SetValue(_variant_t(m_bShadow));

	pProp = (CMFCPropertyGridProperty*)pGrid->FindItemByData(208);
	if(pProp != NULL)
		pProp->SetValue(_variant_t(m_bImgNumber));

	if(m_bShadow)
	{
		CMFCPropertyGridColorProperty* pColor = (CMFCPropertyGridColorProperty*)pGrid->FindItemByData(207);
		if(pColor != NULL)
			pColor->SetColor(m_BKFontColor);
	}

	if(m_bImgNumber)
	{
		CMFCPropertyGridFileProperty* pFile = (CMFCPropertyGridFileProperty*)pGrid->FindItemByData(209);
		if(pFile != NULL)
			pFile->SetValue(_variant_t(m_ImgFontTexture));
	}
}

void CUIToolStatic::PropertyChange(CString pStr, CMFCPropertyGridProperty* pProp)
{
	CUIToolBaseObject::PropertyChange(pStr, pProp);

	if(pStr.Compare(TEXT("��������")) == 0)
	{
		m_strRowArray = _variant_t(pProp->GetValue());
	}
	else if(pStr.Compare(TEXT("��������")) == 0)
	{
		m_strColumnArray = _variant_t(pProp->GetValue());
	}
	else if(pStr.Compare(TEXT("õ����','ǥ��")) == 0)
	{
		m_bNumberComma = _variant_t(pProp->GetValue());
	}
	else if(pStr.Compare(TEXT("�׸���ȿ��")) == 0)
	{
		m_bShadow = _variant_t(pProp->GetValue());
		
		CMFCPropertyGridCtrl* pGrid = ((CMainFrame*)AfxGetMainWnd())->m_wndProperties.GetPropertyList();
		
		if(m_bShadow)
		{
			CMFCPropertyGridProperty* pGroup4 = new CMFCPropertyGridProperty(_T("�׸��� �ؽ�Ʈ����"));
			pGroup4->SetData(207);

			COLORREF hColor = RGB(255, 255, 255);//0xffff0000;

			CMFCPropertyGridColorProperty* pColor = new CMFCPropertyGridColorProperty(_T("�׸��ڻ���"), hColor, NULL, _T("�׸��� ������ �����մϴ�."));
			pColor->EnableOtherButton(_T("��Ÿ..."));	// "��Ÿ..." ��ư�� Ȱ��ȭ ��Ŵ
			pColor->AllowEdit(TRUE);
			pGroup4->AddSubItem(pColor);

			pGrid->AddProperty(pGroup4);
		}
		else
		{
			CMFCPropertyGridProperty* pGroup = (CMFCPropertyGridProperty*)pGrid->FindItemByData(207);
			if(pGroup != NULL)
				pGrid->DeleteProperty(pGroup);

			m_BKFontColor = RGB(255, 255, 255);//0xffff0000;
		}
	}
	else if(pStr.Compare(TEXT("�׸��ڻ���")) == 0)
	{
		m_BKFontColor = _variant_t(pProp->GetValue());
	}
	else if(pStr.Compare(TEXT("�̹�����Ʈ")) == 0)
	{
		m_bImgNumber = _variant_t(pProp->GetValue());
		
		CMFCPropertyGridCtrl* pGrid = ((CMainFrame*)AfxGetMainWnd())->m_wndProperties.GetPropertyList();

		if(m_bImgNumber)
		{
			CMFCPropertyGridProperty* pGroup4 = new CMFCPropertyGridProperty(_T("���� �ؽ���"));
			pGroup4->SetData(209);
			static TCHAR BASED_CODE sz_Filter[] = _T("�̹��� ����(*.jpg)|*.jpg|�̹��� ����(*.bmp)|*.bmp|��� ����(*.*)|*.*||");
			CMFCPropertyGridFileProperty* pFileSelect = new CMFCPropertyGridFileProperty(_T("�̹�����Ʈ �ؽ���"), TRUE, _T(""), 0, 0, sz_Filter, _T("���� �̹����� �����մϴ�."));
			//pFileSelect->SetData(301);
			pGroup4->AddSubItem(pFileSelect);
			
			pGrid->AddProperty(pGroup4);
		}
		else
		{
			CMFCPropertyGridProperty* pGroup = (CMFCPropertyGridProperty*)pGrid->FindItemByData(209);
			if(pGroup != NULL)
				pGrid->DeleteProperty(pGroup);

			m_ImgFontTexture = _T("");
		}
	}
	else if(pStr.Compare(TEXT("�̹�����Ʈ �ؽ���")) == 0)
	{
		m_ImgFontTexture = _variant_t(pProp->GetValue());
	}
}

void CUIToolStatic::Render(CBufferDC* pDC)
{
	//if(!m_bVisible)
	//	return;

	Graphics gdi(pDC->m_hDC); // gdi ��ü ����

	CString str_img = m_DefaultImg;

	if(m_ObjState == EOS_MOUSEON && m_MouseOnImg != "")
		str_img = m_MouseOnImg;
	else if(m_ObjState == EOS_CLICK && m_ClickImg != "")
		str_img = m_ClickImg;
	else if(m_ObjState == EOS_DISABLE && m_DisableImg != "")
		str_img = m_DisableImg;

	SetImage(str_img);

	if(m_Static == NULL)
	{
		m_Static = new CMyStatic;
		m_Static->Create(_T(""), WS_CHILD | WS_VISIBLE , CRect(0, 0, 100, 30), pDC->m_pParent, 1234);

		LOGFONT lf;
		CFont* FONT = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
		FONT->GetLogFont(&lf);
		lstrcpy(lf.lfFaceName, _T("���� ���"));
		m_logfont = lf;

		m_Static->SetFont(FONT);
		
		m_Static->MoveWindow(m_Position.x, m_Position.y, m_Size.x, m_Size.y);
		m_pCwndObj = m_Static;
	}

	for(std::list<CUIToolBaseObject*>::iterator iter = m_ChildList.begin(); iter != m_ChildList.end(); iter++)
	{
		(*iter)->Render(pDC);
	}
}

void CUIToolStatic::SetFontColor(COLORREF p_color)
{
	CUIToolBaseObject::SetFontColor(p_color);

	if(m_Static)
	{
		m_Static->SetTextColor_static(p_color);
		m_Static->Invalidate();
	}
}
void CUIToolStatic::SetImage(CString p_strimgpath)
{
	if(m_Static)
	{
		m_Static->SetImagePath(p_strimgpath);
		m_Static->Invalidate();
	}
}
