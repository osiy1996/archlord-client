#include <AgcmRecruit/AgcmRecruit.h>
#include <AgpmRecruit/AgpmRecruit.h>
#include <AcClientSocket/AcSocketManager.h>
#include <AgpmCharacter/AgpmCharacter.h>
#include <AgcmCharacter/AgcmCharacter.h>
#include <AgcModule/AgcEngine.h>

AgcmRecruit::AgcmRecruit()
{
	SetModuleName( "AgcmRecruit" );

	//��������~ ��������~ ��~��~ � ��Ŷ�� ����������? by ġ�佺��~
	SetPacketType(AGPMRECRUIT_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT16));

	m_csPacket.SetFieldType(AUTYPE_INT8,			1, // Operation ���� 
							AUTYPE_INT32,			1, // lControlServerID
							AUTYPE_INT32,			1, // lPlayerID
							AUTYPE_PACKET,			1, // LFPInfo <-��Ƽ�� ã�� ����� ����.
							AUTYPE_PACKET,			1, // LFMInfo <-����� ã�� ������ �ø� �Խù�����.
							AUTYPE_PACKET,			1, // SearchInfo
							AUTYPE_INT32,			1, // lResult    Result
		                    AUTYPE_END,				0
							);
							
	m_csLFPInfo.SetFlagLength( sizeof(INT8) );
	m_csLFPInfo.SetFieldType(
								AUTYPE_INT32,		1, //lTotalCount
		                        AUTYPE_INT32,		1, //lIndex
		                        AUTYPE_INT32,		1, //LV
		                        AUTYPE_INT32,		1, //Class
								AUTYPE_CHAR,		AGPACHARACTER_MAX_ID_STRING + 1, // pstrPlayerID;
								AUTYPE_INT32,		1, // lPlayerID
		                        AUTYPE_END,			0
							);

	m_csLFMInfo.SetFlagLength( sizeof(INT16) );
	m_csLFMInfo.SetFieldType(
								AUTYPE_INT32,		1, //lTotalCount
		                        AUTYPE_INT32,		1, //lIndex
		                        AUTYPE_CHAR,		20, //Purpose
		                        AUTYPE_INT32,		1, //lRequire Member
		                        AUTYPE_INT32,		1, //Min LV
		                        AUTYPE_INT32,		1, //Max LV
		                        AUTYPE_INT32,		1, //LV
		                        AUTYPE_INT32,		1, //Class
								AUTYPE_CHAR,		AGPACHARACTER_MAX_ID_STRING + 1, // pstrLeaderID;
								AUTYPE_INT32,		1, //lLeaderID
		                        AUTYPE_END,			0
							);

	m_csSearchInfo.SetFlagLength(sizeof(INT8));
	m_csSearchInfo.SetFieldType(
								 AUTYPE_INT8,		1,  //�� ������ �������� ã�´�?
								 AUTYPE_INT32,		1,  //Level��?
								 AUTYPE_INT32,		1,  //���° ������ �ΰ�?
								 AUTYPE_INT32,		1,  //Class Bit Flag
		                         AUTYPE_END,		0
								 );

	m_bSearchByMyLevel	=	 false	;
	m_lSelectedClass	=	 0		;	

	m_lControlServerID = 0;
	m_nControlServerNID = 0;
	m_nRecruitServerNID = 0;
}

AgcmRecruit::~AgcmRecruit()
{
	;
}

BOOL AgcmRecruit::OnAddModule()
{
	m_pagpmCharacter = (AgpmCharacter *) GetModule("AgpmCharacter");
	ASSERT( NULL != m_pagpmCharacter && "get public character moudle failure - AgcmRecruit" );

	m_pagcmCharacter = (AgcmCharacter *) GetModule("AgcmCharacter");
	ASSERT( NULL != m_pagcmCharacter && "get client character moudle failure - AgcmRecruit" );

	m_pagpmFactors = (AgpmFactors *) GetModule("AgpmFactors");
	ASSERT( NULL != m_pagpmFactors && "get public factors moudle failure - AgcmRecruit" );

	m_pagpmItem = (AgpmItem *) GetModule("AgpmItem");
	ASSERT( NULL != m_pagpmItem && "get public item moudle failure - AgcmRecruit" );

	m_pagcmConnectManager = (AgcmConnectManager	*) GetModule( "AgcmConnectManager" );
	ASSERT( NULL != m_pagcmConnectManager && "get client connect manager moudle failure - AgcmRecruit" );

	if( !m_pagpmCharacter || !m_pagcmCharacter || !m_pagpmItem || !m_pagcmConnectManager )
		return FALSE;

	m_bBBSWindowShow = FALSE;

	// UI����
	m_clRecruitWriteWindow.SetModulePointer( this );

	return TRUE;
}

