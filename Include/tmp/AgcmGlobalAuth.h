#ifndef _AGCMGLOBALAUTH_H_
#define _AGCMGLOBALAUTH_H_

#include <string>

using std::string;

//�����޸𸮰� ������ �ִ� ���� ������ ������ ���� �����Ǵ� ������ ���̺귯�� tj@090526
#ifdef _SHARE_MEM_ //WEBZEN AUTHKEY ���� ����� �ϳ���.(����� �Ⱦ�)
	#ifdef _DEBUG
		#pragma comment(lib,"ShareMemory_d.lib")
	#else
		#pragma comment(lib,"ShareMemory.lib")
	#endif
#endif

#define GAMENAME "Archlord"
#define GAMEID_LEN 20
#define GAMESTR_LEN 2048

class AgcmGlobalAuth
{
public:
	AgcmGlobalAuth();
	~AgcmGlobalAuth();
#ifdef _SHARE_MEM_
	bool ProcessBySharedMemory();
#endif
	bool Initialize(char* szCmdLine);
	bool GetMACAddress( );

	const char* GetGameString()
	{
		return m_szGameString.c_str();
	}
	const char* GetGameAuthKey()
	{
		return m_szGameAuthKey.c_str();
	};
	const char* GetGameID()
	{
		return m_szGameId.c_str();
	};
	const char* GetAccountId()
	{ 
		return m_szAccountId.c_str();
	}
	bool GetAutoLogin()
	{
		return m_autoLogin;
	}
	
private:
	bool	m_autoLogin;
	string m_szGameString;
	string m_szAccountId;
	string m_szGameId;
	string m_szGameAuthKey;
};

extern AgcmGlobalAuth g_gbAuth; //global -> gb �� ����.

#endif//_AGCMGLOBALAUTH_H_