// AgsmConfig.cpp
// (C) NHN Games - ArchLord Development Team
// kelovon, 20051006

#include "AgsmConfig.h"
#include "AgsModuleDBStream.h"
#include "AgsmSystemMessage.h"
#include "AgsmCharacter.h"
#include "AgpmCharacter.h"
#include "StrUtil.h"

AgsmConfig* AgsmConfig::m_pInstance = NULL;

AgsmConfig::AgsmConfig()
	: m_ulClockCount(0), pLuaState(0)
{
	m_pInstance = this;
	m_Mutex.Init();
	
	SetModuleName("AgsmConfig");
	SetModuleType(APMODULE_TYPE_PUBLIC);
	
	bDoubleEventState = FALSE;
	
	m_pagsmSystemMessage	= NULL;
	m_pagpmCharacter		= NULL;
	m_pcsAgpmConfig			= NULL;
	m_pcsAgsmServerManager2	= NULL;
	m_pagsmCharacter		= NULL;

	m_bReloadConfigLua = FALSE;

	pLuaState = lua_open();
	luaL_openlibs(pLuaState);
}

AgsmConfig::~AgsmConfig()
{
	if(pLuaState)
		lua_close(pLuaState);
}

BOOL AgsmConfig::OnAddModule()
{
	m_pcsAgpmConfig			= (AgpmConfig*)GetModule("AgpmConfig");
	m_pcsAgsmServerManager2	= (AgsmServerManager2*)GetModule("AgsmServerManager2");

	if (!m_pcsAgpmConfig
		|| !m_pcsAgsmServerManager2)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL AgsmConfig::OnInit()
{
	m_pagsmSystemMessage	= (AgsmSystemMessage*)GetModule("AgsmSystemMessage");
	m_pagsmCharacter		= (AgsmCharacter*)GetModule("AgsmCharacter");
	m_pagpmCharacter		= (AgpmCharacter*)GetModule("AgpmCharacter");

	if(m_pagsmCharacter)
	{
		if (!m_pagsmCharacter->SetCallbackEnterGameworld(CBSendEventState, this)) return FALSE;
	}
	
	//////////////////////////////////////////////////////////////////////////
	// lua
	
	if(pLuaState)
	{
		lua_register(pLuaState, "GetDoubleEventState",		AgsmConfig::GetDoubleEventState);
		lua_register(pLuaState, "SetDoubleEventState",		AgsmConfig::SetDoubleEventState);
		lua_register(pLuaState, "GetEventNumber",			AgsmConfig::GetEventNumber);

		m_bReloadConfigLua = TRUE;

		if(!LoadConfigLua())
			return FALSE;
	}
	
	return TRUE;
}

BOOL AgsmConfig::LoadConfigLua()
{
	if(m_bReloadConfigLua)
	{
		pluaPack.Open("INI\\Lua\\EventConfig.lua", (Decrypt_CFunction)StrUtil::Decrypt);

 		if(!pluaPack.dobuffer(pLuaState))
 			return FALSE;

		m_bReloadConfigLua = FALSE;
	}
	
	return TRUE;
}

BOOL AgsmConfig::OnTimer(UINT32 ulClockCount)
{
	INT_PTR GetOwningThreadID = m_Mutex.GetOwningThreadID();
	if ( GetOwningThreadID != 0 && GetOwningThreadID != ::GetCurrentThreadId())
		return FALSE;
	
	AuAutoLock pLock(m_Mutex);
	if(!pLock.Result())
		return FALSE;

	//////////////////////////////////////////////////////////////////////////

	if(ulClockCount < m_ulClockCount + 1000*60) return FALSE;

	m_ulClockCount = ulClockCount;

	LoadConfigLua();
	
	try
	{
		lua_getglobal(pLuaState, "OnTimer");
		int status = lua_pcall(pLuaState, 0, -1, 0);

		if (status != 0)
		{
			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "%s", lua_tostring(pLuaState, -1));
			AuLogFile_s("Log\\LuaError.log", strCharBuff);

			fprintf(stderr, "\t%s\n", lua_tostring(pLuaState, -1));
			lua_pop(pLuaState, 1);
			
			return FALSE;
		}
	}
	catch(...)
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "catch error on  %s", __FUNCTION__);
		AuLogFile_s("Log\\LuaError.log", strCharBuff);
		return FALSE;
	}

	return TRUE;
}

