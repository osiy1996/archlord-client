// QuestGroupDataMaker.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error PCH���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����Ͻʽÿ�.
#endif

#include "resource.h"		// �� ��ȣ


// CQuestGroupDataMakerApp:
// �� Ŭ������ ������ ���ؼ��� QuestGroupDataMaker.cpp�� �����Ͻʽÿ�.
//

class CQuestGroupDataMakerApp : public CWinApp
{
public:
	CQuestGroupDataMakerApp();

// ������
	public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
};

extern CQuestGroupDataMakerApp theApp;