INT32 AgcmRecruit::GetControlServerID()
{
	return m_lControlServerID;
}

INT16 AgcmRecruit::GetControlServerNID()
{
	return m_nControlServerNID;
}

INT16 AgcmRecruit::GetRecruitServerNID()
{
	return m_nRecruitServerNID;
}

void AgcmRecruit::SetControlServerID( INT32 lControlServerID )
{
	m_lControlServerID = lControlServerID;
}

void AgcmRecruit::SetControlServerNID( INT16 nControlServerNID )
{
	m_nControlServerNID = nControlServerNID;
}

void AgcmRecruit::SetRecruitServerNID( INT16 nRecruitServerNID )
{
	m_nRecruitServerNID = nRecruitServerNID;
}

BOOL AgcmRecruit::SendAddToRecruitServer( INT32 lCID )
{
	BOOL			bResult;

	bResult = FALSE;

	INT8	lOperation = AGPMRECRUIT_ADD_TO_RECRUIT;
	INT16	nPacketLength;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMRECRUIT_PACKET_TYPE,
													&lOperation,		//lOperation
													NULL,				//lControlServerID
													&lCID,				//lPlayerID
													NULL,				//pvLFPInfo
													NULL,				//pvLFMInfo
													NULL,				//pvSearchInfo
													NULL				//lResult
												);

	if( pvPacket != NULL )
	{
		bResult = SendPacket(pvPacket, nPacketLength, ACDP_SEND_RECRUITSERVER);
		
		m_csPacket.FreePacket(pvPacket);
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;
}

BOOL AgcmRecruit::SendRegisterToLFP( INT32 lCID, char *pstrCharName )
{
	BOOL			bResult;

	bResult = FALSE;

	if( pstrCharName != NULL )
	{
		INT8	lOperation = AGPMRECRUIT_REGISTER_LFP;
		INT16	nPacketLength;
		INT32	lControlServerID;

		lControlServerID = GetControlServerID();

		void	*pvPlayerInfo;

		pvPlayerInfo = m_csLFPInfo.MakePacket( FALSE, &nPacketLength, AGPMRECRUIT_PACKET_TYPE,
												NULL, //lTotalCount
												NULL, //Index
												NULL, //Level
												NULL, //Class
												pstrCharName, //pstrFileName
												NULL ); //PlayerID

		if( pvPlayerInfo != NULL )
		{
			PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMRECRUIT_PACKET_TYPE,
															&lOperation,		//lOperation
															&lControlServerID,	//lControlServerID
															&lCID,				//lPlayerID
															pvPlayerInfo,		//pvLFPInfo
															NULL,				//pvLFMInfo
															NULL,				//pvSearchInfo
															NULL				//lResult
														);

			m_csPacket.FreePacket(pvPlayerInfo);

			if( pvPacket != NULL )
			{
				bResult = SendPacket(pvPacket, nPacketLength, ACDP_SEND_RECRUITSERVER);

				m_csPacket.FreePacket(pvPacket);
			}
		}
	}

	return bResult;
}

