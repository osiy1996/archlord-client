#pragma once

#include "ApModule.h"
#include "AgcmObject.h"
#include "AgcmAnimationDlg.h"

#define AGCM_OBJECT_DLG_MAX_STR		256

enum AgcmObjectDlgCallbackPoint
{
	AGCMOBJECTDLG_CB_ID_GET_ANIMATION = 0,
	AGCMOBJECTDLG_CB_ID_SET_BSPHERE,
	AGCMOBJECTDLG_CB_ID_READ_RT_ANIM,
	AGCMOBJECTDLG_CB_ID_ADD_ANIMATION,
	AGCMOBJECTDLG_CB_ID_REMOVE_ALL_ANIMATION,
	AGCMOBJECTDLG_CB_ID_NUM
};

class AFX_EXT_CLASS AgcmObjectDlg : public ApModule
{
public:
	AgcmObjectDlg();
	virtual ~AgcmObjectDlg();

	static AgcmObjectDlg *GetInstance();

	CHAR	*GetClumpPathName1();
	CHAR	*GetClumpPathName2();

	CHAR	*GetAnimationPathName1();
	CHAR	*GetAnimationPathName2();

	CHAR	*GetObjectCategoryPathName();

	AgcmObjectList	*GetObjectList()	{	return m_pcsAgcmObjectList;	}
	AgcmObject		*GetAgcmObject()	{	return m_pcsAgcmObject;	}

protected:
	BOOL	OnAddModule();

	BOOL	SetObjectCategory(PVOID pvDlg, INT16 nSetData);
	BOOL	ParseObjectCategory(CHAR *szDest, CHAR *szSrc);
	BOOL	SaveObjectCategory(PVOID pvDlg);

	static BOOL OpenObjectCategoryCB(PVOID pClass, PVOID pCustClass);
	static BOOL OpenObjectTemplateListCB(PVOID pClass, PVOID pCustClass);
	static BOOL	ObjectListDlgEndCB(PVOID pClass, PVOID pCustClass);

public:
	// �ʱ�ȭ�� SetClumpPathExt�� SetClumpFindPathName�� �ҷ���� �Ѵ�.
	VOID	SetClumpPathExt(CHAR *szPath, CHAR *szExt1, CHAR *szExt2);
	VOID	SetClumpFindPathName(CHAR *szFindPathName1, CHAR *szFindPathName2);
	VOID	SetAnimationFindPathName(CHAR *szFindPathName1, CHAR *szFindPathName2);

	// �ʱ�ȭ�� �ҷ��ش�.
	VOID	SetObjectCategoryPathName(CHAR *szPathName);
	BOOL	SetCallbackGetAnimData(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackGenerateBSphere(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	EnumCallbackSetObjectData(INT32 lCallbackPoint, PVOID pvData, PVOID pvCustData = NULL);

	BOOL	SetCallbackAddAnimation(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackRemoveAllAnimation(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackReadRtAnim(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	AddAnimation(AAD_AddAnimationParams *pcsParams);
	BOOL	RemoveAllAnimation(AAD_RemoveAllAnimationParams *pcsParams);
	BOOL	ReadRtAnim(AAD_ReadRtAnimParams *pcsParams);

	// �ʿ��� �� �ҷ��ش�...
	BOOL	OpenObjectDlg(ApdObjectTemplate *pcsApdObjectTemplate, AgcdObjectTemplate *pcsAgcdObjectTemplate);
	BOOL	OpenObjectCategory(CHAR *szDest); // ī�װ��� �����Ѵ�.
	BOOL	OpenObjectTemplateList(CHAR *szDest); // ������Ʈ ���ø��� �����Ѵ�.

protected:
	ApmObject		*m_pcsApmObject;
	AgcmObject		*m_pcsAgcmObject;
	AgcmObjectList	*m_pcsAgcmObjectList;

	CHAR			m_szFindClumpPathName1[AGCM_OBJECT_DLG_MAX_STR];
	CHAR			m_szFindClumpPathName2[AGCM_OBJECT_DLG_MAX_STR];

	CHAR			m_szFindAnimationPathName1[AGCM_OBJECT_DLG_MAX_STR];
	CHAR			m_szFindAnimationPathName2[AGCM_OBJECT_DLG_MAX_STR];

	CHAR			m_szObjectCategoryPathName[AGCM_OBJECT_DLG_MAX_STR];
};
