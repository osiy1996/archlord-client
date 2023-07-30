#pragma once

#include <string>
#include <vector>
#include <AuXmlParser/AuXmlParser.h>

using namespace std;

#ifdef _AREA_GLOBAL_
enum	eLoginGroupOper
{
	E_LOGIN_CONNECT		,		//	Server Connect 작업
	E_LOGIN_MESSAGEBOX	,		//	Message Box 띄우는 작업

	E_LOGIN_COUNT		,
};
#endif
struct	stLoginServerInfo 
{
	string				m_strServerIP;		//	Login Server IP
	BOOL				m_bTryConnect;		//	접속 시도했는지 여부

};

typedef vector< stLoginServerInfo >					VecServerInfo;
typedef vector< stLoginServerInfo >::iterator		VecServerInfoIter;

// 그룹에 매칭되는 로그인 서버 정보가 저장
struct	 stLoginGroup
{
	string				m_strGroupName;
	VecServerInfo		m_vecServerInfo;

#ifdef _AREA_GLOBAL_
	eLoginGroupOper		m_eOper;			//	어떤작업이 할당되었는지 확인
	string				m_strMessage;		//	메세지박스 띄울때 사용
#endif
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
