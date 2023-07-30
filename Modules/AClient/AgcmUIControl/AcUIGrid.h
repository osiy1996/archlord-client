#ifndef	_ACUIGRID_H_
#define	_ACUIGRID_H_

#include <AgcmUIControl/AcUIBase.h>
#include <AgcmUIControl/AcUITexture.h>
#include <AgcSkeleton/AcDefine.h>

#define ACUIGRID_TWINKLE_TIMEGAP							500		// Grid Item Twinkle( �����Ÿ� ) Mode ���� Time Gap

class AgpdGridItem;
class AgpdGrid;

// End Drag�� ������ ������ ������ ���´� 
struct AgpdGridSelectInfo
{
	INT32													lGridRow;		// End Drag�� ���� Row
	INT32													lGridColumn;	// End Drag�� ���� Column
	
	AgpdGridItem*											pGridItem;		// End Drag�� ���� 
	AgcWindow*												pTargetWindow;	// DragDrop �� Target Window 
	INT32													lX;				// DragDrop �� Target Window���� ��ǥ
	INT32													lY;				// DragDrop �� Target Window���� ��ǥ

	AgpdGridSelectInfo( void )
	{
		lGridRow = 0;
		lGridColumn	= 0;
	
		pGridItem = NULL;
		pTargetWindow =	NULL;
	}
};

// Command Message
enum 
{
	UICM_GRIDITEM_DRAG_START								= UICM_BASE_MAX_MESSAGE,
	UICM_GRIDITEM_DRAG_END,
	UICM_GRIDITEM_SET_FOCUS,
	UICM_GRIDITEM_KILL_FOCUS,
	UICM_GRIDITEM_MAX_MESSAGE
};

// Command Message
enum
{
	UICM_GRID_DRAG_START									= UICM_BASE_MAX_MESSAGE,
	UICM_GRID_DRAG_END,
	UICM_GRID_DRAG_DROP,
	UICM_GRID_LBUTTON_ITEM_CLICK,
	UICM_GRID_RBUTTON_ITEM_CLICK,
	UICM_GRID_GRIDITEM_SET_FOCUS,
	UICM_GRID_GRIDITEM_KILL_FOCUS,
	UICM_GRID_MAX_MESSAGE
};

/* ����
	1. SetAgpdGriPointer(~~) �� ���ؼ� AgpdGrid*�� ���� �Ѱ��ش� - GridItemWindow������ Child�� Add
	2. SetGridItemDrawInfo() �� ȣ���� �ش� - �� ������ ������ Add�� Child�� MoveWindow�� �迭�Ѵ� 

	!!! �� �Լ��� ������ �ݵ�� �¾ƾ� �� !!!
*/
class AgcmUIControl;
class AcUIGrid;
class AcUIScroll;

class AcUIGridItem : public AcUITexture
{
public:
	AgpdGridItem*											m_ppdGridItem;
	AcUIGrid*												m_pParentGrid;

	static My2DVertex										m_vAlphaFan[ 7 ];

	AcUIGridItem( void )
	{
		m_ppdGridItem =	NULL;
		m_pParentGrid =	NULL;
		m_nType = TYPE_GRID_ITEM;
	}

public:
	virtual	BOOL 			OnLButtonDown					( RsMouseStatus *ms );
	virtual BOOL 			OnLButtonUp						( RsMouseStatus *ms );
	virtual BOOL 			OnDragDrop						( PVOID pParam1, PVOID pParam2 );
	virtual	void 			OnWindowRender					( void );
	virtual void 			OnSetFocus						( void );
	virtual void 			OnKillFocus						( void );

private:
	BOOL					DrawCoolTime( int lAbsoluteX, int lAbsoluteY, UINT8 nAlpha, RwTexture* pTexture, float fPercent );
};

