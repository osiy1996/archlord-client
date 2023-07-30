#pragma once

//#include <vector>
//#include <deque>
#include <list>
//#include <hash_map>
#include "../../../../LIBRARY/public_src/CriticalSection.h"

/**************************************************************************************************

�ۼ���: 2009-03-12
�ۼ���: ������ (youngmoon@webzen.co.kr)

����: �� ť�� �����ϴ� Ŭ���� (��� Ÿ��)

**************************************************************************************************/

template <typename _TQUVALUE> 
class CQueue
{
public:
	CQueue(void) : m_bAlive(false) {};
	virtual ~CQueue(void) {};

protected:
	std::list<_TQUVALUE> m_Que;  //��������, ���� �Ⱓ ����, ���̺� üũ�� �����Ѵ�.	
	WZPUBLIC::CriticalSection m_cs;	//m_JobQue �� ����ȭ ��ü

public:
	//�۾��� ť�� �ִ´�.
	void push(_TQUVALUE& job)
	{
		autolock(m_cs);
		m_Que.push_back(job);
	};	
	//�۾��� ť���� �����´�.
	bool pop(_TQUVALUE& job)
	{
		autolock(m_cs);
		if(!m_Que.empty())
		{
			job = m_Que.front();
			m_Que.pop_front();
			return true;
		}
		return false;	
	};
	//ť�� ����.
	void clear()
	{
		autolock(m_cs);
		m_Que.clear();
	};

private:
	bool m_bAlive; // ť�� ����ص� �Ǵ��� ���θ� ������.

public:
	bool GetQueueAlive() {return m_bAlive;};
	void SetQueueAlive(bool Alive) {m_bAlive = Alive;};
};
/*
template <typename _TQUVALUE> 
class CAutoDeleteJobValue
{
public:
	CAutoDeleteJobValue(_TQUVALUE value){m_value = value;};
	virtual ~CAutoDeleteJobValue(void) {delete m_value;};

	_TQUVALUE m_value;
};
*/

/**************************************************************************************************

�ۼ���: 2009-04-28
�ۼ���: ������ (youngmoon@webzen.co.kr)

����: �� ť�� �����ϴ� Ŭ���� (����Ʈ Ÿ��)

**************************************************************************************************/

template <typename _TQUVALUE> 
class CPtrQueue
{
public:
	CPtrQueue(void) : m_bAlive(false) {};
	virtual ~CPtrQueue(void) {};

protected:
	std::list<_TQUVALUE> m_Que;  //��������, ���� �Ⱓ ����, ���̺� üũ�� �����Ѵ�.	
	WZPUBLIC::CriticalSection m_cs;	//m_JobQue �� ����ȭ ��ü

public:
	//�۾��� ť�� �ִ´�.
	void push(_TQUVALUE job)
	{
		autolock(m_cs);
		m_Que.push_back(job);
	};	
	//�۾��� ť���� �����´�.
	bool pop(_TQUVALUE& job)
	{
		autolock(m_cs);
		if(!m_Que.empty())
		{
			job = m_Que.front();
			m_Que.pop_front();
			return true;
		}
		return false;	
	};
	//ť�� ����.
	void clear()
	{
		autolock(m_cs);
		m_Que.clear();
	};

private:
	bool m_bAlive; // ť�� ����ص� �Ǵ��� ���θ� ������.

public:
	bool GetQueueAlive() {return m_bAlive;};
	void SetQueueAlive(bool Alive) {m_bAlive = Alive;};
};
