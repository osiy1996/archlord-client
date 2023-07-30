#include "afxwin.h"
#if !defined(AFX_OPTIONDIALOG_H__02B55982_6359_4204_A9FB_FED4C242B678__INCLUDED_)
#define AFX_OPTIONDIALOG_H__02B55982_6359_4204_A9FB_FED4C242B678__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptionDialog dialog

class COptionDialog : public CDialog
{
// Construction
public:
	COptionDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COptionDialog)
	enum { IDD = IDD_OPTION };
	BOOL	m_bUsePolygonLock;
	BOOL	m_bUseBrushPolygon;
	BOOL	m_bCameraBlocking;
	BOOL	m_bShadow2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	// ������Ʈ �ٿ�� ���Ǿ� ǥ��
	BOOL m_bShowObjectBoundingSphere;
	// ������Ʈ �ݸ��� ����� ǥ��
	BOOL m_bShowObjectCollisionAtomic;
	// ������Ʈ ��ŷ ����� ǥ��
	BOOL m_bShowObjectPickingAtomic;
	// ������Ʈ ��  ������ ���� ǥ��
	BOOL m_bShowObjectRealPolygon;

	// ������Ʈ â���� ��Ʈ�� ���� ǥ��.
	BOOL m_bShowOctreeInfo;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONDIALOG_H__02B55982_6359_4204_A9FB_FED4C242B678__INCLUDED_)
