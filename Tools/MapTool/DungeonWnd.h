#if !defined(AFX_DUNGEONWND_H__2E636F4E_A7C5_4C21_9DDF_6A5BCE6EEAE4__INCLUDED_)
#define AFX_DUNGEONWND_H__2E636F4E_A7C5_4C21_9DDF_6A5BCE6EEAE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bmp.h"
#include "DungeonToolBar.h"
#include <vector>

#define DUNGEON_TEMPLATE_FILE	"dungeontemplate.ini"
#define DUNGEON_DATA_FILE		"Cave%04d.ini"

// DungeonWnd.h : header file
//

#define DTM_COMBO_H_START	20
#define DTM_COMBO_HEIGHT	40
#define DTM_TOOL_MENU_WIDTH	50

enum ToolbarMenu
{
	TBM_FLOOR	,	// �ٴ�
	TBM_SLOPE	,	// ���
	TBM_STAIRS	,	// ���
	TBM_WALL2	,	// ��
	TBM_WALL4	,	// ��
	TBM_FENCE	,	// ����
	TBM_PILLAR	,	// ���
	TBM_DOME4	,	// õ��..
	TBM_DOME	,
	TBM_SELECT		// ���� �Ǵ� �ƽ���..
};

const int TB_SIZES[] = 
{
	1 , //TBM_FLOOR	,	// �ٴ�
	1 , //TBM_SLOPE	,	// ���
	1 , //TBM_STAIRS	,	// ���
	2 , //TBM_WALL2	,	// ��
	4 , //TBM_WALL4	,	// ��
	1 , //TBM_FENCE	,	// ����
	1 , //TBM_PILLAR	,	// ���
	4 , //TBM_DOME4	,	// õ��.. õ���� 4Ÿ���� �⺻..
	1 , //TBM_DOME ,    // 1Ÿ�� ¥�� ����
};
 
enum DOME_TYPE
{
	DT_BASE		= 0,	// ����
	DT_SIDE		= 1,	// ���̵�
	DT_CORNAR	= 2// �𼭸�
};

enum	DungeonTurnAngle
{
	DTA_0_CW	,
	DTA_90_CW	,
	DTA_180_CW	,
	DTA_270_CW	
};

enum	Direction
{
	DIR_UP			,
	DIR_DOWN		,
	DIR_LEFT		,
	DIR_RIGHT
};

#define MAX_DUNGEON_INDEX_COUNT	3
#define DUNGEON_LAYER_GAP		30.0f
#define MAX_DUNGEON_HEIGHT_COUNT	12

struct CDungeonTemplate
{
public:
	struct TemplateInfo
	{
		UINT32	uIndex		;
		UINT32	uSize		;

		TemplateInfo():uIndex( 0 ),uSize( 1 ) {}
	};

	UINT32			uTID	;	// ���ø�  ID
	CString			strName	;	//���ø� �̸�

	BOOL			bDomeHeight	;
	FLOAT			fDomeHeight	;
	TemplateInfo	uTemplateIndex[ TBM_SELECT ][ MAX_DUNGEON_INDEX_COUNT ];//�ٴڿ�����Ʈ ���ø� �ε��� ( 4���� )

	// ���� ����..
	FLOAT			afSampleHeight[ MAX_DUNGEON_HEIGHT_COUNT ];

	CDungeonTemplate():fDomeHeight( 0.0f )
	{
		// �ʱ�ȭ,,
		uTID	= 0;
		strName	= "�̸�������";
		bDomeHeight	= FALSE;

		for( int i = 0 ; i < MAX_DUNGEON_HEIGHT_COUNT ; i ++ )
		{
			afSampleHeight[ i ] = 100.0f * ( FLOAT ) i;
		}
	}

	void	copy( const CDungeonTemplate & param	)
	{
		this->uTID		= param.uTID	;
		this->strName	= param.strName	;

		for( int i = 0 ; i < TBM_SELECT ; ++ i )
		{
			for( int j = 0 ; j < MAX_DUNGEON_INDEX_COUNT ; ++j )
			{
				this->uTemplateIndex[ i ][ j ] = param.uTemplateIndex[ i ][ j ];
			}
		}

		for( int i = 0 ; i < MAX_DUNGEON_HEIGHT_COUNT ; i ++ )
		{
			afSampleHeight[ i ] = param.afSampleHeight[ i ];
		}
	}
};

