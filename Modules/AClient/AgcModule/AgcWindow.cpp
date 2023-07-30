#include <rwcore.h>
#include <rpworld.h>
#include <AgcSkeleton/skeleton.h>
#include <AgcSkeleton/menu.h>
#include <AgcSkeleton/camera.h>
#include <AgcModule/AgcModule.h>
#include "AgcWindow.h"
#include <AgcmUIControl/AcUIBase.h>
#include <ApMemoryTracker/ApMemoryTracker.h>
#ifndef USE_MFC
	#include <AgcmUIOption/AgcmUIOption.h>
#endif
#include <AgcModule/AgcEngine.h>

//---------------------------- CMessageMng ------------------------

CMessageMng::CMessageMng( AgcWindow* pAgcWindow ) : m_pAgcWindow(pAgcWindow)
{
	m_listMessage.clear();
}

CMessageMng::~CMessageMng()
{
	m_listMessage.clear();
}

void	CMessageMng::MessagePush( INT32 id, PVOID param1, PVOID param2 )
{
	if( m_listMessage.size() > s_MessageMax )	return;

	m_listMessage.push_back( Message( id, param1, param2 ) );
}

void	CMessageMng::MessageUpdate()
{
	for( MessageListItr Itr = m_listMessage.begin(); Itr != m_listMessage.end(); ++Itr )
		m_pAgcWindow->WindowControlMessageProc( (*Itr).message, (*Itr).pParam1, (*Itr).pParam2 );

	m_listMessage.clear();
}

//---------------------------- AgcWindow -------------------------
extern AgcEngine *	g_pEngine;
extern MSG g_msgLastReceived;
#define	AGCWINDOW_FAST_LIST_UNLINK(LISTS,NODE)		if(NODE == LISTS.head)						\
													{											\
														if(LISTS.tail == LISTS.head)			\
															LISTS.tail = NODE->next;			\
														LISTS.head = NODE->next;				\
														if(LISTS.head) LISTS.head->prev = NULL;	\
													}											\
													else if(NODE == LISTS.tail)					\
													{											\
														LISTS.tail->prev->next = NULL;			\
														LISTS.tail = LISTS.tail->prev;			\
													}											\
													else										\
													{											\
														if(NODE->prev)							\
															NODE->prev->next = NODE->next;		\
														if(NODE->next)							\
															NODE->next->prev = NODE->prev;		\
													}											

AgcWindow::AgcWindow() : CMessageMng(this)
{
	// do no op -_-;
	m_bOpened	= FALSE;
	m_bUseDrag	=	FALSE;

	m_nType		=	0;

	x			= 0;
	y			= 0;
	w			= 0;
	h			= 0;

	m_bNeedWindowListUpdate	= false;

	m_pFocusedChild	= NULL	;
	m_lControlID	=	0	;
	m_lUserData_1	=	0	;

	// Parn Added pParent = NULL
	pParent			= NULL	;
	m_bMoveMovableWindow	=	FALSE;

	m_bDeleteThis	= FALSE;
	m_bVisibleChildWindowForShink	=	FALSE	;

	m_bUIWindow		= FALSE	;

	m_bActiveEdit	= FALSE	;

	m_listChild.head = NULL;
	m_listChild.tail = NULL;
	
	m_listSetTop.head = NULL;
	m_listSetTop.tail = NULL;
	m_listAdd.head = NULL;
	m_listAdd.tail = NULL;

	m_bUseModalMessage	= FALSE;

	m_pcsOwnerBase	= NULL;
}

AgcWindow::~AgcWindow()
{
	// do no op -_-;
	AgcWindowNode*	cur_node = m_listChild.head;
	AgcWindowNode*	remove_node;
	while(cur_node)
	{
		remove_node = cur_node;
		cur_node = cur_node->next;

		delete remove_node;
	}

	cur_node = m_listSetTop.head;
	while(cur_node)
	{
		remove_node = cur_node;
		cur_node = cur_node->next;

		delete remove_node;
	}

	cur_node = m_listAdd.head;
	while(cur_node)
	{
		remove_node = cur_node;
		cur_node = cur_node->next;

		delete remove_node;
	}

	m_listChild.head = NULL;
	m_listChild.tail = NULL;
	m_listSetTop.head = NULL;
	m_listSetTop.tail = NULL;
	m_listAdd.head = NULL;
	m_listAdd.tail = NULL;
}

BOOL AgcWindow::Close()
{
	// Not yet Implemented
	// ���ϵ� ������ ���� �� Ŭ���� �� �Ŀ�
	// �ڽ��� ������ ���� ��Ŵ.
	KillFocus();

	AgcWindowNode*	cur_node = m_listChild.head;
	while(cur_node)
	{
		cur_node->pWindow->Close();
		cur_node = cur_node->next;
	}

	// Ŭ���� �̺�Ʈ �߻�. Message �����°� �ƴϴ���... -_-; MessageProc���� Close()�� �θ���.
	OnClose();
	m_bOpened	= FALSE;

	return TRUE;
}

