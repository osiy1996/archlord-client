#pragma once

// Thread ���� Class
// Thread�� ���� class���� ��� ���� �Ŀ�.. Run�� ������ �ϸ� �ȴ�
// �� �� Start �Լ��� ȣ���ϸ� ��
// MK	2008.8.14

#include <Windows.h>

class AuThread
{
public:
	AuThread( UINT nStackSize = 0 , UINT nPriority = THREAD_PRIORITY_NORMAL );
	AuThread( AuThread& rhs );			//	��������ڴ� ���� �س��� ������ ���� �ʴ´�(��������ڸ�����)
	virtual ~AuThread( VOID );

	// Thread ����
	BOOL				Start( VOID );		//

	VOID				Destroy( VOID );	//

	// default ���� ������ ��ٸ���
	// ��ٷȴٰ� Thread ���̱�
	BOOL				WaitForTermination( IN DWORD dwMilliSeconds = INFINITE );	//

	// Thread �켱 ���� ���� �� Ȯ��
	VOID				SetPriority( IN UINT nPriority	);							//
	UINT				GetPriority( VOID				)	const;					//


	// �� Class�� ��� �޴� ��� Class�� Run�� ������ �ؾ��Ѵ�
	virtual VOID		Run( VOID )			= 0;

private:
	static DWORD WINAPI _THREAD_ENTRY_POINT( IN VOID* pData );						//

protected:
	DWORD				m_dwThreadID;			//	Thread ID
	HANDLE				m_hThread;				//	Thread Handle
	UINT				m_nStackSize;			//	Thread Stack Size
	UINT				m_nPriority;			//	Thread Priority

};

