#pragma once


// 2005.04.04. steeple
// �ش� ĳ���Ϳ��� ���� ��ũ�θ� �����Ű�� ���ؼ� ĳ���Ϳ� ��ũ�θ� ����.

#include "Resource.h"
#include "afxwin.h"

#include "AlefAdminXTExtension.h"

// AlefAdminCharMacro �� ���Դϴ�.

class AlefAdminCharMacro : public CFormView
{
	DECLARE_DYNCREATE(AlefAdminCharMacro)

protected:
	AlefAdminCharMacro();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~AlefAdminCharMacro();

public:
	enum { IDD = IDD_CHARMACRO };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	vector<CAlefAdminMacro*> m_vectorMacro;

public:
	virtual void OnInitialUpdate();

	BOOL LoadMacro(LPCTSTR szFileName);
	BOOL SetMacroFunction(CAlefAdminMacro* pcsMacro);
	BOOL ClearMacro();
	BOOL SaveMacro();

	BOOL ShowMacro();

	BOOL ExecuteMacro(CAlefAdminMacro* pcsMacro);
	BOOL ExecuteMacroCommand(CAlefAdminMacro* pcsMacro);

	// Macro Function
	static BOOL MF_SOS(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL MF_Disconnect(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL MF_AllSkillLearn(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL MF_AllSkillUpgrade(PVOID pData, PVOID pClass, PVOID pCustData);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	virtual void OnOK() {;}
	virtual void OnCancel() {;}

	DECLARE_MESSAGE_MAP()

public:
	CListBox m_csMacroList;
	CXTButton m_csExcuteBtn;
	CXTButton m_csLoadBtn;
	CXTButton m_csAddBtn;
	CXTButton m_csRemoveBtn;
	CXTButton m_csSaveBtn;

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnBnClickedBLoad();
	afx_msg void OnBnClickedBRunMacro();
	afx_msg void OnDestroy();
};


