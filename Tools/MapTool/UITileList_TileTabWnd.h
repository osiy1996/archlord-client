#if !defined(AFX_UITILELIST_TILETABWND1_H__DCB98719_2659_4C2B_BE2A_4B8C23FC7184__INCLUDED_)
#define AFX_UITILELIST_TILETABWND1_H__DCB98719_2659_4C2B_BE2A_4B8C23FC7184__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UITileList_TileTabWnd1.h : header file
//

#include "bmp.h"
#include "AgcmMap.h"

/////////////////////////////////////////////////////////////////////////////
// CUITileList_TileTabWnd window

// ȭ�� ����.
// ���� 300
// ī�װ�1| Ÿ��1  , Ÿ��2 , Ÿ��3 , Ÿ��4 |  <- �Ѷ��ο� Ÿ�� 4�� ǥ��.. 
//          |                                |  <- Ÿ�� ���� ������� 50*50 ���� ó���� ���۵ȴ�.
// ī�װ�2| �޿���...                    |
//          |                                |
//-------------------------------------------|
// ī�װ��� ������ ���� �����Ѵ�.
// ī�װ� ��� ���콺 ������ ��ư�� ������ �˾� �޴��� �߸鼭 �޴����� ( ī�װ�����Ʈ )
//
//----------------------------------------------------------------------------------
// �⺻����
//----------------------------------------------------------------------------------
// 1, ī�װ��� ����
//      - ī�װ��� ���̶���Ʈ �Ÿ鼭 , ������ â�� ��ω� Ÿ�ϵ��� ǥ�õȴ�.
//      - ǥ�õŴ� Ÿ�ϵ��� �̸��� �Բ� ǥ�É´�.
// 2, Ÿ�� ����
//      - Ÿ���� ���̶���Ʈ �´�.
//      - ���Ŀ� ���� Ÿ�Ϸμ� �� Ÿ���� ���É´�.
// 3, ī�װ����� ���콺 ������ ��ư Ŭ��.
//      - ī�װ� ���� �Ŵ��� �θ���.
//      - Add,Delete,Edit �� �ҷ�����.
// 4, Ÿ�ϸŴ����� ���콺 ������ ��ư Ŭ��
//      - Ÿ�� ���� �޴��� ����.
//      - ���⼭ Ÿ���� ����/�̸����� �� �� �� �ִ�.
// 5, Ÿ���� ī�װ��� �巡��&���
//      - ī�װ� ���� ����� �����Ѵ�.


// �ҽ�����Ÿ�� ���� ���������ʰ� , �����Ѱ͸� �����Ѵ�.
// �ҽ�����Ÿ�� �Է¹ް� , ���� �̹����� �Բ� ��¡�� �ؼ�
// Ŭ���̾�Ʈ ����Ÿ�� �����Ѵ�.

class CUITileList_TileTabWnd : public CWnd
{
// Construction
public:
	struct	stTileInfo
	{
		CString	name		;// Ÿ���̸�..
		int		index		;// ���� �ε���.. ������..
		CBmp	*pBmp		;// �̹���..
		int		applyalpha	;// ����� ���� �̹���.
	};

	struct stTileCategory
	{
		CString					name	;	// ī�װ� �̸�
		int						category;	// ī�װ��� ������ Ÿ�� Ÿ�� ID
		AuList< stTileInfo >	list	;
	};

	// Ÿ�ϸ���Ʈ�� �ִ� �༮�� �� �������� ������ ���.
	AuList< stTileCategory * >		m_listCategory		;
	stTileCategory *				m_pSelectedCategory	;	// ���ĉ°� �����ͷ� ������ �մ´�.
	stTileInfo *					m_pSelectedTile		;	// ���É� Ÿ�� ����..
	int								m_nSelectedLayer	;	// ���� �۾����� ���̾� ����.
	
	CBitmap			m_backgroundbitmap	;
	CBrush			m_backgroundbrush	;
	CBitmap			m_aBitmapLayer[ TILELAYER_COUNT ];

	CBitmap			m_bitmapCategoryBoxSelected	;
	CBitmap			m_bitmapCategoryBoxUnselect	;
	CBitmap			m_bitmapTileSelect			;
	CBitmap			m_bitmapTileSelect2			;

	CRect			m_rectCursor				;
	CDC				m_MemDC						;
	CBitmap			m_MemBitmap					;	// ������۸���..

	
	CUITileList_TileTabWnd();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUITileList_TileTabWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	void ChangeLayer	( int layer		);
	void SelectTile		( int tileindex	);
	BOOL SaveScript		();
	void LoadScript		();

	// ������ (2004-05-31 ���� 1:06:41) : Ÿ���� ���ž����� �˻�.
	BOOL IsUsedTileName	( stTileInfo * pTInfo );

	BOOL CreateTextureList	( AcTextureList *				pList		);

	BOOL LoadBitmap			( stTileInfo  * pTileInfo			, BOOL bForce = FALSE );

	BOOL AddCategory		( stTileCategory *					pCategory	);
	BOOL AddTile			( int index , char * comment					);

	void UpdateDimensino	();

	stTileCategory *	FindCategory			( int		index );
	stTileCategory *	GetCategoryFromPoint	( CPoint	point );
	stTileInfo *		GetTileFromPoint		( CPoint	point );

	virtual ~CUITileList_TileTabWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CUITileList_TileTabWnd)
	afx_msg void OnPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnTilewindowAddcategory();
	afx_msg void OnTilewindowDeleteCategory();
	afx_msg void OnTilewindowEditCategory();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnTilewindowEditTile();
	afx_msg void OnTilewindowDeleteTile();
	afx_msg void OnTilewindowChangeTile();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UITILELIST_TILETABWND1_H__DCB98719_2659_4C2B_BE2A_4B8C23FC7184__INCLUDED_)