RsEventStatus	AgcWindow::UserInputMessageProc		( RsEvent event , PVOID param ) // ���콺 �޽��� ����;
{
	if( param == NULL )
	{
		return rsEVENTPROCESSED;
	}

	if (g_pEngine->m_bModalMessage && !m_bUseModalMessage)
	{
		return rsEVENTNOTPROCESSED;
	}

	if( PreTranslateInputMessage( event , param ) )
	{
		return rsEVENTPROCESSED;
	}

	// ������ UI ���� ���콺 �޽����� �ѱ�.
	BOOL						bExit		= FALSE;
	BOOL						bFirst		= TRUE;
	BOOL						bKillFocus	= FALSE;

	AgcWindowNode*				cur_node = m_listChild.tail;

	RsMouseStatus stMouseStatus;

	while(cur_node)
	{
		switch( event )
		{
			case rsLEFTBUTTONDOWN	:
			case rsLEFTBUTTONUP		:
			case rsRIGHTBUTTONDOWN	:
			case rsRIGHTBUTTONUP	:
			case rsMOUSEMOVE		:
			case rsLEFTBUTTONDBLCLK	:
			case rsRIGHTBUTTONDBLCLK:
			case rsMIDDLEBUTTONDOWN	:
			case rsMIDDLEBUTTONUP	:
			case rsMIDDLEBUTTONDBLCLK:
			case rsMOUSEWHEELMOVE	:
			{
				bKillFocus	= TRUE;
				RsMouseStatus* pMStatus = (RsMouseStatus*) param;
				if( cur_node->pWindow->m_bOpened && cur_node->pWindow->HitTest( ( INT32 ) pMStatus->pos.x ,  ( INT32 ) pMStatus->pos.y ) )
				{
					if( rsEVENTPROCESSED == cur_node->pWindow->UserInputMessageProc( event , param ) )
					{
						// ���� �����쿡�� ó���� �ƴٸ� ���̻� ���� ������� �޽����� ������ �ʴ´�..
						bExit =	TRUE;	// ���� ���ϵ�� ���̻� ������ �ʴ´�.

						// LeftButtonUp�� ��� �̰� SetFocus�� �Ǹ�, �̰ſ� ���ؼ� ���� �����찡 ������ ����.
						if (event != rsLEFTBUTTONUP && cur_node->pWindow )
							cur_node->pWindow->SetFocus();	// �̳༮�� ž���� �÷�����..
					}
				}
			}
			break;
			
			case rsKEYDOWN			:
			case rsKEYUP			:
			case rsCHAR				:
			case rsIMECOMPOSING		:
			case rsIMEShowReadingWindow	:
			case rsIMEHideReadingWindow :
			case rsIMEShowCandidate		:
			case rsIMEHideCandidate		:

			if ( FALSE != cur_node->pWindow->m_Property.bVisible )
			{
				if( rsEVENTPROCESSED == cur_node->pWindow->UserInputMessageProc( event , param ) )
				{
					// ���� �����쿡�� ó���� �ƴٸ� ���̻� ���� ������� �޽����� ������ �ʴ´�..
					bExit	= TRUE;	// ���� ���ϵ�� ���̻� ������ �ʴ´�.
				}
			}
			break;
			
			default:
			break;
		}

		if( bExit )
		{
			// �������� �޽����� ó���ؼ� �� ������� �� �޽����� ó������ �ʴ´�.
			return rsEVENTPROCESSED;
		}

		cur_node = cur_node->prev;
	}					
		
	if ( !m_Property.bUseInput )
		return rsEVENTNOTPROCESSED;
	
	switch( event )
	{
	case rsKEYDOWN:
		{
			if( OnKeyDown( (RsKeyStatus *) param ) )
			{
				g_pEngine->m_pFocusedWindow	=	this	;
				return rsEVENTPROCESSED		;
			}
			else
				return rsEVENTNOTPROCESSED	;
		}
		
	case rsKEYUP:
		{
			if( OnKeyUp( (RsKeyStatus *)param ))
			{
				g_pEngine->m_pFocusedWindow	=	this	;
				return rsEVENTPROCESSED		;
			}
			else
				return rsEVENTNOTPROCESSED	;
		}
	case rsCHAR				:
		{
			if( OnChar( ( char * ) param , 0 ))
			{
				g_pEngine->m_pFocusedWindow	=	this	;
				return rsEVENTPROCESSED		;
			}
			else
				return rsEVENTNOTPROCESSED	;
		}
	case rsIMECOMPOSING		:
		{
			if( OnIMEComposing( ( char * ) param , 0 ))
			{
				g_pEngine->m_pFocusedWindow	=	this	;
				return rsEVENTPROCESSED		;
			}
			else
				return rsEVENTNOTPROCESSED	;
		}

	case rsIMEShowReadingWindow	:
		{
			if( OnIMEReading())
			{
				g_pEngine->m_pFocusedWindow	=	this	;
				return rsEVENTPROCESSED		;
			}
			else
				return rsEVENTNOTPROCESSED	;
		}
	case rsIMEHideReadingWindow :
		{
			if( OnIMEHideReading())
			{
				g_pEngine->m_pFocusedWindow	=	this	;
				return rsEVENTPROCESSED		;
			}
			else
				return rsEVENTNOTPROCESSED	;
		}
	case rsIMEShowCandidate		:
		{
			if( OnIMECandidate())
			{
				g_pEngine->m_pFocusedWindow	=	this	;
				return rsEVENTPROCESSED		;
			}
			else
				return rsEVENTNOTPROCESSED	;
		}
	case rsIMEHideCandidate		:
		{
			if( OnIMEHideCandidate())
			{
				g_pEngine->m_pFocusedWindow	=	this	;
				return rsEVENTPROCESSED		;
			}
			else
				return rsEVENTNOTPROCESSED	;
		}

	case rsLEFTBUTTONDOWN:
		{			
			stMouseStatus = * (RsMouseStatus *)param;
			ScreenToClient( stMouseStatus );
			if( OnLButtonDown( &stMouseStatus ))
			{
				g_pEngine->m_pFocusedWindow	=	this	;
				return rsEVENTPROCESSED		;
			}
			else
				return rsEVENTNOTPROCESSED	;
		}
	case rsLEFTBUTTONDBLCLK:
		{			
			stMouseStatus = * (RsMouseStatus *)param;
			ScreenToClient	( stMouseStatus	);
			if( OnLButtonDblClk	( &stMouseStatus ))
			{
				g_pEngine->m_pFocusedWindow	=	this	;
				return rsEVENTPROCESSED		;
			}
			else
				return rsEVENTNOTPROCESSED	;
		}
	case rsRIGHTBUTTONDBLCLK:
		{			
			stMouseStatus = * (RsMouseStatus *)param;
			ScreenToClient	( stMouseStatus	);
			if( OnRButtonDblClk	( &stMouseStatus ))
			{
				g_pEngine->m_pFocusedWindow	=	this	;
				return rsEVENTPROCESSED		;
			}
			else
				return rsEVENTNOTPROCESSED	;
		}
		
	case rsLEFTBUTTONUP:
		{			
			stMouseStatus = * (RsMouseStatus *)param;
			ScreenToClient( stMouseStatus );
			if( OnLButtonUp( &stMouseStatus ))
			{
				g_pEngine->m_pFocusedWindow	=	this	;
				return rsEVENTPROCESSED		;
			}
			else
				return rsEVENTNOTPROCESSED	;
		}
		
	case rsRIGHTBUTTONDOWN:
		{			
			stMouseStatus = * (RsMouseStatus *)param;
			ScreenToClient( stMouseStatus );
			if( OnRButtonDown( &stMouseStatus ))
			{
				g_pEngine->m_pFocusedWindow	=	this	;
				return rsEVENTPROCESSED		;
			}
			else
				return rsEVENTNOTPROCESSED	;
		}
		
	case rsRIGHTBUTTONUP:
		{			
			stMouseStatus = * (RsMouseStatus *)param;
			ScreenToClient( stMouseStatus );
			if( OnRButtonUp( &stMouseStatus ))
			{
				g_pEngine->m_pFocusedWindow	=	this	;
				return rsEVENTPROCESSED		;
			}
			else
				return rsEVENTNOTPROCESSED	;
		}

	case rsMIDDLEBUTTONDOWN:
		{			
			stMouseStatus = * (RsMouseStatus *)param;
			ScreenToClient( stMouseStatus );
			if( OnMButtonDown( &stMouseStatus ))
			{
				g_pEngine->m_pFocusedWindow	=	this	;
				return rsEVENTPROCESSED		;
			}
			else
				return rsEVENTNOTPROCESSED	;
		}

	case rsMIDDLEBUTTONUP:
		{			
			stMouseStatus = * (RsMouseStatus *)param;
			ScreenToClient( stMouseStatus );
			if( OnMButtonUp( &stMouseStatus ))
			{
				g_pEngine->m_pFocusedWindow	=	this	;
				return rsEVENTPROCESSED		;
			}
			else
				return rsEVENTNOTPROCESSED	;
		}

	case rsMIDDLEBUTTONDBLCLK:
		{			
			stMouseStatus = * (RsMouseStatus *)param;
			ScreenToClient( stMouseStatus );
			if( OnMButtonDblClk( &stMouseStatus ))
			{
				g_pEngine->m_pFocusedWindow	=	this	;
				return rsEVENTPROCESSED		;
			}
			else
				return rsEVENTNOTPROCESSED	;
		}
		
	case rsMOUSEMOVE:
		{			
			stMouseStatus = * (RsMouseStatus *)param;
			ScreenToClient( stMouseStatus );
			if( OnMouseMove( &stMouseStatus ))
			{
				g_pEngine->m_pFocusedWindow	=	this	;
				return rsEVENTPROCESSED		;
			}
			else
				return rsEVENTNOTPROCESSED	;
		}
		
	case rsMOUSEWHEELMOVE:
		{
			RsMouseStatus * pMStatus = (RsMouseStatus *)param;
			if( OnMouseWheel( pMStatus->wheel ))
			{
				g_pEngine->m_pFocusedWindow	=	this	;
				return rsEVENTPROCESSED		;
			}
			else
				return rsEVENTNOTPROCESSED	;
		}
	default:
		break;
	}
	
	return rsEVENTNOTPROCESSED;			// ����� ���� ��鷯�� ������ �ȵž� �ִ� ��..
}

