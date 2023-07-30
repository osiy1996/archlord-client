#include <AgcmUISearch/AgcmUISearch.h>
#include <AgcmUILogin/AgcmUILogin.h>
#include <AuLocalize/AuRegionNameConvert.h>
#include <AuStrTable/AuStrTable.h>
#include <AgcmUIManager2/AgcmUIManager2.h>
#include <ApmMap/ApmMap.h>

// -----------------------------------------------------------------------------

AgcmUISearch::AgcmUISearch()
{
	m_pcsAgcmUISearch		= NULL;
	m_pcsAgcmUIManager2		= NULL;
	m_pcsAgpmSearch			= NULL;
	m_pcsApmMap				= NULL;

	m_pstUDSearchResultList	= NULL;
	
	m_lEventSearchResultUIOpen	= 0;
	m_lEventSearchResultUIClose	= 0;
	
	m_ICurrentResultSelectIndex	= 0;

	m_lMsgBoxSearchFailed		= 0;

	for (INT32 i = 0; i < AGPMSEARCH_MAX_COUNT; ++i)
		m_arSearchResult[i] = i;

	m_SearchResultList.reserve(AGPMSEARCH_MAX_COUNT);

	// module initialize
	SetModuleName("AgcmUISearch");
}

// -----------------------------------------------------------------------------

AgcmUISearch::~AgcmUISearch()
{
	m_SearchResultList.clear();
}


BOOL	AgcmUISearch::OnAddModule()
{
	m_pcsAgcmUISearch		= (AgcmUISearch*)GetModule("AgcmUISearch");
	m_pcsAgcmUIManager2		= (AgcmUIManager2*)GetModule("AgcmUIManager2");
	m_pcsAgpmSearch			= (AgpmSearch*)GetModule("AgpmSearch");
	m_pcsApmMap				= (ApmMap*)GetModule("ApmMap");

	if(!m_pcsAgpmSearch || !m_pcsAgcmUISearch || !m_pcsApmMap)
		return FALSE;

	if(!m_pcsAgpmSearch->SetCallbackResult(CBSearchResult, this))
		return FALSE;

	if (!AddEvent())
		return FALSE;

	if (!AddFunction())
		return FALSE;

	if (!AddUserData())
		return FALSE;
	
	if (!AddDisplay())
		return FALSE;

	return TRUE;
}

BOOL	AgcmUISearch::OnInit()
{
	/*AS_REGISTER_TYPE_BEGIN(AgcmUISearch, AgcmUISearch);
		AS_REGISTER_METHOD0(void, AddSearchResultList);
		AS_REGISTER_METHOD0(void, OpenSearchResultUI);
		AS_REGISTER_METHOD0(void, CloseSearchResultUI);
	AS_REGISTER_TYPE_END;*/
	
	return TRUE;
}

BOOL	AgcmUISearch::OnDestroy()
{
	m_RaceClassNameMap.clear();
	m_ClassNameMap.clear();

	return TRUE;
}

BOOL	AgcmUISearch::OnIdle(UINT32 ulClockCount)
{
	return TRUE;
}

BOOL	AgcmUISearch::AddEvent()
{
	//. UI Open/Close Event
	AGCMUI_CHECK_RETURN(m_lEventSearchResultUIOpen, m_pcsAgcmUIManager2->AddEvent("SeachResultUI_Open"));
	AGCMUI_CHECK_RETURN(m_lEventSearchResultUIClose, m_pcsAgcmUIManager2->AddEvent("SearchResultUI_Close"));

	//. Search Failed Msg Box.
	AGCMUI_CHECK_RETURN(m_lMsgBoxSearchFailed, m_pcsAgcmUIManager2->AddEvent("SearchFailed_MsgBox"));

	return TRUE;
}

BOOL	AgcmUISearch::AddFunction()
{
	if (!m_pcsAgcmUIManager2->AddFunction(this, "SeachReultList_Selected", CBSearchReultListSelect, 0))
		return FALSE;

	return TRUE;
}

BOOL	AgcmUISearch::AddUserData()
{
	m_pstUDSearchResultList = m_pcsAgcmUIManager2->AddUserData("SearchResultList_UD", &m_arSearchResult[0], sizeof(INT32), AGPMSEARCH_MAX_COUNT, AGCDUI_USERDATA_TYPE_INT32);
	if (NULL == m_pstUDSearchResultList)
		return FALSE;

	return TRUE;
}

#define UI_DISPLAY_CALLBACK_CHAR_NAME_INDEX			0
#define UI_DISPLAY_CALLBACK_CHAR_LEVEL_INDEX		1
#define UI_DISPLAY_CALLBACK_CHAR_RACE_INDEX			2
#define UI_DISPLAY_CALLBACK_CHAR_CLASS_INDEX		3
#define UI_DISPLAY_CALLBACK_CHAR_REGION_INDEX		4

