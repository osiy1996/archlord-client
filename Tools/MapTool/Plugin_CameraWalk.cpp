// Plugin_CameraWalk.cpp : implementation file
//

#include "stdafx.h"
#include "maptool.h"
#include "Plugin_CameraWalk.h"
#include "MainFrm.h"
#include "MainWindow.h"
#include "AgcuCamPathWork.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern MainWindow			g_MainWindow	;

#define	CW_CLICKED_BUTTON( uID )	if( uID == LOWORD( wParam ) && BN_CLICKED == HIWORD( wParam ) )

RwReal
RtLinePointLength			(RwLine * line,RwV3d * pPoint)
{
    RwV3d               lineNormal;
    RwReal              recipLineLength;
    RwReal              atomicDistance; /* Distance of atomic along line */
    RwV3d               vE0;
    RwReal              hyp2; /* Distance from line start to atomic center ^2 */

    /* Calculate line parameters */
    RwV3dSub(&lineNormal, &line->end, &line->start);
    recipLineLength = _rwV3dNormalize(&lineNormal, &lineNormal);

    /* Offset atomic's center so that line is effectively starting at (0,0,0) */
    RwV3dSub(&vE0, pPoint, &line->start);

    /* Project atomic's center distance along line */
    atomicDistance = RwV3dDotProduct(&vE0, &lineNormal);

    /* Distance to atomic center */
    hyp2 = RwV3dDotProduct(&vE0, &vE0);

	return atomicDistance * recipLineLength;
}


/////////////////////////////////////////////////////////////////////////////
// CPlugin_CameraWalk

CPlugin_CameraWalk::CPlugin_CameraWalk()
{
	m_bPlay				= FALSE	;
	m_uDuration			= 30000	;
	m_uCurrent			= 0		;

	m_pPathWork			= NULL	;
	m_pAngleWork		= NULL	;

	m_nImVertexCount	= 0		;
	m_uRunSpeed			= 4000	;
	m_pSphereAtomic		= NULL	;
	m_nSelectedPoint	= -1	;

	m_nType				= SINGLESPLINE	;
	m_nSSType			= SST_NORMAL	;
	m_bAccel			= TRUE			;
	m_bClosed			= FALSE			;
	m_bLoop				= FALSE			;

	m_nSelectedAxis		= CAxisManager::AXIS_NOSELECTED;
	m_fSelectedValue1	= 0.0f;
	m_fSelectedValue2	= 0.0f;

	m_bPrevControlState	= FALSE	;

	m_strShortName = "Camera";
}

CPlugin_CameraWalk::~CPlugin_CameraWalk()
{
}

BEGIN_MESSAGE_MAP(CPlugin_CameraWalk, CWnd)
	//{{AFX_MSG_MAP(CPlugin_CameraWalk)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlugin_CameraWalk message handlers

AgcuPathWork *	CPlugin_CameraWalk::PreparePathWork( BOOL bTestMode )
{
	AgcuPathWork * pSplinePathWork = NULL;
	if( m_vecCtrlMatrix.size() >= 4 )
	{
		switch( m_nType )
		{
		case 	SINGLESPLINE:
			{
				switch( m_nSSType )
				{
				default:
				case	SST_NORMAL		:
					{
						AgcuPathSingleSpline * pSpline = new AgcuPathSingleSpline( m_uDuration );

						if( m_bLoop )	pSpline->bLoop();
						else			pSpline->bNoLoop();

						pSpline->bReset();

						pSpline->bForward();
						//pSpline->bBackward();

						pSpline->bSetActType(1);		//flight

						if( m_bAccel )	pSpline->bSetAccel(1);			//accel
						else			pSpline->bSetAccel(0);			//accel

						stSetSpline	stSet;

						if( m_bClosed ) stSet.closed		= true;
						else			stSet.closed		= false;

						stSet.ctrlMatrices	= &m_vecCtrlMatrix[ 0 ];
						stSet.numCtrl		= m_vecCtrlMatrix.size();

						pSpline->bInitSpline(stSet);

						pSplinePathWork = pSpline;
					}
					break;
				case	SST_STARE	:
					{ 
						AgcuPathSingleSpline * pSpline = new AgcuPathSingleSpline( m_uDuration );

						// ù��° �༮�� ��ġ�� ���..
						if( m_vecCtrlMatrix.size() >= 5 )
						{
							if( m_bLoop )	pSpline->bLoop();
							else			pSpline->bNoLoop();

							pSpline->bReset();

							pSpline->bForward();
							//pSpline->bBackward();

							( ( AgcuPathSingleSpline * ) pSpline )->bSetActType(1);		//flight

							if( m_bAccel )	pSpline->bSetAccel(1);			//accel
							else			pSpline->bSetAccel(0);			//accel

							stSetSpline	stSet;

							if( m_bClosed ) stSet.closed		= true;
							else			stSet.closed		= false;

							stSet.ctrlMatrices	= &m_vecCtrlMatrix[ 0 ] + 1;
							stSet.numCtrl		= m_vecCtrlMatrix.size() - 1;

							pSpline->bInitSpline(stSet);
							pSpline->bSetFixedLookat( &m_vecCtrlMatrix[ 0 ].pos);

							pSplinePathWork = pSpline;
						}
					}
					break;
				case SST_PARALLEL:
					{
						// ...
					}
					break;
				case SST_FLIGHT:
					{
						// ...
					}
					break;
				}
			}
			break;
		case	DOUBLESPLINE:
			{
				AgcuPathDoubleSpline * pSpline = new AgcuPathDoubleSpline( m_uDuration );

				if( m_bLoop )	pSpline->bLoop();
				else			pSpline->bNoLoop();

				pSpline->bReset();

				pSpline->bForward();
				//pSpline->bBackward();

				pSpline->bSetActType(0);	//side vector is parallel with xz_plane

				if( m_bAccel )	pSpline->bSetAccel(1);			//accel
				else			pSpline->bSetAccel(0);			//accel

				stSetSpline	stSet;

				if( m_bClosed ) stSet.closed		= true;
				else			stSet.closed		= false;

				stSet.ctrlMatrices	= &m_vecCtrlMatrix[ 0 ];
				stSet.numCtrl		= m_vecCtrlMatrix.size();

				pSpline->bInitSpline(stSet);

				pSplinePathWork = pSpline;
			}
			break;
		}

	}
	else
	{
		DisplayMessage( RGB( 255 , 0 , 0 ) , "�ּ��� 4���� ��尡 �־ �������� ���ϸ��̼��� �����մϴ�." );
	}

	return pSplinePathWork;
}

