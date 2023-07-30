// Plugin_GeometryEffect.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "maptool.h"
#include "Mainfrm.h"
#include "MyEngine.h"
#include "Plugin_GeometryEffect.h"


// CPlugin_GeometryEffect

IMPLEMENT_DYNAMIC(CPlugin_GeometryEffect, CWnd)
CPlugin_GeometryEffect::CPlugin_GeometryEffect()
{
	m_strShortName = "GEffect";
}

CPlugin_GeometryEffect::~CPlugin_GeometryEffect()
{
}


BEGIN_MESSAGE_MAP(CPlugin_GeometryEffect, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()

// CPlugin_GeometryEffect �޽��� ó�����Դϴ�.


void CPlugin_GeometryEffect::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect	rect;
	GetClientRect( rect );
	dc.FillSolidRect( rect , RGB( 0 , 0 , 64 ) );
}

BOOL	__DivisionRenderSectorSelectionGridCallback ( DivisionInfo * pDivisionInfo , PVOID pData )
{
	CPlugin_GeometryEffect * pPlugin = ( CPlugin_GeometryEffect * ) pData;

	ApWorldSector * pSector;
	int x , z;
	for( z = pDivisionInfo->nZ	; z < pDivisionInfo->nZ + pDivisionInfo->nDepth ; ++ z )
	{
		for( x = pDivisionInfo->nX	; x < pDivisionInfo->nX + pDivisionInfo->nDepth ; ++ x )
		{
			pSector	= g_pcsApmMap->GetSectorByArray( x , 0 , z );

			if( NULL == pSector ) continue;

			if( pSector->GetFlag() & ApWorldSector::OP_GEOMETRYEFFECTENABLE )
			{
				pPlugin->RenderSectorSelection( pSector );
			}
		}
	}

	return TRUE;
}

BOOL CPlugin_GeometryEffect::OnWindowRender()
{
	AGCMMAP_THIS->EnumLoadedDivision( __DivisionRenderSectorSelectionGridCallback , ( PVOID ) this );
	return TRUE;
}

void	CPlugin_GeometryEffect::RenderSectorSelection	( ApWorldSector * pSector )
{
	AGCMMAP_THIS->AllocSectorGridInfo( pSector );
	// �׸��� Į�� �ٲ㼭 �ѹ��� ��´�.

	// ����Ÿ ������ ����
	RwIm3DVertex	* pImVertex = AGCMMAP_THIS->GetAgcmAttachedData( pSector )->m_pImVertex;
	for( int i = 0 ; i < 65 ; i ++ )
		RwIm3DVertexSetRGBA	( &pImVertex[ i ] , 255 , 0 , 0 , 255  );

	// ���������� ���� �����..

	if( RwIm3DTransform( pImVertex , 65 , NULL, rwIM3D_ALLOPAQUE | rwIM3D_VERTEXRGBA ) )
	{                         
		RwIm3DRenderPrimitive( rwPRIMTYPELINELIST );
		RwIm3DEnd();
	}

	for( int i = 0 ; i < 65 ; i ++ )
		RwIm3DVertexSetRGBA	( &pImVertex[ i ] , 255 , 255 , 255 , 255  );
}

BOOL CPlugin_GeometryEffect::OnLButtonDownGeometry	( RwV3d * pPos )
{
	ApWorldSector * pSector = g_pcsApmMap->GetSector( pPos->x , pPos->z );
	if( NULL == pSector ) return TRUE;

	// ���� ����

	// ����Ÿ ���� ���� Ȯ��
	if( pSector->GetFlag() & ApWorldSector::OP_GEOMETRYEFFECTENABLE )
	{
		// ��ȯ ���� Ȯ��
		if( IDYES == MessageBox( "���� ȿ���� �����ϰڽ��ϱ�?" , "Geometry Effect Plugin" , MB_YESNOCANCEL ) )
		{
			// ��ȯ �۾�..
			pSector->SetFlag( pSector->GetFlag() & ~ApWorldSector::OP_GEOMETRYEFFECTENABLE );
			AGCMMAP_THIS->Update( pSector );
			if( AGCMMAP_THIS->LockSector		( pSector , AGCMMAP_THIS->GetGeometryLockMode() , SECTOR_HIGHDETAIL	) )
			{
				AGCMMAP_THIS->RecalcNormal		( pSector , SECTOR_HIGHDETAIL							);
				if( pSector->GetNearSector( TD_EAST ) )
				{
					AGCMMAP_THIS->RecalcNormal		( pSector->GetNearSector( TD_EAST ) , SECTOR_HIGHDETAIL	);
				}
				if( pSector->GetNearSector( TD_SOUTH ) )
				{
					AGCMMAP_THIS->RecalcNormal		( pSector->GetNearSector( TD_SOUTH ) , SECTOR_HIGHDETAIL	);
				}
				if( pSector->GetNearSector( TD_SOUTH ) && pSector->GetNearSector( TD_SOUTH )->GetNearSector( TD_EAST ) )
				{
					AGCMMAP_THIS->RecalcNormal		( pSector->GetNearSector( TD_SOUTH )->GetNearSector( TD_EAST ) , SECTOR_HIGHDETAIL	);
				}

				AGCMMAP_THIS->UnlockSectors		( TRUE													);
			}
			SaveSetChangeMoonee();
		}
	}
	else
	{
		// ��ȯ ���� Ȯ��
		if( IDYES == MessageBox( "���� ȿ���� ��ġ�ϰڽ��ϱ�? Ÿ�� ���������� ��� ������� �˴ϴ�. ��κҰ�!" , "Geometry Effect Plugin" , MB_YESNOCANCEL ) )
		{
			// ��ȯ �۾�..
			pSector->SetFlag( pSector->GetFlag() | ApWorldSector::OP_GEOMETRYEFFECTENABLE );
			AGCMMAP_THIS->Update( pSector );
			if( AGCMMAP_THIS->LockSector		( pSector , AGCMMAP_THIS->GetGeometryLockMode() , SECTOR_HIGHDETAIL	) )
			{
				AGCMMAP_THIS->RecalcNormal		( pSector , SECTOR_HIGHDETAIL							);
				if( pSector->GetNearSector( TD_EAST ) )
				{
					AGCMMAP_THIS->RecalcNormal		( pSector->GetNearSector( TD_EAST ) , SECTOR_HIGHDETAIL	);
				}
				if( pSector->GetNearSector( TD_SOUTH ) )
				{
					AGCMMAP_THIS->RecalcNormal		( pSector->GetNearSector( TD_SOUTH ) , SECTOR_HIGHDETAIL	);
				}
				if( pSector->GetNearSector( TD_SOUTH ) && pSector->GetNearSector( TD_SOUTH )->GetNearSector( TD_EAST ) )
				{
					AGCMMAP_THIS->RecalcNormal		( pSector->GetNearSector( TD_SOUTH )->GetNearSector( TD_EAST ) , SECTOR_HIGHDETAIL	);
				}

				AGCMMAP_THIS->UnlockSectors		( TRUE													);
			}
			SaveSetChangeMoonee();
		}
	}

	return TRUE;
}