void AgcWindow::ClientToScreen	( RsMouseStatus &ms		)
{
	int posx , posy;

	posx = ( int ) ms.pos.x;
	posy = ( int ) ms.pos.y;

	ClientToScreen( &posx , &posy );
	
	ms.pos.x = ( float ) posx;
	ms.pos.y = ( float ) posy;
}

void AgcWindow::ScreenToClient	( RsMouseStatus &ms		) // ���콺 ����Ʈ , ��ǥ ��ġ��.
{
	int posx , posy;

	posx = ( int ) ms.pos.x;
	posy = ( int ) ms.pos.y;

	ScreenToClient( &posx , &posy );
	
	ms.pos.x = ( float ) posx;
	ms.pos.y = ( float ) posy;
}

void AgcWindow::ClientToScreen	( int *px , int *py		)
{
	PROFILE("AgcWindow::ClientToScreen");

	int parentx = 0 , parenty = 0;
	
	if( pParent )
	{
		pParent->ClientToScreen( &parentx , &parenty );
	}

	if (px)
		*px = *px + parentx + this->x;

	if (py)
		*py = *py + parenty + this->y;
}

void AgcWindow::ScreenToClient	( int *px , int *py		)
{
	int parentx = 0 , parenty = 0;

	if( pParent )
	{
		pParent->ClientToScreen( &parentx , &parenty );
	}

	*px = *px - parentx - this->x;
	*py = *py - parenty - this->y;
}

BOOL AgcWindow::HitTest			( INT32 x , INT32 y )	// �ش� ����Ʈ�� �� ��⿡ �����ִ��� ����. âUI���� �߿���.
{
	// ShowWindow FALSE�̰ų� ���� Window(WindowListUpdate���� ó���ϸ�)��� FALSE�� return�Ѵ�. 
	if ( !m_Property.bVisible || !m_bOpened )		return FALSE;

	int		posx = 0 , posy = 0 ;
	ClientToScreen( &posx , &posy );

	if( x >= posx && x < posx + w && y >= posy && y < posy + h )
		return TRUE;
	
	return FALSE;
}

