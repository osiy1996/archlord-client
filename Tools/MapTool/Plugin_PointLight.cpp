// Plugin_PointLight.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "MapTool.h"
#include "Mainfrm.h"
#include "Plugin_PointLight.h"
#include "MainWindow.h"

#define POINT_LIGHT_OBJECT_INDEX	1684

extern AgcmDynamicLightmap*	g_pcsAgcmDynamicLightmap;
extern MainWindow			g_MainWindow	;

BOOL __ObjectLoadCallback_AdjustPosition(PVOID pData, PVOID pClass, PVOID pCustData);

// CPlugin_PointLight

IMPLEMENT_DYNAMIC(CPlugin_PointLight, CWnd)
CPlugin_PointLight::CPlugin_PointLight()
{
	m_pSphereAtomic		= NULL	;
	m_nSelectedPoint	= -1	;

	m_nSelectedAxis		= CAxisManager::AXIS_NOSELECTED;
	m_fSelectedValue1	= 0.0f;
	m_fSelectedValue2	= 0.0f;

	m_strShortName = "PLight";
}

CPlugin_PointLight::~CPlugin_PointLight()
{
}


BEGIN_MESSAGE_MAP(CPlugin_PointLight, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CPlugin_PointLight �޽��� ó�����Դϴ�.

BOOL CPlugin_PointLight::OnWindowRender			()
{
	PLOInfo	* pInfo;
	RwFrame	* pFrame;

	// ��ġ ǥ��.
	for( int i = 0 ; i < ( int ) m_listLight.size() ; i ++ )
	{
		pInfo = &m_listLight[ i ];
		pFrame = RpLightGetFrame( pInfo->pLight );

		DrawSphere( &pFrame->ltm.pos );
	}

	// ���� �� �༮ ǥ��..
	if( m_nSelectedPoint != -1 )
	{
		pInfo = &m_listLight[ m_nSelectedPoint ];
		pFrame = RpLightGetFrame( pInfo->pLight );

		if( ISBUTTONDOWN( VK_CONTROL ) )
		{
			g_MainWindow.DrawAreaSphere( pFrame->ltm.pos.x , pFrame->ltm.pos.y , pFrame->ltm.pos.z , RpLightGetRadius( pInfo->pLight ) );
		}
		else
		{
			m_cAxis.Render();
		}
	}


	return TRUE;
}

BOOL CPlugin_PointLight::Window_OnLButtonDown	( RsMouseStatus *ms )
{
	PLOInfo	* pInfo;
	RwFrame	* pFrame;

	if( m_nSelectedPoint != -1 )
	{
		pInfo = &m_listLight[ m_nSelectedPoint ];
		pFrame = RpLightGetFrame( pInfo->pLight );

		if( ISBUTTONDOWN( VK_CONTROL ) )
		{
			// ���� ����
			RwV3d	pos;
			INT32	nPolygonIndex;
			if( AGCMMAP_THIS->GetMapPositionFromMousePosition_tool(
				( INT32 ) ms->pos.x , ( INT32 ) ms->pos.y , &pos.x , &pos.y , &pos.z , &nPolygonIndex ) )
			{
				FLOAT	fDistance;
				fDistance = AUPOS_DISTANCE_XZ( pos , pFrame->ltm.pos );
				RpLightSetRadius( pInfo->pLight , fDistance );

				RadiusUpdate();
			}

			return TRUE;
		}
		else
		{
			RwLine			pixelRay		;
			RwCameraCalcPixelRay( g_pEngine->m_pCamera , &pixelRay , &ms->pos );
			// �̹̼��É°� ������..
			// ���� ���É°� �ƴ��� ����..
			switch( m_nSelectedAxis = m_cAxis.GetCollisionAxis( g_pEngine->m_pCamera , ms ) )
			{
			case CAxisManager::AXISX:
				{
					TRACE( "X��\n" );

					// �ݸ����� ��ġ�� ����..�׸��� ���� ���� ������..
					// xy ���� �ݸ���..
					RwV3d	pos;

					FLOAT fPlaneZ = pFrame->ltm.pos.z;
					pos.z = fPlaneZ;

					// �ش�鿡���� �ݸ��� ��ġ ���ϱ�..

					// x�� ���ϱ�..
					// x = ( x2 - x1 ) / ( y2 - y1 ) * ( y - y1 ) + x1;

					pos.x = ( pixelRay.end.x - pixelRay.start.x ) /
							( pixelRay.end.z - pixelRay.start.z ) * 
							( fPlaneZ - pixelRay.start.z ) + 
							pixelRay.start.x;

					m_fSelectedValue1 = pos.x;
					m_fSelectedValue2 = pFrame->ltm.pos.x;
				}
				return TRUE;
			case CAxisManager::AXISY:
				{
					TRACE( "Y��\n" );
					// xy���..
					RwV3d	pos;

					FLOAT fPlaneZ = pFrame->ltm.pos.z;
					pos.z = fPlaneZ;

					// �ش�鿡���� �ݸ��� ��ġ ���ϱ�..

					// x�� ���ϱ�..
					// x = ( x2 - x1 ) / ( y2 - y1 ) * ( y - y1 ) + x1;

					pos.y = ( pixelRay.end.y - pixelRay.start.y ) /
							( pixelRay.end.z - pixelRay.start.z ) * 
							( fPlaneZ - pixelRay.start.z ) + 
							pixelRay.start.y;

					m_fSelectedValue1 = pos.y;
					m_fSelectedValue2 = pFrame->ltm.pos.y;
				}
				return TRUE;
			case CAxisManager::AXISZ:
				{
					TRACE( "Z��\n" );
					// xz ���..
					RwV3d	pos;

					FLOAT fPlaneY = pFrame->ltm.pos.y;
					pos.y = fPlaneY;

					// �ش�鿡���� �ݸ��� ��ġ ���ϱ�..

					// x�� ���ϱ�..
					// x = ( x2 - x1 ) / ( y2 - y1 ) * ( y - y1 ) + x1;

					pos.z = ( pixelRay.end.z - pixelRay.start.z ) /
							( pixelRay.end.y - pixelRay.start.y ) * 
							( fPlaneY - pixelRay.start.y ) + 
							pixelRay.start.z;

					m_fSelectedValue1 = pos.z;
					m_fSelectedValue2 = pFrame->ltm.pos.z;
				}
				return TRUE;
			default:
				break;
			}
		}
	}

	INT32	nFound = GetCollisionIndex( ms );
	m_nSelectedPoint = nFound;

	if( -1 == m_nSelectedPoint )
	{
		// �ƹ� �͵� �����ϱ�..
		// ���� �ϳ� �����ұ�?..

		RwV3d	pos;
		INT32	nPolygonIndex;
		if( AGCMMAP_THIS->GetMapPositionFromMousePosition_tool(
			( INT32 ) ms->pos.x , ( INT32 ) ms->pos.y , &pos.x , &pos.y , &pos.z , &nPolygonIndex ) )
		{
			// ������ (2005-05-19 ���� 8:27:23) : 
			// ������Ʈ �߰� & �̺�Ʈ ��� ����.

			/*
			// ����Ʈ �ϳ� �߰�
			RpLight	* pLight = RpLightCreate( rpLIGHTPOINT );
			RwRGBAReal	rgb;

			rgb.red		= ( FLOAT ) m_ctlRGBSelectStatic.m_nR / 255.0f;
			rgb.green	= ( FLOAT ) m_ctlRGBSelectStatic.m_nG / 255.0f;
			rgb.blue	= ( FLOAT ) m_ctlRGBSelectStatic.m_nB / 255.0f;
			rgb.alpha	= 1.0f;

			RpLightSetColor( pLight , &rgb );
			RpLightSetRadius( pLight , 6400.0f );

			RwFrame	* pFrame = RwFrameCreate();
			RpLightSetFrame( pLight , pFrame );

			pos.y	+= 50.0f; // 50��ġ ��
			RwFrameTranslate( pFrame , &pos , rwCOMBINEREPLACE );

			m_listLight.push_back( pLight );

			if( g_pcsAgcmRender->AddLightToRenderWorld( pLight , TRUE ) 
				//&& g_pcsAgcmDynamicLightmap->addLight( pLight , TRUE )
				)
			{
				SetSaveData();
			}
			else
			{
				MD_SetErrorMessage( "����Ʈ �߰� ����" );
			}
			*/

			BOOL	bObjectOnly = TRUE;
			switch( MessageBox( "����Ʈ�� ������Ʈ���� ������ �ְ� �ұ��?" , "Point Light" , MB_YESNOCANCEL ) )
			{
			case IDNO:
				bObjectOnly = FALSE;
				// break; // ���� �극��ũ ���°� �´°���!
			case IDYES:
				{
					// ������Ʈ �߰� ����..
					ApdObjectTemplate	*	pstApdObjectTemplate	;
					ApdObject			*	pstApdObject			;

					pstApdObjectTemplate = g_pcsApmObject->GetObjectTemplate( POINT_LIGHT_OBJECT_INDEX );
					if( !pstApdObjectTemplate )
					{
						MessageBox( "���ø� ID���� ���ø� ���� ��� ����..\n" );
						return TRUE;
					}

					pstApdObject = g_pcsApmObject->AddObject( 
						g_pcsApmObject->GetEmptyIndex( GetDivisionIndexF( pos.x , pos.z ) ) , pstApdObjectTemplate->m_lID );
					ASSERT( NULL != pstApdObject );
					if( !pstApdObject )
					{
						MessageBox( "g_pcsApmObject->AddObject ������Ʈ �߰�����.." );
						return TRUE;
					}

					// ��ġ����..
					pstApdObject->m_stPosition.x	= pos.x;
					pstApdObject->m_stPosition.z	= pos.z;
					pstApdObject->m_stPosition.y	= pos.y + 50.0f;	// 50�� ����Ʈ �ɼ�..

					g_pcsApmObject->UpdateInit( pstApdObject );

					try
					{
						ApdObject * pObject = pstApdObject;
						ApdEvent *	pstEvent;

						// EventNature Event Struct ��..
						pstEvent	= g_pcsApmEventManager->GetEvent( pObject , APDEVENT_FUNCTION_POINTLIGHT );

						if( NULL == pstEvent )
						{
							// �̺�Ʈ ����..
							ApdEventAttachData *pstEventAD = g_pcsApmEventManager->GetEventData( pObject );

							pstEvent = g_pcsApmEventManager->AddEvent(pstEventAD, APDEVENT_FUNCTION_POINTLIGHT , pObject , FALSE);
						}

						if( pstEvent )
						{
							AgpdPointLight		*pstAgpdPLight	= (AgpdPointLight *)(pstEvent->m_pvData);
							AgcdPointLight		*pstAgcdPLight	= g_pcsAgcmEventPointLight->GetPointLightClientData(pstAgpdPLight);

							// �ɼǺ���..
							if( bObjectOnly )	pstAgcdPLight->nEnableFlag	= AGCPLF_AFFECT_OBJECT | AGCPLF_ENABLE_ALLTIME;
							else				pstAgcdPLight->nEnableFlag	= AGCPLF_AFFECT_OBJECT | AGCPLF_AFFECT_GEOMETRY | AGCPLF_ENABLE_ALLTIME;

							// Į�� ����..
							pstAgcdPLight->uRed		= ( UINT8 ) m_ctlRGBSelectStatic.m_nR;
							pstAgcdPLight->uGreen	= ( UINT8 ) m_ctlRGBSelectStatic.m_nG;
							pstAgcdPLight->uBlue	= ( UINT8 ) m_ctlRGBSelectStatic.m_nB;

							// ����Ʈ �߰�.
							//g_pcsAgcmEventPointLight->AddLight( pstAgcdPLight , &pstApdObject->m_stPosition );

							if( g_pcsAgcmEventPointLight->AddLightToArray( pstAgcdPLight , pstApdObject ) &&
								g_pcsAgcmEventPointLight->m_bEnablePointLight )
							{
								g_pcsAgcmEventPointLight->AddLight( pstAgcdPLight , &pstApdObject->m_stPosition );
							}

							// �ʱⰪ ����..

							if( pstAgcdPLight->pLight )
							{
								PLOInfo	stInfo;
								stInfo.nOID		= pstApdObject->m_lID	;
								stInfo.pLight	= pstAgcdPLight->pLight	;
								stInfo.pstInfo	= pstAgcdPLight			;

								// ��̿� �߰�.
								m_listLight.push_back( stInfo );
							}
							else
							{
								// ���� ���� �߻�
								g_pcsApmObject->DeleteObject( pObject );
							}
						}
					}
					catch(...)
					{
						MessageBox( "�̺�Ʈ �߰��߿� ũ���� �߻�.." );
					}
				}
				break;
			default:
				// do nothing.
				break;
			}
		}
	}
	else
	{
		pInfo = &m_listLight[ m_nSelectedPoint ];
		pFrame = RpLightGetFrame( pInfo->pLight );

		// ���É³༮

		static FLOAT sfScale = 1000.0f;

		//m_cAxis.Init();
		m_cAxis.SetScale( sfScale );
		m_cAxis.SetPosition( &pFrame->ltm.pos );

		const RwRGBAReal	*pRgb;
		pRgb = RpLightGetColor( pInfo->pLight );

		m_ctlRGBSelectStatic.SetRGB( ( int ) ( pRgb->red * 255.0f ) , ( int ) ( pRgb->green * 255.0f ) , ( int ) ( pRgb->blue * 255.0f ) );

		Invalidate( FALSE );
	}
	return TRUE;
}

BOOL CPlugin_PointLight::Window_OnLButtonUp		( RsMouseStatus *ms )
{
	m_nSelectedAxis = CAxisManager::AXIS_NOSELECTED;

	return TRUE;
}

BOOL CPlugin_PointLight::Window_OnMouseMove		( RsMouseStatus *ms )
{
	PLOInfo * pInfo;
	RwFrame	* pFrame;

	if( m_nSelectedPoint != -1 && m_nSelectedAxis != CAxisManager::AXIS_NOSELECTED )
	{
		pInfo = &m_listLight[ m_nSelectedPoint ];
		pFrame = RpLightGetFrame( pInfo->pLight );

		RwLine			pixelRay		;
		RwCameraCalcPixelRay( g_pEngine->m_pCamera , &pixelRay , &ms->pos );

		switch( m_nSelectedAxis )
		{
		case CAxisManager::AXISX:
			{
				TRACE( "X��\n" );

				// �ݸ����� ��ġ�� ����..�׸��� ���� ���� ������..
				// xy ���� �ݸ���..
				RwV3d	pos = pFrame->ltm.pos;

				FLOAT fPlaneZ = pFrame->ltm.pos.z;

				// �ش�鿡���� �ݸ��� ��ġ ���ϱ�..

				// x�� ���ϱ�..
				// x = ( x2 - x1 ) / ( y2 - y1 ) * ( y - y1 ) + x1;

				pos.x = ( pixelRay.end.x - pixelRay.start.x ) /
						( pixelRay.end.z - pixelRay.start.z ) * 
						( fPlaneZ - pixelRay.start.z ) + 
						pixelRay.start.x;

				// pFrame->ltm.pos.x = m_fSelectedValue2 + fDelta;
				RwFrameTranslate( pFrame	, &pos	, rwCOMBINEREPLACE	);

			}
			break;
		case CAxisManager::AXISY:
			{
				TRACE( "Y��\n" );
				// xy���..
				RwV3d	pos = pFrame->ltm.pos;

				FLOAT fPlaneZ = pFrame->ltm.pos.z;
				// �ش�鿡���� �ݸ��� ��ġ ���ϱ�..

				// x�� ���ϱ�..
				// x = ( x2 - x1 ) / ( y2 - y1 ) * ( y - y1 ) + x1;

				pos.y = ( pixelRay.end.y - pixelRay.start.y ) /
						( pixelRay.end.z - pixelRay.start.z ) * 
						( fPlaneZ - pixelRay.start.z ) + 
						pixelRay.start.y;

				RwFrameTranslate( pFrame	, &pos	, rwCOMBINEREPLACE	);
			}
			break;
		case CAxisManager::AXISZ:
			{
				TRACE( "Z��\n" );
				// xz ���..
				RwV3d	pos = pFrame->ltm.pos;

				FLOAT fPlaneY = pFrame->ltm.pos.y;

				// �ش�鿡���� �ݸ��� ��ġ ���ϱ�..

				// x�� ���ϱ�..
				// x = ( x2 - x1 ) / ( y2 - y1 ) * ( y - y1 ) + x1;

				pos.z = ( pixelRay.end.z - pixelRay.start.z ) /
						( pixelRay.end.y - pixelRay.start.y ) * 
						( fPlaneY - pixelRay.start.y ) + 
						pixelRay.start.z;

				RwFrameTranslate( pFrame	, &pos	, rwCOMBINEREPLACE	);
			}
			break;
		default:
			break;
		}

		m_cAxis.SetPosition( &pFrame->ltm.pos );

		PositionUpdate();
	}
	return FALSE;
}

BOOL CPlugin_PointLight::Window_OnKeyDown		( RsKeyStatus *ks )
{
    switch( ks->keyCharCode )
    {
	case rsDEL:
		{
			// ������ (2005-05-19 ���� 8:32:36) : 
			// �������� ���⼭..

			if( m_nSelectedPoint != -1 )
			{
				SetSaveData();

				PLOInfo * pInfo;

				vector< PLOInfo >::iterator iter = m_listLight.begin() + m_nSelectedPoint;

				pInfo = &(*iter);

				// ������Ʈ ����.
				g_pcsApmObject->DeleteObject( pInfo->nOID );


				m_listLight.erase( iter );
				m_nSelectedPoint = -1;
			}
		}
		break;
	case rsESC:
		{
			// �������.
			m_nSelectedPoint = -1;
		}
		break;
	default:
		break;
	}
	return TRUE;
}

void		CPlugin_PointLight::DrawSphere( RwV3d	* pPos )
{
	TranslateSphere( pPos );
	RpAtomicRender( m_pSphereAtomic );	
}

RpAtomic *	CPlugin_PointLight::TranslateSphere( RwV3d * pPos )
{
	RwFrame		*	pFrame	;
	RwV3d			scale	;
	
	VERIFY( pFrame		=	RpAtomicGetFrame( m_pSphereAtomic ) );

	 FLOAT _fScale = g_pMainFrame->m_Document.m_fBrushRadius;
	scale.x		=	_fScale	;
	scale.y		=	_fScale	;
	scale.z		=	_fScale	;

	RwFrameScale		( pFrame , &scale	, rwCOMBINEREPLACE		);
	RwFrameTranslate	( pFrame , pPos		, rwCOMBINEPOSTCONCAT	);

	return m_pSphereAtomic;
}

INT32		CPlugin_PointLight::GetCollisionIndex( RsMouseStatus *ms )
{
	// ī�޶� ���� ���ؼ� ������ ����..

	// ���� Ŀ�� ��ġ�� �ִ� Ŭ������ ���س���.
	RwLine			pixelRay		;
	RwCameraCalcPixelRay( g_pEngine->m_pCamera , &pixelRay , &ms->pos );

	INT32	nFound			= -1	;
	FLOAT	fFoundDistance	= 0.0f	;
	FLOAT	fDistance				;
	RwSphere	sphere;

	sphere.radius	= g_pMainFrame->m_Document.m_fBrushRadius;

	PLOInfo	* pInfo;
	RwFrame	* pFrame;

	for( int i = 0 ; i < ( int ) m_listLight.size() ; i ++ )
	{
		pInfo = &m_listLight[ i ];
		pFrame = RpLightGetFrame( pInfo->pLight );

		sphere.center	= pFrame->ltm.pos;

		if( RtLineSphereIntersectionTest( &pixelRay , &sphere , &fDistance ) )
		{
			// ����..
			if( nFound == -1 ||
				fDistance < fFoundDistance )
			{
				// ����
				nFound			= i			;
				fFoundDistance	= fDistance	;
			}
		}
	}

	return nFound;
}

int CPlugin_PointLight::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CUITileList_PluginBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	// ���Ǿ� ����..
	{
		VERIFY( m_pSphereAtomic = g_pcsAgcmEventNature->CreateSkyAtomic() );

		// ��Ƽ���� ����..
		RpGeometry	*	pGeometry	;
		RpMaterial	*	pMaterial	;
		RwRGBA			newRGBA		;
		const RwRGBA*	pRGBA		;

		// ���İ� ����..
		INT32 nAlpha = 255;

		VERIFY( pGeometry = RpAtomicGetGeometry( m_pSphereAtomic ) );

		RpGeometryLock( pGeometry, rpGEOMETRYLOCKPRELIGHT );
		RpGeometrySetFlags( pGeometry, RpGeometryGetFlags( pGeometry ) | rpGEOMETRYMODULATEMATERIALCOLOR );

		VERIFY( pMaterial = RpGeometryGetMaterial( pGeometry , 0 ) );

		pRGBA			= RpMaterialGetColor( pMaterial )	;
		newRGBA			= * pRGBA							;

		newRGBA.alpha	= nAlpha							;
		newRGBA.green	= 0;
		newRGBA.blue	= 0;
		newRGBA.red		= 255;

		RpMaterialSetColor		( pMaterial	, &newRGBA	);

		RpGeometryUnlock( pGeometry );
	}

	m_cAxis.Init();

	CRect	rect;
	rect.SetRect( 0 , 20 , 170 , 20 + 89 );

	m_ctlRGBSelectStatic.Create( "RGB����â" , WS_CHILD | WS_VISIBLE | SS_NOTIFY , rect , this	);
	m_ctlRGBSelectStatic.Init( CHSVControlStatic::CIRCLETYPE , RGB( 128 , 128 , 128 )			);

	// ������ (2005-05-19 ���� 5:23:01) : //
	// �ݹ���..
	g_pcsAgpmEventPointLight->SetCallback_AddLight		( CBOnPointLightAddObject		, this );
	g_pcsAgpmEventPointLight->SetCallback_RemoveLight	( CBOnPointLightRemoveObject	, this );

	return 0;
}

