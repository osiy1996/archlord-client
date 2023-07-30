#pragma once

#include "MyEngine.h"
#include "UITileList_PluginBase.h"
#include "SpawnDataLoad.h"
#include "AgpmPath.h"

#include <list>


#define		ID_COMBO_SPAWNNAME			23230
#define		ID_COMBO_PATHTYPE			23231

#define		ID_BUTTON_DELETE_PATH		30003
#define		ID_BUTTON_ONE_SAVE			30004
#define		ID_BUTTON_ALL_SAVE			30005

#define		ID_BUTTON_CAMERA_MOVE		30006

class CPlugin_PathEditor :
	public CUITileList_PluginBase
{
public:
	struct ClumpData
	{
		ClumpData( RpClump * clump, size_t pathIdx, size_t pointIdx ) : clump(clump), pathIdx(pathIdx), pointIdx(pointIdx) {}
		RpClump * clump;
		int pathIdx;
		int pointIdx;
	};

	CPlugin_PathEditor(void);
	virtual ~CPlugin_PathEditor(void);

	virtual BOOL			OnSelectedPlugin		( VOID );
	virtual BOOL			OnDeSelectedPlugin		( VOID );

	virtual BOOL			OnWindowRender			( VOID );

	virtual BOOL			OnLButtonDownGeometry	( RwV3d * pPos );
	virtual BOOL			OnLButtonUpGeometry		( RwV3d * pPos );
	virtual BOOL			OnRButtonDownGeometry	( RwV3d * pPos );
	virtual BOOL			OnMouseMoveGeometry		( RwV3d * pPos );
	virtual BOOL			Window_OnLButtonDown	( RsMouseStatus *ms	);

	virtual	BOOL			Window_OnKeyDown		( RsKeyStatus *ks );

	DECLARE_MESSAGE_MAP()

	afx_msg VOID			OnPaint					( VOID );
	afx_msg INT				OnCreate				( LPCREATESTRUCT lpCreateStruct );
	afx_msg LRESULT			WindowProc				( UINT message, WPARAM wParam, LPARAM lParam );

	HBRUSH					OnCtlColor				( CDC * pDC, CWnd * pWnd, UINT nCtlColor );

private:
	typedef Agpm::Path Path;
	typedef Agpd::PathPoint Point;
	typedef Agpm::PathOwner Owner;
	typedef Agpm::PathManager PathManager;
	typedef std::list< ClumpData > PathClumpList;
	typedef PathClumpList::iterator PathClumpListIter;

	//////////////////////////////////////////////////////////////////////////////
	//

	VOID					saveData				( bool question = true );
	void					loadData				( bool question = true );
	void					clearData				( bool question = true );

	//////////////////////////////////////////////////////////////////////////////
	//

	BOOL					_AddClump				( float x, float z, size_t pathIdx, size_t pointIdx );
	BOOL					_DelClump				( ClumpData clump );
	VOID					_DelClump				( size_t pathIndex, size_t pointIndex );
	VOID					_ClearClumpList			( VOID );

	//////////////////////////////////////////////////////////////////////////////
	//

	VOID					moveCamera				( VOID );

	void					drawPathLine			( Agpm::Path * path );

	Point::eTag				getSelectedFrom			();

	Point::eTag				getSelectedTo			();

	bool					checkPickClump			( RwV3d * pos );

	bool					selectPickClump			( ClumpData clump );

	ClumpData 				findClump				( size_t pathIndex, size_t pointIndex );

	void					refreshViewPaths		();

	void					createNewPath			();

	void					setPointTag				();

	//-----------------------------------------------------------------------
	//

	void					refreshInfo				();

	void					refreshPathInfo			();

	void					refreshPointInfo		();

	void					setSelectedPath			( Agpm::Path * path );

	void					setSelectedPoint		( Agpd::PathPoint * point );

	//-----------------------------------------------------------------------
	// 

	CButton					loadButton_;
	CButton					saveButton_;
	CButton					clearButton_;

	CEdit					fromLable_;
	CEdit					toLabel_;
	CComboBox				fromComboBox_;
	CComboBox				toComboBox_;
	CButton					newPathButton_;

	//-----------------------------------------------------------------------
	// 

	CEdit					pathLabel_;
	CEdit					pathIndexInfo_;
	CEdit					fromInfo_;
	CEdit					toInfo_;

	//-----------------------------------------------------------------------
	//

	CEdit					pointLabel_;

	CEdit					xInfo_;
	CEdit					yInfo_;
	CEdit					tagInfo_;
	CComboBox				tagComboBox_;
	CEdit					pointIndexInfo_;

	CButton					cameraMoveButton_;

	//-----------------------------------------------------------------------
	// 

	CListBox				pathList_;

	//-----------------------------------------------------------------------
	//

	PathManager				pathManager_;

	//-----------------------------------------------------------------------
	//

	typedef std::map< int, std::string > PointTypes;

	PointTypes				pointTypes_;

	AgpmEventSpawn*			m_pEventSpawn;
	CSpawnDataLoad			m_SpawnDataLoad;
	CStatic					m_staticMessage;
	AgpmMonsterPath			m_MonsterPath;

	PathClumpList			clumpList_;

	//-----------------------------------------------------------------------
	//

	Path *					selectedPath_;
	Point *					selectedPoint_;

	//-----------------------------------------------------------------------
	//

	CBrush					editBackGround_;
	PathManager::Paths		viewPaths_;

	//-----------------------------------------------------------------------
	//

	bool					modified_;

	//-----------------------------------------------------------------------
	//

	typedef std::set< CEdit* > InfoLables;
	InfoLables infoLables_;

	unsigned int low_16 ( void * p );

	void CreateControl( CButton & control, TCHAR * text, int x, int y, int width, int height, DWORD attr );

	template< typename Control >
	void CreateControl( Control & control, int x, int y, int width, int height, DWORD attr ) {
		CRect rect;
		rect.SetRect( x, y, x + width, y + height );
		control.Create( attr, rect, this, low_16(&control) );
	}

	void CreateControl( CEdit & control, TCHAR * text, int x, int y, int width, int height, DWORD attr );

	template< typename Control >
	CRect Pos( Control & control ) {
		CRect rect;
		control.GetWindowRect( &rect );
		ScreenToClient( &rect );
		return rect;
	}

	//-----------------------------------------------------------------------
};
