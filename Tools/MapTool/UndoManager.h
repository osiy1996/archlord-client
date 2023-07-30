// UndoManager.h: interface for the CUndoManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNDOMANAGER_H__6D87007F_1D75_48FA_8C34_13F167902225__INCLUDED_)
#define AFX_UNDOMANAGER_H__6D87007F_1D75_48FA_8C34_13F167902225__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <AuList.h>

#define	MAX_UNDO_BLOCK_SIZE	16

class ApWorldSector;

class	ActionUnit
{
public:
	int				nType	;
	ApWorldSector	*	pSector	;	// ������ ����..
};

class	ActionUnit_Geometry : public ActionUnit
{
public:
	INT32		nSegmentX	;
	INT32		nSegmentZ	;
	float		fOriginal	;	// ���� ��
	float		fChanged	;	// �ٲ� ��.
};

class	ActionUnit_Tile : public ActionUnit
{
public:
	INT32		nSegmentX	;
	INT32		nSegmentZ	;

	UINT32		anOriginalIndex	[ TD_DEPTH ];
	UINT32		anChangedIndex	[ TD_DEPTH ];
};

class	ActionUnit_VertexColor : public ActionUnit
{
public:
	INT32		nSegmentX	;
	INT32		nSegmentZ	;
	ApRGBA		rgbOriginal	;	// ���� ��
	ApRGBA		rgbChanged	;	// �ٲ� ��.
};


class	ActionUnit_Object : public ActionUnit
{
public:
	UINT32	oid			;	// Object ID

	AuPOS	stScale		;	// Scale Vector
	AuPOS	stPosition	;	// Base Position
	FLOAT	fDegreeX	;	// Rotation Degree
	FLOAT	fDegreeY	;	// Rotation Degree

	// Changed;;
	AuPOS	stChangedScale		;	// Scale Vector
	AuPOS	stChangedPosition	;	// Base Position
	FLOAT	fChangedDegreeX		;	// Rotation Degree
	FLOAT	fChangedDegreeY		;	// Rotation Degree
};

// ������ (2004-06-15 ���� 3:28:34) : ������ �������.
class	ActionUnit_EdgeTurn : public ActionUnit
{
public:
	INT32		nSegmentX		;
	INT32		nSegmentZ		;
	BOOL		bTurnOriginal	;
	BOOL		bTurnChanged	;
};

class CUndoManager  
{
public:
	enum	TYPE
	{
		GEOMETRY	,
		TILE		,
		VERTEXCOLOR	,
		SUBDIVISION	,
		OBJECTMANAGE,
		EDGETURN	,
		TYPECOUNT
	};

	class	ActionBlock
	{
	public:
		int						nType		;	// �׼� Ÿ��.
		AuList< ActionUnit * >	listUnit	;

		~ActionBlock();
	};

	AuList< ActionBlock * >		listUndo				;
	ActionBlock *				m_pCurrentActionBlock	;
	AuNode< ActionBlock * >		* m_pCurrentNode		;

	// ��κ� ��Ʈ��
	void	StartActionBlock		( int actiontype );
	void	EndActionBlock			();

	// �׼� �߰�
	void	AddObjectActionUnit		( ObjectUndoInfo * original , ObjectUndoInfo * changed );

	void	AddGeometryActionUnit	( ApWorldSector * pSector , int nSegmentX , int nSegmentZ , float fChangedValue );
	void	AddTileActionUnit		( ApWorldSector * pSector , int nSegmentX , int nSegmentZ ,
									UINT32 nChangedIndex	,
									UINT32 nChangedIndex2	= ALEF_TEXTURE_NO_TEXTURE	,
									UINT32 nChangedIndex3	= ALEF_TEXTURE_NO_TEXTURE	,
									UINT32 nChangedIndex4	= ALEF_TEXTURE_NO_TEXTURE	,
									UINT32 nChangedIndex5	= ALEF_TEXTURE_NO_TEXTURE	,
									UINT32 nChangedIndex6	= ALEF_TEXTURE_NO_TEXTURE	);
	void	AddVertexColorActionunit( ApWorldSector * pSector , int nSegmentX , int nSegmentZ , ApRGBA rgbChanged );

	void	AddEdgeTurnActionunit( ApWorldSector * pSector , int nSegmentX , int nSegmentZ , BOOL bTurnChanged );
	// ��� ����
	BOOL	Undo();
	BOOL	Redo();

	// ��� ����Ÿ Ŭ���� ���. ( �޸� ũ���� ���� )
	void	ClearObjectUndoData		( int oid	);
	void	ClearAllUndoData		(			);

	CUndoManager();
	virtual ~CUndoManager();

protected:
	ActionBlock *	_StartActionBlock		( int actiontype );
	void			_EndActionBlock			( ActionBlock * pBlock );

	void			_AddGeometryActionUnit	( ActionBlock * pBlock , ApWorldSector * pSector , int nSegmentX , int nSegmentZ , float fChangedValue );
	void			_AddTileActionUnit		( ActionBlock * pBlock , ApWorldSector * pSector , int nSegmentX , int nSegmentZ ,
											UINT32 nChangedIndex	, UINT32 nChangedIndex2	,
											UINT32 nChangedIndex3	, UINT32 nChangedIndex4	,
											UINT32 nChangedIndex5	, UINT32 nChangedIndex6	);
	void			_AddVertexColorActionunit( ActionBlock * pBlock , ApWorldSector * pSector , int nSegmentX , int nSegmentZ , ApRGBA rgbChanged );

	void			_AddObjectActionUnit	(  ActionBlock * pBlock , ObjectUndoInfo * original , ObjectUndoInfo * changed );
	void			_AddEdgeTurnActionUnit	(  ActionBlock * pBlock , ApWorldSector * pSector , int nSegmentX , int nSegmentZ , BOOL bTurnChanged );

};

#endif // !defined(AFX_UNDOMANAGER_H__6D87007F_1D75_48FA_8C34_13F167902225__INCLUDED_)
