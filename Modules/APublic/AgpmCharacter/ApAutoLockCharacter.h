#ifndef	_APAUTOLOCKCHARACTER_H_
#define _APAUTOLOCKCHARACTER_H_

#include <ApBase/ApDefine.h>

class AgpdCharacter;
class AgpmCharacter;

class ApAutoLockCharacter
{
public:
	ApAutoLockCharacter(AgpmCharacter *pcsAgpmCharacter, INT32 lCID);
	ApAutoLockCharacter(AgpmCharacter *pcsAgpmCharacter, CHAR *szGameID);
	~ApAutoLockCharacter();

	inline AgpdCharacter *GetCharacterLock()
	{
		return m_pcsAgpdCharacter;
	}
private:
	AgpdCharacter	*m_pcsAgpdCharacter;
};

#endif