void CPlugin_PointLight::OnDestroy()
{
	CUITileList_PluginBase::OnDestroy();

	if( m_pSphereAtomic )
	{
		RpAtomicDestroy( m_pSphereAtomic );
		m_pSphereAtomic = NULL;
	}

	RemoveAllLights();
}

void CPlugin_PointLight::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	dc.SetBkMode( TRANSPARENT );

	if( m_nSelectedPoint != -1 )
	{
		PLOInfo	* pInfo;

		pInfo = &m_listLight[ m_nSelectedPoint ];

		CString	str;

		str.Format( "�ݰ� %.0f m" , RpLightGetRadius( pInfo->pLight ) / 100.0f );
		dc.SetTextColor	( RGB( 255 , 255 , 168 )	);
		dc.TextOut( 0 , 0 , str );
	}
	// �׸��� �޽����� ���ؼ��� CUITileList_PluginBase::OnPaint()��(��) ȣ������ ���ʽÿ�.
}

LRESULT CPlugin_PointLight::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	switch( message )
	{
	case WM_HSVCONTROL_NOTIFY:
		{
			if( m_nSelectedPoint != -1 )
			{
				PLOInfo	* pInfo;

				pInfo = &m_listLight[ m_nSelectedPoint ];

				RwRGBAReal	rgb;

				rgb.red		= ( FLOAT ) m_ctlRGBSelectStatic.m_nR / 255.0f;
				rgb.green	= ( FLOAT ) m_ctlRGBSelectStatic.m_nG / 255.0f;
				rgb.blue	= ( FLOAT ) m_ctlRGBSelectStatic.m_nB / 255.0f;
				rgb.alpha	= 1.0f;

				RpLightSetColor( pInfo->pLight , &rgb );

				ColorUpdate();
			}
		}
		break;
	}

	return CUITileList_PluginBase::WindowProc(message, wParam, lParam);
}

