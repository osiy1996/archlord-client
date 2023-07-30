/******************************************************************************
Module:  AgsdServer.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 12. 10
******************************************************************************/

#if !defined(__AGSDSERVER_H__)
#define __AGSDSERVER_H__

#include <dplay8.h>
#include "ApBase.h"

#include "ApBase.h"
#include "AsDefine.h"

#define AGSMSERVER_MAX_CONTROL_AREA			20

typedef enum _eAGSMSERVER_TYPE {
	AGSMSERVER_TYPE_NONE					= 0,
	AGSMSERVER_TYPE_GAME_SERVER,
	AGSMSERVER_TYPE_MASTER_SERVER,
	AGSMSERVER_TYPE_BACKUP_SERVER,
	AGSMSERVER_TYPE_LOGIN_SERVER,
	AGSMSERVER_TYPE_CHARACTER_SERVER,
	AGSMSERVER_TYPE_ITEM_SERVER,
	AGSMSERVER_TYPE_DEAL_SERVER,
	AGSMSERVER_TYPE_RECRUIT_SERVER,
	AGSMSERVER_TYPE_RELAY_SERVER,
	AGSMSERVER_TYPE_MAX
} eAGSMSERVER_TYPE;

class AgsdControlArea {
public:
	AuAREA		m_stControlArea;

	INT16		m_nNumAroundUnit;
	PVOID*		m_pcsAroundUnit[7];		// ���� �� ������ ������ ������ �ٸ� ������ �����ϴ� �����̶�� ���⿡ �� ���� ����Ÿ �����͸� �ִ´�.
};

class AgsdServerTemplate : public ApBase {
public:
	INT32		m_lTID;
	CHAR		m_szName[AGSM_MAX_SERVER_NAME];
	CHAR		m_szGroupName[AGSM_MAX_SERVER_NAME];

	// server type
	UINT8		m_cType;	// AGSM_TYPE_GAME_SERVER
							// AGSM_TYPE_MASTER_SERVER
							// AGSM_TYPE_BACKUP_SERVER
							// AGSM_TYPE_LOGIN_SERVER
							// AGSM_TYPE_CHARACTER_SERVER
							// AGSM_TYPE_ITEM_SERVER
							// AGSM_TYPE_DEAL_SERVER
							// AGSM_TYPE_RECRUIT_SERVER

	// server address
	//CHAR		m_szIPv4[16];		// ip address (version 4)
	//UINT32	m_ulPort;			// port number
	CHAR		m_szIPv4Addr[23];	// expected form of "xxx.xxx.xxx.xxx:port"
	//CHAR		m_szIPv6[];			// reserved

	INT16		m_nControlArea;										// �����ϴ� ���� ����
	AgsdControlArea	m_csControlArea[AGSMSERVER_MAX_CONTROL_AREA];	// ���� �����ΰ�� �����ϴ� ����

	BOOL		m_bIsConnectLoginServer;	// �α� ������ ������ �ʿ䰡 �ִ� ���������� ��Ÿ��

	INT16		m_nServerFlag;				// ���� �Ƶ� �տ� ���� ���� �÷���
	INT16		m_nFlagSize;				// ���� �÷��� ����

	UINT64		m_ullItemDBIDStart;
	INT16		m_nItemDBIDServerFlag;		// Item DBID ���� ���� �÷���
	INT16		m_nItemDBIDFlagSize;		// flag size

	UINT64		m_ullSkillDBIDStart;
	INT16		m_nSkillDBIDServerFlag;		// Skill DBID ���� ���� �÷���
	INT16		m_nSkillDBIDFlagSize;		// flag size
};

class AgsdServer : public ApBase {
public:
	// basic information
	INT32		m_lServerID;

	INT32		m_lTID;

	DPNID		m_dpnidServer;	// DPNID of server
	INT16		m_nidServer;	// network_id of server

	INT16		m_nNumReconnect;

	INT16		m_nNumPlayer;	// ���� ó������ player ��...

	INT16		m_nStatus;		// �� ������ ���� ����. (active���� �ƴ��� ����� ���� ���´�)

	BOOL		m_bIsConnected;
	BOOL		m_bIsAccepted;

	BOOL		m_bIsAuth;		// ���ῡ ���� ������ �޾Ҵ��� ���� (connect ���� ����Ŷ�� ������ �װſ� ���� ������ ���������� ������ TRUE)

	time_t		m_lTime;		// ������ ���۵� �ð�.

	ApBase*		m_pcsTemplate;
};

#endif //__AGSDSERVER_H__