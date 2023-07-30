// SubWindow.cpp: implementation of the SubWindow class.
//
//////////////////////////////////////////////////////////////////////
#include "rwcore.h"
#include "rpworld.h"

#ifdef RWLOGO
#include "rplogo.h"
#endif

#include "rtcharse.h"

#include <skeleton.h>
#include <menu.h>
//#include "events.h"
#include <camera.h>
#include <AgcEngine.h>
#include <MagDebug.h>
#include "MainWindow.h"
#include "SubWindow.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SubWindow::SubWindow()
{
	m_pRaster	= NULL;
}

SubWindow::~SubWindow()
{

}

BOOL SubWindow::OnInit			(					)	// �ʱ�ȭ , ����Ÿ ���� & �׷��� ����Ÿ �ε�.	WindowUI �ǰ�� Open�Ŵ� ����, Full UI�� ��� Setting�Ŵ� ����.
{
	m_pRaster = RsRasterReadFromBitmap( "86.bmp" );
	return TRUE;
}

BOOL SubWindow::OnLButtonDown	( RsMouseStatus *ms	)
{
	return FALSE;
}

BOOL SubWindow::OnRButtonDown	( RsMouseStatus *ms	)
{
	return FALSE;
}

void SubWindow::OnRender		( RwRaster *raster	)	// ������ ���⼭;
{
 	if( m_pRaster )
	{
		RwRasterPushContext	( raster );
		RwRasterRender		( m_pRaster , this->x , this->y );
		RwRasterPopContext	();
	}
}
void SubWindow::OnClose		(					)	// ������ , �׷��� ����Ÿ ������. WindowUI �� ��� Close�Ŵ� ����,  FullUI�� ��� �ٸ� UI�� Setting�ű� ��.
{
	if( m_pRaster )
	{
		RwRasterDestroy( m_pRaster );
	}
}
