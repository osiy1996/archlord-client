#ifndef _AGSMEVENTSYSTEM_H
#define _AGSMEVENTSYSTEM_H

#include <AgsEngine.h>

class AgpmCharacter;
class AgsmCharacter;
class AgsmServerManager2;
class AgsmNpcManager;
class AgsmAccountManager;
class AgpmItem;
class AgsmItem;
class AgsmItemManager;
class AgpmLog;

class AgsmEventSystem : public AgsModule
{
	AgsmServerManager2*	m_pagsmServerManager2;
	AgpmCharacter*		m_pagpmCharacter;
	AgsmCharacter*		m_pagsmCharacter;
	AgsmNpcManager*		m_pagsmNpcManager;
	AgsmAccountManager*	m_pagsmAccountManager;
	AgpmItem*			m_pagpmItem;
	AgsmItem*			m_pagsmItem;
	AgsmItemManager*	m_pagsmItemManager;
	AgpmLog*			m_pagpmLog;

public:
	AgsmEventSystem();
	virtual ~AgsmEventSystem();

	BOOL OnInit();

	//////////////////////////////////////////////////////////////////////////
	// �⼮ üũ �ý��� - arycoat 2009.02
	BOOL ProcessAttendance(INT32 CID, INT32 NID, INT32 EID, INT32 STEP, INT32 Type);
	BOOL ProcessAttendanceResult(INT32 CID, INT32 NID, INT32 EID, INT32 STEP, INT32 Type, INT32 nResult);

	//////////////////////////////////////////////////////////////////////////
	// ������ �ο� �̺�Ʈ absenty 2009. 03. 11
	BOOL ProcessEventUser(INT32 CID, INT32 NID, INT32 EID, INT32 STEP, INT32 Type);
	BOOL ProcessEventUserResult(INT32 CID, INT32 NID, INT32 EID, INT32 STEP, INT32 Type, INT32 nResult);
	BOOL ProcessEventUserFlagUpdate(INT32 CID, INT32 NID, INT32 EID, INT32 STEP, INT32 Type, INT32 Flag);

	//////////////////////////////////////////////////////////////////////////
	// �̸� ���� �ý��� - arycoat 2009.03
	BOOL ProcessChangeName(INT32 CID, INT32 NID, INT32 EID, INT32 STEP);
	BOOL ProcessChangeNameResult(INT32 CID, INT32 NID, INT32 EID, INT32 STEP, INT32 nResult);

	//////////////////////////////////////////////////////////////////////////
	// ĳ���� ���� �ý��� - arycoat 2009.04.
	BOOL ProcessCerariumOrb(INT32 CID, INT32 NID, INT32 EID, INT32 STEP);
	BOOL CharacterSealing(INT32 CID, INT32 NID, INT32 EID, INT32 STEP, CHAR* strCharName);
	BOOL CharacterSealingResult(INT32 CID, INT32 NID, INT32 EID, INT32 STEP, INT32 ItemID, UINT64 DBID, INT32 nResult);
	BOOL CharacterSealingRequestList(INT32 CID, INT32 NID, INT32 EID, INT32 STEP);
	BOOL CharacterSealingResultList(INT32 CID, INT32 NID, INT32 EID, INT32 STEP, CHAR** strCharName, INT32* lSlot);
	BOOL CharacterSealingRequestCharacterName(INT32 CID, INT32 NID, INT32 EID, INT32 STEP);
	BOOL CharacterSealingResultCharacterName(INT32 CID, INT32 NID, INT32 EID, INT32 STEP, CHAR* strCharName);
	BOOL ProcessCharacterReleaseSealing(INT32 CID, INT32 NID, INT32 EID, INT32 STEP);
	BOOL SendRequestSealingDataAll();	
	BOOL ReceiveSealingData(INT32 ItemID, PVOID pcsSealData);
	BOOL CharacterReleaseSealingResult(INT32 CID, INT32 NID, INT32 EID, INT32 STEP, INT32 ItemID, UINT64 DBID, INT32 nResult);

	//////////////////////////////////////////////////////////////////////////
	// ĳ���� �����ð� üũ - �ڰ浵 2010.03.
	BOOL ProcessCharacterCreationDateCheck(INT32 CID, INT32 NID, INT32 EID, INT32 STEP, char* strStartDate, char* strEndDate);
	BOOL ProcessCharacterCreationDateCheckResult(INT32 CID, INT32 NID, INT32 EID, INT32 STEP, INT32 nResult);
};

#endif //_AGSMEVENTSYSTEM_H