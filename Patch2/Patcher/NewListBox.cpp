// NewListBox.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Patcher2.h"
#include "NewListBox.h"


// CNewListBox

IMPLEMENT_DYNAMIC(CNewListBox, CListBox)

CNewListBox::CNewListBox()
{

}

CNewListBox::~CNewListBox()
{
}


BEGIN_MESSAGE_MAP(CNewListBox, CListBox)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CNewListBox �޽��� ó�����Դϴ�.



BOOL CNewListBox::OnEraseBkgnd(CDC* pDC)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	
	return CListBox::OnEraseBkgnd(pDC);
}