BOOL AgsmConfig::OnDestroy()
{
	return TRUE;
}

BOOL AgsmConfig::LoadConfig()
{
#ifndef _AREA_CHINA_
	AgsdServer2* pagsdServer = m_pcsAgsmServerManager2->GetThisServer();
	if (!pagsdServer)
		return FALSE;

	LPCTSTR szTmpValue = NULL;

	szTmpValue = pagsdServer->GetExtraValue(AGSMSERVER_EXTRA_KEY_IS_ADULT_SERVER);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetAdultServer( ((strnicmp("y", szTmpValue, 1)==0)?TRUE:FALSE) );
	}
	else
	{
		m_pcsAgpmConfig->SetAdultServer();
	}

	szTmpValue = pagsdServer->GetExtraValue(AGSMSERVER_EXTRA_KEY_EXP_ADJUSTMENT_RATIO);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetExpAdjustmentRatio((FLOAT)atof(szTmpValue));
	}
	else
	{
		m_pcsAgpmConfig->SetExpAdjustmentRatio();
	}

	szTmpValue = pagsdServer->GetExtraValue(AGSMSERVER_EXTRA_KEY_DROP_ADJUST_RATIO);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetDropAdjustmentRatio((FLOAT)atof(szTmpValue));
	}
	else
	{
		m_pcsAgpmConfig->SetDropAdjustmentRatio();
	}

	szTmpValue = pagsdServer->GetExtraValue(AGSMSERVER_EXTRA_KEY_GHELD_DROP_ADJUST_RATIO);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetGheldDropAdjustmentRatio((FLOAT)atof(szTmpValue));
	}
	else
	{
		m_pcsAgpmConfig->SetGheldDropAdjustmentRatio();
	}

	szTmpValue = pagsdServer->GetExtraValue(AGSMSERVER_EXTRA_KEY_ADM_CMD_TO_ALL);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetAllAdmin( ((strnicmp("y", szTmpValue, 1)==0)?TRUE:FALSE) );
	}
	else
	{
		m_pcsAgpmConfig->SetAllAdmin();
	}

	szTmpValue = pagsdServer->GetExtraValue(AGSMSERVER_EXTRA_KEY_IS_NEW_SERVER);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetNewServer( ((strnicmp("y", szTmpValue, 1)==0)?TRUE:FALSE) );
	}
	else
	{
		m_pcsAgpmConfig->SetNewServer();
	}
	
	szTmpValue = pagsdServer->GetExtraValue(AGSMSERVER_EXTRA_KEY_IS_EVENT_SERVER);
	if (NULL != szTmpValue)
		m_pcsAgpmConfig->SetEventServer( ((strnicmp("y", szTmpValue, 1)==0)?TRUE:FALSE) );
	else
		m_pcsAgpmConfig->SetEventServer(FALSE);

	szTmpValue = pagsdServer->GetExtraValue(AGSMSERVER_EXTRA_KEY_PC_DROP_ITEM);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetPCDropItemOnDeath( ((strnicmp("y", szTmpValue, 1)==0)?TRUE:FALSE) );
	}
	else
	{
		m_pcsAgpmConfig->SetPCDropItemOnDeath();
	}
	
	szTmpValue = pagsdServer->GetExtraValue(AGSMSERVER_EXTRA_KEY_EXP_PENALTY);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetExpPenaltyOnDeath( ((strnicmp("y", szTmpValue, 1)==0)?TRUE:FALSE) );
	}
	else
	{
		m_pcsAgpmConfig->SetExpPenaltyOnDeath();
	}

	szTmpValue = pagsdServer->GetExtraValue(AGSMSERVER_EXTRA_KEY_INI_DIR);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetIniDir( szTmpValue );
	}
	else
	{
		m_pcsAgpmConfig->SetIniDir();
	}

	szTmpValue = pagsdServer->GetExtraValue(AGSMSERVER_EXTRA_KEY_ACCOUNT_AUTH);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetAccountAuth(szTmpValue[0]);
	}
	else
	{
		m_pcsAgpmConfig->SetAccountAuth();
	}

	szTmpValue = pagsdServer->GetExtraValue(AGSMSERVER_EXTRA_KEY_IGNORE_LOG_FAIL);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetIgnoreLogFail( ((strnicmp("y", szTmpValue, 1)==0)?TRUE:FALSE));
	}
	else
	{
		m_pcsAgpmConfig->SetIgnoreLogFail();
	}

	szTmpValue = pagsdServer->GetExtraValue(AGSMSERVER_EXTRA_KEY_FILE_LOG);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetFileLog( ((strnicmp("y", szTmpValue, 1)==0)?TRUE:FALSE));
	}
	else
	{
		m_pcsAgpmConfig->SetFileLog();
	}

	szTmpValue = pagsdServer->GetExtraValue(AGSMSERVER_EXTRA_KEY_MAX_USER_COUNT);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetMaxUserCount(atoi(szTmpValue));
	}

	szTmpValue = pagsdServer->GetExtraValue(AGSMSERVER_EXTRA_KEY_ENC_PUBLIC);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetEncPublic(szTmpValue);
	}

	szTmpValue = pagsdServer->GetExtraValue(AGSMSERVER_EXTRA_KEY_ENC_PRIVATE);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetEncPrivate(szTmpValue);
	}

	szTmpValue = pagsdServer->GetExtraValue(AGSMSERVER_EXTRA_KEY_TEST_SERVER);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetTestServer((strnicmp("y", szTmpValue, 1)==0) ? TRUE : FALSE);
	}

	szTmpValue = pagsdServer->GetExtraValue(AGSMSERVER_EXTRA_KEY_START_LEVEL);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetStartLevel(atoi(szTmpValue));
	}

	szTmpValue = pagsdServer->GetExtraValue(AGSMSERVER_EXTRA_KEY_START_GHELD);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetStartGheld(_atoi64(szTmpValue));
	}

	szTmpValue = pagsdServer->GetExtraValue(AGSMSERVER_EXTRA_KEY_START_CHARISMA);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetStartCharisma(atoi(szTmpValue));
	}

	szTmpValue = pagsdServer->GetExtraValue(AGSMSERVER_EXTRA_KEY_JUMPING_START_LEVEL);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetJumpingStartLevel(atoi(szTmpValue));
	}

	szTmpValue = pagsdServer->GetExtraValue(AGSMSERVER_EXTRA_KEY_JUMPING_START_GHELD);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetJumpingStartGheld(_atoi64(szTmpValue));
	}

	szTmpValue = pagsdServer->GetExtraValue(AGSMSERVER_EXTRA_KEY_JUMPING_START_CHARISMA);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetJumpingStartCharisma(atoi(szTmpValue));
	}

	szTmpValue = pagsdServer->GetExtraValue(AGSMSERVER_EXTRA_KEY_ENABLE_AUCTION);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetEnableAuction((strnicmp("y", szTmpValue, 1)==0) ? TRUE : FALSE);
	}

	szTmpValue = pagsdServer->GetExtraValue(AGSMSERVER_EXTRA_KEY_AIM_EVENT_SERVER);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetAimEventServer((strnicmp("y", szTmpValue, 1)==0) ? TRUE : FALSE);
	}

	szTmpValue = pagsdServer->GetExtraValue(AGSMSERVER_EXTRA_KEY_EVENT_CHATTING);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetEventChatting((strnicmp("y", szTmpValue, 1)==0) ? TRUE : FALSE);
	}

	return TRUE;