BOOL AgcmRecruit::SendRegisterToLFM( INT32 lCID, char *pstrCharName, char *pstrPurpose, INT32 lRequireMember, INT32 lMinLV, INT32 lMaxLV, INT32 lClass )
{
	BOOL			bResult;

	bResult = FALSE;

	if( pstrCharName != NULL )
	{
		INT8	lOperation = AGPMRECRUIT_REGISTER_LFM;
		INT16	nPacketLength;
		INT32	lControlServerID;

		lControlServerID = GetControlServerID();

		void	*pvLFMInfo;

		pvLFMInfo = m_csLFMInfo.MakePacket( FALSE, &nPacketLength, AGPMRECRUIT_PACKET_TYPE,
												NULL,				//lTotalCount
												NULL,				//Index
												pstrPurpose,		//Purpose
												&lRequireMember,	//Require Members
												&lMinLV,			//Min Lv
												&lMaxLV,			//Max Lv
												NULL,				//LV
												&lClass,			//lClass
												pstrCharName,		//pstrCharName
												&lCID );			//LeaderID

		if( pvLFMInfo != NULL )
		{
			PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMRECRUIT_PACKET_TYPE,
															&lOperation,		//lOperation
															&lControlServerID,	//lControlServerID
															&lCID,				//lPlayerID
															NULL,				//pvLFPInfo
															pvLFMInfo,			//pvLFMInfo
															NULL,				//pvSearchInfo
															NULL				//lResult
														);

			m_csLFMInfo.FreePacket(pvLFMInfo);

			if( pvPacket != NULL )
			{
				bResult = SendPacket(pvPacket, nPacketLength, ACDP_SEND_RECRUITSERVER);
				
				m_csPacket.FreePacket(pvPacket);
			}
		}
	}

	return bResult;
}

BOOL AgcmRecruit::SendCancelFromLFP( INT32 lCID, char *pstrCharName )
{
	BOOL			bResult;

	bResult = FALSE;

	if( pstrCharName != NULL )
	{
		INT8	lOperation = AGPMRECRUIT_CANCEL_LFP;
		INT16	nPacketLength;
		INT32	lControlServerID;

		lControlServerID = GetControlServerID();

		void	*pvPlayerInfo;

		pvPlayerInfo = m_csLFPInfo.MakePacket( FALSE, &nPacketLength, AGPMRECRUIT_PACKET_TYPE,
												NULL, //lTotalCount
												NULL, //Index
												NULL, //Level
												NULL, //Class
												pstrCharName, //pstrFileName
												NULL ); //PlayerID

		if( pvPlayerInfo != NULL )
		{
			PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMRECRUIT_PACKET_TYPE,
															&lOperation,		//lOperation
															&lControlServerID,	//lControlServerID
															&lCID,				//lPlayerID
															pvPlayerInfo,		//pvPlayerInfo
															NULL,				//pvLFMInfo
															NULL,				//pvSearchInfo
															NULL				//lResult
														);

			m_csLFPInfo.FreePacket(pvPlayerInfo);

			if( pvPacket != NULL )
			{
				bResult = SendPacket(pvPacket, nPacketLength, ACDP_SEND_RECRUITSERVER);
				
				m_csPacket.FreePacket(pvPacket);
			}
		}
	}

	return bResult;
}

BOOL AgcmRecruit::SendCancelFromLFM( INT32 lCID, char *pstrCharName )
{
	BOOL			bResult;

	bResult = FALSE;

	if( pstrCharName != NULL )
	{
		INT8	lOperation = AGPMRECRUIT_CANCEL_LFM;
		INT16	nPacketLength;
		INT32	lControlServerID;

		lControlServerID = GetControlServerID();

		void	*pvLFMInfo;

		pvLFMInfo = m_csLFMInfo.MakePacket( FALSE, &nPacketLength, AGPMRECRUIT_PACKET_TYPE,
												NULL,				//lTotalCount
												NULL,				//Index
												NULL,				//Purpose
												NULL,				//Require Members
												NULL,				//Min Lv
												NULL,				//Max Lv
												NULL,				//LV
												NULL,				//lClass
												pstrCharName,		//pstrCharName
												NULL );				//LeaderID

		if( pvLFMInfo != NULL )
		{
			PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMRECRUIT_PACKET_TYPE,
															&lOperation,		//lOperation
															&lControlServerID,	//lControlServerID
															&lCID,				//lPlayerID
															NULL,				//pvLFPInfo
															pvLFMInfo,			//pvLFMInfo
															NULL,				//pvSearchInfo
															NULL				//lResult
														);

			m_csLFMInfo.FreePacket(pvLFMInfo);

			if( pvPacket != NULL )
			{
				bResult = SendPacket(pvPacket, nPacketLength, ACDP_SEND_RECRUITSERVER);
				
				m_csPacket.FreePacket(pvPacket);
			}
		}
	}

	return bResult;
}

