#include "stdafx.h"
#include "ModelTool.h"

BOOL CModelToolApp::ReleaseAllObject(BOOL bCloseEffectDlg)
{
	if(!ReleaseModalessDlg(bCloseEffectDlg))
		return FALSE;

	ReleaseClump(&m_pcsBottomClump);
	ReleaseClump(&m_pcsArmourClump1);
	ReleaseClump(&m_pcsArmourClump2);

	if(!ReleaseStaticModel())
		return FALSE;

	if(!ReleaseCharacter())
		return FALSE;

	if(!ReleaseItem())
		return FALSE;

	if(!ReleaseObject())
		return FALSE;

	return TRUE;
}

BOOL CModelToolApp::ReleaseModalessDlg(BOOL bCloseEffectDlg)
{
	if (bCloseEffectDlg)
	{
		m_csAgcEngine.GetAgcmSkillDlgModule()->CloseCharacterSkillDlg();
		m_csAgcEngine.GetAgcmEffectDlgModule()->CloseAllEffectDlg();
	}

	m_csAgcEngine.GetAgcmLODDlgModule()->CloseLODDlg();
	m_csAgcEngine.GetAgcmBlockingDlgModule()->CloseEditBlocking();
	m_csAgcEngine.GetAgcmAnimationDlgModule()->CloseCharAnim();
	//m_csAgcEngine.GetAgcmImportClientDataModule()->CloseProcessInfoDlg();
	m_csAgcEngine.GetAgcmUDADlgModule()->CloseMainUDADlg();

	if(CModelToolDlg::GetInstance())
	{
		CModelToolDlg::GetInstance()->CloseEditEquipmentsDlg();
		CModelToolDlg::GetInstance()->CloseAnimationOptionDlg();
		CModelToolDlg::GetInstance()->CloseAttachFaceDlg();
		CModelToolDlg::GetInstance()->CloseCustomizePreviewDlg();
		CModelToolDlg::GetInstance()->CloseRideDlg();
	}

	return TRUE;
}

BOOL CModelToolApp::ReleaseClump(RpClump **ppstClump)
{
	if(*ppstClump)
	{
		m_csAgcEngine.GetAgcmRenderModule()->RemoveClumpFromWorld(*ppstClump);

		RpWorld	*pstWorld = RpClumpGetWorld(*ppstClump);
		if(pstWorld)
		{
			RpWorldRemoveClump(pstWorld, *ppstClump);
		}

		RpClumpDestroy(*ppstClump);

		*(ppstClump) = NULL;
	}

	return TRUE;
}

BOOL CModelToolApp::ReleaseStaticModel()
{
	if(m_pcsStaticModel)
	{
		m_csAgcEngine.GetAgcmRenderModule()->RemoveClumpFromWorld(m_pcsStaticModel);

		RpWorld *pstWorld = RpClumpGetWorld(m_pcsStaticModel);
		if(pstWorld)
		{
			RpWorldRemoveClump(pstWorld, m_pcsStaticModel);
		}

		RpClumpDestroy(m_pcsStaticModel);
		m_pcsStaticModel = NULL;
		RwMatrixSetIdentity(&m_pcsInitTransform);
	}

	return TRUE;
}

BOOL CModelToolApp::ReleaseCharacter()
{
	INT32 lIndex = 0;

	for (	AgpdCharacter *pcsAgpdCharacter = m_csAgcEngine.GetAgpmCharacterModule()->GetCharSequence(&lIndex);
			pcsAgpdCharacter;
			pcsAgpdCharacter = m_csAgcEngine.GetAgpmCharacterModule()->GetCharSequence(&lIndex)					)
	{
		//if ((pcsAgpdCharacter->m_lID != AMT_CHARACTER_TARGET_ID1) && (pcsAgpdCharacter->m_lID != AMT_CHARACTER_TARGET_ID2))

//		m_csAgcEngine.GetAgcmTextBoardModule()->ClearHPBar( pcsAgpdCharacter );
//		m_csAgcEngine.GetAgcmTextBoardModule()->ClearAllNode( pcsAgpdCharacter );

		if (pcsAgpdCharacter->m_lID < AMT_BOUNDARY_TARGET_ID)
			m_csAgcEngine.GetAgpmCharacterModule()->RemoveCharacter(pcsAgpdCharacter->m_lID);
	}

	m_pcsPickedAtomic	= NULL;
	//m_pstStartAnimation	= NULL;

	return TRUE;
}

BOOL CModelToolApp::ReleaseItem()
{
	INT32 lIndex = 0;
	for (	AgpdItem *pcsAgpdItem = m_csAgcEngine.GetAgpmItemModule()->GetItemSequence(&lIndex);
			pcsAgpdItem;
			m_csAgcEngine.GetAgpmItemModule()->RemoveItem(pcsAgpdItem->m_lID),
			pcsAgpdItem = m_csAgcEngine.GetAgpmItemModule()->GetItemSequence(&lIndex)				);

	return TRUE;
}