AgcuPathWork *	CPlugin_CameraWalk::PrepareAngleWork( BOOL bTestMode )
{
	AgcuPathWork * pSplineAngleWork = NULL;
	if( m_vecProjection.size() >= 4 )
	{
		AgcuPathSingleSpline * pSpline = new AgcuPathSingleSpline( m_uDuration );

		if( m_bLoop )	pSpline->bLoop();
		else			pSpline->bNoLoop();

		pSpline->bReset();

		pSpline->bForward();
		//pSpline->bBackward();

		pSpline->bSetActType(1);		//flight

		if( m_bAccel )	pSpline->bSetAccel(1);			//accel
		else			pSpline->bSetAccel(0);			//accel

		stSetSpline	stSet;

		if( m_bClosed ) stSet.closed		= true;
		else			stSet.closed		= false;

		vector< RwMatrix >	matPos;
		matPos.reserve( m_vecProjection.size() );
		RwMatrix mat;
		mat.pos.y = 0.0f;
		mat.pos.z = 0.0f;

		// Matrix �� pos ���� ��� ���ο��� �����.
		for( int i = 0 ; i < ( int ) m_vecProjection.size() ; i ++ )
		{
			mat.pos.x = m_vecProjection[ i ];
			matPos.push_back( mat );
		}

		stSet.ctrlMatrices	= &matPos[ 0 ];
		stSet.numCtrl		= m_vecProjection.size();

		pSpline->bInitSpline(stSet);

		pSplineAngleWork = pSpline;
	}
	else
	{
		DisplayMessage( RGB( 255 , 0 , 0 ) , "�ּ��� 4���� ��尡 �־ �������� ���ϸ��̼��� �����մϴ�." );
	}

	return pSplineAngleWork;
}


LRESULT CPlugin_CameraWalk::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if( WM_COMMAND == message )
	{
		CW_CLICKED_BUTTON( CWCID_BUTTONSETTING )
		{
			CCameraWalkingSettingDlg dlg;

			dlg.m_nType		= m_nType	;
			dlg.m_nSSType	= m_nSSType	;
			dlg.m_bAccel	= m_bAccel	;
			dlg.m_bClosed	= m_bClosed	;
			dlg.m_bLoop		= m_bLoop	;

			// ������..
			if( dlg.DoModal() )
			{
				m_nType		= dlg.m_nType	;
				m_nSSType	= dlg.m_nSSType	;
				m_bAccel	= dlg.m_bAccel	;
				m_bClosed	= dlg.m_bClosed	;
				m_bLoop		= dlg.m_bLoop	;

				// ���� ����
				// ȭ�� ����..
				UpdateVertex();
				Invalidate( TRUE );
			}
			//
		}
		CW_CLICKED_BUTTON( CWCID_BUTTONADD )
		{
			if( m_bPlay )
			{
				DisplayMessage( RGB( 255 , 0 , 0 ) , "����߿� ���� �ȵſ�~" );
			}
			else
			{
				TRACE( "CWCID_BUTTONADD Ŭ���Ǿ���\n" );
				// ���� ����Ʈ�� ���Ĵܿ� �߰���..
				// ����Ʈ �ڽ� ������Ʈ.

				ASSERT( NULL != g_pEngine->m_pCamera );

				RwMatrix	*	pMatrix = RwFrameGetMatrix( RwCameraGetFrame( g_pEngine->m_pCamera ) );

				ASSERT( NULL != pMatrix );

				RwV3d * pPos = RwMatrixGetPos( pMatrix );

				ASSERT( NULL != pPos );

				if( m_vecCtrlMatrix.size() < CW_MAX_SPLINE_NODE )
				{
					// ����Ʈ ����..
					m_vecCtrlMatrix.push_back( * pMatrix );
					m_vecProjection.push_back( g_pEngine->GetProjection() );

					DisplayMessage( "����Ʈ ������ �����߾��~" );
				}
				else
				{
					DisplayMessage( RGB( 255 , 0 , 0 ) , "����Ʈ ���۰� ������ ������ �����.(���� %d��)~" , CW_MAX_SPLINE_NODE );
				}

				UpdateVertex();
			}
		}
		CW_CLICKED_BUTTON( CWCID_BUTTONREMOVE )
		{
			if( m_bPlay )
			{
				DisplayMessage( RGB( 255 , 0 , 0 ) , "����߿� ���� �ȵſ�~" );
			}
			else
			{
				TRACE( "CWCID_BUTTONREMOVE Ŭ���Ǿ���\n" );
				// ����Ʈ �ڽ��� ���� �Ǿ� �ִ� �༮�� ������...
				// ����Ʈ �ڽ� ������Ʈ

				INT32	uNode = m_nSelectedPoint;

				if( uNode < ( int ) m_vecCtrlMatrix.size() )
				{
					for( INT32 i = uNode ; i < ( int ) m_vecCtrlMatrix.size() - 1 ; ++i )
					{
						m_vecCtrlMatrix[ i ] = m_vecCtrlMatrix[ i + 1 ];
					}

					m_vecCtrlMatrix.erase( m_vecCtrlMatrix.begin() + m_vecCtrlMatrix.size() - 1 );

					m_nSelectedPoint = -1;
				}
			}

			UpdateVertex();
		}
		CW_CLICKED_BUTTON( CWCID_BUTTONSETPLAYTIME )
		{
			if( m_bPlay )
			{
				DisplayMessage( RGB( 255 , 0 , 0 ) , "����߿� ���� �ȵſ�~" );
			}
			else
			{
				TRACE( "CWCID_BUTTONSETPLAYTIME Ŭ���Ǿ���\n" );
				// ��ü �÷��̽ð� ������Ʈ..
				CDurationSetDlg	dlg;
				dlg.m_uDuration	= m_uDuration	;

				if( IDOK == dlg.DoModal() )
				{
					m_uDuration = dlg.m_uDuration	;

					DisplayMessage( RGB( 0 , 0 , 0 ) , "����ð��� %d�� ����" , m_uDuration );
					UpdateVertex();
				}
			}

			Invalidate( TRUE );
		}

		CW_CLICKED_BUTTON( CWCID_BUTTONPLAY )
		{
			TRACE( "CWCID_BUTTONPLAY Ŭ���Ǿ���\n" );
			m_bPlay	= !m_bPlay;
			if( m_bPlay )
			{
				// Play
				TRACE( "Play ó��~\n" );

				Stop();
				m_bPlay = TRUE;


				if( NULL == ( m_pPathWork = PreparePathWork() ) ||
					NULL == ( m_pAngleWork = PrepareAngleWork() ) )
				{
					DisplayMessage( RGB( 255 , 0 , 0 ) , "��忡 ������ �־��~" );
					m_bPlay = FALSE;
					Stop();
				}
				else
				{
					m_uCurrent = 0;
				}
			}
			else
			{
				// Stop
				TRACE( "Stop ó��~\n" );
				Stop();
			}
		}
		CW_CLICKED_BUTTON( CWCID_BUTTONREMOVEALL )
		{
			// ����Ʈ �ʱ�ȭ..
			RemoveAll();
		}

		CW_CLICKED_BUTTON( CWCID_BUTTONSAVE )
		{
			TRACE( "Save Ŭ���Ǿ���\n" );
			Save();
		}

		CW_CLICKED_BUTTON( CWCID_BUTTONLOAD )
		{
			TRACE( "Load Ŭ���Ǿ���\n" );
			Load();
		}
	}
	
	return CWnd::WindowProc(message, wParam, lParam);
}

