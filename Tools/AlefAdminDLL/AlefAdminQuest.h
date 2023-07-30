#pragma once

#include "Resource.h"
#include "afxwin.h"
#include "AgpdAdmin.h"
#include "AgpmQuest.h"

#include "AlefAdminXTExtension.h"

//ĳ���� ����Ʈ ������ ����.

class AlefAdminQuest : public CFormView
{
	DECLARE_DYNCREATE(AlefAdminQuest)

protected:
	AlefAdminQuest();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~AlefAdminQuest();

public:
	enum { IDD = IDD_QUEST };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	virtual void OnInitialUpdate();

	void ClearContent();
	BOOL OnReceiveQuestInfo(stAgpdAdminCharQuest* pstQuest);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	virtual void OnOK() {;}
	virtual void OnCancel() {;}

	DECLARE_MESSAGE_MAP()

public:
	CListBox m_csQuestList;

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
};