BOOL AgcmRecruit::SendSearchLFPByPage( INT32 lCID, bool bSearchByMyLevel, INT32 lPage, INT32 lClass )
{
	AgpdCharacter	*pcAgpdCharacter;
	BOOL			bResult;

	bResult = FALSE;

	INT8	lOperation = AGPMRECRUIT_SEARCH_LFP_BY_PAGE;
	INT16	nPacketLength;
	INT32	lControlServerID;
	void	*pvSearchInfo;

	pcAgpdCharacter = m_pagpmCharacter->GetCharacter( lCID );

	if( pcAgpdCharacter != NULL )
	{
		INT32			lLevel = -1;

		if( bSearchByMyLevel == true )
		{
			lLevel	= m_pagpmCharacter->GetLevel(pcAgpdCharacter);
		}
		else
		{
			lLevel = 0;
		}

		if( lLevel != -1 )
		{
			//�׽�Ʈ�� �ڵ� �������� �����ش�.
			for( int i=0; i<MAX_RECRUIT_ROW; ++i )
			{
				m_clRecruitWindow.m_clBbsMercenary.SetCellText( i, 0, "" );
				m_clRecruitWindow.m_clBbsMercenary.SetCellText( i, 1, "" );
				m_clRecruitWindow.m_clBbsMercenary.SetCellText( i, 2, "" );
			}

			lControlServerID = GetControlServerID();

			pvSearchInfo = m_csSearchInfo.MakePacket(FALSE, &nPacketLength, AGPMRECRUIT_PACKET_TYPE,
												&bSearchByMyLevel,		//bSearchByMyLevel
												&lLevel,				//Level
												&lPage,					//Page
												&lClass					//lClass
												);


			PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMRECRUIT_PACKET_TYPE,
															&lOperation,		//lOperation
															&lControlServerID,	//lControlServerID
															&lCID,				//lPlayerID
															NULL,				//pvLFPInfo
															NULL,				//pvLFMInfo
															pvSearchInfo,		//pvSearchInfo
															NULL				//lResult
														);

			if (pvSearchInfo)
				m_csSearchInfo.FreePacket(pvSearchInfo);

			if( pvPacket != NULL )
			{
				bResult = SendPacket(pvPacket, nPacketLength, ACDP_SEND_RECRUITSERVER);
				
				m_csPacket.FreePacket(pvPacket);
			}
		}
	}

	return bResult;
}

BOOL AgcmRecruit::SendSearchLFMByPage( INT32 lCID, bool bSearchByMyLevel, INT32 lPage, INT32 lClass )
{
	AgpdCharacter	*pcAgpdCharacter;
	BOOL			bResult;

	bResult = FALSE;

	INT8	lOperation = AGPMRECRUIT_SEARCH_LFM_BY_PAGE;
	INT16	nPacketLength;
	INT32	lControlServerID;
	void	*pvSearchInfo;

	pcAgpdCharacter = m_pagpmCharacter->GetCharacter( lCID );

	if( pcAgpdCharacter != NULL )
	{
		INT32			lLevel = -1;

		if( bSearchByMyLevel == true )
		{
			lLevel	= m_pagpmCharacter->GetLevel(pcAgpdCharacter);
		}
		else
		{
			lLevel = 0;
		}

		if( lLevel != -1 )
		{
			//�׽�Ʈ�� �ڵ� �������� �����ش�.
			for( int i=0; i<MAX_RECRUIT_ROW; ++i )
			{
				m_clRecruitWindow.m_clBbsMercenary.SetCellText( i, 0, "" );
				m_clRecruitWindow.m_clBbsMercenary.SetCellText( i, 1, "" );
				m_clRecruitWindow.m_clBbsMercenary.SetCellText( i, 2, "" );
			}

			lControlServerID = GetControlServerID();

			pvSearchInfo = m_csSearchInfo.MakePacket(FALSE, &nPacketLength, AGPMRECRUIT_PACKET_TYPE,
												&bSearchByMyLevel,		//bSearchByMyLevel
												&lLevel,				//Level
												&lPage,					//Page
												&lClass					//lClass
												);


			PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMRECRUIT_PACKET_TYPE,
															&lOperation,		//lOperation
															&lControlServerID,	//lControlServerID
															&lCID,				//lPlayerID
															NULL,				//pvLFPInfo
															NULL,				//pvLFMInfo
															pvSearchInfo,		//pvSearchInfo
															NULL				//lResult
														);

			if (pvSearchInfo)
				m_csSearchInfo.FreePacket(pvSearchInfo);

			if( pvPacket != NULL )
			{
				bResult = SendPacket(pvPacket, nPacketLength, ACDP_SEND_RECRUITSERVER);
				
				m_csPacket.FreePacket(pvPacket);
			}
		}
	}

	return bResult;
}

