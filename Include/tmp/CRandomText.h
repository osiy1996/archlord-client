#ifndef __CLASS_RANDOM_TEXT_H__
#define __CLASS_RANDOM_TEXT_H__





#include "ContainerUtil.h"
#include "Windows.h"
#include <string>



struct stRandomTextEntry
{
	std::string									m_strText;
	int											m_nRate;

	stRandomTextEntry( void )
	{
		m_strText = "";
		m_nRate = 0;
	}
};

class CRandomText
{
private :
	ContainerVector< stRandomTextEntry >		m_vecText;

public :
	CRandomText( void );
	~CRandomText( void );

public :
	BOOL				OnTextAdd				( char* pText, int nRate = 0 );
	char*				GetRandomText			( void );
};





#endif