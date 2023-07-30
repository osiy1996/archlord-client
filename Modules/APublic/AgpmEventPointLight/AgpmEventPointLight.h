#pragma once

// ������ (2005-05-19 ���� 11:32:44) : 
// ����Ʈ����Ʈ ���� ���̱� �������̽�.

#include <ApModule/ApModule.h>

#define AGPMEVENTPOINTLIGHT_INI_NAME_START			"PointLightStart"
#define AGPMEVENTPOINTLIGHT_INI_NAME_END			"PointLightEnd"

typedef enum
{
	AGPMPOINTLIGHT_DATA		= 0,
} AgpmPointLightData;

// ��ī�� ������ ���� ��Ʈ����..
class AgpdPointLight : public ApBase
{
public:
	// nothing..

	/*
	UINT8	uRed		;
	UINT8	uGreen		;
	UINT8	uBlue		;
	FLOAT	fRadius		;
	UINT32	uEnableFlag	;

	// 0��° ��Ʈ�� 0��
	// 1��° ��Ʈ�� 1��
	// ...
	// 23��° ��Ʈ�� 23��
	*/
};

enum AgpmEventPointLightCallbackPoint			// Callback ID
{
	AGPMEVENTPOINTLIGHT_ADDOBJECT		= 0	,			
	AGPMEVENTPOINTLIGHT_REMOVEOBJECT	= 1
};

class AgpmEventPointLight : public ApModule 
{
public:
	class ApmEventManager*		m_pcsApmEventManager;

public:
	AgpmEventPointLight(void);
	~AgpmEventPointLight(void);

public:
	// ����Ʈ�� �ε��ɶ�..
	BOOL	SetCallback_AddLight(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
		return SetCallback(AGPMEVENTPOINTLIGHT_ADDOBJECT, pfCallback, pClass);
	}
	// ����Ʈ�� ���ŵɶ�..
	BOOL	SetCallback_RemoveLight(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
		return SetCallback(AGPMEVENTPOINTLIGHT_REMOVEOBJECT, pfCallback, pClass);
	}

public:
	virtual BOOL	OnAddModule	();

	// Callback ��ϰ� Data Attach API
	INT16	AttachPointLightData(PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor)
	{
		return SetAttachedModuleData( pClass , AGPMPOINTLIGHT_DATA , nDataSize , pfConstructor , pfDestructor );
	}

// Event Streaming
	static BOOL		CBEventPointLightConstructor(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBEventPointLightDestructor	(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBEventPointLightStreamWrite(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBEventPointLightStreamRead	(PVOID pData, PVOID pClass, PVOID pCustData);

	// Add & Remove
	static BOOL	CBOnAddObject		( PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBOnRemoveObject	( PVOID pData, PVOID pClass, PVOID pCustData);
};
