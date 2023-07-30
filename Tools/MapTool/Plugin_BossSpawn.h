#pragma once

#include "MyEngine.h"
#include "UITileList_PluginBase.h"
#include "BossSpawnDlg.h"
#include <vector>

// CPlugin_BossSpawn

#define ID_COMBO_SPAWNDATA		21532
#define ID_COMBO_SPAWNPOINT		22532
#define ID_BUTTON_PATHCREATE	27532
#define ID_BUTTON_PATHDEL		27555

class CPlugin_BossSpawn : public CUITileList_PluginBase , ApModule
{
	DECLARE_DYNAMIC(CPlugin_BossSpawn)

public:
	CPlugin_BossSpawn();
	virtual ~CPlugin_BossSpawn();

	static CPlugin_BossSpawn * m_spThis;
	static ApModule * GetClass()
	{
		if( NULL == CPlugin_BossSpawn::m_spThis )
			CPlugin_BossSpawn::m_spThis = new CPlugin_BossSpawn;
		return static_cast<ApModule*>( CPlugin_BossSpawn::m_spThis );
	}

public:
	int	m_nCharacterIDCounter;
	// �ɸ��� ���̵��.. �ϳ��� ������Ű�鼭 �����.
	AgpdCharacter	*	m_pCurrentCharacter	;
	RpClump			*	m_pSelectedClump	;

	RsMouseStatus	m_PrevMouseStatus	;
	BOOL			m_bLButtonDown		;

	// �˾� �޴� ������.
	AgpdCharacter	*	m_pPopupCurrentCharacter	;
	RpClump			*	m_pPopupSelectedClump		;

	INT32			m_nLastReadSpawnCharacterID	;

	struct Spawn
	{
		INT32	nOID;	// �ö� ������Ʈ ID
		INT32	nCID;	// �ö� �ɸ��� ID

		INT32	nTID;	// �ɸ��� ���ø� ID

		Spawn() : nOID( 0 ) , nCID( 0 ) , nTID( 0 ) {}

		void	Delete(){ nOID = 0; nCID = 0; nTID = 0; }

		ApdObject		* GetObject		();
		AgpdCharacter	* GetCharacter	();
	};

	vector< Spawn >	m_vecSpawn;
	Spawn * GetSpawn( INT32 nCID )
	{
		for( int i = 0 ; i < static_cast<int> ( m_vecSpawn.size() ) ; i ++ )
		{
			if( m_vecSpawn[ i ].nCID == nCID ) return &m_vecSpawn[ i ];
		}
		return NULL;
	}

	Spawn * GetSpawnByObject( INT32 nOID )
	{
		for( int i = 0 ; i < static_cast<int> ( m_vecSpawn.size() ) ; i ++ )
		{
			if( m_vecSpawn[ i ].nOID == nOID ) return &m_vecSpawn[ i ];
		}
		return NULL;
	}

	// ���� ���� ������Ʈ�� �����Ҷ� ����Ʈ ������ ������Ʈ �߰��� �̺�Ʈ ���.
	BOOL	AddSpawnObject( AgpdCharacter * pCharacter );
	// ����������Ʈ�� �ε���� , �� ������ �ɸ��͸� �߰��ϱ� ���Ѱ�.
	BOOL	AddCharacter( ApdObject * pObject , INT32 nTemplateID );

	// ��ġ,������ ���� ������ ������ ���������� ��ũ�� ����.
	BOOL	UpdateCharacter( AgpdCharacter * pCharacter );

	// �ɸ��͸� �����ϸ鼭 , ���� ��ϵ� ���� ������Ʈ�� ������.
	// m_vecSpawn���� �� �ε����� 0���� �����صּ� ���߿� UpdateList ���� �����Ű���.
	BOOL	DeleteCharacter( AgpdCharacter * pCharacter );
	
	// m_vecSpawn���� ����� ������Ʈ���� �����Ѵ�.
	void	UpdateList();

	// �ش� ĳ������ �н��� �����Ѵ�
	// ���־����� ��ϵ� ���⼭ �Ѵ�
	VOID	PushCharacterPath( VOID );			
	VOID	DelCharacterPath( VOID );

	// ���̺� �ε�
	static BOOL CBCheckSpawn(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBReadSpawn(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CBSave ( DivisionInfo * pDivisionInfo , PVOID pData );
	static BOOL	CBLoad ( DivisionInfo * pDivisionInfo , PVOID pData );

	// NPC���� ��Ʈ����..
	static BOOL	BossInfoStreamWriteCB	(PVOID pData, ApModule *pClass, ApModuleStream *pcsStream);
	static BOOL	BossInfoStreamReadCB	(PVOID pData, ApModule *pClass, ApModuleStream *pcsStream);
public:

	virtual	BOOL OnSelectedPlugin		();
	virtual	BOOL OnDeSelectedPlugin		();
	virtual	BOOL OnLButtonDownGeometry	( RwV3d * pPos );
	virtual	BOOL OnLButtonUpGeometry	( RwV3d * pPos );
	virtual BOOL OnMouseMoveGeometry	( RwV3d * pPos );
	virtual	BOOL OnSelectObject			( RpClump * pClump );
	virtual BOOL OnQuerySaveData		( char * pStr );
	virtual BOOL OnSaveData				();
	virtual BOOL OnLoadData				();
	virtual	BOOL OnCleanUpData			();
	virtual BOOL Window_OnKeyDown		( RsKeyStatus *ks	);

	virtual	BOOL Window_OnLButtonDown	( RsMouseStatus *ms );
	virtual	BOOL Window_OnLButtonUp		( RsMouseStatus *ms );
	virtual BOOL Window_OnMouseMove		( RsMouseStatus *ms );
	virtual	BOOL Window_OnLButtonDblClk	( RsMouseStatus *ms );
	virtual	BOOL Window_OnRButtonDown	( RsMouseStatus *ms );

	virtual BOOL OnWindowRender			();

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

protected:
	CButton				m_btnPath;
	CButton				m_btnPathDel;
	CComboBox			m_comboPathPoint;

	CString				m_strSelectName;
	CString				m_strSelectPosition;
	CString				m_strSelectPathPointCount;
	CBossSpawnDlg*		m_pBossSpawnDlg;
	
		
};


