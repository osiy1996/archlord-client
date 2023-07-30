#pragma once
#include "sdkconfig.h"
#include "SessionInfo.h"
#include "SessionManage.h"
#include "ClientInterface.h"
#include "SDMsgPipe.h"
#include "SDMsgProcessor.h"
#include "ThreadMaster.h"
#include "LoginDB.h"
#include "QueueWithLock.h"
#include "Lock\Lock.h"
using namespace CleverLock;

typedef map<unsigned int, CSDMessage*> MAP_MSGEX;

class PTSDK_STUFF CServerInterface: public CSDMsgProcessor, public CSessionOwner, public CWithLock
{
public:
	CServerInterface(void);
	~CServerInterface(void);

	enum IdType{sdid = 0, ptid = 1, gmid = 2};

	// ��֤PT�˺ź��������ȷ��
	void PTAuthen( 
		CSessionPtr & p_session,			// �����Ự������
		const char * ptAccount,				// PT�˺�
		const char * password,				// ����
		const char * pwdEncryptCode = NULL	// ������ܷ�ʽ
											// "0",NULL: ������
											// "1": SHA1 ����SHA1�г��ֵ�0X00��0X01�滻
											// "2": MD5  ����MD5����ֵ�0X00��0X01�滻
											// "3": 3DES ���ܺ�Ŀ���ʹ��BASE64����
											// "4": �����ϵı߷�����㷨(��ת����Сд�ַ��������Ϸ�˺���Ϊ��Կ��������м���
		);	

	// ʹ����Ϸ�˺Ž�����֤
	void GMAuthen(
		CSessionPtr & p_session,			// �����Ự������
		const char * gmAccount,				// ��Ϸ�˻�
		const char * password,				// ����
		const char * pwdEncryptCode = NULL	// ������ܷ�ʽ
											// "0",NULL: ������
											// "1": SHA1 ����SHA1�г��ֵ�0X00��0X01�滻
											// "2": MD5  ����MD5����ֵ�0X00��0X01�滻
											// "3": 3DES ���ܺ�Ŀ���ʹ��BASE64����
											// "4": �����ϵı߷�����㷨(��ת����Сд�ַ��������Ϸ�˺���Ϊ��Կ��������м���
		);

	// Ϊ�ܱ���֤�ṩ�ܱ�,���δ������̬������֤����Ҫ�ֶ�����Session�Ự״̬�е���������
	//��ptid������ekeyType����"challenge"
	void GiveEkey (
		CSessionPtr & p_session,			// �����Ự������
		const char * ekey					// �û�������ܱ���
		);

	// ��֤�û��ı�������,���δ�����ܱ���֤,���ֶ�����Session�Ự״̬�еı���"ptid"
	void GiveReservePwd	( 
		CSessionPtr & p_session,			// �����Ự������
		const char * reservePwd				// �û��ı�������
		);

	// �û��޸�����
	void ChangePassword	( 
		CSessionPtr & p_session,			// �����Ự������
		const char * id,					// ����ID��POPTANG ID
		const char * oldPassword,			// �Ͼ�̬����
		const char * newPassword,			// �¾�̬����
		const char * ekey,					// �ܱ�
		const char * challenge				// ��ս��
		);

	// ����˺��Ƿ����
	int CheckAccount( 
		CSessionPtr & p_session ,			// �����Ự������
		const char * ptAccount				// �������˺�
		);

	// ������Ϸ�˺�ΪPT�˺�
	void UpgradeAccount	( 
		CSessionPtr & p_session,			// �����Ự������		
		const char * gmAccount,				// ��Ϸ�ʺ�
		const char * ptId,					// POPTANG ID
		const char * password,				// ����
		const char * name,					// ����
		const char * question1,				// ������ʾ����1
		const char * answer1,				// ������ʾ�����1
		const char * question2,				// ������ʾ����2
		const char * answer2,				// ������ʾ�����2
		const char * peopleId,				// ���֤
		const char * email,					// �����ʼ�
		const char * telnum,				// �绰
		const char * mobilenum,				// �ֻ�����
		const char * birthday,				// ����
		const char * address,				// ��ַ
		const char * privateInfo,			// ������Ϣ
		const char * sex,					// �Ա�
		const char * ip,					// IP��ַ
		const char * otherInfo = NULL		// ������Ϣ,Ŀǰֻ����Ϸ�������ʺ�
		);


