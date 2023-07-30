#include <AgcmTextBoard/AgcmIDBoard.h>
#include <AgcmUIOption/AgcmUIOption.h>
#include <AgpmCharacter/AgpmCharacter.h>
#include <AgcmCharacter/AgcmCharacter.h>
#include <AgcmFont/AgcmFont.h>
#include <AgpmBattleGround/AgpmBattleGround.h>
#include <AgcModule/AgcEngine.h>
#include <AgpmGuild/AgpmGuild.h>
#include <AgpmSiegeWar/AgpmSiegeWar.h>
#include <AgcmUIControl/AgcmUIControl.h>
#include <AgcmSkill/AgcmSkill.h>
#include <AgcmResourceLoader/AgcmResourceLoader.h>
#include <AgcmSiegeWar/AgcmSiegeWar.h>


AgcmIDBoard::AgcmIDBoard( eBoardType eType )
: AgcmBaseBoard( eType )
{
	SetPosition( NULL );
	SetApBase( NULL );
	SetIDType( TB_OTHERS );
	SetEnabled( TRUE );
	SetFontType( 0 );
	SetColor( 0 );
	SetEnableParty( 0 );
	SetParty( FALSE );
	SetEnableGuild( 0 );
	SetGuild( FALSE );

	SetText( "" );
	SetGuildText( "" );
	SetNickNameText( "" );
	SetTitleNameText( "" );
	SetCharismaText( "" );
	SetBattleText( "" );

	SetColorGuild( 0 );
	SetColorCharisma( 0 );
	SetFlag( TB_FLAG_NONE );

	m_nPrevFlag = 0;

	m_fOffsetXLeft = 0.0f;
	m_fOffsetXRight = 0.0f;

	m_nTimeLastTick = 0;
	m_nTimeTickDuration = 1000;		// 1�ʿ� �ѹ���
	m_bIsDrawName = TRUE;
}

AgcmIDBoard::~AgcmIDBoard(void)
{
}

BOOL AgcmIDBoard::Update( IN DWORD tickDiff , IN RwMatrix* CamMat )
{
	::memset(m_TBOffsetY, 0, sizeof(RwReal) * TB_OFFSET_LINE_MAX);
	if( GetBoardType() == AGCM_BOARD_ITEMID )
	{
		SetDraw( FALSE );

		if( GetEnabled() == FALSE )			return FALSE;
		
		if( GetClump() && ms_pMng->GetLastTick() - GetClump()->stUserData.calcDistanceTick < 2 )
		{
			SetDraw( TRUE );
			ms_pMng->Trans3DTo2D( &m_CameraPos , &m_ScreenPos , &m_Recipz , (const RwV3d*)m_pPosition ,
						 m_Height , m_pClump , m_Depth );
		}
	}

	return TRUE;
}

void AgcmIDBoard::Render()
{
	if( !ms_pMng->GetAgcmCharacter()->GetSelfCharacter() )
		return;

	// �׸��Ⱑ ���� ������ �׳� ����
	if( GetVisible() == FALSE ) return;
	if( GetEnabled() == FALSE ) return;

	switch( m_Type )
	{
	case AGCM_BOARD_IDBOARD:     DrawChar();	SetVisible( FALSE );	break;
	case AGCM_BOARD_ITEMID:      DrawItem();							break;
	}

}

void AgcmIDBoard::_DrawName( RwReal fPosX, RwReal fPosY, INT nAlpha , BOOL bDraw )
{
	if( GetEnabled()	== FALSE )	return;

	INT				nResult			=	0;
	DWORD			dwColor			=	GetColor();
	BOOL			bSecurityName	=	FALSE;
	BOOL			bParty			=	GetParty() && GetEnableParty() == 1 ? TRUE : FALSE;
	AgpdCharacter*	pSelfCharacter	=	ms_pMng->GetAgcmCharacter()->GetSelfCharacter();
	AgpdCharacter*	pCharacter		=	(AgpdCharacter*)m_pBase;
	if( !_IsCheckDrawByPVP( pCharacter ) ) return;
	
	// ��Ʋ �׶��忡�� �������� �̸� ������ �ٲ��ش�
	if( _IsPlayerInBattleGround( pSelfCharacter, pCharacter ) )
	{
		INT32	nType		= ms_pMng->GetAgpmFactors()->GetRace( &pCharacter->m_csFactor );
		INT32	nSelfType	= ms_pMng->GetAgpmFactors()->GetRace( &pSelfCharacter->m_csFactor );
		DWORD	dwTemp		= 0;

		switch( nType )
		{
		case AURACE_TYPE_HUMAN:			dwColor   = ms_pMng->GetBGNameColor(BATTLEGROUND_HUMAN_NAME_COLOR);			break;
		case AURACE_TYPE_ORC:			dwColor   = ms_pMng->GetBGNameColor(BATTLEGROUND_ORC_NAME_COLOR);			break;
		case AURACE_TYPE_MOONELF:		dwColor   = ms_pMng->GetBGNameColor(BATTLEGROUND_MOONELF_NAME_COLOR);		break;
		case AURACE_TYPE_DRAGONSCION:	dwColor   = ms_pMng->GetBGNameColor(BATTLEGROUND_DRAGONSCION_NAME_COLOR);   break;
		}	

		// ��Ʋ�׶���� ������ �̸��� ***** �� ������ �Ѵ�
		// ���� ������ �̸��� ���´�
		if( nType != nSelfType )
			bDraw	=	FALSE;
	}
	else
	{
		// ĳ������ �Ǵ緹�� �� �������ο� ���� ID Color ��ó��
		dwColor = _GetIDColorByPVP( pCharacter );
	}

	INT nTargetResult	=	ms_pMng->GetApmMap()->CheckRegionPerculiarity( pCharacter->m_nBindingRegionIndex	, APMMAP_PECULIARITY_GUILD_PVP );
	INT nSelfResult		=	ms_pMng->GetApmMap()->CheckRegionPerculiarity( pSelfCharacter->m_nBindingRegionIndex, APMMAP_PECULIARITY_GUILD_PVP );

	if( nTargetResult	==	APMMAP_PECULIARITY_RETURN_DISABLE_USE &&
		nSelfResult		==	APMMAP_PECULIARITY_RETURN_DISABLE_USE	)
	{
		dwColor		=	0xFFFFFFFF;
	}

	if( GetParty() && GetEnableParty() == 1 )
	{
		ms_pMng->GetAgcmFont()->DrawTextIMMode2DWorld( fPosX, fPosY, m_ScreenPos.z , m_Recipz , 
			bDraw ? GetText() :GetBattleText() , GetFontType() , 
			nAlpha, dwColor, true, true, 0xbf8b3f3f );
	}
	else
	{
		ms_pMng->GetAgcmFont()->DrawTextIMMode2DWorld( fPosX, fPosY, m_ScreenPos.z , m_Recipz , 
			bDraw ? GetText() : GetBattleText()   , GetFontType() , 
						nAlpha, dwColor, true );
	}

}

