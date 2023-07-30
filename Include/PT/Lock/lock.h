#pragma once
//////////////////////////////////////////////////////////////////////////////////////
//	˵����	���������ڶ��߳��������еĻ��⣬ʵ�������õ���Դ������������ʵ�ַ����
//			��Դ����������顢�����ָ���
//
//	���ܣ�	������Դ������������������Դ����������顢�����ָ�
//
//	�ص㣺	1���������غ��Զ��������ڶ��֦�����м��ٿ�����䣬���������ǿ���
//			2���Զ���������������ԣ�����࿪���ٿ�
//			3����������Դ�Զ�������Դ����
//			4����Դ�������Զ����ɣ���������Ĺ���
//			5����ȫ�������
//////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////////////////
//
//		CWithLock:	������ʵ������
//		CLock:		������CWithLockʵ������
//		CWithManagedLock:	������ʵ�����࣬������Դ����ϵͳ�����Զ�����
//		CResourceManage:	�Զ���Դ����������������CWithManagedLockʵ�������ܼ������
//
//////////////////////////////////////////////////////////////////////////////////////
#include "sdkconfig.h"
//	����������һ�������ֻ��ʹ��һ�������ڣ����ڵ�һ�㣬ֻ��LOCKһ��
#define LOCK(object)		CLock Lock(object);
#define UNLOCK()			Lock.UnLock();

//	����������������ʱʹ�ã������ͬһ������Σ���ʹ�ò�ͬ������
#define LOCK2(object,name)	CLock Lock_##name(object);
#define UNLOCK2(name)		Lock_##name.UnLock()

//	�����������󵽿���֮��Ĵ����ǲ��������
#define LOCK_CODE()			static CWithLock CodeWithLock; CLock CodeLock(CodeWithLock);
#define UNLOCK_CODE()		CodeLock.UnLock();

//	�����ƴ�������������ͬһ������ж��ʹ�õĴ�����
#define LOCK_CODE2(name)	static CWithLock CodeWithLock_##name; CLock CodeLock_##name(CodeWithLock_##name);
#define UNLOCK_CODE2(name)	CodeLock##name.UnLock();

//	ʹ�ö����������ռ�
namespace CleverLock
{

	class CWithLock;
	class CLock;
	//class CResourceManage;
	//class CWithManagedLock;
	//class CGarbo;

	// ������ʵ������
	class PTSDK_STUFF CWithLock
	{
	public:
		CWithLock();
		~CWithLock();
		HANDLE	GetOwnerThread();

	protected:
		CRITICAL_SECTION	m_csAccess;

		inline	void Lock();
		inline	void UnLock();
		inline	BOOL TryLock();

		friend class CLock;
	};

	// ������CWithLockʵ������
	class PTSDK_STUFF CLock
	{
	public:
		CLock( CWithLock &door, bool bLook = true);
		~CLock();

		void Lock();
		void UnLock();
		BOOL TryLock();

		static void Lock(CWithLock &door);
		static void UnLock(CWithLock &door);
	private:
		bool m_bLocked;
		CWithLock * m_pDoor;
	};

	////�Զ���Դ����������������CWithManagedLockʵ�������ܼ������ 
	//class CResourceMgr: public CWithLock
	//{
	//public:
	//	~CResourceMgr();
	//	static CResourceMgr * GetInstance();

	//	CWithManagedLock * CheckDeadLock();

	//private:	
	//	CResourceMgr();
	//	static CResourceMgr * m_pInstance;
	//	CArray<CWithManagedLock *, CWithManagedLock *> m_aResourceArray;

	//protected:
	//	void AddResource(CWithManagedLock * p_resource);
	//	void RemoveResource(CWithManagedLock * p_resource);

	//	friend CWithManagedLock;

	//	class CGarbo
	//	{
	//	public:
	//		~CGarbo()
	//		{
	//			if (CResourceMgr::m_pInstance)
	//				delete CResourceMgr::m_pInstance;
	//		}
	//	};

	//	static CGarbo Garbo;
	//};

	//// ������ʵ�����࣬������Դ����ϵͳ�����Զ�����
	//class CWithManagedLock: public CWithLock
	//{
	//public:
	//	CWithManagedLock();
	//	~CWithManagedLock();

	//	void	ForceFree();
	//	HANDLE	GetWaitingThread(int index);

	//protected:
	//	CWithLock m_WaitQueueLock;	
	//	CArray<HANDLE, HANDLE> m_aWaitingThread;

	//	virtual	void Lock();

	//	friend CResourceMgr;
	//};

	//CResourceMgr::CGarbo CResourceMgr::Garbo;
};

