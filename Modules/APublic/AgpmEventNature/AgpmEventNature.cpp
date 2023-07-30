// AgpmEventNature.cpp: implementation of the AgpmEventNature class.
//
//////////////////////////////////////////////////////////////////////

#include <AgpmEventNature/AgpmEventNature.h>
#include <ApModule/ApModuleStream.h>
#include <AgpmTimer/AgpmTimer.h>
#include <ApmEventManager/ApmEventManager.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//const UINT64	AgpmEventNature::c_YearMiliSecond	= 31104000000				;	// 1000 * 60 * 60 * 24 * 360	;	// �и��� * �� * �� * �� * ��
//const UINT32	AgpmEventNature::c_DayMiliSecond	= 1000 * 60 * 60 * 24		;	// �и��� * �� * �� * ��
//const UINT32	AgpmEventNature::c_HourMiliSecond	= 1000 * 60 * 60			;	// �и��� * �� * ��
//const UINT32	AgpmEventNature::c_MinuteMiliSecond	= 1000 * 60					;	// �и��� * ��
//const UINT32	AgpmEventNature::c_MiliSecond		= 1000						;	// �и���

static char g_str_TID	[ 128 ]		= "TID";
static char g_str_Name[ 128 ]		= "NAME";

AgpmEventNature::AgpmEventNature()
{
	SetModuleName("AgpmEventNature");

	//EnableIdle(TRUE);

	SetPacketType(AGPMEVENT_NATURE_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(AUTYPE_INT8, 1,			// Weather
//							AUTYPE_INT8, 1,			// Time Ratio
//							AUTYPE_INT64, 1,		// Time
							AUTYPE_INT64, 1,		// Weather Minimum Delay Time
							AUTYPE_INT64, 1,		// Weather Maximum Delay Time
							AUTYPE_END, 0
							);

//	m_ullInitialTime		= 0;
//	m_ulInitialTickCount	= 0;

	m_csNature.m_eType		= APBASE_TYPE_NONE;
	m_csNature.m_lID		= 0;
	m_csNature.m_eWeather	= AGPDNATURE_WEATHER_TYPE_CALM;
//	m_csNature.m_ullTime	= 0;
	m_csNature.m_Mutex.		Init();
	
//	m_bStopTimer			= FALSE	;

	// ������ (2003-03-25 ���� 3:37:05) : ������ڵ�. �ð��� ���Ƿ� �����ص�..
//	m_ullInitialTime	= 1;
	// ���ؽð��� ������.. �׷��� �Ŀ� ƽ�� ��⿡�� �ް� �ȴ�..

	// Sky Structure ����..
	SetModuleData( sizeof( AgpdSkySet ) , AGPDNATURE_SKY_DATA_OBJECT);
	
	m_pcsAgpmCharacter		= NULL;
	m_pcsApmEventManager	= NULL;
	m_pcsAgpmTimer			= NULL;
}

AgpmEventNature::~AgpmEventNature()
{
	m_csNature.m_Mutex.Destroy();
}

BOOL	AgpmEventNature::OnAddModule()
{
	// ���� Module�� �����´�.
	VERIFY( m_pcsApmEventManager	= ( ApmEventManager	* ) GetModule( "ApmEventManager"	) );
	VERIFY( m_pcsAgpmTimer			= ( AgpmTimer		* ) GetModule( "AgpmTimer"			) );
	VERIFY( m_pcsAgpmCharacter		= ( AgpmCharacter	* ) GetModule( "AgpmCharacter"		) );

	if (!m_pcsApmEventManager || !m_pcsAgpmCharacter || !m_pcsAgpmTimer)
		return FALSE;
	// Event Manager�� Event�� ����Ѵ�.
	if (!m_pcsApmEventManager->RegisterEvent(
		APDEVENT_FUNCTION_NATURE	,
		CBEventNatureConstructor	,
		CBEventNatureDestructor		,
		NULL						,
		CBEventNatureStreamWrite	,
		CBEventNatureStreamRead		,
		this						))
		return FALSE;

	if (!AddStreamCallback( AGPDNATURE_SKY_DATA_OBJECT , SkySetReadCB, SkySetWriteCB, this ) )
		return FALSE;

	return TRUE;
}

BOOL AgpmEventNature::OnInit()
{
	return TRUE;
}

BOOL AgpmEventNature::OnDestroy()
{
	// ��ī�� ��� ����..
	RemoveAllSkySet	();
	return TRUE;
}

BOOL AgpmEventNature::OnIdle(UINT32 ulClockCount)
{
/*	if (m_ullInitialTime)
	{
		// �̴ϼ� Ÿ���� ������ ���¿����� .. ƽ�� üũ��..
		if (!m_ulInitialTickCount)
			m_ulInitialTickCount = ulClockCount;
		else
			// �����Ŀ� �ð� ����.. ���� �ð��� ���⿡ �����̵ȴ�..
			if( !GetTimer() )
				m_csNature.m_ullTime =
						m_ullInitialTime +
						((ulClockCount - m_ulInitialTickCount) << m_csNature.m_ucTimeRatio );
	}*/

	return TRUE;
}

BOOL AgpmEventNature::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	// m_csPacket
	INT8		cWeatherType = -1;
//	UINT8		ucTimeRatio = 0;
//	UINT64		ullTime = 0;
	UINT64		ullMinDelay = 0;
	UINT64		ullMaxDelay = 0;

	m_csPacket.GetField(TRUE, pvPacket, nSize, 
						&cWeatherType, 
//						&ucTimeRatio,
//						&ullTime,
						&ullMinDelay,
						&ullMaxDelay);

	if (cWeatherType != -1)
	{
		m_csNature.m_eWeather = (AgpdNatureWeatherType) cWeatherType;
	}

/*	if (ucTimeRatio)
	{
		m_csNature.m_ucTimeRatio = ucTimeRatio;
	}

	if (ullTime)
	{
		m_ullInitialTime = ullTime;
		m_ulInitialTickCount = 0;

		m_csNature.m_ullTime = ullTime;
	}*/

	if (ullMinDelay)
	{
		m_csNature.m_ullWeatherMinDelay = ullMinDelay;
	}

	if (ullMaxDelay)
	{
		m_csNature.m_ullWeatherMaxDelay = ullMaxDelay;
	}

	EnumCallback(AGPMEVENT_TELEPORT_CB_ID_NATURE, &m_csNature, NULL);

	return TRUE;
}