#else
	m_mapConfigKeyValue.clear();

	AgsdServer2	*pThisServer = m_pcsAgsmServerManager2->GetThisServer();
	AgsdServer2 *pMasterServer = m_pcsAgsmServerManager2->GetMasterDBServer();
	if (!pThisServer || !pMasterServer)
		return FALSE;

	CHAR *pszString = NULL;
	CHAR szQuery[512];

	// Open DB
	eAUDB_VENDER eVender;
	CHAR *pszVender = pMasterServer->m_szDBVender;
	if (NULL == pszVender || 0 == strlen(pszVender) || 0 == stricmp(pszVender, "ORACLE"))
		eVender = AUDB_VENDER_ORACLE;
	else if (0 == stricmp(pszVender, "MSSQL"))
		eVender = AUDB_VENDER_MSSQL;
	else
		return FALSE;	
	
	AgsModuleDBStream csStream;
	csStream.Initialize(eVender);
	
	if (!csStream.OpenCustom(m_pcsAgsmServerManager2->m_szConfigDBDSN,
								m_pcsAgsmServerManager2->m_szConfigDBUser,
								m_pcsAgsmServerManager2->m_szConfigDBPwd) )
	{
		return FALSE;
	}

	// Query
	sprintf(szQuery, "select extra from serverconfig where section = %d", pThisServer->m_lServerID);
	if (!csStream.QueryCustom(szQuery))
		return FALSE;

	pszString = csStream.GetCustomValue(0, 0);

	ParseConfigString(pszString);

	LPCTSTR szTmpValue = NULL;

	szTmpValue = GetExtraValue(AGSMSERVER_EXTRA_KEY_IS_ADULT_SERVER);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetAdultServer( ((strnicmp("y", szTmpValue, 1)==0)?TRUE:FALSE) );
	}
	else
	{
		m_pcsAgpmConfig->SetAdultServer();
	}

	szTmpValue = GetExtraValue(AGSMSERVER_EXTRA_KEY_EXP_ADJUSTMENT_RATIO);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetExpAdjustmentRatio((FLOAT)atof(szTmpValue));
	}
	else
	{
		m_pcsAgpmConfig->SetExpAdjustmentRatio();
	}

	szTmpValue = GetExtraValue(AGSMSERVER_EXTRA_KEY_DROP_ADJUST_RATIO);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetDropAdjustmentRatio((FLOAT)atof(szTmpValue));
	}
	else
	{
		m_pcsAgpmConfig->SetDropAdjustmentRatio();
	}

	szTmpValue = GetExtraValue(AGSMSERVER_EXTRA_KEY_GHELD_DROP_ADJUST_RATIO);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetGheldDropAdjustmentRatio((FLOAT)atof(szTmpValue));
	}
	else
	{
		m_pcsAgpmConfig->SetGheldDropAdjustmentRatio();
	}

	szTmpValue = GetExtraValue(AGSMSERVER_EXTRA_KEY_ADM_CMD_TO_ALL);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetAllAdmin( ((strnicmp("y", szTmpValue, 1)==0)?TRUE:FALSE) );
	}
	else
	{
		m_pcsAgpmConfig->SetAllAdmin();
	}

	szTmpValue = GetExtraValue(AGSMSERVER_EXTRA_KEY_IS_NEW_SERVER);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetNewServer( ((strnicmp("y", szTmpValue, 1)==0)?TRUE:FALSE) );
	}
	else
	{
		m_pcsAgpmConfig->SetNewServer();
	}
	
	szTmpValue = GetExtraValue(AGSMSERVER_EXTRA_KEY_IS_EVENT_SERVER);
	if (NULL != szTmpValue)
		m_pcsAgpmConfig->SetEventServer( ((strnicmp("y", szTmpValue, 1)==0)?TRUE:FALSE) );
	else
		m_pcsAgpmConfig->SetEventServer(FALSE);

	szTmpValue = GetExtraValue(AGSMSERVER_EXTRA_KEY_PC_DROP_ITEM);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetPCDropItemOnDeath( ((strnicmp("y", szTmpValue, 1)==0)?TRUE:FALSE) );
	}
	else
	{
		m_pcsAgpmConfig->SetPCDropItemOnDeath();
	}
	
	szTmpValue = GetExtraValue(AGSMSERVER_EXTRA_KEY_EXP_PENALTY);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetExpPenaltyOnDeath( ((strnicmp("y", szTmpValue, 1)==0)?TRUE:FALSE) );
	}
	else
	{
		m_pcsAgpmConfig->SetExpPenaltyOnDeath();
	}

	szTmpValue = GetExtraValue(AGSMSERVER_EXTRA_KEY_INI_DIR);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetIniDir( szTmpValue );
	}
	else
	{
		m_pcsAgpmConfig->SetIniDir();
	}

	szTmpValue = GetExtraValue(AGSMSERVER_EXTRA_KEY_ACCOUNT_AUTH);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetAccountAuth(szTmpValue[0]);
	}
	else
	{
		m_pcsAgpmConfig->SetAccountAuth();
	}

	szTmpValue = GetExtraValue(AGSMSERVER_EXTRA_KEY_IGNORE_LOG_FAIL);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetIgnoreLogFail( ((strnicmp("y", szTmpValue, 1)==0)?TRUE:FALSE));
	}
	else
	{
		m_pcsAgpmConfig->SetIgnoreLogFail();
	}

	szTmpValue = GetExtraValue(AGSMSERVER_EXTRA_KEY_FILE_LOG);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetFileLog( ((strnicmp("y", szTmpValue, 1)==0)?TRUE:FALSE));
	}
	else
	{
		m_pcsAgpmConfig->SetFileLog();
	}

	szTmpValue = GetExtraValue(AGSMSERVER_EXTRA_KEY_MAX_USER_COUNT);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetMaxUserCount(atoi(szTmpValue));
	}

	szTmpValue = GetExtraValue(AGSMSERVER_EXTRA_KEY_ENC_PUBLIC);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetEncPublic(szTmpValue);
	}

	szTmpValue = GetExtraValue(AGSMSERVER_EXTRA_KEY_ENC_PRIVATE);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetEncPrivate(szTmpValue);
	}

	szTmpValue = GetExtraValue(AGSMSERVER_EXTRA_KEY_TEST_SERVER);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetTestServer((strnicmp("y", szTmpValue, 1)==0) ? TRUE : FALSE);
	}

	szTmpValue = GetExtraValue(AGSMSERVER_EXTRA_KEY_START_LEVEL);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetStartLevel(atoi(szTmpValue));
	}

	szTmpValue = GetExtraValue(AGSMSERVER_EXTRA_KEY_START_GHELD);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetStartGheld(_atoi64(szTmpValue));
	}

	szTmpValue = GetExtraValue(AGSMSERVER_EXTRA_KEY_ENABLE_AUCTION);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetEnableAuction((strnicmp("y", szTmpValue, 1)==0) ? TRUE : FALSE);
	}

	szTmpValue = GetExtraValue(AGSMSERVER_EXTRA_KEY_AIM_EVENT_SERVER);
	if (NULL != szTmpValue)
	{
		m_pcsAgpmConfig->SetAimEventServer((strnicmp("y", szTmpValue, 1)==0) ? TRUE : FALSE);
	}

	// AE¡¾aE¡©
	m_mapConfigKeyValue.clear();

	return TRUE;