BOOL	AgcmUISearch::AddDisplay()
{
	//. Search Result List Display callback
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "SearchResultList_Display_Name", UI_DISPLAY_CALLBACK_CHAR_NAME_INDEX, CBSearchResultList, AGCDUI_USERDATA_TYPE_INT32))	//. �̸�
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, "SearchResultList_Display_Level", UI_DISPLAY_CALLBACK_CHAR_LEVEL_INDEX, CBSearchResultList, AGCDUI_USERDATA_TYPE_INT32))	//. ����
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, "SearchResultList_Display_Race", UI_DISPLAY_CALLBACK_CHAR_RACE_INDEX, CBSearchResultList, AGCDUI_USERDATA_TYPE_INT32))	//. ����
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, "SearchResultList_Display_Class", UI_DISPLAY_CALLBACK_CHAR_CLASS_INDEX, CBSearchResultList, AGCDUI_USERDATA_TYPE_INT32))	//. Ŭ����
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, "SearchResultList_Display_Region", UI_DISPLAY_CALLBACK_CHAR_REGION_INDEX, CBSearchResultList, AGCDUI_USERDATA_TYPE_INT32))	//. ��ġ
		return FALSE;

	return TRUE;
}

//. Refresh List.
BOOL	AgcmUISearch::RefreshSearchResultList()
{
	m_pstUDSearchResultList->m_stUserData.m_lCount = m_SearchResultList.size();
	return m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDSearchResultList);
}

