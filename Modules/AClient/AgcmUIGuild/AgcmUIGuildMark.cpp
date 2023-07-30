#include <AgcmUIGuild/AgcmUIGuild.h>
#include <AgpmCharacter/ApAutoLockCharacter.h>
#include <AgcmUIManager2/AgcmUIManager2.h>
#include <AuStrTable/AuStrTable.h>
#include <AgpmGuild/AgpmGuild.h>
#include <AgpmCharacter/AgpmCharacter.h>
#include <AgcmCharacter/AgcmCharacter.h>
#include <AgpmGrid/AgpmGrid.h>

/*
	2005.10.10. By SungHoon
	��� ��ũ ����â�� ����.
*/
BOOL AgcmUIGuild::OpenGuildMarkUI()
{
//	�渶���� Ȯ�� �ϱ�
	if(!m_pcsAgcmGuild->IsSelfGuildMaster()) return FALSE;

	m_pcsAgcmUIManager2->ThrowEvent(m_lEventGuildMarkUIOpen);

	return TRUE;
}

/*
	2005.10.10. By SungHoon
	��� ��ũ ����â�� �ݴ´�.
*/
BOOL AgcmUIGuild::CloseGuildMarkUI()
{
	m_pcsAgcmUIManager2->ThrowEvent(m_lEventGuildMarkUIClose);

	return TRUE;
}
/*
	2005.10.10. By SungHoon
	�����θ޴�(NPC)���� ��� ��ũ ������ �����ߴ�.
*/
BOOL AgcmUIGuild::CBGuildCreateGuildMark(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIGuild* pThis = ( AgcmUIGuild *)pClass;
	if( !pThis ) return FALSE;

	AgpdGuild* pcsGuild = pThis->m_pcsAgcmGuild->GetSelfGuildLock();
	if( !pcsGuild )		return FALSE;

	INT32 lMemberCount = pThis->m_pcsAgpmGuild->GetMemberCount( pcsGuild );
	pcsGuild->m_Mutex.Release();

	if (lMemberCount < AGPMGUILD_GUILDMARK_ENABLE_MEMBER_COUNT)		//	��� �ο��� �۴�.
	{
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventGuildMarkNotEnoughtGuildMemberCount);
		return FALSE;
	}
	pThis->OpenGuildMarkUI();

	return TRUE;
}

/*
	2005.10.10. By SungHoon
	��帶ũUI�� ���� �ʱ�ȭ ���� �۾� 
*/
BOOL AgcmUIGuild::CBGuildGuildMarkUIInit(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1) return FALSE;
	AgcmUIGuild *pThis = ( AgcmUIGuild *)pClass;

	AgcdUIControl	*pcsUIGuildMarkDetailInfo = (AgcdUIControl *)	pData1;

	if (pcsUIGuildMarkDetailInfo->m_lType == AcUIBase::TYPE_EDIT)
	{
		AcUIEdit *	pcsUIGuildMarkDetailInfotEdit	= (AcUIEdit *) pcsUIGuildMarkDetailInfo->m_pcsBase;
		pcsUIGuildMarkDetailInfotEdit->SetLineDelimiter(pThis->m_pcsAgcmUIManager2->GetLineDelimiter());
	}


	pThis->m_stGuildMarkGrid.m_pcsSelectedGridItem = NULL;
	pThis->m_lGuildMarkSelectedTID = pThis->m_pcsAgpmGuild->GetGuildMarkID(pThis->m_pcsAgcmCharacter->GetSelfCharacter());
	pThis->m_lGuildMarkSelectedColor = pThis->m_pcsAgpmGuild->GetGuildMarkColor(pThis->m_pcsAgcmCharacter->GetSelfCharacter());

	pThis->InitSeletedGridItem();

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pcsUDGuildMarkGrid);
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pcsUDGuildMarkLayerIndex );
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pcsUDGuildMarkDetailInfo );

	pThis->SetPreviewGuildMark();
	return TRUE;
}
/*
	2005.10.07. By SungHoon
	�׸��� �������� Grid�� Add�Ѵ�.
*/
VOID AgcmUIGuild::SetGridItem()
{
	AgpdGuildMarkTemplate **ppTemplate = NULL;
	INT32 lIndex = 0;

	INT32	lAddedItemBottom = 0;
	INT32	lAddedItemPattern = 0;
	INT32	lAddedItemSymbol = 0;
	INT32	lAddedItemColor = 0;

	for(ppTemplate = (AgpdGuildMarkTemplate**)m_pcsAgpmGuild->m_csGuildMarkTemplateList.GetObjectSequence(&lIndex); ppTemplate;
		ppTemplate = (AgpdGuildMarkTemplate**)m_pcsAgpmGuild->m_csGuildMarkTemplateList.GetObjectSequence(&lIndex))
	{
		if(!*ppTemplate)
			continue;

		AgpdGuildMarkTemplate *pTemplate = *(ppTemplate);
		INT32 lRow = 0;
		INT32 lCol = 0;
		INT32 lLayer = -1;
		if (pTemplate->m_lTID & 0x000000FF)
		{
			lLayer = 0;
			if (lAddedItemBottom > 0)
			{
				lRow = lAddedItemBottom / AGCMUIGUILD_GUILDMARK_GRID_COLUMN;
				lCol = lAddedItemBottom % AGCMUIGUILD_GUILDMARK_GRID_COLUMN;
			}
			lAddedItemBottom++;
		}
		if (pTemplate->m_lTID & 0x0000FF00)
		{
			lLayer = 1;
			if (lAddedItemPattern > 0)
			{
				lRow = lAddedItemPattern / AGCMUIGUILD_GUILDMARK_GRID_COLUMN;
				lCol = lAddedItemPattern % AGCMUIGUILD_GUILDMARK_GRID_COLUMN;
			}
			lAddedItemPattern++;
		}
		if (pTemplate->m_lTID & 0x00FF0000)
		{
			lLayer = 2;
			if (lAddedItemSymbol > 0)
			{
				lRow = lAddedItemSymbol / AGCMUIGUILD_GUILDMARK_GRID_COLUMN;
				lCol = lAddedItemSymbol % AGCMUIGUILD_GUILDMARK_GRID_COLUMN;
			}
			lAddedItemSymbol++;
		}
		if (pTemplate->m_lTID & 0xFF000000 && lAddedItemColor < AGCMUIGUILD_GUILDMARK_MAX_COLOR)
		{
			lLayer = 3;
			if (lAddedItemColor > 0)
			{
				lRow = lAddedItemColor / AGCMUIGUILD_GUILDMARK_GRID_COLOR_COLUMN;
				lCol = lAddedItemColor % AGCMUIGUILD_GUILDMARK_GRID_COLOR_COLUMN;
			}
			lAddedItemColor++;
		}
		if (lLayer != -1)
		{
			m_pcsAgcmGuild->SetGridGuildAttachedTexture(pTemplate);
			m_pcsAgcmGuild->SetGridGuildAttachedSmallTexture(pTemplate);

			if (lLayer == 3) m_pcsAgpmGrid->Add(&m_stGuildMarkColorGrid, 0, lRow, lCol, pTemplate->m_pcsGridItem, 1, 1);
			else m_pcsAgpmGrid->Add(&m_stGuildMarkGrid, lLayer, lRow, lCol, pTemplate->m_pcsGridItem, 1, 1);

		}
	}
}

