#pragma once
#include "sdkconfig.h"
#include "logindb.h"
#include "InfoCast.h"
#include "ThreadMaster.h"
#include "ConfigInterface.h"
#include "d3wrapper.h"
#include "base64.h"
#include "Lock\Lock.h"

using namespace CleverLock;

// Base of sdLoginDB
class PTSDK_STUFF CSDLoginDB : public CLoginDB,public CInfoNode
{
public:
	CSDLoginDB(void);
	virtual ~CSDLoginDB(void);

	int Init( CConfigInterface & config);
	virtual bool IsOpen(){ return true;}
	virtual bool Open(	const char * dbServer = NULL,
						const char * dbName = NULL, 
						const char * userName = NULL,
						const char * userPwd = NULL,
						int port = 0
						){return true;};

	virtual void CloseDB2() {};


	void CloseDB();
	void KeepOpen(int cycleTime=-1);	

protected:
	bool m_bAutoOpen;
	string m_DBServer, m_DBName, m_UserName, m_UserPwd;
	int	   m_Port;
	int m_nResetSize;	//重建连接上限数

	bool m_bEncrypt;
	CD3Wrapper m_Encrypter, m_Decrypter;
	void Encrypt(const string src, string & res);
	void Decrypt(const string src, string & res);

	CThreadMaster m_OpenKeeper;
	int m_CheckTimer;
	static void KeepOpen2(void * parameter);

	void MakeSQLField(const char * src, string &value);
};