int CPlugin_CameraWalk::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	CRect	rect;

	rect.left	= 5			;
	rect.right	= 155		;
	rect.top	= 5			;
	rect.bottom	= 30		;
	
	VERIFY( m_ctlButtonSetting.Create( "Setting" , WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, rect , this , CWCID_BUTTONSETTING ) );

	rect.left	= 5			;
	rect.right	= 155		;
	rect.top	= 160		;
	rect.bottom	= 180		;
	VERIFY( m_ctlButtonAdd.Create( "Add Current Point" , WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, rect , this , CWCID_BUTTONADD ) );

	rect.top	= 185		;
	rect.bottom	= 205		;
	VERIFY( m_ctlButtonRemove.Create( "Remove Selected" , WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, rect , this , CWCID_BUTTONREMOVE ) );

	rect.top	= 210		;
	rect.bottom	= 230		;
	VERIFY( m_ctlButtonSetPlayTime.Create( "Set Total Playtime" , WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, rect , this , CWCID_BUTTONSETPLAYTIME ) );

	rect.left	= 160		;
	rect.right	= 295		;

	rect.top	= 160		;
	rect.bottom	= 180		;
	VERIFY( m_ctlButtonRemoveAll.Create( "Remove All Node" , WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, rect , this , CWCID_BUTTONREMOVEALL ) );

	rect.top	= 185		;
	rect.bottom	= 205		;
	VERIFY( m_ctlButtonSave.Create( "Save Node" , WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, rect , this , CWCID_BUTTONSAVE ) );

	rect.top	= 210		;
	rect.bottom	= 230		;
	VERIFY( m_ctlButtonLoad.Create( "Load Node" , WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, rect , this , CWCID_BUTTONLOAD ) );
	
	// �����ư ����.

	rect.top	= 250		;
	rect.bottom	= 270		;

	rect.left	= 5			;
	rect.right	= 100		;
	VERIFY( m_ctlButtonPlay.Create( "Play" , WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, rect , this , CWCID_BUTTONPLAY ) );

	m_rectProgress.SetRect( 5 , 280 , 295 , 300 );

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

	return 0;
}

BOOL CPlugin_CameraWalk::OnIdle( UINT32 ulClockCount )
{
	static UINT32	uLastTime		= GetTickCount();
	UINT32	uCurrentTime	= GetTickCount();
	UINT32	uDeltaTime		= uCurrentTime - uLastTime;
	uLastTime = uCurrentTime;
	
	if( m_bPlay && m_pPathWork && m_pAngleWork )
	{
		m_uCurrent	+= uDeltaTime;

		if( m_pPathWork->GetAccumTime() >= m_pPathWork->GetLife() )
		{
			m_uCurrent = m_uDuration - 1; // ������ ī��Ʈ�� ���������� 0���� ���µ�..
			m_bPlay = FALSE;
			Stop();
		}
		else
		{
			// ī�޶� ������ ������Ʈ..
			RwFrame		*pFrame = RwCameraGetFrame	( g_pEngine->m_pCamera	);
			RwMatrix	mat , matAngle;
			RwMatrixSetIdentity(&mat);

			if( 1 != m_pPathWork->bOnIdle( mat , uDeltaTime ) )
				// 1�̸� ������.. ��Ʈ���� ������Ʈ�� ���Ѵ�.
			{
				RwMatrixUpdate(&mat);
				RwFrameTransform( pFrame, &mat, rwCOMBINEREPLACE );

				if( g_pcsAgcmEventNature )
				{
					AuPOS	posCamera;
					posCamera.x	= mat.pos.x;
					posCamera.y	= mat.pos.y;
					posCamera.z	= mat.pos.z;

					if( posCamera.y > 10000.0f / 1.2f )
						g_pcsAgcmEventNature->SetSkyHeight( posCamera.y * 1.2f );
					else
						g_pcsAgcmEventNature->SetSkyHeight( 10000.0f			);

					g_pcsAgcmEventNature->SetCharacterPosition( posCamera );
				}

				if( g_pcsAgcmShadow2 )
				{
					ApWorldSector * pSector = g_pcsApmMap->GetSector( mat.pos.x , mat.pos.z );
					if( pSector )
					{
						if( g_MainWindow.m_pCameraSector != pSector )
						{
							g_MainWindow.m_pCameraSector = pSector;
							if( pSector )
							{
								g_pcsAgcmShadow2->CB_SECTOR_CAHNGE( ( PVOID ) pSector , ( PVOID ) g_pcsAgcmShadow2 , NULL );
							}
						}
					}
				}	

				InvalidateRect( m_rectProgress , FALSE );
			}

			if( 1 != m_pAngleWork->bOnIdle( mat , uDeltaTime ) )
			{
				g_pEngine->SetProjection( mat.pos.x );
			}
		}
	}
	return TRUE;
}

void	CPlugin_CameraWalk::Stop()
{
	TRACE( "���ö��� ���ϸ��̼� ����\n" );
	m_bPlay		= FALSE	;
	m_uCurrent	= 0		;

	if( m_pPathWork )
	{
		delete m_pPathWork;
		m_pPathWork = NULL;
	}

	if( m_pAngleWork )
	{
		delete m_pAngleWork;
		m_pAngleWork = NULL;
	}
}

void CPlugin_CameraWalk::OnDestroy() 
{
	CWnd::OnDestroy();
	
	if( m_pSphereAtomic )
	{
		RpAtomicDestroy( m_pSphereAtomic );
		m_pSphereAtomic = NULL;
	}

	Stop();	
}
/////////////////////////////////////////////////////////////////////////////
// CDurationSetDlg dialog


CDurationSetDlg::CDurationSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDurationSetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDurationSetDlg)
	m_uDuration = 0;
	m_bType = FALSE;
	//}}AFX_DATA_INIT
}


void CDurationSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDurationSetDlg)
	DDX_Text(pDX, IDC_DURATION, m_uDuration);
	DDX_Check(pDX, IDC_TYPE, m_bType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDurationSetDlg, CDialog)
	//{{AFX_MSG_MAP(CDurationSetDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDurationSetDlg message handlers

