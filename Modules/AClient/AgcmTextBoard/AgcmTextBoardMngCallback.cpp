#include <AgcmTextBoard/AgcmTextBoardMng.h>
#include <AuStrTable/AuStrTable.h>
#include "AgcmDmgBoard.h"
#include <AgcmTextBoard/AgcmHPBar.h>
#include <AgcmTextBoard/AgcmIDBoard.h>
#include "AgcmImoticonBoard.h"
#include <AgcmTextBoard/AgcmTextBoard.h>
#include <AgcmUIOption/AgcmUIOption.h>
#include <AgcmTitle/AgcmTitle.h>
#include <AgpmCharacter/AgpmCharacter.h>
#include <AgcmCharacter/AgcmCharacter.h>
#include <AgcmRender/AgcmRender.h>
#include <AgcmPostFX/AgcmPostFX.h>
#include <AgpmGuild/AgpmGuild.h>
#include <AgpmPvP/AgpmPvP.h>
#include <AgcmSound/AgcmSound.h>
#include <AgcmGuild/AgcmGuild.h>
#include <AgpmParty/AgpdParty.h>
#include <AgcmItem/AgcmItem.h>

#define ABSF( x ) ( ( x ) > 0 ? ( x ) : - ( x ) )

BOOL AgcmTextBoardMng::CB_POST_RENDER( PVOID pData, PVOID pClass, PVOID pCustData )
{
	PROFILE( "AgcmTextBoardMng::CB_POST_RENDER" );

	if( pClass == NULL ) return FALSE;

	AgcmTextBoardMng*    pThis    = static_cast< AgcmTextBoardMng* >( pClass );
	pThis->PostRender();

	return TRUE;
}	

BOOL AgcmTextBoardMng::CB_UpdateIDMurderIconFlag( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if( pClass == NULL || pData == NULL ) return FALSE;

	AgcmTextBoardMng*    pThis     = static_cast< AgcmTextBoardMng* >( pClass );
	AgpdFactor*          pcsFactor = static_cast< AgpdFactor*       >( pData  );

	INT32 nOwnerID   = 0;
	pThis->GetAgpmFactors()->GetValue( pcsFactor , &nOwnerID , AGPD_FACTORS_TYPE_OWNER , AGPD_FACTORS_OWNER_TYPE_ID );
	if( nOwnerID == 0 ) return FALSE;

	AgpdCharacter* pcsCharacter   = pThis->GetAgpmCharacter()->GetCharacter( nOwnerID );
	if( !pcsCharacter ) return FALSE;

	INT nCurrentMurderLevel    = pThis->GetAgpmCharacter()->GetMurdererLevel( pcsCharacter );

	pThis->DisableIDFlag( (ApBase*)pcsCharacter , TB_FLAG_MURDER1 );
	pThis->DisableIDFlag( (ApBase*)pcsCharacter , TB_FLAG_MURDER2 );
	pThis->DisableIDFlag( (ApBase*)pcsCharacter , TB_FLAG_MURDER3 );

	//. nCurrentMurderLevel�� ���� �ش� icon�� ��� flag�� on
	if (nCurrentMurderLevel >= AGPMCHAR_MURDERER_LEVEL3_POINT)
		pThis->EnableIDFlag((ApBase *) pcsCharacter, TB_FLAG_MURDER3);

	else if (nCurrentMurderLevel >= AGPMCHAR_MURDERER_LEVEL2_POINT)
		pThis->EnableIDFlag((ApBase *) pcsCharacter, TB_FLAG_MURDER2);

	else if (nCurrentMurderLevel >= AGPMCHAR_MURDERER_LEVEL1_POINT)	
		pThis->EnableIDFlag((ApBase *) pcsCharacter, TB_FLAG_MURDER1);

	return TRUE;
}	

BOOL AgcmTextBoardMng::CB_UpdateIDFirstAttackerIconFlag( PVOID pData, PVOID pClass, PVOID pCustData )
{

	AgcmTextBoardMng*	pThis        = static_cast< AgcmTextBoardMng * >(pClass);
	AgpdCharacter*	    pcsCharacter = static_cast< AgpdCharacter *    >(pData);
	if (!pcsCharacter)
		return FALSE;

	//AgpdCharacterCriminalStatus nCurrentCriminalStatus = *((AgpdCharacterCriminalStatus*)pCustData);
	AgpdCharacterCriminalStatus nCurrentCriminalStatus = (AgpdCharacterCriminalStatus) pcsCharacter->m_unCriminalStatus;

	if( nCurrentCriminalStatus == AGPDCHAR_CRIMINAL_STATUS_INNOCENT)
		pThis->DisableIDFlag((ApBase *) pcsCharacter, TB_FLAG_FIRSTATTACKER);

	else if(nCurrentCriminalStatus == AGPDCHAR_CRIMINAL_STATUS_CRIMINAL_FLAGGED)
		pThis->EnableIDFlag((ApBase *) pcsCharacter, TB_FLAG_FIRSTATTACKER);

	return TRUE;
}