#endif
}

BOOL AgsmConfig::ParseConfigString(TCHAR *szConfigString)
{
	TCHAR szKey[AGSMSERVER_MAX_EXTRA_VALUE+1];
	TCHAR szValue[AGSMSERVER_MAX_EXTRA_VALUE+1];

	TCHAR *psz = szConfigString;
	TCHAR *pszBuf = NULL;

	while (_T('\0') != *psz)
	{
		ZeroMemory(szKey, sizeof(szKey));
		ZeroMemory(szValue, sizeof(szValue));

		// key
		pszBuf = szKey;
		while (_T('\0') != *psz && AGSMSERVER_EXTRA_KEY_DELIM != *psz)
			*pszBuf++ = *psz++;
		if (AGSMSERVER_EXTRA_KEY_DELIM == *psz)
			psz++; // skip _AGSMDATABASECONFIG_DELIM1
		*pszBuf = _T('\0');

		// value
		pszBuf = szValue;
		while (_T('\0') != *psz && AGSMSERVER_EXTRA_VALUE_DELIM != *psz)
			*pszBuf++ = *psz++;
		if (AGSMSERVER_EXTRA_VALUE_DELIM == *psz)
			psz++; // skip 
		*pszBuf = _T('\0');

		if (_tcslen(szKey) > 0 && _tcslen(szValue) > 0)
		{
			SetExtraValue(szKey, szValue);
		}
	}

	return TRUE;
}

