#include "AgsmBillingChina.h"
#include "AgsmCashMall.h"
#include "BillAPI.h"
#include "AuTimeStamp.h"


#include <stdio.h>

#define CRLF						"\r\n"

// -========================================================================-
CBillingBridge g_billingBridge;

namespace
{
	CBillAPI billing;
}

// -========================================================================-

CBillingBridge::CBillingBridge()
{
	InitializeCriticalSection(&m_cs);
}

CBillingBridge::~CBillingBridge()
{
	DeleteCriticalSection(&m_cs);
}

void CBillingBridge::SetCharManager(AgpmCharacter* pmChar, AgsmCharacter* smChar)
{
	m_pAgpmCharacter = pmChar;
	m_pAgsmCharacter = smChar;
}

void CBillingBridge::SetItemManager(AgpmItem* pmItem, AgsmItem* smItem, AgsmItemManager* itemManager)
{
	m_pAgpmItem = pmItem;
	m_pAgsmItem = smItem;
	m_pAgsmItemManager = itemManager;
}

void CBillingBridge::SetCashMallManager(AgpmCashMall* pmMall, AgsmCashMall* smMall)
{
	m_pAgpmCashMall = pmMall;
	m_pAgsmCashMall = smMall;
}

void CBillingBridge::SetBilling(AgsmBilling* billing)
{
	m_pAgsmBilling = billing;
}

void CBillingBridge::Add(char* uid, AgsdBilling* billing)
{
	Lock lock(m_cs);

	string strCharName(uid);

	StrBillingMap::iterator iter = m_map.find(strCharName);
	if (iter != m_map.end()) 
	{
		m_map.erase(strCharName);
	}

	m_map.insert(make_pair(strCharName, billing));
}

AgsdBilling* CBillingBridge::Find(char* uid)
{
	Lock lock(m_cs);

	string strCharName(uid);

	StrBillingMap::iterator iter = m_map.find(strCharName);
	return iter == m_map.end() ? 0 : iter->second;
}

void CBillingBridge::Remove(char* uid)
{
	Lock lock(m_cs);

	string strCharName(uid);

	m_map.erase(strCharName);
}

void CBillingBridge::AddItem(char *uid, AgsdBillingItem *pBillingItem)
{
	if(uid || pBillingItem)
	{
		Lock lock(m_cs);

		string strCharName(uid);

		StrBillingItemMap::iterator iter = m_BillItemMap.find(strCharName);
		if (iter != m_BillItemMap.end()) 
		{
			m_BillItemMap.erase(strCharName);
		}

		m_BillItemMap.insert(make_pair(strCharName, pBillingItem));
	}
}

void CBillingBridge::RemoveItem(char* uid)
{
	if(uid)
	{
		Lock lock(m_cs);

		string strCharName(uid);

		m_map.erase(strCharName);
	}
}

AgsdBillingItem* CBillingBridge::FindItem(char* uid)
{
	if(uid)
	{
		Lock lock(m_cs);

		string strName(uid);

		StrBillingItemMap::iterator iter = m_BillItemMap.find(strName);
		if(iter != m_BillItemMap.end())
			return iter->second;
	}

	return NULL;
}

// -========================================================================-
// version	:	v1.6.5 snda 빌링 모듈 버젼.
// desc		:	item charge 부분만 구현되어있음.
//				AccountAuthen/AccountLock/AccountUnlock 3 step으로 되어 있음.
//				인게임내에서만 Gold point 또는 Snda point 차감하기 위한 용도.
// step		:
// by supertj@20101103