BOOL AgcmTextBoardMng::CB_SetSelfCharacter( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmTextBoardMng   *pThis		= (AgcmTextBoardMng *)pClass;
	AgpdCharacter	   *pCharacter	= (AgpdCharacter*)pData;
	sBoardDataPtr       pBoard      = NULL;

	BOOL		        bFind = FALSE;

	pBoard   = pThis->GetMainCharacter();

	if( pBoard )
	{
		pThis->DestroyBoard( pBoard );
	}


	CHAR*	szTitle = pCharacter->m_szID;
	sBoardAttr sAttr;

	sAttr.eType  = AGCM_BOARD_IDBOARD;  sAttr.String = szTitle;
	sAttr.fType  = 0;                   sAttr.Color  = 0xFFFFFFFF;      
			
	pBoard = pThis->CreateBoard( (ApBase*)pData , &sAttr );
	if( !pBoard ) return FALSE;

	pBoard->pIDBoard->SetEnabled( pThis->GetDrawNameMine() );

	pThis->SetMainCharacter( pBoard );
	pBoard->pHPBar->SetColorCharisma( 0xFFFFFFFF );
	pBoard->pHPBar->SetColorNickName( 0xFFFFFFFF );


	pThis->m_pcRender->RemoveUpdateInfoFromClump( pBoard->pHPBar->GetClump() , pThis, CB_UpdateIDBoard , NULL , pBoard );
	pThis->m_pcRender->AddUpdateInfotoClump( pBoard->pHPBar->GetClump() , pThis , CB_UpdateIDBoardHPBar , NULL , pBoard , NULL);
	
	pBoard->pHPBar->SetDrawHPMP( FALSE );

	pBoard->pHPBar->GetOffsetHPMP().x	= -60.0f;
	pBoard->pHPBar->GetOffsetHPMP().y	=  10.0f;

	pBoard->pHPBar->SetDecreaseHP( FALSE );
	pBoard->pHPBar->SetDecreaseMP( FALSE );

	pBoard->pHPBar->SetPerResultHP( pThis->GetPointPercent( pCharacter , 0 ) );
	pBoard->pHPBar->SetPerResultMP( pThis->GetPointPercent( pCharacter , 1 ) );


	//if( DEF_FLAG_CHK( pCharacter->m_unEventStatusFlag, AGPM_CHAR_BIT_FLAG_EXPEDITION ) )
	//	pThis->EnableIDFlag((ApBase *) pCharacter, TB_FLAG_EVENTSTATUS_EXPEDITION);
	//else
	//	pThis->DisableIDFlag((ApBase *) pCharacter, TB_FLAG_EVENTSTATUS_EXPEDITION);

	//. pCharacter�� Criminal Status�� icon flag�� �߰�
	AgpdCharacterCriminalStatus nCurrentCriminalStatus = (AgpdCharacterCriminalStatus) pCharacter->m_unCriminalStatus;

	if( nCurrentCriminalStatus == AGPDCHAR_CRIMINAL_STATUS_INNOCENT)
		pThis->DisableIDFlag((ApBase *) pCharacter, TB_FLAG_FIRSTATTACKER);
	else if(nCurrentCriminalStatus == AGPDCHAR_CRIMINAL_STATUS_CRIMINAL_FLAGGED)
		pThis->EnableIDFlag((ApBase *) pCharacter, TB_FLAG_FIRSTATTACKER);

	//. pCharacter�� Murderer Level�� icon flag�� �߰�
	int nCurrentMurderLevel = pThis->m_pcsAgpmCharacter->GetMurdererLevel(pCharacter);

	//. All murder level flags off
	pThis->DisableIDFlag((ApBase *) pCharacter, TB_FLAG_MURDER1);
	pThis->DisableIDFlag((ApBase *) pCharacter, TB_FLAG_MURDER2);
	pThis->DisableIDFlag((ApBase *) pCharacter, TB_FLAG_MURDER3);

	//. nCurrentMurderLevel�� ���� �ش� icon�� ��� flag�� on
	if (nCurrentMurderLevel >= AGPMCHAR_MURDERER_LEVEL3_POINT)
		pThis->EnableIDFlag((ApBase *) pCharacter, TB_FLAG_MURDER3);
	else if (nCurrentMurderLevel >= AGPMCHAR_MURDERER_LEVEL2_POINT)
		pThis->EnableIDFlag((ApBase *) pCharacter, TB_FLAG_MURDER2);
	else if (nCurrentMurderLevel >= AGPMCHAR_MURDERER_LEVEL1_POINT)	
		pThis->EnableIDFlag((ApBase *) pCharacter, TB_FLAG_MURDER1);

	// Ÿ��Ʋ ���ڿ� ������Ʈ
	AgcmTitle* pcmTitle = GetAgcmTitle();
	if( pcmTitle )
	{
		char* pTitleName = pcmTitle->GetCurrentActivateTitleName( pCharacter->m_szID );
		DWORD dwColor = pcmTitle->GetTitleFontColor( TitleState_Activate );
		pcmTitle->OnUpdateTitleTextBoard( pCharacter, pTitleName, dwColor, TRUE );
	}
	
	return TRUE;
}

BOOL AgcmTextBoardMng::CB_ReleaseSelfCharacter( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmTextBoardMng	*pThis = static_cast< AgcmTextBoardMng* >(pClass);
	
	if( pThis->GetMainCharacter() && pThis->GetMainCharacter()->pHPBar )
	{
		// Ÿ��Ʋ ���ڿ� ����
		AgcmTitle* pcmTitle = GetAgcmTitle();
		if( pcmTitle )
		{
			DWORD dwColor = pcmTitle->GetTitleFontColor( TitleState_Activate );
			pcmTitle->OnUpdateTitleTextBoard( pThis->GetMainCharacter(), "", dwColor, TRUE );
		}

		pThis->GetAgcmRender()->RemoveUpdateInfoFromClump( pThis->GetMainCharacter()->pHPBar->GetClump() , pThis , CB_UpdateIDBoardHPBar , NULL, pThis->GetMainCharacter()->pHPBar );
		pThis->DestroyBoard( pThis->GetMainCharacter() );
	}

	return TRUE;
}

