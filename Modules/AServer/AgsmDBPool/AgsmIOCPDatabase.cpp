// AgsmIOCPDatabase.cpp: implementation of the AgsmIOCPDatabase class.
//
//////////////////////////////////////////////////////////////////////

#include "AgsmIOCPDatabase.h"
#include <memory>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LONG g_lExecuteCount = 0L;

AgsmIOCPDatabase::AgsmIOCPDatabase()
{
	// DB Manager�� Oracle�� OLEDB�� �ʱ�ȭ
	m_DBManager.Initialize(AUDB_API_OLEDB, AUDB_ORACLE);

	// select�� ����� ���� ���۸� 2MB�� �ʱ�ȭ
	m_DBManager.InitializeHeap(2);

	InitConnectParam();
}

AgsmIOCPDatabase::AgsmIOCPDatabase(eAgsmIOCPDatabaseApi eApi)
{
	BOOL bInitialized = FALSE;
	switch(eApi)
	{
		case AGSMIOCPDATABASE_API_OLEDB:
			m_DBManager.Initialize(AUDB_API_OLEDB, AUDB_ORACLE);
			bInitialized = TRUE;
			break;

		case AGSMIOCPDATABASE_API_ODBC:
			m_DBManager.Initialize(AUDB_API_ODBC, AUDB_MYSQL);
			bInitialized = TRUE;
			break;
		
		default:
			ASSERT(!"AgsmIOCPDatabase::m_DBManager �ʱ�ȭ ����");
			break;
	}

	if(bInitialized)
		m_DBManager.InitializeHeap(2);

	InitConnectParam();
}

AgsmIOCPDatabase::~AgsmIOCPDatabase()
{
	m_DBManager.TerminateHeap();
	m_DBManager.Terminate();
}

BOOL AgsmIOCPDatabase::InitConnectParam()
{
	m_szDataSource = NULL;
	m_szUser = NULL;
	m_szPassword = NULL;

	return TRUE;
}

BOOL AgsmIOCPDatabase::SetConnectParam(LPCTSTR szDataSource, LPCTSTR szUser, LPCTSTR szPassword)
{
	if(!szDataSource || !szUser || !szPassword)
		return FALSE;

	m_szDataSource = szDataSource;
	m_szUser = szUser;
	m_szPassword = szPassword;

	return TRUE;
}

BOOL AgsmIOCPDatabase::Create(INT32 lDBConnectionCount)
{
	if (!m_DBEvents.Create(lDBConnectionCount)) return FALSE;

	// Worker Thread�� �����Ѵ�.
	ApIOCP::Create(lDBConnectionCount, DBWorkerThread, PVOID(this));

	// DB�� Connection�� ��� ����ɶ����� ��ٸ���.
	return m_DBEvents.Wait();
}

BOOL AgsmIOCPDatabase::PostSQL(SQLBuffer* pSQLBuffer)
{
	return PostStatus((DWORD)pSQLBuffer);
}

// 2004.2.29. ������ ����.
// �ܺο��� Connect Param �� ������ �� �ְ� ����
BOOL AgsmIOCPDatabase::CreateDBConnection(AuDatabase** ppConnection, AuStatement** ppStatement, AuRowset** ppRowset)
{
	// Connect Param �� ���� �ȵǾ� ������ ������.
	if(!m_szDataSource || !m_szUser || !m_szPassword)
		return FALSE;

	// DB connection ����
	*ppConnection = m_DBManager.CreateDatabase();
	if (!(*ppConnection)->Open(m_szDataSource, m_szUser, m_szPassword)) return FALSE;

	// Statement ����
	*ppStatement = m_DBManager.CreateStatement(*ppConnection);
	if (!(*ppStatement)->Open()) return FALSE;
	
	// Rowset�� �̸� ����
	*ppRowset = m_DBManager.CreateRowset(NULL);
	(*ppRowset)->InitializeBuffer(4);	// 4K. pre-alloc

	return TRUE;
}