BOOL AgcmRecruit::ProcessSearchLFPResult( void *pvPlayerInfo )
{
	BOOL				bResult;

	bResult = FALSE;

	if( pvPlayerInfo != NULL )
	{
		INT32				lTotalCount = -1;
		INT32				lIndex = -1;
		INT32				lLevel = -1;
		INT32				lClass = -1;
		
		char				*pstrCharName = NULL;

		m_csLFPInfo.GetField( FALSE, pvPlayerInfo, 0,
							&lTotalCount,
							&lIndex,
							&lLevel,
							&lClass,
							&pstrCharName,
							NULL );

		if( (lTotalCount != -1) && (lIndex != -1) && (lLevel != -1) && (lClass != -1) && (pstrCharName != NULL ) )
		{
			int				iTotalPage;

			if( lTotalCount == 0 )
			{
				iTotalPage = 1;
			}
			else
			{
				iTotalPage = lTotalCount/MAX_RECRUIT_ROW;

				if( (lTotalCount % MAX_RECRUIT_ROW) != 0 )
				{
					iTotalPage++;
				}
			}

			// �Խ��ǿ� ���� �Է� 
			if ( NULL != pstrCharName )
			{
				m_clRecruitWindow.m_clBbsMercenary.SetPageInfo( iTotalPage, 1 );
				m_clRecruitWindow.m_clBbsMercenary.SetCellText( lIndex, 0, pstrCharName );
				char szNumber[128];
				wsprintf( szNumber, "%d", lClass );
				m_clRecruitWindow.m_clBbsMercenary.SetCellText( lIndex, 1, szNumber );
				wsprintf( szNumber, "%d", lLevel );
				m_clRecruitWindow.m_clBbsMercenary.SetCellText( lIndex, 2, szNumber );
			}
			else 
			{
				// pstrCharName�� NULL�̸� �� lIndex����� 
				m_clRecruitWindow.m_clBbsMercenary.ClearCellText_ByRow( lIndex );
			}

			bResult = TRUE;
		}								
	}

	return bResult;
}

