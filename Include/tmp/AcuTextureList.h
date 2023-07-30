#ifndef	_ACUTEXTURELIST_H_
#define _ACUTEXTURELIST_H_

#include "AuList.h"
#include "rwcore.h"
#include "magdebug.h"

struct	stTextureListInfo
{
	RwTexture*										pTexture;
	INT32											lTextureListID;
	CHAR											szName[ 64 ];

	BOOL operator==( const stTextureListInfo & TextureListInfo )
	{
		return memcmp( this, &TextureListInfo, sizeof( stTextureListInfo ) ) ? FALSE : TRUE;
	}
};

class AcuTextureList
{
public:
	AcuTextureList( void );
	virtual ~AcuTextureList( void );

private:
	AuList< stTextureListInfo >						m_listTexture;

public:
	INT32											m_lAddImageIndex;	// Add Image �� �߰� �ɶ����� �����Ѵ�

	INT32					AddImage				( char * filename,	BOOL bRead = TRUE);	// �ش� �ε����� �̹��� �����͸� ������..
	RwTexture*				GetImage_ID				( INT32 lID );
	CHAR*					GetImageName_ID			( INT32 lID );
	RwTexture*  			GetImage_Index			( INT32 lIndex, INT32 *plID = NULL);
	CHAR*					GetImageName_Index		( INT32 lIndex, INT32 *plID = NULL);
	BOOL					SetImage_Index			( INT32 lIndex, char *filename, BOOL bRead = TRUE );
	BOOL					SetImage_ID				( INT32 lID, char *filename, BOOL bRead = TRUE );
	BOOL					SetImageName_Index		( INT32 lIndex, CHAR *szName );
	BOOL					SetImageID_Name			( INT32 lID, char *filename, BOOL bRead = TRUE );
	INT32					GetCount				( void );
	stTextureListInfo*		GetHeadTextureListInfo	( void );
	BOOL					DeleteImage_ID			( INT32	lID, BOOL bDestroyTexture = TRUE );
	BOOL					DeleteImage_Index		( INT32 lIndex );
	
	INT32					AddOnlyThisImage		( char* filename, BOOL bRead = TRUE );	//List�� ��� �̹����� ����� �Ѿ�� �̹����� ��´�. 
	void					DestroyTextureList		( void );

	void					LoadTextures			( void );
	RwTexture*				LoadTexture				( char* pFileName );
	void					UnloadTextures			( void );

	INT32					AddTexture				( RwTexture * pstTexture );

	AcuTextureList & operator=( const AcuTextureList & csTextureList ); 

private :
	BOOL					_SafeReleaseTexture		( RwTexture* pTexture );
	void					_ParseTextureFileName	( char* pFullFileName, char* pFileName, char* pExtension );
};

#endif		// _ACUTEXTURELIST_H_			