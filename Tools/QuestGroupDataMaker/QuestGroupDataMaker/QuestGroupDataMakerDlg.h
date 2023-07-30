// QuestGroupDataMakerDlg.h : ��� ����
//

#pragma once

#include "hashmapnpc.h"
#include "hashmaptemplate.h"
//#include "HashMapObject.h"

using namespace std;
class AuExcelTxtLib;

struct QuestNode
{
	BOOL	bUse;
	INT32	lGiveCount;
	INT32	lConfirmCount;
};

const int MAX_NODE_COUNT = 10000;

// CQuestGroupDataMakerDlg ��ȭ ����
class CQuestGroupDataMakerDlg : public CDialog
{
// ����
public:
	CQuestGroupDataMakerDlg(CWnd* pParent = NULL);	// ǥ�� ������

// ��ȭ ���� ������
	enum { IDD = IDD_QUESTGROUPDATAMAKER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ����


// ����
protected:
	HICON m_hIcon;

	// �޽��� �� �Լ��� �����߽��ϴ�.
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	typedef hash_map<INT32, CString> HashMapString;
	typedef HashMapString::iterator IterHashMapCheckPoint;

public:
	CHashMapQuestGroup	m_HashMapQuestGroup;
	HashMapTemplate		m_HashMapTemplate;
	HashMapString		m_HashMapCheckPoint;

	CString				m_strPathTemplateData;
	CString				m_strPathNPCData;
	CString				m_strPathObjectData;
	QuestNode			m_vtQusetNode[MAX_NODE_COUNT];

public:
	BOOL GetFilePath(CString &rResult);
	void MakeData();
	void WriteFile();
	void CheckStringLength(AuExcelTxtLib* pExcelTxt, INT32 lTID, INT32 lColumn, INT32 lRow, INT32 lMaxLength);

	void Check_QuestData();
	//void Check_NPCInfo();	// ����Ʈ �ο���, ���� ���� Ȯ��
	//void Check_QuestNode();	// ����Ʈ ��� Ȯ��

	afx_msg void OnBnClickedLoadTemplateData();
	afx_msg void OnBnClickedLoadNpcData();
	afx_msg void OnBnClickedMakeQuestGroupData();
	afx_msg void OnBnClickedValidationCheck();
	afx_msg void OnBnClickedClearData();
	afx_msg void OnBnClickedLoadObjectData();
};