BOOL AgcWindow::WindowControlMessageProc	( INT32 message , PVOID pParam1 , PVOID pParam2	)
{
	// ������ UI ���� ���콺 �޽����� �ѱ�.
	AgcWindowNode*	cur_node;
	
	#define	Message_Forward_To_Child	cur_node	= m_listChild.head;								\
			while( cur_node )																		\
			{																						\
				cur_node->pWindow->WindowControlMessageProc( message , pParam1 , pParam2 );			\
				cur_node = cur_node->next;															\
			}

	#define	Message_Backward_To_Child	cur_node = m_listChild.tail;								\
			while( cur_node )																		\
			{																						\
				cur_node->pWindow->WindowControlMessageProc( message , pParam1 , pParam2 );			\
				cur_node = cur_node->prev;															\
			}
	
	#define Message_Forward_To_Child_Render cur_node	= m_listChild.head;							\
			while( cur_node )																		\
			{																						\
				if( g_pEngine->GetMonopolyWindow() != cur_node->pWindow )									\
					cur_node->pWindow->WindowControlMessageProc( message , pParam1 , pParam2 );		\
				cur_node = cur_node->next;															\
			}

	// Message Dispatch
	switch( message )
	{
	case	MESSAGE_INIT		:	// void
		return OnInit();
	case	MESSAGE_POSTINIT		:	// void
		OnPostInit();
		Message_Forward_To_Child
		return TRUE;
	case	MESSAGE_CLOSE		:	// void
		{
			Close();

			return TRUE;
		}
	case	MESSAGE_PRERENDER	:	// RwRaster *
		{
			if ( FALSE == m_Property.bVisible ) return FALSE;

			// �������� ��Ʈ�� ���� �����Ѵ�.
			OnPreRender		( ( RwRaster * ) pParam1 );
			Message_Forward_To_Child_Render
			return TRUE;
		}
	case	MESSAGE_RENDER		:	// RwRaster *
		{
			if ( FALSE == m_Property.bVisible ) return FALSE;

			// �������� ��Ʈ�� ���� �����Ѵ�.
			OnRender		( ( RwRaster * ) pParam1 );
			Message_Forward_To_Child_Render
			return TRUE;
		}
	case	MESSAGE_POSTRENDER	:	// RwRaster *
		{
			if ( FALSE == m_Property.bVisible ) return FALSE;

			// �������� ��Ʈ�� ���� �����Ѵ�.
			OnPostRender	( ( RwRaster * ) pParam1 );
			Message_Forward_To_Child_Render
			return TRUE;
		}
	case	MESSAGE_KEYDOWN		:	// RsKeyStatus *
		{
			if ( FALSE == m_Property.bUseInput ) return FALSE;
			return OnKeyDown	( (RsKeyStatus *)pParam1 );
		}
	case	MESSAGE_KEYUP		:	// RsKeyStatus *
		{
			if ( FALSE == m_Property.bUseInput ) return FALSE;
			return OnKeyUp		( (RsKeyStatus *)pParam1 );
		}
	case	MESSAGE_CHAR		:	// char
		{
			// do no op now.
			return FALSE;
		}
	case	MESSAGE_IMECOMPOSING:	//
		{
			// do no op now.
			return FALSE;
		}
	case	MESSAGE_LBUTTONDOWN	:	// RsMouseStatus *
		{
			if ( FALSE == m_Property.bUseInput ) return FALSE;
			return OnLButtonDown	( (RsMouseStatus *)pParam1 );
		}
	case	MESSAGE_LBUTTONUP	:	// RsMouseStatus *
		{
			if ( FALSE == m_Property.bUseInput ) return FALSE;
			return OnLButtonUp		( (RsMouseStatus *)pParam1 );
		}
	case	MESSAGE_LBUTTONDBLCLK	:	// RsMouseStatus *
		{
			if ( FALSE == m_Property.bUseInput ) return FALSE;
			return OnLButtonDblClk		( (RsMouseStatus *)pParam1 );
		}
	case	MESSAGE_RBUTTONDBLCLK	:	// RsMouseStatus *
		{
			if ( FALSE == m_Property.bUseInput ) return FALSE;
			return OnRButtonDblClk		( (RsMouseStatus *)pParam1 );
		}	
	case	MESSAGE_RBUTTONDOWN	:	// RsMouseStatus *
		{
			if ( FALSE == m_Property.bUseInput ) return FALSE;
			return OnRButtonDown	( (RsMouseStatus *)pParam1 );
		}
	case	MESSAGE_RBUTTONUP	:	// RsMouseStatus *
		{
			if ( FALSE == m_Property.bUseInput ) return FALSE;
			return OnRButtonUp		( (RsMouseStatus *)pParam1 );
		}
	case	MESSAGE_MOUSEMOVE	:	// RsMouseStatus *
		{
			if ( FALSE == m_Property.bUseInput ) return FALSE;
			return OnMouseMove		( (RsMouseStatus *)pParam1 );
		}
	case	MESSAGE_MOUSEWHEEL	:	// BOOL
		{
			if ( FALSE == m_Property.bUseInput ) return FALSE;
			RsMouseStatus * pStatus = ( RsMouseStatus * )pParam1;
			return OnMouseWheel( pStatus->wheel );
		}
	case	MESSAGE_IDLE		:	// 
		// do no op
		{
			// ����� �޽����� �Ѿ���� �ʴ´�..
			Message_Forward_To_Child
			OnIdle( ( UINT ) pParam1 );
		}
		return FALSE;

	case	MESSAGE_COMMAND		:
		{
			// ���ϵ�� ������ ����...
			return OnCommand( ( INT32 ) pParam1 , pParam2 );
		}
		
	case	MESSAGE_DRAG_DROP		:
		{
			return OnDragDrop( pParam1, pParam2 );
		}
		
	case	MESSAGE_SETFOCUS	:
		{
			OnSetFocus();
			return TRUE;
		}
	case	MESSAGE_KILLFOCUS	:
		{
			OnKillFocus();
			return TRUE;
		}

	case MESSAGE_EDIT_ACTIVE	:
		{
			OnEditActive();
			return TRUE;
		}

	case MESSAGE_EDIT_DEACTIVE	:
		{
			OnEditDeactive();
			return TRUE;
		}
		
	default:
		return FALSE;
	}
}

