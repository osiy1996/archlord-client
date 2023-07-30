#pragma once
#include "sdkconfig.h"
#include "serverinterface.h"
#include "TCPConnection.h"
#include "DispatchSvr.h"
#include "ClientAgent.h"
#include "SDLoginDB.h"
#include "SDPacketEncrypt.h"
#include "AuthProcess.h"
#include "InfoCast.h"
#include "IniConfig.h"

class PTSDK_STUFF CAccountServer :
	public CServerInterface, private CInfoNode, public CInfoViewer
{
private:
	CAccountServer(void);
	~CAccountServer(void);
	
	//void myPTAuthen( 
	//	CSessionPtr & p_session,			// �����Ự�����ģ�������"ClientIP"
	//	const char * ptAccount,				// PT�˺�
	//	const char * password,				// ����
	//	const char * pwdEncryptCode = NULL	// ������ܷ�ʽ
	//	);

public:
	static CAccountServer * NewInstance();
	static void DeleteInstance(CAccountServer * p_as);
	CInfoNode * GetInfoNode();
	void Init(const char * iniFileName);
	void Init(CConfigInterface & config);

	void SetClientInterface(CClientInterface * p_client);

	// ��֤PT�˺ź��������ȷ��
	void PTAuthen( 
		CSessionPtr & p_session,			// �����Ự�����ģ�������"ClientIP"
		const char * ptAccount,				// PT�˺�
		const char * password,				// ����
		const char * pwdEncryptCode = NULL	// ������ܷ�ʽ
		);	

	// ʹ����Ϸ�˺Ž�����֤
	void GMAuthen(
		CSessionPtr & p_session,			// �����Ự������
		const char * gmAccount,				// ��Ϸ�˻�
		const char * password,				// ����
		const char * pwdEncryptCode = NULL	// ������ܷ�ʽ
		);

protected:

	CSDPacketEncrypt	m_Encrypter,
						m_Decrypter;

	CSDMsgPipe			m_ASPipe;

	vector<CTCPConnection*>	m_TCPConnections;

	CDispatchSvr		m_DispatchSvr;

	CAuthProcess		m_PreProcess;

	CSDLoginDB *		m_pLoginDB;

	static void ConnectionCheck(void * parameter);	
	CThreadMaster m_KeepConnection;

	vector< pair<string, int> > m_StaticAS;

	void Clear();

	bool ConnectToAS( CTCPConnection &connection);
	void MakeFirstPacket(CSDPacket & packet);
	virtual void GotInfo(int intInfo, const char * strInfo);
};

class PTSDK_STUFF CAccountServerPtr
{
public:
	CAccountServerPtr();
	~CAccountServerPtr();

	CAccountServer * GetPoint();
	CAccountServer& operator*(void);
	CAccountServer * operator->(void);
	CInfoNode	   * GetInfoNode();
private:
	CAccountServer * m_pAS;
};
