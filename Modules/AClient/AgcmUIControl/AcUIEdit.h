#ifndef			_ACUIEDIT_H_
#define			_ACUIEDIT_H_

#include <AgcmUIControl/AcUIBase.h>

#define	ACUI_EDIT_DEFAULT_BUFFER_SIZE					2048
#define ACUI_EDIT_LF_SIZE								8
#define ACUI_EDIT_MAX_LINE								300
#define ACUI_EDIT_LEFT_MARGIN							10
#define ACUI_EDIT_DEFAILT_LF							"\n"

// Command Message 
enum
{
	UICM_EDIT_INPUT_END									= UICM_BASE_MAX_MESSAGE,	// Input�� ������ ó���� �ֶ�	
	UICM_EDIT_INPUT_TAB,															// Editâ�� Tab�� �ƴ�
	UICM_EDIT_INPUT_SHIFT_TAB,														// Editâ�� Shift + Tab�� �ƴ�
	UICM_EDIT_ACTIVE,																// Editâ�� Active�ȴ�.
	UICM_EDIT_DEACTIVE,																// Editâ�� Deactive�ȴ�.
	UICM_EDIT_MAX_MESSAGE
};

// Tool Tip�� ���� String Struct - List�� �����ȴ� -
typedef struct AgcdUIEditStringInfo
{
	BOOL												bNewLine;		// �̳༮�� TRUE�̸� �Ʒ� ������ �ǹ̰� ���� 
	CHAR*												szString;

	INT32												lOffsetX;
	INT32												lOffsetY;
	INT32												lWidth;
	INT32												lHeight;
	INT32												lFontType;	
	INT32												lLineIndex;
	INT32												lStringLength;

	BOOL												bBold;
	DWORD												dwColor;
	DWORD												dwBGColor;

	AgcdUIEditStringInfo *								pstNext;

	AgcdUIEditStringInfo( void )
	{
		bNewLine		= FALSE;
		szString		= NULL;
		lOffsetX		= 0;
		lOffsetY		= 0;
		lWidth			= 0;
		lFontType		= 0;
		dwColor			= 0;
		dwBGColor		= 0;
		bBold			= FALSE;
		pstNext			= NULL;
		lLineIndex		= 0;
		lStringLength	= -1;
	}
} AgcdUIEditStringInfo;

class AcUIScroll;
struct tag_AgcdFontClippingBox;
typedef struct tag_AgcdFontClippingBox AgcdFontClippingBox;

class AcUIEdit : public AcUIBase 
{
public:
	AcUIEdit( BOOL bPassword = FALSE, BOOL bNumber = FALSE );
	virtual ~AcUIEdit( void );

public:
	enum LineControl
	{
		CURRENTLINE	= 0,														// ������
		NEWLINE	= 1,															// ������
		CONTINOUSLINE = 2														// �������ε� �̾����� ��..
	};

	CHAR*												m_szText;
	CHAR*												m_szTextDisplay;

	INT32												m_lHotkey;
	AgcdUIHotkeyType									m_eHotkeyType;

	ApSafeArray< CHAR, ACUI_EDIT_LF_SIZE + 1 >			m_szLineDelimiter;
	ApSafeArray< INT32, ACUI_EDIT_MAX_LINE + 1 >		m_alStringStartY;
	ApSafeArray< INT32, ACUI_EDIT_MAX_LINE >			m_alStringHeight;
	ApSafeArray< INT32, ACUI_EDIT_MAX_LINE >			m_alStringWidth;
	ApSafeArray< INT32, ACUI_EDIT_MAX_LINE >			m_alStringLength;

	CHAR												m_szComposing[ 256 ];

	AgcdUIEditStringInfo*								m_pstStringInfoHead;
	AgcdUIEditStringInfo*								m_pstStringInfoTail;
	AgcdUIEditStringInfo*								m_pstStringInfoCursor;

	INT32												m_lTextLength;			// Text ����
	INT32												m_lTextDisplayLength;	// Display�Ǵ� Text ����
	INT32												m_lDelimiterLength;		// Delimiter ����

	INT32												m_lCursorPosX;			// Cursor X ��ǥ
	INT32												m_lCursorPosY;			// Cursor Y ��ǥ
	INT32												m_lCursorPoint;			// ���� Cursor Point (m_szText�������� Index)
	INT32												m_lCursorPointDisplay;	// Display�Ǵ� Cursor Point

	UINT32												m_lCursorCount;			// 0-4 none 5-9 draw cursor
	UINT32												m_lLastTick;

	INT32												m_lCursorRow;			// ���� Cursor�� Row
	INT32												m_lCursorColumn;		// ���� Cursor�� StringItem���� ������ Column

