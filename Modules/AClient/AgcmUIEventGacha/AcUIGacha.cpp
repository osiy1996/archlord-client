
#include <AgcmUIEventGacha/AgcmUIEventGacha.h>   
#include <AgpmCharacter/AgpmCharacter.h>
#include <AgcmItem/AgcmItem.h>
#include <AgpmItem/AgpmItem.h>
#include <AgcModule/AgcEngine.h>
#include <AgcmSound/AgcmSound.h>
#include <AgcmCamera2/AgcuCamPathWork.h>
#include <AgcmCharacter/AgcmCharacter.h>
#include <AgcModule/AgcEngine.h>
#include <AgcmEventGacha/AgcmEventGacha.h>

const INT32 g_cnWidth = 50; // ���� ���� ũ��. 
const FLOAT g_cfDisplayScrollOffset = 1.3f;	// 1�ʿ� �����ִ� ������ ����
static UINT32 s_nGachaRollingTime = cGacha_Rolling_Time; // 10��

INT32	g_nRotateCount	= 7;

static	AgpmItem	*	s_pcsAgpmItem	= NULL;
static	AgcmItem	*	s_pcsAgcmItem	= NULL;

AcUIGacha::AcUIGacha():
	m_nPosition		( 0 ),
	m_nMaxPosition	( 0 ),
	m_uPrevTick		( 0 ),
	m_eRollingMode	( RM_DISPLAY ),
	m_uRollingStartTime( 0 ),
	m_nResultStartOffset( 0 ),
	m_pcsAgcmUIEventGacha( NULL ),
	m_nResultTID			( 0 ),
	m_pSplineData			( NULL ),
	m_pCurrentSlot			( NULL )
{
}

AcUIGacha:: ~AcUIGacha()
{
}

BOOL AcUIGacha::OnInit			()
{
	// ��� ������ Ȯ��.
	s_pcsAgpmItem	= ( AgpmItem * ) g_pEngine->GetModule( "AgpmItem" );
	s_pcsAgcmItem	= ( AgcmItem * ) g_pEngine->GetModule( "AgcmItem" );

	m_uPrevTick			= ::GetTickCount();
	m_uRollingStartTime	= ::GetTickCount();

	return TRUE;
}

BOOL AcUIGacha::OnPostInit()
{
	return TRUE;
}

BOOL	AcUIGacha::SetSlotItems( vector< INT32 > * pVector )
{
	// ���� �ʱ�ȭ
	m_nPosition		= 0;
	m_nMaxPosition	= ( INT32 ) g_cnWidth * ( INT32 ) pVector->size();
	m_uPrevTick		= 0;
	m_eRollingMode	= RM_DISPLAY;
	m_pCurrentSlot	= NULL;

	m_vecSlot.clear();

	vector< INT32 >::iterator	iter;
	for( iter = pVector->begin();
		iter != pVector->end();
		iter++ )
	{
		INT32	nTID = *iter;

		AgpdItemTemplate	* pcsAgpdItemTemplate	 = s_pcsAgpmItem->GetItemTemplate( nTID );
		ASSERT( pcsAgpdItemTemplate );	
		
		AgcdItemTemplate	* pcsAgcdItemTemplate	 = s_pcsAgcmItem->GetTemplateData( pcsAgpdItemTemplate );
		ASSERT( pcsAgcdItemTemplate );

		VERIFY( s_pcsAgcmItem->LoadTemplateTexture( pcsAgpdItemTemplate , pcsAgcdItemTemplate ) );

		RwTexture * pcsTexture = pcsAgcdItemTemplate->m_pTexture;

		// ������ �־ �ȵȴ�.
		ASSERT( pcsTexture );

		SlotMachine	stSlot;
		stSlot.nTID		= nTID		;
		stSlot.pTexture	= pcsTexture;
		stSlot.strName	= pcsAgpdItemTemplate->m_szName;

		m_vecSlot.push_back( stSlot );
	}

	//���� ǥ�õ� �༮ ����
	if( m_vecSlot.size() )
	{
		m_pcsAgcmUIEventGacha->m_stringName = m_vecSlot[ ( 2 ) % m_vecSlot.size() ].strName;
	}
	else
	{
		m_pcsAgcmUIEventGacha->m_stringName = "----";
	}
	return TRUE;
}