void CPlugin_CameraWalk::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// ����������~..
	CRect	rect;
	GetClientRect( rect );
	dc.FillSolidRect( rect , RGB( 99 , 99 , 99 ) );

	CString	str;

	//dc.SetBkMode( TRANSPARENT );
	dc.SetTextColor	( RGB( 255 , 255 , 168 )	);
	dc.SetBkColor	( RGB( 99 , 99 , 99 )		);

	switch( m_nType )
	{
	case 	SINGLESPLINE:
		{
			dc.TextOut( 10 , 35 , "Single Spline" );

			switch( m_nSSType )
			{
			default:
			case	SST_NORMAL		:
				dc.TextOut( 10 , 55 , "- Normal Mode" );
				break;
			case	SST_STARE	:
				dc.TextOut( 10 , 55 , "- Stare Mode" );
				break;
			case SST_PARALLEL:
				dc.TextOut( 10 , 55 , "- Parallel Mode" );
				break;
			case SST_FLIGHT:
				dc.TextOut( 10 , 55 , "- Flight Mode" );
				break;
			}
		}
		break;
	case	DOUBLESPLINE:
		dc.TextOut( 10 , 35 , "Double Spline" );
		break;
	}


	str.Format( "��ü %.1f��" , ( FLOAT ) m_uDuration / 1000.0f );

	dc.TextOut		( 110 , 250 , str			);

	#define	RGB_PROGRESSBACKGROUND	RGB( 72 , 53, 102 )
	#define	RGB_PROGRESSBAR			RGB( 255 , 21, 43 )

	INT32	nWidth = 0;
	if( m_bPlay && m_pPathWork )
	{
		nWidth = ( INT32 ) ( ( FLOAT ) m_rectProgress.Width() * ( FLOAT ) m_pPathWork->GetAccumTime() / ( FLOAT ) m_pPathWork->GetLife() );
	}
	dc.FillSolidRect( m_rectProgress.left - 1 , m_rectProgress.top - 1 , m_rectProgress.Width() + 2 , m_rectProgress.Height() + 2 , RGB( 255 , 255 , 255 ) );
	dc.FillSolidRect( m_rectProgress.left , m_rectProgress.top , nWidth , m_rectProgress.Height() , RGB_PROGRESSBAR );
	dc.FillSolidRect( m_rectProgress.left + nWidth , m_rectProgress.top , m_rectProgress.Width() - nWidth , m_rectProgress.Height() , RGB_PROGRESSBACKGROUND );

	// �ð����� ǥ��..
	dc.SetBkColor	( RGB( 99 , 99 , 99 )		);
	dc.TextOut		( 10 , m_rectProgress.bottom + 10  , "�Ÿ����� ���.." );

	UpdateDistance();
	str.Format	( "�ӵ� : %d" , m_uRunSpeed					);
	dc.TextOut	( 10 , m_rectProgress.bottom + 30  , str	);
	str.Format	( "�Ÿ� : %.1f ����" , m_fDistance / 100.0f	);
	dc.TextOut	( 10 , m_rectProgress.bottom + 50  , str	);
	str.Format	( "�ð� : %.1f ��" , m_fTime / 1000.0f		);
	dc.TextOut	( 10 , m_rectProgress.bottom + 70  , str	);

	if( m_bPrevControlState && m_nSelectedPoint != -1 )
	{
		// ����Ʈ�� ���� ǥ��..
		dc.SetTextColor( RGB( 180 , 180 , 255 ) );
		str.Format	( "���� ����Ʈ�� PS�� = %.2f" , m_vecProjection	[ m_nSelectedPoint ] );
		dc.TextOut	( 10 , m_rectProgress.bottom + 70  , str	);
	}
	
	// Do not call CWnd::OnPaint() for painting messages
}

void	CPlugin_CameraWalk::RemoveAll	()
{
	TRACE( "Remove all\n" );
	if( m_bPlay )
	{
		DisplayMessage( RGB( 255 , 0 , 0 ) , "����߿� ���� �ȵſ�~" );
	}
	else
	{
		m_vecCtrlMatrix.clear();
		m_vecProjection.clear();
		UpdateVertex();
	}
}

BOOL	CPlugin_CameraWalk::Save		()
{
	// ���̺��ϴ±��
	CString szFileName;
	CFileDialog dlg(FALSE,_T("SAB"),_T("*.SAB"),OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
					_T("���ö��� ���ϸ��̼� ���̳ʸ� ���� (*.SAB)|*.SAB|�˴� (*.*)|*.*|"));
	
	char	strDir[ 1024 ];
	GetCurrentDirectory( 1024 , strDir );

	if(dlg.DoModal()==IDOK)
	{
		SetCurrentDirectory( strDir );

		szFileName=dlg.GetPathName();

		// ����..

		FILE	* pFile = fopen( ( LPCTSTR ) szFileName , "wb" );
		if( NULL == pFile )
		{
			MessageBox( "���� ���½���~ �����ؼ� üũ�Ͻð� �ٽ� �õ��ϼ�" );
			return FALSE;
		}
		else
		{
			// ���� ���� ���..
			// fwrite( ( void * ) &version , sizeof version , 1 , pFile );

			// �ɼ�.
			fwrite( ( void * ) &m_nType		 , sizeof m_nType	 , 1 , pFile );
			fwrite( ( void * ) &m_nSSType	 , sizeof m_nSSType	 , 1 , pFile );
			fwrite( ( void * ) &m_bAccel	 , sizeof m_bAccel	 , 1 , pFile );
			fwrite( ( void * ) &m_bClosed	 , sizeof m_bClosed	 , 1 , pFile );
			fwrite( ( void * ) &m_bLoop		 , sizeof m_bLoop	 , 1 , pFile );

			// ����
			fwrite( ( void * ) &m_uDuration , sizeof m_uDuration , 1 , pFile );

			// ��� ����..
			INT32	nSize = m_vecCtrlMatrix.size();
			fwrite( ( void * ) &nSize, sizeof nSize , 1 , pFile );

			// ��Ʈ���� ī��..
			fwrite( ( void * ) &m_vecCtrlMatrix[ 0 ] , sizeof RwMatrix , nSize , pFile );

			// ��Ʈ���� ī��..
			fwrite( ( void * ) &m_vecProjection[ 0 ] , sizeof FLOAT , nSize , pFile );

			fclose( pFile );
			return TRUE;
		}
	}
	else
	{
		SetCurrentDirectory( strDir );
		return FALSE;
	}
}

BOOL	CPlugin_CameraWalk::Load		()
{
	if( m_bPlay )
	{
		DisplayMessage( RGB( 255 , 0 , 0 ) , "����߿� ���� �ȵſ�~" );
		return FALSE;
	}

	char	strDir[ 1024 ];
	GetCurrentDirectory( 1024 , strDir );

	CFileDialog OpenPS( TRUE , _T("SAB"),_T("*.SAB") , OFN_HIDEREADONLY , _T("���ö��� ���ϸ��̼� ���̳ʸ� ���� (*.SAB)|*.SAB|�˴� (*.*)|*.*|") , this );

	if (OpenPS.DoModal() == IDOK)
	{
		char filename[ 1024 ];
		strncpy( filename , (LPCTSTR) OpenPS.GetPathName() , 1024 );
		char	drive[ 256 ] , dir [ 256 ] , fname [ 256 ] , ext[ 256 ];
		_splitpath( filename , drive, dir, fname, ext );

		wsprintf( dir , "%s%s" , drive, dir );

		SetCurrentDirectory( dir );

		wsprintf( filename , "%s%s" , fname , ext );

		// Load..

		FILE	* pFile = fopen( filename , "rb" );
		
		SetCurrentDirectory( strDir );
		if( NULL == pFile )
		{
			MessageBox( "���� ���½���~ �����ؼ� üũ�Ͻð� �ٽ� �õ��ϼ�" );
			return FALSE;
		}
		else
		{
			RemoveAll();

			// ���� ���� ���..
			// fwrite( ( void * ) &version , sizeof version , 1 , pFile );

			// �ɼ�.
			fread( ( void * ) &m_nType		, sizeof m_nType	, 1 , pFile );
			fread( ( void * ) &m_nSSType	, sizeof m_nSSType	, 1 , pFile );
			fread( ( void * ) &m_bAccel		, sizeof m_bAccel	, 1 , pFile );
			fread( ( void * ) &m_bClosed	, sizeof m_bClosed	, 1 , pFile );
			fread( ( void * ) &m_bLoop		, sizeof m_bLoop	, 1 , pFile );

			// ����
			fread( ( void * ) &m_uDuration , sizeof m_uDuration , 1 , pFile );

			// ��� ����..
			INT32	nSize;
			fread( ( void * ) &nSize , sizeof nSize , 1 , pFile );

			// ��Ʈ���� ī��..
			m_vecCtrlMatrix.resize( nSize );
			fread( ( void * ) &m_vecCtrlMatrix[ 0 ] , sizeof RwMatrix , nSize , pFile );
			m_vecProjection.resize( nSize );
			fread( ( void * ) &m_vecProjection[ 0 ] , sizeof FLOAT , nSize , pFile );

			fclose( pFile );

			UpdateVertex();
			return TRUE;
		}
	}
	else
	{
		SetCurrentDirectory( strDir );
		return FALSE;
	}
}

