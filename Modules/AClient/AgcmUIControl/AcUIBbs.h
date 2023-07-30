#ifndef _ACUIBBS_H_
#define _ACUIBBS_H_

#include <AgcmUIControl/AcUIBase.h>
#include <AgcmUIControl/AcUIBbsContent.h>

#define ACUIBBS_COLUMN_NAME_MAX									64
#define ACUIBBS_CELL_CONTENT_LENGTH_MAX							64
#define	ACUIBBS_COLUMN_NUM_MAX									16
#define	ACUIBBS_CALLBACKPOINT_MAX_NUM							10
#define	ACUIBBS_PAGEMANAGER_NUM									12

enum 
{
	UICM_BBS_CHANGE_PAGE										= UICM_BASE_MAX_MESSAGE,
	UICM_BBS_CLICK_CONTENT,
	UICM_BBS_MAX_MESSAGE
};

enum HANFONT_ARRANGETYPE
{
	HANFONT_LEFTARRANGE											= 0,
	HANFONT_CENTERARRANGE,
	HANFONT_RIGHTARRANGE
};

typedef enum AcUIBbsCallbackPoint
{
	ACUIBBS_CLICK_CONTENT										= 0,
	ACUIBBS_CHANGE_PAGE,
	ACUIBBS_WINDOW_CLOSE,
} AcUIBbsCallbackPoint;

struct stAgcUIBbsColumnInfo 
{
	char szColumnName[ ACUIBBS_COLUMN_NAME_MAX ];
	UINT16 nColumnWidth;
	HANFONT_ARRANGETYPE eArrayType;
};

struct stAgcUIBbsCellContent
{
	UINT32				lIndex;									// �� Row�� ������ �ִ� ���� Index - ȭ��󿡴� ǥ�õ��� �ʴ´�. - Row�� �� Cell�� ���� Index�� ���´�. 
	char				szCellContent[ ACUIBBS_CELL_CONTENT_LENGTH_MAX ];	// Cell�� ��� String Info 
	RwRect				rectMyArea;
	RwRect				rectSelectArea;
};

struct stAcUIBbsInfo
{
	UINT8				m_cColumnNum;							// 0 ~ ACUIBBS_COLUMN_NUM_MAX
	RwV2d				m_v2dCellStart;							//CellStartPointer
	UINT16				m_nCellHeight;							//CellHekght
	UINT8				m_cRowNum;								//�̹������� Image ���� 

	UINT16				m_nAllPageNum;							//�� ������ �� 
	UINT16				m_nNowPageNum;							//������ ������ 
	UINT16				m_nGapColumnCell;						//Column Name�� Cell Content ������ Gap 
	UINT16				m_nColumnGap;							//Column ������ Gap
};

struct stAgcUIBbsPageManager
{
	INT32				lPageIndex;								//Page Index �� -2 : PrePage, -1 : NextPage 0�̸� �׸��� ���� 
	char				szPageIndex[ 10 ];				
	RwRect				rectPageSelectArea;				
};

// Content Click���� �Ͼ���� Command Message�� �Բ� ������. 
struct stAgcUIBbsCommandMessageInfo
{
	INT32				lControlID;
	UINT32				lSelectedIndex;
	UINT8				lSelectedRow;
	UINT8				lSelectedColumn;
};


/**

-- 2008. 06. 12. ���� ���̴� ���� AcUIRecruit ������ �����..
   ����� ���� �ִ� AcUIBbsContent �� AcUIBbs �������� ���̳� ������ ���ǰ� ������ �ʴµ�..
   ���� ������� �ʴ� ���� Ȯ�εǸ� ������ ���� ��ġ�ұ� ��.. by ����

*/
class AcUIBbs : public AcUIBase
{
public:
	AcUIBbs();
	virtual ~AcUIBbs();

	// Cell������ ��� ������ 
	stAgcUIBbsCellContent*								m_pstCellContent;
	stAgcUIBbsCommandMessageInfo						m_stCommandMessageInfo;

private:
	// �ʱ� Setting�� �Ǿ����� ���� 
	BOOL												m_bInitSetting;
		
	// Column ���� 
	stAgcUIBbsColumnInfo								m_stColumnInfo[ ACUIBBS_COLUMN_NUM_MAX ];

