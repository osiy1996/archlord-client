#if !defined _AGCWINDOW_H_
#define _AGCWINDOW_H_

#include <list>

using namespace std;

class	AgcWindowProperty
{
public:
	BOOL	bUseInput	;	// �Է� ����
	BOOL	bMovable	;	// �̵� ����
	BOOL	bModal		;	// �� ������ ���� Ű&���콺 �Է��� ����.
							// ���� ���ϵ�� ���ӵ�;
	BOOL	bTopmost	;	// ���ϵ� ������ ž ��Ʈ ���

	BOOL	bVisible	;	// ���̴��� �Ⱥ��̴��� ����.

	AgcWindowProperty() : bUseInput(TRUE), bMovable(FALSE), bModal(FALSE), bTopmost( FALSE ), bVisible(TRUE)		{		}
};

class AgcWindow;
struct	AgcWindowNode
{
	AgcWindowNode*		next;
	AgcWindowNode*		prev;

	AgcWindow*			pWindow;
};

struct	AgcWindowList
{
	AgcWindowNode*		head;
	AgcWindowNode*		tail;
};

class AgcWindow;
class CMessageMng
{
	// Message Structure
	struct	Message
	{
		Message( INT32 id, PVOID param1, PVOID param2 ) : message(id), pParam1(param1), pParam2(param2)		{		}

		INT32		message;
		PVOID		pParam1;
		PVOID		pParam2;
	};
	typedef list<Message>			MessageList;
	typedef MessageList::iterator	MessageListItr;

public:
	CMessageMng( AgcWindow* pAgcWindow );
	~CMessageMng();

	void	MessagePush( INT32 id, PVOID param1, PVOID param2 );
	void	MessageUpdate();

public:
	static const INT32	s_MessageMax = 100;

protected:
	AgcWindow*		m_pAgcWindow;	//��ȣ�����̴� ��.��
	MessageList		m_listMessage;
};

class AcUIBase;
class AgcmUIManager2;
class AgcmTargeting;
class AgcWindow : public CMessageMng
{
public:
	friend class AgcEngine;
	friend class AgcmUIManager2;
	friend class AgcmUIControl;
	friend class AgcmTargeting;

	enum
	{
		// �޽��� �ε���			// pParam				// ����.
		MESSAGE_INIT			,	// void
		MESSAGE_CLOSE			,	// void
		MESSAGE_PRERENDER		,	// RwRaster *
		MESSAGE_RENDER			,	// RwRaster *
		MESSAGE_POSTRENDER		,	// RwRaster *
		MESSAGE_KEYDOWN			,	// RsKeyStatus *
		MESSAGE_KEYUP			,	// RsKeyStatus *
		MESSAGE_CHAR			,	// char
		MESSAGE_IMECOMPOSING	,	//
		MESSAGE_LBUTTONDOWN		,	// RsMouseStatus *
		MESSAGE_LBUTTONDBLCLK	,	// RsMouseStatus *
		MESSAGE_LBUTTONUP		,	// RsMouseStatus *
		MESSAGE_RBUTTONDOWN		,	// RsMouseStatus *
		MESSAGE_RBUTTONDBLCLK	,	// RsMouseStatus *
		MESSAGE_MBUTTONUP		,	// RsMouseStatus *
		MESSAGE_MBUTTONDOWN		,	// RsMouseStatus *
		MESSAGE_MBUTTONDBLCLK	,	// RsMouseStatus *
		MESSAGE_RBUTTONUP		,	// RsMouseStatus *		
		MESSAGE_MOUSEMOVE		,	// RsMouseStatus *
		MESSAGE_MOUSEWHEEL		,	// BOOL
		MESSAGE_IDLE			,	// 

		MESSAGE_SETFOCUS		,
		MESSAGE_KILLFOCUS		,

		MESSAGE_MOUSEACTIVATE	,	// ���콺�� ������ �������� ���´�.
		MESSAGE_MOUSELEAVE		,	// ���콺�� ������ ������ �����.
		
