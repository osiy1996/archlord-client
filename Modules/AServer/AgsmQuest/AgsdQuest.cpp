#include "AgsdQuest.h"

AgsdQuest::AgsdQuest()
{
	::ZeroMemory(&m_btQuest, sizeof(m_btQuest));
// AGSDQUEST_EXPAND_BLOCK
	::ZeroMemory(&m_btQuestSE, sizeof(m_btQuest));
}

// ID�� ��ȿ�� �������� �ִ��� Ȯ��
BOOL AgsdQuest::IsValid(INT32 lQuestDBID)
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

// ID�� �ش��ϴ� Bit�� ���� �����´�.
BOOL AgsdQuest::GetValueByDBID(INT32 lQuestDBID)
{
	BYTE  btTargetByte = 0x00;
	INT32 lRemainder = 0x00;
	INT32 lQuotient = 0x00;
// AGSDQUEST_EXPAND_BLOCK
	UINT8 bBlockIndex = 1; 
	if (IsValid(lQuestDBID))
	{
		if( lQuestDBID >= (AGSDQUEST_MAX_FLAG * AGSDQUEST_BITS))	
		{
			lQuestDBID -= (AGSDQUEST_MAX_FLAG * AGSDQUEST_BITS);
			bBlockIndex = 2;
		}
		lQuotient		= lQuestDBID / AGSDQUEST_BITS;
		lRemainder	= lQuestDBID % AGSDQUEST_BITS;
		
		if( 1 == bBlockIndex)
			btTargetByte	= m_btQuest[lQuotient];
		else
			btTargetByte	= m_btQuestSE[lQuotient];

		btTargetByte >>= lRemainder;

		return (btTargetByte & 0x01);
	} 
	else {return FALSE;}
}

// ID�� �ش��ϴ� Bit�� ���� ����
BOOL AgsdQuest::SetValueByDBID(INT32 lQuestDBID, BOOL bMaster)
{
	if (!IsValid(lQuestDBID)) return FALSE;

	BYTE  btTargetByte;
// AGSDQUEST_EXPAND_BLOCK
	UINT8 bBlockIndex = 1;
	if( lQuestDBID >= (AGSDQUEST_MAX_FLAG * AGSDQUEST_BITS))	// DBID �� 2048 �̻��̸�	
	{
		lQuestDBID -= (AGSDQUEST_MAX_FLAG * AGSDQUEST_BITS);	// DBID �� 2048 �̻��̸� - 2048
		bBlockIndex = 2;										// 2��° ���� ����
	}
	INT32 lQuotient		= lQuestDBID / AGSDQUEST_BITS;
	INT32 lRemainder	= lQuestDBID % AGSDQUEST_BITS;

	if (bMaster)
	{
		// Ư�� ��Ʈ�� 1�� �����Ҷ�
		btTargetByte = bMaster;
		btTargetByte <<= lRemainder;
		if( 1 == bBlockIndex)
			m_btQuest[lQuotient] |= btTargetByte;
		else
			m_btQuestSE[lQuotient] |= btTargetByte;
	}
	else
	{
		// Ư�� ��Ʈ�� 0���� �����Ҷ�(Toggle�� �ƴϸ� ������ ���� �ؾߵȴ�.)
		btTargetByte = TRUE;
		btTargetByte <<= lRemainder;
		btTargetByte ^= 0xFF;
		if( 1 == bBlockIndex)
			m_btQuest[lQuotient] &= btTargetByte;
		else
			m_btQuestSE[lQuotient] &= btTargetByte;
	}
	return TRUE;
}
/*
// ID�� �ش��ϴ� Bit�� ���� ����
BOOL AgsdQuest::SetValueByDBID(INT32 lQuestDBID, BOOL bMaster)
{
	if (IsValid(lQuestDBID))
	{
		INT32 lQuotient		= lQuestDBID / AGSDQUEST_BITS;
		INT32 lRemainder	= lQuestDBID % AGSDQUEST_BITS;

		BYTE btTargetByte = bMaster;
		btTargetByte <<= lRemainder;

		m_btQuest[lQuotient] |= btTargetByte;

		return TRUE;
	}
	else {return FALSE;}
}
*/

/*
AgsdQuest::AgsdQuestProxy::AgsdQuestProxy(BYTE* pByte, INT32 lQuestID, BOOL bValid) 
	: m_pByte(pByte), m_lQuestID(lQuestID), m_bValid(bValid) 
{
}

AgsdQuest::AgsdQuestProxy AgsdQuest::operator[](INT32 lQuestID)
{
	return AgsdQuestProxy(&m_btQuest[lQuestID/AgsdQuest_BITS], lQuestID, IsValid(lQuestID));
}

AgsdQuest::AgsdQuestProxy& AgsdQuest::AgsdQuestProxy::operator=(const AgsdQuest::AgsdQuestProxy& rProxy)
{
	*this = (BOOL)rProxy;
	return *this;
}

AgsdQuest::AgsdQuestProxy& AgsdQuest::AgsdQuestProxy::operator=(BOOL bMaster)
{
	BYTE  btTargetByte;
	if (bMaster)
	{
		// Ư�� ��Ʈ�� 1�� �����Ҷ�
		btTargetByte = bMaster;
		btTargetByte <<= m_lQuestID % AgsdQuest_BITS;
		*m_pByte |= btTargetByte;
	}
	else
	{
		// Ư�� ��Ʈ�� 0���� �����Ҷ�
		btTargetByte = TRUE;
		btTargetByte <<= m_lQuestID % AgsdQuest_BITS;
		btTargetByte ^= 0xFF;
		*m_pByte &= btTargetByte;
	}

	return *this;
}

AgsdQuest::AgsdQuestProxy::operator BOOL() const
{
	BYTE  btTargetByte	= *m_pByte;
	btTargetByte >>= m_lQuestID % AgsdQuest_BITS;
	return (btTargetByte & 0x01);
}
*/