// auRowset �� ���� �޴� ������� ����. - 2004.03.04. steeple
BOOL AgsmIOCPDatabase::ExecuteSQL(AuStatement* pStatement, AuRowset* pRowset, SQLBuffer* pSQLBuffer)
{
	if (pStatement->Execute(pSQLBuffer->Buffer))
	{
		// ����� ������ update, insert, delete�� ���� ������ �ǹ�
		if (!pStatement->HasResult()) 
			return TRUE;

		// �Ʒ����ʹ� select����� ���� ������ �̹Ƿ� ����� �°� ó���Ѵ�.
		pRowset->Set(*pStatement);		// �̸� ������ rowset�� statement �Ҵ�.
		pRowset->Bind(TRUE);
		
		while (!pRowset->IsEnd())
			pRowset->Fetch();		// Fetch�� ����� ���� ����� �����Ǹ�
											// row count�� ������ fetch�� �� ����(������ �� ������� �ƴ�)

		// Callback �� �ҷ��ش�.
		if(pSQLBuffer->m_pfCallback)
			pSQLBuffer->m_pfCallback(pRowset, pSQLBuffer->m_pClass, pSQLBuffer->m_pData);

		pRowset->Close();
	}

	return FALSE;
}

UINT WINAPI AgsmIOCPDatabase::DBWorkerThread(PVOID pParam)
{
	AgsmIOCPDatabase	*pThis = (AgsmIOCPDatabase*)pParam;
	DWORD				dwTransferred	= 0;
	LPOVERLAPPED		pOverlapped		= NULL;
	SQLBuffer*			pSQLBuffer		= NULL;

	// DB�� ���õ� ������
	AuDatabase*		pConnection	= NULL;
	AuStatement*	pStatement	= NULL;
	AuRowset*		pRowset		= NULL;

	INT32 lEventIndex = pThis->m_DBEvents.GetIndex();
	ASSERT((INVALID_INDEX != lEventIndex) && "�����ڰ� ����ִ� Index�� ���ų� Event ������ �����߽��ϴ�.");

	// COM library�� �� �����庰�� �ʱ�ȭ������Ѵ�.
	CoInitialize(NULL);

	while (!pThis->CreateDBConnection(&pConnection, &pStatement, &pRowset))
	{
		// DB���� ������ �Ϸ���� ������ �ٽ� ���� �õ�
	}

	printf("\n DB ���� �Ϸ�");

	// DB������ �Ϸ� �Ǿ����� Event�� ����
	pThis->m_DBEvents.SetDBEvent(lEventIndex);
	
	while(TRUE)
	{
		if (pThis->GetStatus((PULONG_PTR)&pSQLBuffer, &dwTransferred, &pOverlapped, INFINITE))
		{
			// SQL ����
			if (NULL == pSQLBuffer)
			{
				printf("\npSQL�� NULL�̴�.");
				continue;
			}

			std::auto_ptr<SQLBuffer> autoPtr(pSQLBuffer);
			printf("Execute Query Count : %d\n", InterlockedIncrement(&g_lExecuteCount));
			if (!pThis->ExecuteSQL(pStatement, pRowset, pSQLBuffer))
			{
				// DB���� ������ ������ ������� Ȯ���ϰ� ���� �������ٸ� �ٽ� ������ ��õ� �Ѵ�.
				if (!pConnection->IsAlive())
					{
					/*
					pConnection->Close();
					pConnection->Open("", "", "");
					connection�� ���������Ƿ� �����¿��� pStatement�� invalid�ϴ�.
					delete�ϰ� �ٽ� CreateStatement(*pStatement)�ϴ���
					�ο�°� �����ϰ� auDatabase���� Set(pStatement)�� �� �ֵ��� ���̺귯���� �����ؾ��Ѵ�..
					*/
					}
			}
		}
		else
		{
			printf("\nSQL ó�� ���� : %s", pSQLBuffer->Buffer);
		}
	}
	
	CoUninitialize();  // never reach... ^^;
	return TRUE;
}