/***********************************************************************************************************
*   Class : AuIniManager
*   Comment  : 
*
*	���� :	
*	[section]
*	key=value
*	.
*	.
*	.
*
*	Ini ���� ������ :
*	1. Section Name�� '[' ']'���̿� ������� ������ �� Section�� �̸��� "" �� �ȴ�. 
*	3. Key Name �� Value Name ���̴� �ݵ�� '='�� �־�� �ϸ� ���鵵 ���ڷ� ó���ȴ�. 
*
*	���� : 
*	1. SetValue�� �������� �ʴ� Section�� �Ѱ��ָ� SetValue�� �۵����� �ʴ´�. 
*	Key�� �����ϴ°� �˻��Ͽ� �����ϸ� value���� ����� �������� ������ Key�� �����Ѵ�. 
*
*
*
*   Date&Time : 2002-12-05, ���� 7:24
*   Code By : Seong Yon-jun@NHN Studio 
***********************************************************************************************************/
#ifndef		_AUINIMANAGER_H_
#define		_AUINIMANAGER_H_

#include <vector>
#include <string>

#ifndef _AREA_CHINA_
#include <AuStaticPool/static_pool.h>
#endif // _AREA_CHINA_

using namespace std;

/*********DEFINE*********/
#ifdef _AREA_GLOBAL_
	#define	AUINIMANAGER_MAX_SECTIONNUM				20000
	#define	AUINIMANAGET_MAX_KEYNUM					20000
#else
	#define	AUINIMANAGER_MAX_SECTIONNUM				1024*16
	#define	AUINIMANAGET_MAX_KEYNUM					1024*16
#endif

#define AUINIMANAGER_MAX_NAME					1024
#define AUINIMANAGER_MAX_KEYVALUE				1024
#define AUINIMANAGER_MAX_VALUEDATA				64

typedef enum
{
	AUINIMANAGER_MODE_NORMAL = 0,
	AUINIMANAGER_MODE_NAME_OVERWRITE,
	AUINIMANAGER_MAX_MODE
} AuIniManagerMode;

typedef enum
{
	AUINIMANAGER_TYPE_NORMAL		= 0x00,
	AUINIMANAGER_TYPE_PART_INDEX	= 0x01,
	AUINIMANAGER_TYPE_KEYINDEX		= 0x02,
	AUINIMANAGER_MAX_TYPE
} AuIniManagerType;

enum KEYTYPE
{
	KT_NONE		,
	KT_I		,	// integer
	KT_I64		,	// integer 64 bit
	KT_F		,	// float
	KT_STR			// string
};

/*********STRUCT*********/
struct stIniKey
{
	//char KeyName [AUINIMANAGER_MAX_NAME		];				// KeyName = KeyValue
	int		nKeyIndex;
	char	KeyValue[AUINIMANAGER_MAX_KEYVALUE	];			

	KEYTYPE	eType;

	stIniKey():eType( KT_NONE ),nKeyIndex(0) {}
};

struct	stIniSection
{
	char	  SectionName[AUINIMANAGER_MAX_NAME];		// '[' Section Name ']'
	int		  lKeyNum;
	stIniKey* stKey;									//Pointer of Key Struct
};

// AuIniManger Binary ������.

struct	stIniKeyBinary
{
	int		nKeyIndex	;
	KEYTYPE	eType		;

	union DATA
	{
		char	str[ AUINIMANAGER_MAX_KEYVALUE ];
		INT32	i								;
		INT64	i64								;
		FLOAT	f								;
	};

	// union ���� ����.. �˾Ƽ� �߽����..
	// ������� �ұ� �ϴٰ� ���ٽ������� ���Ͽ����� �޺�..
	DATA	data;

	stIniKeyBinary():eType( KT_NONE ),nKeyIndex(0) {}
};

struct	stIniSectionBinary
{
	string						strSection	;
	vector< stIniKeyBinary >	vecKey		;
};

/*********CLASS*********/
class AuIniManagerA
{
public:
	enum	PROCESSMODE
	{
		NONE	= 0x00,
		TXT		= 0x01,
		BIN		= 0x02,
	};

	DWORD				GetProcessMode() { return m_eProcessMode; }
	void				SetProcessMode( DWORD eMode ) { m_eProcessMode = eMode; }
	bool				IsProcessMode( PROCESSMODE eMode ) { return ( GetProcessMode() & eMode ) ? true : false; }

	const DWORD 		GetType() { return m_eType; }

private:	
	DWORD				m_eProcessMode			; // PROCESSMODE�� �÷��� �������� ������.
	//char*				m_pszPathName;					//Ini Path 	
	string				m_strPathName;
		
	int 				lSectionNum;
	//@{ 2006/11/22 burumal
	int					lHalfSectionNum;
	bool				bIsSectionNumOdd;
	//@}

	stIniSection*				m_stSection	;
	vector<stIniSectionBinary>	m_vecSection;

	AuIniManagerMode	m_eMode;
	DWORD				m_eType;

	int					m_lPartNum;
	int*				m_plPartIndices;

