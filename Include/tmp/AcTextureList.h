// AlefTextureList.h: interface for the AcTextureList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALEFTEXTURELIST_H__3B84C638_E220_4F07_8A25_5C6BFBC4657E__INCLUDED_)
#define AFX_ALEFTEXTURELIST_H__3B84C638_E220_4F07_8A25_5C6BFBC4657E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "rwcore.h"
#include "AuList.h"

#define	ALEF_TILE_LIST_FILE_NAME	"map/tile/tilelist.txt"
#define	ALEF_TILE_EXPORT_FILE_NAME	"a%06xt%s"
// EXT�� ���� �༮����..
#define	ALEF_TILE_TEXTURENAME_NAME	"a%06xt"

// �ؽ��� ������ ��Ÿ���� �ν���.
#define	ALEF_TEXTURE_NO_TEXTURE	( 0 )

class AcTextureList  
{
public:
	BOOL				m_bEdited					;
	void				SetEdited	()				{ m_bEdited = TRUE	; }
	void				ClearEdited	()				{ m_bEdited = FALSE	; }
	BOOL				IsEdited	()				{ return m_bEdited	; }

	char *				m_strTextureDirectory		;

	char				m_strTextureDictionary[256]	;
	RwTexDictionary *	m_pTexDict					;	// NULL�̸� ���� ��������

	struct	TextureInfo
	{
		RwImage		*	pImage				;	// ���� ���� �༮,���� �޸� ���� ����..
		int				nIndex				;	// �ؽ��� �ε���.
		int				nProperty			;	// Ÿ�� �Ӽ� ����..
		char			strComment	[ 256 ]	;	// �ڸ�Ʈ.
		char			strFilename	[ 256 ]	;	// �����̸� �����ص�.

		TextureInfo() : pImage( NULL ) , nIndex( -1 ) , nProperty( APMMAP_MATERIAL_SOIL ) {}
	};

	AuList< TextureInfo >		TextureList;

	BOOL		IsEmpty				() { if( TextureList.GetCount() == 0 ) return TRUE;else return FALSE; }

	int			LoadScript			( char *filename	);	// ��ũ��Ʈ�� �ε��Ѵ�.
	int			SaveScript			( char *filename	);
	BOOL		ExportTile			( char *destination , void ( *callback ) ( void * pData ) = NULL );
			// ���� �� �������� �ؽ��ĸ� �ε��ϰ� 
			// Material�� �����Ͽ� ������ �ִ´�.
	
	BOOL		GetExportFileName	( char *filename , int nIndex	, char * ext = NULL	);

	RwTexture *		GetTexture		( int textureindex	);	// �ý��������͸� ���� �Լ�.
	TextureInfo *	GetTextureInfo	( int textureindex	);	// �ؽ��� �˻�.... �˻� �¸�.

	void		DeleteAll			( void				);	// ��ϵǾ��ִ� �ؽ��� ����Ʈ ��θ� ������.

	// Ref Count�� �Ű��� ����.
	RwTexture *	LoadTexture			( TextureInfo * textureinfo		, BOOL bForceUpdate = FALSE );
	RwRaster *	LoadRaster			( int			textureindex	);
	RwImage *	LoadImage			( int			textureindex	);


	// ���� ����..
	TextureInfo * AddTexture		( int dimension , int category , int index , int type , char *filename , char *comment = NULL );
	BOOL		DeleteTexture		( int textureindex	, BOOL bDeleteGraphicFile = FALSE );	// �ؽ����� ������ ����Ʈ���� �����Ѵ�.
	AcTextureList();
	virtual ~AcTextureList();

	BOOL		MakeTempBitmap		( TextureInfo * pTi				);	// �ӽ� ��Ʈ�� ������. Ŭ���̾�Ʈ ����Ÿ�� ����.

	int			CleanUpUnusedTile	();
protected:
	BOOL		InitialTempFiles	( void							);	// Ÿ�� ���� �����쿡�� ����� ��Ʈ�� ���� ����.
	void		Delete				( AuNode< TextureInfo > * pNode , BOOL bDeleteGraphicFile = FALSE );

};

#endif // !defined(AFX_ALEFTEXTURELIST_H__3B84C638_E220_4F07_8A25_5C6BFBC4657E__INCLUDED_)