AgsmBillingChina::AgsmBillingChina()
:	m_Sndabilling(billing)
{
	m_eBsipErr				= BSIP_ERR_NONE;
	m_err					= 0;
	m_ulInitializedClock	= 0;
	m_pApModule				= NULL;

	m_lServerIDItem			= 0;
	m_lAreaIDItem			= 0;
	m_lGroupIDItem			= 0;

	memset(m_logFileName, 0, sizeof(m_logFileName));

	SYSTEMTIME st;
	GetLocalTime(&st);

	sprintf_s(m_logFileName, "LOG\\CashLog_%d%02d%02d_%02d%02d%02d.log", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
}

AgsmBillingChina::~AgsmBillingChina()
{
	m_Sndabilling.m_pUninitialize(0);
	m_Sndabilling.FreeDll();
}

bool AgsmBillingChina::Init(ApModule *pModule)
{
	if (pModule)
		m_pApModule	= pModule;

	if (m_eBsipErr <= BSIP_ERR_LOAD_LIBRARY &&
		m_Sndabilling.LoadDll() != BSIP_OK)
	{
		printf_s("Can't load BSIP dll\n");
		m_eBsipErr	= BSIP_ERR_LOAD_LIBRARY;
		return false;
	}

	if (m_eBsipErr <= BSIP_ERR_GET_FUNC_ADDR &&
		m_Sndabilling.GetFunc() != BSIP_OK)
	{
		m_eBsipErr	= BSIP_ERR_GET_FUNC_ADDR;
		printf_s("Can't get func address\n");
		return false;
	}

	if (m_eBsipErr <= BSIP_ERR_SET_CALLBACK_FUNC &&
		m_Sndabilling.SetCallBackFunc() != BSIP_OK)
	{
		m_eBsipErr	= BSIP_ERR_SET_CALLBACK_FUNC;
		printf_s("Can't set callback func\n");
		return false;
	}

	if (m_eBsipErr < BSIP_ERR_GS_INITIALIZE)
		m_Sndabilling.IntSockEnv();

	if (m_eBsipErr <= BSIP_ERR_GS_INITIALIZE)
	{
		int ret = m_Sndabilling.m_pGSInitialize(&billing.m_funCallBack, "ini\\ConfigClient.ini", 0);
		if (BSIP_OK != ret)
		{
			m_eBsipErr	= BSIP_ERR_GS_INITIALIZE;
			m_err		= ret;
			printf_s("Can't initialize billing system, error code: %d\n", ret);
			return false;
		}
	}

	m_eBsipErr	= BSIP_ERR_SUCCESS;

	if (m_pApModule)
		m_ulInitializedClock	= m_pApModule->GetClockCount();

	/*
	printf("Waiting for connecting to BSIP\n");
	Sleep(5000);	// waiting 5s
	*/

	return true;
}

int AgsmBillingChina::SendAccountAuthen(char* account, char* ip, AgsdBilling *billing)
{
	if (!IsConnectedBSIP())
		return (-1);

	GSBsipAccountAuthenReqDef AccountAuthenReq = {0, };

	AccountAuthenReq.user_type = 1;//1--pt 2--sndaid
	AccountAuthenReq.uid_type =1;
	strncpy_s(AccountAuthenReq.user_id, MAX_PTID_LEN+1, account, _TRUNCATE);
	m_Sndabilling.m_pGetNewId(AccountAuthenReq.sess_id);//session id (처음 접속시에 받은 session_id를 접속 종료시까지 유지한다.)
#ifdef _AREA_CHINA_
	strncpy_s( billing->m_sess_id, MAX_SESSID_LEN+1, AccountAuthenReq.sess_id, _TRUNCATE );
#endif
	m_Sndabilling.m_pGetNewId(AccountAuthenReq.order_id);//order id
	AccountAuthenReq.serv_id_player = -1;
	AccountAuthenReq.area_id_player = -1;
	AccountAuthenReq.group_id_player = -1;
	AccountAuthenReq.endpoint_type = 1;
	AccountAuthenReq.platform_type = 1;  
	strcpy_s(AccountAuthenReq.endpoint_ip, "127.0.0.1");

	SYSTEMTIME st;
	GetLocalTime(&st);

	sprintf_s(AccountAuthenReq.call_time, "%d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	AccountAuthenReq.indication_authen  = 1; 
	AccountAuthenReq.pay_type  = 0; 

	// save auth info
	g_billingBridge.Add( account, billing ); // save a session id.

	// log
	AccountAuthenLog(account, ip, AccountAuthenReq.sess_id, AccountAuthenReq.order_id);
	return m_Sndabilling.m_pSendAccountAuthenRequest( &AccountAuthenReq );
}
// 빌링서버로 구매요청
int AgsmBillingChina::SendAccountLock(char* account, int itemID, int itemPrice, AgsdBillingItem* pBillingItem)
{
	if (!IsConnectedBSIP())
		return (-1);

	AgsdBilling* pBilling = g_billingBridge.Find(account);
	if(NULL == pBilling)
		return (-1);

	char szBuff[2048] = {0,};
	GSBsipAccountLockReqDef *AccountLockReq = (GSBsipAccountLockReqDef *)szBuff;

	AccountLockReq->pay_type	= 3;
	AccountLockReq->app_type	= 2;
	AccountLockReq->status		= 2;
	AccountLockReq->couple_type	= 2;
	AccountLockReq->fee			= 0;
	AccountLockReq->lock_period	= 0;
	AccountLockReq->uid_type	= 1;
	strncpy_s( AccountLockReq->user_id, MAX_PTID_LEN+1, account, _TRUNCATE );
#ifdef _AREA_CHINA_
	strncpy_s( AccountLockReq->sess_id, MAX_SESSID_LEN+1, pBilling->m_sess_id, _TRUNCATE );

	// 구매 프로세스 끝까지 유지해줘야하는 context_id
	m_Sndabilling.m_pGetNewId( AccountLockReq->context_id ); 

	// 갖고있는 context_id가 있으면 초기화 해준다.
	if(strlen(pBillingItem->m_context_id) >0)
	{
		ZeroMemory(pBillingItem->m_context_id, sizeof(pBillingItem->m_context_id));
	}

	strncpy_s( pBillingItem->m_context_id, MAX_ORDERID_LEN+1, AccountLockReq->context_id, _TRUNCATE);
#endif

	m_Sndabilling.m_pGetNewId( AccountLockReq->order_id );
	AccountLockReq->serv_id_player = -1;
	AccountLockReq->area_id_player = -1;
	AccountLockReq->group_id_player = -1;
	AccountLockReq->endpoint_type = 1;
	strcpy_s(AccountLockReq->endpoint_ip , "127.0.0.1");
	AccountLockReq->platform_type = 1;

	SYSTEMTIME st;
	GetLocalTime(&st);

	sprintf_s(AccountLockReq->call_time, "%d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	AccountLockReq->indication_lock = 1;
	AccountLockReq->item_num = 1;
	AccountLockReq->goods_info[0].area_id_item = m_lAreaIDItem;
	AccountLockReq->goods_info[0].serv_id_item = m_lServerIDItem;
	AccountLockReq->goods_info[0].group_id_item = m_lGroupIDItem;
	AccountLockReq->goods_info[0].item_id = itemID;
	AccountLockReq->goods_info[0].item_num = 1;
	AccountLockReq->goods_info[0].item_amount = itemPrice;
	AccountLockReq->goods_info[0].discount = 0;

	// save auth info context_id
	// 한 구매 트랜잭션에서 유지 되어야 할 값이므로 복사해둠
//	strncpy_s( billing->m_context_id, AccountLockReq->context_id, sizeof(AccountLockReq->context_id) ) ;
#ifdef _AREA_CHINA_
	g_billingBridge.AddItem( pBillingItem->m_context_id, pBillingItem );
#endif

	AccountLockLog(account, itemID, itemPrice, AccountLockReq->sess_id, AccountLockReq->order_id, AccountLockReq->context_id);

	return m_Sndabilling.m_pSendAccountLockRequest(AccountLockReq);
}

int AgsmBillingChina::SendAccountUnlock(int itemID, int itemPrice, GSBsipAccountLockResDef *pRes)
{
	if (!IsConnectedBSIP())
		return (-1);

	if(NULL == pRes)
		return (-1);

	SYSTEMTIME st;
	GetLocalTime(&st);

	AgsdBilling* pBilling = g_billingBridge.Find(pRes->user_id);
	if(NULL == pBilling)
		return (-1);

	AgsdBillingItem *pBillingItem = g_billingBridge.FindItem(pRes->context_id);
	if(NULL == pBillingItem)
		return (-1);

	char szBuff[2048] = {0,};
	GSBsipAccountUnlockReqDef *AccountUnLockReq = (GSBsipAccountUnlockReqDef *)szBuff;

	//the unlock request must be matched with the lock request
	AccountUnLockReq->uid_type = pRes->uid_type;
	strncpy_s( AccountUnLockReq->user_id, MAX_PTID_LEN + 1, pRes->user_id, _TRUNCATE );
#ifdef _AREA_CHINA_
	strncpy_s( AccountUnLockReq->sess_id, MAX_SESSID_LEN + 1, pBilling->m_sess_id, _TRUNCATE );	//session id
#endif
	strncpy_s( AccountUnLockReq->context_id, MAX_ORDERID_LEN + 1, pRes->context_id, _TRUNCATE );		// context_id
	m_Sndabilling.m_pGetNewId(AccountUnLockReq->order_id);							//order id : 주문 번호 아님 ;;;; 매 호출마다 새로 얻어준다.
	AccountUnLockReq->serv_id_player = -1;
	AccountUnLockReq->area_id_player = -1;
	AccountUnLockReq->group_id_player = -1;
	AccountUnLockReq->endpoint_type = 1; 
	AccountUnLockReq->platform_type = 1;
	strcpy_s( AccountUnLockReq->endpoint_ip , "127.0.0.1" );
	sprintf_s( AccountUnLockReq->call_time, "%d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond );

	AccountUnLockReq->app_type = 2;
	AccountUnLockReq->pay_type = 3; 
	AccountUnLockReq->couple_type =	2;
	AccountUnLockReq->status = 2;
	AccountUnLockReq->amount = 1 * itemPrice; //total account item_num * item_amount (무조건 한 거래에 하나씩만 산다)
	AccountUnLockReq->confirm =	1;//1- confirm lock; 2-cancel lock
	AccountUnLockReq->responsable	=	1 ; // 0 - server do not response; 1-server response
	AccountUnLockReq->indication_unlock	=	1;

	AccountUnLockReq->item_num	=	1; //  0 < item_num <= 5
	AccountUnLockReq->goods_info[0].area_id_item = m_lAreaIDItem; //area
	AccountUnLockReq->goods_info[0].serv_id_item = m_lServerIDItem ;//server
	AccountUnLockReq->goods_info[0].group_id_item = m_lGroupIDItem ;//group
	AccountUnLockReq->goods_info[0].item_id = itemID; //item id
	AccountUnLockReq->goods_info[0].item_num = 1; // number
	AccountUnLockReq->goods_info[0].item_amount = itemPrice; // account
	AccountUnLockReq->goods_info[0].discount = 0;//discount

	AccountUnlockLog( pRes->user_id, itemID, itemPrice, AccountUnLockReq->sess_id, AccountUnLockReq->order_id, pRes->context_id);

	return m_Sndabilling.m_pSendAccountUnlockRequest( AccountUnLockReq );
}

void AgsmBillingChina::WriteErrorLog()
{
	CHAR	szBuffer[32];
	ZeroMemory(szBuffer, sizeof(szBuffer));
	AuTimeStamp::GetCurrentTimeStampString(szBuffer, 32);

	switch (m_eBsipErr) {
		case BSIP_ERR_LOAD_LIBRARY:
			{
				char strCharBuff[256] = { 0, };
				sprintf_s(strCharBuff, sizeof(strCharBuff), "[%s] Can't load BSIP dll\n", szBuffer);
				AuLogFile_s("LOG\\BsipErr.txt", strCharBuff);
				break;
			}
		case BSIP_ERR_GET_FUNC_ADDR:
			{
				char strCharBuff[256] = { 0, };
				sprintf_s(strCharBuff, sizeof(strCharBuff), "[%s] Can't get func address\n", szBuffer);
				AuLogFile_s("LOG\\BsipErr.txt", strCharBuff);
				break;
			}
		case BSIP_ERR_SET_CALLBACK_FUNC:
			{
				char strCharBuff[256] = { 0, };
				sprintf_s(strCharBuff, sizeof(strCharBuff), "[%s] Can't set callback func\n", szBuffer);
				AuLogFile_s("LOG\\BsipErr.txt", strCharBuff);
				break;
			}
		case BSIP_ERR_GS_INITIALIZE:
			{
				char strCharBuff[256] = { 0, };
				sprintf_s(strCharBuff, sizeof(strCharBuff), "[%s] Can't initialize billing system, error code: %d\n", szBuffer, m_err);
				AuLogFile_s("LOG\\BsipErr.txt", strCharBuff);
				break;
			}
	}

	return;
}

void AgsmBillingChina::CheckConnection()
{
	if (IsConnectedBSIP())
		return;

	Init();

	return;
}

bool AgsmBillingChina::IsConnectedBSIP()
{
	UINT32	ulCurrentClockCount	= 0;
	if (m_pApModule)
		ulCurrentClockCount	= m_pApModule->GetClockCount();

	if (ulCurrentClockCount > 0)
	{
		if (m_eBsipErr == BSIP_ERR_SUCCESS &&
			m_ulInitializedClock + 5000 < ulCurrentClockCount)
			return true;
	}
	else
	{
		if (m_eBsipErr == BSIP_ERR_SUCCESS)
			return true;
	}

	return false;
}

void AgsmBillingChina::AccountAuthenLog(char* account, char* ip, char* sessid, char* orderid)
{
	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "AccountAuthen: %s, %s, %s, %s\n", account, ip, sessid, orderid);
	AuLogFile_s(m_logFileName, strCharBuff);
}

void AgsmBillingChina::AccountAuthenResLog(char* account, int balance)
{
	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "AccountAutheRes: %s, %d\n", account, balance);
	AuLogFile_s(m_logFileName, strCharBuff);
}

void AgsmBillingChina::AccountLockLog(char* account, int itemID, int itemPrice, char* sessid, char* orderid, char* contextid)
{
	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "AccountLock: %s, %d, %d, %s, %s, %s\n", account, itemID, itemPrice, sessid, orderid, contextid);
	AuLogFile_s(m_logFileName, strCharBuff);
}

void AgsmBillingChina::AccountLockResLog(char* account, char* ip, int itemID, int itemPrice, char* orderid)
{
	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "AccountLockRes: %s, %s, %d, %d, %s\n", account, ip, itemID, itemPrice, orderid);
	AuLogFile_s(m_logFileName, strCharBuff);
}

