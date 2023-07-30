// ConstManager.h: interface for the CConstManager class.
//
// ���α׷����� ����ϴ� Constance Value�� �����Ѵ�.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONSTMANAGER_H__5800F305_4B80_48E4_BA5F_44D250EB6DA1__INCLUDED_)
#define AFX_CONSTMANAGER_H__5800F305_4B80_48E4_BA5F_44D250EB6DA1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CConstManager
{
public:
// ����Ŵ°�
	float	m_fSectorWidth			;	// ���� ���� 
	int		m_nHoriSectorCount		;	// ���� ���� ����.
	int		m_nVertSectorCount		;
	int		m_nSectorDepth			;	// ���� �ϳ��� ������.

// ���� �ȵŴ°�.
	char	m_strCurrentDirectory[ 1024 ];
	int		m_nPreviewMapSelectSize	;	// 

	int		m_nGeometryLockMode		;
	int		m_nVertexColorLockMode	;
	
	BOOL	m_bCameraBlocking		;	// ī�޶� ���� �ɸ��� �ϱ�.
	
	BOOL	m_bShowCollisionAtomic	;	// �ݸ�������� ȭ�鿡 ǥ��.

	CBitmap	m_bitmapLoadNormal		;
	CBitmap	m_bitmapLoadExport		;
	CBrush	m_brushLoadNormal		;
	CBrush	m_brushLoadExport		;

	// ������Ʈ �ٿ�� ���Ǿ� ǥ��
	BOOL m_bShowObjectBoundingSphere	;
	// ������Ʈ �ݸ��� ����� ǥ��
	BOOL m_bShowObjectCollisionAtomic;
	// ������Ʈ ��ŷ ����� ǥ��
	BOOL m_bShowObjectPickingAtomic	;
	// ������Ʈ ��  ������ ���� ǥ��
	BOOL m_bShowObjectRealPolygon	;
	// ������Ʈ â���� ��Ʈ�� ���� ǥ��
	BOOL m_bShowOctreeInfo			;

// ������Ʈ���� ����Ŵ°�..

	// ������ (2004-06-28 ���� 6:27:09) : �ε� Ÿ��..
	DWORD	m_nGlobalMode			;	// ������� �ͽ���Ʈ����.
	
	DWORD	m_nLoading_range_x1		;
	DWORD	m_nLoading_range_y1		;
	DWORD	m_nLoading_range_x2		;
	DWORD	m_nLoading_range_y2		;

	BOOL	m_bUsePolygonLock		;
	DWORD	m_bUseBrushPolygon		;
	
	//Save ����..
	DWORD	m_bMapInfo			;
	DWORD	m_bTileVertexColor	;
	DWORD	m_bTileList			;
	DWORD	m_bObjectList		;

	DWORD	m_bObjectTemplate	;
	DWORD	m_bDungeonData		;
	

	DWORD	m_bUseEffect		;
	
	// ��ī�� ����..
	FLOAT	m_fSkyWidth			;
	FLOAT	m_fFogFarClip		;
	FLOAT	m_fFogDistance		;
	DWORD	m_bUseFog			;
	DWORD	m_bUseSky			;

	DWORD	m_nLoadRange_Data	;
	DWORD	m_nLoadRange_Rough	;
	DWORD	m_nLoadRange_Detail	;

public:

	// �ͽ���Ʈ �ڵ�ȭ���� �����.
	struct	ExportAutomation
	{
		BOOL	bCompactData	;
		BOOL	bMapDetail		;
		BOOL	bObject			;
		BOOL	bMapRough		;
		BOOL	bTile			;
		BOOL	bMinimap		;
		BOOL	bServer			;

		CString	strFilename		;
		INT32	nDivision		;

		ExportAutomation()
		{
			bCompactData	= FALSE	;
			bMapDetail		= FALSE	;
			bObject			= FALSE	;
			bMapRough		= FALSE	;
			bTile			= FALSE	;
			bMinimap		= FALSE ;
			bServer			= FALSE	;
			nDivision		= 0		;
		}
	};
	
	ExportAutomation	m_stExportAutomateInfo;

public:

	
	BOOL	Save	();
	BOOL	Load	();

	BOOL	Update	();	// ���� ������ �ٲ�� ȣ�� ���.

	BOOL	StoreRegistry();

	void	LoadBmpResource();
	
	CConstManager();
	virtual ~CConstManager();
};

extern CConstManager g_Const;

#endif // !defined(AFX_CONSTMANAGER_H__5800F305_4B80_48E4_BA5F_44D250EB6DA1__INCLUDED_)