BOOL CPlugin_PointLight::OnQuerySaveData		( char * pStr )
{
	strcpy( pStr , "����Ʈ ����Ʈ ����" );

	return TRUE;
}

BOOL CPlugin_PointLight::CBPointLightFilter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	ASSERT( NULL != pData );
	if( NULL == pData ) return FALSE;

	ApdObject *			pcsApdObject = ( ApdObject * ) pData;

	// ������ (2004-07-05 ���� 5:23:48) : �̺�Ʈ ���°� ó������ ����..
	if( !g_pcsApmEventManager->CheckEventAvailable( pcsApdObject ) )
		return FALSE;

	// ����Ʈ ����Ʈ �� ����..
	if( g_pcsApmEventManager->GetEvent( pcsApdObject , APDEVENT_FUNCTION_POINTLIGHT ) )
		return TRUE;
	else
		return FALSE;
}

BOOL	CPlugin_PointLight::CBSaveDivision( DivisionInfo * pDivisionInfo , PVOID pData )
{
	char	*pSubDir = ( char * ) pData;
	char	strFilename[ 1024 ];

	// OBDN : Object Division Normal
	// OBDS : Object Division Static

	//TRACE( "���� �ͽ���Ʈ '%s'\b" , strFilename );

	AuBOX bbox;
	bbox.inf.x	=	pDivisionInfo->fStartX;
	bbox.inf.z	=	pDivisionInfo->fStartZ;
	bbox.inf.y	=	0.0f;
	bbox.sup.x	=	pDivisionInfo->fStartX + pDivisionInfo->fWidth;
	bbox.sup.z	=	pDivisionInfo->fStartZ + pDivisionInfo->fWidth;
	bbox.sup.y	=	0.0f;

	wsprintf( strFilename , "%s\\" APMOBJECT_LOCAL_INI_FILE_NAME_2_PLIGHT ,
		ALEF_CURRENT_DIRECTORY , GetDivisionIndex( pDivisionInfo->nX , pDivisionInfo->nZ ) );

	if( g_pcsApmObject->StreamWrite( bbox , 0 , strFilename, CPlugin_PointLight::CBPointLightFilter )	)
	{
		// ����
		// do nothing..
	}
	else
	{
		// ����
		MD_SetErrorMessage("ERROR : g_pcsApmObject->StreamWrite( bbox , 0 , %s , FALSE )\n" , strFilename );
		return FALSE;
	}

	{
		CUITileList_ObjectTabWnd * pWnd = g_pMainFrame->m_pTileList->m_pObjectWnd;
		//////////////////////////////////////////////////////////////////////////
		// ������ (2004-11-29 ���� 12:03:07) : 
		// SubData ����..
		wsprintf( strFilename , "%s\\" APMOBJECT_LOCAL_INI_FILE_NAME_2_PLIGHT ,
			pSubDir ,
			GetDivisionIndex( pDivisionInfo->nX , pDivisionInfo->nZ ) );

		if( g_pcsApmObject->StreamWrite( bbox , 0 , strFilename, CPlugin_PointLight::CBPointLightFilter )	)
		{
			// ����
			// do nothing..
		}
		else
		{
			// ����
			MD_SetErrorMessage("ERROR : g_pcsApmObject->StreamWrite( bbox , 0 , %s , FALSE )\n" , strFilename );
			return FALSE;
		}
	}

	return TRUE;
}