BOOL CModelToolApp::ReleaseObject()
{
	INT32 lIndex = 0;
	for (	ApdObject *pcsApdObject = m_csAgcEngine.GetApmObjectModule()->GetObjectSequence(&lIndex);
			pcsApdObject;
			m_csAgcEngine.GetApmObjectModule()->DeleteObject(pcsApdObject),
			pcsApdObject = m_csAgcEngine.GetApmObjectModule()->GetObjectSequence(&lIndex)				);

//	m_pstStartAnimation	= NULL;

	return TRUE;
}

BOOL CModelToolApp::ReleaseCharacterTemplate(INT32 lTID)
{
	// 2008. 06. 10. pm 12:35 by ����
	// ���ø��� �����ϱ� ���� �ش������� �̸� �����ش�.
	// ĳ���� ���ø��� INI\\CharacterTemplate ���� ���� ���ϵ��� �ܾ �ε��ϹǷ�
	// ���ø��� ���ﶧ ���ϱ��� ������ ������ �Ǵٽ� �ε�ȴ�.
	// --> ���ǰ�� �ش� ������ ������ �����Ǿ�� �ȵȴٰ� ��..
	//     ��а� �ذ� ����� ������ ������ �� ������ ����
	//std::string szFileName = _FindTemplateFile( "INI\\CharacterTemplate", lTID );
	//if( szFileName != "" )
	//{
	//	::DeleteFile( szFileName.c_str() );
	//}

	return m_csAgcEngine.GetAgpmCharacterModule()->RemoveCharacterTemplate(lTID);
}

BOOL CModelToolApp::ReleaseItemTemplate(INT32 lTID)
{
	AgpdItemTemplate *pcsAgpdItemTemplate = m_csAgcEngine.GetAgpmItemModule()->GetItemTemplate(lTID);
	if(!pcsAgpdItemTemplate)
		return FALSE;

	return m_csAgcEngine.GetAgpmItemModule()->DestroyTemplate(pcsAgpdItemTemplate);
}

BOOL CModelToolApp::ReleaseObjectTemplate(INT32 lTID)
{
	// 2008. 06. 10. pm 12:35 by ����
	// ���ø��� �����ϱ� ���� �ش������� �̸� �����ش�.
	// ������Ʈ ���ø��� INI\\ObjectTemplate ���� ���� ���ϵ��� �ܾ �ε��ϹǷ�
	// ���ø��� ���ﶧ ���ϱ��� ������ ������ �Ǵٽ� �ε�ȴ�.
	// --> ���ǰ�� �ش� ������ ������ �����Ǿ�� �ȵȴٰ� ��..
	//     ��а� �ذ� ����� ������ ������ �� ������ ����
	//std::string szFileName = _FindTemplateFile( "INI\\ObjectTemplate", lTID );
	//if( szFileName != "" )
	//{
	//	::DeleteFile( szFileName.c_str() );
	//}

	return m_csAgcEngine.GetApmObjectModule()->DeleteObjectTemplate(lTID);
}

BOOL CModelToolApp::ReleaseSkillTemplate(INT32 lTID)
{
	return m_csAgcEngine.GetAgpmSkillModule()->RemoveSkillTemplate(lTID);
}

BOOL CModelToolApp::DeleteResourceData(eAmtObjectType eType, INT32 lTID)
{
	if ((eType == m_eObjectType) && (lTID == m_lTID))
	{
		CModelToolDlg::GetInstance()->MessageBox("�۾� ���� �����ʹ� �����Ҽ� �����ϴ�!", "ERROR!", MB_OK);
		return FALSE;
	}

	switch (eType)
	{
	case AMT_OBJECT_TYPE_CHARACTER:
		{
			if (!ReleaseCharacterTemplate(lTID))
				return FALSE;

			ResetCharacterResourceTreeDNF(GetNationalCode());
			CModelToolDlg::GetInstance()->GetResourceForm()->SaveCharacterLabel( AMT_CHARACTER_LABEL_TXT_PATH_NAME, SetStringDlgTextCB, AMT_START_UP_SAVE_CHAR_TEMPLATE_STR );
		}
		break;

	case AMT_OBJECT_TYPE_ITEM:
		{
			if (!ReleaseItemTemplate(lTID))
				return FALSE;

			ResetItemResourceTreeDNF(GetNationalCode());
		}
		break;

	case AMT_OBJECT_TYPE_OBJECT:
		{
			if (!ReleaseObjectTemplate(lTID))
				return FALSE;

			ResetObjectResourceTreeDNF(GetNationalCode());
			CModelToolDlg::GetInstance()->GetResourceForm()->SaveObjectLabel( AMT_OBJECT_CATEGORY_PATH_NAME, SetStringDlgTextCB, AMT_START_UP_SAVE_OBJT_TEMPLATE_STR );
		}
		break;

	case AMT_OBJECT_TYPE_SKILL:
		{
			if (!ReleaseSkillTemplate(lTID))
				return FALSE;

			ResetSkillResourceTreeDNF(GetNationalCode());
		}
		break;

	default:
		return FALSE;
	}
					
	return TRUE;
}