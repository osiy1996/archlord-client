#pragma once

#include "afxwin.h"
#include "AgpmMonsterPath.h"
#include "AgpdCharacter.h"


#define		MONSTER_PATH_DIRECTORY		"BossPath"
#define		PATH_TEXTURENUM_COUNT		100
#define		TEXTURE_NUMBER_SIZE			50
#define		TEXT_NUMBER_WIDTH			22

// CBossSpawnDlg 대화 상자입니다.

class CBossSpawnDlg : public CDialog
{
	DECLARE_DYNAMIC(CBossSpawnDlg)
private:
	typedef std::list< RpClump* >			PathClumpList;
	typedef std::list< RpClump* >::iterator	PathClumpListIter;

public:
	CBossSpawnDlg										( CWnd* pParent = NULL );   // 표준 생성자입니다.
	virtual ~CBossSpawnDlg								( VOID );

	enum { IDD = IDD_BOSSSPAWNDLG };

	virtual BOOL			OnInitDialog				( VOID );


	STMonsterPathInfoPtr	CreateBossSpawnPath			( CString&				strBossName , ePathType eType = E_PATH_LOOP );

	BOOL					DestroyBossSpawnPath		( CString&				strBossName );
	BOOL					DestroyBossSpawnPath		( STMonsterPathInfoPtr	pBossPath	);

	BOOL					AddBossSpawnPath			( CString&				strBossName , AuPOS	vPos );
	BOOL					AddBossSpawnPath			( STMonsterPathInfoPtr	pBossPath	, AuPOS vPos );

	BOOL					SetBossSpawnPath			( CString&				strBossName	, INT nIndex , RwV3d vPos );
	BOOL					SetBossSpawnPath			( STMonsterPathInfoPtr	pBossPath	, INT nIndex , RwV3d vPos );
	BOOL					SetBossSpawnType			( CString&				strBossName , ePathType	eType );
	BOOL					SetBossSpawnType			( STMonsterPathInfoPtr	pBossPath	, ePathType eType );

	INT						DelBossSpawnPath			( CString&				strBossName	, AuPOS	vPos );
	INT						DelBossSpawnPath			( STMonsterPathInfoPtr	pBossPath	, AuPOS vPos );

	AuPOS					GetBossSpawnIndexPos		( CString&				strBossName , INT nIndex );
	AuPOS					GetBossSpawnIndexPos		( STMonsterPathInfoPtr	pBossPath	, INT nIndex );

	INT						GetBossSpawnPathCount		( CString&				strBossName );
	INT						GetBossSpawnPathCount		( STMonsterPathInfoPtr	pBossPath	);

	STMonsterPathInfoPtr	GetMonsterSpawnData			( CString&				strBossName );

	VOID					AddSpawnName				( const CString&		strSpawnName );
	VOID					GetText						( INT nSel , CString&	strText );
	INT						GetCurSel					( VOID );

	VOID					SetSelectBoss				( const char*	strSpawnName , const char* strCharacterName , AuPOS vPos  );
	VOID					SetSelectCharacter			( AgpdCharacter*		pcsCharacter  )	{	m_pSelectCharacter	=	pcsCharacter;	}
	VOID					UpdatePathListBox			( VOID );

	VOID					RenderUpdateBossPath		( CString&				strBossName );
	VOID					RenderUpdateBossPath		( STMonsterPathInfoPtr	pBossPath	);

	static RpAtomic*		CBClumpSetting				( RpAtomic*	pAtomic , PVOID data );
	static RpMaterial*		CBMaterialSetTexture		( RpMaterial *material, PVOID data	);
	

	BOOL					CheckPathSet				( VOID )	{	return m_checkPathSet.GetCheck();	}
	BOOL					OnInitNumber				( VOID );

protected:
	DECLARE_MESSAGE_MAP()
	virtual void			DoDataExchange				( CDataExchange* pDX );    
	
	BOOL					_AddClump					( INT nX , INT nZ );
	BOOL					_DelClump					( RpClump*	pClump );

	VOID					_ClearClumpList				( VOID );
	BOOL					_ClumpNumberSetting			( RpClump*		pClump	,	INT nNumber );

	BOOL					_InitTexture				( RwImage* pImage , INT nNumber );
	VOID					_NumberDraw					( RwImage* pImageTarget , INT nFirst , INT nLast );

	CButton					m_checkPathSet;

	CListBox				m_listboxSpawn;
	CListBox				m_listboxPath;

	CStatic					m_staticCharacter;
	CStatic					m_staticSpawn;
	CStatic					m_staticPosition;
	CStatic					m_staticPathCount;
	AgpmMonsterPath			m_MonsterPath;
	AgpdCharacter*			m_pSelectCharacter;

	RwTexture*				m_pArrayNumTexture[ PATH_TEXTURENUM_COUNT ];
	RwImage*				m_pNumberImage;

	PathClumpList			m_ClumpList;
	PathClumpList			m_LineClumpList;
	
public:
	CButton m_radioPathType;

	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonReload();
	afx_msg void OnBnClickedButtonAllsave();
	afx_msg void OnBnClickedRadioPathloop();
	afx_msg void OnBnClickedRadioPathreverse();
public:
	afx_msg void OnLbnSelchangeListSpawnlist();
};