BOOL AgcmTextBoardMng::CB_AddChar( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmTextBoardMng *	pThis = static_cast< AgcmTextBoardMng *>(pClass);

	AgpdCharacter*	pcsCharacter = (AgpdCharacter *) pData;
	if( !pcsCharacter )		return FALSE;
	if( DEF_FLAG_CHK( pcsCharacter->m_ulCharType, AGPMCHAR_TYPE_CREATURE ) )		return TRUE;

	if ( pThis->m_pcCmCharacter->GetSelfCharacter() == pcsCharacter )
	{
		pThis->GetMainCharacter()->pHPBar->SetText( pcsCharacter->m_szID );
		return TRUE;
	}

	CHAR*	szTitle;
	if (pcsCharacter->m_szID[0])
		szTitle = pcsCharacter->m_szID;
	else if( pcsCharacter->m_pcsCharacterTemplate->m_szTName )
		szTitle = pcsCharacter->m_pcsCharacterTemplate->m_szTName;
	else if (strstr(pcsCharacter->m_pcsCharacterTemplate->m_szTName, ClientStr().GetStr(STI_GUARD)))
		szTitle = ClientStr().GetStr(STI_GUARD);

	sBoardAttr Attr;
	Attr.eType = AGCM_BOARD_IDBOARD;  Attr.String = szTitle; Attr.fType = 0; Attr.Color = 0xFFFFFFFF;
	pThis->CreateBoard( (ApBase*)pData , &Attr );

	//if (pcsCharacter->m_unEventStatusFlag & AGPM_CHAR_BIT_FLAG_EXPEDITION)
	//	pThis->EnableIDFlag((ApBase *) pcsCharacter, TB_FLAG_EVENTSTATUS_EXPEDITION);
	//else
	//	pThis->DisableIDFlag((ApBase *) pcsCharacter, TB_FLAG_EVENTSTATUS_EXPEDITION);

	//. pCharacter�� Criminal Status�� icon flag�� �߰�
	AgpdCharacterCriminalStatus nCurrentCriminalStatus = (AgpdCharacterCriminalStatus) pcsCharacter->m_unCriminalStatus;
	if( nCurrentCriminalStatus == AGPDCHAR_CRIMINAL_STATUS_INNOCENT)
		pThis->DisableIDFlag((ApBase *) pcsCharacter, TB_FLAG_FIRSTATTACKER);
	else if(nCurrentCriminalStatus == AGPDCHAR_CRIMINAL_STATUS_CRIMINAL_FLAGGED)
		pThis->EnableIDFlag((ApBase *) pcsCharacter, TB_FLAG_FIRSTATTACKER);

	//. pCharacter�� Murderer Level�� icon flag�� �߰�
	int nCurrentMurderLevel = pThis->m_pcsAgpmCharacter->GetMurdererLevel(pcsCharacter);

	//. All murder level flags off
	pThis->DisableIDFlag((ApBase *) pcsCharacter, TB_FLAG_MURDER1);
	pThis->DisableIDFlag((ApBase *) pcsCharacter, TB_FLAG_MURDER2);
	pThis->DisableIDFlag((ApBase *) pcsCharacter, TB_FLAG_MURDER3);

	//. nCurrentMurderLevel�� ���� �ش� icon�� ��� flag�� on
	if (nCurrentMurderLevel >= AGPMCHAR_MURDERER_LEVEL3_POINT)
		pThis->EnableIDFlag((ApBase *) pcsCharacter, TB_FLAG_MURDER3);
	else if (nCurrentMurderLevel >= AGPMCHAR_MURDERER_LEVEL2_POINT)
		pThis->EnableIDFlag((ApBase *) pcsCharacter, TB_FLAG_MURDER2);
	else if (nCurrentMurderLevel >= AGPMCHAR_MURDERER_LEVEL1_POINT)	
		pThis->EnableIDFlag((ApBase *) pcsCharacter, TB_FLAG_MURDER1);

	// ���� �߰� �� �� ��忡 �ش��ϴ� �͵� �������� ���ش�.
	if( pThis->m_pcsAgpmGuild )
	{
		CHAR* szGuildID = pThis->m_pcsAgpmGuild->GetJoinedGuildID(pcsCharacter);
		if(szGuildID && _tcslen(szGuildID) > 0 && pThis->m_pcCmCharacter->GetSelfCharacter())
			CB_PvPRefreshGuild(pThis->m_pcCmCharacter->GetSelfCharacter(), pThis, szGuildID);
	}

	// Ÿ��Ʋ ���ڿ� ������Ʈ
	AgcmTitle* pcmTitle = GetAgcmTitle();
	if( pcmTitle )
	{
		char* pTitleName = pcmTitle->GetCurrentActivateTitleName( pcsCharacter->m_szID );
		DWORD dwColor = pcmTitle->GetTitleFontColor( TitleState_Activate );
		pcmTitle->OnUpdateTitleTextBoard( pcsCharacter, pTitleName, dwColor, FALSE );
	}

	return TRUE;
}

BOOL AgcmTextBoardMng::CB_RemoveChar( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmTextBoardMng *	pThis        = static_cast< AgcmTextBoardMng* >(pClass);
	AgpdCharacter*      pdCharacter = static_cast< AgpdCharacter*    >(pData);

	// Ÿ��Ʋ ���ڿ� ����
	AgcmTitle* pcmTitle = GetAgcmTitle();
	if( pcmTitle )
	{
		DWORD dwColor = pcmTitle->GetTitleFontColor( TitleState_Activate );
		pcmTitle->OnUpdateTitleTextBoard( pdCharacter, "", dwColor, FALSE );
	}

	pThis->DisableHPBar( (ApBase*)pData );
	pThis->DestroyBoard( pdCharacter );	

	return TRUE;
}

BOOL AgcmTextBoardMng::CB_UpdateFactor( PVOID pData, PVOID pClass, PVOID pCustData )
{

	AgcmTextBoardMng *	pThis     =  static_cast< AgcmTextBoardMng * >(pClass);
	AgpdFactor *	    pcsFactor =  static_cast< AgpdFactor *       >(pData);

	INT32			lDamage     = 0;
	INT32			lStartCount = 0;
	INT32			lInterval   = 400;	// 0.5�ʸ��� ������ ��

	if ( pThis->GetAgpmFactors() )
	{
		INT32			lOwnerAddr	= 0;
		pThis->m_pcsAgpmFactors->GetValue(pcsFactor, &lOwnerAddr, AGPD_FACTORS_TYPE_OWNER, AGPD_FACTORS_OWNER_TYPE_OWNER);
		if (lOwnerAddr == 0)
			return FALSE;

		AgpdCharacter * pcsCharacter = (AgpdCharacter *) lOwnerAddr;

		return pThis->DisplayUpdateFactor(pcsCharacter, (AgpdFactor *) pThis->m_pcsAgpmFactors->GetFactor(pcsFactor, AGPD_FACTORS_TYPE_RESULT));
	}

	return TRUE;

}

BOOL AgcmTextBoardMng::CB_MissAttack( PVOID pData, PVOID pClass, PVOID pCustData )
{
	CB_MissCastSkill( pData , pClass , pCustData );

	return TRUE;
}

BOOL AgcmTextBoardMng::CB_MissCastSkill( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmTextBoardMng*	pThis			= static_cast< AgcmTextBoardMng* >(pClass);
	ApBase			   *pcsAttacker	    = static_cast< ApBase *          >(pData);
	ApBase			   *pcsTarget		= static_cast< ApBase *          >(pCustData);

	if (pcsAttacker)
	{
		// ������ ���� �� ������ ���� ���⼭ ���ش�.
	}

	if (pcsTarget)
	{
		// �ǰ����ڿ��� ǥ������ ������ ���� ���⼭ ������ش�.
		sBoardAttr Attr;
		Attr.eType     = (eBoardType)IMOTICON_MISS;
		Attr.ViewCount = 800;

		pThis->CreateBoard( pcsTarget->m_lID , &Attr );
	}

	return TRUE;
}

BOOL AgcmTextBoardMng::CB_BlockCastSkill( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmTextBoardMng*	pThis       = static_cast< AgcmTextBoardMng* >(pClass);
	ApBase*             pcsTarget	= static_cast< ApBase *          >(pData);
	
	// Block Emoticon�� �����
	sBoardAttr Attr;
	Attr.eType     = (eBoardType)IMOTICON_BLOCK;
	Attr.ViewCount = 800;

	pThis->CreateBoard( pcsTarget->m_lID , &Attr );

	return TRUE;
}

