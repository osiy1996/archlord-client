#ifndef		_ACUITOOLTIP_H_
#define		_ACUITOOLTIP_H_

#include <AgcmUIControl/AcUIBase.h>
#include <AuList.h>
#include <AgcSkeleton/AcDefine.h>

#define	ACUITOOLTIP_TTTEXTURE_NUM								9
#define	ACUITOOLTIP_TTBOX_LINE_REAL_WIDTH						8
#define ACUITOOLTIP_TTBOX_LINE_TEXTURE_WIDTH					16
#define	ACUITOOLTIP_TTBOX_LINE_CORNER_WIDTH						28

#define ACUITOOLTIP_TTBOX_INIT_WIDTH							32
#define ACUITOOLTIP_TTBOX_INIT_HEIGHT							16
#define ACUITOOLTIP_TTBOX_STRING_START_X						12
#define ACUITOOLTIP_TTBOX_STRING_START_Y						16
#define	ACUITOOLTIP_TTBOX_STRING_YGAP							2

#define ACUITOOLTIP_TTBOX_STRING_LINE_HEIGHT					14		// Font + Y_Gap

#define ACUITOOLTIP_LEFT_TOOLTIP								0x01
#define ACUITOOLTIP_RIGHT_TOOLTIP								0x02

class AgcUIWindow;

enum 
{
	UICM_TOOLTIP_MAX_MESSAGE									=		UICM_BASE_MAX_MESSAGE
};

// Tool Tip에 쓰일 String Struct - List로 관리된다 -
struct AgcdUIToolTipItemInfo
{
	BOOL														bNewLine;	// 이녀석이 TRUE이면 아래 값들은 의미가 없다 

	CHAR*														pszString;
	RwTexture*													pstTexture;

	INT32														lStartX;
	INT32														lStartY;
	INT32														lFontType;	
	DWORD														lColor;

	AgcdUIToolTipItemInfo( void )
	{
		bNewLine = FALSE;
		pszString =	NULL;
		pstTexture = NULL;
		lStartX	= lStartY = lFontType =	0;
		lColor = 0;		
	}
};

class AcUIToolTip : public AcUIBase 
{
public:
	AcUIToolTip( void );
	virtual ~AcUIToolTip( void );

	enum eSubTooltipType {
		sub_tooltip_equip,
		sub_tooltip_refinery,
		sub_tooltip_none,		
	};

public:
	static RwTexture*											m_pTTTexture[ ACUITOOLTIP_TTTEXTURE_NUM ];

public:	
	void						DrawToolTipBox					( void );		// Tool Tip Box를 그린다 
	void						SetBoxIm2D						( My2DVertex* vert, float x, float y, float w, float h );
	void						SetBoxIm2D2						( My2DVertex* vert, float x, float y, float w, float h );

	virtual	void 				OnWindowRender					( void );
	virtual	void 				OnClose							( void );

public:		// New Tool Tip UI System 

	AuList< AgcdUIToolTipItemInfo >								m_listStringInfo;
	INT32														m_lStringStartY;	// 현재 라인에서 String이 써지는 y좌표 

	BOOL						AddString						( CHAR* pszString, INT32 l_x = 10, DWORD lColor = 0xffffffff, INT32 lFontType = 0 );
	BOOL						AddToolTipTexture				( RwTexture* pstTexture, INT32 lWidth, INT32 l_x = 10, DWORD lColor = 0xffffffff, BOOL bIsOverWrite = FALSE );
	BOOL						AddNewLine						( INT32 lLineGap_Y = ACUITOOLTIP_TTBOX_STRING_YGAP );
	void						DeleteAllStringInfo				( void );

	VOID						EnableLineColor					( BOOL bEnable , DWORD dwColor = 0xffffffff ) { m_bEnableLineColor	=	bEnable;	m_dwLineColor	=	dwColor;	}

	void						DrawItemInfo					( void );

	INT32						GetLinePos						( void ) { return m_lStringStartY; }
	VOID						SetLinePos						( INT32 lLinePos ) { m_lStringStartY = lLinePos; }

	//@{ 2006/09/26 burumal
	VOID						SetParentUIWindow				( AgcUIWindow* pParentUIWindow ) { m_pParentUIWindow = pParentUIWindow; }
	AgcUIWindow*				GetParentUIWindow				( void ) { return m_pParentUIWindow; }
	//@}

	// Tooltip 윈도우를 움직일 수 있도록 한다.
	VOID						SetMoveWindow					(BOOL bMoveWindow);

	VOID						SetSubTooltipType				(eSubTooltipType type) { m_eSubTooltipType = type;}
	RwV2d						GetRenderPos					( UINT*	unTooltipStatus = NULL , INT nOffsetX = -1 , INT nOffsetY = -1 );			// 실제로 렌더링이 될 툴팁의 시작위치
	static BOOL					CBTooltipExit					( PVOID pClass , PVOID pData1 , PVOID pData2 , PVOID pButton );

	BOOL														m_bDrawTooltipBox;

	//@{ 2006/09/26 burumal
	AgcUIWindow*												m_pParentUIWindow;
	//@}

	void						ShowTooltip( BOOL bShow )		{ m_bViewToolTip = bShow; }
	eSubTooltipType				GetSubToolTipType()				{ return m_eSubTooltipType; }

	static	BOOL												m_bViewToolTip;			// Option창에서 설정

private :
	void*						_GetRasterVoidPtr				( INT32 nTextureIndex );

	void						_DrawToolTipBoxBody				( My2DVertex* pVert, float fPosX, float fPosY,
		float fCornerWidth, float fLineTextureWidth );
	void						_DrawToolTipBoxCorner			( My2DVertex* pVert, float fPosX, float fPosY,
		float fCornerWidth, float fLineTextureWidth );
	void						_DrawToolTipBoxEdge				( My2DVertex* pVert, float fPosX, float fPosY,
		float fCornerWidth, float fLineTextureWidth );

	void						_DrawLine						( INT32 nPosX , INT32 nPosY );

	BOOL						m_bEnableLineColor;
	DWORD						m_dwLineColor;

	eSubTooltipType				m_eSubTooltipType;
	INT							m_nOffsetX;
	INT							m_nOffsetY;
};

#endif // _ACUITOOLTIP_H_