/*
	2005.10.12. By SungHoon
	��� ��ũ �׸��� �������� �ʱ�ȭ �Ѵ�.
*/
VOID AgcmUIGuild::InitGuildMarkGridItem()
{
	INT32 lGuildMarkGridRow = AGCMUIGUILD_GUILDMARK_GRID_ROW;
	INT32 lMaxItem = m_pcsAgpmGuild->GetMaxGuildMarkGridItem();
	if (lMaxItem > 1)
		lGuildMarkGridRow = (lMaxItem / AGCMUIGUILD_GUILDMARK_GRID_COLUMN) + 1;
	m_pcsAgpmGrid->Init(&m_stGuildMarkGrid, AGCMUIGUILD_GUILDMARK_GRID_LAYER, lGuildMarkGridRow, AGCMUIGUILD_GUILDMARK_GRID_COLUMN );
	m_stGuildMarkGrid.m_lGridType	= AGPDGRID_ITEM_TYPE_GUILDMARK;

	m_pcsAgpmGrid->Init(&m_stGuildMarkColorGrid, 1, AGCMUIGUILD_GUILDMARK_GRID_COLOR_ROW, AGCMUIGUILD_GUILDMARK_GRID_COLOR_COLUMN );
	m_stGuildMarkColorGrid.m_lGridType	= AGPDGRID_ITEM_TYPE_GUILDMARK;

	AgpdGridItem *ppGridItem[6][3];
	ZeroMemory(&ppGridItem, sizeof(ppGridItem));

	AgpdGrid *ppGrid[6][3];
	ppGrid[0][0] = &m_stGuildMarkPreviewLargeGrid;
	ppGrid[0][1] = &m_stGuildMarkPreviewLargePatternGrid;
	ppGrid[0][2] = &m_stGuildMarkPreviewLargeSymbolGrid;

	ppGrid[1][0] = &m_stGuildMarkPreviewSmallGrid;
	ppGrid[1][1] = &m_stGuildMarkPreviewSmallPatternGrid;
	ppGrid[1][2] = &m_stGuildMarkPreviewSmallSymbolGrid;

	ppGrid[2][0] = &m_stGuildMarkPreviewBuyGrid;
	ppGrid[2][1] = &m_stGuildMarkPreviewBuyPatternGrid;
	ppGrid[2][2] = &m_stGuildMarkPreviewBuySymbolGrid;

	ppGrid[3][0] = &m_stGuildMarkMainGrid;
	ppGrid[3][1] = &m_stGuildMarkMainPatternGrid;
	ppGrid[3][2] = &m_stGuildMarkMainSymbolGrid;

	ppGrid[4][0] = &m_stGuildMarkGuildListGrid;
	ppGrid[4][1] = &m_stGuildMarkGuildListPatternGrid;
	ppGrid[4][2] = &m_stGuildMarkGuildListSymbolGrid;

	ppGrid[5][0] = &m_stGuildMarkGuildDestroyGrid;
	ppGrid[5][1] = &m_stGuildMarkGuildDestroyPatternGrid;
	ppGrid[5][2] = &m_stGuildMarkGuildDestroySymbolGrid;

	for ( int i = 0 ; i < 6; i++)
	{
		m_pcsAgpmGrid->Init(ppGrid[i][0], 1, 1, 1);
		m_pcsAgpmGrid->Init(ppGrid[i][1], 1, 1, 1);
		m_pcsAgpmGrid->Init(ppGrid[i][2], 1, 1, 1);
	}

	DestroyPreviewGuildMark();

	for ( int i = 0 ; i < 6; i++)
	{
		for ( int j = 0 ; j < 3; j++)
		{
			ppGridItem[i][j] = m_pcsAgpmGrid->CreateGridItem();
			if (ppGridItem[i][j])
			{
				ppGridItem[i][j]->m_eType = AGPDGRID_ITEM_TYPE_GUILDMARK;
				ppGridItem[i][j]->SetParentBase(NULL);
				m_pcsAgpmGrid->Add(ppGrid[i][j], 0, 0, 0, ppGridItem[i][j], 1, 1);
			}
		}
	}
	m_pcsGuildMarkPreviewLargeGridItem =ppGridItem[0][0];
	m_pcsGuildMarkPreviewLargePatternGridItem = ppGridItem[0][1];
	m_pcsGuildMarkPreviewLargeSymbolGridItem = ppGridItem[0][2];


	m_pcsGuildMarkPreviewSmallGridItem = ppGridItem[1][0];
	m_pcsGuildMarkPreviewSmallPatternGridItem = ppGridItem[1][1];
	m_pcsGuildMarkPreviewSmallSymbolGridItem = ppGridItem[1][2];

	m_pcsGuildMarkPreviewBuyGridItem = ppGridItem[2][0];
	m_pcsGuildMarkPreviewBuyPatternGridItem = ppGridItem[2][1];
	m_pcsGuildMarkPreviewBuySymbolGridItem = ppGridItem[2][2];

	m_pcsGuildMarkMainGridItem = ppGridItem[3][0];
	m_pcsGuildMarkMainPatternGridItem = ppGridItem[3][1];
	m_pcsGuildMarkMainSymbolGridItem = ppGridItem[3][2];

	m_pcsGuildMarkGuildListGridItem = ppGridItem[4][0];
	m_pcsGuildMarkGuildListPatternGridItem = ppGridItem[4][1];
	m_pcsGuildMarkGuildListSymbolGridItem = ppGridItem[4][2];

	m_pcsGuildMarkGuildDestroyGridItem = ppGridItem[5][0];
	m_pcsGuildMarkGuildDestroyPatternGridItem = ppGridItem[5][1];
	m_pcsGuildMarkGuildDestroySymbolGridItem = ppGridItem[5][2];

	SetGridItem();
}

