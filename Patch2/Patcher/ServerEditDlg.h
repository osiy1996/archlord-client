#pragma once
#include "afxcmn.h"


// CServerEditDlg ��ȭ �����Դϴ�.

class CServerEditDlg : public CDialog
{


	DECLARE_DYNAMIC(CServerEditDlg)

public:
	CServerEditDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CServerEditDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ADD_EDIT_DIALOG };

	VOID					SetName( const CString& str )	{ m_NameData	=	str;	}
	const CString&			GetName( VOID )					{ return m_NameData;		}

	VOID					SetIP( CString& strIP )			{ m_IPData		=	strIP;	}
	CString&				GetIP( VOID )					{ return m_IPData;			}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

	CString				m_NameData;
	CString				m_IPData;

public:
	afx_msg void OnBnClickedOk();
};
