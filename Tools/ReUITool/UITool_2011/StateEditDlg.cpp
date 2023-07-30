// StateEditDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "UITool_2011.h"
#include "StateEditDlg.h"
#include "UITool_2011Doc.h"

// CStateEditDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CStateEditDlg, CDialog)

CStateEditDlg::CStateEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStateEditDlg::IDD, pParent)
	, m_strTexturePath(_T(""))
	, m_ColorR(_T(""))
	, m_ColorG(_T(""))
	, m_ColorB(_T(""))
	, m_strID(_T(""))
	, m_strPosX(_T(""))
	, m_strPosY(_T(""))
	, m_strSizeX(_T(""))
	, m_strSizeY(_T(""))
	, m_JustCopyObj(NULL)
{

}

CStateEditDlg::~CStateEditDlg()
{
}

void CStateEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_TEXTUREADDRESS, m_strTexturePath);
	DDX_Text(pDX, IDC_EDIT_STRINGCOLOR_R, m_ColorR);
	DDX_Text(pDX, IDC_EDIT_STRINGCOLOR_G, m_ColorG);
	DDX_Text(pDX, IDC_EDIT_STRINGCOLOR_B, m_ColorB);
	DDX_Control(pDX, IDC_EDIT_STRINGCOLOR_B, m_EditColorB);
	DDX_Control(pDX, IDC_EDIT_STRINGCOLOR_G, m_EditColorG);
	DDX_Control(pDX, IDC_EDIT_STRINGCOLOR_R, m_EditColorR);
	DDX_Control(pDX, IDC_EDIT_TEXTUREADDRESS, m_EditTexturePath);
	DDX_Control(pDX, IDC_LIST2, m_ListCtrl);
	DDX_Control(pDX, IDC_EDIT_ID, m_Edit_ID);
	DDX_Text(pDX, IDC_EDIT_ID, m_strID);
	DDX_Control(pDX, IDC_EDIT_POSX, m_Edit_PosX);
	DDX_Text(pDX, IDC_EDIT_POSX, m_strPosX);
	DDX_Control(pDX, IDC_EDIT_POSY, m_Edit_PosY);
	DDX_Text(pDX, IDC_EDIT_POSY, m_strPosY);
	DDX_Control(pDX, IDC_EDIT_SIZEW, m_Edit_SizeX);
	DDX_Text(pDX, IDC_EDIT_SIZEW, m_strSizeX);
	DDX_Control(pDX, IDC_EDIT_SIZEH, m_Edit_SizeY);
	DDX_Text(pDX, IDC_EDIT_SIZEH, m_strSizeY);
	DDX_Control(pDX, IDC_CHECK_VISIBLE, m_CheckBut_visivle);
}


BEGIN_MESSAGE_MAP(CStateEditDlg, CDialog)
	ON_BN_CLICKED(IDADD, &CStateEditDlg::OnBnClickedAdd)
	ON_BN_CLICKED(IDDELETE, &CStateEditDlg::OnBnClickedDelete)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE, &CStateEditDlg::OnBnClickedButtonTexture)
	ON_BN_CLICKED(IDC_COLORLOAD, &CStateEditDlg::OnBnClickedColorload)
	ON_BN_CLICKED(IDC_CHECK_VISIBLE, &CStateEditDlg::OnBnClickedCheckVisible)
END_MESSAGE_MAP()


// CStateEditDlg 메시지 처리기입니다.

void CStateEditDlg::OnBnClickedAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_Edit_ID.GetWindowText(m_strID);

	m_Edit_PosX.GetWindowText(m_strPosX);
	m_Edit_PosY.GetWindowText(m_strPosY);
	m_Edit_SizeX.GetWindowText(m_strSizeX);
	m_Edit_SizeY.GetWindowText(m_strSizeY);
	
	//m_EditTexturePath.GetWindowText(m_strTexturePath);

	CUITool_2011Doc* pDoc = (CUITool_2011Doc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();
	CUIToolBaseObject* pObj = pDoc->GetPickUpObject();

	if(pObj != NULL)
	{
		ObjectStatus* StaObj = new ObjectStatus;

		StaObj->StateID = m_strID;
		CPoint pi = CPoint(_variant_t(m_strPosX), _variant_t(m_strPosY));
		StaObj->StatePos = pi;
		pi = CPoint(_variant_t(m_strSizeX), _variant_t(m_strSizeY));
		StaObj->StateSize = pi;
		BYTE r = _variant_t(m_ColorR);
		BYTE g = _variant_t(m_ColorG);
		BYTE b = _variant_t(m_ColorB);

		StaObj->StateColor = RGB(r, g, b);
		StaObj->StateImgPath = m_strTexturePath;

		pObj->m_StatusEdit.push_back(StaObj);
	}

	m_ListCtrl.InsertItem(0, m_strID);

	CString str = m_strPosX;
	if(m_strPosX != _T("") || m_strPosY != _T(""))
		str += _T(", ");
	str += m_strPosY;
	m_ListCtrl.SetItemText(0, 1, str);
	
	str = m_strSizeX;
	if(m_strSizeX != _T("") || m_strSizeY != _T(""))
		str += _T(", ");
	str += m_strSizeY;
	m_ListCtrl.SetItemText(0, 2, str);

	m_ListCtrl.SetItemText(0, 3, m_strTexturePath);
}