/*
	2005.10.12. By SungHoon
	������ �׸��� �������� �����Ѵ�.
*/
VOID AgcmUIGuild::DestroyPreviewGuildMark()
{
	AgpdGridItem *ppGridItem[6][3];

	ppGridItem[0][0] = m_pcsGuildMarkPreviewLargeGridItem;
	ppGridItem[0][1] = m_pcsGuildMarkPreviewLargePatternGridItem;
	ppGridItem[0][2] = m_pcsGuildMarkPreviewLargeSymbolGridItem;

	ppGridItem[1][0] = m_pcsGuildMarkPreviewSmallGridItem;
	ppGridItem[1][1] = m_pcsGuildMarkPreviewSmallPatternGridItem;
	ppGridItem[1][2] = m_pcsGuildMarkPreviewSmallSymbolGridItem;

	ppGridItem[2][0] = m_pcsGuildMarkPreviewBuyGridItem;
	ppGridItem[2][1] = m_pcsGuildMarkPreviewBuyPatternGridItem;
	ppGridItem[2][2] = m_pcsGuildMarkPreviewBuySymbolGridItem;

	ppGridItem[3][0] = m_pcsGuildMarkMainGridItem;
	ppGridItem[3][1] = m_pcsGuildMarkMainPatternGridItem;
	ppGridItem[3][2] = m_pcsGuildMarkMainSymbolGridItem;

	ppGridItem[4][0] = m_pcsGuildMarkGuildListGridItem;
	ppGridItem[4][1] = m_pcsGuildMarkGuildListPatternGridItem;
	ppGridItem[4][2] = m_pcsGuildMarkGuildListSymbolGridItem;

	ppGridItem[5][0] = m_pcsGuildMarkGuildDestroyGridItem;
	ppGridItem[5][1] = m_pcsGuildMarkGuildDestroyPatternGridItem;
	ppGridItem[5][2] = m_pcsGuildMarkGuildDestroySymbolGridItem;

	for (int i = 0; i < 6; i++ )
	{
		for (int j = 0 ; j < 3; j++)
		{
			if (ppGridItem[i][j])
			{
				m_pcsAgpmGrid->DeleteGridItem(ppGridItem[i][j]);
			}
		}
	}
	m_pcsGuildMarkPreviewLargeGridItem = NULL;
	m_pcsGuildMarkPreviewSmallGridItem = NULL;
	m_pcsGuildMarkPreviewLargePatternGridItem = NULL;

	m_pcsGuildMarkPreviewSmallPatternGridItem = NULL;
	m_pcsGuildMarkPreviewLargeSymbolGridItem = NULL;
	m_pcsGuildMarkPreviewSmallSymbolGridItem = NULL;

	m_pcsGuildMarkPreviewBuyGridItem = NULL;		//	2005.10.20. By SungHoon
	m_pcsGuildMarkPreviewBuyPatternGridItem = NULL;	//	2005.10.20. By SungHoon
	m_pcsGuildMarkPreviewBuySymbolGridItem = NULL;	//	2005.10.20. By SungHoon

	m_pcsGuildMarkMainGridItem = NULL;				//	2005.10.24. By SungHoon
	m_pcsGuildMarkMainPatternGridItem = NULL;		//	2005.10.24. By SungHoon
	m_pcsGuildMarkMainSymbolGridItem = NULL;		//	2005.10.24. By SungHoon

	m_pcsGuildMarkGuildListGridItem = NULL;			//	2005.10.24. By SungHoon
	m_pcsGuildMarkGuildListPatternGridItem = NULL;	//	2005.10.24. By SungHoon
	m_pcsGuildMarkGuildListSymbolGridItem = NULL;	//	2005.10.24. By SungHoon

	m_pcsGuildMarkGuildDestroyGridItem = NULL;			//	2005.10.24. By SungHoon
	m_pcsGuildMarkGuildDestroyPatternGridItem = NULL;	//	2005.10.24. By SungHoon
	m_pcsGuildMarkGuildDestroySymbolGridItem = NULL;	//	2005.10.24. By SungHoon

}

