// AgcUIManager.h: interface for the AgcUIManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGCUIMANAGER_H__622605AB_570B_440D_9438_5976C6E237E9__INCLUDED_)
#define AFX_AGCUIMANAGER_H__622605AB_570B_440D_9438_5976C6E237E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <AgcmUIManager2/AgcdUIManager2.h>

#define	AGC_UI_RIGHT_CLICK_DELAY							200
#define AGC_UI_RIGHT_CLICK_EFFECTIVE_MAX_DISTANCE			10

class AgcmCharacter;
class AgcmUIManager2;
class AgcmUIHotkey;

class AgcUIManager : public AgcWindow  
{
private:
	AgcmUIManager2*											m_pcsAgcmUIManager2;
	AgcdUIHotkeyType										m_eKeyStatus;

	//@{ kday 20041209
	class AgcmCamera2*										m_pAgcmCamera2;
	//@} kday

	AgcmCharacter*											m_pAgcmCharacter;
	RsMouseStatus											m_PrevMouseStatus;		// ���� ���콺 ���� 
	BOOL													m_bLoginMode;			// TRUE �̸� LOGIN �� FALSE �̸� GAME�� 

	UINT32													m_ulRClickStart;		// Right Button �� ���� �ð�. �� �ð��� �� �ð��� �������� RClick�� �Ǵ��Ѵ�.
	RsMouseStatus											m_stRClickStartStatus;	// Right Button �� ������ ���� ���콺 ����

	POINT													m_stCursorPos;
	UINT32													m_ulPrevClockCount;		// ���� ClockCount
	INT32													m_lZoomStatus;			// ���� Camera�� Zoom ����
	INT32													m_lRotateStatus;		// ���� Camera�� Rotate ����
	INT32													m_lPanStatus;			// ���� Camera�� Pan ����

	//@{ 2006/06/27 burumal
	AgcmUIHotkey*											m_pAgcmUIHotkey;		// ī�޶�� ���� Hotkey ó��
	AgcdUIHotkey*											m_pHkMinus;
	AgcdUIHotkey*											m_pHkPlus;
	AgcdUIHotkey*											m_pHkLeft;
	AgcdUIHotkey*											m_pHkRight;
	AgcdUIHotkey*											m_pHkUp;
	AgcdUIHotkey*											m_pHkDown;	
	AgcdUIHotkey*											m_pHkF10;
	AgcdUIHotkey*											m_pHkF11;
	AgcdUIHotkey*											m_pHkF12;
	//@}
	
public:
	BOOL													m_bCameraMoveMode;		// ���콺 ������ ��ư�� ��ų������� ī�޶� Move �������
	BOOL													m_bRButtonClicked;		// Right Button �� ���� �ִ°��� ����

	AgcUIManager( AgcmUIManager2 *pcsAgcmUIManager2 = NULL );
	virtual ~AgcUIManager( void );
	void					SetManager						( AgcmUIManager2* pcsAgcmUIManager2 ) { m_pcsAgcmUIManager2 = pcsAgcmUIManager2; }
	
	// 2003-10-28 98pastel
	virtual BOOL			OnMouseWheel					( INT32	lDelta );
	virtual BOOL			OnMouseMove						( RsMouseStatus *ms	);
	virtual BOOL			OnIdle							( UINT32 ulClockCount );

	void					SetAgcmCameraModule				( AgcmCamera2* pAgcmCamera );
	void					SetAgcmCharacterModule			( AgcmCharacter* pAgcmCharacter );
	
	// 2003-10-28 98pastel

	BOOL					OnKeyDown						( RsKeyStatus *ks );
	BOOL					OnKeyUp							( RsKeyStatus *ks );
	BOOL					OnChar							( CHAR* pChar, UINT lParam );
	BOOL					OnLButtonDown					( RsMouseStatus *ms	);
	BOOL					OnRButtonDown					( RsMouseStatus *ms	);
	BOOL					OnMButtonDown					( RsMouseStatus *ms );
	BOOL					OnLButtonUp						( RsMouseStatus *ms );
	BOOL					OnRButtonUp						( RsMouseStatus *ms );
	BOOL					OnMButtonUp						( RsMouseStatus *ms );
	BOOL					OnRButtonDblClk					( RsMouseStatus *ms	);	
	BOOL					OnLButtonDblClk					( RsMouseStatus *ms	);	

	void					UpdateChildWindow				( void)	{ WindowListUpdate(); }
	void					SetLoginMode					( BOOL bLoginMode ){ m_bLoginMode = bLoginMode;	}
	BOOL					PreTranslateInputMessage		( RsEvent event, PVOID param );
	inline AgcdUIHotkeyType	GetKeyStatus					( void ) { return m_eKeyStatus;	}
	BOOL					SetCameraViewProcessHotkeys		( void );
	void					UpdateCameraViewProcess			( INT32 lDeltaClock );	
};

#endif // !defined(AFX_AGCUIMANAGER_H__622605AB_570B_440D_9438_5976C6E237E9__INCLUDED_)
