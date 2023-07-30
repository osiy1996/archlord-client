#ifndef _AGCM_SYSTEM_MESSAGE_H_
#define _AGCM_SYSTEM_MESSAGE_H_

#include <ApModule/ApModule.h>

struct AgpdSystemMessage;

class AgcmUISystemMessage 
	:	public ApModule
{
private:
	class AgcmUIManager2		*m_pcsAgcmUIManager2;
	class AgcmChatting2		*m_pcsAgcmChatting;

public:
	AgcmUISystemMessage						( VOID );
	virtual ~AgcmUISystemMessage			( VOID );

	BOOL			OnAddModule				( VOID );
	BOOL			OnInit					( VOID );
	BOOL			OnDestroy				( VOID );

	static BOOL		CBSystemMessage			( PVOID pData, PVOID pClass, PVOID pCustData );
	
	BOOL			AddSystemMessage		( CHAR* szMsg, DWORD dwColor = 0XFFFFFF00 );
	BOOL			AddModalMessage			( CHAR* szMsg );
	BOOL			AddModallessMessage		( CHAR* szMsg );


	VOID			GetStringAndColor		( IN AgpdSystemMessage* pstSysMessage , OUT CHAR* szMessage , OUT DWORD& dwColor );
	
};

#endif // _AGCM_SYSTEM_MESSAGE_H_