//class	CDungeonSectorData
//{
//	class CSegment
//	{
//		UINT32	uTemplateIndex	[ MAX_DUNGEON_INDEX_COUNT ]; // ��Ÿ�Ͽ� 4������ ��.. �ٴ� , �� , ��� , ����.. ����?...
//		UINT32	uTurnAngle		[ MAX_DUNGEON_INDEX_COUNT ]; // �� ������Ʈ�� ȸ�� ����.. 0 1 2 3 �ð�������� �����..
//		FLOAT	fHeight			; // ����..
//	};
//
//	CSegment	pTile[ 1024 ]; // 32 * 32 .. ���͸� 32�����..
//};

/////////////////////////////////////////////////////////////////////////////
// CDungeonWnd window


class CDungeonWnd : public CWnd , ApModule
{
// Construction
public:
	CDungeonWnd();

// Attributes
public:

	AuList< CDungeonTemplate >	m_listTemplate;
	CDungeonTemplate *			GetTemplate				( int TID );
	CDungeonTemplate *			GetCurrentTemplate		();
	UINT32						GetCurrentTemplateID	();

	UINT32						GetSize( int nType )
	{
		CDungeonTemplate * pTemplate = GetCurrentTemplate();
		if( pTemplate )
		{
			if( pTemplate->uTemplateIndex[ nType ]->uSize > 0)
				return pTemplate->uTemplateIndex[ nType ]->uSize;
			else
				return TB_SIZES[ nType ] * 2;
		}
		else
			return TB_SIZES[ nType ] * 2;
	}

	void						RemoveAllElement		();

	BOOL						LoadTemplate();
	BOOL						SaveTemplate();

	BOOL						LoadDungeon();
	BOOL						SaveDungeon();

	struct DungeonElement
	{
		UINT32	uTemplateID		;	// ���� ���ø� ID
		UINT32	uType			;	// Ÿ��..

		INT32	nObjectIndex	;	// ��ũ�� ������Ʈ �ε���.

		UINT32	uIndex			;	// �ش� ���ø��� �ε��� ( 0~2 ���� �ϵ�.. )

		// ��ġ����..
		UINT32	uOffsetX		;
		UINT32	uOffsetZ		;
		UINT32	uWidth			;	// ����
		UINT32	uHeight			;	// ����

		FLOAT	fHeight			;	// ����
		UINT32	uTurnAngle		;	

		CDungeonTemplate	* pTemplate;

		DungeonElement()
		{
			Clean();
		}

		void Clean()
		{
			uTemplateID		= 0		;
			uType			= 0		;
			uIndex			= 0		;
			nObjectIndex	= 0		;	// �����°� ����..

			uOffsetX		= 0		;
			uOffsetZ		= 0		;
			fHeight			= 0.0f	;
			uTurnAngle		= 0		;

			uWidth			= 1		;
			uHeight			= 1		;

			pTemplate		= NULL	;
		}
	};

	AuList< DungeonElement >	m_listDungeonElement[ TBM_SELECT ];

	UINT32						m_uSelectedType	;
	AuNode< DungeonElement >	* m_pSelectedElement;

	enum PORTION
	{
		LEFTTOP		,
		RIGHTTOP	,
		LEFTBOTTOM	,
		RIGHTBOTTOM
	};
	PORTION						m_eSelectedPortion;
	
	// ���� ���۸�..
	CDC				m_memDC		;
	CBitmap			m_memBitmap	;
	CRect			m_memRect	;	//
	CDC *			GetMemDC()	;

	CBmp			m_bmpDivision	;
	CBmp			m_bmpHeight		;

	void			UpdateDungeonPreviewBitmap	( BOOL bReload = TRUE );
	void			UpdateDungeonHeightBitmap	();