INT AgcmIDBoard::_DrawName( RwReal fPosX, RwReal* pfPosY, INT nIndex, INT nAlpha , BOOL bDraw )
{
	if( GetEnabled()	== FALSE )
		return nIndex;

	INT				nResult			=	0;
	DWORD			dwColor			=	GetColor();
	BOOL			bSecurityName	=	FALSE;
	BOOL			bParty			=	GetParty() && GetEnableParty() == 1 ? TRUE : FALSE;
	AgpdCharacter*	pSelfCharacter	=	ms_pMng->GetAgcmCharacter()->GetSelfCharacter();
	AgpdCharacter*	pCharacter		=	(AgpdCharacter*)m_pBase;
	//if( !_IsCheckDrawByPVP( pCharacter ) )
	//	return nIndex+1;

	// ��Ʋ �׶��忡�� �������� �̸� ������ �ٲ��ش�
	if( _IsPlayerInBattleGround( pSelfCharacter, pCharacter ) )
	{
		INT32	nType		= ms_pMng->GetAgpmFactors()->GetRace( &pCharacter->m_csFactor );
		INT32	nSelfType	= ms_pMng->GetAgpmFactors()->GetRace( &pSelfCharacter->m_csFactor );
		DWORD	dwTemp		= 0;

		switch( nType )
		{
		case AURACE_TYPE_HUMAN:			dwColor   = ms_pMng->GetBGNameColor(BATTLEGROUND_HUMAN_NAME_COLOR);			break;
		case AURACE_TYPE_ORC:			dwColor   = ms_pMng->GetBGNameColor(BATTLEGROUND_ORC_NAME_COLOR);			break;
		case AURACE_TYPE_MOONELF:		dwColor   = ms_pMng->GetBGNameColor(BATTLEGROUND_MOONELF_NAME_COLOR);		break;
		case AURACE_TYPE_DRAGONSCION:	dwColor   = ms_pMng->GetBGNameColor(BATTLEGROUND_DRAGONSCION_NAME_COLOR);   break;
		}	

		// ��Ʋ�׶���� ������ �̸��� ***** �� ������ �Ѵ�
		// ���� ������ �̸��� ���´�
		if( nType != nSelfType )
			bDraw	=	FALSE;
	}
	else
	{
		// ĳ������ �Ǵ緹�� �� �������ο� ���� ID Color ��ó��
		dwColor = _GetIDColorByPVP( pCharacter );
	}

	INT nTargetResult	=	ms_pMng->GetApmMap()->CheckRegionPerculiarity( pCharacter->m_nBindingRegionIndex	, APMMAP_PECULIARITY_GUILD_PVP );
	INT nSelfResult		=	ms_pMng->GetApmMap()->CheckRegionPerculiarity( pSelfCharacter->m_nBindingRegionIndex, APMMAP_PECULIARITY_GUILD_PVP );

	if( nTargetResult	==	APMMAP_PECULIARITY_RETURN_DISABLE_USE &&
		nSelfResult		==	APMMAP_PECULIARITY_RETURN_DISABLE_USE	)
	{
		dwColor		=	0xFFFFFFFF;
	}

	if( GetParty() && GetEnableParty() == 1 )
	{
		ms_pMng->GetAgcmFont()->DrawTextIMMode2DWorld( fPosX, pfPosY[nIndex], m_ScreenPos.z , m_Recipz , 
			bDraw ? GetText() :GetBattleText() , GetFontType() , 
			nAlpha, dwColor, true, true, 0xbf8b3f3f );
	}
	else
	{
		ms_pMng->GetAgcmFont()->DrawTextIMMode2DWorld( fPosX, pfPosY[nIndex], m_ScreenPos.z , m_Recipz , 
			bDraw ? GetText() : GetBattleText()   , GetFontType() , 
			nAlpha, dwColor, true );
	}

	return nIndex+1;
}

void AgcmIDBoard::_DrawTitleName( RwReal fPosX, RwReal fPosY, INT nAlpha )
{
	if( GetTitle()	== FALSE )	return;

	AgpdCharacter*	pSelfCharacter	=	ms_pMng->GetAgcmCharacter()->GetSelfCharacter();
	AgpdCharacter*	pCharacter		=	(AgpdCharacter*)m_pBase;
	
	if( pSelfCharacter != pCharacter )
	{
		// ��Ʋ �׶��忡�� �������� �̸� ������ �ٲ��ش�
		if( ms_pMng->GetAgpmBattleGround()->IsInBattleGround( pSelfCharacter ) && ms_pMng->GetAgpmCharacter()->IsPC( pCharacter ) )
		{
			return;
		}
	}

	DWORD dwColor =	GetColorTitleName();
	char* pTitleName = GetTitleNameText();
	if( !pTitleName || strlen( pTitleName ) <= 0 ) return;

	//fPosX = GetOffsetTitle().x;
	float fStartX = m_ScreenPos.x + GetOffsetTitle().x;
	fPosY += GetOffset().y - 6.0f;

	//ms_pMng->GetAgcmFont()->DrawTextIMMode2DWorld( fPosX, fPosY, m_ScreenPos.z, m_Recipz, pTitleName, GetFontType(), nAlpha, dwColor, true );
	ms_pMng->GetAgcmFont()->DrawTextIMMode2DWorld( fStartX, fPosY, m_ScreenPos.z, m_Recipz, pTitleName, GetFontType(), nAlpha, dwColor, true );
}

INT AgcmIDBoard::_DrawTitleName( RwReal fPosX, RwReal* pfPosY, INT nIndex, INT nAlpha )
{
	if( GetTitle()	== FALSE )
		return nIndex;

	AgpdCharacter*	pSelfCharacter	=	ms_pMng->GetAgcmCharacter()->GetSelfCharacter();
	AgpdCharacter*	pCharacter		=	(AgpdCharacter*)m_pBase;

	if( pSelfCharacter != pCharacter )
	{
		// ��Ʋ �׶��忡�� �������� �̸� ������ �ٲ��ش�
		if( ms_pMng->GetAgpmBattleGround()->IsInBattleGround( pSelfCharacter ) && ms_pMng->GetAgpmCharacter()->IsPC( pCharacter ) )
		{
			return nIndex;
		}
	}

	DWORD dwColor =	GetColorTitleName();
	char* pTitleName = GetTitleNameText();
	if( !pTitleName || strlen( pTitleName ) <= 0 ) 
		return nIndex;

	//fPosX = GetOffsetTitle().x;
	float fStartX = m_ScreenPos.x + GetOffsetTitle().x;
	//fPosY += GetOffset().y - 6.0f;

	//ms_pMng->GetAgcmFont()->DrawTextIMMode2DWorld( fPosX, fPosY, m_ScreenPos.z, m_Recipz, pTitleName, GetFontType(), nAlpha, dwColor, true );
	ms_pMng->GetAgcmFont()->DrawTextIMMode2DWorld( fStartX, pfPosY[nIndex], m_ScreenPos.z, m_Recipz, pTitleName, GetFontType(), nAlpha, dwColor, true );

	return nIndex+1;
}

