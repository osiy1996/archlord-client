//
// 2007.11.19. steeple
// ������ �ִ� �� ����.


#pragma once

#include <string>

class AuGameEnv
{
public:
	AuGameEnv( void );
	void InitEnvironment( void );

	bool IsAlpha    ( void ) { return m_isAlpha; }
	bool IsDebugTest( void ) { return m_isDebugTest; }
	bool IsAdmin	( void ) { return m_isAdmin; }

private:
	void	CheckOption( std::string& token );
	bool	m_isAlpha;		// ���� ȯ��
	bool	m_isDebugTest;	// ���� ȯ���� �� �����ϰ� ������ ����ȴ�.
	bool	m_isAdmin;
};

AuGameEnv& GetGameEnv( void );