BOOL AgcmRecruit::ProcessSearchLFMResult( void *pvPlayerInfo )
{
	BOOL				bResult;

	bResult = FALSE;

	if( pvPlayerInfo != NULL )
	{
		INT32				lTotalCount;
		INT32				lIndex;
		INT32				lRequireMember;
		INT32				lMinLV, lMaxLV, lLevel;
		INT32				lClass;
		INT32				lLeaderID;
		
		char				*pstrPurpose = NULL;
		char				*pstrCharName = NULL;

		m_csLFMInfo.GetField( FALSE, pvPlayerInfo, 0,
							&lTotalCount,
							&lIndex,
							&pstrPurpose,
							&lRequireMember,
							&lMinLV,
							&lMaxLV,
							&lLevel,
							&lClass,
							&pstrCharName,
							&lLeaderID );

		if( (pstrPurpose != NULL ) && (pstrCharName != NULL) )
		{
			int				iTotalPage;

			if( lTotalCount == 0 )
			{
				iTotalPage = 1;
			}
			else
			{
				iTotalPage = lTotalCount/MAX_RECRUIT_ROW;

				if( (lTotalCount % MAX_RECRUIT_ROW) != 0 )
				{
					iTotalPage++;
				}
			}
			
			// �Խ��ǿ� ���� �Է� 
			if ( NULL != pstrCharName )
			{
				m_clRecruitWindow.m_clBbsParty.SetPageInfo( iTotalPage, 1 );
				m_clRecruitWindow.m_clBbsParty.SetCellText( lIndex, 0, pstrCharName );
				char szNumber[128];
				wsprintf( szNumber, "%d", lRequireMember );
				m_clRecruitWindow.m_clBbsParty.SetCellText( lIndex, 1, szNumber );
				wsprintf( szNumber, "%d~%d", lMinLV, lMaxLV );
				m_clRecruitWindow.m_clBbsParty.SetCellText( lIndex, 2, szNumber );
			}
			else 
			{
				// pstrCharName�� NULL�̸� �� lIndex����� 
				m_clRecruitWindow.m_clBbsMercenary.ClearCellText_ByRow( lIndex );
			}

			bResult = TRUE;
		}								
	}

	return bResult;

}

BOOL AgcmRecruit::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	BOOL			bResult;

	INT32			lControlServerID;
	INT8			lOperation;
	INT32			lPlayerID;
	void			*pvLFPInfo;
	void			*pvLFMInfo;
	void			*pvSearchInfo;
	INT32			lResult;

	bResult = FALSE;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&lOperation,
						&lControlServerID,
						&lPlayerID,
						&pvLFPInfo,
						&pvLFMInfo,
						&pvSearchInfo,
						&lResult );

	//LFP�� ����� �Ϸ�Ǿ���.
	if( lOperation == AGPMRECRUIT_REGISTER_LFP )
	{
	}
	//LFP���� ���� �Ϸ�~
	else if( lOperation == AGPMRECRUIT_CANCEL_LFP )
	{
	}
	//LFM�� ����� �Ϸ�Ǿ���.
	else if( lOperation == AGPMRECRUIT_REGISTER_LFM )
	{
	}
	//LFM���� ���� �Ϸ�~
	else if( lOperation == AGPMRECRUIT_CANCEL_LFM )
	{
	}
	//LFP�� �˻� ����� Page������ ���δ�.
	else if( lOperation == AGPMRECRUIT_SEARCH_LFP_BY_PAGE )
	{
		bResult = ProcessSearchLFPResult( pvLFPInfo );
	}
	//LFM�� �˻� ����� Page������ ���δ�.
	else if ( lOperation == AGPMRECRUIT_SEARCH_LFM_BY_PAGE )
	{
		bResult = ProcessSearchLFMResult( pvLFMInfo );
	}
	else if( lOperation == AGPMRECRUIT_RESULT )
	{
		//�˻�
		if( lResult == AGPMRECRUIT_RESULT_NO_SEARCH_RESULT )
		{
			if ( m_clRecruitWindow.pParent )
				g_pEngine->MessageDialog( (m_clRecruitWindow.pParent), "�˻������ �����ϴ�" );
		}
		//���� ����Ʈ ������ ť�� ��â �׿��� ó���� �� ���� ��Ȳ��.
		else if( lResult == AGPMRECRUIT_RESULT_QUEUE_FULL )
		{
			//������ �޽����� ������!! �d�d~
		}
	}
	//���������� ��Ŷ�� �޾Ҵ�. �߸� ���� ���߸��̱�. -_-; �����κ���?
	else
	{
	}

	return bResult;
}

