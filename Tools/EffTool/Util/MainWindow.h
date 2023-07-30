// MainWindow.h: interface for the MainWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINWINDOW_H__B1E2FB1E_7FBD_474B_9099_05DA4F586D90__INCLUDED_)
#define AFX_MAINWINDOW_H__B1E2FB1E_7FBD_474B_9099_05DA4F586D90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgcmEff2.h"
#include "AcUIControlHeader.h"



class MainWindow : public AgcWindow  
{
public:	
	BOOL		m_bLBtnCamRot;

public:
	MainWindow();
	virtual ~MainWindow();

	BOOL OnInit			(					);	// �ʱ�ȭ , ����Ÿ ���� & �׷��� ����Ÿ �ε�.	WindowUI �ǰ�� Open�Ŵ� ����, Full UI�� ��� Setting�Ŵ� ����.
	BOOL OnLButtonDown	( RsMouseStatus *ms	);
	BOOL OnMouseMove	( RsMouseStatus *ms	);
	BOOL OnLButtonUp	( RsMouseStatus *ms	);
	BOOL OnRButtonDown	( RsMouseStatus *ms	);
	BOOL OnMButtonUp	( RsMouseStatus *ms	);
	BOOL OnMButtonDown	( RsMouseStatus *ms	);
	BOOL OnMouseWheel	( INT32	lDelta		);
	BOOL OnRButtonUp	( RsMouseStatus *ms	);
	void OnPreRender	( RwRaster *raster	);	// ������ ���⼭;
	void OnRender		( RwRaster *raster	);	// ������ ���⼭;
	void OnClose		(					);	// ������ , �׷��� ����Ÿ ������. WindowUI �� ��� Close�Ŵ� ����,  FullUI�� ��� �ٸ� UI�� Setting�ű� ��.
	BOOL OnKeyDown		( RsKeyStatus *ks	);
	BOOL OnKeyUp		( RsKeyStatus *ks	);
	BOOL OnIdle			(UINT32 ulClockCount);
};

#endif // !defined(AFX_MAINWINDOW_H__B1E2FB1E_7FBD_474B_9099_05DA4F586D90__INCLUDED_)