	// ���� �Խ��ǿ� �ʿ��� Data�� 
	stAcUIBbsInfo										m_stBbsInfo;

	// �Խ��� ������ ���� Window
	AcUIBbsContent										m_clContentWindow;

	// Page �Ŵ���
	stAgcUIBbsPageManager								m_stPageManager[ ACUIBBS_PAGEMANAGER_NUM ];

public:
	// �ʱ�ȭ �� ������
	void					InitInfoInput				( stAcUIBbsInfo *pstBbsInfo );
	BOOL					Destroy						( void );
	void					PageManagerSetting			( void );

	// �����Է�..
	BOOL					SetColumnInfo				( UINT8 cColumnIndex, char *pszColumnName, UINT16 nColumnWidth, HANFONT_ARRANGETYPE eArrayType = HANFONT_LEFTARRANGE );
	BOOL					SetColumnInfo				( UINT8 cColumnIndex, stAgcUIBbsColumnInfo *pstBbsColumnInfo );
	void					SetPageInfo					( UINT16 nAllPage,	UINT16 nNowPage );
	BOOL					SetCellText					( UINT8 nRow, UINT8 nColumn, char* pszContent );

	// �������.. �ش� ĭ�� ������ų� �ϴ°� �ƴϰ� Cell �� ����ִ� ���븸 ������
	void					ClearAllCellText			( void );							// ���� ����
	void					ClearCellText_ByRow			( UINT8 nRow );						// ���� ����
	void					ClearCellText				( UINT32 nRow, UINT32 nColumn );	// Cell �ϳ� ����

private:
	// Cell �޸� ���� �� ����
	void					_CreateCellContentMemory	( void );
	void					_DestroyCellContentMemory	( void );

	// ���� �ε����� ������ �����ϴ��� �˻��Ѵ�.
	BOOL					_IsValidArrayIndex			( UINT16 nArrayIndex );
	BOOL					_IsValidRowIndex			( UINT8 nRow );
	BOOL					_IsValidColumnIndex			( UINT8 nCol );
	BOOL					_IsValidPageIndex			( void );
	BOOL					_IsValidCallBackIndex		( UINT32 nCallBackIndex );

	// ���� ������ ���Դ����� �˻��Ѵ�.
	BOOL					_IsInAreaPageManager		( stAgcUIBbsPageManager* pPageManager, INT32 nMouseX, INT32 nMouseY );
	BOOL					_IsInAreaCell				( stAgcUIBbsCellContent* pCell, INT32 nMouseX, INT32 nMouseY );

	// ���� Cell �� ��ȯ�Ѵ�.
	stAgcUIBbsCellContent*	_GetCell					( UINT32 nRow, UINT32 nCol );
	stAgcUIBbsCellContent*	_GetCell					( UINT16 nArrayIndex );

	// �޼����� ����
	BOOL					_SendChangePage				( INT32 nNewPageNumber );
	BOOL					_SendClickCell				( UINT32 lIndex, INT32 nRow, INT32 nColumn );

	// Cell �׸���
	void					_DrawColumnName				( RwRect* pRect, INT32 nColCount );
	void					_DrawCell					( RwRect* pRect, INT32 nRowCount, INT32 nColCount );
	void					_DrawBbsContent				( void );
	void					_DrawPageManager			( void );

	//virtual function
public:
	virtual	void			OnPostRender				( RwRaster *raster );
	virtual	void			OnWindowRender				( void );
	virtual	BOOL			OnLButtonDown				( RsMouseStatus *ms	);	
	virtual	void			OnClose						( void );
	virtual BOOL			OnInit						( void );
};

#endif _ACUIBBS_H_

/*
		*�Խ��� ������
			Instance ����
			AddImage
			AddImageForContentWindow
			InitInfoInput
			SetColumn
			.
			.
			.
			Destroy

	

		* �� Cell�� �ڽ��� ������ rectMyArea�� ������									->	�� content�� �ش��ϴ� String�� ���� ��� 
		* �� Cell�� DrawText���� �������� String�� ������ ���� rectSelectArea�� ������.	->  �� content�� ���콺�� �����Ҷ� üũ �ϱ� ���Ͽ� ��� 

*/