BOOL AcUIGacha::OnIdle	( UINT32 ulClockCount	)
{
	ulClockCount	= ::GetTickCount();

	UINT32 uDiff = ulClockCount - m_uPrevTick;
	UINT32	uOffsetTime	= ulClockCount - m_uRollingStartTime;

	switch( m_eRollingMode )
	{
	case RM_DISPLAY:
		{
			const INT32 nCount = ( INT32 ) m_vecSlot.size(); // �������� ����

			m_nPosition = ( UINT32 ) ( ( FLOAT )( uOffsetTime ) * ( FLOAT ) g_cnWidth * g_cfDisplayScrollOffset / 1000.0f );
		}
		break;
	case RM_ROLLING:
		{
			//const INT32 nTargetPosition ;

			// �̰� ��� TID�� Index�� �˾Ƴ��� , ���� ���� ���Ŀ� �ش� �����ǿ� ���߰� �ϱ� ���� �̸� ����صд�.
			// nTargetIndex = �˾Ƽ� ��-�ϱ�;
			// nTargetPosition = nWidth * ( nCount * nRolling + nTargetIndex + 2 ( ��� ���� ���ؼ� ) );

			if( uOffsetTime >= s_nGachaRollingTime )
			{
				m_nPosition = m_nResultPosition;
				m_eRollingMode = RM_RESULT;
				this->m_pcsAgcmUIEventGacha->OnRollEnd();
			}
			else
			{
				double	dTime	= ( double ) uOffsetTime / ( double ) s_nGachaRollingTime;

				BOOL	bShift = g_pEngine->IsLShiftDown();

				if( m_pSplineData )
				{
					// �������� ����Ÿ�� ������ �װ� �̿���.
					RwMatrix	mat;
					m_pSplineData->bOnIdleOffset( mat , uOffsetTime );
					m_nPosition = m_nResultStartOffset + ( INT32 ) ( mat.pos.y * ( double ) ( m_nResultPosition - m_nResultStartOffset )) ;
				}
				else
				{
					if( bShift )
					{
						dTime	= 0.2 + dTime * 0.8;
						double	x	= (1-cos( 3.1415927 * dTime )) * 0.5 ;
						double	x2	= 3.1415927 * x;
						m_nPosition = m_nResultStartOffset + ( INT32 ) ( ( 1.0 - cos( x2 ) ) / 2.0 * ( double ) ( m_nResultPosition - m_nResultStartOffset ));
					}
					else
					{
						m_nPosition = m_nResultStartOffset + ( INT32 ) ( ( 1.0 - cos( 3.1415927 * dTime ) ) / 2.0 * ( double ) ( m_nResultPosition - m_nResultStartOffset ));
					}
				}
			}
		}
		break;
	case RM_RESULT:
		// �ϰ͵� ����...
		// �ɸ� ������ ǥ������?..
		break;
	}

	m_uPrevTick = ulClockCount;

	if( m_vecSlot.size() )
	{
		static INT32 nLastImage = 0;

		INT32 nFirstImage = ( m_nPosition / g_cnWidth ) % m_vecSlot.size();    // ���� ��µ� �̹���

		if( nFirstImage != nLastImage )
		{
			m_pCurrentSlot	= &m_vecSlot[ ( nFirstImage + 2 ) % m_vecSlot.size() ];
			m_pcsAgcmUIEventGacha->m_stringName = m_pCurrentSlot->strName;
			nLastImage = nFirstImage;

			m_pcsAgcmUIEventGacha->UpdateControlItem();

			static	AgcmSound	* pcsAgcmSound = ( AgcmSound * ) g_pEngine->GetModule( "AgcmSound" );

			if( ulClockCount % 2 )	pcsAgcmSound->PlaySampleSound( "SOUND\\UI\\U_LB_A1.wav" );
			else					pcsAgcmSound->PlaySampleSound( "SOUND\\UI\\U_LB_A2.wav" );
		}
	}

	return TRUE;
}