BOOL AgcWindow::AddChild	( AgcWindow * pWindow , INT32 nid , BOOL bInit )
{
	ASSERT( pWindow );

	AgcWindowNode*		cur_node = m_listChild.head;
	while(cur_node)
	{
		if(cur_node->pWindow == pWindow)
		{
			if( !pWindow->m_bOpened)
			{
				pWindow->m_bOpened = TRUE;
				return TRUE;
			}
			
			return FALSE;
		}

		cur_node = cur_node->next;
	}

	AgcWindowNode*		cur_add = m_listAdd.head;
	while(cur_add)
	{
		if(cur_add->pWindow == pWindow) return FALSE;
		cur_add = cur_add->next;
	}

	// �˻簡 �������Ƿ�..
	// �����츦 ���� �߰��Ѵ�.
	WindowListLinkTail(&m_listAdd,pWindow);

	// ID ����..
	pWindow->m_lControlID	= (INT32) pWindow	;
	pWindow->pParent		= this	;

	m_bNeedWindowListUpdate	= TRUE	;

	if( pWindow->m_bOpened ) 
	{
		//����?..;;
	}
	else
	{
		pWindow->m_bOpened	= TRUE;
		// �ʱ�ȭ �޽��� ����..
		if ( bInit )
			pWindow->WindowControlMessageProc( AgcWindow::MESSAGE_INIT );
	}
	
	// ���� ������ �ʱ�ȭ�� �ܺο��� �ؾ��Ѵ�.
	// ������ (2004-07-19 ���� 12:22:50) : 2�� �ʱ�ȭ �߰�~
	pWindow->ReculsiveWindowControlMessageProc( AgcWindow::MESSAGE_POSTINIT );

/*
	if( RsGlobal.maximumWidth != 1280 || RsGlobal.maximumHeight != 1024 )
	{
		//-----------------------------------------------------------------------
		// ���� �ػ󵵿� ���� UI ��ġ�� �����մϴ�.
		//
		if( pWindow->pParent && !pWindow->pParent->pParent ) // ��� ui�� �ڽ��̸�
		{
			// 1024 768 �������� ����� ui ��ǥ�� ���� �ػ� ������ ��ǥ�� �����մϴ�.

			// ui�� ������ �������� ���
			float centerX = (pWindow->x + pWindow->w)*0.5f;
			float centerY = (pWindow->y + pWindow->h)*0.5f;

			// x�� 1024�� ���� 512�� �������� �����մϴ�.
			float newCenterX = (centerX * (RsGlobal.maximumWidth*0.5f) / 512);
			pWindow->x += (INT16)(newCenterX - centerX);

			// y�� ���Ϳ��� ����� �����ڸ��� �������� �����մϴ�.
			float newCenterY = (centerY * (RsGlobal.maximumHeight*0.5f) / 384);
			pWindow->y += (INT16)(newCenterY - centerY);
		}
		//-----------------------------------------------------------------------
	}
*/

	return TRUE;
}

BOOL	AgcWindow::ReculsiveWindowControlMessageProc( INT32 message , PVOID pParam1 , PVOID pParam2)
{
	// �޽����� ���� ������ ��ο��� ������.
	BOOL bRet = WindowControlMessageProc( AgcWindow::MESSAGE_POSTINIT );

	AgcWindowNode*		cur_node = m_listChild.head;
	while(cur_node)
	{
		cur_node->pWindow->ReculsiveWindowControlMessageProc( message , pParam1 , pParam2 );
		cur_node = cur_node->next;
	}

	return bRet;
}


void	AgcWindow::WindowListUnlink(AgcWindowList*	pList,AgcWindow*	pRemoveWindow)
{
	if(!pList->head)	return;

	AgcWindowNode*		pHead = pList->head;
	AgcWindowNode*		pTail = pList->tail;
	AgcWindowNode*		pRemoveNode;

	if(pRemoveWindow == pHead->pWindow)
	{
		pRemoveNode = pHead;

		pList->head = pHead->next;
		if(pList->head) pList->head->prev = NULL;

		if(pTail->pWindow == pRemoveWindow)
		{
			pList->tail = pList->head;
		}
	}
	else if(pRemoveWindow == pTail->pWindow)
	{
		pRemoveNode = pTail;

		pTail->prev->next = NULL;							// head�� �ƴϹǷ� �˻�x
		pList->tail = pTail->prev;
	}
	else
	{
		AgcWindowNode*	cur_node = pHead;
		AgcWindowNode*	bef_node;
		while(cur_node)
		{
			if(cur_node->pWindow == pRemoveWindow)
			{
				pRemoveNode = cur_node;

				bef_node->next = cur_node->next;
				cur_node->next->prev = bef_node;			// tail�� �ƴϹǷ�..�˻�X
				break;
			}

			bef_node = cur_node;
			cur_node = cur_node->next;
		}
	}

	pRemoveNode->pWindow = NULL;
	g_pEngine->m_listWindowToDelete.AddTail(pRemoveNode);
}

void	AgcWindow::WindowListLinkTail(AgcWindowList*	pList,AgcWindow*	pAddWindow)
{
	AgcWindowNode*		pAddNode;

	pAddNode = new AgcWindowNode;

	pAddNode->next = NULL;
	pAddNode->pWindow = pAddWindow;

	if(!pList->tail)		// head�� null�� ���..
	{
		pList->head = pAddNode;
		pList->tail = pAddNode;
		pAddNode->prev = NULL;
	}
	else
	{
		pAddNode->prev = pList->tail;
		pList->tail->next = pAddNode;
		pList->tail = pAddNode;
	}
}

void	AgcWindow::WindowListLinkBefore(AgcWindowList*	pList,AgcWindow*		pBaseWindow,AgcWindow*	pAddWindow)
{
	AgcWindowNode*		pAddNode;

	pAddNode = new AgcWindowNode;

	pAddNode->next = NULL;
	pAddNode->prev = NULL;
	pAddNode->pWindow = pAddWindow;

	if(pBaseWindow == pList->head->pWindow)
	{
		pAddNode->next = pList->head;
		pList->head->prev = pAddNode;

		pList->head = pAddNode;
	}
	else
	{
		AgcWindowNode*	cur_node = m_listChild.head;
		AgcWindowNode*	bef_node;
		while(cur_node)
		{
			if(cur_node->pWindow == pBaseWindow)
			{
				bef_node->next = pAddNode;
				pAddNode->next = cur_node;
				pAddNode->prev = bef_node;
				cur_node->prev = pAddNode;

				break;
			}

			bef_node = cur_node;
			cur_node = cur_node->next;
		}
	}
}

