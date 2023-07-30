#pragma once

//#include <vector>
//#include <deque>
#include <list>
//#include <hash_map>
#include "../../../../LIBRARY/public_src/CriticalSection.h"

/**************************************************************************************************

작성일: 2009-03-12
작성자: 문상현 (youngmoon@webzen.co.kr)

설명: 잡 큐를 관리하는 클래스 (밸류 타입)

**************************************************************************************************/

template <typename _TQUVALUE> 
class CQueue
{
public:
	CQueue(void) : m_bAlive(false) {};
	virtual ~CQueue(void) {};

protected:
	std::list<_TQUVALUE> m_Que;  //정량차감, 정액 기간 만료, 라이브 체크를 관리한다.	
	WZPUBLIC::CriticalSection m_cs;	//m_JobQue 의 동기화 객체

public:
	//작업을 큐에 넣는다.
	void push(_TQUVALUE& job)
	{
		autolock(m_cs);
		m_Que.push_back(job);
	};	
	//작업을 큐에서 꺼내온다.
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
	//큐를 비운다.
	void clear()
	{
		autolock(m_cs);
		m_Que.clear();
	};

private:
	bool m_bAlive; // 큐를 사용해도 되는지 여부를 가진다.

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

작성일: 2009-04-28
작성자: 문상현 (youngmoon@webzen.co.kr)

설명: 잡 큐를 관리하는 클래스 (포인트 타입)

**************************************************************************************************/

template <typename _TQUVALUE> 
class CPtrQueue
{
public:
	CPtrQueue(void) : m_bAlive(false) {};
	virtual ~CPtrQueue(void) {};

protected:
	std::list<_TQUVALUE> m_Que;  //정량차감, 정액 기간 만료, 라이브 체크를 관리한다.	
	WZPUBLIC::CriticalSection m_cs;	//m_JobQue 의 동기화 객체

public:
	//작업을 큐에 넣는다.
	void push(_TQUVALUE job)
	{
		autolock(m_cs);
		m_Que.push_back(job);
	};	
	//작업을 큐에서 꺼내온다.
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
	//큐를 비운다.
	void clear()
	{
		autolock(m_cs);
		m_Que.clear();
	};

private:
	bool m_bAlive; // 큐를 사용해도 되는지 여부를 가진다.

public:
	bool GetQueueAlive() {return m_bAlive;};
	void SetQueueAlive(bool Alive) {m_bAlive = Alive;};
};
