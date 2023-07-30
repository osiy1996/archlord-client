#ifdef _AREA_JAPAN_
#pragma once

#include <vector>

class CJapaneseClientAuth
{
public:
	CJapaneseClientAuth();

	bool  Init( char* cmdLine );
	void  Refresh( void );
	bool  UpdateGamestring( char* newGamestring );
	char* HanAuthErrorPrint( int errorCode );

	// query
	const bool  GetAutoLogin ( void ) { return m_autoLogin; }
	const bool  IsRealService( void );
	
	const char* GetMemberID  ( void ) { return &m_memberID[0]; }
	const char* GetAuthString( void );
	char*       GetGameString( void ) { return &m_gamestring[0]; }
	
private:
	char* GetSecondParam( char* szCmdLine, int length = 4096 );

	// HanAuth�� ������ �ʰ� �Ϲ� �α��� ����ؼ� �� ��츦 ����
	// �׽�Ʈ ȯ�� ������ ��¿�� ���� �߰�
	bool			  m_autoLogin;

	char		  m_BillNo[32];	//billing number
	std::vector<char> m_gamestring; //���ӽ�Ʈ��
	std::vector<char> m_memberID; //�������̵�
	std::vector<char> m_authString;
};

extern CJapaneseClientAuth g_jAuth;

#endif //_AREA_JAPAN_