// CSLog.h: interface for the CCSLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CSLOG_H__329DB686_C493_46B9_8A0C_FFE58F63C3C9__INCLUDED_)
#define AFX_CSLOG_H__329DB686_C493_46B9_8A0C_FFE58F63C3C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <string>
using namespace std;
#include <ApBase/ApRWLock.h>

class CSLogManager	;
class CCSLog		;
class CSLogIndicator;

class CSLogManager
{
private:
	CSLogManager():m_pCurrentIndicator( NULL ) {};

	CSLogIndicator	* m_pCurrentIndicator;
	// Lock �߰��ؾ���..
	
	CSLogIndicator	* GetIndicator() { return m_pCurrentIndicator; }
	
public:
	static CSLogManager * GetInstance()
	{
		static CSLogManager	stSingleton;
		return & stSingleton;
	}

	struct	CSLogInfo
	{
		enum STATE
		{
			NON_INITIALIZED	,
			ENTER			,
			ESCAPE			,
			ESCAPEERROR		,
			STACKOVERFLOW		
		};

		unsigned int	uSerial	;
		STATE			eState	;

		CSLogInfo(): uSerial( 0 ),eState( NON_INITIALIZED ) {}
	};

	vector< CSLogInfo >	m_vecLog;
	ApRWLock			m_Lock;

	bool	StartLog	( CSLogIndicator * pIndicator )
	{
		ApAutoWriterLock	csLock( m_Lock );
		if( GetIndicator() ) return false;
		// �Ѱ��� ���� �����ؾ��Ѵ�.
		m_pCurrentIndicator = pIndicator;
		m_vecLog.clear();

		return true;
	}

	bool	EndLog		( CSLogIndicator * pIndicator )
	{
		ApAutoWriterLock	csLock( m_Lock );
		if( !GetIndicator() ) return false;

		m_pCurrentIndicator = NULL;
		m_vecLog.clear();

		return true;
	}

	bool	Push		( CCSLog	* pLog );
	bool	Pop			( CCSLog	* pLog );

	static bool	Print		( char * buf );
};

class CCSLog  
{
public:
	unsigned int uSerial;
	// �����ɶ��� ����ؼ� , �׳� ���ڷ� �Ѵ�..
	// ��Ʈ���� ��� ������ �Ұ�.. ���ϵ� ����..

public:
	CCSLog( unsigned int uSerial ){
		this->uSerial = uSerial;
		CSLogManager * pManager = CSLogManager::GetInstance();

		pManager->Push( this );
	}

	virtual ~CCSLog()
	{
		CSLogManager * pManager = CSLogManager::GetInstance();
		// �̶� this�� ������� ���� ���� �ִ�.
		// Pop ���ο��� �������.
		pManager->Pop( this );
	}
};

class CSLogIndicator
{
public:
	CSLogIndicator( const char * pMessage )
	{
		strIndicatorLog	= pMessage;
	}

	string	strIndicatorLog;

	void	Start	()
	{
		CSLogManager * pManager = CSLogManager::GetInstance();
		pManager->StartLog( this );
	};
	void	End		()
	{
		CSLogManager * pManager = CSLogManager::GetInstance();

		// ���ó� ���⼭�� this�� ������� ���� �� �ִ�.
		pManager->EndLog( this );
	};
};

class	CSLogAutoIndicator
{
protected:
	CSLogIndicator	* m_pIndicator;
public:
	CSLogAutoIndicator( CSLogIndicator * pIndicator ) : m_pIndicator( NULL )
	{
		m_pIndicator	= pIndicator;

		m_pIndicator->Start();
	}

	~CSLogAutoIndicator()
	{
		m_pIndicator->End();
	}
};
#endif // !defined(AFX_CSLOG_H__329DB686_C493_46B9_8A0C_FFE58F63C3C9__INCLUDED_)
