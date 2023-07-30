#ifndef __AGCDITEM_H__
#define __AGCDITEM_H__

#include "rwcore.h"
#include "rpworld.h"
#include "rpcollis.h"
#include "rphanim.h"
#include "rpskin.h"
#include "rtcharse.h"

#include "acdefine.h"
#include "ApRwLock.h"

#include "AgpdItem.h"
#include "AgcmOctree.h"
#include "AgcdRenderType.h"

#include <map>

#define AGCDITEM
#define	AGCDITEM_DFF_NAME_LENGTH				64
#define	AGCDITEM_TEXTURE_NAME_LENGTH			64

#define	AGCDITEM_MAX_AUTO_LOOTING_RANGE			900				// �ڵ����� ������ ����鶧 �ִ� �Ÿ�
#define	AGCDITEM_AUTO_LOOTING_INTERVAL_TIME		300				// �ڵ����� ���� ����� ���͹� (MSec)

typedef struct	AgcdItem					AgcdItem;
typedef struct	AgcdItemTemplateEquipSet	AgcdItemTemplateEquipSet;
typedef enum	eAgcdItemMaterial			eAgcdItemMaterial;

enum eAgcdItemMaterial
{
	AGCD_ITEM_METERIAL_NUM = 0
};

enum AgcdItemStatus
{
	AGCDITEM_STATUS_INIT			= 0x00,		// �ʱ���� (Constructor�� �Ҹ�)
	AGCDITEM_STATUS_LOADED			= 0x01,		// Template�� Load�� ����
	AGCDITEM_STATUS_MAKE_ITEM_CLUMP	= 0x02,		// Equip �̳� Field ���¿��� Clump�� �������
	AGCDITEM_STATUS_ATTACH			= 0x08,		// Equip �� ��� Character�� Attach��
	AGCDITEM_STATUS_WORLD_ADDED		= 0x04,		// Equip �̳� Field ���¿��� World�� Add��
	AGCDITEM_STATUS_REMOVED			= 0x10,		// Item�� Remove��.
};

struct ItemTransformInfo
{
	ItemTransformInfo( int nCount )
	{
		if( nCount )
		{
			m_nCount		= nCount;
			m_astTransform	= new RwMatrix[ nCount ];
			m_alPartID		= new INT32[ nCount ];
			memset( m_astTransform, 0, sizeof (RwMatrix) * nCount );
			memset( m_alPartID, -1, sizeof(INT32) * nCount );
		}
	}
	ItemTransformInfo( ItemTransformInfo* pSrc )
	{
		if( pSrc )
		{
			m_nCount		= pSrc->m_nCount;
			m_astTransform	= new RwMatrix[ m_nCount ];
			m_alPartID		= new INT32[ m_nCount ];
			memcpy( m_astTransform, pSrc->m_astTransform, sizeof (RwMatrix) * m_nCount );
			memcpy( m_alPartID, pSrc->m_alPartID, sizeof(INT32) * m_nCount );
		}
	}
	~ItemTransformInfo()
	{
		DEF_SAFEDELETE( m_astTransform );
		DEF_SAFEDELETE( m_alPartID );
	}

	INT32		m_nCount;
	RwMatrix*	m_astTransform;
	INT32*		m_alPartID;
};
typedef std::map< INT32, ItemTransformInfo* >	ItemTransformInfoMap;
typedef ItemTransformInfoMap::iterator			ItemTransformInfoMapItr;

class AgcdItemTemplate;
class ApModuleStream;
class CItemTransformInfo
{
public:
	CItemTransformInfo()	{	Clear();	}
	~CItemTransformInfo()	{	Clear();	}

	void	Clear()			{	m_mapItemTransformInfo.clear();	}
	void	Assign( CItemTransformInfo* pDst );

	ItemTransformInfo*	Insert( INT32 lID, INT32 nCount );
	ItemTransformInfo*	GetInfo( INT32 lID );

