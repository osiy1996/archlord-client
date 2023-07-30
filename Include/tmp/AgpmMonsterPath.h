#pragma once

// MK		2009. 5. 21

#include <string>
#include <map>
#include <set>
#include <vector>
#include "ApModule.h"
#include "AgpdMonsterPath.h"

using namespace std;

class AgpmMonsterPath
	:	public	ApModule
{
private:
	typedef	map< string , STMonsterPathInfoPtr >				MonsterPathMap;
	typedef map< string , STMonsterPathInfoPtr >::iterator		MonsterPathMapIter;	

public:
	AgpmMonsterPath									( VOID );
	virtual ~AgpmMonsterPath						( VOID );

	virtual		BOOL			OnInit				( VOID );
	virtual		BOOL			OnAddModule			( VOID );
	virtual		BOOL			OnDestroy			( VOID );

	//	�� �����
	VOID						AllClear			( VOID );							

	// Instance ����
	STMonsterPathInfoPtr		CreatePathInfo		( const string	&strSpawnName , ePathType eType = E_PATH_LOOP	);	//	Monster Path Info �߰�

	// Instance �����
	BOOL						DestroyPathInfo		( const string	&strSpawnName	);	//	Monster Path Info ����
	BOOL						DestroyPathInfo		( STMonsterPathInfoPtr	pMonsterInfo );

	// �ش� ���ϵ��� �ִ� ������ �����ϸ� �˾Ƽ� �� �о�ͼ� ����ȴ�
	BOOL						LoadPath			( const string	&strINIFolder	);	//	Monster Path�� ����� �������� ��� �о�´�

	// ������ INI ������ �����ϸ� �����Ѵ�
	// bUpdateListSave�� TRUE�� �ٲ� ���鸸 ����ȴ�
	// FALSE�� ������ �� ����
	BOOL						SavePath			( const string	&strINIFolder , BOOL bUpdateListSave = TRUE	);	

	BOOL						OneSavePath			( const string	&strINIFolder , const string	&strSpawnName );
	BOOL						OneSavePath			( const string	&strINIFolder , STMonsterPathInfoPtr	pMonsterPath );

	//	�ش� ������ ��� �߰�
	INT							AddMonsterPath		( STMonsterPathInfoPtr pMonsterPath , AuPOS Position );		
	INT							AddMonsterPath		( STMonsterPathInfoPtr pMonsterPath , INT nX , INT nZ );	
	INT							AddMonsterPath		( const string& strSpawnName , INT nX , INT nZ	);
	INT							AddMonsterPath		( const string& strSpawnName , AuPOS Position );

	// �ش� ������ ��� ����
	BOOL						EditMonsterPath		( const string& strSpawnName , INT nIndex ,  INT nX , INT nZ );
	BOOL						EditMonsterPath		( STMonsterPathInfoPtr pMonsterPath , INT nIndex ,  INT nX , INT nZ );

	// �ش� ������ ���� ��� ����
	BOOL						EditPathType		( STMonsterPathInfoPtr pMonsterPath , ePathType	eNewType );
	BOOL						EditPathType		( const string& strSpawnName , ePathType eNewType );

	// �ش� ������ ��� ����
	INT							DelMonsterPath		( STMonsterPathInfoPtr pMonsterPath , INT nIndex );			//	�ش� ������ ��� ����
	INT							DelMonsterPath		( STMonsterPathInfoPtr pMonsterPath , AuPOS Position );		//	�ش� ������ ��� ����
	INT							DelMonsterPath		( const string& strSpawnName , INT nIndex );
	INT							DelMonsterPath		( const string& strSpawnName , AuPOS Position );

	// Get
	inline STMonsterPathInfoPtr	GetMonsterPath		( const string& strSpawnName	);
	inline INT					GetMonsterPathCount	( VOID );							//	����

private:
	BOOL						_LoadPath			( const string	&strFullPath	);									//	LoadPath ���ο��� ���
	BOOL						_SavePath			( const string	&strFullPath , STMonsterPathInfoPtr pMonsterPath );	//	SavePath ���ο��� ���

	BOOL						_PushPathInfo		( STMonsterPathInfoPtr pPathInfo );	//	map�� Path ����Ÿ �߰�
	BOOL						_PopPathInfo		( STMonsterPathInfoPtr pPathInfo );	//	map�� Path ����Ÿ ����

	// Monster Path Data
	MonsterPathMap				m_MonsterPath;
	INT							m_nMonsterPathCount;
};