BOOL CPlugin_CameraWalk::OnWindowRender			()
{
	if( m_bPlay )
	{
		// do nothing
	}
	else
	{
		if( m_nSelectedPoint != -1 )
		{
			// �÷������� �ƴϰ�..
			// ���É� ����Ʈ�� �ִ°��..

			BOOL	bCurrentControlState = ISBUTTONDOWN( VK_CONTROL ) ;

			if( m_bPrevControlState != bCurrentControlState )
			{
				static RwMatrix __smatSaved;
				static FLOAT	__sfPerspective = 0.7f;
				if( bCurrentControlState )
				{
					// ī�޶� ���� ����..
					// �ش� ����Ʈ�� �̵�..
					RwFrame		*	pFrame	= RwCameraGetFrame( g_pEngine->m_pCamera );
					RwMatrix	*	pMatrix = RwFrameGetMatrix( pFrame );

					// ��Ʈ���� �����ص�..
					__smatSaved = * pMatrix;
					__sfPerspective = g_pEngine->GetProjection();

					// ī�޶� ����Ʈ�� �̵���Ŵ..
					RwFrameTransform( pFrame, &m_vecCtrlMatrix[ m_nSelectedPoint ], rwCOMBINEREPLACE );
					g_pEngine->SetProjection( m_vecProjection	[ m_nSelectedPoint ] );

					Invalidate( TRUE );
				}
				else
				{
					// ī�޶� ����Ʈ�� ��Ʈ���� ����..
					// ī�޶� ���� ��ġ�� �̵�..

					RwFrame		*	pFrame	= RwCameraGetFrame( g_pEngine->m_pCamera );
					RwMatrix	*	pMatrix = RwFrameGetMatrix( pFrame );

					// ����Ʈ�� ��Ʈ���� ����
					m_vecCtrlMatrix	[ m_nSelectedPoint ] = * pMatrix;
					m_vecProjection	[ m_nSelectedPoint ] = g_pEngine->GetProjection();

					// ī�޶� ����Ʈ�� �̵���Ŵ..
					RwFrameTransform( pFrame, &__smatSaved, rwCOMBINEREPLACE );
					g_pEngine->SetProjection( __sfPerspective );

					Invalidate( TRUE );
				}

				m_bPrevControlState = bCurrentControlState;
			}
		}

		if( m_nImVertexCount && !m_bPrevControlState)
		{
			if( RwIm3DTransform( m_pImVertex , m_nImVertexCount , NULL, rwIM3D_ALLOPAQUE | rwIM3D_VERTEXRGBA ) )
			{                         
				RwIm3DRenderPrimitive( rwPRIMTYPEPOLYLINE );
				RwIm3DEnd();
			}

			for( int i = 0 ; i < ( int ) m_vecCtrlMatrix.size() ; i ++ )
			{
				DrawSphere( &m_vecCtrlMatrix[ i ].pos );
			}

			if( m_nSelectedPoint != -1 )
			{
				m_cAxis.Render();
			}
		}
	}

	return TRUE;
}

void	CPlugin_CameraWalk::UpdateVertex()
{
	AgcuPathWork	* pSpline = NULL;

	pSpline = PreparePathWork();

	if( pSpline )
	{
		RwMatrix matrix;

		pSpline->bOnIdle( matrix , 0 );

		for( int i = 0 ; i < CW_MAX_SPLINE_DISPLAY_VERTEX ; ++ i )
		{
			RwIm3DVertexSetPos	( &m_pImVertex[ i ] , matrix.pos.x , matrix.pos.y , matrix.pos.z );
			RwIm3DVertexSetU	( &m_pImVertex[ i ] , 1.0f				);    
			RwIm3DVertexSetV	( &m_pImVertex[ i ] , 1.0f				);
			RwIm3DVertexSetRGBA	( &m_pImVertex[ i ] , 255 , ( UINT32 ) ( 255.0f * ( FLOAT ) i / ( FLOAT ) CW_MAX_SPLINE_DISPLAY_VERTEX ), 0 , 255  );

			pSpline->bOnIdle( matrix , ( m_uDuration / CW_MAX_SPLINE_DISPLAY_VERTEX ) );
		}
		
		m_nImVertexCount	= CW_MAX_SPLINE_DISPLAY_VERTEX;

		delete pSpline;
	}
	else
	{
		m_nImVertexCount = 0;
	}

	if( m_nSelectedPoint >= 0 && m_nSelectedPoint < ( int ) m_vecCtrlMatrix.size() )
	{
		m_cAxis.SetPosition( &m_vecCtrlMatrix[ m_nSelectedPoint ].pos );
	}
	else
	{
		m_nSelectedPoint = -1;
	}

	UpdateDistance( TRUE );

	Invalidate( TRUE );
}


void CPlugin_CameraWalk::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CWnd::OnRButtonDown(nFlags, point);
}

void CPlugin_CameraWalk::UpdateDistance( BOOL bForce )
{
	if( bForce )
	{
		// �ƹ� üũ ����..
	}
	else
	{
		static UINT32 uLastCalcuratedRunSpeed = 0;

		// ������ (2004-09-08 ���� 11:43:47) : ���� ����ѰͰ� ������ �ѱ�..
		if( uLastCalcuratedRunSpeed == g_pMainFrame->m_Document.m_uRunSpeed )
			return;

		uLastCalcuratedRunSpeed = g_pMainFrame->m_Document.m_uRunSpeed;
		m_uRunSpeed = g_pMainFrame->m_Document.m_uRunSpeed;
	}

	m_fDistance	= 0.0f;
	m_fTime		= 0.0f;

	if( m_vecCtrlMatrix.size() == 0 ) return;

//	RwV3d	vVector;

	for( INT32 i = 0 ; i < ( int ) m_vecCtrlMatrix.size() - 1 ; ++ i )
	{
		m_fDistance += AUPOS_DISTANCE_XZ( m_vecCtrlMatrix[ i ].pos , m_vecCtrlMatrix[ i + 1 ].pos );
	}

	// �ð� ���..

	m_fTime		= m_fDistance * 10000.0f / ( FLOAT ) m_uRunSpeed;

	Invalidate( TRUE );
}

