#pragma once
#include "AgcModule.h"
#include <string>
#include <map>
#include "AgcAuthUtill.h"
#include "AuMD5Encrypt.h"

//-----------------------------------------------------------------------
// 여러가지 인증 버전을 한가지 인터페이스로 통합합니다. - 2011.04.18 kdi

class AgcmLogin;
class AgcmUILogin;

class AgcAuth
{
protected:
	virtual bool	init			( std::string & cmdline );

public:
	virtual void	error_handling	();
	virtual void	final_process	();
	virtual bool	login			( AgcmLogin * cmLogin, AgcmUILogin * uiLogin );
	virtual bool	encrypt			( char * code, char * account, size_t account_size, char * password, size_t password_size );

	virtual			~AgcAuth		()	{};

	bool			init			( char const * cmdline );
	template< typename Value >
	Value			getProperty		( std::string key );
	template< typename Value >
	void			setProperty		( std::string key, Value value );
	friend			AgcAuth &	AuthManager();

protected:
					AgcAuth			()	{};
	bool			isTest();
	AuMD5Encrypt	md5Encrypt_;

private:
	typedef std::map< std::string, std::string > PropertyMap;

	PropertyMap		propertyMap_;

	void			setProperty_impl( std::string key, int value );
	void			setProperty_impl( std::string key, bool value );
	void			setProperty_impl( std::string key, std::string value );
};

//-----------------------------------------------------------------------
//

template< typename Value >
Value AgcAuth::getProperty( std::string key ) 
{
	AuAuthUtill::tolower( key );
	AuAuthUtill::caster< Value > cast;
	return cast( propertyMap_[key] );
}

//-----------------------------------------------------------------------
//

template< typename Value >
void AgcAuth::setProperty( std::string key, Value value )
{
	AuAuthUtill::tolower( key );
	setProperty_impl( key, value );
}

//-----------------------------------------------------------------------