BOOL AgcmTextBoardMng::CB_DisplayActionResult( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmTextBoardMng				*pThis			= static_cast< AgcmTextBoardMng *            >(pClass);
	AgpdCharacter					*pcsCharacter	= static_cast< AgpdCharacter *               >(pData);
	AgcmCharacterActionQueueData	*pcsActionData	= static_cast< AgcmCharacterActionQueueData *>(pCustData);

	if( pThis->GetAgcmCharacter() )
	{
		AgpdCharacter* pcsSelfCharacter = pThis->GetAgcmCharacter()->GetSelfCharacter();

		if( pcsSelfCharacter) 
		{
			if( pcsCharacter->m_lID != pcsSelfCharacter->m_lID && pcsActionData->lActorID != pcsSelfCharacter->m_lID )
				return TRUE;
		}
	}

	switch( pcsActionData->eActionType )
	{
	case AGPDCHAR_ACTION_TYPE_ATTACK:
		{
			switch (pcsActionData->eActionResultType)
			{
			case AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_SUCCESS:
				return pThis->DisplayUpdateFactor(pcsCharacter, &pcsActionData->csFactorDamage);
			case AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_MISS:
				{
					sBoardAttr Attr;
					Attr.eType     = (eBoardType)IMOTICON_MISS;
					Attr.ViewCount = 800;
					
					if( pThis->CreateBoard( pcsCharacter->m_lID , &Attr ) ) return TRUE;
					else													return FALSE;
				}
				
			case AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_CRITICAL:
			case AGPDCHAR_ACTION_RESULT_TYPE_DEATH_STRIKE:
			case AGPDCHAR_ACTION_RESULT_TYPE_COUNTER_ATTACK:
				{
					AgpdCharacter* pcsAgpdCharacter = pThis->GetAgcmCharacter()->GetAgpmCharacter()->GetCharacter(pcsActionData->lActorID);
					if( pcsAgpdCharacter )
					{
						if(pcsAgpdCharacter == pThis->GetAgcmCharacter()->GetSelfCharacter())
						{
							if( pThis->GetAgcmUIOption()->m_eImpact == AGCD_OPTION_TOGGLE_ON )
							{
								//pThis->PlayEffectFX( "Wave", 2.0f );

								if( pThis->GetAgcmCharacter()->GetCharacterData(pcsCharacter) )
								{
									RpClump* pClump = pThis->GetAgcmCharacter()->GetCharacterData(pcsCharacter)->m_pClump;
									if( pClump )
									{
										RwSphere sphere = pClump->stType.boundingSphere;
										RwV3d world, view, screen;
										float one_z;
										RwV3dTransformPoints(&world, &sphere.center, 1, RwFrameGetLTM(RpClumpGetFrame(pClump)));
										pThis->GetWorldPosToScreenPos(&world, &view, &screen, &one_z);
					
										Technique*	pTech	=	pThis->m_pcsAgcmPostFX->CreateFX( POSTFX_NAME_GENERATE  , E_PIPELINE_WAVE );
										pTech->SetCenter(screen.x/RwRasterGetWidth(RwCameraGetRaster(pThis->m_pCamera)), screen.y/RwRasterGetHeight(RwCameraGetRaster(pThis->m_pCamera)));
										pTech->SetPlayTime( 500 );
										pTech->SetSpeedScale( 1.5f );
										pTech->SetUpdate( TRUE );
									}
								}

								
								

							}
						}
					}

					if( pcsActionData->eActionResultType == AGPDCHAR_ACTION_RESULT_TYPE_DEATH_STRIKE )
					{
						return pThis->DisplayUpdateFactor(pcsCharacter, &pcsActionData->csFactorDamage);
					}

					if( pcsActionData->eActionResultType == AGPDCHAR_ACTION_RESULT_TYPE_COUNTER_ATTACK )
					{
						sBoardAttr Attr;
						Attr.eType     = (eBoardType)IMOTICON_ATTACK;  Attr.ViewCount = 800;
						if( pThis->CreateBoard( pcsCharacter->m_lID , &Attr ) ) return TRUE;
						else                                                    return FALSE;
					}

					// ũ��Ƽ�� ������ ǥ��
					if (pThis->m_pcsAgpmFactors)
					{
						AgpdFactor*	pcsFactor = &pcsActionData->csFactorDamage;
						INT32	lDamage = 0;

						if (pThis->m_pcsAgpmFactors->GetValue(pcsFactor, &lDamage, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_DMG_NORMAL) && lDamage < 0)
						{
							lDamage = -lDamage;
						}

						sBoardAttr Attr;
						Attr.eType    = (eBoardType)AGCM_BOARD_CRITICALDAMAGE;   Attr.nDamage  = lDamage;	Attr.Color	= 0xFFFFFFFF;
						pThis->CreateBoard( pcsCharacter , &Attr );
					}

					// ũ��Ƽ�� ��Ʈ
					sBoardAttr Attr;
					Attr.eType     = (eBoardType)IMOTICON_CRITICAL;   Attr.ViewCount = 800;
					if( pThis->CreateBoard( pcsCharacter->m_lID , &Attr ) ) return TRUE;
					else													return FALSE;
				}
				break;


			case AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_EVADE:		// ������ �ٰŸ� ���� ȸ��
				{
					sBoardAttr Attr;
					Attr.eType     = (eBoardType)IMOTICON_EVADE;    Attr.ViewCount = 800;
					pThis->CreateBoard( pcsCharacter->m_lID , &Attr );
					
				}
				break;

			case AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_DODGE:		// ������ ���Ÿ� ���� ȸ��
				{
					sBoardAttr Attr;
					Attr.eType     = (eBoardType)IMOTICON_DODGE;   Attr.ViewCount = 800;
					pThis->CreateBoard( pcsCharacter->m_lID , &Attr );
					break;
				}

			case AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_BLOCK:
			case AGPDCHAR_ACTION_RESULT_TYPE_REDUCE_DAMAGE:		// ������ ������ ���з� ��
				{
					sBoardAttr Attr;
					Attr.eType     = (eBoardType)IMOTICON_BLOCK;   Attr.ViewCount = 900;
					if( pThis->CreateBoard( pcsCharacter->m_lID , &Attr ) ) return TRUE;
					else													return FALSE;
				}


			case AGPDCHAR_ACTION_RESULT_TYPE_REFLECT_DAMAGE:	// ������ ���ݿ� ���� �ݻ�. 2005.12.24. steeple
				{
					sBoardAttr Attr;
					Attr.eType      = (eBoardType)IMOTICON_REFLEX;    Attr.ViewCount  = 800;
					if( pThis->CreateBoard( pcsCharacter->m_lID , &Attr ) ) return TRUE;
					else													return FALSE;
				}
			default:
				return TRUE;
			}
		}
		break;

	case AGPDCHAR_ACTION_TYPE_SKILL:
		{
			switch( pcsActionData->eActionResultType )
			{
			case AGPMSKILL_ACTION_CAST_SKILL_RESULT:	return pThis->DisplayUpdateFactor( pcsCharacter, &pcsActionData->csFactorDamage );
			case AGPMSKILL_ACTION_MISS_CAST_SKILL:		
				{
					sBoardAttr Attr;
					Attr.eType = (eBoardType)IMOTICON_MISS;  Attr.ViewCount = 800;
					if( pThis->CreateBoard( pcsCharacter->m_lID , &Attr ) ) return TRUE;
					else													return FALSE;
				}
			default:									return TRUE;
			}
		}
		break;
	}

	return TRUE;
}

