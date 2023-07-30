#include "AgsmPatchServer2.h"
#include "ApServerMonitorPacket.h"
#include "AuIniManager.h"

AuCircularBuffer	g_csOutBuffer;


AgsmPatchServer2::AgsmPatchServer2()
{
	SetModuleName("AgsmPatchServer2");

	m_iCurrentUsers = 0;

	g_csOutBuffer.Init(100 * 1024 * 1024);
}

AgsmPatchServer2::~AgsmPatchServer2()
{
}

BOOL AgsmPatchServer2::OnAddModule()
{
	return TRUE;
}


BOOL AgsmPatchServer2::OnInit()
{
	if (!m_csClientPatchInfo.InitializeObject(sizeof(VectorPatchInfo *), 10240))
		return FALSE;

	AgsEngine *pModuleManager = (AgsEngine *) GetModuleManager();
	if (NULL == pModuleManager)
		return FALSE;
	
	return pModuleManager->SetCBPatch(this, DispatchPatch, DisconnectPatch);
}

BOOL AgsmPatchServer2::DispatchPatch(PVOID pvPacket, PVOID pvParam, PVOID pvSocket)
{
	if (NULL == pvPacket || NULL == pvParam || NULL == pvSocket)
		return FALSE;

	AgsmPatchServer2		*pThis = (AgsmPatchServer2 *) pvParam;
	AsServerSocket	*pSocket = (AsServerSocket *) pvSocket;

	return pThis->OnReceive(pvPacket, pSocket->GetIndex());
}


BOOL AgsmPatchServer2::DisconnectPatch(PVOID pvPacket, PVOID pvParam, PVOID pvSocket)
{
	if (NULL == pvParam || NULL == pvSocket)
		return FALSE;

	AgsmPatchServer2		*pThis = (AgsmPatchServer2 *) pvParam;
	AsServerSocket	*pSocket = (AsServerSocket *) pvSocket;

	return pThis->OnDisconnect(pSocket);
}

BOOL AgsmPatchServer2::OnDisconnect(AsServerSocket *pSocket)
{
	UINT32					ulNID = pSocket->GetIndex();

	RemovePatchInfo(ulNID);

	return TRUE;
}

