#pragma once

#include "ApModule.h"
#include "AgpmFactors.h"
#include "AgpmCharacter.h"
#include "AgcmCharacter.h"
#include "AgcmEventEffect.h"

#define AGCM_CHARACTER_DLG_MAX_STR		256

typedef enum
{
	AGCMCHARACTERDLG_CB_ID_SET_HEIGHT = 0,
	AGCMCHARACTERDLG_CB_ID_SET_DEPTH,
	AGCMCHARACTERDLG_CB_ID_SET_HIT_RANGE,
	AGCMCHARACTERDLG_CB_ID_SET_BSPHERE,
	AGCMCHARACTERDLG_CB_ID_NUM
} AgcmCharacterDlgCallbackPoint;

class AFX_EXT_CLASS AgcmCharacterDlg : public ApModule
{
public:
	AgcmCharacterDlg();
	virtual ~AgcmCharacterDlg();

	static AgcmCharacterDlg *GetInstance();

	CHAR					*GetFindPathName1();
	CHAR					*GetFindPathName2();

	BOOL					EnumCallbackSetCharacterData(INT32 lCallbackPoint, PVOID pvData, PVOID pvCustData = NULL);

	AgpmFactors				*GetAgpmFactors();
	AgpmCharacter			*GetAgpmCharacter();
	AgcmCharacter			*GetAgcmCharacter();
	AgcmEventEffect			*GetAgcmEventEffect();

protected:
	BOOL	OnAddModule();

public:
	// �ʱ�ȭ�� SetClumpPathExt�� SetClumpFindPathName�� �ҷ���� �Ѵ�.
	VOID	SetClumpPathExt(CHAR *szPath, CHAR *szExt1, CHAR *szExt2);
	VOID	SetClumpFindPathName(CHAR *szFindPathName1, CHAR *szFindPathName2);

	// �ʱ�ȭ�� �ҷ��ش�.(pData�� ������ �����͸� �Ѱ��ش�.)
	BOOL	SetCallbackSetCharacterHeight(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackSetCharacterDepth(ApModuleDefaultCallBack pfCallback, PVOID pClass);			//. 2005. 10. 07. Nonstopdj
	BOOL	SetCallbackSetCharacterHitRange(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackSetCharacterBSphere(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	// �ʿ��� �� �ҷ��ش�...
	BOOL	OpenCharacterDlg(AgpdCharacterTemplate *pcsAgpdCharacterTemplate, AgcdCharacterTemplate *pcsAgcdCharacterTemplate);
	INT32	OpenCharacterTemplateList();

protected:
	AgpmFactors		*m_pcsAgpmFactors;
	AgpmCharacter	*m_pcsAgpmCharacter;
	AgcmCharacter	*m_pcsAgcmCharacter;
	AgcmEventEffect	*m_pcsAgcmEventEffect;

	CHAR			m_szFindPathName1[AGCM_CHARACTER_DLG_MAX_STR];
	CHAR			m_szFindPathName2[AGCM_CHARACTER_DLG_MAX_STR];

};
