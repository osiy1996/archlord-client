#if !defined(AFX_MAPSELECTSTATIC_H__2F4E5A76_1128_4800_BAEF_B4D3071249FB__INCLUDED_)
#define AFX_MAPSELECTSTATIC_H__2F4E5A76_1128_4800_BAEF_B4D3071249FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MapSelectStatic.h : header file
//

// ������ (2002-06-11 ���� 3:02:49) : 
// ������ ����
// ���콺 ��ǥ -> ȭ�� �� ��ǥ�� ��ȯ ���
// ȭ�鿡 ���� ���
// ��� ���� �߱�
// �ε��Ҷ� �� �̹��� ����.
// ����Ŭ���Ұ�� ��Ʈ�� �޽��� ����.

#include "bmp.h"
#include <vector>

/////////////////////////////////////////////////////////////////////////////
// CMapSelectStatic window

#define	ALEF_PREVIEW_MAP_WIDTH	3
#define	ALEF_PREVIEW_MAP_HEIGHT	3

// 3 * 16
#define ALEF_PREVIEW_DIVISION_WIDTH	( 48 )

//#define	ALEF_PREVIEW_MAP_SELECT_SIZE	20

// wParam �� x , lParam�� y
#define	WM_MAPFOCUSCHANGED	( WM_USER + 1006 )
#define	WM_MAPSELECTED		( WM_USER + 1007 )

#define	MAP_HARDCODED_OFFSET_X	17
#define	MAP_HARDCODED_OFFSET_Z	17

#define MAP_DIVISION_MAX		50

#define MAP_HARDCODED_RANGE_X		16
#define MAP_HARDCODED_RANGE_Z		16

enum MODE
{
	NORMAL_LOADING_SELECT	,
	EXPORT_RANGE_SELECT
};

class CMapSelectStatic : public CStatic
{
// Construction
public:
	CMapSelectStatic();

// Attributes
public:
	// ������۸�
	BOOL	m_bSelected	;
	int		m_nSelectedIndexX;
	int		m_nSelectedIndexY;

	BOOL	m_bLButtonDown	;

	// �ͽ���Ʈ ����� ���..
	// ���õ� ������� ����Ʈ�� ������ �ִ´�.
	//AuList< int >	m_listDivision;
	vector< int >	m_vectorDivision;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMapSelectStatic)
	//}}AFX_VIRTUAL

// Implementation
public:
	int m_nMode;
	void SetMode( int nMode );

	void ReportParent		();
	BOOL GetIndex			( int mousex , int mousey , int & indexX , int & indexY);
	void LoadPreviewData	();
	void Init				();
	void ChangeSelectSize	( int zDelta );

	void AddSelection		( int nDivision );

	enum
	{
		WORLD_MAP	,
		//DUNGEON_MAP	,
		MAP_MAX
	};
	int	m_nCurrentMap;

	struct MapInfo
	{
		int		nStartOffsetX	;
		int		nStartOffsetZ	;
		CString	strName			;
		CBmp	bmpMap			;
	};

	MapInfo	m_MapList[ MAP_MAX ];

	MapInfo*	GetMapInfo	(){ return & m_MapList[ m_nCurrentMap ]; }

	void		SetMap		( int nMap )
	{
		ASSERT( nMap >= 0 && nMap < MAP_MAX );
		if( nMap >= 0 && nMap < MAP_MAX )
		{
			m_nCurrentMap = nMap;
			Invalidate( FALSE );
		}
	}

	int			GetMap		() { return m_nCurrentMap; }

	int			GetMapFromPos	( int x , int z );

	virtual ~CMapSelectStatic();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMapSelectStatic)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAPSELECTSTATIC_H__2F4E5A76_1128_4800_BAEF_B4D3071249FB__INCLUDED_)