BOOL AgsmPatchServer2::OnReceive(PVOID pvPacket, UINT32 ulNID)
{
	if (NULL == pvPacket || 0 == ulNID)
		return FALSE;

	PPATCH_PACKET_HEADER *	pPacketHeader = (PPATCH_PACKET_HEADER *) pvPacket;
	CHAR *					pcPacket = (CHAR *) pvPacket;

	// Echo Packet
	if( (CHAR) 0xff == pcPacket[4] )
	{
		CPacketTypeCS_0xff *	pcPacketTypeCS_0xff = (CPacketTypeCS_0xff *) pvPacket;
		CPacketTypeSC_0xff *	pcPacketTypeSC_0xff = (CPacketTypeSC_0xff *) g_csOutBuffer.Alloc(sizeof(CPacketTypeSC_0xff));

		pcPacketTypeSC_0xff->Init();

		INT32				iIndex;

		iIndex = 0;
		pcPacketTypeSC_0xff->m_iSeq = pcPacketTypeCS_0xff->m_iSeq;

		SendPacket((PVOID) pcPacketTypeSC_0xff, pcPacketTypeSC_0xff->m_iSize, ulNID);
	}
	// Patch ���� Packet
	else if( 0x00 == pcPacket[4] )
	{
		VectorPatchInfo *		pcPatchFileInfo;
		CPacketTypeCS_0x00 *	pcPacketTypeCS_0x00 = (CPacketTypeCS_0x00 *) pvPacket;

		pcPatchFileInfo = new VectorPatchInfo;

		//�ֽ���ġ������ �޾ƾ��� ������ ���� ������.
		INT32 iResult = GetPatchFiles( pcPacketTypeCS_0x00->m_iVersion, pcPacketTypeCS_0x00->m_iPatchCheckCode, pcPatchFileInfo );

		if( iResult == 0 )
		{
			IterPatchInfo	iter;
			__int64			iTotalSize;

			iTotalSize = 0;

			for (iter = pcPatchFileInfo->begin(); iter != pcPatchFileInfo->end(); ++iter)
			{
				iTotalSize += (*iter)->m_iFileSize;
				iTotalSize += (*iter)->m_iRawFileSize;
			}

			//��ġ ������ ��ġ���� ������ ���� �����Ѵ�.
			CPacketTypeSC_0x01 *	pcPacketTypeSC_0x01 = (CPacketTypeSC_0x01 *) g_csOutBuffer.Alloc(sizeof(CPacketTypeSC_0x01));

			pcPacketTypeSC_0x01->Init();

			INT32				iIndex;

			iIndex = 0;
			pcPacketTypeSC_0x01->m_iRecvFiles = pcPatchFileInfo->size(); //�޾ƾ��ϴ� ������ ��ΰ�?
			pcPacketTypeSC_0x01->m_iVersion = m_iVersion;                    //�÷����ϴ� ������?
			pcPacketTypeSC_0x01->m_iPatchCheckCode =  m_cAuPatchCheckCode.GetCode(m_iVersion);
			pcPacketTypeSC_0x01->m_iTotalSize = iTotalSize;
			memset( pcPacketTypeSC_0x01->m_strServerIP, 0, sizeof(pcPacketTypeSC_0x01->m_strServerIP) );
			strcat( pcPacketTypeSC_0x01->m_strServerIP, GetDownloadServerIP() );
			pcPacketTypeSC_0x01->m_iServerPort = GetDownloadServerPort();

			SendPacket((PVOID) pcPacketTypeSC_0x01, pcPacketTypeSC_0x01->m_iSize, ulNID);

			if (GetPatchInfo(ulNID))
			{
				// ���� ó��
				printf("Already exist PatchInfo NID(%d)...\n", ulNID);
				RemovePatchInfo(ulNID);
			}

			if (!m_csClientPatchInfo.AddObject((PVOID) &pcPatchFileInfo, ulNID))
			{
				printf("Error adding PatchInfo NID(%d)...\n", ulNID);
			}
		}
		else
		{
			INT32				iIndex;

			iIndex = 0;

			CPacketTypeSC_0xEE *	pcPacketTypeSC_0xEE = (CPacketTypeSC_0xEE *) g_csOutBuffer.Alloc(sizeof(CPacketTypeSC_0xEE));

			pcPacketTypeSC_0xEE->Init();
			pcPacketTypeSC_0xEE->m_iErrorCode = iResult;

			SendPacket((PVOID) pcPacketTypeSC_0xEE, pcPacketTypeSC_0xEE->m_iSize, ulNID);
		}
	}
	//��ġ���� ������ �޶�� ��Ŷ�� �޾Ҵٸ�?
	else if( 0x04 == pcPacket[4] )
	{
		if (SendPatchFileInfo(ulNID))
		{
			// ����� �����´�. ���� ó��
		}
		else
		{
			//������ �� ���������� ������ ���´�.
			DestroyClient(ulNID);
		}
	}
	
	return TRUE;
}

BOOL AgsmPatchServer2::LoadDownlodINI(CHAR *szFile)
{
	// Patch Server FTP �ּҸ� �о�´�.
	AuIniManagerA	csIniManager;

	csIniManager.SetPath(szFile);
	if (csIniManager.ReadFile())
	{
		CHAR *			szServerIP	= csIniManager.GetValue("DownloadServer", "IP");
		unsigned short	iServerPort	= csIniManager.GetValueI("DownloadServer", "Port");

		if (szServerIP)
		{
			printf( "Download Server : %s (%d)\n", szServerIP, iServerPort);
			SetDownloadServer(szServerIP, iServerPort);
		}
	}

	return TRUE;
}

CHAR *AgsmPatchServer2::GetDownloadServerIP()
{
	return m_strServerIP;	
}

UINT32 AgsmPatchServer2::GetDownloadServerPort()
{
	return m_iServerPort;	
}

BOOL AgsmPatchServer2::SetDownloadServer( CHAR *pstrServerIP, UINT32 iServerPort )
{
	memset( m_strServerIP, 0, sizeof(m_strServerIP) );
	strcat( m_strServerIP, pstrServerIP );

	m_iServerPort = iServerPort;

	return TRUE;
}

BOOL AgsmPatchServer2::LoadPatchCode( CHAR *pstrFileName )
{
	return m_cAuPatchCheckCode.LoadPatchCode( pstrFileName );
}

INT32 AgsmPatchServer2::GetPatchFiles( INT32 iVersion, UINT32 iPatchCheckCode, VectorPatchInfo *pcList )
{
	INT32				iResult;

	//���� ������ üũ�ڵ尡 �³�����.
	if( m_cAuPatchCheckCode.GetCode( iVersion ) == iPatchCheckCode )
	{
		//iVersion���� ������� ��ġ�� ������. ���� ������������ ������ �ִ��� �˻��ϱ� ���ؼ��� 1�� ������Ų��.
		iVersion++;

		if( iVersion > m_iVersion )
		{
			iResult = g_dErrorNoPatchFile;
		}
		else
		{
			//��·��, ����� ������ ������ ������.
			iResult = 0;

			for( INT32 i=iVersion; i<=m_iVersion; i++ )
			{
				CJZPFileInfo		*pcJZPFileInfo;
				

				if( (pcJZPFileInfo = m_cAuPackingManager.GetJZPInfo(i) ) != NULL )
				{
					pcList->push_back( pcJZPFileInfo );
				}
			}
		}
	}
	else
	{
		iResult = g_dErrorInvalidPatchCode;
	}

	return iResult;
}

