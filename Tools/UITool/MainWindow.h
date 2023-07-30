// MainWindow.h: interface for the MainWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINWINDOW_H__B1E2FB1E_7FBD_474B_9099_05DA4F586D90__INCLUDED_)
#define AFX_MAINWINDOW_H__B1E2FB1E_7FBD_474B_9099_05DA4F586D90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgcEngine.h"
#include "rtimport.h"
#include "AcuObject.h"

#include "AgcmUIManager2.h"

#include "UITWindowProperties.h"

typedef enum
{
	UIT_MODE_EDIT	= 0,
	UIT_MODE_RUN,
	UIT_MODE_AREA,
} UITMode;

typedef struct
{
	FLOAT			fDistance;
	AcuObjectType	eType;
	INT32			lID;
} SelectedObject;

class MainWindow : public AgcWindow  
{
public:
	UITMode		m_eMode;

	BOOL		m_bDrawGrid;
	BOOL		m_bDrawingArea;
	BOOL		m_bDrawArea;
	RwV2d		m_stAreaStart;
	RwV2d		m_stAreaEnd;

	char		m_strBuffer[ 256 ]		;// �����Է� �׽�Ʈ..�ӽ�..
	char		m_strBufferComposing[ 256 ];

	INT32		m_nXPos;
	INT32		m_nYPos;

	AgcdUI *				m_pcsUI;
	AgcdUIControl *			m_pcsControl;

	UITWindowProperties *	m_pdlgWinProperty;

	INT32		m_lControlX;
	INT32		m_lControlY;

	AcUIBase *	m_pcsSelectedWindow;

public:
	MainWindow();
	virtual ~MainWindow();

	BOOL OnInit			(					);	// �ʱ�ȭ , ����Ÿ ���� & �׷��� ����Ÿ �ε�.	WindowUI �ǰ�� Open�Ŵ� ����, Full UI�� ��� Setting�Ŵ� ����.
	BOOL OnLButtonDown	( RsMouseStatus *ms	);
	BOOL OnMouseMove	( RsMouseStatus *ms	);
	BOOL OnLButtonUp	( RsMouseStatus *ms	);
	BOOL OnRButtonDown	( RsMouseStatus *ms	);
	BOOL OnRButtonUp	( RsMouseStatus *ms	);
	void OnClose		(					);	// ������ , �׷��� ����Ÿ ������. WindowUI �� ��� Close�Ŵ� ����,  FullUI�� ��� �ٸ� UI�� Setting�ű� ��.
	BOOL OnKeyDown		( RsKeyStatus *ks	);
	BOOL OnKeyUp		( RsKeyStatus *ks	);

	BOOL PreTranslateInputMessage	( RsEvent event , PVOID param	);

	void OnWindowRender	()					 ;	// Im2D�� Render�ϱ�
	void OnRender		( RwRaster *raster	);	// ������ ���⼭;

	BOOL OnIdle			( UINT32 ulClockCount);

	BOOL OnChar			( char *pChar		, UINT lParam );
	BOOL OnIMEComposing	( char *composing	, UINT lParam );
	
	void OnMoveWindow		();

	void RefreshProperty();
	void UpdateProperty	();

	void NewControl		(INT32 lType);

	void NewUI		();
	void OpenUI		(AgcdUI *pcsUI);

	void DeleteControl	(AgcdUIControl *pcsControl);
	void DeleteUI		(AgcdUI *pcsUI);

	void AddControlPosition	();

	void DrawSelected		();
	void DrawArea			();
	void DrawHandler		();

	void ChangeMode			(UITMode eMode, BOOL bSave = FALSE);
	BOOL ChangeName			(const CHAR *szName);

	void SetGridView		(BOOL bViewGrid);
};

extern CHAR *	g_aszMessages[AcUIBase::TYPE_COUNT][20];


#endif // !defined(AFX_MAINWINDOW_H__B1E2FB1E_7FBD_474B_9099_05DA4F586D90__INCLUDED_)