BOOL
AgcmTextBoardMng::CB_ItemAdd(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmTextBoardMng *	pThis   = static_cast<AgcmTextBoardMng *>(pClass);
	AgpdItem *	        pdItem  = static_cast<AgpdItem *>(pData);
	sBoardAttr			attr;
	UINT32				ulColor = 0;

	if(!pdItem) 
		return FALSE;
	if (pThis->m_pcsAgcmItem->IsItemDropSettingsEnabled())
		ulColor = pThis->m_pcsAgcmItem->GetItemDropColor(pdItem->m_lTID);
	if (ulColor) {
		/* Flip RGB */
		attr.Color = 0xFF000000 |
			((ulColor >> 16) & 0xFF) |
			(ulColor & 0xFF00) |
			((ulColor & 0xFF) << 16);

	}
	else {
		attr.Color = 0xFF96FBFF;
	}
	attr.eType = AGCM_BOARD_ITEMID; 
	attr.fType = 0;  
	attr.String = pdItem->m_pcsItemTemplate->m_szName;
	pThis->CreateBoard(pdItem, &attr);
	return TRUE;
}

BOOL AgcmTextBoardMng::CB_ItemRemove( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmTextBoardMng *	pThis = (AgcmTextBoardMng *) pClass;

	pThis->DestroyBoard( (ApBase*)pData );

	return TRUE;
}

BOOL AgcmTextBoardMng::CB_UpdateActionStatus( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmTextBoardMng    *pThis        =  static_cast< AgcmTextBoardMng* >(pClass);
	AgpdCharacter	    *pcsCharacter =  static_cast< AgpdCharacter *   >(pData);
	INT16			     nOldStatus	  = *static_cast< INT16*        >(pCustData);

	if ( nOldStatus != pcsCharacter->m_unActionStatus &&
		(nOldStatus == AGPDCHAR_STATUS_DEAD || pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD))
		pThis->CB_HPUpdate(pcsCharacter, pThis, NULL);


	return TRUE;
}

BOOL AgcmTextBoardMng::CB_RegionChange( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmTextBoardMng* pThis        = static_cast< AgcmTextBoardMng* >(pClass);
	AgpdCharacter*    pcsCharacter = static_cast< AgpdCharacter*    >(pData);
	INT16 nPrevRegionIndex         = pCustData ? *static_cast<INT16*>(pCustData) : -1;	// NULL �� �ö��� ADD_CHARACTER_TO_MAP ���� �Ҹ�����.

	AgpdCharacter* pcsSelfCharacter   = NULL;

	if( pThis->GetMainCharacter()->pHPBar->GetApBase() )
		pcsSelfCharacter    = (AgpdCharacter*)pThis->GetMainCharacter()->pHPBar->GetApBase();

	if(pcsSelfCharacter == pcsCharacter)
	{
		// �ڱ� ���� ������Ʈ ��
		// ���� ĳ���͸� ��� ���鼭 ó�����ش�.
		BoardDataList& rBoardList  = pThis->GetBoardDataList();
		BoardDataListIter Iter     = rBoardList.begin();

		for( ; Iter != rBoardList.end() ; ++Iter )
		{
			AgpdCharacter* pcsTarget   = (AgpdCharacter*)(*Iter)->pBase;

			if( pThis->GetAgpmCharacter()->IsPC(pcsTarget) == FALSE ||
				_tcslen(pcsTarget->m_szID) == 0 )
			{
				continue;
			}

			char* szTargetGuildID   = pThis->GetAgpmGuild()->GetJoinedGuildID( pcsTarget );
			if( szTargetGuildID && _tcslen( szTargetGuildID ) > 0 )
			{
				CB_PvPRefreshGuild( pcsSelfCharacter , pThis , szTargetGuildID );
			}
			else
			{
				CB_PvPRefreshChar( pcsTarget, pThis , NULL );
			}
		}
	}

	else
	{
		CHAR* szGuildID = pThis->m_pcsAgpmGuild->GetJoinedGuildID(pcsCharacter);
		if(szGuildID && _tcslen(szGuildID) > 0)
			CB_PvPRefreshGuild(pcsSelfCharacter, pThis, szGuildID);
		else
			CB_PvPRefreshChar(pcsCharacter, pThis, NULL);
	}

	return TRUE;
}

BOOL AgcmTextBoardMng::CB_SkillAddHPBar( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmTextBoardMng	*pThis = static_cast     < AgcmTextBoardMng* >(pClass);
	ApBase			    *pBase = static_cast     < ApBase*           >(pData);
	INT32			    iCount = reinterpret_cast< INT32             >(pCustData);

	if( pThis->GetAgcmCharacter()->m_pcsSelfCharacter == (AgpdCharacter*)pData)
		pThis->EnableHPBar( pBase , TRUE , iCount );

	return TRUE;
}

BOOL AgcmTextBoardMng::CB_HPUpdate( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmTextBoardMng*		pThis				=	static_cast< AgcmTextBoardMng* >(pClass);
	AgpdCharacter			*pdCharacter		=	static_cast< AgpdCharacter *   >(pData);
	
	sBoardDataPtr pBoardData					=	pThis->GetBoard( pdCharacter );
	if( pBoardData && pBoardData->pHPBar ) 
	{
		if( pBoardData->pHPBar->GetDecreaseHP()  == FALSE)
		{
			pBoardData->pHPBar->SetDecreaseHP( TRUE );
			pBoardData->pHPBar->SetPerAfterImageHP( pBoardData->pHPBar->GetPerResultHP() );
			pBoardData->pHPBar->SetSpeedAfterImageHP( AGCM_TEXT_BOARD_AFTERIMAGE_DECREASE_SPEED );
		}
		pBoardData->pHPBar->SetPerResultHP( pThis->GetPointPercent( pdCharacter , 0 ) );
	}

	return TRUE;
}