void AgsmConfig::SetExtraValue(LPCTSTR szKey, LPCTSTR szValue)
{
	ApString<AGSMSERVER_MAX_EXTRA_VALUE> szTmpKey(szKey); szTmpKey.MakeLower();
	ApString<AGSMSERVER_MAX_EXTRA_VALUE> szTmpValue(szValue); szTmpValue.MakeLower();

	m_mapConfigKeyValue.insert(pair<string, string>((LPCTSTR)szTmpKey, (LPCTSTR)szTmpValue));
}

LPCTSTR AgsmConfig::GetExtraValue(LPCTSTR szKey)
{
	ApString<AGSMSERVER_MAX_EXTRA_VALUE> szTmpKey(szKey); szTmpKey.MakeLower();
	map<string, string>::iterator itr = m_mapConfigKeyValue.find((LPCTSTR)szTmpKey);

	if (itr != m_mapConfigKeyValue.end())
	{
		return itr->second.c_str();
	}

	return NULL;
}

int AgsmConfig::GetDoubleEventState(lua_State* L)
{
	AgsmConfig* pThis = AgsmConfig::GetInstance();

	lua_pushboolean(L, pThis->bDoubleEventState);

	return 1;
}

int AgsmConfig::GetEventNumber(lua_State* L)
{
	AgsmConfig* pThis = AgsmConfig::GetInstance();

	int nEventNumber = pThis->m_pcsAgpmConfig->GetEventNumber();
	lua_pushinteger(L, nEventNumber);

	return 1;
}

