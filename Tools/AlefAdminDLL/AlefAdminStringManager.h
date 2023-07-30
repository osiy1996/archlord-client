// AlefAdminStringManager.h
// (C) NHN - ArchLord Development Team
// steeple, 2006.03.07
//
// �ٱ��� ������ ���ؼ� ��Ʈ�� ü�踦 �����Ͽ���.
// ���Ӱ� ���� ������ ���� ���̴� ��Ʈ���� �����ٰ� ����ϰ�,
// �ƴ϶�� ���ҽ����� ���´�.
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "resource.h"
#include "AgcmUIManager2.h"

class AlefAdminStringManager
{
protected:
	AgcmUIManager2* m_pcsAgcmUIManager2;

public:
	AlefAdminStringManager();
	virtual ~AlefAdminStringManager();

	void Init();

	CString GetUIMessage(TCHAR* szKey);
	CString GetResourceMessage(UINT uID);
};