	BOOL	Read( ApModuleStream *pcsStream, AgcdItemTemplate* pTempalte );
	BOOL	Write( ApModuleStream *pcsStream );
	
private:
	ItemTransformInfoMap	m_mapItemTransformInfo;
};

class AgcdPreItemTemplate
{
public:
	CHAR*	m_pszLabel;

	CHAR*	m_pszBaseDFFName;
	CHAR*	m_pszSecondDFFName;
	CHAR*	m_pszFieldDFFName;
	CHAR*	m_pszPickDFFName;

	CHAR*	m_pszTextureName;
	CHAR*	m_pszSmallTextureName;
	CHAR*	m_pszDurabilityZeroTextureName;
	CHAR*	m_pszDurability5UnderZeroTextureName;

	RwRGBA	m_stPreLight;

	AgcdPreItemTemplate()
	{
		m_pszLabel								= NULL;

		m_pszBaseDFFName						= NULL;
		m_pszSecondDFFName						= NULL;
		m_pszFieldDFFName						= NULL;
		m_pszPickDFFName						= NULL;

		m_pszTextureName						= NULL;
		m_pszSmallTextureName					= NULL;
		m_pszDurabilityZeroTextureName			= NULL;
		m_pszDurability5UnderZeroTextureName	= NULL;

		m_stPreLight.red						= 0;
		m_stPreLight.green						= 0;
		m_stPreLight.blue						= 0;
		m_stPreLight.alpha						= 0;
	}
};

#ifdef _BIN_EXEC_
//#include "mapped_pool.h"
class ItemClumpInfo
{
public:
	typedef std::map< int, ItemClumpInfo > Dic;
	static ItemClumpInfo & Get( int tid ) { return dic_[tid];	}