int AgsmConfig::SetDoubleEventState(lua_State* L)
{
	AgsmConfig* pThis = AgsmConfig::GetInstance();
	
	BOOL bState				= (BOOL)lua_toboolean(L, 1);
	int ExpRatio			= (int)lua_tointeger(L, 2);
	int ItemDropRatio		= (int)lua_tointeger(L, 3);
	int GheldDropRatio		= (int)lua_tointeger(L, 4);
	int CharismaDropRatio	= (int)lua_tointeger(L, 5);
	BOOL bPVPItemDrop		= (BOOL)lua_toboolean(L, 6);
	BOOL bPVPExpDrop		= (BOOL)lua_toboolean(L, 7);
	BOOL bEnablePVP			= (BOOL)lua_toboolean(L, 8);
	int EventNumber			= (int)lua_tointeger(L, 9);
	int MessageNumber		= (int)lua_tointeger(L, 10);
	
	pThis->bDoubleEventState = bState;
	pThis->m_pcsAgpmConfig->SetEventNumber(EventNumber);
	pThis->m_pcsAgpmConfig->SetExpAdjustmentRatio((float)ExpRatio/100);
	pThis->m_pcsAgpmConfig->SetDropAdjustmentRatio((float)ItemDropRatio/100);
	pThis->m_pcsAgpmConfig->SetGheldDropAdjustmentRatio((float)GheldDropRatio/100);
/*
	pThis->m_pcsAgpmConfig->SetCharismaDropAdjustmentRatio((float)CharismaDropRatio/100);
	pThis->m_pcsAgpmConfig->SetEnablePvP(bEnablePVP);
	pThis->m_pcsAgpmConfig->SetEventItemDrop(bPVPItemDrop);
	pThis->m_pcsAgpmConfig->SetExpPenaltyOnDeath(bPVPExpDrop);*/

	AuXmlNode* pRoot = pThis->GetNoticeNodeDoubleEvent();
	if(!pRoot)
		return 0;

	for(AuXmlNode* pNode = pRoot->FirstChild("EventMessage"); pNode; pNode = pNode->NextSibling("EventMessage"))
	{
		AuXmlElement* pElemEventeNum = pNode->FirstChildElement("EventNumber");
		AuXmlElement* pElemMessageNum = pNode->FirstChildElement("MessageNumber");
		AuXmlElement* pElemMesssage = pNode->FirstChildElement("Message");

		if(pElemEventeNum && pElemMessageNum && pElemMesssage && 
			EventNumber == atoi(pElemEventeNum->GetText()) && 
			MessageNumber == atoi(pElemMessageNum->GetText()))
		{
			char strSystemMessage[AGPMSYSTEMMESSAGE_MAX_STRING_LENGTH];
			memset(strSystemMessage, 0, sizeof(strSystemMessage));
			sprintf( strSystemMessage, "%s", pElemMesssage->GetText());

			pThis->m_pagsmSystemMessage->SendSystemMessageAllUser(AGPMSYSTEMMESSAGE_CODE_GENERAL_STRING, -1, -1, strSystemMessage, NULL);
		}
	}

	//AuXmlElement* pElem = pRoot->FirstChildElement((bState) ? "Notice_01" : "Notice_02");
	//if(pElem)
	//{
	//	char strSystemMessage[AGPMSYSTEMMESSAGE_MAX_STRING_LENGTH];
	//	memset(strSystemMessage, 0, sizeof(strSystemMessage));
	//	sprintf( strSystemMessage, "%s", pElem->GetText());
	//
	//	pThis->m_pagsmSystemMessage->SendSystemMessageAllUser(AGPMSYSTEMMESSAGE_CODE_GENERAL_STRING, -1, -1, strSystemMessage, NULL);
	//}
	
	return 0;
}