BOOL AgcmTextBoardMng::CB_MPUpdate( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmTextBoardMng*	 pThis       = static_cast< AgcmTextBoardMng* >(pClass);
	AgpdCharacter		*pdCharacter = static_cast< AgpdCharacter *   >(pData);

	if( pThis->GetMainCharacter() == NULL ) return FALSE;

	AgcmHPBar*           pHPBar      = pThis->GetMainCharacter()->pHPBar;

	if( pHPBar )
	{
		if( pHPBar->GetApBase() == (ApBase*)pData)
		{
			if( pHPBar->GetDecreaseMP() == FALSE )
			{
				pHPBar->SetDecreaseMP( TRUE );
				pHPBar->SetPerAfterImageMP( pHPBar->GetPerResultMP() );
				pHPBar->SetSpeedAfterImageMP( AGCM_TEXT_BOARD_AFTERIMAGE_DECREASE_SPEED );
			}
			
			pHPBar->SetPerResultMP( pThis->GetPointPercent( pdCharacter , 1 ) );
		}
	}

	return TRUE;
}

BOOL AgcmTextBoardMng::CB_Guild( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmTextBoardMng*	pThis       = static_cast< AgcmTextBoardMng* >(pClass);
	AgpdCharacter	   *pdCharacter	= static_cast< AgpdCharacter *   >(pData);

	AgpdGuildADChar*	pADG = pThis->m_pcsAgpmGuild->GetADCharacter(pdCharacter);

	if(strlen(pADG->m_szGuildID))
	{
		DWORD dwGuildColor = pThis->GetGuildIDColor(pADG->m_szGuildID, pdCharacter->m_szID);
		pThis->EnableGuild((ApBase*)pData,pADG->m_szGuildID, dwGuildColor);
	}
	else
	{
		pThis->DisableGuild((ApBase*)pData);
		CB_PvPRefreshChar((ApBase*)pData, pThis, pCustData);
	}

	CB_PvPRefreshGuild(pThis->m_pcCmCharacter->GetSelfCharacter(), pThis, pCustData);

	return TRUE;
}

BOOL AgcmTextBoardMng::CB_PartyAdd( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmTextBoardMng*	 pThis  = static_cast< AgcmTextBoardMng* >(pClass);
	AgpdParty			*pParty	= static_cast< AgpdParty *       >(pData);

	for( int i=0; i<pParty->m_nCurrentMember; ++i )
		pThis->EnableParty((ApBase*)pThis->m_pcsAgpmCharacter->GetCharacter(pParty->m_lMemberListID[i]));

	return TRUE;
}

BOOL AgcmTextBoardMng::CB_PartyLeave( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmTextBoardMng*		pThis = static_cast< AgcmTextBoardMng* >(pClass);

	pThis->DisableAllParty();

	return TRUE;
}

BOOL AgcmTextBoardMng::CB_PartyAddMember( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmTextBoardMng*	 pThis      = static_cast< AgcmTextBoardMng* >(pClass);
	AgpdCharacter		*pCharacter	= static_cast< AgpdCharacter *   >(pCustData);

	pThis->EnableParty((ApBase*)pCharacter);

	return TRUE;
}

BOOL AgcmTextBoardMng::CB_PartyLeaveMember( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmTextBoardMng*	pThis       = static_cast< AgcmTextBoardMng* >(pClass);
	AgpdCharacter		*pCharacter	= static_cast< AgpdCharacter *   >(pCustData);

	pThis->DisableParty((ApBase*)pCharacter);

	return TRUE;
}

BOOL AgcmTextBoardMng::CB_UpdateIDBoard( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if( !pData || !pClass ) return FALSE;

	sBoardDataPtr	   pBoardData	= static_cast< sBoardDataPtr     >(pData);
	AgcmTextBoardMng*  pThis	    = static_cast< AgcmTextBoardMng* >(pClass);

	AgcmIDBoard*       pIDBoard     = pBoardData->pIDBoard;

	if( pIDBoard == NULL ) return FALSE;

	if( !pIDBoard->GetEnabled() && pIDBoard->GetEnableGuild() != 1 ) return TRUE;
	if(  pIDBoard->GetClump()->stType.eType & ACUOBJECT_TYPE_INVISIBLE         ) return TRUE;

	RwV3d	posScreen;
	pThis->Trans3DTo2D( &(pIDBoard->GetCameraPos()) , &posScreen , pIDBoard->GetRecipzPtr() , 
					    (const RwV3d*)pIDBoard->GetPosition() , pIDBoard->GetHeight() ,
						pIDBoard->GetClump() , pIDBoard->GetDepth() );

	FLOAT fGap = ABSF( pIDBoard->GetScreenPos().x - posScreen.x );
	if( fGap < 1 && ( int ) ( pIDBoard->GetScreenPos().x ) != (int)(posScreen.x))
		;
	else
		pIDBoard->GetScreenPos().x = posScreen.x;

	fGap = ABSF( pIDBoard->GetScreenPos().y - posScreen.y );
	if( fGap < 1 && ( int ) ( pIDBoard->GetScreenPos().y ) != ( int ) ( posScreen.y ))
		;
	else
		pIDBoard->GetScreenPos().y = posScreen.y;

	pIDBoard->GetScreenPos().z = posScreen.z;

	INT TmpAlpha = ( int ) ( ((12000.0f - pIDBoard->GetCameraPos().z) / 10000.0f) * 255.0f );
	TmpAlpha     = max( 64 , min( TmpAlpha , 255 ) );

	pIDBoard->SetAlpha  ( TmpAlpha );
	pIDBoard->SetVisible( TRUE     );

	return TRUE;
}


