#ifndef __CLASS_NPC_TEXT_MANAGER_H__
#define __CLASS_NPC_TEXT_MANAGER_H__




#include "CNpcText.h"




struct stNpcDialogEntry
{
	int													m_nIndex;
	CNpcText*											m_pNpc;

	stNpcDialogEntry( void )
	{
		m_nIndex = -1;
		m_pNpc = NULL;
	}
};

class CNpcTextManager
{
private :
	ContainerMap< int, stNpcDialogEntry >				m_mapNpcText;

public :
	CNpcTextManager( void );
	virtual ~CNpcTextManager( void );

public :	// 외부에서 호출하는 함수는 아래의 3가지 함수만 호출한다.
	BOOL				OnLoadNpcDialogText				( char* pFileName, BOOL bIsEncrypt = TRUE );
	char*				GetNpcDialogText				( int nNpcIndex, void* pUserCharacter );
	BOOL				OnClearNpcDialogText			( void );

public :	// 아래의 4가지 함수는 내부에서만 사용한다. ( 외부에서 사용해도 되긴 하지만 그럴일이 없을듯? 일단은 public 으로 잡았으나 private 으로 해도 상관없음 )
	BOOL				OnAddNpc						( int nNpcIndex, int nNpcID, char* pNpcName );

	BOOL				OnAddNpcDialogText				( int nNpcIndex, eNpcTextType eType, char* pText, char* pRate, char* pTimeMin = NULL, char* pTimeMax = NULL );
	BOOL				OnAddNpcDialogText				( int nNpcIndex, eNpcTextType eType, char* pText, int nRate, __int64 nTimeMin = 0, __int64 nTimeMax = 0 );
};




#endif