void AgcmIDBoard::_DrawNameEx( RwReal fPosX, RwReal fPosY, INT nAlpha )
{
	
	if( !ms_pMng->GetAgpmCharacter()->IsPC( (AgpdCharacter*)GetApBase() ) )		return;
	if( !strlen( ((AgpdCharacter*)GetApBase())->m_szID ) )						return;

	AgpdCharacter* pcsSelfCharacter = ms_pMng->GetAgcmCharacter()->GetSelfCharacter();
	if( !pcsSelfCharacter )		return;

	if( ms_pMng->GetAgpmBattleGround()->IsInBattleGround( pcsSelfCharacter ) )    return;

	int  nAddPos = 0;
	char szAllName[128] = { 0, };

	if( GetNickNameText()[0] )
	{
		if( nAddPos )
		{
			strcpy( szAllName + nAddPos, "   " );
			nAddPos += strlen( "   " );
		}

		strcpy( szAllName + nAddPos , GetNickNameText() );
	}

	if( szAllName[0] )
	{
		fPosY += ( GetOffset().y - 34.f );
		float fStartX = (float)ms_pMng->GetAgcmFont()->GetTextExtent( GetFontType() , szAllName, strlen( szAllName ) ) * 0.5f;
		ms_pMng->GetAgcmFont()->DrawTextIMMode2DWorld( m_ScreenPos.x - fStartX , fPosY , m_ScreenPos.z , m_Recipz , szAllName, GetFontType() , nAlpha , GetColorNickName() , true );
	}
}

INT AgcmIDBoard::_DrawNameEx( RwReal fPosX, RwReal* pfPosY, INT nIndex, INT nAlpha )
{

	if( !ms_pMng->GetAgpmCharacter()->IsPC( (AgpdCharacter*)GetApBase() ) )
		return nIndex;
	if( !strlen( ((AgpdCharacter*)GetApBase())->m_szID ) )
		return nIndex;

	AgpdCharacter* pcsSelfCharacter = ms_pMng->GetAgcmCharacter()->GetSelfCharacter();
	if( !pcsSelfCharacter )
		return nIndex;

	if( ms_pMng->GetAgpmBattleGround()->IsInBattleGround( pcsSelfCharacter ) )
		return nIndex;

	int  nAddPos = 0;
	char szAllName[128] = { 0, };

	if( GetNickNameText()[0] )
	{
		if( nAddPos )
		{
			strcpy( szAllName + nAddPos, "   " );
			nAddPos += strlen( "   " );
		}

		strcpy( szAllName + nAddPos , GetNickNameText() );
	}

	if( szAllName[0] )
	{
		//fPosY += ( GetOffset().y - 34.f );
		float fStartX = (float)ms_pMng->GetAgcmFont()->GetTextExtent( GetFontType() , szAllName, strlen( szAllName ) ) * 0.5f;
		ms_pMng->GetAgcmFont()->DrawTextIMMode2DWorld( m_ScreenPos.x - fStartX , pfPosY[nIndex] , m_ScreenPos.z , m_Recipz , szAllName, GetFontType() , nAlpha , GetColorNickName() , true );
	}

	return nIndex+1;
}

void AgcmIDBoard::_DrawGoMark( RwReal fPosX, RwReal fPosY, INT nAlpha )
{
	if( !ms_pMng->GetImgGoMark() )                                                      return;
	if( !ms_pMng->GetAgpmCharacter()->IsStatusGo( (AgpdCharacter*)GetApBase() ) )		return;

	RwV2d vImageSize	= ms_pMng->GetTextureSize( ms_pMng->GetImgGoMark() );
	RwV2d vOffset		= { -46 * .5f, GetOffset().y - ( 10.f * 0.5f ) - 11.f };	//Real Image (49, 19 )
	//RwV2d vOffset		= { 0.f, 0.f };
	g_pEngine->DrawIm2DInWorld( ms_pMng->GetImgGoMark() , &m_CameraPos, &m_ScreenPos, m_Recipz , &vOffset, vImageSize.x, vImageSize.y );
}

VOID AgcmIDBoard::_DrawFlag( VOID )
{
	if( !ms_pMng ) return;

	AgpmGuild* ppmGuild = ( AgpmGuild* )ms_pMng->GetModule( "AgpmGuild" );
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )GetApBase();
	if( !ppdCharacter || !ppmGuild ) return;

	EventStatusFlag	unBitFlag;
	memset(&unBitFlag, 0, sizeof(unBitFlag));

	unBitFlag.BitFlag = ppdCharacter->m_unEventStatusFlag;
	m_nPrevFlag = unBitFlag.NationFlag;

	void* pTexture = ms_pMng->GetFlagTexture( unBitFlag.NationFlag );
	if( !pTexture ) return;

	// ��尡 �ֳ�..
	float fGuildMarkOffset = 0.0f;
	BOOL bHaveGuild = ppmGuild->GetGuild( ppdCharacter ) ? TRUE : FALSE;
	if( bHaveGuild && ppmGuild )
	{
		if( ppmGuild->GetGuildMarkTemplate( AGPMGUILD_ID_MARK_BOTTOM, ppdCharacter ) )
		{
			for( int nIndex = AGPMGUILD_ID_MARK_BOTTOM; nIndex <= AGPMGUILD_ID_MARK_SYMBOL; nIndex++ )
			{
				fGuildMarkOffset = 0.0f;
				RwTexture* pTexture = ( RwTexture* )ppmGuild->GetGuildMarkTexture( nIndex, ppdCharacter, true );
				if( pTexture )
				{
					fGuildMarkOffset = ms_pMng->GetTextureSize( pTexture ).y;
					if( fGuildMarkOffset > 0.0f )
					{
						bHaveGuild = TRUE;
					}
				}
			}
		}
		else
		{
			// ��帶ũ�� ������ ��ũ�ε��� ��ũ�ε帶ũ��ŭ �������� ����
			if( ms_pMng->GetAgpmCharacter()->IsArchlord( ppdCharacter ) )
			{
				RwTexture* pTexture = ms_pMng->GetAgcmUIcontrol()->m_pArchlordGuildMark;
				if( pTexture )
				{
					fGuildMarkOffset = ms_pMng->GetTextureSize( pTexture ).y;
				}
			}
		}
	}

	BOOL bHaveCharisma = FALSE;
	if( ( INT )ms_pMng->GetTextureVec().size() > GetCharismaStep() && GetCharismaStep() >= 0 )
	{
		RwTexture* pTextureCharisma = ms_pMng->GetTextureVec()[ GetCharismaStep() ];
		if( pTextureCharisma )
		{
			bHaveCharisma = TRUE;
		}
	}

	RwV2d vImageSize = ms_pMng->GetTextureSize( ( RwTexture* )pTexture );

	float fOffsetY = ms_pMng->GetFlagOffsetY( bHaveGuild, bHaveCharisma );
	RwV2d vOffset = { GetOffsetXLeft() + (-vImageSize.x) - 4.0f, GetOffset().y - ( fGuildMarkOffset + 1.0f ) + fOffsetY };

	g_pEngine->DrawIm2DInWorld( ( RwTexture* )pTexture , &m_CameraPos, &m_ScreenPos, m_Recipz , &vOffset, vImageSize.x, vImageSize.y );
}