		MESSAGE_COMMAND			,	//  ��Ʈ�� Ŀ�ǵ� �޽���.. ���ڴ� �������.;;
		MESSAGE_DRAG_DROP		,	//  Drag�� ���ٴ� �޼��� 

		MESSAGE_IME_COMPOSING	,	// IME ó��..

		MESSAGE_POSTINIT		,	// Init�� �������� �Ѿ��..Init�� ������ �����͵� ������ ������ üũ��..

		MESSAGE_EDIT_ACTIVE		,	// ActiveEdit�� �ɶ�
		MESSAGE_EDIT_DEACTIVE	,	// ActiveEdit���� �����ɶ�

		MESSAGE_COUNT
	};

	INT32			m_nType;				// ��Ʈ���� Ÿ��..(0�ϰ�� Control�� �ƴϴ�)

	BOOL			m_bUIWindow			;	// AcUIBase�� ��ӹ��� UI Window�ΰ�?

	BOOL			m_bOpened			;
	BOOL			m_bUseDrag			;	// ���콺�� �����츦 �巹�� �� �� �ִ��� ����

	AgcWindow*		pParent				;	// ���� �������� ������ ( �ֻ����� ��� NULL );
	AgcWindow*		m_pFocusedChild		;	// ���� �������� ����༮�� ��Ŀ���� ������ �ִ��� ����.

	// AuList�������� ��ü list�� Ʃ��..(2004.10.27 gemani)
	// Windows list handling
	AgcWindowList	m_listChild			;	// ���� �������� ������ ( ����Ʈ );
	
	AgcWindowList	m_listSetTop		;	// Set Top�� �����ϱ� ���� ����.
	AgcWindowList	m_listAdd			;	// �߰��� ť..

	INT32			m_lControlID		;	// ID!!! �����찡 �ʿ��Ұ�� ���� ����Ѵ�.

	INT16			x, y, w, h;
	INT32			m_lUserData_1;		// User�� ��ü������ ����Ѵ�. 

	// Window Movable�� ����..
	RwV2d			m_v2dDragPoint;		// Window�� �����ǿ��� Ŭ�� ��ġ ��� ��ǥ 
	BOOL			m_bMoveMovableWindow;		// Window�� �ű�� �ִ� ��

	BOOL			m_bDeleteThis;
	
	BOOL			m_bVisibleChildWindowForShink		;		// Shink�� Visible�� Child�������� ������ ��� �ӽð��� - default : FALSE

	BOOL			m_bActiveEdit			;		// �� Window�� Active�� Edit �ΰ�?

	BOOL			m_bUseModalMessage		;		// �ٸ� �����찡 Modal�� ���־ �޼����� �޾ƾ� �ϴ°�?

	ApBase*			m_pcsOwnerBase;

	AgcWindowProperty	m_Property;	// ������ �Ӽ� ����.

protected:
	bool	m_bNeedWindowListUpdate	;

	void	MessageQueueProcessing();

	void	WindowListUpdate();
	void	WindowListUnlink(AgcWindowList*		pList,AgcWindow*	pRemoveWindow);
	void	WindowListLinkTail(AgcWindowList*	pList,AgcWindow*	pAddWindow);
	void	WindowListLinkBefore(AgcWindowList*	pList,AgcWindow*	pBaseWindow, AgcWindow*	pAddWindow);
	
public:
	// Operations
	AgcWindow();
	virtual ~AgcWindow();

	virtual	BOOL OnInit()							{	return TRUE;	}	// �ʱ�ȭ , ����Ÿ ���� & �׷��� ����Ÿ �ε�.	WindowUI �ǰ�� Open�Ŵ� ����, Full UI�� ��� Setting�Ŵ� ����.
	virtual BOOL OnPostInit()						{	return TRUE;	}
	virtual	void OnClose()							{					}
	virtual BOOL OnIdle( UINT32 ulClockCount )		{	return TRUE;	}

	virtual	void OnPreRender	( RwRaster *raster	)	{					}
	virtual	void OnRender		( RwRaster *raster	)	{					}
	virtual	void OnPostRender	( RwRaster *raster	)	{					}
	virtual	void OnWindowRender	()						{					}