/*
	2005.10.10. By SungHoon
	��帶ũ ����� â�� �ݴ´�.
*/
BOOL AgcmUIGuild::CBGuildGuildMarkUIClose(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass) return FALSE;
	AgcmUIGuild *pThis = ( AgcmUIGuild *)pClass;

	pThis->CloseGuildMarkUI();

	return TRUE;
}

/*
	2005.10.10. By SungHoon
	��� ��ũ UI �� ���� Ȱ��ȭ ���θ� üũ�Ѵ�.
*/
BOOL AgcmUIGuild::CBIsGuildMarkSelected(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
{
	AgcmUIGuild *		pThis = (AgcmUIGuild *) pClass;
	AgcdUIUserData *	pstUserData = pThis->m_pcsAgcmUIManager2->GetControlUserData(pcsSourceControl);

	if (!pstUserData)
		return FALSE;

	if (pstUserData->m_lSelectedIndex == pcsSourceControl->m_lUserDataIndex)
		return TRUE;

	return FALSE;
}

/*
	2005.10.10. By SungHoon
	��� ��ũ Grid ���̾ Update�Ѵ�.
*/
BOOL AgcmUIGuild::CBGuildupdateGuildMarkLayer(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIGuild *		pThis = (AgcmUIGuild *) pClass;
	AgcdUIControl *		pcsGridControl = (AgcdUIControl *) pData1;
	AgcdUIUserData *	pstUserData = NULL;

	if (pcsGridControl)
	{
		pstUserData = pThis->m_pcsAgcmUIManager2->GetControlUserData(pcsSourceControl);
		if (pstUserData && pstUserData->m_lSelectedIndex != -1)
		{
			((AcUIGrid *) pcsGridControl->m_pcsBase)->SetNowLayer(pstUserData->m_lSelectedIndex);
			pThis->InitSeletedGridItem();
		}
	}

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pcsUDGuildMarkGrid);
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pcsUDGuildMarkLayerIndex );

	return TRUE;
}

/*
	2005.10.11. By SungHoon
	��� ��ũ Grid�� Ŭ������ ���
*/
BOOL AgcmUIGuild::CBGuildMarkGridLClickDown(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIGuild			*pThis			= (AgcmUIGuild *) pClass;

	AcUIGrid			*pcsUIGrid		= (AcUIGrid *) pcsSourceControl->m_pcsBase;

	AgpdGridSelectInfo	*pcsClickInfo	= pcsUIGrid->GetGridItemClickInfo();
	if (!pcsClickInfo || !pcsClickInfo->pGridItem)
		return FALSE;

	pThis->m_stGuildMarkGrid.m_pcsSelectedGridItem	= pcsClickInfo->pGridItem;

	if (pcsClickInfo->pGridItem->m_eType == AGPDGRID_ITEM_TYPE_GUILDMARK)
	{
//	���⼭ �̸� ���⸦ �����Ѵ�
		AgpdGuildMarkTemplate *pGuildMarkTemplate = (AgpdGuildMarkTemplate *)pcsClickInfo->pGridItem->GetParentBase();
		if (pGuildMarkTemplate)
		{
			INT32 lSeleteMarkTID =  pGuildMarkTemplate->m_lTID;
			INT32 lIndex = pThis->GetCurrentTabIndex();
			if (pThis->m_pcsAgpmGuild->GetGuildMarkID( lIndex, pThis->m_lGuildMarkSelectedTID) == pGuildMarkTemplate->m_lTID)
				lSeleteMarkTID = 0;
			pThis->m_lGuildMarkSelectedTID = pThis->m_pcsAgpmGuild->SetGuildMarkID(
					lIndex, pThis->m_lGuildMarkSelectedTID, lSeleteMarkTID );
		}
		pThis->SetPreviewGuildMark();
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pcsUDGuildMarkDetailInfo );
	}

	return TRUE;
}

/*
	2005.10.12. By SungHoon
	��� ��ũ ������ Grid�� Ŭ������ ���
*/
BOOL AgcmUIGuild::CBGuildMarkColorGridLClickDown(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIGuild			*pThis			= (AgcmUIGuild *) pClass;

	AcUIGrid			*pcsUIGrid		= (AcUIGrid *) pcsSourceControl->m_pcsBase; 

	AgpdGridSelectInfo	*pcsClickInfo	= pcsUIGrid->GetGridItemClickInfo();
	if (!pcsClickInfo || !pcsClickInfo->pGridItem)
		return FALSE;

	pThis->m_stGuildMarkColorGrid.m_pcsSelectedGridItem	= pcsClickInfo->pGridItem;

	if (pcsClickInfo->pGridItem->m_eType == AGPDGRID_ITEM_TYPE_GUILDMARK)
	{
//	���⼭ �̸����⿡ ������ �����Ѵ�
//	���⼭ �̸� ���⸦ �����Ѵ�
		INT32 lIndex = pThis->GetCurrentTabIndex();
		AgpdGuildMarkTemplate *pGuildMarkColorTemplate = (AgpdGuildMarkTemplate *)pcsClickInfo->pGridItem->GetParentBase();
		if (pGuildMarkColorTemplate)
		{
			pThis->m_lGuildMarkSelectedColor = pThis->m_pcsAgpmGuild->SetGuildMarkColor(
					lIndex, pThis->m_lGuildMarkSelectedColor, pGuildMarkColorTemplate->m_lIndex);
		}
		pThis->SetPreviewGuildMark();
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pcsUDGuildMarkDetailInfo );
	}

	return TRUE;
}

