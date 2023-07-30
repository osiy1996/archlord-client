#include "StdAfx.h"
#include "SpawnDataLoad.h"

CSpawnDataLoad::CSpawnDataLoad( VOID )
{

}

CSpawnDataLoad::~CSpawnDataLoad( VOID )
{

}

BOOL	CSpawnDataLoad::Init( const string& strSpawnDataFile )
{

	AuExcelTxtLib		ExcelTxtLib;

	if (!ExcelTxtLib.OpenExcelFile( strSpawnDataFile.c_str() , TRUE) )
		return FALSE;

	INT32	lMaxRow			= ExcelTxtLib.GetRow();
	INT32	lMaxColumn		= ExcelTxtLib.GetColumn();

	INT32	lCurrentRow		= 1;
	INT32	lCurrentColumn	= 0;

	for ( ; ; )
	{
		if (lCurrentRow > lMaxRow)
			break;

		CHAR	*szBuffer	= ExcelTxtLib.GetData( 1 , lCurrentRow );

		if (szBuffer)
		{
			CHAR	*szColumnName	= ExcelTxtLib.GetData( 1 , 0 );
			if (!szColumnName)
				break;

			if (strcmp(szColumnName, AGPMSPAWN_SPAWN_NAME) == 0)
			{
					m_SpawnDataSet.insert( szBuffer );
			}

		}
	
		lCurrentRow++;
	}

	ExcelTxtLib.CloseFile();

	return TRUE;
}