// AgpdQustFlag.cpp: implementation of the AgpdQustFlag class.
//
//////////////////////////////////////////////////////////////////////

#include "AgpdQuestFlag.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AgpdQuestFlag::AgpdQuestFlag()
{
	InitFlags();
}

AgpdQuestFlag::~AgpdQuestFlag()
{

}

void AgpdQuestFlag::InitFlags()
{
//	memset(m_btQuest, 0x01, sizeof(m_btQuest));
	// ������ �ܼ� �Ǽ��� ���ؼ� 0x02�� �ؾߵȴ�.
	// 8�� ����� ���� ������ 8�� ��� + 1���ؼ� ������ �Ǿ���.
	memset(m_btQuestBlock1, DEFAULT_FLAG, sizeof(m_btQuestBlock1));
	memset(m_btQuestBlock2, DEFAULT_FLAG, sizeof(m_btQuestBlock2));		//AGSDQUEST_EXPAND_BLOCK
	memset(m_btQuestBlock3, DEFAULT_FLAG, sizeof(m_btQuestBlock3));		//AGSDQUEST_EXPAND_BLOCK
}

INT32 AgpdQuestFlag::GetBlockIndex(INT32 lQuestDBID)
{
	return (lQuestDBID / (AGSDQUEST_MAX_FLAG * AGSDQUEST_BITS));
}

BOOL AgpdQuestFlag::MemoryCopy(BYTE* pMemory, INT32 lBlockIndex)
{
	ASSERT(pMemory);

	switch (lBlockIndex)
	{
	case 0: memcpy(m_btQuestBlock1, pMemory, AGSDQUEST_MAX_FLAG); break;
	case 1: memcpy(m_btQuestBlock2, pMemory, AGSDQUEST_MAX_FLAG); break;
	case 2: memcpy(m_btQuestBlock3, pMemory, AGSDQUEST_MAX_FLAG); break;
	default:
		{
			ASSERT(!"index error");
			return FALSE;
		}
		break;
	};
	
	return TRUE;
}


// ID�� ��ȿ�� �������� �ִ��� Ȯ��
BOOL AgpdQuestFlag::IsValid(INT32 lQuestDBID)
{
// AGSDQUEST_EXPAND_BLOCK
	if ((lQuestDBID < (AGSDQUEST_MAX_FLAG * AGSDQUEST_BITS * AGSDQUEST_BLOCK_COUNT)) && (lQuestDBID > 0))
		return TRUE;
	else
	{
		ASSERT(!"Quest TID�� ��ȿ�� ���� �ƴմϴ�.");
		return FALSE;
	}
}

BYTE* AgpdQuestFlag::GetBlockMemory(INT32 lBlockIndex)
{
	switch (lBlockIndex)
	{
	case 0:	return m_btQuestBlock1;
	case 1: return m_btQuestBlock2;
	case 2: return m_btQuestBlock3;
	default:
		ASSERT(!"error index");
		break;
	};

	return NULL;
}

// ID�� �ش��ϴ� Bit�� ���� �����´�.
BOOL AgpdQuestFlag::GetValueByDBID(INT32 lQuestDBID)
{
	if (FALSE == IsValid(lQuestDBID))
		return FALSE;

	BYTE* pBlock = GetBlockMemory(GetBlockIndex(lQuestDBID));
	if (NULL == pBlock)
		return FALSE;

	INT32 OffsetID = lQuestDBID % (AGSDQUEST_MAX_FLAG * AGSDQUEST_BITS);

	INT32 lQuotient	= OffsetID / AGSDQUEST_BITS;
	INT32 lRemainder = OffsetID % AGSDQUEST_BITS;

	if(pBlock[lQuotient] < 0)
		pBlock[lQuotient] = pBlock[lQuotient] + 256;

	BYTE  btTargetByte = pBlock[lQuotient];
	btTargetByte >>= lRemainder;

	return (btTargetByte & 0x01);
}

