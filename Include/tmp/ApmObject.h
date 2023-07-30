#pragma once

#include "ApdObject.h"
#include "ApModuleStream.h"
#include "ApmMap.h"

#include <vector>

enum ApmObjectCallbackPoint			// Callback ID in Object Module
{
	APMOBJECT_CB_ID_INIT			= 0	,
	APMOBJECT_CB_ID_REMOVE				,
	APMOBJECT_CB_ID_UPDATE_STATUS		,
	APMOBJECT_CB_ON_ADD_OBJECT			,
	APMOBJECT_CB_ON_LOAD_SECTOR			,
	APMOBJECT_CB_ON_LOAD_ERROR			,
};


#define APMOBJECT_INI_NAME_NAME				"Name"
#define APMOBJECT_INI_NAME_BTYPE			"BlockingType"
#define	APMOBJECT_INI_NAME_OBJECTTYPE		"ObjectType"

#define APMOBJECT_INI_NAME_BOX_INF			"BoxInf"
#define APMOBJECT_INI_NAME_BOX_SUP			"BoxSup"
#define APMOBJECT_INI_NAME_SPHERE_CENTER	"SphereCenter"
#define APMOBJECT_INI_NAME_SPHERE_RADIUS	"SphereRadius"
#define APMOBJECT_INI_NAME_CYLINDER_CENTER	"CylinderCenter"
#define APMOBJECT_INI_NAME_CYLINDER_HEIGHT	"CylinderHeight"
#define APMOBJECT_INI_NAME_CYLINDER_RADIUS	"CylinderRadius"

#define APMOBJECT_INI_NAME_BOX				"Box"
#define APMOBJECT_INI_NAME_SPHERE			"Sphere"
#define APMOBJECT_INI_NAME_CYLINDER			"Cylinder"
#define APMOBJECT_INI_NAME_MINBOX			"MinBox"
#define APMOBJECT_INI_NAME_NONE				"None"

#define APMOBJECT_INI_NAME_TID				"TID"
#define APMOBJECT_INI_NAME_SCALE			"Scale"
#define APMOBJECT_INI_NAME_POSITION			"Position"
#define	APMOBJECT_INI_NAME_DEGREE_X			"DegreeX"
#define	APMOBJECT_INI_NAME_DEGREE_Y			"DegreeY"

#define APMOBJECT_INI_NAME_STATIC			"Static"

// ������ ������� ����..
#define APMOBJECT_INI_NAME_AXIS				"Axis"
#define APMOBJECT_INI_NAME_DEGREE			"Degree"

#define APMOBJECT_INI_NAME_OCTREE_IDNUM		"OcTreeIDNum"
#define APMOBJECT_INI_NAME_OCTREE_IDDATA	"OcTreeIDData"

// ���� �̸�..
#define	APMOBJECT_INI_FILE_NAME				"Obj%05d.ini"
#define	APMOBJECT_INI_SERVER_FILE_NAME		"OBS%04d.ini"

// ������ ���� ���� 1.1
#define APMOBJECT_LOCAL_INI_FILE_NAME_NORMAL	"Ini\\obdn%04d.ini"
#define APMOBJECT_LOCAL_INI_FILE_NAME_STATIC	"Ini\\obds%04d.ini"

// ���� �� ���� ���� 1.2
// ������ (2005-04-14 ���� 4:00:56) : ���� ����Ÿ�� ���� �� ����..
#define APMOBJECT_LOCAL_INI_FILE_NAME_2_NORMAL	"map\\data\\object\\obdn%04d.ini"
#define APMOBJECT_LOCAL_INI_FILE_NAME_2_STATIC	"map\\data\\object\\obds%04d.ini"
#define APMOBJECT_LOCAL_INI_FILE_NAME_2_ARTIST	"map\\data\\object\\artist\\obda%04d.ini"
#define APMOBJECT_LOCAL_INI_FILE_NAME_2_DESIGN	"map\\data\\object\\design\\obdd%04d.ini"
#define APMOBJECT_LOCAL_INI_FILE_NAME_2_PLIGHT	"map\\data\\object\\pointlight\\obdl%04d.ini"
	// ����Ʈ����Ʈ ������Ʈ ����.

// ���������� �۾��� ���������� ���⿡ ����. �������� ���� �ʰ�
// ���������� �о����!..
#define APMOBJECT_LOCAL_INI_FILE_NAME_2_SPAWN		"ini\\object\\design\\obdp%04d.ini"
#define APMOBJECT_LOCAL_INI_FILE_NAME_2_BOSSSPAWN	"ini\\object\\design\\obdb%04d.ini"
	// �������� ��� ���� ����

#define	AGPMOBJECT_PROCESS_REMOVE_INTERVAL	3000
#define	AGPMOBJECT_PRESERVE_CHARACTER_DATA	5000

#define	AGPMOBJECT_ONIDLE_REMOVE_COUNT	1

class CMapToolDebug
{
public:
	void	SetTemplateReplace( int nOrigianl , int nReplace )
	{
		POINT	pt;
		pt.x	= nOrigianl	;
		pt.y	= nReplace	;
		
		m_vectorTemplateMatchingTable.push_back( pt );
	}

	int		GetTemplateReplaceIndex( int nOriginal )
	{
		std::vector<POINT>::iterator Iter;
		for ( Iter = m_vectorTemplateMatchingTable.begin( ) ; Iter != m_vectorTemplateMatchingTable.end( ) ; Iter++ )
		{
			if( (*Iter).x == nOriginal ) return (*Iter).y;
		}

		return nOriginal; // �׳� ������ ����..
	}

	std::vector< POINT >	m_vectorTemplateMatchingTable;
};

class ApmObject : public ApModule, public CMapToolDebug
{
public:
	ApmObject();
	virtual ~ApmObject()	{		}

