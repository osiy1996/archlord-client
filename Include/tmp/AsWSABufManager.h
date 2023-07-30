// AsWSABufManager.h: interface for the AsWSABufManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ASWSABUFMANAGER_H__2DB4BB11_A873_4D29_A35B_0F27729612A9__INCLUDED_)
#define AFX_ASWSABUFMANAGER_H__2DB4BB11_A873_4D29_A35B_0F27729612A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApDefine.h"
#include "ApBase.h"

#if defined _M_X64
#define MAX_WSABUF_COUNT 1500
#else
#define MAX_WSABUF_COUNT 500
#endif

enum Enum_ADD_PACKET_RESULT
{
	ADD_PACKET_RESULT_NONE = 0,
	ADD_PACKET_RESULT_SUCCESS,		// ���������� ��Ŷ �߰�
	ADD_PACKET_RESULT_DROP,			// ��Ŷ ����
	ADD_PACKET_RESULT_DESTROY,		// ���̻� ��Ŷ�� ������ ���� ����, ������Ѿߵ�
	ADD_PACKET_RESULT_MAX
};

class AsWSABufManager  
{
private:
	WSABUF			m_WsaBuffer[MAX_WSABUF_COUNT];		// ��Ŷ���� �����ּ�
	PACKET_PRIORITY	m_Priority[MAX_WSABUF_COUNT];		// ��Ŷ �켱����
	INT32			m_lPriorityCount[PACKET_PRIORITY_MAX];	// �켱������ �ش��ϴ� ��Ŷ�� ����
	INT32			m_lCurrentCount;					// ���� �����Ǿ� �ִ� ��Ŷ ����
	INT32			m_lTotalBufSize;					// �����Ǿ� �ִ� ��Ŷ�� ��ü ũ��

public:
	BOOL bSetTimerEvent;

private:
	BOOL RemoveWSABuffer(INT32 lIndex);
	PACKET_PRIORITY GetLastPriority(PACKET_PRIORITY ePriority);

public:
	AsWSABufManager();
	virtual ~AsWSABufManager();

	void Initialize();

	inline INT32 GetTotalBufferSize()	{return m_lTotalBufSize;}
	inline INT32 GetCurrentCount()		{return m_lCurrentCount;}

	Enum_ADD_PACKET_RESULT AddPacket(CHAR* pData, INT32 lLength, PACKET_PRIORITY ePriority);
	CHAR* GetStartBuffer();
	void BufferClear();
};

#endif // !defined(AFX_ASWSABUFMANAGER_H__2DB4BB11_A873_4D29_A35B_0F27729612A9__INCLUDED_)
