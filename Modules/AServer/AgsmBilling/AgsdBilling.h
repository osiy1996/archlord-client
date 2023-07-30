/*===================================================================

	AgsdBilling.h

===================================================================*/

#ifndef	_AGSD_BILLING_H_
	#define	_AGSD_BILLING_H_


#include "ApBase.h"
#include "AgpmCharacter.h"
#ifdef _AREA_CHINA_
#include "msgmacro.h"
#endif

/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
const enum eAGSMBILLING_TYPE
	{
	AGSMBILLING_TYPE_NONE = 0,
	AGSMBILLING_TYPE_MONEY,
	AGSMBILLING_TYPE_ITEM,
	};


const enum eAGSMBILLING_RESULT
	{
	AGSMBILLING_RESULT_FAIL = 0,
	AGSMBILLING_RESULT_SUCCESS,
	AGSMBILLING_RESULT_SYSTEM_FAILURE,
	AGSMBILLING_RESULT_NOT_ENOUGH_MONEY,
	};




/************************************************/
/*		The Definition of AgsdBilling class		*/
/************************************************/
//
//	==========	Base	==========
//
class AgsdBilling
	{
	protected:
		eAGSMBILLING_TYPE		m_eType;
		
	public :
		INT32					m_lID;
		AgpdCharacter			*m_pAgpdCharacter;
		INT32					m_lResult;
#ifdef _AREA_CHINA_
		char					m_sess_id[MAX_SESSID_LEN+1];//중국 샨다 빌링서버에 최초 인증시 얻어지는 값. 유저의 session 값.클라이언트 접속 종료때 까지 유지.supertj@20110119
		char					m_context_id[MAX_ORDERID_LEN+1]; //한 트랜잭션시에 동일하게 유지 되어야 하는 값.
		char					m_szOrder_ID[MAX_ORDERID_LEN+1]; // 주문해야하는 Order번호
#endif

	public :
		AgsdBilling()
		{
#ifdef _AREA_CHINA_
			ZeroMemory(m_sess_id, sizeof(m_sess_id));
#endif
			m_eType = AGSMBILLING_TYPE_NONE;
			m_lID = 0;
			m_pAgpdCharacter = NULL;
			m_lResult = AGSMBILLING_RESULT_FAIL;
		}
		
		virtual ~AgsdBilling()	
		{
#ifdef _AREA_CHINA_

			ZeroMemory(m_sess_id, sizeof(m_sess_id));
			ZeroMemory(m_context_id, sizeof(m_context_id));
			ZeroMemory(m_szOrder_ID, sizeof(m_szOrder_ID));
#endif
		}
		
		void Release()
			{
			delete this;
			}
			
		eAGSMBILLING_TYPE	Type()
			{
			return m_eType;
			}
		
		virtual BOOL	Decode(char *pszPacket, INT32 lCode) = NULL;
	};


//
//	==========	Money	==========
//
class AgsdBillingMoney : public AgsdBilling, public ApMemory<AgsdBillingMoney, 10000>
	{
	public :
		INT64		m_llMoney;
		INT64		m_llInternalEventMoney;
		INT64		m_llExternalEventMoney;
		INT64		m_llCouponMoney;
	
	public :
		AgsdBillingMoney()
			{
			m_eType = AGSMBILLING_TYPE_MONEY;
			m_llMoney = 0;
			m_llInternalEventMoney = 0;
			m_llExternalEventMoney = 0;
			m_llCouponMoney = 0;
			}
		
		virtual BOOL	Decode(char *pszPacket, INT32 lCode);
	};

class AgsdBillingMoneyGlobal : public AgsdBilling, public ApMemory<AgsdBillingMoneyGlobal, 10000>
{
public :
	INT64		m_lWCoin;
	INT64		m_lPCoin;

public :
	AgsdBillingMoneyGlobal()
	{
		m_eType = AGSMBILLING_TYPE_MONEY;
		m_lWCoin = 0;
		m_lPCoin = 0;
	}

	virtual BOOL	Decode(char *pszPacket, INT32 lCode) 
	{
		return TRUE; 
	};
};

//
//	==========	Item	==========
//
#ifdef _AREA_CHINA_
#define MAX_ORDERID_LEN 32
#endif

class AgsdBillingItem : public AgsdBilling, public ApMemory<AgsdBillingItem, 10000>
	{
	public :
		INT32		m_lProductID;
		INT64		m_llBuyMoney;
		UINT64		m_ullListSeq;
		INT32		m_lType;
		
		UINT64		m_ullOrderNo;
	
	public :
		AgsdBillingItem()
			{
			m_eType = AGSMBILLING_TYPE_ITEM;
			m_lProductID = 0;
			m_llBuyMoney = 0;
			m_ullListSeq = 0;
			m_ullOrderNo = 0;
			m_lType      = 0;
			#ifdef _AREA_CHINA_
			ZeroMemory(m_context_id,MAX_ORDERID_LEN+1);
			#endif
			}
		
		void Release()
			{
			delete this;
			}
	
		virtual BOOL	Decode(char *pszPacket, INT32 lCode);
	};


#endif