BOOL AgpmEventNature::SetWeather(AgpdNatureWeatherType eWeather)
{
	m_csNature.m_eWeather = eWeather;

	EnumCallback(AGPMEVENT_TELEPORT_CB_ID_NATURE, &m_csNature, NULL);

	return TRUE;
}


BOOL AgpmEventNature::SetCallbackNature(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_TELEPORT_CB_ID_NATURE, pfCallback, pClass);
}

/*BOOL AgpmEventNature::StreamWrite(CHAR *szFile)
{
	ApModuleStream		csStream;

	// Section�� ���ϰ�
	if (!csStream.SetSection(AGPMNATURE_STREAM_NAME_NATURE))
		return FALSE;

//	if (!csStream.WriteValue(AGPMNATURE_STREAM_NAME_TIME, (INT64) m_csNature.m_ullTime))
//		return FALSE;

//	if (!csStream.WriteValue(AGPMNATURE_STREAM_NAME_TIME_RATIO, m_csNature.m_ucTimeRatio))
//		return FALSE;

	if (!csStream.WriteValue(AGPMNATURE_STREAM_NAME_WEATHER, m_csNature.m_eWeather))
		return FALSE;

	if (!csStream.WriteValue(AGPMNATURE_STREAM_NAME_WEATHER_MIN_DELAY, (INT64) m_csNature.m_ullWeatherMinDelay))
		return FALSE;

	if (!csStream.WriteValue(AGPMNATURE_STREAM_NAME_WEATHER_MIN_DELAY, (INT64) m_csNature.m_ullWeatherMinDelay))
		return FALSE;

	csStream.Write(szFile);

	return TRUE;
}*/

