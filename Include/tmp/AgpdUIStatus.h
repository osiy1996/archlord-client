#ifndef	__AGPDUISTATUS_H__
#define	__AGPDUISTATUS_H__

#include "ApBase.h"

#define	AGPMUISTATUS_MAX_QUICKBELT_STRING			512

#define	AGPMUISTATUS_MAX_QUICKBELT_COLUMN			10
#define	AGPMUISTATUS_MAX_QUICKBELT_LAYER			4

#define	AGPMUISTATUS_HALF_QUICKBELT_COLUMN			5

#define AGPMUISTATUS_MAX_COOLDOWN_STRING			512

class AgpdUIStatusADChar {
public:
	//ApBaseExLock	m_acsQBeltItem[AGPMUISTATUS_MAX_QUICKBELT_LAYER * AGPMUISTATUS_MAX_QUICKBELT_COLUMN];
	ApSafeArray<ApBaseExLock, AGPMUISTATUS_MAX_QUICKBELT_LAYER * AGPMUISTATUS_MAX_QUICKBELT_COLUMN>	m_acsQBeltItem;

	CHAR			*pszQBeltEncodedString;

	INT32			m_lHPPotionTID;
	INT32			m_lMPPotionTID;

	INT8			m_cOptionViewHelmet;

	INT8			m_cAutoUseHPGage;
	INT8			m_cAutoUseMPGage;

	CHAR			*pszCooldown;
};

#endif	//__AGPDUISTATUS_H__