	// INI ���̳ʸ�ȭ �۾�.
	vector< string >	m_vecKeyTable;
	const char *		GetKeyNameTable	( int nIndex );
	int					GetKeyIndex	( const char * pStr );

private :
	void	Initialize				();
	
	//@{ 2006/09/01 burumal
	//bool	StringMid				( char*	pDest, char* pSource, int lStart, int lEnd, int lDestLength);
	bool	StringMid				( char*	pDest, char* pSource, int nSrcLen, int lStart, int lEnd, int lDestLength);
	//@}

	int		AddKey					( int			SectionID	, const char*	pszKeyName	, KEYTYPE eType , stIniKeyBinary::DATA data );
	int		AddKey					( const char*	pszSection	, const char*	pszKeyName	, KEYTYPE eType , stIniKeyBinary::DATA data );

	bool	KeySetting				( int	SectionID	, int KeyID , KEYTYPE eType , stIniKeyBinary::DATA data );
public :
	AuIniManagerA();
	~AuIniManagerA();

	// �б� ����.
	void	SetPath					( const char* pszPath	)	;

	BOOL	ReadFile				( int lIndex = 0		, BOOL bDecryption = FALSE)	;
	BOOL	WriteFile				( int lIndex = 0		, BOOL bEncryption = FALSE)	;
	
	//@{ 2006/09/05 burumal
	//BOOL	FromMemory				( int lIndex, BOOL bDecryption);
	BOOL	FromMemory				( BOOL bDecryption);
	BOOL	ParseMemory				( char* pBuffer, unsigned long nBufferSize, BOOL bDecryption );
	//@}
	BOOL	FromFile				( int lIndex, BOOL bDecryption);
	
	void	ClearAllSectionKey		();

	// Get �Լ���.
	int		GetNumSection			(						)	{ return lSectionNum; }
	char *	GetSectionName			( int lSectionID		);
	int		GetNumKeys				( int lSectionID		);				
	const char *	GetKeyName		( int lSectionID		, int lkeyID	);

	// Find ������
	// Return ���� ID �� �Űڽ�.
	int		FindSection				( const char* pszSection						);	
	int		FindKey					( const char* pszSection, const char* pszKey	);
	int		FindKey					( int SectionID			, const char* pszKey	);

	int		AddSection				( const char* pszSectionName					);

	// Get Functions
	char *	GetValue				( int			lSectionID		, int	lKeyID		, char*		defValue	= NULL	);
	char *	GetValue				( const char*	pszSectionName	, char*	pszKeyName	, char*		defValue	= NULL	);
	int		GetValueI				( const char*	pszSectionName	, char*	pszKeyName	, int		defValue	= 0		);
	__int64	GetValueI64				( const char*	pszSectionName	, char*	pszKeyName	, __int64	defValue	= 0		);
	bool	GetValueB				( const char*	pszSectionName	, char*	pszKeyName	, bool		defValue	= 0		) { return ( GetValueI( pszSectionName, pszKeyName, (int)defValue ) != 0 ); }
	double	GetValueF				( const char*	pszSectionName	, char*	pszKeyName	, double	defValue	= 0.0f	);
	bool	GetStringValue			( const char*	pszSectionName	, char*	pszKeyName	, char*		pszDest				,
									  int			lszDestLength	, char*	defValue	= NULL							);
	bool	GetStringValue			( int			lSectionID		, int	lKeyID		, char*		pszDest				,
									  int			lszDestLength	, char*	defValue	= NULL							);
	
	// Set Functions
private:
	bool	SetValue				( int	SectionID			  , int	KeyID			  , KEYTYPE eType , stIniKeyBinary::DATA data);
	bool	SetValue				( const char*	pszSectionName, const char*	pszKeyName, KEYTYPE eType , stIniKeyBinary::DATA data);

public:
	bool	SetValue				( const char*	pszSectionName, const char*	pszKeyName, const char* pszvalue);
	bool	SetValueI				( const char*	pszSectionName, const char*	pszKeyName, int		value		);
	bool	SetValueI64				( const char*	pszSectionName, const char*	pszKeyName, __int64	value		);
	bool	SetValueB				( const char*	pszSectionName, const char*	pszKeyName, bool	value		) { return SetValueI( pszSectionName, pszKeyName, int(value) ); }
	bool	SetValueF				( const char*	pszSectionName, const char*	pszKeyName, double	value		);	
	
	bool	SetMode					( AuIniManagerMode eMode			);
	bool	SetType					( DWORD				 eType			);

	//@{ 2006/09/05 burumal
	//bool	AddPartIndices			( int	lStartIndex		, int	lEndIndex	, int	lOffset		);
	//@}

	int		ReadPartIndices			( char* pBuffer			);
	bool	ReadPartIndices			( FILE *pFile			);
	bool	WritePartIndices		( FILE *pFile			);

	int		GetStartIndex			();

	bool	EncryptSave				();
	#ifndef _AREA_CHINA_
	umtl::static_pool::wstring GetPackName();
	#endif
};

// Ű�� Ȯ�ο� ���.
inline bool	IsKeyAvailable( int nKey ) { return nKey != -1 ? true : false; }

int print_compact_format( char * pStr , const char * pFmt , ... );

#endif //_AUINIMANAGER_H_