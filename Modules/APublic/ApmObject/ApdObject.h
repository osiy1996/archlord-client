#pragma once

#include <ApBase/ApBase.h>
#include <ApmOcTree/ApmOcTree.h>

#define APMOBJECT_DATA_OBJECT				0
#define APMOBJECT_DATA_OBJECT_TEMPLATE		1

#define APDOBJECT_MAX_OBJECT_NAME			32
#define APDOBJECT_MAX_BLOCK_INFO			12

enum ApdObjectType
{
	APDOBJECT_TYPE_BLOCKING		= 0x0100,
	APDOBJECT_TYPE_RIDABLE		= 0x0200
};

enum eApdObjectStatus
{
	APDOBJ_STATUS_INVALID	= 0,
	APDOBJ_STATUS_NORMAL,
	APDOBJ_STATUS_ACTIVE_TYPE1,
	APDOBJ_STATUS_ACTIVE_TYPE2,
	APDOBJ_STATUS_ACTIVE_TYPE3,
	APDOBJ_STATUS_ACTIVE_TYPE4,
	APDOBJ_STATUS_ACTIVE_TYPE5,
};

class ApdObjectTemplate;
class ApdObject : public ApBase
{
public:
	INT32				m_nDimension;		// �� �ε���.

	INT32				m_lTID;
	
	AuPOS				m_stScale;
	AuPOS				m_stPosition;
	AuMATRIX			m_stMatrix;
	FLOAT				m_fDegreeX;
	FLOAT				m_fDegreeY;

	INT16				m_nBlockInfo;
	AuBLOCKING			m_astBlockInfo[APDOBJECT_MAX_BLOCK_INFO];

	BOOL				m_bStatic;			// Static Model�ΰ�? TRUE�̸�, Dynamic�ϰ� Load���� �ʴ´�.
	INT32				m_lCurrentStatus;	// ���� OBJECT�� ����
	INT32				m_nObjectType;		// ��� �����Ŀ� ���� ����.

	INT32				m_nOcTreeID;
	OcTreeIDList*		m_listOcTreeID;		// OcTree ������ID List
	
	UINT32				m_ulRemoveTimeMSec;
	BOOL				m_bAddedToWorld;

	ApdObjectTemplate* m_pcsTemplate;		// Object Template Pointer

	void	Init();
	void	CalcWorldBlockInfo( int nBlockCount , AuBLOCKING * pBlockingTemplate , AuBLOCKING * pBlock );
};

class ApaObject : public ApAdmin
{
public:
	ApaObject()				{	}
	virtual ~ApaObject()	{	}

};

class ApdObjectTemplate : public ApBase
{
public:
	CHAR				m_szName[APDOBJECT_MAX_OBJECT_NAME];

	INT16				m_nBlockInfo;
	AuBLOCKING			m_astBlockInfo[APDOBJECT_MAX_BLOCK_INFO];

	INT32				m_nObjectType;		// ��� �����Ŀ� ���� ����. AcuObjectType �� ���� ���� ����Ѵ�..

	void	Init();
	CHAR * GetName()		{ return m_szName; }
};

class ApaObjectTemplate : public ApAdmin
{
public:
	ApaObjectTemplate()				{	}
	virtual ~ApaObjectTemplate()	{	}

};