BOOL	CPlugin_PointLight::CBLoadDivision( DivisionInfo * pDivisionInfo , PVOID pData )
{
	//nTryCount++;
	//wsprintf( strMessage , "(Div %04d) ����Ʈ����Ʈ ������Ʈ ���� �ε�" , uDivision );
	//_ProgressCallback( strMessage , pResult->dlgProgress.m_nCurrent + 1 , pResult->dlgProgress.m_nTarget , ( void * ) &pResult->dlgProgress );
	char	strFilename[ 1024 ];
	UINT32		uDivision	= GetDivisionIndex( pDivisionInfo->nX , pDivisionInfo->nZ );

	CFileFind	ff;

	wsprintf( strFilename , APMOBJECT_LOCAL_INI_FILE_NAME_2_PLIGHT , uDivision );
	if( ff.FindFile( strFilename ) )
	{
		VERIFY( g_pcsApmObject->StreamRead( strFilename , FALSE , __ObjectLoadCallback_AdjustPosition , TRUE	) 	);
		//nSuccessCount++;
	}
	else
	{
		// MD_SetErrorMessage( "(Div %04d) ����Ʈ����Ʈ ������Ʈ ���� �� �����." , uDivision );
	}

	return TRUE;
}

BOOL CPlugin_PointLight::OnSaveData				()
{
	/*
	RpLight	* pLight;
	RwFrame	* pFrame;

	FILE	*pFile;
	pFile = fopen( "map\\data\\Plight.dat" , "wb" );
	if( NULL == pFile ) return FALSE;

	FLOAT	fRadius;
	const RwRGBAReal	*pColor;
	for( int i = 0 ; i < ( int ) m_listLight.size() ; i ++ )
	{
		pLight = m_listLight[ i ];
		pFrame = RpLightGetFrame( pLight );
		
		fRadius	= RpLightGetRadius( pLight );
		pColor	= RpLightGetColor( pLight );

		fwrite( ( void * ) &pFrame->ltm.pos , sizeof RwV3d , 1 , pFile );
		fwrite( ( void * ) &fRadius , sizeof FLOAT , 1 , pFile );
		fwrite( ( void * ) pColor , sizeof RwRGBAReal , 1 , pFile );
	}

	fclose( pFile );
	*/

	// ����� ����
	char	strSub[ 1024 ];
	GetSubDataDirectory( strSub );

	_CreateDirectory( strSub );
	_CreateDirectory( "%s\\Map"								, strSub );
	_CreateDirectory( "%s\\Map\\Data"						, strSub );
	_CreateDirectory( "%s\\Map\\Data\\Object"				, strSub );
	_CreateDirectory( "%s\\Map\\Data\\Object\\PointLight"	, strSub );

	AGCMMAP_THIS->EnumLoadedDivision( CPlugin_PointLight::CBSaveDivision , ( void * ) strSub );

	return TRUE;
}