/*
	2005.10.12. By SungHoon
	��帶ũ �̸����� â�� �ؽ��ĸ� ������.
*/
VOID AgcmUIGuild::SetPreviewGuildMark()
{
	AgpdGridItem *ppGridItem[3][2];
	ppGridItem[0][0] = m_pcsGuildMarkPreviewLargeGridItem;
	ppGridItem[0][1] = m_pcsGuildMarkPreviewSmallGridItem;
	ppGridItem[1][0] = m_pcsGuildMarkPreviewLargePatternGridItem;
	ppGridItem[1][1] = m_pcsGuildMarkPreviewSmallPatternGridItem;
	ppGridItem[2][0] = m_pcsGuildMarkPreviewLargeSymbolGridItem;
	ppGridItem[2][1] = m_pcsGuildMarkPreviewSmallSymbolGridItem;

	AgcdUIUserData *ppGridControl[3][2];
	ppGridControl[0][0] = m_pcsUDGuildMarkPreviewLargeGrid;
	ppGridControl[0][1] = m_pcsUDGuildMarkPreviewSmallGrid;
	ppGridControl[1][0] = m_pcsUDGuildMarkPreviewLargePatternGrid;
	ppGridControl[1][1] = m_pcsUDGuildMarkPreviewSmallPatternGrid;
	ppGridControl[2][0] = m_pcsUDGuildMarkPreviewLargeSymbolGrid;
	ppGridControl[2][1] = m_pcsUDGuildMarkPreviewSmallSymbolGrid;

	INT32 lGuildMarkTypeCode[3] = { AGPMGUILD_ID_MARK_BOTTOM, AGPMGUILD_ID_MARK_PATTERN, AGPMGUILD_ID_MARK_SYMBOL };
	for (int i = 0 ; i < 3; i++)
	{
		RwTexture**	ppRwLargeTexture = m_pcsAgcmUIControl->GetAttachGridItemTextureData(ppGridItem[i][0]);
		RwTexture**	ppRwSmallTexture	= m_pcsAgcmUIControl->GetAttachGridItemTextureData(ppGridItem[i][1]);
		if (!ppRwSmallTexture || !ppRwLargeTexture) return;
		*ppRwLargeTexture = NULL;
		*ppRwSmallTexture = NULL;

		INT32 lColor = m_pcsAgpmGuild->GetGuildMarkColor(lGuildMarkTypeCode[i], m_lGuildMarkSelectedColor);
		AgpdGuildMarkTemplate *pTemplate = m_pcsAgpmGuild->GetGuildMarkTemplate(lGuildMarkTypeCode[i], m_lGuildMarkSelectedTID);
		if (pTemplate)
		{
			for (int j = 0; j < 2; j++)
			{
				AcUIGrid *pcsUIGrid = (AcUIGrid *)(*ppGridControl[i][j]->m_ppControls)->m_pcsBase;
				if (pcsUIGrid)
				{
					if (pcsUIGrid->m_apGridItemWindow)
					{
						AcUIGridItem *pGridItem = pcsUIGrid->m_apGridItemWindow[0];
						if (pGridItem) pGridItem->SetColorValue(lColor);
					}
				}
			}
			AgcdGuildMarkTemplate	*pAgcdGuildMarkTemplate	= m_pcsAgcmGuild->GetAttachDataGuildMarkTemplate(pTemplate);
			if (pAgcdGuildMarkTemplate)
			{
				*ppRwLargeTexture = pAgcdGuildMarkTemplate->m_pGuildMarkTexture;
				*ppRwSmallTexture = pAgcdGuildMarkTemplate->m_pSmallGuildMarkTexture;
			}
		}
	}

	for (int i=0;i<3;i++)
	{
		m_pcsAgcmUIManager2->SetUserDataRefresh( ppGridControl[i][0]);
		m_pcsAgcmUIManager2->SetUserDataRefresh( ppGridControl[i][1]);
	}

	AgpdCharacter *pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	AgpdGuild *pcsGuild = m_pcsAgcmGuild->GetSelfGuildLock();
	if( !pcsGuild )		return;

	INT32 lGuildMarkTID = pcsGuild->m_lGuildMarkTID;
	pcsGuild->m_Mutex.Release();

	m_bEnableBuyGuildMark = FALSE;
	if( m_pcsAgpmGuild->IsMaster( pcsGuild, pcsCharacter->m_szID ) )
	{
		if (lGuildMarkTID != m_lGuildMarkSelectedTID && m_pcsAgpmGuild->IsEnableBuyGuildMark(m_lGuildMarkSelectedTID, pcsCharacter) == -1)
			m_bEnableBuyGuildMark = TRUE;
	}

	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstEnableBuyGuildMark);
}