void		CPlugin_CameraWalk::DrawSphere( RwV3d	* pPos )
{
	TranslateSphere( pPos );
	RpAtomicRender( m_pSphereAtomic );	
}

RpAtomic *	CPlugin_CameraWalk::TranslateSphere( RwV3d * pPos )
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

struct __CollisionTest
{
	BOOL	bFound		;
	FLOAT	fDistance	;
};

static RpCollisionTriangle * __RsIntersectionCallBackGeometryTriangleAtomicTestOnly
											(RpIntersection *intersection, RpCollisionTriangle *collTriangle,
											RwReal distance, void *data)
{

	__CollisionTest	* pCT = ( __CollisionTest * ) data;

	pCT->bFound		= TRUE		;
	pCT->fDistance	= distance	;

	// ����� �ѹ���..
	return NULL;
}

INT32		CPlugin_CameraWalk::GetCollisionIndex( RsMouseStatus *ms )
{
	// ī�޶� ���� ���ؼ� ������ ����..

	// ���� Ŀ�� ��ġ�� �ִ� Ŭ������ ���س���.
	RwLine			pixelRay		;
	RwCameraCalcPixelRay( g_pEngine->m_pCamera , &pixelRay , &ms->pos );

	__CollisionTest	collisionstruct;
	INT32	nFound			= -1	;
	FLOAT	fFoundDistance	= 0.0f	;
	FLOAT	fDistance				;
	RwSphere	sphere;

	sphere.radius	= g_pMainFrame->m_Document.m_fBrushRadius;

	for( int i = 0 ; i < ( int ) m_vecCtrlMatrix.size() ; i ++ )
	{
		collisionstruct.bFound	= FALSE;

		sphere.center	= m_vecCtrlMatrix[ i ].pos;

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

BOOL CPlugin_CameraWalk::Window_OnLButtonDown	( RsMouseStatus *ms )
{
	if( m_nSelectedPoint != -1 )
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

				FLOAT fPlaneZ = m_vecCtrlMatrix[ m_nSelectedPoint ].pos.z;
				pos.z = fPlaneZ;

				// �ش�鿡���� �ݸ��� ��ġ ���ϱ�..

				// x�� ���ϱ�..
				// x = ( x2 - x1 ) / ( y2 - y1 ) * ( y - y1 ) + x1;

				pos.x = ( pixelRay.end.x - pixelRay.start.x ) /
						( pixelRay.end.z - pixelRay.start.z ) * 
						( fPlaneZ - pixelRay.start.z ) + 
						pixelRay.start.x;

				m_fSelectedValue1 = pos.x;
				m_fSelectedValue2 = m_vecCtrlMatrix[ m_nSelectedPoint ].pos.x;
			}
			return TRUE;
		case CAxisManager::AXISY:
			{
				TRACE( "Y��\n" );
				// xy���..
				RwV3d	pos;

				FLOAT fPlaneZ = m_vecCtrlMatrix[ m_nSelectedPoint ].pos.z;
				pos.z = fPlaneZ;

				// �ش�鿡���� �ݸ��� ��ġ ���ϱ�..

				// x�� ���ϱ�..
				// x = ( x2 - x1 ) / ( y2 - y1 ) * ( y - y1 ) + x1;

				pos.y = ( pixelRay.end.y - pixelRay.start.y ) /
						( pixelRay.end.z - pixelRay.start.z ) * 
						( fPlaneZ - pixelRay.start.z ) + 
						pixelRay.start.y;

				m_fSelectedValue1 = pos.y;
				m_fSelectedValue2 = m_vecCtrlMatrix[ m_nSelectedPoint ].pos.y;
			}
			return TRUE;
		case CAxisManager::AXISZ:
			{
				TRACE( "Z��\n" );
				// xz ���..
				RwV3d	pos;

				FLOAT fPlaneY = m_vecCtrlMatrix[ m_nSelectedPoint ].pos.y;
				pos.y = fPlaneY;

				// �ش�鿡���� �ݸ��� ��ġ ���ϱ�..

				// x�� ���ϱ�..
				// x = ( x2 - x1 ) / ( y2 - y1 ) * ( y - y1 ) + x1;

				pos.z = ( pixelRay.end.z - pixelRay.start.z ) /
						( pixelRay.end.y - pixelRay.start.y ) * 
						( fPlaneY - pixelRay.start.y ) + 
						pixelRay.start.z;

				m_fSelectedValue1 = pos.z;
				m_fSelectedValue2 = m_vecCtrlMatrix[ m_nSelectedPoint ].pos.z;
			}
			return TRUE;
		default:
			break;
		}
	}

	INT32	nFound = GetCollisionIndex( ms );
	m_nSelectedPoint = nFound;

	TRACE( "%d ��° ��� �ݸ���\n" , nFound );

	if( -1 == m_nSelectedPoint )
	{
		RwLine			pixelRay		;
		RwCameraCalcPixelRay( g_pEngine->m_pCamera , &pixelRay , &ms->pos );

		__CollisionTest	collisionstruct;
		INT32	nFound			= -1	;
		FLOAT	fFoundDistance	= 0.0f	;
		FLOAT	fDistance				;
		RwSphere	sphere;

		sphere.radius	= g_pMainFrame->m_Document.m_fBrushRadius;

		for( int i = 0 ; i < m_nImVertexCount ; i ++ )
		{
			collisionstruct.bFound	= FALSE;

			sphere.center	= m_pImVertex[ i ].objVertex;

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

		if( nFound == -1 )
		{
			// �ϰ͵� ��..
		}
		else
		{
			if( IDYES == MessageBox( "�ش� ��ġ�� ����Ʈ �ϳ��� �߰��ҷ���?" , "ī�޶��ŷ" , MB_YESNOCANCEL ) )
			{

				TRACE( "%d ��° ����Ʈ �ݸ���\n" , nFound );
				// �ش� ����Ʈ�� ���� ó���� ���⿡..

				// ��� �߰�..

				AgcuPathWork * pPathWork = PreparePathWork( TRUE );
				if( pPathWork )
				{
					DWORD	uDeltaTime = m_uDuration * nFound / m_nImVertexCount;

					RwMatrix	mat;
					RwMatrixSetIdentity(&mat);

					/*
					if( 1 != pPathWork->bOnIdle( mat , uDeltaTime ) )
						// 1�̸� ������.. ��Ʈ���� ������Ʈ�� ���Ѵ�.
					{
						// ����..
					}
					else
					{
						// �̷���� �߻�����..
						ASSERT( !"�ӽö�!" );
					}
					*/

					INT32	aStartArray[ CW_MAX_SPLINE_NODE ];
					INT32	nOffset = 0;
					FLOAT	fDistance	= 0.0f;
					FLOAT	fPrevDistance = 0.0f;

					aStartArray[ 0 ] = 0;

					RwV3d	vector;

					RwV3dSub( &vector , &m_pImVertex[ 0 ].objVertex , &m_vecCtrlMatrix[ nOffset + 1 ].pos );
					fDistance = RwV3dLength( &vector );
					fPrevDistance = fDistance;

					for( int i = 0 ; i < m_nImVertexCount ; i ++ )
					{
						if( i == nFound ) break;

						RwV3dSub( &vector , &m_pImVertex[ i ].objVertex , &m_vecCtrlMatrix[ nOffset + 1 ].pos );
						fDistance = RwV3dLength( &vector );

						if( fDistance > fPrevDistance ) 
						{
							aStartArray[ nOffset ] = i;
							nOffset ++;

							// ���� �Ÿ� �ٽ� ���..
							RwV3dSub( &vector , &m_pImVertex[ i ].objVertex , &m_vecCtrlMatrix[ nOffset + 1 ].pos );
							fDistance = RwV3dLength( &vector );
						}

						fPrevDistance = fDistance;
					}

					// ���°�� �߰����� ����..
					INT32	nInsertIndex = nOffset + 1;
					FLOAT	fAngle	;
					ASSERT( nInsertIndex <= ( int ) m_vecCtrlMatrix.size() );

					mat		= m_vecCtrlMatrix[ nInsertIndex ];
					mat.pos	= m_pImVertex[ nFound ].objVertex;
					fAngle	= m_vecProjection[ nInsertIndex ];

					// �о..
					m_vecCtrlMatrix.push_back( mat );
					m_vecProjection.push_back( fAngle );
					for( int i = nInsertIndex ; i <= ( int ) m_vecCtrlMatrix.size() ; i ++ )
					{
						m_vecCtrlMatrix[ m_vecCtrlMatrix.size() - ( i - nInsertIndex ) ] = m_vecCtrlMatrix[ m_vecCtrlMatrix.size() - 1 - ( i - nInsertIndex ) ];
						m_vecProjection[ m_vecProjection.size() - ( i - nInsertIndex ) ] = m_vecProjection[ m_vecProjection.size() - 1 - ( i - nInsertIndex ) ];
					}

					m_vecCtrlMatrix[ nInsertIndex ] = mat;
					m_vecProjection[ nInsertIndex ] = fAngle;

					// �迭�� �о� �ְ� ���� ����.

					// ���ý� ���� ����..

					UpdateVertex();

					delete pPathWork;
				}
				else
				{
					// �н����� ������ �� ����..
				}

			}
		}
	}
	else
	{
		// ���É³༮

		static FLOAT sfScale = 1000.0f;

		//m_cAxis.Init();
		m_cAxis.SetScale( sfScale );
		m_cAxis.SetPosition( &m_vecCtrlMatrix[ m_nSelectedPoint ].pos );
	}
	return TRUE;
}

