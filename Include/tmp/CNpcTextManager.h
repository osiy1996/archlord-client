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

public :	// �ܺο��� ȣ���ϴ� �Լ��� �Ʒ��� 3���� �Լ��� ȣ���Ѵ�.
	BOOL				OnLoadNpcDialogText				( char* pFileName, BOOL bIsEncrypt = TRUE );
	char*				GetNpcDialogText				( int nNpcIndex, void* pUserCharacter );
	BOOL				OnClearNpcDialogText			( void );

public :	// �Ʒ��� 4���� �Լ��� ���ο����� ����Ѵ�. ( �ܺο��� ����ص� �Ǳ� ������ �׷����� ������? �ϴ��� public ���� ������� private ���� �ص� ������� )
	BOOL				OnAddNpc						( int nNpcIndex, int nNpcID, char* pNpcName );

	BOOL				OnAddNpcDialogText				( int nNpcIndex, eNpcTextType eType, char* pText, char* pRate, char* pTimeMin = NULL, char* pTimeMax = NULL );
	BOOL				OnAddNpcDialogText				( int nNpcIndex, eNpcTextType eType, char* pText, int nRate, __int64 nTimeMin = 0, __int64 nTimeMax = 0 );
};




#endif