BOOL AgcmTextBoardMng::CB_UpdateIDBoardHPBar( PVOID pData, PVOID pClass, PVOID pCustData )
{
	sBoardDataPtr           pBoardData = static_cast< sBoardDataPtr    >(pData);
	AgcmTextBoardMng*		pThis      = static_cast< AgcmTextBoardMng*>(pClass);

	if( pBoardData->pHPBar == NULL ) return FALSE;

	pThis->UpdateIDBoardHPBar( pBoardData->pHPBar );

	RwV3d	posScreen;
	pThis->Trans3DTo2D( &(pBoardData->pHPBar->GetCameraPos()), &posScreen, pBoardData->pHPBar->GetRecipzPtr(), 
						(const RwV3d*)pBoardData->pHPBar->GetPosition(), pBoardData->pHPBar->GetHeight(), 
						pBoardData->pHPBar->GetClump(), pBoardData->pHPBar->GetDepth() );

	FLOAT fGap = ABSF( pBoardData->pHPBar->GetScreenPos().x - posScreen.x );
	if( fGap >= 1 )    pBoardData->pHPBar->GetScreenPos().x = posScreen.x;

	fGap       = ABSF( pBoardData->pHPBar->GetScreenPos().y - posScreen.y );
	if( fGap >= 1 )    pBoardData->pHPBar->GetScreenPos().y = posScreen.y;

	pBoardData->pHPBar->GetScreenPos().x  = ( float ) ( ( int ) pBoardData->pHPBar->GetScreenPos().x );
	pBoardData->pHPBar->GetScreenPos().y  = ( float ) ( ( int ) pBoardData->pHPBar->GetScreenPos().y );
	pBoardData->pHPBar->GetScreenPos().z  = posScreen.z;
	
	INT TmpAlpha  = (int) ( ((12000.0f - pBoardData->pHPBar->GetCameraPos().z) / 10000.0f) * 255.0f );
	TmpAlpha      = max( 64 , min( TmpAlpha , 255 ) );

	pBoardData->pHPBar->SetAlpha( TmpAlpha );   // Alpha�� ����
	pBoardData->pHPBar->SetVisible( TRUE );     // Visible�� TRUE�� �Ǹ� �׷����� ��

	return TRUE;
}

BOOL AgcmTextBoardMng::CB_PvPRefreshChar( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if(!pData || !pClass)
		return FALSE;

	AgcmTextBoardMng* pThis     = static_cast< AgcmTextBoardMng* >(pClass);
	AgpdCharacter* pcsCharacter = static_cast< AgpdCharacter*    >(pData);

	INT32 lTargetCID = 0;
	if(pCustData)          lTargetCID = *((INT32*)pCustData);

	AgpdCharacter* pcsSelfCharacter = pThis->m_pcCmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter)
		return FALSE;

	AgpdCharacter* pcsTarget = NULL;
	if(lTargetCID != AP_INVALID_CID && pcsSelfCharacter->m_lID != lTargetCID)
		pcsTarget = pThis->m_pcsAgpmCharacter->GetCharacter(lTargetCID);
	else
		pcsTarget = pcsCharacter;

	if( !pcsTarget )						return TRUE;
	if( !strlen( pcsTarget->m_szID ) )		return TRUE;

	DWORD dwColor = pThis->GetTargetIDColor(pcsTarget);

	AgcdCharacter* pcsAgcdCharacter = pThis->m_pcCmCharacter->GetCharacterData(pcsTarget);
	if( pcsAgcdCharacter )
		pcsAgcdCharacter->m_dwCharIDColor = dwColor;

	// pcsTarget �� �ٽ� �׷��ش�.
	if( !pThis->GetAgpmCharacter()->IsNPC( pcsTarget ) )
		pThis->ChangeIDColor(pcsTarget, dwColor);

	return TRUE;
}

BOOL AgcmTextBoardMng::CB_PvPRefreshGuild( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgcmTextBoardMng* pThis       = static_cast< AgcmTextBoardMng* >(pClass);
	AgpdCharacter* pcsCharacter   = static_cast< AgpdCharacter*    >(pData);
	CHAR*          szOtherGuildID = static_cast< CHAR*             >(pCustData);

	// pcsCharacter �� �ڱⰡ �ƴϸ� ���ص� �ȴ�.
	AgpdCharacter* pcsSelfCharacter = pThis->m_pcCmCharacter->GetSelfCharacter();
	if( !pcsSelfCharacter )					return FALSE;
	if( pcsSelfCharacter != pcsCharacter )	return TRUE;
	if( !strlen(szOtherGuildID) || strlen(szOtherGuildID) > AGPMGUILD_MAX_GUILD_ID_LENGTH)	return TRUE;

	CHAR* szSelfGuildID = pThis->m_pcsAgpmGuild->GetJoinedGuildID( pcsCharacter );

	BOOL bOtherCharacter = FALSE;
	BOOL bEnemy          = FALSE;
	BOOL bEnemyGuild     = FALSE;
	BOOL bJointGuild     = FALSE;

	DWORD dwColor = 0xFFFFFFFF;
	//if( pThis->m_pcsAgpmPvP->IsCombatPvPMode(pcsCharacter) )
	//	dwColor = 0xFF41E6FF;	// ���

	if( pThis->m_pcsAgpmPvP->IsEnemyGuild(pcsCharacter, szOtherGuildID) )
	{
		if( szSelfGuildID && szSelfGuildID[0] )
		{
			//AgcmGuild* pcsAgcmGuild = (AgcmGuild*)pThis->GetModule("AgcmGuild");
			//if( pcsAgcmGuild && pcsAgcmGuild->IsGuildBattle( pcsCharacter ) )
			{
				dwColor = 0xFF0000FF;	// ����
				bEnemy = TRUE;
				bEnemyGuild = TRUE;
			}
		}
	}
	else
	{
		if( szSelfGuildID && _tcslen(szSelfGuildID) > 0)
		{
			// ����/���� ������� Ȯ�����ش�.
			if(pThis->m_pcsAgpmGuild->IsJointGuild(szSelfGuildID, szOtherGuildID))
			{
				dwColor = 0xFF009900;
				bJointGuild = TRUE;
			}
			else if(pThis->m_pcsAgpmGuild->IsHostileGuild(szSelfGuildID, szOtherGuildID))
			{
				dwColor = 0xFF0000FF;	// ����
				bEnemy = TRUE;
			}
		}
	}

	BOOL bIsSelfGuild = FALSE;
	if(pThis->m_pcsAgpmPvP->IsFriendGuild(pcsCharacter, szOtherGuildID))
	{
		// ������ ���� ��Ÿ���� ���� �ش�.
		dwColor = 0xFF2AE180;	// ���
	}
	else
	{
		// �������� �ƴ� ���� �ڱ� ������� �ѹ� �� Ȯ��
		if( szSelfGuildID && strlen(szSelfGuildID) > 0 && !strcmp(szSelfGuildID, szOtherGuildID) )
		{
			dwColor = 0xFF2AE180;	// ���	
			bIsSelfGuild = TRUE;
		}
		else if( !bEnemy )
			bOtherCharacter = TRUE;
	}

	CHAR    szTmpGuildID[AGPMGUILD_MAX_GUILD_ID_LENGTH + 1 + 4];	// [ ] �� �߰��Ϸ���
	memset( szTmpGuildID, 0, sizeof(CHAR) * (AGPMGUILD_MAX_GUILD_ID_LENGTH + 1 + 4) );
	strcpy( szTmpGuildID, "[" );
	strcat( szTmpGuildID, szOtherGuildID );
	strcat( szTmpGuildID, "]" );
	
	AgcmIDBoard*       pIDBoard = NULL;
	BoardDataList&     rBoardList = pThis->GetBoardDataList();
	BoardDataListIter  Iter       = rBoardList.begin();

	for( ; Iter != rBoardList.end() ; ++Iter )
	{
		pIDBoard   = (*Iter)->pIDBoard;
		if( pIDBoard == NULL ) continue;

		if( strlen( pIDBoard->GetGuildText() ) > 0 && !strcmp( pIDBoard->GetGuildText() , szTmpGuildID ) )
		{

			AgcmGuild* pcsAgcmGuild   = (AgcmGuild*)pThis->GetModule( "AgcmGuild" );
			if( bEnemyGuild && !pcsAgcmGuild->IsGuildBattleMember( pcsCharacter , (AgpdCharacter*)pIDBoard->GetApBase() ) )
			{
			}

			else
			{
				pIDBoard->SetColor( dwColor );
				pIDBoard->SetColorGuild( dwColor != 0xFFFFFFFF ? dwColor : 0xFF81FAB2 );

				if( pIDBoard->GetApBase() && pIDBoard->GetApBase()->m_eType == APBASE_TYPE_CHARACTER )
				{
					if( bIsSelfGuild )
						pIDBoard->SetColorGuild( pThis->GetGuildIDColor(szOtherGuildID , ((AgpdCharacter*)pIDBoard->GetApBase())->m_szID , pIDBoard->GetColorGuild() ) );

					if( bOtherCharacter && pThis->GetAgpmPvP()->IsNowPvP( (AgpdCharacter*)pIDBoard->GetApBase() ) )
						dwColor = 0xFFFFFFFF;

					AgcdCharacter* pcsAgcdCharacter = pThis->GetAgcmCharacter()->GetCharacterData((AgpdCharacter*)pIDBoard->GetApBase() );
					if( pcsAgcdCharacter )	pcsAgcdCharacter->m_dwCharIDColor = dwColor;
				}
			}
		}
	}

	pThis->ChangeGuildIDColor( pcsCharacter , szTmpGuildID );
	return TRUE;
}

