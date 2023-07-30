// ApSet.h: interface for the ApSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APSET_H__AC54872A_884A_4B4F_B2E3_2A4C7A472F2D__INCLUDED_)
#define AFX_APSET_H__AC54872A_884A_4B4F_B2E3_2A4C7A472F2D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApBase.h"
#include <set>
#include "ApMutualEx.h"

using namespace std;

template <typename TKey>
class ApSet
{
private:
	ApCriticalSection	Mutex_;

public:
//	typedef std::set< TKey >::iterator Iterator;
//	typedef std::pair< std::set< TKey >::iterator, bool > PairReturn;
	std::set<TKey>			Set_;

public:
	ApSet()
	{
		Clear();
	}

	virtual ~ApSet()
	{
		Clear();
	}

	bool Add(TKey value)
	{
		AuAutoLock Lock(Mutex_);
		if (!Lock.Result()) return FALSE;

		std::pair< set<TKey>::iterator, bool> pr;
		pr = Set_.insert(value);
		return pr.second;
	}

	bool Find(TKey value)
	{
		AuAutoLock Lock(Mutex_);
		if (!Lock.Result()) return FALSE;

		// lValue�� ���� �����ϸ� TRUE �׷��� ������ FALSE
		set<TKey>::iterator iter = Set_.find(value);

		if (Set_.end() == iter) 
			return false;

		return true;
	}

	bool Remove(TKey value)
	{
		AuAutoLock Lock(Mutex_);
		if (!Lock.Result()) return FALSE;
		size_t count = Set_.erase(value);
		// ������ set�� ������ 0�̸� �������� �ʴ� key���� ������ ���
		return (bool)count;
	}

	void Clear()
	{
		AuAutoLock Lock(Mutex_);
		if (!Lock.Result()) return;
		Set_.clear();
	}
};

#endif // !defined(AFX_APSET_H__AC54872A_884A_4B4F_B2E3_2A4C7A472F2D__INCLUDED_)