	virtual	BOOL OnKeyDown		( RsKeyStatus *ks	)		{	return FALSE;	}
	virtual	BOOL OnKeyUp		( RsKeyStatus *ks	)		{	return FALSE;	}
	virtual	BOOL OnLButtonDown	( RsMouseStatus *ms	)		{	return FALSE;	}
	virtual	BOOL OnLButtonDblClk( RsMouseStatus *ms	)		{	return FALSE;	}
	virtual	BOOL OnLButtonUp	( RsMouseStatus *ms	)		{	return FALSE;	}
	virtual	BOOL OnRButtonDown	( RsMouseStatus *ms	)		{	return FALSE;	}
	virtual	BOOL OnRButtonDblClk( RsMouseStatus *ms	)		{	return FALSE;	}
	virtual	BOOL OnMButtonUp	( RsMouseStatus *ms	)		{	return FALSE;	}
	virtual	BOOL OnMButtonDown	( RsMouseStatus *ms	)		{	return FALSE;	}
	virtual	BOOL OnMButtonDblClk( RsMouseStatus *ms	)		{	return FALSE;	}
	virtual	BOOL OnRButtonUp	( RsMouseStatus *ms	)		{	return FALSE;	}
	virtual	BOOL OnMouseMove	( RsMouseStatus *ms	)		{	return FALSE;	}
	virtual	BOOL OnMouseWheel	( INT32	lDelta		)		{	return FALSE;	}

	virtual	void OnSetFocus		(					)		{		}
	virtual	void OnKillFocus	(					)		{		}
	virtual void OnMouseActivate(					)		{		}
	virtual void OnMouseLeave	(					)		{		}

	virtual BOOL OnCommand		( INT32	nID , PVOID pParam	){ return TRUE; }
	virtual BOOL OnDragDrop		( PVOID pParam1 , PVOID pParam2  ){ return TRUE;	}

	virtual	BOOL OnChar			( char * pChar		, UINT lParam ) { return FALSE; }
	virtual	BOOL OnIMEComposing	( char * composing	, UINT lParam ) { return FALSE; }
	virtual BOOL OnIMEReading	(								  ) { return FALSE; }	// reading window �׸���
	virtual BOOL OnIMEHideReading(								  ) { return FALSE; }	// reading window ���߱�
	virtual BOOL OnIMECandidate	(								  ) { return FALSE; }	// candidate window �׸���
	virtual BOOL OnIMEHideCandidate(							  ) { return FALSE; }	// candidate window ���߱�

	virtual void OnMoveWindow()		{		}
	virtual	void OnPreRemoveThis()	{		}	// ��Ʈ ����Ʈ���� ������ ������ ������ ���x�Լ�.

	virtual void OnEditActive()		{		}
	virtual void OnEditDeactive()	{		}

public:	
	virtual void WindowRender();					// Window �� ������ Render�ϴ� �Լ� 
	virtual	BOOL HitTest( INT32 x , INT32 y	);	// �ش� ����Ʈ�� �� ��⿡ �����ִ��� ����. âUI���� �߿���. x,y �� ������ Screen ��ǥ�� �ǰ���.

	// ������ Ű����/���콺�� �Է��� �� ���ϵ� �����쿡 �ѷ��ֱ� ���ؼ� , �������� ����ϴ� �Լ�.
	// ���� ���� ��Ʈ�� �ϴ� ���� ������?..
	virtual	RsEventStatus	UserInputMessageProc		( RsEvent event , PVOID param									); // Ű����&���콺 �޽��� ����
	virtual	BOOL			WindowControlMessageProc	( INT32 message , PVOID pParam1 = NULL , PVOID pParam2	= NULL	);
	virtual BOOL			PreTranslateInputMessage	( RsEvent event , PVOID param									){ return FALSE; }
	virtual void			SetDialogMSGFuncClass( DIALOG_MSG_FUNC pfDialogMsgFunc, PVOID pClass );		// ��޸��� ���̾�α� �޽����� ó���ϱ� ���ؼ� ����Ѵ�. 2004.01.12. steeple

public:
	BOOL		Close();