/*****************************************************************
*   Function : OnInit
*   Comment  : On Initialize virtual function 
*   Date&Time : 2003-03-14, ���� 11:09
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
BOOL	AgcmRecruit::OnInit()
{
/*	//�Խ����� �̹��� 
	m_clBbs.AddImage("C:\\board_back.bmp");
	//�Խ��� ���� ������ �̹��� 
	m_clBbs.SetContentWindowImage("C:\\board_content.bmp");
	//�Խ��� ���� ������ ��ư �̹��� 
	m_clBbs.SetContentWindowButtonImage("C:\\board_button.bmp");
	//�Խ����� ũ�� 
	m_clBbs.MoveWindow( 50, 50 , 512, 420 );
	//�Խ��� ���� ������ ũ�� 
	m_clBbs.SetContentWindowRect( 25, 25, 462, 370 );
	//�Խ��� ���� ���忡�� ������ ���� ũ�� 
	m_clBbs.SetContentWindowDrawRect( 30, 30, 402, 402 );

	//�Խ��� �Ӽ� �Է� 
	stAcUIBbsInfo stBbsInfo;
	stBbsInfo.m_cColumnNum = 3;
	stBbsInfo.m_cRowNum = 10;
	stBbsInfo.m_nCellHeight = 25;
	stBbsInfo.m_nColumnGap = 12;
	stBbsInfo.m_nGapColumnCell = 50;
	stBbsInfo.m_nNowPageNum = 1;
	stBbsInfo.m_nAllPageNum = 1;
	stBbsInfo.m_v2dCellStart.x = 10.0f;
	stBbsInfo.m_v2dCellStart.y = 50.0f;
	m_clBbs.InitInfoInput( &stBbsInfo );
	
	//�Խ��� �÷� ���� �Է� 
	m_clBbs.SetColumnInfo( 0, "Index��������������𸮵�", 100, HANFONT_CENTERARRANGE );
	m_clBbs.SetColumnInfo( 1, "Content Title", 300, HANFONT_CENTERARRANGE );
	m_clBbs.SetColumnInfo( 2, "Name", 100 );

	BBSWindowShow( FALSE );

	m_clBbs.SetBbsCallback( ACUIBBS_CLICK_CONTENT, CBBbsClickContent, (PVOID)this );
	m_clBbs.SetBbsCallback( ACUIBBS_CHANGE_PAGE, CBBbsChangePage, (PVOID)this );
	m_clBbs.SetBbsCallback( ACUIBBS_WINDOW_CLOSE, CBBbsWindowClose, (PVOID)this );

	m_clBbs.SetPageInfo( 10, 3 );*/

	m_clRecruitWindow.m_pAgcmRecruit = this;
	m_clRecruitWindow.ChildControlInit();
	m_clRecruitWriteWindow.OnInit();

	return TRUE;	
}

