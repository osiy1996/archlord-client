// AlefMapDocument.h: interface for the CAlefMapDocument class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALEFMAPDOCUMENT_H__651448DD_2163_4A1D_BFA3_3458BE6DD263__INCLUDED_)
#define AFX_ALEFMAPDOCUMENT_H__651448DD_2163_4A1D_BFA3_3458BE6DD263__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CAlefMapDocument  
{
public:
	void	NewDocument	();
	BOOL	Save		();
	BOOL	SaveAs		();
	BOOL	Load		( CString filename );

	BOOL	SetModified	( BOOL bModify );
	BOOL	IsModified	();

public:
	// Operations..
	void	ToggleGrid	();
	
	// View Option..
	
	
public:
	// ������� �ʴ� ������.
	int		m_nSelectedDetail		;

	BOOL	m_bShowObjectBlocking	;	// ������Ʈ  ��ŷ ǥ�� �¿���.
	BOOL	m_bShowGeometryBlocking	;	// ���� ��ŷ  ǥ�� �¿���.

	BOOL	m_bShowObject			;	// ������Ʈ ǥ��.

	bool	m_bUseFirstPersonView	;	// 1��Ī ���� �� ���.
	BOOL	m_bFPSEditMode			;	// 1��Ī �������� ����Ʈ ���..
	
	int		m_nSelectedMode			;	// ���� �۾��ϰ� �ִ� ��� ������ ����.
	int		m_nShowGrid				;	// �׸��� ǥ��

	int		m_nCurrentTileLayer		;	// �۾����� Ÿ�� ���̾�.

	float	m_fBrushRadius			;	// �귯���� ������.
	int		m_nBrushType			;	// ���� ������� �귯�� Ÿ��.
	
	UINT32	m_uRunSpeed				;

	FLOAT	m_fCameraMovingSpeed	;	
	
	RwMatrix	m_MatrixCamera		;	// ����ī�޶� ��ġ ������..
	void		SaveCameraPosition		();
	void		RestoreCameraPosition	();

	// �������� ���� ���� ����..
	int		m_nSubDivisionDetail	;
	BOOL	m_bSubDivisionTiled		;
	BOOL	m_bSubDivisionUpperTiled;

	// Ÿ�� ����
	AuList< int >	m_listSelectedTileIndex	;
	int				GetSelectedTileIndex()	;
	int				m_nCurrentAlphaTexture	;	// ���� �������� ���� �ؽ���.


	bool	m_bToggleShadingMode;
	BOOL	m_bShowSystemObject	;
	UINT32	m_uAtomicFilter		;	// ������Ʈ ���� �ɼ�..
	UINT32	m_uFilterEventID	;
	enum	ATOMICFILTER
	{
		AF_GEOMTERY			= 0x0001	,	// ����ǥ��..
		AF_OBJECTALL		= 0x0002	,
		AF_SYSTEMOBJECT		= 0x0004	,	// �ý��� ������Ʈ ǥ��..
		AF_OBJECT_RIDABLE	= 0x0008	,
		AF_OBJECT_BLOCKING	= 0x0010	,
		AF_OBJECT_EVENT		= 0x0020	,
		AF_OBJECT_GRASS		= 0x0040	,
		AF_OBJECT_OTHERS	= 0x0080	,
		AF_OBJECT_DOME		= 0x0100	,
		AF_SHOWALL			= 0x01ff	,
		AF_OBJECTMASK		= 0x01fc	,
	};

	int		m_nDWSectorSetting;

	enum	RangeSphereType
	{
		RST_NONE		,
		RST_SPAWN		,
		RST_NATURE		,
		RST_MAX
	};

	INT32	m_nRangeSphereType;

public:
	
	bool	IsInFirstPersonViewMode		() { return m_bUseFirstPersonView; }
	void	ToggleFirstPersonViewMode	( INT32	nTemplateID	= 1 ) ;

protected:
	BOOL	m_bModified		; // ������̵� üũ.

	// ����Ŵ� ������

public:
	CAlefMapDocument();
	virtual ~CAlefMapDocument();

};

#endif // !defined(AFX_ALEFMAPDOCUMENT_H__651448DD_2163_4A1D_BFA3_3458BE6DD263__INCLUDED_)