BOOL CPlugin_CameraWalk::Window_OnLButtonUp		( RsMouseStatus *ms )
{
	m_nSelectedAxis = CAxisManager::AXIS_NOSELECTED;

	return TRUE;
}

BOOL CPlugin_CameraWalk::Window_OnMouseMove		( RsMouseStatus *ms )
{
	if( m_nSelectedPoint != -1 && m_nSelectedAxis != CAxisManager::AXIS_NOSELECTED )
	{
		RwLine			pixelRay		;
		RwCameraCalcPixelRay( g_pEngine->m_pCamera , &pixelRay , &ms->pos );

		switch( m_nSelectedAxis )
		{
		case CAxisManager::AXISX:
			{
				TRACE( "X��\n" );

				// �ݸ����� ��ġ�� ����..�׸��� ���� ���� ������..
				// xy ���� �ݸ���..
				RwV3d	pos;

				FLOAT fPlaneZ = m_vecCtrlMatrix[ m_nSelectedPoint ].pos.z;
				pos.z = fPlaneZ;

				// �ش�鿡���� �ݸ��� ��ġ ���ϱ�..

				// x�� ���ϱ�..
				// x = ( x2 - x1 ) / ( y2 - y1 ) * ( y - y1 ) + x1;

				pos.x = ( pixelRay.end.x - pixelRay.start.x ) /
						( pixelRay.end.z - pixelRay.start.z ) * 
						( fPlaneZ - pixelRay.start.z ) + 
						pixelRay.start.x;

				FLOAT fDelta = pos.x - m_fSelectedValue1 ;

				char str[ 1024 ];
				sprintf( str , "%.1f ��Ÿ!\n" , fDelta );
				TRACE( str );

				m_vecCtrlMatrix[ m_nSelectedPoint ].pos.x = m_fSelectedValue2 + fDelta;
			}
			break;
		case CAxisManager::AXISY:
			{
				TRACE( "Y��\n" );
				// xy���..
				RwV3d	pos;

				FLOAT fPlaneZ = m_vecCtrlMatrix[ m_nSelectedPoint ].pos.z;
				pos.z = fPlaneZ;

				// �ش�鿡���� �ݸ��� ��ġ ���ϱ�..

				// x�� ���ϱ�..
				// x = ( x2 - x1 ) / ( y2 - y1 ) * ( y - y1 ) + x1;

				pos.y = ( pixelRay.end.y - pixelRay.start.y ) /
						( pixelRay.end.z - pixelRay.start.z ) * 
						( fPlaneZ - pixelRay.start.z ) + 
						pixelRay.start.y;

				FLOAT fDelta = pos.y - m_fSelectedValue1 ;

				char str[ 1024 ];
				sprintf( str , "%.1f ��Ÿ!\n" , fDelta );
				TRACE( str );

				m_vecCtrlMatrix[ m_nSelectedPoint ].pos.y = m_fSelectedValue2 + fDelta;
			}
			break;
		case CAxisManager::AXISZ:
			{
				TRACE( "Z��\n" );
				// xz ���..
				RwV3d	pos;

				FLOAT fPlaneY = m_vecCtrlMatrix[ m_nSelectedPoint ].pos.y;
				pos.y = fPlaneY;

				// �ش�鿡���� �ݸ��� ��ġ ���ϱ�..

				// x�� ���ϱ�..
				// x = ( x2 - x1 ) / ( y2 - y1 ) * ( y - y1 ) + x1;

				pos.z = ( pixelRay.end.z - pixelRay.start.z ) /
						( pixelRay.end.y - pixelRay.start.y ) * 
						( fPlaneY - pixelRay.start.y ) + 
						pixelRay.start.z;

				FLOAT fDelta = pos.z - m_fSelectedValue1 ;

				char str[ 1024 ];
				sprintf( str , "%.1f ��Ÿ!\n" , fDelta );
				TRACE( str );

				m_vecCtrlMatrix[ m_nSelectedPoint ].pos.z = m_fSelectedValue2 + fDelta;
			}
			break;
		default:
			break;
		}

		UpdateVertex();
	}
	return FALSE;
}
BOOL CPlugin_CameraWalk::Window_OnKeyDown		( RsKeyStatus *ks )
{
	return TRUE;
}// C:\ALEF\tools\maptool\Plugin_CameraWalk.cpp : ���� �����Դϴ�.
//

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
// CCameraWalkingSettingDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CCameraWalkingSettingDlg, CDialog)
CCameraWalkingSettingDlg::CCameraWalkingSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCameraWalkingSettingDlg::IDD, pParent)
	, m_bAccel(FALSE)
	, m_bClosed(FALSE)
	, m_bLoop(FALSE)
{
	m_nType		= SINGLESPLINE	;
	m_nSSType	= SST_NORMAL	;
}

