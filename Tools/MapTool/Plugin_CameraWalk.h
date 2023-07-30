#if !defined(AFX_PLUGIN_CAMERAWALK_H__A27CF605_E799_40E3_AAB0_542A19C36CE9__INCLUDED_)
#define AFX_PLUGIN_CAMERAWALK_H__A27CF605_E799_40E3_AAB0_542A19C36CE9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Plugin_CameraWalk.h : header file
//

#include "MyEngine.h"
#include "UITileList_PluginBase.h"
#include "AgcuCamPathWork.h"
#include "AxisManager.h"

/////////////////////////////////////////////////////////////////////////////
// CPlugin_CameraWalk window

#define CW_MAX_SPLINE_DISPLAY_VERTEX	1000

class CPlugin_CameraWalk : public CUITileList_PluginBase
{
// Construction
public:
	CPlugin_CameraWalk();

// Attributes
public:
	/*enum	CAMERA_WALKING_TYPE
	{
		CWT_AIRPLANE		,
		CWT_TOWARDTARGET	,
		CWT_TYPEMAX
	};
	*/

	// �� ��Ʈ�ѵ��� id �ο�
	enum	CONTROL_ID
	{
		CWCID_BUTTONSETTING				= 1001,
		CWCID_BUTTONADD					= 1002,
		CWCID_BUTTONREMOVE				= 1003,
		CWCID_BUTTONSETPLAYTIME			= 1004,
		CWCID_BUTTONPLAY				= 1005,
		CWCID_BUTTONREMOVEALL			= 1007,
		CWCID_BUTTONSAVE				= 1008,
		CWCID_BUTTONLOAD				= 1009,
		CWCID_MAX
	};

	// �� ��Ʈ�ѵ��� Ŭ������..
	CButton		m_ctlButtonSetting		;

	CButton		m_ctlButtonAdd			;
	CButton		m_ctlButtonRemove		;
	CButton		m_ctlButtonSetPlayTime	;

	CButton		m_ctlButtonPlay			;
	CButton		m_ctlButtonRemoveAll	;
	CButton		m_ctlButtonSave			;
	CButton		m_ctlButtonLoad			;

	CRect		m_rectProgress			;

public:
	// ������ ����..
	RpAtomic *	m_pSphereAtomic			;
	INT32		m_nSelectedPoint		;

	RpAtomic *	TranslateSphere( RwV3d * pPos );
	void		DrawSphere( RwV3d	* pPos );
	INT32		GetCollisionIndex( RsMouseStatus *ms );
	// -1 �� ���°�..

	CAxisManager	m_cAxis;
public:
	// ������� ����Ÿ��..
	BOOL		m_bPlay					;	// �÷���������..
	UINT32		m_uCurrent				;	// ���� ���� ��..


	AgcuPathWork *	PreparePathWork	( BOOL bTestMode = FALSE );
	AgcuPathWork *	PrepareAngleWork( BOOL bTestMode = FALSE );

	AgcuPathWork *	m_pPathWork			;	// ���ö�������
	AgcuPathWork *	m_pAngleWork		;	// �ޱ�����
	// x���� �ޱ۰����� �����.

	//RwMatrix		m_pCtrlMatrix	[ CW_MAX_SPLINE_NODE ];
	//INT32			m_nCtrlMatrixCount		;

	vector< RwMatrix >	m_vecCtrlMatrix;
	vector< FLOAT >		m_vecProjection;

	UINT32			m_uDuration				;	// ��ü����

	INT32			m_nType					;
	INT32			m_nSSType				;
	BOOL			m_bAccel				;
	BOOL			m_bClosed				;
	BOOL			m_bLoop					;

	BOOL			m_bPrevControlState		;

	RwIm3DVertex	m_pImVertex		[ CW_MAX_SPLINE_DISPLAY_VERTEX ];
	INT32			m_nImVertexCount;

	void		Stop		();
	void		RemoveAll	();
	BOOL		Save		();
	BOOL		Load		();

	void		UpdateVertex();

	// ���������� ����� �Ÿ������..
	FLOAT		m_fDistance	;	// �Ÿ�
	FLOAT		m_fTime		;	// ���޽ð�..
	UINT32		m_uRunSpeed	;	// �޸��� �ӵ�

	void		UpdateDistance( BOOL bForce = FALSE );

	INT32		m_nSelectedAxis		;
	FLOAT		m_fSelectedValue1	;
	FLOAT		m_fSelectedValue2	;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlugin_CameraWalk)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPlugin_CameraWalk();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPlugin_CameraWalk)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual BOOL OnIdle					( UINT32 ulClockCount );
	virtual BOOL OnWindowRender			();

	virtual	BOOL Window_OnLButtonDown	( RsMouseStatus *ms );
	virtual	BOOL Window_OnLButtonUp		( RsMouseStatus *ms );
	virtual BOOL Window_OnMouseMove		( RsMouseStatus *ms );
	virtual BOOL Window_OnKeyDown		( RsKeyStatus *ks );
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CDurationSetDlg dialog

class CDurationSetDlg : public CDialog
{
// Construction
public:
	CDurationSetDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDurationSetDlg)
	enum { IDD = IDD_DURATION_SET_DIALOG };
	UINT	m_uDuration;
	BOOL	m_bType;		// 2004/02/14 �Ƚ��~
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDurationSetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDurationSetDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
// CCameraWalkingSettingDlg ��ȭ �����Դϴ�.

class CCameraWalkingSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CCameraWalkingSettingDlg)

public:
	CCameraWalkingSettingDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CCameraWalkingSettingDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_CAMERAWALKSETTING };

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlugin_CameraWalk)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bAccel	;
	BOOL m_bClosed	;
	BOOL m_bLoop	;

	INT32	m_nType	;
	INT32	m_nSSType;
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_PLUGIN_CAMERAWALK_H__A27CF605_E799_40E3_AAB0_542A19C36CE9__INCLUDED_)