BOOL AgcWindow::DeleteChild	( AgcWindow * pWindow , BOOL bClose	, BOOL bDeleteMemory	)
{
	ASSERT( pWindow );

	AgcWindowNode*	cur_node = m_listChild.head;
	while( cur_node )
	{
		if( cur_node->pWindow == pWindow )
		{
			g_pEngine->PopMonopolyWindow(pWindow);

			if( pWindow->m_bOpened && bClose )
			{
				pWindow->WindowControlMessageProc( AgcWindow::MESSAGE_CLOSE );
			}
			else
			{
				pWindow->KillFocus();
			}

			m_bNeedWindowListUpdate	= TRUE	;

			pWindow->m_bDeleteThis	= bDeleteMemory	;
			pWindow->m_bOpened		= FALSE	;
			return TRUE;
		}

		cur_node = cur_node->next;
	}

	cur_node = m_listAdd.head;
	AgcWindowNode*	before_node;
	while(cur_node)
	{
		if(cur_node->pWindow == pWindow)
		{
			g_pEngine->PopMonopolyWindow(pWindow);

			pWindow->pParent = NULL;

			AGCWINDOW_FAST_LIST_UNLINK(m_listAdd, cur_node);

			if (!bDeleteMemory)
				cur_node->pWindow = NULL;

			g_pEngine->m_listWindowToDelete.AddTail(cur_node);

			return	TRUE;				
		}
		
		before_node = cur_node;
		cur_node = cur_node->next;
	}

	return FALSE;
}

BOOL AgcWindow::DeleteChild	( INT32			nid	, BOOL bClose	, BOOL bDeleteMemory	)
{
	AgcWindow*		pWindow;
	AgcWindowNode*	cur_node = m_listChild.head;
	while(cur_node)
	{
		pWindow = cur_node->pWindow;
		if(pWindow->m_lControlID == nid)
		{
			g_pEngine->PopMonopolyWindow(pWindow);

			if( pWindow->m_bOpened && bClose )
			{
				pWindow->WindowControlMessageProc( AgcWindow::MESSAGE_CLOSE );
			}
			else
			{
				pWindow->KillFocus();
			}

			m_bNeedWindowListUpdate	= true	;

			pWindow->m_bDeleteThis	= bDeleteMemory	;
			pWindow->m_bOpened		= FALSE	;

			return TRUE;
		}
		cur_node = cur_node->next;
	}

	cur_node = m_listAdd.head;
	AgcWindowNode*	before_node;
	while(cur_node)
	{
		pWindow = cur_node->pWindow;
		if(pWindow->m_lControlID == nid)
		{
			g_pEngine->PopMonopolyWindow(pWindow);

			pWindow->pParent = NULL;

			AGCWINDOW_FAST_LIST_UNLINK (m_listAdd, cur_node);

			if (!bDeleteMemory)
				cur_node->pWindow = NULL;

			g_pEngine->m_listWindowToDelete.AddTail(cur_node);

			return	TRUE;
		}

		before_node = cur_node;
		cur_node = cur_node->next;
	}
		
	return FALSE;
}

INT32 AgcWindow::GetDepth()
{
	if( !pParent )		return 0;

	INT32 nDepth = 0;
	GetDepthRecurse( this, &nDepth );
	return nDepth;
}

void AgcWindow::GetDepthRecurse(AgcWindow* pCurWindow, INT32* pDepth)
{
	ASSERT(pCurWindow);

	(*pDepth)++;
	
	if ( pCurWindow->pParent != NULL )
		GetDepthRecurse(pCurWindow->pParent, pDepth);
}

void AgcWindow::MoveWindow		( INT32 x , INT32 y , INT32 w , INT32 h )
{
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;

	OnMoveWindow();
}