/*
	2005.10.13. By SungHoon
	������ ��帶ũ�� ���������� Editâ�� ����Ѵ�.
*/
BOOL AgcmUIGuild::CBDisplayGuildMarkInfo(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl* pcsSourceControl)
{
	if(!pData || !pClass || eType != AGCDUI_USERDATA_TYPE_INT32 || !pcsSourceControl)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	AgpdCharacter *pcsCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsCharacter) 
	{
		((AcUIEdit*)pcsSourceControl->m_pcsBase)->SetText("");
		return FALSE;
	}

	CHAR szGuildMarkName[3][512] ={0};
	CHAR szSkullName[3][512] ={0};
	INT32 lSkullCount[3];

	ZeroMemory( &szGuildMarkName, sizeof(szGuildMarkName));
	ZeroMemory( &szSkullName, sizeof(szSkullName));
	ZeroMemory( &lSkullCount, sizeof(lSkullCount));

	INT32 lGheld = 0;

	AgpdGuildMarkTemplate *pGuildMarkTemplate[3];
	pGuildMarkTemplate[0] = pThis->m_pcsAgpmGuild->GetGuildMarkTemplate(AGPMGUILD_ID_MARK_BOTTOM, pThis->m_lGuildMarkSelectedTID);
	pGuildMarkTemplate[1] = pThis->m_pcsAgpmGuild->GetGuildMarkTemplate(AGPMGUILD_ID_MARK_PATTERN, pThis->m_lGuildMarkSelectedTID);
	pGuildMarkTemplate[2] = pThis->m_pcsAgpmGuild->GetGuildMarkTemplate(AGPMGUILD_ID_MARK_SYMBOL, pThis->m_lGuildMarkSelectedTID);

	CHAR *szLineDelimiter = pThis->m_pcsAgcmUIManager2->GetLineDelimiter();

	CHAR* szGuildMarkNameTitle[3] =
	{
		ClientStr().GetStr(STI_BACKGROUND),
		ClientStr().GetStr(STI_DECO),
		ClientStr().GetStr(STI_SYMBOL)
	};

	CHAR szBuffer[512] = {0};
	CHAR szTempBuffer[512] = {0};

	for (int i = 0 ; i < 3; i++ )
	{
		if (pGuildMarkTemplate[i]) 
		{
			wsprintf(szTempBuffer, " %s : %s%s",szGuildMarkNameTitle[i], pGuildMarkTemplate[i]->m_szName, szLineDelimiter);
			strcat(szBuffer, szTempBuffer);
			lGheld += pGuildMarkTemplate[i]->m_lGheld;;
		}
	}

	if (lGheld > 0)
	{
		// apply tax
		INT32 lTaxRatio = pThis->m_pcsAgpmCharacter->GetTaxRatio(pcsCharacter);
		INT32 lTax = 0;
		if (lTaxRatio > 0)
		{
			lTax = (lGheld * lTaxRatio) / 100;
		}
		lGheld = lGheld + lTax;		
	
		wsprintf(szTempBuffer,"%s[%s]%s", szLineDelimiter, ClientStr().GetStr(STI_TOTALCOST), szLineDelimiter);
		strcat(szBuffer, szTempBuffer);
		if(pcsCharacter->m_llMoney < lGheld)
			strcat(szBuffer, "<HFFFF0000>");
		CHAR szGheld[20];
		pThis->MakeCommaNumber(lGheld, szGheld, 20 );
		wsprintf(szTempBuffer," %s%s%s", szGheld, ClientStr().GetStr(STI_GELD), szLineDelimiter);
		strcat(szBuffer, szTempBuffer);
		strcat(szBuffer, "<HFFFFFFFF>");
	}

	//�ذ񳻿� ����..
	//for (int i = 0; i < 3; i++ )
	//{
	//	if( !pGuildMarkTemplate[i] )	continue;

	//	AgpdItemTemplate* pSkullItemTemplate = pThis->m_pcsAgpmItem->GetItemTemplate(pGuildMarkTemplate[i]->m_lSkullTID);
	//	if( !pSkullItemTemplate )		continue;

	//	AgpdItem* pcsItem = pThis->m_pcsAgpmItem->GetInventoryItemByTID(pcsCharacter,pGuildMarkTemplate[i]->m_lSkullTID);
	//	if( pcsItem && pcsItem->m_nCount >= pGuildMarkTemplate[i]->m_lSkullCount )
	//		strcat(szBuffer, "<HFFFF0000>");

	//	wsprintf( szTempBuffer, " %s %d%s%s", pSkullItemTemplate->m_szName, pGuildMarkTemplate[i]->m_lSkullCount, ClientStr().GetStr(STI_NUMBER_UNIT), szLineDelimiter );
	//	strcat(szBuffer, szTempBuffer);
	//	strcat(szBuffer, "<HFFFFFFFF>");
	//}		

	((AcUIEdit*)pcsSourceControl->m_pcsBase)->SetText(szBuffer);


	return TRUE;
}

/*
	2005.10.14. By SungHoon
	��� ��ũ �ʱ�ȭ ��ư�� ������ ���
*/
BOOL AgcmUIGuild::CBGuildGuildMarkClickInitialize(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass) return FALSE;
	AgcmUIGuild *pThis = ( AgcmUIGuild *)pClass;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventGuildMarkInitialize);

	return TRUE;
}

/*
	2005.10.20. By SungHoon
	��� ��ũ�� �ʱ�ȭ�Ѵ�.
*/
BOOL AgcmUIGuild::GuildMarkInitialize()
{
	m_stGuildMarkGrid.m_pcsSelectedGridItem = NULL;
	m_stGuildMarkColorGrid.m_pcsSelectedGridItem= NULL;
	m_lGuildMarkSelectedTID = 0;
	m_lGuildMarkSelectedColor = 0xFFFFFFFF;

	m_pcsAgcmUIManager2->SetUserDataRefresh( m_pcsUDGuildMarkGrid);
	m_pcsAgcmUIManager2->SetUserDataRefresh( m_pcsUDGuildMarkLayerIndex );
	m_pcsAgcmUIManager2->SetUserDataRefresh( m_pcsUDGuildMarkDetailInfo );
	m_pcsAgcmUIManager2->SetUserDataRefresh( m_pcsUDGuildMarkColorGrid );

	SetPreviewGuildMark();

	return TRUE;
}

/*
	2005.10.14. By SungHoon
	��� ��ũ ���� ��ư�� ������ ���
*/
BOOL AgcmUIGuild::CBGuildGuildMarkClickBuy(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass) return FALSE;
	AgcmUIGuild *pThis = ( AgcmUIGuild *)pClass;

	if (pThis->m_pcsAgpmGuild->GetGuildMarkID(AGPMGUILD_ID_MARK_BOTTOM, pThis->m_lGuildMarkSelectedTID) == 0)
	{
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventGuildMarkNoExistBottom);
		return FALSE;
	}
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventGuildMarkBuy);
	return TRUE;
}

