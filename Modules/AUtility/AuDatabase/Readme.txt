========================================================================
       STATIC LIBRARY : auDatabase
========================================================================

Usage 1:

	// �Ŵ��� ���� �� �ʱ�ȭ.
	auDatabaseManager Manager;
	Manager.Initialize(auDatabaseOLEDB, auOracle);		enum�� auDatabase.h ����

	// Heap�� ���� create�ϱ� ���Ϲ�...
	Manager.InitializeHeap(dwSize);

	// connection creation
	auConnection* pConnection = Manager.CreateConnecton();

	// connection open
	pConnection->Open(dsn, user, password);

	// if you wnat to use transaction facility
	pConnection->StartTransaction();

	// statment creation
	auStatement* pStatement = Manager.CreateStatement(pConnection);

	// statement open and execute
	pStatement->Open();
	pStatement->Execute(SQL);

	
	if (pStatement->HasResult)		// select
		;
	else			//if update, insert, delete, ...
		;

	auRowset* pRowset = Manger.CreateRowset(pStatement);
	
	pRowset->Bind(bBindToString = TRUE);	// �̶� �÷��� �÷��� ������.
											// bBindToString�� DB���� ���ۿ� ���ε��Ҷ� ��Ʈ������ ��ȯ�ؼ� ����
											// �ƴϸ� �� Ÿ�Ժ��� ������ ���Ѵ�. default�� string

	while (!pRowset->IsEnd())
		pRowset->Fetch(FetchCount))		// Fetch�� ����� ���� ����� �����Ǹ�
										// row count�� ������ fetch�� �� ����(������ �� ������� �ƴ�)

	LPCTSTR pHeader = pRowset->GetHeader(ulCol);		// �÷���

	for (r... GetRowCount()
		for (c... GetColCount()
			LPCTSTR pValue = pRowset->Get(r, c);		// ��. 

					
												


	pRowset->Release();

	�� instance���� close�� ��������� ���൵ �Ǳ� �����൵ ��.

Usage 2: alternative use of rowset

	auRowset* pRowset = Manger.CreateRowset(pStatement);
	pRowset->InitializeBuffer(size); !!! K-Byte !!!

	pStatement �ͱ׷��� ���� ��Ű��...

	prowset->Set(pStatement);		// reset used size (to 0)

	bind

	fetch




////////////////////////////////////////////////////////
�����ؾ� �� ����.

����� String���� Binding�Ѵ�. �� ��� Result Set�� ����� Ŀ���� ������ �ִ�.
�̸� �� Type���� Binding�� ���(eg. NUMBER(3) - short) auRowset�� GetHeader() ��� 
Bind�� Column�� ������ �˼��ֵ��� GetColumnInfo()�� ���� �޼ҵ带 �߰��ϰ�
Name, Type, Size���� �� �� �ֵ��� �ؾ��Ѵ�.
���� Get()�� �� Type�� �°� ������ �� �ֵ��� Ȯ��Ǿ�� �Ѵ�.

auEnvironmentOLEDB���� enumerator�� ������� �ʴ´�.
���� �ǵ��� �ش� eauDatabaseVender�� �޾� �ش� datasource�� ã���� �Ѱǵ�
�Ⱦ����ִ�. �׷��� �� ���... ���߿� �ʿ��Ϲ� �Ѵ�.

MySQL Test

Information method �߰��ض�.
		IsOpen()
		NumConnection();
		NumStatement() ���... 

Parameter Set/Get method�߰��ض�...

Type ����. ApDefine�� �ִ� �ѵ�� �����...

Database(connection)�� timeout�߰�...

BUG
	ODBC���� DATEŸ���� STRING(SQL_C_TCHAR)���� ���ε��� ���...
	�ο��� ���� NULL�϶� SQLFetch()���� ���� �߻�.
	Fetch�� �Ǳ������� ���� ���� NULL���� �ƴ��� �� �� ���� �ذ����� ����ϴ�.
	����Ŭ�� �׷����� �� �׷������� Ȯ���غ����Ѵ�.