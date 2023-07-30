// UIListView.cpp : implementation file
//

#include "stdafx.h"
#include "UITool.h"
#include "UIListView.h"

#include "MyEngine.h"
#include ".\uilistview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CUIToolApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CUIListView

IMPLEMENT_DYNCREATE(CUIListView, CListView)

CUIListView::CUIListView()
{
}

CUIListView::~CUIListView()
{
}


BEGIN_MESSAGE_MAP(CUIListView, CListView)
	//{{AFX_MSG_MAP(CUIListView)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUIListView drawing

void CUIListView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CUIListView diagnostics

#ifdef _DEBUG
void CUIListView::AssertValid() const
{
	CListView::AssertValid();
}

void CUIListView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CUIListView message handlers

void CUIListView::OnInitialUpdate() 
{
	CListView::OnInitialUpdate();

	CListCtrl &	csList = GetListCtrl();

	csList.ModifyStyle(0, LVS_REPORT);
	csList.InsertColumn(1, "UI Name", LVCFMT_LEFT, 148);
}

void CUIListView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW *	pNMListView	= (NM_LISTVIEW*)pNMHDR;
	CListCtrl &		csList		= GetListCtrl();
	POSITION		pos			= csList.GetFirstSelectedItemPosition();
	INT32			lListIndex;
	AgcdUI *		pcsUI;

	if (pos)
	{
		lListIndex = csList.GetNextSelectedItem(pos);
		pcsUI = (AgcdUI *) csList.GetItemData(lListIndex);
		if (!pcsUI)
			return;

		g_MainWindow.OpenUI(pcsUI);
	}
	
	*pResult = 0;
}

void CUIListView::OnSize(UINT nType, int cx, int cy) 
{
	CListView::OnSize(nType, cx, cy);
}

LRESULT CUIListView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	return CListView::WindowProc(message, wParam, lParam);
}

BOOL CUIListView::PreTranslateMessage(MSG* pMsg)
{
	/*
	// �������� �޽��� ������..
	MSG messagestruct;
	messagestruct.hwnd		= GetSafeHwnd();
	messagestruct.message	= pMsg->;
	messagestruct.wParam	= wParam;
	messagestruct.lParam	= lParam;
	*/

	if (pMsg->message == WM_CHAR || pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP || pMsg->message == WM_MOUSEWHEEL)
		if (theApp.GetRenderWare().MessageProc( pMsg ))
			return TRUE;

	CHAR	szTemp[128];

	sprintf(szTemp, "Message : %d\n", pMsg->message);
	OutputDebugString(szTemp);

	return CListView::PreTranslateMessage(pMsg);
}

// SortTextItems - ����Ʈ�� �÷� �ؽ�Ʈ�� ���� �����ϴ� �Լ�
// Returns - ������ TRUE ����
// nCol - ��Ʈ�� ���ڿ��� ������ �ִ� �÷���ȣ
// bAscending - ��Ʈ���� ����
// low - ���� ���� �� - �⺻���� 0
// high - ���� �������� - -1�� ���������� ����ŵ�ϴ�.
BOOL CUIListView::SortTextItems( int nCol, BOOL bAscending,int low /*= 0*/, int high /*= -1*/ )
{
	/*
	if( nCol >= ((CHeaderCtrl*)GetDlgItem(0))->GetItemCount() )
		return FALSE;

	if( high == -1 ) 
		high = GetItemCount() - 1;

	int lo = low;
	int hi = high;
	
	CString midItem;

	if( hi <= lo ) 
		return FALSE;

	midItem = GetItemText( (lo+hi)/2, nCol );

	// �ε������� �����ɶ����� ����Ʈ�� ���ϴ�(Loop).
	while( lo <= hi )
	{
		// rowText ������ ���ٿ� ���� ��� �÷����ڿ��� ������ �˴ϴ�.
		CStringArray rowText;

		//���� �ε������� �����Ͽ� ���� ��Һ��� ũ�ų� ���� ù° ��Ҹ� ã��.
		if( bAscending )
			while( ( lo < high ) && ( GetItemText(lo, nCol) < midItem ) )
				++lo;
		else
			while( ( lo < high ) && ( GetItemText(lo, nCol) > midItem ) )
				++lo;
		��
		//������ �ε������� �����Ͽ� ���� ��Һ��� ũ�ų� ���� ��Ҹ� ã��.
		if( bAscending )
			while( ( hi > low ) && ( GetItemText(hi, nCol) > midItem ) )
				--hi;
		else
			while( ( hi > low ) && ( GetItemText(hi, nCol) < midItem ) )
				--hi;
		
		// ���� �ε����� �������� �ʾҴٸ� ��ȯ�ϰ�, ���� �������� �����ʴٸ�,
		if( lo <= hi )
		{
			// �������� ���� �������� ��ȯ�Ѵ�.
			if( GetItemText(lo, nCol) != GetItemText(hi, nCol))
			{
				// �ٵ��� ��ȯ�Ѵ�.
				LV_ITEM lvitemlo, lvitemhi;
				int nColCount =?
				((CHeaderCtrl*)GetDlgItem(0))->GetItemCount();
				rowText.SetSize( nColCount );
				int i;
				for( i=0; i<nColCount; i++)
					rowText[i] = GetItemText(lo, i);
				
				lvitemlo.mask = LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
				lvitemlo.iItem = lo;
				lvitemlo.iSubItem = 0;
				lvitemlo.stateMask = LVIS_CUT | LVIS_DROPHILITED |
				LVIS_FOCUSED | LVIS_SELECTED |
				LVIS_OVERLAYMASK | LVIS_STATEIMAGEMASK;
				lvitemhi = lvitemlo;
				lvitemhi.iItem = hi;
				GetItem( &lvitemlo );
				GetItem( &lvitemhi );
				for( i=0; i<nColCount; i++)
					SetItemText(lo, i, GetItemText(hi, i));
				lvitemhi.iItem = lo;
				SetItem( &lvitemhi );
				for( i=0; i<nColCount; i++)
					SetItemText(hi, i, rowText[i]);
				lvitemlo.iItem = hi;
				SetItem( &lvitemlo );
			}
		++lo;
		--hi;
		}
	}

	// ���� ������ �ε����� �迭�� ���� ���� ���� �ʾҴٸ� ���� ������ 
	// �����ؾ� �Ѵ�.
	if( low < hi )
		SortTextItems( nCol, bAscending , low, hi);

	// ���� ���� �ε����� �迭�� ������ ���� ���� �ʾҴٸ� ������ ������
	// �����ؾ� �Ѵ�.
	if( lo < high )
		SortTextItems( nCol, bAscending , lo, high );
	*/

	return TRUE;
}

