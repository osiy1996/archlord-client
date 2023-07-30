#pragma once

#include <list>
#include <string>
#include <set>

enum	ePathType
{
	E_PATH_LOOP			,				//	������ ����Ʈ�� ���޽� ó����ġ�� �̵��ؼ� �ݺ�
	E_PATH_REVERSE		,				//	������ ����Ʈ�� ���޽� ������ �ݺ�

	E_PATH_COUNT		,
};


typedef struct	_stMonsterPathNode 
{

	INT		nX;			
	INT		nZ;

	INT		nPointIndex;

}STMonsterPathNode , *STMonsterPathNodePtr;

typedef std::list< STMonsterPathNode >					MonsterPathList;
typedef std::list< STMonsterPathNode >::iterator		MonsterPathListIter;

typedef struct	_stMonsterPathInfo
{
	_stMonsterPathInfo()	{	nPointCount	=	0;	bUpdate = FALSE; eType = E_PATH_LOOP;	}

	std::string				strSpawnName;			//	Spawn Name

	MonsterPathList			PathList;				//	��� ����Ʈ
	INT						nPointCount;			//	��� ����
	ePathType				eType;					//	���� ���

	BOOL					bUpdate;				//	Save�� ����Ǿ� �� ����

}STMonsterPathInfo , *STMonsterPathInfoPtr;