	int			m_nGridWidth	;	// �׸��� ����
									// �� Ÿ�� �Ÿ��� ��Ÿ��.
	int			m_nStepSize		;	// 

	INT32		m_nLeftX		;	// �»� ��ǥ..
	INT32		m_nTopZ			;
			// ���� �׸��� ���̿� ������
			// �ȼ� ���� �ɼ����� ó���ǹǷ�.
			// �׸��� ������ �ٲ��� �̰͵� ������� ����ž����..

	COLORREF	m_colorGrid		;
	COLORREF	m_colorBack		;
	COLORREF	m_colorGridEdge	;
	COLORREF	m_colorGridTile	;
	COLORREF	m_colorGridHalf	;

	// ������ (2005-05-10 ���� 11:17:47) : 
	// õ��׸���� ������ ����..
	vector< POINT >	m_vectorSelectedPoint;
	INT32	GetMovement( INT32 nOffset );

public:
	// ��� �� ���..
	BOOL		m_bRbuttonDown	;	// ������ ��ư ������ ����..
	CPoint		m_pointLastPress;

public:
	int			GetGridWidth() { return m_nGridWidth;}
	int			SetGridWidth( int nWidth );

	// �׸��� ǥ�ÿ�
	enum DETYPE
	{
		DET_NORMAL		,
		DET_SELECTED	,
		DET_HEIGHT		,
		DET_MAX
	};
	BOOL		DrawElement( CDC * pDC , UINT32 uType , DungeonElement * pElement , UINT32 uSelected = DET_NORMAL );

	int			GetXOffset( int nScreenX , int * pDep = NULL)
	{
		int	nDelta = ( nScreenX - m_nLeftX );
		if( pDep )
		{
			* pDep = ( nDelta - ( nDelta / m_nGridWidth ) * m_nGridWidth ) * 100 / m_nGridWidth;
		}

		return nDelta / m_nGridWidth;
	}
	int			GetZOffset( int nScreenY , int * pDep = NULL )
	{	
		int	nDelta = ( nScreenY - m_nTopZ );
		if( pDep )
		{
			* pDep = ( nDelta - ( nDelta / m_nGridWidth ) * m_nGridWidth ) * 100 / m_nGridWidth;
		}

		return nDelta / m_nGridWidth;
	}

	int			GetXOffsetLocal( int nScreenX )
	{
		int	nOffset			= GetXOffset( nScreenX );
		int nDelta			= ( nScreenX - m_nLeftX ) - nOffset * m_nGridWidth;
		int nLocalOffset	= ( nDelta * 3 ) / m_nGridWidth;

		ASSERT( nLocalOffset >= 0 );
		if( nLocalOffset >= 3 ) nLocalOffset = 2;
		return nLocalOffset;
	}
	
	int			GetZOffsetLocal( int nScreenY )
	{
		int	nOffset			= GetZOffset( nScreenY );
		int nDelta			= ( nScreenY - m_nTopZ ) - nOffset * m_nGridWidth;
		int nLocalOffset	= ( nDelta * 3 ) / m_nGridWidth;

		ASSERT( nLocalOffset >= 0 );
		if( nLocalOffset >= 3 ) nLocalOffset = 2;
		return nLocalOffset;
	}

	int			GetXOffsetStep( int nScreenX ){	return GetXOffset( nScreenX ) - GetXOffset( nScreenX ) % m_nStepSize;}
	int			GetZOffsetStep( int nScreenY ){	return GetZOffset( nScreenY ) - GetZOffset( nScreenY ) % m_nStepSize;}

	int			GetScreenX( int nOffsetX ){	return m_nLeftX + nOffsetX * m_nGridWidth;}
	int			GetScreenY( int nOffsetZ ){	return m_nTopZ + nOffsetZ * m_nGridWidth;}

	BOOL		AddElement		( UINT32 uType , DungeonElement * pEle );
	BOOL		RemoveElement	( UINT32 uType , AuNode< DungeonElement > * pNode );
	BOOL		UpdateElement	( UINT32 uType , DungeonElement * pEle );
		// DungeonElement �������� �� ������Ʈ�� ������Ʈ�� ( �ַ� �̵� )