// ID�� �ش��ϴ� Bit�� ���� ����
BOOL AgpdQuestFlag::SetValueByDBID(INT32 lQuestDBID, BOOL bMaster)
{
	if (!IsValid(lQuestDBID)) 
		return FALSE;

	BYTE  btTargetByte;
	BYTE *pBlock = GetBlockMemory(GetBlockIndex(lQuestDBID));
	if (NULL == pBlock)
		return FALSE;

	INT32 OffsetID = lQuestDBID % (AGSDQUEST_MAX_FLAG * AGSDQUEST_BITS);

	INT32 lQuotient	= OffsetID / AGSDQUEST_BITS;
	INT32 lRemainder = OffsetID % AGSDQUEST_BITS;

	if (bMaster)
	{
		// Ư�� ��Ʈ�� 1�� �����Ҷ�
		btTargetByte = bMaster;
		btTargetByte <<= lRemainder;
		pBlock[lQuotient] |= btTargetByte;
	}
	else
	{
		// Ư�� ��Ʈ�� 0���� �����Ҷ�(Toggle�� �ƴϸ� ������ ���� �ؾߵȴ�.)
		btTargetByte = TRUE;
		btTargetByte <<= lRemainder;
		btTargetByte ^= 0xFF;
		pBlock[lQuotient] &= btTargetByte;
	}

	return TRUE;
}


/*
BOOL AgpdQuestFlag::SetValueByDBID(INT32 lQuestDBID, BOOL bMaster)
{
	if (!IsValid(lQuestDBID)) return FALSE;

	BYTE  btTargetByte;
	INT32 lQuotient		= lQuestDBID / AGSDQUEST_BITS;
	INT32 lRemainder	= lQuestDBID % AGSDQUEST_BITS;
	if (bMaster)
	{
		// Ư�� ��Ʈ�� 1�� �����Ҷ�
		btTargetByte = bMaster;
		btTargetByte <<= lRemainder;
		m_btQuest[lQuotient] |= btTargetByte;
	}
	else
	{
		// Ư�� ��Ʈ�� 0���� �����Ҷ�(Toggle�� �ƴϸ� ������ ���� �ؾߵȴ�.)
		btTargetByte = TRUE;
		btTargetByte <<= lRemainder;
		btTargetByte ^= 0xFF;
		m_btQuest[lQuotient] &= btTargetByte;
	}

	return TRUE;
}

BOOL AgpdQuestFlag::MemoryCopy(BYTE* pMemory, BYTE* pMemorySE)
{
	memcpy(m_btQuest, pMemory, AGSDQUEST_MAX_FLAG);
#ifdef AGSDQUEST_ADD_BLOCK
	memcpy(m_btQuestSE, pMemorySE, AGSDQUEST_MAX_FLAG);
#endif
	return TRUE;
}

// ID�� ��ȿ�� �������� �ִ��� Ȯ��
BOOL AgpdQuestFlag::IsValid(INT32 lQuestDBID)
{
#ifdef AGSDQUEST_ADD_BLOCK
	if ((lQuestDBID < (AGSDQUEST_MAX_FLAG * AGSDQUEST_BITS * AGSDQUEST_BLOCK_COUNT)) && (lQuestDBID > 0))
		return TRUE;
	else
	{
		ASSERT(!"Quest TID�� ��ȿ�� ���� �ƴմϴ�.");
		return FALSE;
	}
#else
	if ((lQuestDBID < (AGSDQUEST_MAX_FLAG * AGSDQUEST_BITS)) && (lQuestDBID > 0))
		return TRUE;
	else
	{
		ASSERT(!"Quest TID�� ��ȿ�� ���� �ƴմϴ�.");
		return FALSE;
	}
#endif
}

BOOL AgpdQuestFlag::IsValid(INT32 lQuestDBID)
{
	if ((lQuestDBID < (AGSDQUEST_MAX_FLAG * AGSDQUEST_BITS)) && (lQuestDBID > 0))
		return TRUE;
	else
	{
		ASSERT(!"Quest TID�� ��ȿ�� ���� �ƴմϴ�.");
		return FALSE;
	}
}

BOOL AgpdQuestFlag::GetValueByDBID(INT32 lQuestDBID)
{
	if (IsValid(lQuestDBID))
	{
		INT32 lQuotient		= lQuestDBID / AGSDQUEST_BITS;
		INT32 lRemainder	= lQuestDBID % AGSDQUEST_BITS;
		BYTE  btTargetByte	= m_btQuest[lQuotient];

		btTargetByte >>= lRemainder;

		return (btTargetByte & 0x01);
	} 
	else {return FALSE;}
}

void AgpdQuestFlag::InitFlags()
{
//	memset(m_btQuest, 0x01, sizeof(m_btQuest));
	// ������ �ܼ� �Ǽ��� ���ؼ� 0x02�� �ؾߵȴ�.
	// 8�� ����� ���� ������ 8�� ��� + 1���ؼ� ������ �Ǿ���.
	memset(m_btQuest, DEFAULT_FLAG, sizeof(m_btQuest));

}
*/