VOID AgcmIDBoard::_DrawGuildCompetition( VOID )
{
	if( !ms_pMng ) return;

	// ��Ʋ�׶��忡���� ������״�.
	AgpmBattleGround* ppmBattleGround = ( AgpmBattleGround* )g_pEngine->GetModule( "AgpmBattleGround" );
	if( !ppmBattleGround ) return;
	if( ppmBattleGround->IsInBattleGround( ( AgpdCharacter* )GetApBase() ) ) return;

	AgpdGuildADChar* ppdAttachedGuild = ms_pMng->GetAgpmGuild()->GetADCharacter( ( AgpdCharacter* )GetApBase() );

	// �������� ���� �̹��� ���
	if( ppdAttachedGuild )
	{
		int nImgIndex = -1;
		switch( ppdAttachedGuild->m_lBRRanking )		//  TextBoard.ini ���Ͽ� ���ǵ� �ε����� ��Ī
		{
		case WINNER_GUILD_1STPLACE :	nImgIndex = 1;	break;
		case WINNER_GUILD_2NDPLACE :	nImgIndex = 4;	break;
		}

		if( nImgIndex >= 0 )
		{
			RwTexture* pTexture = ms_pMng->GetImgEtc( nImgIndex );
			if( pTexture )
			{
				RwV2d vImageSize	= ms_pMng->GetTextureSize( pTexture );
				RwV2d vOffset		= { -vImageSize.x * .5f, GetOffset().y - vImageSize.y - 8.0f - 20.f };
				g_pEngine->DrawIm2DInWorld( pTexture , &GetCameraPos(), &GetScreenPos() , GetRecipz() , &vOffset, vImageSize.x, vImageSize.y );
			}
		}
	}
}

void AgcmIDBoard::_DrawGuild( RwReal fPosX, RwReal fPosY, INT nAlpha )
{
	INT		nResult		=	0;

	if( GetEnableGuild() == FALSE )		return;

	if( ms_pMng->GetAgcmUIcontrol() == NULL )			return;
	if( GetGuild()	== FALSE || GetEnableGuild()	!= 1 )	return;

	AgpdCharacter*	pcsSelfCharacter	= ms_pMng->GetAgcmCharacter()->GetSelfCharacter();
	if( pcsSelfCharacter == NULL )	return;

	if( ms_pMng->GetAgpmBattleGround()->IsInBattleGround( pcsSelfCharacter ) )	return;

	ms_pMng->CheckPeculiarity( pcsSelfCharacter , nResult );

	AgpdSiegeWar*	pcsSiegeWar		= ms_pMng->GetAgpmSiegeWar()->GetArchlordCastle();
	if( pcsSiegeWar == NULL )		return;

	AgpdGuildADChar* pcsAttachedSelfGuild	= ms_pMng->GetAgpmGuild()->GetADCharacter( pcsSelfCharacter );
	AgpdGuildADChar* pcsAttachedGuild		= ms_pMng->GetAgpmGuild()->GetADCharacter( (AgpdCharacter*)GetApBase() );

	fPosX += GetOffsetGuild().x;
	fPosY += GetOffsetGuild().y;
	
	ms_pMng->GetAgcmFont()->DrawTextIMMode2DWorld( fPosX , fPosY , GetScreenPos().z , GetRecipz() , GetGuildText() , GetFontType() , nAlpha , GetColorGuild() , true );

	AgpdGuild* pcsSelfGuild	= ms_pMng->GetAgpmGuild()->GetGuild( pcsAttachedSelfGuild->m_szGuildID );

	if( pcsSelfGuild && ms_pMng->GetAgpmGuild()->IsJointGuild( pcsSelfGuild, pcsAttachedGuild->m_szGuildID ) )
	{
		JointVector& vecJoint = *pcsSelfGuild->m_csRelation.m_pJointVector;
		int nJointMax = vecJoint.size();
		int nJointIndex = 1;
		for( JointIter Itr = vecJoint.begin(); Itr != vecJoint.end(); ++Itr, ++nJointIndex )
		{
			if( pcsSiegeWar->m_strOwnerGuildName.Compare( Itr->m_szGuildID ) == COMPARE_EQUAL && ms_pMng->GetAgcmUIcontrol()->m_pArchlordGuildMark )
				ms_pMng->DrawGuildImage( this , fPosX,  ms_pMng->GetAgcmUIcontrol()->m_pArchlordGuildMark, 0xffffffff, .5f, nJointMax, nJointIndex );
			else
			{
				int nJointGuildMarkTID		= ms_pMng->GetAgpmGuild()->GetJointGuildMarkID( Itr->m_szGuildID );
				int nJointGuildMarkColor	= ms_pMng->GetAgpmGuild()->GetJointGuildMarkColor( Itr->m_szGuildID );
				if( nJointGuildMarkTID )
				{
					for( int nIndex = AGPMGUILD_ID_MARK_BOTTOM; nIndex <= AGPMGUILD_ID_MARK_SYMBOL; ++nIndex )
					{
						int nColor = ms_pMng->GetAgpmGuild()->GetGuildMarkColor( nIndex , nJointGuildMarkColor );
						RwTexture* pTexture = (RwTexture*)( ms_pMng->GetAgpmGuild()->GetGuildMarkTexture( nIndex, nJointGuildMarkTID, true) );
						ms_pMng->DrawGuildImage( this , fPosX, pTexture, nColor, .5f, nJointMax, nJointIndex );
					}

					ms_pMng->DrawGuildImage( this, fPosX, ms_pMng->GetImgEtc(0) , 0xffffffff, .5f, nJointMax, nJointIndex );
				}
			}
		}
	}
	else
	{
		if( pcsSiegeWar->m_strOwnerGuildName.Compare(pcsAttachedGuild->m_szGuildID) == COMPARE_EQUAL && ms_pMng->GetAgcmUIcontrol()->m_pArchlordGuildMark )
			ms_pMng->DrawGuildImage( this ,  fPosX, ms_pMng->GetAgcmUIcontrol()->m_pArchlordGuildMark );
		else
		{
			if( ms_pMng->GetAgpmGuild()->GetGuildMarkTemplate( AGPMGUILD_ID_MARK_BOTTOM, (AgpdCharacter*)GetApBase() ) )
			{
				for( int nIndex = AGPMGUILD_ID_MARK_BOTTOM; nIndex <= AGPMGUILD_ID_MARK_SYMBOL; nIndex++ )
				{
					int nColor = ms_pMng->GetAgpmGuild()->GetGuildMarkColor( nIndex, (AgpdCharacter*)GetApBase() );
					RwTexture* pTexture = (RwTexture*)ms_pMng->GetAgpmGuild()->GetGuildMarkTexture( nIndex, (AgpdCharacter*)GetApBase(), true );
					ms_pMng->DrawGuildImage( this, fPosX, pTexture, nColor );
				}

				ms_pMng->DrawGuildImage( this, fPosX, ms_pMng->GetImgEtc(0) );
			}
		}
	}

	//. ����/������ ������ �����ΰ�?
	int iJoinSiegeWar = ms_pMng->GetAgcmSiegeWar()->GetSiegeWarGuildType( (AgpdCharacter*)GetApBase() );
	if( iJoinSiegeWar > -1 )
	{
		//��帶ũ�� �׷Ⱦ�����..
		RwTexture* pTexture = (RwTexture*)ms_pMng->GetAgpmGuild()->GetGuildMarkTexture( AGPMGUILD_ID_MARK_BOTTOM, (AgpdCharacter*)GetApBase(), true );
		ms_pMng->DrawGuildImage( this, fPosX, ms_pMng->GetImgEtc( iJoinSiegeWar+1 ) , 0xffffffff, 1.f, 0, 0, pTexture ? TRUE : FALSE );
	}
}