BOOL AgcmTextBoardMng::CB_PvPInit( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgcmTextBoardMng*	pThis			= static_cast< AgcmTextBoardMng* >(pClass);
	AgpdCharacter*		pcsCharacter    = static_cast< AgpdCharacter* >(pData);

	BoardDataList&      rBoardList		= pThis->GetBoardDataList();
	BoardDataListIter   Iter			= rBoardList.begin();

	// ���� ĳ���� ���鼭 ���� �׷��ָ� �ɵ�.
	while( Iter != rBoardList.end() )
	{
		sBoardDataPtr pBoard	=	(*Iter);
		if( !pBoard )
		{
			rBoardList.erase( Iter++ );
			continue;
		}
		
		if( pBoard->pBase && pBoard->pBase->m_eType == APBASE_TYPE_CHARACTER )
		{
			CB_PvPRefreshChar( pBoard->pBase , pThis , NULL );
		}

		++Iter;
	}

	return TRUE;
}

BOOL AgcmTextBoardMng::CB_DestroyBoard( PVOID pData , PVOID pClass , PVOID pCustData )
{
	AgcmTextBoardMng*		pThis			= static_cast< AgcmTextBoardMng* >(pClass);
	AgcdCharacter*			pcdCharacter	= static_cast< AgcdCharacter* >(pData);

	if( pcdCharacter && pcdCharacter->m_pvTextboard )
	{
		pThis->DestroyBoard( ((sBoardDataPtr)pcdCharacter->m_pvTextboard) );
		pcdCharacter->m_pvTextboard	=	NULL;
	}

	return TRUE;
}

BOOL AgcmTextBoardMng::CB_KillCharacter( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmTextBoardMng*		pThis         = static_cast< AgcmTextBoardMng* >(pClass);
	AgpdCharacter			*pdCharacter  = static_cast< AgpdCharacter *   >(pData);
	AgpdCharacter			*pdActor	  = static_cast< AgpdCharacter *   >(pCustData);

	if( pdActor && (pdActor == pThis->GetAgcmCharacter()->GetSelfCharacter()) )
	{
		
		RpClump* pClump = pThis->GetAgcmCharacter()->GetCharacterData(pdCharacter)->m_pClump;
		if( pClump && pThis->m_pcsAgcmUIOption->m_eImpact==AGCD_OPTION_TOGGLE_ON )
		{
			// set the center to the current enemy center.
			RwSphere sphere = pClump->stType.boundingSphere;
			RwV3d world, view, screen;
			float one_z;

			pThis->LockFrame();
			RwV3dTransformPoints(&world, &sphere.center, 1, RwFrameGetLTM(RpClumpGetFrame(pClump)));
			pThis->UnlockFrame();

			pThis->GetWorldPosToScreenPos(&world, &view, &screen, &one_z);

			Technique*	pTech	=	pThis->m_pcsAgcmPostFX->CreateFX( POSTFX_NAME_GENERATE  , E_PIPELINE_WAVE );
			pTech->SetCenter(screen.x/RwRasterGetWidth(RwCameraGetRaster(pThis->m_pCamera)), screen.y/RwRasterGetHeight(RwCameraGetRaster(pThis->m_pCamera)));
			pTech->SetPlayTime( 300 );
			pTech->SetSpeedScale( 1.0f );
			pTech->SetUpdate( TRUE );
			
			pThis->GetAgcmSound()->PlaySampleSound("Sound\\UI\\Visual_ES.wav");
		}
	}

	return TRUE;
}

BOOL AgcmTextBoardMng::CB_UpdateNickName( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmTextBoardMng*	pThis        = static_cast< AgcmTextBoardMng * >(pClass);

	if( pThis == NULL ) return FALSE;

	return pThis->UpdateNickName( (ApBase*)pData );
}