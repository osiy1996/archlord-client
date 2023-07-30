#pragma once
#include "stdafx.h"
#include "sdkconfig.h"
#include "sdmsgprocessor.h"
#include "InfoCast.h"
#include "ConfigInterface.h"
#include <string>
using namespace std;

class PTSDK_STUFF CDispatchSvr : public CSDMsgProcessor, public CInfoNode
{
public:
	CDispatchSvr(void);
	~CDispatchSvr(void);

	virtual int ProcessMsg(CSDMessage * p_msg, int flag = 0);

	bool GetAccountServer(string &ip, int &port);
	void Init( CConfigInterface & config);

protected:
	string	m_DSIP;
	int		m_DSPort;
	string	m_Key;

	int	m_GameType, m_AreaNum;
	HANDLE	m_hComeEvent;
	string	m_ASIP;
	int		m_ASPort;
	bool	m_bGot;
};
