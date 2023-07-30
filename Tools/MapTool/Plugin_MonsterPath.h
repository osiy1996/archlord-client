#pragma once

#include "MyEngine.h"
#include "UITileList_PluginBase.h"
#include "SpawnDataLoad.h"
#include "AgpmMonsterPath.h"

#include <list>


#define		ID_COMBO_SPAWNNAME			23230
#define		ID_COMBO_PATHTYPE			23231

#define		ID_BUTTON_DELETE_PATH		30003
#define		ID_BUTTON_ONE_SAVE			30004
#define		ID_BUTTON_ALL_SAVE			30005

#define		ID_BUTTON_CAMERA_MOVE		30006


typedef std::list< RpClump* >			PathClumpList;
typedef std::list< RpClump* >::iterator	PathClumpListIter;
 
class CPlugin_MonsterPath :
	public CUITileList_PluginBase
{
public:
	
	CPlugin_MonsterPath(void);
	virtual ~CPlugin_MonsterPath(void);

	virtual BOOL			OnSelectedPlugin		( VOID );
	virtual BOOL			OnDeSelectedPlugin		( VOID );

	virtual BOOL			OnWindowRender			( VOID );

	virtual BOOL			OnLButtonDownGeometry	( RwV3d * pPos );
	virtual BOOL			OnLButtonUpGeometry		( RwV3d * pPos );
	virtual BOOL			OnMouseMoveGeometry		( RwV3d * pPos );

	DECLARE_MESSAGE_MAP()

	afx_msg VOID			OnPaint					( VOID );
	afx_msg INT				OnCreate				( LPCREATESTRUCT lpCreateStruct );
	afx_msg LRESULT			WindowProc				( UINT message, WPARAM wParam, LPARAM lParam );

protected:
	VOID					_ChangePathUpdate		( VOID );
	VOID					_ChangePathType			( VOID );
	VOID					_UpdateClumpList		( STMonsterPathInfoPtr	pMonsterPath );
	VOID					_ClearClumpList			( VOID );
	VOID					_PathDelete				( VOID );
	VOID					_SaveData				( VOID );
	VOID					_AllSaveData			( VOID );
	VOID					_MoveCamera				( VOID );

	BOOL					_AddMonsterPath			( STMonsterPathInfoPtr	pMonsterPath , INT nX , INT nZ );
	BOOL					_AddMonsterPath			( const CString& strSpawnName , INT nX , INT nZ );

	BOOL					_DeleteMonsterPath		( STMonsterPathInfoPtr	pMonsterPath , INT Index , RpClump* pClump );

	BOOL					_AddClump				( INT nX , INT nZ );
	BOOL					_DelClump				( RpClump*	pClump );

	BOOL					m_bPathData;			
	
	CButton					m_btnDeletePath;
	CButton					m_btnDestroyPath;

	CButton					m_btnSave;
	CButton					m_btnAllSave;

	CButton					m_btnCameraMove;

	CComboBox				m_comboPathType;
	CComboBox				m_comboSpawnName;

	AgpmEventSpawn*			m_pEventSpawn;
	CSpawnDataLoad			m_SpawnDataLoad;
	CStatic					m_staticMessage;
	AgpmMonsterPath			m_MonsterPath;

	PathClumpList			m_ClumpList;
};