void CStateEditDlg::OnBnClickedDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	POSITION pos = m_ListCtrl.GetFirstSelectedItemPosition();
	if(pos != NULL)
	{
		int nItem = m_ListCtrl.GetNextSelectedItem(pos);
		m_ListCtrl.DeleteItem(nItem);
	}
}

BOOL CStateEditDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CRect rect;
	LV_COLUMN lvcolumn;
	TCHAR rgtsz[4][10] = {_T("ID"), _T("Position"), _T("Size"), _T("Texture")};
	m_ListCtrl.GetClientRect(&rect);
	for(int i=0; i<4; i++) {  // 4는 컬럼의 개수
		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM |
			LVCF_TEXT | LVCF_WIDTH;
		lvcolumn.fmt = LVCFMT_CENTER;
		lvcolumn.pszText = rgtsz[i];
		lvcolumn.iSubItem = i;
		lvcolumn.cx = rect.Width() / 4;    // 4는 컬럼의 개수
		m_ListCtrl.InsertColumn(i, &lvcolumn);
	}


	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CStateEditDlg::OnBnClickedButtonTexture()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	static TCHAR BASED_CODE sz_Filter[] = _T("이미지 파일(*.jpg)|*.jpg|이미지 파일(*.bmp)|*.bmp|모든 파일(*.*)|*.*||");
	CFileDialog FileDialog(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, sz_Filter, this);   

	if(FileDialog.DoModal() == IDOK)
	{
		m_strTexturePath = FileDialog.GetPathName();
		m_EditTexturePath.SetWindowText(m_strTexturePath);
	}
}

void CStateEditDlg::OnBnClickedColorload()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CColorDialog ColorDialog(RGB(0,0,0), CC_FULLOPEN);

	 CString tmp = _T("");

	 if(ColorDialog.DoModal() == IDOK)
	 {
		 COLORREF color = ColorDialog.GetColor();

		 m_ColorR = _variant_t(GetRValue(color));
		 m_ColorG = _variant_t(GetGValue(color));
		 m_ColorB = _variant_t(GetBValue(color));

		 m_EditColorR.SetWindowText(m_ColorR);
		 m_EditColorG.SetWindowText(m_ColorG);
		 m_EditColorB.SetWindowText(m_ColorB);
	 }

	 /* 메서드 설명
	 RGB : RGB 색상 설정
	 GetColor : 컬러 반환 메서드
	 Get(R,G,B)Value : 해당 요소의 컬러를 반환하는 메서드
	 */

}


void CStateEditDlg::OnBnClickedCheckVisible()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	BOOL bCheck = m_CheckBut_visivle.GetCheck();

	CUITool_2011Doc* pDoc = (CUITool_2011Doc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();
	CUIToolBaseObject* pObj = pDoc->GetPickUpObject();
	
	if(pObj == NULL)
		return;

	if(bCheck)
	{
		if (m_JustCopyObj == NULL)
			m_JustCopyObj = new ObjectStatus;

		m_JustCopyObj->StatePos = pObj->m_Position;
		m_JustCopyObj->StateSize = pObj->m_Size;
		m_JustCopyObj->StateImgPath = pObj->m_DefaultImg;
		m_JustCopyObj->StateColor = pObj->m_Color;

		CPoint pi = CPoint(_variant_t(m_strPosX), _variant_t(m_strPosY));
		pObj->m_Position = pi;
		pi = CPoint(_variant_t(m_strSizeX), _variant_t(m_strSizeY));
		pObj->m_Size = pi;
		BYTE r = _variant_t(m_ColorR);
		BYTE g = _variant_t(m_ColorG);
		BYTE b = _variant_t(m_ColorB);

		pObj->m_Color = RGB(r, g, b);
		pObj->m_DefaultImg = m_strTexturePath;
	}
	else
	{
		pObj->m_Position = m_JustCopyObj->StatePos;
		pObj->m_Size = m_JustCopyObj->StateSize;
		pObj->m_DefaultImg = m_JustCopyObj->StateImgPath;
		pObj->m_Color = m_JustCopyObj->StateColor;

		delete m_JustCopyObj;
		m_JustCopyObj = NULL;
	}
	
	AfxGetMainWnd()->Invalidate(TRUE);	
}

void CStateEditDlg::OnCancel()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CUITool_2011Doc* pDoc = (CUITool_2011Doc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();
	if(pDoc != NULL && pDoc->GetPickUpObject() != NULL)
	{
		CUIToolComboBox* pCombo = (CUIToolComboBox*)pDoc->GetPickUpObject();
		if(pCombo)
		{
			for(int i = 0; i < m_ListCtrl.GetItemCount(); i++)
			{
				CString str = m_ListCtrl.GetItemText(i, 0);
				str = m_ListCtrl.GetItemText(i, 1);
				str = m_ListCtrl.GetItemText(i, 2);
				str = m_ListCtrl.GetItemText(i, 3);
			}
		}
	}

	CDialog::OnCancel();
}