	BOOL	OnAddModule();
	BOOL	OnInit();
	BOOL	OnDestroy();
	BOOL	OnIdle(UINT32 ulClockCount);

	BOOL	IsUseIndexUpdate()		{	return m_bUseObjectIndexUpdate;	}
	void	FlushRemoveList()		{				}

	//ApdObject
	ApdObject*	GetObjectSequence(INT32 *plIndex);
	INT16		AttachObjectData(PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor = NULL, ApModuleDefaultCallBack pfDestructor = NULL);
	BOOL		SetMaxObject(INT32 lCount);

	ApdObject*	CreateObject();
	BOOL		DestroyObject(ApdObject *pcsObject);

	ApdObject*	GetObject( INT32 lOID );
	ApdObject*	AddObject( INT32 lOID, INT32 lTID );
	ApdObject*	AddObject( ApdObject* pcsObject	);
	BOOL		DeleteObject( AuBOX bbox, BOOL bForce = FALSE, BOOL bImmediate = FALSE	);	// BBox �����ϰ� �׾ȿ� �ִ� �༮�� ����Ʈ..
	BOOL		DeleteObject( INT32 lOID, BOOL bForce = FALSE, BOOL bImmediate = FALSE	);
	BOOL		DeleteObject( ApdObject* pcsObject, BOOL bForce = FALSE, BOOL bImmediate = FALSE );
	BOOL		DeleteObject( ApWorldSector* pSector, BOOL bForce = FALSE, BOOL bImmediate = FALSE	);
	BOOL		DeleteAllObject( BOOL bForce = FALSE, BOOL bImmediate = FALSE );
	
	BOOL	StreamWrite( const CHAR *szFile, BOOL bEncryption = FALSE , ApModuleDefaultCallBack pCheckCallback = NULL);
	BOOL	StreamWrite( const CHAR *szFile, BOOL bStatic = FALSE	, BOOL bEncryption = FALSE );
	BOOL	StreamRead( const CHAR *szFile, BOOL bDecryption	= FALSE	, ApModuleDefaultCallBack pCheckCallback = NULL	, BOOL bIndexUpdate = FALSE );

	BOOL	StreamWrite( AuBOX bbox, INT32 lPartIndex , const CHAR *szFile , ApModuleDefaultCallBack pCheckCallback = NULL );	// �ݺ����� �˻���. .
	BOOL	StreamRead( AuBOX bbox, INT32 lPartIndex , const CHAR *szFile	);	// ���Ͽ��� ���� �ȿ� �༮�鸸 �鿩��.
	BOOL	StreamRead( ApWorldSector* pSector );	// ���� �������� �ε���..

	BOOL	StreamReadAllDivision( char* fmt, ApModuleDefaultCallBack pCheckCallback = NULL , BOOL bIndexUpdate = FALSE);
	BOOL	StreamReadAllDivision();
	BOOL	StreamReadAllDivisionVersion2();

	static BOOL	ObjectWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	static BOOL	ObjectReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);

	//ApdObjectTemplate
	ApdObjectTemplate*	GetObjectTemplateSequence(INT32 *plIndex);
	INT16				AttachObjectTemplateData(PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor = NULL, ApModuleDefaultCallBack pfDestructor = NULL);
	BOOL				SetMaxObjectTemplate(INT32 lCount);

	ApdObjectTemplate*	CreateTemplate();
	BOOL				DestroyTemplate	(ApdObjectTemplate *pcsTemplate);
	BOOL				DestroyAllTemplate();

	ApdObjectTemplate*	GetObjectTemplate(INT32 lTID);
	ApdObjectTemplate*	AddObjectTemplate(INT32 lTID);
	BOOL				DeleteObjectTemplate(INT32 lTID);

	BOOL	StreamWriteTemplate(const CHAR *szFile, BOOL bEncryption);
	BOOL	StreamReadTemplate(const CHAR *szFile, ProgressCallback pfCallback = NULL , void * pData = NULL, CHAR *pszErrorMessage = NULL, BOOL bDecryption = FALSE );

	static BOOL			ObjectTemplateWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	static BOOL			ObjectTemplateReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);

	//�ڽ��� addmin...
	BOOL	SetMaxObjectRemove(INT32 lCount);
	BOOL	ProcessRemove(UINT32 ulClockCount);
	BOOL	AddRemoveObject(ApdObject *pcsObject);
	
	BOOL	UpdateStatus( INT32 lOID, INT32 lStatus );

	//set callback
	BOOL	UpdateInit( ApdObject *pcsObject );		//callback ����..
	BOOL	SetCallbackInitObject( ApModuleDefaultCallBack pfCallback, PVOID pClass );
	BOOL	SetCallbackRemoveObject( ApModuleDefaultCallBack pfCallback, PVOID pClass );
	BOOL	SetCallbackUpdateStatus( ApModuleDefaultCallBack pfCallback, PVOID pClass );
	BOOL	SetCallbackAddObject( ApModuleDefaultCallBack pfCallback, PVOID pClass );
	BOOL	SetCallbackLoadSector( ApModuleDefaultCallBack pfCallback, PVOID pClass );
	BOOL	SetCallbackLoadError( ApModuleDefaultCallBack pfCallback, PVOID pClass );

	INT32	GetEmptyIndex( INT32 nDivisionIndex );

private:
	BOOL	ParsePOS(const CHAR *szPoint, AuPOS *pstPos	);
	ApmMap*				m_pcsApmMap;

public:
	ApaObject			m_clObjects;
	ApaObjectTemplate	m_clObjectTemplates;
	ApAdmin				m_csAdminRemove;

	char				m_strDir[256];
	UINT32				m_ulPrevRemoveClockCount;
	BOOL				m_bUseObjectIndexUpdate;
};