class AcUIGrid : public AcUIBase
{
public:
	AcUIGrid( void );
	virtual ~AcUIGrid( void );

public:
	AgpdGrid*												m_pAgpdGrid;					// �� Ŭ������ �⺻�� �Ǵ� Grid
	AcUIGridItem**											m_apGridItemWindow;				// Grid Item �� �ش��ϴ� Window - SetAgpdGridPointer �Լ� ȣ��� �����ȴ� 	

	INT32													m_lGridItemWindowNum;			// Grid Item�� Num
	INT32													m_lNowLayer;					// ������ Layer

	INT32													m_lGridItemStart_x;
	INT32													m_lGridItemStart_y;
	INT32													m_lGridItemGap_x;
	INT32													m_lGridItemGap_y;
	INT32													m_lGridItemWidth;
	INT32													m_lGridItemHeight;

	BOOL													m_bDrawAreas;
	DWORD													m_dwAreaColor;
	UINT8													m_ucAreaAlpha;

	BOOL													m_bMoveItemCopy;				// default : FALSE - Item�� �̵��Ҷ� �� �ڸ��� ���� Window�� �����Ѵ� 
	AcUIGridItem											m_clCopyGridItem;				// ��ҿ��� Width, Height�� 0�̴�. 

	BOOL													m_bGridItemMovable;				// Grid Item �� ������ �� �ִ°��� ���� - default �� TRUE 

	BOOL													m_bDrawImageForeground;			// Grid Item �տ� �̹����� �׸��� ����
	
	INT32													m_lItemToolTipX;				// Item Tool Tip �� �׷��� X ��ǥ 
	INT32													m_lItemToolTipY;				// Item Tool Tip �� �׷��� Y ��ǥ 
	INT32													m_lItemToolTipID;				// �׷��� Item Tool Tip�� IID 
	AgpdGridItem*											m_pToolTipAgpdGridItem;			// ToolTip�� ���� AgpdGridItem Pointer
	
	BOOL													m_bGridItemBottomCountWrite;	// Grid Item �� Left �Ǵ� Right �ϴܿ� Count�� �����ΰ� ���� 

	INT32													m_lReusableDisplayTextureID;

	// Scroll ����
	AcUIScroll *											m_pcsGridScroll;
	INT32													m_lGridItemStartRow;
	INT32													m_lGridItemVisibleRow;

public:
	void					SetGridItemMovable				( BOOL bMovable );																	// Grid Item �� �����ϼ� �ִ°��� ���θ� Setting �Ѵ�  
	void					SetGridItemBottomCountWrite		( BOOL bBottomCount ) { m_bGridItemBottomCountWrite = bBottomCount;	}				// Grid Item �� Count�� �����ΰ��� ���� 	
	void					SetGridItemMoveItemCopy			( BOOL bMoveItemCopy ) { m_bMoveItemCopy = bMoveItemCopy; }							// Move Item �� Copy�� �Ѱ��ΰ��� ����
	void					SetGridItemDrawImageForeground	( BOOL bDrawImageForeground ) { m_bDrawImageForeground = bDrawImageForeground; }	// Move Item �� Copy�� �Ѱ��ΰ��� ����
	BOOL					GetGridItemMovable				( void ) { return m_bGridItemMovable; }
	BOOL					GetGridItemBottomCountWrite		( void ) { return m_bGridItemBottomCountWrite; }
	BOOL					GetGridItemMoveItemCopy			( void ) { return m_bMoveItemCopy; }
	BOOL					GetGridItemDrawImageForeground	( void ) { return m_bDrawImageForeground; }
	
	void					SetAgpdGridPointer				( AgpdGrid* pAgpdGrid );
	void					SetNowLayer						( INT32 lNowLayer );		// ������ Layer�� Setting�Ѵ� - �ʱⰪ�� 0
	void					SetGridItemDrawInfo				( INT32 lStartX, INT32 lStartY, INT32 lGapX, INT32 lGapY, INT32 lGridItemWidth, INT32 lGridItemHeight );

	AgpdGrid *				GetAgpdGrid						( void ) { return m_pAgpdGrid; }