	// ע��һ��PT�˺�
	void RegistPT( 
		CSessionPtr & p_session,			// �����Ự������
		const char * ptId,					// POPTANG ID
		const char * password,				// ����
		const char * name,					// ����
		const char * question1,				// ������ʾ����1
		const char * answer1,				// ������ʾ�����1
		const char * question2,				// ������ʾ����2
		const char * answer2,				// ������ʾ�����2
		const char * peopleId,				// ���֤
		const char * email,					// �����ʼ�
		const char * telnum,				// �绰
		const char * mobilenum,				// �ֻ�����
		const char * birthday,				// ����
		const char * address,				// ��ַ
		const char * privateInfo,			// ������Ϣ
		const char * sex,					// �Ա�
		const char * ip						// IP��ַ
		);

	// ͨ��pt�ʺ��޸���Ϸ�ʺ�
	void ChangeGameID(
		CSessionPtr & p_session,			// �����Ự������
		const char * ptId,					// POPTANG ID
		const char * newGameID				// �µ���Ϸ�˺�
		);

	void NewSession( CSessionPtr & p_session);
	void EndSession( CSessionPtr & p_session);

	static void MakeChallenge(string & challenge, int length);

	bool IsConnect();

	void SetLoginDB(CLoginDB * p_loginDB);
	void SetClientInterface( CClientInterface * p_client);
	void SetASPipe( CSDMsgPipe * p_asPipe);

	void SetWorkThreadCount(int count = 1);
	void SetSessionLife(int life);

	virtual void GotInfo(int intInfo, const char * strInfo)=0;

protected:

	int	m_GameType,		m_AreaNum;
	CLoginDB*			m_pLoginDB;
	CClientInterface*	m_pClientInterface;
	CSDMsgPipe*			m_pASPipe;
	CMutiThreadMaster	m_MsgProThread;
	CThreadMaster		m_ReboundMsgProThread;
	CThreadMaster		m_KeepAliveThread;
	//int	m_nLastSend;
	int m_nKeepAliveTimeout;

	CQueueWithLock<CSDMessage*> m_ComeMsgs;
	CQueueWithLock<CSDMessage*> m_ReboundMsgs;
	void Clear();

	void SetMsg( CSDMessage & msg);
	bool IsNum( const char * id);

	int  SendSDMessage( CSDMessage & msg );

	virtual int ProcessMsg(CSDMessage * p_msg, int flag = 0);
	static	void ProcessComeMsg(void * parameter);
	static	void ProcessReboundMsg(void * parameter);
	static	void KeepAliveThread(void * parameter);

	void LocalAuthen(CSessionPtr & p_session, int idType, const char *id, const char * password);
	int  LocalCheck(int idType, const char *id, const char * password, CLoginDB::S_UserInfo &userInfo);	
	void LocalReservePwdAuthen( CSessionPtr & p_session , const char * reservePwd);

    
	bool PtAuthResult		( CSessionPtr & p_session, CSDMessage & sdmessage);
	bool EkeyAuthResult		( CSessionPtr & p_session, CSDMessage & sdmessage);
	bool UpgradeResult		( CSessionPtr & p_session, CSDMessage & sdmessage);
	bool RegistResult		( CSessionPtr & p_session, CSDMessage & sdmessage);
	bool ChangePWDResult	( CSessionPtr & p_session, CSDMessage & sdmessage);
	bool CheckAccountResult	( CSessionPtr & p_session, CSDMessage & sdmessage);
	bool ChangeGameIDResult ( CSessionPtr & p_session, CSDMessage & sdmessage);
	bool PreProcessComeMsg	(CSDMessage& sdmessage);

	virtual void SessionTimeOut(CSessionPtr &session);
	unsigned int	m_SessionId;
	CSessionManage	m_SessionManage;

private:
	MAP_MSGEX m_mapMsgEx;
	CWithLock m_lockMsgEx;
	void PushMsgEx(CSDMessage* v_msg);
	CSDMessage* PopMsgEx(unsigned int v_transid);
};
