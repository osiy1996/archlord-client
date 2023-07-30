#pragma once

#include "MyEngine.h"
#include <set>
#include <string>

typedef std::set< string  >					setSpawnData;
typedef std::set< string  >::iterator		setSpawnDataIter;

class CSpawnDataLoad
{
public:
	CSpawnDataLoad					( VOID );
	virtual ~CSpawnDataLoad			( VOID );

	BOOL							Init							( const string& strSpawnDataFile );
	setSpawnData&					GetSpawnDataSet					( VOID )	{	return m_SpawnDataSet;	}


protected:
	setSpawnData					m_SpawnDataSet;

};
