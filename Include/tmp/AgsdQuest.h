#pragma once

#include "ApBase.h"

// AGSDQUEST_EXPAND_BLOCK
const INT32 AGSDQUEST_MAX_FLAG		= 1024;
const INT32 AGSDQUEST_BITS			= 8;
// AGSDQUEST_EXPAND_BLOCK
	const INT32 AGSDQUEST_BLOCK_COUNT			= 2;

class AgsdQuest
{
private:
	BYTE	m_btQuest[AGSDQUEST_MAX_FLAG];
// AGSDQUEST_EXPAND_BLOCK
	BYTE	m_btQuestSE[AGSDQUEST_MAX_FLAG];

public:
	AgsdQuest();
	~AgsdQuest();

	// ID�� ��ȿ�� �������� �ִ��� Ȯ��
	BOOL IsValid(INT32 lQuestDBID);

	// ID�� �ش��ϴ� Bit�� ���� �����´�.
	BOOL GetValueByDBID(INT32 lQuestDBID);

	// ID�� �ش��ϴ� Bit�� ���� ����
	BOOL SetValueByDBID(INT32 lQuestDBID, BOOL bMaster);

/*
public:
	// ����Ʈ�� ID�� �̿��Ͽ� �迭�� �����ϵ��� �����ְ� �ϱ�����
	// Proxy Class�� �ذ�å���� ����
	class AgpdQuestProxy
	{
	private:
		BYTE *m_pByte;
		INT32 m_lQuestID;
		BOOL m_bValid;		// AgpdQuest�� [] �����ڿ��� AgpdQuestProxy�� �ν��Ͻ��� �����ؾ� �ϱ� ������
							// Quest ID�� �߸� �־ �˷��ټ� �ִ� �ذ�å�� ����(exception�� ����). 
							// �׷��� ������ �����Ͱ� ��ȿ������ Ȯ���Ҽ� �ִ� m_bValid�� �߰�
							// ������� ���� �ذ�å.....

	public:
		AgpdQuestProxy(BYTE* pByte, INT32 lQuestID, BOOL bValid);
		AgpdQuestProxy& operator=(const AgpdQuestProxy& rProxy);
		AgpdQuestProxy& operator=(BOOL bMaster);
		operator BOOL() const;

		BOOL IsValid()	{return m_bValid;}
	};

public:

	// []�� �̿��Ͽ� ID�� �ش��ϴ� Bit�� ���� �����´�.
	AgpdQuestProxy operator[](INT32 lQuestID);

	friend class AgpdQuestProxy;
*/
};