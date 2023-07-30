/*
	Notices: Copyright (c) NHN Studio 2003
	Created by: Bryan Jeong (2003/12/24)
 */

// AcQueue.h: interface for the AcQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACQUEUE_H__CE50BCCD_86C0_4D0E_A696_F382DE904421__INCLUDED_)
#define AFX_ACQUEUE_H__CE50BCCD_86C0_4D0E_A696_F382DE904421__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <queue>
#include <ApMemory/ApMemory.h>
#include <ApPacket.h>

const INT32 PACKET_MAX_COUNT = 1000;

// Queue�� �� ����ü�� �޸� Ǯ�� ����
class AcPacketData// : public ApMemory<AcPacketData, PACKET_MAX_COUNT>
{
public:
	BYTE *m_Buffer;
	INT32 m_lLength;
	INT32 m_lNID;
};

// AcClientSocket�� ������ �����Ǵ��� Queue�� �ϳ��� �����ؾ� �Ǳ� ������ �۷ι��� ����
class AcQueue;
extern AcQueue g_AcReceiveQueue;

class AcQueue  
{
private:
	std::queue<AcPacketData*>	m_stlQueue;
	ApMutualEx			m_csMutex;

public:
	AcQueue();
	virtual ~AcQueue();

	VOID		Push(AcPacketData* pQueueData);
	AcPacketData* Pop();
	
	BOOL		IsEmpty();
	INT32		GetCount();
};

#endif // !defined(AFX_ACQUEUE_H__CE50BCCD_86C0_4D0E_A696_F382DE904421__INCLUDED_)