void	AgcWindow::WindowListUpdate()
{
	PROFILE("AgcWindow::WindowListUpdate()");

	// Child ���� ������Ʈ
	AgcWindowNode*	cur_node = m_listChild.head;
	while(cur_node)
	{
		// calling update functions...
		if(cur_node->pWindow->m_Property.bVisible)
			cur_node->pWindow->WindowListUpdate();

		cur_node = cur_node->next;
	}
	
	// Disable �� �༮ ���ֹ���..
	if( m_bNeedWindowListUpdate )
	{
		AgcWindow	*pWindowTopmostStart = NULL;
		AgcWindowNode*		remove_node;
		AgcWindowNode*		before_node;
		AgcWindow*	del_window;

		// ���� SetTop Queue���� Close�� ����� �������� �Ѵ�.
		cur_node = m_listSetTop.head;
		while(cur_node)
		{
			if( cur_node->pWindow->m_bOpened == FALSE )
			{
				AGCWINDOW_FAST_LIST_UNLINK(m_listSetTop, cur_node);

				remove_node = cur_node;
				cur_node = cur_node->next;

				remove_node->pWindow = NULL;
				g_pEngine->m_listWindowToDelete.AddTail(remove_node);
			}
			else
			{
				before_node = cur_node;
				cur_node = cur_node->next;
			}
		}

		cur_node = m_listChild.head;
		while(cur_node)
		{
			if( cur_node->pWindow->m_bOpened == FALSE )
			{
				AGCWINDOW_FAST_LIST_UNLINK(m_listChild, cur_node);

				remove_node = cur_node;
				del_window = cur_node->pWindow;
				cur_node = cur_node->next;

				//delete	pWindow;
				del_window->OnPreRemoveThis();
				del_window->KillFocus();
				del_window->pParent = NULL;

				if( !del_window->m_bDeleteThis )
					remove_node->pWindow = NULL;

				g_pEngine->m_listWindowToDelete.AddTail(remove_node);
			}
			else
			{
				if (!pWindowTopmostStart && cur_node->pWindow->m_Property.bTopmost)
				{
					pWindowTopmostStart = cur_node->pWindow;
				}
				else
				{
					if (pWindowTopmostStart && !cur_node->pWindow->m_Property.bTopmost)
					{
						// �ƿ��, �ڵ� ���� �� �������� ����.. -_-;
						cur_node->pWindow->SetFocus();
					}

					if( m_listSetTop.head )
					{
						AgcWindowNode*	cur_settop = m_listSetTop.head;
						while(cur_settop)
						{
							if( cur_node->pWindow == cur_settop->pWindow )
							{
								AGCWINDOW_FAST_LIST_UNLINK(m_listChild, cur_node);

								//del_window = cur_node->pWindow;
								remove_node = cur_node;
								cur_node = cur_node->next;

								remove_node->pWindow = NULL;
								g_pEngine->m_listWindowToDelete.AddTail(remove_node);
							
								break;
							}
							
							cur_settop = cur_settop->next;
						}

						if(cur_settop)
							continue;
					}
				}
				
				before_node = cur_node;
				cur_node = cur_node->next;
			}
		}

		if( m_listSetTop.head )
		{
			AgcWindowNode*	cur_settop = m_listSetTop.head;
			AgcWindowNode*	remove_settop;

			while( cur_settop )
			{
				if( pWindowTopmostStart == cur_settop->pWindow )
				{
					if(cur_settop->pWindow != m_listChild.tail->pWindow )
					{
						WindowListUnlink(&m_listChild,cur_settop->pWindow);
						WindowListLinkTail(&m_listChild,cur_settop->pWindow);
					}
				}
				else if( pWindowTopmostStart && !cur_settop->pWindow->m_Property.bTopmost)
					WindowListLinkBefore(&m_listChild,pWindowTopmostStart,cur_settop->pWindow);
				else 
					WindowListLinkTail(&m_listChild,cur_settop->pWindow);

				remove_settop = cur_settop;
				cur_settop = cur_settop->next;

				remove_settop->pWindow = NULL;
				g_pEngine->m_listWindowToDelete.AddTail(remove_settop);
			}

			m_listSetTop.head = NULL;
			m_listSetTop.tail = NULL;
		}

		if( m_listAdd.head )
		{
			AgcWindowNode*	cur_add = m_listAdd.head;
			AgcWindowNode*	remove_add;
			
			while(cur_add)
			{
				if( pWindowTopmostStart && !cur_add->pWindow->m_Property.bTopmost)
					WindowListLinkBefore(&m_listChild,pWindowTopmostStart,cur_add->pWindow);
				else 
					WindowListLinkTail(&m_listChild,cur_add->pWindow);
					
				cur_add->pWindow->WindowListUpdate();

				remove_add = cur_add;
				cur_add = cur_add->next;

				remove_add->pWindow = NULL;
				g_pEngine->m_listWindowToDelete.AddTail(remove_add);
			}

			m_listAdd.head = NULL;
			m_listAdd.tail = NULL; 
		}

		m_bNeedWindowListUpdate = false;
	}
}

void	AgcWindow::MessageQueueProcessing()
{
	AgcWindowNode*	cur_node = m_listChild.head;
	while(cur_node)
	{
		cur_node->pWindow->MessageQueueProcessing();
		cur_node = cur_node->next;
	}

	MessageUpdate();
}

void AgcWindow::MousePositionToUVCoordinate( RwV2d* pPos , FLOAT* pU , FLOAT* pV )
{
	ASSERT( g_pEngine );
	ASSERT( g_pEngine->m_pCamera );
	ASSERT( pPos );

	RwRaster* pRaster = RwCameraGetRaster( g_pEngine->m_pCamera );
	if( !pRaster )		return;

	FLOAT fWidth = (FLOAT)RwRasterGetWidth( pRaster );
	FLOAT fHeight = (FLOAT)RwRasterGetHeight( pRaster );

	// ��ü ȭ���� ������ ����� ��.
	if( pU )
	{
		if ( 1 > fWidth ) 
			*pU = 1.0f;
		else
		{
			* pU = pPos->x / ( fWidth - 1 )	;

			if( *pU < 0.0f ) *pU = 0.0f	;
			else if( *pU > 1.0f ) *pU = 1.0f	;
		}
	}

	if( pV )
	{
		if ( 1 > fHeight )
			*pV = 1.0f;
		else 
		{
			* pV = pPos->y / ( fHeight -1 )	;

			if( *pV < 0.0f ) *pV = 0.0f	;
			else if( *pV > 1.0f ) *pV = 1.0f	;
		}
	}
}

BOOL AgcWindow::PostMessage	( INT32 message , PVOID pParam1 , PVOID pParam2	)
{
	MessagePush( message, pParam1, pParam2 );

	return TRUE;
}

BOOL	AgcWindow::SendMessage	( INT32 message , PVOID pParam1 , PVOID pParam2	)
{
	return WindowControlMessageProc( message , pParam1 , pParam2 );
}

BOOL AgcWindow::IsFocused		() // �� �����찡 ��Ŀ���� ������ �ִ��� ����.
{
	if( !pParent )		return TRUE; // ��Ʈ�� ������ �׽� ��Ŀ���� �����̴�.
		
	return pParent->m_pFocusedChild == this ? TRUE : FALSE;
}

void AgcWindow::SetFocus()	// ��Ŀ���� �����´�..
{
	if( !pParent )		return;

	// �ߺ��˻�.
	AgcWindowNode*	cur_node = pParent->m_listSetTop.head;
	while(cur_node)
	{
		if( cur_node->pWindow == this )	return;
		cur_node = cur_node->next;
	}

	pParent->WindowListLinkTail(&pParent->m_listSetTop,this);
	pParent->m_bNeedWindowListUpdate	= TRUE;
}

void AgcWindow::KillFocus		()	// ��Ŀ���� ���δ�.. KILLFOCUS �޼����� ��ߵǴµ�, ���ش�.
{
	AgcWindowNode*		cur_node = m_listChild.head;
	while(cur_node)
	{
		// ������ ã��.
		cur_node->pWindow->KillFocus();
		cur_node = cur_node->next;
	}

	if( this == m_pFocusedChild )
	{
		m_pFocusedChild->PostMessage( MESSAGE_KILLFOCUS );
		m_pFocusedChild = NULL;
	}

	if( this == g_pEngine->m_pFocusedWindow )
	{
		g_pEngine->m_pFocusedWindow = NULL;
	}

	// MouseOver �Ǿ� �ִ� �������� Check 
	if( this == g_pEngine->m_pMouseOverUI )
	{
		// MESSAGE_MOUSEOVER �� ���� ���� ��ؾ� �ϳ� ���� SET, KILL FOCUS�� ���� Control�� ���� ���� �����ϰڽ��ϴ�. - 98pastel
		g_pEngine->m_pMouseOverUI->PostMessage( MESSAGE_KILLFOCUS );
		g_pEngine->m_pMouseOverUI	=	NULL;
	}
}