INT AgcmIDBoard::_DrawGuild( RwReal fPosX, RwReal* pfPosY, INT nIndex, INT nAlpha )
{
	INT		nResult		=	0;

	if( GetEnableGuild() == FALSE )
		return nIndex;

	if( ms_pMng->GetAgcmUIcontrol() == NULL )
		return nIndex;

	if( GetGuild()	== FALSE || GetEnableGuild()	!= 1 )
		return nIndex;

	AgpdCharacter*	pcsSelfCharacter	= ms_pMng->GetAgcmCharacter()->GetSelfCharacter();
	if( pcsSelfCharacter == NULL )
		return nIndex;

	if( ms_pMng->GetAgpmBattleGround()->IsInBattleGround( pcsSelfCharacter ) )
		return nIndex;

	ms_pMng->CheckPeculiarity( pcsSelfCharacter , nResult );

	AgpdSiegeWar*	pcsSiegeWar		= ms_pMng->GetAgpmSiegeWar()->GetArchlordCastle();
	if( pcsSiegeWar == NULL )
		return nIndex;

	AgpdGuildADChar* pcsAttachedSelfGuild	= ms_pMng->GetAgpmGuild()->GetADCharacter( pcsSelfCharacter );
	AgpdGuildADChar* pcsAttachedGuild		= ms_pMng->GetAgpmGuild()->GetADCharacter( (AgpdCharacter*)GetApBase() );

	fPosX += GetOffsetGuild().x;
//	fPosY += GetOffsetGuild().y;

	ms_pMng->GetAgcmFont()->DrawTextIMMode2DWorld( fPosX , pfPosY[nIndex] , GetScreenPos().z , GetRecipz() , GetGuildText() , GetFontType() , nAlpha , GetColorGuild() , true );

	AgpdGuild* pcsSelfGuild	= ms_pMng->GetAgpmGuild()->GetGuild( pcsAttachedSelfGuild->m_szGuildID );

	if( pcsSelfGuild && ms_pMng->GetAgpmGuild()->IsJointGuild( pcsSelfGuild, pcsAttachedGuild->m_szGuildID ) )
	{
		JointVector& vecJoint = *pcsSelfGuild->m_csRelation.m_pJointVector;
		int nJointMax = vecJoint.size();
		int nJointIndex = 1;
		for( JointIter Itr = vecJoint.begin(); Itr != vecJoint.end(); ++Itr, ++nJointIndex )
		{
			if( pcsSiegeWar->m_strOwnerGuildName.Compare( Itr->m_szGuildID ) == COMPARE_EQUAL && ms_pMng->GetAgcmUIcontrol()->m_pArchlordGuildMark )
				ms_pMng->DrawGuildImage( this , fPosX, /*pfPosY[nIndex], */ ms_pMng->GetAgcmUIcontrol()->m_pArchlordGuildMark, 0xffffffff, .5f, nJointMax, nJointIndex );
			else
			{
				int nJointGuildMarkTID		= ms_pMng->GetAgpmGuild()->GetJointGuildMarkID( Itr->m_szGuildID );
				int nJointGuildMarkColor	= ms_pMng->GetAgpmGuild()->GetJointGuildMarkColor( Itr->m_szGuildID );
				if( nJointGuildMarkTID )
				{
					for( int i = AGPMGUILD_ID_MARK_BOTTOM; i <= AGPMGUILD_ID_MARK_SYMBOL; ++i )
					{
						int nColor = ms_pMng->GetAgpmGuild()->GetGuildMarkColor( i , nJointGuildMarkColor );
						RwTexture* pTexture = (RwTexture*)( ms_pMng->GetAgpmGuild()->GetGuildMarkTexture( i, nJointGuildMarkTID, true) );
						ms_pMng->DrawGuildImage( this , fPosX, /*pfPosY[nIndex], */pTexture, nColor, .5f, nJointMax, nJointIndex );
					}

					ms_pMng->DrawGuildImage( this, fPosX, /*pfPosY[nIndex], */ms_pMng->GetImgEtc(0) , 0xffffffff, .5f, nJointMax, nJointIndex );
				}
			}
		}
	}
	else
	{
		if( pcsSiegeWar->m_strOwnerGuildName.Compare(pcsAttachedGuild->m_szGuildID) == COMPARE_EQUAL && ms_pMng->GetAgcmUIcontrol()->m_pArchlordGuildMark )
			ms_pMng->DrawGuildImage( this ,  fPosX, /*pfPosY[nIndex], */ms_pMng->GetAgcmUIcontrol()->m_pArchlordGuildMark );
		else
		{
			if( ms_pMng->GetAgpmGuild()->GetGuildMarkTemplate( AGPMGUILD_ID_MARK_BOTTOM, (AgpdCharacter*)GetApBase() ) )
			{
				for( int i = AGPMGUILD_ID_MARK_BOTTOM; i <= AGPMGUILD_ID_MARK_SYMBOL; i++ )
				{
					int nColor = ms_pMng->GetAgpmGuild()->GetGuildMarkColor( i, (AgpdCharacter*)GetApBase() );
					RwTexture* pTexture = (RwTexture*)ms_pMng->GetAgpmGuild()->GetGuildMarkTexture( i, (AgpdCharacter*)GetApBase(), true );
					ms_pMng->DrawGuildImage( this, fPosX, /*pfPosY[nIndex], */pTexture, nColor );
				}

				ms_pMng->DrawGuildImage( this, fPosX, /*pfPosY[nIndex], */ms_pMng->GetImgEtc(0) );
			}
		}
	}

	//. ����/������ ������ �����ΰ�?
	int iJoinSiegeWar = ms_pMng->GetAgcmSiegeWar()->GetSiegeWarGuildType( (AgpdCharacter*)GetApBase() );
	if( iJoinSiegeWar > -1 )
	{
		//��帶ũ�� �׷Ⱦ�����..
		RwTexture* pTexture = (RwTexture*)ms_pMng->GetAgpmGuild()->GetGuildMarkTexture( AGPMGUILD_ID_MARK_BOTTOM, (AgpdCharacter*)GetApBase(), true );
		ms_pMng->DrawGuildImage( this, fPosX, /*pfPosY[nIndex], */ms_pMng->GetImgEtc( iJoinSiegeWar+1 ) , 0xffffffff, 1.f, 0, 0, pTexture ? TRUE : FALSE );
	}

	return nIndex+1;
}