AgsmPatchServer2::VectorPatchInfo *	AgsmPatchServer2::GetPatchInfo(UINT32 ulNID)
{
	VectorPatchInfo **	ppcsPatchFileInfo;

	ppcsPatchFileInfo = (VectorPatchInfo **) m_csClientPatchInfo.GetObject(ulNID);
	if (!ppcsPatchFileInfo)
		return NULL;

	return *ppcsPatchFileInfo;
}

BOOL				AgsmPatchServer2::RemovePatchInfo(UINT32 ulNID)
{
	VectorPatchInfo *	pcsPatchFileInfo = GetPatchInfo(ulNID);

	if (!pcsPatchFileInfo)
		return FALSE;

	m_csClientPatchInfo.RemoveObject(ulNID);

	delete pcsPatchFileInfo;

	return TRUE;
}

BOOL AgsmPatchServer2::SendPatchFileInfo(UINT32 ulNID)
{
	VectorPatchInfo *		pcsPatchFileInfo;
	IterPatchInfo			iter;
	CJZPFileInfo *			pcsFileInfo;

	pcsPatchFileInfo = GetPatchInfo(ulNID);
	if (!pcsPatchFileInfo)
		return FALSE;

	iter = pcsPatchFileInfo->begin();
	if (iter == pcsPatchFileInfo->end())
		return FALSE;

	pcsFileInfo = (*iter);
	if (!pcsFileInfo)
		return FALSE;

	//������ �ִ� ������ �ִٸ� ������.
	{
		CHAR			*pstrPacket;

		INT32			iPacketSize;
		INT32			iCurrentPos;
		INT32			iFileNameSize;
		INT32			iFileSize;

		UINT32			lFlag = 0;

		//��Ŷũ�⸦ �ùķ��̼�.
		CPacketTypeSC_0x02	cPacketTypeSC_0x02;

		iPacketSize = 0;
		iCurrentPos = 0;

		//File�̸�ũ�⸦ �����Ѵ�.
		iFileNameSize = pcsFileInfo->m_iFileNameSize;
		//FileSize�� �����Ѵ�.
		iFileSize = pcsFileInfo->m_iFileSize;

		//��Ŷ ���.
		iPacketSize += sizeof( cPacketTypeSC_0x02.m_iSize );
		iPacketSize += sizeof( cPacketTypeSC_0x02.m_iCommand );
		//�����̸� ũ��.
		iPacketSize += sizeof(iFileNameSize);
		//�����̸�
		iPacketSize += iFileNameSize;
		//����ũ���� ������ �߰�.
		iPacketSize += sizeof( iFileSize );
		//CRCCheckSum�� �߰�.
		iPacketSize += sizeof( pcsFileInfo->m_iCRCCheckSum );

		//��Ŷ ����� �ִ´�.
		cPacketTypeSC_0x02.m_iSize = iPacketSize;

		//������ �о ��Ŷ�� �����.
		pstrPacket = (CHAR *) g_csOutBuffer.Alloc(iPacketSize);

		//��Ŷ�������.
		memcpy( &pstrPacket[iCurrentPos], &cPacketTypeSC_0x02.m_iSize, sizeof(cPacketTypeSC_0x02.m_iSize) );
		iCurrentPos+=sizeof(cPacketTypeSC_0x02.m_iSize);
		memcpy( &pstrPacket[iCurrentPos], &cPacketTypeSC_0x02.m_iCommand, sizeof(cPacketTypeSC_0x02.m_iCommand) );
		iCurrentPos+=sizeof(cPacketTypeSC_0x02.m_iCommand);
		//�����̸�ũ�⺹��.
		memcpy( &pstrPacket[iCurrentPos], &iFileNameSize, sizeof(iFileNameSize) );
		iCurrentPos+=sizeof(iFileNameSize);
		//�����̸�����.
		memcpy( &pstrPacket[iCurrentPos], pcsFileInfo->m_pstrFileName, iFileNameSize );
		iCurrentPos+=iFileNameSize;
		//����ũ�⺹��.
		memcpy( &pstrPacket[iCurrentPos], &iFileSize, sizeof(iFileSize) );
		iCurrentPos+=sizeof(iFileSize);
		//CRC CheckSum����
		memcpy( &pstrPacket[iCurrentPos], &pcsFileInfo->m_iCRCCheckSum, sizeof(pcsFileInfo->m_iCRCCheckSum) );
		iCurrentPos+=sizeof(pcsFileInfo->m_iCRCCheckSum);

		// ���� ������ ���� ������
		if( SendRemoveFileInfo( pcsFileInfo->m_iVersion , ulNID ) )
		{
			// Packet ���������� ���� ó��
		}

		// ��ġ�� ������ ������
		if (SendPacket(pstrPacket, iPacketSize, ulNID))
		{
			// Packet ���������� ���� ó��
		}

		//�������� �ѱ��.
		pcsPatchFileInfo->erase(iter);
	}

	// ���̻� ���� �� ������, ����.
	if (!pcsPatchFileInfo->size())
		RemovePatchInfo(ulNID);

	return TRUE;
}

