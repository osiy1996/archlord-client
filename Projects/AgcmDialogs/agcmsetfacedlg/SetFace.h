#pragma once
#include "afxwin.h"
#include "ApBase.h"

#include <rwcore.h>
#include <rpworld.h>
#include <rplodatm.h>
#include <rtcharse.h>
#include <rttiff.h>
#include <rtpng.h>
#include <rtbmp.h>
#include <rphanim.h>
#include <rtanim.h>
#include <rpskin.h>
#include <rpusrdat.h>


// CSetFace ��ȭ �����Դϴ�.

class CSetFace : public CDialog
{
	DECLARE_DYNCREATE(CSetFace)

public:
	BOOL		SetFaceAtomic(RpAtomic *pstAtomic);

	CSetFace(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CSetFace();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_CHARACTER_FACE };
	CComboBox	m_csFaceAtomic;

protected:
	RpAtomic*	m_pstFaceAtomic;
	RpClump*	m_pstClump;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	virtual BOOL OnInitDialog();

	afx_msg void OnCbnSelchangeFaceAtomic();
	afx_msg void OnOk();
	afx_msg void OnCancel();

	DECLARE_MESSAGE_MAP()
};