/*BOOL AgpmEventNature::StreamRead(CHAR *szFile)
{
	ApModuleStream		csStream;
	const CHAR *		szValueName;

	if (!csStream.Open(szFile))
		return FALSE;

	if (!csStream.SetSection(AGPMNATURE_STREAM_NAME_NATURE))
		return FALSE;

	while (csStream.ReadNextValue())
	{
		szValueName = csStream.GetValueName();
//		if (!strcmp(szValueName, AGPMNATURE_STREAM_NAME_TIME))
//			csStream.GetValue((INT64 *) &m_csNature.m_ullTime);
//		else if (!strcmp(szValueName, AGPMNATURE_STREAM_NAME_TIME_RATIO))
//			csStream.GetValue((INT32 *) &m_csNature.m_ucTimeRatio);
//		else if (!strcmp(szValueName, AGPMNATURE_STREAM_NAME_WEATHER))
		if (!strcmp(szValueName, AGPMNATURE_STREAM_NAME_WEATHER))
			csStream.GetValue((INT32 *) &m_csNature.m_eWeather);
		else if (!strcmp(szValueName, AGPMNATURE_STREAM_NAME_WEATHER_MIN_DELAY))
			csStream.GetValue((INT64 *) &m_csNature.m_ullWeatherMinDelay);
		else if (!strcmp(szValueName, AGPMNATURE_STREAM_NAME_WEATHER_MIN_DELAY))
			csStream.GetValue((INT64 *) &m_csNature.m_ullWeatherMaxDelay);
	}

	return TRUE;
}*/

/*UINT8	AgpmEventNature::SetSpeedRate	( UINT8 speed	)
{
	UINT8	backup = m_csNature.m_ucTimeRatio	;
	m_csNature.m_ucTimeRatio	= speed;
	return backup;
}

void	AgpmEventNature::SetTime			( INT32 hour , INT32 minute )
{
	// ASSERT( GetTimer() );	// �ð� ���� �־����..

	m_csNature.m_ullTime =
		( hour * 60 + minute ) * 60 * 1000;
}

void	AgpmEventNature::SetTime			( INT32 hour , INT32 minute, INT32 second, UINT64 *pullTime)
{
	if (!pullTime)
		return ;

	*pullTime = 
		( hour * 60 * 60 + minute * 60 + second ) * 1000;

	return ;
}

UINT64		AgpmEventNature::ConvertRealtoGameTime	( UINT32 time	)
{
	return (UINT64) (time / 1000 * GetSpeedRate());
}

UINT32		AgpmEventNature::ConvertGametoRealTime	( UINT64 time	)
{
	return (UINT32) (time * 1000 / GetSpeedRate());
}*/

////////////////////////////////////////////////////////////
// Sky Streaming Part
////////////////////////////////////////////////////////////

BOOL	AgpmEventNature::SetCallback_AddSky				(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_SKY_CB_ADD, pfCallback, pClass);
}
BOOL	AgpmEventNature::SetCallback_RemoveSky			(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_SKY_CB_REMOVE, pfCallback, pClass);
}

BOOL	AgpmEventNature::SkySetWriteCB	(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	AgpdSkySet *	pcsAgpdSkySet = (AgpdSkySet *) pData;

	// Object�� ������ Write�Ѵ�.
	if (!pStream->WriteValue( g_str_TID , (INT32) pcsAgpdSkySet->m_nIndex ) )
		return FALSE;

	if (!pStream->WriteValue( g_str_Name , pcsAgpdSkySet->m_strName ) )
		return FALSE;

	return TRUE;
}

