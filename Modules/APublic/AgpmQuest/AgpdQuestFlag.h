// AgpdQustFlag.h: interface for the AgpdQustFlag class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGPDQUSTFLAG_H__C047CC68_2778_4D0E_8A54_2A65D2C7280C__INCLUDED_)
#define AFX_AGPDQUSTFLAG_H__C047CC68_2778_4D0E_8A54_2A65D2C7280C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <ApBase/ApBase.h>

// AGSDQUEST_EXPAND_BLOCK					//����������Ʈ�޸� Ȯ���۾� 2005.9.7 Young Hoon

const INT32 AGSDQUEST_MAX_FLAG		= 256;

const INT32 AGSDQUEST_BITS			= 8;

const INT32 AGSDQUEST_BLOCK_COUNT	= 3;		//AGSDQUEST_EXPAND_BLOCK

#define DEFAULT_FLAG 0x02

class AgpdQuestFlag  
{
public:
	BYTE	m_btQuestBlock1[AGSDQUEST_MAX_FLAG];
	BYTE	m_btQuestBlock2[AGSDQUEST_MAX_FLAG];
	BYTE	m_btQuestBlock3[AGSDQUEST_MAX_FLAG];

private:
	BYTE* GetBlockMemory(INT32 lBlockIndex);
	INT32 GetBlockIndex(INT32 lQuestDBID);

public:
	AgpdQuestFlag();
	virtual ~AgpdQuestFlag();

	void InitFlags();

	BOOL MemoryCopy(BYTE* pMemory, INT32 lBlockIndex);

	// ID�� ��ȿ�� �������� �ִ��� Ȯ��
	BOOL IsValid(INT32 lQuestDBID);

	// ID�� �ش��ϴ� Bit�� ���� �����´�.
	BOOL GetValueByDBID(INT32 lQuestDBID);

	// ID�� �ش��ϴ� Bit�� ���� ����
	BOOL SetValueByDBID(INT32 lQuestDBID, BOOL bMaster);

};

#endif // !defined(AFX_AGPDQUSTFLAG_H__C047CC68_2778_4D0E_8A54_2A65D2C7280C__INCLUDED_)
