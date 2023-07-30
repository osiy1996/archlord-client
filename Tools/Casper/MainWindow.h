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
//----> kday 20040917
//#include "AgcdEffect.h"
#include "RpWorld.h"
#include "RpLODAtm.h"
#include "RpSkin.h"
#include "RtSkinSp.h"
//<----

#include "AcuObject.h"


class MainWindow : public AgcWindow  
{
public:

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


	BOOL OnIdle			( UINT32 ulClockCount);

	BOOL OnChar			( char *pChar		, UINT lParam );
	BOOL OnIMEComposing	( char *composing	, UINT lParam );
};


#endif // !defined(AFX_MAINWINDOW_H__B1E2FB1E_7FBD_474B_9099_05DA4F586D90__INCLUDED_)