AgcWindow *	AgcWindow::GetFocusedWindow() // Get the window currently has focus.
{
	return m_listChild.head ? m_listChild.tail->pWindow : NULL;
}

AgcWindow *	AgcWindow::WindowFromPoint		( INT32 x , INT32 y, BOOL bCheckUseInput )
{
	AgcWindowNode*		cur_node = m_listChild.tail;
	AgcWindow*			child_window;
	while(cur_node)
	{
		if(cur_node->pWindow->HitTest( x , y ) )
		{
			// Reculsive Call ..
			child_window = cur_node->pWindow->WindowFromPoint( x, y, bCheckUseInput );
			if( child_window )
				return child_window;
		}
	
		cur_node = cur_node->prev;
	}

	if ( (!bCheckUseInput || m_Property.bUseInput) && HitTest( x , y ) )
		return this;

	return NULL;
}

BOOL	AgcWindow::SetModal()
{
	return g_pEngine->PushMonopolyWindow( this );
}

BOOL	AgcWindow::ReleaseModal()
{
	return g_pEngine->PopMonopolyWindow( this );
}

BOOL	AgcWindow::MovableOn		( RsMouseStatus *ms )
{
	if ( !g_pEngine->PushMonopolyWindow( this ) )	return FALSE;
	if ( !m_Property.bMovable )						return FALSE;

	m_bMoveMovableWindow = TRUE;

	ClientToScreen( *ms );
	
	// ���� Position�� Mouse��ġ���� ����� ��ǥ�� �����Ѵ� 
	m_v2dDragPoint.x = ms->pos.x - (RwReal)x;
	m_v2dDragPoint.y = ms->pos.y - (RwReal)y;

	return TRUE;
}

BOOL	AgcWindow::MovableOff		(					)
{
	g_pEngine->PopMonopolyWindow( this );
	m_bMoveMovableWindow = FALSE;
	
	return TRUE;
}

BOOL	AgcWindow::BeInWindowRect( INT32 lScreenX, INT32 lScreenY )
{
	int		posx = 0 , posy = 0 ;
	ClientToScreen( &posx , &posy );

	if( lScreenX >= posx && lScreenX < posx + w && lScreenY >= posy && lScreenY < posy + h )
	{
		return TRUE;
	}
	
	return FALSE;
}

void	AgcWindow::WindowRender()
{
	if( !m_Property.bVisible ) return;

			// �������� ��Ʈ�� ���� �����Ѵ�.
	OnWindowRender	( );

	AgcWindowNode*	cur_node = m_listChild.head;		// ���ϵ� �����츦 �� ���鼭
	while(cur_node)
	{
		cur_node->pWindow->WindowRender();
		cur_node = cur_node->next;
	}
}

void AgcWindow::SetDialogMSGFuncClass(DIALOG_MSG_FUNC pfDialogMsgFunc, PVOID pClass)
{
	RsSetDialogMsgFunc(pfDialogMsgFunc);
	RsSetDialogMsgFuncClass(pClass);
}

// ----------------------------- AgcModalWindow -----------------------------
AgcModalWindow::AgcModalWindow()
{
	memset( m_szMessage, 0, sizeof( m_szMessage ) );
	m_nModalReturnValue		=	0	;
	m_lModalCount			=	0	;
}

void	AgcModalWindow::SetMessage  ( char*	pszMessage		)
{
	if ( !pszMessage )
	{
		m_szMessage[0] = NULL;
		return;
	}

	strncpy( m_szMessage, pszMessage, (AGCMODALWINDOW_MESSAGE_MAX - 1));
}

INT32		AgcModalWindow::DoModal		( AgcWindow * pParentWindow	)
{
	OnPreDoModal();

	ASSERT( NULL != pParentWindow );
	
	// ���ϰ� �ʱ�ȭ..
	m_nModalReturnValue	= 0;

	if( !m_lModalCount )
	{
		if( !pParentWindow->AddChild( this, 0, FALSE ) ) return 0;

		// ����..
		VERIFY( g_pEngine->OpenModalWindow( this ) );
	}

	INT32	lModalCount	= m_lModalCount;

	++g_pEngine->m_lIdleModal;
	++m_lModalCount;

	while( !RsGlobal.quit )
	{
		if( PeekMessage(&g_msgLastReceived, NULL, 0U, 0U, PM_REMOVE|PM_NOYIELD) )
		{
			if( g_msgLastReceived.message == WM_QUIT )
			{
				break;
			}
			else
			{
				if( g_pEngine->GetCharCheckState() )
				{
					TranslateMessage(&g_msgLastReceived);
				}
				DispatchMessage(&g_msgLastReceived);
			}
		}
		else if( g_pEngine->m_nActivated != AgcEngine::AGCENGINE_SUSPENDED	)
		{
			/* application is idling - do all background processing */
			RsEventHandler( rsIDLE, NULL );
		}
		else
		{
			WaitMessage();
		}

		if( m_lModalCount == lModalCount )
			break;
	}

	--g_pEngine->m_lIdleModal;

	g_pEngine->SetImmediateIdleTerminateFlag();

	return m_nModalReturnValue;
}

void		AgcModalWindow::EndDialog	( INT32 nRetValue		)
{
	if (m_lModalCount <= 0)
		return;

	m_nModalReturnValue = nRetValue;

	--m_lModalCount;

	if( !m_lModalCount )
	{
		pParent->DeleteChild( this, FALSE );

		VERIFY( g_pEngine->CloseModalWindow( this ) );
	}

	return;
}