	FLOAT	__GetWorldXFromElement( UINT32 uType , DungeonElement * pElement );
	FLOAT	__GetWorldZFromElement( UINT32 uType , DungeonElement * pElement );
	FLOAT	__GetWorldDeltaYFromElement( UINT32 uType , DungeonElement * pElement );

	void		Apply( BOOL bAdjustHeight = FALSE );

	FLOAT		GetXPosFromOffset( UINT32 uOffsetX );
	FLOAT		GetZPosFromOffset( UINT32 uOffsetZ );
	INT32		GetXOffsetFromPos( FLOAT fPosX );
	INT32		GetZOffsetFromPos( FLOAT fPosZ );

	void		MoveToPosition( RwV3d * pPos );

	void		Idle();

	BOOL		m_bControlState;

	AuNode< DungeonElement > *	GetElement( UINT32 uType , int nOffsetX , int nOffsetZ , int nDeltaX = 0 , int nDeltaZ = 0 );
	DungeonElement *			GetElementByObjectIndex( INT32 nObjectIndex );
	BOOL		DeleteSelected();
public:
	// Selection ����..
	enum	SELECTMODE
	{
		SM_NONE		,
		SM_RANGE	,
		SM_LINE
	};
	SELECTMODE	m_nSelectMode			;
	int			m_SelectedPosX1			;
	int			m_SelectedPosZ1			;
	int			m_SelectedPosX2			;
	int			m_SelectedPosZ2			;
	
// Operations
public:

	// ���� ���� ��Ʈ����..
	static BOOL	DungeonObjectStreamWriteCB	(PVOID pData, ApModule *pClass, ApModuleStream *pcsStream);
	static BOOL	DungeonObjectStreamReadCB	(PVOID pData, ApModule *pClass, ApModuleStream *pcsStream);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDungeonWnd)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDungeonWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDungeonWnd)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
};

// Ŭ������ ��ü ���̸� ���س���..
// ���� ���̸� �˾Ƴ����� �Ҷ� �����..
FLOAT	__GetClumpHeight( RpClump * pClump );

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DUNGEONWND_H__2E636F4E_A7C5_4C21_9DDF_6A5BCE6EEAE4__INCLUDED_)

/*
 *	
 ���� ������ ���α׷� ����..

  ����Ÿ ����..
  class CDungeonTemplate
  {
	���ø� �̸�
	�ٴڿ�����Ʈ ���ø� �ε��� ( 4���� )
	�� ������Ʈ ���ø� �ε��� 
	���� ������Ʈ ���ø� �ε���..
	��
	���
	���
	���
  }

  ���� ����Ÿ�� ���� ����Ÿ ������ 1:1�� �����Ǹ�
  Ÿ���� ���� Ÿ�Ͽ� �ι�� ������ ����Ѵ�.
  ���͵���Ÿ�� �߰�����Ÿ�� ������ Ok �Ϸ���? ;;
  class	CDungeonSectorData
  {
	class CSegment
	{
		uTemplateIndex[ 4 ]; // ��Ÿ�Ͽ� 4������ ��.. �ٴ� , �� , ��� , ����.. ����?...
		uTurnAngle[ 4 ]; // �� ������Ʈ�� ȸ�� ����.. 0 1 2 3 �ð�������� �����..
		FLOAT	fHeight	; // ����..
	};

	CSegment	pTile[ 32 * 32 ];

	GetSegment~
	���..
  }


  1, ���ø� ����Ʈ â
  ���� ����Ʈ�� ���ø� �ε����� ������..
  �װɷ� ���..
  ���ø��� �����ϸ� ������ ��.
  Map/DungeonTemplate.ini
  ���ٰ� ������..
  ������ �׳� ����ϰ�.

  2, ����..
  ���ø� �����ϰ� �޴�����..
  ���� �ϴ� �͸� ������ �Ϻκ��� ������..

  ���ñ��� �̱��� ����..
 */