BOOL	CPlugin_PointLight::CBOnPointLightAddObject		( PVOID pData, PVOID pClass, PVOID pCustData)
{
	// ����Ʈ ������Ʈ�� �ε� �ƴ�.
	// �ش� ������Ʈ�� ���� ����Ʈ���� �����Ѵ�.

	CPlugin_PointLight	*pThis			= ( CPlugin_PointLight * )(pClass);
	ApdObject			*pcsApdObject	= ( ApdObject			* ) pData	;

	ASSERT( NULL != pThis );
	ASSERT( NULL != pcsApdObject );

	// �̺�Ʈ ��Ʈ���ĵ� ��..
	ApdEvent *	pstEvent	;
	pstEvent	= g_pcsApmEventManager->GetEvent( pcsApdObject , APDEVENT_FUNCTION_POINTLIGHT );

	if( pstEvent )
	{
		AgpdPointLight		*pstAgpdPLight	= (AgpdPointLight *)(pstEvent->m_pvData);
		AgcdPointLight		*pstAgcdPLight	= g_pcsAgcmEventPointLight->GetPointLightClientData(pstAgpdPLight);

		if( pstAgcdPLight->pLight )
		{
			// ����Ʈ�� ����� �Ǿ��ֳ�~

			PLOInfo	stInfo;
			stInfo.nOID		= pcsApdObject->m_lID	;
			stInfo.pLight	= pstAgcdPLight->pLight	;
			stInfo.pstInfo	= pstAgcdPLight			;

			// ��̿� �߰�.
			pThis->m_listLight.push_back( stInfo );
		}
	}

	return TRUE;
}
BOOL	CPlugin_PointLight::CBOnPointLightRemoveObject	( PVOID pData, PVOID pClass, PVOID pCustData)
{
	CPlugin_PointLight	*pThis			= ( CPlugin_PointLight * )(pClass);
	ApdObject			*pcsApdObject	= ( ApdObject			* ) pData	;

	ASSERT( NULL != pThis );
	ASSERT( NULL != pcsApdObject );

	// �̺�Ʈ ��Ʈ���ĵ� ��..
	ApdEvent *	pstEvent	;
	pstEvent	= g_pcsApmEventManager->GetEvent( pcsApdObject , APDEVENT_FUNCTION_POINTLIGHT );

	if( pstEvent )
	{
		AgpdPointLight		*pstAgpdPLight	= (AgpdPointLight *)(pstEvent->m_pvData);
		AgcdPointLight		*pstAgcdPLight	= g_pcsAgcmEventPointLight->GetPointLightClientData(pstAgpdPLight);

		if( pstAgcdPLight->pLight )
		{
			// ����Ʈ�� ����� �Ǿ��ֳ�~

			// ��̿��� ����.
			pThis->RemoveLightInfo( pstAgcdPLight->pLight );
		}
	}

	return TRUE;
}