/*
	2005.10.14. By SungHoon
	��� ��ũ ���� Ȯ�� ��ư�� ������ ���
*/

BOOL AgcmUIGuild::CBGuildGuildMarkClickBuyOK(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass) return FALSE;
	AgcmUIGuild *pThis = ( AgcmUIGuild *)pClass;

	pThis->m_pcsAgcmGuild->SendGuildBuyGuildMark(pThis->m_lGuildMarkSelectedTID, pThis->m_lGuildMarkSelectedColor, FALSE);
	return TRUE;
}


/*
	2005.10.18. By SungHoon
	���� Tab�� �ε����� �����´�.
*/
INT32 AgcmUIGuild::GetCurrentTabIndex()
{
	INT32 lIndex = AGPMGUILD_ID_MARK_BOTTOM;
	if (m_pcsUDGuildMarkLayerIndex->m_lSelectedIndex >= 0 && m_pcsUDGuildMarkLayerIndex->m_lSelectedIndex < AGPMGUILD_ID_MARK_SYMBOL)
		return (m_pcsUDGuildMarkLayerIndex->m_lSelectedIndex + 1);

	return AGPMGUILD_ID_MARK_BOTTOM;
}

/*
	2005.10.18. By SungHoon
	��� ��ũUI�� ���۵ǰų� Tab�� �ٲ��� ���õ� �������� Ȱ��ȭ ��Ų��.
*/
VOID AgcmUIGuild::InitSeletedGridItem()
{
	m_stGuildMarkGrid.m_pcsSelectedGridItem = NULL;
	m_stGuildMarkColorGrid.m_pcsSelectedGridItem= NULL;

	INT32 lIndex = GetCurrentTabIndex();

	AgpdGuildMarkTemplate *pGuildMarkTemplate = m_pcsAgpmGuild->GetGuildMarkTemplate(lIndex, m_lGuildMarkSelectedTID);
	if (pGuildMarkTemplate) m_stGuildMarkGrid.m_pcsSelectedGridItem = pGuildMarkTemplate->m_pcsGridItem;

	AgpdGuildMarkTemplate *pGuildMarkColorTemplate = m_pcsAgpmGuild->GetGuildMarkTemplate(
		m_pcsAgpmGuild->GetGuildMarkColorID(lIndex, m_lGuildMarkSelectedColor) );
	if (pGuildMarkColorTemplate) m_stGuildMarkColorGrid.m_pcsSelectedGridItem = pGuildMarkColorTemplate->m_pcsGridItem;

}

BOOL AgcmUIGuild::CBGuildGuildMarkInitialize(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage)
{
	if (!pClass) return FALSE;
	AgcmUIGuild	*pThis	= (AgcmUIGuild *)	pClass;

	if (lTrueCancel == (INT32) TRUE)
	{
		pThis->GuildMarkInitialize();
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventGuildMarkInitializeComplete);
	}

	return TRUE;
}

BOOL AgcmUIGuild::CBGuildGuildMarkDuplicate(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage)
{
	if (!pClass) return FALSE;
	AgcmUIGuild	*pThis	= (AgcmUIGuild *)	pClass;

	if (lTrueCancel == (INT32) TRUE)
	{
		pThis->m_pcsAgcmGuild->SendGuildBuyGuildMark(pThis->m_lGuildMarkSelectedTID, pThis->m_lGuildMarkSelectedColor, TRUE);
	}

	return TRUE;
}

/*
	2005.10.19. By SungHoon
	��� ��ũ������ �����ߴ�.
*/
BOOL AgcmUIGuild::CBGuildBuyGuildMark(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData) return FALSE;

	AgcmUIGuild *pThis = (AgcmUIGuild *)pClass;
	PVOID *ppBuffer = (PVOID *)pData;
	CHAR *szGuildID = (CHAR *)pCustData;

	INT32 *plCID = ( INT32 *)ppBuffer[0];
	INT32 *pGuildMarkTID = ( INT32 *)ppBuffer[1];
	INT32 *pGuildMarkColor = ( INT32 *)ppBuffer[2];

	AgpdGuild *pcsGuild = pThis->m_pcsAgpmGuild->GetGuildLock(szGuildID);
	if( !pcsGuild )		return FALSE;

	pcsGuild->m_lGuildMarkTID	= *pGuildMarkTID;
	pcsGuild->m_lGuildMarkColor	= *pGuildMarkColor;

	if (strcmp( pThis->m_pcsAgcmGuild->GetSelfGuildID(), pcsGuild->m_szID))	//	�ڱ� ��尡 �ƴϸ�
	{
		pcsGuild->m_Mutex.Release();
		return FALSE;
	}

	pThis->m_lGuildMarkSelectedTID = pcsGuild->m_lGuildMarkTID;
	pThis->m_lGuildMarkSelectedColor = pcsGuild->m_lGuildMarkColor;

	pThis->InitSeletedGridItem();

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pcsUDGuildMarkGrid);
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pcsUDGuildMarkLayerIndex );
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pcsUDGuildMarkDetailInfo );

	pThis->SetPreviewGuildMark();
	
	AgpdCharacter *pcsCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (pcsCharacter)
	{
		if (pThis->m_pcsAgpmGuild->IsMaster(pcsGuild,pcsCharacter->m_szID))
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventGuildMarkBuyComplete);

		BOOL bIsWinnerGuild = pcsGuild->m_lBRRanking == WINNER_GUILD_1STPLACE ? TRUE : FALSE;
		pThis->m_pcsAgpmGuild->SetCharAD(pcsCharacter, szGuildID, pcsGuild->m_lGuildMarkTID, pcsGuild->m_lGuildMarkColor, bIsWinnerGuild, NULL);
	}
	pcsGuild->m_Mutex.Release();

	pThis->CloseGuildMarkUI();

	return  TRUE;
}

