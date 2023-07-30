#pragma once

#ifndef PTSDK_DLL
#	define PTSDK_DLL
#endif
#include "ServerInterface.h"
#include "ConfigInterface.h"
#include "InfoCast.h"

class PTSDK_STUFF CAccountServer :
	public CServerInterface/*, private CInfoNode*/, private CInfoViewer
{
private:
	CAccountServer(void);
	~CAccountServer(void);
public:
	static CAccountServer * NewInstance();
	static void DeleteInstance(CAccountServer * p_as);
	CInfoNode * GetInfoNode();
	void Init(const char * iniFileName);
	void Init(CConfigInterface & config);

	void SetClientInterface(CClientInterface * p_client);

	// ��֤PT�˺ź��������ȷ��
	void PTAuthen( 
		CSessionPtr & p_session,			// �����Ự������
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

	(operator CInfoNode &)();
	(operator CServerInterface&)();
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
