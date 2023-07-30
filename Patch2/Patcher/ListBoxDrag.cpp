// ListBoxDrag.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Patcher2.h"
#include "ListBoxDrag.h"
#include "Patcher2Dlg.h"


// CListBoxDrag

IMPLEMENT_DYNAMIC(CListBoxDrag, CListBox)

CListBoxDrag::CListBoxDrag()
{
}

CListBoxDrag::~CListBoxDrag()
{
}


BEGIN_MESSAGE_MAP(CListBoxDrag, CListBox)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()



// CListBoxDrag �޽��� ó�����Դϴ�.



void CListBoxDrag::OnDropFiles(HDROP hDropInfo)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	CPatcher2Dlg*		pParentDlg		=	static_cast< CPatcher2Dlg* >(GetParent());

	if( hDropInfo )
	{
		INT				nCount			=	DragQueryFile( hDropInfo , 0xFFFFFFFF , NULL , 0 );
		CHAR*			pFileName		=	NULL;
		DWORD			dwFileNameSize	=	0;

		for( INT i = 0 ; i < nCount ; ++i )
		{
			dwFileNameSize		=	DragQueryFile( hDropInfo, i , NULL , 0 );
			pFileName			=	new char[ dwFileNameSize+1 ];
			dwFileNameSize		=	DragQueryFile( hDropInfo , i , pFileName , dwFileNameSize+1 );

			pParentDlg->DragFileAdd( pFileName );

			delete [] pFileName;
			pFileName		=	NULL;
		}

	}

	CListBox::OnDropFiles(hDropInfo);
}
