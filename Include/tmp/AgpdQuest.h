
#if !defined(__AGPDQUEST_H__)
#define __AGPDQUEST_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApBase.h"
#include "AgpdQuestFlag.h"
const INT32 AGPDQUEST_CURRENT_ID_MAX	= 10;

#include <vector>

using namespace std;

class AgpdCurrentQuest
{
public:
	INT32 lQuestID;
	INT32 lParam1;
	INT32 lParam2;
	CHAR szNPCName[AGPACHARACTER_MAX_ID_STRING + 1];

	AgpdCurrentQuest() : lQuestID(0), lParam1(0), lParam2(0)
	{
		ZeroMemory(szNPCName, sizeof(szNPCName));
	}
};

class AgpdQuest
{
public:
	typedef vector<AgpdCurrentQuest> VectorQuest;
	typedef VectorQuest::iterator IterQuest;
	VectorQuest		m_vtQuest;
	AgpdQuestFlag	m_csFlag;

public:
	AgpdQuest();
	
	// ���ο� �������� ����Ʈ �߰�
	BOOL AddNewQuest(INT32 lQuestTID, INT32 lParam1 = 0, INT32 lParam2 = 0);

	// �������� ����Ʈ ���
	BOOL CancelQuest(INT32 lQuestTID);

	// �ο����� ����Ʈ ���� Ȯ��
	BOOL IsHaveQuest(INT32 lQuestTID);

	BOOL UpdateParam(INT32 lQuestTID, INT32 lParam1, INT32 lParam2);

	// Param1�� ���� ī���ͷ� ���
	INT32 GetMonsterCount1(INT32 lQuestTID);
	INT32 GetMonsterCount2(INT32 lQuestTID);
	INT32 IncrementMonsterCount(INT32 lQuestTID);
	INT32 SetMonsterCount(INT32 lQuestTID, INT32 lCount);
	
	INT32 GetCount();

	vector<AgpdCurrentQuest>::iterator FindByQuestID(INT32 lQuestID);

};

#endif