void AgcmIDBoard::_DrawMonsterAbility( RwReal fPosX, RwReal fPosY, INT nAlpha )
{
	if( !ms_pMng->GetAgpmBattleGround()->IsInBattleGround( ms_pMng->GetAgcmCharacter()->GetSelfCharacter() ) )	return;	//BattleGround�� �ƴϸ� Would you please �����ٷ�?

	AgpdCharacter*	pdCharacter = static_cast< AgpdCharacter* >(GetApBase());
	if( !pdCharacter )											   return;
	if( !ms_pMng->GetAgpmCharacter()->IsMonster( pdCharacter ) )   return;

	RwTexture* pTexture = ms_pMng->GetAgcmSkill()->GetBuffSkillSmallTexture( pdCharacter, 0, TRUE );
	if( !pTexture )			return;

	RwV2d vImageSize	= ms_pMng->GetTextureSize( pTexture );
	RwV2d vOffset		= { GetOffset().x +(-vImageSize.x) - 2.f - 4.f, GetOffset().y - 8.f };
	g_pEngine->DrawIm2DInWorld( pTexture, &m_CameraPos, &m_ScreenPos, m_Recipz, &vOffset, vImageSize.x, vImageSize.y );
}

void AgcmIDBoard::_DrawCharisma( RwReal fPosX, RwReal fPosY, INT nAlpha )
{

	AgpdCharacter*	pdCharacter = static_cast< AgpdCharacter* >(GetApBase());
	if( !pdCharacter )										 return;
	if( !ms_pMng->GetAgpmCharacter()->IsPC( pdCharacter ) )	 return;

	if( (INT)ms_pMng->GetTextureVec().size() <= GetCharismaStep() || GetCharismaStep() < 0 )   return;

	RwTexture* pTexture = ms_pMng->GetTextureVec()[ GetCharismaStep() ];
	if( !pTexture )		return;

	// ��尡 �ֳ�..
	float fGuildMarkOffset = 0.0f;
	AgpmGuild* ppmGuild = ( AgpmGuild* )ms_pMng->GetModule( "AgpmGuild" );
	BOOL bHaveGuild = ppmGuild->GetGuild( pdCharacter ) ? TRUE : FALSE;
	if( bHaveGuild && ppmGuild )
	{
		AgpdGuildMarkTemplate* pMarkTemplateBottom = ppmGuild->GetGuildMarkTemplate( AGPMGUILD_ID_MARK_BOTTOM, pdCharacter );
		AgpdGuildMarkTemplate* pMarkTemplatePattern = ppmGuild->GetGuildMarkTemplate( AGPMGUILD_ID_MARK_PATTERN, pdCharacter );
		AgpdGuildMarkTemplate* pMarkTemplateSimbol = ppmGuild->GetGuildMarkTemplate( AGPMGUILD_ID_MARK_SYMBOL, pdCharacter );

		if( pMarkTemplateBottom || pMarkTemplatePattern || pMarkTemplateSimbol )
		{
			for( int nIndex = AGPMGUILD_ID_MARK_BOTTOM; nIndex <= AGPMGUILD_ID_MARK_SYMBOL; nIndex++ )
			{	
				RwTexture* pTexture = ( RwTexture* )ppmGuild->GetGuildMarkTexture( nIndex, pdCharacter, true );
				if( pTexture )
				{
					float fTextureHeight = ms_pMng->GetTextureSize( pTexture ).y;
					fGuildMarkOffset = fGuildMarkOffset < fTextureHeight ? fTextureHeight : fGuildMarkOffset;
				}
			}
		}
		else
		{
			// ��帶ũ�� ������ ��ũ�ε��� ��ũ�ε帶ũ��ŭ �������� ����
			if( ms_pMng->GetAgpmCharacter()->IsArchlord( pdCharacter ) )
			{
				RwTexture* pTexture = ms_pMng->GetAgcmUIcontrol()->m_pArchlordGuildMark;
				if( pTexture )
				{
					fGuildMarkOffset = ms_pMng->GetTextureSize( pTexture ).y;
				}
			}
		}
	}

	RwV2d vImageSize	= ms_pMng->GetTextureSize( pTexture );
	//RwV2d vPos		= { pInfo->screen_pos.x - (vImageSize.x * .5f), fPosY + ( pInfo->screen_pos.y - 20.f ) };
	RwV2d vOffset		= { GetOffsetXLeft() + (-vImageSize.x) - 4.0f, GetOffset().y - ( fGuildMarkOffset + 1.0f ) };

	g_pEngine->DrawIm2DInWorld( pTexture, &m_CameraPos, &m_ScreenPos, m_Recipz , &vOffset, vImageSize.x, vImageSize.y );
}

void AgcmIDBoard::DrawItem()
{

	FLOAT stX    = 0;
	FLOAT stY    = 0;

	BOOL bItemNameDraw = TRUE;
	AgcmUIOption* pOption = (AgcmUIOption *)ms_pMng->GetModule("AgcmUIOption");
	if( pOption  )
		bItemNameDraw = pOption->m_bVisibleViewItem;

	if( bItemNameDraw )
	{
		if( GetDraw() )
		{
			stX = GetScreenPos().x + GetOffset().x;
			stY = GetScreenPos().y + GetOffset().y;

			ms_pMng->GetAgcmFont()->FontDrawStart( GetFontType() );
			ms_pMng->GetAgcmFont()->DrawTextIMMode2DWorld( stX , stY , GetScreenPos().z, m_Recipz , GetText() , GetFontType() , 255 , GetColor() ,true );
			ms_pMng->GetAgcmFont()->FontDrawEnd();
		}
	}

}

void AgcmIDBoard::DrawChar()
{
	// ĳ���� ������ �н�
	AgpdCharacter* pdCharacter = ( AgpdCharacter* )m_pBase;
	if( !pdCharacter ) return;

	// ���� ID �� ������ �н�
	if( !ms_pMng->GetDrawID() ) return;

	// �׷��� �� ĳ���Ͱ� �ƴϸ� �н�
	if( !ms_pMng->IsRender( pdCharacter ) )	return;

	if( _IsInGuildStadium( pdCharacter ) )
	{
		// ������忡���� ID, ����, ��帶ũ�� ������ �Ѵܴ�..
		_Draw_InGuildStadium( pdCharacter );
	}
	else
	{
		// �⺻ �׸���
		_Draw_Normal( pdCharacter );
	}
}

