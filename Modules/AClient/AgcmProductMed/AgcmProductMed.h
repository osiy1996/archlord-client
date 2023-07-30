/*===================================================================

	AgcmProductMed.h

===================================================================*/

#ifndef _AGCM_PRODUCT_MED_H_
	#define _AGCM_PRODUCT_MED_H_

#include <AgcModule/AgcModule.h>

#if _MSC_VER < 1300
#ifdef _DEBUG
	#pragma comment(lib, "AgcmProductMedD.lib")
#else
	#pragma comment(lib, "AgcmProductMed.lib")
#endif
#endif

class AgpdSkill;

/****************************************************/
/*		The Definition of AgcmProductMed class		*/
/****************************************************/
//
class AgcmProductMed : public AgcModule
	{
	private:
		//	Related modules
		class AgpmSkill				*m_pAgpmSkill;
		class AgcmSkill				*m_pAgcmSkill;
		class AgpmProduct			*m_pAgpmProduct;
		class AgcmCharacter			*m_pAgcmCharacter;
		class AgcmProduct			*m_pAgcmProduct;
		class AgcmUIProduct			*m_pAgcmUIProduct;
		class AgcmUIProductSkill	*m_pAgcmUIProductSkill;
		class AgcmUICooldown		*m_pAgcmUICooldown;

		//	
		AgpdSkill			*m_pAgpdSkill;
		INT32				m_lTargetID;
		//BOOL				m_bHold;
		
		INT32				m_lCooldownID;

	public:
		AgcmProductMed();
		virtual ~AgcmProductMed();

		//	ApModule inherited
		BOOL OnAddModule();
		
		//	Callback
		static BOOL	CBPreProcessSkill(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBActionSkillResult(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBAfterCooldown(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBProductResult(PVOID pData, PVOID pClass, PVOID pCustData);
		
		//	Result
		BOOL	OnResult( INT32 lResult, INT32 lSkillID, INT32 lItemTID, INT32 lExp, int nCount );
		
		//	Packet
		BOOL	SendPacketGather(INT32 lSkillID, INT32 lTargetID);
	};

#endif