BOOL	AgpmEventNature::SkySetReadCB	(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	const char *	szValueName;
	AgpdSkySet *	pcsAgpdSkySet = (AgpdSkySet *) pData;

	// ���� Value�� ���������� �� �׸� �´� ���� �д´�.
	while (pStream->ReadNextValue())
	{
		szValueName = pStream->GetValueName();

		if (!strcmp(szValueName, g_str_TID))
		{
			pStream->GetValue((INT32 *) &pcsAgpdSkySet->m_nIndex);
		}
		else if (!strncmp(szValueName, g_str_Name, strlen(g_str_Name) ))
		{
			pStream->GetValue( pcsAgpdSkySet->m_strName , AGPDSKYSET_MAX_NAME );
		}
	}

	return TRUE;
}

AgpdSkySet * AgpmEventNature::CreateSkySet	()// ��༮ ����..
{
	AgpdSkySet *	pcsSkySet;
	VERIFY( pcsSkySet = (AgpdSkySet *) CreateModuleData( AGPDNATURE_SKY_DATA_OBJECT ) );
	
	ZeroMemory(pcsSkySet->m_strName, AGPDSKYSET_MAX_NAME + 1);

	return pcsSkySet;
}

BOOL	AgpmEventNature::DeleteSkySet	( AgpdSkySet * pSkySet	)
{
	ASSERT( NULL != pSkySet );
	if( NULL == pSkySet ) return FALSE;

	VERIFY( DestroyModuleData( pSkySet , AGPDNATURE_SKY_DATA_OBJECT ) );
	return TRUE;
}

BOOL	AgpmEventNature::AddSkySet		( AgpdSkySet * pSkySet	)
{
	ASSERT( NULL != pSkySet );

	// ����Ʈ�� �ֱ����� �ݹ� ȣ��..
	if( FALSE == EnumCallback( AGPMEVENT_SKY_CB_ADD , ( PVOID ) pSkySet , NULL ) )
	{
		TRACE( "SKY ADD Callback Return Fails!\n" );
//		TRACEFILE(ALEF_ERROR_FILENAME, "SKY ADD Callback Return Fails!\n" );
		
		return FALSE;
	}

	// �ϴ� �ְ�.. ������ ��Ʈ�����Ҷ� �Ѵ�..
	m_listSkySet.AddTail( pSkySet );

	return TRUE;
}

BOOL	AgpmEventNature::AddSkySet		( INT32 nIndex , char * pStrName		)
	// �ε����� 0����..
{
	ASSERT( NULL != pStrName );

	if( GetSkySet( nIndex ) )
	{
		// ��ģ��..
		TRACE( "�̹� �ش� Sky Set�� �����մϴ�.(%d)\n" , nIndex );
//		TRACEFILE(ALEF_ERROR_FILENAME, "�̹� �ش� Sky Set�� �����մϴ�.(%d)\n" , nIndex );
		return FALSE;
	}

	AgpdSkySet *	pcsSkySet = CreateSkySet();

	// ī��..
	pcsSkySet->m_nIndex	= nIndex;
	ZeroMemory(pcsSkySet->m_strName, sizeof(CHAR) * (AGPDSKYSET_MAX_NAME + 1));
	strncpy( pcsSkySet->m_strName , pStrName , AGPDSKYSET_MAX_NAME );

	if( AddSkySet( pcsSkySet ) )
	{
		return TRUE;
	}
	else
	{
		TRACE( "SKY ADD Callback Return Fails!\n" );
//		TRACEFILE(ALEF_ERROR_FILENAME, "SKY ADD Callback Return Fails!\n" );
		
		DeleteSkySet( pcsSkySet );

		return FALSE;
	}
}

INT32	AgpmEventNature::GetEmptySkySetIndex		()
{
	INT32	nIndex = 0;

	while( GetSkySet( nIndex ) )
	{
		++nIndex;
	}

	return nIndex;
}

BOOL	AgpmEventNature::RemoveSkySet	( INT32 nIndex			)
{
	return RemoveSkySet( GetSkySet( nIndex ) );
}