/*
	2005.10.20. By SungHoon
	��� ��ũ ���� ��ư�� ��������� Ȯ�� UI�� ��ﶧ �ʱ�ȭ ���� �۾��� �Ѵ�.
*/
BOOL AgcmUIGuild::CBGuildGuildMarkBuyConfirmUIOpen(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass) return FALSE;
	AgcmUIGuild *pThis = ( AgcmUIGuild *)pClass;
	return (pThis->GuildMarkBuyConfirmUIOpen());

}

/*
	2005.10.20. By SungHoon
	��� ��ũ ���� ��ư�� ��������� Ȯ�� UI�� ��ﶧ �ʱ�ȭ ���� �۾��� �Ѵ�.
*/
BOOL AgcmUIGuild::GuildMarkBuyConfirmUIOpen()
{
	AgpdGridItem *ppGridItem[3];
	ppGridItem[0] = m_pcsGuildMarkPreviewBuyGridItem;
	ppGridItem[1] = m_pcsGuildMarkPreviewBuyPatternGridItem;
	ppGridItem[2] = m_pcsGuildMarkPreviewBuySymbolGridItem;

	AgcdUIUserData *ppGridControl[3];
	ppGridControl[0] = m_pcsUDGuildMarkPreviewBuyGrid;
	ppGridControl[1] = m_pcsUDGuildMarkPreviewBuyPatternGrid;
	ppGridControl[2] = m_pcsUDGuildMarkPreviewBuySymbolGrid;

	return GuildMarkDraw(ppGridItem, ppGridControl, m_lGuildMarkSelectedTID, m_lGuildMarkSelectedColor);
}

BOOL AgcmUIGuild::GuildMarkDraw(AgpdGridItem **ppGridItem, AgcdUIUserData **ppGridControl, INT32 lGuildMarkTID, INT32 lGuildMarkColor)
{

	INT32 lGuildMarkTypeCode[3] = { AGPMGUILD_ID_MARK_BOTTOM, AGPMGUILD_ID_MARK_PATTERN, AGPMGUILD_ID_MARK_SYMBOL };
	for (int i = 0 ; i < 3; i++)
	{
		RwTexture**	ppRwLargeTexture = m_pcsAgcmUIControl->GetAttachGridItemTextureData(ppGridItem[i]);
		if (!ppRwLargeTexture) continue;
		*ppRwLargeTexture = NULL;

		INT32 lColor = m_pcsAgpmGuild->GetGuildMarkColor(lGuildMarkTypeCode[i], lGuildMarkColor);
		AgpdGuildMarkTemplate *pTemplate = m_pcsAgpmGuild->GetGuildMarkTemplate(lGuildMarkTypeCode[i], lGuildMarkTID);
		if (pTemplate)
		{
			AcUIGrid *pcsUIGrid = (AcUIGrid *)(*ppGridControl[i]->m_ppControls)->m_pcsBase;
			if (pcsUIGrid)
			{
				if (pcsUIGrid->m_apGridItemWindow)
				{
					AcUIGridItem *pGridItem = pcsUIGrid->m_apGridItemWindow[0];
					if (pGridItem) pGridItem->SetColorValue(lColor);
				}
			}
			AgcdGuildMarkTemplate	*pAgcdGuildMarkTemplate	= m_pcsAgcmGuild->GetAttachDataGuildMarkTemplate(pTemplate);
			if (pAgcdGuildMarkTemplate)
				*ppRwLargeTexture = pAgcdGuildMarkTemplate->m_pGuildMarkTexture;
		}
	}
	for (int i=0;i<3;i++)
	{
		m_pcsAgcmUIManager2->SetUserDataRefresh( ppGridControl[i]);
	}
	return TRUE;
}

/*
	2005.10.27. By SungHoon
	���ڸ� �Է¹޾� �޸��� �и��� ���ڷ� �����.
*/
BOOL AgcmUIGuild::MakeCommaNumber( CONST CHAR *szNumber, CHAR *szTargetNumber, INT16 lTargetLength )
{
	ZeroMemory( szTargetNumber, lTargetLength );
	INT16 lLength = strlen(szNumber);
	if (lLength >= lTargetLength)
	{
		strncpy(szTargetNumber, szNumber, lTargetLength - 1 );
		return FALSE;
	}

	INT16 nTargetNumberIndex = 0;
	for (INT16 i = 0 ; i < lLength; i++)
	{
		if ( i && ( ( ( lLength - i ) % 3) == 0 ) )
		{
			if (nTargetNumberIndex >= lTargetLength - 1) return FALSE;
			szTargetNumber[nTargetNumberIndex++] = ',';
		}

		if (nTargetNumberIndex >= lTargetLength - 1) return FALSE;
		szTargetNumber[nTargetNumberIndex++] = szNumber[i];
	}
	return TRUE;
}

/*
	2005.10.27. By SungHoon
	���ڸ� �Է¹޾� �޸��� �и��� ���ڷ� �����.
*/
BOOL AgcmUIGuild::MakeCommaNumber( INT32 lNumber, CHAR *szTargetNumber, INT16 lTargetLength )
{
	CHAR szNumber[128];
	wsprintf(szNumber, "%d", lNumber);

	return MakeCommaNumber(szNumber, szTargetNumber, lTargetLength);
}