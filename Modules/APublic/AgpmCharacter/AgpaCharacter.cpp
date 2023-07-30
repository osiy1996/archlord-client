#include "AgpaCharacter.h"


/*****************************************************************
*   Function : AgpaCharacter()
*   Comment  : ������ 
*                    
*   Date&Time : 2002-04-15, ���� 4:01
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
AgpaCharacter::AgpaCharacter()
{
	
}

/*****************************************************************
*   Function : ~AgpaCharacter()
*   Comment  : �Ҹ��� 
*                    
*   Date&Time : 2002-04-15, ���� 4:01
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
AgpaCharacter::~AgpaCharacter()
{
	
}

/*****************************************************************
*   Function : GetCharacter(AgpdCharacter* , UINT32)
*   Comment  : Get Character Pointer by Character ID                    
*   Date&Time : 2002-04-16, ���� 12:17
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
AgpdCharacter*	AgpaCharacter::GetCharacter(INT32 lCID)
{
	AgpdCharacter **pvRetVal = (AgpdCharacter **) GetObject( lCID );

	if (pvRetVal)
		return *pvRetVal;

	return NULL;
}

/*****************************************************************
*   Function : GetCharacter(AgpdCharacter, CHAR*)
*   Comment  : Get Character Pointer by Character ID
*   Date&Time : 2002-04-16, ���� 12:23
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
AgpdCharacter*	AgpaCharacter::GetCharacter(CHAR*	szGameID )
{
	AgpdCharacter **pvRetVal = (AgpdCharacter **) GetObject( szGameID );

	if (pvRetVal)
		return *pvRetVal;

	return NULL;
}

/*****************************************************************
*   Function : AddCharacter(AgpdCharacter*, UINT32, CHAR*)
*   Comment  : Add Character Pointer. ( necessary CID, GameID )
*   Date&Time : 2002-04-16, ���� 2:05
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
AgpdCharacter*	AgpaCharacter::AddCharacter(AgpdCharacter* pCharacter, INT32 lCID, CHAR* szGameID)
{
	if (AddObject( (PVOID*) &pCharacter, lCID, szGameID ))
		return pCharacter;

	return NULL;
}

/*****************************************************************
*   Function : RemoveCharacter(UINT32)
*   Comment  : Eliminate Character by GameID
*   Date&Time : 2002-04-16, ���� 2:13
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
BOOL	AgpaCharacter::RemoveCharacter(INT32 lCID, CHAR* szGameID)
{
	return RemoveObject( lCID, szGameID );
}

//		IsCharacter
//	Functions
//		- character�� �����ϴ��� �˻��Ѵ�.
//	Arguments
//		- ulCID : ã�� character id
//	Return value
//		- BOOL : ���� ����
///////////////////////////////////////////////////////////////////////////////
BOOL AgpaCharacter::IsCharacter(INT32 ulCID)
{
	return IsObject(ulCID);
}

//		IsCharacter
//	Functions
//		- character�� �����ϴ��� �˻��Ѵ�.
//	Arguments
//		- szGameID : ã�� character game id
//	Return value
//		- BOOL : ���� ����
///////////////////////////////////////////////////////////////////////////////
BOOL AgpaCharacter::IsCharacter(CHAR* szGameID)
{
	return IsObject(szGameID);
}