BOOL	AgpmEventNature::RemoveSkySet	( AgpdSkySet * pSkySet	)
{
	ASSERT( NULL != pSkySet );
	if( NULL == pSkySet ) return FALSE;

	// ��¥ �ִ��� ����..

	AuNode< AgpdSkySet * >	* pNode = m_listSkySet.GetHeadNode();

	while( pNode )
	{
		if( pNode->GetData() == pSkySet )
			// Found...
			break;

		m_listSkySet.GetNext( pNode );
	}

	ASSERT( NULL != pNode );
	if( NULL == pNode )
	{
		TRACE( "����Ʈ�ȿ� �ش� ����Ÿ�� �������� �ʽ��ϴ�.\n" );
//		TRACEFILE(ALEF_ERROR_FILENAME, "����Ʈ�ȿ� �ش� ����Ÿ�� �������� �ʽ��ϴ�.\n" );
		return FALSE;
	}

	// ����Ʈ�� �������� �ݹ� ȣ��..
	if( FALSE == EnumCallback( AGPMEVENT_SKY_CB_REMOVE , ( PVOID ) pSkySet , NULL ) )
	{
		TRACE( "SKY REMOVE Callback Return Fails!\n" );
//		TRACEFILE(ALEF_ERROR_FILENAME, "SKY REMOVE Callback Return Fails!\n" );

		// ���� �� ����;;
		
		return FALSE;
	}

	m_listSkySet.RemoveNode( pNode );

	DeleteSkySet( pSkySet );

	return TRUE;
}

AgpdSkySet *	AgpmEventNature::GetSkySet		( INT32	nIndex	)
{
	AuNode< AgpdSkySet * >	*	pNode		= m_listSkySet.GetHeadNode();
	AgpdSkySet *				pcsSkySet	;

	while( pNode )
	{
		pcsSkySet	= pNode->GetData();
		if( pcsSkySet->m_nIndex == nIndex )
			// Found...
			return pcsSkySet;

		m_listSkySet.GetNext( pNode );
	}

	return NULL;
}

BOOL	AgpmEventNature::SkySet_StreamWrite		(CHAR *szFile, BOOL bEncryption)
{
	ApModuleStream		csStream;
	INT32				lIndex = 0;
	CHAR				szTID[32];

	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);

	AuNode< AgpdSkySet * >	*	pNode		= m_listSkySet.GetHeadNode();
	AgpdSkySet *				pcsSkySet	;

	while( pNode )
	{
		pcsSkySet	= pNode->GetData();

		sprintf( szTID , "%d", pcsSkySet->m_nIndex );

		// TID�� Section�� �����ϰ�
		csStream.SetSection(szTID);

		if (!csStream.EnumWriteCallback(AGPMEVENTNATURE_SKY_DATA_STREAM, pcsSkySet, this))
		{
			ASSERT(!"AgpmEventNature::SkySet_StreamWrite() EnumWriteCallback Failure !!!");
			return FALSE;
		}

		m_listSkySet.GetNext( pNode );
	}

	csStream.Write(szFile, 0, bEncryption);

	return TRUE;
}

BOOL	AgpmEventNature::SkySet_StreamRead		(CHAR *szFile, BOOL bDecryption)
{
	RemoveAllSkySet();

	ApModuleStream		csStream;
	UINT16				nNumKeys;
	INT32				i;
	AgpdSkySet *		pcsSkySet	;

	// szFile�� �д´�.
	csStream.Open(szFile, 0, bDecryption);

	nNumKeys = csStream.GetNumSections();
	
	INT32	nCount = 0;

	// �� Section�� ���ؼ�...
	for (i = 0; i < nNumKeys; ++i)
	{
		// Object�� �����. 
		pcsSkySet = CreateSkySet();
		if (!pcsSkySet)
			return NULL;

		// OID�� �����ؼ�
		pcsSkySet->m_nIndex = atoi(csStream.ReadSectionName(i));

		// Stream Enumerate �Ѵ�.
		if (!csStream.EnumReadCallback(AGPMEVENTNATURE_SKY_DATA_STREAM, pcsSkySet, this ) )
		{
			ASSERT(!"AgpmEventNature::SkySet_StreamRead() EnumReadCallback Failure !!!");
			return FALSE;
		}

		// ���������� Add
		if ( AddSkySet( pcsSkySet ) )
		{
			// do nothing
			nCount++;
		}
		else
		{
			ASSERT(!"AgpmEventNature::SkySet_StreamRead() AddSkySet Failure !!!");
			DeleteSkySet(pcsSkySet);
			return FALSE;
		}
	}

	if( nCount == 0 )
	{
		// ����Ʈ ��Ÿ�� ����..
		AddDefaultSkySet();
	}

	return TRUE;
}