void AgsmBillingChina::AccountUnlockLog(char* account, int itemID, int itemPrice, char* sessid, char* orderid, char* contextid)
{
	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "AccountUnlock: %s, %d, %d, %s, %s, %s\n", account, itemID, itemPrice, sessid, orderid, contextid);
	AuLogFile_s(m_logFileName, strCharBuff);
}

void AgsmBillingChina::SomethingWrong(char* log)
{
	char strCharBuff[512] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "Error: %s\n", log);
	AuLogFile_s(m_logFileName, strCharBuff);
}

BOOL AgsmBillingChina::SettingSNDAParam(const CHAR* szFileName)
{
	if(NULL == szFileName)
		return FALSE;

	HANDLE hConfigFile = ::CreateFile(_T(szFileName), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if( hConfigFile == INVALID_HANDLE_VALUE)
		return FALSE;

	// 마지막에 NULL 문자열을 추가해야 하니까 파일사이즈 + 1 해서 초기화한다.
	DWORD dwBufferSize = ::GetFileSize( hConfigFile, NULL ) + 1;
	CHAR* pBuffer = new CHAR[ dwBufferSize ];
	memset( pBuffer, 0, sizeof( CHAR ) * dwBufferSize );

	CHAR szDummy[1024];
	ZeroMemory(szDummy, sizeof(szDummy));

	DWORD dwReadByte = 0;
	if( ::ReadFile( hConfigFile, pBuffer, dwBufferSize, &dwReadByte, NULL ) )
	{	
		std::string FileBuffer(pBuffer);
		std::string line;

		std::istringstream stream(FileBuffer);

		while(std::getline(stream, line))
		{
			std::string key;
			std::string value;

			std::string::size_type idx = line.find('=');

			if(idx == std::string::npos)
				continue;
			else
			{
				key		= line.substr(0, idx);
				value	= line.substr(idx+1);

				if( strncmp(key.c_str(), AGSM_SNDA_BILLING_SERVER_ID_ITEM, strlen(AGSM_SNDA_BILLING_SERVER_ID_ITEM)) == 0 )
				{
					m_lServerIDItem		= atoi(value.c_str());
				}
				else if ( strncmp(key.c_str(), AGSM_SNDA_BILLING_AREA_ID_ITEM, strlen(AGSM_SNDA_BILLING_AREA_ID_ITEM)) == 0 )
				{
					m_lAreaIDItem		= atoi(value.c_str());
				}
				else if( strncmp(key.c_str(), AGSM_SNDA_BILLING_GROUP_ID_ITEM, strlen(AGSM_SNDA_BILLING_GROUP_ID_ITEM)) == 0 )
				{
					m_lGroupIDItem		= atoi(value.c_str());
				}
			}
		}		
	}

	return TRUE;
}