BOOL	AgsmPatchServer2::SendRemoveFileInfo( UINT32 ulPatchVersion , UINT32 ulNID )
{
	CPatchVersionInfo*				pcsPatchInfo	=	m_cAuPackingManager.GetPatchInfo( ulPatchVersion );
	CListNode< CPatchFolderInfo* >* pSourceFolder	=	NULL;
	CListNode< CPackingFile*	>*	pPackingFile	=	NULL;
	CPackingFile*					pFileInfo		=	NULL;
	CHAR*							pstrPacket		=	NULL;
	UINT32							ulCurrentPos	=	0;
	BOOL							bSend			=	FALSE;
	CPacketTypeSC_0x03				cPacketType_03;
	string							strTemp;

	// loop �� ���鼭 ���� ���� ����� �̾Ƴ���
	for( pSourceFolder = pcsPatchInfo->m_clFolderList->GetStartNode() ;	pSourceFolder ;	pSourceFolder = pSourceFolder->m_pcNextNode )
	{
		CPatchFolderInfo*	pFolder		=	pSourceFolder->getData();
		for( pPackingFile = pFolder->m_clFileList->GetStartNode() ;	pPackingFile ; pPackingFile	=	pPackingFile->m_pcNextNode )
		{
			pFileInfo	=	pPackingFile->getData();

			if( pFileInfo->m_lOperation	== Packing_Operation_Remove )
			{
				strTemp	+=	pFolder->m_pstrFolderName;
				strTemp	+=	pFileInfo->m_pstrFileName;
				strTemp +=	"#";

				bSend	=	TRUE;
			}
		}
	}

	// ���� ������ ��Ͽ� �ϳ��� ���ٸ� ��Ŷ�� ������ �ʴ´�
	if( !bSend )
		return FALSE;

	INT nPacketLength = 0;

	nPacketLength +=	sizeof(cPacketType_03.m_iSize);
	nPacketLength +=	sizeof(cPacketType_03.m_iCommand);
	nPacketLength +=	sizeof(cPacketType_03.m_PatchVersion);
	nPacketLength +=	strTemp.size();

	pstrPacket	=	g_csOutBuffer.Alloc( nPacketLength );

	memcpy_s	( &pstrPacket[ ulCurrentPos ] , nPacketLength , &cPacketType_03.m_iSize , sizeof(cPacketType_03.m_iSize ) );
	ulCurrentPos += sizeof(cPacketType_03.m_iSize);
	memcpy_s	( &pstrPacket[ ulCurrentPos ] , nPacketLength , &cPacketType_03.m_iCommand , sizeof(cPacketType_03.m_iCommand) );
	ulCurrentPos += sizeof(cPacketType_03.m_iCommand);
	memcpy_s	( &pstrPacket[ ulCurrentPos ] , nPacketLength , &cPacketType_03.m_PatchVersion , sizeof(cPacketType_03.m_PatchVersion) );
	ulCurrentPos += sizeof(cPacketType_03.m_PatchVersion);

	memcpy_s	( &pstrPacket[ ulCurrentPos ] , strTemp.size() , strTemp.c_str() , strTemp.size() );

	if( !SendPacket	( pstrPacket , nPacketLength , ulNID ) )
	{
		CHAR strDebugMsg[ MAX_DEBUG_MESSAGE ];
		sprintf_s( strDebugMsg , "Sending Error : %s %s %s" , __FILE__ , __LINE__ , __FUNCTION__ );
		OutputDebugString( strDebugMsg );
	}
	
	return TRUE;
}