INT16	AgpmEventNature::AttachSkySetData	(PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor , ApModuleDefaultCallBack pfDestructor )
{
	return SetAttachedModuleData( pClass , AGPMEVENTNATURE_SKY_DATA_STREAM , nDataSize , pfConstructor , pfDestructor);
}

void	AgpmEventNature::RemoveAllSkySet	()
{
	AuList< AgpdSkySet * > * pList = GetSkySetList	();
	AuNode< AgpdSkySet * > * pNode	;
	AgpdSkySet * pSkySet;

	while( pNode = pList->GetHeadNode() )
	{
		pSkySet	= pNode->GetData();

		// ����Ʈ�� �������� �ݹ� ȣ��..
		if( FALSE == EnumCallback( AGPMEVENT_SKY_CB_REMOVE , ( PVOID ) pSkySet , NULL ) )
		{
			TRACE( "SKY REMOVE Callback Return Fails!\n" );
//			TRACEFILE(ALEF_ERROR_FILENAME, "SKY REMOVE Callback Return Fails!\n" );

			// ���� �� ����;;
		}

		m_listSkySet.RemoveNode( pNode );

		DeleteSkySet( pSkySet );
	}
}

BOOL	AgpmEventNature::AddDefaultSkySet	()
{
	// �� �����..;;
	RemoveAllSkySet();
	
	AddSkySet( 0 , "Default" );

	return TRUE;
}

BOOL				AgpmEventNature::CBEventNatureConstructor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpmEventNature *		pThis		= (AgpmEventNature *) pClass;
	ApdEvent *				pstDstEvent	= (ApdEvent *) pData;
	ApdEvent *				pstSrcEvent	= (ApdEvent *) pCustData;

	// m_pvData �� ��ī�� ���ø� ID�� �־�д�. INT��.
	pstDstEvent->m_pvData = ( PVOID ) 0;

	if (pstSrcEvent)
	{
		pstDstEvent->m_pvData	= pstSrcEvent->m_pvData	;
	}

	return TRUE;
}

BOOL				AgpmEventNature::CBEventNatureDestructor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpmEventNature *		pThis		= (AgpmEventNature *) pClass;
	ApdEvent *				pstEvent = (ApdEvent *) pData;

	return TRUE;
}

BOOL	AgpmEventNature::CBEventNatureStreamWrite(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpmEventNature *		pThis		= (AgpmEventNature *) pClass;
	ApdEvent *				pstEvent	= (ApdEvent *) pData;
	ApModuleStream *		pstStream	= (ApModuleStream *) pCustData;

	if (!pstStream->WriteValue(AGPMNATURE_INI_NAME_START, 0))
		return FALSE;

	if (!pstStream->WriteValue(AGPMNATURE_INI_NAME_SKYTEMPLATEID, ( INT_PTR ) pstEvent->m_pvData ))
		return FALSE;

	if (!pstStream->WriteValue(AGPMNATURE_INI_NAME_END, 0))
		return FALSE;

	return TRUE;
}