BOOL CPlugin_PointLight::OnLoadData				()
{
	RemoveAllLights();

	/*
	FILE	*pFile;
	pFile = fopen( "map\\data\\Plight.dat" , "rb" );
	if( NULL == pFile ) return FALSE;

	RwV3d		pos		;
	FLOAT		fRadius	;
	RwRGBAReal	color	;
	int count = 0;
	do
	{
		count = 0;
		count += fread( ( void * ) &pos , sizeof RwV3d , 1 , pFile );
		count += fread( ( void * ) &fRadius , sizeof FLOAT , 1 , pFile );
		count += fread( ( void * ) &color , sizeof RwRGBAReal , 1 , pFile );

		if( count == 3 )
		{
			// ����Ʈ �ϳ� �߰�
			RpLight	* pLight = RpLightCreate( rpLIGHTPOINT );

			RpLightSetColor( pLight , &color );
			RpLightSetRadius( pLight , fRadius );

			RwFrame	* pFrame = RwFrameCreate();
			RpLightSetFrame( pLight , pFrame );

			RwFrameTranslate( pFrame , &pos , rwCOMBINEREPLACE );

			m_listLight.push_back( pLight );

			if( g_pcsAgcmRender->AddLightToRenderWorld( pLight , TRUE )
				//&& g_pcsAgcmDynamicLightmap->addLight( pLight , TRUE )
				)
			{
			}
			else
			{
				MD_SetErrorMessage( "����Ʈ �߰� ����" );
			}
		}
		else
		{
			break;
		}
	}
	while(1);

	fclose( pFile );
	*/

	AGCMMAP_THIS->EnumLoadedDivision( CPlugin_PointLight::CBLoadDivision , NULL );

	return TRUE;	
}