	#define _dec_set_member__(name) \
	public: \
		void	name( char const * arg )	{ if( arg ){ name##_ = arg; } } \
		char *	name()						{ return name##_.empty() ? 0 : (char*)##name##_.c_str(); } \
	private: \
		std::string name##_;
		

	_dec_set_member__( baseDff );
	_dec_set_member__( secondDff );
	_dec_set_member__( fieldDff );
	_dec_set_member__( pickDff );
	_dec_set_member__( textureName );
	_dec_set_member__( smallTextureName );
	_dec_set_member__( durZeroTextureName );
	_dec_set_member__( dur5UnderTextureName );

	#undef _dec_set_member__

private:
	static Dic dic_;
};
#endif

class AgcdItemTemplate
{
public:
	AgcdPreItemTemplate		*m_pcsPreData;

	RpClump					*m_pstBaseClump;
	RpClump					*m_pstSecondClump;
	RpClump					*m_pstFieldClump;
	RpAtomic				*m_pstPickingAtomic;
	RwMatrix				*m_pstFieldClumpTransform;
	UINT32					m_lAtomicCount;

	RwSphere				m_stBSphere;

	RwTexture				*m_pTexture;					// Texture Image For Inventory
	RwTexture				*m_pDurabilityZeroTexture;
	RwTexture				*m_pDurability5UnderTexture;
	RwTexture				*m_pSmallTexture;

	CItemTransformInfo		*m_pItemTransformInfo;
	AgcdLOD					m_stLOD;

	BOOL					m_bEquipTwoHands;				// Tiger claw ���� ����� ���� �� �տ� �����ؾ��� ���!
	BOOL					m_bWithHair;
	BOOL					m_bWithFace;

	INT32					m_lObjectType;

	INT32					m_lRefCount;
	BOOL					m_bLoaded;

	INT32					m_nDNF;

	AgcdClumpRenderType		m_csClumpRenderType;

	OcTreeRenderData2		m_stOcTreeData;		// item,character�� (occluder���� �����Ƿ� �޸� �Ƴ���)

	INT32					m_lAttachedToGrid;

#ifndef _BIN_EXEC_

protected:
	UINT64					m_ulExportDffID;				// 16��Ʈ�� ���(BaseDFF | SecondDFF | FieldDFF | PickDFF)
	UINT64					m_ulExportTextureID;			// 16��Ʈ�� ���(Texture | SmallTexture | DurabilityZeroTexture | Durability5UnderTexture)

public:
	inline VOID		SetBaseDFFID(UINT32 ulExportedID)	{	m_ulExportDffID	|= ((UINT64)(ulExportedID) << 48);	}
	inline VOID		SetSecondDFFID(UINT32 ulExportedID)	{	m_ulExportDffID	|= ((UINT64)(ulExportedID) << 32);	}
	inline VOID		SetFieldDFFID(UINT32 ulExportedID)	{	m_ulExportDffID	|= ((UINT64)(ulExportedID) << 16);	}
	inline VOID		SetPickDFFID(UINT32 ulExportedID)	{	m_ulExportDffID	|= ulExportedID;	}
	inline UINT32	GetBaseDFFID()						{	return (UINT32)(m_ulExportDffID >> 48);	}
	inline UINT32	GetSecondDFFID()					{	return (UINT32)((m_ulExportDffID & 0xffff00000000) >> 32);	}
	inline UINT32	GetFieldDFFID()						{	return (UINT32)((m_ulExportDffID & 0xffff0000) >> 16);	}
	inline UINT32	GetPickDFFID()						{	return (UINT32)(m_ulExportDffID & 0xffff);	}
	inline VOID		GetBaseDFFName(CHAR *pszName)		{	sprintf( pszName, "%s%.*x.%s", AC_EXPORT_HD_CHAR_BASE_ITEM, AC_EXPORT_ID_LENGTH, GetBaseDFFID(), AC_EXPORT_EXT_CLUMP );		}
	inline VOID		GetSecondDFFName(CHAR *pszName)		{	sprintf( pszName, "%s%.*x.%s", AC_EXPORT_HD_CHAR_SECOND_ITEM, AC_EXPORT_ID_LENGTH, GetSecondDFFID(), AC_EXPORT_EXT_CLUMP );	}
	inline VOID		GetFieldDFFName(CHAR *pszName)		{	sprintf( pszName, "%s%.*x.%s", AC_EXPORT_HD_CHAR_FILED_ITEM, AC_EXPORT_ID_LENGTH, GetFieldDFFID(), AC_EXPORT_EXT_CLUMP );	}
	inline VOID		GetPickDFFName(CHAR *pszName)		{	sprintf( pszName, "%s%.*x.%s", AC_EXPORT_HD_CHAR_ITEM_PICKING_DATA, AC_EXPORT_ID_LENGTH, GetPickDFFID(), AC_EXPORT_EXT_CLUMP );	}

	inline VOID		SetTextureID(UINT32 ulExportedID)			{	m_ulExportTextureID	|= ((UINT64)(ulExportedID) << 48);	}
	inline VOID		SetSmallTextureID(UINT32 ulExportedID)		{	m_ulExportTextureID	|= ((UINT64)(ulExportedID) << 32);	}
	inline VOID		SetDurZeroTextureID(UINT32 ulExportedID)	{	m_ulExportTextureID	|= ((UINT64)(ulExportedID) << 16);	}
	inline VOID		SetDur5UnderTextureID(UINT32 ulExportedID)	{	m_ulExportTextureID	|= ulExportedID;	}
	inline UINT32	GetTextureID()								{	return (UINT32)(m_ulExportTextureID >> 48);	}
	inline UINT32	GetSmallTextureID()							{	return (UINT32)((m_ulExportTextureID & 0xffff00000000) >> 32);	}
	inline UINT32	GetDurZeroTextureID()						{	return (UINT32)((m_ulExportTextureID & 0xffff0000) >> 16);	}
	inline UINT32	GetDur5UnderTextureID()						{	return (UINT32)(m_ulExportTextureID & 0xffff);	}
	inline VOID		GetTextureName(CHAR *pszName)				{	sprintf( pszName, "%s%.*x", AC_EXPORT_HD_TEX_ITEM_ITEM, AC_EXPORT_ID_LENGTH, GetTextureID() );				}
	inline VOID		GetSmallTextureName(CHAR *pszName)			{	sprintf( pszName, "%s%.*x", AC_EXPORT_HD_TEX_ITEM_SMALL, AC_EXPORT_ID_LENGTH, GetSmallTextureID() );		}
	inline VOID		GetDurZeroTextureName(CHAR *pszName)		{	sprintf( pszName, "%s%.*x", AC_EXPORT_HD_TEX_ITEM_DUR_ZERO, AC_EXPORT_ID_LENGTH, GetDurZeroTextureID() );	}
	inline VOID		GetDur5UnderTextureName(CHAR *pszName)		{	sprintf( pszName,  "%s%.*x", AC_EXPORT_HD_TEX_ITEM_DUR_5_UNDER, AC_EXPORT_ID_LENGTH, GetDur5UnderTextureID() );	}

#else // _BIN_EXEC_
public:
	inline VOID		SetBaseDFFID(UINT32 ulExportedID)			{}
	inline VOID		SetSecondDFFID(UINT32 ulExportedID)			{}
	inline VOID		SetFieldDFFID(UINT32 ulExportedID)			{}
	inline VOID		SetPickDFFID(UINT32 ulExportedID)			{}

	inline VOID		SetTextureID(UINT32 ulExportedID)			{}
	inline VOID		SetSmallTextureID(UINT32 ulExportedID)		{}
	inline VOID		SetDurZeroTextureID(UINT32 ulExportedID)	{}
	inline VOID		SetDur5UnderTextureID(UINT32 ulExportedID)	{}
	
#endif // _BIN_EXEC_

	BOOL	IsSkipExport( ApServiceArea eArea )
	{
		return m_nDNF & GETSERVICEAREAFLAG( eArea );
	}
};

struct AgcdItemCreateArmourPartParams
{
	PVOID		m_pvClass;
	RpClump*	m_pstBaseClump;
	INT32		m_lPartID;
};

class AttachedAtomicList
{
public:
	RpAtomic			*m_pstAtomic;
	AttachedAtomicList	*m_pcsNext;

	AttachedAtomicList()
	{
		m_pstAtomic = NULL;
		m_pcsNext	= NULL;
	}
};

class AttachedAtomics
{
public:
	AttachedAtomicList	*pcsList;
	RpHAnimHierarchy	*m_pstHierarchy;

	AttachedAtomics()
	{
		pcsList			= NULL;
		m_pstHierarchy	= NULL;		
	}
};

struct AgcdItemTemplateEquipSet
{
	UINT16					m_nCBCount;

	INT16					m_nPart;
	INT16					m_nKind;

	RpClump					*m_pstBaseClump;
	RpHAnimHierarchy		*m_pstBaseHierarchy;
	RpHAnimHierarchy		*m_pstEquipmentHierarchy;
	ItemTransformInfo		*m_pstTransformInfo;

	AttachedAtomics			*m_pcsAttachedAtomics;

	PVOID					m_pvClass;
	PVOID					m_pvCustData;

	INT32					m_nItemTID;
};

struct AgcdItem
{
	PVOID					m_pvClass;
	PVOID					m_pvTextboard;
	RpClump					*m_pstClump;				// �⺻ Ŭ����
	AttachedAtomics			m_csAttachedAtomics;		// ĳ���Ϳ� �پ� �ִ� ����͵�...
	RpAtomic				*m_pstPickAtomic;
	RpHAnimHierarchy		*m_pstHierarchy;
	AgcdItemTemplate		*m_pstAgcdItemTemplate;

	INT32					m_lStatus;					// Item Status (Init, Ready ��)

	UINT32					m_ulLastUseTime;

	ApRWLock				m_rwLock;

	BOOL					m_bHaveExtraData;
	AgpdSealData			m_SealData;

	AgcdItem()
	{
		m_pvClass				=	NULL;
		m_pvTextboard			=	NULL;
		m_pstClump				=	NULL;
		m_pstPickAtomic			=	NULL;
		m_pstHierarchy			=	NULL;
		m_pstAgcdItemTemplate	=	NULL;
	}
};

#endif
