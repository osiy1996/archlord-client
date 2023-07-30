// AcUIButton.h: interface for the AcUIButton class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACUIBUTTON_H__9862A601_119C_4D25_AEC1_98DE27B959CC__INCLUDED_)
#define AFX_ACUIBUTTON_H__9862A601_119C_4D25_AEC1_98DE27B959CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <AgcmUIControl/AcUIBase.h>

#define	 ACUIBUTTON_H_MAX_STRING				256
#define	 ACUIBUTTON_INIT_INDEX					-1

// Command Message
enum 
{
	UICM_BUTTON_MESSAGE_CLICK					= UICM_BASE_MAX_MESSAGE,
	UICM_BUTTON_MESSAGE_PUSHDOWN,
	UICM_BUTTON_MESSAGE_PUSHUP,
	UICM_BUTTON_MESSAGE_DRAG_DROP,
	UICM_BUTTON_MAX_MESSAGE
};

// Button Mode 
enum
{
	ACUIBUTTON_MODE_NORMAL						= 0,
	ACUIBUTTON_MODE_ONMOUSE, 
	ACUIBUTTON_MODE_CLICK, 
	ACUIBUTTON_MODE_DISABLE,
	ACUIBUTTON_MAX_MODE,
};

typedef struct
{
	BOOL										m_bPushButton;
	BOOL										m_bStartOnClickStatus;
	BOOL										m_bClickOnButtonDown;
} AcUIButtonProperty;

class AcUIGridItem;

class AcUIButton : public AcUIBase
{
public:
	AcUIButton( void );
	virtual ~AcUIButton( void );

protected:
	INT8										m_cButtonMode;				//Button Mode : 0-�Ϲ�, 1-��Ŀ��(���콺���ö��), 2-Ŭ��(���ʹ�ưŬ��), 3-��Ȱ�� 
	INT8										m_cPrevButtonMode;			// Disable�Ǳ� ���� Mode

public:	
	INT32										m_lButtonTextureID[ ACUIBUTTON_MAX_MODE ];	//Button Mode�� ���� Texture�� ID 
	char										m_szButtonText[ ACUIBUTTON_H_MAX_STRING ];	//Button���� Text���� 

	float										m_fTextureWidth;			// Button Texture �� Width
	float										m_fTextureHeight;			// Button Texture �� Height 
	
	static char*								m_pszButtonClickSound;		// ��ư Ŭ���� �߻��� ����

	AcUIButtonProperty							m_stProperty;				// ��ư �Ӽ�
	AcUIGridItem*								m_pcsDragDropGridItem;		// �� ��ư�� ������ Grid Item ������

	INT32										m_lButtonDownStringOffsetX;	// ��ư�� ���������� String�� X�� ��ġ ��ȭ
	INT32										m_lButtonDownStringOffsetY;	// ��ư�� ���������� String�� X�� ��ġ ��ȭ

	DWORD										m_lDisableTextColor;		// Disable Text Color

// ��ư ���۰���
public:
	void			AddButtonImage				( char * filename, UINT8 cButtonMode );
	void			SetButtonImage				( INT32 lImageID, INT8 cButtonMode );
	INT32			GetButtonImage				( INT8 cButtonMode );

	void			SetButtonEnable				( BOOL bEnable );	
	void			SetButtonMode				( UINT8	cMode, BOOL bSendMessage = FALSE );
	UINT8			GetButtonMode				( void );

// Ư�����
public :
	AcUIGridItem*	GetDragDropGridItem();

// Virtual Functions
public :
	virtual void	OnAddImage					( RwTexture* pTexture );
	virtual void	OnPostRender				( RwRaster *raster );		// ���� ������ ��.
	virtual	void	OnWindowRender				( void );
	virtual BOOL	OnLButtonDown				( RsMouseStatus *ms	);	
	virtual BOOL	OnLButtonDblClk				( RsMouseStatus *ms	);
	virtual BOOL	OnLButtonUp					( RsMouseStatus *ms );
	virtual BOOL	OnMouseMove					( RsMouseStatus *ms );
	virtual BOOL	OnDragDrop					( PVOID pParam1, PVOID pParam2 );
	virtual void	OnSetFocus					( void );
	virtual void	OnKillFocus					( void );

/**

	Not Use.. Commented in 2008. 06. 12. by ����
public :
	void			SetDisableTextColor			( DWORD lColor ) { m_lDisableTextColor = lColor; }

*/


};

class AcUIPushButton : public AcUIButton
{
public:
	AcUIPushButton( void )						{ m_stProperty.m_bPushButton = TRUE; }
	virtual ~AcUIPushButton( void )				{	}
};

#endif // !defined(AFX_ACUIBUTTON_H__9862A601_119C_4D25_AEC1_98DE27B959CC__INCLUDED_)