void	CPlugin_PointLight::RemoveAllLights()
{
	// ������ (2005-05-19 ���� 8:34:12) : 
	// ������Ʈ �������� �ڵ����� �ɰŴϱ�..
	// �Ű澲������.

	/*
	PLOInfo	* pInfo;
	RwFrame	* pFrame;

	for( int i = 0 ; i < ( int ) m_listLight.size() ; i ++ )
	{
		pInfo = &m_listLight[ i ];
		pFrame = RpLightGetFrame( pInfo->pLight );

		// ����Ʈ ����..
		RwFrameDestroy( pFrame );
		RpLightSetFrame( pInfo->pLight , NULL );

		//g_pcsAgcmDynamicLightmap->removeLight( pLight );
		g_pcsAgcmRender->RemoveLightFromRenderWorld( pInfo->pLight , TRUE );
	}

	m_listLight.clear();
	*/
}

INT32	CPlugin_PointLight::GetLightOffset( RpLight *	pLight	)
{
	// �����ͷ� �ɼ� ã��.

	PLOInfo *pInfo;
	for( int i = 0 ; i < ( int ) m_listLight.size() ; i ++ )
	{
		pInfo = &m_listLight[ i ];
		if( pInfo->pLight == pLight ) return i;
	}
	return -1;
}
INT32	CPlugin_PointLight::GetLightOffset( INT32		nOID	)
{
	// �ε����� �ɼ� ã��.

	PLOInfo *pInfo;
	for( int i = 0 ; i < ( int ) m_listLight.size() ; i ++ )
	{
		pInfo = &m_listLight[ i ];
		if( pInfo->nOID == nOID ) return i;
	}
	return -1;
}