CCameraWalkingSettingDlg::~CCameraWalkingSettingDlg()
{
}

void CCameraWalkingSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_ACCEL, m_bAccel);
	DDX_Check(pDX, IDC_CLOSED, m_bClosed);
	DDX_Check(pDX, IDC_LOOP, m_bLoop);
}

BEGIN_MESSAGE_MAP(CCameraWalkingSettingDlg, CDialog)
END_MESSAGE_MAP()


LRESULT CCameraWalkingSettingDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch( message )
	{
	case WM_INITDIALOG:
		{
			switch( m_nType )
			{
			case SINGLESPLINE:
				{
					( ( CButton * ) GetDlgItem( IDC_SINGLESPLINE	) )->SetCheck( TRUE );
					GetDlgItem( IDC_SINGLE_NORMAL	)->EnableWindow( TRUE	);
					GetDlgItem( IDC_SINGLE_STARE	)->EnableWindow( TRUE	);
					GetDlgItem( IDC_SINGLE_PARALLEL	)->EnableWindow( TRUE	);
					GetDlgItem( IDC_SINGLE_FLIGHT	)->EnableWindow( FALSE	);
				}
				break;
			case DOUBLESPLINE:
				{
					( ( CButton * ) GetDlgItem( IDC_DOUBLESPLINE	) )->SetCheck( TRUE );
					GetDlgItem( IDC_SINGLE_NORMAL	)->EnableWindow( FALSE	);
					GetDlgItem( IDC_SINGLE_STARE	)->EnableWindow( FALSE	);
					GetDlgItem( IDC_SINGLE_PARALLEL	)->EnableWindow( FALSE	);
					GetDlgItem( IDC_SINGLE_FLIGHT	)->EnableWindow( FALSE	);
				}
				break;
			}

			switch( m_nSSType )
			{
			case SST_NORMAL		:( ( CButton * ) GetDlgItem( IDC_SINGLE_NORMAL	) )->SetCheck( TRUE );break;
			case SST_STARE		:( ( CButton * ) GetDlgItem( IDC_SINGLE_STARE	) )->SetCheck( TRUE );break;
			case SST_PARALLEL	:( ( CButton * ) GetDlgItem( IDC_SINGLE_PARALLEL) )->SetCheck( TRUE );break;
			case SST_FLIGHT		:( ( CButton * ) GetDlgItem( IDC_SINGLE_FLIGHT	) )->SetCheck( TRUE );break;
			}
		}
		break;
	case WM_COMMAND:
		{
			CButton * pButton = ( CButton * ) GetDlgItem( LOWORD( wParam ) );
			CW_CLICKED_BUTTON( IDC_SINGLESPLINE )
			{
				m_nType = SINGLESPLINE;
				pButton->SetCheck( TRUE );
				( ( CButton * ) GetDlgItem( IDC_DOUBLESPLINE	) )->SetCheck( FALSE );
				GetDlgItem( IDC_SINGLE_NORMAL	)->EnableWindow( TRUE	);
				GetDlgItem( IDC_SINGLE_STARE	)->EnableWindow( TRUE	);
				GetDlgItem( IDC_SINGLE_PARALLEL	)->EnableWindow( TRUE	);
				//GetDlgItem( IDC_SINGLE_FLIGHT	)->EnableWindow( TRUE	);
				return TRUE;
			}
			CW_CLICKED_BUTTON( IDC_DOUBLESPLINE )
			{
				m_nType = DOUBLESPLINE;
				pButton->SetCheck( TRUE );
				( ( CButton * ) GetDlgItem( IDC_SINGLESPLINE	) )->SetCheck( FALSE );
				GetDlgItem( IDC_SINGLE_NORMAL	)->EnableWindow( FALSE	);
				GetDlgItem( IDC_SINGLE_STARE	)->EnableWindow( FALSE	);
				GetDlgItem( IDC_SINGLE_PARALLEL	)->EnableWindow( FALSE	);
				//GetDlgItem( IDC_SINGLE_FLIGHT	)->EnableWindow( FALSE	);
				return TRUE;
			}
			CW_CLICKED_BUTTON( IDC_SINGLE_NORMAL )
			{
				m_nSSType = SST_NORMAL;
				( ( CButton * ) GetDlgItem( IDC_SINGLE_NORMAL	) )->SetCheck( TRUE );
				( ( CButton * ) GetDlgItem( IDC_SINGLE_STARE	) )->SetCheck( FALSE );
				( ( CButton * ) GetDlgItem( IDC_SINGLE_PARALLEL	) )->SetCheck( FALSE );
				( ( CButton * ) GetDlgItem( IDC_SINGLE_FLIGHT	) )->SetCheck( FALSE );
				return TRUE;
			}
			CW_CLICKED_BUTTON( IDC_SINGLE_STARE )
			{
				m_nSSType = SST_STARE;
				( ( CButton * ) GetDlgItem( IDC_SINGLE_NORMAL	) )->SetCheck( FALSE );
				( ( CButton * ) GetDlgItem( IDC_SINGLE_STARE	) )->SetCheck( TRUE );
				( ( CButton * ) GetDlgItem( IDC_SINGLE_PARALLEL	) )->SetCheck( FALSE );
				( ( CButton * ) GetDlgItem( IDC_SINGLE_FLIGHT	) )->SetCheck( FALSE );
				return TRUE;
			}
			CW_CLICKED_BUTTON( IDC_SINGLE_PARALLEL )
			{
				m_nSSType = SST_PARALLEL;
				( ( CButton * ) GetDlgItem( IDC_SINGLE_NORMAL	) )->SetCheck( FALSE );
				( ( CButton * ) GetDlgItem( IDC_SINGLE_STARE	) )->SetCheck( FALSE );
				( ( CButton * ) GetDlgItem( IDC_SINGLE_PARALLEL	) )->SetCheck( TRUE );
				( ( CButton * ) GetDlgItem( IDC_SINGLE_FLIGHT	) )->SetCheck( FALSE );
				return TRUE;
			}
			CW_CLICKED_BUTTON( IDC_SINGLE_FLIGHT )
			{
				m_nSSType = SST_FLIGHT;
				( ( CButton * ) GetDlgItem( IDC_SINGLE_NORMAL	) )->SetCheck( FALSE );
				( ( CButton * ) GetDlgItem( IDC_SINGLE_STARE	) )->SetCheck( FALSE );
				( ( CButton * ) GetDlgItem( IDC_SINGLE_PARALLEL	) )->SetCheck( FALSE );
				( ( CButton * ) GetDlgItem( IDC_SINGLE_FLIGHT	) )->SetCheck( TRUE );
				return TRUE;
			}
		}
		break;
	default:
		break;
	}
	return CWnd::WindowProc(message, wParam, lParam);
}
// CCameraWalkingSettingDlg �޽��� ó�����Դϴ�.