VOID AgcmIDBoard::ReCalulateOffsetX( VOID )
{
	// IDBoard �� ��� ��ġ�ϴ� ��Ī, ����, Ÿ��Ʋ��, ĳ���͸��߿��� ���� �� ���ڿ��� �������� ���� OffsetX ���� ���Ѵ�.
	float fLengthName = 0.0f;
	int nNameLength = ( int )strlen( m_Text );
	if( nNameLength > 0 && nNameLength <= TB_ID_MAX_CHAR )
	{
		fLengthName = ( float )ms_pMng->GetAgcmFont()->GetTextExtent( GetFontType(), m_Text, nNameLength );
	}

	float fLengthNickName = 0.0f;
	int nNickNameLength = ( int )strlen( m_strNickName );
	if( nNickNameLength > 0 && nNickNameLength <= TB_ID_MAX_CHAR )
	{
		fLengthNickName = ( float )ms_pMng->GetAgcmFont()->GetTextExtent( GetFontType(), m_strNickName, nNickNameLength );
	}

	float fLengthGuildName = 0.0f;
	int nGuildNameLength = ( int )strlen( m_strGuild );
	if( nGuildNameLength > 0 && nGuildNameLength <= TB_ID_MAX_CHAR )
	{
		fLengthGuildName = ( float )ms_pMng->GetAgcmFont()->GetTextExtent( GetFontType(), m_strGuild, nGuildNameLength );
	}

	float fLengthTitleName = 0.0f;
	int nTitleNameLength = ( int )strlen( m_strTitleName );
	if( nTitleNameLength > 0 && nTitleNameLength <= TB_ID_MAX_CHAR )
	{
		fLengthTitleName = ( float )ms_pMng->GetAgcmFont()->GetTextExtent( GetFontType(), m_strTitleName, nTitleNameLength );
	}

	// ���� ���� Ÿ��Ʋ ���������� ����
	GetOffsetTitle().x = fLengthTitleName * -0.5f;

	float fLongestLength = fLengthNickName > fLengthGuildName ? fLengthNickName : fLengthGuildName;
	fLongestLength = fLongestLength > fLengthTitleName ? fLongestLength : fLengthTitleName;
	fLongestLength = fLongestLength > fLengthName ? fLongestLength : fLengthName;

	// ���� �� ���ڿ��� ���̸� �������� ���ʿ� ���� �����ܵ��� OffsetX �� ����
	m_fOffsetXLeft = fLongestLength * -0.5f;

	// ���� �� ���ڿ��� ���̸� �������� �����ʿ� ���� �����ܵ��� OffsetX �� ����
	m_fOffsetXRight = fLongestLength * 0.5f;
}

void AgcmIDBoard::_Draw_Normal( AgpdCharacter* ppdCharacter )
{
	if( !ppdCharacter ) return;
	if( !ms_pMng ) return;

	AgpdCharacter* ppdSelfCharacter = ms_pMng->GetAgcmCharacter()->GetSelfCharacter();
	if( !ppdSelfCharacter ) return;

	RwV2d   Offset = { 0.0f , 0.0f };
	FLOAT   stX , stY;
	
	RwTexture* pTexStatus = ms_pMng->GetStateTexture( ppdCharacter );
	if( pTexStatus )
	{
		Offset.x = -RwRasterGetWidth( RwTextureGetRaster( pTexStatus ) ) * 0.5f;
		Offset.y = GetOffset().y - RwRasterGetHeight( RwTextureGetRaster( pTexStatus ) ) - 8.0f;
		ms_pMng->RenderTexture( pTexStatus, m_CameraPos , m_ScreenPos , m_Recipz , &Offset );
	}

	/* Priority for header, high-to-low:
	 * - Team flags.
	 * - Best of each class.
	 * - Castle holder. */
	RwTexture* pTexHeader = ms_pMng->GetTeamFlagTexture(ppdCharacter->m_ulSpecialStatus);
	if (!pTexHeader) {
		pTexHeader = ms_pMng->GetBestOfClassTexture(ppdCharacter);
		if (!pTexHeader)
			pTexHeader = ms_pMng->GetCastleTexture(ppdCharacter->m_ulSpecialStatus);
	}
	if (pTexHeader) {
		float fOffsetY = ppdCharacter->m_szNickName[0] ? 46.0f : 34.0f;
		Offset.x = (float)-RwRasterGetWidth(RwTextureGetRaster(pTexHeader)) * 0.5f;
		Offset.y = GetOffset().y - RwRasterGetHeight(RwTextureGetRaster(pTexHeader)) - fOffsetY;
		ms_pMng->RenderTexture(pTexHeader, m_CameraPos, m_ScreenPos, m_Recipz, &Offset);
	}
	INT		nResult		=	0;
	INT		nIndex		=	0;
	BOOL	bDraw		=	TRUE;
	INT32	nType		= ms_pMng->GetAgpmFactors()->GetRace( &ppdCharacter->m_csFactor );
	INT32	nSelf		= ms_pMng->GetAgpmFactors()->GetRace( &ppdSelfCharacter->m_csFactor );
	
	ms_pMng->CheckPeculiarity( ppdSelfCharacter , nResult );
	switch( nResult )
	{
	case APMMAP_PECULIARITY_RETURN_DISABLE_USE:	
		bDraw	=	FALSE;
		break;

	case APMMAP_PECULIARITY_RETURN_ENABLE_USE_UNION:
		if( (AgpdCharacter*)m_pBase != ppdSelfCharacter )
		{
			if( nType != nSelf )   
				bDraw   = FALSE;
		}
		break;
	}

	stX = ( FLOAT ) ( (INT) ( GetScreenPos().x + GetOffset().x ) );
	stY = ( FLOAT ) ( (INT) ( GetScreenPos().y + GetOffset().y ) );

	for(int i=TB_OFFSET_LINE_FIRST ; i<TB_OFFSET_LINE_MAX ; ++i)
	{
		if(TB_OFFSET_LINE_FIRST != i)
			m_TBOffsetY[i] = stY + GetOffset().y - (TB_ID_INTERVAL * (i - 1) + 6);
		else
			m_TBOffsetY[i] = stY;
	}

	ms_pMng->GetAgcmFont()->FontDrawStart( GetFontType() );

	nIndex =	_DrawName          			( stX, m_TBOffsetY, nIndex, GetAlpha() , bDraw );
	nIndex =	_DrawTitleName     			( stX, m_TBOffsetY, nIndex, GetAlpha() );
				_DrawGoMark        			( stX, stY, GetAlpha() );

	if( bDraw )
	{
		nIndex =_DrawGuild					( stX, m_TBOffsetY, nIndex, GetAlpha() );
				_DrawGuildCompetition		();
		nIndex =_DrawNameEx        			( stX, m_TBOffsetY, nIndex, GetAlpha() );
				_DrawCharisma      			( stX, stY, GetAlpha() );
				_DrawMonsterAbility			( stX, stY, GetAlpha() );
				_DrawFlag();
	}

	ms_pMng->GetAgcmFont()->FontDrawEnd();

	Offset.x  =  GetOffsetXRight() + 2;
	Offset.y  =  GetOffset().y - 1;

	for( INT i = 0 ; i < TB_MAX_TAG ; ++i )
	{
		if( ms_pMng->GetImgIDTag(i) && (GetFlag() & (1<<i)) )
		{
			ms_pMng->RenderTexture( ms_pMng->GetImgIDTag(i) , m_CameraPos , m_ScreenPos , m_Recipz , &Offset );
			Offset.x  += TBID_TAG_SIZE;
		}
	}
}