BOOL AgsmConfig::CBSendEventState(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgsmConfig* pThis			= (AgsmConfig*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	
	if (!pThis->m_pagpmCharacter->IsPC(pcsCharacter) || !strlen(pcsCharacter->m_szID))
		return TRUE;
	
	if(!pThis->bDoubleEventState) 
		return TRUE;

	AuXmlNode* pRoot = pThis->GetNoticeNodeDoubleEvent();
	if(!pRoot)
		return FALSE;
	
	try
	{
		int EventNumber = pThis->m_pcsAgpmConfig->GetEventNumber();
		int MessageNumber = 3;

		for(AuXmlNode* pNode = pRoot->FirstChild("EventMessage"); pNode; pNode = pNode->NextSibling("EventMessage"))
		{
			AuXmlElement* pElemEventeNum = pNode->FirstChildElement("EventNumber");
			AuXmlElement* pElemMessageNum = pNode->FirstChildElement("MessageNumber");
			AuXmlElement* pElemMesssage = pNode->FirstChildElement("Message");

			if(pElemEventeNum && pElemMessageNum && pElemMesssage && 
				EventNumber == atoi(pElemEventeNum->GetText()) && 
				MessageNumber == atoi(pElemMessageNum->GetText()))
			{
				char strSystemMessage[AGPMSYSTEMMESSAGE_MAX_STRING_LENGTH];
				memset(strSystemMessage, 0, sizeof(strSystemMessage));
				sprintf( strSystemMessage, "%s", pElemMesssage->GetText());

				pThis->m_pagsmSystemMessage->SendSystemMessage( pcsCharacter , AGPMSYSTEMMESSAGE_CODE_GENERAL_STRING, -1, -1, strSystemMessage, NULL);
			}
		}
	}
	catch(...)
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "catch error on  %s", __FUNCTION__);
		AuLogFile_s("Log\\LuaError.log", strCharBuff);
		return FALSE;
	}
	
	return TRUE;
}

AuXmlNode* AgsmConfig::GetNoticeNodeDoubleEvent()
{
	static AuXmlNode* m_pNoticeNode = NULL;

	if(!m_pNoticeNode)
		m_pNoticeNode = m_pagsmSystemMessage->GetRootNode()->FirstChildElement("DoubleEvent");

	return m_pNoticeNode;
}

#ifndef _AREA_CHINA_
BOOL AgsmConfig::LoadTPackConfig()
{
	static AuXmlDocument m_csTPackXmlDoc;

	if(!m_csTPackXmlDoc.LoadFile(TPACKFILENAME))
		return FALSE;

	AuXmlNode *pTPackNode = m_csTPackXmlDoc.FirstChild("TPack");
	if(!pTPackNode)
		return FALSE;

	AuXmlNode *pBenefitNode = pTPackNode->FirstChild("Benefit");
	if(!pBenefitNode)
		return FALSE;

	m_pcsAgpmConfig->InitTPack();

	AuXmlElement *pElemTPackExp			= pBenefitNode->FirstChildElement("Exp");
	AuXmlElement *pElemTPackDrop		= pBenefitNode->FirstChildElement("Drop");
	AuXmlElement *pElemTPackGheld		= pBenefitNode->FirstChildElement("Gheld");
	AuXmlElement *pElemTPackConvert		= pBenefitNode->FirstChildElement("Convert");
	AuXmlElement *pElemTPackCharisma	= pBenefitNode->FirstChildElement("Charisma");

	if(pElemTPackExp)
		m_pcsAgpmConfig->SetTPackExpRatio(static_cast<FLOAT>(atof(pElemTPackExp->GetText())));

	if(pElemTPackDrop)
		m_pcsAgpmConfig->SetTPackDropRatio(static_cast<FLOAT>(atof(pElemTPackDrop->GetText())));

	if(pElemTPackGheld)
		m_pcsAgpmConfig->SetTPackGheldRatio(static_cast<FLOAT>(atof(pElemTPackGheld->GetText())));

	if(pElemTPackConvert)
		m_pcsAgpmConfig->SetTPackConvertRatio(static_cast<FLOAT>(atof(pElemTPackConvert->GetText())));

	if(pElemTPackCharisma)
		m_pcsAgpmConfig->SetTPackCharismaRatio(static_cast<FLOAT>(atof(pElemTPackCharisma->GetText())));

	return TRUE;
}
#endif