//. Packet������.
BOOL	AgcmUISearch::SendSearchCondition(eAgpmSearchPacketType etype, AgpdSearch& pstSearch)
{
	INT16 nPacketLength = 0;

	PVOID pvPacket = m_pcsAgpmSearch->MakeSearchPacket(&nPacketLength, etype, pstSearch);
		
	if (NULL == pvPacket)
	{
		m_pcsAgpmSearch->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmSearch->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

INT32	AgcmUISearch::FindClassName(TCHAR* szClassName)
{
	if(!szClassName)
		return 0;

	INT32	lIndex;

	if(m_RaceClassNameMap.empty())
	{
		//. ĳ���� Ŭ���� �̸� ����. 
		//. Ŭ���� �̸��� ����.
		//. ref. AgcmUIChatting2::ProcessChatMessage
		//. ref. AgcmUISearch::CBSearchResultList
		string	strClassName;
		char*	pszUIMessage;

		// ======================== Class -> ID

		pszUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_LG_CLASS_ARCHER);
		for (lIndex = 0; lIndex < (INT32) strlen(pszUIMessage); ++lIndex) pszUIMessage[lIndex] = tolower(pszUIMessage[lIndex]);
		strClassName.assign(pszUIMessage, strlen(pszUIMessage));
		m_RaceClassNameMap.insert(pair<string, INT32>(strClassName, 1) );		//. �޸վ�ó

		pszUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_LG_CLASS_KNIGHT);
		for (lIndex = 0; lIndex < (INT32) strlen(pszUIMessage); ++lIndex) pszUIMessage[lIndex] = tolower(pszUIMessage[lIndex]);
		strClassName.assign(pszUIMessage, strlen(pszUIMessage));
		m_RaceClassNameMap.insert(pair<string, INT32>(strClassName, 96) );		//. �޸ճ���Ʈ

		pszUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_LG_CLASS_MAGE);
		for (lIndex = 0; lIndex < (INT32) strlen(pszUIMessage); ++lIndex) pszUIMessage[lIndex] = tolower(pszUIMessage[lIndex]);
		strClassName.assign(pszUIMessage, strlen(pszUIMessage));
		m_RaceClassNameMap.insert(pair<string, INT32>(strClassName, 6) );		//. �޸ո�����

		pszUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_LG_CLASS_BERSERKER);
		for (lIndex = 0; lIndex < (INT32) strlen(pszUIMessage); ++lIndex) pszUIMessage[lIndex] = tolower(pszUIMessage[lIndex]);
		strClassName.assign(pszUIMessage, strlen(pszUIMessage));
		m_RaceClassNameMap.insert(pair<string, INT32>(strClassName, 4) );		//. ��ũ����Ŀ

		pszUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_LG_CLASS_HUNTER);
		for (lIndex = 0; lIndex < (INT32) strlen(pszUIMessage); ++lIndex) pszUIMessage[lIndex] = tolower(pszUIMessage[lIndex]);
		strClassName.assign(pszUIMessage, strlen(pszUIMessage));
		m_RaceClassNameMap.insert(pair<string, INT32>(strClassName, 8) );		//. ��ũ��ó

		pszUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_LG_CLASS_SORCERER);
		for (lIndex = 0; lIndex < (INT32) strlen(pszUIMessage); ++lIndex) pszUIMessage[lIndex] = tolower(pszUIMessage[lIndex]);
		strClassName.assign(pszUIMessage, strlen(pszUIMessage));
		m_RaceClassNameMap.insert(pair<string, INT32>(strClassName, 3) );		//. ��ũ�Ҽ���

		pszUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_LG_CLASS_RANGER);
		for (lIndex = 0; lIndex < (INT32) strlen(pszUIMessage); ++lIndex) pszUIMessage[lIndex] = tolower(pszUIMessage[lIndex]);
		strClassName.assign(pszUIMessage, strlen(pszUIMessage));
		m_RaceClassNameMap.insert(pair<string, INT32>(strClassName, 460) );		//. ������������

		pszUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_LG_CLASS_ELEMENTALIST);
		for (lIndex = 0; lIndex < (INT32) strlen(pszUIMessage); ++lIndex) pszUIMessage[lIndex] = tolower(pszUIMessage[lIndex]);
		strClassName.assign(pszUIMessage, strlen(pszUIMessage));
		m_RaceClassNameMap.insert(pair<string, INT32>(strClassName, 9) );		//. ������������Ż����Ʈ

		// ======================== ID -> Class

		pszUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_LG_CLASS_ARCHER);
		strClassName.assign(pszUIMessage, strlen(pszUIMessage));
		m_ClassNameMap.insert(pair<INT32, string>(1, strClassName) );			//. ��ó

		pszUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_LG_CLASS_KNIGHT);
		strClassName.assign(pszUIMessage, strlen(pszUIMessage));
		m_ClassNameMap.insert(pair<INT32, string>(96, strClassName) );			//. ����Ʈ

		pszUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_LG_CLASS_MAGE);
		strClassName.assign(pszUIMessage, strlen(pszUIMessage));
		m_ClassNameMap.insert(pair<INT32, string>(6, strClassName) );			//. ������

		pszUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_LG_CLASS_BERSERKER);
		strClassName.assign(pszUIMessage, strlen(pszUIMessage));
		m_ClassNameMap.insert(pair<INT32, string>(4, strClassName) );			//. ����Ŀ

		pszUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_LG_CLASS_HUNTER);
		strClassName.assign(pszUIMessage, strlen(pszUIMessage));
		m_ClassNameMap.insert(pair<INT32, string>(8, strClassName) );			//. ����

		pszUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_LG_CLASS_SORCERER);
		strClassName.assign(pszUIMessage, strlen(pszUIMessage));
		m_ClassNameMap.insert(pair<INT32, string>(3, strClassName) );			//. �Ҽ���

		pszUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_LG_CLASS_RANGER);
		strClassName.assign(pszUIMessage, strlen(pszUIMessage));
		m_ClassNameMap.insert(pair<INT32, string>(460, strClassName) );			//. ������

		pszUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_LG_CLASS_ELEMENTALIST);
		strClassName.assign(pszUIMessage, strlen(pszUIMessage));
		m_ClassNameMap.insert(pair<INT32, string>(9, strClassName) );			//. ������Ż����Ʈ

		pszUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_LG_CLASS_SWASHBUCKLER);
		strClassName.assign(pszUIMessage, strlen(pszUIMessage));
		m_ClassNameMap.insert(pair<INT32, string>(9, strClassName) );			//. ������Ż����Ʈ
	}

	for (lIndex = 0; lIndex < (INT32) strlen(szClassName); ++lIndex) szClassName[lIndex] = tolower(szClassName[lIndex])
		;
	string strName(szClassName);

	//. ���ڿ��� ĳ���� Ŭ�����̸����� ���Ͽ� �ش� TID����
	std::map<string, INT32>::iterator iter = m_RaceClassNameMap.find(strName);

	if(iter != m_RaceClassNameMap.end())
		return (INT32)(iter->second);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//. callback functions