void AgcmIDBoard::_Draw_InGuildStadium( AgpdCharacter* ppdCharacter )
{
	if( !ms_pMng ) return;

	FLOAT   stX , stY;
	INT		nIndex		= 0;

	RwTexture* pTexFlag = ms_pMng->GetTeamFlagTexture(ppdCharacter->m_ulSpecialStatus);
	if (pTexFlag) {
		float fOffsetY = ppdCharacter->m_szNickName[0] ? 46.0f : 34.0f;
		RwV2d Offset = {
			RwRasterGetWidth(RwTextureGetRaster(pTexFlag)) * -0.5f,
			GetOffset().y - RwRasterGetHeight(RwTextureGetRaster(pTexFlag)) - fOffsetY };
		ms_pMng->RenderTexture(pTexFlag, m_CameraPos, m_ScreenPos, m_Recipz, &Offset);
	}
	stX = ( FLOAT ) ( (INT) ( GetScreenPos().x + GetOffset().x ) );
	stY = ( FLOAT ) ( (INT) ( GetScreenPos().y + GetOffset().y ) );

	m_TBOffsetY[TB_OFFSET_LINE_FIRST]	= stY;
	m_TBOffsetY[TB_OFFSET_LINE_SECOND]	= stY + GetOffset().y - 6.0f;

	ms_pMng->GetAgcmFont()->FontDrawStart( GetFontType() );

	nIndex =	_DrawName	( stX, m_TBOffsetY, nIndex, GetAlpha() , TRUE );
				_DrawGoMark	( stX, stY, GetAlpha() );
	nIndex =	_DrawGuild	( stX, m_TBOffsetY, nIndex, GetAlpha() );

	ms_pMng->GetAgcmFont()->FontDrawEnd();
}

BOOL AgcmIDBoard::_IsInGuildStadium( AgpdCharacter* ppdCharacter )
{
	if( !ppdCharacter ) return FALSE;

	// �� ��� ��� �н�
	ApmMap* ppmMap = ( ApmMap* )g_pEngine->GetModule( "ApmMap" );
	if( !ppmMap ) return FALSE;

	// ������ �˼� ��� �н�
	ApmMap::RegionTemplate* pRegionTemplate = ppmMap->GetTemplate( ppdCharacter->m_nBindingRegionIndex );
	if( !pRegionTemplate ) return FALSE;

	// ������� ���� TID ����Ʈ.. ��.��.. ���� Ÿ���� �ְ��� �ߵ�ϸ� ����.. �� TID ����
	// ���߿� ��� ���Ϸ� ���� ��.. ��.��;; ������ ���� ��ġ�� �ؾ� �ϴ�..
	static int g_nStadiumTID[] = { 89, 90, 91, 92, 162 };
	static int g_nStadiumCount = 5;

	for( int nCount = 0 ; nCount < g_nStadiumCount ; nCount++ )
	{
		if( pRegionTemplate->nIndex == g_nStadiumTID[ nCount ] )
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL AgcmIDBoard::_IsPlayerInBattleGround( AgpdCharacter* ppdSelfCharacter, AgpdCharacter* ppdCharacter )
{
	if( !ppdSelfCharacter || !ppdCharacter ) return FALSE;

	AgpmBattleGround* ppmBattleGround = ( AgpmBattleGround* )g_pEngine->GetModule( "AgpmBattleGround" );
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )g_pEngine->GetModule( "AgpmCharacter" );
	if( !ppmBattleGround || !ppmCharacter ) return FALSE;

	// ����� �� �ڽ��̸� �ȵȴ�.
	if( ppdSelfCharacter == ppdCharacter ) return FALSE;

	// ���� ��Ʋ�׶��忡 ������ �������.
	if( !ppmBattleGround->IsInBattleGround( ppdSelfCharacter ) ) return FALSE;

	// ����� �÷��̾� ĳ���Ͱ� �ƴϸ� �������.
	if( !ppmCharacter->IsPC( ppdCharacter ) ) return FALSE;

	// ���� ������ �����ϸ� TRUE
	return TRUE;
}

BOOL AgcmIDBoard::_IsCheckDrawByPVP( AgpdCharacter* ppdCharacter )
{
	if( !ppdCharacter ) return FALSE;

	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )g_pEngine->GetModule( "AgpmCharacter" );
	if( !ppmCharacter ) return FALSE;

	// ������ �ƴϸ� �׳� �׸���.
	BOOL bIsFirstAttacker = ppdCharacter->m_unCriminalStatus == AGPDCHAR_CRIMINAL_STATUS_CRIMINAL_FLAGGED ? TRUE : FALSE;
	if( !bIsFirstAttacker ) return TRUE;

	// �������ΰ�� �������� �ð��� 10�� �̻��̸� �׳� �̸� ���..
	int nRemainTime = ppmCharacter->GetRemainedCriminalTime( ppdCharacter );
	if( nRemainTime >= 10000 ) return TRUE;

	// ���� �����ð��� 10�� �̸��� ��� 1�ʰ������� �����Ÿ���.
	__int64 nCurrTime = ::timeGetTime();
	if( nCurrTime - m_nTimeLastTick >= 1000 )
	{
		m_nTimeLastTick += 1000;
		m_bIsDrawName = !m_bIsDrawName;
	}

	return m_bIsDrawName;
}

DWORD AgcmIDBoard::_GetIDColorByPVP( AgpdCharacter* ppdCharacter )
{
	DWORD dwColor = GetColor();
	if( !ppdCharacter ) return dwColor;

	// ��Ʋ�׶��� �̿��� ��� ������ �Ǵ緹�� �� �����ڻ��¿� ���� ID ������ �����Ѵ�.
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )g_pEngine->GetModule( "AgpmCharacter" );
	AgcmResourceLoader* pcmResourceLoader = ( AgcmResourceLoader* )g_pEngine->GetModule( "AgcmResourceLoader" );
	if( !ppmCharacter || !pcmResourceLoader ) return dwColor;

	// �������� ����� ���� ����
// 	BOOL bIsFirstAttacker = ppdCharacter->m_unCriminalStatus == AGPDCHAR_CRIMINAL_STATUS_CRIMINAL_FLAGGED ? TRUE : FALSE;
// 	if( bIsFirstAttacker )
// 	{
// 		dwColor = pcmResourceLoader->GetColor( "IDColor_FirstAttacker" );
// 	}
// 	else
	{
		// �Ǵ緹���� ���� ��쿡 ���� ���� ����.. �����ڻ��󺸴� �켱������ �����Ƿ� ���߿� üũ�Ѵ�.
		int nCurrMurdererLevel = ppmCharacter->GetMurdererLevel( ppdCharacter );

		switch( nCurrMurdererLevel )
		{
			case AGPMCHAR_MURDERER_LEVEL1_POINT :	dwColor = pcmResourceLoader->GetColor( "IDColor_MurdererLevel1" );	break;
			case AGPMCHAR_MURDERER_LEVEL2_POINT :	dwColor = pcmResourceLoader->GetColor( "IDColor_MurdererLevel2" );	break;
			case AGPMCHAR_MURDERER_LEVEL3_POINT :	dwColor = pcmResourceLoader->GetColor( "IDColor_MurdererLevel3" );	break;
		}
	}

	BOOL bIsFirstAttacker = ppdCharacter->m_unCriminalStatus == AGPDCHAR_CRIMINAL_STATUS_CRIMINAL_FLAGGED ? TRUE : FALSE;
	if( bIsFirstAttacker )
	{
		//dwColor = pcmResourceLoader->GetColor( "IDColor_FirstAttacker" );
	}

	return dwColor;
}