	INT32					GetStartX						( void ) { return m_lGridItemStart_x; }
	INT32					GetStartY						( void ) { return m_lGridItemStart_y; }
	INT32					GetGapX							( void ) { return m_lGridItemGap_x; }
	INT32					GetGapY							( void ) { return m_lGridItemGap_y; }
	INT32					GetItemWidth					( void ) { return m_lGridItemWidth; }
	INT32					GetItemHeight					( void ) { return m_lGridItemHeight; }
	
	void					UpdateUIGrid					( void );	// m_pAgpdGrid Pointer�� �̿��ؼ� UI�� �ݿ��Ѵ� 
	AgpdGridSelectInfo*		GetDragDropMessageInfo			( void );	// ó������ ���� EndDragMessage�� �������� NULL�� ���� 
	AgpdGridSelectInfo* 	GetGridItemClickInfo			( void );
	BOOL					GetGridRowColumn				( RsMouseStatus* ms, INT32* lRow, INT32 * lColumn );
	BOOL					GetGridRowColumn				( INT32 lX, INT32 lY, INT32* lRow, INT32 * lColumn );
	BOOL					GetGridRowColumn				( INT32 lIndex, INT32* lRow, INT32* lColumn );

	void					SetDrawAreas					( BOOL bDrawAreas ) { m_bDrawAreas = bDrawAreas; }
	void					SetDrawColor					( DWORD dwColor, UINT8 ucAlpha ) { m_dwAreaColor = dwColor; m_ucAreaAlpha = ucAlpha; }

	void					OnWindowRender					( void );

	void					DeleteAllChild					( void );
	void					SetClickInfoTargetWindow		( AgcWindow* pWindow, INT32 lX, INT32 lY );

	BOOL					SetReusableDisplayImage			( INT32 lImageID );
	INT32					GetReusableDisplayImage			( void );

	// Scroll ����
	VOID					SetScroll						( AcUIScroll *pcsScroll );

private:
	BOOL													m_bDragDropMessageExist;	// ó������ ���� EndDragMessage�� ������ TRUE
	AgpdGridSelectInfo										m_stDragDropMessageInfo;
	BOOL													m_bGridItemClickInfoExist;	// ó������ ���� GridItemClickInfo �� �ִ� 
	AgpdGridSelectInfo										m_stGridItemClickInfo;		// Grid�� Click�� ���� 

	BOOL					SetGridItemMemory				( void );
	void					AddGridItemWindow				( void );					// Grid Item Window�� Add�� �ش� 
	virtual void			MoveGridItemWindow				( void );					// Grid Item Draw Info�� ������ �迭�Ѵ� - SkillTree���� ���¸� �����ϱ� ���� virtual���� 
	void					SetGridItemTextureAndInfo		( void );					// Grid Item Window�� Texture�� Setting�Ѵ� 
	INT32					GetIndex						( INT32 lRow, INT32 lColumn );
	AcUIGridItem*			GetGridItemWindow				( INT32 lRow, INT32 lColumn );
	AcUIGridItem*			GetGridItemWindowByPos			( INT32 x, INT32 y );
	void					SetCopyGridItem					( INT32 lGridItemIndex );
	void					ReleaseCopyGridItem				( void );

public:
	virtual BOOL			OnPostInit						( void );
	virtual void			OnClose							( void );
	virtual BOOL			OnDragDrop						( PVOID pParam1, PVOID pParam2 );
	virtual BOOL			OnCommand						( INT32	nID, PVOID pParam );
	virtual	BOOL			OnLButtonDown					( RsMouseStatus *ms );
	virtual	BOOL			OnLButtonUp						( RsMouseStatus *ms );
	virtual	BOOL			OnRButtonDown					( RsMouseStatus *ms );
	virtual	BOOL			OnRButtonUp						( RsMouseStatus *ms );
	virtual	BOOL			OnMouseWheel					( INT32	lDelta );

	virtual VOID			SetFocus						( VOID );
};

#endif			// _ACUIGRID_H_