BOOL	AgpmEventNature::CBEventNatureStreamRead(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpmEventNature *		pThis		= (AgpmEventNature *) pClass;
	ApdEvent *				pstEvent	= (ApdEvent *) pData;
	ApModuleStream *		pstStream	= (ApModuleStream *) pCustData;
	const CHAR *			szValueName;
	INT32					nTemplateID	;

	if (!pstStream->ReadNextValue())
		return TRUE;

	szValueName = pstStream->GetValueName();
	if (strcmp(szValueName, AGPMNATURE_INI_NAME_START))
		return TRUE;

	while (pstStream->ReadNextValue())
	{
		szValueName = pstStream->GetValueName();

		if (!strcmp(szValueName, AGPMNATURE_INI_NAME_SKYTEMPLATEID))
		{
			pstStream->GetValue( &nTemplateID );
			pstEvent->m_pvData	= IntToPtr(nTemplateID);
		}
		else if (!strcmp(szValueName, AGPMNATURE_INI_NAME_END))
			break;
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// ���⼭���� AgpmTimer�߰��� ���� BOB�� �۾�(061003)
UINT8 AgpmEventNature::SetSpeedRate(UINT8 speed)
{
	return m_pcsAgpmTimer->SetSpeedRate(speed);
}

UINT8 AgpmEventNature::GetSpeedRate()
{
	return m_pcsAgpmTimer->GetSpeedRate();
}

UINT64 AgpmEventNature::ConvertRealtoGameTime(UINT32 time)
{
	return m_pcsAgpmTimer->ConvertRealtoGameTime(time);
}

UINT32 AgpmEventNature::ConvertGametoRealTime(UINT64 time)
{
	return m_pcsAgpmTimer->ConvertGametoRealTime(time);
}

BOOL AgpmEventNature::GetTimer()
{
	return m_pcsAgpmTimer->TimerIsStop();
}

void AgpmEventNature::StartTimer()
{
	m_pcsAgpmTimer->StartTimer();
}

void AgpmEventNature::StopTimer()
{
	m_pcsAgpmTimer->StopTimer();
}

void AgpmEventNature::ToggleTimer()
{
	m_pcsAgpmTimer->ToggleTimer();
}

void AgpmEventNature::SetTime(INT32 hour , INT32 minute)
{
	m_pcsAgpmTimer->SetTime(hour, minute);
}

void AgpmEventNature::SetTime(INT32 hour, INT32 minute, INT32 second, UINT64 *pullTime)
{
	m_pcsAgpmTimer->SetTime(hour, minute, second, pullTime);
}
	
UINT64 AgpmEventNature::GetGameTime()
{
	return m_pcsAgpmTimer->GetGameTime();
}

UINT32 AgpmEventNature::GetYear(UINT64 time)
{
	return m_pcsAgpmTimer->GetYear(time);
}

UINT32 AgpmEventNature::GetMonth(UINT64 time)
{
	return m_pcsAgpmTimer->GetMonth(time);
}

UINT32 AgpmEventNature::GetDay(UINT64 time)
{
	return m_pcsAgpmTimer->GetDay(time);
}

UINT32 AgpmEventNature::GetHour(UINT64 time)
{
	return m_pcsAgpmTimer->GetHour(time);
}

BOOL AgpmEventNature::IsPM(UINT64 time)
{
	return m_pcsAgpmTimer->IsPM(time);
}

UINT32 AgpmEventNature::GetMinute(UINT64 time)
{
	return m_pcsAgpmTimer->GetMinute(time);
}

UINT32 AgpmEventNature::GetSecond(UINT64 time)
{
	return m_pcsAgpmTimer->GetSecond(time);
}

UINT32 AgpmEventNature::GetMS(UINT64 time)
{
	return m_pcsAgpmTimer->GetMS(time);
}

UINT32 AgpmEventNature::GetDayTimeDWORD(UINT64 time)
{
	return m_pcsAgpmTimer->GetDayTimeDWORD(time);
}

UINT32 AgpmEventNature::GetHourTimeDWORD(UINT64 time)
{
	return m_pcsAgpmTimer->GetHourTimeDWORD(time);
}

UINT32 AgpmEventNature::GetDayMiliSecond()
{
	return m_pcsAgpmTimer->GetDayMiliSecond();
}
// ������� AgpmTimer�߰��� ���� BOB�� �۾�(061003)
///////////////////////////////////////////////////////////////////////////////