/*****************************************************************
*   CallBack Function : CB-
*   Comment  : BBs���� �޴� Callback �Լ� 
*   Date&Time : 2003-03-14, ���� 5:31
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
BOOL	AgcmRecruit::CBBbsClickContent( PVOID pClass, PVOID pData_1, PVOID pData_2, PVOID pData_3 )
{
	AgcmRecruit* pThis = (AgcmRecruit*)pClass;
	UINT32	lClickedIndex	= *((UINT32*)pData_1);
	UINT8	cClickedRow		= *((UINT8*)pData_2);
	UINT8	cClickedColumn	= *((UINT8*)pData_3);

	//�Խ����� Content�� �����ΰ��� Ŭ���Ǿ����� 

	return TRUE;
}

BOOL	AgcmRecruit::CBBbsChangePage( PVOID pClass, PVOID pData_1, PVOID pData_2, PVOID pData_3 )
{
	AgcmRecruit*	pThis = (AgcmRecruit*)pClass;
	UINT32			lPage	= (UINT32)pData_1;				//�ٲٰ� ���� Page

	pThis->SendSearchLFPByPage( pThis->m_pagcmCharacter->m_pcsSelfCharacter->m_lID, false, lPage, 0 );

	return TRUE;
}

BOOL	AgcmRecruit::CBBbsWindowClose( PVOID pClass, PVOID pData_1, PVOID pData_2, PVOID pData_3 )
{
	AgcmRecruit* pThis = (AgcmRecruit*)pClass;

	return TRUE;
}

/*****************************************************************
*   Function : ClickMercenaryBbsContent
*   Comment  : ClickMercenaryBbsContent
*   Date&Time : 2003-04-28, ���� 9:21
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
void	AgcmRecruit::ClickMercenaryBbsContent( UINT32 lIndex, UINT8 cRow, UINT8 cColumn )
{
	
}

/*****************************************************************
*   Function : ClickPartyBbsContent
*   Comment  : ClickPartyBbsContent
*   Date&Time : 2003-04-28, ���� 9:21
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
void	AgcmRecruit::ClickPartyBbsContent( UINT32 lIndex, UINT8 cRow, UINT8 cColumn )
{
	
}

void	AgcmRecruit::UIClickRegisterLFP()
{
	if ( NULL == m_pagcmCharacter || NULL == m_pagpmCharacter ) return;

	AgpdCharacter* ppdCharacter = m_pagpmCharacter->GetCharacter( m_pagcmCharacter->m_lSelfCID );
	ASSERT( NULL != ppdCharacter );
	if ( NULL == ppdCharacter ) return;

	SendRegisterToLFP( m_pagcmCharacter->m_lSelfCID, ppdCharacter->m_szID );
}

void	AgcmRecruit::UIClickDeleteLFP()
{
	if ( NULL == m_pagcmCharacter || NULL == m_pagpmCharacter ) return;

	AgpdCharacter* ppdCharacter = m_pagpmCharacter->GetCharacter( m_pagcmCharacter->m_lSelfCID );
	ASSERT( NULL != ppdCharacter );
	if ( NULL == ppdCharacter ) return;

	SendCancelFromLFP( m_pagcmCharacter->m_lSelfCID, ppdCharacter->m_szID );
}

void	AgcmRecruit::UIClickRegisterLFM( char *pstrPurpose, INT32 lRequireMember, INT32 lMinLV, INT32 lMaxLV, INT32 lClass )
{
	if ( NULL == m_pagcmCharacter || NULL == m_pagpmCharacter ) return;

	AgpdCharacter* ppdCharacter = m_pagpmCharacter->GetCharacter( m_pagcmCharacter->m_lSelfCID );
	ASSERT( NULL != ppdCharacter );

	SendRegisterToLFM( m_pagcmCharacter->m_lSelfCID, ppdCharacter->m_szID, pstrPurpose, lRequireMember, 
		lMinLV, lMaxLV, lClass );
}

void	AgcmRecruit::UIClickDeleteLFM()
{
	if ( NULL == m_pagcmCharacter || NULL == m_pagpmCharacter ) return;

	AgpdCharacter* ppdCharacter = m_pagpmCharacter->GetCharacter( m_pagcmCharacter->m_lSelfCID );
	ASSERT( NULL != ppdCharacter );
	if ( NULL == ppdCharacter ) return;

	SendCancelFromLFM( m_pagcmCharacter->m_lSelfCID, ppdCharacter->m_szID );
}

void	AgcmRecruit::UILFPSearch( bool bSearchByMyLevel, INT32 lPage, INT32 lClass )
{
	if ( NULL == m_pagcmCharacter ) return;

	m_lSelectedClass	= lClass			;

	SendSearchLFPByPage( m_pagcmCharacter->m_lSelfCID, bSearchByMyLevel, lPage, lClass );
}

void	AgcmRecruit::UILFMSearch( bool bSearchByMyLevel, INT32 lPage, INT32 lClass )
{
	if ( NULL == m_pagcmCharacter ) return;

	m_lSelectedClass	= lClass			;

	SendSearchLFMByPage( m_pagcmCharacter->m_lSelfCID, bSearchByMyLevel, lPage, lClass );
}

void	AgcmRecruit::UILFPPageChange( INT32 lPage )
{
	if ( NULL == m_pagcmCharacter ) return;

	SendSearchLFPByPage( m_pagcmCharacter->m_lSelfCID, m_bSearchByMyLevel, lPage, m_lSelectedClass );
}

void	AgcmRecruit::UILFMPageChange( INT32 lPage )
{
	if ( NULL == m_pagcmCharacter ) return;

	SendSearchLFMByPage( m_pagcmCharacter->m_lSelfCID, m_bSearchByMyLevel, lPage, m_lSelectedClass );
}