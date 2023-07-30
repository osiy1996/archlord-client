#pragma once

#include <string>
#include <vector>

#include "AuXmlParser.h"

using namespace std;



// Archlord.ini ���Ͽ��� ������ �о�´�
// �α��� IP �ּҸ� ���´�

struct	stLoginServerInfo 
{
	string				m_strServerIP;		//	Login Server IP
	BOOL				m_bTryConnect;		//	���� �õ��ߴ��� ����

};

typedef vector< stLoginServerInfo >					VecServerInfo;
typedef vector< stLoginServerInfo >::iterator		VecServerInfoIter;

// �׷쿡 ��Ī�Ǵ� �α��� ���� ������ ����
struct	 stLoginGroup
{
	string				m_strGroupName;
	VecServerInfo		m_vecServerInfo;
};


typedef vector< stLoginGroup* >				VecLoginGroup;
typedef vector< stLoginGroup* >::iterator	VecLoginGroupIter;



class AcArchlordInfo
{
private:


public:
	AcArchlordInfo				( VOID );
	virtual ~AcArchlordInfo		( VOID );

	BOOL						LoadFile				( IN BOOL bEncryt );
	VOID						Destroy					( VOID );

	stLoginGroup*				GetGroup				( INT nIndex );
	INT32						GetGroupCount			( VOID )	{	return (INT32)m_VecLoginGroup.size();	}

	stLoginServerInfo*			GetPatchServer			( INT nIndex );
	INT32						GetPatchServerCount		( VOID )	{	return (INT32)m_VecPatchServer.size();	}

private:
	BOOL						_LoadArchlordInfoXML	( IN CONST string& strFileName );

	AuXmlDocument				m_XMLDocument;
	AuXmlNode*					m_XMLRootNode;

	VecLoginGroup				m_VecLoginGroup;
	VecServerInfo				m_VecPatchServer;

};