	AcUIScroll*											m_pcsVScroll;
	BOOL												m_bVScrollLeft;

	INT32												m_lBoxWidth;
	INT32												m_lOriginalBoxWidth;

	BOOL												m_bPasswordEdit;		// �н����� ��� Edit�� '*'���
	BOOL												m_bForHotkey;			// Hotkey��
	BOOL												m_bNumberOnly;			// Number Only Edit?
	BOOL												m_bReadOnly;			// Read Only? (No Edit)
	BOOL												m_bEnableTag;			// Is Tag enabled? (<>)
	BOOL												m_bMultiLine;
	BOOL												m_bAutoLF;				// �ڵ� �� �ٲ�
	BOOL												m_bFilterCase;			// ��ҹ��� �ڵ� ����
	BOOL												m_bReleaseEditInputEnd;	// ���͸� ������ Edit�� Release�ɰ��ΰ�?
	BOOL												m_bEnableTextLapping;	// �ܾ� �پ��ִ°� ������������ �ѱ�� ( ������ )
	BOOL												m_bChangeText;			// Ű���尡 ������.

	UINT32												m_ulTextMaxLength;		// �ִ� Text ����

	UINT32												m_ulLine;				// ���� Line
	INT32												m_lStartLine;			// Display�Ǵ� ùLine

	INT32												m_lStartX;				// Clipping�� Text X ��ǥ
	INT32												m_lStartY;				// Clipping�� Text X ��ǥ

	INT32												m_nOffSetFromLeftEdge;	// ���ʿ������� ����
	INT32												m_nOffSetFromTopEdge;	// ���ʿ������� ����

	BOOL												m_bIsShowReadingWin;	// reading window show/hide flag
	BOOL												m_bIsShowCandidateWin;	// candidadte window show/hide flag

	//@{ 2006/07/02 burumal
	static AcUIDefalutCallback							m_pCBHotkeyOverwriteCheck;
	static PVOID										m_pHotkeyCheckTargetInstacne;
	//@}

	// Virtual Functions
public:
	virtual	VOID				OnClose					( void );				// ������ , �׷��� ����Ÿ ������. WindowUI �� ��� Close�Ǵ� ����,  FullUI�� ��� �ٸ� UI�� Setting�ű� ��.
	virtual VOID				OnAddImage				( RwRaster*	pRaster );
	virtual VOID				OnPostRender			( RwRaster* pRaster	);	// ���� ������ ��.
	virtual VOID				OnWindowRender			( void );				// ���� Render�ϴ� �Լ� 
	virtual BOOL				OnLButtonDown			( RsMouseStatus *ms	);	
	virtual BOOL				OnChar					( CHAR* pChar, UINT lParam );
	virtual	BOOL				OnKeyDown				( RsKeyStatus* ks );
	virtual	VOID				OnMoveWindow			( void );
	virtual	BOOL				OnCommand				( INT32	nID, PVOID pParam );
	virtual	BOOL				OnMouseWheel			( INT32	lDelta );

	virtual BOOL				OnIdle					( UINT32 ulClockCount );
	virtual BOOL				OnIMEComposing			( CHAR* composing, UINT lParam );

	virtual BOOL				OnIMEReading			( void );				// reading window �׸���
	virtual BOOL				OnIMEHideReading		( void );				// reading window ���߱�
	virtual BOOL				OnIMECandidate			( void );				// candidate window �׸���
	virtual BOOL				OnIMEHideCandidate		( void );				// candidate window ���߱�

	virtual VOID				OnEditActive			( void );
	virtual VOID				OnEditDeactive			( void );

	//VOID	PasswordEditOnPostRender( RwRaster *raster );	// Password Edit�̸� �� �Լ����� OnPostRender	

private :
	INT32						_GetStringWidth			( INT32 nLineIndex );
	INT32						_GetStringHeight		( INT32 nLineIndex );
	INT32						_GetStringStartPosY		( INT32 nLineIndex );

	void						_DrawStringAlignCenter	( AgcdUIEditStringInfo* pStringList,
		AgcdFontClippingBox* pClippingBox, INT32 nFontType,
		INT32 nAbsX, INT32 nAbsY, INT32 nScrollMargin );
	void						_DrawStringAlignRight	( AgcdUIEditStringInfo* pStringList,
		AgcdFontClippingBox* pClippingBox, INT32 nFontType,
		INT32 nAbsX, INT32 nAbsY, INT32 nScrollMargin );
	void						_DrawStringAlignLeft	( AgcdUIEditStringInfo* pStringList,
		AgcdFontClippingBox* pClippingBox, INT32 nFontType,
		INT32 nAbsX, INT32 nAbsY, INT32 nScrollMargin );
	void						_DrawCaretComposer		( INT32 nAbsX, INT32 nAbsY, INT32 nOffSetX, INT32 nScrollMargin );