void	CPlugin_PointLight::RemoveLightInfo( RpLight *	pLight	)
{
	int	nOffset = GetLightOffset( pLight );
	if( nOffset != -1 )
	{
		m_listLight.erase( m_listLight.begin() + nOffset );
		// ������~

		// ���� �ε��� �ʱ�ȭ ( �ٲ�Ƿ� �׳� �θ� ��� )
		m_nSelectedPoint = -1;
	}
}

void	CPlugin_PointLight::RadiusUpdate()
{
	// ����� ����Ʈ ���� �� ����Ÿ �����Ŵ.
	PLOInfo	* pInfo;

	if( m_nSelectedPoint != -1 )
	{
		pInfo	= &m_listLight[ m_nSelectedPoint ];

		FLOAT	fDistance = RpLightGetRadius( pInfo->pLight );

		pInfo->pstInfo->fRadius	= fDistance;

		SetSaveData();
	}
}

void	CPlugin_PointLight::PositionUpdate()
{
	PLOInfo	* pInfo;

	if( m_nSelectedPoint != -1 )
	{
		pInfo	= &m_listLight[ m_nSelectedPoint ];
		RwFrame	* pFrame;
		pFrame = RpLightGetFrame( pInfo->pLight );
		
		RwV3d	pos = pFrame->ltm.pos;

		ApdObject * pcsObject = g_pcsApmObject->GetObject( pInfo->nOID );
		if( pcsObject )
		{
			pcsObject->m_stPosition.x	= pos.x;
			pcsObject->m_stPosition.y	= pos.y;
			pcsObject->m_stPosition.z	= pos.z;
			g_pcsAgcmObject->SetupObjectClump_TransformOnly( pcsObject );
		}

		SetSaveData();
	}
}

void	CPlugin_PointLight::ColorUpdate()
{
	PLOInfo	* pInfo;

	if( m_nSelectedPoint != -1 )
	{
		pInfo	= &m_listLight[ m_nSelectedPoint ];
		
		const RwRGBAReal	* pRgb = RpLightGetColor( pInfo->pLight );

		pInfo->pstInfo->uRed	= ( UINT8 ) ( pRgb->red		* 255.0f );
		pInfo->pstInfo->uGreen	= ( UINT8 ) ( pRgb->green	* 255.0f );
		pInfo->pstInfo->uBlue	= ( UINT8 ) ( pRgb->blue	* 255.0f );
		SetSaveData();
	}
}
