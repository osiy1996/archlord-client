// UITPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "uitool.h"
#include "UITPropertyPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// UITPropertyPage property page

IMPLEMENT_DYNCREATE(UITPropertyPage, CPropertyPage)

UITPropertyPage::UITPropertyPage() : CPropertyPage()
{
	//{{AFX_DATA_INIT(UITPropertyPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

UITPropertyPage::~UITPropertyPage()
{
}

void UITPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(UITPropertyPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(UITPropertyPage, CPropertyPage)
	//{{AFX_MSG_MAP(UITPropertyPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// UITPropertyPage message handlers
