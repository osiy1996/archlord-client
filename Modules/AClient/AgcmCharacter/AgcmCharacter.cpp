#include <AgpmCharacter/AgpmCharacter.h>
#include <AgcmCharacter/AgcmCharacter.h>
#include "AgcCharacterUtil.h"
#include <ApBase/MagDebug.h>
#include <AgcmUIConsole/AgcmUIConsole.h>
#include <AgcmPreLODManager/AgcmPreLODManager.h>
#include <AgpmSiegeWar/AgpmSiegeWar.h>
#include <AgcmTargeting/AgcmTargeting.h>
#include <AgcmWater/AgcmWater.h>		//2005. 8.17 Nonstopdj. ������ ���̸� ���� AgcmWater����� ���
#include <AgcmSound/AgcmSound.h>		//. 2006. 9. 20. Nonstopdj. PreStop3DSound�� ���ֱ� ����.
#include <rplodatm.h>
#include <rtpitexd.h>
#ifdef USE_DPVS
#include "rpdpvs.h"
#endif //USE_DPVS
#include <AuMath/AuMath.h>
#include <ApMemoryTracker/ApMemoryTracker.h>
#include <AcuIMDraw/AcuObjecWire.h>
#include <AgcmExportResource/RWUtil.h>
#include <AgcmUIHotkey/AgcmUIHotkey.h>
#include <AgcmSkill/AgcmSkill.h>
#include <AuGameGuard/AuNPGameLib.h>
#include <AgcmEventEffect/AgcmEventEffect.h>
#include <AgcmGuild/AgcmGuild.h>
#include <AgcmUIItem/AgcmUIItem.h>
#include <AgcModule/AgcEngine.h>
#include <AgcmMap/AgcmMap.h>
#include <AgcmRender/AgcmRender.h>
#include <AgpmParty/AgpmParty.h>
#include <AgpmPvP/AgpmPvP.h>
#include <AgcmShadowmap/AgcmShadowmap.h>
#include <AgpmSkill/AgpmSkill.h>
#include <AgcmLODManager/AgcmLODManager.h>
#include <ApmObject/ApmObject.h>
#include <AgcmObject/AgcmObject.h>
#include <AgcmResourceLoader/AgcmResourceLoader.h>
#include <AgpmOptimizedPacket2/AgpmOptimizedPacket2.h>
#include <AgcmItem/AgcmItem.h>
#include <AgpmGrid/AgpmGrid.h>
#include <AgpmCharacter/AgpmCharacter.h>
#include <AgcmUICharacter/AgcmUICharacter.h>
#include <AgcmShadow2/AgcmShadow2.h>
#include <AgpmItem/AgpmItem.h>
#include <AgpmCasper/AgpmCasper.h>
#include <AgpmSummons/AgpmSummons.h>
#include <AgcmUIManager2/AgcmUIManager2.h>
#include <AgcmShadow/AgcmShadow.h>
#include <AgcmConnectManager/AgcmConnectManager.h>
#include <AgpmPathFind/AgpmPathFind.h>
#include <AgpmArchlord/AgpmArchlord.h>

typedef void (*OutputExportLog)(const char *);
extern OutputExportLog _outputExportLog;

RwLine	g_rwBlockingCheckLine;
BOOL	g_bBlockingCheckBlocked	= FALSE;
static AgcmEventEffect * g_pcsAgcmEventEffect = NULL;

// ������ (2005-06-08 ���� 4:37:23) : 
// �������� ���� �Ÿ� ���÷��׸� �����ؾ��Ѵ�.
#define	BOKCHUNGGUJO

//. 2005. 12. 26. Nonstopdj
//. resource file packing���θ� ���� �׽�Ʈ define�� profiling macro�߰�
UINT32	uStartTime2  = 0;
UINT32	uCurrentCheckTime2 = 0;
char	strMessage2[ 1024 ];

// ���� ���� ������ ����.
static vector< INT32 >	g_vecSiegeMonster;
static ApRWLock			g_lockSiegeMonster;

static CHAR *g_aszActionName[AGPDCHAR_MAX_ACTION_TYPE] =
{
	"NONE",
	"ATTACK",
	"ATTACK_MISS",
	"SKILL",
	"PICKUP_ITEM",
	"EVENT_TELEPORT",
	"EVENT_NPC_TRADE",
	"EVENT_MASTERY_SPECIALIZE",
	"EVENT_BANK",
	"EVENT_ITEMCONVERT",
	"EVENT_GUILD",
	"EVENT_PRODUCT",
	"EVENT_NPC_DIALOG",
	"EVENT_SKILL_MASTER",
	"EVENT_REFINERY",
	"EVENT_QUEST",
	"MOVE",
	"PRODUCT_SKILL"
};

//static INT32	g_sMoveRetryMaxCount		= 100;
static INT32	g_sMoveRetryMaxCount		= 3;
// ������ (2005-06-10 ���� 6:58:02) : 3�� ���� �ո����ΰ� ����.

static INT32	g_snMaxTwistAngle			= 50;
static FLOAT	g_fPathFindMinimumDistance	= MAP_STEPSIZE / 2.0f;

#define ENABLE_PATH_FIND_MODE

/*
const int AGCMCHAR_RIDE_FOOT_FL_ID	= 106;	//front left
const int AGCMCHAR_RIDE_FOOT_FR_ID	= 105;	//front right
const int AGCMCHAR_RIDE_FOOT_BL_ID	= 108;	//back left
const int AGCMCHAR_RIDE_FOOT_BR_ID	= 107;	//back right
*/

const int AGCMCHAR_FOOT_FL_ID		= 102;	//front left
const int AGCMCHAR_FOOT_FR_ID		= 101;	//front right
const int AGCMCHAR_FOOT_BL_ID		= 104;	//back left
const int AGCMCHAR_FOOT_BR_ID		= 103;	//back right

const int AGCMCHAR_FOOT_F_ID		= 501;	//front
const int AGCMCHAR_FOOT_B_ID		= 502;	//back

RwTextureCallBackRead AgcmCharacter::m_fnDefaultTextureReadCB = NULL;

BOOL	AgcmCharacter::m_sbStandAloneMode	= FALSE;

// �ӽ÷� ���� �Լ�... ����� ������ ���Ѵ�.. -_-;
VOID UtilRenderSphere(RwSphere *sphere, RwMatrix *ltm, RwRGBA *rgba)
{
	RwIm3DVertex	circle[21];
    RwV3d			point;
    RwInt32			i;
    RwMatrix		_ltm;
	RwRGBA			_rgba;
	RwRGBA			red		= {255,   255,   0, 255};

	if(ltm)
	{
		RwMatrixCopy(&_ltm, ltm);
	}
	else
	{
		RwMatrixSetIdentity(&_ltm);
	}

	if(rgba)
	{
		memcpy(&_rgba, rgba, sizeof(RwRGBA));
	}
	else
	{
		memcpy(&_rgba, &red, sizeof(RwRGBA));
	}

    for(i = 0; i < 20 + 1; i++)
    {
		point.x = sphere->center.x + ((RwReal)RwCos(i / (20 / 2.0f) * rwPI) * sphere->radius);
        point.y = sphere->center.y + ((RwReal)RwSin(i / (20 / 2.0f) * rwPI) * sphere->radius);
        point.z = 0.0f;

        RwIm3DVertexSetPos(&circle[i], point.x, point.y, point.z);
        RwIm3DVertexSetRGBA(&circle[i], _rgba.red, _rgba.green, _rgba.blue, _rgba.alpha);
    }

	if(RwIm3DTransform(circle, 20 + 1, &_ltm, rwIM3D_ALLOPAQUE))
    {
        RwIm3DRenderPrimitive(rwPRIMTYPEPOLYLINE);
        RwIm3DEnd();
    }
}

static FLOAT __GetAngle( FLOAT x , FLOAT y )
{
	// ���..
	RwV3d	vDelta;
	vDelta.x	 = x;
	vDelta.z	 = y;
	vDelta.y	 = 0.0f;

	RwV3d	xAxis;
	xAxis.x = 1.0f; xAxis.y = 0.0f; xAxis.z = 0.0f;
	if( vDelta.z < 0.0f )
	{
		xAxis.x = -1.0f;
	}

	// RwV3dNormalize( &vDelta , &vDelta );
	FLOAT fSize = sqrt( vDelta.x * vDelta.x + vDelta.y * vDelta.y + vDelta.z * vDelta.z );
	vDelta.x	= vDelta.x / fSize;
	vDelta.y	= vDelta.y / fSize;
	vDelta.z	= vDelta.z / fSize;

	FLOAT	fDot;
	fDot = vDelta.x * xAxis.x + vDelta.y * xAxis.y + vDelta.z * xAxis.z;

	FLOAT fAngle = acos( fDot );

	fAngle = 360.0f * fAngle / ( 3.1415927f * 2 );

	if( vDelta.z < 0.0f )
	{
		fAngle += 180.0f;
	}

	return fAngle;
}

static BOOL __IsLeft( RwV3d *pAt , RwV3d *pNormal )
{
	FLOAT	fAngle1, fAngle2;

	// �����̳� �������̳�
	// ���� �̵� ��ġ ���.

	fAngle1 = __GetAngle( pAt->x , pAt->z );
	fAngle2 = __GetAngle( -pNormal->x , -pNormal->z );

	if( fAngle1 < fAngle2 )
	{
		if( fAngle2 - fAngle1 >= 180.0f )
			return FALSE;
		else
			return TRUE;
	}
	else
	{
		if( fAngle1 - fAngle2 >= 180.0f )
			return TRUE;
		else
			return FALSE;
	}
}

AgcdCharacterAnimation* AgcdCharacterTemplate::GetAnimation( AgcmCharacterAnimType eType , INT32 lCustAnimType2 )
{
	if( m_pacsAnimationData[eType][lCustAnimType2] ) 
		return m_pacsAnimationData[eType][lCustAnimType2];

	return NULL;
	//else
	//{
	//	// ����׸� Ȯ���� �ϱ� ���ؼ�
	//	// �ֶ��� ���ִ� �༮�� �������
	//	#ifdef _DEBUG
	//	return m_pacsAnimationData[AGCMCHAR_ANIM_TYPE_WAIT][0];
	//	#else
	//	return m_pacsAnimationData[eType][0];
	//	#endif
	//}
}

const float	AGCMCHARACTER_FOLLOW_ADJUST_RATE			= 1.3f;
const int	AGCMCHARACTER_SEND_MOVE_INTERVAL			= 50;				// milisecond

AgcmCharacter	*AgcmCharacter::m_pThisAgcmCharacter = NULL;

LuaGlue	LG_LogSiegeCharacter( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();

	FILE	* pFile = fopen( "LogSiegeMonster.txt" , "wt" );
	if( pFile )
	{
		AgpmCharacter	* pcsAgpmCharacter = ( AgpmCharacter * ) g_pEngine->GetModule( "AgpmCharacter" );

		for( vector< INT32 >::iterator iter = g_vecSiegeMonster.begin();
			iter != g_vecSiegeMonster.end() ;
			iter++ )
		{
			INT32	nCID	= *iter;
			AgpdCharacter * pcsCharacter = pcsAgpmCharacter->GetCharacter( nCID );

			if( pcsCharacter )
			{
				fprintf( pFile , "%d , %s\n" , pcsCharacter->m_lID , pcsCharacter->m_szID );
			}
			else
			{
				fprintf( pFile , "Character Pointer not found (%d)\n" , nCID  );
			}

		}

		fclose( pFile );

		g_pEngine->LuaErrorMessage( "Log Siege Character Info file...." );
	}

	LUA_RETURN( TRUE )
}

void	AgcmCharacter::OnLuaInitialize	( AuLua * pLua			)
{
	luaDef MVLuaGlue[] = 
	{
		{"LogSiegeCharacter"	,	LG_LogSiegeCharacter},
		{NULL					,	NULL				},
	};

	for(int i=0; MVLuaGlue[i].name; i++)
	{
		pLua->AddFunction(MVLuaGlue[i].name, MVLuaGlue[i].func);
	}
}

/*****************************************************************
*   Function : AgcmCharacter
*   Comment  : ������
*   Date&Time : 2002-04-28, ���� 1:21
*   Code By : Seong Yon-jun@NHN Studio
*****************************************************************/
AgcmCharacter::AgcmCharacter()
{
	SetModuleName("AgcmCharacter");

	EnableIdle(TRUE);

	SetPacketType(AGCMCHAR_PACKET_TYPE);

	// flag size = sizeof(CHAR)
	m_csPacket.SetFlagLength(sizeof(CHAR));
	m_csPacket.SetFieldType(
							AUTYPE_INT8, 1,			// Operation
							AUTYPE_CHAR, 12,		// Account Name
							AUTYPE_INT32, 1,		// Character Template ID
							AUTYPE_CHAR, AGPACHARACTER_MAX_ID_STRING + 1,		// Character Name
							AUTYPE_INT32, 1,		// Character id (ó���� ĳ���� �Ƶ�)
							AUTYPE_POS, 1,			// Character Position
							//AUTYPE_CHAR, 1,		// unused field
							AUTYPE_INT32, 1,		// m_lReceivedAuthKey
							AUTYPE_END, 0
							);

	m_pcsAgpmFactors		= NULL;
	m_pcsAgpmCharacter		= NULL;
	m_pcsAgpmParty			= NULL;
	m_pcsAgpmSkill			= NULL;
	m_pcsAgpmPvP			= NULL;
	m_pcsAgcmRender			= NULL;
	m_pcsAgcmShadow			= NULL;
	m_pcsAgcmShadowmap		= NULL;
	m_pcsAgcmConnectManager	= NULL;
	m_pcsApmMap				= NULL;
	m_pcsAgcmMap			= NULL;
	m_pcsApmObject			= NULL;
	m_pcsAgcmObject			= NULL;
	m_pcsAgcmLODManager		= NULL;
	m_pcsAgcmFont			= NULL;
	m_pcsAgcmResourceLoader	= NULL;
	m_pcsAgcmShadow2		= NULL;
	m_pcsAgpmOptimizedPacket2	= NULL;
	m_pcsAgcmSound			= NULL;
	m_pcsAgpmSummons		= NULL;
	m_pcsAgcmUIHotkey		= NULL;

	m_pSelfRideClump		= NULL;

	m_nCharacterAttachIndex	= 0;
	m_nTemplateAttachIndex	= 0;
	m_pWorld	= NULL;
	m_pCamera	= NULL;
	m_lMaxAnimation	= 0;
	m_fCameraPanInitial	= 15.0f;
	m_fCameraPanUnit = 5.0f;
	m_fCameraPan	= m_fCameraPanInitial;;

	m_ulPrevSendMoveTime = 0;
	m_ulSendMoveDelay = AGCMCHAR_DEFAULT_SEND_MOVE_DELAY;

	m_bUseTexDict	= FALSE;
	m_pstTexDict = NULL;

	m_ulSelectedTID	= 0;

	m_bFirstSendAttack	= FALSE;
	m_bStartAttack	= FALSE;

	m_lLoaderIDInit		= 0;
	m_lLoaderIDRemove	= 0;
	m_lLoaderCustomize	= 0;

	m_ulCurTick	= 0;
	m_lCurCharacterNum = 0;								//Add�� Character�� ��
	for(int i=0;i<CLIENT_CHARACTER_MAX_NUM;++i)
	{
		m_lCurCID[i] = 0;
		m_pCurCharacter[i] = NULL;
	}
	
	m_lSelfCID	= 0;
	m_pPlayerFrame = NULL;
	m_pcsSelfCharacter	= NULL;

	m_bIsDeletePrevSelfCharacterWhenNewSelfCharacterisSet = FALSE;
	m_pcsPrevSelfCharacter = NULL;
	m_pMainCharacterClump = NULL;

	m_szTexDict[0]	= '\0';
	m_szTexturePath[0] = 0;
	
	AgcmCharacter::m_pThisAgcmCharacter = this;
	
	m_lLastSendActionTarget	= AP_INVALID_CID;
	::ZeroMemory(m_szCharName, AGPACHARACTER_MAX_ID_STRING + 1);
	
	strcpy( m_szClumpPath		, "" );
	strcpy( m_szAnimationPath	, "" );

	//m_ulActionBlockTime	= 0;

	m_szTexturePath[0] = 0;

	m_lReceivedAuthKey	= 0;

	m_ulModuleFlags		= E_AGCM_CHAR_FLAGS_NONE;

	m_lFollowTargetCID	= AP_INVALID_CID;
	m_lFollowDistance		= 0;

	m_ulLastSendMoveTime	= 0;
	m_bReservedMove			= FALSE;

	ZeroMemory(&m_stReservedMoveDestPos, sizeof(m_stReservedMoveDestPos));
	m_eReservedMoveDirection	= MD_NODIRECTION;
	m_bReservedMovePathFinding	= 0;
	m_bReservedMoveMoveFast		= 0;

	m_bFollowAttackLock			= FALSE;
	m_lFollowAttackTargetID		= AP_INVALID_CID;

	ZeroMemory(&m_stNextAction, sizeof(m_stNextAction));

	m_bShowPickingInfo			= FALSE;
	m_bShowActionQueue			= FALSE;

	//@{ 2006/03/08 burumal
	m_bShowBoundingSphere		= FALSE;
	//@}

	// ������ (2005-03-11 ���� 2:26:45) : 
	m_cPathNodeList.allocSwapBuffer( AGPMPATHFIND_TRY_COUNT );
	m_cPathNodeList.resetIndex();

	m_bAdjustMovement = FALSE	;	// �̵� ������.
	m_nRetryCount		= 0		;

	ZeroMemory(m_szGameServerAddress, sizeof(m_szGameServerAddress));

	m_lookAtFactor = 0.0f;

	//@{ 2006/07/14 burumal
	m_bLookAtFirstUpdateFlag = true;
	//@}

	//@{ 2007/01/03 burumal
	m_bBendingDisabled = false;
	//@}

	m_bKeyDownW	= FALSE;
	m_bKeyDownS	= FALSE;
	m_bKeyDownA	= FALSE;
	m_bKeyDownD	= FALSE;

	m_eReadType	= AGCMCHAR_READ_TYPE_ALL;

#ifdef USE_MFC
	//. 2006. 3. 16. nonstopdj
	//. only using Modeltool debug.
	m_bisSaveHairClump = FALSE;
#endif

	//@{ 2006/07/05 burumal
	m_bMoveKeyEnabled = FALSE;
	ZeroMemory(m_pMovementHotkey, sizeof(AgcdUIHotkey*) * 4);
	ZeroMemory(m_nMoveVirtualKeys, sizeof(UINT) * 4);
	//@}

	m_bTurnLeft		= FALSE;
	m_bTurnRight	= FALSE;

	// �Դٸ��� ����Ʈ��.
	m_bStrafeMove	= TRUE;
}

/*****************************************************************
*   Function : ~AgcmCharacter
*   Comment  : �Ҹ��� 
*   Date&Time : 2002-04-28, ���� 1:21
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
AgcmCharacter::~AgcmCharacter()
{

}

/*****************************************************************
*   Function : OnAddModule
*   Comment  : 
*   Date&Time : 2002-04-28, ���� 1:26
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
BOOL	AgcmCharacter::OnAddModule()
{
	m_pcsAgpmFactors		= (	AgpmFactors			*	) GetModule( "AgpmFactors"			);
	m_pcsApmMap				= ( ApmMap				*	) GetModule( "ApmMap"				);
	m_pcsAgpmCharacter		= ( AgpmCharacter		*	) GetModule( "AgpmCharacter"		);
	m_pcsAgpmParty			= ( AgpmParty			*	) GetModule( "AgpmParty"			);
	m_pcsAgpmSkill			= ( AgpmSkill			*	) GetModule( "AgpmSkill"			);
	m_pcsAgpmPvP			= ( AgpmPvP				*	) GetModule( "AgpmPvP"				);
	m_pcsAgcmMap			= ( AgcmMap				*	) GetModule( "AgcmMap"				);
	m_pcsAgcmRender			= ( AgcmRender			*	) GetModule( "AgcmRender"			);
	m_pcsAgcmShadow			= ( AgcmShadow			*	) GetModule( "AgcmShadow"			);
	m_pcsAgcmShadowmap		= (	AgcmShadowmap		*	) GetModule( "AgcmShadowmap"		);
	m_pcsAgcmConnectManager	= ( AgcmConnectManager	*	) GetModule( "AgcmConnectManager"	);
	m_pcsAgcmLODManager		= ( AgcmLODManager		*	) GetModule( "AgcmLODManager"		);
	m_pcsAgcmFont			= ( AgcmFont			*	) GetModule( "AgcmFont"				);
	m_pcsApmObject			= ( ApmObject			*	) GetModule( "ApmObject"			);
	m_pcsAgcmObject			= ( AgcmObject			*	) GetModule( "AgcmObject"			);
	m_pcsAgcmResourceLoader	= ( AgcmResourceLoader	*	) GetModule( "AgcmResourceLoader"	);
	m_pcsAgcmShadow2		= ( AgcmShadow2			*	) GetModule( "AgcmShadow2"			);
	m_pcsAgpmOptimizedPacket2	= ( AgpmOptimizedPacket2	*	) GetModule( "AgpmOptimizedPacket2"	);
	m_pcsAgcmSound			= ( AgcmSound			*	) GetModule( "AgcmSound"			);
	m_pcsAgpmPathFind		= ( AgpmPathFind		*	) GetModule( "AgpmPathFind"			);
	m_pcsAgpmSummons		= ( AgpmSummons			*	) GetModule( "AgpmSummons"			);
	m_pcsAgpmArchlord		= ( AgpmArchlord		*	) GetModule( "AgpmArchlord"			);
	m_pcsAgcmGuild			= ( AgcmGuild			*	) GetModule( "AgcmGuild"			);
	
	if (!m_pcsAgpmArchlord)
		return FALSE;

	// setting attached data in character module
	if (m_pcsAgpmCharacter)
	{
		m_nCharacterAttachIndex = m_pcsAgpmCharacter->AttachCharacterData(this, sizeof(AgcdCharacter), CharacterConstructor, CharacterDestructor);
		if (m_nCharacterAttachIndex < 0) return FALSE;

		m_nTemplateAttachIndex = m_pcsAgpmCharacter->AttachCharacterTemplateData(this, sizeof(AgcdCharacterTemplate), TemplateConstructor, TemplateDestructor);
		if (m_nTemplateAttachIndex < 0)	return FALSE;

		// Callback�� ����Ѵ�.
		if (!m_pcsAgpmCharacter->SetCallbackInitChar(CBInitCharacter, this)) return FALSE;
		if (!m_pcsAgpmCharacter->SetCallbackRemoveChar(CBRemoveCharacter, this)) return FALSE;
		if (!m_pcsAgpmCharacter->SetCallbackMoveChar(CBMoveCharacter, this)) return FALSE;
		if (!m_pcsAgpmCharacter->SetCallbackStopChar(CBStopCharacter, this)) return FALSE;
		if (!m_pcsAgpmCharacter->SetCallbackUpdateChar(CBUpdateCharacter2, this)) return FALSE;
		if (!m_pcsAgpmCharacter->SetCallbackUpdatePosition(CBUpdatePosition, this)) return FALSE;
		if (!m_pcsAgpmCharacter->SetCallbackUpdatePositionCheck(CBUpdatePositionCheck, this)) return FALSE;
		if (!m_pcsAgpmCharacter->SetCallbackCheckIsActionMove(CBCheckISActionMove, this)) return FALSE;
		if (!m_pcsAgpmCharacter->SetCallbackActionAttack(CBActionAttack, this))	return FALSE;
		if (!m_pcsAgpmCharacter->SetCallbackUpdateStatus(CBUpdateStatus, this)) return FALSE;
		if (!m_pcsAgpmCharacter->SetCallbackUpdateActionStatusCheck(CBUpdateActionStatusCheck, this)) return FALSE;
		if (!m_pcsAgpmCharacter->SetCallbackUpdateActionStatus(CBUpdateActionStatus, this)) return FALSE;
		if (!m_pcsAgpmCharacter->SetCallbackUpdateLevel(CBUpdateLevel, this)) return FALSE;
		if (!m_pcsAgpmCharacter->SetCallbackReceiveAction(CBReceiveAction, this)) return FALSE;
		if (!m_pcsAgpmCharacter->SetCallbackCheckNowUpdateActionFactor(CBCheckNowUpdateActionFactor, this)) return FALSE;
		if (!m_pcsAgpmCharacter->SetCallbackNewCreatedCharacter(CBNewCreateCharacter, this)) return FALSE;
		if (!m_pcsAgpmCharacter->SetCallbackDisconnectByAnotherUser(CBDisconnectByAnotherUser, this) ) return FALSE;
		if (!m_pcsAgpmCharacter->SetCallbackTransformAppear(CBTransformAppear, this)) return FALSE;
		if (!m_pcsAgpmCharacter->SetCallbackRestoreTransform(CBTransformAppear, this)) return FALSE;
		if (!m_pcsAgpmCharacter->SetCallbackEvolution(CBEvolutionAppear, this))	return FALSE;
		if (!m_pcsAgpmCharacter->SetCallbackRestoreEvolution(CBEvolutionAppear, this)) return FALSE;
		if (!m_pcsAgpmCharacter->SetCallbackSocialAnimation(CBSocialAnimation, this)) return FALSE;
		if (!m_pcsAgpmCharacter->SetCallbackUpdateCustomize(CBUpdateCustomize, this)) return FALSE;
		if (!m_pcsAgpmCharacter->SetCallbackUpdateOptionFlag(CBUpdateOptionFlag, this)) return FALSE;
		if (!m_pcsAgpmCharacter->SetCallbackCheckRemoveChar2(CBCheckRemoveChar, this)) return FALSE;

		if( !AddStreamCallback( AGCMCHAR_DATA_TYPE_TEMPLATE, TemplateStreamReadCB, TemplateStreamWriteCB, this) ) return FALSE;
		if( !AddStreamCallback( AGCMCHAR_DATA_TYPE_ANIMATION, AnimationStreamReadCB, AnimationStreamWriteCB, this) ) return FALSE;
		if( !AddStreamCallback( AGCMCHAR_DATA_TYPE_CUSTOMIZE, CustomizeStreamReadCB, CustomizeStreamWriteCB, this ) ) return FALSE;

		if( !m_pcsAgpmCharacter->SetCallbackBindingRegionChange( CBBindingRegionChange , this ) ) return FALSE;
		if( !m_pcsAgpmCharacter->SetCallbackRemoveRide( CBRemoveRide , this ) )	return FALSE;
		if( !m_pcsAgpmCharacter->SetCallbackAlreadyExistChar( CBAlreadyExistChar , this ) )	return FALSE;
		if( !m_pcsAgpmCharacter->SetCallbackStreamReadImportData(CharacterImportDataReadCB, this)) return FALSE;
		if( !m_pcsAgpmCharacter->SetCallbackGameguardAuth( CBGameguardAuth, this ) ) return FALSE;
		if( !m_pcsAgpmCharacter->SetCallBackCheckMovementLock( CBCheckMovementLock, this ) ) return FALSE;

	}

	// netong �߰�
	if(m_pcsApmMap)
	{
		if (!m_pcsApmMap->SetCallbackAddChar	(CBAddChar		, this)) return FALSE;
		if (!m_pcsApmMap->SetCallbackRemoveChar	(CBRemoveChar	, this)) return FALSE;
	}

	// Seong Yon-jun �߰� 
	if (m_pcsAgpmFactors)
	{
		if (!m_pcsAgpmFactors->SetCallbackUpdateFactorUI( CBUpdateFactor, (PVOID)this )) return FALSE;
		if (!m_pcsAgpmFactors->SetCallbackUpdateFactorMovement( CBUpdateFactorMovement, this )) return FALSE;
		if (!m_pcsAgpmFactors->SetCallbackUpdateFactorHp( CBUpdateFactorHP, this ))	return FALSE;
		if (!m_pcsAgpmFactors->SetCallbackUpdateFactorMp( CBUpdateFactorMP, this )) return FALSE;
	}

	if (m_pcsAgcmMap)
	{
		if ( !m_pcsAgcmMap->SetCallbackMapLoadEnd( CBLoadMap, (PVOID)this ) ) return FALSE;
	}

	if (m_pcsAgcmResourceLoader)
	{
		m_lLoaderIDInit		= m_pcsAgcmResourceLoader->RegisterLoader(this, CBInitCharacterBackThread, CBInitCharacterMainThread);
		m_lLoaderCustomize	= m_pcsAgcmResourceLoader->RegisterLoader(this, NULL, CBCustomizeLoader	);

		if (m_lLoaderIDInit		< 0 ||
			m_lLoaderIDRemove	< 0	||
			m_lLoaderCustomize	< 0	)
			return FALSE;
	}

	if (m_pcsAgpmOptimizedPacket2)
	{
		if (!m_pcsAgpmOptimizedPacket2->SetCallbackMoveActionAttack(CBMoveActionAttack, this))
			return FALSE;
		if (!m_pcsAgpmOptimizedPacket2->SetCallbackMoveActionRelease(CBMoveActionRelease, this))
			return FALSE;
	}

	if( m_pcsAgcmObject )
	{
		// ���̴��� üũ..
		if (!m_pcsAgcmObject->SetCallbackRidableUpdate( CBRidableUpdate , this ) )
			return FALSE;
	}

	m_szClumpPath[0] = '\0';

	SetWorld(GetWorld(), GetCamera());

	return TRUE;	
}

VOID	AgcmCharacter::ToggleStrafeMove()
{
	m_bStrafeMove = !m_bStrafeMove;

	if( m_bStrafeMove )
		print_ui_console( _T("Use Key A and D as StrafeMove") );
	else
		print_ui_console( _T("Use Key A and D as Turning Character") );
}

/*****************************************************************
*   Function : OnInit
*   Comment  : 
*   Date&Time : 2002-04-28, ���� 1:26
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
BOOL	AgcmCharacter::OnInit()
{
	if (m_lMaxAnimation == 0) return FALSE;

	m_csAnimation2.SetMaxRtAnim(m_lMaxAnimation);

	// ������ (2006-02-01 ���� 11:32:08) : ���ϸ��̼� �÷����� �迭�ʰ��� �ø�..
	// m_csAnimation2.SetMaxFlags(m_lMaxAnimation / 3);
	m_csAnimation2.SetMaxFlags(m_lMaxAnimation / 2);

	//m_csAnimation2.SetAnimationPathAndExtension(m_szAnimationPath, AGCD_ANIMATION_DEFAULT_EXT);
	m_csAnimation2.SetAnimationPathAndExtension(
		m_szAnimationPath,
		(m_ulModuleFlags & E_AGCM_CHAR_FLAGS_EXPORT) ? (AC_EXPORT_EXT_ANIMATION) : (NULL)	);

	m_csAnimation2.SetAttachedData(
		AGCD_CHARACTER_ANIMATION_ATTACHED_DATA_KEY,
		sizeof (ACA_AttachedData),
		RemoveAnimAttachedDataCB,
		NULL										);

	AS_REGISTER_TYPE_BEGIN(AgcmCharacter, AgcmCharacter);
	AS_REGISTER_METHOD0(void, ShowPickingInfo);
	AS_REGISTER_METHOD0(void, HidePickingInfo);

	//@{ 2006/03/08 burumal
	AS_REGISTER_METHOD0(void, ShowBoundingSphere);
	AS_REGISTER_METHOD0(void, HideBoundingSphere);
	//@}

	AS_REGISTER_VARIABLE(string, m_szNextAction);
	AS_REGISTER_METHOD0(void, ShowNextAction);

	AS_REGISTER_VARIABLE(string, m_szActionQueue);
	AS_REGISTER_VARIABLE(string, m_szActionQueueAcc);
	AS_REGISTER_VARIABLE(string, m_szAttackSentAcc);
	AS_REGISTER_METHOD0(void, ShowActionQueue);
	AS_REGISTER_METHOD0(void, ClearActionQueue);
	AS_REGISTER_METHOD0(void, ShowMyCID);


// #ifdef _AREA_CHINA_
// 	AS_REGISTER_METHOD0(void, ShowNumOfCharacByType);
// #endif


	AS_REGISTER_METHOD0(void, ShowRegionInfo);
	AS_REGISTER_METHOD0(void, ToggleStrafeMove);

	AS_REGISTER_METHOD_HIDE(void, KillGameServer1);

#ifdef __PROFILE__
	AS_REGISTER_METHOD0(void, EnableFrameCallStack);
	AS_REGISTER_METHOD0(void, DisableFrameCallStack);
#endif
	AS_REGISTER_TYPE_END;
	
	g_pcsAgcmEventEffect = static_cast< AgcmEventEffect * > ( GetModule( "AgcmEventEffect" ) );

	return TRUE;
}

void	AgcmCharacter::ShowRegionInfo()
{
	if( !m_pcsSelfCharacter )
	{
		print_ui_console( _T("There is not a self character.") );
		return;
	}

	ApmMap::RegionTemplate*	pTemplate = this->m_pcsApmMap->GetTemplate( m_pcsSelfCharacter->m_nBindingRegionIndex );

	if( !pTemplate )
	{
		print_ui_console( _T("Cannot get the Template") );
		return;
	}
	else
	{
		print_ui_console( pTemplate->pStrName );
		switch( pTemplate->ti.stType.uFieldType )
		{
		case	ApmMap::FT_FIELD	: 	print_ui_console( _T("FT_FIELD") );break;
		case	ApmMap::FT_TOWN		:	print_ui_console( _T("FT_TOWN") );break;
		case	ApmMap::FT_PVP		: 	print_ui_console( _T("FT_PVP") );break;
		}

		int nDivision = GetDivisionIndexF( m_pcsSelfCharacter->m_stPos.x , m_pcsSelfCharacter->m_stPos.z );
		char str[ 256 ];
		sprintf( str , "%d Division" , nDivision );
		print_ui_console( str );
		
		// ������ (2005-11-03 ���� 3:09:38) : �Ŀ� �� �߰�
	}
}

	
/*****************************************************************
*   Function : OnDestroy
*   Comment  : 
*   Date&Time : 2002-04-28, ���� 1:26
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
BOOL	AgcmCharacter::OnDestroy()
{
	m_csAnimation2.RemoveAllRtAnim();
	m_PolyMorphTableMap.Clear();
	return TRUE;
}

BOOL	AgcmCharacter::OnIdle(UINT32 ulClockCount)
{
	PROFILE("AgcmCharacter::OnIdle");

	//@{ 2006/08/21 burumal
#ifdef USE_MFC
	if ( (GetPlayerLookAtFactor() > 0.0f) && (GetAsyncKeyState(VK_CONTROL) >= 0) )
		SetPlayerLookAtFactor(GetPlayerLookAtFactor() - (RwReal)(ulClockCount-m_ulCurTick)/2000.0f);
#endif
	//@}

	m_ulCurTick = ulClockCount;

	UpdateAnimationTimeForAllCharacter();

	//@{ Jaewon 20050826
	// Remove the character whose fading-out has completed.
	RwUInt32 ctime = timeGetTime();
	while(!m_fadeOutReomvalList.empty())
	{
		AgcdCharacter *pAgcdCharacter = GetCharacterData(m_fadeOutReomvalList.front().first);
		ASSERT(pAgcdCharacter);
		ASSERT(pAgcdCharacter->m_pClump);

		if(m_fadeOutReomvalList.front().second + 2000 > ctime)
			break;

		m_pcsAgpmCharacter->RemoveCharacter(m_fadeOutReomvalList.front().first->m_lID);
		m_fadeOutReomvalList.pop_front();
	}
	//@} Jaewon

#ifdef USE_MFC
	//. 2006. 3. 13. Nonstopdj
	//. if build version was 'MODELTOOL', then we not use keyboard move process.
	return TRUE;
#endif
	
	KeyboardIdleProcess	();
	PolymorphIdleProcess();

	return TRUE;
}

/*****************************************************************
*   Function : SetWorld
*   Comment  : ����ϰ� �ִ� World�� �����͸� �Ѱ��ش�. 
*   Date&Time : 2002-04-29, ���� 4:07
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
BOOL	AgcmCharacter::SetWorld(RpWorld* pWorld, RwCamera* pCamera)
{
	m_pWorld = pWorld;
	m_pCamera = pCamera;

	return TRUE;
}

BOOL	AgcmCharacter::SetClumpPath(CHAR *szPath)
{
	strncpy(m_szClumpPath, szPath, AGCMCHAR_MAX_PATH);
	m_szClumpPath[AGCMCHAR_MAX_PATH - 1] = '\0';

	return TRUE;
}

BOOL	AgcmCharacter::SetAnimationPath(CHAR *szPath)
{
	strncpy(m_szAnimationPath, szPath, AGCMCHAR_MAX_PATH);
	m_szAnimationPath[AGCMCHAR_MAX_PATH - 1] = '\0';

	return TRUE;
}

void AgcmCharacter::SetMaxAnimations(INT32 lAnimationNum)
{
	m_lMaxAnimation			= lAnimationNum;
}

void AgcmCharacter::SetSendMoveDelay(UINT32 ulDelay)
{
	m_ulSendMoveDelay = ulDelay;
}

void AgcmCharacter::SetTexDictFile(CHAR *szTexDict)
{
	m_bUseTexDict = TRUE;

	strncpy(m_szTexDict, szTexDict, AGCMCHAR_MAX_PATH);
	m_szTexDict[AGCMCHAR_MAX_PATH - 1] = '\0';
}

void AgcmCharacter::SetTexturePath(CHAR *szTexturePath)
{
	strncpy(m_szTexturePath, szTexturePath, AGCMCHAR_MAX_PATH);
	m_szTexturePath[AGCMCHAR_MAX_PATH - 1] = '\0';
}


/*****************************************************************
*   Function : GetCharacterData
*   Comment  : Get Character Client Data to Character Public Data
*   Date&Time : 2002-04-28, ���� 1:21
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
AgcdCharacter*	AgcmCharacter::GetCharacterData(AgpdCharacter *pstCharacter)
{
	return m_pcsAgpmCharacter ? (AgcdCharacter *) m_pcsAgpmCharacter->GetAttachedModuleData(m_nCharacterAttachIndex, (void *) pstCharacter) : NULL;
}

/*****************************************************************
*   Function : GetCharacter
*   Comment  : Get Character Character Public Data
*   Date&Time : 2002-04-28, ���� 1:21
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
AgpdCharacter*	AgcmCharacter::GetCharacter(AgcdCharacter *pstCharacter)
{
	return m_pcsAgpmCharacter ? (AgpdCharacter *) m_pcsAgpmCharacter->GetParentModuleData(m_nCharacterAttachIndex, (void *) pstCharacter) : NULL;
}

/*****************************************************************
*   Function : GetTemplateData
*   Comment  : Get Character Client Data to Character Public Data
*   Date&Time : 2002-04-28, ���� 1:21
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
AgcdCharacterTemplate*	AgcmCharacter::GetTemplateData(AgpdCharacterTemplate *pstTemplate)
{
	return m_pcsAgpmCharacter ? (AgcdCharacterTemplate *) m_pcsAgpmCharacter->GetAttachedModuleData(m_nTemplateAttachIndex, (void *) pstTemplate) : NULL;
}

/*****************************************************************
*   Function : GetTemplateData
*   Comment  : Get Character Client Data to Character Public Data
*   Date&Time : 2002-04-28, ���� 1:21
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
AgpdCharacterTemplate*	AgcmCharacter::GetTemplate(AgcdCharacterTemplate *pstTemplate)
{
	return m_pcsAgpmCharacter ? (AgpdCharacterTemplate *) m_pcsAgpmCharacter->GetParentModuleData(m_nTemplateAttachIndex, (void *) pstTemplate) : NULL;
}

/******************************************************************************
* Purpose : Effect�� �����´�~
*
* 121702. Bob Jung
******************************************************************************/
/*AgcdEventCharacterEffect* AgcmCharacter::GetEFfectData(AgpdCharacterTemplate *pstTemplate)
{
return (AgcdEventCharacterEffect *)(m_pcsAgpmCharacter ? m_pcsAgpmCharacter->GetAttachedModuleData(m_nEffectAttachIndex, (void *)(pstTemplate)) : NULL);
}*/

/*****************************************************************
*   Function : SetHierarchyForSkinAtomic
*   Comment  : 
*   Date&Time : 2002-05-02, ���� 11:57
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
RpAtomic* AgcmCharacter::SetHierarchyForSkinAtomic(RpAtomic *atomic, void *data)
{
	/*	RpGeometry	*pGeom = RpAtomicGetGeometry(atomic);
	RpSkin		*pSkin;
	if (pGeom)
	{
	pSkin = RpSkinGeometryGetSkin(pGeom);
	if (!pSkin)
	return atomic;
	}*/

	RpSkinAtomicSetHAnimHierarchy(atomic, (RpHAnimHierarchy *)data);

	return atomic;
}

BOOL AgcmCharacter::MoveSelfCharacter(AuPOS * pstPos, MOVE_DIRECTION eMoveDirection, BOOL bPathFinding, BOOL bMoveFast, RwV3d *pstAt)
{
	if (!m_pcsSelfCharacter) return FALSE;

	AgcmItem* pcmItem = ( AgcmItem* )GetModule( "AgcmItem" );
	if( pcmItem )
	{
		if( pcmItem->IsNowWaitForUseItem() ) return FALSE;
	}

	if (m_pcsAgpmCharacter->IsActionBlockCondition(m_pcsSelfCharacter))
	{
		/*
		AgcdCharacter	*pstAgcdCharacter	= GetCharacterData(m_pcsSelfCharacter);

		pstAgcdCharacter->m_stNextAction.m_eActionType		= AGPDCHAR_ACTION_TYPE_MOVE;
		pstAgcdCharacter->m_stNextAction.m_stTargetPos		= *pstPos;

		pstAgcdCharacter->m_stNextAction.m_lUserData[0]		= (INT32) bMoveDirection;
		pstAgcdCharacter->m_stNextAction.m_lUserData[1]		= (INT32) bPathFinding;
		pstAgcdCharacter->m_stNextAction.m_lUserData[2]		= (INT32) bMoveFast;
		*/

		AgpdCharacterAction	stNextAction;
		ZeroMemory(&stNextAction, sizeof(AgpdCharacterAction));

		stNextAction.m_eActionType			= AGPDCHAR_ACTION_TYPE_MOVE;
		stNextAction.m_stTargetPos			= *pstPos;
		stNextAction.m_lUserData[0]			= (INT32) eMoveDirection;
		stNextAction.m_lUserData[1]			= (INT32) bPathFinding;
		stNextAction.m_lUserData[2]			= (INT32) bMoveFast;

		SetNextAction(&stNextAction);

		return FALSE;
	}

	UINT32	ulClockCount	= GetClockCount();

	if (m_ulLastSendMoveTime + AGCMCHARACTER_SEND_MOVE_INTERVAL > ulClockCount)
	{
		m_stReservedMoveDestPos		= *pstPos;
		m_eReservedMoveDirection	= eMoveDirection;
		m_bReservedMovePathFinding	= bPathFinding;
		m_bReservedMoveMoveFast		= bMoveFast;
		if (pstAt)
			m_stReservedMoveAt			= *pstAt;

		m_bReservedMove				= TRUE;

		return TRUE;
	}

	m_bReservedMove			= FALSE;
	m_ulLastSendMoveTime	= ulClockCount;

	UnLockTarget(m_pcsSelfCharacter);

	ReleaseFollowTarget();

	AuPOS vValid;
	if (eMoveDirection && pstAt)
	{
		pstPos	= (AuPOS *) pstAt;
		// ������ 2006/08/30
		// �Ѽ��� �Ÿ����� �ɸ��°� ������
		// �ɸ��µ� ���� �̵����� ó����
		vValid = GetSelfCharacter()->m_stPos;
		// �Ѽ��� �Ÿ���ŭ üũ.
		vValid.x = vValid.x + pstAt->x * MAP_SECTOR_WIDTH;
		vValid.y = vValid.y + pstAt->y * MAP_SECTOR_WIDTH;
		vValid.z = vValid.z + pstAt->z * MAP_SECTOR_WIDTH;

		ApmMap::BLOCKINGTYPE eType = m_pcsAgpmCharacter->GetBlockingType( GetSelfCharacter() );

		BOOL bBlocked = FALSE;
		if( eType == ApmMap::GROUND			&&
			GetValidDestination( 
				&GetSelfCharacter()->m_stPos	,
				&vValid							,
				&vValid							) )
		{
			TRACE( "GetValidDestination üũ�ɸ�! \n" );
			// üũ�ƴ�.
			bBlocked = TRUE;
		}

		if( m_pcsAgpmCharacter->GetValidDestination( 
			&GetSelfCharacter()->m_stPos	,
			&vValid											,
			&vValid											,
			eType											,
			m_pcsSelfCharacter								) )
		{
			TRACE( "GetValidDestination �ɸ��� üũ�ɸ�! \n" );
			// üũ�ƴ�.
			bBlocked = TRUE;
		}

		if( bBlocked )
		{
			// pThis->MoveSelfCharacter( &vValid , FALSE );

			switch( eMoveDirection )
			{
			default:
			case MD_FORWARD			:
			case MD_FORWARDRIGHT	:
			case MD_FORWARDLEFT		:
				// �ش� ��ŷ ����Ʈ�� �̵�������� ������.
				pstPos	= &vValid;
				eMoveDirection = MD_NODIRECTION;
				break;

			case MD_RIGHT			:
			case MD_BACKWARDRIGHT	:
			case MD_BACKWARD		:
			case MD_BACKWARDLEFT	:
			case MD_LEFT			:
				// ���ϸ��̼� ����Ÿ�� ó���� ����� ��� �ϴ� ��ġ..
				// �԰����̳� �ް�������
				// ���� �̿��� ���� ����ϴ°��� ������ ( ���κ�ŷ ó���Ȱ� �������� ����� üũ�ϴϱ� ���� ���� )
				break;
			}
		}
	}

#ifdef ENABLE_PATH_FIND_MODE

	switch( m_pcsAgpmCharacter->GetBlockingType( GetSelfCharacter() ) )
	{
	default:
	case ApmMap::GROUND			:	// ���붥.
	case ApmMap::UNDERGROUND	:	// ����.. �Ƹ� �Ⱦ��� ������
		break;
	case ApmMap::SKY			:	// �ϴ�.. ������ Ż��
	case ApmMap::GHOST			:	// ����.. ���ŷ�� �����Ѵ�.
		bPathFinding = FALSE;
		break;
	}

	if( bPathFinding )
	{
		StopSelfCharacter();

		// �̵� ������ �ʱ�ȭ �Ѵ�.
		// �� ����Ÿ�� ��ŷ�� �߻������� ����Ѵ�.
		m_posMoveStart		= m_pcsSelfCharacter->m_stPos	;	// �̵� ������
		m_posMoveEnd		= * pstPos						;	// �̵� ������
		m_bAdjustMovement	= TRUE							;	// �̵� ������.
		m_nRetryCount		= 0								;

		m_cPathNodeList.resetIndex();

		// ��ŷ üũ �ּ� �Ÿ� ���� �ɸ��� ������Ʈ�� �ִ��� Ȯ���Ѵ�.
	}

	if( m_bAdjustMovement )
	{
		// �ɸ����� At ���� ,
		RwV3d	at;
		{
			RwV3d	vPos;
			vPos.x	= m_pcsSelfCharacter->m_stPos.x;
			vPos.z	= m_pcsSelfCharacter->m_stPos.z;
			vPos.y	= m_pcsSelfCharacter->m_stPos.y;

			RwV3d	vDst;
			vDst.x	= pstPos->x;
			vDst.z	= pstPos->z;
			vDst.y	= pstPos->y;//m_pcsAgcmMap->GetHeight( pstPos->x , pstPos->z );

			RwV3dSub( &at , &vDst , &vPos );
			RwV3dNormalize( &at , &at );
		}

		/*
		// ���� �ɸ����� ��ġ
		RwV3d	pos;
		{
			pos.x	= m_pcsSelfCharacter->m_stPos.x;
			pos.z	= m_pcsSelfCharacter->m_stPos.z;
			pos.y	= 0.0f;
		}
		*/

		// ���� ���� ���..
		AuPOS	posDst;

		RwMatrix	* pMatrix = RwMatrixCreate();

		RwV3d	yAxis;
		yAxis.x = 0.0f;
		yAxis.y = 1.0f;
		yAxis.z = 0.0f;

		// ������ (2005-03-22 ���� 2:37:33) : 
		// ��Ʈ�� ���� ..

		BOOL	bFoundDestination = FALSE;

		BOOL	bNoCollision = FALSE;
		int nTurn = 0;
		{
			// ������ �� �ִ� ���� ã��..
			RwV3d	vComputedAt;
			RwV3d	vDest;
			BOOL	bLeft = FALSE;

			RwV3dTransformVector( &vComputedAt , &at , pMatrix );

			// ������ (2005-03-29 ���� 7:12:36) : y���� ����...
			vComputedAt.y = 0;
			RwV3dNormalize( &vComputedAt , &vComputedAt );

			RwV3dScale( &vDest , &vComputedAt , g_fPathFindMinimumDistance );
			posDst.x = vDest.x + m_pcsSelfCharacter->m_stPos.x;
			posDst.z = vDest.z + m_pcsSelfCharacter->m_stPos.z;

			posDst.y = m_pcsAgcmMap->GetHeight( posDst.x , posDst.z , m_pcsSelfCharacter->m_stPos.y );

			RwV3d	vNormal;

			// �ɸ�����ġ���� posDst ���� �浹 �˻縦 �Ѵ�.
			switch( BlockingPositionCheck( &m_pcsSelfCharacter->m_stPos , &posDst , &vNormal , nTurn == 0 ? &bLeft : NULL ) )
			{
			case	GEOMETRY_BLOCKING	:
			case	OBJECT_BLOCKING		:
				{
					// ����� 90�� �������� Ʋ� üũ..
					
					if( bLeft )
						RwMatrixRotate( pMatrix , &yAxis , -90.0f , rwCOMBINEREPLACE );
					else
						RwMatrixRotate( pMatrix , &yAxis , +90.0f , rwCOMBINEREPLACE );

					RwV3dTransformVector( &vComputedAt , &vNormal , pMatrix );
					vComputedAt.y = 0;
					RwV3dNormalize( &vComputedAt , &vComputedAt );

					RwV3dScale( &vDest , &vComputedAt , g_fPathFindMinimumDistance );
					posDst.x = vDest.x + m_pcsSelfCharacter->m_stPos.x;
					posDst.z = vDest.z + m_pcsSelfCharacter->m_stPos.z;
					posDst.y = m_pcsAgcmMap->GetHeight( posDst.x , posDst.z , m_pcsSelfCharacter->m_stPos.y );

					switch( BlockingPositionCheck( &m_pcsSelfCharacter->m_stPos , &posDst , NULL , NULL ) )
					{
					case	GEOMETRY_BLOCKING	:
					case	OBJECT_BLOCKING		:
						{
							// �������� �ؼ� ���̸� �ű⼭ ���� ���غ���..
							for( nTurn = 10 ; nTurn <= g_snMaxTwistAngle ; nTurn += 10 )
							{
								if( bLeft )
									RwMatrixRotate( pMatrix , &yAxis , ( FLOAT ) ( -90.0f + nTurn ) , rwCOMBINEREPLACE );
								else
									RwMatrixRotate( pMatrix , &yAxis , ( FLOAT ) ( +90.0f - nTurn ) , rwCOMBINEREPLACE );

								// At ���͸� Ư�� ���� ��ŭ ȸ�����Ѽ�
								// �׻��¿��� ��Ÿ�� �Ÿ� ��ŭ �̵���
								// ��������Ʈ�� ��ǥ���� ����.

								RwV3dTransformPoint( &vComputedAt , &at , pMatrix );

								// ������ (2005-03-29 ���� 7:12:36) : y���� ����...
								vComputedAt.y = 0;
								RwV3dNormalize( &vComputedAt , &vComputedAt );

								RwV3dScale( &vDest , &vComputedAt , g_fPathFindMinimumDistance );
								posDst.x = vDest.x + m_pcsSelfCharacter->m_stPos.x;
								posDst.z = vDest.z + m_pcsSelfCharacter->m_stPos.z;
								posDst.y = m_pcsAgcmMap->GetHeight( posDst.x , posDst.z , m_pcsSelfCharacter->m_stPos.y );

								// �ɸ�����ġ���� posDst ���� �浹 �˻縦 �Ѵ�.
								switch( BlockingPositionCheck( &m_pcsSelfCharacter->m_stPos , &posDst , NULL , NULL ) )
								{
								case	GEOMETRY_BLOCKING	:
								case	OBJECT_BLOCKING		:
									{
										// �ٽ� �õ�..
										break;
									}

								default:
								case	NOBLOCKING			:
								case	SECTOR_NOTFOUND		:
								case	UNKNOWN_BLOCKING	:
									{
										// �̴�� ���� �ȴ�..
										bFoundDestination = TRUE;
										break;
									}
								}

								if( bFoundDestination ) break;
							}
							
						}
						break;
					default:
					case	NOBLOCKING			:
					case	SECTOR_NOTFOUND		:
					case	UNKNOWN_BLOCKING	:
						{
							// �̴�� ���� �ȴ�..
							bFoundDestination	= TRUE;
							break;
						}
					}

					break;
				}

			default:
			case	NOBLOCKING			:
			case	SECTOR_NOTFOUND		:
			case	UNKNOWN_BLOCKING	:
				{
					// �̴�� ���� �ȴ�..
					bFoundDestination	= TRUE;
					bNoCollision		= TRUE;
					break;
				}
			}
/*
			for( nTurn = 0 ; nTurn < g_snMaxTwistAngle ; nTurn += 10 )
			{
				if( bLeft )
					RwMatrixRotate( pMatrix , &yAxis , ( FLOAT ) ( nTurn ) , rwCOMBINEREPLACE );
				else
					RwMatrixRotate( pMatrix , &yAxis , ( FLOAT ) ( -nTurn ) , rwCOMBINEREPLACE );

				// At ���͸� Ư�� ���� ��ŭ ȸ�����Ѽ�
				// �׻��¿��� ��Ÿ�� �Ÿ� ��ŭ �̵���
				// ��������Ʈ�� ��ǥ���� ����.

				RwV3dTransformPoint( &vComputedAt , &at , pMatrix );

				// ������ (2005-03-29 ���� 7:12:36) : y���� ����...
				vComputedAt.y = 0;
				RwV3dNormalize( &vComputedAt , &vComputedAt );

				RwV3dScale( &vDest , &vComputedAt , g_fPathFindMinimumDistance );
				posDst.x = vDest.x + m_pcsSelfCharacter->m_stPos.x;
				posDst.z = vDest.z + m_pcsSelfCharacter->m_stPos.z;
				posDst.y = m_pcsAgcmMap->GetHeight( posDst.x , posDst.z );

				// �ɸ�����ġ���� posDst ���� �浹 �˻縦 �Ѵ�.
				switch( BlockingPositionCheck( &m_pcsSelfCharacter->m_stPos , &posDst , NULL , nTurn == 0 ? &bLeft : NULL ) )
				{
				case	GEOMETRY_BLOCKING	:
				case	OBJECT_BLOCKING		:
					{
						// �ٽ� �õ�..
						break;
					}

				default:
				case	NOBLOCKING			:
				case	SECTOR_NOTFOUND		:
				case	UNKNOWN_BLOCKING	:
					{
						// �̴�� ���� �ȴ�..
						bFoundDestination = TRUE;
						break;
					}
				}

				if( bFoundDestination ) break;
			}
*/
		}

		RwMatrixDestroy( pMatrix );
		
		if( bFoundDestination )
		{
			if( bNoCollision )
			{
				// �׳� ���� ok..
				SendMoveCharacter(pstPos, eMoveDirection, bPathFinding, TRUE);
				//if (AuPacket::GetLastProcessedFrameTick() >= 
				//	m_pcsAgpmCharacter->GetMoveLockFrameTick())
				//	m_pcsAgpmCharacter->MoveCharacter(m_pcsSelfCharacter, 
				//		pstPos, eMoveDirection, bPathFinding, TRUE);
			}
			else
			{
				// vDest�� �̵� �õ���.

				m_cPathNodeList.resetIndex();

				AgpdPathFindPoint	cCurrentPathPoint;

				// ������ (2005-03-22 ���� 4:46:25) : �Ųٷ� �־�� ������� ����.
				// ��ǥ��
				cCurrentPathPoint.m_fX = m_posMoveEnd.x;
				cCurrentPathPoint.m_fY = m_posMoveEnd.z;
				m_cPathNodeList.insert( 0, &cCurrentPathPoint );

				// ������ (2005-03-29 ���� 6:23:17) : 
				// ��������Ʈ ���� �ٷ��̵�.

				// ������
				//cCurrentPathPoint.m_fX = posDst.x;
				//cCurrentPathPoint.m_fY = posDst.z;
				//m_cPathNodeList.insert( 0, &cCurrentPathPoint );

				SendMoveCharacter( &posDst, eMoveDirection, bPathFinding, TRUE);
			}
			return FALSE;
		}
		else
		{
			// do nothing..
			// ���� ����..

			// ������ �����.
			return FALSE;
		}
	}
	else
	{
		SendMoveCharacter( pstPos , eMoveDirection, bPathFinding, TRUE);
	}
#else
	SendMoveCharacter( pstPos , eMoveDirection, bPathFinding, TRUE);
#endif // ENABLE_PATH_FIND_MODE


	// ������ (2005-03-11 ���� 11:35:29) : 
	// ���⼭
	// ������ġ�� ���������� �佺���ε� ������ ����.

	//�������� ���� ã�Ƶд�.

	/*
	// ������ (2005-03-22 ���� 12:16:42) : �ٸ� ��� ������ ���� �ּ�ó��.
	BOOL bResult = m_pcsAgpmPathFind->pathFind(  m_pcsSelfCharacter->m_stPos.x,
	m_pcsSelfCharacter->m_stPos.z,
	pstPos->x,
	pstPos->z,
	&m_cPathNodeList );

	CPriorityListNode< int , AgpdPathFindPoint *> *pcsNode;

	if( bResult )
	{
	#define	MAX_CONTROL_POINTS	100
	RwV3d	aCtrlPoints[ MAX_CONTROL_POINTS ];
	int		nCtrlPoints = 0;
	AgpdPathFindPoint	cCurrentPathPoint;

	while( pcsNode = m_cPathNodeList.GetNextPath() )
	{
	aCtrlPoints[ nCtrlPoints ].x = pcsNode->data->m_fX;
	aCtrlPoints[ nCtrlPoints ].y = 0.0f;
	aCtrlPoints[ nCtrlPoints ].z = pcsNode->data->m_fY;
	nCtrlPoints++;
	}

	m_cPathNodeList.resetIndex();

	if( nCtrlPoints >= 4 )
	{
	RpSpline * pSpline = RpSplineCreate( nCtrlPoints , rpSPLINETYPEOPENLOOPBSPLINE , aCtrlPoints );

	if( pSpline )
	{
	// ���� ����..
	RwV3d	pos , tan;

	// ���ö��� �����ؼ� ����Ʈ ���..
	int	nPeekCount = nCtrlPoints * 5 ; // ��Ʈ�� ����Ʈ�� 5��..
	for( int i = 1 ; i <= nPeekCount ; i ++ )
	{
	RpSplineFindPosition( pSpline , rpSPLINEPATHSMOOTH , ( FLOAT ) i / ( FLOAT ) nPeekCount , &pos , &tan );

	cCurrentPathPoint.m_fX = pos.x;
	cCurrentPathPoint.m_fY = pos.z;

	m_cPathNodeList.insert( 0, &cCurrentPathPoint );
	}

	}
	else
	{
	// �����ϸ� �׳� �����..
	for( int i = 0 ; i < nCtrlPoints ; i ++ )
	{
	cCurrentPathPoint.m_fX = aCtrlPoints[ nCtrlPoints - i - 1 ].x;
	cCurrentPathPoint.m_fY = aCtrlPoints[ nCtrlPoints - i - 1 ].z;

	m_cPathNodeList.insert( 0, &cCurrentPathPoint );
	}
	}
	}
	else
	{
	for( int i = 0 ; i < nCtrlPoints ; i ++ )
	{
	cCurrentPathPoint.m_fX = aCtrlPoints[ nCtrlPoints - i - 1 ].x;
	cCurrentPathPoint.m_fY = aCtrlPoints[ nCtrlPoints - i - 1 ].z;

	m_cPathNodeList.insert( 0, &cCurrentPathPoint );
	}
	}
	}

	pcsNode = m_cPathNodeList.GetNextPath();

	if( pcsNode )
	{
	AuPOS			csPos;

	csPos.x = pcsNode->data->m_fX;
	csPos.y = 0.0f;
	csPos.z = pcsNode->data->m_fY;

	SendMoveCharacter(&csPos, bMoveDirection, bPathFinding, TRUE);
	}
	else
	{
	/// -_-?;;;;
	SendMoveCharacter( pstPos , bMoveDirection, bPathFinding, TRUE);
	}

	*/

	return TRUE;
}

BOOL AgcmCharacter::StopSelfCharacter()
{
	if (!m_pcsSelfCharacter) return FALSE;
	if (m_pcsAgpmCharacter->IsActionBlockCondition(m_pcsSelfCharacter))	return FALSE;

	// ������ (2005-03-22 ���� 12:19:41) : ����~
	m_bAdjustMovement = FALSE;
	m_cPathNodeList.resetIndex();
	
	return TRUE;
}

BOOL AgcmCharacter::TurnSelfCharacter(FLOAT fDegreeX, FLOAT fDegreeY, BOOL bSendPacket)
{
	if (!m_pcsSelfCharacter) return FALSE;
	if (m_pcsAgpmCharacter->IsActionBlockCondition(m_pcsSelfCharacter))	return FALSE;

	if (m_pcsAgpmCharacter)
	{
		if (!m_pcsAgpmCharacter->TurnCharacter(m_pcsSelfCharacter, fDegreeX, fDegreeY)) return FALSE;
	}
	
	/*
	if (bSendPacket && m_ulPrevSendMoveTime + m_ulSendMoveDelay <= m_ulPrevTime)
	SendMoveCharacter();
	*/

	return TRUE;
}

BOOL AgcmCharacter::MoveTurnSelfCharacter(AuPOS * pstPos, FLOAT fDegreeX, FLOAT fDegreeY, BOOL bMoveDirection, BOOL bPathFinding)
{
	if (m_pcsAgpmCharacter->IsActionBlockCondition(m_pcsSelfCharacter))
		return FALSE;

	return TRUE;
}

BOOL AgcmCharacter::ActionSelfCharacter(AgpdCharacterActionType eType, INT32 lTargetCID, INT32 lSkillID, BOOL bForceAttack)
{
	//AuPOS			stCalcPos;
	AgpdCharacter	*pcsTargetChar = NULL;
	
	if (m_pcsAgpmCharacter)
		pcsTargetChar = m_pcsAgpmCharacter->GetCharacter(lTargetCID);
	
	if (!pcsTargetChar)	return FALSE;
	
	if (eType == AGPDCHAR_ACTION_TYPE_ATTACK)
	{
		if (m_pcsSelfCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD ||
			pcsTargetChar->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
			return FALSE;

		AgcdCharacter	*pstAgcdCharacter	= GetCharacterData(m_pcsSelfCharacter);

		if (m_pcsAgpmCharacter->IsActionBlockCondition(m_pcsSelfCharacter, AGPDCHAR_ACTION_BLOCK_TYPE_SKILL))
		{
			/*
			if (pstAgcdCharacter->m_stNextAction.m_eActionType != AGPDCHAR_ACTION_TYPE_SKILL)
			{
			pstAgcdCharacter->m_stNextAction.m_eActionType			= AGPDCHAR_ACTION_TYPE_ATTACK;
			pstAgcdCharacter->m_stNextAction.m_csTargetBase.m_eType	= APBASE_TYPE_CHARACTER;
			pstAgcdCharacter->m_stNextAction.m_csTargetBase.m_lID	= lTargetCID;
			pstAgcdCharacter->m_stNextAction.m_bForceAction			= bForceAttack;
			}
			*/

			AgpdCharacterAction	stNextAction;
			ZeroMemory(&stNextAction, sizeof(AgpdCharacterAction));

			stNextAction.m_eActionType	= AGPDCHAR_ACTION_TYPE_ATTACK;
			stNextAction.m_csTargetBase.m_eType	= pcsTargetChar->m_eType;
			stNextAction.m_csTargetBase.m_lID	= pcsTargetChar->m_lID;
			stNextAction.m_bForceAction			= bForceAttack;
			stNextAction.m_lUserData[0]			= lSkillID;

			SetNextAction(&stNextAction);

			return FALSE;
		}

		/*
		pstAgcdCharacter->m_stNextAction.m_eActionType			= AGPDCHAR_ACTION_TYPE_NONE;
		*/

		/*
		if (m_pcsAgpmCharacter->GetAttackIntervalMSec(m_pcsSelfCharacter) <= 0)
		return FALSE;
		*/

		if (m_bFollowAttackLock && m_lFollowAttackTargetID == pcsTargetChar->m_lID && m_pcsSelfCharacter->m_bMove)
			return TRUE;

		m_bFollowAttackLock		= FALSE;
		m_lFollowAttackTargetID	= AP_INVALID_CID;

		AgcmGuild* pcsAgcmGuild = (AgcmGuild*)GetModule("AgcmGuild");
		if (!m_pcsAgpmCharacter->CheckVaildNormalAttackTarget(m_pcsSelfCharacter, pcsTargetChar, bForceAttack) &&
			(pcsAgcmGuild && !pcsAgcmGuild->IsGuildBattleMember( m_pcsSelfCharacter, pcsTargetChar )) )
			return FALSE;

		// ���ݿ� �ʿ��� ����� �����ؾ� �ϴ� ��� ����� ����� �ִ��� �˻��Ѵ�.
		/////////////////////////////////////////////////////////////////////////////
		AgpdCharacterActionResultType	eActionCheckResult	= AGPDCHAR_ACTION_RESULT_TYPE_NONE;
		m_pcsAgpmCharacter->CheckActionRequirement(m_pcsSelfCharacter, &eActionCheckResult, eType);

		if (eActionCheckResult == AGPDCHAR_ACTION_RESULT_TYPE_NOT_ENOUGH_ARROW)
		{
			EnumCallback(AGCMCHAR_CB_ID_ACTION_RESULT_NOT_ENOUGH_ARROW, m_pcsSelfCharacter, NULL);
			return FALSE;
		}
		else if (eActionCheckResult == AGPDCHAR_ACTION_RESULT_TYPE_NOT_ENOUGH_BOLT)
		{
			EnumCallback(AGCMCHAR_CB_ID_ACTION_RESULT_NOT_ENOUGH_BOLT, m_pcsSelfCharacter, NULL);
			return FALSE;
		}
		else if (eActionCheckResult == AGPDCHAR_ACTION_RESULT_TYPE_NOT_ENOUGH_MP)
		{
			EnumCallback(AGCMCHAR_CB_ID_ACTION_RESULT_NOT_ENOUGH_MP, m_pcsSelfCharacter, NULL);
			return FALSE;
		}
		//m_pcsAgpmCharacter->SetActionBlockTime(m_pcsSelfCharacter, 
		//	m_pcsAgpmCharacter->GetAttackIntervalMSec(m_pcsSelfCharacter), 
		//	AGPDCHAR_ACTION_BLOCK_TYPE_SKILL);

		ApBase	csTargetCharacter;

		csTargetCharacter.m_eType = APBASE_TYPE_CHARACTER;
		csTargetCharacter.m_lID = lTargetCID;

		BOOL	bResult	= TRUE;

		if (m_bShowActionQueue)
		{
			CHAR	szTemp[16];

			sprintf(szTemp, "%d;", AGPDCHAR_ACTION_TYPE_ATTACK);
			m_szAttackSentAcc	+= szTemp;
		}

		if (IsSendActionTime(eType, GetSelfCharacter(), lTargetCID))
		{
			INT8	cComboInfo	= 0;
			if (!m_bStartAttack)
				cComboInfo	= 1;

			//@{ 2006/03/21 burumal			
			//bResult	= SendActionInfo(eType, lTargetCID, lSkillID, cComboInfo, bForceAttack);
			AgcdCharacter* pAgcdTargetChar = GetCharacterData(pcsTargetChar);
			ASSERT(pAgcdTargetChar);
			bResult	= SendActionInfo(eType, lTargetCID, lSkillID, cComboInfo, bForceAttack, CalcPickAtomicIndex(pAgcdTargetChar));			
			//@}

			if (bResult) {
				m_pcsAgpmCharacter->SetMoveLockFrameTick(
					g_pEngine->GetFrameTick());
				pstAgcdCharacter->m_ulNextAttackTime = m_ulCurTick + 
					m_pcsAgpmCharacter->GetAttackIntervalMSec(m_pcsSelfCharacter);
				pstAgcdCharacter->m_ulNextActionSelfCharacterTime = 
					m_ulCurTick + AGCD_CHARACTER_ACTION_SELF_CHARACTER_INTERVAL;
				m_lLastSendActionTarget	= lTargetCID;
				if (!m_bStartAttack)
					m_bFirstSendAttack	= TRUE;
			}
		}

		if (!pstAgcdCharacter->m_bIsLockTarget)
			pstAgcdCharacter->m_ulNextActionSelfCharacterTime	= 0;

		pstAgcdCharacter->m_bIsCastSkill	= FALSE;
		

		if (pstAgcdCharacter->m_bQueuedDeathEvent || (m_bStartAttack && m_pcsAgpmFactors))
		{
			if (m_pcsAgpmFactors->IsInRange(&m_pcsSelfCharacter->m_stPos, &pcsTargetChar->m_stPos, &m_pcsSelfCharacter->m_csFactor, &pcsTargetChar->m_csFactor, NULL))
			{				
				m_pcsAgpmCharacter->ActionCharacter(eType, m_pcsSelfCharacter, (ApBase *) pcsTargetChar, NULL, TRUE);
				
				SetAttackStatus(GetCharacterData(m_pcsSelfCharacter));
				SetAttackStatus(GetCharacterData(pcsTargetChar));

				StartAnimation(m_pcsSelfCharacter, AGCMCHAR_ANIM_TYPE_ATTACK);
			}
			else
			{
				int i = 0;
			}
		}
		else
		{
			int i = 0;
		}

		return bResult;
	}
	/*
	else if (nType == AGPDCHAR_ACTION_TYPE_SKILL)
	{
	return SendActionInfo(nType, lTargetCID, lSkillID);
	}
	*/

	//return FALSE;
	return TRUE; // FALSE�� ������ ���� �ʳ�?(BOB, 241003)
}

void	AgcmCharacter::CharacterPosIdleProcess()
{
	AgpdCharacter * pCharacter = GetSelfCharacter();

	if( pCharacter && pCharacter->m_stPos.y <= INVALID_HEIGHT )
	{
		// �ɸ��Ͱ� ���ӿ� �ִ�
		// ���� Ƣ�ܶ�.

		FLOAT	fHeight = this->m_pcsAgcmMap->GetHeight( pCharacter->m_stPos.x , pCharacter->m_stPos.z , SECTOR_MAX_HEIGHT );
		if( fHeight > INVALID_HEIGHT )
		{
			AuPOS	newPos = pCharacter->m_stPos;
			newPos.y = fHeight;
			this->m_pcsAgpmCharacter->UpdatePosition( pCharacter , & newPos );
		}
	}
}

void	AgcmCharacter::KeyboardIdleProcess()
{
	//@{ 2006/07/05 burumal
	/*
	BOOL	bKeyDownW = GetAsyncKeyState( 'W' ) < 0;
	BOOL	bKeyDownA = GetAsyncKeyState( 'A' ) < 0;
	BOOL	bKeyDownS = GetAsyncKeyState( 'S' ) < 0;
	BOOL	bKeyDownD = GetAsyncKeyState( 'D' ) < 0;

	if (m_bKeyDownW != bKeyDownW ||
		m_bKeyDownS != bKeyDownS ||
		m_bKeyDownA != bKeyDownA ||
		m_bKeyDownD != bKeyDownD )
	{
		// ��Ƽ�� ��� ���� ����..
		if( g_pEngine->GetActivate() != AgcEngine::AGCENGINE_NORMAL ) return;
		if( g_pEngine->GetCharCheckState() ) return;

		m_bKeyDownW = bKeyDownW;
		m_bKeyDownS = bKeyDownS; 
		m_bKeyDownA = bKeyDownA; 
		m_bKeyDownD = bKeyDownD; 

		SelfMoveDirection();
	}
	*/

	/*
	//[KTH] �� �������� �ڵ� ����(�ڵ��������α׷�)
	if( m_pcsSelfCharacter )
	{
		AgpdFactor* pFactorResult = (AgpdFactor*)m_pcsAgpmFactors->GetFactor( &m_pcsSelfCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT );
		
		//HP ����Ծ��ش�
		FLOAT fHp, fHpMax;
		m_pcsAgpmFactors->GetValue(pFactorResult, &fHp, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
		m_pcsAgpmFactors->GetValue(pFactorResult, &fHpMax, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP );

		float fHpRate = fHp * 100.f / fHpMax;
		if( fHpRate <= 20.f )
		{
		}

		//MP ����Ծ��ش�
		FLOAT fMp, fMpMax;
		m_pcsAgpmFactors->GetValue(pFactorResult, &fMp, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
		m_pcsAgpmFactors->GetValue(pFactorResult, &fMpMax, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP );

		float fMpRate = fMp * 100.f / fMpMax;
		if( fMpRate <= 20.f )
		{
		}

		//���� LV�� ���� ����� ���� ã�´�
		for( int i = 0; i<m_lCurCharacterNum; ++i )
		{
			//AgpdCharacter* pChar = GetCharacter( m_pCurCharacter[i] );
			//pChar->m_eType
			//pChar->m_stPos 
		}
	}
	*/


	// ��� �����찡 �� ������ ������ �۾��� ����Ǿ�� �ȵȴ�.
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )GetModule( "AgcmUIManager2" );
	if( pcmUIManager->IsOpenModalWindow() ) return;


	static bool bFirstUpdate = true;

	if ( bFirstUpdate )
	{		
		m_pcsAgcmUIHotkey = (AgcmUIHotkey*) GetModule("AgcmUIHotkey");
		ASSERT(m_pcsAgcmUIHotkey);				
		if ( !m_pcsAgcmUIHotkey)
			return;

		if ( m_pcsAgcmUIHotkey->IsRegistered() )
		{
			m_bMoveKeyEnabled = SetMovementKeys();
			bFirstUpdate = false;
		}

		return;
	}
	
	ASSERT(m_pcsAgcmUIHotkey);

	// ��Ƽ�� ��� ���� ����..
	if( g_pEngine->GetActivate() != AgcEngine::AGCENGINE_NORMAL ) 
		return;

	if( g_pEngine->GetCharCheckState() ) 
		return;

	// �̵�Ű ó��
	BOOL	bKeyDownW = FALSE;
	BOOL	bKeyDownA = FALSE;
	BOOL	bKeyDownS = FALSE;
	BOOL	bKeyDownD = FALSE;

	if ( m_bMoveKeyEnabled )
	{		
		INT32 nIdx;
		BOOL* pIsPressed[4] =
		{
			&bKeyDownW, 			
			&bKeyDownS,
			&bKeyDownA,
			&bKeyDownD
		};

		for ( nIdx = 0; nIdx < 4; nIdx++ )
		{
			*pIsPressed[nIdx] = 
				m_pcsAgcmUIHotkey->GetAsyncHotkeyState(m_pMovementHotkey[nIdx]->m_eType, m_nMoveVirtualKeys[nIdx]);
		}
	}
	else
	{
		bKeyDownW = GetAsyncKeyState( 'W' ) < 0;
		bKeyDownA = GetAsyncKeyState( 'A' ) < 0;
		bKeyDownS = GetAsyncKeyState( 'S' ) < 0;
		bKeyDownD = GetAsyncKeyState( 'D' ) < 0;
	}

	//#define	SEND_KEY_MOVE_INTERVAL	200
	static UINT32	_suResendInterval = 20;
	static UINT32	_suLastSendTime	= GetTickCount();
	UINT32	uCurrentTick = GetTickCount();

	if (m_bKeyDownW != bKeyDownW ||
		m_bKeyDownS != bKeyDownS ||
		m_bKeyDownA != bKeyDownA ||
		m_bKeyDownD != bKeyDownD || 
		( ( m_bKeyDownW || m_bKeyDownS || m_bKeyDownA || m_bKeyDownD ) && uCurrentTick - _suLastSendTime > _suResendInterval )
		)
	{
		_suLastSendTime = uCurrentTick;

		m_bKeyDownW = bKeyDownW;
		m_bKeyDownS = bKeyDownS; 
		m_bKeyDownA = bKeyDownA; 
		m_bKeyDownD = bKeyDownD; 

		SelfMoveDirection();
	}	
	//@}
}

BOOL AgcmCharacter::SelfMoveDirection()
{
	if( NULL == m_pcsSelfCharacter ) return FALSE;

	//@{ 2006/09/01 burumal		
	if ( m_bKeyDownW || m_bKeyDownS )
	{
		AgcmUIConsole* pcsAgcmUIConsole = (AgcmUIConsole*) GetModule("AgcmUIConsole");
		if ( pcsAgcmUIConsole )
			pcsAgcmUIConsole->getScriptEngine().execute("AgcmGlyph.HideDestMoveTarget()");
	}

	AgcmTargeting* pcsAgcmTargeting = (AgcmTargeting*) GetModule("AgcmTargeting");
	//@}

	if (m_bKeyDownW && m_bKeyDownS ||
		m_bKeyDownA && m_bKeyDownD)
	{
		m_pcsAgpmCharacter->StopCharacter(m_pcsSelfCharacter, NULL);		
		return SendStopCharacter();
	}

	m_bTurnLeft		= FALSE;
	m_bTurnRight	= FALSE;

	BOOL	bIsCameraMoving	= FALSE;

	EnumCallback(AGCMCHAR_CB_ID_IS_CAMERA_MOVING, &bIsCameraMoving, NULL);

	if (!bIsCameraMoving && !IsStrafeMove() )
	{
		if (m_bKeyDownA && !m_bKeyDownW && !m_bKeyDownS)
		{
			m_pcsAgpmCharacter->StopCharacter(m_pcsSelfCharacter, NULL);
			SendStopCharacter();
			m_bTurnLeft		= TRUE;
			return TRUE;
			//return TurnSelfCharacter(m_pcsSelfCharacter->m_fTurnX, m_pcsSelfCharacter->m_fTurnY + 10);
		}
		else if (m_bKeyDownD && !m_bKeyDownW && !m_bKeyDownS)
		{
			m_pcsAgpmCharacter->StopCharacter(m_pcsSelfCharacter, NULL);
			SendStopCharacter();
			m_bTurnRight	= TRUE;
			return TRUE;
			//return TurnSelfCharacter(m_pcsSelfCharacter->m_fTurnX, m_pcsSelfCharacter->m_fTurnY - 10);
		}
	}
	
	{
		INT32	lTurnAngle	= 0;

		if (m_bKeyDownW && m_bKeyDownA)
			lTurnAngle	= 45;
		else if (m_bKeyDownW && m_bKeyDownD)
			lTurnAngle	= 315;
		else if (m_bKeyDownS && m_bKeyDownA)
			lTurnAngle	= 135;
		else if (m_bKeyDownS && m_bKeyDownD)
			lTurnAngle	= 225;
		else if (m_bKeyDownW)
			lTurnAngle	= 0;
		else if (m_bKeyDownS)
			lTurnAngle	= 180;
		else if (m_bKeyDownA)
			lTurnAngle	= 90;
		else if (m_bKeyDownD)
			lTurnAngle	= 270;
		else
		{
			BOOL bKeyDownE = GetAsyncKeyState( 'E' ) < 0;
			if( bKeyDownE )
			{
				// �׳� ��� �ڴ�
				return TRUE;
			}
			else
			{
				m_pcsAgpmCharacter->StopCharacter(m_pcsSelfCharacter, NULL);		
				return SendStopCharacter();
			}
		}

		RwFrame * pFrame = RwCameraGetFrame( m_pCamera );
		RwMatrix * pMatrix = RwFrameGetLTM( pFrame );

		RwV3d	vAt = pMatrix->at;

		vAt.y = 0;
		RwV3dNormalize( &vAt , &vAt );
	
		RwMatrix matRotate;
		RwV3d	vAxisY;

		vAxisY.x = 0;
		vAxisY.y = 1;
		vAxisY.z = 0;

		RwMatrixRotate( &matRotate , &vAxisY , (float) lTurnAngle , rwCOMBINEREPLACE );
		RwV3dTransformPoint( &vAt , &vAt , &matRotate );

		//m_pcsAgpmCharacter->TurnCharacter(m_pcsSelfCharacter, 0, m_pcsAgpmCharacter->GetWorldDirection(pMatrix->at.x, pMatrix->at.z) + lTurnAngle);

		#ifdef _DEBUG
		{
			char strKeyState[ 6 ] ;
			if( m_bKeyDownW )	strKeyState[ 0 ] = 'W';
			else				strKeyState[ 0 ] = '_';
			if( m_bKeyDownA )	strKeyState[ 1 ] = 'A';
			else				strKeyState[ 1 ] = '_';
			if( m_bKeyDownS )	strKeyState[ 2 ] = 'S';
			else				strKeyState[ 2 ] = '_';
			if( m_bKeyDownD )	strKeyState[ 3 ] = 'D';
			else				strKeyState[ 3 ] = '_';

			strKeyState[ 4 ] = '\0';
		}
		#endif

		MOVE_DIRECTION eDirection = MD_FORWARD;
		if     ( m_bKeyDownW && m_bKeyDownA )	eDirection = MD_FORWARDLEFT		;
		else if( m_bKeyDownW && m_bKeyDownD )	eDirection = MD_FORWARDRIGHT	;
		else if( m_bKeyDownS && m_bKeyDownA )	eDirection = MD_BACKWARDLEFT	;
		else if( m_bKeyDownS && m_bKeyDownD )	eDirection = MD_BACKWARDRIGHT	;
		else if( m_bKeyDownA )					eDirection = MD_LEFT			;
		else if( m_bKeyDownD )					eDirection = MD_RIGHT			;
		else if( m_bKeyDownS )					eDirection = MD_BACKWARD		;
		else									eDirection = MD_FORWARD			;

		#ifdef _DEBUG
		char	str[ 256 ];
		switch( eDirection )
		{
		default:
		case MD_FORWARD			: strcpy( str , "MD_FORWARD				" ); break;
		case MD_FORWARDRIGHT	: strcpy( str , "MD_FORWARDRIGHT		" ); break;
		case MD_RIGHT			: strcpy( str , "MD_RIGHT				" ); break;
		case MD_BACKWARDRIGHT	: strcpy( str , "MD_BACKWARDRIGHT		" ); break;
		case MD_BACKWARD		: strcpy( str , "MD_BACKWARD			" ); break;
		case MD_BACKWARDLEFT	: strcpy( str , "MD_BACKWARDLEFT		" ); break;
		case MD_LEFT			: strcpy( str , "MD_LEFT				" ); break;
		case MD_FORWARDLEFT		: strcpy( str , "MD_FORWARDLEFT			" ); break;
		}

		strcat( str , "\n" );
		OutputDebugString( str );
		#endif

		if( m_pcsAgpmCharacter->IsRideOn( m_pcsSelfCharacter ) )
		{
			// Ż�Ϳ� Ÿ�� ������ , �԰����� ���� �ʴ´�.
			eDirection = MD_FORWARD;
		}

		return MoveSelfCharacter(&m_pcsSelfCharacter->m_stPos, eDirection, FALSE, TRUE, &vAt);
	}

	return TRUE;
}

BOOL AgcmCharacter::MoveForward()
{
	m_bKeyDownW	= TRUE;

	return SelfMoveDirection();
}

BOOL AgcmCharacter::MoveBackward()
{
	m_bKeyDownS	= TRUE;

	return SelfMoveDirection();
}

BOOL AgcmCharacter::MoveLeft()
{
	m_bKeyDownA	= TRUE;

	return SelfMoveDirection();
}

BOOL AgcmCharacter::MoveRight()
{
	m_bKeyDownD	= TRUE;

	return SelfMoveDirection();
}

BOOL AgcmCharacter::StopForward()
{
	m_bKeyDownW	= FALSE;

	return SelfMoveDirection();
}

BOOL AgcmCharacter::StopBackward()
{
	m_bKeyDownS	= FALSE;

	return SelfMoveDirection();
}

BOOL AgcmCharacter::StopLeft()
{
	m_bKeyDownA	= FALSE;

	return SelfMoveDirection();
}

BOOL AgcmCharacter::StopRight()
{
	m_bKeyDownD	= FALSE;

	return SelfMoveDirection();
}

BOOL AgcmCharacter::IsSendActionTime(AgpdCharacterActionType eType, AgpdCharacter *pcsCharacter, INT32 lTargetID)
{
	if (!pcsCharacter)
		return FALSE;

	switch (eType) {
	case AGPDCHAR_ACTION_TYPE_ATTACK:
		{
			m_ulCurTick	= GetClockCount();

			AgcdCharacter	*pstAgcdCharacter	= GetCharacterData(pcsCharacter);
			if (!pstAgcdCharacter)
				return FALSE;

			//if (!m_pcsAgpmCharacter->SyncActionPosition(eType, pcsCharacter, (ApBase *) m_pcsAgpmCharacter->GetCharacter(lTargetID), &pcsCharacter->m_stPos) &&
			//	lTargetID != m_lLastSendActionTarget)
			{
				if (m_ulCurTick < pstAgcdCharacter->m_ulNextAttackTime)
				{
					if (m_bShowActionQueue)
					{
						CHAR	szTemp[32];

						sprintf(szTemp, "-(%d);", pstAgcdCharacter->m_ulLastSendAttackTime + m_pcsAgpmCharacter->GetAttackIntervalMSec(pcsCharacter) - m_ulCurTick);
						m_szAttackSentAcc	+= szTemp;
					}

					return FALSE;
				}

				if (pstAgcdCharacter->m_ulLastSendAttackTime + m_pcsAgpmCharacter->GetAttackIntervalMSec(pcsCharacter) > m_ulCurTick)
				{
					if (m_bShowActionQueue)
					{
						CHAR	szTemp[32];

						sprintf(szTemp, "+(%d);", pstAgcdCharacter->m_ulLastSendAttackTime + m_pcsAgpmCharacter->GetAttackIntervalMSec(pcsCharacter) - m_ulCurTick);
						m_szAttackSentAcc	+= szTemp;
					}

					return FALSE;
				}
			}

			pstAgcdCharacter->m_ulLastSendAttackTime	= m_ulCurTick;

			return TRUE;
		}
		break;

	default:
		{
			return TRUE;
		}
		break;
	}

	return FALSE;
}

//@{ 2006/03/06 burumal
/*
BOOL AgcmCharacter::LockTarget(AgpdCharacter *pcsCharacter, INT32 lTargetID, BOOL bForceAttack)
*/
BOOL AgcmCharacter::LockTarget(AgpdCharacter* pcsCharacter, INT32 lTargetID, RpAtomic* pNewTargetFirstPickAtomic, BOOL bForceAttack)
//@}
{
	if (!pcsCharacter || lTargetID == AP_INVALID_CID || pcsCharacter->m_lID == lTargetID)
		return FALSE;

	AgcdCharacter	*pstAgcdCharacter	= GetCharacterData(pcsCharacter);
	if (!pstAgcdCharacter)
		return FALSE;

	pstAgcdCharacter->m_bIsLockTarget	= TRUE;
	pstAgcdCharacter->m_lLockTargetID	= lTargetID;
	pstAgcdCharacter->m_bIsForceAttack	= bForceAttack;

	//@{ 2006/03/06 burumal
	// character target lock ���۽� ��Ƽ picking atomic �� ���ؼ� ó���ϴ� ���
	// lock ���۶� ���õ� pickatomic �� Target Character�� first pick atomic ���� �� �����Ѵ�.
	AgpdCharacter* pTarget = m_pcsAgpmCharacter->GetCharacter(lTargetID);
	if ( pTarget && pNewTargetFirstPickAtomic )
	{
		AgcdCharacter* pTargetData = GetCharacterData(pTarget);
	
		if ( pTargetData )
		{
			bool bFind = false;
			RpAtomic* pOldFirst;
			RpAtomic* pCurTargetAtomic;
			pCurTargetAtomic = pOldFirst = pTargetData->m_pPickAtomic;

			do {
				if ( pCurTargetAtomic == pNewTargetFirstPickAtomic )
				{
					bFind = true;
					break;
				}
				pCurTargetAtomic = pCurTargetAtomic->next;
			} while ( pCurTargetAtomic != pOldFirst );

			if ( bFind )
				pTargetData->m_pPickAtomic = pNewTargetFirstPickAtomic;
		}
	}	
	//@}

	EnumCallback(AGCMCHAR_CB_ID_LOCK_TARGET, pcsCharacter, NULL);

	if (m_lFollowTargetCID != AP_INVALID_CID)
	{
		ReleaseFollowTarget();

		if (m_pcsSelfCharacter->m_bMove)
		{
			m_pcsAgpmCharacter->StopCharacter(m_pcsSelfCharacter, NULL);
			SendStopCharacter();
		}
	}

	return TRUE;
}

BOOL AgcmCharacter::UnLockTarget(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgcdCharacter	*pstAgcdCharacter	= GetCharacterData(pcsCharacter);
	if (!pstAgcdCharacter)
		return FALSE;

	pstAgcdCharacter->m_bIsLockTarget	= FALSE;
	pstAgcdCharacter->m_lLockTargetID	= AP_INVALID_CID;

	EnumCallback(AGCMCHAR_CB_ID_UNLOCK_TARGET, pcsCharacter, NULL);

	return TRUE;
}

BOOL AgcmCharacter::SelectTarget(AgpdCharacter *pcsCharacter, INT32 lTargetID)
{
	if (!pcsCharacter || lTargetID == AP_INVALID_CID)
		return FALSE;

	AgcdCharacter* pstAgcdCharacter	= GetCharacterData(pcsCharacter);
	if (!pstAgcdCharacter)
		return FALSE;

	pstAgcdCharacter->m_lSelectTargetID	= lTargetID;

	EnumCallback(AGCMCHAR_CB_ID_SELECT_TARGET, pcsCharacter, NULL);

	if (m_lFollowTargetCID != AP_INVALID_CID)
	{
		ReleaseFollowTarget();

		if (m_pcsSelfCharacter->m_bMove)
		{
			m_pcsAgpmCharacter->StopCharacter(m_pcsSelfCharacter, NULL);
			SendStopCharacter();
		}
	}

	return TRUE;
}

BOOL AgcmCharacter::UnSelectTarget(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgcdCharacter	*pstAgcdCharacter	= GetCharacterData(pcsCharacter);
	if (!pstAgcdCharacter)
		return FALSE;

	pstAgcdCharacter->m_lSelectTargetID	= AP_INVALID_CID;

	EnumCallback(AGCMCHAR_CB_ID_UNSELECT_TARGET, pcsCharacter, NULL);

	return TRUE;
}

BOOL AgcmCharacter::IsSelectTarget(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgcdCharacter	*pstAgcdCharacter	= GetCharacterData(pcsCharacter);
	if (!pstAgcdCharacter)
		return FALSE;

	if (pstAgcdCharacter->m_lSelectTargetID == AP_INVALID_CID)
		return FALSE;

	return TRUE;
}

INT32 AgcmCharacter::GetSelectTarget(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return AP_INVALID_CID;

	AgcdCharacter	*pstAgcdCharacter	= GetCharacterData(pcsCharacter);
	if (!pstAgcdCharacter)
		return AP_INVALID_CID;

	return pstAgcdCharacter->m_lSelectTargetID;
}

BOOL AgcmCharacter::SetCastSkill(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgcdCharacter	*pstAgcdCharacter	= GetCharacterData(pcsCharacter);
	if (!pstAgcdCharacter)
		return FALSE;

	pstAgcdCharacter->m_bIsCastSkill					= TRUE;
	pstAgcdCharacter->m_ulNextActionSelfCharacterTime	= GetClockCount();

	return TRUE;
}

BOOL AgcmCharacter::ResetCastSkill(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgcdCharacter	*pstAgcdCharacter	= GetCharacterData(pcsCharacter);
	if (!pstAgcdCharacter)
		return FALSE;

	pstAgcdCharacter->m_bIsCastSkill	= FALSE;

	return TRUE;
}

// 2005.09.09. steeple
//@{ Jaewon 20051101
// 'fadeOutTiming' parameter added.
BOOL AgcmCharacter::UpdateTransparent(AgpdCharacter* pcsCharacter, BOOL bStart, RwUInt16 fadeOutTiming)
{
	if(!pcsCharacter)
		return FALSE;

	AgcdCharacter* pcsAgcdCharacter = GetCharacterData(pcsCharacter);
	if(!pcsAgcdCharacter)
		return FALSE;

	//@{ Jaewon 20051024
	// If m_pClump == NULL, don't do anything.
	if(NULL == pcsAgcdCharacter->m_pClump)
		return FALSE;
	//@} Jaewon

	AgpdCharacter* pcsSelfCharacter = GetSelfCharacter();
	if(!pcsSelfCharacter)
		return FALSE;

	// 2005.10.10. steeple
	// ������ ���� ��Ƽ ������� Ȯ��.
	BOOL bPartyMember = FALSE;
	if(pcsCharacter->m_lID != pcsSelfCharacter->m_lID && m_pcsAgpmParty)
	{
		AgpdPartyADChar* pcsPartyADChar = m_pcsAgpmParty->GetADCharacter(pcsSelfCharacter);
		if(pcsPartyADChar && pcsPartyADChar->lPID != 0)
		{
			AgpdParty* pcsParty = m_pcsAgpmParty->GetParty(pcsPartyADChar->lPID);
			if(pcsParty)
			{
				if(m_pcsAgpmParty->IsMember(pcsParty, pcsCharacter->m_lID))
					bPartyMember = TRUE;
			}
		}
	}

	INT8 cOldTransparentType = pcsAgcdCharacter->m_cTransparentType;

	if(bStart)
	{
		if(pcsCharacter->m_lID == pcsSelfCharacter->m_lID ||
			(m_pcsAgpmPvP && m_pcsAgpmPvP->IsFriendCharacter(pcsSelfCharacter, pcsCharacter)) ||
			bPartyMember ||
			m_pcsAgpmCharacter->IsStatusHalfTransparent(pcsCharacter) ||
			m_pcsAgpmSummons->IsMySummons(pcsSelfCharacter, pcsCharacter->m_lID))
			pcsAgcdCharacter->m_cTransparentType = (INT8)AGCMCHAR_TRANSPARENT_HALF;
		else
			pcsAgcdCharacter->m_cTransparentType = (INT8)AGCMCHAR_TRANSPARENT_FULL;
	}
	else
	{
		// 2006.09.21. steeple
		// �������� ���� �� �̰� Ÿ�� �´�. �׷��� ���� ���¸� �����ؼ� ����� �Ѵ�.
		if(m_pcsAgpmCharacter->IsStatusFullTransparent(pcsCharacter))
		{
			if(pcsCharacter->m_lID == pcsSelfCharacter->m_lID ||
				(m_pcsAgpmPvP && m_pcsAgpmPvP->IsFriendCharacter(pcsSelfCharacter, pcsCharacter)) ||
				bPartyMember ||
				m_pcsAgpmCharacter->IsStatusHalfTransparent(pcsCharacter) ||
				m_pcsAgpmSummons->IsMySummons(pcsSelfCharacter, pcsCharacter->m_lID))
				pcsAgcdCharacter->m_cTransparentType = (INT8)AGCMCHAR_TRANSPARENT_HALF;
			else
				pcsAgcdCharacter->m_cTransparentType = (INT8)AGCMCHAR_TRANSPARENT_FULL;
		}
		else
			pcsAgcdCharacter->m_cTransparentType = (INT8)AGCMCHAR_TRANSPARENT_NONE;
	}

	//@{ Jaewon 20050912
	// Turn on/off the invisible fx & adjust its picking appropriately.
	if(cOldTransparentType != pcsAgcdCharacter->m_cTransparentType)
	{
		if(pcsAgcdCharacter->m_cTransparentType != (INT8)AGCMCHAR_TRANSPARENT_NONE)
		{
			if(cOldTransparentType != (INT8)AGCMCHAR_TRANSPARENT_NONE)
			{
				// �̷��ٸ� �ϴ� ���ش�. 2005.10.11. steeple
				m_pcsAgcmRender->InvisibleFxEnd(pcsAgcdCharacter->m_pClump);
				pcsAgcdCharacter->m_pClump->stType.eType &= ~ACUOBJECT_TYPE_INVISIBLE;
			}

			if(pcsAgcdCharacter->m_cTransparentType == (INT8)AGCMCHAR_TRANSPARENT_HALF)
			{
				m_pcsAgcmRender->InvisibleFxBegin(pcsAgcdCharacter->m_pClump, 77, fadeOutTiming);
			}
			else if(pcsAgcdCharacter->m_cTransparentType == (INT8)AGCMCHAR_TRANSPARENT_FULL)
			{
				m_pcsAgcmRender->InvisibleFxBegin(pcsAgcdCharacter->m_pClump, 0, fadeOutTiming);
				pcsAgcdCharacter->m_pClump->stType.eType |= ACUOBJECT_TYPE_INVISIBLE;
			}
		}
		else
		{
			m_pcsAgcmRender->InvisibleFxEnd(pcsAgcdCharacter->m_pClump);
			pcsAgcdCharacter->m_pClump->stType.eType &= ~ACUOBJECT_TYPE_INVISIBLE;
		}
	}
	//@} Jaewon

	// 2005.10.04. steeple
	// �ڱ� ĳ���Ͱ� �ƴϰ�, �����ϰ� �ǹ����� ��!!! ���� Ÿ���� �ϰ� ������ Ǯ���ش�.
	if(pcsCharacter->m_lID != pcsSelfCharacter->m_lID &&
		cOldTransparentType != (INT8)AGCMCHAR_TRANSPARENT_FULL &&
		pcsAgcdCharacter->m_cTransparentType == (INT8)AGCMCHAR_TRANSPARENT_FULL)
	{
		AgcdCharacter* pcsAgcdSelfCharacter = GetCharacterData(pcsSelfCharacter);
		if(pcsAgcdSelfCharacter)
		{
			if(pcsAgcdSelfCharacter->m_lLockTargetID == pcsCharacter->m_lID)
				UnLockTarget(pcsSelfCharacter);
			if(pcsAgcdSelfCharacter->m_lSelectTargetID == pcsCharacter->m_lID)
				UnSelectTarget(pcsSelfCharacter);
		}
	}

	EnumCallback(AGCMCHAR_CB_ID_UPDATE_TRANSPARENT, pcsCharacter, &cOldTransparentType);

	return TRUE;
}
//@} Jaewon

/*****************************************************************
*   Function : `InitCharacter
*   Comment  : Add Character�Ǿ����� CALL BACK �Լ� 
*   Date&Time : 2002-04-28, ���� 5:29
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
BOOL AgcmCharacter::CBInitCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if((!pClass) || (!pData)) return FALSE;

	AgcmCharacter *	pThis			= (AgcmCharacter*) pClass	;
	AgpdCharacter * pcsCharacter	= (AgpdCharacter*) pData	;

#ifdef USE_MFC
	if( pThis->m_pcsAgcmMap->IsRangeApplied() )
	{
		INT32 x1,x2,z1,z2;
		x1 = pThis->m_pcsAgcmMap->GetLoadRangeX1() + ( MAP_DEFAULT_DEPTH - pThis->m_pcsAgcmMap->GetLoadRangeX1() ) % MAP_DEFAULT_DEPTH;
		z1 = pThis->m_pcsAgcmMap->GetLoadRangeZ1() + ( MAP_DEFAULT_DEPTH - pThis->m_pcsAgcmMap->GetLoadRangeZ1() ) % MAP_DEFAULT_DEPTH;
		x2 = pThis->m_pcsAgcmMap->GetLoadRangeX2() - ( pThis->m_pcsAgcmMap->GetLoadRangeX2() % MAP_DEFAULT_DEPTH );
		z2 = pThis->m_pcsAgcmMap->GetLoadRangeZ2() - ( pThis->m_pcsAgcmMap->GetLoadRangeZ2() % MAP_DEFAULT_DEPTH );

		int				nDivisionCount	= 0;

		//#ifdef _USE_MAP_EXPANSION_
		// ���Ʒ��¿�� �ѵ������ �߰�����.
		x1 -= AgcmMap::EnumDivisionExpandValue;
		z1 -= AgcmMap::EnumDivisionExpandValue;
		x2 += AgcmMap::EnumDivisionExpandValue;
		z2 += AgcmMap::EnumDivisionExpandValue;
		//#endif //_USE_MAP_EXPANSION_

		INT32	uDivisionX1 , uDivisionZ1 , uDivisionX2 , uDivisionZ2;

		uDivisionX1	= GetDivisionXIndex( GetDivisionIndex( x1 , z1 ) );
		uDivisionZ1	= GetDivisionZIndex( GetDivisionIndex( x1 , z1 ) );
		uDivisionX2	= GetDivisionXIndex( GetDivisionIndex( x2 , z2 ) );
		uDivisionZ2	= GetDivisionZIndex( GetDivisionIndex( x2 , z2 ) );

		if( GetSectorStartX	( ArrayIndexToSectorIndexX( x1 ) ) < pcsCharacter->m_stPos.x &&
			pcsCharacter->m_stPos.x <= GetSectorEndX	( ArrayIndexToSectorIndexX( x2 ) ) &&
			GetSectorStartZ	( ArrayIndexToSectorIndexZ( z1 ) ) < pcsCharacter->m_stPos.z &&
			pcsCharacter->m_stPos.z <= GetSectorEndZ	( ArrayIndexToSectorIndexX( z2 ) ) )
		{
			return pThis->InitCharacter( pcsCharacter );
		}
		else
		{
			return TRUE;
		}
	}
	else
	{
		return pThis->InitCharacter( pcsCharacter );
	}
#else // USE_MFC

	// ���� ������Ʈ�� ��� , �����ڸ��� ���� TID�� ������ �Ŵ°��
	// Ȥ�ó� ������ ������ �ִ��� üũ�ؼ� �ߺ� �����Ŵ°� ���´�.
	static AgpmSiegeWar * pmAgpmSiegeWar = ( AgpmSiegeWar * ) pThis->GetModule("AgpmSiegeWar");
	if( pmAgpmSiegeWar->IsSiegeWarMonster( pcsCharacter ) )
	{
		INT32	lIndex = 0;
		vector< INT32 >	vecID;
		for (AgpdCharacter	* pcsCharacterSearch = pThis->m_pcsAgpmCharacter->GetCharSequence(&lIndex);
			pcsCharacterSearch;
			pcsCharacterSearch = pThis->m_pcsAgpmCharacter->GetCharSequence(&lIndex))
		{
			if( pcsCharacterSearch->m_lID	!= pcsCharacter->m_lID		&&
				pcsCharacterSearch->m_lTID1	== pcsCharacter->m_lTID1	&&
				pcsCharacterSearch->m_stPos	== pcsCharacter->m_stPos	)
			{
				vecID.push_back( pcsCharacterSearch->m_lID );
			}
		}

		for( vector< INT32 >::iterator iTer = vecID.begin();
			iTer != vecID.end();
			iTer++ )
		{
			INT32	nCID = *iTer;
			pThis->m_pcsAgpmCharacter->RemoveCharacter( nCID );

			#ifdef _DEBUG
			char	str[65535];
			sprintf( str , "Remove Corrupted SiegeWar Character ( %d , %s )\n" , nCID , pcsCharacter->m_pcsCharacterTemplate->m_szTName );
			OutputDebugString( str );
			#endif
		}
	}

	return pThis->InitCharacter( pcsCharacter );
#endif // USE_MFC
}

BOOL AgcmCharacter::LoadCharacterData(AgpdCharacter *pcsAgpdCharacter, BOOL bUseThread)
{
	PROFILE("AgcmCharacter::LoadCharacterData");
	AgcdCharacter*	pstAgcdCharacter = GetCharacterData(pcsAgpdCharacter);

#ifdef USE_MFC
	// 2005/02/17 ������
	// ���ø��� ������ , �޸� ���� �ü��� ������ �Ѵ�.
	// �������� ���ø� ���ϰ�� �۾��� ���ϱ� ������ ����..
	//return 

	if( NULL == pcsAgpdCharacter->m_pcsCharacterTemplate )
	{
		pcsAgpdCharacter->m_pcsCharacterTemplate = this->m_pcsAgpmCharacter->m_csACharacterTemplate.GetCharacterTemplate(1);
		ASSERT( NULL != pcsAgpdCharacter->m_pcsCharacterTemplate );
	}
#else
	// ���ø��� ���̸� ���α׷� �״´�...
	ASSERT( NULL != pcsAgpdCharacter->m_pcsCharacterTemplate );
#endif

	pstAgcdCharacter->m_pstAgcdCharacterTemplate = GetTemplateData(pcsAgpdCharacter->m_pcsCharacterTemplate);
	pstAgcdCharacter->m_pvClass = this;

	ASSERT(pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_lRefCount >= 0);
	++(pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_lRefCount);

	if (m_pcsAgcmResourceLoader && bUseThread)
	{
		m_pcsAgcmResourceLoader->RemoveLoadEntry(m_lLoaderIDInit, (PVOID) pcsAgpdCharacter, NULL);
		m_pcsAgcmResourceLoader->AddLoadEntry(m_lLoaderIDInit, (PVOID) pcsAgpdCharacter, NULL);
	}
	else
	{
		if (!InitCharacterBackThread(pcsAgpdCharacter)) return FALSE;
		if (!InitCharacterMainThread(pcsAgpdCharacter)) return FALSE;
	}

	pcsAgpdCharacter->m_ulCheckBlocking	= ApTileInfo::BLOCKNONE;

	pstAgcdCharacter->m_cTransparentType = (INT8)AGCMCHAR_TRANSPARENT_NONE;

	// set default animation speed
	CopyMemory(pstAgcdCharacter->m_afDefaultAnimSpeedRate,
		pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_afDefaultAnimSpeedRate,
		sizeof(pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_afDefaultAnimSpeedRate));

	return TRUE;
}

BOOL AgcmCharacter::InitCharacter(AgpdCharacter *pcsAgpdCharacter, BOOL bUseThread)
{
	PROFILE("AgcmCharacter::InitCharacter");

	if(!pcsAgpdCharacter)
		return FALSE;

	LoadCharacterData(pcsAgpdCharacter, bUseThread);

	//. 2006. 6. 26. Nonstopdj
	//. ������ ������ ĳ������ ��쿣 �̸� TemplateData�� �ε��Ѵ�.
	if( m_pcsAgpmCharacter->IsPolyMorph(pcsAgpdCharacter))
	{
		stCharacterPolyMorphEntry* pEntry = m_PolyMorphTableMap.Get( pcsAgpdCharacter->m_lFixTID );
		if( pEntry )
		{
			int nTIDCount = pEntry->m_vecTID.GetSize();
			for( int nCount = 0 ; nCount < nTIDCount ; nCount++ )
			{
				int* pTID = pEntry->m_vecTID.Get( nCount );
				if( pTID )
				{
					AgpdCharacterTemplate* ppdCharacterTemplate = m_pcsAgpmCharacter->GetCharacterTemplate( *pTID );
					if( ppdCharacterTemplate )
					{
						AgcdCharacterTemplate* pcdCharacterTemplate = GetTemplateData( ppdCharacterTemplate );
						if( pcdCharacterTemplate )
						{
							pcdCharacterTemplate->m_lRefCount++;
							if( !pcdCharacterTemplate->m_bLoaded )
							{
								if( !LoadTemplateData( ppdCharacterTemplate, pcdCharacterTemplate ) )
								{
									return FALSE;
								}
							}
						}
					}
				}
			}
		}
	}
		
	return TRUE;
}

BOOL AgcmCharacter::CBMoveCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if((!pClass) || (!pData)) return FALSE;

	AgcmCharacter*	pThis = (AgcmCharacter *) pClass;
	AgpdCharacter*	pcsAgpdCharacter = (AgpdCharacter *) pData;
	AgcdCharacter*	pstAgcdCharacter = pThis->GetCharacterData(pcsAgpdCharacter);

	pstAgcdCharacter->m_bIsCombo	= FALSE;

	if (!pThis->IsValidCharacter(pstAgcdCharacter))
		return TRUE;

	if (!pcsAgpdCharacter->m_bHorizontal)
	{
		if(pcsAgpdCharacter->m_bMoveFast)
		{
			AgcmCharacterAnimType eAnimType;

			//			if(	(!pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_paAnim[AGCMCHAR_ANIM_TYPE_RUN][pstAgcdCharacter->m_lAnimHandEquipType]) &&
			//				(!pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_paAnim[AGCMCHAR_ANIM_TYPE_RUN][0])											)
			//			if (	(!pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_csAnimationData.m_ppcsAnimation[AGCMCHAR_ANIM_TYPE_RUN][0]) &&
			//					(!pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_csAnimationData.m_ppcsAnimation[AGCMCHAR_ANIM_TYPE_RUN][pstAgcdCharacter->m_lAnimHandEquipType])	)
			if (	(!pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_pacsAnimationData[AGCMCHAR_ANIM_TYPE_RUN][0]) &&
				(!pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_pacsAnimationData[AGCMCHAR_ANIM_TYPE_RUN][pstAgcdCharacter->m_lCurAnimType2])	)
				eAnimType = AGCMCHAR_ANIM_TYPE_WALK;
			else
			{
				// eAnimType = AGCMCHAR_ANIM_TYPE_RUN;

				switch( pcsAgpdCharacter->m_eMoveDirection )
				{
				default:
				case MD_FORWARD			: eAnimType = AGCMCHAR_ANIM_TYPE_RUN			; break;
				case MD_FORWARDRIGHT	: eAnimType = AGCMCHAR_ANIM_TYPE_RUN			; break;
				case MD_RIGHT			: eAnimType = AGCMCHAR_ANIM_TYPE_RIGHTSIDE_STEP	; break;
				case MD_BACKWARDRIGHT	: eAnimType = AGCMCHAR_ANIM_TYPE_BACKWARD_STEP	; break;
				case MD_BACKWARD		: eAnimType = AGCMCHAR_ANIM_TYPE_BACKWARD_STEP	; break;
				case MD_BACKWARDLEFT	: eAnimType = AGCMCHAR_ANIM_TYPE_BACKWARD_STEP	; break;
				case MD_LEFT			: eAnimType = AGCMCHAR_ANIM_TYPE_LEFTSIDE_STEP	; break;
				case MD_FORWARDLEFT		: eAnimType = AGCMCHAR_ANIM_TYPE_RUN			; break;
				}
			}

			pThis->StartAnimation(pcsAgpdCharacter, eAnimType);
		}
		else
		{
			pThis->StartAnimation(pcsAgpdCharacter, AGCMCHAR_ANIM_TYPE_WALK);
		}
	}

	return TRUE;
}

BOOL AgcmCharacter::CBStopCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if((!pData) || (!pClass)) return FALSE;

	AgcmCharacter*	pThis = (AgcmCharacter *) pClass;
	AgpdCharacter*	pcsAgpdCharacter = (AgpdCharacter *) pData;
	AgcdCharacter*	pstAgcdCharacter = pThis->GetCharacterData(pcsAgpdCharacter);

	if (pcsAgpdCharacter->m_lID == pThis->m_lSelfCID)
	{
		pThis->m_bFollowAttackLock		= FALSE;
		pThis->m_lFollowAttackTargetID	= AP_INVALID_CID;

		pThis->EnumCallback(AGCMCHAR_CB_ID_SELF_STOP_CHARACTER, pData, NULL);

		// ���� ��ġ�� �̵�!

		if( !pThis->MoveReservedPath() )
		{
			// ������ (2005-03-22 ���� 12:19:41) : ����~
			pThis->m_bAdjustMovement = FALSE;
			pThis->m_cPathNodeList.resetIndex();
		}
	}

	if (pcsAgpdCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
	{
		if (!pThis->StartAnimation(pcsAgpdCharacter, AGCMCHAR_ANIM_TYPE_DEAD)) return FALSE;
	}
	else
	{
		// �޸��ų� ���� ���� �⺻�ڼ��� �ٲ۴�...
		//@{ Jaewon 20051129
		// Set to the default animation in case of social types also.
		if ((pstAgcdCharacter->m_eCurAnimType == AGCMCHAR_ANIM_TYPE_WALK			) ||
			(pstAgcdCharacter->m_eCurAnimType == AGCMCHAR_ANIM_TYPE_LEFTSIDE_STEP	) ||
			(pstAgcdCharacter->m_eCurAnimType == AGCMCHAR_ANIM_TYPE_RIGHTSIDE_STEP	) ||
			(pstAgcdCharacter->m_eCurAnimType == AGCMCHAR_ANIM_TYPE_BACKWARD_STEP	) ||
			(pstAgcdCharacter->m_eCurAnimType == AGCMCHAR_ANIM_TYPE_RUN				) ||
			(pstAgcdCharacter->m_eCurAnimType == AGCMCHAR_ANIM_TYPE_SOCIAL			) ||
			(pstAgcdCharacter->m_eCurAnimType == AGCMCHAR_ANIM_TYPE_SOCIAL_REPEAT	))
		//@} Jaewon
		{
			if (!pThis->StartAnimation(pcsAgpdCharacter, AGCMCHAR_ANIM_TYPE_WAIT)) return FALSE;
		}

		if (pcsAgpdCharacter->m_lID == pThis->m_lSelfCID &&
			pThis->m_lFollowTargetCID != AP_INVALID_CID)
		{
			AgpdCharacter	*pcsFollowTarget	= pThis->m_pcsAgpmCharacter->GetCharacter(pThis->m_lFollowTargetCID);
			if (pcsFollowTarget)
			{
				AuPOS	stDelta;

				// stDelta�� ���� ��ġ���� ��ǥ ��ġ������ Vector
				AuMath::V3DSubtract(&stDelta, &pcsAgpdCharacter->m_stPos, &pcsFollowTarget->m_stPos);

				stDelta.y	= 0;

				// fDiv�� �� �Ÿ�
				FLOAT	fDiv = AuMath::V3DLength(&stDelta);

				if (fDiv > pThis->m_lFollowDistance * AGCMCHARACTER_FOLLOW_ADJUST_RATE)
				{
					pThis->ReleaseFollowTarget();
				}
			}
		}
	}

	return TRUE;
}

BOOL AgcmCharacter::CBUpdateCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgcmCharacter::CBUpdateCharacter()");

	if((!pData) || (!pClass)) return FALSE;

	AgcmCharacter*	pThis = (AgcmCharacter *) pClass;
	AgpdCharacter*	pcsAgpdCharacter = (AgpdCharacter *) pData;

	AgcdCharacter*	pstAgcdCharacter = pThis->GetCharacterData(pcsAgpdCharacter);
	if( pstAgcdCharacter && pstAgcdCharacter->m_pClump && RpClumpGetFrame(pstAgcdCharacter->m_pClump) )
	{
		static DebugValueINT32	_sValue( 0 , "CBUpdateCharacter TID : %d" );
		_sValue = pcsAgpdCharacter->m_pcsCharacterTemplate->m_lID;	// ���ø� ���̵� ����..

		UINT32	ulDeltaTime = pThis->m_ulCurTick - pstAgcdCharacter->m_ulPrevTime;

		if(pstAgcdCharacter->m_ulPrevTime == 0)
			ulDeltaTime = 0;
		else if (ulDeltaTime > pstAgcdCharacter->m_csAnimation.GetDuration())
			ulDeltaTime = 0;

		pThis->LockFrame();

		pThis->UpdateMainAnimation(pcsAgpdCharacter, pstAgcdCharacter, ulDeltaTime);
		//pThis->UpdateSubAnimation(pstAgcdCharacter, ulDeltaTime);

		//@{ 2006/08/25 burumal
		AgpdCharacter* pcsTargetCharacter = pThis->m_pcsAgpmCharacter->GetCharacter(pcsAgpdCharacter->m_alTargetCID[0]);
		//@}

		//@{ Jaewon 20051208
		// If this is the player and not in a transformed state, update its look-at motion.
		//. 2006. 3. 7. Nonstopdj
		//. If this is the player and not AGCMCHAR_ANIM_TYPE_DEAD, update its look-at motion.
		//if ( pcsAgpdCharacter == pThis->GetSelfCharacter() 
		//	&& !pcsAgpdCharacter->m_bIsTrasform 
		//	&& !pstAgcdCharacter->m_bTransforming
		//	&& pstAgcdCharacter->m_eCurAnimType != AGCMCHAR_ANIM_TYPE_DEAD
		//	//@{ 2006/08/25 burumal			
		//	&& pstAgcdCharacter->m_eCurAnimType != AGCMCHAR_ANIM_TYPE_ATTACK 
		//	&& pcsTargetCharacter
		//	&& (pcsTargetCharacter->IsDead() == FALSE)
		//	)
		//	//@}
		//{
		//	pThis->UpdateLookAt(pThis->m_pcsAgcmRender->m_ulCurTickDiff);
		//}
		//@} Jaewon

		//@{ 2006/08/25 burumal		
		pThis->UpdateBendOver(pThis->m_pcsAgcmRender->m_ulCurTickDiff, pcsAgpdCharacter, pcsTargetCharacter);

		RwFrameUpdateObjects(RpClumpGetFrame(pstAgcdCharacter->m_pClump));

		//@{ 2006/02/20 burumal

//		if ( !pstAgcdCharacter->m_bStop && pstAgcdCharacter->m_pstNodeFrame )
//		{
//			RwMatrix *			pstNodeLTM	= RwFrameGetLTM(pstAgcdCharacter->m_pstNodeFrame);
//			RwMatrix *			pstClumpLTM	= RwFrameGetLTM(RpClumpGetFrame(pstAgcdCharacter->m_pClump));
//
//			if (!(pstAgcdCharacter->m_lStatus & AGCMCHAR_STATUS_FLAG_START_ANIM) && pstAgcdCharacter->m_pPickAtomic)
//			{
//				// ������ ( 2006/01/16 ) : �÷����߿� ������ ��ŷ ������� ��ġ�� ��߳��� ���� ������ �ּ�ó��
//				//. 2006. 1. 24. Nonstopdj
//				//. �ö��׸��� ��ŷ�ڽ���ġ ������ �ּ�����.
//				RwMatrix			stInvMatrix1;
//				RwMatrix			stInvMatrix2;
//
//				RwMatrixInvert(&stInvMatrix1, pstNodeLTM);
//				RwMatrixInvert(&stInvMatrix2, pstClumpLTM);
//				RwFrameTransform(RpAtomicGetFrame(pstAgcdCharacter->m_pPickAtomic), pstClumpLTM, rwCOMBINEREPLACE);
//				RwFrameTransform(RpAtomicGetFrame(pstAgcdCharacter->m_pPickAtomic), &stInvMatrix1, rwCOMBINEPOSTCONCAT);
//
//				RwV3dSub(&pstAgcdCharacter->m_vOffsetBSphere, &pstClumpLTM->pos, &pstNodeLTM->pos);			
//				// ������ ( 2006/01/16 ) : �÷����߿� ������ ��ŷ ������� ��ġ�� ��߳��� ���� ������ �ּ�ó��
//
//				pstAgcdCharacter->m_lStatus |= AGCMCHAR_STATUS_FLAG_START_ANIM;
//			}
//
//
//			//@{ kday 20050609
//			//pstAgcdCharacter->m_pClump->stType.boundingSphere	= pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_stBSphere;
//
//			////RwV3dScale(&pstAgcdCharacter->m_pClump->stType.boundingSphere.center, &pstAgcdCharacter->m_pClump->stType.boundingSphere.center, 1.f/pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_fScale);
//
//			//pstAgcdCharacter->m_pClump->stType.boundingSphere.center.x	-= (pstClumpLTM->pos.x - pstNodeLTM->pos.x - pstAgcdCharacter->m_vOffsetBSphere.x);// * pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_fScale;
//			//pstAgcdCharacter->m_pClump->stType.boundingSphere.center.y	-= (pstClumpLTM->pos.y - pstNodeLTM->pos.y - pstAgcdCharacter->m_vOffsetBSphere.y);// * pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_fScale;
//			//pstAgcdCharacter->m_pClump->stType.boundingSphere.center.z	-= (pstClumpLTM->pos.z - pstNodeLTM->pos.z - pstAgcdCharacter->m_vOffsetBSphere.z);// * pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_fScale;
//			{
//
//				RwMatrix			mat = pstAgcdCharacter->m_pPickAtomic ? *RwFrameGetLTM(RpAtomicGetFrame(pstAgcdCharacter->m_pPickAtomic)) : *RwFrameGetLTM(RpClumpGetFrame(pstAgcdCharacter->m_pClump));
//				RwMatrix			stInvMatrix2;
//				RwMatrixInvert(&stInvMatrix2, pstClumpLTM);
//				RwMatrixTransform ( &mat, &stInvMatrix2, rwCOMBINEPOSTCONCAT );
//
//				RwV3dTransformPoint(&pstAgcdCharacter->m_pClump->stType.boundingSphere.center
//					, &pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_stBSphere.center
//					, &mat//pstAgcdCharacter->m_pPickAtomic ? RwFrameGetLTM(RpAtomicGetFrame(pstAgcdCharacter->m_pPickAtomic)) : RwFrameGetLTM(RpClumpGetFrame(pstAgcdCharacter->m_pClump))
//					);
//
//			}
//			//@} kday
//
//			pThis->m_pcsAgcmRender->UpdateBoundingSphere(pstAgcdCharacter->m_pClump);
//		}
		
		//@{ 2006/03/08 burumal				
		RpAtomic* pCurPickAtomic = pstAgcdCharacter->m_pOrgPickAtomic;

		RwMatrix* pstClumpLTM = RwFrameGetLTM(RpClumpGetFrame(pstAgcdCharacter->m_pClump));
		
		if ( !pstAgcdCharacter->m_bStop && pstAgcdCharacter->m_pstNodeFrame[0] )
		{
			if ( !(pstAgcdCharacter->m_lStatus & AGCMCHAR_STATUS_FLAG_START_ANIM) )
			{
				RwMatrix* pstNodeLTM = RwFrameGetLTM(pstAgcdCharacter->m_pstNodeFrame[0]);

				if ( pCurPickAtomic && RpAtomicGetFrame(pCurPickAtomic) )
				{
					RwMatrix	stInvMatrix1;
					//RwMatrix	stInvMatrix2;

					RwMatrixInvert(&stInvMatrix1, pstNodeLTM);

					RwFrameTransform(RpAtomicGetFrame(pCurPickAtomic), pstClumpLTM, rwCOMBINEREPLACE);
					RwFrameTransform(RpAtomicGetFrame(pCurPickAtomic), &stInvMatrix1, rwCOMBINEPOSTCONCAT);

					RwV3dSub(&(pstAgcdCharacter->m_vOffsetBSphere), &pstClumpLTM->pos, &pstNodeLTM->pos);
				}

				pstAgcdCharacter->m_lStatus |= AGCMCHAR_STATUS_FLAG_START_ANIM;
			}
		
			// ù��° Pick Atomic�� �̿� bs�� center�� ���
			RwMatrix stInvMat;
			RwMatrixInvert(&stInvMat, pstClumpLTM);

			pCurPickAtomic = pstAgcdCharacter->m_pOrgPickAtomic;

			RwMatrix matTemp = pCurPickAtomic ? *RwFrameGetLTM(RpAtomicGetFrame(pCurPickAtomic)) : *RwFrameGetLTM(RpClumpGetFrame(pstAgcdCharacter->m_pClump));
			RwMatrixTransform (&matTemp, &stInvMat, rwCOMBINEPOSTCONCAT );

			RwV3dTransformPoint(&pstAgcdCharacter->m_pClump->stType.boundingSphere.center,
									&pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_stBSphere.center, &matTemp);			

			/*
			// picking atomic���� bs�� update ��Ų��.

			//. code blocking by nonstopdj
			//pCurPickAtomic = pstAgcdCharacter->m_pOrgPickAtomic;
			//RpAtomic* pCurTemplatePickAtomic = pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_pPickingAtomic;

			//do {
			//	if ( !pCurPickAtomic || !pCurTemplatePickAtomic )
			//		break;

			//	matTemp = *RwFrameGetLTM(RpAtomicGetFrame(pCurPickAtomic));

			//	matTemp.pos.x *= pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_fScale;
			//	matTemp.pos.y *= pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_fScale;
			//	matTemp.pos.z *= pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_fScale;

			//	RwMatrixTransform(&matTemp, &stInvMat, rwCOMBINEPOSTCONCAT);

			//	RwV3dTransformPoint(&pCurPickAtomic->boundingSphere.center, &pCurTemplatePickAtomic->boundingSphere.center, &matTemp);

			//	pCurPickAtomic = pCurPickAtomic->next;
			//	pCurTemplatePickAtomic = pCurTemplatePickAtomic->next;

			//} while ( pCurPickAtomic != pstAgcdCharacter->m_pOrgPickAtomic );
			//.
			*/
		}
		//@}

		pThis->UnlockFrame();

		pstAgcdCharacter->m_ulPrevTime =  pThis->m_ulCurTick;

		if (pThis->m_bShowPickingInfo)
		{			
			//@{ 2006/03/01 burumal
			pCurPickAtomic = pstAgcdCharacter->m_pPickAtomic;
			do {
				RwUtilAtomicRenderWireMesh(pCurPickAtomic, &RWUTIL_BLUE);
				
				RwSphere sphere	= pCurPickAtomic->boundingSphere;
				RwV3dTransformPoint(&sphere.center,&sphere.center,RwFrameGetLTM(RpAtomicGetFrame(pstAgcdCharacter->m_pPickAtomic)));
				//RwV3dTransformPoint(&sphere.center, &sphere.center, RwFrameGetLTM(RpClumpGetFrame(pstAgcdCharacter->m_pClump)));
				AcuObjecWire::bGetInst().bRenderSphere(sphere);			

				pCurPickAtomic = pCurPickAtomic->next;
			} while ( pCurPickAtomic != pstAgcdCharacter->m_pPickAtomic && pCurPickAtomic );
			//@}
		}

		if ( pThis->m_bShowBoundingSphere )
		{
			//@{ kday 20050609
			RwSphere	sphere	= pstAgcdCharacter->m_pClump->stType.boundingSphere;
			RwV3dTransformPoint(&sphere.center,&sphere.center,RwFrameGetLTM(RpClumpGetFrame(pstAgcdCharacter->m_pClump)));
			AcuObjecWire::bGetInst().bRenderSphere( sphere );

			//RwMatrix	stLTM	= *(RwFrameGetLTM(RwCameraGetFrame(pThis->GetCamera())));
			//stLTM.pos.x			= pstAgcdCharacter->m_pClump->stType.boundingSphere.center.x;
			//stLTM.pos.y			= pstAgcdCharacter->m_pClump->stType.boundingSphere.center.y;
			//stLTM.pos.z			= pstAgcdCharacter->m_pClump->stType.boundingSphere.center.z;
			//UtilRenderSphere(&pstAgcdCharacter->m_pClump->stType.boundingSphere, &stLTM, NULL);
			//@} kday
		}
	}

	return TRUE;
}

BOOL AgcmCharacter::CBUpdateCharacter2(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmCharacter			*pThis				= (AgcmCharacter *)		pClass;
	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)		pData;
	UINT32					ulClockCount		= (UINT32)				pCustData;
	INT32					lActionInterval		= AGCD_CHARACTER_ACTION_SELF_CHARACTER_INTERVAL;

	AgcdCharacter			*pstAgcdCharacter	= pThis->GetCharacterData(pcsCharacter);
	if (!pstAgcdCharacter)
		return FALSE;

	// 2005. 8. 17 Nonstopdj
	//
	// if(Player Character && (AniType == RUN || AniType == WALK))
	//		if(Current Position in Water Geometry)
	//			EnumCallback�� �̿��Ͽ� EventWaveFxTrigger()�� call
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	if(  pThis->m_pcsAgpmCharacter->IsPC(pcsCharacter) &&
		(	(pstAgcdCharacter->m_eCurAnimType == AGCMCHAR_ANIM_TYPE_WALK			) ||
			(pstAgcdCharacter->m_eCurAnimType == AGCMCHAR_ANIM_TYPE_LEFTSIDE_STEP	) ||
			(pstAgcdCharacter->m_eCurAnimType == AGCMCHAR_ANIM_TYPE_RIGHTSIDE_STEP	) ||
			(pstAgcdCharacter->m_eCurAnimType == AGCMCHAR_ANIM_TYPE_BACKWARD_STEP	) ||
			(pstAgcdCharacter->m_eCurAnimType == AGCMCHAR_ANIM_TYPE_RUN				)	) )
	{
		float _CurrentWaterHeight;

		static AgcmWater * _spcsAgcmWater = ( AgcmWater * ) pThis->GetModule( "AgcmWater" ) ;
		if( _spcsAgcmWater ) {
			BOOL _bIsInWater = _spcsAgcmWater->GetWaterHeight(pcsCharacter->m_stPos.x, 
				pcsCharacter->m_stPos.z, 
				&_CurrentWaterHeight);

			if( _bIsInWater &&  pcsCharacter->m_stPos.y <= _CurrentWaterHeight)
			{
				if(pstAgcdCharacter->m_ulStartWaveFxTime == 0)
					pstAgcdCharacter->m_ulStartWaveFxTime =  pThis->m_ulCurTick;

				if(pThis->m_ulCurTick - pstAgcdCharacter->m_ulStartWaveFxTime > 200 )
				{	
					float _fTempOffest = _CurrentWaterHeight - pcsCharacter->m_stPos.y;

					pThis->EnumCallback(AGCMCHAR_CB_ID_WAVE_FX_TRIGGER, pData, (PVOID*)(&_fTempOffest));

					//effect ���۽ð� �ʱ�ȭ
					pstAgcdCharacter->m_ulStartWaveFxTime = 0;
				}
			}
		}
	}
	else
	{
		pstAgcdCharacter->m_ulStartWaveFxTime = 0;
	}


	// pcsCharacter�� ����ĳ�����ΰ�� ������ Ÿ���� ��ŷ�ϰ� �ִ��� ���캻��.
	// ��ŷ ���̶�� ���� ���� �ð��� ������� ActionSelfCharacter()�� ȣ���� �׼��� �����Ѵ�.
	//////////////////////////////////////////////////////////////////////////////////////
	if (pThis->m_lSelfCID == pcsCharacter->m_lID)
	{
		/*
		if (pstAgcdCharacter->m_stNextAction.m_eActionType != AGPDCHAR_ACTION_TYPE_NONE)
		{
		if (pThis->ProcessReservedAction(pcsCharacter))
		pstAgcdCharacter->m_stNextAction.m_eActionType	= AGPDCHAR_ACTION_TYPE_NONE;
		}
		else
		*/
		{
			AgpdCharacterAction	*pstNextAction	= pThis->GetNextAction();

			if (pstNextAction->m_eActionType != AGPDCHAR_ACTION_TYPE_NONE &&
				!pThis->m_pcsAgpmCharacter->IsActionBlockCondition(pcsCharacter, AGPDCHAR_ACTION_BLOCK_TYPE_ALL))
			{
				if (pThis->m_bShowActionQueue)
					pThis->m_szAttackSentAcc	+= "/";

				pThis->ProcessNextAction();
			}
			else if (pstNextAction->m_eActionType == AGPDCHAR_ACTION_TYPE_NONE &&
				pstAgcdCharacter && pstAgcdCharacter->m_bIsLockTarget && pstAgcdCharacter->m_lLockTargetID != AP_INVALID_CID && !pstAgcdCharacter->m_bIsCastSkill)
			{
				if (pstAgcdCharacter->m_ulNextActionSelfCharacterTime <= ulClockCount)
				{
					if (pThis->m_bShowActionQueue)
						pThis->m_szAttackSentAcc	+= "*";

					if (!pThis->ActionSelfCharacter(AGPDCHAR_ACTION_TYPE_ATTACK, pstAgcdCharacter->m_lLockTargetID, pstAgcdCharacter->m_bIsForceAttack))
					{
						if (!pThis->m_pcsAgpmCharacter->IsActionBlockCondition(pcsCharacter, AGPDCHAR_ACTION_BLOCK_TYPE_ALL))
						{
							//							pstAgcdCharacter->m_lLockTargetID	= AP_INVALID_CID;
							//							lActionInterval						= pThis->m_pcsAgpmCharacter->GetAttackIntervalMSec(pcsCharacter);
						}
					}
					else
					{
						lActionInterval						= pThis->m_pcsAgpmCharacter->GetAttackIntervalMSec(pcsCharacter);
					}

					pstAgcdCharacter->m_ulNextActionSelfCharacterTime = ulClockCount + lActionInterval;
				}

				/*
				if (pstAgcdCharacter->m_ulNextAttackTime <= ulClockCount)
				{
				pThis->ActionSelfCharacter(AGPDCHAR_ACTION_TYPE_ATTACK, pstAgcdCharacter->m_lLockTargetID);
				}
				*/
			}
		}

		// ����� Move�� �ִ��� ����.
		if (pThis->m_bReservedMove && pThis->m_ulLastSendMoveTime + AGCMCHARACTER_SEND_MOVE_INTERVAL <= ulClockCount)
		{
			pThis->MoveSelfCharacter(&pThis->m_stReservedMoveDestPos, pThis->m_eReservedMoveDirection, pThis->m_bReservedMovePathFinding, pThis->m_bReservedMoveMoveFast, &pThis->m_stReservedMoveAt);
		}

		if (pThis->m_bTurnLeft)
		{
			pThis->TurnSelfCharacter(pThis->m_pcsSelfCharacter->m_fTurnX, pThis->m_pcsSelfCharacter->m_fTurnY + 3);
			pThis->EnumCallback(AGCMCHAR_CB_ID_TURN_CAMERA, &pThis->m_bTurnLeft, &pThis->m_bTurnRight);
		}
		else if (pThis->m_bTurnRight)
		{
			pThis->TurnSelfCharacter(pThis->m_pcsSelfCharacter->m_fTurnX, pThis->m_pcsSelfCharacter->m_fTurnY - 3);
			pThis->EnumCallback(AGCMCHAR_CB_ID_TURN_CAMERA, &pThis->m_bTurnLeft, &pThis->m_bTurnRight);
		}
	}
	// pcsCharacter�� ����ĳ���Ͱ� �ƴ� ��� ���� �޺� ������ ���������� ����.
	// �޺� �������̶�� ��� �޺������� �ϵ��� �ִϸ��̼� ��Ų��.
	//////////////////////////////////////////////////////////////////////////////////////
	else
	{
		if (pstAgcdCharacter->m_bIsCombo)
		{
			AgpdCharacter	*pcsTargetCharacter	= pThis->m_pcsAgpmCharacter->GetCharacter(pcsCharacter->m_alTargetCID[0]);

			// ������ �ð����� ������ ���� �ʾҴٸ� ������ ����� �Ѵ�. �ð��� üũ�Ѵ�.
			if (pstAgcdCharacter->m_ulNextAttackTime + AGCD_CHARACTER_MAX_WAIT_TIME_FOR_COMBO_ACTION <= ulClockCount ||
				!pcsTargetCharacter ||
				pcsTargetCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
			{
				pstAgcdCharacter->m_bIsCombo = FALSE;
			}
			// ���������� �ִϸ��̼� ��Ų �ð��� ���� �̳��� ��� �޺��� �� �� �ִ� �ð��� �ǵ��� �ִϸ��̼� �����ش�.
			else if (pstAgcdCharacter->m_ulNextActionSelfCharacterTime <= ulClockCount)
			{
				pThis->SetAttackStatus(pstAgcdCharacter);
				pThis->SetAttackStatus(pThis->GetCharacterData(pcsTargetCharacter));

				if (!pThis->StartAnimation(pcsCharacter, AGCMCHAR_ANIM_TYPE_ATTACK)) return FALSE;

				pstAgcdCharacter->m_ulNextActionSelfCharacterTime = ulClockCount + AGCD_CHARACTER_ACTION_SELF_CHARACTER_INTERVAL;
			}
		}
	}

	INT32	lIndex	= 0;
	AgcmCharacterActionQueueData	*pcsActionData	= pThis->GetSequenceActionData(pcsCharacter, &lIndex);
	while (pcsActionData)
	{
		// �� �׼��� ���� �ð��� ����ؼ� ���� �����ٰ� �����ش�.
		if (pcsActionData->ulProcessTime < ulClockCount)
		{
			// pcsActionData�� ó�����ش�.
			////////////////////////////////////////////////////////////

			BOOL	bRemoveActionData	= TRUE;

			switch (pcsActionData->eActionType) {
			case AGPDCHAR_ACTION_TYPE_NONE:
				{
					// bDeath�� ���õǾ� �ִ��� ����.
					if (pcsActionData->bDeath && pstAgcdCharacter->m_lNumActionData == 1)
					{
						pstAgcdCharacter->m_bQueuedDeathEvent = FALSE;

						pThis->m_pcsAgpmCharacter->UpdateActionStatus(pcsCharacter, AGPDCHAR_STATUS_DEAD, FALSE);

						if (pThis->m_pcsSelfCharacter && pcsActionData->lActorID == pThis->m_pcsSelfCharacter->m_lID)
							pThis->EnumCallback(AGCMCHAR_CB_ID_KILL_CHARACTER, pcsCharacter, pThis->m_pcsSelfCharacter);
					}
					else
					{
						bRemoveActionData = FALSE;
					}
				}
				break;

			default:
				{
					// ó���Ѵ�.
					pThis->ApplyActionData(pcsCharacter, pcsActionData);
				}
				break;
			}

			if (bRemoveActionData)
			{
				// ������ ó�������ϱ� ���� �ִ��� ����������.
				pThis->RemoveActionData(pcsCharacter, lIndex - 1);
			}
		}

		pcsActionData	= pThis->GetSequenceActionData(pcsCharacter, &lIndex);
	}

	return TRUE;
}

BOOL AgcmCharacter::InclinePlaneQuadruped( RwMatrix* pstModelling, AgcdCharacter* pstAgcdCharacter, const RwV3d* pstPos )
{
	int nFL, nFR, nBL, nBR;
	RpHAnimHierarchy* pInHierarchy = NULL;
	if( pstAgcdCharacter->m_pRide ) {
		pInHierarchy = pstAgcdCharacter->m_pRide->m_pInHierarchy;
		nFL = AGCMCHAR_FOOT_FL_ID;
		nFR = AGCMCHAR_FOOT_FR_ID;
		nBL = AGCMCHAR_FOOT_BL_ID;
		nBR = AGCMCHAR_FOOT_BR_ID;
	}
	else {
		pInHierarchy = pstAgcdCharacter->m_pInHierarchy;
		nFL = AGCMCHAR_FOOT_FL_ID;
		nFR = AGCMCHAR_FOOT_FR_ID;
		nBL = AGCMCHAR_FOOT_BL_ID;
		nBR = AGCMCHAR_FOOT_BR_ID;
	}
	if( pInHierarchy == NULL )
		return FALSE;
	RwInt32 lFLNodeIndex = RpHAnimIDGetIndex( pInHierarchy, nFL );
	if( lFLNodeIndex == -1 )
		return FALSE;
	RwInt32 lFRNodeIndex = RpHAnimIDGetIndex( pInHierarchy, nFR );
	if( lFRNodeIndex == -1 )
		return FALSE;
	RwInt32 lBLNodeIndex = RpHAnimIDGetIndex( pInHierarchy, nBL );
	if( lBLNodeIndex == -1 )
		return FALSE;
	RwInt32 lBRNodeIndex = RpHAnimIDGetIndex( pInHierarchy, nBR );
	if( lBRNodeIndex == -1 )
		return FALSE;

	RwFrame* pFLFrame = pInHierarchy->pNodeInfo[lFLNodeIndex].pFrame;
	RwFrame* pFRFrame = pInHierarchy->pNodeInfo[lFRNodeIndex].pFrame;
	RwFrame* pBLFrame = pInHierarchy->pNodeInfo[lBLNodeIndex].pFrame;
	RwFrame* pBRFrame = pInHierarchy->pNodeInfo[lBRNodeIndex].pFrame;

	if( pFLFrame == NULL )
		return FALSE;
	if( pFRFrame == NULL )
		return FALSE;
	if( pBLFrame == NULL )
		return FALSE;
	if( pBRFrame == NULL )
		return FALSE;

	RwV3d* pvtxFLPos = RwMatrixGetPos( RwFrameGetLTM( pFLFrame ) );
	RwV3d* pvtxFRPos = RwMatrixGetPos( RwFrameGetLTM( pFRFrame ) );
	RwV3d* pvtxBLPos = RwMatrixGetPos( RwFrameGetLTM( pBLFrame ) );
	RwV3d* pvtxBRPos = RwMatrixGetPos( RwFrameGetLTM( pBRFrame ) );

	float fFLHeight = m_pcsAgcmMap ? m_pcsAgcmMap->GetHeight( pvtxFLPos->x , pvtxFLPos->z , pstPos->y ) : 0;
	float fFRHeight = m_pcsAgcmMap ? m_pcsAgcmMap->GetHeight( pvtxFRPos->x , pvtxFRPos->z , pstPos->y ) : 0;
	float fBLHeight = m_pcsAgcmMap ? m_pcsAgcmMap->GetHeight( pvtxBLPos->x , pvtxBLPos->z , pstPos->y ) : 0;
	float fBRHeight = m_pcsAgcmMap ? m_pcsAgcmMap->GetHeight( pvtxBRPos->x , pvtxBRPos->z , pstPos->y ) : 0;

	float fF = (fFLHeight+fFRHeight)/2.f;
	float fB = (fBLHeight+fBRHeight)/2.f;

	RwV3d vtxFPos, vtxBPos;
	vtxFPos.x = (pvtxFLPos->x + pvtxFRPos->x)/2.f;
	vtxFPos.y = 0;
	vtxFPos.z = (pvtxFLPos->z + pvtxFRPos->z)/2.f;

	vtxBPos.x = (pvtxBLPos->x + pvtxBRPos->x)/2.f;
	vtxBPos.y = 0;
	vtxBPos.z = (pvtxBLPos->z + pvtxBRPos->z)/2.f;

	RwV3d vtxTemp;
	RwV3dSub( &vtxTemp, &vtxFPos, &vtxBPos );
	RwReal fLength = RwV3dLength( &vtxTemp );

	float fHeight = fF - fB;
	if( fHeight > fLength*0.83f ) { //�������� 70��
		fHeight = fLength*0.83f;
	}

	RwV3d* pAt	  = RwMatrixGetAt( pstModelling );
	RwV3d* pRight = RwMatrixGetRight( pstModelling );
	RwV3d* pUp	  = RwMatrixGetUp ( pstModelling );
	pAt->x *= fLength;
	pAt->y = fHeight;
	pAt->z *= fLength;
	RwV3dNormalize( pAt, pAt );
	RwV3dCrossProduct( pRight, &m_stYAxis, pAt );
	RwV3dNormalize( pRight, pRight );
	RwV3dCrossProduct( pUp, pAt, pRight );
	RwV3dNormalize( pUp, pUp );

	RwMatrixScale( pstModelling, &pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_stScale, rwCOMBINEPOSTCONCAT);

	return TRUE;
}

BOOL AgcmCharacter::InclinePlaneBiped( RwMatrix* pstModelling, AgcdCharacter* pstAgcdCharacter, const RwV3d* pstPos )
{
	int nF, nB;
	RpHAnimHierarchy* pInHierarchy = NULL;
	pInHierarchy = pstAgcdCharacter->m_pInHierarchy;
	nF = AGCMCHAR_FOOT_F_ID;
	nB = AGCMCHAR_FOOT_B_ID;

	if( pInHierarchy == NULL )
		return FALSE;
	RwInt32 lFNodeIndex = RpHAnimIDGetIndex( pInHierarchy, nF );
	if( lFNodeIndex == -1 )
		return FALSE;
	RwInt32 lBNodeIndex = RpHAnimIDGetIndex( pInHierarchy, nB );
	if( lBNodeIndex == -1 )
		return FALSE;

	RwFrame* pFFrame = pInHierarchy->pNodeInfo[lFNodeIndex].pFrame;
	RwFrame* pBFrame = pInHierarchy->pNodeInfo[lBNodeIndex].pFrame;

	if( pFFrame == NULL )
		return FALSE;
	if( pBFrame == NULL )
		return FALSE;

	RwV3d* pvtxFPos = RwMatrixGetPos( RwFrameGetLTM( pFFrame ) );
	RwV3d* pvtxBPos = RwMatrixGetPos( RwFrameGetLTM( pBFrame ) );

	float fFHeight = m_pcsAgcmMap ? m_pcsAgcmMap->GetHeight( pvtxFPos->x , pvtxFPos->z , pstPos->y ) : 0;
	float fBHeight = m_pcsAgcmMap ? m_pcsAgcmMap->GetHeight( pvtxBPos->x , pvtxBPos->z , pstPos->y ) : 0;

	RwV3d vtxFPos, vtxBPos;
	vtxFPos.x = pvtxFPos->x;
	vtxFPos.y = 0;
	vtxFPos.z = pvtxFPos->z;

	vtxBPos.x = pvtxBPos->x;
	vtxBPos.y = 0;
	vtxBPos.z = pvtxBPos->z;

	RwV3d vtxTemp;
	RwV3dSub( &vtxTemp, &vtxFPos, &vtxBPos );
	RwReal fLength = RwV3dLength( &vtxTemp );

	float fHeight = fFHeight - fBHeight;
	if( fHeight > fLength*0.83f ) { //�������� 70��
		fHeight = fLength*0.83f;
	}

	RwV3d* pAt	  = RwMatrixGetAt( pstModelling );
	RwV3d* pRight = RwMatrixGetRight( pstModelling );
	RwV3d* pUp	  = RwMatrixGetUp ( pstModelling );
	pAt->x *= fLength;
	pAt->y = fHeight;
	pAt->z *= fLength;
	RwV3dNormalize( pAt, pAt );
	RwV3dCrossProduct( pRight, &m_stYAxis, pAt );
	RwV3dNormalize( pRight, pRight );
	RwV3dCrossProduct( pUp, pAt, pRight );
	RwV3dNormalize( pUp, pUp );

	RwMatrixScale( pstModelling, &pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_stScale, rwCOMBINEPOSTCONCAT);

	return TRUE;
}

BOOL AgcmCharacter::CBUpdatePosition(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgcmCharacter::CBUpdatePosition()");

	if( (!pData) || (!pClass) || (!pCustData) )	return FALSE;

	AgcmCharacter *	pThis				= (AgcmCharacter *) pClass;
	AgpdCharacter *	pcsAgpdCharacter	= (AgpdCharacter *) pData;
	AgcdCharacter *	pstAgcdCharacter	= pThis->GetCharacterData(pcsAgpdCharacter);

	RwV3d *			pstPos				= (RwV3d *) &pcsAgpdCharacter->m_stPos;
	RwV3d			stPos;
	RwFrame *		pstFrame;
	FLOAT			fHeight;

	RwV3d *			pPrevPos			= ( RwV3d * ) pCustData;
	RpClump*		pClump = NULL;

	if( pstAgcdCharacter->m_pRide ) {
		pClump = pstAgcdCharacter->m_pRide->m_pClump;
	}
	else {
		pClump = pstAgcdCharacter->m_pClump;
	}

	if( pClump == NULL ) {
		return TRUE;
	}

	pstFrame = RpClumpGetFrame( pClump );
	if( pstFrame == NULL ) {
		return TRUE;
	}

	RwMatrix*	pstModelling = RwFrameGetMatrix(pstFrame);

	AuPOS	posPrev , posCurrent;

	posPrev.x		= pPrevPos->x;
	posPrev.y		= pPrevPos->y;
	posPrev.z		= pPrevPos->z;

	// ������ (2005-05-31 ���� 11:25:31) : 
	if( pstPos->y == 0.0f )
	{
		pstPos->y = SECTOR_MAX_HEIGHT;
	}

	posCurrent.x	= pcsAgpdCharacter->m_stPos.x;
	posCurrent.y	= pcsAgpdCharacter->m_stPos.y;
	posCurrent.z	= pcsAgpdCharacter->m_stPos.z;

	// ������ (2005-06-08 ���� 12:10:46) : NPC , ������ ��츸 �� ������ ������Ʈ�� ���־�� �Ѵ�.
	// PC�� ��� �������� ���ƿ��� ���̸� �׳� ����Ѵ�.

	{
		PROFILE("AgcmCharacter::CBUpdatePosition() - GetHeight");

		// ���� ���� ���..
		// �ڽŸ� ���̿� ���� üũ�ϰ� , NPC�� ���ʹ� ������ ����..
		// Ÿ PC�� ó������ �ʴ´�.
		if ( pcsAgpdCharacter->m_lID == pThis->m_lSelfCID )	// �Ǵ� �����ɸ���.
		{
			if( pstPos->y != SECTOR_MAX_HEIGHT )
			{
				pstPos->y = pPrevPos->y;
			}

			fHeight = pThis->m_pcsAgcmMap ? pThis->m_pcsAgcmMap->GetHeight( pstPos->x , pstPos->z , pstPos->y ) : 0;
			if( fHeight == INVALID_HEIGHT )
			{
				fHeight = pThis->m_pcsAgcmMap ? pThis->m_pcsAgcmMap->GetHeight( pstPos->x , pstPos->z , SECTOR_MAX_HEIGHT ) : 0;
			}

			pstPos->y = fHeight;
		}
		else if( pThis->m_pcsAgpmCharacter->IsTrap( pcsAgpdCharacter ) )
		{
			// Ʈ���Ӽ��� ���̸� �׳� ����Ѵ�.
		}
		else if(	!pThis->m_pcsAgpmCharacter->IsPC( pcsAgpdCharacter )  ) // PC�� �ƴϰ� NPC�� �ƴѰ��..
		{
			if( pThis->m_pcsAgpmSummons && pThis->m_pcsAgpmCharacter->IsStatusSummoner( pcsAgpdCharacter ) )
			{
				// ������ (2005-10-06 ���� 10:43:13) : ��ȯ�� ���� ���� ó��
				INT32	nCIDOwner = pThis->m_pcsAgpmSummons->GetOwnerCID( pcsAgpdCharacter );
				AgpdCharacter *	pcsOwner	= pThis->m_pcsAgpmCharacter->GetCharacter( nCIDOwner );

				if( pcsOwner )
				{
					// �����ְ� �ִ� ��ȯ���� ��� ������ ��ġ�� �������� ���̸� üũ�Ѵ�.
					fHeight = pThis->m_pcsAgcmMap ? pThis->m_pcsAgcmMap->GetHeight( pstPos->x , pstPos->z , pcsOwner->m_stPos.y) : 0;
					pstPos->y = fHeight;
				}
				else
				{
					// �����ְ� ������ �׳� ���� �÷�������.
					fHeight = pThis->m_pcsAgcmMap ? pThis->m_pcsAgcmMap->GetHeight( pstPos->x , pstPos->z , SECTOR_MAX_HEIGHT ) : 0;

					// Invalid �ϰ�츸 ����..
					if ( fHeight != INVALID_HEIGHT )
						pstPos->y = fHeight;
				}
			}
			else
			{
				FLOAT	fHeightSample = SECTOR_MAX_HEIGHT;
				if( pcsAgpdCharacter->m_alTargetCID[0] )
				{
					AgpdCharacter *	pcsAgpdCharacter2	= pThis->m_pcsAgpmCharacter->GetCharacter( pcsAgpdCharacter->m_alTargetCID[0] );

					if( pcsAgpdCharacter2 )
					{
						fHeightSample = pcsAgpdCharacter2->m_stPos.y;
					}
				}

				// �����̵� ���� �÷� ������.
				fHeight = pThis->m_pcsAgcmMap ? pThis->m_pcsAgcmMap->GetHeight( pstPos->x , pstPos->z , fHeightSample ) : 0;

				// Invalid �ϰ�츸 ����..
				if ( fHeight != INVALID_HEIGHT )
				{
					pstPos->y = fHeight;
				}
				else
				{
					fHeight = pThis->m_pcsAgcmMap ? pThis->m_pcsAgcmMap->GetHeight( pstPos->x , pstPos->z , SECTOR_MAX_HEIGHT ) : 0;
					if ( fHeight != INVALID_HEIGHT )
					{
						pstPos->y = fHeight;
					}
				}
			}
		}
		else
		{
			// do nothing
			#ifndef BOKCHUNGGUJO
			// ���δ� ���� �÷�����..
			fHeight = pThis->m_pcsAgcmMap ? pThis->m_pcsAgcmMap->GetHeight( pstPos->x , pstPos->z , SECTOR_MAX_HEIGHT ) : 0;
			if (fHeight != 0 && fHeight != INVALID_HEIGHT)
				pstPos->y = fHeight;
			#else
			fHeight = pThis->m_pcsAgcmMap ? pThis->m_pcsAgcmMap->GetHeight( pstPos->x , pstPos->z , pstPos->y ) : 0;
			if (fHeight != 0 && fHeight != INVALID_HEIGHT)
				pstPos->y = fHeight;
			#endif // BOKCHUNGGUJO
		}
	}

	if (pcsAgpdCharacter->m_lID == pThis->m_lSelfCID )
	{
		PROFILE("AgcmCharacter::CBUpdatePosition() - MyChar");
		{
			PROFILE("AgcmCharacter::CBUpdatePosition() - IsBadReadPtr");

			if(IsBadReadPtr(pPrevPos, sizeof(RwV3d))) return FALSE;
		}

		if (pThis->m_pcsAgcmMap)
			pThis->m_pcsAgcmMap->MyCharacterPositionChange( posPrev , posCurrent );

		{
			PROFILE("AgcmCharacter::CBUpdatePosition() - EnumCB");
			pThis->EnumCallback(AGCMCHAR_CB_ID_SELF_UPDATA_POSITION, pData, NULL);
		}
	}

	if (!pstPos)
	{
		stPos = *RwMatrixGetPos(RwFrameGetMatrix(pstFrame));
		pstPos = &stPos;
	}

	//Shadow������ �ִ��� üũ�ؼ� RenderCallback�� �����Ѵ�. (2004.6.21 gemani) -----------------------------
	if((pstAgcdCharacter->m_lStatus & AGCMCHAR_STATUS_FLAG_ADDED_WORLD) && pThis->IsValidCharacter(pstAgcdCharacter))
	{
		// ������ (2004-07-08 ���� 4:47:47) : NULL üũ. -_-
		BOOL	bInShadow = FALSE;
		if( pThis->m_pcsAgcmMap->GetOnObject( pcsAgpdCharacter->m_stPos.x , pcsAgpdCharacter->m_stPos.y , pcsAgpdCharacter->m_stPos.z ) )
		{
			// do nothing..
		}
		else
		if( pThis->m_pcsAgcmShadow2 )
		{
			bInShadow = pThis->m_pcsAgcmShadow2->GetShadowBuffer(pstPos);
		}

		if( bInShadow )
		{
			if(!pstAgcdCharacter->m_bInShadow)
			{
				//@{ Jaewon 20050913
				// No render callback change, the lighting type change only.
				pThis->m_pcsAgcmRender->CustomizeLighting(pstAgcdCharacter->m_pClump, LIGHT_IN_SHADOW);

				//-----------------------------------------------------------------------
				// Ż�Ϳ��� �׸��� ������ �����մϴ�. - 2011.01.13 kdi
				if( pstAgcdCharacter->m_pRide && pstAgcdCharacter->m_pRide->m_pClump )
					pThis->m_pcsAgcmRender->CustomizeLighting(pstAgcdCharacter->m_pRide->m_pClump, LIGHT_IN_SHADOW);
				//-----------------------------------------------------------------------
				pstAgcdCharacter->m_bInShadow = TRUE;
				//@} Jaewon
			}
		}
		else
		{
			if(pstAgcdCharacter->m_bInShadow)
			{
				pstAgcdCharacter->m_bInShadow = FALSE;

				//@{ Jaewon 20050913
				// No render callback change, the lighting type change only.
				pThis->m_pcsAgcmRender->ResetLighting(pstAgcdCharacter->m_pClump);
				//@} Jaewon

				//-----------------------------------------------------------------------
				// Ż�Ϳ��� �׸��� ������ �����մϴ�. - 2011.01.13 kdi
				if( pstAgcdCharacter->m_pRide && pstAgcdCharacter->m_pRide->m_pClump )
					pThis->m_pcsAgcmRender->ResetLighting(pstAgcdCharacter->m_pRide->m_pClump);
				//-----------------------------------------------------------------------
			}
		}
		//--------------------------------------------------------------------------------------------------------
	}

	//dobal 20051024
	int nLandAttach = 0;
	if( pstAgcdCharacter->m_pRide ) {
		AgpdCharacter *	pstAgcdCharacterRide = pThis->GetCharacter(pstAgcdCharacter->m_pRide);
		nLandAttach = pstAgcdCharacterRide->m_pcsCharacterTemplate->m_lLandAttachType;
	}
	else {
		nLandAttach = pcsAgpdCharacter->m_pcsCharacterTemplate->m_lLandAttachType;
	}

	{
		PROFILE("AgcmCharacter::CBUpdatePosition() - FrameTransform");

//		RwMatrixSetIdentity( pstModelling );
		RwMatrixScale(pstModelling, &pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_stScale, rwCOMBINEREPLACE);
		RwMatrixRotate(pstModelling, &m_stXAxis, pcsAgpdCharacter->m_fTurnX, rwCOMBINEPOSTCONCAT);
		RwMatrixRotate(pstModelling, &m_stYAxis, pcsAgpdCharacter->m_fTurnY, rwCOMBINEPOSTCONCAT);
	}

	float fTurnZ = 0;
	switch( nLandAttach )
	{
	case 1:
		pThis->InclinePlaneQuadruped( pstModelling, pstAgcdCharacter, pstPos );
		break;
	case 2:
		pThis->InclinePlaneBiped( pstModelling, pstAgcdCharacter, pstPos );
		break;
	default:
		{
			if( pstAgcdCharacter->m_pRide ) {
				pThis->InclinePlaneQuadruped( pstModelling, pstAgcdCharacter, pstPos );
			}
		}
		break;
	};

	RwMatrixTranslate(pstModelling, pstPos, rwCOMBINEPOSTCONCAT);
	RwFrameUpdateObjects(pstFrame);
	//dobal

	// ���󰡱� ���� �ڵ�
	if (pThis->m_lFollowTargetCID != AP_INVALID_CID &&
		pThis->m_lFollowTargetCID == pcsAgpdCharacter->m_lID &&
		pThis->m_lFollowDistance > 0 &&
		pThis->m_pcsSelfCharacter)
	{
		if (!pThis->m_pcsAgpmCharacter->IsActionBlockCondition(pThis->m_pcsSelfCharacter) &&
			!pThis->m_pcsSelfCharacter->m_bMove &&
			pThis->m_pcsSelfCharacter->m_unActionStatus != AGPDCHAR_STATUS_DEAD)
		{
			AuPOS	stDelta;

			// stDelta�� ���� ��ġ���� ��ǥ ��ġ������ Vector
			AuMath::V3DSubtract(&stDelta, &pcsAgpdCharacter->m_stPos, &pThis->m_pcsSelfCharacter->m_stPos);

			stDelta.y	= 0;

			// fDiv�� �� �Ÿ�
			FLOAT	fDiv = AuMath::V3DLength(&stDelta);

			if (fDiv > pThis->m_lFollowDistance * AGCMCHARACTER_FOLLOW_ADJUST_RATE)
			{
				pThis->SendMoveFollowCharacter(pThis->m_lFollowTargetCID, pThis->m_lFollowDistance);
			}
		}
	}

	return TRUE;
}

BOOL AgcmCharacter::CBCheckISActionMove(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmCharacter		*pThis				= (AgcmCharacter *)		pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)		pData;
	BOOL				*pbIsActionMove		= (BOOL *)				pCustData;

	if (pThis->m_pcsSelfCharacter)
	{
		if (pThis->m_pcsSelfCharacter->m_lID != pcsCharacter->m_lID)
			*pbIsActionMove = FALSE;
	}

	return TRUE;
}

BOOL AgcmCharacter::CBActionAttack(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmCharacter *	pThis = (AgcmCharacter *) pClass;
	AgpdCharacter *	pcsAgpdCharacter = (AgpdCharacter *) pData;
	AgcdCharacter *	pstAgcdCharacter = pThis->GetCharacterData(pcsAgpdCharacter);

	PVOID	*ppvBuffer	= (PVOID *) pCustData;

	AgpdCharacter *pcsAgpdTargetCharacter = (AgpdCharacter *)(ppvBuffer[0]);
	AgcdCharacter *pcsAgcdTargetCharacter = (AgcdCharacter *)(pThis->GetCharacterData(pcsAgpdTargetCharacter));

	// �̰��� ���� �����.
	// �̰� �����ؼ� Animation, Effect, UI Message ���� ó���Ѵ�.
	///////////////////////////////////////////////////////////////
	INT32		lAttackResult		= (INT32)			ppvBuffer[1];
	AgpdFactor	*pcsFactorDamage	= (AgpdFactor *)	ppvBuffer[2];

	UINT8		cComboInfo			= (UINT8)			ppvBuffer[3];	
	
	if (pcsAgpdCharacter != pThis->m_pcsSelfCharacter) {
		// cComboInfo �� ���� �� ������ ���� �����ϴ� �޺� �׼����� �ƴ����� �Ǵ��� �ִϸ��̼� �����ش�.
		if (cComboInfo != 0 && pThis->m_pcsAgpmCharacter->IsPC(pcsAgpdCharacter)) {
			pstAgcdCharacter->m_bIsCombo	= TRUE;

			// ���� ���� �ִϸ��̼��� �ϰ� �ִ� ������ �ƴ��� ����.
			// ���� ���� �ִϸ��̼� ���̶�� ������Ű�� ���� ÷���� �ٽ� �ִϸ��̼� ��Ų��.

//			pThis->InitLinkAnimData(pstAgcdCharacter);
		}
	}
	else {
		pThis->m_pcsAgpmCharacter->SetActionBlockTime(
			pThis->m_pcsSelfCharacter,
			pThis->m_pcsAgpmCharacter->GetAttackIntervalMSec(
				pThis->m_pcsSelfCharacter),
			AGPDCHAR_ACTION_BLOCK_TYPE_ATTACK);
	}
	pThis->SetAttackStatus(pstAgcdCharacter);
	pThis->SetAttackStatus(pcsAgcdTargetCharacter);
	
	if (!pThis->StartAnimation(pcsAgpdCharacter, AGCMCHAR_ANIM_TYPE_ATTACK)) return FALSE;
	return TRUE;
}

BOOL AgcmCharacter::CBRemoveCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData)
		return FALSE;

	AgcmCharacter *pcsThis			= (AgcmCharacter *)(pClass);
	AgpdCharacter *pcsAgpdCharacter = (AgpdCharacter *)(pData);

	return pcsThis->RemoveCharacter(pcsAgpdCharacter);
}

UINT32	g_uDebugLastRemoveCharacterTID = 0;

BOOL AgcmCharacter::RemoveCharacter(AgpdCharacter *pcsAgpdCharacter)
{
	g_uDebugLastRemoveCharacterTID = pcsAgpdCharacter->m_pcsCharacterTemplate->m_lID;

	AgcdCharacter *pstAgcdCharacter = GetCharacterData(pcsAgpdCharacter);

	TRACE("AgcmCharacter::RemoveCharacter - Clump - %x , lStatus - %d\n",pstAgcdCharacter->m_pClump,pstAgcdCharacter->m_lStatus);

	if (pstAgcdCharacter->m_lStatus & AGCMCHAR_STATUS_FLAG_REMOVED)
	{
		#ifdef _DEBUG
		if( pcsAgpdCharacter->m_pcsCharacterTemplate )
		{
			TRACE("AgcmCharacter::CBRemoveCharacter() Already Removed/Not Added %d %x(%s)\n", pcsAgpdCharacter->m_lID, pstAgcdCharacter->m_pClump,pcsAgpdCharacter->m_pcsCharacterTemplate->m_szTName);
		}
		#endif

		return TRUE;
	}

	// ���� ������Ʈ�� ��� ���� ����..
	AgpmSiegeWar * pmAgpmSiegeWar = ( AgpmSiegeWar * ) GetModule("AgpmSiegeWar");
	if( !this->m_pcsAgcmMap->IsLocked() && 
		pmAgpmSiegeWar->IsSiegeWarMonster( pcsAgpdCharacter ) )
	{
		ApAutoWriterLock	csLock( g_lockSiegeMonster );

		for( vector< INT32 >::iterator iter = g_vecSiegeMonster.begin();
			iter != g_vecSiegeMonster.end() ;
			iter++ )
		{
			if( *iter == pcsAgpdCharacter->m_lID )
			{
				g_vecSiegeMonster.erase(iter);
				break;
			}
		}
	}	

	pstAgcdCharacter->m_lStatus |= AGCMCHAR_STATUS_FLAG_REMOVED;
	pstAgcdCharacter->m_bInShadow = FALSE;

	EnumCallback(AGCMCHAR_CB_ID_PRE_REMOVE_DATA, (PVOID)(pcsAgpdCharacter), (PVOID)(pstAgcdCharacter));

	if (pstAgcdCharacter->m_lStatus & AGCMCHAR_STATUS_FLAG_ADDED_WORLD)
	{
		if (m_pcsAgcmResourceLoader)
		{
			m_pcsAgcmResourceLoader->RemoveLoadEntry(m_lLoaderIDInit, (PVOID) pcsAgpdCharacter, NULL);
			m_pcsAgcmResourceLoader->RemoveLoadEntry(m_lLoaderCustomize, (PVOID) pcsAgpdCharacter, NULL);
		}

		if (m_pWorld && pstAgcdCharacter->m_pClump)
		{
			if(RwFrameGetParent(RpClumpGetFrame(pstAgcdCharacter->m_pClump)))
			{
				LockFrame();

				RwFrameRemoveChild(RpClumpGetFrame(pstAgcdCharacter->m_pClump));

				UnlockFrame();
			}

			if (m_pcsAgcmShadow)
				m_pcsAgcmShadow->DeleteShadow(pstAgcdCharacter->m_pClump);

			//@{ Jaewon 20040702
			// unregister the clump of the character as a shadow map receiver.
			if(m_pcsAgcmShadowmap)
			{
				m_pcsAgcmShadowmap->unregisterClump(pstAgcdCharacter->m_pClump);

				// ������ (2004-09-07 ���� 8:54:23) : �����ɸ��͸� ������ �ʱ�ȭ.
				if( pcsAgpdCharacter == m_pcsSelfCharacter )
				{
					m_pcsAgcmShadowmap->setPlayerClump( NULL );
				}
			}
			//@} Jaewon

			TRACE("AgcmCharacter::CBRemoveCharacter() Remove (RefCount = %d) %d %x (%s)\n", pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_lRefCount , pcsAgpdCharacter->m_lID, pstAgcdCharacter->m_pClump, pcsAgpdCharacter->m_pcsCharacterTemplate->m_szTName);

			if( g_pcsAgcmEventEffect ) g_pcsAgcmEventEffect->CharaterEquipEffectRemove( pcsAgpdCharacter );
			if (m_pcsAgcmRender)
				m_pcsAgcmRender->RemoveClumpFromWorld(pstAgcdCharacter->m_pClump);
			else
				RpWorldRemoveClump(m_pWorld, pstAgcdCharacter->m_pClump);
		}
	}

	EnumCallback(AGCMCHAR_CB_ID_REMOVE_CHARACTER, (PVOID)(pcsAgpdCharacter), NULL);

	if (pcsAgpdCharacter == GetSelfCharacter())
		ReleaseSelfCharacter();

	AgpdCharacter	*pcsSelfCharacter	= GetSelfCharacter();
	if (pcsSelfCharacter)
	{
		AgcdCharacter	*pcsAgcdSelfCharacter	= GetCharacterData(pcsSelfCharacter);

		if (!pstAgcdCharacter->m_bTransforming)
		{
			if (pcsAgcdSelfCharacter->m_lLockTargetID == pcsAgpdCharacter->m_lID)
				UnLockTarget(m_pcsSelfCharacter);
			if (pcsAgcdSelfCharacter->m_lSelectTargetID == pcsAgpdCharacter->m_lID)
				UnSelectTarget(m_pcsSelfCharacter);

			if (pcsAgpdCharacter->m_lID == m_lFollowTargetCID)
			{
				ReleaseFollowTarget();

				if (pcsSelfCharacter->m_bMove)
					SendStopCharacter();
			}
		}
	}

	return TRUE;
}
		

BOOL AgcmCharacter::CBUpdateStatus(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}

BOOL AgcmCharacter::CBUpdateActionStatus(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmCharacter	*pThis				= (AgcmCharacter *) pClass;
	AgpdCharacter	*pcsAgpdCharacter	= (AgpdCharacter *) pData;
	AgcdCharacter	*pstAgcdCharacter	= pThis->GetCharacterData(pcsAgpdCharacter);
	INT16			*pnOldStatus		= (INT16 *) pCustData;

	/*
	if (pThis->m_pcsAgcmMap && *pnOldStatus == AGPDCHAR_STATUS_SELECT_CHAR && pcsCharacter->m_unActionStatus != AGPDCHAR_STATUS_SELECT_CHAR)
	{
		pThis->m_pcsAgcmMap->MyCharacterPositionChange(pcsCharacter->m_stPos, pcsCharacter->m_stPos);

		//if (pstAgcdCharacter->m_pClump)
		//	RwFrameUpdateObjects(RpClumpGetFrame(pstAgcdCharacter->m_pClump));
	}

	pcsCharacter->m_bMove = FALSE;
	*/

	if (*pnOldStatus != AGPDCHAR_STATUS_DEAD && pcsAgpdCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
	{
		pThis->SetHPZero(pcsAgpdCharacter);

		return pThis->StartAnimation(pcsAgpdCharacter, pstAgcdCharacter, AGCMCHAR_ANIM_TYPE_DEAD);
	}

	if (*pnOldStatus == AGPDCHAR_STATUS_DEAD && pcsAgpdCharacter->m_unActionStatus != AGPDCHAR_STATUS_DEAD)
	{
		return pThis->StartAnimation(pcsAgpdCharacter, pstAgcdCharacter, AGCMCHAR_ANIM_TYPE_WAIT);
	}

	return TRUE;
}

BOOL AgcmCharacter::CBUpdateActionStatusCheck(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmCharacter		*pThis			= (AgcmCharacter *)	pClass;
	AgpdCharacter		*pcsCharacter	= (AgpdCharacter *)	pData;

	PVOID				*pvBuffer		= (PVOID *)			pCustData;

	UINT16				nNewStatus		= (UINT16)			pvBuffer[0];
	if (nNewStatus != AGPDCHAR_STATUS_DEAD)
		return TRUE;

	BOOL				*pbIsUpdate		= (BOOL *)			pvBuffer[1];

	// �ٷ� ������Ʈ�� ���ϰ� ���⼭ �ϴ� ť�� �������� ť���� ���� ó���Ѵ�.
	// ���⼱ *pbIsUpdate�� FALSE�� ����� Public Module ���� ������Ʈ ���ϰ� �����
	// ActionData�� ������ ť�� ���� �ȴ�.

	*pbIsUpdate	= FALSE;

	// Action Queue�� ���� �ִ´�.
	// Ÿ�ٿ� �ϴ� �ִ´�.
	///////////////////////////////////////////////////////////////
	AgcmCharacterActionQueueData	stActionData;
	ZeroMemory(&stActionData, sizeof(stActionData));

	stActionData.eActionType		= AGPDCHAR_ACTION_TYPE_NONE;
	stActionData.eActionResultType	= AGPDCHAR_ACTION_RESULT_TYPE_NONE;
	stActionData.lActorID			= AP_INVALID_CID;
	stActionData.ulProcessTime		= 0;
	stActionData.bDeath				= TRUE;

	if (pcsCharacter->m_lID == pThis->GetSelectTarget(pThis->m_pcsSelfCharacter))
		stActionData.lActorID	= pThis->m_pcsSelfCharacter->m_lID;

	if (!pThis->AddReceivedAction(pcsCharacter, &stActionData))
		*pbIsUpdate = TRUE;

	AgcdCharacter		*pstAgcdCharacter	= pThis->GetCharacterData(pcsCharacter);

	pstAgcdCharacter->m_bQueuedDeathEvent	= TRUE;

	return TRUE;
}

BOOL AgcmCharacter::CBUpdateLevel( PVOID pData, PVOID pClass, PVOID pCustData	)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmCharacter		*pThis			= (AgcmCharacter *) pClass;
	AgpdCharacter		*pcsCharacter	= (AgpdCharacter *)	pData;
	BOOL				bIsLevelUp		= *(BOOL *)			pCustData;

	// level up�� ������ effect, animation ����� ���ش�.
	//
	//
	//
	//
	//
	//

	return TRUE;
}

BOOL AgcmCharacter::CBReceiveAction(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmCharacter		*pThis				= (AgcmCharacter *)		pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)		pData;
	PVOID				*ppvBuffer			= (PVOID *)				pCustData;

	AgpdCharacter		*pcsTargetChar		= (AgpdCharacter *)		ppvBuffer[0];
	INT32				lActionResult		= (INT32)				ppvBuffer[1];
	AgpdFactor			*pcsFactorDamage	= (AgpdFactor *)		ppvBuffer[2];
	INT32				lAction				= (INT32)				ppvBuffer[3];
	PVOID				pvPacketFactor		= (PVOID)				ppvBuffer[4];
	INT32				lSkillTID			= (INT32)				ppvBuffer[5];
	UINT32				ulAdditionalEffect	= (UINT32)				ppvBuffer[6];	
	UINT8				cHitIndex			= (UINT8)				ppvBuffer[7];

	AgcdCharacter		*pstAgcdCharacter	= pThis->GetCharacterData(pcsCharacter);

	if (!pcsTargetChar)
		return TRUE;

	//@{ 2006/03/20 burumal	
	if ( pstAgcdCharacter )
	{
		if ( pcsCharacter != pThis->m_pcsSelfCharacter )
		{
			AgcdCharacter* pAgcdTargetChar = pThis->GetCharacterData(pcsTargetChar);
			if ( pAgcdTargetChar )
			{
				if ( pAgcdTargetChar->m_pOrgPickAtomic && 
					(pAgcdTargetChar->m_pOrgPickAtomic != pAgcdTargetChar->m_pOrgPickAtomic->next) )
				{
					UINT8 uCount;
					pAgcdTargetChar->m_pPickAtomic = pAgcdTargetChar->m_pOrgPickAtomic;
					for ( uCount = 0; uCount < cHitIndex; uCount++ )
						pAgcdTargetChar->m_pPickAtomic = pAgcdTargetChar->m_pPickAtomic->next;
				}
			}
		}
		else
		{			
			char* pAttacker = pcsCharacter->m_szID;
			char* pTarget = pcsTargetChar->m_szID;
		}
	}	
	

	// AGPDCHAR_ACTION_RESULT_TYPE_NOT_ENOUGH_ARROW �� ���� ť�� ���� �ʱ� �ݹ��� �ҷ��ְ� ����������.
	///////////////////////////////////////////////////////////////
	if ((AgpdCharacterActionResultType) lActionResult == AGPDCHAR_ACTION_RESULT_TYPE_NOT_ENOUGH_ARROW)
	{
		pstAgcdCharacter->m_ulNextActionSelfCharacterTime	= 0;

		return pThis->EnumCallback(AGCMCHAR_CB_ID_ACTION_RESULT_NOT_ENOUGH_ARROW, pcsCharacter, NULL);
	}
	else if ((AgpdCharacterActionResultType) lActionResult == AGPDCHAR_ACTION_RESULT_TYPE_NOT_ENOUGH_BOLT)
	{
		pstAgcdCharacter->m_ulNextActionSelfCharacterTime	= 0;

		return pThis->EnumCallback(AGCMCHAR_CB_ID_ACTION_RESULT_NOT_ENOUGH_BOLT, pcsCharacter, NULL);
	}
	else if ((AgpdCharacterActionResultType) lActionResult == AGPDCHAR_ACTION_RESULT_TYPE_NOT_ENOUGH_MP)
	{
		pstAgcdCharacter->m_ulNextActionSelfCharacterTime	= 0;

		return pThis->EnumCallback(AGCMCHAR_CB_ID_ACTION_RESULT_NOT_ENOUGH_MP, pcsCharacter, NULL);
	}

	// Action Queue�� ���� �ִ´�.
	// Ÿ�ٿ� �ϴ� �ִ´�.
	///////////////////////////////////////////////////////////////
	AgcmCharacterActionQueueData	stActionData;
	ZeroMemory(&stActionData, sizeof(stActionData));

	stActionData.eActionType		= (AgpdCharacterActionType) lAction;
	stActionData.eActionResultType	= (AgpdCharacterActionResultType) lActionResult;
	stActionData.lActorID			= pcsCharacter->m_lID;
	stActionData.lSkillTID			= lSkillTID;
	if (pThis->m_pcsAgpmFactors && pcsFactorDamage)
		pThis->m_pcsAgpmFactors->CopyFactor(&stActionData.csFactorDamage, pcsFactorDamage, TRUE, FALSE);
	if (pvPacketFactor)
	{
		stActionData.bIsSetPacketFactor	= TRUE;
		UINT16 nPacketSize = (*((UINT16 *) pvPacketFactor) + sizeof(UINT16));
		stActionData.pszPacketFactor = new CHAR[nPacketSize];
		memcpy(stActionData.pszPacketFactor, pvPacketFactor, nPacketSize);
	}
	stActionData.ulProcessTime		= pThis->m_ulCurTick + AGCD_CHARACTER_MAX_ACTION_ATTACK_PRESERVE_TIME;
	stActionData.bDeath				= FALSE;
	stActionData.ulAdditionalEffect = ulAdditionalEffect;
	//stActionData.cHitIndex			= cHitIndex; //#2#

	pThis->AddReceivedAction( pcsTargetChar , &stActionData );

	if (pcsCharacter != pThis->m_pcsSelfCharacter)
	{
		pstAgcdCharacter->m_ulNextAttackTime				= pThis->m_ulCurTick + pThis->m_pcsAgpmCharacter->GetAttackIntervalMSec(pcsCharacter);
		pstAgcdCharacter->m_ulNextActionSelfCharacterTime	= pThis->m_ulCurTick + AGCD_CHARACTER_ACTION_SELF_CHARACTER_INTERVAL;
	}

	if (pcsCharacter == pThis->m_pcsSelfCharacter &&
		!pThis->m_bStartAttack &&
		pThis->m_bFirstSendAttack)
	{
		pThis->m_bStartAttack		= TRUE;
		pThis->m_bFirstSendAttack	= FALSE;
	}

	INT32	lDamage	= 0;
	pThis->m_pcsAgpmFactors->GetValue(pcsFactorDamage, &lDamage, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_DMG_NORMAL);

	if (pcsFactorDamage)
		delete pcsFactorDamage;

	return TRUE;
}

BOOL AgcmCharacter::CBCheckNowUpdateActionFactor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	PVOID*		pCharacterData		=	(PVOID*)pData;

	AgcmCharacter		*pThis				= (AgcmCharacter *)		pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)		pCharacterData[0];
	BOOL				*pbIsNowUpdate		= (BOOL *)				pCustData;

	// �ڱ� �ڽ��� �������� ���۸� ������������ ���� ó���� ���⼭�Ѵ�.
	// ���۸��� ���ϰ� �ٷ� �������� �ݿ���Ű���� *pbIsNowUpdate = FALSE �� �ϸ� �ȴ�.

	if (pThis->m_pcsSelfCharacter == pcsCharacter)
		*pbIsNowUpdate	= TRUE;
	else
		*pbIsNowUpdate	= FALSE;

	return TRUE;
}

BOOL AgcmCharacter::CBNewCreateCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if ((!pClass) || (!pData))
		return FALSE;

	AgcmCharacter		*pThis				= (AgcmCharacter *)		pClass;
	AgpdCharacter		*pcsAgpdCharacter	= (AgpdCharacter *)		pData;

	AgcdCharacter		*pstAgcdCharacter	= pThis->GetCharacterData(pcsAgpdCharacter);

	pstAgcdCharacter->m_lStatus				|= AGCMCHAR_STATUS_FLAG_NEW_CREATE;

	return TRUE;
}

BOOL AgcmCharacter::CBAddChar(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmCharacter *	pThis = (AgcmCharacter *) pClass;
	AgpdCharacter * pcsCharacter	= (AgpdCharacter *) pCustData;
	if (!pcsCharacter)
		return FALSE;

	if (pcsCharacter->m_lID == pThis->m_lSelfCID)
	{
		if (pThis->m_pcsAgcmMap)
		{
			pThis->m_pcsAgcmMap->MyCharacterPositionChange(pcsCharacter->m_stPos, pcsCharacter->m_stPos);

			/*
			if (pstAgcdCharacter->m_pClump)
				RwFrameUpdateObjects(RpClumpGetFrame(pstAgcdCharacter->m_pClump));
			*/
		}
	}
	
	// ���� ������Ʈ�� ��� ���� ����..
	AgpmSiegeWar * pmAgpmSiegeWar = ( AgpmSiegeWar * ) pThis->GetModule("AgpmSiegeWar");
	if( pmAgpmSiegeWar->IsSiegeWarMonster( pcsCharacter ) )
	{
		ApAutoWriterLock	csLock( g_lockSiegeMonster );
		g_vecSiegeMonster.push_back( pcsCharacter->m_lID );
	}	

	return TRUE;
}

BOOL AgcmCharacter::CBBindingRegionChange(PVOID	pData,PVOID	pClass,PVOID	pCustData)
{
	AgcmCharacter*	pThis = (AgcmCharacter*) pClass;
	AgpdCharacter*	pdCharacter = (AgpdCharacter*)	pData;

	ApmMap::RegionTemplate*	pTemplate = pThis->m_pcsApmMap->GetTemplate( pdCharacter->m_nBindingRegionIndex );
	ASSERT( NULL != pTemplate );

	if(pThis->m_pcsSelfCharacter == pdCharacter)
	{
		pThis->EnumCallback(AGCMCHAR_CB_ID_SELF_REGION_CHANGE,pData,pCustData);

		// ������ (2005-08-26 ���� 10:59:21) : 
		// �ϴ� ��
		/*
		// ������ (2005-08-24 ���� 5:02:43) : 
		// ���ε��� ���� ������ ���⼭..
		if( pTemplate )
		{
			pThis->m_pcsAgcmMap->SelfCharacterRegionChange( pTemplate );
		}
		*/
	}

	// ���� room type ó��

	if(pThis->m_pcsSelfCharacter == pdCharacter)
	{
		// �������� Ȯ��
		if( pTemplate && pTemplate->ti.stType.uFieldType == ApmMap::FT_PVP)
		{
			pThis->m_pcsAgcmSound->SetRoom( SoundEffectRoomType_Cave );
		}
		else
		{
			pThis->m_pcsAgcmSound->SetRoom( SoundEffectRoomType_Generic );
		}
	}

    return	TRUE;
}

RwFrame *AgcmCharacter::GetChildFrameHierarchy(RwFrame *frame, void *data)
{	
	RpHAnimHierarchy **hierarchy = (RpHAnimHierarchy **) data;

	*hierarchy = RpHAnimFrameGetHierarchy(frame);
	if( *hierarchy == NULL )
	{
		RwFrameForAllChildren(frame, GetChildFrameHierarchy, data);

		return frame;
	}

	return NULL;
}

RpHAnimHierarchy *AgcmCharacter::GetHierarchy(RpClump *clump)
{
	RpHAnimHierarchy *hierarchy = NULL;

	RwFrameForAllChildren(RpClumpGetFrame(clump), GetChildFrameHierarchy, (void *)&hierarchy);

	return hierarchy;
}

BOOL AgcmCharacter::CharacterConstructor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if((!pClass) || (!pData))
		return FALSE;

	int i;
	AgcmCharacter *pThis = (AgcmCharacter *) pClass;
	AgpdCharacter *pstAgpdCharacter = (AgpdCharacter *) pData;
	AgcdCharacter *pstAgcdCharacter = pThis->GetCharacterData(pstAgpdCharacter);
	if(!pstAgcdCharacter)
		return FALSE;

	memset(pstAgcdCharacter, 0, sizeof(AgcdCharacter));

	pstAgcdCharacter->m_lLastAttackTime	= -1;

	pstAgcdCharacter->m_lStatus = AGCMCHAR_STATUS_FLAG_NONE;

	pstAgcdCharacter->m_bForceAnimation	= FALSE;

	pstAgcdCharacter->m_lSelectTargetID	= AP_INVALID_CID;
	pstAgcdCharacter->m_bIsCastSkill	= FALSE;
	pstAgcdCharacter->m_lLastCastSkillTID = 0;

	pstAgcdCharacter->m_dwCharIDColor	= 0;

	pstAgcdCharacter->m_bAttachableHair		= TRUE;
	pstAgcdCharacter->m_bAttachableFace		= TRUE;
	pstAgcdCharacter->m_bViewHelmet			= TRUE;

	for( i = 0; i < AGCMCHAR_MAX_ANIM_TYPE; i++ )
	{
		pstAgcdCharacter->m_afDefaultAnimSpeedRate[ i ] = 1.0f;
	}

	for( i = 0 ; i < AGPMITEM_PART_V_BODY  ; i ++ )
	{
		pstAgcdCharacter->m_aLightInfo[ i ].Clear();
	}

	return TRUE;
}

BOOL AgcmCharacter::CharacterDestructor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmCharacter *			pThis = (AgcmCharacter *) pClass;

	return pThis->DestroyCharacter((AgpdCharacter *) pData);
}

VOID AgcmCharacter::DestroyTextBoard( AgcdCharacter* pstCharacter )
{
	if( !pstCharacter || !pstCharacter->m_pvTextboard )
		return;

	EnumCallback( AGCMCHAR_CB_ID_DESTROY_TEXTBOARD , pstCharacter , NULL );
}

BOOL AgcmCharacter::DestroyCharacter(AgpdCharacter *pcsAgpdCharacter)
{
	ApAutoWriterLock	csLock( m_pcsAgcmRender->lockRender );
	AuAutoLock			csLock2( m_pcsAgcmResourceLoader->m_csMutexRemoveResource );

	AgcdCharacter *			pstAgcdCharacter;
	AgcdCharacterTemplate *	pstAgcdCharacterTemplate;

	pstAgcdCharacter = GetCharacterData(pcsAgpdCharacter);
	pstAgcdCharacterTemplate = pstAgcdCharacter->m_pstAgcdCharacterTemplate;

	DetachRide( pstAgcdCharacter, TRUE );

	DestroyTextBoard( pstAgcdCharacter );

	this->m_pcsAgcmMap->LoadLock();
	CBRemoveCharacter(pcsAgpdCharacter, this, NULL);
	this->m_pcsAgcmMap->ReleaseLock();
	
	if (pstAgcdCharacter && pstAgcdCharacter->m_pClump)
	{
		//. clump�� ����� ���带 stop.
		if(pstAgcdCharacter->m_pClump)
		{
			static AgcmSound* _pcsAgcmSound = (AgcmSound*)GetModule("AgcmSound");

			if(_pcsAgcmSound)
				_pcsAgcmSound->PreStop3DSound(pstAgcdCharacter->m_pClump);
		}

		EnumCallback(AGCMCHAR_CB_ID_PRE_REMOVE_DATA, (PVOID)(pcsAgpdCharacter), (PVOID)(pstAgcdCharacter));

		TRACE("AgcmCharacter::CharacterDestructor() Destroy %d %x %x (%s)\n", pcsAgpdCharacter->m_lID, pstAgcdCharacter->m_pClump, RpClumpGetFrame(pstAgcdCharacter->m_pClump),pcsAgpdCharacter->m_pcsCharacterTemplate->m_szTName);

		if (pstAgcdCharacter->m_csSubAnim.m_pInHierarchy)
		{
			if (m_pcsAgcmResourceLoader)
				m_pcsAgcmResourceLoader->AddDestroyHierarchy(pstAgcdCharacter->m_csSubAnim.m_pInHierarchy);
			else
				RpHAnimHierarchyDestroy(pstAgcdCharacter->m_csSubAnim.m_pInHierarchy);

			pstAgcdCharacter->m_csSubAnim.m_pInHierarchy = NULL;
		}

		if (pstAgcdCharacter->m_csSubAnim.m_pInHierarchy2)
		{
			if (m_pcsAgcmResourceLoader)
				m_pcsAgcmResourceLoader->AddDestroyHierarchy(pstAgcdCharacter->m_csSubAnim.m_pInHierarchy2);
			else
				RpHAnimHierarchyDestroy(pstAgcdCharacter->m_csSubAnim.m_pInHierarchy2);

			pstAgcdCharacter->m_csSubAnim.m_pInHierarchy2 = NULL;
		}

		if (pstAgcdCharacter->m_csSubAnim.m_pOutHierarchy)
		{
			if (m_pcsAgcmResourceLoader)
				m_pcsAgcmResourceLoader->AddDestroyHierarchy(pstAgcdCharacter->m_csSubAnim.m_pOutHierarchy);
			else
				RpHAnimHierarchyDestroy(pstAgcdCharacter->m_csSubAnim.m_pOutHierarchy);

			pstAgcdCharacter->m_csSubAnim.m_pOutHierarchy = NULL;
		}

		if ( pstAgcdCharacter->m_pPickAtomic && !RpAtomicGetClump(pstAgcdCharacter->m_pPickAtomic) )
		{
			RpAtomic* pDel;
			RpAtomic* pCurPickAtomic = pstAgcdCharacter->m_pOrgPickAtomic;
			
			if ( pCurPickAtomic )
			{
				// ������ PickAtomic �������� �ֻ��� PickAtomic ���� traverse �Ѵ�
				pCurPickAtomic = pCurPickAtomic->prev;

				do {

					RwFrame* pstPickAtomicFrame = RpAtomicGetFrame(pCurPickAtomic);

					RpAtomicSetFrame(pCurPickAtomic, NULL);
					
					if ( pstAgcdCharacter->m_pClump && pstPickAtomicFrame != RpClumpGetFrame(pstAgcdCharacter->m_pClump) && pstPickAtomicFrame)
					{
						if ( RwFrameGetParent(pstPickAtomicFrame)  )
							RwFrameRemoveChild(pstPickAtomicFrame);	

						RwFrameDestroy(pstPickAtomicFrame);
					}
				
					//RpAtomicSetFrame(pCurPickAtomic, NULL);
										
					pDel = pCurPickAtomic;
					pCurPickAtomic = pCurPickAtomic->prev;

					if ( m_pcsAgcmResourceLoader )
						m_pcsAgcmResourceLoader->AddDestroyAtomic(pDel);
					else
						RpAtomicDestroy(pDel);

				} while ( pCurPickAtomic != pstAgcdCharacter->m_pPickAtomic );
			}
			//@}

			pstAgcdCharacter->m_pPickAtomic = NULL;
			pstAgcdCharacter->m_pOrgPickAtomic = NULL;
		}

		if(pstAgcdCharacter->m_pInHierarchy)
		{
			if (m_pcsAgcmResourceLoader)
				m_pcsAgcmResourceLoader->AddDestroyHierarchy(pstAgcdCharacter->m_pInHierarchy);
			else
				RpHAnimHierarchyDestroy(pstAgcdCharacter->m_pInHierarchy);

			pstAgcdCharacter->m_pInHierarchy = NULL;
		}

		if(pstAgcdCharacter->m_pInHierarchy2)
		{
			if (m_pcsAgcmResourceLoader)
				m_pcsAgcmResourceLoader->AddDestroyHierarchy(pstAgcdCharacter->m_pInHierarchy2);
			else
				RpHAnimHierarchyDestroy(pstAgcdCharacter->m_pInHierarchy2);

			pstAgcdCharacter->m_pInHierarchy2 = NULL;
		}

		if(pstAgcdCharacter->m_pOutHierarchy)
		{
			if (m_pcsAgcmResourceLoader)
				m_pcsAgcmResourceLoader->AddDestroyHierarchy(pstAgcdCharacter->m_pOutHierarchy);
			else
				RpHAnimHierarchyDestroy(pstAgcdCharacter->m_pOutHierarchy);

			pstAgcdCharacter->m_pOutHierarchy = NULL;
		}

		DestoryCustomize( pstAgcdCharacter );

		//@{ 2006/09/15 burumal
		if ( pstAgcdCharacter->m_pClump )
		//@}
		{
			if (m_pcsAgcmResourceLoader)
				m_pcsAgcmResourceLoader->AddDestroyClump(pstAgcdCharacter->m_pClump);
			else
				RpClumpDestroy(pstAgcdCharacter->m_pClump);

			pstAgcdCharacter->m_pClump = NULL;
		}

		//@{ 2006/11/03 burumal
		for ( INT32 nIdx = 0; nIdx < AGCD_CHARACTER_MAX_PREENGAGED_EVENT_EFFECT; nIdx++ )
		{	
			AgcdPreengagedEventEffect* pPreenagedEffect = pstAgcdCharacter->m_pPreengagedEventEffect[nIdx];
			if ( pPreenagedEffect )
				delete pPreenagedEffect;

			pstAgcdCharacter->m_pPreengagedEventEffect[nIdx] = NULL;
		}
		//@}

		// RemoveCharacter()�� �Ҹ��� �ʰ� CharacterDestructor()�� �Ҹ� ��츦 ����� �ѹ� �� EnumCallback()�Ѵ�. (BOB, 220504)
		EnumCallback(AGCMCHAR_CB_ID_REMOVE_CHARACTER, (PVOID)(pcsAgpdCharacter), NULL);
	}

	pstAgcdCharacter->m_pClump = NULL;

	//@{ Jaewon 20050502
	pstAgcdCharacter->m_pFace = NULL;
	pstAgcdCharacter->m_pHair = NULL;
	//@} Jaewon

	pstAgcdCharacter->m_lStatus	= AGCMCHAR_STATUS_FLAG_NONE;

	// m_csAnimation�� ���� �ı��ڰ� �ȺҸ��Ƿ�, ��������� Reset����� ��
	pstAgcdCharacter->m_csAnimation.ResetAnimCB(FALSE);

	DropAnimation(pstAgcdCharacter);


	if( pstAgcdCharacterTemplate )
	{
		/*
		RpAtomic ** paAtomic = &pstAgcdCharacterTemplate->m_vpFace[ 0 ];
		INT32		nCount = pstAgcdCharacterTemplate->m_vpFace.size();
		*/

		TRACE( "" );
	}

	ReleaseTemplateData(pstAgcdCharacterTemplate);
	return TRUE;
}

BOOL AgcmCharacter::TemplateConstructor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmCharacter			*pThis				= (AgcmCharacter *)(pClass);
	AgpdCharacterTemplate	*pcsAgpdTemplate	= (AgpdCharacterTemplate *)(pData);
	AgcdCharacterTemplate	*pcsAgcdTemplate	= (AgcdCharacterTemplate *)(pThis->GetTemplateData(pcsAgpdTemplate));

	memset(pcsAgcdTemplate, 0, sizeof(AgcdCharacterTemplate));
	pcsAgcdTemplate->m_fScale			= 1.0f;

	if (!(pThis->m_ulModuleFlags & E_AGCM_CHAR_FLAGS_EXPORT))
		pcsAgcdTemplate->m_pcsPreData		= new AgcdPreCharacterTemplate();

	//@{ 2006/02/20 burumal
	//pcsAgcdTemplate->m_lPickingNodeIndex	= 1;

	UINT32 uIdx;
	for ( uIdx = 0; uIdx < D_ACC_MAX_BLOCKING_COUNT_PER_CLUMP; uIdx++ )
		pcsAgcdTemplate->m_lPickingNodeIndex[uIdx] = 1;
	//@}

	//@{ 2006/03/24 burumal
	pcsAgcdTemplate->m_bTagging = FALSE;
	//@} 

	//@{ 2006/08/22 burumal
	pcsAgcdTemplate->m_lLookAtNode = 1; // default look-at node
	//@}
	
	pcsAgcdTemplate->m_lFaceAtomicIndex		= -1;

	for( int i = 0; i < AGCMCHAR_MAX_ANIM_TYPE; i++ )
	{
		pcsAgcdTemplate->m_afDefaultAnimSpeedRate[ i ] = 1.0f;
	}

	//@{ 2006/08/28 burumal
	pcsAgcdTemplate->m_bUseBending = FALSE;
	pcsAgcdTemplate->m_fBendingFactor = 1.0f;
	pcsAgcdTemplate->m_fBendingDegree = 0.0f;
	//@}
	
	return TRUE;
}

BOOL AgcmCharacter::RemovePreData(AgcdCharacterTemplate *pcsAgcdCharacterTemplate)
{
	if (pcsAgcdCharacterTemplate->m_pcsPreData)
	{
		if( pcsAgcdCharacterTemplate->m_pcsPreData->m_pszLabel ) {
			delete [] pcsAgcdCharacterTemplate->m_pcsPreData->m_pszLabel;
			pcsAgcdCharacterTemplate->m_pcsPreData->m_pszLabel = NULL;
		}

		if( pcsAgcdCharacterTemplate->m_pcsPreData->m_pszDFFName ) {
			delete [] pcsAgcdCharacterTemplate->m_pcsPreData->m_pszDFFName;
			pcsAgcdCharacterTemplate->m_pcsPreData->m_pszDFFName = NULL;
		}

		if( pcsAgcdCharacterTemplate->m_pcsPreData->m_pszDADFFName ) {
			delete [] pcsAgcdCharacterTemplate->m_pcsPreData->m_pszDADFFName;
			pcsAgcdCharacterTemplate->m_pcsPreData->m_pszDADFFName = NULL;
		}

		if( pcsAgcdCharacterTemplate->m_pcsPreData->m_pszPickDFFName ) {
			delete [] pcsAgcdCharacterTemplate->m_pcsPreData->m_pszPickDFFName;
			pcsAgcdCharacterTemplate->m_pcsPreData->m_pszPickDFFName = NULL;
		}

		delete pcsAgcdCharacterTemplate->m_pcsPreData;
		pcsAgcdCharacterTemplate->m_pcsPreData	= NULL;
	}

	return TRUE;
}

BOOL AgcmCharacter::DeleteTemplateAnimation(AgcdCharacterTemplate *pstTemplate)
{
	INT32				lNumAnimType2	= GetAnimType2Num(pstTemplate);
	AgcdAnimData2		*pcsCurrent			= NULL;
	ACA_AttachedData	*pcsAttachedData	= NULL;

	for (INT32 lAnimType = 0; lAnimType < AGCMCHAR_MAX_ANIM_TYPE; ++lAnimType)
	{
		for (INT32 lAnimType2 = 0; lAnimType2 < lNumAnimType2; ++lAnimType2)
		{
			if (	!pstTemplate->m_pacsAnimationData ||
					(!pstTemplate->m_pacsAnimationData[lAnimType][lAnimType2]) ||
					(!pstTemplate->m_pacsAnimationData[lAnimType][lAnimType2]->m_pcsAnimation)	)
				continue;

			pcsCurrent	= pstTemplate->m_pacsAnimationData[lAnimType][lAnimType2]->m_pcsAnimation->m_pcsHead;
			while (pcsCurrent)
			{
				pcsAttachedData	=
					(ACA_AttachedData *)(m_csAnimation2.GetAttachedData(
						AGCD_CHARACTER_ANIMATION_ATTACHED_DATA_KEY,
						pcsCurrent										));
				if (!pcsAttachedData)
				{
					ASSERT(!"!pcsAttachedData");
				}
				else
				{
					if (pcsAttachedData->m_pszPoint)
					{
						delete [] pcsAttachedData->m_pszPoint;
						pcsAttachedData->m_pszPoint = NULL;
					}

					if (pcsAttachedData->m_pcsBlendingData)
					{
						m_csAnimation2.RemoveAnimData(&pcsAttachedData->m_pcsBlendingData);
					}

					if (pcsAttachedData->m_pcsSubData)
					{
						m_csAnimation2.RemoveAnimData(&pcsAttachedData->m_pcsSubData);
					}
				}

				pcsCurrent	= pcsCurrent->m_pcsNext;
			}

			m_csAnimation2.RemoveAllAnimation(&pstTemplate->m_pacsAnimationData[lAnimType][lAnimType2]->m_pcsAnimation);

			delete pstTemplate->m_pacsAnimationData[lAnimType][lAnimType2];
			pstTemplate->m_pacsAnimationData[lAnimType][lAnimType2]	= NULL;
		}

		if ( pstTemplate->m_pacsAnimationData )
		{
			delete [] pstTemplate->m_pacsAnimationData[lAnimType];
			pstTemplate->m_pacsAnimationData[lAnimType] = NULL;
		}
	}

	if ( pstTemplate )
	{
		delete [] pstTemplate->m_pacsAnimationData;
		pstTemplate->m_pacsAnimationData = NULL;
	}

	return TRUE;
}

BOOL AgcmCharacter::TemplateDestructor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmCharacter *pThis = (AgcmCharacter *) pClass;
	AgpdCharacterTemplate *pstAgpdCharacterTemplate = (AgpdCharacterTemplate *) pData;
	AgcdCharacterTemplate *pstAgcdCharacterTemplate = pThis->GetTemplateData(pstAgpdCharacterTemplate);

	pThis->m_pcsAgcmRender->m_csRenderType.DeleteClumpRenderType(&pstAgcdCharacterTemplate->m_csClumpRenderType);

	pThis->ReleaseTemplateClump(pstAgcdCharacterTemplate);
	pThis->DeleteTemplateAnimation(pstAgcdCharacterTemplate);

	if (pstAgcdCharacterTemplate->m_stLOD.m_pstList)
	{
		if (pThis->m_pcsAgcmLODManager)
			pThis->m_pcsAgcmLODManager->m_csLODList.RemoveAllLODData(&pstAgcdCharacterTemplate->m_stLOD);

		pstAgcdCharacterTemplate->m_stLOD.m_pstList = NULL;
	}

	pThis->RemovePreData(pstAgcdCharacterTemplate);

	if( pstAgcdCharacterTemplate->m_pcsDefaultHeadData ) {
		delete pstAgcdCharacterTemplate->m_pcsDefaultHeadData;
		pstAgcdCharacterTemplate->m_pcsDefaultHeadData = NULL;
	}

	pstAgcdCharacterTemplate->m_pClump				= NULL;
	pstAgcdCharacterTemplate->m_pDefaultArmourClump	= NULL;
	pstAgcdCharacterTemplate->m_pPickingAtomic		= NULL;

	return TRUE;
}

RpAtomic *	AgcmCharacter::CBRemoveCheckAtomic		( RpAtomic	*	pstAtomic	, PVOID pvData )
{
	return pstAtomic;
}

INT32 AgcmCharacter::ConvertAnimType2(INT32 lAnimType2, INT32 lAnimEquip)
{
	if (!(m_ulModuleFlags & E_AGCM_CHAR_FLAGS_CONVERT_ANIMTYPE2))
		return lAnimEquip;

	switch (lAnimType2)
	{
	case AGCMCHAR_AT2_WARRIOR:
		{
			if (lAnimEquip > 6)
				return -2;

			return lAnimEquip;
		}
		break;

	case AGCMCHAR_AT2_ARCHER:
		{
			if (lAnimEquip > 0)
			{
				if (	(lAnimEquip < 7) ||
						(lAnimEquip > 8)	)
					return -2;
			}

			return (lAnimEquip == 0) ? (0) : (lAnimEquip - 6);
		}
		break;

	case AGCMCHAR_AT2_WIZARD:
		{
			if (lAnimEquip > 0)
			{
				if (	(lAnimEquip < 10) ||
						(lAnimEquip > 11)	)
					return -2;
			}

			return (lAnimEquip == 0) ? (0) : (lAnimEquip - 9);
		}
		break;

	case AGCMCHAR_AT2_BASE:
	case AGCMCHAR_AT2_BOSS_MONSTER:
		{
			if (lAnimEquip != 0)
				return -1;

			return 0;
		}
		break;

	//@{ 2006/06/07 burumal
	case AGCMCHAR_AT2_ARCHLORD :
		{
			if ( lAnimEquip > 2 )
				return -1;

			return lAnimEquip;

			//return 0;
		}
		break;
	//@}
	}

	return -1;
}

RpAtomic *AgcmCharacter::UnHookLODAtomic(RpAtomic *atomic, void *data)
{
	RpClump *	pstClump;

	pstClump = RpAtomicGetClump(atomic);
	if (pstClump)
	{
		if (RpAtomicGetFrame(atomic) == RpClumpGetFrame(pstClump))
		{
			m_pThisAgcmCharacter->LockFrame();
			RwFrame *	pstFrame = RwFrameCreate();

			RpAtomicSetFrame(atomic, pstFrame);

			RwFrameAddChild(RpClumpGetFrame(pstClump), pstFrame);
			m_pThisAgcmCharacter->UnlockFrame();
		}
	}
	RpLODAtomicUnHookRender(atomic);

	return atomic;
}

RwTexture *	AgcmCharacter::CBTextureRead(const RwChar *name, const RwChar *maskName)
{
	RwTexture *pstTexture;

	pstTexture = RwD3D9DDSTextureRead(name, maskName);
	if (!pstTexture)
		pstTexture = m_fnDefaultTextureReadCB(name, maskName);

	if (pstTexture)
	{
		//RwTextureSetFilterMode(pstTexture, rwFILTERLINEARMIPLINEAR);
	}

	return pstTexture;
}

RpClump *AgcmCharacter::LoadClump(CHAR *szDFF)
{
	RwStream *	stream	= (RwStream *	)NULL;
	RpClump *	clump	= (RpClump *	)NULL;
	RwChar *	path	= (RwChar *		)NULL;
//	CHAR		szOldPath[AGCMCHAR_MAX_PATH];
	CHAR		szFullPath[AGCMCHAR_MAX_PATH];

	sprintf(szFullPath, "%s%s", m_szClumpPath, szDFF);

	if( m_pcsAgcmResourceLoader )
	{
		clump = m_pcsAgcmResourceLoader->LoadClump(szFullPath, "Character.txd", NULL, rwFILTERMIPLINEAR, m_szTexturePath);
	}
	//@{ Jaewon 20050629
	// Seize the crash!
	else
		MD_SetErrorMessage("[%d] m_pcsAgcmResourceLoader is NULL, so AgcmCharacter::LoadClump() failed.\n", GetCurrentThreadId());
	//@} Jaewon

//	SetCurrentDirectory(szOldPath);

	return clump;
}

//@{ Jaewon 20051118
static float getBalrogUVAnimFxWeight(float oldHpNorm, float newHpNorm)
{
	// Check whether the transition is necessary.
	INT32 weightIndex = 0;
	float weight[] = { 0.0f, 0.2f, 0.5f, 0.8f };
	if(newHpNorm < 0.25f)
	{
		if(oldHpNorm >= 0.25f)
			weightIndex = 3;
	}
	else if(newHpNorm < 0.5f)
	{
		if(oldHpNorm >= 0.5f)
			weightIndex = 2;
	}
	else if(newHpNorm < 0.75f)
	{
		if(oldHpNorm >= 0.75f)
			weightIndex = 1;
	}

	return weight[weightIndex];
}
//@} Jaewon
//@{ Jaewon 20051102
#include <AcuRpMatD3DFx/AcuRpMatD3DFx.h>
static RpAtomic *adjustUVAnimFxWeightCB(RpAtomic *atomic, void *data)
{
	const char *const fxParamName = "weightBase";
	float weight = *(float*)data;
	RpGeometry *geometry = RpAtomicGetGeometry(atomic);
	if(geometry)
	{
		for(INT32 i=0; i<RpGeometryGetNumMaterials(geometry); ++i)
		{
			RpMaterial *material = RpGeometryGetMaterial(geometry, i);
			if(RpMaterialD3DFxGetEffect(material) == NULL)
				return NULL;
			ASSERT(stricmp(RpMaterialD3DFxGetEffect(material)->pSharedD3dXEffect->name, "uvAnimAddAlpha3.fx")==0);
			RwBool successs = RpMaterialD3DFxSetFloat(material, const_cast<char*>(fxParamName), weight);
			ASSERT(successs);
		}
	}

	return atomic;
}
//@} Jaewon

BOOL AgcmCharacter::InitCharacterBackThread(AgpdCharacter *pcsAgpdCharacter)
{
	AgcdCharacter *				pstAgcdCharacter	= GetCharacterData(pcsAgpdCharacter);
	AgpdCharacterTemplate *		pcsAgpdCharacterTemplate;
	AgcdCharacterTemplate *		pstAgcdCharacterTemplate;
	RwSphere *					bs = NULL;
	AgcdLODData	*				pstLODData = NULL;
	
	if ((!pstAgcdCharacter) || (!pstAgcdCharacter->m_pstAgcdCharacterTemplate) || pstAgcdCharacter->m_pClump)
		return FALSE;

	if (pstAgcdCharacter->m_lStatus & AGCMCHAR_STATUS_FLAG_REMOVED)
	{
		TRACE("AgcmCharacter::InitCharacterBackThread() Error Removed Character (%d,%s)!!!\n", pcsAgpdCharacter->m_lID, pcsAgpdCharacter->m_szID);
		return FALSE;
	}

	pcsAgpdCharacterTemplate = (AgpdCharacterTemplate *) pcsAgpdCharacter->m_pcsCharacterTemplate;
	if (!pcsAgpdCharacterTemplate)
		return FALSE;

	pstAgcdCharacterTemplate = pstAgcdCharacter->m_pstAgcdCharacterTemplate;
	if (!LoadTemplateData(pcsAgpdCharacterTemplate, pstAgcdCharacter->m_pstAgcdCharacterTemplate)) return FALSE;

	pstAgcdCharacterTemplate = pstAgcdCharacter->m_pstAgcdCharacterTemplate;
	if ( NULL == pstAgcdCharacterTemplate )	return FALSE;

	if (!pstAgcdCharacterTemplate->m_pClump) return FALSE;

	{
		PROFILE("AgcmCharacter::InitCharacterBackThread - ClumpClone");

		// Ŭ������ �����Ѵ�~
		LockFrame();
		pstAgcdCharacter->m_pClump = RpClumpClone(pstAgcdCharacterTemplate->m_pClump);		
		UnlockFrame();

		if (!pstAgcdCharacter->m_pClump) return FALSE;

		static AgpmSiegeWar * pmAgpmSiegeWar = ( AgpmSiegeWar * ) GetModule("AgpmSiegeWar");

		if( pmAgpmSiegeWar->IsSiegeWarMonster( pcsAgpdCharacter ) )
		{
			// �����̸� �ø����� ����.
			pstAgcdCharacter->m_pClump->ulFlag |= RWFLAG_DONOT_CULL;
		}

		//@{ Jaewon 20051118
		const INT32 balrogId = 817;
		if(pcsAgpdCharacter->m_lTID1==balrogId)
		{
			// Adjust Balrog's uv-animation-fx weight according to its HP.
				INT32 curHp, maxHp;
				AgpdFactor *pcsFactorResult = (AgpdFactor*)m_pcsAgpmFactors->GetFactor(&pcsAgpdCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT);
				if(pcsFactorResult)
				{
					m_pcsAgpmFactors->GetValue(pcsFactorResult, &curHp, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
					m_pcsAgpmFactors->GetValue(pcsFactorResult, &maxHp, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);

					if(curHp > maxHp)
						curHp = maxHp;

					float newHpNorm;
					if(maxHp <= 0)
						newHpNorm = 0;
					else
						newHpNorm = (float)curHp/(float)maxHp;

					float weight = getBalrogUVAnimFxWeight(1.0f, newHpNorm);
					if(weight)
						RpClumpForAllAtomics(pstAgcdCharacter->m_pClump, adjustUVAnimFxWeightCB, &weight);
				}
		}
		//@} Jaewon

		pstAgcdCharacter->m_pClump->pvApBase = pcsAgpdCharacter;
#ifdef _DEBUG
		if (pcsAgpdCharacter->m_pcsCharacterTemplate)
			//@{ Jaewon 20050905
			// ;)
			//pstAgcdCharacter->m_pClump->szName = pcsAgpdCharacter->m_pcsCharacterTemplate->m_szTName;
			RpClumpSetName(pstAgcdCharacter->m_pClump, pcsAgpdCharacter->m_pcsCharacterTemplate->m_szTName);
			//@} Jaewon
#endif 
	}

	{
		PROFILE("AgcmCharacter::InitCharacterBackThread - Hierarchy");

		// Hierarchy�� �����Ѵ�~
		pstAgcdCharacter->m_pInHierarchy = GetHierarchy(pstAgcdCharacter->m_pClump);
		if (!pstAgcdCharacter->m_pInHierarchy) return FALSE;

		RpHAnimHierarchySetFlags(pstAgcdCharacter->m_pInHierarchy,
								 (RpHAnimHierarchyFlag)
								 ( RpHAnimHierarchyGetFlags(pstAgcdCharacter->m_pInHierarchy) | 
								   rpHANIMHIERARCHYUPDATELTMS |
								   rpHANIMHIERARCHYUPDATEMODELLINGMATRICES ) );// | rpHANIMHIERARCHYLOCALSPACEMATRICES) );

		RpHAnimHierarchyFlag flags = (RpHAnimHierarchyFlag) (pstAgcdCharacter->m_pInHierarchy)->flags;

		LockFrame();

		pstAgcdCharacter->m_pOutHierarchy = RpHAnimHierarchyCreateFromHierarchy(pstAgcdCharacter->m_pInHierarchy,
			flags, AGCMCHAR_HIERARCHY_MAX_KEYFRAME);
		pstAgcdCharacter->m_pInHierarchy2 = RpHAnimHierarchyCreateFromHierarchy(pstAgcdCharacter->m_pInHierarchy,
			flags, AGCMCHAR_HIERARCHY_MAX_KEYFRAME);

		pstAgcdCharacter->m_pOutHierarchy->parentFrame = pstAgcdCharacter->m_pInHierarchy->parentFrame;
		pstAgcdCharacter->m_pInHierarchy2->parentFrame = pstAgcdCharacter->m_pInHierarchy->parentFrame;

		RpHAnimHierarchySetKeyFrameCallBacks(pstAgcdCharacter->m_pOutHierarchy, rpHANIMSTDKEYFRAMETYPEID);

		RpHAnimHierarchyAttach(pstAgcdCharacter->m_pInHierarchy);
		RpHAnimHierarchyAttach(pstAgcdCharacter->m_pInHierarchy2);
		RpHAnimHierarchyAttach(pstAgcdCharacter->m_pOutHierarchy);

		UnlockFrame();

		// Sub-hierarchy�� �����Ѵ�.
		if (RpHAnimIDGetIndex(pstAgcdCharacter->m_pInHierarchy, AGCMCHAR_SUBANIM_NODEINDEX_SPINE) < 0)
		{
			pstAgcdCharacter->m_csSubAnim.m_pInHierarchy	= NULL;
			pstAgcdCharacter->m_csSubAnim.m_pInHierarchy2	= NULL;
			pstAgcdCharacter->m_csSubAnim.m_pOutHierarchy	= NULL;
		}
		else
		{
			pstAgcdCharacter->m_csSubAnim.m_pInHierarchy	= CreateSubHierarchy(pstAgcdCharacter->m_pInHierarchy, AGCMCHAR_SUBANIM_NODEINDEX_SPINE);
			pstAgcdCharacter->m_csSubAnim.m_pInHierarchy2	= CreateSubHierarchy(pstAgcdCharacter->m_pInHierarchy, AGCMCHAR_SUBANIM_NODEINDEX_SPINE);
			pstAgcdCharacter->m_csSubAnim.m_pOutHierarchy	= CreateSubHierarchy(pstAgcdCharacter->m_pInHierarchy, AGCMCHAR_SUBANIM_NODEINDEX_SPINE);

			pstAgcdCharacter->m_csSubAnim.m_pOutHierarchy->currentAnim->keyFrameApplyCB	= RpHAnimKeyFrameApply;
			pstAgcdCharacter->m_csSubAnim.m_pOutHierarchy->currentAnim->keyFrameAddCB	= RpHAnimKeyFrameAdd;
		}

		pstAgcdCharacter->m_csSubAnim.InitComponent();
	}

	RwFrame* pstClumpFrame = RpClumpGetFrame(pstAgcdCharacter->m_pClump);
	if (!pstClumpFrame)
		return FALSE;

	// scale
	pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_stScale.x = pstAgcdCharacterTemplate->m_fScale;
	pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_stScale.y = pstAgcdCharacterTemplate->m_fScale;
	pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_stScale.z = pstAgcdCharacterTemplate->m_fScale;

	LockFrame();
	RwFrameScale(pstClumpFrame, &pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_stScale, rwCOMBINEREPLACE);
	UnlockFrame();

	{
		PROFILE("AgcmCharacter::InitCharacterBackThread - PickAtomic");

		pstAgcdCharacter->m_lStatus |= AGCMCHAR_STATUS_FLAG_LOAD_CLUMP;

		// Picking�� atomic�� �����Ѵ�.
		if ( pstAgcdCharacterTemplate->m_pPickingAtomic )
		{
			LockFrame();

			//@{ 2006/02/14 burumal 

			//pstAgcdCharacter->m_pPickAtomic = RpAtomicClone(pstAgcdCharacterTemplate->m_pPickingAtomic);

			UINT32 uAtomicCount = 0;

			RpAtomic* pAtomicTable[D_ACC_MAX_BLOCKING_COUNT_PER_CLUMP];
			ZeroMemory(pAtomicTable, sizeof(RpAtomic*) * D_ACC_MAX_BLOCKING_COUNT_PER_CLUMP);
			
			RpAtomic* pTempAtomic = pstAgcdCharacterTemplate->m_pPickingAtomic;
			if ( pTempAtomic )
			{
				do {
					pAtomicTable[uAtomicCount++] = pTempAtomic;

					pTempAtomic = pTempAtomic->next;

					if ( !pTempAtomic || (pAtomicTable[0] == pTempAtomic) )
						break;

				} while(true);
			}
			
			pTempAtomic = pstAgcdCharacterTemplate->m_pPickingAtomic;
			
			for ( UINT32 uIdx = 0; uIdx < uAtomicCount; uIdx++ )
			{
				pAtomicTable[uIdx] = RpAtomicClone(pAtomicTable[uIdx]);				
				pTempAtomic = pTempAtomic->next;
			}

			for ( UINT32 uIdx = 0; uIdx < uAtomicCount; uIdx++ )
			{
				if ( uIdx > 0 )
				{
					pAtomicTable[uIdx]->prev = pAtomicTable[uIdx - 1];
					pAtomicTable[uIdx]->next = pAtomicTable[(uIdx + 1) % uAtomicCount];
				}
				else
				{
					pAtomicTable[0]->prev = pAtomicTable[uAtomicCount - 1];
					pAtomicTable[0]->next = pAtomicTable[1 % uAtomicCount];
				}
			}

			pstAgcdCharacter->m_pPickAtomic = pAtomicTable[0];
			pstAgcdCharacter->m_pOrgPickAtomic = pstAgcdCharacter->m_pPickAtomic;
			
			//@}

			//@{ Jaewon 20050906
#ifdef _DEBUG
			std::string strTmp(RpAtomicGetName(pstAgcdCharacter->m_pPickAtomic));
			strTmp += "_cloned";
			RpAtomicSetName(pstAgcdCharacter->m_pPickAtomic, strTmp.c_str());		
#endif
			ZeroMemory(pstAgcdCharacter->m_pstNodeFrame, sizeof(RwFrame*) * D_ACC_MAX_BLOCKING_COUNT_PER_CLUMP);
			RpAtomic* pCurPickAtomic = pstAgcdCharacter->m_pPickAtomic;
			ASSERT(pCurPickAtomic);

			for ( UINT32 uIdx = 0; uIdx < uAtomicCount; uIdx++ )
			{
				INT32 nCurPickNodeIndex = pstAgcdCharacterTemplate->m_lPickingNodeIndex[uIdx];

				if ( pstAgcdCharacterTemplate->m_bTagging )
					nCurPickNodeIndex = RpHAnimIDGetIndex(pstAgcdCharacter->m_pInHierarchy, nCurPickNodeIndex);

				if ( pstAgcdCharacter->m_pInHierarchy && 
					(nCurPickNodeIndex >= 0) && 
					(pstAgcdCharacter->m_pInHierarchy->numNodes > nCurPickNodeIndex) )
				{
					pstAgcdCharacter->m_pstNodeFrame[uIdx] = 
						pstAgcdCharacter->m_pInHierarchy->pNodeInfo[nCurPickNodeIndex].pFrame;					
				}

				RwMatrix	stInvMatrix;

				if ( pstAgcdCharacter->m_pstNodeFrame[uIdx] )
				{
					RpAtomicSetFrame(pCurPickAtomic, RwFrameCreate());

					RpAtomic* pCurParent = NULL;

					if ( uIdx > 0 )
					{
						for ( UINT32 uTempIdx = uIdx; uTempIdx > 0; uTempIdx-- )
						{
							if ( nCurPickNodeIndex == pstAgcdCharacterTemplate->m_lPickingNodeIndex[uTempIdx - 1] ) 
							{
								pCurParent = pstAgcdCharacter->m_pPickAtomic;
								for ( UINT32 uTempLoop = 0; uTempLoop < uTempIdx - 1; uTempLoop++ )
									pCurParent = pCurParent->next;
								break;
							}
						}
					}
					
					if ( pCurParent == NULL )
					{
						RwMatrixInvert(&stInvMatrix, RwFrameGetLTM(pstAgcdCharacter->m_pstNodeFrame[uIdx]));
						RwFrameTransform(RpAtomicGetFrame(pCurPickAtomic), RwFrameGetLTM(pstClumpFrame), rwCOMBINEREPLACE);
						RwFrameTransform(RpAtomicGetFrame(pCurPickAtomic), &stInvMatrix, rwCOMBINEPOSTCONCAT);
						RwFrameAddChild(pstAgcdCharacter->m_pstNodeFrame[uIdx], RpAtomicGetFrame(pCurPickAtomic));
					}
					else
					{						
						RwMatrixInvert(&stInvMatrix, RwFrameGetLTM(RpAtomicGetFrame(pCurParent)));
						RwFrameTransform(RpAtomicGetFrame(pCurPickAtomic), RwFrameGetLTM(pstClumpFrame), rwCOMBINEREPLACE);
						RwFrameTransform(RpAtomicGetFrame(pCurPickAtomic), &stInvMatrix, rwCOMBINEPOSTCONCAT);
						RwFrameAddChild(RpAtomicGetFrame(pCurParent), RpAtomicGetFrame(pCurPickAtomic));
					}
				}
				else
				{
					RpAtomicSetFrame(pCurPickAtomic, RpClumpGetFrame(pstAgcdCharacter->m_pClump));
				}
				
				pCurPickAtomic = pCurPickAtomic->next;
			}
			//@}

			UnlockFrame();

//			bs = RpAtomicGetBoundingSphere(pstAgcdCharacter->m_pPickAtomic);
			bs = &pstAgcdCharacterTemplate->m_stBSphere;
		}
	}

	pstAgcdCharacter->m_lStatus |= AGCMCHAR_STATUS_FLAG_INIT_HIERARCHY;

	{
		PROFILE("AgcmCharacter::InitCharacterBackThread - 4");

		if (m_pcsAgcmLODManager)
			pstLODData = m_pcsAgcmLODManager->GetLODData(&pstAgcdCharacterTemplate->m_stLOD, 0, FALSE);

		if (bs)
		{
			// BBOX �ӽ÷� ������
			float	ty = bs->center.y + bs->radius;
			float	x1 = bs->center.x - bs->radius;
			float	x2 = bs->center.x + bs->radius;
			float	z1 = bs->center.z - bs->radius;
			float	z2 = bs->center.z + bs->radius;

			pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[0].x	= x1;
			pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[0].y	= ty;
			pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[0].z	= z1;
			pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[1].x	= x1;
			pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[1].y	= ty;
			pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[1].z	= z2;
			pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[2].x	= x2;
			pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[2].y	= ty;
			pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[2].z	= z1;
			pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[3].x	= x2;
			pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[3].y	= ty;
			pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[3].z	= z2;
		}
		else
		{
			float	ty = 150.0f;
			float	x1 = -80.0f;
			float	x2 = 80.0f;
			float	z1 = -80.0f;
			float	z2 = 80.0f;

			pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[0].x	= x1;
			pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[0].y	= ty;
			pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[0].z	= z1;
			pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[1].x	= x1;
			pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[1].y	= ty;
			pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[1].z	= z2;
			pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[2].x	= x2;
			pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[2].y	= ty;
			pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[2].z	= z1;
			pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[3].x	= x2;
			pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[3].y	= ty;
			pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[3].z	= z2;
		}

		// ���� UsrData�� �����Ѵ�.
		AcuObject::SetClumpType(pstAgcdCharacter->m_pClump												,
								ACUOBJECT_TYPE_CHARACTER | pstAgcdCharacterTemplate->m_lObjectType		,
								pcsAgpdCharacter->m_lID													,
								pcsAgpdCharacter														,
								pcsAgpdCharacterTemplate												,
								((pstLODData) && (pstLODData->m_ulMaxLODLevel)) ? (pstLODData) : (NULL)	,
								pstAgcdCharacter->m_pPickAtomic											,
								bs																		,
								NULL																	,
								&pstAgcdCharacterTemplate->m_stOcTreeData);

	}

//	{
//		if (pstAgcdCharacterTemplate->m_csClumpRenderType.m_lSetCount > 0)
//		{
//			m_pcsAgcmRender->ClumpSetRenderType(pstAgcdCharacter->m_pClump, &pstAgcdCharacterTemplate->m_csClumpRenderType);
//		}
//	}

	return TRUE;
}

BOOL AgcmCharacter::InitCharacterMainThread(AgpdCharacter *pstAgpdCharacter)
{
	PROFILE("AgcmCharacter::InitCharacterMainThread");

	AgcdCharacter *				pstAgcdCharacter;
	AgpdCharacterTemplate *		pcsAgpdCharacterTemplate;
	AgcdCharacterTemplate *		pstAgcdCharacterTemplate;
	RwSphere *					bs = NULL;
	AgcdLODData	*				pstLODData = NULL;

	pstAgcdCharacter = GetCharacterData(pstAgpdCharacter);

	// 2005.2.14 gemani �ٽ� �齺����� �Ű���ϴ� hierarchy���� �κ�
	if (!(pstAgcdCharacter->m_lStatus & AGCMCHAR_STATUS_FLAG_INIT_HIERARCHY) || pstAgcdCharacter->m_lStatus & AGCMCHAR_STATUS_FLAG_REMOVED)
		return TRUE;

	pcsAgpdCharacterTemplate = (AgpdCharacterTemplate *) pstAgpdCharacter->m_pcsCharacterTemplate;
	if (!pcsAgpdCharacterTemplate)
		return FALSE;

	pstAgcdCharacterTemplate = pstAgcdCharacter->m_pstAgcdCharacterTemplate;

	if (!pstAgcdCharacter->m_pClump)
		return TRUE;

	{
		PROFILE("AgcmCharacter::InitCharacterMainThread - Prelight");

		// PreLight�� �����Ѵ�.
		if (	(pstAgcdCharacterTemplate->m_pcsPreData) &&
				(pstAgcdCharacterTemplate->m_lObjectType & ACUOBJECT_TYPE_USE_PRE_LIGHT)	)
		{
			AcuObject::SetClumpPreLitLim(pstAgcdCharacter->m_pClump, &pstAgcdCharacterTemplate->m_pcsPreData->m_stPreLight);
		}
	}

	{
		PROFILE("AgcmCharacter::InitCharacterMainThread - Animation");

		// �⺻ �ִϸ��̼� �ӵ��� �����Ѵ�.
		pstAgcdCharacter->m_fSkillSpeedRate = 1.0f;
		for (INT16 nType = AGCMCHAR_ANIM_TYPE_WAIT; nType < AGCMCHAR_MAX_ANIM_TYPE; ++nType)
			pstAgcdCharacter->m_afAnimSpeedRate[nType] = 1.0f;

		/*if (pstAgpdCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
		{
			SetDeadAnimation(pstAgpdCharacter, pstAgcdCharacter);
		}
		else
		{*/
			if (pstAgpdCharacter->m_bMove)
			{
				if (pstAgpdCharacter->m_bMoveFast)
				{
/*					if(	(!pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_paAnim[AGCMCHAR_ANIM_TYPE_RUN][pstAgcdCharacter->m_lAnimHandEquipType]) &&
						(!pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_paAnim[AGCMCHAR_ANIM_TYPE_RUN][0])											)*/
					/*if (	(!pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_csAnimationData.m_ppcsAnimation[AGCMCHAR_ANIM_TYPE_RUN][pstAgcdCharacter->m_lAnimHandEquipType]) &&
							(!pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_csAnimationData.m_ppcsAnimation[AGCMCHAR_ANIM_TYPE_RUN][0])											)*/
					if (	(!pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_pacsAnimationData[AGCMCHAR_ANIM_TYPE_RUN][pstAgcdCharacter->m_lCurAnimType2] || !pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_pacsAnimationData[AGCMCHAR_ANIM_TYPE_RUN][pstAgcdCharacter->m_lCurAnimType2]->m_pcsAnimation) &&
							(!pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_pacsAnimationData[AGCMCHAR_ANIM_TYPE_RUN][0] || !pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_pacsAnimationData[AGCMCHAR_ANIM_TYPE_RUN][0]->m_pcsAnimation))
						StartAnimation(pstAgpdCharacter, AGCMCHAR_ANIM_TYPE_WALK);
					else
						StartAnimation(pstAgpdCharacter, AGCMCHAR_ANIM_TYPE_RUN);
				}
				else
				{
					StartAnimation(pstAgpdCharacter, AGCMCHAR_ANIM_TYPE_WALK);
				}
			}
			else
			{
				StartAnimation(pstAgpdCharacter, AGCMCHAR_ANIM_TYPE_WAIT);
			}
		//}

/*		if (pstAgpdCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
		{
			SetDeadAnimation(pstAgpdCharacter, pstAgcdCharacter);
		}
		else
		{
			switch (pstAgcdCharacter->m_eCurAnimType)
			{
			case AGCMCHAR_ANIM_TYPE_NONE:
				{
					DropAnimation(pstAgcdCharacter);
				}
				break;

			default:
				{
					if (pstAgpdCharacter->m_bMove)
					{
						if (pstAgpdCharacter->m_bMoveFast)
						{
							AgcmCharacterAnimType eAnimType;

							if(	(!pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_paAnim[AGCMCHAR_ANIM_TYPE_RUN][pstAgcdCharacter->m_lAnimHandEquipType]) &&
								(!pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_paAnim[AGCMCHAR_ANIM_TYPE_RUN][0])											)
								eAnimType = AGCMCHAR_ANIM_TYPE_WALK;
							else
								eAnimType = AGCMCHAR_ANIM_TYPE_RUN;

							StartAnimation(pstAgpdCharacter, eAnimType);
						}
						else
							StartAnimation(pstAgpdCharacter, AGCMCHAR_ANIM_TYPE_WALK);
					}
					else
						StartAnimation(pstAgpdCharacter, AGCMCHAR_ANIM_TYPE_WAIT);
				}
				break;			
			}
		}*/
	}

	{
		PROFILE("AgcmCharacter::InitCharacterMainThread - Etc");

		// Shadow�� �����Ѵ�.
		if (m_lSelfCID == pstAgpdCharacter->m_lID)
		{
			SetSelfCharacter(pstAgpdCharacter);
			AcuObject::SetClumpDInfo_Shadow(pstAgcdCharacter->m_pClump,1);
		}
		else
		{
			AcuObject::SetClumpDInfo_Shadow(pstAgcdCharacter->m_pClump,2);
		}

		if (m_pcsAgcmShadow)
			m_pcsAgcmShadow->AddShadow(pstAgcdCharacter->m_pClump);

		if (m_pcsAgcmShadowmap)
			m_pcsAgcmShadowmap->registerClump(pstAgcdCharacter->m_pClump);

		pstAgcdCharacter->m_lStatus |= AGCMCHAR_STATUS_FLAG_INIT_COMPLETE;

		// netong �߰� (÷�� �߰��� ĳ���͵��� ���ӿ��� ���߿� ���� �ִ°�� ó����)
		m_pcsAgpmCharacter->UpdatePosition(pstAgpdCharacter, NULL);

		{
			static AgpmSiegeWar * pmAgpmSiegeWar = ( AgpmSiegeWar * ) GetModule("AgpmSiegeWar");

			if( pmAgpmSiegeWar->IsSiegeWarMonster( pstAgpdCharacter ) )
			{
				// �����̸� �ø����� ����.
				pstAgcdCharacter->m_pClump->ulFlag |= RWFLAG_DONOT_CULL;
			}
		}

		// LOD�� �����Ѵ�.
		if (m_pcsAgcmLODManager)
		{
			pstLODData = m_pcsAgcmLODManager->GetLODData(&pstAgcdCharacterTemplate->m_stLOD, 0, FALSE);
			if ((pstLODData) && (pstLODData->m_ulMaxLODLevel))
			{
				//. 2006. 2. 8. Nonstopdj
				m_pcsAgcmLODManager->SetCharacterDistanceRate(pstLODData);

				m_pcsAgcmLODManager->SetLODCallback(pstAgcdCharacter->m_pClump);
				AcuObject::SetClumpDInfo_LODLevel(pstAgcdCharacter->m_pClump, pstLODData->m_ulMaxLODLevel);
			}
		}

		// �ʱ�ȭ ���� �ݹ��� �ҷ��ش�.
		EnumCallback(AGCMCHAR_CB_ID_INIT_CHARACTER, (PVOID)(pstAgpdCharacter), (PVOID)(pstAgcdCharacter));
	}

	{
		if (pstAgcdCharacterTemplate->m_csClumpRenderType.m_lSetCount > 0)
		{
			m_pcsAgcmRender->ClumpSetRenderType(pstAgcdCharacter->m_pClump, &pstAgcdCharacterTemplate->m_csClumpRenderType);
		}
	}

	// ���忡 �߰��Ѵ�.
	if(m_pWorld)
	{
		PROFILE("AgcmCharacter::InitCharacterMainThread - AddWorld");

		if (m_pcsAgcmRender)
		{
			//@{ 2006/02/28 burumal			
			// PickAtomic�� Character Clump�� attach ��Ű�� �ʴ´�
			/*
			if (m_bShowPickingInfo)
				RpClumpAddAtomic(pstAgcdCharacter->m_pClump, pstAgcdCharacter->m_pPickAtomic);
			*/
			//@}

			m_pcsAgcmRender->AddClumpToWorld(pstAgcdCharacter->m_pClump);
		}
		else
		{
			RpWorldAddClump(m_pWorld, pstAgcdCharacter->m_pClump);
		}

		pstAgcdCharacter->m_lStatus |= AGCMCHAR_STATUS_FLAG_ADDED_WORLD;
		pstAgcdCharacter->m_bInShadow = FALSE;

		m_pcsAgcmRender->AddUpdateInfotoClump(pstAgcdCharacter->m_pClump,this,CBUpdateCharacter,NULL,pstAgpdCharacter,NULL);
	}


	OnUpdateCharacterCustomize( pstAgpdCharacter, pstAgcdCharacter );

	// 2005.10.10. steeple
	// ó�� �޾ƿ��� �� �̹� ���� ���¶�� �ٷ� �ҷ��ش�.
	if(m_pcsAgpmCharacter->IsStatusTransparent(pstAgpdCharacter))
		//@{ Jaewon 20051101
		// Make it invisible immediately, i.e. no fade.
		UpdateTransparent(pstAgpdCharacter, TRUE, 2000);
		//@} Jaewon


	//@{ 2006/11/03 burumal	
	if ( pstAgcdCharacter->m_nPreengagedEventEffectCount > 0 )
	{
		AgcmEventEffect* pEventEffect = (AgcmEventEffect*) GetModule("AgcmEventEffect");
		if ( pEventEffect )
		{
			INT32 nIdx;
			for ( nIdx = 0; nIdx < pstAgcdCharacter->m_nPreengagedEventEffectCount; nIdx++ )
			{	
				AgcdPreengagedEventEffect* pPreenagedEffect = pstAgcdCharacter->m_pPreengagedEventEffect[nIdx];
				ASSERT( pPreenagedEffect );

				if ( pPreenagedEffect )
				{
					ASSERT(pstAgpdCharacter == pPreenagedEffect->pcsAgpdTarget);

					if ( pstAgcdCharacter->m_pClump )
					{
						if ( pPreenagedEffect->lCommonEffectID == -1 )
						{
							pEventEffect->EventSkillAddPreengagedPassiveSkillEffect(pPreenagedEffect->lBuffedTID, pPreenagedEffect->lCasterTID, pPreenagedEffect->pcsAgpdTarget);
						}
						else
						{
							pEventEffect->SetCommonCharacterEffect(pPreenagedEffect->pcsAgpdTarget, (eAgcmEventEffectDataCommonCharType) pPreenagedEffect->lCommonEffectID, 0);
						}
					}

					delete pPreenagedEffect;

					pstAgcdCharacter->m_pPreengagedEventEffect[nIdx] = NULL;
				}
			}

			pstAgcdCharacter->m_nPreengagedEventEffectCount = 0;
		}
	}
	//@}

	return TRUE;
}

VOID AgcmCharacter::CheckCustAnimType(INT32 *plDestAnimType, AgcdCharacter *pstAgcdCharacter, AgcmCharacterAnimType eChangeType)
{
	switch (eChangeType)
	{
	case AGCMCHAR_ANIM_TYPE_WAIT:
		{
			if (pstAgcdCharacter->m_lLastAttackTime > -1)
			{
				*(plDestAnimType)	= AGCMCHAR_WAIT_ANIM_TYPE_ATTACK;
			}
		}
		break;
	}	
}

/*INT32 AgcmCharacter::GenerateAnimIndex(	AgcdCharacter			*pstAgcdCharacter,
										AgcmCharacterAnimType	eChangeType,
										AgcdAnimation2			*pstAgcdAnimation,
										INT32					lCustAnimType		)*/
BOOL AgcmCharacter::SetNextAnimData(	AgcdCharacter *pstAgcdCharacter,
										AgcmCharacterAnimType eChangeType,
										AgcdCharacterAnimation *pstAgcdAnimation,
										INT32 lCustAnimType					)
{
	CheckCustAnimType(&lCustAnimType, pstAgcdCharacter, eChangeType);

	AgcdAnimData2		*pcsCurrent			= NULL;
	ACA_AttachedData	*pcsAttachedData	= NULL;

	INT32			lCount			= 0;
	INT32			lSetIndex		= 0;
	INT32			lMaxIndex		= 0;	
	INT32			lOffset			= 0;	
	INT32			lRandomNumber	= m_csRandom.randInt(AGCMCHAR_RAND_PERCENTAGE);

	INT32			alActiveRate[AGCD_CHARACTER_MAX_ALLOC_ANIM_DATA];
	INT32			alAnimIndex[AGCD_CHARACTER_MAX_ALLOC_ANIM_DATA];

	memset(alActiveRate, 0, sizeof(INT32) * AGCD_CHARACTER_MAX_ALLOC_ANIM_DATA);
	memset(alAnimIndex, 0, sizeof(INT32) * AGCD_CHARACTER_MAX_ALLOC_ANIM_DATA);

/*	AgcdCharacterAnimationAttachedData	*pstCharAnimAttachedData = 
		(AgcdCharacterAnimationAttachedData *)(m_csAnimation.GetAttachedData(pstAgcdAnimation, AGCD_CHARACTER_ANIMATION_ATTACHED_DATA_KEY)	);
	if ((!pstAgcdAnimation->m_ppastAnimData) || (!pstCharAnimAttachedData))
		return 0;*/

/*	for (lCount = 0; lCount < AGCD_CHARACTER_MAX_ALLOC_ANIM_DATA; ++lCount)
	{
		if (!pstAgcdAnimation->m_ppastAnimData[lCount])
			break;

		if (pstCharAnimAttachedData->m_alCustAnimType[lCount] == lCustAnimType)
		{
			alActiveRate[lIndex]	= pstCharAnimAttachedData->m_alActiveRate[lCount];
			alAnimIndex[lIndex]		= lCount;
			++lIndex;
		}
	}*/

	if (!pstAgcdAnimation->m_pcsAnimation)
		return FALSE;

	pcsCurrent	= pstAgcdAnimation->m_pcsAnimation->m_pcsHead;
	while (pcsCurrent)
	{
		pcsAttachedData	=
			(ACA_AttachedData *)(m_csAnimation2.GetAttachedData(
				AGCD_CHARACTER_ANIMATION_ATTACHED_DATA_KEY,
				pcsCurrent										));

		if (!pcsAttachedData)
		{
			ASSERT(!"!pcsAttachedData");
//			return 0;
			return FALSE;
		}

		if (pcsAttachedData->m_nCustType == (INT8)(lCustAnimType))
		{
			alActiveRate[lMaxIndex]	= (INT32)(pcsAttachedData->m_unActiveRate);
			alAnimIndex[lMaxIndex]		= lCount;
			++lMaxIndex;
		}

		++lCount;
		pcsCurrent	= pcsCurrent->m_pcsNext;
	}

	if (lMaxIndex > 0)
		lSetIndex	= alAnimIndex[0];

	for (lCount = 0; lCount < lMaxIndex; ++lCount)
	{
		if ((lOffset < lRandomNumber) && (lRandomNumber <= (lOffset += alActiveRate[lCount])))
		{
			//lSetIndex	= lCount;
			lSetIndex	= alAnimIndex[lCount];
			break;
		}
	}

/*	if(lCount == lIndex)
		return alAnimIndex[0];

	return alAnimIndex[lCount];*/

	lCount		= 0;
	pcsCurrent	= pstAgcdAnimation->m_pcsAnimation->m_pcsHead;
	while (pcsCurrent)
	{
		if (lCount == lSetIndex)
		{
			pstAgcdCharacter->m_pcsNextAnimData	= pcsCurrent;
			return TRUE;
		}

		pcsCurrent	= pcsCurrent->m_pcsNext;

		++lCount;
	}

	return FALSE;
}

BOOL AgcmCharacter::OnPreReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	return TRUE;
}

BOOL AgcmCharacter::SetSelfCharacter(AgpdCharacter *pcsCharacter, BOOL bSetMainFrame)
{
	TRACE("AgcmCharacter::SetSelfCharacter() %x\n", pcsCharacter);

	if (pcsCharacter)
	{
		AgcdCharacter		*pstAgcdCharacter = GetCharacterData(pcsCharacter);
		RwFrame *	pstCameraFrame = NULL		;

		pcsCharacter->m_ulCheckBlocking	= ApTileInfo::BLOCKGEOMETRY;

		if ( m_pCamera ) pstCameraFrame	= RwCameraGetFrame(m_pCamera);
		
		RwV3d		stXAxis			= {1.0f, 0.0f, 0.0f};
		RwV3d		stCameraPos		= {0.0f, 400.0f, -800.0f};

		m_pcsSelfCharacter = pcsCharacter;
		m_lSelfCID = pcsCharacter->m_lID;

		//@{ Jaewon 20040705
		// register a shadow caster.
		AgcmShadowmap* pAgcmShadowmap = (AgcmShadowmap*)GetModule("AgcmShadowmap");
		if(pAgcmShadowmap)
			pAgcmShadowmap->setPlayerClump(pstAgcdCharacter->m_pClump);
		//@} Jaewon

		if (m_pcsAgcmShadow)
			AcuObject::SetClumpDInfo_Shadow(pstAgcdCharacter->m_pClump,1); // shadow level high�� ����n

		// DP�� �ڽ��� CID�� �˷��ش�. ��Ŷ �պκп� ���� �Ƶڸ� ������ �ִ����̴�.
		SetSelfCID(m_lSelfCID);

		// �ϴ� �������� �ƹ�Ÿ �������� ó���� �ɸ��� �߰��ɶ� �ٲ۴�.
		// MyCharacterPositionChange�� �ȹٲٴ� ������ �ȿ��� �ε� �Ƴ� �ȵƳ� üũ�ϴϱ�...
		if (m_pcsAgcmMap)
			m_pcsAgcmMap->m_posCurrentAvatarPosition = m_pcsSelfCharacter->m_stPos;

//		if (m_pPlayerFrame)
//			RwFrameDestroy(m_pPlayerFrame);

//		m_pPlayerFrame = RwFrameCreate();
//		if (!m_pPlayerFrame)
//			return FALSE;

		/*����*/
		//if (!RwFrameAddChild(m_pPlayerFrame, pstCameraFrame))
		//	return FALSE;

//		if (!RwFrameAddChild(m_pPlayerFrame, RpClumpGetFrame(pstAgcdCharacter->m_pClump)))
//			return FALSE;

		if (!pstAgcdCharacter->m_pClump)
			return TRUE;

		m_pMainCharacterClump = pstAgcdCharacter->m_pClump;

		m_pPlayerFrame = RpClumpGetFrame(pstAgcdCharacter->m_pClump);
		if ( pstCameraFrame != NULL )
		{
			LockFrame();

			RwMatrix*		pstCameraModelling = RwFrameGetMatrix(pstCameraFrame);
            
			RwMatrixRotate(pstCameraModelling, &stXAxis, m_fCameraPanInitial, rwCOMBINEREPLACE);
			RwMatrixTranslate(pstCameraModelling, &stCameraPos, rwCOMBINEPOSTCONCAT);
			RwFrameUpdateObjects(pstCameraFrame);

			UnlockFrame();
		}

		// 012303 Bob�� �۾�(Add, SetCallbackSetSelfCharacter()�� ���� �۾�~ ���d�d~)
		EnumCallback(AGCMCHAR_CB_ID_SET_SELF_CHARACTER, (PVOID)(pcsCharacter), (PVOID)(pstAgcdCharacter));

		// 070203 gemani �۾�(Render���� LOD �Ÿ��� ����ϱ� ����)
		// 110504(BOB, �������� ĳ���� �������� ���� �ʴ´�.)
		if ((bSetMainFrame) && (m_pcsAgcmRender))
			m_pcsAgcmRender->SetMainFrame(m_pPlayerFrame);
				
		// Character Status ���� �Է� 
		// SetCharacterStatusInfo();

		// Personal Info ���� �Է� 
		// SetPersonalInfo();

		if (m_bIsDeletePrevSelfCharacterWhenNewSelfCharacterisSet && m_pcsPrevSelfCharacter)
		{
			//m_pcsAgpmCharacter->RemoveCharacter(m_pcsPrevSelfCharacter, FALSE, FALSE);
		}

		m_bIsDeletePrevSelfCharacterWhenNewSelfCharacterisSet = FALSE;
		m_pcsPrevSelfCharacter	= NULL;


		{
			// Lua ��ũ�ο� ����..
			AuLua * pLua = AuLua::GetPointer();
			char	str[ 256 ];

			sprintf( str , "nSelf = %d" , pcsCharacter->m_lID );
			pLua->RunString( str );
		}
	}

	return TRUE;
}

BOOL AgcmCharacter::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pvPacket)
		return FALSE;

	//INT32		lServerID = -1;
	INT32		lCID = 0;
	INT32		lTID = 0;
	CHAR		*szAccountName = NULL;
	CHAR		*szCharName = NULL;
	AgpdCharacter	*pcsCharacter = NULL;
	INT8		cOperation = -1;
	AuPOS		stPosition;
	ZeroMemory(&stPosition, sizeof(AuPOS));
	INT32		lAuthKey	= 0;

	m_csPacket.GetField(TRUE, pvPacket, nSize, 
						&cOperation, 
						&szAccountName,
						&szCharName,
						&lTID,
						&lCID,
						&stPosition,
						&lAuthKey);

	switch (cOperation)
	{

#ifndef	__ATTACH_LOGINSERVER__

	case AGCMCHAR_PACKET_OPERATION_LOGIN_RESULT:
		{
			if (!szAccountName || !szAccountName[0])
			{
				MessageBox(NULL, "Login Failed !!!", "AgcmCharacter", MB_OK);
				return FALSE; 
			}
			
			//m_lSelfCID = lCID;
			//SetSelfCID(m_lSelfCID);
			/*
			char szConfirmMessage[64];
			sprintf( szConfirmMessage, "Login Success - CID : %d", lCID );
			MessageBox( NULL, szConfirmMessage, "AgcmCharacter", MB_OK );
			*/

			SendCreateChar(szAccountName, m_ulSelectedTID, szAccountName);	// �̰� ������ Ŭ���̾�Ʈ�� lTID, szName ���� �����ؼ� ������ �ϴµ� ������ UI�� ��ĥ �ð��� ��� �� Login�Ҷ� ���� TID, szName�� �޾Ƽ� ó���Ѵ�.
		}
		break;

	case AGCMCHAR_PACKET_OPERATION_CREATECHAR_RESULT:
		{
			if (!lCID) return FALSE;

			INT32					lIndex;
			AgpdCharacter			*pcsCharacter;

			lIndex = 0;
			if (m_pcsAgpmCharacter)
			{
				for (pcsCharacter = m_pcsAgpmCharacter->GetCharSequence(&lIndex); pcsCharacter; pcsCharacter = m_pcsAgpmCharacter->GetCharSequence(&lIndex))
				{
					if (pcsCharacter->m_lID != lCID)
					{
						m_pcsAgpmCharacter->RemoveCharacter(pcsCharacter);
					}
				}
			}

			SendSelectChar(szAccountName, lCID);
		}
		break;

	case AGCMCHAR_PACKET_OPERATION_DELETECHAR_RESULT:
		{
		}
		break;

	case AGCMCHAR_PACKET_OPERATION_SELECTCHAR_RESULT:
		{
			// ���⼭ ����� �����̸� ���� ����Ÿ�� �ε��ؼ� ���ӿ� ����.
			//
			m_lSelfCID = lCID;
			SetSelfCID(m_lSelfCID);

			LockRender();

			SetSelfCharacter(m_pcsAgpmCharacter ? m_pcsAgpmCharacter->GetCharacter(lCID) : NULL);

			UnlockRender();
		}
		break;

#else
		case AGCMCHAR_PACKET_OPERATION_SETTING_CHARACTER_OK:
			{
				m_lSelfCID = lCID;
				SetSelfCID(m_lSelfCID);
				
				AgpdCharacter *pcsAgpdCharacter = m_pcsAgpmCharacter->GetCharacter(lCID);
				if(!pcsAgpdCharacter)
					return FALSE;

				SetSelfCharacter(pcsAgpdCharacter);

				m_pcsAgpmCharacter->UpdatePosition(pcsAgpdCharacter, NULL);

				AgcdCharacter *pstAgcdCharacter = GetCharacterData(pcsAgpdCharacter);
				if(!pstAgcdCharacter)
					return FALSE;

				pstAgcdCharacter->m_ulNID = ulNID;
				EnumCallback(AGCMCHAR_CB_ID_SETTING_CHARACTER_OK, (PVOID)pcsAgpdCharacter->m_szID, NULL);
			}
			break;

		case AGCMCHAR_PACKET_OPERATION_CHARACTER_POSITION:
			{
				SetSelfCID(lCID);

				// stPosition�� �߽����� ���� �ε��Ѵ�.
				m_pcsAgcmMap->LoadTargetPosition(stPosition.x, stPosition.z);

				m_pcsAgpmCharacter->RemoveAllCharacters();

				SendLoadingComplete(lCID);
			}
			break;

		case AGCMCHAR_PACKET_OPERATION_AUTH_KEY:
			{
				m_lReceivedAuthKey	= lAuthKey;
			}
			break;

#endif	//__ATTACH_LOGINSERVER__

	default:
		{
			return FALSE;
		}
		break;
	}

	return TRUE;
}

INT16 AgcmCharacter::GetSelfNID()
{
	AgcdCharacter *pstAgcdCharacter = GetCharacterData(m_pcsSelfCharacter);
	if(!pstAgcdCharacter)
		return 0;

	return pstAgcdCharacter->m_ulNID;
}

BOOL AgcmCharacter::SendEnterGameWorld(CHAR *szCharName, CHAR *szServerAddress, INT32 lCID)
{
	if (!m_pcsAgcmConnectManager || !szCharName || !strlen(szCharName))
		return FALSE;

	ZeroMemory(m_szGameServerAddress, sizeof(m_szGameServerAddress));
	strcpy(m_szGameServerAddress, szServerAddress);

#ifdef	__ATTACH_LOGINSERVER__

	INT16	nNID = m_pcsAgcmConnectManager->Connect(szServerAddress, ACDP_SERVER_TYPE_GAMESERVER, this, 
													CBSocketOnConnect, CBSocketOnDisConnect, CBSocketOnError);

	if (nNID < 0) return FALSE;

	m_pcsAgcmConnectManager->SetSelfCID(lCID);

	AgcEngine *pcsModuleManager = (AgcEngine *) GetModuleManager();
	pcsModuleManager->SetGameServerID(nNID);

	strncpy(m_szCharName, szCharName, AGPACHARACTER_MAX_ID_STRING);

#endif	//__ATTACH_LOGINSERVER__

	return TRUE;
}

BOOL AgcmCharacter::SendLoadingComplete(INT32 lCID)
{
	if (lCID == AP_INVALID_CID)
		return FALSE;

	INT8	cOperation = AGCMCHAR_PACKET_OPERATION_LOADING_COMPLETE;
	INT16	nSize;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, &nSize, AGCMCHAR_PACKET_TYPE, 
								&cOperation,
								NULL,
								NULL,
								NULL,
								&lCID,
								NULL,
								NULL);

	if (!pvPacket || nSize < 1)
		return FALSE;

	BOOL	bSendResult	= SendPacket(pvPacket, nSize);

	m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgcmCharacter::SendCancelTransform(INT32 lCID)
{
	if (lCID == AP_INVALID_CID ||
		(m_pcsSelfCharacter && m_pcsSelfCharacter->m_lID != lCID))
		return FALSE;

	INT8	cOperation = AGPMCHAR_PACKET_OPERATION_CANCEL_TRANSFORM;
	INT16	nPacketLength = 0;

	PVOID	pvPacket = m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMCHARACTER_PACKET_TYPE,
																&cOperation,					// Operation
																&lCID,							// Character ID
																NULL,							// Character Template ID
																NULL,							// Game ID
																NULL,							// Character Status
																NULL,							// Move Packet
																NULL,							// Action Packet
																NULL,							// Factor Packet
																NULL,							// llMoney
																NULL,							// bank money
																NULL,							// cash
																NULL,							// character action status
																NULL,							// character criminal status
																NULL,							// attacker id (������� ������ �ʿ�)
																NULL,							// ���� �����Ǽ� �ʿ� �������� ����
																NULL,							// region index
																NULL,							// social action index
																NULL,							// special status
																NULL,							// is transform status
																NULL,							// skill initialization text
																NULL,							// face index
																NULL,							// hair index
																NULL,							// Option Flag
																NULL,							// bank size
																NULL,							// event status flag
																NULL,							// remained criminal status time
																NULL,							// remained murderer point time
																NULL,							// nick name
																NULL,							// gameguard
																NULL							// last killed time in battlesquare
																);

	if (!pvPacket ||
		nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	BOOL	bSendResult = SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgcmCharacter::SendRequestCouponInfo(INT32 lCID)
{
	PACKET_CHARACTER_COUPON_REQUEST stPacket;

	stPacket.m_lCID = lCID;

	return g_pEngine->SendPacket(stPacket);
}

BOOL AgcmCharacter::SendRequestCouponUse(INT32 lCID, stCouponInfo & couponInfo )
{
	if( !couponInfo.m_szCouponNo[0] )
		return FALSE;

	PACKET_CHARACTER_REQUEST_COUPON_USE stPacket;

	stPacket.m_lCID = lCID;
	stPacket.m_stRequestCoupon = couponInfo;

	return g_pEngine->SendPacket(stPacket);
}

BOOL
AgcmCharacter::SendCancelAction(AgpdCharacter * pcsCharacter)
{
	INT8	cOperation = AGCMCHAR_PACKET_OPERATION_LOADING_COMPLETE;
	INT16	nSize;
	PVOID	pvPacket;

	pvPacket = m_pcsAgpmOptimizedPacket2->MakePacketCancelAction(
		pcsCharacter, &nSize);
	if (!pvPacket)
		return FALSE;
	if (!SendPacket(pvPacket, nSize, 0, PACKET_PRIORITY_NONE, GetSelfNID())) {
		m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}
	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);
	return TRUE;
}

BOOL AgcmCharacter::SendCancelEvolution(INT32 lCID)
{
	if (lCID == AP_INVALID_CID || ( m_pcsSelfCharacter && m_pcsSelfCharacter->m_lID != lCID ) )	return FALSE;

	// ���� �������� ��ȭ��ų�� ��������ϴ� ���·� ����ȴ�.
	AgpdSkillTemplate* pSkillTemplate = _GetCurrentEvolutionSkillTemplate();
	if( !pSkillTemplate ) return FALSE;

	AgpdSkill* ppdSkill = m_pcsAgpmSkill->GetSkillByTID( m_pcsSelfCharacter, pSkillTemplate->m_lID );
	if( !pSkillTemplate || !ppdSkill ) return FALSE;

	INT16	nPacketLength = 0;
	PVOID	pvPacket = m_pcsAgpmSkill->MakeCastPacket( AGPMSKILL_ACTION_CANCEL_CAST_SKILL, &ppdSkill->m_lID, &pSkillTemplate->m_lID, ( ApBase* )m_pcsSelfCharacter,
													  0, ( ApBase* )m_pcsSelfCharacter, &m_pcsSelfCharacter->m_stPos, FALSE, 0, 0, 0, NULL, 0, &nPacketLength );

	if( !pvPacket || nPacketLength < 1 )	return FALSE;

	// pvPacket�� ������ ������.
	if( !SendPacket( pvPacket, nPacketLength ) )
	{
		m_pcsAgpmSkill->m_csPacket.FreePacket( pvPacket );
		return FALSE;
	}

	// 2007.02.13. steeple
	AgcmSkill* pcmSkill = ( AgcmSkill* )GetModule( "AgcmSkill" );
	AgcdSkill* pcdSkill = pcmSkill->GetAgcdSkill( ppdSkill );

	if( pcdSkill )
	{
		pcdSkill->m_alAffectedTargetCID.MemSetAll();
	}

	m_pcsAgpmSkill->m_csPacket.FreePacket( pvPacket );
	return TRUE;
}

BOOL AgcmCharacter::SendMoveCharacter(AuPOS *posDest, MOVE_DIRECTION eMoveDirection, BOOL bPathFinding, BOOL bMoveFast)
{
	PVOID		pvPacket;
	PVOID		pvPacketMove;
	INT16		nSize;
	INT8		cOperation = AGPMCHAR_PACKET_OPERATION_UPDATE;
	INT8		cMoveFlag;
	INT8		cMoveDirection = 0;

	if (m_pcsAgpmOptimizedPacket2)
	{
		cMoveFlag = (eMoveDirection != MD_NODIRECTION ? AGPMCHARACTER_MOVE_FLAG_DIRECTION : 0) |
					(bPathFinding ? AGPMCHARACTER_MOVE_FLAG_PATHFINDING : 0) |
					(bMoveFast ? AGPMCHARACTER_MOVE_FLAG_FAST : 0);

		cMoveDirection = (INT8)eMoveDirection;

		pvPacket	= m_pcsAgpmOptimizedPacket2->MakePacketCharMove(m_lSelfCID, cMoveFlag, cMoveDirection, posDest, &nSize);
	}
	else
	{
		if (!m_pcsAgpmCharacter)
			return FALSE;

		cMoveFlag = (eMoveDirection != MD_NODIRECTION ? AGPMCHARACTER_MOVE_FLAG_DIRECTION : 0) |
					(bPathFinding ? AGPMCHARACTER_MOVE_FLAG_PATHFINDING : 0) |
					(bMoveFast ? AGPMCHARACTER_MOVE_FLAG_FAST : 0);

		cMoveDirection = (INT8)eMoveDirection;

		pvPacketMove = m_pcsAgpmCharacter->m_csPacketMove.MakePacket(FALSE, &nSize, AGPMCHARACTER_PACKET_TYPE, 
															NULL,
															posDest,
															NULL,
															NULL,
															NULL,
															NULL,
															&cMoveFlag,
															&cMoveDirection);

		if (!pvPacketMove) return FALSE;

		pvPacket = m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, &nSize, AGPMCHARACTER_PACKET_TYPE, 
															&cOperation,						// Operation
															&m_lSelfCID,						// Character ID
															NULL,								// Character Template ID
															NULL,								// Game ID
															NULL,								// Character Status
															pvPacketMove,						// Move Packet
															NULL,								// Action Packet
															NULL,								// Factor Packet
															NULL,								// llMoney
															NULL,								// bank money
															NULL,								// cash
															NULL,								// character action status
															NULL,								// character criminal status
															NULL,								// attacker id (������� ������ �ʿ�)
															NULL,								// ���� �����Ǽ� �ʿ� �������� ����
															NULL,								// region index
															NULL,								// social action index
															NULL,								// special status
															NULL,								// is transform status
															NULL,								// skill initialization text
															NULL,								// face index
															NULL,								// hair index
															NULL,								// Option Flag
															NULL,								// bank size
															NULL,								// event status flag
															NULL,								// remained criminal status time
															NULL,								// remained murderer point time
															NULL,								// nick name
															NULL,								// gameguard
															NULL								// last killed time in battlesquare
															);

		m_pcsAgpmCharacter->m_csPacketMove.FreePacket(pvPacketMove);
	}

	if (!pvPacket) return FALSE;
	if (!SendPacket(pvPacket, nSize, 0, PACKET_PRIORITY_NONE, GetSelfNID()))
	{
		m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

	m_ulPrevSendMoveTime = m_ulCurTick;

	return TRUE;
}

BOOL AgcmCharacter::SendMoveFollowCharacter(INT32 lTargetCID, INT32 lFollowDistance)
{
	INT16		nSize = 0;
	INT8		cOperation = AGPMCHAR_PACKET_OPERATION_UPDATE;

	if (!m_pcsAgpmCharacter)
		return FALSE;

	INT8	cMoveFlag = AGPMCHARACTER_MOVE_FLAG_FOLLOW;
	INT8	cMoveDirection = 0;

	PVOID	pvPacketMove = m_pcsAgpmCharacter->m_csPacketMove.MakePacket(FALSE, &nSize, AGPMCHARACTER_PACKET_TYPE, 
														NULL,
														NULL,
														&lTargetCID,
														&lFollowDistance,
														NULL,
														NULL,
														&cMoveFlag,
														&cMoveDirection);

	if (!pvPacketMove) return FALSE;

	PVOID	pvPacket = m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, &nSize, AGPMCHARACTER_PACKET_TYPE, 
														&cOperation,							// Operation
														&m_lSelfCID,							// Character ID
														NULL,									// Character Template ID
														NULL,									// Game ID
														NULL,									// Character Status
														pvPacketMove,							// Move Packet
														NULL,									// Action Packet
														NULL,									// Factor Packet
														NULL,									// llMoney
														NULL,									// bank money
														NULL,									// cash
														NULL,									// character action status
														NULL,									// character criminal status
														NULL,									// attacker id (������� ������ �ʿ�)
														NULL,									// ���� �����Ǽ� �ʿ� �������� ����
														NULL,									// region index
														NULL,									// social action index
														NULL,									// special status
														NULL,									// is transform status
														NULL,									// skill initialization text
														NULL,									// face index
														NULL,									// hair index
														NULL,									// Option Flag
														NULL,									// bank size
														NULL,									// event status flag
														NULL,									// remained criminal status time
														NULL,									// remained murderer point time
														NULL,									// nick name
														NULL,									// gameguard
														NULL									// last killed time in battlesquare
														);

	m_pcsAgpmCharacter->m_csPacketMove.FreePacket(pvPacketMove);
	if (!pvPacket) return FALSE;

	if (!SendPacket(pvPacket, nSize, 0, PACKET_PRIORITY_NONE, GetSelfNID()))
	{
		m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgcmCharacter::SendStopCharacter()
{
	if (!m_pcsSelfCharacter || !m_pcsAgpmCharacter)
		return FALSE;

	m_bReservedMove	= FALSE;

	PVOID		pvPacket		= NULL;
	PVOID		pvPacketMove	= NULL;
	INT16		nSize			= 0;
	INT8		cOperation		= AGPMCHAR_PACKET_OPERATION_UPDATE;
	INT8		cMoveFlag		= AGPMCHARACTER_MOVE_FLAG_STOP;
	INT8		cMoveDirection	= MD_NODIRECTION;
	
	pvPacketMove = m_pcsAgpmCharacter->m_csPacketMove.MakePacket(FALSE, &nSize, AGPMCHARACTER_PACKET_TYPE, 
														&m_pcsSelfCharacter->m_stPos,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL,
														&cMoveFlag,
														&cMoveDirection);

	if (!pvPacketMove) return FALSE;

	pvPacket = m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, &nSize, AGPMCHARACTER_PACKET_TYPE, 
														&cOperation,						// Operation
														&m_lSelfCID,						// Character ID
														NULL,								// Character Template ID
														NULL,								// Game ID
														NULL,								// Character Status
														pvPacketMove,						// Move Packet
														NULL,								// Action Packet
														NULL,								// Factor Packet
														NULL,								// llMoney
														NULL,								// bank money
														NULL,								// cash
														NULL,								// character action status
														NULL,								// character criminal status
														NULL,								// attacker id (������� ������ �ʿ�)
														NULL,								// ���� �����Ǽ� �ʿ� �������� ����
														NULL,								// region index
														NULL,								// social action index
														NULL,								// special status
														NULL,								// is transform status
														NULL,								// skill initialization text
														NULL,								// face index
														NULL,								// hair index
														NULL,								// Option Flag
														NULL,								// bank size
														NULL,								// event status flag
														NULL,								// remained criminal status time
														NULL,								// remained murderer point time
														NULL,								// nick name
														NULL,								// gameguard
														NULL								// last killed time in battlesquare
														);

	m_pcsAgpmCharacter->m_csPacketMove.FreePacket(pvPacketMove);
	if (!pvPacket) return FALSE;

	if (!SendPacket(pvPacket, nSize, 0, PACKET_PRIORITY_NONE, GetSelfNID()))
	{
		m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgcmCharacter::SendActionInfo(AgpdCharacterActionType nType, INT32 lTargetCID, INT32 lSkillTID, UINT8 cComboInfo, BOOL bForceAttack, UINT8 cHitIndex)
{
	PVOID		pvPacket;
	INT16		nSize;

	if (m_bShowActionQueue && nType == AGPDCHAR_ACTION_TYPE_ATTACK)
	{
		CHAR	szTemp[16];

		sprintf(szTemp, "(%d);", nType);
		m_szAttackSentAcc	+= szTemp;
	}

	if (m_pcsAgpmOptimizedPacket2)
	{		
		pvPacket	= m_pcsAgpmOptimizedPacket2->MakePacketCharAction(m_lSelfCID, lTargetCID, cComboInfo, bForceAttack, 0, cHitIndex, &nSize);
	}
	else
	{
		if (!m_pcsAgpmCharacter)
			return FALSE;

		INT8		cOperation = AGPMCHAR_PACKET_OPERATION_UPDATE;
		//StartAnimation(m_pcsSelfCharacter, AGCMCHAR_ANIM_TYPE_NORMALATTACK1);

		PVOID pvPacketAction = m_pcsAgpmCharacter->m_csPacketAction.MakePacket(FALSE, &nSize, AGPMCHARACTER_PACKET_TYPE,  
																&nType, 
																&lTargetCID, 
																&lSkillTID,
																NULL,
																NULL,
																NULL,
																(cComboInfo > 0) ? &cComboInfo : NULL,
																(bForceAttack) ? &bForceAttack : NULL,
																NULL,
																(cHitIndex != 0) ? &cHitIndex : NULL);

		if (!pvPacketAction) return FALSE;

		pvPacket = m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, &nSize, AGPMCHARACTER_PACKET_TYPE,
															&cOperation,				// Operation
															&m_lSelfCID,				// Character ID
														 	NULL,						// Character Template ID
															NULL,						// Game ID
															NULL,						// Character Status
															NULL,						// Move Packet
															pvPacketAction,				// Action Packet
															NULL,						// Factor Packet
															NULL,						// llMoney
															NULL,						// bank money
															NULL,						// cash
															NULL,						// character action status
															NULL,						// character criminal status
															NULL,						// attacker id (������� ������ �ʿ�)
															NULL,						// ���� �����Ǽ� �ʿ� �������� ����
															NULL,						// region index
															NULL,						// social action index
															NULL,						// special status
															NULL,						// is transform status
															NULL,						// skill initialization text
															NULL,						// face index
															NULL,						// hair index
															NULL,						// Option Flag
															NULL,						// bank size
															NULL,						// event status flag
															NULL,						// remained criminal status time
															NULL,						// remained murderer point time
															NULL,						// nick name
															NULL,						// gameguard
															NULL						// last killed time in battlesquare
															);

		m_pcsAgpmCharacter->m_csPacketAction.FreePacket(pvPacketAction);
	}

	if (!pvPacket) return FALSE;
	if (!SendPacket(pvPacket, nSize, 0, PACKET_PRIORITY_NONE, GetSelfNID()))
	{
		m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgcmCharacter::SendSocialAnimation	( UINT8 ucSocialAnimation	)
{
	// kermi 2005.7.29 - ���� ź �����̰ų� ������ ���¿����� �������� �ʴ´�.
	AgpdCharacter *pCharacter = GetSelfCharacter();
	if (NULL == pCharacter)
		return FALSE;

	if (pCharacter->m_bRidable)
		return FALSE;

	if (FALSE == m_pcsAgpmArchlord->IsArchlord(pCharacter->m_szID))
	{
		// ��ũ�ε尡 �ƴϰ� ���� ���� �϶���
		if (pCharacter->m_bIsTrasform )
			return FALSE;
	}

	if (pCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
		return FALSE;

	INT8 cOperation		= AGPMCHAR_PACKET_OPERATION_SOCIAL_ANIMATION;
	INT16 nPacketLength	= 0;
	
	PVOID pvPacket		= m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMCHARACTER_PACKET_TYPE, 
															&cOperation,							// Operation
															&m_pcsSelfCharacter->m_lID,				// Character ID
															NULL,									// Character Template ID
															NULL,									// Game ID
															NULL,									// Character Status
															NULL,									// Move Packet
															NULL,									// Action Packet
															NULL,									// Factor Packet
															NULL,									// llMoney
															NULL,									// bank money
															NULL,									// cash
															NULL,									// character action status
															NULL,									// character criminal status
															NULL,									// attacker id (������� ������ �ʿ�)
															NULL,									// ���� �����Ǽ� �ʿ� �������� ����
															NULL,									// region index
															&ucSocialAnimation,						// social action index
															NULL,									// special status
															NULL,									// is transform status
															NULL,									// skill initialization text
															NULL,									// face index
															NULL,									// hair index
															NULL,									// Option Flag
															NULL,									// bank size
															NULL,									// event status flag
															NULL,									// remained criminal status time
															NULL,									// remained murderer point time
															NULL,									// nick name
															NULL,									// gameguard
															NULL									// last killed time in battlesquare
															);

	if (!pvPacket)
		return FALSE;

	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

RpCollisionTriangle *AgcmCharacter::CollisionWorldFirstCallback(RpIntersection *pstIntersection,
							RpWorldSector *pstSector, 
							RpCollisionTriangle *pstCollTriangle,
							RwReal fDistance, 
							void *pvData)
{
	RwV3d	*pstPosition = (RwV3d *) pvData;
	
	pstPosition->x = pstIntersection->t.line.start.x + (pstIntersection->t.line.end.x - pstIntersection->t.line.start.x) * fDistance;
	pstPosition->y = pstIntersection->t.line.start.y + (pstIntersection->t.line.end.y - pstIntersection->t.line.start.y) * fDistance;
	pstPosition->z = pstIntersection->t.line.start.z + (pstIntersection->t.line.end.z - pstIntersection->t.line.start.z) * fDistance;

	return NULL;
}

// �Ⱦ��� �Լ���

/*****************************************************************
*   Function : ReceiveAddCharacter
*   Comment  : Add Character�� ���� ���� ���� 
*   Date&Time : 2002-04-30, ���� 4:17
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
/*
BOOL	AgcmCharacter::ReceiveAddCharacter(void* pAgpdCharacter)
{
	if ( m_lCurCharacterNum >= CLIENT_CHARACTER_MAX_NUM )
			return FALSE;

	AgpdCharacter*			pCharacterPublicData;	//���ڷ� �Ѿ�� pAgpdCharacter
	AgcdCharacter			pCharacterClientData;	//Client Character Data 
	AgpmCharacter*			pParentModule;			//Parent Module
	stAPADMINOBJECTCXT		stCTemplate;				
	stAPADMINOBJECTCXT		stCCharacter;
	AgcdCharacterTemplate*	pCharacterTemplateData;
	
	pCharacterPublicData = (AgpdCharacter*)pAgpdCharacter;

	m_lCurCID[ m_lCurCharacterNum ] = pCharacterPublicData->m_stCharacter.m_lID ;

	if ( !GetCCharacterTemplate( pCharacterPublicData->m_stCharacter.m_lTID1, &stCTemplate ) )
			return FALSE;

	stCTemplate.pMutex->RLock();
	pCharacterTemplateData = (AgcdCharacterTemplate*)stCTemplate.pObject;
	stCTemplate.pMutex->Release();

	// pCharacterClientData �ۼ� 
	pCharacterClientData.nTemplate = pCharacterPublicData->m_stCharacter.m_lTID1;
	pCharacterClientData.pClump = RpClumpClone(pCharacterTemplateData->pClump);
		
	SetCharacterClientData( &pCharacterClientData, pCharacterPublicData->m_stCharacter.m_lID );
	
	pParentModule = (AgpmCharacter*)GetParentModule(0);
	pParentModule->m_csACharacter.GetCharacter( pCharacterPublicData->m_stCharacter.m_lID, &stCCharacter );
	
	stCCharacter.pMutex->RLock();
	m_pCurCharacter[ m_lCurCharacterNum ] = (AgcdCharacter*)GetAttachedModuleData( 0, stCCharacter.pObject );
	stCCharacter.pMutex->Release();
	m_lCurCharacterNum++;

	//�ڷ� �Է� ��, World�� �߰��ϰ� Hierarchy, Anim �����ϴ� �κ� 
	stAPADMINOBJECTCXT	stCCharacter2;
	pParentModule->m_csACharacter.GetCharacter( 1, &stCCharacter2 );
	AgpdCharacter* pCharacterPublicData2 = (AgpdCharacter*)stCCharacter2.pObject;
	AgcdCharacter* pInputClientData = NULL;
	pInputClientData = (AgcdCharacter*)(	GetAttachedModuleData( 0, (PVOID)pCharacterPublicData2) );

	RpWorldAddClump( m_gWorld, pInputClientData->pClump );

	pInputClientData->pInHierarchy = UtilGetHierarchy( pInputClientData->pClump );
	RpClumpForAllAtomics( pInputClientData->pClump, SetHierarchyForSkinAtomic, (void *)pInputClientData->pInHierarchy );

	RpHAnimHierarchySetFlags(pInputClientData->pInHierarchy,
							 (RpHAnimHierarchyFlag)
							 ( RpHAnimHierarchyGetFlags(pInputClientData->pInHierarchy) | 
							   rpHANIMHIERARCHYUPDATELTMS |
							   rpHANIMHIERARCHYUPDATEMODELLINGMATRICES | rpHANIMHIERARCHYLOCALSPACEMATRICES) );

	RpHAnimHierarchySetCurrentAnim( pInputClientData->pInHierarchy, pCharacterTemplateData->pAnim[0] );
	RpHAnimHierarchyAttach(pInputClientData->pInHierarchy);
	
	return TRUE;
}


/*****************************************************************
*   Function : CharacterUpdateAnimation
*   Comment  : Character�� Animation�� ������Ʈ �Ѵ�. 
*   Date&Time : 2002-05-02, ���� 6:08
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
/*
void	AgcmCharacter::CharacterUpdateAnimation(RwReal deltaTime)
{
	//���� Add�Ǿ� �ִ� Character�� Animation�� Update�Ѵ�. 

#ifdef	_TEST_

	if ( m_lCurCharacterNum == 0 ) return;

	AgcdCharacter*	pAgcdCharacter = NULL;

	AgpmCharacter* aaa = (AgpmCharacter*)GetParentModule(0);
	stAPADMINOBJECTCXT	bbb;
	aaa->m_csACharacter.GetCharacter( 1, &bbb );
	AgpdCharacter* ccc = (AgpdCharacter*)bbb.pObject;
	AgcdCharacter* ddd = NULL;
	ddd = (AgcdCharacter*)(	GetAttachedModuleData( 0, (PVOID)ccc) );
	
	RpHAnimHierarchyStdKeyFrameAddAnimTime(ddd->pInHierarchy , deltaTime);
	RpHAnimHierarchyUpdateMatrices(ddd->pInHierarchy);	
	
#endif
}

//Test�� ���� �Լ� 
BOOL	AgcmCharacter::Test()
{

#ifdef	_TEST_

	RwUInt32 thisTime;
    RwReal deltaTime;

    static RwBool firstCall = TRUE;
    static RwUInt32 lastFrameTime, lastAnimTime;

	if( firstCall )
    {
        lastFrameTime = lastAnimTime = timeGetTime();

        firstCall = FALSE;
    }

	thisTime = timeGetTime();

	if( thisTime > (lastFrameTime + 1000) )
    {
        lastFrameTime = thisTime;
    }

	deltaTime = (thisTime - lastAnimTime) * 0.001;
   
	CharacterUpdateAnimation(deltaTime);

	lastAnimTime = thisTime;

	return TRUE;

#endif
	return TRUE;
}

*/

/*BOOL AgcmCharacter::SetCurrentAnimTime(INT32 lCID, FLOAT fTime)
{
	AgpdCharacter *pstAgpdCharacter = m_pcsAgpmCharacter->GetCharacter(lCID);

	return SetCurrentAnimTime(pstAgpdCharacter, fTime);
}

BOOL AgcmCharacter::SetCurrentAnimTime(AgpdCharacter *pstAgpdCharacter, FLOAT fTime)
{
	AgcdCharacter *pstAgcdCharacter = GetCharacterData(pstAgpdCharacter);

	return SetCurrentAnimTime(pstAgcdCharacter, fTime);
}

BOOL AgcmCharacter::SetCurrentAnimTime(AgcdCharacter *pstAgcdCharacter, FLOAT fTime)
{
//	if(pstAgcdCharacter->m_bStop)
//		return FALSE;

//	if(pstAgcdCharacter->m_bBlending) // �ϴ� FASLE
//		return FALSE;

	RpHAnimHierarchySetCurrentAnimTime(pstAgcdCharacter->m_pInHierarchy, fTime);
	RpHAnimHierarchyUpdateMatrices(pstAgcdCharacter->m_pInHierarchy);

	return TRUE;
}*/

RpHAnimHierarchy *AgcmCharacter::CreateSubHierarchy(RpHAnimHierarchy *pParentHierarchy, RwInt32 lNodeID)
{
	return RpHAnimHierarchyCreateSubHierarchy(pParentHierarchy, RpHAnimIDGetIndex(pParentHierarchy, lNodeID), (RpHAnimHierarchyFlag)(pParentHierarchy->flags), -1);
}

AgcmCharacterSubAnimNodeIndex AgcmCharacter::FindNodeIndexFromSubAnimType(AgcmCharacterSubAnimType eSubAnimType)
{
	AgcmCharacterSubAnimNodeIndex eNodeIndex = AGCMCHAR_SUBANIM_NODEINDEX_ERROR;

	switch(eSubAnimType)
	{
	case AGCMCHAR_SUBANIM_TYPE_SPINE:
		{
			eNodeIndex = AGCMCHAR_SUBANIM_NODEINDEX_SPINE;
		}
		break;

	default:
		{ // skip
		}
		break;
	}

	return eNodeIndex;
}

BOOL AgcmCharacter::DumpTexDict()
{
	if (!m_pstTexDict)
		return FALSE;

	// Texture Dictionary�� ������, StreamWrite�Ѵ�. Platform Independent Data��
	if (m_pstTexDict && m_szTexDict[0])
	{
		RwStream *		stream;

		stream = RwStreamOpen( rwSTREAMFILENAME, rwSTREAMWRITE, m_szTexDict );
		if( stream )
		{
			RtPITexDictionaryStreamWrite( m_pstTexDict , stream );
   
			RwStreamClose(stream, NULL);
		}
		else
			return FALSE;
	}

	return TRUE;
}

BOOL AgcmCharacter::SetCallbackInitCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_INIT_CHARACTER, pfCallback, pClass);
}

/*BOOL AgcmCharacter::SetCallbackInitCharClump(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_INIT_CHAR_CLUMP, pfCallback, pClass);
}*/

BOOL AgcmCharacter::SetCallbackChangeAnim(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_CHANGE_ANIM, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackChangeAnimWait(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_CHANGE_ANIM_WAIT, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackChangeAnimWalk(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_CHANGE_ANIM_WALK, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackChangeAnimRun(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_CHANGE_ANIM_RUN, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackChangeAnimAttack(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_CHANGE_ANIM_ATTACK, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackChangeAnimStruck(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_CHANGE_ANIM_STRUCK, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackChangeAnimDead(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_CHANGE_ANIM_DEAD, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackChangeAnimSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_CHANGE_ANIM_SKILL, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackEndSocialAnim(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_END_SOCIAL_ANIM, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackChangeAnimSocial(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_CHANGE_ANIM_SOCIAL, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackAttackSuccess(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_ATTACK_SUCESS, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackStruckCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_STRUCK_CHARACTER, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackProcessSkillStruckCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_PROCESS_SKILL_STRUCK_CHARACTER, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackSkillStruckCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_SKILL_STRUCK_CHARACTER, pfCallback, pClass);
}

/*BOOL AgcmCharacter::SetCallbackChangeAction(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_CHANGE_ACTION, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackActionAttack(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_ACTION_ATTACK, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackActionStruck(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_ACTION_STRUCK, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackActionSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_ACTION_SKILL, pfCallback, pClass);
}*/

BOOL AgcmCharacter::SetCallbackSelfUpdatePosition(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_SELF_UPDATA_POSITION, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackSelfStopCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_SELF_STOP_CHARACTER, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackSetSelfCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_SET_SELF_CHARACTER, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackReleaseSelfCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_RELEASE_SELF_CHARACTER, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackRemoveCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_REMOVE_CHARACTER, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackPreRemoveCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_PRE_REMOVE_DATA, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackSettingCharacterOK(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_SETTING_CHARACTER_OK, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackLoadClump(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_LOAD_CLUMP, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackLoadDefaultArmourClump(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_LOAD_DEFAULT_ARMOUR_CLUMP, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackDisplayActionResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_DISPLAY_ACTION_RESULT, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackLockTarget(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_LOCK_TARGET, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackUnlockTarget(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_UNLOCK_TARGET, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackSelectTarget(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_SELECT_TARGET, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackUnSelectTarget(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_UNSELECT_TARGET, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackGameServerDisconnect(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_GAMESERVER_DISCONNECT, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackGameServerError(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_GAMESERVER_ERROR, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackActionResultNotEnoughArrow(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_ACTION_RESULT_NOT_ENOUGH_ARROW, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackActionResultNotEnoughBolt(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_ACTION_RESULT_NOT_ENOUGH_BOLT, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackActionResultNotEnoughMP(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_ACTION_RESULT_NOT_ENOUGH_MP, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackActionSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_ACTION_SKILL, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackUpdateCharHP(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_UPDATE_CHAR_HP, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackUpdateCharMP(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_UPDATE_CHAR_MP, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackUpdateMovement(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_UPDATE_MOVEMENT, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackKillCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_KILL_CHARACTER, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackLoadTemplate(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_LOAD_TEMPLATE, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackReleaseTemplate(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_RELEASE_TEMPLATE, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackSelfRegionChange(ApModuleDefaultCallBack	pfCallback, PVOID	pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_SELF_REGION_CHANGE, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackLoadingComplete(ApModuleDefaultCallBack	pfCallback, PVOID	pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_LOADING_COMPLETE, pfCallback, pClass);
}

/*
	2005.06.01. By SungHoon
	�ź� ����� ���� ��� �Ҹ����� �ݹ��Լ� ���
*/
BOOL AgcmCharacter::SetCallbackUpdateOptionFlag(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_UPDATE_OPTION_FLAG, pfCallback, pClass);
}

// 2005. 8. 17 Nonstopdj
// �������� ������ �߻��ϴ� Effect Event Callback�� ����
BOOL AgcmCharacter::SetCallbackWaveFxTrigger(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_WAVE_FX_TRIGGER, pfCallback, pClass);
}

// 2005.09.09. steeple
// ����!!!
BOOL AgcmCharacter::SetCallbackUpdateTransparent(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_UPDATE_TRANSPARENT, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackSocketOnConnect(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_SOCKET_ON_CONNECT, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackIsCameraMoving(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_IS_CAMERA_MOVING, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackTurnCamera(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHAR_CB_ID_TURN_CAMERA, pfCallback, pClass);
}

BOOL AgcmCharacter::SetCallbackDestroyBoard( ApModuleDefaultCallBack pfCallback, PVOID pClass )
{
	return SetCallback(AGCMCHAR_CB_ID_DESTROY_TEXTBOARD , pfCallback, pClass );
}

BOOL AgcmCharacter::CBRemoveChar( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmCharacter *	pThis = (AgcmCharacter *) pClass;
	AgpdCharacter * pcsCharacter	= (AgpdCharacter *) pCustData;
	if (!pcsCharacter)
		return FALSE;

	if (pcsCharacter->m_lID == pThis->m_lSelfCID)
	{
		if (pThis->m_pcsAgcmMap)
		{
			pThis->m_pcsAgcmMap->DeleteMyCharacterCheck();
		}
	}

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 042403 Bob Jung.
******************************************************************************/
CHAR *AgcmCharacter::GetAnimationPath()
{
	return m_szAnimationPath;
}

/*****************************************************************
*   Function : SetCharacterStatusInfo
*   Comment  : SetCharacterStatusInfo
*   Date&Time : 2003-05-05, ���� 3:50
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************
void	AgcmCharacter::SetCharacterStatusInfo()
{
	if ( NULL == m_pcsSelfCharacter || NULL == m_pcsAgpmFactors ) return;

	AgpdFactor	*pcsFactorResult = (AgpdFactor *) m_pcsAgpmFactors->GetFactor(&m_pcsSelfCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT);

	if ( NULL == pcsFactorResult )	return;
	
	INT32	lValue	=	0;

	// CHA
	m_pcsAgpmFactors->GetValue( pcsFactorResult, &lValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CHA );
	m_clCharacterStatus.m_clBigNumber[ACUICS_BIG_NUM_CHA].SetNumber( lValue );

	// STR
	m_pcsAgpmFactors->GetValue( pcsFactorResult, &lValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_STR );
	m_clCharacterStatus.m_clBigNumber[ACUICS_BIG_NUM_STR].SetNumber( lValue );

	// INT
	m_pcsAgpmFactors->GetValue( pcsFactorResult, &lValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_INT );
	m_clCharacterStatus.m_clBigNumber[ACUICS_BIS_NUM_INT].SetNumber( lValue );

	// CON
	m_pcsAgpmFactors->GetValue( pcsFactorResult, &lValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CON );
	m_clCharacterStatus.m_clBigNumber[ACUICS_BIG_NUM_CON].SetNumber( lValue );

	// WIS	
	m_pcsAgpmFactors->GetValue( pcsFactorResult, &lValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_WIS );
	m_clCharacterStatus.m_clBigNumber[ACUICS_BIG_NUM_WIZ].SetNumber( lValue );

	// DEX
	m_pcsAgpmFactors->GetValue( pcsFactorResult, &lValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_DEX );
	m_clCharacterStatus.m_clBigNumber[ACUICS_BIG_NUM_DEX].SetNumber( lValue );
	
	// PARTYA
	// PARTD

	// PHYSIC ATTACK
	m_pcsAgpmFactors->GetValue( pcsFactorResult, &lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL );
	m_clCharacterStatus.m_clSmallNumber[ACUICS_SMALL_NUM_PHYSICAMAX].SetNumber( lValue );

	m_pcsAgpmFactors->GetValue( pcsFactorResult, &lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL );
	m_clCharacterStatus.m_clSmallNumber[ACUICS_SMALL_NUM_PHYSICAMIN].SetNumber( lValue );

	// WATER ATTACK
	m_pcsAgpmFactors->GetValue( pcsFactorResult, &lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_WATER );
	m_clCharacterStatus.m_clSmallNumber[ACUICS_SMALL_NUM_WATERA].SetNumber( lValue );

	// FIRE ATTACK
	m_pcsAgpmFactors->GetValue( pcsFactorResult, &lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE );
	m_clCharacterStatus.m_clSmallNumber[ACUICS_SMALL_NUM_FIREA].SetNumber( lValue );

	// MAGIC ATTACK
	m_pcsAgpmFactors->GetValue( pcsFactorResult, &lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH );
	m_clCharacterStatus.m_clSmallNumber[ACUICS_SMALL_NUM_MAGICA].SetNumber( lValue );

	// AIR ATTACK
	m_pcsAgpmFactors->GetValue( pcsFactorResult, &lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_AIR );
	m_clCharacterStatus.m_clSmallNumber[ACUICS_SMALL_NUM_AIRA].SetNumber( lValue );

	// EARTH ATTACK
	m_pcsAgpmFactors->GetValue( pcsFactorResult, &lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH );
	m_clCharacterStatus.m_clSmallNumber[ACUICS_SMALL_NUM_EARTHA].SetNumber( lValue );	 

	// HP
	m_pcsAgpmFactors->GetValue( pcsFactorResult, &lValue, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP );
	m_clCharacterStatus.m_clSmallNumber[ACUICS_SMALL_NUM_HP].SetNumber( lValue );

	// MP
	m_pcsAgpmFactors->GetValue( pcsFactorResult, &lValue, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP );
	m_clCharacterStatus.m_clSmallNumber[ACUICS_SMALL_NUM_MP].SetNumber( lValue );

	// SP
	m_pcsAgpmFactors->GetValue( pcsFactorResult, &lValue, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP );
	m_clCharacterStatus.m_clSmallNumber[ACUICS_SMALL_NUM_SP].SetNumber( lValue );

	// PHYSIC DEFENDENCY
	m_pcsAgpmFactors->GetValue( pcsFactorResult, &lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT,
		AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL );
	m_clCharacterStatus.m_clSmallNumber[ACUICS_SMALL_NUM_PHYSICD].SetNumber( lValue );
	
	// RESIST WATER
	m_pcsAgpmFactors->GetValue( pcsFactorResult, &lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT,
		AGPD_FACTORS_ATTRIBUTE_TYPE_WATER );
	m_clCharacterStatus.m_clExpand.m_clNumber[ACUICS_EXPAND_NUMBER_WATER_RESIST].SetNumber( lValue );

	// RESIST AIR
	m_pcsAgpmFactors->GetValue( pcsFactorResult, &lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, 
		AGPD_FACTORS_ATTRIBUTE_TYPE_AIR );
	m_clCharacterStatus.m_clExpand.m_clNumber[ACUICS_EXPAND_NUMBER_AIR_RESIST].SetNumber( lValue );

	// RESIST FIRE
	m_pcsAgpmFactors->GetValue( pcsFactorResult, &lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, 
		AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE );
	m_clCharacterStatus.m_clExpand.m_clNumber[ACUICS_EXPAND_NUMBER_FIRE_RESIST].SetNumber( lValue );

	// RESIST EARTH
	m_pcsAgpmFactors->GetValue( pcsFactorResult, &lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, 
		AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH );
	m_clCharacterStatus.m_clExpand.m_clNumber[ACUICS_EXPAND_NUMBER_EARTH_RESIST].SetNumber( lValue );

	// RESIST MAGIC
	m_pcsAgpmFactors->GetValue( pcsFactorResult, &lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, 
		AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC );
	m_clCharacterStatus.m_clExpand.m_clNumber[ACUICS_EXPAND_NUMBER_MAGIC_RESIST].SetNumber( lValue );	
	
	return;

}

/*****************************************************************
*   Function : SetPersonalInfo
*   Comment  : SetPersonalInfo
*   Date&Time : 2003-05-09, ���� 5:03
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************
void	AgcmCharacter::SetPersonalInfo()
{
	m_clPersonalInfo.SetTitle1Rect( 0 );		// TEST ��
	m_clPersonalInfo.SetTitle2Rect( 0 );		// TEST ��

	if ( NULL == m_pcsSelfCharacter || NULL == m_pcsAgpmFactors ) return;

	AgpdFactor	*pcsFactorResult = (AgpdFactor *) m_pcsAgpmFactors->GetFactor(&m_pcsSelfCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT);

	if ( NULL == pcsFactorResult )	return;

	INT32	lValue = 0 ;
	
	// Class �Է� 
	m_pcsAgpmFactors->GetValue( pcsFactorResult, &lValue, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_CLASS );
	m_clPersonalInfo.SetClassRect( lValue );
	
	// Level �Է� 
	m_pcsAgpmFactors->GetValue( pcsFactorResult, &lValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL );
	m_clPersonalInfo.m_clLevel.SetNumber( lValue );	
	
	// Rank Bar �Է�
	lValue = m_pcsAgpmCharacter->GetMurdererPoint( m_pcsSelfCharacter );
	m_clPersonalInfo.SetRankBar( 120, lValue );
}
*/

/*****************************************************************
*   Function : CBUpdateFactor
*   Comment  : CBUpdateFactor
*   Date&Time : 2003-05-05, ���� 4:27
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
BOOL	AgcmCharacter::CBUpdateFactor			( PVOID pData, PVOID pClass, PVOID pCustData				)
{
	AgpdFactor*		pFactor =	(AgpdFactor*)pData		;
	AgcmCharacter*	pThis	=	(AgcmCharacter*)pClass	;

	// Self Character���� üũ 
	INT32	lCID = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	
	if ( NULL == pThis->m_pcsAgpmFactors ) return TRUE;

	pThis->m_pcsAgpmFactors->GetValue( pFactor, &lCID, AGPD_FACTORS_TYPE_OWNER, AGPD_FACTORS_OWNER_TYPE_ID );
	if ( pThis->m_lSelfCID != lCID ) return TRUE;

	// Factor Data ���� 
	// pThis->SetCharacterStatusInfo();

	// Personal Info�� Level ������ ����
	// pThis->SetPersonalInfo();

	return TRUE;
}


/******************************************************************************
******************************************************************************/

struct	__BlockingCollisionStruct
{
	RpIntersection	stIntersection		;
	BOOL			bCollision			;
	RwV3d			vCollisionNormal	;
};

static RpCollisionTriangle * __BlockingCollisionTriangleCallback
    (RpIntersection *intersection, RpCollisionTriangle *collTriangle,
    RwReal distance, void *data)
{
	__BlockingCollisionStruct	* pBlockingData = ( __BlockingCollisionStruct * ) data;

	pBlockingData->bCollision		= TRUE;
	pBlockingData->vCollisionNormal	= collTriangle->normal;

	return NULL; // ���̻� �˻����� ����..
}

static RpAtomic * __BlockingCollisionAtomicCallback (RpAtomic * atomic, void *data)
{
	__BlockingCollisionStruct	* pBlockingData = ( __BlockingCollisionStruct * ) data;

	RwFrame	* pFrame	= RpAtomicGetFrame( atomic );


	// ���������� ������Ʈ���� ����.
	RpGeometry * pLODGeometry = RpLODAtomicGetGeometry( atomic , 0 );

	if( pLODGeometry && pLODGeometry->triangles )
	{
		RpIntersection	stLocalIntersect	= pBlockingData->stIntersection	;
		RwMatrix *		pInverseLTM			= RwMatrixCreate()				;
		RwMatrix *		pLTM				= RwFrameGetLTM( pFrame )		;

		RwMatrixInvert( pInverseLTM , pLTM );

		RwV3dTransformPoint( &stLocalIntersect.t.line.start	, &stLocalIntersect.t.line.start	, pInverseLTM );
		RwV3dTransformPoint( &stLocalIntersect.t.line.end	, &stLocalIntersect.t.line.end		, pInverseLTM );

		RwMatrixDestroy( pInverseLTM );

		RpCollisionGeometryForAllIntersections(
			pLODGeometry							,
			&stLocalIntersect						,
			__BlockingCollisionTriangleCallback		,
			( void * ) & pBlockingData );

		if( pBlockingData->bCollision )
		{
			RwV3dTransformVector( &pBlockingData->vCollisionNormal , &pBlockingData->vCollisionNormal , pLTM );
		}
	}
	else
	{
		// �ͽ���Ʈ�ȳ༮��.. ������Ʈ�� ������ ����..
		// �׳� �������� �ѱ�.
		return atomic;
	}

	if( pBlockingData->bCollision )
	{
		// ���̻� üũ���� ����..
		return NULL;
	}
	else
	{
		return atomic;
	}
}

INT32	AgcmCharacter::GetValidDestination	( AuPOS * pStart , AuPOS * pDest , AuPOS * pValid , RwV3d * pNormal , BOOL * pbLeft )
{
	ASSERT( NULL != pStart );
	ASSERT( NULL != pDest );

	AuPOS	posDest = * pDest;

	//  ������  .. �������ʿ䰡 ������,.
	// m_pcsApmMap->GetValidDestination( pStart , pDest , & posDest );

	if( pValid )
	{
		* pValid = * pDest;
	}

	switch( m_pcsAgpmCharacter->GetBlockingType( GetSelfCharacter() ) )
	{
	default:
	case ApmMap::GROUND			:	// ���붥.
		break;
	case ApmMap::SKY			:	// �ϴ�.. ������ Ż��
		return NOBLOCKING;
	case ApmMap::UNDERGROUND	:	// ����.. �Ƹ� �Ⱦ��� ������
		if( pValid ) *pValid = *pStart;
		return UNKNOWN_BLOCKING;
	case ApmMap::GHOST			:	// ����.. ���ŷ�� �����Ѵ�.
		return NOBLOCKING;
	}

	INT32	nStartX , nEndX , nStartZ , nEndZ;
	INT32	nSegmentXStart	, nSegmentZStart;
	INT32	nSegmentXEnd	, nSegmentZEnd	;

	{
		INT32	nAX , nAZ , nBX , nBZ;

		//ApWorldSector	* pSectorA	=	m_pcsApmMap->GetSector( pStart->x , pStart->z );
		//ApWorldSector	* pSectorB	=	m_pcsApmMap->GetSector( posDest.x , posDest.z );

		nAX = PosToSectorIndexX( pStart->x );
		nAZ = PosToSectorIndexZ( pStart->z );
		nBX = PosToSectorIndexX( posDest.x );
		nBZ = PosToSectorIndexZ( posDest.z );

		if( nAX <= nBX )
		{
			nStartX	= nAX;
			nEndX	= nBX;
		}
		else
		{
			nStartX	= nBX;
			nEndX	= nAX;
		}

		if( nAZ <= nBZ )
		{
			nStartZ	= nAZ;
			nEndZ	= nBZ;
		}
		else
		{
			nStartZ	= nBZ;
			nEndZ	= nAZ;
		}

		// Ÿ�Ͽɼ�..���س�..
		FLOAT	fStart , fEnd  , fTop , fBottom;
		if( posDest.x < pStart->x )
		{
			fStart	= posDest.x;
			fEnd	= pStart->x;
		}
		else
		{
			fEnd	= posDest.x;
			fStart	= pStart->x;
		}
		
		if( posDest.z < pStart->z )
		{
			fTop	= posDest.z;
			fBottom	= pStart->z;
		}
		else
		{
			fBottom	= posDest.z;
			fTop	= pStart->z;
		}

		INT32	nXStart = ( INT32 ) GetSectorStartX( nStartX );
		INT32	nZStart = ( INT32 ) GetSectorStartX( nStartZ );
		INT32	nStepSize = ( INT32 ) MAP_STEPSIZE;

		nSegmentXStart = ( ( INT32 ) fStart - nXStart ) / nStepSize;
		nSegmentZStart = ( ( INT32 ) fTop - nZStart ) / nStepSize;

		nXStart = ( INT32 ) GetSectorStartX( nEndX );
		nZStart = ( INT32 ) GetSectorStartX( nEndZ );

		nSegmentXEnd = ( ( INT32 ) fEnd - nXStart ) / nStepSize;
		nSegmentZEnd = ( ( INT32 ) fBottom - nZStart ) / nStepSize;
	}

	INT32	nSectorX , nSectorZ;

	INT32	nSegLeft , nSegTop , nSegRight , nSegBottom;

	ApWorldSector * pSector;

	#define SECTOR_TOTAL_COL_COUNT	( SECTOR_MAX_COLLISION_OBJECT_COUNT * 10 )
	INT32	aObjectTotalList[ SECTOR_TOTAL_COL_COUNT ];
	INT32	nObjectTotalCount = 0;

	for( nSectorZ = nStartZ ; nSectorZ <= nEndZ /* ���ԵǾ����*/ ; nSectorZ ++ )
	{
		for( nSectorX = nStartX ; nSectorX <= nEndX /* ���ԵǾ����*/ ; nSectorX ++ )
		{
			pSector	= m_pcsApmMap->GetSector( nSectorX , nSectorZ );

			if( pSector )
			{
				if( nSectorX == nStartX )	nSegLeft	= nSegmentXStart;
				else						nSegLeft	= 0				;

				if( nSectorX == nEndX	)	nSegRight	= nSegmentXEnd	;
				else						nSegRight	= 15			;

				if( nSectorZ == nStartZ )	nSegTop		= nSegmentZStart;
				else						nSegTop		= 0				;

				if( nSectorZ == nEndZ	)	nSegBottom	= nSegmentZEnd	;
				else						nSegBottom	= 15			;

				INT32	aObjectList[ SECTOR_MAX_COLLISION_OBJECT_COUNT ];
				INT32	nObjectCount;

				INT32	nSegX , nSegZ;
				
				for( nSegZ = nSegTop ; nSegZ <= nSegBottom /* ���ԵǾ���� */; nSegZ ++ )
				{
					for( nSegX = nSegLeft ; nSegX <= nSegRight /* ���ԵǾ���� */ ; nSegX ++ )
					{
						nObjectCount	=	pSector->GetObjectCollisionID( nSegX , nSegZ , aObjectList , SECTOR_MAX_COLLISION_OBJECT_COUNT );

						while( nObjectCount )
						{
							// �ڿ��ź��� �˻��ؼ� �ߺ� �˻��� ��Ż����Ʈ�� �߰�..

							// �ߺ��˻�..
							int i = 0;
							for(i = 0 ; i < nObjectTotalCount ; ++ i )
							{
								if( aObjectTotalList[ i ] == aObjectList[ nObjectCount - 1 ] )
								{
									break;
								}
							}

							if( i == nObjectTotalCount )
							{
								// not found
								// ������Ʈ ���� �ʰ� , ���� Ȯ�� ���.
								// ASSERT( nObjectTotalCount < SECTOR_TOTAL_COL_COUNT );
								if( nObjectTotalCount < SECTOR_TOTAL_COL_COUNT )
								{
									aObjectTotalList[ nObjectTotalCount ++ ] = aObjectList[ nObjectCount - 1 ];
								}
							}

							// ����..
							nObjectCount--;
						}
						/////////////////////////

					}
				}
			}
			///////////////
		}
	}

	if( nObjectTotalCount )
	{
		__BlockingCollisionStruct	blockingdata;

		blockingdata.stIntersection.type				= rpINTERSECTLINE;

		blockingdata.stIntersection.t.line.start.x	= pStart->x	;
		blockingdata.stIntersection.t.line.start.z	= pStart->z	;
		blockingdata.stIntersection.t.line.start.y	= pStart->y	+ 100.0f	;	// 100�� 1���ʹϱ�.. 50cm ��..

		blockingdata.stIntersection.t.line.end.x	= posDest.x				;
		blockingdata.stIntersection.t.line.end.z	= posDest.z				;
		blockingdata.stIntersection.t.line.end.y	= posDest.y	+ 100.0f		;

		RwV3d	direction	;
		FLOAT	fDistance	;

		RwV3dSub( &direction , &blockingdata.stIntersection.t.line.end , &blockingdata.stIntersection.t.line.start );
		fDistance = RwV3dLength( &direction );
		RwV3dNormalize( &direction , & direction );

		static FLOAT _sfCheckDistance = 50.0f;
		RwV3dScale( &direction , & direction , _sfCheckDistance );	// ��Ÿ�� �Ÿ�..
		RwV3dAdd( &blockingdata.stIntersection.t.line.end , &blockingdata.stIntersection.t.line.start , &direction );


		// ������ (2005-05-31 ���� 12:13:30) : 
		// ����׿�..
		g_rwBlockingCheckLine = blockingdata.stIntersection.t.line; 

		// 50.0f ������ üũ
		int nSeparateCount = ( int ) ( fDistance / _sfCheckDistance );

		//@{ 2006/06/30 burumal
		// Dist�� 50 ������ �����Ұ�� �ʹ����� ���� ����ȴٰ� �ľ� �켱 �Ѱ谪�� �����ص�
		// Magoja���� Ȯ���� �ٰ��� ��û...
		if ( nSeparateCount > 100 )
			nSeparateCount = 100;
		//@}

		FLOAT	fLastHeight = pStart->y	+ 100.0f;
		
		for( int nSperator = 0 ; nSperator <= nSeparateCount ; nSperator ++ )
		{
			blockingdata.stIntersection.t.line.start.x	= pStart->x	+ direction.x * nSperator;
			blockingdata.stIntersection.t.line.start.z	= pStart->z	+ direction.z * nSperator;
			blockingdata.stIntersection.t.line.start.y	= fLastHeight	;	// 100�� 1���ʹϱ�.. 50cm ��..

			blockingdata.stIntersection.t.line.end.x	= pStart->x	+ direction.x * ( nSperator	+ 1 );
			blockingdata.stIntersection.t.line.end.z	= pStart->z	+ direction.z * ( nSperator	+ 1 );
			blockingdata.stIntersection.t.line.end.y	=
				m_pcsAgcmMap->HP_GetHeight(		blockingdata.stIntersection.t.line.end.x ,
												blockingdata.stIntersection.t.line.end.z ,
												blockingdata.stIntersection.t.line.start.y )
				+100.0f;

			fLastHeight = blockingdata.stIntersection.t.line.end.y;

			ApdObject		*	pstApdObject	;
			AgcdObject		*	pstAgcdObject	;

			blockingdata.bCollision			= FALSE;

			for( int i = 0 ; i < nObjectTotalCount ; ++ i )
			{
				pstApdObject	= m_pcsApmObject->GetObject		( aObjectTotalList[ i ]	);

				//  2004/07/26 ������ , ����ó�� �߰���..
				if( NULL == pstApdObject ) continue;

				pstAgcdObject	= m_pcsAgcmObject ? m_pcsAgcmObject->GetObjectData	( pstApdObject		) : NULL;

				//  2004/07/26 ������ , ����ó�� �߰���..
				if( NULL == pstAgcdObject ) continue;

				// ������ (2004-04-09 ���� 12:48:49) : �ӽ÷� , ������ ��ŷ ó���� ���� ����.
				if( 0 == strncmp( pstApdObject->m_pcsTemplate->m_szName , "tree" , 4 ) ) continue;
				
				if( pstApdObject && pstAgcdObject &&
					AcuObject::GetProperty( pstAgcdObject->m_lObjectType ) & ACUOBJECT_TYPE_BLOCKING )
				{
					if( pstAgcdObject->m_pstCollisionAtomic							&&
						//@{ kday 20050706
						//??
						pstAgcdObject->m_pstCollisionAtomic->geometry				&&
						//@} kday
						pstAgcdObject->m_pstCollisionAtomic->geometry->triangles	&&
						RpAtomicGetFrame( pstAgcdObject->m_pstCollisionAtomic )		)	// �������� ���� ��찡 �ִ�?..
					{
						LockFrame();

						//@{ 2006/06/30 burumal
						RwV3d vFrag;
						RwV3dSub(&vFrag, &blockingdata.stIntersection.t.line.end, &blockingdata.stIntersection.t.line.start);
						if ( vFrag.x < 0 )
							vFrag.x = -vFrag.x;
						if ( vFrag.y < 0 )
							vFrag.y = -vFrag.y;
						if ( vFrag.z < 0)
							vFrag.z = -vFrag.z;
						if ( vFrag.x + vFrag.y + vFrag.z <= 0.1f )
						{
							blockingdata.bCollision = false;
						}
						else
						{
						//@}

							// If there is collision atomic..
							RpAtomicForAllIntersections(
								pstAgcdObject->m_pstCollisionAtomic		,
								&blockingdata.stIntersection			,
								__BlockingCollisionTriangleCallback		,
								( void * ) & blockingdata	);

							if( blockingdata.bCollision )
							{
								RwFrame * pFrame	= RpAtomicGetFrame( pstAgcdObject->m_pstCollisionAtomic );
								RwMatrix * pLTM		= RwFrameGetLTM( pFrame )		;
								RwV3dTransformVector( &blockingdata.vCollisionNormal , &blockingdata.vCollisionNormal , pLTM );
							}

						//@{ 2006/06/30 burumal
						}
						//@}

						UnlockFrame();
					}
					else
					{
						AgcdObjectGroupData	*pstAgcdObjectGroupData;
						AgcdObjectGroupList	*pstAgcdObjectGroupList = pstAgcdObject->m_stGroup.m_pstList;
						while(pstAgcdObjectGroupList)
						{
							pstAgcdObjectGroupData = &pstAgcdObjectGroupList->m_csData;

							if(pstAgcdObjectGroupData->m_pstClump)
							{
								LockFrame();

								RpClumpForAllAtomics(pstAgcdObjectGroupData->m_pstClump, __BlockingCollisionAtomicCallback, (void *)(&blockingdata));

								UnlockFrame();
							}
	//						if( pstAgcdObject->m_pstClump )
	//							RpClumpForAllAtomics(  pstAgcdObject->m_pstClump
	//								, __BlockingCollisionAtomicCallback , ( void * ) & blockingdata );
							
							pstAgcdObjectGroupList = pstAgcdObjectGroupList->m_pstNext;
						}
					}
				}

				if( blockingdata.bCollision )
				{
					//  �ɸ���..
					if( pNormal )
					{
						* pNormal = blockingdata.vCollisionNormal;
					}

					if( pbLeft )
					{
						// �¿�üũ.

						// �ɸ����� At ���� ,
						RwV3d	at;
						{
							RwV3dSub( &at , &blockingdata.stIntersection.t.line.end , &blockingdata.stIntersection.t.line.start );
							RwV3dNormalize( &at , &at );
						}
			
						* pbLeft = __IsLeft( &at , &blockingdata.vCollisionNormal );
					}

					g_bBlockingCheckBlocked = TRUE;

					if( pValid )
					{
						pValid->x = blockingdata.stIntersection.t.line.start.x;
						pValid->y = blockingdata.stIntersection.t.line.start.y;
						pValid->z = blockingdata.stIntersection.t.line.start.z;
					}
					return OBJECT_BLOCKING;
				}
			}
		}
		
		g_bBlockingCheckBlocked = FALSE;
		return NOBLOCKING;
	}
	else
	{
		// �ɸ��� ������Ʈ�� ������ üũ���� �ʴ´�..
		g_bBlockingCheckBlocked = FALSE;
		return	NOBLOCKING;
	}
}

INT32	AgcmCharacter::BlockingPositionCheck( AuPOS * pSrc , AuPOS * pstPos , RwV3d * pNormal , BOOL * pbLeft )
{
	ASSERT( NULL != pSrc );
	ASSERT( NULL != pstPos );

	INT32	nStartX , nEndX , nStartZ , nEndZ;
	INT32	nSegmentXStart	, nSegmentZStart;
	INT32	nSegmentXEnd	, nSegmentZEnd	;

	{
		ApWorldSector	* pSectorA	=	m_pcsApmMap->GetSector( pSrc->x , pSrc->z );
		ApWorldSector	* pSectorB	=	m_pcsApmMap->GetSector( pstPos->x , pstPos->z );

		if( NULL == pSectorA || NULL == pSectorB )
		{
			return SECTOR_NOTFOUND;
		}

		if( pSectorA->GetIndexX() <= pSectorB->GetIndexX() )
		{
			nStartX	= pSectorA->GetIndexX();
			nEndX	= pSectorB->GetIndexX();
		}
		else
		{
			nStartX	= pSectorB->GetIndexX();
			nEndX	= pSectorA->GetIndexX();
		}

		if( pSectorA->GetIndexZ() <= pSectorB->GetIndexZ() )
		{
			nStartZ	= pSectorA->GetIndexZ();
			nEndZ	= pSectorB->GetIndexZ();
		}
		else
		{
			nStartZ	= pSectorB->GetIndexZ();
			nEndZ	= pSectorA->GetIndexZ();
		}

		// Ÿ�Ͽɼ�..���س�..
		FLOAT	fStart , fEnd  , fTop , fBottom;
		if( pstPos->x < pSrc->x )
		{
			fStart	= pstPos->x;
			fEnd	= pSrc->x;
		}
		else
		{
			fEnd	= pstPos->x;
			fStart	= pSrc->x;
		}
		
		if( pstPos->z < pSrc->z )
		{
			fTop	= pstPos->z;
			fBottom	= pSrc->z;
		}
		else
		{
			fBottom	= pstPos->z;
			fTop	= pSrc->z;
		}
			
		pSectorA	=	m_pcsApmMap->GetSector( nStartX	, nStartZ	);
		pSectorB	=	m_pcsApmMap->GetSector( nEndX	, nEndZ		);

		if( NULL == pSectorA || NULL == pSectorB )
		{
			return SECTOR_NOTFOUND;
		}

		pSectorA->D_GetSegment( SECTOR_HIGHDETAIL , fStart , fTop	, &nSegmentXStart	, &nSegmentZStart	);
		pSectorB->D_GetSegment( SECTOR_HIGHDETAIL , fEnd , fBottom	, &nSegmentXEnd		, &nSegmentZEnd		);
	}

	INT32	nSectorX , nSectorZ;

	INT32	nSegLeft , nSegTop , nSegRight , nSegBottom;

	ApWorldSector * pSector;

	#define SECTOR_TOTAL_COL_COUNT	( SECTOR_MAX_COLLISION_OBJECT_COUNT * 10 )
	INT32	aObjectTotalList[ SECTOR_TOTAL_COL_COUNT ];
	INT32	nObjectTotalCount = 0;

	for( nSectorZ = nStartZ ; nSectorZ <= nEndZ /* ���ԵǾ����*/ ; nSectorZ ++ )
	{
		for( nSectorX = nStartX ; nSectorX <= nEndX /* ���ԵǾ����*/ ; nSectorX ++ )
		{
			pSector	= m_pcsApmMap->GetSector( nSectorX , nSectorZ );

			if( pSector )
			{
				if( nSectorX == nStartX )	nSegLeft	= nSegmentXStart;
				else						nSegLeft	= 0				;

				if( nSectorX == nEndX	)	nSegRight	= nSegmentXEnd	;
				else						nSegRight	= 15			;

				if( nSectorZ == nStartZ )	nSegTop		= nSegmentZStart;
				else						nSegTop		= 0				;

				if( nSectorZ == nEndZ	)	nSegBottom	= nSegmentZEnd	;
				else						nSegBottom	= 15			;

				INT32	aObjectList[ SECTOR_MAX_COLLISION_OBJECT_COUNT ];
				INT32	nObjectCount;

				INT32	nSegX , nSegZ;
				
				for( nSegZ = nSegTop ; nSegZ <= nSegBottom /* ���ԵǾ���� */; nSegZ ++ )
				{
					for( nSegX = nSegLeft ; nSegX <= nSegRight /* ���ԵǾ���� */ ; nSegX ++ )
					{
						nObjectCount	=	pSector->GetObjectCollisionID( nSegX , nSegZ , aObjectList , SECTOR_MAX_COLLISION_OBJECT_COUNT );

						while( nObjectCount )
						{
							// �ڿ��ź��� �˻��ؼ� �ߺ� �˻��� ��Ż����Ʈ�� �߰�..

							// �ߺ��˻�..
							int i = 0;
							for(i = 0 ; i < nObjectTotalCount ; ++ i )
							{
								if( aObjectTotalList[ i ] == aObjectList[ nObjectCount - 1 ] )
								{
									break;
								}
							}

							if( i == nObjectTotalCount )
							{
								// not found
								// ������Ʈ ���� �ʰ� , ���� Ȯ�� ���.
								ASSERT( nObjectTotalCount <= SECTOR_TOTAL_COL_COUNT );
								if( nObjectTotalCount < SECTOR_TOTAL_COL_COUNT )
								{
									aObjectTotalList[ nObjectTotalCount ++ ] = aObjectList[ nObjectCount - 1 ];
								}
							}

							// ����..
							nObjectCount--;
						}
						/////////////////////////

					}
				}
			}
			///////////////
		}
	}

	if( nObjectTotalCount )
	{
		__BlockingCollisionStruct	blockingdata;

		blockingdata.stIntersection.type				= rpINTERSECTLINE;

		blockingdata.stIntersection.t.line.start.x	= pSrc->x	;
		blockingdata.stIntersection.t.line.start.z	= pSrc->z	;
		blockingdata.stIntersection.t.line.start.y	= pSrc->y	+ 100.0f	;	// 100�� 1���ʹϱ�.. 50cm ��..

		blockingdata.stIntersection.t.line.end.x	= pstPos->x				;
		blockingdata.stIntersection.t.line.end.z	= pstPos->z				;
		blockingdata.stIntersection.t.line.end.y	= pstPos->y	+ 100.0f		;

		RwV3d	direction	;
		FLOAT	fDistance	;

		RwV3dSub( &direction , &blockingdata.stIntersection.t.line.end , &blockingdata.stIntersection.t.line.start );
		fDistance = RwV3dLength( &direction );
		RwV3dNormalize( &direction , & direction );

		static FLOAT _sfCheckDistance = 50.0f;
		RwV3dScale( &direction , & direction , _sfCheckDistance );	// ��Ÿ�� �Ÿ�..
		RwV3dAdd( &blockingdata.stIntersection.t.line.end , &blockingdata.stIntersection.t.line.start , &direction );

		// ������ (2005-05-31 ���� 12:13:30) : 
		// ����׿�..
		g_rwBlockingCheckLine = blockingdata.stIntersection.t.line; 

		ApdObject		*	pstApdObject	;
		AgcdObject		*	pstAgcdObject	;

		blockingdata.bCollision			= FALSE;
		
		for( int i = 0 ; i < nObjectTotalCount ; ++ i )
		{
			pstApdObject	= m_pcsApmObject->GetObject		( aObjectTotalList[ i ]	);

			//  2004/07/26 ������ , ����ó�� �߰���..
			if( NULL == pstApdObject ) continue;

			pstAgcdObject	= m_pcsAgcmObject ? m_pcsAgcmObject->GetObjectData	( pstApdObject		) : NULL;

			//  2004/07/26 ������ , ����ó�� �߰���..
			if( NULL == pstAgcdObject ) continue;

			// ������ (2004-04-09 ���� 12:48:49) : �ӽ÷� , ������ ��ŷ ó���� ���� ����.
			if( 0 == strncmp( pstApdObject->m_pcsTemplate->m_szName , "tree" , 4 ) ) continue;
			
			if( pstApdObject && pstAgcdObject &&
				AcuObject::GetProperty( pstAgcdObject->m_lObjectType ) & ACUOBJECT_TYPE_BLOCKING )
			{
				if( pstAgcdObject->m_pstCollisionAtomic							&&
					//@{ kday 20050706
					//??
					pstAgcdObject->m_pstCollisionAtomic->geometry				&&
					//@} kday
					pstAgcdObject->m_pstCollisionAtomic->geometry->triangles	)
				{
					LockFrame();

					// If there is collision atomic..
					RpAtomicForAllIntersections(
						pstAgcdObject->m_pstCollisionAtomic		,
						&blockingdata.stIntersection			,
						__BlockingCollisionTriangleCallback		,
						( void * ) & blockingdata	);

					if( blockingdata.bCollision )
					{
						RwFrame * pFrame	= RpAtomicGetFrame( pstAgcdObject->m_pstCollisionAtomic );
						RwMatrix * pLTM		= RwFrameGetLTM( pFrame )		;
						RwV3dTransformVector( &blockingdata.vCollisionNormal , &blockingdata.vCollisionNormal , pLTM );
					}

					UnlockFrame();
				}
				else
				{
					AgcdObjectGroupData	*pstAgcdObjectGroupData;
					AgcdObjectGroupList	*pstAgcdObjectGroupList = pstAgcdObject->m_stGroup.m_pstList;
					while(pstAgcdObjectGroupList)
					{
						pstAgcdObjectGroupData = &pstAgcdObjectGroupList->m_csData;

						if(pstAgcdObjectGroupData->m_pstClump)
						{
							LockFrame();

							RpClumpForAllAtomics(pstAgcdObjectGroupData->m_pstClump, __BlockingCollisionAtomicCallback, (void *)(&blockingdata));

							UnlockFrame();
						}
//						if( pstAgcdObject->m_pstClump )
//							RpClumpForAllAtomics(  pstAgcdObject->m_pstClump
//								, __BlockingCollisionAtomicCallback , ( void * ) & blockingdata );
						
						pstAgcdObjectGroupList = pstAgcdObjectGroupList->m_pstNext;
					}
				}
			}

			if( blockingdata.bCollision )
			{
				//  �ɸ���..
				if( pNormal )
				{
					* pNormal = blockingdata.vCollisionNormal;
				}

				if( pbLeft )
				{
					// �¿�üũ.

					// �ɸ����� At ���� ,
					RwV3d	at;
					{
						RwV3d	vPos;
						vPos.x	= pSrc->x;
						vPos.z	= pSrc->z;
						vPos.y	= pSrc->y;

						RwV3d	vDst;
						vDst.x	= pstPos->x;
						vDst.z	= pstPos->z;
						vDst.y	= pstPos->y;

						RwV3dSub( &at , &vDst , &vPos );
						RwV3dNormalize( &at , &at );
					}
		
					* pbLeft = __IsLeft( &at , &blockingdata.vCollisionNormal );
				}

				g_bBlockingCheckBlocked = TRUE;
				return OBJECT_BLOCKING;
			}
		}

		g_bBlockingCheckBlocked = FALSE;
		return NOBLOCKING;
	}
	else
	{
		// �ɸ��� ������Ʈ�� ������ üũ���� �ʴ´�..
		g_bBlockingCheckBlocked = FALSE;
		return	NOBLOCKING;
	}

}
BOOL	AgcmCharacter::CBUpdatePositionCheck	( PVOID pData, PVOID pClass, PVOID pCustData				)
{
	AgcmCharacter	*	pThis		=	( AgcmCharacter	*	)	pClass		;
	AgpdCharacter	*	pCharacter	=	( AgpdCharacter	*	)	pData		;
	AuPOS			*	pstPos		=	( AuPOS			*	)	pCustData	;

	// ���� �ɸ��͸� üũ�Ѵ�.
	if (pCharacter->m_lID != pThis->m_lSelfCID || pCharacter->m_bSync)
		return TRUE;

	ASSERT( NULL != pThis					);
	ASSERT( NULL != pThis->m_pcsApmMap		);
	ASSERT( NULL != pThis->m_pcsApmObject	);
	ASSERT( NULL != pCharacter				);

	ApmMap::BLOCKINGTYPE eType = pThis->m_pcsAgpmCharacter->GetBlockingType( pThis->GetSelfCharacter() );
	if( eType == ApmMap::SKY ) return TRUE;

	// ���� ��쵵 �ֳ���..
	// ASSERT( NULL != pstPos		);
	if( NULL == pstPos ) return TRUE;

	BOOL bBlocked = FALSE;
	AuPOS vValid;
	if(	pstPos											&& 
		pThis->GetValidDestination( 
		&pThis->GetSelfCharacter()->m_stPos	,
		pstPos											,
		&vValid											) )
	{
		TRACE( "GetValidDestination üũ�ɸ�! \n" );
		// üũ�ƴ�.
		bBlocked = TRUE;
	}

	if( pstPos											&& 
		pThis->m_pcsAgpmCharacter->GetValidDestination( 
		&pThis->GetSelfCharacter()->m_stPos	,
		pstPos											,
		&vValid											,
		eType											,
		pCharacter											) )
	{
		TRACE( "GetValidDestination �ɸ��� üũ�ɸ�! \n" );
		// üũ�ƴ�.
		bBlocked = TRUE;
	}

	if( bBlocked )
	{
		pThis->SendStopCharacter();
		pThis->MoveSelfCharacter( &vValid , MD_NODIRECTION );
		return TRUE;
	}


	RwV3d	vNormal;
	BOOL	bLeft;
	switch( pThis->BlockingPositionCheck( &pCharacter->m_stPos , pstPos , &vNormal , &bLeft ) )
	{
	case	SECTOR_NOTFOUND		:
		// ����������.
		return FALSE;
	case	OBJECT_BLOCKING		:
		{
			if( AgcmCharacter::m_sbStandAloneMode )
			{
				// do nothing..;
			}
			else
			{
				// ������ (2005-03-22 ���� 12:24:19) : 
				// �ɸ��� ����� ó��..

				// ������ (2005-03-22 ���� 3:48:46) : �����

				if( pThis->m_bAdjustMovement )
				//if( pThis->m_bAdjustMovement && pThis->m_cPathNodeList.GetNextPath() == NULL )
				{
					// ���⼭ �̵� ���..
					// X Z ��鿡���� ���.

					// �ɸ����� At ���� ,
					RwV3d	at;
					{
						RwV3d	vPos;
						vPos.x	= pCharacter->m_stPos.x;
						vPos.z	= pCharacter->m_stPos.z;
						vPos.y	= pCharacter->m_stPos.y;

						RwV3d	vDst;
						vDst.x	= pstPos->x;
						vDst.z	= pstPos->z;
						vDst.y	= pstPos->y;

						RwV3dSub( &at , &vDst , &vPos );
						RwV3dNormalize( &at , &at );
					}
		
					// ���� ���� ���..
					AuPOS	posDst;

					RwMatrix	* pMatrix = RwMatrixCreate();

					RwV3d	yAxis;
					yAxis.x = 0.0f;
					yAxis.y = 1.0f;
					yAxis.z = 0.0f;

					// ������ (2005-03-22 ���� 2:37:33) : 
					// ��Ʈ�� ���� ..

					BOOL	bFoundDestination = FALSE;
					BOOL	bNoCollision = FALSE;

					{
						// ������ �� �ִ� ���� ã��..
						RwV3d	vComputedAt;
						RwV3d	vDest;

						RwV3dTransformVector( &vComputedAt , &at , pMatrix );

						// ������ (2005-03-29 ���� 7:12:36) : y���� ����...
						vComputedAt.y = 0;
						RwV3dNormalize( &vComputedAt , &vComputedAt );

						RwV3dScale( &vDest , &vComputedAt , g_fPathFindMinimumDistance );
						posDst.x = vDest.x + pCharacter->m_stPos.x;
						posDst.z = vDest.z + pCharacter->m_stPos.z;

						posDst.y = pThis->m_pcsAgcmMap->GetHeight( posDst.x , posDst.z , pCharacter->m_stPos.y );

						// �ɸ�����ġ���� posDst ���� �浹 �˻縦 �Ѵ�.
						switch( pThis->BlockingPositionCheck( &pCharacter->m_stPos , &posDst , &vNormal , NULL ) )
						{
						case	GEOMETRY_BLOCKING	:
						case	OBJECT_BLOCKING		:
							{
								// ����� 90�� �������� Ʋ� üũ..
								
								if( bLeft )
									RwMatrixRotate( pMatrix , &yAxis , -90.0f , rwCOMBINEREPLACE );
								else
									RwMatrixRotate( pMatrix , &yAxis , 90.0f , rwCOMBINEREPLACE );

								RwV3dTransformVector( &vComputedAt , &vNormal , pMatrix );
								vComputedAt.y = 0;
								RwV3dNormalize( &vComputedAt , &vComputedAt );

								RwV3dScale( &vDest , &vComputedAt , g_fPathFindMinimumDistance );
								posDst.x = vDest.x + pCharacter->m_stPos.x;
								posDst.z = vDest.z + pCharacter->m_stPos.z;

								posDst.y = pThis->m_pcsAgcmMap->GetHeight( posDst.x , posDst.z , pCharacter->m_stPos.y );

								switch( pThis->BlockingPositionCheck( &pCharacter->m_stPos , &posDst , NULL , NULL ) )
								{
								case	GEOMETRY_BLOCKING	:
								case	OBJECT_BLOCKING		:
									{
										// �ű⼭ ���� ���� ���ذ�..
										for( int i = 10 ; i <= g_snMaxTwistAngle ; i += 10 )
										{
											if( bLeft )
												RwMatrixRotate( pMatrix , &yAxis , -90.0f + ( FLOAT ) ( i ) , rwCOMBINEREPLACE );
											else
												RwMatrixRotate( pMatrix , &yAxis , 90.0f + ( FLOAT ) ( -i ) , rwCOMBINEREPLACE );

											// At ���͸� Ư�� ���� ��ŭ ȸ�����Ѽ�
											// �׻��¿��� ��Ÿ�� �Ÿ� ��ŭ �̵���
											// ��������Ʈ�� ��ǥ���� ����.

											RwV3dTransformPoint( &vComputedAt , &at , pMatrix );

											// ������ (2005-03-29 ���� 7:12:36) : y���� ����...
											vComputedAt.y = 0;
											RwV3dNormalize( &vComputedAt , &vComputedAt );

											RwV3dScale( &vDest , &vComputedAt , g_fPathFindMinimumDistance );
											posDst.x = vDest.x + pCharacter->m_stPos.x;
											posDst.z = vDest.z + pCharacter->m_stPos.z;

											posDst.y = pThis->m_pcsAgcmMap->GetHeight( posDst.x , posDst.z , pCharacter->m_stPos.y );

											// �ɸ�����ġ���� posDst ���� �浹 �˻縦 �Ѵ�.
											switch( pThis->BlockingPositionCheck( &pCharacter->m_stPos , &posDst , &vNormal , NULL ) )
											{
											case	GEOMETRY_BLOCKING	:
											case	OBJECT_BLOCKING		:
												{
													// �ٽ� �õ�..
													break;
												}

											default:
											case	NOBLOCKING			:
											case	SECTOR_NOTFOUND		:
											case	UNKNOWN_BLOCKING	:
												{
													// �̴�� ���� �ȴ�..
													bFoundDestination = TRUE;
													break;
												}
											}

											if( bFoundDestination ) break;
										}
									}
									break;
								default:
								case	NOBLOCKING			:
								case	SECTOR_NOTFOUND		:
								case	UNKNOWN_BLOCKING	:
									{
										// �̴�� ���� �ȴ�..
										bFoundDestination	= TRUE;
										break;
									}
								}

								break;
							}

						default:
						case	NOBLOCKING			:
						case	SECTOR_NOTFOUND		:
						case	UNKNOWN_BLOCKING	:
							{
								// �̴�� ���� �ȴ�..
								bFoundDestination	= TRUE;
								bNoCollision		= TRUE;
								break;
							}
						}

						/*
						for( int i = 10 ; i < g_snMaxTwistAngle ; i += 10 )
						{
							if( bLeft )
								RwMatrixRotate( pMatrix , &yAxis , ( FLOAT ) ( i ) , rwCOMBINEREPLACE );
							else
								RwMatrixRotate( pMatrix , &yAxis , ( FLOAT ) ( -i ) , rwCOMBINEREPLACE );

							// At ���͸� Ư�� ���� ��ŭ ȸ�����Ѽ�
							// �׻��¿��� ��Ÿ�� �Ÿ� ��ŭ �̵���
							// ��������Ʈ�� ��ǥ���� ����.

							RwV3dTransformPoint( &vComputedAt , &at , pMatrix );

							// ������ (2005-03-29 ���� 7:12:36) : y���� ����...
							vComputedAt.y = 0;
							RwV3dNormalize( &vComputedAt , &vComputedAt );

							RwV3dScale( &vDest , &vComputedAt , g_fPathFindMinimumDistance );
							posDst.x = vDest.x + pCharacter->m_stPos.x;
							posDst.z = vDest.z + pCharacter->m_stPos.z;
							posDst.y = pThis->m_pcsAgcmMap->GetHeight( posDst.x , posDst.z );

							// �ɸ�����ġ���� posDst ���� �浹 �˻縦 �Ѵ�.
							switch( pThis->BlockingPositionCheck( &pCharacter->m_stPos , &posDst , &vNormal , NULL ) )
							{
							case	GEOMETRY_BLOCKING	:
							case	OBJECT_BLOCKING		:
								{
									// �ٽ� �õ�..
									break;
								}

							default:
							case	NOBLOCKING			:
							case	SECTOR_NOTFOUND		:
							case	UNKNOWN_BLOCKING	:
								{
									// �̴�� ���� �ȴ�..
									bFoundDestination = TRUE;
									break;
								}
							}

							if( bFoundDestination ) break;
						}

						*/
					}

					RwMatrixDestroy( pMatrix );
					
					if( bFoundDestination || bNoCollision )
					{
						// vDest�� �̵� �õ���.

						pThis->m_cPathNodeList.resetIndex();

						AgpdPathFindPoint	cCurrentPathPoint;

						// ������ (2005-03-22 ���� 4:46:25) : �Ųٷ� �־�� ������� ����.
						// ��ǥ��
						cCurrentPathPoint.m_fX = pThis->m_posMoveEnd.x;
						cCurrentPathPoint.m_fY = pThis->m_posMoveEnd.z;
						pThis->m_cPathNodeList.insert( 0, &cCurrentPathPoint );

						// ������
						cCurrentPathPoint.m_fX = posDst.x;
						cCurrentPathPoint.m_fY = posDst.z;
						pThis->m_cPathNodeList.insert( 0, &cCurrentPathPoint );

						pThis->SendStopCharacter();
					}
					else
					{
						// do nothing..
						// ���� ����..
						pThis->SendStopCharacter();
					}

					return FALSE;
				}
				else
				{
					pThis->SendStopCharacter();
				}
			}

			return FALSE;
		}
		
	default:
	case	NOBLOCKING			:
	case	UNKNOWN_BLOCKING	:
	case	GEOMETRY_BLOCKING	:
		return TRUE;
	}
}

BOOL	AgcmCharacter::MoveReservedPath()
{
	#ifdef ENABLE_PATH_FIND_MODE
	// ������ (2005-03-11 ���� 11:37:24) : 
	// �н����ε� ó�� ���⼭..
	if( !m_bAdjustMovement ) return FALSE;

	if( m_nRetryCount >= g_sMoveRetryMaxCount )
		// ���� �̻��̸� ó������..
		return FALSE;

	// ������ (2005-03-29 ���� 4:09:03) : ��õ� Ƚ�� ����..
	m_nRetryCount++;

	CPriorityListNode< int , AgpdPathFindPoint *> *pcsNode;

	pcsNode = m_cPathNodeList.GetNextPath();

	if( pcsNode )
	{
		AuPOS			csPos;

		csPos.x = pcsNode->data->m_fX;
		csPos.z = pcsNode->data->m_fY;
		csPos.y = this->m_pcsAgcmMap->GetHeight( csPos.x , csPos.z , m_pcsSelfCharacter->m_stPos.y );
		
		MoveSelfCharacter( & csPos , MD_NODIRECTION );		
		return TRUE;
	}
	else
	{
		return FALSE;
	}
	#else
	return FALSE;
	#endif // ENABLE_PATH_FIND_MODE
}

BOOL AgcmCharacter::SendMoveMoneyFromInventoryToBank(AgpdCharacter *pcsCharacter, INT64 llMoney)
{
	if (!pcsCharacter ||
		llMoney <= 0)
		return FALSE;

	INT64	llCurrentMoney	= 0;
	if (!m_pcsAgpmCharacter->GetMoney(pcsCharacter, &llCurrentMoney))
		return FALSE;

	if (llCurrentMoney < llMoney)
		llMoney	= llCurrentMoney;

	INT8	cOperation = AGPMCHAR_PACKET_OPERATION_MOVE_BANKMONEY;
	INT16	nPacketLength = 0;

	PVOID	pvPacket = m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMCHARACTER_PACKET_TYPE,
																&cOperation,							// Operation
																&pcsCharacter->m_lID,					// Character ID
																NULL,									// Character Template ID
																NULL,									// Game ID
																NULL,									// Character Status
																NULL,									// Move Packet
																NULL,									// Action Packet
																NULL,									// Factor Packet
																NULL,									// llMoney
																&llMoney,								// bank money
																NULL,									// cash
																NULL,									// character action status
																NULL,									// character criminal status
																NULL,									// attacker id (������� ������ �ʿ�)
																NULL,									// ���� �����Ǽ� �ʿ� �������� ����
																NULL,									// region index
																NULL,									// social action index
																NULL,									// special status
																NULL,									// is transform status
																NULL,									// skill initialization text
																NULL,									// face index
																NULL,									// hair index
																NULL,									// Option Flag
																NULL,									// bank size
																NULL,									// event status flag
																NULL,									// remained criminal status time
																NULL,									// remained murderer point time
																NULL,									// nick name
																NULL,									// gameguard
																NULL									// last killed time in battlesquare
																);

	if (!pvPacket ||
		nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult = SendPacket(pvPacket, nPacketLength, 0, PACKET_PRIORITY_NONE, GetSelfNID());

	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgcmCharacter::SendMoveMoneyFromBankToInventory(AgpdCharacter *pcsCharacter, INT64 llMoney)
{
	if (!pcsCharacter ||
		llMoney <= 0)
		return FALSE;

	INT64	llCurrentBankMoney	= m_pcsAgpmCharacter->GetBankMoney(pcsCharacter);

	if (llCurrentBankMoney < llMoney)
		llMoney	= llCurrentBankMoney;

	INT8	cOperation = AGPMCHAR_PACKET_OPERATION_MOVE_BANKMONEY;
	INT16	nPacketLength = 0;
	INT64	llBankMoney = -llMoney;

	PVOID	pvPacket = m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMCHARACTER_PACKET_TYPE,
																&cOperation,								// Operation
																&pcsCharacter->m_lID,						// Character ID
																NULL,										// Character Template ID
																NULL,										// Game ID
																NULL,										// Character Status
																NULL,										// Move Packet
																NULL,										// Action Packet
																NULL,										// Factor Packet
																NULL,										// llMoney
																&llBankMoney,								// bank money
																NULL,										// cash
																NULL,										// character action status
																NULL,										// character criminal status
																NULL,										// attacker id (������� ������ �ʿ�)
																NULL,										// ���� �����Ǽ� �ʿ� �������� ����
																NULL,										// region index
																NULL,										// social action index
																NULL,										// special status
																NULL,										// is transform status
																NULL,										// skill initialization text
																NULL,										// face index
																NULL,										// hair index
																NULL,										// Option Flag
																NULL,										// bank size
																NULL,										// event status flag
																NULL,										// remained criminal status time
																NULL,										// remained murderer point time
																NULL,										// nick name
																NULL,										// gameguard
																NULL										// last killed time in battlesquare
																);

	if (!pvPacket ||
		nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult = SendPacket(pvPacket, nPacketLength, 0, PACKET_PRIORITY_NONE, GetSelfNID());

	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgcmCharacter::CBUpdateMoney(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmCharacter		*pThis			= (AgcmCharacter *)	pClass;
	AgpdCharacter		*pcsCharacter	= (AgpdCharacter *)	pData;

	// �κ��丮�� ���� ������ ���⼭ �����丮�� ���� ������Ʈ ���ش�.
	//
	//
	//
	//
	//
	//
	//

	return TRUE;
}

BOOL AgcmCharacter::CBUpdateBankMoney(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmCharacter		*pThis			= (AgcmCharacter *)	pClass;
	AgpdCharacter		*pcsCharacter	= (AgpdCharacter *)	pData;

	// ��ũ�� ���� ������ ���⼭ ��ũ�� ���� ������Ʈ �����ش�.
	//
	//
	//
	//
	//
	//
	//
	//

	return TRUE;
}

BOOL AgcmCharacter::LoadAllTemplateClump()
{
	AgcdCharacterTemplate	*pstAgcdCharacterTemplate;
	INT32					lIndex = 0;

	for (	AgpdCharacterTemplate *pstAgpdCharacterTemplate = m_pcsAgpmCharacter->GetTemplateSequence(&lIndex);
			pstAgpdCharacterTemplate;
			pstAgpdCharacterTemplate = m_pcsAgpmCharacter->GetTemplateSequence(&lIndex)							)
	{
		pstAgcdCharacterTemplate = GetTemplateData(pstAgpdCharacterTemplate);
		if (!pstAgcdCharacterTemplate)
			return FALSE;

		++pstAgcdCharacterTemplate->m_lRefCount;
		if (!LoadTemplateData(pstAgpdCharacterTemplate, pstAgcdCharacterTemplate))
			return FALSE;

/*		if(pstAgcdCharacterTemplate->m_szDFFName[0])
			VERIFY(pstAgcdCharacterTemplate->m_pClump = LoadClump(pstAgcdCharacterTemplate->m_szDFFName));

		if(pstAgcdCharacterTemplate->m_szDADFFName[0])
			VERIFY(pstAgcdCharacterTemplate->m_pDefaultArmourClump = LoadClump(pstAgcdCharacterTemplate->m_szDADFFName));*/
	}

	return TRUE;
}

RpAtomic *AgcmCharacter::LoadAtomic( const CHAR *szDFF, const CHAR *szSubPath, RpHAnimHierarchy* pstHierarchy )
{
	RwStream *	stream	= (RwStream *	)NULL;
	RpAtomic *	atomic	= (RpAtomic *	)NULL;
	RwChar *	path	= (RwChar *		)NULL;
	//	CHAR		szOldPath[AGCMCHAR_MAX_PATH];
	CHAR		szFullPath[AGCMCHAR_MAX_PATH];

	strcpy( szFullPath, m_szClumpPath );
	if( szSubPath ) {
		strcat( szFullPath, szSubPath );
	}
	strcat( szFullPath, szDFF );

	if( m_pcsAgcmResourceLoader )
	{
		atomic = m_pcsAgcmResourceLoader->LoadAtomic(szFullPath, "Character.txd", NULL, -1, m_szTexturePath, pstHierarchy);
	}

	return atomic;
}

BOOL	AgcmCharacter::CBLoadMap( PVOID pData, PVOID pClass, PVOID pCustData				)
{
	AgcmCharacter *	pThis = (AgcmCharacter *) pClass;
	ApWorldSector *	pSector = (ApWorldSector *) pData;
	AgpdCharacter *	pcsCharacter;
	ApWorldSector::IdPos *	pUser;

	// 2005/02/21 ������
	// Ŭ���̾�Ʈ�� ����� ó���� �����Ƿ�....
	// 0�� ����� ����Ÿ�� �׳� ����Ѵ�..
	ApWorldSector::Dimension	* pDimension;
	pDimension = pSector->GetDimension( 0 );

	if( pDimension )
	{
		ApAutoReaderLock	csLock1( pDimension->lockUsers		);
		ApAutoReaderLock	csLock2( pDimension->lockNPCs		);
		ApAutoReaderLock	csLock3( pDimension->lockMonsters	);

		// �ƴϸ�, �ش� Map�� �ִ� User���� �����ͼ� UpdatePosition() ���ش�.
		for (pUser = pDimension->pUsers; pUser; pUser = pUser->pNext)
		{
			pcsCharacter = pThis->m_pcsAgpmCharacter->GetCharacter(pUser->id2);
			if (pcsCharacter)
			{
				pThis->m_pcsAgpmCharacter->UpdatePosition(pcsCharacter, NULL, FALSE);
			}
		}

		for (pUser = pDimension->pNPCs; pUser; pUser = pUser->pNext)
		{
			pcsCharacter = pThis->m_pcsAgpmCharacter->GetCharacter(pUser->id2);
			if (pcsCharacter)
			{
				pThis->m_pcsAgpmCharacter->UpdatePosition(pcsCharacter, NULL, FALSE);
			}
		}

		for (pUser = pDimension->pMonsters; pUser; pUser = pUser->pNext)
		{
			pcsCharacter = pThis->m_pcsAgpmCharacter->GetCharacter(pUser->id2);
			if (pcsCharacter)
			{
				pThis->m_pcsAgpmCharacter->UpdatePosition(pcsCharacter, NULL, FALSE);
			}
		}
	}

	return TRUE;
}

BOOL AgcmCharacter::CBTransformAppear( PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgcmCharacter::CBTransformAppear");
	if((!pClass) || (!pData)) return FALSE;

	AgcmCharacter *			pThis				= (AgcmCharacter*) pClass;
	AgpdCharacter *			pcsAgpdCharacter	= (AgpdCharacter *) pData;
	INT32					nPrevTID			= *(INT32 *) pCustData;

	// �ɸ��Ͱ� �̹� ������ �ǰ� �ִ� ���¶�� ���� �����Ѵ�.
	if( !pcsAgpdCharacter->m_bIsReadyRemove )
	{
		AgpmSiegeWar * pmAgpmSiegeWar = ( AgpmSiegeWar * ) pThis->GetModule("AgpmSiegeWar");
		if( pmAgpmSiegeWar->IsSiegeWarMonster( pcsAgpdCharacter ) )
		{
			// ����������Ʈ�� ��� ť�� �̿��ؼ� ���̵鿡�� ó��..
			pThis->QueueCharPolyMorphByTID( pcsAgpdCharacter , nPrevTID );
		}
		else
		{
			pThis->OnPolyMorph( pcsAgpdCharacter, nPrevTID );
		}
	}

	return TRUE;
}

BOOL AgcmCharacter::CBEvolutionAppear( PVOID pData, PVOID pClass, PVOID pCustData )
{
	PROFILE("AgcmCharacter::CBEvolutionAppear");
	if((!pClass) || (!pData)) return FALSE;

	AgcmCharacter *			pThis				= (AgcmCharacter*) pClass;
	AgpdCharacter *			pcsAgpdCharacter	= (AgpdCharacter *) pData;

	// �ɸ��Ͱ� �̹� ������ �ǰ� �ִ� ���¶�� ���� �����Ѵ�.
	if( !pcsAgpdCharacter->m_bIsReadyRemove )
	{
		AuCharClassType eNextClassType = pThis->GetClassTypeByTID( pcsAgpdCharacter->m_lTID1 );
		if( eNextClassType == AUCHARCLASS_TYPE_NONE ) return FALSE;

		// ĳ���� �𵨸��� �ٲٰ�..
		if( pThis->StartCharEvolutionByTID( pcsAgpdCharacter , pcsAgpdCharacter->m_lTID1 ) )
		{
			// ���ӳʴ� ����ó�� ���ش�.
			if( eNextClassType != AUCHARCLASS_TYPE_MAGE )
			{
				// ĳ���Ϳ� ������ �������� �ٲ۴�.
				pThis->OnChagneEquipItemByEvolution( pcsAgpdCharacter, eNextClassType );
			}
			else
			{
				// ���ӳ��� ��� Ư�� ��� �����۸� ó�����ش�.. ī�и�..
				pThis->OnEquipCharonForSummoner( pcsAgpdCharacter );
			}
		}
	}

	return TRUE;
}

BOOL AgcmCharacter::CBSocialAnimation(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmCharacter		*pThis				= (AgcmCharacter *)		pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)		pData;
	UINT8				ucSocialAnimation	= *(UINT8 *)			pCustData;

	//
	//
	//
	//
	//
	//
	//

	return TRUE;
}

BOOL AgcmCharacter::CBUpdateCustomize		( PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmCharacter		*pThis				= (AgcmCharacter *)		pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)		pData;

	AgcdCharacter		*pstAgcdCharacter	= pThis->GetCharacterData(pcsCharacter);

	pThis->m_pcsAgcmResourceLoader->RemoveLoadEntry	(pThis->m_lLoaderCustomize, (PVOID) pcsCharacter, NULL);
	pThis->m_pcsAgcmResourceLoader->AddLoadEntry	(pThis->m_lLoaderCustomize, (PVOID) pcsCharacter, NULL);

	return TRUE;
}

BOOL AgcmCharacter::CBCustomizeLoader		( PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmCharacter *			pThis = (AgcmCharacter *) pClass;
	AgpdCharacter *			pcsCharacter = (AgpdCharacter *) pData;

	AgcdCharacter		*pstAgcdCharacter	= pThis->GetCharacterData(pcsCharacter);

	pThis->OnUpdateCharacterCustomize( pcsCharacter, pstAgcdCharacter );
	return TRUE;
}


/*
	2005.05.31 By SungHoon
	�ź� ����ũ�� ����� ��� �Ҹ���.
*/
BOOL AgcmCharacter::CBUpdateOptionFlag( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmCharacter		*pThis				= (AgcmCharacter *)		pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)		pData;
	INT32				lOptionFlag			= *(INT32		 *)		pCustData;

	pThis->EnumCallback(AGCMCHAR_CB_ID_UPDATE_OPTION_FLAG, pcsCharacter, pCustData);
	
	pcsCharacter->m_lOptionFlag = lOptionFlag;

	return TRUE;
}

BOOL AgcmCharacter::CBDisconnectByAnotherUser(PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmCharacter			*pThis;
	BOOL					bResult;

	bResult = FALSE;

	pThis = (AgcmCharacter *)pClass;

	return bResult;
}

BOOL AgcmCharacter::CBUpdateFactorMovement(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmCharacter		*pThis			= (AgcmCharacter *)		pClass;
	AgpdFactor			*pcsFactor		= (AgpdFactor *)		pData;

	AgpdFactorOwner		*pcsFactorOwner	= (AgpdFactorOwner *) pThis->m_pcsAgpmFactors->GetFactor(pcsFactor, AGPD_FACTORS_TYPE_OWNER);
	if (!pcsFactorOwner || pcsFactorOwner->lValue[AGPD_FACTORS_OWNER_TYPE_ID] == AP_INVALID_CID || !pcsFactorOwner->lValue[AGPD_FACTORS_OWNER_TYPE_OWNER])
		return TRUE;

	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pcsFactorOwner->lValue[AGPD_FACTORS_OWNER_TYPE_OWNER];

	if (pcsCharacter->m_bMove)
	{
		return pThis->m_pcsAgpmCharacter->MoveCharacter(pcsCharacter,
														&pcsCharacter->m_stDestinationPos,
														pcsCharacter->m_eMoveDirection,
														pcsCharacter->m_bPathFinding,
														pcsCharacter->m_bMoveFast,
														pcsCharacter->m_bHorizontal);
	}

	pThis->EnumCallback(AGCMCHAR_CB_ID_UPDATE_MOVEMENT, pcsCharacter, NULL);

	return TRUE;
}

BOOL AgcmCharacter::CBUpdateFactorHP(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmCharacter		*pThis			= (AgcmCharacter *)		pClass;
	AgpdFactor			*pcsFactor		= (AgpdFactor *)		pData;
	PVOID				*ppvBuffer		= (PVOID *)				pCustData;

	AgpdFactorOwner		*pcsFactorOwner	= (AgpdFactorOwner*)pThis->m_pcsAgpmFactors->GetFactor(pcsFactor, AGPD_FACTORS_TYPE_OWNER);
	if (!pcsFactorOwner || pcsFactorOwner->lValue[AGPD_FACTORS_OWNER_TYPE_ID] == AP_INVALID_CID || !pcsFactorOwner->lValue[AGPD_FACTORS_OWNER_TYPE_OWNER])
		return TRUE;

	AgpdCharacter		*pcsCharacter	= (AgpdCharacter *)	pcsFactorOwner->lValue[AGPD_FACTORS_OWNER_TYPE_OWNER];

	BOOL				bIsReflectFactor= (BOOL) ppvBuffer[1];



	if (bIsReflectFactor)
	{	
		// Action Queue�� �ִ� Factor���� ��� �� �ʱ�ȭ ��Ų��.
		///////////////////////////////////////////////////////

		AgcdCharacter	*pstAgcdCharacter	= pThis->GetCharacterData(pcsCharacter);
		if (!pstAgcdCharacter)
			return FALSE;

		for (int i = 0; i < pstAgcdCharacter->m_lNumActionData; ++i)
		{
			pThis->ClearActionFactor(pcsCharacter, &pstAgcdCharacter->m_astActionQueue[i]);
		}
	}

	INT32				lOldHP			= (INT32) ppvBuffer[0];


	// 2006. 6. 21. Nonstopdj
	//. TID Polymorph
	//. �ϴ� ���� �������� ���ŵǴ� TID�� ĳ���͸� �ش�.
	if(pThis->m_pcsAgpmCharacter->IsPolyMorph(pcsCharacter))
	{
		INT32		CurHp,MaxHp;
		float		Per_Result;

		AgpdFactor* pcsFactorResult = (AgpdFactor*)pThis->m_pcsAgpmFactors->GetFactor(&pcsCharacter->m_csFactor,
			AGPD_FACTORS_TYPE_RESULT);
		if(!pcsFactorResult)
			pcsFactorResult	= &pcsCharacter->m_csFactor;

		pThis->m_pcsAgpmFactors->GetValue(pcsFactorResult, &CurHp, AGPD_FACTORS_TYPE_CHAR_POINT,
			AGPD_FACTORS_CHARPOINT_TYPE_HP);
		pThis->m_pcsAgpmFactors->GetValue(pcsFactorResult, &MaxHp, AGPD_FACTORS_TYPE_CHAR_POINT_MAX,
			AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);

		if (CurHp > MaxHp)
			CurHp = MaxHp;

		if (MaxHp <= 0 || CurHp <= 0)
			Per_Result = 0;
		else
			Per_Result = (float)CurHp / (float)MaxHp;

		//################################################################3
		// ��� �ڵ�
		// ��ġ�� �ӹ��� ���� ���� �ӽ��ڵ�
		if (0 == CurHp)
			pThis->RemoveDuplicateSiegeWarObject(pcsCharacter);
			
		pThis->SetCharacterPolyMorph(pcsCharacter, Per_Result);	
	}

	pThis->EnumCallback(AGCMCHAR_CB_ID_UPDATE_CHAR_HP, pcsCharacter, &lOldHP);

	const INT32 balrogId = 817;
	if(pcsCharacter->m_lTID1==balrogId)
	{
		AgcdCharacter *pstAgcdCharacter	= pThis->GetCharacterData(pcsCharacter);
		if(pstAgcdCharacter->m_pClump)
		// Adjust Balrog's uv-animation-fx weight according to its HP.
		{
			// Get old & new normalized HP of the Balrog.
			INT32 curHp, maxHp;
			AgpdFactor *pcsFactorResult = (AgpdFactor*)pThis->m_pcsAgpmFactors->GetFactor(pcsFactor, AGPD_FACTORS_TYPE_RESULT);
			if(!pcsFactorResult)
				pcsFactorResult	= pcsFactor;

			pThis->m_pcsAgpmFactors->GetValue(pcsFactorResult, &curHp, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
			pThis->m_pcsAgpmFactors->GetValue(pcsFactorResult, &maxHp, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);

			if(curHp > maxHp)
				curHp = maxHp;

			float newHpNorm, oldHpNorm;
			if(maxHp <= 0)
				newHpNorm = oldHpNorm = 0;
			else
			{
				newHpNorm = (float)curHp/(float)maxHp;
				oldHpNorm = (float)lOldHP/(float)maxHp;
			}

			float weight = getBalrogUVAnimFxWeight(oldHpNorm, newHpNorm);
			// If a transition is necessary, do the transition to the appropriate weight.
			if(weight)
				RpClumpForAllAtomics(pstAgcdCharacter->m_pClump, adjustUVAnimFxWeightCB, &weight);
		}
	}
	//@} Jaewon

	return TRUE;
}

BOOL AgcmCharacter::CBUpdateFactorMP(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmCharacter		*pThis			= (AgcmCharacter *)		pClass;
	AgpdFactor			*pcsFactor		= (AgpdFactor *)		pData;
	PVOID				*ppvBuffer		= (PVOID *)				pCustData;

	AgpdFactorOwner		*pcsFactorOwner	= (AgpdFactorOwner *) pThis->m_pcsAgpmFactors->GetFactor(pcsFactor, AGPD_FACTORS_TYPE_OWNER);
	if (!pcsFactorOwner || pcsFactorOwner->lValue[AGPD_FACTORS_OWNER_TYPE_ID] == AP_INVALID_CID || !pcsFactorOwner->lValue[AGPD_FACTORS_OWNER_TYPE_OWNER])
		return TRUE;

	AgpdCharacter		*pcsCharacter	= (AgpdCharacter *)	pcsFactorOwner->lValue[AGPD_FACTORS_OWNER_TYPE_OWNER];

	/*
	BOOL				bIsReflectFactor	= (BOOL) ppvBuffer[1];

	if (bIsReflectFactor)
	{	
		// Action Queue�� �ִ� Factor���� ��� �� �ʱ�ȭ ��Ų��.
		///////////////////////////////////////////////////////

		AgcdCharacter	*pstAgcdCharacter	= pThis->GetCharacterData(pcsCharacter);
		if (!pstAgcdCharacter)
			return FALSE;

		for (int i = 0; i < pstAgcdCharacter->m_lNumActionData; ++i)
		{
			pThis->ClearActionFactor(&pstAgcdCharacter->m_astActionQueue[i]);
		}
	}
	*/

	INT32				lOldMP			= (INT32) ppvBuffer[0];

	pThis->EnumCallback(AGCMCHAR_CB_ID_UPDATE_CHAR_MP, pcsCharacter, &lOldMP);

	return TRUE;
}

//@{ Jaewon 20050826
// Tell AgpmCharacter not to remove this character and register it to the fade-out removal list.
// And make its clump fade out. 
BOOL AgcmCharacter::CBCheckRemoveChar( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmCharacter	*pThis		= (AgcmCharacter *)	pClass;
	INT32			*lCID		= (INT32 *)		pData;
	BOOL			*pbRemove	= (BOOL *)		pCustData;

	// If it's already FALSE, any further processings are not necessary.
	if(*pbRemove == FALSE)
		return TRUE;

	if(*lCID == AP_INVALID_CID)
		return FALSE;

	AgpdCharacter *pcsCharacter = pThis->m_pcsAgpmCharacter->GetCharacter(*lCID);
	if(!pcsCharacter)
		return FALSE;

	AgcdCharacter *pAgcdCharacter = pThis->GetCharacterData(pcsCharacter);
	if(!pAgcdCharacter)
		return FALSE;

	RpClump *pClump = pAgcdCharacter->m_pClump;
	if(!pClump)
		return FALSE;

	if (pThis->m_pcsSelfCharacter)
	{
		if (pcsCharacter->m_lID == pThis->m_pcsSelfCharacter->m_lID)
		{
			*pbRemove = FALSE;
			return FALSE;
		}
	}

	*pbRemove = FALSE;
	std::pair<AgpdCharacter*, RwUInt32> entry;
	entry.first = pcsCharacter;
	entry.second = timeGetTime();
	pThis->m_fadeOutReomvalList.push_back(entry);
	pThis->m_pcsAgcmRender->StartFadeOutClump(pClump, 0);	
	//@{ Jaewon 20050829
	// If it has a horse, fade out that, too.
	if(pAgcdCharacter->m_pRide && pAgcdCharacter->m_pRide->m_pClump)
		pThis->m_pcsAgcmRender->StartFadeOutClump(pAgcdCharacter->m_pRide->m_pClump, 0);
	//@} Jaewon

	return TRUE;
}
//@} Jaewon

//@{ Jaewon 20050913
// If a character in fade-out removal list has been readded,
// it have to be removed from the list so that it remains in the world.
#include <functional>
class reAdded : std::unary_function<std::pair<AgpdCharacter*, RwUInt32>, bool>
{
public:
	reAdded(const AgpdCharacter *pCharacterReAdded) 
	{ pCharacterReAdded_ = pCharacterReAdded; }

	result_type operator()(argument_type x)
	{
		if(x.first == pCharacterReAdded_)
			return true;
		else
			return false;
	}

private:
	const AgpdCharacter *pCharacterReAdded_;
};

struct _GG_AUTH_DATA;
BOOL AgcmCharacter::CBGameguardAuth( PVOID pData, PVOID pClass, PVOID pCustData )
{
#ifndef _AREA_CHINA_
	if ( !pCustData ) 
		return FALSE;

	GetNProtect().Auth( pCustData );
#endif
	return TRUE;
}

BOOL
AgcmCharacter::CBCheckMovementLock(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PVOID * pArg = (PVOID *)pData;
	AgpdCharacter * pcsChar = (AgpdCharacter *)pArg[0];
	BOOL * pLocked = (BOOL *)pArg[1];
	AgcmCharacter * pThis = (AgcmCharacter *)pClass;

	*pLocked = (pThis->m_pcsSelfCharacter == pcsChar && 
		AuPacket::GetLastProcessedFrameTick() < pThis->m_pcsAgpmCharacter->GetMoveLockFrameTick());
	return TRUE;
}

BOOL	AgcmCharacter::CBAlreadyExistChar(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmCharacter	*pThis			= (AgcmCharacter *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	BOOL			*pbIsNeedRemove	= (BOOL *)			pCustData;

	// Search the fade-out removal list for the character.
	std::deque<std::pair<AgpdCharacter*, RwUInt32> >::iterator found
		= std::find_if(pThis->m_fadeOutReomvalList.begin(), pThis->m_fadeOutReomvalList.end(), reAdded(pcsCharacter));

	// Remove it from the list(It has been readded, so it should not be removed).
	if(found != pThis->m_fadeOutReomvalList.end())
	{
		AgcdCharacter *pAgcdCharacter = pThis->GetCharacterData(found->first);
		if(pAgcdCharacter)
		{
			RpClump *pClump = pAgcdCharacter->m_pClump;
			if(pClump)
			{
				pThis->m_pcsAgcmRender->StartFadeInClump(pClump, 0);	
				if(pAgcdCharacter->m_pRide && pAgcdCharacter->m_pRide->m_pClump)
					pThis->m_pcsAgcmRender->StartFadeInClump(pAgcdCharacter->m_pRide->m_pClump, 0);
			}
		}

		pThis->m_fadeOutReomvalList.erase(found);

		*pbIsNeedRemove	= TRUE;
	}

	return TRUE;
}
//@} Jaewon

BOOL AgcmCharacter::SetPreLightForAllCharacter(FLOAT fOffset)
{
	if ((fOffset < 0.0f) || (fOffset > 2.0f))
		return FALSE;

	AgcdCharacterTemplate	*pstAgcdCharacterTemplate;
	AgpdCharacter			*pstAgpdCharacter;
	AgcdCharacter			*pstAgcdCharacter;
	RwRGBA					stRGBA;
	FLOAT					fRed, fBlue, fGreen, fAlpha;

	INT32					lIndex = 0;

	for (	pstAgpdCharacter = m_pcsAgpmCharacter->GetCharSequence(&lIndex);
			pstAgpdCharacter;
			pstAgpdCharacter = m_pcsAgpmCharacter->GetCharSequence(&lIndex)		)
	{
		pstAgcdCharacter = GetCharacterData(pstAgpdCharacter);
		if (!pstAgcdCharacter)
			return FALSE;

		pstAgcdCharacterTemplate = pstAgcdCharacter->m_pstAgcdCharacterTemplate;

		if ((pstAgcdCharacter->m_pClump) && (pstAgcdCharacterTemplate->m_pcsPreData))
		{
			/*fRed			= (FLOAT)(pstAgcdCharacterTemplate->m_stPreLight.red) * fOffset;
			fBlue			= (FLOAT)(pstAgcdCharacterTemplate->m_stPreLight.green) * fOffset;
			fGreen			= (FLOAT)(pstAgcdCharacterTemplate->m_stPreLight.blue) * fOffset;
			fAlpha			= (FLOAT)(pstAgcdCharacterTemplate->m_stPreLight.alpha) * fOffset;*/
			fRed			= (FLOAT)(pstAgcdCharacterTemplate->m_pcsPreData->m_stPreLight.red) * fOffset;
			fBlue			= (FLOAT)(pstAgcdCharacterTemplate->m_pcsPreData->m_stPreLight.green) * fOffset;
			fGreen			= (FLOAT)(pstAgcdCharacterTemplate->m_pcsPreData->m_stPreLight.blue) * fOffset;
			fAlpha			= (FLOAT)(pstAgcdCharacterTemplate->m_pcsPreData->m_stPreLight.alpha) * fOffset;

			if (fRed > 255.0f)
				fRed = 255.0f;
			if (fBlue > 255.0f)
				fBlue = 255.0f;
			if (fGreen > 255.0f)
				fGreen = 255.0f;
			if (fAlpha > 255.0f)
				fAlpha = 255.0f;

			stRGBA.red		= (UINT8)(fRed);
			stRGBA.blue		= (UINT8)(fBlue);
			stRGBA.green	= (UINT8)(fGreen);
			stRGBA.alpha	= (UINT8)(fAlpha);

			AcuObject::SetClumpPreLitLim(pstAgcdCharacter->m_pClump, &stRGBA);
		}
	}

	return TRUE;
}

BOOL AgcmCharacter::AddReceivedAction(AgpdCharacter *pcsCharacter, AgcmCharacterActionQueueData *pstActionData)
{
	if (!pcsCharacter || !pstActionData)
		return FALSE;

	AgcdCharacter	*pstAgcdCharacter	= GetCharacterData(pcsCharacter);
	if (!pstAgcdCharacter)
		return FALSE;

	if (pstAgcdCharacter->m_lNumActionData == AGCD_CHARACTER_MAX_ACTION_QUEUE_SIZE)
		return FALSE;

	CopyMemory(&pstAgcdCharacter->m_astActionQueue[pstAgcdCharacter->m_lNumActionData],
			   pstActionData,
			   sizeof(AgcmCharacterActionQueueData));

	++pstAgcdCharacter->m_lNumActionData;

	if (pcsCharacter == m_pcsSelfCharacter && m_bShowActionQueue)
	{
		CHAR	szTemp[16];

		sprintf(szTemp, "%d(%d);", pstActionData->eActionType
								 , pstActionData->eActionResultType
			   );
		BuildActionQueue();
		m_szActionQueueAcc += szTemp;
	}

	return TRUE;
}

AgcmCharacterActionQueueData* AgcmCharacter::GetHeadActionData(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return NULL;

	AgcdCharacter	*pstAgcdCharacter	= GetCharacterData(pcsCharacter);
	if (!pstAgcdCharacter)
		return NULL;

	if (pstAgcdCharacter->m_lNumActionData <= 0)
		return NULL;

	return &pstAgcdCharacter->m_astActionQueue[0];
}

BOOL AgcmCharacter::RemoveHeadActionData(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgcdCharacter	*pstAgcdCharacter	= GetCharacterData(pcsCharacter);
	if (!pstAgcdCharacter)
		return FALSE;

	if (pstAgcdCharacter->m_lNumActionData <= 0)
		return FALSE;

	if (m_pcsAgpmFactors)
		m_pcsAgpmFactors->DestroyFactor(&pstAgcdCharacter->m_astActionQueue[0].csFactorDamage);

	CopyMemory(pstAgcdCharacter->m_astActionQueue,
			   pstAgcdCharacter->m_astActionQueue + 1,
			   sizeof(AgcmCharacterActionQueueData) * (pstAgcdCharacter->m_lNumActionData - 1));

	--pstAgcdCharacter->m_lNumActionData;

	return TRUE;
}

// ���� : �����㿡 ����ϰ� ������ Factor�� �ı��ؾ� �Ѵ�. �ȱ׷� ��� �޸� ���� �߻��Ѵ�.
//////////////////////////////////////////////////////////////////////////////////////
BOOL AgcmCharacter::GetReceivedAction(AgpdCharacter *pcsCharacter, AgpdCharacterActionType eType, INT32 lActorID, AgcmCharacterActionQueueData *pstReceiveBuffer, INT32 lSkillTID)
{
	if (!pcsCharacter || lActorID == AP_INVALID_CID || !pstReceiveBuffer)
		return FALSE;

	AgcdCharacter	*pstAgcdCharacter	= GetCharacterData(pcsCharacter);
	if (!pstAgcdCharacter)
		return FALSE;

	for (int i = 0; i < pstAgcdCharacter->m_lNumActionData; ++i)
	{
		if (pstAgcdCharacter->m_astActionQueue[i].eActionType == eType &&
			pstAgcdCharacter->m_astActionQueue[i].lActorID == lActorID)
		{
			// 2005.03.29. steeple
			// SkillTID �� �Ѿ�� �����, �ش� ��ų�� Queue �� �ִ� �� �ѹ� �� �˻��ؾ� �Ѵ�.
			if(lSkillTID != 0 && lSkillTID != pstAgcdCharacter->m_astActionQueue[i].lSkillTID)
				continue;

			CopyMemory(pstReceiveBuffer,
					   pstAgcdCharacter->m_astActionQueue + i,
					   sizeof(AgcmCharacterActionQueueData));

			CopyMemory(pstAgcdCharacter->m_astActionQueue + i,
					   pstAgcdCharacter->m_astActionQueue + i + 1,
					   sizeof(AgcmCharacterActionQueueData) * (pstAgcdCharacter->m_lNumActionData - i - 1));

			--pstAgcdCharacter->m_lNumActionData;

			// ������ �ڹٲ� ���... �տ� �ִ� �ѵ��� Factor ��Ŷ �� ������ ǥ���ϴ� �κи� ���� ����������.
			// HP�ٿ� ǥ�õǴ� ������ �ٲ�� �ȵǱ� �����̴�.
			for (int j = 0; j < i; ++j)
			{
				ClearActionFactor(pcsCharacter, &pstAgcdCharacter->m_astActionQueue[j], TRUE);	// ShowDamage TRUE. 2005.06.02. steeple
			}

			if (pcsCharacter == m_pcsSelfCharacter && m_bShowActionQueue)
				BuildActionQueue();

			return TRUE;
		}
	}

	if (pcsCharacter == m_pcsSelfCharacter)
		BuildActionQueue();

	return FALSE;
}

BOOL AgcmCharacter::ClearActionFactor(AgpdCharacter* pcsCharacter, AgcmCharacterActionQueueData *pcsActionData, BOOL bShowDamage)
{
	if (!pcsActionData)
		return FALSE;

	if (pcsActionData->bIsSetPacketFactor)
	{
		AgpdFactor	*pcsFactorDummy		= new AgpdFactor;
		AgpdFactor	*pcsFactorDamage	= new AgpdFactor;

		m_pcsAgpmFactors->InitFactor(pcsFactorDamage);

		//m_pcsAgpmFactors->ReflectPacket(pcsFactorDamage, (PVOID) pcsActionData->pszPacketFactor, 0);
		m_pcsAgpmFactors->ReflectPacket(pcsFactorDummy, (PVOID) pcsActionData->pszPacketFactor, 0, pcsFactorDamage);

		if (m_pcsAgpmFactors)
			m_pcsAgpmFactors->CopyFactor(&pcsActionData->csFactorDamage, pcsFactorDamage, TRUE, FALSE);

		delete pcsFactorDamage;
		delete pcsFactorDummy;

		// 2005.06.02. steeple.
		if(bShowDamage)
		{
			// 2005.05.25. steeple
			// ������ ���� �Ÿ��� �ִϸ��̼��� ������ �����ִ� �������� ������ ����.
			// Damage �� ���� ���� �����δ�.
			INT32 lDamage = 0;
			m_pcsAgpmFactors->GetValue(&pcsActionData->csFactorDamage, &lDamage, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_DMG_NORMAL);

			AgcdCharacter* pstAgcdCharacter = GetCharacterData(pcsCharacter);

			//. 2006. 3. 22. nonstopdj
			//. AGCMCHAR_ANIM_TYPE_WAIT���·� Ÿ���������� �и��� ���� ����:
			//. AnimType�� AGCMCHAR_ANIM_TYPE_RUN���¿��� Skill Struck�̸� Stuck�ִϸ��̼��� ĵ���ϰ�
			//. �ش� Effect�� �÷����Ѵ�.
			if(lDamage < 0 && 
				pstAgcdCharacter && 
				pstAgcdCharacter->m_eCurAnimType != AGCMCHAR_ANIM_TYPE_RUN)
			{
				pstAgcdCharacter->m_bForceAnimation = TRUE;
				StartAnimation(pcsCharacter, AGCMCHAR_ANIM_TYPE_STRUCK);
				pstAgcdCharacter->m_bForceAnimation = FALSE;
			}
			else
			{
				AgpdCharacter	*pcsAgpdAttacker	= m_pcsAgpmCharacter->GetCharacter(pstAgcdCharacter->m_lAttackerID);
				if (pcsAgpdAttacker)
					EnumCallback(AGCMCHAR_CB_ID_ATTACK_SUCESS, pcsAgpdAttacker, pcsCharacter);
			}

			// Damage �ѷ�����.
			EnumCallback(AGCMCHAR_CB_ID_DISPLAY_ACTION_RESULT, pcsCharacter, pcsActionData);
		}

		pcsActionData->bIsSetPacketFactor	= FALSE;

		// ���⼭�� ������ �ϳ�.... 2004.11.30. steeple
		if(pcsActionData->pszPacketFactor)
		{
			//TRACEFILE2("steeple_memory.log", "AgcmCharacter.cpp::ClearActionFactor, pszPacketFactor(0x%08x) delete", pcsActionData->pszPacketFactor);
			delete pcsActionData->pszPacketFactor;
			pcsActionData->pszPacketFactor = NULL;
		}
	}

	return TRUE;
}

AgpdCharacterActionResultType AgcmCharacter::GetNextReceivedActionResult(AgpdCharacter *pcsActor, AgpdCharacter *pcsTarget, AgpdCharacterActionType eActionType)
{
	if (!pcsActor || !pcsTarget)
		return AGPDCHAR_ACTION_RESULT_TYPE_NONE;

	AgcdCharacter	*pstAgcdCharacter	= GetCharacterData(pcsTarget);
	if (!pstAgcdCharacter)
		return AGPDCHAR_ACTION_RESULT_TYPE_NONE;

	for (int i = 0; i < pstAgcdCharacter->m_lNumActionData; ++i)
	{
		if (pstAgcdCharacter->m_astActionQueue[i].eActionType == eActionType &&
			pstAgcdCharacter->m_astActionQueue[i].lActorID == pcsActor->m_lID)
		{
			return pstAgcdCharacter->m_astActionQueue[i].eActionResultType;
		}
	}

	return AGPDCHAR_ACTION_RESULT_TYPE_NONE;
}

AgpdFactor* AgcmCharacter::GetNextReceivedActionDamageFactor(AgpdCharacter *pcsActor, AgpdCharacter *pcsTarget, AgpdCharacterActionType eActionType)
{
	if (!pcsActor || !pcsTarget)
		return NULL;

	AgcdCharacter	*pstAgcdCharacter	= GetCharacterData(pcsTarget);
	if (!pstAgcdCharacter)
		return NULL;

	for (int i = 0; i < pstAgcdCharacter->m_lNumActionData; ++i)
	{
		if (pstAgcdCharacter->m_astActionQueue[i].eActionType == eActionType &&
			pstAgcdCharacter->m_astActionQueue[i].lActorID == pcsActor->m_lID)
		{
			if (pstAgcdCharacter->m_astActionQueue[i].bIsSetPacketFactor)
			{
				AgpdFactor	*pcsFactorDamage	= new AgpdFactor;

				m_pcsAgpmFactors->InitFactor(pcsFactorDamage);

				m_pcsAgpmCharacter->ReflectFactorPacket(pcsTarget, (PVOID) pstAgcdCharacter->m_astActionQueue[i].pszPacketFactor, pcsFactorDamage);

				m_pcsAgpmCharacter->EnumCallback(AGPMCHAR_CB_ID_UPDATE_FACTOR, pcsTarget, NULL);

				if (m_pcsAgpmFactors)
					m_pcsAgpmFactors->CopyFactor(&pstAgcdCharacter->m_astActionQueue[i].csFactorDamage, pcsFactorDamage, TRUE, FALSE);

				delete pcsFactorDamage;
			}

			return &pstAgcdCharacter->m_astActionQueue[i].csFactorDamage;
		}
	}

	return NULL;
}

AgcmCharacterActionQueueData* AgcmCharacter::GetSequenceActionData(AgpdCharacter *pcsCharacter, INT32 *plIndex)
{
	if (!pcsCharacter || !plIndex)
		return FALSE;

	AgcdCharacter	*pstAgcdCharacter	= GetCharacterData(pcsCharacter);
	if (!pstAgcdCharacter)
		return FALSE;

	if (*plIndex < 0 || *plIndex >= pstAgcdCharacter->m_lNumActionData)
		return NULL;

	++(*plIndex);

	return &pstAgcdCharacter->m_astActionQueue[*plIndex - 1];
}

BOOL AgcmCharacter::RemoveActionData(AgpdCharacter *pcsCharacter, INT32 lIndex)
{
	if (!pcsCharacter)
		return FALSE;

	AgcdCharacter	*pstAgcdCharacter	= GetCharacterData(pcsCharacter);
	if (!pstAgcdCharacter)
		return FALSE;

	if (pstAgcdCharacter->m_lNumActionData <= 0 || lIndex < 0 || lIndex >= pstAgcdCharacter->m_lNumActionData)
		return FALSE;

	if (m_pcsAgpmFactors)
		m_pcsAgpmFactors->DestroyFactor(&pstAgcdCharacter->m_astActionQueue[lIndex].csFactorDamage);

	// 2004.11.30. steeple
	if (pstAgcdCharacter->m_astActionQueue[lIndex].bIsSetPacketFactor)
	{
		if(pstAgcdCharacter->m_astActionQueue[lIndex].pszPacketFactor)
		{
			//TRACEFILE2("steeple_memory.log", "AgcmCharacter.cpp::RemoveActionData, pszPacketFactor(0x%08x) delete", pstAgcdCharacter->m_astActionQueue[lIndex].pszPacketFactor);
			delete pstAgcdCharacter->m_astActionQueue[lIndex].pszPacketFactor;
			pstAgcdCharacter->m_astActionQueue[lIndex].pszPacketFactor = NULL;
		}
	}

	CopyMemory(pstAgcdCharacter->m_astActionQueue + lIndex,
			   pstAgcdCharacter->m_astActionQueue + lIndex + 1,
			   sizeof(AgcmCharacterActionQueueData) * (pstAgcdCharacter->m_lNumActionData - lIndex - 1));

	--pstAgcdCharacter->m_lNumActionData;

	return TRUE;
}

BOOL AgcmCharacter::ApplyActionData(AgpdCharacter *pcsCharacter, AgcmCharacterActionQueueData *pcsActionData)
{
	if (!pcsCharacter || !pcsActionData)
		return FALSE;

	if (pcsActionData->bIsSetPacketFactor)
	{
		AgpdFactor	*pcsFactorDamage	= new AgpdFactor;

		m_pcsAgpmFactors->InitFactor(pcsFactorDamage);

		m_pcsAgpmCharacter->ReflectFactorPacket(pcsCharacter, (PVOID) pcsActionData->pszPacketFactor, pcsFactorDamage);

		m_pcsAgpmCharacter->EnumCallback(AGPMCHAR_CB_ID_UPDATE_FACTOR, pcsCharacter, NULL);

		if (m_pcsAgpmFactors)
			m_pcsAgpmFactors->CopyFactor(&pcsActionData->csFactorDamage, pcsFactorDamage, TRUE, FALSE);

		delete pcsFactorDamage;
	}

	if (pcsActionData->lNumDivideForShow > 1 &&
		pcsActionData->ulDivideIntervalMSec > 0)
	{
		for (int i = 0; i < pcsActionData->lNumDivideForShow - 1; ++i)
		{
			AgcmCharacterActionQueueData	stActionData;
			ZeroMemory(&stActionData, sizeof(stActionData));

			stActionData.eActionType		= pcsActionData->eActionType;
			stActionData.eActionResultType	= pcsActionData->eActionResultType;
			stActionData.lActorID			= pcsActionData->lActorID;
			stActionData.lSkillTID			= pcsActionData->lSkillTID;
			if (m_pcsAgpmFactors)
				m_pcsAgpmFactors->CopyFactor(&stActionData.csFactorDamage, &pcsActionData->csFactorDamage, TRUE, FALSE);
			stActionData.ulProcessTime		= m_ulCurTick + pcsActionData->ulDivideIntervalMSec * (i + 1);
			stActionData.bDeath				= FALSE;
			stActionData.ulAdditionalEffect = pcsActionData->ulAdditionalEffect;
			//stActionData.cHitIndex			= pcsActionData->cHitIndex; //#2#

			BOOL	bAddResult	= AddReceivedAction(pcsCharacter, &stActionData);

			//AddIdleEvent(m_ulCurTick + pcsActionData->ulDivideIntervalMSec * i, pcsCharacter->m_lID, this, ProcessStruckIdleEvent, (PVOID) pcsActionData->lActorID);
		}
	}

	// 2005.05.25. steeple
	// ��ų ������ ���� �Ÿ��� �ִϸ��̼��� ������ �����ִ� �������� ������ ����.
	if(pcsActionData->eActionType == AGPDCHAR_ACTION_TYPE_SKILL)
	{
		// Damage �� ���� ���� �����δ�.
		INT32 lDamage = 0;
		m_pcsAgpmFactors->GetValue(&pcsActionData->csFactorDamage, &lDamage, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_DMG_NORMAL);

		AgcdCharacter* pstAgcdCharacter = GetCharacterData(pcsCharacter);

		//. 2006. 3. 22. nonstopdj
		//. AGCMCHAR_ANIM_TYPE_WAIT���·� Ÿ���������� �и��� ���� ����:
		//. AnimType�� AGCMCHAR_ANIM_TYPE_RUN���¿��� Skill Struck�̸� Stuck�ִϸ��̼��� ĵ���ϰ�
		//. �ش� Effect�� �÷����Ѵ�.
		if(lDamage < 0 && 
			pstAgcdCharacter && 
			pstAgcdCharacter->m_eCurAnimType != AGCMCHAR_ANIM_TYPE_RUN)
		{
			pstAgcdCharacter->m_bForceAnimation = TRUE;
			StartAnimation(pcsCharacter, AGCMCHAR_ANIM_TYPE_STRUCK);
			pstAgcdCharacter->m_bForceAnimation = FALSE;
		}
		else
		{
			AgpdCharacter	*pcsAgpdAttacker	= m_pcsAgpmCharacter->GetCharacter(pstAgcdCharacter->m_lAttackerID);
			if (pcsAgpdAttacker)
				EnumCallback(AGCMCHAR_CB_ID_ATTACK_SUCESS, pcsAgpdAttacker, pcsCharacter);
		}

	}

	// pcsActionData�� ���õǾ� �ִ� Damage�� �����ش�.
	BOOL bResult = EnumCallback(AGCMCHAR_CB_ID_DISPLAY_ACTION_RESULT, pcsCharacter, pcsActionData);

	// 2005.03. 14. steeple
	if(pcsActionData->bIsNowUpdate)
	{
		m_pcsAgpmCharacter->EnumCallback(AGPMCHAR_CB_ID_UPDATE_FACTOR, pcsCharacter, NULL);
	}

	return bResult;
}

BOOL AgcmCharacter::ProcessActionQueue(AgpdCharacter *pcsActor, AgpdCharacter *pcsTarget, AgpdCharacterActionType eActionType, INT32 lSkillTID)
{
	if (!pcsActor || !pcsTarget)
		return FALSE;

	AgcmCharacterActionQueueData	stReceiveBuffer;
	ZeroMemory(&stReceiveBuffer, sizeof(AgcmCharacterActionQueueData));

	BOOL	bRetval	= FALSE;

	BOOL	bGetReceivedAction	= GetReceivedAction(pcsTarget, eActionType, pcsActor->m_lID, &stReceiveBuffer, lSkillTID);

	while (bGetReceivedAction)
	{
		bRetval	= TRUE;

		ApplyActionData(pcsTarget, &stReceiveBuffer);

		if (m_pcsAgpmFactors)
			m_pcsAgpmFactors->DestroyFactor(&stReceiveBuffer.csFactorDamage);

		ZeroMemory(&stReceiveBuffer, sizeof(AgcmCharacterActionQueueData));

		bGetReceivedAction	= GetReceivedAction(pcsTarget, eActionType, pcsActor->m_lID, &stReceiveBuffer, lSkillTID);
	}

	return bRetval;
}

BOOL AgcmCharacter::IsReceivedDeadAction(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	INT32	lIndex	= 0;

	AgcmCharacterActionQueueData	*pcsActionData	= GetSequenceActionData(pcsCharacter, &lIndex);
	while (pcsActionData)
	{
		if (pcsActionData->bDeath)
			return TRUE;

		pcsActionData	= GetSequenceActionData(pcsCharacter, &lIndex);
	}	

	return FALSE;
}

BOOL AgcmCharacter::ProcessStruckIdleEvent(INT32 lCID, PVOID pClass, UINT32 ulClockCount, PVOID pvData)
{
	if (!pClass || !pvData)
		return FALSE;

	AgcmCharacter	*pThis				= (AgcmCharacter *)	pClass;

	AgpdCharacter	*pcsCharacter		= pThis->m_pcsAgpmCharacter->GetCharacter(lCID);
	if (!pcsCharacter)
		return TRUE;

	INT32			lAttackerID			= (INT32)	pvData;

	return pThis->ProcessActionQueue(pThis->m_pcsAgpmCharacter->GetCharacter(lAttackerID), pcsCharacter, AGPDCHAR_ACTION_TYPE_ATTACK);
}

BOOL AgcmCharacter::ProcessSkillStruckIdleEvent(INT32 lCID, PVOID pClass, UINT32 ulClockCount, PVOID pvData)
{
	if (!pClass || !pvData)
		return FALSE;

	AgcmCharacter	*pThis				= (AgcmCharacter *)	pClass;

	AgpdCharacter	*pcsCharacter		= pThis->m_pcsAgpmCharacter->GetCharacter(lCID);
	if (!pcsCharacter)
		return TRUE;

	INT32			lAttackerID			= (INT32)	pvData;

	AgpdCharacter	*pcsAttacker		= pThis->m_pcsAgpmCharacter->GetCharacter(lAttackerID);

	if (!pThis->ProcessActionQueue(pcsAttacker, pcsCharacter, AGPDCHAR_ACTION_TYPE_SKILL))
	{
		return pThis->EnumCallback(AGCMCHAR_CB_ID_SKILL_STRUCK_CHARACTER, pcsAttacker, pcsCharacter);
	}
	else
	{
		return pThis->EnumCallback(AGCMCHAR_CB_ID_PROCESS_SKILL_STRUCK_CHARACTER, pcsAttacker, pcsCharacter);
	}

	return TRUE;
}

BOOL AgcmCharacter::CBSocketOnConnect( PVOID pData, PVOID pClass, PVOID pCustData)
{
#ifdef	__ATTACH_LOGINSERVER__

	if (NULL == pClass) return FALSE;

	AgcmCharacter *pThis = (AgcmCharacter*)pClass;
	AcClientSocket *pSocket = (AcClientSocket*)pData;

	// 2006.04.14. steeple
	// �׳� �ݹ� �θ��� ��
	INT32 lNID = pSocket->GetIndex();
	pThis->EnumCallback(AGCMCHAR_CB_ID_SOCKET_ON_CONNECT, NULL, &lNID);
#endif // __ATTACH_LOGINSERVER__

	return TRUE;
}

BOOL AgcmCharacter::CBSocketOnDisConnect( PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmCharacter* pThis = (AgcmCharacter*)pClass;

	if ( pThis != NULL )
		pThis->EnumCallback( AGCMCHAR_CB_ID_GAMESERVER_DISCONNECT , NULL, pCustData );

	return TRUE;

}

BOOL AgcmCharacter::CBSocketOnError( PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmCharacter* pThis = (AgcmCharacter*)pClass;
	AcClientSocket* pSocket = (AcClientSocket*)pData	;

	if ( pSocket ) pSocket->Close();
	
	if ( pThis != NULL )
	{
		pThis->EnumCallback( AGCMCHAR_CB_ID_GAMESERVER_ERROR, NULL, pCustData );

		BOOL	bIsDestroyNormal	= FALSE;

		pThis->EnumCallback( AGCMCHAR_CB_ID_GAMESERVER_DISCONNECT , NULL, (PVOID) &bIsDestroyNormal );
	}

	return TRUE;
}

BOOL AgcmCharacter::CBInitCharacterBackThread( PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmCharacter *			pThis = (AgcmCharacter *) pClass;
	AgpdCharacter *			pcsCharacter = (AgpdCharacter *) pData;

	if (!pcsCharacter)
		return TRUE;

//	if (!pcsCharacter->m_Mutex.WLock())
//		return TRUE;

	if (!pThis->InitCharacterBackThread(pcsCharacter))
	{
//		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}
	
//	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgcmCharacter::CBInitCharacterMainThread( PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgcmCharacter::CBInitCharacterLoader2");

	AgcmCharacter *			pThis = (AgcmCharacter *) pClass;
	AgpdCharacter *			pcsCharacter = (AgpdCharacter *) pData;

	if (!pcsCharacter)
		return TRUE;

//	if (!pcsCharacter->m_Mutex.WLock())
//		return TRUE;

	if (!pThis->InitCharacterMainThread(pcsCharacter))
	{
//		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	pThis->CBUpdatePosition( pcsCharacter, pThis, &pcsCharacter->m_stPos );
	
//	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgcmCharacter::CBRemoveCharacterLoader( PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgcmCharacter::CBRemoveCharacterLoader");

	AgcmCharacter *			pThis = (AgcmCharacter *) pClass;
	AgpdCharacter *			pcsCharacter = (AgpdCharacter *) pData;

	if (!pThis->RemoveCharacter(pcsCharacter))
	{
		return FALSE;
	}

	return TRUE;
}

/*****************************************************************
*   Function : ReleaseSelfCharacter
*   Comment  : Selt Character �� ������� - Self Character ������ �ʱ�ȭ �Ѵ� 
*   Date&Time : 2004-01-07, ���� 12:40
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
void	AgcmCharacter::ReleaseSelfCharacter()
{
	// Self Character�� �������� Render Module�� ������ 
	if ( m_pcsAgcmRender && m_pCamera )
		m_pcsAgcmRender->SetMainFrame( RwCameraGetFrame( m_pCamera ) );
		
	AgpdCharacter* pcsSelfCharacter = m_pcsSelfCharacter;
	AgcdCharacter* pstAgcdCharacter = GetCharacterData(pcsSelfCharacter);

	m_lSelfCID	=	AP_INVALID_CID		;
	m_pcsSelfCharacter	=	NULL		;

	EnumCallback(AGCMCHAR_CB_ID_RELEASE_SELF_CHARACTER, pcsSelfCharacter, pstAgcdCharacter);

#ifndef USE_MFC
	// ���� ���忡���� �̰� �ϸ� �ƴϵǿ�.
	this->m_pcsAgcmMap->m_pSelfCharacterSector = NULL;
	this->m_pcsAgcmMap->ClearAllSectors();
#endif
}

BOOL AgcmCharacter::SetTemplateClumpRenderType(RpClump *pstClump, AgcdClumpRenderType *pcsRenderType)
{
	if (pcsRenderType->m_lSetCount > 0)
	{
		UtilSetUDAInt32Params	csParams;
		csParams.m_lCBCount		= 0;
		strcpy(csParams.m_szUDAName, AGCMLOD_ATOMIC_INDEX);
		RpClumpForAllAtomics(pstClump, UtilSetUDAInt32CB, (void *)(&csParams));

		//@{ 2006/11/14 burumal
		if ( csParams.m_nValidCount == 0 )
		{
			csParams.Clear();
			strcpy(csParams.m_szUDAName, AGCMLOD_ATOMIC_INDEX_EXTRA);
			RpClumpForAllAtomics(pstClump, UtilSetUDAInt32CB, (void *)(&csParams));
		}
		//@}

		m_pcsAgcmRender->ClumpSetRenderTypeCheckCustData(pstClump, pcsRenderType, csParams.m_alUDAInt32);
	}

	return TRUE;
}

BOOL AgcmCharacter::LoadTemplateClump(AgpdCharacterTemplate *pstAgpdCharacterTemplate, AgcdCharacterTemplate *pstAgcdCharacterTemplate)
{
	UINT32	ulIndex					= 0;
	CHAR	*pszDefaultClumpName	= NULL;
	CHAR	*pszDefaultArmourName	= NULL;
	CHAR	*pszPickingDataName		= NULL;

	if (pstAgcdCharacterTemplate->m_pcsPreData)
	{
		if (	(pstAgcdCharacterTemplate->m_pcsPreData->m_pszDFFName) &&
				(!pstAgcdCharacterTemplate->m_pClump)						)
		{
			pszDefaultClumpName	= new CHAR [strlen(pstAgcdCharacterTemplate->m_pcsPreData->m_pszDFFName) + 1];
			strcpy(pszDefaultClumpName, pstAgcdCharacterTemplate->m_pcsPreData->m_pszDFFName);
		}

		if (	(pstAgcdCharacterTemplate->m_pcsPreData->m_pszDADFFName) &&
				(!pstAgcdCharacterTemplate->m_pDefaultArmourClump)			)
		{
			pszDefaultArmourName	= new CHAR [strlen(pstAgcdCharacterTemplate->m_pcsPreData->m_pszDADFFName) + 1];
			strcpy(pszDefaultArmourName, pstAgcdCharacterTemplate->m_pcsPreData->m_pszDADFFName);
		}

		if (	(pstAgcdCharacterTemplate->m_pcsPreData->m_pszPickDFFName) &&
				(!pstAgcdCharacterTemplate->m_pPickingAtomic)					)
		{
			pszPickingDataName	= new CHAR [strlen(pstAgcdCharacterTemplate->m_pcsPreData->m_pszPickDFFName) + 1];
			strcpy(pszPickingDataName, pstAgcdCharacterTemplate->m_pcsPreData->m_pszPickDFFName);
		}
	}
	else
	{
		CHAR	szTemp[256];
		memset(szTemp, 0, sizeof (CHAR) * 256);

		if (!pstAgcdCharacterTemplate->m_pClump)
		{
			if (pstAgcdCharacterTemplate->GetClumpID() > 0)
			{
				pstAgcdCharacterTemplate->GetClumpName(szTemp);

				pszDefaultClumpName	= new CHAR [strlen(szTemp) + 1];
				strcpy(pszDefaultClumpName, szTemp);
			}
		}

		if (!pstAgcdCharacterTemplate->m_pDefaultArmourClump)
		{
			if (pstAgcdCharacterTemplate->GetDefArmourID() > 0)
			{
				pstAgcdCharacterTemplate->GetDefArmourName(szTemp);

				pszDefaultArmourName	= new CHAR [strlen(szTemp) + 1];
				strcpy(pszDefaultArmourName, szTemp);
			}
		}

		if (!pstAgcdCharacterTemplate->m_pPickingAtomic)
		{
			if (pstAgcdCharacterTemplate->GetPickClumpID() > 0)
			{
				pstAgcdCharacterTemplate->GetPickClumpName(szTemp);

				pszPickingDataName	= new CHAR [strlen(szTemp) + 1];
				strcpy(pszPickingDataName, szTemp);
			}
		}
	}

	if (pszDefaultClumpName)
	{
		pstAgcdCharacterTemplate->m_pClump	= LoadClump(pszDefaultClumpName);
		if( pstAgcdCharacterTemplate->m_pClump )
		{
			EnumCallback(AGCMCHAR_CB_ID_LOAD_CLUMP, pstAgpdCharacterTemplate, m_szClumpPath);
			SetTemplateClumpRenderType(pstAgcdCharacterTemplate->m_pClump, &pstAgcdCharacterTemplate->m_csClumpRenderType);
		}
	}

	if (pszDefaultArmourName)
	{
		pstAgcdCharacterTemplate->m_pDefaultArmourClump	= LoadClump(pszDefaultArmourName);
		if( pstAgcdCharacterTemplate->m_pDefaultArmourClump )
		{
			RpClumpSetName(pstAgcdCharacterTemplate->m_pDefaultArmourClump, pstAgpdCharacterTemplate->m_szTName);
			EnumCallback(AGCMCHAR_CB_ID_LOAD_DEFAULT_ARMOUR_CLUMP, pstAgpdCharacterTemplate, m_szClumpPath);
			SetTemplateClumpRenderType(pstAgcdCharacterTemplate->m_pDefaultArmourClump, &pstAgcdCharacterTemplate->m_csClumpRenderType);
		}
	}

	if (pszPickingDataName)
	{
		pstAgcdCharacterTemplate->m_pPickingAtomic	= LoadAtomic(pszPickingDataName);
	}

	//add by dobal
	//@{ Jaewon 20050601
	// ;)
	ASSERT(pstAgcdCharacterTemplate->m_pClump);
	if(pstAgcdCharacterTemplate->m_pClump == NULL)
		MD_SetErrorMessage("[%d] %d (pstAgcdCharacterTemplate->m_pClump == NULL) : %d, %s\n", GetCurrentThreadId(), __LINE__, pstAgpdCharacterTemplate->m_lID, pszDefaultClumpName);
	//@} Jaewon
	//@{ Jaewon 20050726
	// If the clump is NULL, skip additional loadings(for exporting).
	else
	//@} Jaewon
	{
		LoadTemplateDefaultFace( pstAgcdCharacterTemplate );
		LoadTemplateDefaultHair( pstAgcdCharacterTemplate );
	}

	if (pszDefaultClumpName)
		delete [] pszDefaultClumpName;
	if (pszDefaultArmourName)
		delete [] pszDefaultArmourName;
	if (pszPickingDataName)
		delete [] pszPickingDataName;

	return TRUE;
}

BOOL AgcmCharacter::LoadTemplateAnimation(AgpdCharacterTemplate *pstAgpdCharacterTemplate, AgcdCharacterTemplate *pstAgcdCharacterTemplate)
{
	ASSERT( NULL != pstAgpdCharacterTemplate );
	// ������ (2005-05-02 ���� 11:32:22) : 
	// ���̸��̼� ����Ÿ�� ���� ��찡 �ִ�?...
	ASSERT( NULL != pstAgcdCharacterTemplate->m_pacsAnimationData );
	if( NULL == pstAgcdCharacterTemplate->m_pacsAnimationData ) return FALSE;

	INT32				lNumAnimType2		= GetAnimType2Num(pstAgcdCharacterTemplate);

	AgcdAnimData2		*pcsCurrent			= NULL;
	ACA_AttachedData	*pcsAttachedData	= NULL;
	INT32				lStartAnimType		= 0;
	INT32				lEndAnimType		= AGCMCHAR_MAX_ANIM_TYPE;
	INT32				lStartAnimType2		= 0;
	INT32				lEndAnimType2		= lNumAnimType2;
	if (m_eReadType == AGCMCHAR_READ_TYPE_WAIT_ANIM_ONLY)
	{
		lStartAnimType	= AGCMCHAR_ANIM_TYPE_WAIT;
		lEndAnimType	= AGCMCHAR_ANIM_TYPE_WAIT + 1;

		lStartAnimType2		= 0;
		lEndAnimType2		= 1;
	}

	for (INT32 lAnimType = lStartAnimType; lAnimType < lEndAnimType; ++lAnimType)
	{
		for (INT32 lAnimType2 = lStartAnimType2; lAnimType2 < lEndAnimType2; ++ lAnimType2)
		{
			if (	(!pstAgcdCharacterTemplate->m_pacsAnimationData[lAnimType][lAnimType2]) ||
					(!pstAgcdCharacterTemplate->m_pacsAnimationData[lAnimType][lAnimType2]->m_pcsAnimation)	)
				continue;

			if (!m_csAnimation2.ReadRtAnim( pstAgcdCharacterTemplate->m_pacsAnimationData[lAnimType][lAnimType2]->m_pcsAnimation )) continue;

			if (m_eReadType == AGCMCHAR_READ_TYPE_ALL)
			{
				pcsCurrent	= pstAgcdCharacterTemplate->m_pacsAnimationData[lAnimType][lAnimType2]->m_pcsAnimation->m_pcsHead;
				while (pcsCurrent)
				{
					pcsAttachedData	=
						(ACA_AttachedData *)(m_csAnimation2.GetAttachedData(
							AGCD_CHARACTER_ANIMATION_ATTACHED_DATA_KEY,
							pcsCurrent										));
					if (!pcsAttachedData)
					{
						ASSERT(!"!pcsAttachedData");
					}
					else
					{
						if (pcsAttachedData->m_pcsBlendingData)
						{
							if (!m_csAnimation2.ReadRtAnim(pcsAttachedData->m_pcsBlendingData))
							{
								pcsCurrent	= pcsCurrent->m_pcsNext;
								continue;
							}
						}

						if (pcsAttachedData->m_pcsSubData)
						{
							if (!m_csAnimation2.ReadRtAnim(pcsAttachedData->m_pcsSubData))
							{
								pcsCurrent	= pcsCurrent->m_pcsNext;
								continue;
							}
						}
					}

					pcsCurrent	= pcsCurrent->m_pcsNext;
				}
			}
		}
	}

	return TRUE;
}

BOOL AgcmCharacter::LoadTemplateData(AgpdCharacterTemplate *pstAgpdCharacterTemplate, AgcdCharacterTemplate *pstAgcdCharacterTemplate)
{
	ASSERT(pstAgcdCharacterTemplate->m_lRefCount >= 0);
	
	if (pstAgcdCharacterTemplate->m_lRefCount >= 1 && !pstAgcdCharacterTemplate->m_bLoaded)
	{
		if (!LoadTemplateClump(pstAgpdCharacterTemplate, pstAgcdCharacterTemplate))
			return FALSE;

		if (!LoadTemplateAnimation(pstAgpdCharacterTemplate, pstAgcdCharacterTemplate))
			return FALSE;

		EnumCallback(
			AGCMCHAR_CB_ID_LOAD_TEMPLATE,
			(PVOID)(pstAgpdCharacterTemplate),
			(PVOID)(pstAgcdCharacterTemplate) );

		pstAgcdCharacterTemplate->m_bLoaded = TRUE;
	}

	return TRUE;
}

BOOL AgcmCharacter::ReleaseTemplateDefaultFace( AgcdCharacterTemplate *pstTemplate )
{
	RpAtomic*	pstAtomic;
	INT32 nFaceNum = (INT32)pstTemplate->m_vpFace.size();
	for( INT32 i = 0; i < nFaceNum; i++ )
	{
		pstAtomic = pstTemplate->m_vpFace[i];
		if( pstAtomic ) {
			if (m_pcsAgcmResourceLoader)
				m_pcsAgcmResourceLoader->AddDestroyAtomic(pstAtomic);
			else
				RpAtomicDestroy(pstAtomic);
		}
	}
	pstTemplate->m_vpFace.clear();

	return TRUE;
}

BOOL AgcmCharacter::ReleaseTemplateDefaultHair( AgcdCharacterTemplate *pstTemplate )
{
	RpAtomic*	pstAtomic;
	INT32 nHairNum = (INT32)pstTemplate->m_vpHair.size();
	for( INT32 i = 0; i < nHairNum; i++ )
	{
		pstAtomic = pstTemplate->m_vpHair[i];
		if( pstAtomic ) {
			if (m_pcsAgcmResourceLoader)
				m_pcsAgcmResourceLoader->AddDestroyAtomic(pstAtomic);
			else
				RpAtomicDestroy(pstAtomic);
		}
	}
	pstTemplate->m_vpHair.clear();

	return TRUE;
}

BOOL AgcmCharacter::ReleaseTemplateClump(AgcdCharacterTemplate *pstTemplate)
{
	RpClump *	pstClump;
	RpAtomic *	pstAtomic;

	pstClump = pstTemplate->m_pClump;
	if (pstClump)
	{
		pstTemplate->m_pClump = NULL;

		LockFrame();
		if (RwFrameGetParent(RpClumpGetFrame(pstClump)))
			RwFrameRemoveChild(RpClumpGetFrame(pstClump));
		UnlockFrame();

		// TemplateData�� Fade In/Out �� ���� ������ �׳� Destroy
		if (m_pcsAgcmResourceLoader)
			m_pcsAgcmResourceLoader->AddDestroyClump(pstClump);
		else
			RpClumpDestroy(pstClump);
	}

	pstClump = pstTemplate->m_pDefaultArmourClump;
	if (pstClump)
	{
		pstTemplate->m_pDefaultArmourClump = NULL;

		LockFrame();
		if (RwFrameGetParent(RpClumpGetFrame(pstClump)))
			RwFrameRemoveChild(RpClumpGetFrame(pstClump));
		UnlockFrame();

		// TemplateData�� Fade In/Out �� ���� ������ �׳� Destroy
		if (m_pcsAgcmResourceLoader)
			m_pcsAgcmResourceLoader->AddDestroyClump(pstClump);
		else
			RpClumpDestroy(pstClump);
	}


	//@{ 2006/02/15 burumal

//	pstAtomic = pstTemplate->m_pPickingAtomic;
//	if (pstAtomic)
//	{
//		pstTemplate->m_pPickingAtomic = NULL;
//
//		if (m_pcsAgcmResourceLoader)
//			m_pcsAgcmResourceLoader->AddDestroyAtomic(pstAtomic);
//		else
//			RpAtomicDestroy(pstAtomic);
//	}	
	
	if ( !IsBadReadPtr( pstTemplate->m_pPickingAtomic, sizeof(RpAtomic)) )
	{
		if ( pstTemplate->m_pPickingAtomic == pstTemplate->m_pPickingAtomic->next )
		{
			pstAtomic = pstTemplate->m_pPickingAtomic;
			if (pstAtomic)
			{
				pstTemplate->m_pPickingAtomic = NULL;

				if (m_pcsAgcmResourceLoader)
					m_pcsAgcmResourceLoader->AddDestroyAtomic(pstAtomic);
				else
					RpAtomicDestroy(pstAtomic);
			}
		}
		else
		{			
			RpAtomic* pFirst = pstTemplate->m_pPickingAtomic;
			RpAtomic* pTempCur = pFirst;

			while ( pTempCur )
			{
				RpAtomic* pDel = pTempCur;

				if ( pTempCur->next == pFirst )
					pTempCur = NULL;
				else
					pTempCur = pTempCur->next;

				if ( m_pcsAgcmResourceLoader )
					m_pcsAgcmResourceLoader->AddDestroyAtomic(pDel);
				else
					RpAtomicDestroy(pDel);
			}

			pstTemplate->m_pPickingAtomic = NULL;
		}
	}
	//@}

	ReleaseTemplateDefaultFace( pstTemplate );
	ReleaseTemplateDefaultHair( pstTemplate );

	return TRUE;
}

BOOL AgcmCharacter::ReleaseTemplateAnimation(AgcdCharacterTemplate *pstTemplate)
{
	INT32				lNumAnimType2		= GetAnimType2Num(pstTemplate);

	AgcdAnimData2		*pcsCurrent			= NULL;
	ACA_AttachedData	*pcsAttachedData	= NULL;
	for (INT32 lAnimType = 0; lAnimType < AGCMCHAR_MAX_ANIM_TYPE; ++lAnimType)
	{
		for (INT32 lAnimType2 = 0; lAnimType2 < lNumAnimType2; ++lAnimType2)
		{
			if (	(!pstTemplate->m_pacsAnimationData[lAnimType][lAnimType2]) ||
					(!pstTemplate->m_pacsAnimationData[lAnimType][lAnimType2]->m_pcsAnimation)	)
				continue;

			if (!m_csAnimation2.RemoveRtAnim(pstTemplate->m_pacsAnimationData[lAnimType][lAnimType2]->m_pcsAnimation))
			{
				ASSERT(!"!m_csAnimation2.RemoveRtAnim()");
				return FALSE;
			}

			pcsCurrent	= pstTemplate->m_pacsAnimationData[lAnimType][lAnimType2]->m_pcsAnimation->m_pcsHead;
			while (pcsCurrent)
			{
				pcsAttachedData	=
					(ACA_AttachedData *)(m_csAnimation2.GetAttachedData(
						AGCD_CHARACTER_ANIMATION_ATTACHED_DATA_KEY,
						pcsCurrent										));
				if (!pcsAttachedData)
				{
					ASSERT(!"!pcsAttachedData");
					return FALSE;
				}

				if (pcsAttachedData->m_pcsBlendingData)
				{
					m_csAnimation2.RemoveRtAnim(pcsAttachedData->m_pcsBlendingData);
				}

				if (pcsAttachedData->m_pcsSubData)
				{
					m_csAnimation2.RemoveRtAnim(pcsAttachedData->m_pcsSubData);
				}

				pcsCurrent	= pcsCurrent->m_pcsNext;
			}
		}
	}

	return TRUE;
}

BOOL AgcmCharacter::ReleaseTemplateData(AgcdCharacterTemplate *pstTemplate)
{
	if (!pstTemplate)
		return FALSE;
	
	--(pstTemplate->m_lRefCount);

	ASSERT(pstTemplate->m_lRefCount >= 0);

	if (pstTemplate->m_lRefCount < 1 && pstTemplate->m_bLoaded)
	{
		if (!ReleaseTemplateClump(pstTemplate))
			return FALSE;

		if (!ReleaseTemplateAnimation(pstTemplate))
			return FALSE;

		EnumCallback(
			AGCMCHAR_CB_ID_RELEASE_TEMPLATE,
			(PVOID)(GetTemplate(pstTemplate)),
			(PVOID)(pstTemplate)				);

		pstTemplate->m_bLoaded = FALSE;
	}
	/*
	if (pstTemplate->m_lRefCount < 1 && pstTemplate->m_bLoaded)
	{
		BOOL bPreSetting = this->m_pcsAgcmResourceLoader->m_bForceImmediate;
		this->m_pcsAgcmResourceLoader->m_bForceImmediate = TRUE;

		try
		{
			if (!ReleaseTemplateClump(pstTemplate))
			{
				throw;
			}

			if (!ReleaseTemplateAnimation(pstTemplate))
			{
				throw;
			}

			EnumCallback(
				AGCMCHAR_CB_ID_RELEASE_TEMPLATE,
				(PVOID)(GetTemplate(pstTemplate)),
				(PVOID)(pstTemplate)				);

			this->m_pcsAgcmResourceLoader->m_bForceImmediate = FALSE;
		}
		catch( ... )
		{
			this->m_pcsAgcmResourceLoader->m_bForceImmediate = bPreSetting;
			return FALSE;
		}

		pstTemplate->m_bLoaded = FALSE;
	}
	*/

	return TRUE;
}

/*
BOOL AgcmCharacter::SetActionBlockTime(UINT32 ulClockCount)
{
	m_ulActionBlockTime	= ulClockCount;

	return TRUE;
}

BOOL AgcmCharacter::IsActionBlockCondition(UINT32 ulClockCount)
{
	if (m_ulActionBlockTime > ulClockCount)
		return TRUE;

	return FALSE;
}
*/

BOOL AgcmCharacter::ProcessReservedAction(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgcdCharacter		*pstAgcdCharacter	= GetCharacterData(pcsCharacter);

	switch (pstAgcdCharacter->m_stNextAction.m_eActionType) {
	case AGPDCHAR_ACTION_TYPE_MOVE:
		{
			return MoveSelfCharacter(&pstAgcdCharacter->m_stNextAction.m_stTargetPos, 
									(MOVE_DIRECTION) pstAgcdCharacter->m_stNextAction.m_lUserData[0],
									(BOOL) pstAgcdCharacter->m_stNextAction.m_lUserData[1],
									(BOOL) pstAgcdCharacter->m_stNextAction.m_lUserData[2]);
		}
		break;

	case AGPDCHAR_ACTION_TYPE_ATTACK:
		{
			return ActionSelfCharacter(AGPDCHAR_ACTION_TYPE_ATTACK,
										pstAgcdCharacter->m_stNextAction.m_csTargetBase.m_lID,
										0,
										pstAgcdCharacter->m_stNextAction.m_bForceAction);
		}
		break;

	case AGPDCHAR_ACTION_TYPE_SKILL:
		{
			return EnumCallback(AGCMCHAR_CB_ID_ACTION_SKILL, pcsCharacter, NULL);
		}
		break;
	}

	return TRUE;
}

BOOL AgcmCharacter::SetFollowTarget(INT32 lTargetCID, INT32 lFollowDistance)
{
	if (lTargetCID == AP_INVALID_CID ||
		lTargetCID == m_lSelfCID ||
		lFollowDistance < 1)
		return FALSE;

	m_lFollowTargetCID	= lTargetCID;
	m_lFollowDistance	= lFollowDistance;

	return TRUE;
}

BOOL AgcmCharacter::ReleaseFollowTarget()
{
	m_lFollowTargetCID	= AP_INVALID_CID;
	m_lFollowDistance	= 0;

	return TRUE;
}

BOOL AgcmCharacter::CBMoveActionAttack(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmCharacter	*pThis	= (AgcmCharacter *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	if (pThis->m_pcsSelfCharacter != pcsCharacter)
		return TRUE;

	pThis->m_lFollowAttackTargetID	= pcsCharacter->m_lFollowTargetID;
	pThis->m_bFollowAttackLock		= TRUE;

	return TRUE;
}

BOOL AgcmCharacter::CBMoveActionRelease(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmCharacter	*pThis	= (AgcmCharacter *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	if (pThis->m_pcsSelfCharacter != pcsCharacter)
		return TRUE;

	pThis->m_bFollowAttackLock		= FALSE;
	pThis->m_lFollowAttackTargetID	= AP_INVALID_CID;

	return TRUE;
}

BOOL AgcmCharacter::CBRidableUpdate		(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmCharacter	* pThis		= ( AgcmCharacter	* ) pClass	;
	ApWorldSector	* pSector	= ( ApWorldSector	* ) pData	;

	ASSERT( NULL != pThis	);
	ASSERT( NULL != pSector	);

	if( NULL == pSector || NULL == pThis ) return FALSE;

	// 2005/02/21 ������
	// Ŭ���̾�Ʈ�� ����� ó���� �����Ƿ�....
	// 0�� ����� ����Ÿ�� �׳� ����Ѵ�..
	ApWorldSector::Dimension	* pDimension;
	pDimension = pSector->GetDimension( 0 );

	if( NULL == pDimension ) return FALSE;

	// �ö��ִ� �ɸ��� ��ġ ����..

	ApWorldSector::IdPos * pNode = pDimension->pUsers;
	AgpdCharacter *pcsCharacter;
	AuPOS	pos;
	FLOAT	fHeight;

	while( pNode )
	{
		pcsCharacter = pThis->m_pcsAgpmCharacter->GetCharacter( pNode->id2 );
		if( pcsCharacter )
		{
			pos = pcsCharacter->m_stPos;

			// ���� ���� ���..
			// �ڽŸ� ���̿� ���� üũ�ϰ� , NPC�� ���ʹ� ������ ����..
			// Ÿ PC�� ó������ �ʴ´�.
			if( !pThis->m_pcsAgpmCharacter->IsPC( pcsCharacter )	|| // PC�� �ƴѰ��� ���δ�..
				pcsCharacter->m_lID == pThis->m_lSelfCID			 ) 	// �Ǵ� �����ɸ���.
			{
				// �����̵� ���� �÷� ������.
				fHeight = pThis->m_pcsAgcmMap ? pThis->m_pcsAgcmMap->GetHeight( pos.x , pos.z , SECTOR_MAX_HEIGHT ) : 0;
				if (fHeight != 0 && fHeight != INVALID_HEIGHT)
					pos.y = fHeight;
			}
			else
			{
				// �� ���� ���,, 
				// �ڱ� ���� PC ��.
				// do nothing
				#ifndef BOKCHUNGGUJO
				fHeight = pThis->m_pcsAgcmMap ? pThis->m_pcsAgcmMap->GetHeight( pos.x , pos.z , SECTOR_MAX_HEIGHT ) : 0;
				if (fHeight != 0 && fHeight != INVALID_HEIGHT)
					pos.y = fHeight;
				#endif // BOKCHUNGGUJO
			}

			pThis->m_pcsAgpmCharacter->UpdatePosition( pcsCharacter , &pos );
		}
		pNode = pNode->pNext;
	}

	return TRUE;
}

BOOL AgcmCharacter::SetNextAction(AgpdCharacterAction *pstNextAction)
{
	if (!pstNextAction)
		return FALSE;

	m_stNextAction	= *pstNextAction;

	m_szNextAction	= g_aszActionName[m_stNextAction.m_eActionType];

	return TRUE;
}

AgpdCharacterAction* AgcmCharacter::GetNextAction()
{
	return &m_stNextAction;
}

BOOL AgcmCharacter::ProcessNextAction()
{
	switch (m_stNextAction.m_eActionType) {
	case AGPDCHAR_ACTION_TYPE_ATTACK:
		ActionSelfCharacter(m_stNextAction.m_eActionType, m_stNextAction.m_csTargetBase.m_lID, m_stNextAction.m_lUserData[0], m_stNextAction.m_bForceAction);
		break;

	case AGPDCHAR_ACTION_TYPE_SKILL:
		EnumCallback(AGCMCHAR_CB_ID_ACTION_SKILL, m_pcsSelfCharacter, NULL);
		break;

	case AGPDCHAR_ACTION_TYPE_MOVE:
		MoveSelfCharacter(&m_stNextAction.m_stTargetPos, (MOVE_DIRECTION) m_stNextAction.m_lUserData[0], (BOOL) m_stNextAction.m_lUserData[1], (BOOL) m_stNextAction.m_lUserData[2]);
		break;
	}

	ZeroMemory(&m_stNextAction, sizeof(m_stNextAction));
	m_szNextAction	= g_aszActionName[m_stNextAction.m_eActionType];

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 250205 BOB
******************************************************************************/
VOID AgcmCharacter::ShowPickingInfo()
{
	if (m_bShowPickingInfo)
		return;

	//@{ 2006/02/28 burumal			
	// PickAtomic�� Character Clump�� attach ��Ű�� �ʴ´�
	/*
	INT32			lIndex = 0;
	AgpdCharacter *	pcsCharacter;
	AgcdCharacter *	pstCharacter;

	for (pcsCharacter = m_pcsAgpmCharacter->GetCharSequence(&lIndex); pcsCharacter; pcsCharacter = m_pcsAgpmCharacter->GetCharSequence(&lIndex))
	{
		pstCharacter = GetCharacterData(pcsCharacter);

		if (pstCharacter->m_pClump && pstCharacter->m_pPickAtomic)
		{			
			RpClumpAddAtomic(pstCharacter->m_pClump, pstCharacter->m_pPickAtomic);
			m_pcsAgcmRender->AddAtomicToWorld(pstCharacter->m_pPickAtomic);			
		}
	}
	*/
	//@}

	m_bShowPickingInfo = TRUE;
}

VOID AgcmCharacter::HidePickingInfo()
{
	if (!m_bShowPickingInfo)
		return;

	//@{ 2006/02/28 burumal			
	// PickAtomic�� Character Clump�� attach ��Ű�� �ʴ´�
	/*
	INT32			lIndex = 0;
	AgpdCharacter *	pcsCharacter;
	AgcdCharacter *	pstCharacter;

	for (pcsCharacter = m_pcsAgpmCharacter->GetCharSequence(&lIndex); pcsCharacter; pcsCharacter = m_pcsAgpmCharacter->GetCharSequence(&lIndex))
	{
		pstCharacter = GetCharacterData(pcsCharacter);

		if (pstCharacter->m_pClump && pstCharacter->m_pPickAtomic)
		{			
			m_pcsAgcmRender->RemoveAtomicFromWorld(pstCharacter->m_pPickAtomic);
			RpClumpRemoveAtomic(pstCharacter->m_pClump, pstCharacter->m_pPickAtomic);			
		}
	}
	*/
	//@}

	m_bShowPickingInfo = FALSE;
}

//@{ 2006/03/08 burumal
VOID AgcmCharacter::ShowBoundingSphere()
{
	if ( m_bShowBoundingSphere )
		return;

	m_bShowBoundingSphere = TRUE;
}

VOID AgcmCharacter::HideBoundingSphere()
{
	if ( !m_bShowBoundingSphere )
		return;

	m_bShowBoundingSphere = FALSE;
}
//@}

VOID AgcmCharacter::ShowNextAction()
{
	AgcmUIConsole *	pcsAgcmUIConsole	= (AgcmUIConsole *) GetModule("AgcmUIConsole");
	if (pcsAgcmUIConsole)
	{
		pcsAgcmUIConsole->getConsole().registerWatch(0, "AgcmCharacter.m_szNextAction", 0xffffff00);
	}
}

VOID AgcmCharacter::ShowActionQueue()
{
	m_bShowActionQueue	= !m_bShowActionQueue;

	AgcmUIConsole *	pcsAgcmUIConsole	= (AgcmUIConsole *) GetModule("AgcmUIConsole");
	if (pcsAgcmUIConsole)
	{
		pcsAgcmUIConsole->getConsole().registerWatch(0, "AgcmCharacter.m_szActionQueue", 0xffffff00);
		pcsAgcmUIConsole->getConsole().registerWatch(1, "AgcmCharacter.m_szActionQueueAcc", 0xffffff00);
		pcsAgcmUIConsole->getConsole().registerWatch(2, "AgcmCharacter.m_szAttackSentAcc", 0xffffff00);
	}
}

VOID AgcmCharacter::BuildActionQueue()
{
	if (!m_bShowActionQueue)
		return;

	m_szActionQueue = "";

	if (!m_pcsSelfCharacter)
		return;

	AgcdCharacter	*pstAgcdCharacter	= GetCharacterData(m_pcsSelfCharacter);
	if (!pstAgcdCharacter)
		return;

	CHAR	szTemp[16];

	for (int i = 0; i < pstAgcdCharacter->m_lNumActionData; ++i)
	{
		sprintf(szTemp, "%d(%d);", pstAgcdCharacter->m_astActionQueue[i].eActionType
							 , pstAgcdCharacter->m_astActionQueue[i].eActionResultType
				);
		m_szActionQueue += szTemp;
	}
}

VOID AgcmCharacter::ClearActionQueue()
{
	m_szActionQueue		= "";
	m_szActionQueueAcc	= "";
	m_szAttackSentAcc	= "";
}

void AgcmCharacter::ShowMyCID()
{
	AgcmUIConsole *	pcsAgcmUIConsole	= (AgcmUIConsole *) GetModule("AgcmUIConsole");
	if (pcsAgcmUIConsole && GetSelfCharacter())
	{
		CHAR strTemp[10] = {0,};
		sprintf(strTemp, "%d", GetSelfCharacter()->m_lID);
		pcsAgcmUIConsole->getConsole().print(strTemp);
	}	
}

// #ifdef _AREA_CHINA_
// void AgcmCharacter::ShowNumOfCharacByType()
// {
// 	AgcmUIConsole *	pcsAgcmUIConsole	= (AgcmUIConsole *) GetModule("AgcmUIConsole");
// 	if (pcsAgcmUIConsole && m_pcsAgpmCharacter)
// 	{
// 		pcsAgcmUIConsole->getConsole().print(m_pcsAgpmCharacter->GetNumOfCharByType().getBuff());
// 	}
// 
// };
// #endif

BOOL AgcmCharacter::IsValidCharacter(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter || pcsCharacter->m_eType != APBASE_TYPE_CHARACTER)
		return FALSE;

	return IsValidCharacter(GetCharacterData(pcsCharacter));
}

BOOL AgcmCharacter::IsValidCharacter(AgcdCharacter *pstCharacter)
{
	if (!pstCharacter)
		return FALSE;

	if ((pstCharacter->m_lStatus & AGCMCHAR_STATUS_FLAG_INIT_COMPLETE) && !(pstCharacter->m_lStatus & AGCMCHAR_STATUS_FLAG_REMOVED))
		return TRUE;

	return FALSE;
}

BOOL AgcmCharacter::IsCharacterTransforming(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	return GetCharacterData(pcsCharacter)->m_bTransforming;
}

/*
	2005.05.31 By SungHoon
	�ź� ����� ������ �����Ѵ�.
*/
BOOL AgcmCharacter::SendPacketOptionFlag(AgpdCharacter *pcsCharacter, INT32 lOptionFlag)
{
	if (!pcsCharacter) return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmCharacter->MakePacketOptionFlag(pcsCharacter, lOptionFlag, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgcmCharacter::SendRequestClientConnection(INT32 lSocketIndex)
{
	if(lSocketIndex < 0)
		return FALSE;

	INT8	cOperation = AGCMCHAR_PACKET_OPERATION_REQUEST_CLIENT_CONNECT;
	INT16	nSize;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, &nSize, AGCMCHAR_PACKET_TYPE, 
								&cOperation,
								NULL,
								NULL,
								m_szCharName,
								NULL,
								NULL,
								&m_lReceivedAuthKey);

	if (!pvPacket) return FALSE;
	if (!SendPacket(pvPacket, nSize, ACDP_SEND_SERVER, PACKET_PRIORITY_NONE, lSocketIndex))
	{
		m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	m_csPacket.FreePacket(pvPacket);
	return TRUE;
}

bool AgcmCharacter::IsValidLOD( AgpdCharacterTemplate* pAgpdCharacterTemplate, AgcdCharacterTemplate* pAgcdCharacterTemplate )
{
	AgcdLODList	*pstLODList = pAgcdCharacterTemplate->m_stLOD.m_pstList;
	AgcmPreLODManager *pAgcmPreLODManager = (AgcmPreLODManager *)GetModule("AgcmPreLODManager");

	AgcdPreLOD	*pstAgcdPreLOD = pAgcmPreLODManager->GetCharacterPreLOD(pAgpdCharacterTemplate);
	if( !pstAgcdPreLOD )
		return false;

	RpClump* pClump = LoadClump( pAgcdCharacterTemplate->m_pcsPreData->m_pszDFFName );
	if( pClump == NULL )
		return false;
	RpClumpDestroy ( pClump );

	if( pstAgcdPreLOD ) {
		int nAtomicNum = RpClumpGetNumAtomics( pAgcdCharacterTemplate->m_pClump );
		if( nAtomicNum == 0 )
			return true;
		AgcdPreLODList* pstList = pstAgcdPreLOD->m_pstList;
		while( pstList )
		{
			for( int i = 0; i < 5; i++ )
			{
				if( strcmp( pstList->m_csData.m_aszData[i], "" ) == 0 )
					continue;

				pClump = LoadClump( pstList->m_csData.m_aszData[i] );
				if( pClump == NULL )
					continue;

				int nAtomicNum2 = RpClumpGetNumAtomics( pClump );
				RpClumpDestroy ( pClump );
				if( nAtomicNum2 != 1 && nAtomicNum != nAtomicNum2 ) 
					return false;
			}
			pstList = pstList->m_pstNext;
		}
	}

	return true;
}

bool AgcmCharacter::GetLodClumpPolyNum( std::vector< int >& vResult, AgpdCharacter* pAgpdCharacter )
{
	AgpdCharacterTemplate* pAgpdCharacterTemplate = pAgpdCharacter->m_pcsCharacterTemplate;
	AgcdCharacter* pAgcdCharacter = GetCharacterData( pAgpdCharacter );
	AgcdCharacterTemplate* pAgcdCharacterTemplate = pAgcdCharacter->m_pstAgcdCharacterTemplate;
	
	if(pAgcdCharacter->m_pClump == NULL)
	{
		AgcdLODList	*pstLODList = pAgcdCharacterTemplate->m_stLOD.m_pstList;
		AgcmPreLODManager *pAgcmPreLODManager = (AgcmPreLODManager *)GetModule("AgcmPreLODManager");

		AgcdPreLOD	*pstAgcdPreLOD = pAgcmPreLODManager->GetCharacterPreLOD(pAgpdCharacterTemplate);
		if( !pstAgcdPreLOD )
			return false;

		RpClump* pClump = LoadClump( pAgcdCharacterTemplate->m_pcsPreData->m_pszDFFName );
		if( pClump == NULL )
			return false;
		RpClumpDestroy ( pClump );

		if( pstAgcdPreLOD ) {
			int nPolyNums	= RwUtilClumpGetTriangles( pAgcdCharacterTemplate->m_pClump );
			vResult.push_back( nPolyNums );
			if( nPolyNums == 0 )
				return true;
			AgcdPreLODList* pstList = pstAgcdPreLOD->m_pstList;
			while( pstList )
			{
				for( int i = 0; i < 5; i++ )
				{
					if( strcmp( pstList->m_csData.m_aszData[i], "" ) == 0 )
						continue;

					pClump = LoadClump( pstList->m_csData.m_aszData[i] );
					if( pClump == NULL )
						continue;

					nPolyNums	= RwUtilClumpGetTriangles( pClump );
					vResult.push_back( nPolyNums );
					RpClumpDestroy ( pClump );
				}
				pstList = pstList->m_pstNext;
			}
		}
	}

	return true;
}


//void AgcmCharacter::SetPlayerAttackDirection(const RwV3d& vAttDir)
//{
//	//CBTransformAppear( (PVOID)m_pcsSelfCharacter, (PVOID)this, (PVOID) &m_pcsSelfCharacter->m_lTID1);
//
//	//. ��ȯ�� ���·� fake.
//	m_pcsSelfCharacter->m_bIsTrasform = TRUE;
//	m_pcsAgpmCharacter->RestoreTransformCharacter(m_pcsSelfCharacter);
//}

//@{ 2006/05/11 burumal
void AgcmCharacter::SetPlayerAttackDirection(const RwV3d& vAttDir)
{		
	AgcdCharacter* pcsAgcdCharacter	= GetCharacterData(m_pcsSelfCharacter);

	RpClump* pClump;

	if ( pcsAgcdCharacter->m_pRide )
		pClump = pcsAgcdCharacter->m_pRide->m_pClump;
	else
		pClump = pcsAgcdCharacter->m_pClump;

	if ( !pcsAgcdCharacter || !pClump )
		return;

	RwFrame* pFrame = RpClumpGetFrame(pClump);
	if ( !pFrame )
		return;

	if ( !(pcsAgcdCharacter->m_eCurAnimType == AGCMCHAR_ANIM_TYPE_WAIT || 
		pcsAgcdCharacter->m_eCurAnimType == AGCMCHAR_ANIM_TYPE_ATTACK ||
		pcsAgcdCharacter->m_eCurAnimType == AGCMCHAR_ANIM_TYPE_STRUCK ||
		pcsAgcdCharacter->m_eCurAnimType == AGCMCHAR_ANIM_TYPE_SKILL) )
	{
		return;
	}
	
	ASSERT(m_pcsAgpmCharacter);
	AgpdCharacter* pAgpdTargetChar = m_pcsAgpmCharacter->GetCharacter(pcsAgcdCharacter->m_lLockTargetID);
	if ( !pAgpdTargetChar )
		return;
	
	ASSERT(m_pcsAgpmFactors);
	if ( m_pcsAgpmFactors->IsInRange(&m_pcsSelfCharacter->m_stPos, &pAgpdTargetChar->m_stPos, 
							&m_pcsSelfCharacter->m_csFactor, &pAgpdTargetChar->m_csFactor, NULL) == FALSE )
	{
		return;
	}

	RwReal fAngle = m_pcsAgpmCharacter->GetWorldDirection(-vAttDir.x, -vAttDir.z);

	RwV3d vWorldUp;
	vWorldUp.x = 0; vWorldUp.y = 1; vWorldUp.z = 0;

	RwMatrix matRotY;
	RwMatrixRotate(&matRotY, &vWorldUp, fAngle, rwCOMBINEREPLACE);

	RwV3d vPosBackup;
	RwMatrix* pClumpMat = RwFrameGetMatrix(pFrame);
	vPosBackup = pClumpMat->pos;
	pClumpMat->right = matRotY.right;
	pClumpMat->up = matRotY.up;
	pClumpMat->at = matRotY.at;
	RwMatrixScale(pClumpMat, &pcsAgcdCharacter->m_pstAgcdCharacterTemplate->m_stScale, rwCOMBINEPOSTCONCAT);
	pClumpMat->pos = vPosBackup;

	RwFrameUpdateObjects(pFrame);
}
//@}



//. 2006. 6. 21. Nonstopdj
BOOL	AgcmCharacter::SetCharacterPolyMorph(AgpdCharacter* pdCharacter, const float fHPrate)
{
	if(!pdCharacter)
		return FALSE;

	//. 'PolyMorphTable.txt'�κ��� �ε��� TID���� �ĺ�.

	stCharacterPolyMorphEntry* pEntry = m_PolyMorphTableMap.Get( pdCharacter->m_lFixTID );
	if( pEntry && pEntry->m_vecTID.GetSize() > 0 )
	{
		BOOL bIsDead = pdCharacter->IsDead();
		int nPrevTID = pdCharacter->m_lFixTID;
		int nCurrTID = pdCharacter->m_lFixTID;

		switch( pEntry->m_eConditionType )
		{
		case AGCMCHAR_POLYMORPH_CONDITION_TYPE1 :
			{
				if( bIsDead )
				{
					nCurrTID = *( pEntry->m_vecTID.Get( 0 ) );
				}
			}
			break;

		case AGCMCHAR_POLYMORPH_CONDITION_TYPE2 :
			{
				if( fHPrate < 0.5f )
				{
					nCurrTID = *( pEntry->m_vecTID.Get( 0 ) );
				}

				if( bIsDead )
				{
					nCurrTID = *( pEntry->m_vecTID.Get( 1 ) );
				}
			}
			break;

		case AGCMCHAR_POLYMORPH_CONDITION_TYPE3 :
			{
				if( fHPrate < 0.7f && fHPrate > 0.4f )
				{
					nCurrTID = *( pEntry->m_vecTID.Get( 0 ) );
				}
				else if( fHPrate <= 0.4f )
				{
					nCurrTID = *( pEntry->m_vecTID.Get( 1 ) );
				}

				if( bIsDead )
				{
					nCurrTID = *( pEntry->m_vecTID.Get( 2 ) );
				}
			}
			break;
		}

		if( nCurrTID != 0 && nCurrTID != pdCharacter->m_lTID1 )
		{
			RemoveDuplicateSiegeWarObject( pdCharacter );
			if( pEntry->m_eConditionType > AGCMCHAR_POLYMORPH_CONDITION_TYPE1 )
			{
				pdCharacter->m_lFixTID = nPrevTID;
				m_pcsAgpmCharacter->SetTypePolyMorph( m_pcsAgpmCharacter->GetCharacterTemplate( nCurrTID ) );
			}

			QueueCharPolyMorphByTID( pdCharacter, nCurrTID );
			return TRUE;
		}
	}

	return FALSE;
}

BOOL AgcmCharacter::EndCharPolyMorph( AgpdCharacter* pdCharacter )
{
	if (pdCharacter->m_lFixTID != pdCharacter->m_pcsCharacterTemplate->m_lID  )
	{
		QueueCharPolyMorphByTID(pdCharacter, pdCharacter->m_lFixTID);
		return TRUE;
	}
	else
		return FALSE;
}

BOOL AgcmCharacter::EndCharEvolution( AgpdCharacter* pdCharacter )
{
	if (pdCharacter->m_lFixTID != pdCharacter->m_pcsCharacterTemplate->m_lID  )
	{
		QueueCharPolyMorphByTID(pdCharacter, pdCharacter->m_lFixTID);
		return TRUE;
	}
	else
		return FALSE;
}

BOOL AgcmCharacter::QueueCharPolyMorphByTID( AgpdCharacter *	pcsAgpdCharacter, INT32	 lNewTID)
{
	// ���������� ť���ߴٰ� �׻� ���̵� Ÿ�ӿ��� �����Ѵ�.
	ApAutoWriterLock	csLock( m_RWLockPolymorphTable );

	PolymorphQueueInfo	stData;
	stData.pCharacter	= pcsAgpdCharacter	;
	stData.lNewTID		= lNewTID			;
	stData.uRequestTime	= ::GetTickCount() 	+ 100;
	stData.eProgress	= PolymorphQueueInfo::PROCESSING;

	if (pcsAgpdCharacter->m_lID == m_lSelfCID)
	{
		stData.bSelf = TRUE;
	}

	BOOL	bFound = FALSE;

	// �̹� ���ִ��� �˻� ����.
	for( vector< PolymorphQueueInfo >::iterator iter = m_vecPolymorphQueueTable.begin();
		iter != m_vecPolymorphQueueTable.end() && iter ;
		iter ++ )
	{
		PolymorphQueueInfo * pQueue = &*iter;
		if( pQueue->pCharacter == pcsAgpdCharacter )
		{
			bFound = TRUE;

			if( pQueue->lNewTID	== lNewTID )
			{
				// do nothing.. 
			}
			else
			{
				pQueue->lNewTID			= lNewTID;
				pQueue->uRequestTime	= stData.uRequestTime;
			}
		}
	}

	if( !bFound )
	{
		m_vecPolymorphQueueTable.push_back( stData );
		DestroyCharacter(pcsAgpdCharacter);
	}

	return TRUE;
}

void	AgcmCharacter::PolymorphIdleProcess	()
{
	// ���̵�Ÿ�̿����� ���� �������� ����.
	ApAutoWriterLock	csLock( m_RWLockPolymorphTable );

	if( m_vecPolymorphQueueTable.size() )
	{
		UINT32	uCurrent = GetTickCount();

		BOOL	bFailed = FALSE;

		for( vector< PolymorphQueueInfo >::iterator iter = m_vecPolymorphQueueTable.begin() ;
			iter != m_vecPolymorphQueueTable.end();
			iter++ )
		{
			PolymorphQueueInfo *pstData = &*iter;

			if( pstData->eProgress == PolymorphQueueInfo::PROCESSING )
			{
				if( pstData->uRequestTime < uCurrent &&
					m_pcsAgcmResourceLoader->IsEmptyEntry( m_lLoaderIDInit )		)
				{
					AgpdCharacter *			pcsAgpdCharacter	= pstData->pCharacter;
					AgcdCharacter *			pstAgcdCharacter	= GetCharacterData(pcsAgpdCharacter);

					if( pstAgcdCharacter->m_pClump && pstAgcdCharacter->m_pClump->ulFlag & RWFLAG_RENDER_ADD )
					{
						// Wait..
						DestroyCharacter( pcsAgpdCharacter );
					}

					{
						pstData->eProgress = PolymorphQueueInfo::PROCESSED;

						pcsAgpdCharacter->m_lTID1 = pstData->lNewTID;
						AgpdCharacterTemplate *pcsCharacterTemplate;
						pcsCharacterTemplate = m_pcsAgpmCharacter->GetCharacterTemplate(pstData->lNewTID);
						pcsAgpdCharacter->m_pcsCharacterTemplate = pcsCharacterTemplate;
						InitCharacter(pcsAgpdCharacter, FALSE);
						
						if( !m_pcsAgpmCharacter->IsPolyMorph( pcsAgpdCharacter ) )
						{
							g_pcsAgcmEventEffect->CharaterEquipEffectUpdate( pcsAgpdCharacter );
						}

						//. ��������� AgcdCharactemplate�� ����.
						if(pstAgcdCharacter->m_pstAgcdCharacterTemplate != GetTemplateData(pcsCharacterTemplate))
							pstAgcdCharacter->m_pstAgcdCharacterTemplate = GetTemplateData(pcsCharacterTemplate);

						m_pcsAgpmCharacter->UpdatePosition(pcsAgpdCharacter, NULL, TRUE, TRUE);

						if(pstData->bSelf)
						{
							SetSelfCharacter(pcsAgpdCharacter);
						}
					}
				}
				else bFailed = TRUE;

			}
			else	bFailed = TRUE;
		}

		// ��� ó���� �Ǿ����� Ȯ��
		if( !bFailed ) m_vecPolymorphQueueTable.clear();
	}

	// ���� ������Ʈ �������� �߰� ����..
	#ifdef _DEBUG
	BOOL	bDoPolyMorph = TRUE;
	if( m_pcsAgcmResourceLoader->IsEmptyEntry(m_lLoaderIDInit) && bDoPolyMorph )
	#else
	if( m_pcsAgcmResourceLoader->IsEmptyEntry(m_lLoaderIDInit) ) 
	#endif 
	{
		ApAutoWriterLock	csLock( g_lockSiegeMonster );
		for( vector< INT32 >::iterator iter = g_vecSiegeMonster.begin();
			iter != g_vecSiegeMonster.end() && iter;
			iter++ )
		{
			AgpdCharacter * pcsCharacter = this->m_pcsAgpmCharacter->GetCharacter( *iter );
			if( pcsCharacter )
			{
				if( CheckCharacterPolyMorph( pcsCharacter ) ) break;
			}
		}
	}
}

bool	AgcmCharacter::CheckCharacterPolyMorph	( AgpdCharacter * pcsCharacter )
{
	INT32		CurHp,MaxHp;
	float		Per_Result;
	AgpdFactor* pcsFactorResult = (AgpdFactor*)m_pcsAgpmFactors->GetFactor(&pcsCharacter->m_csFactor,
		AGPD_FACTORS_TYPE_RESULT);
	if(!pcsFactorResult)
		pcsFactorResult	= &pcsCharacter->m_csFactor;

	m_pcsAgpmFactors->GetValue(pcsFactorResult, &CurHp, AGPD_FACTORS_TYPE_CHAR_POINT,
		AGPD_FACTORS_CHARPOINT_TYPE_HP);
	m_pcsAgpmFactors->GetValue(pcsFactorResult, &MaxHp, AGPD_FACTORS_TYPE_CHAR_POINT_MAX,
		AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);

	if (CurHp > MaxHp)
		CurHp = MaxHp;

	if (MaxHp <= 0 || CurHp <= 0)
		Per_Result = 0;
	else
		Per_Result = (float)CurHp / (float)MaxHp;

	if( SetCharacterPolyMorph(pcsCharacter, Per_Result) )
		return true;
	else
		return false;
}


BOOL AgcmCharacter::StartCharPolyMorphByTID( AgpdCharacter *	pcsAgpdCharacter, INT32	nPrevTID )
{
	if(!pcsAgpdCharacter) return FALSE;

	AgcdCharacter *			pstAgcdCharacter	= GetCharacterData(pcsAgpdCharacter);
	INT32					nNextTID			= pcsAgpdCharacter->m_lTID1;
	AgpdCharacterTemplate *	pcsOldTemplate		= m_pcsAgpmCharacter->GetCharacterTemplate(nPrevTID);
	AgpdCharacterTemplate *	pcsNewTemplate		= m_pcsAgpmCharacter->GetCharacterTemplate(nNextTID);
	BOOL					bSelfCharacter		= FALSE;

	if(!pcsOldTemplate || !pcsNewTemplate)
		return FALSE;

	pstAgcdCharacter->m_bTransforming			= TRUE;

	if (pcsAgpdCharacter->m_lID == m_lSelfCID)
	{
		bSelfCharacter = TRUE;
	}

	pstAgcdCharacter->m_bForceAnimation	= FALSE;

	pcsAgpdCharacter->m_lTID1					= nPrevTID;
	pcsAgpdCharacter->m_pcsCharacterTemplate	= pcsOldTemplate;
	
	DestroyCharacter(pcsAgpdCharacter);

	pstAgcdCharacter->m_lStatus					= AGCMCHAR_STATUS_FLAG_NONE;

	pcsAgpdCharacter->m_lTID1					= nNextTID;
	pcsAgpdCharacter->m_pcsCharacterTemplate	= pcsNewTemplate;

	pstAgcdCharacter->m_lStatus					= AGCMCHAR_STATUS_FLAG_NONE;
	pstAgcdCharacter->m_eCurAnimType			= AGCMCHAR_ANIM_TYPE_WAIT;
	pstAgcdCharacter->m_lCurAnimType2			= 0;

//#ifdef _DEBUG
//	AgcdCharacterTemplate* pcdTemplate = GetTemplateData(pcsNewTemplate);
//	if(pcdTemplate)
//	{
//		if(!pcdTemplate->m_bLoaded)
//			DebugBreak();
//	}
//#endif

	InitCharacter(pcsAgpdCharacter, FALSE);

	//. ��������� AgcdCharactemplate�� ����.
	if(pstAgcdCharacter->m_pstAgcdCharacterTemplate != GetTemplateData(pcsNewTemplate))
		pstAgcdCharacter->m_pstAgcdCharacterTemplate = GetTemplateData(pcsNewTemplate);

	m_pcsAgpmCharacter->UpdatePosition(pcsAgpdCharacter, NULL, TRUE, TRUE);

	if (bSelfCharacter)
	{
		SetSelfCharacter(pcsAgpdCharacter);
	}

	if( !m_pcsAgpmCharacter->IsPolyMorph( pcsAgpdCharacter ) )
	{
		g_pcsAgcmEventEffect->CharaterEquipEffectUpdate( pcsAgpdCharacter );
	}

	pstAgcdCharacter->m_bTransforming			= FALSE;

	//m_pcsAgpmCharacter->UpdateInit( pcsAgpdCharacter );

	return TRUE;
}

BOOL AgcmCharacter::StartCharEvolutionByTID( AgpdCharacter *	pcsAgpdCharacter, INT32	 lNewTID)
{
	if(!pcsAgpdCharacter) return FALSE;

	AgcdCharacter *			pstAgcdCharacter	= GetCharacterData(pcsAgpdCharacter);
	INT32					lOldTID				= pcsAgpdCharacter->m_lTID1;
	AgpdCharacterTemplate *	pcsOldTemplate		= m_pcsAgpmCharacter->GetCharacterTemplate(lOldTID);
	AgpdCharacterTemplate *	pcsNewTemplate		= m_pcsAgpmCharacter->GetCharacterTemplate(lNewTID);
	BOOL					bSelfCharacter		= FALSE;

	if(!pcsOldTemplate || !pcsNewTemplate) return FALSE;
	pstAgcdCharacter->m_bTransforming			= TRUE;

	if (pcsAgpdCharacter->m_lID == m_lSelfCID)
	{
		bSelfCharacter = TRUE;
	}

	pstAgcdCharacter->m_bForceAnimation	= FALSE;
	pcsAgpdCharacter->m_lTID1					= lOldTID;
	pcsAgpdCharacter->m_pcsCharacterTemplate	= pcsOldTemplate;
	
	DestroyCharacter(pcsAgpdCharacter);

	pstAgcdCharacter->m_lStatus					= AGCMCHAR_STATUS_FLAG_NONE;
	pcsAgpdCharacter->m_lTID1					= lNewTID;
	pcsAgpdCharacter->m_pcsCharacterTemplate	= pcsNewTemplate;
	pstAgcdCharacter->m_lStatus					= AGCMCHAR_STATUS_FLAG_NONE;
	pstAgcdCharacter->m_eCurAnimType			= AGCMCHAR_ANIM_TYPE_WAIT;
	pstAgcdCharacter->m_lCurAnimType2			= 0;

	_UpdateFactorCharacterType( pcsAgpdCharacter, pcsAgpdCharacter->m_lTID1 );
	InitCharacter(pcsAgpdCharacter, FALSE);

	OnUpdateCharacterCustomize( pcsAgpdCharacter, pstAgcdCharacter );

	//. ��������� AgcdCharactemplate�� ����.
	if(pstAgcdCharacter->m_pstAgcdCharacterTemplate != GetTemplateData(pcsNewTemplate))
	{
		pstAgcdCharacter->m_pstAgcdCharacterTemplate = GetTemplateData(pcsNewTemplate);
	}

	m_pcsAgpmCharacter->UpdatePosition(pcsAgpdCharacter, NULL, TRUE, TRUE);

	// ĳ���Ͱ� ���� ���¸� ���� �ִϸ��̼��� �������ش�.
	
	if( pcsAgpdCharacter->IsDead() )
	{
		StartAnimation( pcsAgpdCharacter, AGCMCHAR_ANIM_TYPE_DEAD );
	}

	if (bSelfCharacter)
	{
		SetSelfCharacter(pcsAgpdCharacter);

		// ��ȭ���� ��ư�� ���� ���� / �Ⱥ��� ó���� �����Ѵ�.
		AgcmUICharacter* pcmUICharacter = ( AgcmUICharacter* )GetModule( "AgcmUICharacter" );
		if( pcmUICharacter )
		{
			pcmUICharacter->OnToggleBtnRestoreEvolution( pcsAgpdCharacter->m_bIsEvolution );
		}
	}

	pstAgcdCharacter->m_bTransforming			= FALSE;
	return TRUE;
}

//@{ 2006/07/05 burumal
BOOL	AgcmCharacter::SetMovementKeys()
{
	if ( !m_pcsAgcmUIHotkey )
		return FALSE;

	m_pMovementHotkey[0] = m_pcsAgcmUIHotkey->FindCurrentHotkeyByDefault(AGCDUI_HOTKEY_TYPE_NORMAL, UI_HOTKEY_MOVEMENT_FORWARD);	// ����Ű W (g_aszHotkeyNameTable����)
	m_pMovementHotkey[1] = m_pcsAgcmUIHotkey->FindCurrentHotkeyByDefault(AGCDUI_HOTKEY_TYPE_NORMAL, UI_HOTKEY_MOVEMENT_BACK);		// ����Ű S (g_aszHotkeyNameTable����)
	m_pMovementHotkey[2] = m_pcsAgcmUIHotkey->FindCurrentHotkeyByDefault(AGCDUI_HOTKEY_TYPE_NORMAL, UI_HOTKEY_MOVEMENT_LEFT);		// ����Ű A (g_aszHotkeyNameTable����)
	m_pMovementHotkey[3] = m_pcsAgcmUIHotkey->FindCurrentHotkeyByDefault(AGCDUI_HOTKEY_TYPE_NORMAL, UI_HOTKEY_MOVEMENT_RIGHT);		// ����Ű D (g_aszHotkeyNameTable����)	

	m_nMoveVirtualKeys[0] = m_pcsAgcmUIHotkey->ScanCodeToVirtual(m_pMovementHotkey[0]->m_cKeyCode);
	m_nMoveVirtualKeys[1] = m_pcsAgcmUIHotkey->ScanCodeToVirtual(m_pMovementHotkey[1]->m_cKeyCode);
	m_nMoveVirtualKeys[2] = m_pcsAgcmUIHotkey->ScanCodeToVirtual(m_pMovementHotkey[2]->m_cKeyCode);
	m_nMoveVirtualKeys[3] = m_pcsAgcmUIHotkey->ScanCodeToVirtual(m_pMovementHotkey[3]->m_cKeyCode);

	return TRUE;
}
//@}

//@{ Jaewon 20051202
// The player character looks at a specific target direction 
// according to mouse-moving or object targetting.
void	AgcmCharacter::SetPlayerLookAtFactor(RwReal factor)
{	
	m_lookAtFactor = min(max(factor, 0.0f), 1.0f);
}

void	AgcmCharacter::SetPlayerLookAtTarget(const RwV3d &lookAtTarget)
{
	m_lookAtTarget = lookAtTarget;
}

RwReal	AgcmCharacter::GetPlayerLookAtFactor() const
{
	return m_lookAtFactor;
}
//@}

VOID AgcmCharacter::KillGameServer1()
{
	AgpmCasper	*pcsAgpmCasper	= (AgpmCasper *)	this->GetModule("AgpmCasper");
	if (pcsAgpmCasper)
	{
		INT16	nPacketLength	= 0;
		PVOID	pvPacket		= pcsAgpmCasper->MakePacketMemoryOverflow(&nPacketLength);

		SendPacket(pvPacket, nPacketLength);
	}
}

BOOL AgcmCharacter::SetHPZero(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	m_pcsAgpmFactors->SetValue(&pcsCharacter->m_csFactor, 0, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);

	PVOID	pvBuffer[2];
	pvBuffer[0]	= IntToPtr(1);
	pvBuffer[1]	= IntToPtr(1);

	CBUpdateFactorHP(&pcsCharacter->m_csFactor, this, pvBuffer);

	return TRUE;
}

//@{ 2006/09/06 burumal
VOID AgcmCharacter::ShowClumpForced(INT32 lCID, BOOL bShow, UINT32 uFadeTime)
{	
	if ( lCID == AP_INVALID_CID )
		return;

	AgpdCharacter*pcsCharacter = m_pcsAgpmCharacter->GetCharacter(lCID);
	if ( !pcsCharacter )
		return;

	AgcdCharacter* pAgcdCharacter = GetCharacterData(pcsCharacter);
	if ( !pAgcdCharacter )
		return;

	RpClump* pClump = pAgcdCharacter->m_pClump;
	if ( !pClump )
		return;	
	
	if ( bShow )
	{
		m_pcsAgcmRender->InvisibleFxEnd(pClump);
		m_pcsAgcmRender->StartFadeInClump(pClump, 0, uFadeTime);
		
		pAgcdCharacter->m_pClump->stType.eType &= ~ACUOBJECT_TYPE_INVISIBLE;
		pAgcdCharacter->m_cTransparentType = (INT8) AGCMCHAR_TRANSPARENT_NONE;
		pcsCharacter->m_ulSpecialStatus &= ~AGPDCHAR_SPECIAL_STATUS_TRANSPARENT;

		//todo: ����� ������ effect�� �׸����� �� �ɼ��� �ְ� ����. �������� checkbox ������ ���������ϰ� �����Ұ���
		pAgcdCharacter->m_pClump->stType.eType &= ~ACUOBJECT_TYPE_FORCED_RENDER_EFFECT;
		
		if ( pAgcdCharacter->m_pRide && pAgcdCharacter->m_pRide->m_pClump )
		{
			m_pcsAgcmRender->InvisibleFxEnd(pAgcdCharacter->m_pRide->m_pClump);
			m_pcsAgcmRender->StartFadeInClump(pAgcdCharacter->m_pRide->m_pClump, 0, uFadeTime);
			pAgcdCharacter->m_pRide->m_pClump->stType.eType &= ~ACUOBJECT_TYPE_INVISIBLE;

			//todo: ����� ������ effect�� �׸����� �� �ɼ��� �ְ� ����. �������� checkbox ������ ���������ϰ� �����Ұ���
			pAgcdCharacter->m_pClump->stType.eType &= ~ACUOBJECT_TYPE_FORCED_RENDER_EFFECT;
		}
	}
	else
	{		
		m_pcsAgcmRender->InvisibleFxBegin(pClump, 0, 2000 - uFadeTime);

		pAgcdCharacter->m_pClump->stType.eType |= ACUOBJECT_TYPE_INVISIBLE;
		pAgcdCharacter->m_cTransparentType = (INT8) AGCMCHAR_TRANSPARENT_FULL;
		pcsCharacter->m_ulSpecialStatus |= AGPDCHAR_SPECIAL_STATUS_TRANSPARENT;

		//todo: ����� ������ effect�� �׸����� �� �ɼ��� �ְ� ����. �������� checkbox ������ ���������ϰ� �����Ұ���
		pAgcdCharacter->m_pClump->stType.eType |= ACUOBJECT_TYPE_FORCED_RENDER_EFFECT;

		if ( pAgcdCharacter->m_pRide && pAgcdCharacter->m_pRide->m_pClump )
		{
			m_pcsAgcmRender->InvisibleFxBegin(pAgcdCharacter->m_pRide->m_pClump, 0, 2000 - uFadeTime);
			pAgcdCharacter->m_pRide->m_pClump->stType.eType |= ACUOBJECT_TYPE_INVISIBLE;

			//todo: ����� ������ effect�� �׸����� �� �ɼ��� �ְ� ����. �������� checkbox ������ ���������ϰ� �����Ұ���
			pAgcdCharacter->m_pClump->stType.eType |= ACUOBJECT_TYPE_FORCED_RENDER_EFFECT;
		}
	}
}
//@}

//@{ 2006/10/23 burumal
BOOL AgcmCharacter::LoadTemplatePickingClump(AgpdCharacterTemplate* pstAgpdCharacterTemplate, AgcdCharacterTemplate *pstAgcdCharacterTemplate)
{
	CHAR* pszPickingDataName		= NULL;

	if ( pstAgpdCharacterTemplate == NULL || pstAgcdCharacterTemplate == NULL )
		return FALSE;

	if ( (pstAgcdCharacterTemplate->m_pcsPreData->m_pszPickDFFName) && (!pstAgcdCharacterTemplate->m_pPickingAtomic) )
	{
		pszPickingDataName	= new CHAR [strlen(pstAgcdCharacterTemplate->m_pcsPreData->m_pszPickDFFName) + 1];
		strcpy(pszPickingDataName, pstAgcdCharacterTemplate->m_pcsPreData->m_pszPickDFFName);
	}

	if (pszPickingDataName)
	{
		pstAgcdCharacterTemplate->m_pPickingAtomic	= LoadAtomic(pszPickingDataName);
	}

	if (pszPickingDataName)
		delete [] pszPickingDataName;

	return TRUE;
}

BOOL AgcmCharacter::ReleaseTemplatePickingClump(AgcdCharacterTemplate *pstTemplate)
{
	if ( pstTemplate == NULL )
		return FALSE;

	RpAtomic* pstAtomic;

	if ( !IsBadReadPtr( pstTemplate->m_pPickingAtomic, sizeof(RpAtomic)) )
	{
		if ( pstTemplate->m_pPickingAtomic == pstTemplate->m_pPickingAtomic->next )
		{
			pstAtomic = pstTemplate->m_pPickingAtomic;
			if (pstAtomic)
			{
				pstTemplate->m_pPickingAtomic = NULL;

				if (m_pcsAgcmResourceLoader)
					m_pcsAgcmResourceLoader->AddDestroyAtomic(pstAtomic);
				else
					RpAtomicDestroy(pstAtomic);
			}
		}
		else
		{			
			RpAtomic* pFirst = pstTemplate->m_pPickingAtomic;
			RpAtomic* pTempCur = pFirst;

			while ( pTempCur )
			{
				RpAtomic* pDel = pTempCur;

				if ( pTempCur->next == pFirst )
					pTempCur = NULL;
				else
					pTempCur = pTempCur->next;

				if ( m_pcsAgcmResourceLoader )
					m_pcsAgcmResourceLoader->AddDestroyAtomic(pDel);
				else
					RpAtomicDestroy(pDel);
			}

			pstTemplate->m_pPickingAtomic = NULL;
		}
	}

	return TRUE;
}
//@}

//@{ 2006/12/15 burumal
VOID AgcmCharacter::SetNonPickingTID(INT32 lTID)
{
	if ( m_NonPickingTIDSet.find(lTID) == m_NonPickingTIDSet.end() )
		m_NonPickingTIDSet.insert(lTID);
}

BOOL AgcmCharacter::IsNonPickingTID(INT32 lTID)
{
	if ( m_NonPickingTIDSet.find(lTID) != m_NonPickingTIDSet.end() )
		return TRUE;

	return FALSE;
}
//@}


//################################################################3
// ��� �ڵ�
// ��ġ�� �ӹ��� ���� ���� �ӽ��ڵ�
void AgcmCharacter::RemoveDuplicateSiegeWarObject(AgpdCharacter *pcsCharacter)
{
	static AgpmSiegeWar * pmAgpmSiegeWar = ( AgpmSiegeWar * ) GetModule("AgpmSiegeWar");
	
	AgpdSiegeWarMonsterType	eMonsterType = pmAgpmSiegeWar->GetSiegeWarMonsterType(pcsCharacter->m_pcsCharacterTemplate);
	
	// 2007.02.09. steeple
	// AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER �� ��Ÿ�� ����.
	if (AGPD_SIEGE_MONSTER_TYPE_NONE >= eMonsterType ||
		eMonsterType >= AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER)
		return;

	INT32 lIndex = 0;
	AgpdCharacter *pcsOther = NULL;

	for (pcsOther = m_pcsAgpmCharacter->GetCharSequence(&lIndex);
		 pcsOther;
		 pcsOther = m_pcsAgpmCharacter->GetCharSequence(&lIndex)
		)
	{
		if (pcsCharacter == pcsOther)
			continue;
	
		if (pcsOther->m_pcsCharacterTemplate->m_lID != pcsCharacter->m_pcsCharacterTemplate->m_lID
			&& pcsOther->m_pcsCharacterTemplate->m_lID != pcsCharacter->m_lFixTID)
			continue;
		
		AgpdSiegeWar *pcsSiegeWar1 = pmAgpmSiegeWar->GetSiegeWarInfo(pcsCharacter);
		AgpdSiegeWar *pcsSiegeWar2 = pmAgpmSiegeWar->GetSiegeWarInfo(pcsOther);
		if (!pcsSiegeWar1 || !pcsSiegeWar2)
			continue;
	
		if (COMPARE_EQUAL != pcsSiegeWar1->m_strCastleName.Compare(pcsSiegeWar2->m_strCastleName.GetBuffer()))
			continue;

		// 2007.01.22. steeple
		// Disable ĳ���͸� ������ �Ѵ�.
		if(!(pcsOther->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_DISABLE))
			continue;
		
		m_pcsAgpmCharacter->RemoveCharacter(pcsOther->m_lID);
	}
}

AgpdSkillTemplate* AgcmCharacter::_GetCurrentEvolutionSkillTemplate( void )
{
	// ���� Ȱ��ȭ�� ��� ������ų�� ������.
	AgpdSkillAttachData* ppdSkillAttachData = m_pcsAgpmSkill->GetAttachSkillData( ( ApBase* )m_pcsSelfCharacter );
	if( !ppdSkillAttachData ) return FALSE;

	INT32 nTotalSkillRate = 0;
	for( INT32 nCount = 0 ; nCount < AGPMSKILL_MAX_SKILL_BUFF ; ++nCount )
	{
		AgpdSkillTemplate* ppdSkillTemplate = m_pcsAgpmSkill->GetSkillTemplate( ppdSkillAttachData->m_astBuffSkillList[ nCount ].lSkillTID );
		if( ppdSkillTemplate )
		{
			if( m_pcsAgpmSkill->IsEvolutionSkill( ppdSkillTemplate ) )
			{
				return ppdSkillTemplate;
			}
		}
	}

	return NULL;
}

BOOL AgcmCharacter::OnChagneEquipItemByEvolution( AgpdCharacter* ppdCharacter, AuCharClassType eNextClassType )
{
	if( !ppdCharacter ) return FALSE;

	// ����ó�� 1. �巡��ÿ¿��Ը� �����Ѵ�.. ���� ��ȹ�� ��ȭ��ų�� �巡��ÿ¿��Ը� �ִ�.
	if( m_pcsAgpmFactors->GetRace( &ppdCharacter->m_csFactor ) != AURACE_TYPE_DRAGONSCION ) return FALSE;

	// �������� �����۵��� ���鼭 �ش� �����۵��� �����Ѵ�.
	AgpmItem* ppmItem = ( AgpmItem* )GetModule( "AgpmItem" );
	AgcmItem* pcmItem = ( AgcmItem* )GetModule( "AgcmItem" );
	AgpmGrid* ppmGrid = ( AgpmGrid* )GetModule( "AgpmGrid" );	
	AgcmUIItem* pcmUIItem = ( AgcmUIItem* )GetModule( "AgcmUIItem" );
	if( !ppmItem || !ppmGrid || !pcmUIItem ) return FALSE;

	AgpdItemADChar* ppdItemADChar = ppmItem->GetADCharacter( ppdCharacter );
	AgpdGrid* ppdEquipGrid = &ppdItemADChar->m_csEquipGrid;

	INT32 nSequencialIndex = 0;	
	AgpdGridItem* ppdGridItem = ppmGrid->GetItemSequence( ppdEquipGrid, &nSequencialIndex );
	while( ppdGridItem )
	{
		// �ش� �׸����� �������� ã�´�.
		AgpdItem* ppdItemCurrent = ppmItem->GetItem( ppdGridItem->m_lItemID );
		if( !ppdItemCurrent ) continue;

		// �� �������� ���� ���밡���� �������̶��..
		if( pcmUIItem->IsUsableItem( ppdCharacter, ppdItemCurrent, eNextClassType ) )
		{
			// �ش� �����ۿ� ItemEvolutionTable ���� �����ؾ� �� �������� ã�� ������.
			OnEquipItemByEvolution( ppdCharacter, ppdItemCurrent, eNextClassType );
		}
		//// �ƴϸ�..
		//else
		//{
		//	// �����.
		//	OnUnEquipitem
		//}

		// �������� ���� �� ���̳� �Ӹ��� �����ؾ� �Ѵٸ� ������Ʈ ���ش�.
		pcmItem->OnUpdateHairAndFace( ppdItemCurrent->m_pcsItemTemplate, GetCharacterData( ppdCharacter ), TRUE );

		// �ش� �����۽����� ��밡�ɿ��θ� ������Ʈ���ش�.
		pcmUIItem->UpdateEquipSlotEnable( ppdCharacter, ppdGridItem, eNextClassType );

		// ������..
		ppdGridItem = ppmGrid->GetItemSequence( ppdEquipGrid, &nSequencialIndex );
	}

	g_pcsAgcmEventEffect->CharaterEquipEffectUpdate( ppdCharacter );
	return TRUE;
}

BOOL AgcmCharacter::OnEquipItemByEvolution( AgpdCharacter* ppdCharacter, AgpdItem* ppdItem, AuCharClassType eNextClassType )
{
	if( !ppdCharacter || !ppdItem ) return FALSE;

	AgcmItem* pcmItem = ( AgcmItem* )GetModule( "AgcmItem" );
	AgpmItem* ppmItem = ( AgpmItem* )GetModule( "AgpmItem" );
	if( !pcmItem || !ppmItem ) return FALSE;

	// �巡��ÿ����� ���ӳ� Ŭ������ ���ؼ��� �������� �ʴ´�.
	AuRaceType eRaceType = ( AuRaceType )m_pcsAgpmFactors->GetRace( &ppdCharacter->m_csFactor );
	AuCharClassType eClassType = GetClassTypeByTID( ppdCharacter->m_lTID1 );
	if( eRaceType == AURACE_TYPE_DRAGONSCION && eClassType == AUCHARCLASS_TYPE_MAGE ) return TRUE;

	AgcdCharacter* pcdCharacter = GetCharacterData( ppdCharacter );
	AgpdItemTemplate* ppdItemTemplate = ppdItem->m_pcsItemTemplate;
	AgpdItemTemplateEquip* ppdItemTemplateEquip = ( AgpdItemTemplateEquip* )ppdItemTemplate;

	AgcdItemTemplate* pcdItemTemplate = pcmItem->GetTemplateData( ppdItemTemplate );
	AgcdItem* pcdItem = pcmItem->GetItemData( ppdItem );
	
	AuCharClassType eCurrentClassType = ( AuCharClassType )m_pcsAgpmFactors->GetClass( &ppdCharacter->m_csFactor );

	pcmItem->DetachItem( pcdCharacter, ( AgpdItemTemplateEquip* )ppdItemTemplate, pcdItem, pcdItemTemplate, eCurrentClassType );
	if( ppdItemTemplateEquip->m_nKind == AGPMITEM_EQUIP_KIND_NECKLACE )
	{
		pcmItem->OnUnEquipItemNecklace( ppdCharacter, ppdItem, pcdItem, eCurrentClassType );
	}

	pcmItem->AttachItem( ppdCharacter->m_lTID1, pcdCharacter, ( AgpdItemTemplateEquip* )ppdItemTemplate, pcdItem, pcdItemTemplate, eNextClassType );
	if( ppdItemTemplateEquip->m_nKind == AGPMITEM_EQUIP_KIND_NECKLACE )
	{
		pcmItem->OnEquipItemNecklace( ppdCharacter, ppdItem, pcdItem, eNextClassType );
	}

	return TRUE;
}

BOOL AgcmCharacter::OnEquipCharonForSummoner( AgpdCharacter* ppdCharacter )
{
	if( !ppdCharacter ) return FALSE;

	// ����ó�� 1. �巡��ÿ¿��Ը� �����Ѵ�.. ���� ��ȹ�� ��ȭ��ų�� �巡��ÿ¿��Ը� �ִ�.
	if( m_pcsAgpmFactors->GetRace( &ppdCharacter->m_csFactor ) != AURACE_TYPE_DRAGONSCION ) return FALSE;

	// ����ó�� 2. ���ӳʿ��Ը� ����Ǿ�� �ϴ� �Լ��̴�.
	AuCharClassType eClassType = ( AuCharClassType )m_pcsAgpmFactors->GetClass( &ppdCharacter->m_csFactor );
	if( eClassType != AUCHARCLASS_TYPE_MAGE ) return FALSE;

	// �������� �����۵��� ���鼭 ī�о������� �ִ��� ã�ƺ���.
	AgpmItem* ppmItem = ( AgpmItem* )GetModule( "AgpmItem" );
	AgcmItem* pcmItem = ( AgcmItem* )GetModule( "AgcmItem" );
	AgpmGrid* ppmGrid = ( AgpmGrid* )GetModule( "AgpmGrid" );	
	if( !ppmItem || !ppmGrid || !pcmItem ) return FALSE;

	AgpdItemADChar* ppdItemADChar = ppmItem->GetADCharacter( ppdCharacter );
	AgpdGrid* ppdEquipGrid = &ppdItemADChar->m_csEquipGrid;

	INT32 nSequencialIndex = 0;	
	AgpdGridItem* ppdGridItem = ppmGrid->GetItemSequence( ppdEquipGrid, &nSequencialIndex );
	while( ppdGridItem )
	{
		// �ش� �׸����� �������� ã�´�.
		AgpdItem* ppdItemCurrent = ppmItem->GetItem( ppdGridItem->m_lItemID );
		if( !ppdItemCurrent ) continue;

		AgpdItemTemplateEquip* ppdItemTemplateEquip = ( AgpdItemTemplateEquip* )ppdItemCurrent->m_pcsItemTemplate;
		if( ppdItemTemplateEquip && ppdItemTemplateEquip->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON )
		{
			AgpdItemTemplateEquipWeapon* ppdItemTemplateWeapon = ( AgpdItemTemplateEquipWeapon* )ppdItemTemplateEquip;
			if( ppdItemTemplateWeapon->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_CHARON || 
				ppdItemTemplateWeapon->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CHARON )
			{
				OnEquipItemByEvolution( ppdCharacter, ppdItemCurrent, AUCHARCLASS_TYPE_MAGE );
				return TRUE;
			}
		}

		// ������..
		ppdGridItem = ppmGrid->GetItemSequence( ppdEquipGrid, &nSequencialIndex );
	}

	return TRUE;
}

BOOL AgcmCharacter::OnPolyMorph( void* pCharacter, INT32 nTargetTID )
{
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	AgcdCharacter* pcdCharacter = GetCharacterData( ppdCharacter );
	if( !ppdCharacter || !pcdCharacter || nTargetTID < 0 ) return FALSE;

	BOOL bIsSelfCharacter = FALSE;

	// ���Ŵ�� TID �� ��ȿ���� �˻�
	INT32 nOldTID = ppdCharacter->m_lTID1;
	INT32 nNewTID = nTargetTID;

	//INT32 nOldTID = nTargetTID;
	//INT32 nNewTID = ppdCharacter->m_lTID1;

	AgpdCharacterTemplate* ppdNewTemplate = m_pcsAgpmCharacter->GetCharacterTemplate( nNewTID );
	AgpdCharacterTemplate* ppdOldTemplate = m_pcsAgpmCharacter->GetCharacterTemplate( nOldTID );
	if( !ppdNewTemplate || !ppdOldTemplate ) return FALSE;

	// ���������� �˸���.
	pcdCharacter->m_bTransforming = TRUE;
	pcdCharacter->m_bForceAnimation = FALSE;
	if( ppdCharacter->m_lID == m_lSelfCID )
	{
		bIsSelfCharacter = TRUE;
	}
	
	// ������ TID ����
	ppdCharacter->m_lTID1 = nOldTID;
	ppdCharacter->m_pcsCharacterTemplate = ppdOldTemplate;

	// �𵨸� ������ ����
	DestroyCharacter( ppdCharacter );

	// ���� �÷��� �ʱ�ȭ
	pcdCharacter->m_lStatus = AGCMCHAR_STATUS_FLAG_NONE;

	// TID �� ���ø� ����
	ppdCharacter->m_lTID1 = nNewTID;
	ppdCharacter->m_pcsCharacterTemplate = ppdNewTemplate;

	// �ִϸ��̼� �ʱ⼼��
	pcdCharacter->m_eCurAnimType = AGCMCHAR_ANIM_TYPE_WAIT;
	pcdCharacter->m_lCurAnimType2 = 0;

	// �𵨸� ������ �����
	InitCharacter( ppdCharacter, FALSE );

	// ���ø� �����ϰ� ��ġ ����
	pcdCharacter->m_pstAgcdCharacterTemplate = GetTemplateData( ppdNewTemplate );
	m_pcsAgpmCharacter->UpdatePosition( ppdCharacter, NULL, TRUE, TRUE );

	// �� ĳ���͸� �� ĳ���Ͷ�� ����
	if( bIsSelfCharacter )
	{
		SetSelfCharacter( ppdCharacter );

		// UI �� �������� ��ư ����
		AgcmUICharacter* pcmUICharacter = ( AgcmUICharacter* )GetModule( "AgcmUICharacter" );
		if( pcmUICharacter )
		{
			pcmUICharacter->OnToggleBtnRestoreTransform( ppdCharacter->m_bIsTrasform );
		}
	}

	// �ִϸ��̼�Ÿ�� ����
	AgcmItem* pcmItem = ( AgcmItem* )GetModule( "AgcmItem" );
	if( pcmItem )
	{
		pcmItem->RefreshEquipItems( ppdCharacter );
		pcmItem->SetEquipAnimType( ppdCharacter, pcdCharacter );
	}

	// �����߷Ḧ �˸���.
	pcdCharacter->m_bTransforming = FALSE;
	return TRUE;
}

AuCharClassType AgcmCharacter::GetClassTypeByTID( INT32 nTID )
{
	AgpdCharacterTemplate* ppdTemplate = m_pcsAgpmCharacter->GetCharacterTemplate( nTID );
	if( !ppdTemplate ) return AUCHARCLASS_TYPE_NONE;

	AuCharClassType eType = ( AuCharClassType )m_pcsAgpmFactors->GetClass( &ppdTemplate->m_csFactor );
	return eType;
}

void AgcmCharacter::OnUpdateCharacterCustomize( AgpdCharacter* ppdCharacter, AgcdCharacter* pcdCharacter )
{
	// �� �Լ������� �ƹ�Ÿ ������ Ȥ�� �Ӹ����� �����ۿ� ���� �Ӹ� �� �� ������ ������� �ʴ´�.
	// ���⼭�� ���� Ŀ���͸���¡�� ���� �Ӹ� �� �� ���游�� �����Ѵ�.
	if( !ppdCharacter || !pcdCharacter ) return;

	AuRaceType eRaceType = ( AuRaceType )m_pcsAgpmFactors->GetRace( &ppdCharacter->m_csFactor );
	AuCharClassType eClassType = GetClassTypeByTID( ppdCharacter->m_lTID1 );

	BOOL bIsTransform = ppdCharacter->m_bIsTrasform;
	BOOL bIsEvolution = ppdCharacter->m_bIsEvolution;

	// ���Ż��¿����� Ŀ���͸���¡�� �������� �ʴ´�.
	if( bIsTransform ) return;

	// ���ӳ��� ��� ������ 0�� ���, 0�� �󱼷� �����Ѵ�.
	if( eRaceType == AURACE_TYPE_DRAGONSCION && eClassType == AUCHARCLASS_TYPE_MAGE )
	{
		SetFace( pcdCharacter, 0 );
		SetHair( pcdCharacter, 0 );
		return;
	}

	// ������ �󱼹�ȣ�� ��ȿ�ؾ� �ϸ�
	if( ppdCharacter->m_lFaceIndex >= 0 )
	{
		// ���� ������ �󱼹�ȣ�� ���� ��쿡��
		if( ppdCharacter->m_lFaceIndex == pcdCharacter->m_nAttachFaceID )
		{
			// ���� �ε��� ���� ������ �ε��Ѵ�.
			if( !pcdCharacter->m_pFace )
			{
				SetFace( pcdCharacter, ppdCharacter->m_lFaceIndex );
			}
		}
		else
		{
			SetFace( pcdCharacter, ppdCharacter->m_lFaceIndex );
		}
	}

	// ������ �Ӹ� ��ȣ�� ��ȿ�ؾ� �ϸ�
	if( ppdCharacter->m_lHairIndex >= 0 )
	{
		// ���� ������ �Ӹ���ȣ�� ���� ��쿡��
		if( ppdCharacter->m_lHairIndex == pcdCharacter->m_nAttachHairID )
		{
			// ���� �ε��� �Ӹ��� ������ �ε��Ѵ�.
			if( !pcdCharacter->m_pHair )
			{
				SetHair( pcdCharacter, ppdCharacter->m_lHairIndex );
			}
		}
		else
		{
			SetHair( pcdCharacter, ppdCharacter->m_lHairIndex );
		}
	}

	GeneralizeAttachedAtomicsNormals( pcdCharacter );
	AttachRideClump( pcdCharacter );
}

void AgcmCharacter::_UpdateFactorCharacterType( AgpdCharacter* ppdCharacter, INT32 nNewTID )
{
	if( !ppdCharacter ) return;

	AgpdFactor* ppdFactor = &ppdCharacter->m_csFactor;
	AgpdFactor* ppdFactorResult = ( AgpdFactor* )m_pcsAgpmFactors->GetFactor( ppdFactor, AGPD_FACTORS_TYPE_RESULT );
	if( !ppdFactorResult ) return;

	AgpdCharacterTemplate* ppdCharacterTemplate = m_pcsAgpmCharacter->GetCharacterTemplate( nNewTID );
	if( !ppdCharacterTemplate ) return;

	AgpdFactor* ppdNewFactor = &ppdCharacterTemplate->m_csFactor;
	m_pcsAgpmFactors->SetFactor( ppdFactor, ppdNewFactor, AGPD_FACTORS_TYPE_CHAR_TYPE );
	m_pcsAgpmFactors->SetFactor( ppdFactorResult, ppdNewFactor, AGPD_FACTORS_TYPE_CHAR_TYPE );
}

#if !defined(USE_MFC) && defined( _USE_NPROTECT_GAMEGUARD_ )
BOOL CALLBACK NPGameMonSend(DWORD dwMsg)
{
	return AgcmCharacter::m_pThisAgcmCharacter->SendGameGuardAuth((PVOID)dwMsg, sizeof(GG_AUTH_DATA));
}

BOOL AgcmCharacter::SendGameGuardAuth(PVOID pggData, INT16 ggDataLength)
{
	AgpdCharacter* pcsCharacter = GetSelfCharacter();
	if(!pcsCharacter)
		return FALSE;

	INT16 pPacketLength = 0;

	PVOID pvPacket = m_pcsAgpmCharacter->MakePacketGameguardAuth(pcsCharacter, pggData, ggDataLength, &pPacketLength);
	
	if(pvPacket)
	{
		SendPacket(pvPacket, pPacketLength);
		m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);
	}

	return TRUE;
}
#endif