// SubWindow.h: interface for the SubWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SUBWINDOW_H__470281DC_D592_4149_AE2D_BEFF04C93603__INCLUDED_)
#define AFX_SUBWINDOW_H__470281DC_D592_4149_AE2D_BEFF04C93603__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgcEngine.h"

class SubWindow : public AgcWindow  
{
public:
	RwRaster	*m_pRaster	;
	
	BOOL OnInit			(					);	// �ʱ�ȭ , ����Ÿ ���� & �׷��� ����Ÿ �ε�.	WindowUI �ǰ�� Open�Ŵ� ����, Full UI�� ��� Setting�Ŵ� ����.
	BOOL OnLButtonDown	( RsMouseStatus *ms	);
	BOOL OnRButtonDown	( RsMouseStatus *ms	);
	void OnRender		( RwRaster *raster	);	// ������ ���⼭;
	void OnClose		(					);	// ������ , �׷��� ����Ÿ ������. WindowUI �� ��� Close�Ŵ� ����,  FullUI�� ��� �ٸ� UI�� Setting�ű� ��.

	SubWindow();
	virtual ~SubWindow();

};

#endif // !defined(AFX_SUBWINDOW_H__470281DC_D592_4149_AE2D_BEFF04C93603__INCLUDED_)