BOOL	AgcmUISearch::CBSearchResult(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgpdSearch*	pcsSearch = (AgpdSearch*)pData;
	AgcmUISearch*	pThis = (AgcmUISearch*)pClass;

	//. ��������� ������.
	AgpdSearch	pdSearch;

	strcpy(pdSearch.m_szName, pcsSearch->m_szName);
	pdSearch.m_lTID = pcsSearch->m_lTID;
	pdSearch.m_lMinLevel = pcsSearch->m_lMinLevel;
	pdSearch.m_lMaxLevel = pcsSearch->m_lMaxLevel;
	pdSearch.m_stPos.x = pcsSearch->m_stPos.x;
	pdSearch.m_stPos.y = pcsSearch->m_stPos.y;
	pdSearch.m_stPos.z = pcsSearch->m_stPos.z;

	pThis->m_SearchResultList.push_back(pdSearch);

	//. â�� ���� ����� �����ش�.
	pThis->OpenSearchResultUI();

	return pThis->RefreshSearchResultList();
}

BOOL	AgcmUISearch::CBSearchFail(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//. display 
BOOL	AgcmUISearch::CBSearchResultList(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || eType != AGCDUI_USERDATA_TYPE_INT32)
		return FALSE;

	AgcmUISearch*	pThis = (AgcmUISearch*)pClass;
	INT32 Index = *(INT32*)pData;

	if ((INT32) pThis->m_SearchResultList.size() > pcsSourceControl->m_lUserDataIndex)
	{
		CHAR szBuffer[64];
		char* pszUIMessage = NULL;

		switch( lID )
		{
		case UI_DISPLAY_CALLBACK_CHAR_NAME_INDEX:
			{
				strncpy(szDisplay, pThis->m_SearchResultList[Index].m_szName, AGPDCHARACTER_NAME_LENGTH);
			}
			break;
		case UI_DISPLAY_CALLBACK_CHAR_LEVEL_INDEX:
			{
				sprintf(szBuffer, "Lv%d", pThis->m_SearchResultList[Index].m_lMinLevel);
				strcpy(szDisplay, szBuffer);
			}
			break;
		case UI_DISPLAY_CALLBACK_CHAR_RACE_INDEX:
			{
				if(pThis->m_SearchResultList[Index].m_lTID == 1 ||pThis->m_SearchResultList[Index].m_lTID == 96 ||pThis->m_SearchResultList[Index].m_lTID == 6)
				{
					//. ����Ʈ, ��ó, ������ => �޸�
					pszUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_LG_RACE_HUMAN);
				}
				else if(pThis->m_SearchResultList[Index].m_lTID == 4 ||pThis->m_SearchResultList[Index].m_lTID == 8 ||pThis->m_SearchResultList[Index].m_lTID == 3)
				{
					//. ����Ŀ, ����, �Ҽ��� => ��ũ
					pszUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_LG_RACE_ORC);
				}
				else if(pThis->m_SearchResultList[Index].m_lTID == 460 ||pThis->m_SearchResultList[Index].m_lTID == 9)
				{
					//. ������, ������Ż����Ʈ => ������
					pszUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_LG_RACE_MOONELF);
				}
				
				if(pszUIMessage)
					strncpy(szDisplay, pszUIMessage, strlen(pszUIMessage));
			}
			break;
		case UI_DISPLAY_CALLBACK_CHAR_CLASS_INDEX:
			{
				std::map<INT32, string>::iterator iter = pThis->m_ClassNameMap.find(pThis->m_SearchResultList[Index].m_lTID);
				
				if(iter != pThis->m_ClassNameMap.end())
					strncpy(szDisplay, iter->second.c_str(), iter->second.size());
			}
			break;
		case UI_DISPLAY_CALLBACK_CHAR_REGION_INDEX:
			{
				INT16 nRegionIndex = pThis->m_pcsApmMap->GetRegion(pThis->m_SearchResultList[Index].m_stPos.x, pThis->m_SearchResultList[Index].m_stPos.z);
				ApmMap::RegionTemplate *pstRegionTemplate = pThis->m_pcsApmMap->GetTemplate(nRegionIndex);

				if(pstRegionTemplate)
				{
					std::string point = RegionLocalName().GetStr( pstRegionTemplate->pStrName );
					if(point == "")
						strncpy(szDisplay, pstRegionTemplate->pStrName, strlen(pstRegionTemplate->pStrName));
					else
						strncpy(szDisplay, point.c_str(), point.size());
				}
			}
			break;
		}
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//. function
BOOL	AgcmUISearch::CBSearchReultListSelect(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//. script
void	AgcmUISearch::AddSearchResultList()
{
}

void	AgcmUISearch::OpenSearchResultUI()
{
	m_pcsAgcmUIManager2->ThrowEvent(m_lEventSearchResultUIOpen);
}

void	AgcmUISearch::CloseSearchResultUI()
{
	m_pcsAgcmUIManager2->ThrowEvent(m_lEventSearchResultUIClose);
}


// -----------------------------------------------------------------------------
// AgcmUISearch.cpp - End of file
// -----------------------------------------------------------------------------