	BOOL						_ProcessKeyEnter		( char* pInputChar );
	BOOL						_ProcessKeyESC			( char* pInputChar );
	BOOL						_ProcessKeyBackSpace	( char* pInputChar );
	BOOL						_ProcessKeyTab			( char* pInputChar );
	BOOL						_ProcessKeyText			( char* pInputChar );

	BOOL						_ProcessKeyLeft			( void );
	BOOL						_ProcessKeyRight		( void );
	BOOL						_ProcessKeyUp			( void );
	BOOL						_ProcessKeyDown			( void );
	BOOL						_ProcessKeyDelete		( void );

	BOOL						_UpdateCursorPosition	( AgcdUIEditStringInfo* pStringList );
	BOOL						_UpdateScrollBarVert	( void );

	VOID						_ParseLine				( CHAR *szLine, INT32 lStringLength = -1, BOOL bChatting = FALSE );
	VOID						_ParseTag				( CHAR *szTag, INT32 *plFontType, DWORD *pdwColor, DWORD *pdwBGColor, BOOL *pbBold );


	// �Լ���..
public :
	inline	VOID				SetBoxWidth(INT32 w);

	inline INT32				GetBoxWidth				( void ) { return m_lOriginalBoxWidth; }
	inline const CHAR*			GetText					( void ) { return m_szText;	}

	AgcdUIEditStringInfo* 		GetParseText			( void ) { return m_pstStringInfoHead; }

	BOOL						SetText					( CHAR *szText = NULL, BOOL bChatting = FALSE );
	VOID						ClearText				( void );
	VOID						SetTextStartLine		( INT32 nLine = 0 );

	AgcdUIEditStringInfo*		AddStringItem			( CHAR *szString, DWORD dwColor, INT32 lFontType = -1, LineControl eNewLine = CURRENTLINE, INT32 lStringLength = -1, DWORD dwBGColor = 0x00000000, BOOL bBold = FALSE , BOOL bChatting = FALSE );
	VOID						ClearStringInfo			( void );

	BOOL						SetMeActiveEdit			( void );
	BOOL						ReleaseMeActiveEdit		( void );

	BOOL						CheckSOL				( BOOL bFirstLine = FALSE );	// ���� ���� Cursor ��ġ�� Start Of Line�̸� TRUE (bFirstLine : ���� ó�� ������ SOL�� OK?)
	BOOL						CheckEOL				( void );						// ���� ���� Cursor ��ġ�� End Of Line�̸� TRUE

	BOOL						SetLineDelimiter		( CHAR *szDelimiter );

	VOID						SetTextMaxLength		( UINT32 ulLength );
	inline UINT32				GetTextMaxLength		( void ) { return m_ulTextMaxLength; }

	VOID						SetVScroll				( AcUIScroll* pcsScroll );
	inline AcUIScroll*			GetVScroll				( void ) { return m_pcsVScroll;	}

	VOID 						DrawReadingWindow		( const float fPosX, const float fPosY );	//reading window �׸���
	VOID 						DrawCandidateWindow		( const float fPosX, const float fPosY );	//candidate window �׸���

	// Hotkey ����
	BOOL						SetHotkey				( INT32 lScanCode );	
	BOOL						SetHotkey				( AgcdUIHotkeyType eType, INT32 lScanCode );
	BOOL						GetHotkey				( INT32 *plScanCode, AgcdUIHotkeyType *peType );

	//@{ 2006/07/02 burumal
	static VOID	SetCallbackHotkeyOverwriteCheck( AcUIDefalutCallback pCB, PVOID pTargetInstance )
	{ 
		m_pCBHotkeyOverwriteCheck = pCB;
		m_pHotkeyCheckTargetInstacne = pTargetInstance;
	}	
	//@}

	// IsDBC�� (str-1)�� ����ϱ� ������ ���ǰ� �ʿ��ϴ�.
	bool						IsDBC					( unsigned char* str ) { return ( this->*IsDBCPtr )( str ); }

private:
	void						InitFuncPtr				( void );

	// �ٱ��� ���� ����
	bool 						(AcUIEdit::*IsDBCPtr)	( unsigned char* str );
	bool 						IsDBC_kr				( unsigned char* str );
	bool 						IsDBC_cn				( unsigned char* str );
	bool 						IsDBC_jp				( unsigned char* str );
	bool 						IsDBC_en				( unsigned char* str );
};

#endif			//_ACUIEDIT_H_

