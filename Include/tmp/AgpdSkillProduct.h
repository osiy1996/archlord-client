#ifndef			__AGPDSKILLPRODUCT_H__
#define			__AGPDSKILLPRODUCT_H__

#include "ApBase.h"
#include "AgpdSkill.h"
#include "ApAdmin.h"

#define		AGPMSKILL_PRODUCT_MAX_LEVEL		100

typedef enum _eAgpmSkillProductConstFactor
{
	AGPMSKILL_PRODUCT_MIN_EXP = 0,
	AGPMSKILL_PRODUCT_MAX_EXP,
	AGPMSKILL_PRODUCT_MIN_PROB,
	AGPMSKILL_PRODUCT_MAX_PROB,
	AGPMSKILL_PRODUCT_LEVELUP_EXP,
	AGPMSKILL_PRODUCT_ACC_LEVEUP_EXP,
	AGPMSKILL_PRODUCT_MAX_CONST_FACTORS,
} eAgpmSkillProduct;

typedef enum _eAgpmSkillProductExcelColumn
{
	AGPMSKILL_PRODUCT_EXCEL_COLUMN_NAME = 0,
	AGPMSKILL_PRODUCT_EXCEL_COLUMN_LEVEL,
	AGPMSKILL_PRODUCT_EXCEL_COLUMN_MIN_EXP,
	AGPMSKILL_PRODUCT_EXCEL_COLUMN_MAX_EXP,
	AGPMSKILL_PRODUCT_EXCEL_COLUMN_SUCCESS_REQ,				// NOT USED IN FACTOR
	AGPMSKILL_PRODUCT_EXCEL_COLUMN_MIN_PROB,
	AGPMSKILL_PRODUCT_EXCEL_COLUMN_MAX_PROB,
	AGPMSKILL_PRODUCT_EXCEL_COLUMN_LEVELUP_EXP,
	AGPMSKILL_PRODUCT_EXCEL_COLUMN_ACC_LEVELUP_EXP
} eAgpmSkillProductExcelColumn;

typedef enum _eAgpmSkillProductType
{
	AGPMSKILL_PRODUCT_TYPE_MEAT = 0,		// ���� �����.. ũ����.
	AGPMSKILL_PRODUCT_TYPE_LEATHER,
	AGPMSKILL_PRODUCT_TYPE_MAX,
} eAgpmSkillProductType;

class AgpdSkillProductTemplate : public ApBase
{
public:
	CHAR		m_szName[AGPMSKILL_MAX_SKILL_NAME + 1];
	INT32		m_lFactors[AGPMSKILL_PRODUCT_MAX_LEVEL][AGPMSKILL_PRODUCT_MAX_CONST_FACTORS];
	INT16		m_nIndex;			// AgpdSkillProductGather�� Index
};

typedef struct _AgpdSkillProductGather
{
	INT16		nCount;				// ä�� ���� Ƚ��
	INT32		lItemTID;			// ��� ������
} AgpdSkillProductGather;

class AgpdSkillProductTemplateAttachData : public ApBase
{
public:
	//AgpdSkillProductGather m_stGather[AGPMSKILL_PRODUCT_TYPE_MAX];
	ApSafeArray<AgpdSkillProductGather, AGPMSKILL_PRODUCT_TYPE_MAX>	m_stGather;
};

#endif