#ifndef		_ACUILIST_H_
#define		_ACUILIST_H_

#include "AcUIBase.h"
#include "AuList.h"
#include "AcUIScroll.h"

#define ACUILIST_LIST_ITEM_STRING_LENGTH					16


// Command Message
enum 
{
	UICM_LIST_SCROLL										= UICM_BASE_MAX_MESSAGE,
	UICM_LIST_MAX_MESSAGE
};

// Command Message
enum 
{
	UICM_LISTITEM_MAX_MESSAGE								= UICM_BASE_MAX_MESSAGE	
};

class AcUIList;
class AcUIListItem;

typedef BOOL ( *AcUIListCallback )( AcUIList* pUIList, AcUIListItem* pUIListItem, PVOID pClass, PVOID pData, INT32* pHeight );

struct AcdUIListItemInfo  // -> �뷫 ID������ �ʿ��� 
{
	CHAR													m_szListItemString[ ACUILIST_LIST_ITEM_STRING_LENGTH ]; 
};

class AcUIList;

/********************************************AcUIListItem**************************************************/
class AcUIListItem : public AcUIBase 
{
public:
	AcUIList*												m_pcsList;
	INT32													m_lItemIndex;
	INT32													m_lListItemWindowHeight;

	AcUIListItem( void )
	{
		m_nType = TYPE_LIST_ITEM;
		m_lListItemWindowHeight = 0;
		m_pcsList =	NULL;
	}

	virtual	VOID			OnWindowRender					( void );

private :
	RwTexture*				_GetTextureByID					( INT32 nTextureID );
};

/***********************************************AcUIList***********************************************/
class AcUIList : public AcUIBase 
{
public:
	AcUIList( void );
	virtual ~AcUIList( void );

public:
	AcUIScroll*												m_pcsScroll;				// List Control�� ���� Scroll

	AuList< AcdUIListItemInfo* >							m_listItemInfo;
	AcUIListItem**											m_ppListItem;

	AcUIListCallback										m_pfConstructorCallback;	// ListItem Window Costructor Callback Function 
	PVOID													m_paConstructorClass;		// Pointer of Class that has Constructor callback Fucntion
	PVOID													m_pConstructorData;			// Pointer of Constructor Data

	AcUIListCallback										m_pfDestructorCallback;		// ListItem Window Destructor Callback Function 
	PVOID													m_paDestructorClass;		// Pointer of Class that has Destructor callback Function 
	PVOID													m_pDestructorData;			// Pointer of Destructor Data

	AcUIListCallback										m_pfRefreshCallback;		// ListItem Window Refresh Callback Function 
	PVOID													m_paRefreshClass;			// Pointer of Class that has Refresh callback Function 
	PVOID													m_pRefreshData;				// Pointer of Refresh Data

	INT32													m_lUseListItemInfoStringIndex;	// ���� ����ϰ� �ִ� ListItem Info String �� Index

	BOOL													m_bStartAtBottom;			// ���� Item�� �ؿ������� ����?

	INT32													m_lSelectedItemTextureID;	// ���õ� ������ TextureID
	INT32													m_lSelectedIndex;			// ���õ� ������

	RsMouseStatus											m_csMouseMoveStatus;
	
	// ���� ���� 
	INT32													m_lListItemWidth;
	INT32													m_lListItemHeight;			// �� List Item�� Height 
	INT32													m_lListItemStartX;
	INT32													m_lListItemStartY;

	INT32													m_lTotalListItemNum;		// �� List
	INT32													m_lVisibleListItemRow;		// ���� ���� ( ���� �����ϴ� ) List Item Row�� ��
	INT32													m_lCurrentExistListItemRow;	// m_lVisibleListItemRow��ŭ �־�� ������ Size���� ������ ���� �����Ҽ��� �ִ� 
	INT32													m_lListItemStartRow;		// ���� ó�� List Item Window�� Row

	INT32													m_lItemColumn;				// Item�� ��
	INT32													m_lTotalItemRow;			// Item�� �࿡ ���� Row

	BOOL					DeleteAllListItemInfo			( void );

	BOOL					SetListItemWindowTotalNum		( INT32 lTotalNum );
	BOOL					SetListItemWindowVisibleRow		( INT32 lVisibleRow );
	VOID					SetListItemWindowStartRow		( INT32 lStartRow, BOOL bDestroy = FALSE, BOOL bChangeScroll = TRUE );			// ȣ��ɶ����� update ���� 
	VOID					SetListItemWindowMoveInfo		( INT32 lItemWidth, INT32 lItemHeight, INT32 lItemStartX, INT32 lItemStartY );	// Window�� Move Info - ȣ��� �� ���� Window�� ��ġ �ٲ�
	VOID					GoNextListPage					( void );
	VOID					GoPreListPage					( void );	

	// Call back ���� 
	VOID					SetCallbackConstructor			( AcUIListCallback pfConstructor, PVOID pClass,	PVOID pData );
	VOID					SetCallbackDestructor			( AcUIListCallback pfDestructor, PVOID pClass, PVOID pData );
	VOID					SetCallbackRefresh				( AcUIListCallback pfDestructor, PVOID pClass, PVOID pData );

	// Set ListItem Info 
	BOOL					SetListInfo_String				( CHAR* pszString );
	BOOL					DeleteListInfo_String			( CHAR* pszString );
		
	VOID					SetListItemColumn				( INT32 lColumn );
	VOID					SetSelectedTexture				( INT32 lTextureID );
	VOID					SelectItem						( INT32 lSelectedIndex );

	inline	VOID			SetListScroll					( AcUIScroll *pcsScroll ) { m_pcsScroll = pcsScroll; }

	// virtual function overriding 
	virtual	VOID 			OnClose							( void );
	virtual	BOOL 			OnCommand						( INT32	nID, PVOID pParam );

private:
	AcUIListItem *			NewListItemWidnow				( INT32 lIndex, INT32 lHeightLeft, BOOL *pbFullHeight = NULL );	// List Item Window�� New�ϸ鼭 AddChild, Constructor ���� �����Ѵ� 
	BOOL					RefreshListItemWindow			( AcUIListItem *pListItem, INT32 lIndex, INT32 lHeightLeft, BOOL *pbFullHeight = NULL );	// List Item Window�� Refresh�Ѵ�.
	BOOL					DeleteListItemWindow			( AcUIListItem* pListItem );
	BOOL					SetMemoryppListItem				( void );
	CHAR*					GetListItemInfoString			( INT32 lIndex );
	void					MoveListItemWindow				( void );

public:
	AcUIListItem*			GetListItemPointer_Index		( INT32 lIndex );

	virtual AcUIListItem *	OnNewListItem					( INT32 lIndex );
	virtual VOID			RefreshList						( void );
	virtual VOID			OnChangeTotalNum				( void ) { };
	virtual BOOL			IsValidListItem					( AcUIListItem *pListItem, INT32 lIndex	) {	return TRUE; }

	virtual VOID			UpdateStartRowByScroll			( void );
	virtual VOID			UpdateScroll					( void );
	virtual	BOOL			OnMouseWheel					( INT32	lDelta );
	virtual	BOOL			OnMouseMove						( RsMouseStatus *ms	);
	virtual	BOOL			OnLButtonUp						( RsMouseStatus *ms	);
	virtual	VOID			OnWindowRender					( void );
};

#endif // _ACUILIST_H_