void AcUIGacha::OnWindowRender	()
{
	if( !m_vecSlot.size() ) return;

	UINT8	uAlphaWindow = ( UINT8 ) ( ((m_lColor >> 24) & 0xff) * (m_pfAlpha ? *m_pfAlpha : 1.0f) );

	const INT32 nDisplayCount = 6;
	INT32 nModulatePosition = 0;
	nModulatePosition = m_vecSlot.size() ? m_nPosition % ( INT32 ) ( m_vecSlot.size() * g_cnWidth ) : 200 ;
	const INT32 nScreenOffsetX = 0;		// �̹��� �ɼ�..
	const INT32 nScreenOffsetY = 0;		// �̹��� �ɼ�..

	INT32 nFirstImage = ( m_nPosition / g_cnWidth ) % m_vecSlot.size();    // ���� ��µ� �̹���

	for( INT32 nIndex = nFirstImage ; nIndex < nFirstImage + nDisplayCount ; nIndex ++ ) // ���÷��� ������ŭ �ݺ�
	{
		INT32 nStart = nIndex * g_cnWidth - nModulatePosition ;
		if( nStart + x < 0 ) continue;
		if( nStart + x + g_cnWidth >= 320) continue; // 320�� UI�ϵ��ڵ�

		SlotMachine	* pSlotMachine = &m_vecSlot[ nIndex % m_vecSlot.size() ];

		INT32	nOffsetX , nOffsetZ;
		nOffsetX = nScreenOffsetX + nStart;
		nOffsetZ = nScreenOffsetY;

		this->ClientToScreen(&nOffsetX, &nOffsetZ);

		g_pEngine->DrawIm2DPixel(
			pSlotMachine->pTexture	, 
			nOffsetX				, 
			nOffsetZ				, 
			g_cnWidth				,
			g_cnWidth				,
			0					,
			0					,
			-1					,
			-1					,
			m_lColor			,
			uAlphaWindow );
	}

	const INT32 lCoverImage = 1;
	RwTexture * pTexture = m_stRenderInfo.lRenderID ? m_csTextureList.GetImage_ID( lCoverImage ) : NULL;
	if( pTexture )
	{
		INT32	nOffsetX , nOffsetZ;

		// Ŀ���̹��� ��ġ �ϵ� �ڵ�
		nOffsetX = 0;
		nOffsetZ = 68;

		this->pParent->ClientToScreen(&nOffsetX, &nOffsetZ);

		g_pEngine->DrawIm2DPixel(
			pTexture	, 
			nOffsetX				, 
			nOffsetZ				, 
			316					,	// Ŀ���̹��� ��ġ �ϵ� �ڵ�
			95					,	// Ŀ���̹��� ��ġ �ϵ� �ڵ�
			0					,
			0					,
			-1					,
			-1					,
			m_lColor			,
			uAlphaWindow );
	}

	
	switch( m_eRollingMode )
	{
	default:
		break;

	case RM_RESULT:
		{
			const INT32 lPopupImage = 2;
			RwTexture * pTexture = m_stRenderInfo.lRenderID ? m_csTextureList.GetImage_ID( lPopupImage ) : NULL;

			if( !m_pCurrentSlot ) break;
			RwTexture * pItemImage = m_pCurrentSlot->pTexture;

			if( pTexture )
			{
				// Ŀ���̹��� ��ġ �ϵ� �ڵ�
				INT32 nOffsetX = 107;
				INT32 nOffsetZ = 66;

				this->pParent->ClientToScreen(&nOffsetX, &nOffsetZ);
				RwRaster* pRaster = RwTextureGetRaster( pTexture );
				if( pRaster )
				{
					g_pEngine->DrawIm2DPixel( pTexture, nOffsetX, nOffsetZ, pRaster->width, pRaster->height, 0, 0, -1, -1, m_lColor, uAlphaWindow );
				}

				nOffsetX = 120;
				nOffsetZ = 79;

				this->pParent->ClientToScreen(&nOffsetX, &nOffsetZ);
				RwTextureFilterMode eFilterMode = RwTextureGetFilterMode( pItemImage );
				RwTextureSetFilterMode(pItemImage, rwFILTERLINEAR);

				pRaster = RwTextureGetRaster( pItemImage );
				if( pRaster )
				{
					//g_pEngine->DrawIm2DPixel( pItemImage, nOffsetX, nOffsetZ, pRaster->width, pRaster->height, 0, 0, -1, -1, m_lColor, uAlphaWindow );
					g_pEngine->DrawIm2D( pItemImage, ( float )nOffsetX, ( float )nOffsetZ, 76, 76, 0.0f, 0.0f, 1.0f, 1.0f, m_lColor, uAlphaWindow );
				}

				RwTextureSetFilterMode(pItemImage, eFilterMode);
			}

		}
		break;
	}

}