	BOOL			IsFocused();
	virtual void	SetFocus();
	void			KillFocus();
	AgcWindow*		GetFocusedWindow();
	
	void		ClientToScreen( RsMouseStatus &ms );
	void		ScreenToClient( RsMouseStatus &ms );
	void		ClientToScreen( int *px , int *py );
	void		ScreenToClient( int *px , int *py );

	void		MousePositionToUVCoordinate( RwV2d* pPos , FLOAT* pU , FLOAT* pV );

	// ���ϵ� ������ ���̴� �Լ�.
	BOOL		AddChild( AgcWindow * pWindow	, INT32 nid = 0			, BOOL bInit = TRUE );
	BOOL		DeleteChild( AgcWindow * pWindow	, BOOL bClose = TRUE	, BOOL bDeleteMemory = FALSE );
	BOOL		DeleteChild( INT32			nid		, BOOL bClose = TRUE	, BOOL bDeleteMemory = FALSE );
	// ���⿡ ���Ŵ� �����͵��� ������ ��ü�� ����Ʈ �ع�����.
	// ���� , ���ϵ�� ���� �������� ���� ������ �����͸� new�� �����Ͽ�
	// ��Ʈ�� �Ѱ��� �� , �Ű��� ���� �ʾƵ� �´�.
	// ���ϵ� �޽��� �ʾƼ��� ȣ���ؼ��� �ȉ´�.

	void		MoveWindow( INT32 x , INT32 y , INT32 w, INT32 h );
	BOOL		MovableOn( RsMouseStatus *ms );
	BOOL		MovableOff();
	
	// �����츦 ������ü���� ������ ���� ����. �Էµ� ���� ����.
	void		ShowWindow( BOOL bVisible = TRUE )	{	m_Property.bVisible	= bVisible;	}
	
	AgcWindow*	WindowFromPoint( INT32 x , INT32 y, BOOL bCheckUseInput = TRUE );

	BOOL		PostMessage	( INT32 message , PVOID pParam1 = NULL , PVOID pParam2	= NULL	);
	BOOL		SendMessage	( INT32 message , PVOID pParam1 = NULL , PVOID pParam2	= NULL	);

	void		SetUseDrag( BOOL bUse )		{	m_bUseDrag = bUse;	}
	BOOL		BeDraged()					{	return m_bUseDrag;	}

	INT32		GetControlID()				{	return m_lControlID;	}
	
	BOOL		SetModal();
	BOOL		ReleaseModal();
	BOOL		BeInWindowRect( INT32 lScreenX, INT32 lScreenY );			// �ܼ��� Window�ȿ� ����ִ°��� �Ǵ����� - Visible���ο� �������
	
	BOOL		GetActiveEdit()		{	return m_bActiveEdit;	}
	ApBase*		GetOwnerBase()		{	return m_pcsOwnerBase;	}
	BOOL		SetOwnerBase(ApBase *pcsOwner)	{	return (m_pcsOwnerBase = pcsOwner) ? TRUE : FALSE;	}

	INT32		GetDepth();
	BOOL		ReculsiveWindowControlMessageProc( INT32 message, PVOID pParam1 = NULL, PVOID pParam2 = NULL );		// �޽����� ���� ������ ��ο��� ������.

protected:
	void GetDepthRecurse(AgcWindow* pCurWindow, INT32* pDepth);
};

#define		AGCMODALWINDOW_MESSAGE_MAX			256

class AgcModalWindow : public AgcWindow
{
public:
	AgcModalWindow();

	virtual	BOOL	OnPreDoModal()					{	return TRUE;	}
	virtual	void	SetMessage( char* szMessage	);

	INT32			DoModal( AgcWindow* pParent );
	void			EndDialog( INT32 nRetValue );

protected:
	INT32		m_nModalReturnValue;
	CHAR		m_szMessage[AGCMODALWINDOW_MESSAGE_MAX];
	INT32		m_lModalCount;								// ���� �� Modal Dialog�� Modal �� Count (�� dialog�� ������ DoModal() �Ǿ��� ��� ó��	

public :
	INT32			GetReturnValue( void )	{ return m_nModalReturnValue; }
};

#endif