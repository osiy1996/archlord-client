// ������ (2002-06-07 ���� 10:46:32) : �������� ���̺귯�� MFC ���� Ŭ����.

// ��ġ����

// 1) WinApp�� �������� Ŭ���� ���
//		CRenderware m_Renderware;
// 2) WinApp �� OnIdle �������̵��� m_Renderware.Idle() ȣ��
// 3) Ÿ�������쿡 PreTraslateMessage �� ���� ���� �߰�
//      return theApp.m_Renderware.MessageProc( message , wParam , lParam );

#if !defined(_RW_FOR_MFC_)
#define _RW_FOR_MFC_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <rwcore.h>
#include <skeleton.h>

//using namespace RenderWare;
//using namespace std;

class CRenderWare  
{
public:     //Construction/destruction
	CRenderWare();
	
protected:  //Representation
	bool	m_bInitialised		;	//If the library has been initialised
	HWND	m_hWnd				;	//Member window which initialised to
	RwBool	m_bForegroundApp	;
	RwBool	m_bValidDelta		;

	// Global
    RwV2d	m_lastMousePos		;

public:     //Public Methods
	bool	Initialize   (HWND hWnd);
	void	Destroy      (void);
	
	bool	SetCameraViewSize (int dx, int dy);
	
	void	Render	( void );
	BOOL	Idle	( void );
	BOOL	MessageProc	( MSG* pMsg );	// �ٰ��������� ������ ���ν����� ���δ�.

	
protected:     //Protected interface

public:
	// �̺�Ʈ��.
	RsEventStatus	OnEventHandler			( RsEvent event , void *param ); // �޽����� ó���Ͽ� ���鿡�� �Ѱ���
	RsEventStatus	OnKeyboardEventHandler	( RsEvent event , void *param ); // �޽����� ó���Ͽ� ���鿡�� �Ѱ���
	RsEventStatus	OnMouseEventHandler		( RsEvent event , void *param ); // �޽����� ó���Ͽ� ���鿡�� �Ѱ���
};

#endif // !defined(_RW_FOR_MFC_)