BOOL    AcUIGacha::StartRoll( INT32 nTIDResult )
{
	if( m_eRollingMode == RM_ROLLING ) return FALSE;

	// �켱 ����ִ� ������ ���..

	INT32	nIndex = 0;
	BOOL	bFound = FALSE;
	vector< SlotMachine >::iterator	iter;
	for( iter = m_vecSlot.begin();
		iter != m_vecSlot.end();
		iter++ , nIndex ++ )
	{
		SlotMachine	* pSlot = &(*iter);

		if( pSlot->nTID == nTIDResult )
		{
			bFound = TRUE;
			break;
		}
	}

	if( !bFound )
	{
		// ���� ���̺� ���� �������� -_-?...
		// �׳� �Ѹ� ȭ�� ���� ����� �ٷ� ǥ����.
		this->m_pcsAgcmUIEventGacha->OnRollEnd();
		return FALSE;
	}

	m_nPosition			= m_nPosition % m_nMaxPosition;
	m_nResultStartOffset= m_nPosition;
	m_nResultPosition	= 
		( 
			m_vecSlot.size() * g_nRotateCount  // �⺻������ ����� ����..
			+ nIndex // �̰� ��ǥ
			- 2			// �̰� �߾� �����ֱ����� �ɼ�
		) * g_cnWidth;

	// ��� ���� �ð� �ʱ�ȭ.,.
	m_eRollingMode		= RM_ROLLING;
	m_uPrevTick			= ::GetTickCount();
	m_uRollingStartTime	= ::GetTickCount();

	m_nResultTID		= nTIDResult;

	{
		AgcmCharacter * pcsAgcmCharacter = ( AgcmCharacter * ) g_pEngine->GetModule( "AgcmCharacter" );
		AgpdCharacter * pcsSelfCharacter = pcsAgcmCharacter->GetSelfCharacter();
		if( pcsSelfCharacter ) 
			pcsSelfCharacter->m_ulSpecialStatus |= AGPDCHAR_SPECIAL_STATUS_DONT_MOVE;

		this->m_pcsAgcmUIEventGacha->m_pcsAgcmEventGacha->SetGachaWindowOpen( TRUE );
	}

	return TRUE;
}

BOOL	AcUIGacha::LoadGachaSetting( const char * pFileName )
{
	if( m_pSplineData )
	{
		delete m_pSplineData;
		m_pSplineData = NULL;
	}

	vector< RwMatrix >	vecMatrix;

	// ���� �ε�
	{
		FILE	 *pFile = fopen( pFileName , "rt" );
		if(! pFile ) return FALSE;

		FLOAT	fXOffset = 0.0f;
		FLOAT	fXIncrement	= 1000.0f;
		INT32	nCount = 0;

		while( !feof( pFile ) )
		{
			RwMatrix	mat;
			RwMatrixSetIdentity( &mat );

			mat.pos.x	= fXOffset + fXIncrement * nCount;
			fscanf( pFile , "%f" , &mat.pos.y );

			vecMatrix.push_back( mat );
		}

		fclose( pFile );
	}

	if( vecMatrix.size() < 4 )
	{
		// ��Ʈ�� �� ����.
		//for_each( vecMatrix.begin() , vecMatrix.end() , fncDeleter() );
		return FALSE; 
	}

	AgcuPathSingleSpline * pSpline = new AgcuPathSingleSpline( s_nGachaRollingTime );

	pSpline->bNoLoop();
	pSpline->bReset();
	pSpline->bForward();
	pSpline->bSetActType(1);		//flight
	pSpline->bSetAccel(0);			//accel

	stSetSpline	stSet;
	stSet.closed		= false;
	stSet.ctrlMatrices	= &vecMatrix[ 0 ];
	stSet.numCtrl		= vecMatrix.size();

	pSpline->bInitSpline(stSet);

	m_pSplineData = pSpline;

	{
		// ResultLog
		FILE	*pFile = fopen( "RollResult.txt" , "wt" );
		for( UINT32 uTime = 0 ; uTime < s_nGachaRollingTime ; uTime += 100 )
		{
			RwMatrix	mat;
			m_pSplineData->bOnIdleOffset( mat , uTime );

			fprintf( pFile , "%d\t%f\n" , uTime , mat.pos.y ); 
		}
		fclose( pFile );
	}

	return TRUE;
}

void AcUIGacha::OnCloseUI()
{
	if( m_eRollingMode == RM_ROLLING )
	{
		AgcmCharacter * pcsAgcmCharacter = ( AgcmCharacter * ) g_pEngine->GetModule( "AgcmCharacter" );
		AgpdCharacter * pcsSelfCharacter = pcsAgcmCharacter->GetSelfCharacter();
		if( pcsSelfCharacter ) 
			pcsSelfCharacter->m_ulSpecialStatus &= ~AGPDCHAR_SPECIAL_STATUS_DONT_MOVE;

		this->m_pcsAgcmUIEventGacha->m_pcsAgcmEventGacha->SetGachaWindowOpen( FALSE );
	}
}
