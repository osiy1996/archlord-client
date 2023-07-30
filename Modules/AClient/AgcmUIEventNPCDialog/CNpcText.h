#ifndef __CLASS_NPC_TEXT_H__
#define __CLASS_NPC_TEXT_H__




#include <ApBase/ApBase.h>
#include <AgcmUIEventNPCDialog/CRandomText.h>




enum eNpcTextType
{
	NpcText_Common = 0,
	NpcText_Baloon,
	NpcText_MurdererLevel1,
	NpcText_MurdererLevel2,
	NpcText_MurdererLevel3,
};

struct stNpcTextEntry
{
	CRandomText*										m_pText;
	eNpcTextType										m_eType;

	__int64												m_nViewTimeMin;
	__int64												m_nViewTimeMax;

	stNpcTextEntry( void )
	{
		m_pText = NULL;
		m_eType = NpcText_Common;

		m_nViewTimeMin = 0;
		m_nViewTimeMax = 0;
	}
};

class CNpcText
{
private :
	ContainerMap< eNpcTextType, stNpcTextEntry >		m_mapNpcText;

	int													m_nNpcIndex;
	int													m_nNPCID;
	std::string											m_strNPCName;

public :
	CNpcText( void );
	~CNpcText( void );

public :
	BOOL				OnSetNpcInfo					( int nIndex, int nID, char* pName );

	BOOL				OnTextAdd						( eNpcTextType eType, char* pText, int nRate, __int64 nTimeMin = 0, __int64 nTimeMax = 0 );
	BOOL				OnTextClear						( void );

public :
	int					GetNpcID						( void ) { return m_nNPCID; }
	char*				GetNpcName						( void ) { return ( char* )m_strNPCName.c_str(); }
	char*				GetNpcText						( eNpcTextType eType );
};




#endif