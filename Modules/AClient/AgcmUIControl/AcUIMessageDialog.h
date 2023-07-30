#ifndef __ACUI_MESSAGE_DIALOG_H__
#define __ACUI_MESSAGE_DIALOG_H__

#pragma once

#include <AgcmUIControl/AcUIControlHeader.h>
#include <AgcmUIControl/AcUIModalImage.h>

#define ACUIMESSAGEDIALOG_WIDTH							412
#define	ACUIMESSAGEDIALOG_HEIGHT						180
#define ACUIMESSAGEDIALOG_MESSAGE_LENGTH				256
#define	ACUIMESSAGEDIALOG_OK_BUTTON_X					173
#define	ACUIMESSAGEDIALOG_OK_BUTTON_Y					140
#define	ACUIMESSAGEDIALOG_OK_BUTTON_WIDTH				64
#define	ACUIMESSAGEDIALOG_OK_BUTTON_HEIGHT				17
#define ACUIMESSAGEDIALOG_OK_BUTTON						1

#define	ACUIOKCANCELDIALOG_OK_BUTTON_X					135
#define	ACUIOKCANCELDIALOG_OK_BUTTON_Y					140
#define	ACUIOKCANCELDIALOG_CANCEL_BUTTON_X				213
#define	ACUIOKCANCELDIALOG_CANCEL_BUTTON_Y				140
#define	ACUIOKCANCELDIALOG_BUTTON_WIDTH					64
#define	ACUIOKCANCELDIALOG_BUTTON_HEIGHT				17
#define ACUIOKCANCELDIALOG_OK_BUTTON					1
#define ACUIOKCANCELDIALOG_CANCEL_BUTTON				2

#ifdef _AREA_GLOBAL_
#define ACUIOKCANCELDIALOG_FIRST_BTN_X					100
#define ACUIOKCANCELDIALOG_FIRST_BTN_Y					140
#endif

#define	ACUI_MESSAGE_MAX_STRING_PIXEL_PER_LINE			( ACUIMESSAGEDIALOG_WIDTH - 100 )
#define	ACUI_MESSAGE_MAX_STRING_PER_LINE				200	// extent�� ����ϹǷ� ũ�� ��Ƴ��´�.
#define ACUI_MESSAGE_MAX_LINE							7

#define ACUI_MESSAGE_FONT_HEIGHT						14

#ifdef _AREA_GLOBAL_
#define ACUI_MESSAGE_BUTTONG_LENGTH						20
#else
#define ACUI_MESSAGE_BUTTONG_LENGTH						16
#endif

enum 
{
	UICM_MESSAGEDIALOG_MAX_MESSAGE						= UICM_BASE_MAX_MESSAGE
};

//---------------------- AcUIWaitingDialog -----------------------
class AcUIWaitingDialog : public AcUIModalImage
{
public:
	AcUIWaitingDialog( void );
	virtual ~AcUIWaitingDialog( void );

	virtual void			SetMessage					( char* szMessage );
	virtual BOOL			OnInit						( void );
	virtual void			OnWindowRender				( void );
	virtual BOOL			OnCommand					( INT32 nID, PVOID pParam );

	VOID					SetButtonName				( char* szName )
	{
		if( szName )
		{
			strcpy( m_szButtonName, szName );
		}
		else 
		{
			m_szButtonName[ 0 ] = NULL;
		}
	}

public:
	static const DWORD									s_dwWaitMaxTick;

	AcUIButton											m_clClose;
	CHAR												m_szButtonName[ ACUI_MESSAGE_BUTTONG_LENGTH ];

	CHAR												m_aszMessageLine[ ACUI_MESSAGE_MAX_LINE ][ ACUI_MESSAGE_MAX_STRING_PER_LINE + 1 ];
	INT32												m_lLine;

	//�����ð��� ������.. �����ư�� Ȱ��ȭ �����ִ�.. Logic
	DWORD												m_dwOpenTick;
};

//---------------------- AcUIMessageDialog -----------------------
class AcUIMessageDialog : public AcUIModalImage
{
public:
	AcUIMessageDialog( void );
	virtual ~AcUIMessageDialog( void );

	virtual void			SetMessage					( char* szMessage );

	virtual BOOL			OnInit						( void );
	virtual void			OnWindowRender				( void );
	virtual BOOL			OnCommand					( INT32 nID, PVOID pParam );
	virtual BOOL			OnKeyDown					( RsKeyStatus* ks );

	VOID					SetButtonName				( CHAR* szOK );

public:
	AcUIButton											m_clOK;
	INT32												m_lLine;

	CHAR												m_szNameOK[ ACUI_MESSAGE_BUTTONG_LENGTH ];
	CHAR												m_aszMessageLine[ ACUI_MESSAGE_MAX_LINE ][ ACUI_MESSAGE_MAX_STRING_PER_LINE + 1 ];
};

//---------------------- AcUIOKCancelDialog -----------------------
class AcUIOKCancelDialog : public AcUIMessageDialog //public AcUIModalImage
{
public:
	AcUIOKCancelDialog( void );
	~AcUIOKCancelDialog( void );

	virtual BOOL			OnInit						( void );
	virtual BOOL			OnCommand					( INT32 nID, PVOID pParam );	
	virtual BOOL			OnKeyDown					( RsKeyStatus* ks );

	VOID					SetButtonName				( CHAR *szOK, CHAR *szCancel );

public:
	AcUIButton											m_clCancel;
	INT32												m_lOKCancelMode;

	CHAR												m_szNameCancel[ ACUI_MESSAGE_BUTTONG_LENGTH ];
};
#ifdef _AREA_GLOBAL_
//---------------------- AcUIOKCancelDialog -----------------------
class AcUIThreeBtnDialog : public AcUIMessageDialog //public AcUIModalImage
{
public:
	AcUIThreeBtnDialog( void );
	~AcUIThreeBtnDialog( void );

	virtual BOOL			OnInit						( void );
	virtual BOOL			OnCommand					( INT32 nID, PVOID pParam );
	virtual BOOL			OnKeyDown					( RsKeyStatus* ks );

public:
	AcUIButton	m_clOneBtn;
	AcUIButton	m_clTwoBtn;

	CHAR	m_szOneBtn[ ACUI_MESSAGE_BUTTONG_LENGTH ];
	CHAR	m_szTwoBtn[ ACUI_MESSAGE_BUTTONG_LENGTH ];
};
#endif
//---------------------- AcUIEditOKDialog -------------------------
class AcUIEditOKDialog : public AcUIMessageDialog
{
public :
	AcUIEditOKDialog( void );
	~AcUIEditOKDialog( void );

	virtual BOOL			OnInit						( void );
	virtual void			OnWindowRender				( void );
	virtual BOOL			OnCommand					( INT32 nID, PVOID pParam );
	virtual BOOL			OnKeyDown					( RsKeyStatus* ks );

	VOID					SetButtonName				( CHAR* szOK );

public :
	AcUIEdit											m_clEdit;
};


#endif