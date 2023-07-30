#include "stdafx.h"
#include <Imagehlp.h>
#include <Mmsystem.h>

#include "d3d9.h"

#include "AuPackingManager.h"
#include "AuIniManager.h"
#include "PatchClientLib.h"

#include "ApModule.h"
#include "ApModuleStream.h"
#include "DriverDownDlg.h"
#include ".\patchclientdlg.h"

#include "hyperlinks.h"			//. �ܺ� static text hyperlink �Լ� ���
#include "XmlLogger/XmlLogger.h"

#include "LangControl.h"


#ifdef _JPN
#include "AuJapaneseClientAuth.h"
#include "HanAuthForClient.h"
#endif

#ifdef _KOR
	#include "KoreaPatchClient.h"
#endif

#ifdef _TIW
	#include "TaiwanPatchClient.h"
#endif

#include "zpack/zpack.h"
#include <fstream>

#include "AutoDetectMemoryLeak.h"

#define TEMP_STRING_LENGTH				256
#define GAMSSTRING_LENGTH				4096

#define PATCH_CONFIG_FILE				"Ini\\PatchConfig.ini"
#define PATCH_CONFIG_AREA_URL_SECTION	"AREA"
#define PATCH_CONFIG_AREA_URL_KEY		"URL"
#define PATCH_CONFIG_AREA_REG_SECTION	"REGISTRY"
#define PATCH_CONFIG_AREA_REG_BASE_KEY	"BASE"
#define PATCH_CONFIG_AREA_FILE			"Ini\\PatchArea.ini"
#define PATCH_AREA_TYPE					"Type"
#define PATCH_AREA_CATEGORY				"Category"
#define PATCH_AREA_PATCH_SERVER			"PatchServer"
#define PATCH_AREA_PATCH_PORT			"PatchPort"
#define PATCH_AREA_LOGIN_SERVER			"LoginServer"
#define PATCH_AREA_LOGIN_PORT			"LoginPort"

#define PATCH_AREA_TYPE_NEW				"New"

extern wchar_t *	g_szMessageTitle;
extern CXmlLogger*  g_pLogger;

CPatchClientLib *	AgpdPatchAreaInfo::m_pcsPatchClientLib = NULL;

const string BakName = "archlord.bak";


void AgpdPatchAreaInfo::Clear()
{
	if (m_pszName)
		delete [] m_pszName;

	if (m_pszPatchServer)
		delete [] m_pszPatchServer;

	if (m_pszLoginServer)
		delete [] m_pszLoginServer;

	m_pszName			= NULL;
	m_eType				= TYPE_NORMAL;
	m_lCategory			= 0;
	m_pszPatchServer	= NULL;
	m_lPatchPort		= 0;
	m_pszLoginServer	= NULL;
	m_lLoginPort		= 0;
}

bool AgpdPatchAreaInfo::SetPatchInfo(char *pszName, char *pszPatchServer, int lPatchPort, char *pszLoginServer, int lLoginPort, int lType, int lCategory)
{
	if (!pszName || !pszPatchServer || !pszLoginServer)
		return false;

	Clear();

	m_pszName = new char [strlen(pszName) + 1];
	strcpy(m_pszName, pszName);

	m_eType = (AgpdPatchAreaInfo::Type) lType;

	m_lCategory = lCategory;

	m_pszPatchServer = new char [strlen(pszPatchServer) + 1];
	strcpy(m_pszPatchServer, pszPatchServer);

	m_lPatchPort = lPatchPort;

	m_pszLoginServer = new char [strlen(pszLoginServer) + 1];
	strcpy(m_pszLoginServer, pszLoginServer);

	m_lLoginPort = lLoginPort;

	return true;
}

bool AgpdPatchAreaInfo::SendPingPacket()
{
	if (!m_csSocket.m_hSocket)
	{
		if (!m_csSocket.initConnect( 0x0202, false, m_pszPatchServer, m_lPatchPort ))
		{
			m_lPingTime = -1;

			if( m_csSocket.m_lErrorCode & 1 )
			{
			}
			if( m_csSocket.m_lErrorCode & 2 )
			{
			}
			if( m_csSocket.m_lErrorCode & 4 )
			{
			}
			if( m_csSocket.m_lErrorCode & 8 )
			{
			}
		}
		else
		{
			PulseEvent(m_pcsPatchClientLib->m_hEventForNewSocket);
		}
	}

	if (m_csSocket.m_hSocket)
	{
		int				iPacketSize;
		int				iIndex;
		char			*pstrData;

		iIndex = 0;

		CPacketTypeCS_0xff  cPacketType0xff;

		cPacketType0xff.m_iSeq = ++m_lPingSeq;
		iPacketSize = cPacketType0xff.m_iSize;

		pstrData = new char[iPacketSize];

		memcpy( &pstrData[iIndex], &cPacketType0xff.m_iSize, sizeof(cPacketType0xff.m_iSize) );
		iIndex += sizeof(cPacketType0xff.m_iSize);

		memcpy( &pstrData[iIndex], &cPacketType0xff.m_iCommand, sizeof(cPacketType0xff.m_iCommand) );
		iIndex += sizeof(cPacketType0xff.m_iCommand);

		memcpy( &pstrData[iIndex], &cPacketType0xff.m_iSeq, sizeof(cPacketType0xff.m_iSeq) );
		iIndex += sizeof(cPacketType0xff.m_iSeq);

		if (!m_csSocket.SendPacket( pstrData, iPacketSize ))
		{
			m_lPingTime = -1;
			m_csSocket.cleanupWinsock();
			return false;
		}

		memcpy(m_adwPingSendTime, m_adwPingSendTime + 1, sizeof(DWORD) * (PATCHCLIENT_PING_WAIT_SEQ - 1));
		m_adwPingSendTime[PATCHCLIENT_PING_WAIT_SEQ - 1] = timeGetTime();
	}

	return true;
}

bool AgpdPatchAreaInfo::CalcPingTime(int lSeq)
{
	int lIndex;

	lIndex = PATCHCLIENT_PING_WAIT_SEQ - (m_lPingSeq - lSeq) - 1;
	if (lIndex < 0 || lIndex >= PATCHCLIENT_PING_WAIT_SEQ)
		return false;

	m_lPingTime = timeGetTime() - m_adwPingSendTime[lIndex];

	return true;
}

//============================================================================
CPatchClientLib::CPatchClientLib()
{
	m_cWSABuf.len = 4096;
	m_cWSABuf.buf = new char[m_cWSABuf.len];

	m_pstrBuffer = NULL;

	m_bPatchClient = false;

	m_pstrBuffer = NULL;
	m_iPacketSize = 0;
	m_iRemainPacketLength = 0;

	m_iVersion = 0;
	m_iRecvPatchFiles = -1;
	m_iCurrentRecvedFiles = 0;

	m_lResult = 0;

	memset( m_strPatchFileName, 0, sizeof(m_strPatchFileName) );
	m_PatchFileSize = 0;
	m_iRecvedBlockSize = 0;

	m_iCurrentPatchFiles = 0;
	m_iTotalPatchFiles = 0;

	m_bExit = false;
	m_bExitPing = false;

	m_phParentCWnd = NULL;

	// �ڱ� ��ġ�� bat ���� ��ũ��Ʈ
	m_szBatchFileContent = _T("@ECHO OFF\n");
	m_szBatchFileContent += _T("\n");
	m_szBatchFileContent += _T("IF NOT EXIST Archlord.bak GOTO RUN\n");
	m_szBatchFileContent += _T("GOTO DELETE_FILE\n");
	m_szBatchFileContent += _T("\n");
	m_szBatchFileContent += _T(":DELETE_FILE\n");
	m_szBatchFileContent += _T("IF EXIST Archlord.exe del Archlord.exe\n");	
	m_szBatchFileContent += _T("IF EXIST Archlord.exe GOTO DELETE_FILE\n");	
	m_szBatchFileContent += _T(":END\n");
	m_szBatchFileContent += _T("IF NOT EXIST Archlord.exe rename Archlord.bak Archlord.exe\n");
	m_szBatchFileContent += _T("IF NOT EXIST Archlord.exe GOTO END\n");
	m_szBatchFileContent += _T(":RUN\n");
	m_szBatchFileContent += _T("del SelfPatch.bat\n");

	// �ڱ� �ڽ��� ��ġ����
	StartupSelfPatchProcess();
	
	m_szAreaDownloadURL	= NULL;
	m_pcsAreaInfo		= NULL;
	m_phEvents			= NULL;

	//@{ 2006/04/11 burumal
	m_cCompress.InitMemPool();
	//@}

	//�������� ����ɼ� ������ ���������� ������ �����ϴ� ���丮�� ����.
	char strCurrentDirectory[MAX_PATH] = {0, };
	char strFileName[MAX_PATH] = {0, };
	char strExtName[MAX_PATH] = {0, };
	char strExeName[MAX_PATH] = {0, };

	memset( m_strAlefClientFullPath, 0, sizeof(m_strAlefClientFullPath ) );

#ifdef _DEBUG
	GetModuleFileName(NULL, strCurrentDirectory, sizeof(strCurrentDirectory) );
	_splitpath( strCurrentDirectory, NULL, NULL, strFileName, strExtName );

	GetCurrentDirectory( MAX_PATH, strCurrentDirectory );
	strncpy_s( m_strAlefClientFullPath, 255, strCurrentDirectory, MAX_PATH );
	strcat_s( m_strAlefClientFullPath, 255, "\\" );
	strcat_s( m_strAlefClientFullPath, 255, PATCHCLIENT_GAMECLIENT_FILE );
#else
	wchar_t	strMsg[256];
	char	strModuleFileName[MAX_PATH];
	char	strDrive[MAX_PATH];
	char	strDirectory[MAX_PATH];

	GetModuleFileName(NULL, strModuleFileName, sizeof(strModuleFileName) );
	_splitpath( strModuleFileName, strDrive, strDirectory, strFileName, strExtName );


#ifdef _JPN
	strncat( strDirectory, "j_archlord\\", MAX_PATH );
	_makepath( strCurrentDirectory, strDrive, strDirectory, "", "" );
	_makepath( m_strAlefClientFullPath, strDrive, strDirectory, PATCHCLIENT_GAMECLIENT_FILE, "" );
#else
	_makepath( strCurrentDirectory, strDrive, strDirectory, "", "" );
#endif

	if( 0 == SetCurrentDirectory(strCurrentDirectory) )
	{
	#ifdef _JPN
		m_cPatchReg.BuildRegistry(REG_KEY_NAME_ARCHLORD, true);
		CreateDirectory( strCurrentDirectory, NULL );
	#endif

		if( 0 == SetCurrentDirectory(strCurrentDirectory) )
		{
			::LoadStringW(g_resInst, IDS_ERROR_SET_DIRECTORY, strMsg, 256);
			::MessageBoxW(m_phParentCWnd->m_hWnd, strMsg, g_szMessageTitle, MB_OK );
		}
	}
#endif // _DEBUG

	//m_cPatchReg.BuildPathRegistry( g_strRegKeyBase );

	m_exeName = string(strFileName) + strExtName;

	AgpdPatchAreaInfo::m_pcsPatchClientLib = this;

	m_hEventForNewSocket = CreateEvent(NULL, FALSE, TRUE, "AlefPatch");

	//. scan file crc.
	ScanCrackedFiles( strCurrentDirectory );


	//-----------------------------------------------------------------------
	// ��쿡 ���� ��ġ�� ��ŵ�� �� �ְ� �մϴ�.

	FILE * f = fopen( "SkipPatch.arc", "rb" );

	if( f )
	{
		m_bExit = true;
		fclose(f);
	}

	//-----------------------------------------------------------------------
}

CPatchClientLib::~CPatchClientLib()
{
	if( m_cWSABuf.buf != NULL )
	{
		delete [] m_cWSABuf.buf;
		m_cWSABuf.len = 0;
		m_cWSABuf.buf = NULL;
	}

	if( m_pstrBuffer != NULL && (m_iRemainPacketLength != 0) )
	{
		delete [] m_pstrBuffer;
		m_iPacketSize = 0;
		m_iRemainPacketLength = 0;
		m_pstrBuffer = NULL;
	}

	if (m_szAreaDownloadURL)
	{
		delete [] m_szAreaDownloadURL;
		m_szAreaDownloadURL = NULL;
	}

	m_csPatchFileList.Destroy();

	CloseHandle( m_hEventForNewSocket );
}

bool CPatchClientLib::LoadReferenceFile()
{
	return m_csAuPackingManager.LoadReferenceFile( NULL, false, true, true );
}

bool CPatchClientLib::HaveCrackedFolder()
{
	return m_csAuPackingManager.GetCrackedFolderInfo()->GetObjectCount() != 0;
}

ApAdmin *CPatchClientLib::GetCrackedFolderInfo()
{
	return m_csAuPackingManager.GetCrackedFolderInfo();
}

bool CPatchClientLib::IsPatchClient()
{
	return m_bPatchClient;
}

bool CPatchClientLib::SetTempDir( char *pstrTempDir )
{
	strncpy_s( m_strTempDir, 256, pstrTempDir, _TRUNCATE );

	bool bResult = true;
	if( !CreateDirectory( m_strTempDir, NULL ) )
		bResult = (GetLastError() == 183);

	return bResult;
}

bool CPatchClientLib::SendCurrentVersion( int iVersion, int iPatchCheckCode )
{
	int		iIndex = 0;

	CPacketTypeCS_0x00  cPacketType0x00;

	int iPacketSize = cPacketType0x00.m_iSize;
	char* pstrData  = new char[iPacketSize];

	memcpy( &pstrData[iIndex], &cPacketType0x00.m_iSize, sizeof(cPacketType0x00.m_iSize) );
	iIndex += sizeof(cPacketType0x00.m_iSize);

	memcpy( &pstrData[iIndex], &cPacketType0x00.m_iCommand, sizeof(cPacketType0x00.m_iCommand) );
	iIndex += sizeof(cPacketType0x00.m_iCommand);

	memcpy( &pstrData[iIndex], &iVersion, sizeof(iVersion) );
	iIndex += sizeof(iVersion);

	memcpy( &pstrData[iIndex], &iPatchCheckCode, sizeof(iPatchCheckCode) );
	iIndex += sizeof(iPatchCheckCode);

	return SendPacket( pstrData, iPacketSize );
}

bool CPatchClientLib::SendRequestFile()
{
	int				    iIndex = 0;
	CPacketTypeCS_0x04  cPacketType0x04;

	int iPacketSize = cPacketType0x04.m_iSize;
	char* pstrData  = new char[iPacketSize];

	memcpy( &pstrData[iIndex], &cPacketType0x04.m_iSize, sizeof(cPacketType0x04.m_iSize) );
	iIndex += sizeof(cPacketType0x04.m_iSize);

	memcpy( &pstrData[iIndex], &cPacketType0x04.m_iCommand, sizeof(cPacketType0x04.m_iCommand) );
	iIndex += sizeof(cPacketType0x04.m_iCommand);

	bool bResult = SendPacket( pstrData, iPacketSize );
	m_iStartTime = timeGetTime();

	return bResult;
}

bool CPatchClientLib::SendFileBlock()
{
	int				iIndex = 0;

	CPacketTypeCS_0x11  cPacketType0x11;

	int   iPacketSize = cPacketType0x11.m_iSize;
	char* pstrData    = new char[iPacketSize];

	memcpy( &pstrData[iIndex], &cPacketType0x11.m_iSize, sizeof(cPacketType0x11.m_iSize) );
	iIndex += sizeof(cPacketType0x11.m_iSize);

	memcpy( &pstrData[iIndex], &cPacketType0x11.m_iCommand, sizeof(cPacketType0x11.m_iCommand) );
	iIndex += sizeof(cPacketType0x11.m_iCommand);

	return SendPacket( pstrData, iPacketSize );
}

void CPatchClientLib::SetDialogInfo( CWnd *phParentCWnd, CCustomProgressCtrl *pcCustomProgressRecvFile, CCustomProgressCtrl *pcCustomProgressRecvBlock, CStatic *pcStaticStatus, CStatic *pcProgressPercent, CStatic *pcStaticDetailInfo )
{
	m_phParentCWnd				= phParentCWnd;
	m_pcCustomProgressRecvFile	= pcCustomProgressRecvFile;
	m_pcCustomProgressRecvBlock = pcCustomProgressRecvBlock;
	m_pcStaticStatus			= pcStaticStatus;
	m_pcStaticDetailInfo		= pcStaticDetailInfo;

	if(!m_cAuPatchCheckCode.LoadPatchCode("alefpatch.dat"))
	{
		m_phParentCWnd->MessageBox( "cannot find 'alefpatch.dat' file!!" );	
		exit( 1 );
	}
}

void CPatchClientLib::InOnesJZPProcess(char* pszFileName, char* pszFullPathFileName, bool* pbSelfPatch , bool bIsMNP)
{
	//. jzp file in "Temp\\MNP" folder.
	GetPatchFiles( bIsMNP ? "Temp\\MNP" : "Temp", pszFileName);

	// �ٿ���� ��ġ������ ������ Ǯ�� ��ŷ
	bool bResult = DecompressSingleJZP(pszFileName);		//. in ones jzp file decompress

	if( bResult )
	{
		int nOldVersion = m_cPatchReg.GetVersion();

		char pPatchVersion[8] = {0, };
		char* pStrVersion = strstr(_strlwr(pszFileName), ".jzp");

		ASSERT(pStrVersion);

		memcpy(pPatchVersion, pStrVersion - 4, 4);
		int nFinishedPatchVersion = atoi(pPatchVersion);

		#ifndef USE_ZPACK
		{
			// ������Ʈ���� ���� ����
			m_cPatchReg.FinishPatch( nFinishedPatchVersion, m_cAuPatchCheckCode.GetCode(nFinishedPatchVersion));

			if ( g_pLogger )
			{
				char szLog[256];
				sprintf(szLog, "Completed! Version (%d) --> (%d)", nOldVersion, nFinishedPatchVersion);
				g_pLogger->Log(szLog, pszFileName);
				g_pLogger->Log("\n\n");
			}
		}
		#else
		{
			lastPatchVersion_ = nFinishedPatchVersion;
		}
		#endif
	}

	//. if jzp decompress status is end, then delete jzp file.
	DeleteFile( _T( pszFullPathFileName ) );

	if ( g_pLogger )
		g_pLogger->Flush();

	// Self Patch�� �ƴ�
	if( m_bPatchClient )
		*pbSelfPatch		=	true;

}


bool CPatchClientLib::DecompressSingleJZP(char* pszFileName)
{
	if( !pszFileName )		return false;

	char strFilePath[TEMP_STRING_LENGTH];
	sprintf_s(strFilePath, TEMP_STRING_LENGTH , "MNP\\%s", pszFileName);
	
#ifdef USE_ZPACK
	if( !PackingToZpack( "Temp", strFilePath ) )
	{
		sprintf_s(strFilePath, TEMP_STRING_LENGTH , "%s", pszFileName);
		return PackingToZpack( "Temp", strFilePath );
	}
#else
	if ( !DecompressJZPEx( "Temp", strFilePath ) )
	{
		sprintf_s(strFilePath, TEMP_STRING_LENGTH , "%s", pszFileName);
		return DecompressJZPEx( "Temp", strFilePath );
	}
#endif

	return true;
}

void CPatchClientLib::ForceExitProgram()
{
	//. ������ �������Ϸ� �����Ų��.
	//. create batch file
	FILE* _pFile = fopen("SelfPatch.bat", "w+");
	if(_pFile)
	{
		fprintf(_pFile, "%s", (LPCTSTR)m_szBatchFileContent);
		fclose(_pFile);
	}

	//. excute batch file
	ShellExecute(NULL, _T("open"), _T("SelfPatch.bat"), NULL, NULL, SW_HIDE);

}

bool CPatchClientLib::ExecuteIOLoop( bool bLogin )
{
#ifdef USE_ZPACK
	patchFiles_.clear();
	progressMax_ = 0;
	progressCount_ = 0;
	lastPatchVersion_ = 0;
#endif

	wchar_t	strMsg[TEMP_STRING_LENGTH];

	bool bResult		= true;
	bool bSelfPatch		= false;

	while( !m_bExit )
	{
		#ifdef USE_ZPACK
		if( bSelfPatch )
			break;
		#endif

		DWORD dwReturn = WSAWaitForMultipleEvents(1, &m_hRecvEvent, FALSE, WSA_INFINITE, FALSE);
		if (m_bExit)
			break;

		dwReturn = WSAEnumNetworkEvents( m_hSocket, m_hRecvEvent, &m_event);

		if( dwReturn == -1 || (m_event.lNetworkEvents & FD_CLOSE) == FD_CLOSE )
		{
			//��ġ�� ������ ���°��.
			if( m_lResult == g_lAlreadyPatched )
			{
				bResult = false;

				m_phParentCWnd->InvalidateRect( m_pcCustomProgressRecvBlock->GetRect(), FALSE );		
				InvalidateStatic( m_pcStaticDetailInfo, strMsg );
 
				::LoadStringW(g_resInst, IDS_MSG_COMPLETE, strMsg, TEMP_STRING_LENGTH);
				m_phParentCWnd->InvalidateRect( m_pcCustomProgressRecvFile->GetRect(), FALSE );	
				InvalidateStatic( m_pcStaticStatus, strMsg );
			}
			else if( m_lResult == g_lNotEnoughDiskSpace )
			{
				bResult = false;
			}
			else		//��ġ�� ������ �ִ°��.
			{
				CListNode<AgpdPatchFileInfo *>		*pcNode;
				AgpdPatchFileInfo					*pcsPatchFileInfo;

				char					*pstrFileName;
				char					strFilePath		[TEMP_STRING_LENGTH];
				char					strMNPFilePath	[TEMP_STRING_LENGTH];
				char					strURL			[TEMP_STRING_LENGTH];
				char					strHeader		[TEMP_STRING_LENGTH];
				UINT32					lTotalRecvByte;
				INT32					lBufferSize		= 4096; 
				FILE *					fp				= NULL;

				char* pstrData = new char[lBufferSize];

				for( pcNode = m_csPatchFileList.GetStartNode(); pcNode; pcNode=pcNode->m_pcNextNode )
				{
					// Self ��ġ�� �ƴٸ� ����
					if( bSelfPatch )
					{
						#ifdef USE_ZPACK
						bResult = true;
						break;
						#else
						return true;
						#endif
					}

					pcsPatchFileInfo = pcNode->getData();
					pstrFileName = pcsPatchFileInfo->m_pstrFileName;

					lTotalRecvByte = 0;

					// URL �����
					sprintf_s( strURL, TEMP_STRING_LENGTH , "%s%s", m_strDownloadServerIP, pstrFileName );

					// Manual Patch ���� �̸� ����� �α� (���߿� �� �����̸����� �˻�)
					memset( strMNPFilePath, 0, sizeof(strMNPFilePath) );
					sprintf_s( strMNPFilePath, TEMP_STRING_LENGTH , "Temp\\MNP\\%s", pstrFileName );

					// ��ġ ���� �̸� ����� �α�
					memset( strFilePath, 0, sizeof(strFilePath) );
					sprintf_s( strFilePath, TEMP_STRING_LENGTH , "Temp\\%s", pstrFileName );

					fp = fopen(strMNPFilePath, "rb");
					if( fp )
					{
						fclose(fp);

						++m_iCurrentRecvedFiles;

						InOnesJZPProcess( pstrFileName, strMNPFilePath, &bSelfPatch , true );

						continue;
					}

					// ���� ������ �޴� ������ ������, �̾�ޱ⸦ ���� ���ؼ� Header�� ���� ����.
					strHeader[0] = 0;
					fp	= fopen(strFilePath, "rb");
					if (fp)
					{
						CStringW msg;
						msg.LoadString(IDS_CONTINUE_DOWNLOAD);

						// ������ �޴� ������ ������, �̾�ޱ⸦ ���� ����
						if (::MessageBoxW(m_phParentCWnd->m_hWnd, msg, g_szMessageTitle, MB_YESNO) == IDYES)
						{
							fseek(fp, 0, SEEK_END);
							lTotalRecvByte	= ftell(fp);

							sprintf_s(strHeader, TEMP_STRING_LENGTH , "Range:bytes=%u-\n", lTotalRecvByte);
						}

						fclose(fp);
					}

					if( lTotalRecvByte >= pcNode->m_tData->m_iFileSize )
					{
						++m_iCurrentRecvedFiles;
						InOnesJZPProcess( pstrFileName, strFilePath , &bSelfPatch );
						continue;
					}

					if ( DownloadFile( strURL, strFilePath, strHeader, lTotalRecvByte, pcsPatchFileInfo ) )		// URL���� File�� download �޴´�.
						InOnesJZPProcess( pstrFileName, strFilePath , &bSelfPatch );
				}

				delete pstrData;

				//�����κ��� ������ ���� ���ϼ��� �޾ƾ��ϴ� ���ϼ��� �³�?
				if( m_iRecvPatchFiles == m_iCurrentRecvedFiles )
				{
					m_phParentCWnd->InvalidateRect( m_pcCustomProgressRecvFile->GetRect(), FALSE );

					RemoveDir( "Temp" );
					m_phParentCWnd->InvalidateRect( m_pcCustomProgressRecvBlock->GetRect(), FALSE );
					InvalidateStatic( m_pcStaticDetailInfo, strMsg );

					::LoadStringW(g_resInst, IDS_MSG_COMPLETE, strMsg, TEMP_STRING_LENGTH);
					m_phParentCWnd->InvalidateRect( m_pcCustomProgressRecvFile->GetRect(), FALSE );
					InvalidateStatic( m_pcStaticStatus, strMsg );
					bResult = true;
				}
				else
				{
					#ifdef USE_ZPACK
					if( bSelfPatch )
						break;
					#endif
					::LoadStringW(g_resInst, IDS_DISCONNECTED, strMsg, TEMP_STRING_LENGTH);
					InvalidateStatic( m_pcStaticStatus, strMsg );
					MessageBoxW( 0, strMsg, L"PatchClient Error", 0 );
					bResult = false;
				}
			}
			break;
		}
		else if( (m_event.lNetworkEvents & FD_CONNECT) == FD_CONNECT )
		{
			if( !bLogin )
			{
				::LoadStringW(g_resInst, IDS_CHECK_VERSION, strMsg, TEMP_STRING_LENGTH);
				InvalidateStatic( m_pcStaticStatus, strMsg );

				//���ӿ� �����ϸ� ������ ���� ��ġ ������ ������.
				if( SendCurrentVersion( m_cPatchReg.GetVersion(), m_cPatchReg.GetPatchCheckCode() ) == false )
				{
					::LoadStringW(g_resInst, IDS_VERSION_ERROR, strMsg, TEMP_STRING_LENGTH);
					::MessageBoxW(m_phParentCWnd->m_hWnd, strMsg, g_szMessageTitle, MB_OK);

					bResult = false;
					break;
				}
			}
		}
		else if( (m_event.lNetworkEvents & FD_READ) == FD_READ )
		{
			//��Ŷ�� �޾Ƽ� Parsing�Ѵ�.
			int				iTransferedBytes;

			iTransferedBytes = recv( m_hSocket, m_cWSABuf.buf, m_cWSABuf.len, 0 );

			if( iTransferedBytes > 0 )
			{
				bResult = ExtractPacket( iTransferedBytes );
				if( !bResult && m_lResult != g_lAlreadyPatched )
				{
					MessageBoxW( 0, L"��Ŷ�� �ջ�Ǿ����ϴ�", L"��ġ ����", 0 );
					break;
				}
			}
			else
			{
				if( iTransferedBytes == 0 )
				{
				}
				else if( iTransferedBytes == SOCKET_ERROR )
				{
					MessageBoxA( 0, "Failed Recv", "Socket Error", 0 );
				}
			}
		}
	}

#ifdef USE_ZPACK
	if( bResult )
		bResult = CompressPatchFiles();

	if( bResult && lastPatchVersion_ > 0 )
		m_cPatchReg.FinishPatch( lastPatchVersion_, m_cAuPatchCheckCode.GetCode(lastPatchVersion_));
#endif

	return bResult;
}

#ifdef USE_ZPACK

static void decryptFile( std::string src )
{
	std::string srcName = src+"tmp";
	std::string destName = src;

	std::ifstream ifs;
	std::ofstream ofs;
	ifs.open( srcName.c_str(), std::ios::in | std::ios::binary );
	ofs.open( destName.c_str(), std::ios::out | std::ios::binary );

	char c;

	char len = 0;

	ifs.seekg( 0, std::ios::end );
	len = (char)ifs.tellg();
	ifs.seekg( 0, std::ios::beg );

	while( ifs.get(c) )
	{
		c = ~c;
		c ^= len;
		ofs.put(c);
	}

	ifs.close();
	ofs.close();

	try { DeleteFileA( srcName.c_str() ); }
	catch(...) {}
}

bool CPatchClientLib::CompressPatchFiles()
{
	bool result = true;

	for( PatchFiles::iterator iter = patchFiles_.begin(); iter != patchFiles_.end(); )
	{
		std::wstring packName = iter->first;

		typedef std::pair< PatchFiles::iterator, PatchFiles::iterator > PackingGroup;
		
		PackingGroup group = patchFiles_.equal_range( packName );

		std::wstring arg;

		int fileCount = 0;

		for( iter = group.first; iter != group.second; ++iter )
		{
			arg += (arg.empty() ? L"" : L":");
			arg += iter->second;
			++fileCount;

			decryptFile( lexical_cast<std::string>( iter->second ) );
		}		

		RefreshProgressBar1( packName );

		if( !arg.empty() )
			result = zpack::add( packName.c_str(), arg.c_str(), 1, zpack::getPassword().c_str() );

		progressCount_ += fileCount-1;

		for( iter = group.first; iter != group.second; ++iter )
		{
			try { DeleteFileW( iter->second.c_str() ); }
			catch(...) {}
		}

		if( !result )
			break;
	}

	return result;
}
#endif

bool CPatchClientLib::DownloadFile(char* pszURL, char* pszFile, char* pszHeader, int lTotalRecvByte, AgpdPatchFileInfo *pcsPatchFileInfo)
{
	UINT32					lLastUpdateTime;
	INT32					lRefreshTime;
	wchar_t					strMsg[TEMP_STRING_LENGTH];

	bool					bResult = true;
	CInternetSession		cSession;
	CInternetFile *			pcsInternetFile;
	CHttpFile *				pcsHTTPFile = NULL;
	FILE *					fp	= NULL;
	char					*pstrData;
	wchar_t					strBuffer[TEMP_STRING_LENGTH];
	wchar_t					pstrFileNameW[TEMP_STRING_LENGTH];
	INT32					lBufferSize;
	INT32					lReadByte;
	DWORD					dwStatusCode;
	CString					strServer;
	CString					strObject;
	INTERNET_PORT			lPort;
	DWORD					dwServiceType = 0;

	// ȭ�鿡 �ѷ��� File �̸� Unicode�� �����
	if (pcsPatchFileInfo)
	{
		MultiByteToWideChar( CP_ACP, 0, pcsPatchFileInfo->m_pstrFileName, (int) strlen(pcsPatchFileInfo->m_pstrFileName) + 1, pstrFileNameW, TEMP_STRING_LENGTH);
	}

	try
	{
		cSession.SetOption(INTERNET_OPTION_CONNECT_RETRIES, 3);
		pcsInternetFile = (CInternetFile *)cSession.OpenURL( pszURL,
															 1,
															 INTERNET_FLAG_TRANSFER_BINARY | INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE,
															 pszHeader,
															 pszHeader ? (int) strlen(pszHeader) : 0);
	}
	catch (CInternetException *pcsException)
	{
		CHAR	szError[128];

		pcsException->GetErrorMessage(szError, 127);
		AfxMessageBox(szError, MB_OK);

		return false;
	}

	if( !pcsInternetFile )
	{
		CString	csMsg;

		cSession.Close();

		csMsg.LoadString(IDS_ERROR_DOWNLOAD);
		AfxMessageBox(csMsg, MB_OK);

		return false;
	}

	lLastUpdateTime = timeGetTime();
	lRefreshTime = 500;

	if( AfxParseURL(pszURL, dwServiceType, strServer, strObject, lPort) && (dwServiceType == AFX_INET_SERVICE_HTTP || dwServiceType == AFX_INET_SERVICE_HTTPS) )
	{
		pcsHTTPFile = (CHttpFile *) pcsInternetFile;
		pcsHTTPFile->QueryInfoStatusCode(dwStatusCode);

		if (dwStatusCode < 200 || dwStatusCode >= 300)
		{
			CString	csMsg;

			pcsInternetFile->Close();
			cSession.Close();

			csMsg.LoadString(IDS_ERROR_DOWNLOAD);
			AfxMessageBox(csMsg, MB_OK);

			return false;
		}
	}

	lBufferSize = 4096;
	pstrData = new char[lBufferSize];

	pcsInternetFile->SetReadBufferSize( lBufferSize );

	if (lTotalRecvByte)
		fp = fopen( pszFile, "ab" );
	else
		fp = fopen( pszFile, "wb" );

	if (pcsPatchFileInfo)
	{
		m_pcCustomProgressRecvBlock->SetMaxCount( pcsPatchFileInfo->m_iFileSize );
		m_pcCustomProgressRecvBlock->SetCurrentCount(0);

		::LoadStringW(g_resInst, IDS_MSG_DOWNLOAD, strMsg, TEMP_STRING_LENGTH);
		swprintf( strBuffer, strMsg, m_iCurrentRecvedFiles+1, m_iRecvPatchFiles, pstrFileNameW );

		m_phParentCWnd->InvalidateRect(m_pcCustomProgressRecvBlock->GetRect(), FALSE );
		InvalidateStatic( m_pcStaticStatus, strBuffer );

        //@{ 2006/04/19
		m_pcCustomProgressRecvFile->SetMaxCount( m_iRecvPatchFiles );
		m_pcCustomProgressRecvFile->SetCurrentCount( m_iCurrentRecvedFiles + 1);

		if (m_phParentCWnd->m_hWnd)
			m_phParentCWnd->InvalidateRect( m_pcCustomProgressRecvFile->GetRect(), FALSE );
		//@}
	}

	if( fp )
	{
		for(;!m_bExit;)
		{
			bool			bRead = true;

			try
			{
				lReadByte = pcsInternetFile->Read( pstrData, lBufferSize );
			}
			catch( CInternetException *pEx )
			{
				UNREFERENCED_PARAMETER(pEx);
				Sleep(100);
				bRead = false;
			}

			if( bRead == false )
				continue;

			lTotalRecvByte+=lReadByte;

			if( lReadByte != 0 )
			{
				int				lWriteBytes;

				lWriteBytes = (int) fwrite( pstrData, 1, lReadByte, fp );

				if (pcsPatchFileInfo)
				{
					m_pcCustomProgressRecvBlock->SetCurrentCount( lTotalRecvByte );

					if( lLastUpdateTime < timeGetTime() )
					{
						lLastUpdateTime = timeGetTime()+lRefreshTime;

						::LoadStringW(g_resInst, IDS_MSG_PROGRESS, strMsg, TEMP_STRING_LENGTH);
						swprintf( strBuffer, strMsg, (float)lTotalRecvByte/(1024 * 1024), (float)pcsPatchFileInfo->m_iFileSize/(1024 * 1024) );
						InvalidateStatic( m_pcStaticDetailInfo, strBuffer );

						if (m_phParentCWnd->m_hWnd)
							m_phParentCWnd->InvalidateRect(m_pcCustomProgressRecvBlock->GetRect(), FALSE );
					}
				}

				if( lWriteBytes != lReadByte )
				{
					fclose( fp );
					pcsInternetFile->Close();
					m_lResult = g_lNotEnoughDiskSpace;
					bResult = false;
					break;
				}
			}
			else
			{
				if( pcsPatchFileInfo && pcsPatchFileInfo->m_iFileSize == lTotalRecvByte )
				{
					m_iCurrentRecvedFiles++;
					m_pcCustomProgressRecvFile->SetMaxCount( m_iRecvPatchFiles );
					m_pcCustomProgressRecvFile->SetCurrentCount( m_iCurrentRecvedFiles );

					if (m_phParentCWnd->m_hWnd)
						m_phParentCWnd->InvalidateRect( m_pcCustomProgressRecvFile->GetRect(), FALSE );
				}

				break;
			}
		}

		fclose( fp );
	}

	pcsInternetFile->Close();

	cSession.Close();

	delete [] pstrData;

	return bResult;
}

bool CPatchClientLib::ExtractPacket( int iTransferedBytes, AgpdPatchAreaInfo *pcsArea )
{
	bool			bResult;
	int				iPacketSize;
	int				iRemainPacketLength;
	int				iPacketHeaderSize;
	int				iOffset;
	WSABUF *		pcsWSABuf = &m_cWSABuf;

    if (pcsArea)
		pcsWSABuf = &pcsArea->m_cWSABuf;

	bResult = true;
	iOffset = 0;
	iPacketHeaderSize = 5;
	iRemainPacketLength = m_iRemainPacketLength;

	while( 1 )
	{
		if( (iRemainPacketLength + iTransferedBytes) >= iPacketHeaderSize )
		{
			//�ƹ��� ����.
		}
		else
		{
			if( iTransferedBytes == 0 )
			{
				//��Ȯ�� ��Ŷ�� �Ľ̵Ǿ� ������.
				m_iRemainPacketLength = 0;
			}
			else
			{
				//������ �ִ°��. �̷��� ������ �������� �����մϴٿ�~ ȣȪ~
			}

			break;
		}

		//������� �Դٸ� ��Ŷ�������Ÿ���� �ּ��� �� �� �ִٴ� ���̴�. ��ȣȣȣ~~
		//��Ŷ�� ũ�⸦ ������~
		if( iRemainPacketLength == 0 )
		{
			memcpy( &iPacketSize, &pcsWSABuf->buf[iOffset], sizeof( iPacketSize) );
		}
		else
		{
			memcpy( &iPacketSize, m_pstrBuffer, sizeof( iPacketSize) );
		}

		//iPacketSize�� �˻��ϴ� ������ �ʿ��Ѵ�.

		//���� �ϳ��� ��Ŷ ������ �ϼ��� �� �ִٸ�?
		if( (iRemainPacketLength + iTransferedBytes) >= iPacketSize )
		{
			char			*pstrPacket;
			int				iCopySize;

			iCopySize = iPacketSize-iRemainPacketLength;

			//��Ŷ�� Queue�� �ִ´�.
			pstrPacket = new char[iPacketSize];

			//Packet�� ���� ������ �����س���.
			if( iRemainPacketLength == 0 )
			{
				//���� ���ۿ��� �ٷ� ������ ������.
				memcpy( pstrPacket, &pcsWSABuf->buf[iOffset], iPacketSize );
			}
			else
			{
				//������ �޴��������� ������ m_strBuffer���� �����ϰ� �������� m_cWSABuf.buf���� �����Ѵ�.
				memcpy( pstrPacket, m_pstrBuffer, iRemainPacketLength );
				memcpy( &pstrPacket[iRemainPacketLength], &pcsWSABuf->buf[iOffset], iCopySize );

				delete [] m_pstrBuffer;
				m_pstrBuffer = NULL;
				iRemainPacketLength = 0;				
			}

			bResult = ParsePacket( pstrPacket, pcsArea );

			if( bResult == false )
			{
				//���� ������ ����!!
				break;
			}
			else
			{
				iRemainPacketLength = 0;
				iOffset += iCopySize;
				iTransferedBytes -= iCopySize;
			}
		}
		//�ϳ��� ��Ŷ ������ �ϼ��ϱ⿣ ���� ��Ŷ�� �۴ٸ�?
		else
		{
			//�׳� ������� �����ع�����. �׷� ������ �˾Ƽ� ó���ȴ�~ ��ȣȣȣ~
			if( m_pstrBuffer == NULL )
				m_pstrBuffer = new char[iPacketSize];

			memcpy( &m_pstrBuffer[iRemainPacketLength], &pcsWSABuf->buf[iOffset], iTransferedBytes );
			m_iRemainPacketLength = iRemainPacketLength + iTransferedBytes;

			break;
		}
	}

	return bResult;
}

bool CPatchClientLib::ParsePacket( char *pstrPacket, AgpdPatchAreaInfo *pcsArea )
{
	bool			bResult;
	unsigned int	iPacketSize;
	unsigned char	iCommand;

	bResult = true;

	memcpy( &iPacketSize, &pstrPacket[0], sizeof( iPacketSize ) );
	memcpy( &iCommand, &pstrPacket[4], sizeof( iCommand ) );

	//��ġ �����Ǽ�, ������ �޴´�.
	if( iCommand == 0x01 )
	{
		bResult = GetPatchInfo( pstrPacket );

		if( !bResult )
		{
			MessageBox( 0, "������ ���� ��ġ ���� �ޱ⿡ �����߽��ϴ�.", "��ġ ����", 0 );
		}
	}
	//������ �޴´�.
	else if( iCommand == 0x02 )
	{
		if( GetPatchFile( pstrPacket ) == true )
		{
			SendRequestFile();
		}
	}
	
	else if( iCommand == 0x03 )
	{
		// ���� ���ϵ��� �����
		RemoveFile( pstrPacket );
		
	}
	else if( iCommand == 0x05 )
	{
		if( m_phParentCWnd )
		{
			( ( CPatchClientDlg* )m_phParentCWnd )->OnReceiveClientFileCRC( pstrPacket );
		}
	}
	//��ġ�� ���� �߻�.
	else if( iCommand == 0xEE )
	{
		CPacketTypeSC_0xEE	cPacketTypeSC_0xEE;
		int				iIndex;

		iIndex = 0;

		memcpy( &cPacketTypeSC_0xEE.m_iCommand, &pstrPacket[iIndex], sizeof(cPacketTypeSC_0xEE.m_iCommand) );
		iIndex += sizeof(cPacketTypeSC_0xEE.m_iCommand);

		memcpy( &cPacketTypeSC_0xEE.m_iSize, &pstrPacket[iIndex], sizeof(cPacketTypeSC_0xEE.m_iSize) );
		iIndex += sizeof(cPacketTypeSC_0xEE.m_iSize);

		memcpy( &cPacketTypeSC_0xEE.m_iErrorCode, &pstrPacket[iIndex], sizeof(cPacketTypeSC_0xEE.m_iErrorCode) );
		iIndex += sizeof(cPacketTypeSC_0xEE.m_iErrorCode);

		//��ġ�Ұ� ��� �׳� ������.
		if( cPacketTypeSC_0xEE.m_iErrorCode == g_dErrorNoPatchFile )
		{
			m_lResult = g_lAlreadyPatched;
		}
		else
		{
			wchar_t tmp[1024] = {0,};

			wprintf_s( tmp, "command : %d, size : %d, error code : %d", cPacketTypeSC_0xEE.m_iCommand, cPacketTypeSC_0xEE.m_iSize, cPacketTypeSC_0xEE.m_iErrorCode );

			MessageBoxW( 0, tmp, L"��ġ ����", 0 );
		}

		bResult = false;

		//������ ���´�.
		cleanupWinsock();
		//closesocket( m_hSocket );
	}
	else if( iCommand == (unsigned char) 0xff )
	{
		CPacketTypeSC_0xff	cPacketTypeSC_0xff;
		int				iIndex;

		iIndex = 0;

		memcpy( &cPacketTypeSC_0xff.m_iCommand, &pstrPacket[iIndex], sizeof(cPacketTypeSC_0xff.m_iCommand) );
		iIndex += sizeof(cPacketTypeSC_0xff.m_iCommand);

		memcpy( &cPacketTypeSC_0xff.m_iSize, &pstrPacket[iIndex], sizeof(cPacketTypeSC_0xff.m_iSize) );
		iIndex += sizeof(cPacketTypeSC_0xff.m_iSize);

		memcpy( &cPacketTypeSC_0xff.m_iSeq, &pstrPacket[iIndex], sizeof(cPacketTypeSC_0xff.m_iSeq) );
		iIndex += sizeof(cPacketTypeSC_0xff.m_iSeq);

		if (pcsArea)
			pcsArea->CalcPingTime(cPacketTypeSC_0xff.m_iSeq);
	}

	delete [] pstrPacket;

	return bResult;
}

bool CPatchClientLib::GetPatchInfo( char *pstrPacket )
{
	CPacketTypeSC_0x01	cPacketTypeSC_0x01;

	bool			bResult;
	int				iIndex;
	__int64			i64FreeBytesToCaller;
	__int64			i64TotalBytes;
	__int64			i64FreeBytes;

	bResult = true;
	iIndex = 0;

	memcpy( &cPacketTypeSC_0x01.m_iSize, &pstrPacket[iIndex], sizeof(cPacketTypeSC_0x01.m_iSize) );
	iIndex += sizeof(cPacketTypeSC_0x01.m_iSize);

	memcpy( &cPacketTypeSC_0x01.m_iCommand, &pstrPacket[iIndex], sizeof(cPacketTypeSC_0x01.m_iCommand) );
	iIndex += sizeof(cPacketTypeSC_0x01.m_iCommand);

	memcpy( &cPacketTypeSC_0x01.m_iRecvFiles, &pstrPacket[iIndex], sizeof(cPacketTypeSC_0x01.m_iRecvFiles) );
	iIndex += sizeof(cPacketTypeSC_0x01.m_iRecvFiles);

	memcpy( &cPacketTypeSC_0x01.m_iVersion, &pstrPacket[iIndex], sizeof(cPacketTypeSC_0x01.m_iVersion) );
	iIndex += sizeof(cPacketTypeSC_0x01.m_iVersion);

	memcpy( &cPacketTypeSC_0x01.m_iPatchCheckCode, &pstrPacket[iIndex], sizeof(cPacketTypeSC_0x01.m_iPatchCheckCode) );
	iIndex += sizeof(cPacketTypeSC_0x01.m_iPatchCheckCode);

	memcpy( &cPacketTypeSC_0x01.m_iTotalSize, &pstrPacket[iIndex], sizeof(cPacketTypeSC_0x01.m_iTotalSize) );
	iIndex += sizeof(cPacketTypeSC_0x01.m_iTotalSize);

	memcpy( &cPacketTypeSC_0x01.m_strServerIP, &pstrPacket[iIndex], sizeof(cPacketTypeSC_0x01.m_strServerIP) );
	iIndex += sizeof(cPacketTypeSC_0x01.m_strServerIP);

	memcpy( &cPacketTypeSC_0x01.m_iServerPort, &pstrPacket[iIndex], sizeof(cPacketTypeSC_0x01.m_iServerPort) );
	iIndex += sizeof(cPacketTypeSC_0x01.m_iServerPort);

	//�����κ��� ������ �޴´�.
//	m_cPatchReg.SetVersion( cPacketTypeSC_0x01.m_iVersion );
	m_cPatchReg.SetPatchCheckCode( cPacketTypeSC_0x01.m_iPatchCheckCode );

	m_iVersion = cPacketTypeSC_0x01.m_iVersion;
	m_iPatchCheckCode = cPacketTypeSC_0x01.m_iPatchCheckCode;
	m_iRecvPatchFiles = cPacketTypeSC_0x01.m_iRecvFiles;

	//DownloadServer������ �����Ѵ�.
	if (!cPacketTypeSC_0x01.m_iRecvFiles)
	{
		cleanupWinsock();
		//closesocket( m_hSocket );
	}
	else
	{
		memset( m_strDownloadServerIP, 0, sizeof(m_strDownloadServerIP) );
		strcat( m_strDownloadServerIP, cPacketTypeSC_0x01.m_strServerIP );

		m_iDownloadServerPort = cPacketTypeSC_0x01.m_iServerPort;

		//���۵Ǿ���ϴ� ������ �޾Ƽ� ���α׷��� �ٸ� �׸���.
		//m_pcCustomProgressRecvFile->SetMaxCount( cPacketTypeSC_0x01.m_iRecvFiles );
		
		//��ũ�� ����ũ�⸦ ����.
		GetDiskFreeSpaceEx( NULL,
							(PULARGE_INTEGER)&i64FreeBytesToCaller,
							(PULARGE_INTEGER)&i64TotalBytes,
							(PULARGE_INTEGER)&i64FreeBytes );

		//�ϵ� ��ũ�� ���������� ����.
		if( cPacketTypeSC_0x01.m_iTotalSize >= i64FreeBytes )
		{
			m_lResult = g_lNotEnoughDiskSpace;
			m_iRequireDiskSpace = cPacketTypeSC_0x01.m_iTotalSize;
			bResult = false;
		}

		if( bResult )
		{
			if( m_iRecvPatchFiles == 0 )
			{
				bResult = false;
			}
			else
			{
				m_pcCustomProgressRecvFile->SetMaxCount( m_iRecvPatchFiles );

				//��ġ�Ұ� ������ ������ �޶���Ѵ�.
				bResult = SendRequestFile();
			}
		}
	}

	return bResult;
}

bool CPatchClientLib::GetPatchFile( char *pstrPacket )
{
	bool			bResult;

	bResult = false;

	//������ġ�� ���� ������ �޴´�.
	if( pstrPacket[4] == 0x02 )
	{
		CPacketTypeSC_0x02	cPacketTypeSC_0x02;

		AgpdPatchFileInfo	*pcsPatchFileInfo;

		int				iCurrentPos;

		iCurrentPos = 0;

		pcsPatchFileInfo = new AgpdPatchFileInfo;

		//���� �߰� ��Ŷ�� �޴´�.
		memcpy( &cPacketTypeSC_0x02.m_iSize, &pstrPacket[iCurrentPos], sizeof( cPacketTypeSC_0x02.m_iSize ) );
		iCurrentPos += sizeof( cPacketTypeSC_0x02.m_iSize );
		memcpy( &cPacketTypeSC_0x02.m_iCommand, &pstrPacket[iCurrentPos], sizeof( cPacketTypeSC_0x02.m_iCommand ) );
		iCurrentPos += sizeof( cPacketTypeSC_0x02.m_iCommand );
		//������ ���̸� ����.
		memcpy( &pcsPatchFileInfo->m_iFileNameSize, &pstrPacket[iCurrentPos], sizeof( int ) );
		iCurrentPos += sizeof( int );
		//���� �̸��� ����.
		pcsPatchFileInfo->m_pstrFileName = new char[pcsPatchFileInfo->m_iFileNameSize];
		memcpy( pcsPatchFileInfo->m_pstrFileName, &pstrPacket[iCurrentPos], pcsPatchFileInfo->m_iFileNameSize );
		iCurrentPos += pcsPatchFileInfo->m_iFileNameSize;
		//������ ũ�⸦ ����.
		memcpy( &pcsPatchFileInfo->m_iFileSize, &pstrPacket[iCurrentPos], sizeof( int ) );
		iCurrentPos += sizeof( int );
		//CRC CheckSum�� �����Ѵ�.
		memcpy( &pcsPatchFileInfo->m_ulCheckSum, &pstrPacket[iCurrentPos], sizeof(unsigned long) );
		iCurrentPos += sizeof(unsigned long);

		m_csPatchFileList.Add( pcsPatchFileInfo );

		bResult = true;
	}

	return bResult;
}

bool CPatchClientLib::RemoveFile( char *pstrPacket )
{
	if( pstrPacket[4] != 0x03 )
		return false;

	INT32					iCurrentPos			=	0;
	CPacketTypeSC_0x03		cPacketTypeSC_0x03;

	//���� �߰� ��Ŷ�� �޴´�.
	memcpy( &cPacketTypeSC_0x03.m_iSize, &pstrPacket[iCurrentPos], sizeof( cPacketTypeSC_0x03.m_iSize ) );
	iCurrentPos += sizeof( cPacketTypeSC_0x03.m_iSize );
	memcpy( &cPacketTypeSC_0x03.m_iCommand, &pstrPacket[iCurrentPos], sizeof( cPacketTypeSC_0x03.m_iCommand ) );
	iCurrentPos += sizeof( cPacketTypeSC_0x03.m_iCommand );
	memcpy( &cPacketTypeSC_0x03.m_PatchVersion , &pstrPacket[iCurrentPos], sizeof( cPacketTypeSC_0x03.m_PatchVersion) );
	iCurrentPos += sizeof( cPacketTypeSC_0x03.m_PatchVersion );

	// ��ū���ڿ��� �Ѿ�� ���ϸ���� �̿��� ������ �����Ѵ�
	DeleteTokenFileName( &pstrPacket[iCurrentPos] );

	return true;
}

bool CPatchClientLib::DeleteTokenFileName( char *pTokenFileName )
{
	CHAR*				pNextChar		=	NULL;
	CHAR*				pFileName		=	NULL;

	string				strFileName;
	string				strFilePath;

	RemoveFileList*		pRemoveFileList;

	m_RemoveFolderMap.clear();

	pFileName		=	strtok_s( pTokenFileName , "#" , &pNextChar );


	for( ; pFileName ; )
	{
		//OneFileDelete( pFileName );

		// ���� ��ο� ���� �̸��� ���� ������
		for( INT i = strlen(pFileName) ; i > 0  ; --i )
		{

			if( pFileName[i] == '\\' )
			{
				strFileName			=	&pFileName[ i+1 ];
				strFilePath			=	pFileName;
				strFilePath.erase( strFilePath.find( strFileName.c_str() )-1 , strFileName.length()+1 );
								
				break;
			}
		}

		// �ش� ��ΰ� �ʿ� ������ ����Ʈ�� ���� �����
		if( m_RemoveFolderMap.find( strFilePath ) == m_RemoveFolderMap.end() )
		{
			 m_RemoveFolderMap.insert( make_pair( strFilePath , RemoveFileList() ) );
			 pRemoveFileList	=	&(*(m_RemoveFolderMap.find( strFilePath ))).second;
		}

		pRemoveFileList->push_back( strFileName );
		pFileName		=	strtok_s( NULL , "#" , &pNextChar );
	}

	// ���� ���ϵ��� �� �����
	DeleteFileList();

	return TRUE;
}

bool CPatchClientLib::IsPackingFolder( const string& strFolderName )
{
	if( m_csAuPackingManager.GetFolder( (char*)strFolderName.c_str() ) )
		return TRUE;

	return FALSE;
}

bool CPatchClientLib::PackingDataFileDelete( const char* strFolderName , RemoveFileList& rFileList )
{

	CPackingFolder* pFolder = m_csAuPackingManager.GetFolder( (char*)strFolderName );
	if( !pFolder )
		return false;
	
	RemoveFileListIter		Iter		=	rFileList.begin();
	for( ; Iter != rFileList.end() ; ++Iter )
	{
		//m_csAuPackingManager.ReadyPacking( strFolderName );

		//m_csAuPackingManager.BuildPackingFile( "temp" , strFolderName , Packing_Data_FileName , 0 , 0 , 0 , 0 , 0 );

		//m_csAuPackingManager.CompletePacking( strFolderName );

	}

	return TRUE;
}

bool CPatchClientLib::DataFileDelete( const char* strFolderName , RemoveFileList& rFileList )
{
	CHAR	strFilePath[ MAX_PATH ];
	ZeroMemory( strFilePath , MAX_PATH );

	RemoveFileListIter	Iter		=	rFileList.begin();

	for( ; Iter != rFileList.end() ; ++Iter )
	{
		if( strFolderName[0] == 'r' &&
			strFolderName[1] == 'o' &&
			strFolderName[2] == 'o' &&
			strFolderName[3] == 't'		)
		{
			sprintf_s( strFilePath , MAX_PATH , "%s" , (*Iter).c_str() );
		}
		else
		{
			sprintf_s( strFilePath , MAX_PATH , "%s\\%s" , strFolderName , (*Iter).c_str() );
		}

		DeleteFile( strFilePath );
	}

	return true;
}

bool CPatchClientLib::DeleteFileList( VOID )
{
	RemoveFolderMap::iterator		RemoveIter;

	//�������� �� ���鼭 ������ �����
	for( RemoveIter = m_RemoveFolderMap.begin() ; RemoveIter!= m_RemoveFolderMap.end() ; ++RemoveIter )
	{
		// Packing �� �������
		if( IsPackingFolder( RemoveIter->first.c_str() ) )
		{
			//PackingDataFileDelete( RemoveIter->first.c_str() , RemoveIter->second );
		}

		// Packing���� ���� �������
		else
		{
			//DataFileDelete( RemoveIter->first.c_str() , RemoveIter->second );
		}
		
	}

	return TRUE;
}

bool CPatchClientLib::PatchFileToDisk( int iFileNameSize, char *pstrFileName, int iFileSize, char *pstrPatchFile )
{
	FILE		*file;

	bool		bResult;
	char		strOriginDir[TEMP_STRING_LENGTH];
	char		strCurrentDir[TEMP_STRING_LENGTH];
	char		strSFileName[TEMP_STRING_LENGTH];

	bResult = false;

	GetCurrentDirectory( TEMP_STRING_LENGTH, strOriginDir );

	//���� ���丮�� temp�� �����.
	if( SetCurrentDirectory( "temp" ) == FALSE )
	{
		CreateDirectory( "temp", NULL );
		SetCurrentDirectory( "temp" );
		GetCurrentDirectory( TEMP_STRING_LENGTH, strCurrentDir );
	}
	else
	{
		SetCurrentDirectory( "temp" );
		GetCurrentDirectory( TEMP_STRING_LENGTH, strCurrentDir );
	}

	sprintf( strSFileName, "%s%s", strCurrentDir, pstrFileName );

	file = fopen( strSFileName, "wb" );

	if( file == NULL )
	{
		int		iErrorCode;

		iErrorCode = GetLastError();

		//ErrorCode�� 0x03�̸� ������ ���°��!! ������ ������ش�.
		if( iErrorCode == 0x03 )
		{
			bool			bGetDirName;
			int				iCurrentPos;
			int				iDPathSize;
			char			strDir[TEMP_STRING_LENGTH];

			bGetDirName = false;
			iDPathSize = (int) strlen( pstrFileName );
			memset( strDir, 0, TEMP_STRING_LENGTH );

			for( int i=0; i<iDPathSize; i++ )
			{
				if( bGetDirName == true )
				{
					strDir[iCurrentPos] = pstrFileName[i];
					iCurrentPos++;
				}

				if( pstrFileName[i] == '\\' )
				{
					if( bGetDirName == false )
					{
						iCurrentPos = 0;
						bGetDirName = true;
					}
					else
					{
						CreateDirectory( strDir, NULL );
						
						//������ �����Ѵ�.
						int				iWriteBytes;

						file = fopen( strSFileName, "wb" );

						if( file != NULL )
						{
							iWriteBytes = (int) fwrite( pstrPatchFile, 1, iFileSize, file );

							if( iWriteBytes == iFileSize )
								bResult = true;

							fclose( file );

							break;
						}
						else
						{
							if( GetLastError() == 0x03 )
							{
								continue;
							}
							else
							{
								bResult = false;
								break;
							}
						}
					}
				}
			}			
		}
		else
		{
			bResult = false;
		}
	}
	else
	{
		int				iWriteBytes;

		iWriteBytes = (int) fwrite( pstrPatchFile, 1, iFileSize, file );

		if( iWriteBytes == iFileSize )
			bResult = true;

		fclose( file );
	}

	SetCurrentDirectory( strOriginDir );

	return bResult;
}

bool CPatchClientLib::RemoveDir( char *pstrDir )
{
	RemoveDirR( pstrDir, pstrDir );

	RemoveDirectory( pstrDir );

	return true;
}

bool CPatchClientLib::RemoveDirR( LPCTSTR pstrSourcePath, LPCTSTR pstrCurrentPath )
{
	CFileFind finder;

	CString strWildcard(pstrCurrentPath);
	strWildcard += _T("\\*.*");

	BOOL bWorking = finder.FindFile(strWildcard);

	while (bWorking)
	{		
		bWorking = finder.FindNextFile();

		if (finder.IsDots())
			continue;

		if (finder.IsDirectory())
		{
			CString str = finder.GetFilePath();

			RemoveDirR( pstrSourcePath, str );

			RemoveDirectory( str );
		}
		else
		{
			CStringA		strString;
			char			*pstrFileName;

			strString = finder.GetFilePath();
			pstrFileName = strString.GetBuffer(0);

			DeleteFile( pstrFileName );
		}
	}

	finder.Close();

	return true;
}

BOOL CPatchClientLib::CopyFileToPatchFolder( char *pstrSPath, char *pstrDPath, char *pstrFullPath, BOOL bOverwrite )
{
	BOOL		bResult;
	char		strSFileName[TEMP_STRING_LENGTH];

	memset( strSFileName, 0, TEMP_STRING_LENGTH );
	strcat( strSFileName, pstrSPath );
	strcat( strSFileName, pstrDPath );

	bResult = CopyFile( pstrFullPath, strSFileName, bOverwrite );

	if( bResult == FALSE )
	{
		int		iErrorCode;

		iErrorCode = GetLastError();
		
		//ErrorCode�� 0x03�̸� ������ ���°��!! ������ ������ش�.
		if( iErrorCode == 0x03 )
		{
			bool			bGetDirName;
			int				iCurrentPos;
			int				iDPathSize;
			char			strDir[TEMP_STRING_LENGTH];

			bGetDirName = false;
			iDPathSize = (int) strlen( pstrDPath );
			memset( strDir, 0, TEMP_STRING_LENGTH );

			for( int i=0; i<iDPathSize; i++ )
			{
				if( bGetDirName == true )
				{
					strDir[iCurrentPos] = pstrDPath[i];
					iCurrentPos++;
				}

				if( pstrDPath[i] == '\\' )
				{
					if( bGetDirName == false )
					{
						iCurrentPos = 0;
						bGetDirName = true;
					}
					else
					{
						CreateDirectory( strDir, NULL );
						
						//������ �����Ѵ�.
						bResult = CopyFile( pstrFullPath, strSFileName, bOverwrite );

						if( bResult == TRUE )
						{
							break;
						}
						else
						{
							if( GetLastError() == 0x03 )
							{
								continue;
							}
							else
							{
								bResult = FALSE;
								break;
							}
						}
					}
				}
			}			
		}
		else
		{
			if( iErrorCode == 80 )
			{
				//���� �ڵ�߻�
			}

			bResult = FALSE;
		}
	}

	//PatchFiles Folder�� �����Ѵ�. Remove���ϼ��ð� ���� �ö� ��ġ ���ϵ��� �������� 
	//PatchFiles�� ���縦�Ѵ�.
	return bResult;
}

bool CPatchClientLib::CopyDir( char *pstrDestDir, char *pstrSourceDir )
{
	//���� �۾������� ����Ѵ�.
	char			strOriginDir[TEMP_STRING_LENGTH];

	//���������� �����Ѵ�.
	GetCurrentDirectory( TEMP_STRING_LENGTH, strOriginDir );

	//���� ������ pstrDestDir�� �������ش�.
	SetCurrentDirectory( pstrDestDir );

	CopyDirR( pstrDestDir, pstrSourceDir, pstrSourceDir );

	//���� ������ ������.
	SetCurrentDirectory( strOriginDir );

	return true;
}

bool CPatchClientLib::CopyDirR( char *pstrDestPath, char *pstrSourcePath, LPCTSTR pstrCurrentPath )
{
	CFileFind finder;

	CString strWildcard(pstrCurrentPath);
	strWildcard += _T("\\*.*");

	BOOL bWorking = finder.FindFile(strWildcard);

	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		if (finder.IsDots())
			continue;

		if (finder.IsDirectory())
		{
			CString str = finder.GetFilePath();

			CopyDirR( pstrDestPath, pstrSourcePath, str );
		}
		else
		{
			CString			strString;
			char			*pstrFileName;
			char			strTempDir[TEMP_STRING_LENGTH];
			char			strPatchFileName[TEMP_STRING_LENGTH];
			int				iDestPathSize;
			int				iPatchFileNameSize;

			strString = finder.GetFilePath();
			pstrFileName = strString.GetBuffer(0);

			sprintf( strTempDir, "%s\\%s", pstrDestPath, "temp" );

			iDestPathSize = (int) strlen(strTempDir);
			iPatchFileNameSize = (int) strlen(pstrFileName)+1 - iDestPathSize;

			memset( strPatchFileName, 0, TEMP_STRING_LENGTH );
			memcpy( strPatchFileName, &pstrFileName[iDestPathSize], iPatchFileNameSize );

			CopyFileToPatchFolder( pstrDestPath, strPatchFileName, pstrFileName, true );
		}
	}

	finder.Close();

	return true;
}

bool CPatchClientLib::GetTotalPatchFiles( char *pstrSourcePath )
{
	GetTotalPatchFilesR( pstrSourcePath, pstrSourcePath );

	return true;
}


bool CPatchClientLib::GetTotalPatchFilesR( char *pstrSourcePath, LPCTSTR pstrCurrentPath )
{
	CFileFind finder;

	CString strWildcard(pstrCurrentPath);
	strWildcard += _T("\\*.jzp");

	BOOL bWorking = finder.FindFile(strWildcard);

	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		if (finder.IsDots())
			continue;

		if (finder.IsDirectory())
		{
			//@{ 2006/10/17 burumal
			if ( finder.IsHidden() )
				continue;
			//@}

			CString str = finder.GetFilePath();

			SearchJZPFileR( pstrSourcePath, str );
		}
		else
		{
			//JZP���Ͽ� ���Ե� ���ϼ��� ����.
			FILE			*file;
			CString			strString;

			int				iVersion, iCompressFiles;
			char			*pstrFileName;

			strString = finder.GetFilePath();
			pstrFileName = strString.GetBuffer(0);

			file = fopen( pstrFileName, "rb" );

			if( file != NULL )
			{
				fread( &iVersion, 1, sizeof( iVersion ), file );
				fread( &iCompressFiles, 1, sizeof( iCompressFiles ), file );

				m_iTotalPatchFiles += iCompressFiles;

				fclose( file);
			}
		}
	}

	finder.Close();

	return true;
}

bool CPatchClientLib::GetPatchFiles( char *pstrSourcePath, char* pstrPatchFile )
{
	if ( !pstrSourcePath || !pstrPatchFile )
		return false;
	
	CFileFind finder;

	CString strWildcard(pstrSourcePath);
	strWildcard += _T("\\*.jzp");

	BOOL bWorking = finder.FindFile(strWildcard);

	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		if (finder.IsDots())
			continue;

		if (finder.IsDirectory())
		{

			if ( finder.IsHidden() )
				continue;

			CString str = finder.GetFilePath();

			SearchJZPFileR( pstrSourcePath, str );
		}
		else
		{
			//JZP���Ͽ� ���Ե� ���ϼ��� ����.
			FILE			*file;
			CString			strString;

			int				iVersion, iCompressFiles;
			char			*pstrFileName;

			strString = finder.GetFilePath();
			pstrFileName = strString.GetBuffer(0);

			int nPatchLen = (int) strlen(pstrPatchFile);
			int nFindLen = (int) strlen(pstrFileName);

			if ( nFindLen >= nPatchLen )
			{
				if ( stricmp(pstrFileName + nFindLen - nPatchLen, pstrPatchFile) == 0 )
				{
					file = fopen( pstrFileName, "rb" );

					if( file != NULL )
					{
						fread( &iVersion, 1, sizeof( iVersion ), file );
						fread( &iCompressFiles, 1, sizeof( iCompressFiles ), file );

						m_iTotalPatchFiles += iCompressFiles;

						fclose( file);
					}

					break;
				}
			}
		}
	}

	finder.Close();

	return true;
}
//@}

bool CPatchClientLib::SearchJZPFile( char *pstrSourcePath )
{
	SearchJZPFileR( pstrSourcePath, pstrSourcePath );

	return true;
}

bool CPatchClientLib::SearchJZPFileR( char *pstrSourcePath, LPCTSTR pstrCurrentPath )
{
	CFileFind finder;

	CString strWildcard(pstrCurrentPath);
	strWildcard += _T("\\*.jzp");

	BOOL bWorking = finder.FindFile(strWildcard);

	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		if (finder.IsDots())
			continue;

		if (finder.IsDirectory())
		{
			//@{ 2006/10/17 burumal
			if ( finder.IsHidden() )
				continue;
			//@}

			CString str = finder.GetFilePath();

			SearchJZPFileR( pstrSourcePath, str );
		}
		else
		{
			//JZP���ϸ� ������ Ǭ��.
			CString			strFileNamePathString;
			CString			strPureFileNameString;
			char			*pstrFileName;    //Filename, Path���� 
			char			*pstrPureFileName;//FileName��.

			strFileNamePathString = finder.GetFilePath();
			pstrFileName = strFileNamePathString.GetBuffer(0);
			strPureFileNameString = finder.GetFileName();
			pstrPureFileName = strPureFileNameString.GetBuffer(0);

			//JZP������ �а� ������ Ǭ��
			DecompressJZP( "temp", pstrPureFileName );
		}
	}

	finder.Close();

	return true;
}

#ifdef USE_ZPACK
#include "zpack/zpack.h"

static bool isZPacking( std::string path )
{
	typedef std::vector< std::string > NonePackFiles;

	static NonePackFiles nonePakcFiles;

	if( nonePakcFiles.empty() )
	{
		nonePakcFiles.push_back( ".lua" );
		nonePakcFiles.push_back( "texture\\notpacked\\" );
		nonePakcFiles.push_back( "sound\\" );
		nonePakcFiles.push_back( ".fx" );
		nonePakcFiles.push_back( "config\\" );
		nonePakcFiles.push_back( "coption.ini" );
		nonePakcFiles.push_back( "world\\" );
		nonePakcFiles.push_back( "autopickup.xml" );
		nonePakcFiles.push_back( "gameguard\\" );
		nonePakcFiles.push_back( "images\\" );
		nonePakcFiles.push_back( ".exe" );
		nonePakcFiles.push_back( "archlord.ini" );
	}

	for( size_t i=0; i < path.length(); ++i )
	{
		path[i] = std::tolower( path[i] );
		if( path[i] == '/' )
			path[i] = '\\';
	}

	if( path.find( "\\" ) == std::string::npos )
		return false;

	for( NonePackFiles::iterator iter = nonePakcFiles.begin(); iter != nonePakcFiles.end(); ++iter )
	{
		if( path.find( *iter ) != std::string::npos )
			return false;
	}

	return true;
}

static void encryptFile( std::string src )
{
	std::string srcName = src;
	std::string destName = src+"tmp";

	std::ifstream ifs; 
	std::ofstream ofs; 
	ifs.open(src.c_str(), std::ios::in | std::ios::binary);
	ofs.open(destName.c_str(), std::ios::out | std::ios::binary);

	char c;

	char len=0;

	ifs.seekg( 0, std::ios::end );
	len = (char)ifs.tellg();
	ifs.seekg( 0, std::ios::beg );

	while( ifs.get(c) )
	{
		c ^= len;
		c = ~c;
		ofs.put( c );
	}

	ifs.close();
	ofs.close();

	try { DeleteFileA( src.c_str() ); }
	catch(...) {}
}

bool CPatchClientLib::PackingToZpack(char* pstrTempFolderName, char* pstrJzpFileName)
{
	FILE*			fpJZP;	
	bool			bPacking;
	char			strCurrentDir[TEMP_STRING_LENGTH];
	char			strJZPFileName[TEMP_STRING_LENGTH];
	int				lVersion;
	int				lMaxFolderCount;
	int				lFolderCount;

	int				lTotalFileCount;
	int				lCurFileProceed;

	int				lFolderNameLen;	
	char			pstrFolderName[512];

	wchar_t			strMsgFile[TEMP_STRING_LENGTH];
	wchar_t			strMsgProgress[TEMP_STRING_LENGTH];
	wchar_t			strMsg[TEMP_STRING_LENGTH];
	wchar_t			strFileNameW[TEMP_STRING_LENGTH];
	wchar_t			strPatchFileNameW[TEMP_STRING_LENGTH];

	if ( !pstrTempFolderName || !pstrJzpFileName )
		return false;

	::LoadStringW(g_resInst, IDS_MSG_APPLY_FILE, strMsgFile, TEMP_STRING_LENGTH);
	::LoadStringW(g_resInst, IDS_MSG_APPLY_PROGRESS, strMsgProgress, TEMP_STRING_LENGTH);


	GetCurrentDirectory( sizeof(strCurrentDir), strCurrentDir );

	sprintf(strJZPFileName , "%s\\%s\\%s", strCurrentDir, pstrTempFolderName, pstrJzpFileName );
	MultiByteToWideChar( CP_ACP, 0, pstrJzpFileName, (int) strlen(pstrJzpFileName) + 1, strPatchFileNameW, TEMP_STRING_LENGTH);

	// temp �����ȿ� �ٿ�޾��� ��ġ������ ���ϴ�.
	fpJZP = fopen( strJZPFileName, "rb" );
	if ( fpJZP == NULL )
		return false;	

#ifdef _DEBUG
	char pDbgMsg[256];
	sprintf(pDbgMsg, "Current Patch File ==> %s\n", pstrJzpFileName);
	OutputDebugString(pDbgMsg);
#endif

	fread( &lVersion, 1, sizeof(int), fpJZP );
	fread( &lTotalFileCount, 1, sizeof(int), fpJZP );

	lCurFileProceed = 0;

	m_pcCustomProgressRecvBlock->SetMaxCount( lTotalFileCount );
	m_pcCustomProgressRecvBlock->SetCurrentCount( 0 );
	m_phParentCWnd->InvalidateRect( m_pcCustomProgressRecvBlock->GetRect(), FALSE );

	m_pcCustomProgressRecvFile->SetMaxCount(m_iTotalPatchFiles);
	m_pcCustomProgressRecvFile->SetCurrentCount(m_iCurrentPatchFiles);
	m_phParentCWnd->InvalidateRect( m_pcCustomProgressRecvFile->GetRect(), FALSE );

	//���� ����
	fread( &lMaxFolderCount, 1, sizeof(int), fpJZP );

	// �޸�Ǯ �غ�
	char* pTempMemPool = NULL;
	#define nMEM_BLOCK_SIZE (1024*1024) // 1M
	pTempMemPool = new char[nMEM_BLOCK_SIZE];
	ASSERT(pTempMemPool);
	if ( !pTempMemPool )
		return false;
	long lMemBlockSize = nMEM_BLOCK_SIZE;

	// garbage size
	long lGarbageDataSize = 0;
	bool bRefBakReallyExist;

	// ��ġ���Ͽ� ��ŷ�� ���� ������ŭ �ݺ�
	for ( lFolderCount = 0; lFolderCount < lMaxFolderCount; lFolderCount++ )
	{
		int				lMaxFileCount;
		int				lCurrentPosition;
		int				lPackedFileCount;
		bool			bPackingFolder;

		FILE* fpBakDat = NULL;
		FILE* fpReference = NULL;

		lCurrentPosition = 0;
		lPackedFileCount = 0;

		char strDataFilePath[256];
		char strReferenceFilePath[256];

		//�����̸� ���̷ε�
		fread( &lFolderNameLen, 1, sizeof(int), fpJZP );

		memset( pstrFolderName, 0, lFolderNameLen );
		fread( pstrFolderName, 1, lFolderNameLen, fpJZP );

		//���ϰ������
		fread( &lMaxFileCount, 1, sizeof(int), fpJZP );

		progressMax_ += lMaxFileCount;

		CPatchFileInfo* pPatchFileInfoArray = NULL;
		pPatchFileInfoArray = new CPatchFileInfo[lMaxFileCount];
		ASSERT(pPatchFileInfoArray);

		if ( pPatchFileInfoArray == NULL )
		{
			((CPatchClientDlg*) m_phParentCWnd)->CloseDlg();
			exit(1);
		}

		//�� ������ ��ŷ�� �ϴ� ������ �����ϴ°�?
		fread( &bPackingFolder, 1, sizeof(bool), fpJZP );

		bRefBakReallyExist = false;

		// ������ ����ִ� ���� ������ŭ ����
		for( int lCounter = 0; lCounter < lMaxFileCount; lCounter++ )
		{
			bool			bDone = false;
			int				lOperation;
			int				lFileNameLength;
			int				lFileSize;				
			char			pstrFileName[512];

			fread( &bPacking, 1, sizeof(bool), fpJZP );
			fread( &lOperation, 1, sizeof(int), fpJZP );
			fread( &lFileNameLength, 1, sizeof(int), fpJZP );
			memset( pstrFileName, 0, lFileNameLength );
			fread(pstrFileName, 1, lFileNameLength, fpJZP );

			if ( lOperation == Packing_Operation_Add )
			{
				//FileSize
				fread( &lFileSize, 1, sizeof(int), fpJZP );

				// �޸�Ǯ ó��
				if ( lFileSize > lMemBlockSize )
				{
					ASSERT(pTempMemPool);
					delete [] pTempMemPool;

					int lNewBlockCnt = lFileSize / nMEM_BLOCK_SIZE;
					if ( (lFileSize % nMEM_BLOCK_SIZE) > 0 )
						lNewBlockCnt++;

					pTempMemPool = new char[lNewBlockCnt * nMEM_BLOCK_SIZE];
					if ( !pTempMemPool )
						return false;

					lMemBlockSize = lNewBlockCnt * nMEM_BLOCK_SIZE;
				}

				{
					//FileData
					fread( pTempMemPool, 1, lFileSize, fpJZP );

#ifdef _DEBUG
					sprintf(pDbgMsg, "\tExtracted file name : %s\\%s\n", pstrFolderName, pstrFileName);
					OutputDebugString(pDbgMsg);
#endif

					if ( !strcmpi( "root", pstrFolderName ) )
					{
						//���� ��ġŬ���̾�Ʈ�� Update�ϴ°���� �ڽ��� �ٸ������� �����Ѵ�.
						//. 2006. 3. 20. nonstopdj
						//. Ȯ���ڸ� ".exe" -> ".bak"�� �ٲ㼭 �����Ѵ�.

						//if( !stricmp( pstrFileName, m_exeName.c_str() ) )
						if( !stricmp( pstrFileName, "Archlord.exe" ) )
						{
							m_bPatchClient = true;
							sprintf(pstrFileName, BakName.c_str() );
						}

						m_cCompress.decompressMemory( pTempMemPool, lFileSize, "", pstrFileName );

						++progressCount_;
					}
					else
					{
						char			strDestDir[TEMP_STRING_LENGTH];

						sprintf( strDestDir, "%s\\%s", pstrFolderName, pstrFileName );

						DeleteFile( strDestDir );

						m_cCompress.decompressMemory( pTempMemPool, lFileSize, "", strDestDir );


						if( isZPacking( strDestDir ) )
						{
							std::wstring wDir = lexical_cast< std::wstring >( strDestDir );

							{
								std::wstring packName = zpack::getPackName( strDestDir );

								if( packName == L"ini.zp" )
								{
									if( !zpack::add( packName.c_str(), wDir.c_str(), 1, zpack::getPassword().c_str() ) )
									{
										::MessageBox( 0, "filed to add", "zpack", 0 );
										exit(1);
										return false;
									}

									try	{ DeleteFileA( strDestDir ); } catch(...) {}

									bDone = true;
								}
								else
								{
									patchFiles_.insert( std::make_pair( packName, wDir ) );
									encryptFile( strDestDir );
								}
							}
						}
						else
						{
							bDone = true;
						}

						if ( g_pLogger )
							g_pLogger->Log("_Added_", strDestDir);
					}
				}
			}
			// Operation�� remove �� �����
			else
			{
				CHAR	szFileFullName[ MAX_PATH ];
				sprintf_s( szFileFullName , MAX_PATH , "%s\\%s\\%s" , strCurrentDir , pstrFolderName , pstrFileName );
				FILE*	pTempFile	=	fopen( szFileFullName , "rb" );

				// ��ŷ���� ����ä�� �ִ� �������� Ȯ���Ѵ�.
				if ( !pTempFile )
				{	
					char destPath[1024];
					sprintf_s( destPath, "%s\\%s", pstrFileName, pstrFileName );

					std::wstring wDir = lexical_cast<std::wstring>( destPath );

					// ��ŷ���Ͽ��� ����
					{
						if( !zpack::del( zpack::getPackName( destPath ), wDir.c_str(), zpack::getPassword().c_str() ) )
						{
							::MessageBox( 0, "filed to delete", "zpack", 0 );
							exit(1);
							return false;
						}

						bDone = true;
					}
				}
				else
				{
					fclose( pTempFile );
					// ��ŷ�� ������ �ƴϹǷ� ���� ������ �����Ѵ�
					char			strDestDir[TEMP_STRING_LENGTH];

					if( !stricmp( "root", pstrFolderName ) )
					{
						sprintf( strDestDir, "%s", pstrFileName );
					}
					else
					{
						sprintf( strDestDir, "%s\\%s", pstrFolderName, pstrFileName );
					}

					DeleteFile( strDestDir );
				}

				++progressCount_;
			}

			// ���α׷����� ����

			// �Ʒ���
			m_iCurrentPatchFiles++;
			m_pcCustomProgressRecvFile->SetCurrentCount( m_iCurrentPatchFiles );
			m_pcCustomProgressRecvFile->SetMaxCount( m_iTotalPatchFiles);
			swprintf( strMsg, strMsgProgress, m_iCurrentPatchFiles, m_iTotalPatchFiles, strPatchFileNameW );
			InvalidateStatic( m_pcStaticStatus, strMsg);
			m_phParentCWnd->InvalidateRect( m_pcCustomProgressRecvFile->GetRect(), FALSE );

			if ( bDone )
			{
				// ����
				RefreshProgressBar1( lexical_cast<std::wstring>( pstrFileName ).c_str() );
			}
		}

		if ( g_pLogger )
			g_pLogger->Flush();
	}

	fclose(fpJZP);

	// �޸�Ǯ ����
	if( pTempMemPool != NULL )
	{
		delete [] pTempMemPool;
		pTempMemPool = NULL;
	}

	return true;
}

void CPatchClientLib::RefreshProgressBar1( std::wstring strFileNameW )
{
	wchar_t strMsg[10240] = {0,};
	static wchar_t strMsgFile[10240] = {0,};

	if( strMsgFile[0] == 0 )
	{
		::LoadStringW(g_resInst, IDS_MSG_APPLY_FILE, strMsgFile, TEMP_STRING_LENGTH);
	}

	swprintf( strMsg, strMsgFile, strFileNameW.c_str());

	m_pcCustomProgressRecvBlock->SetCurrentCount(++progressCount_);
	m_pcCustomProgressRecvBlock->SetMaxCount(progressMax_);
	
	InvalidateStatic( m_pcStaticDetailInfo, strMsg);

	m_phParentCWnd->InvalidateRect( m_pcCustomProgressRecvBlock->GetRect(), FALSE );
}

#endif


bool CPatchClientLib::DecompressJZPEx(char* pstrTempFolderName, char* pstrJzpFileName)
{
	FILE*			fpJZP;	
	bool			bPacking;
	char			strCurrentDir[TEMP_STRING_LENGTH];
	char			strTempDir[TEMP_STRING_LENGTH];
	char			strJZPFileName[TEMP_STRING_LENGTH];
	int				lVersion;
	int				lMaxFolderCount;
	int				lFolderCount;
	
	int				lTotalFileCount;
	int				lCurFileProceed;

	int				lFolderNameLen;	
	char			pstrFolderName[512];

	wchar_t			strMsgFile[TEMP_STRING_LENGTH];
	wchar_t			strMsgProgress[TEMP_STRING_LENGTH];
	wchar_t			strMsg[TEMP_STRING_LENGTH];
	wchar_t			strFileNameW[TEMP_STRING_LENGTH];
	wchar_t			strPatchFileNameW[TEMP_STRING_LENGTH];

	if ( !pstrTempFolderName || !pstrJzpFileName )
		return false;

	::LoadStringW(g_resInst, IDS_MSG_APPLY_FILE, strMsgFile, TEMP_STRING_LENGTH);
	::LoadStringW(g_resInst, IDS_MSG_APPLY_PROGRESS, strMsgProgress, TEMP_STRING_LENGTH);


	GetCurrentDirectory( sizeof(strCurrentDir), strCurrentDir );
	sprintf( strTempDir, "%s\\%s", strCurrentDir, pstrTempFolderName );

	sprintf(strJZPFileName , "%s\\%s\\%s", strCurrentDir, pstrTempFolderName, pstrJzpFileName );
	MultiByteToWideChar( CP_ACP, 0, pstrJzpFileName, (int) strlen(pstrJzpFileName) + 1, strPatchFileNameW, TEMP_STRING_LENGTH);

	fpJZP = fopen( strJZPFileName, "rb" );
	if ( fpJZP == NULL )
		return false;	

#ifdef _DEBUG
	char pDbgMsg[256];
	sprintf(pDbgMsg, "Current Patch File ==> %s\n", pstrJzpFileName);
	OutputDebugString(pDbgMsg);
#endif
	
	fread( &lVersion, 1, sizeof(int), fpJZP );
	fread( &lTotalFileCount, 1, sizeof(int), fpJZP );
	
	lCurFileProceed = 0;
	
	m_pcCustomProgressRecvBlock->SetMaxCount( lTotalFileCount );
	m_pcCustomProgressRecvBlock->SetCurrentCount( 0 );
	m_phParentCWnd->InvalidateRect( m_pcCustomProgressRecvBlock->GetRect(), FALSE );

	m_pcCustomProgressRecvFile->SetMaxCount(m_iTotalPatchFiles);
	m_pcCustomProgressRecvFile->SetCurrentCount(m_iCurrentPatchFiles);
	m_phParentCWnd->InvalidateRect( m_pcCustomProgressRecvFile->GetRect(), FALSE );

	//���� ����
	fread( &lMaxFolderCount, 1, sizeof(int), fpJZP );
	
	// �޸�Ǯ �غ�
	char* pTempMemPool = NULL;
	#define nMEM_BLOCK_SIZE		(1024*1024)	// 1M
	pTempMemPool = new char[nMEM_BLOCK_SIZE];
	ASSERT(pTempMemPool);
	if ( !pTempMemPool )
		return false;
	long lMemBlockSize = nMEM_BLOCK_SIZE;	
	
	// garbage size
	long lGarbageDataSize = 0;
	bool bRefBakReallyExist;


	for ( lFolderCount = 0; lFolderCount < lMaxFolderCount; lFolderCount++ )
	{
		int				lMaxFileCount;
		int				lCurrentPosition;
		int				lPackedFileCount;
		bool			bPackingFolder;

		FILE* fpBakDat = NULL;
		FILE* fpReference = NULL;
		
		lCurrentPosition = 0;
		lPackedFileCount = 0;

		char strDataFilePath[256];
		char strReferenceFilePath[256];

		//�����̸� ���̷ε�
		fread( &lFolderNameLen, 1, sizeof(int), fpJZP );
		
		memset( pstrFolderName, 0, lFolderNameLen );
		fread( pstrFolderName, 1, lFolderNameLen, fpJZP );

		//���ϰ������
		fread( &lMaxFileCount, 1, sizeof(int), fpJZP );

		// ��ġ���� ���� ����
		ApAdmin			csApAdminAdd;
		ApAdmin			csApAdminRemove;
		ApAdmin			csApAdminBakNormal;
		ApAdmin			csApAdminBakEmpty;

		CPackingFolder* pFolder = m_csAuPackingManager.GetFolder( pstrFolderName );
		if( pFolder )
		{
			int nCurrentFileCount = pFolder->m_csFilesInfo.GetObjectCount();
			csApAdminAdd.InitializeObject( sizeof(char *), ( lMaxFileCount + nCurrentFileCount ) * 2 );
			csApAdminRemove.InitializeObject( sizeof(char *), ( lMaxFileCount + nCurrentFileCount ) * 2 );
			csApAdminBakNormal.InitializeObject(sizeof(CPackingFile *), ( lMaxFileCount + nCurrentFileCount ) * 2 );
			csApAdminBakEmpty.InitializeObject(sizeof(CPackingFile *), ( lMaxFileCount + nCurrentFileCount ) * 2 );
		}
		else
		{
			csApAdminAdd.InitializeObject( sizeof(char *), lMaxFileCount * 2 );
			csApAdminRemove.InitializeObject( sizeof(char *), lMaxFileCount * 2 );
			csApAdminBakNormal.InitializeObject(sizeof(CPackingFile *), lMaxFileCount * 2 );
			csApAdminBakEmpty.InitializeObject(sizeof(CPackingFile *), lMaxFileCount * 2 );
		}

		CPatchFileInfo* pPatchFileInfoArray = NULL;
		pPatchFileInfoArray = new CPatchFileInfo[lMaxFileCount];
		ASSERT(pPatchFileInfoArray);

		if ( pPatchFileInfoArray == NULL )
		{
			((CPatchClientDlg*) m_phParentCWnd)->CloseDlg();
			exit(1);
		}

		//�� ������ ��ŷ�� �ϴ� ������ �����ϴ°�?
		fread( &bPackingFolder, 1, sizeof(bool), fpJZP );

		bRefBakReallyExist = false;

		if( bPackingFolder == true )
		{
			//���� Data.Dat Refrence.Dat�� �ִٸ� �̸��� �ٲ��ش�.
			m_csAuPackingManager.ReadyPacking( pstrFolderName );

			// Reference.Dat ���ϰ� Data.Bak ������ ����
			if( !stricmp( pstrFolderName, "root" ) )
			{
				sprintf( strDataFilePath, "%s", Packing_Data_Backup_FileName);
				sprintf( strReferenceFilePath, "%s", Packing_Reference_FileName);
			}
			else
			{
				sprintf( strDataFilePath, "%s\\%s", pstrFolderName, Packing_Data_Backup_FileName);
				sprintf( strReferenceFilePath, "%s\\%s", pstrFolderName, Packing_Reference_FileName);

				m_csAuPackingManager.CreateFolder( pstrFolderName);
			}
			
			fpBakDat = fopen( strDataFilePath, "rb+" );
			if ( !fpBakDat )
				fpBakDat = fopen(strDataFilePath, "rb+");

			fpReference = fopen( strReferenceFilePath, "ab" );
			if ( !fpReference )
				fpReference = fopen( strReferenceFilePath, "ab" );

			if ( fpBakDat && fpReference )
				bRefBakReallyExist = true;

			// ���� �����͸� ���������� �̵�
			if ( fpReference )
				fseek(fpReference, 0, SEEK_END);
		}
		
		// ���������� �ٹδ�
		for( int lCounter = 0; lCounter < lMaxFileCount; lCounter++ )
		{
			bool			bDone = true;
			int				lOperation;
			int				lFileNameLength;
			int				lFileSize;				
			char			pstrFileName[512];

			fread( &bPacking, 1, sizeof(bool), fpJZP );
			fread( &lOperation, 1, sizeof(int), fpJZP );
			fread( &lFileNameLength, 1, sizeof(int), fpJZP );
			memset( pstrFileName, 0, lFileNameLength );
			fread(pstrFileName, 1, lFileNameLength, fpJZP );

			if ( lOperation == Packing_Operation_Add )
			{
				//FileSize
				fread( &lFileSize, 1, sizeof(int), fpJZP );

				// �޸�Ǯ ó��
				if ( lFileSize > lMemBlockSize )
				{
					ASSERT(pTempMemPool);
					delete [] pTempMemPool;

					int lNewBlockCnt = lFileSize / nMEM_BLOCK_SIZE;
					if ( (lFileSize % nMEM_BLOCK_SIZE) > 0 )
						lNewBlockCnt++;

					pTempMemPool = new char[lNewBlockCnt * nMEM_BLOCK_SIZE];
					if ( !pTempMemPool )
						return false;

					lMemBlockSize = lNewBlockCnt * nMEM_BLOCK_SIZE;
				}

				//Packing�̸� ����Ʈ�� �ִ´�
				if ( bPacking == true )
				{						
					// PatchFileInfo ��������
					pPatchFileInfoArray[lCounter].m_bPacking = bPacking;
					pPatchFileInfoArray[lCounter].m_nOperation = lOperation;
					pPatchFileInfoArray[lCounter].m_nNameLength = lFileNameLength;
					pPatchFileInfoArray[lCounter].m_nSize = lFileSize;
					pPatchFileInfoArray[lCounter].m_lFilePos = ftell(fpJZP);
					pPatchFileInfoArray[lCounter].m_pName = new char[pPatchFileInfoArray[lCounter].m_nNameLength];
					memcpy(pPatchFileInfoArray[lCounter].m_pName, pstrFileName, lFileNameLength);

					//FileData
					fseek(fpJZP, lFileSize, SEEK_CUR);

					bDone = false;
					
					char*			pstrData;
					int				lFileNameSize;
					
					//�̸��� �����ͷ� �־��ش�.
					lFileNameSize = (int) strlen(pstrFileName) + 1;
					pstrData = new char[lFileNameSize];
					memset( pstrData, 0, lFileNameSize );
					strcat( pstrData, pstrFileName );
					
					csApAdminAdd.AddObject( (void **)&pstrData, _strlwr(pstrFileName) );

#ifdef _DEBUG
					char strDebug[ 256 ] = { 0, };
					sprintf_s( strDebug, sizeof( char ) * 256, "-- Patch into %s\reference.Dat : %s\n", pstrFolderName, pstrFileName );
					OutputDebugString( strDebug );
#endif
				}
				//Packing�� �ƴϸ� �ش� ������ �׳� ������ �����Ѵ�.
				else
				{
					//FileData
					fread( pTempMemPool, 1, lFileSize, fpJZP );

#ifdef _DEBUG
					sprintf(pDbgMsg, "\tExtracted file name : %s\\%s\n", pstrFolderName, pstrFileName);
					OutputDebugString(pDbgMsg);
#endif

					if ( !strcmpi( "root", pstrFolderName ) )
					{
						//���� ��ġŬ���̾�Ʈ�� Update�ϴ°���� �ڽ��� �ٸ������� �����Ѵ�.
						//. 2006. 3. 20. nonstopdj
						//. Ȯ���ڸ� ".exe" -> ".bak"�� �ٲ㼭 �����Ѵ�.

						if( !stricmp( pstrFileName, m_exeName.c_str() ) )
						{
							m_bPatchClient = true;
							sprintf(pstrFileName, BakName.c_str() );
						}

						m_cCompress.decompressMemory( pTempMemPool, lFileSize, "", pstrFileName );
					}
					else
					{
						char			strDestDir[TEMP_STRING_LENGTH];

						sprintf( strDestDir, "%s\\%s", pstrFolderName, pstrFileName );

						m_cCompress.decompressMemory( pTempMemPool, lFileSize, "", strDestDir );

						if ( g_pLogger )
							g_pLogger->Log("_Added_", strDestDir);
					}
				}
			}
			// Operation�� remove �� �����
			else
			{
				CHAR	szFileFullName[ MAX_PATH ];
				sprintf_s( szFileFullName , MAX_PATH , "%s\\%s\\%s" , strCurrentDir , pstrFolderName , pstrFileName );
				FILE*	pTempFile	=	fopen( szFileFullName , "rb" );

				if ( !pTempFile )
				{						
					// Packing ����Ÿ������ remove �� ���ϵ��� ����Ѵ�
					char*		pstrData;
					int			lFileNameSize;
					lFileNameSize = (int) strlen(pstrFileName)+1;							
					pstrData = new char[lFileNameSize];
					memset( pstrData, 0, lFileNameSize );
					strcat( pstrData, pstrFileName );

					//�̸��� �����ͷ� �־��ش�.
					csApAdminRemove.AddObject( (void **)&pstrData, _strlwr(pstrFileName) );
				}
				else
				{
					fclose( pTempFile );
					// Packing ������ �ƴϹǷ� ���� ������ �����Ѵ�
					char			strDestDir[TEMP_STRING_LENGTH];

					if( !stricmp( "root", pstrFolderName ) )
					{
						sprintf( strDestDir, "%s", pstrFileName );
					}
					else
					{
						sprintf( strDestDir, "%s\\%s", pstrFolderName, pstrFileName );
					}

					DeleteFile( strDestDir );
				}

				
			}

			if ( bDone )
			{
				// ����
				lCurFileProceed++;
				m_pcCustomProgressRecvBlock->SetCurrentCount(lCounter+1);
				m_pcCustomProgressRecvBlock->SetMaxCount(lMaxFileCount);

				// �Ʒ���
				m_iCurrentPatchFiles++;
				m_pcCustomProgressRecvFile->SetCurrentCount( m_iCurrentPatchFiles );
				m_pcCustomProgressRecvFile->SetMaxCount( m_iTotalPatchFiles);
				
				
				MultiByteToWideChar(CP_ACP, 0, pstrFileName, (int) strlen(pstrFileName) + 1, strFileNameW, TEMP_STRING_LENGTH);
				swprintf( strMsg, strMsgFile, strFileNameW);
				InvalidateStatic( m_pcStaticDetailInfo, strMsg);

				swprintf( strMsg, strMsgProgress, m_iCurrentPatchFiles, m_iTotalPatchFiles, strPatchFileNameW );
				InvalidateStatic( m_pcStaticStatus, strMsg);

				m_phParentCWnd->InvalidateRect( m_pcCustomProgressRecvFile->GetRect(), FALSE );
				m_phParentCWnd->InvalidateRect( m_pcCustomProgressRecvBlock->GetRect(), FALSE );
															
			}
		}

		if ( g_pLogger )
			g_pLogger->Flush();

		// ���� Reference.Bak ������ �о���δ�.		
		if ( bRefBakReallyExist )
		{
			m_csAuPackingManager.ReadBakReference(pstrFolderName, &csApAdminAdd, &csApAdminRemove, 
				&csApAdminBakNormal, &csApAdminBakEmpty);
		}


		if ( csApAdminAdd.GetObjectCount() > 0 || csApAdminRemove.GetObjectCount() > 0 )
		{
			
			int			lOperation;
			int			lFileNameLength;
			int			lFileSize;				
			char		pstrFileName[256];

			ZeroMemory(pstrFileName, 256);

			// �ٽ� ��� ���Ͽ� ���ؼ�...
			for( int lCounter = 0; lCounter < lMaxFileCount; lCounter++ )
			{

				bPacking		= pPatchFileInfoArray[lCounter].m_bPacking;
				lOperation		= pPatchFileInfoArray[lCounter].m_nOperation;
				lFileNameLength = pPatchFileInfoArray[lCounter].m_nNameLength;

				if( lOperation == Packing_Operation_Add )
				{

					memcpy(pstrFileName, pPatchFileInfoArray[lCounter].m_pName, lFileNameLength);
					lFileSize = pPatchFileInfoArray[lCounter].m_nSize;

					// �޸�Ǯ ó��
					if ( lFileSize > lMemBlockSize )
					{
						ASSERT(pTempMemPool);
						delete [] pTempMemPool;

						int lNewBlockCnt = lFileSize / nMEM_BLOCK_SIZE;
						if ( (lFileSize % nMEM_BLOCK_SIZE) > 0 )
							lNewBlockCnt++;

						pTempMemPool = new char[lNewBlockCnt * nMEM_BLOCK_SIZE];
						if( !pTempMemPool )
							return false;

						lMemBlockSize = lNewBlockCnt * nMEM_BLOCK_SIZE;
					}

					//Packing�̸� ����Ʈ�� �ִ´�
					if( bPacking == true )
					{
						int	lExtractedFileSize;

						// ������ �������� �̵�
						fseek(fpJZP, pPatchFileInfoArray[lCounter].m_lFilePos, SEEK_SET);

						//FileData
						fread( pTempMemPool, 1, lFileSize, fpJZP );

						//�켱 �о����� Ǯ���.
						// ������ �ƴ� �޸𸮷� Ǯ���.
						char* pDecompMem = NULL;
						lExtractedFileSize = m_cCompress.decompressMemory(pTempMemPool, lFileSize, &pDecompMem);
						ASSERT(lExtractedFileSize > 0);
						ASSERT(pDecompMem != NULL);

						CPackingFile* pOverwriteInfo = NULL;
						
						pOverwriteInfo = SearchBakPackingEmptyList(pstrFileName, 
							lExtractedFileSize, &csApAdminBakEmpty, &lGarbageDataSize);

						if ( pOverwriteInfo )
						{
							bool bProcessed = OverwritePackingData(fpReference, fpBakDat, pstrFolderName, pOverwriteInfo, 
								pstrFileName, pDecompMem, lExtractedFileSize);

							if ( bProcessed )
							{
								// ����� Empyt ���� ����
								csApAdminBakEmpty.RemoveObject(pOverwriteInfo->m_pstrFileName);
								
								delete pOverwriteInfo;
							}
							else
							{

								((CPatchClientDlg*) m_phParentCWnd)->CloseDlg();
								exit(-1);
								return FALSE;
							}
						}
						else
						{	
							
							if ( fpBakDat )						
								fclose(fpBakDat);								
							
							// Attach ó��
							if ( AttachAddedPackingData(fpReference, pstrFolderName, pstrFileName, lExtractedFileSize, pDecompMem)  )
							{

							}
							else
							{

								((CPatchClientDlg*) m_phParentCWnd)->CloseDlg();

								exit(-1);
								return FALSE;
							}

							
							// Dat ���� �����
							if( !stricmp( pstrFolderName, "root" ) )
								sprintf( strDataFilePath, "%s", Packing_Data_Backup_FileName);
							else
								sprintf( strDataFilePath, "%s\\%s", pstrFolderName, Packing_Data_Backup_FileName);

							fpBakDat = fopen( strDataFilePath, "rb+" );
							ASSERT(fpBakDat);
							if ( fpBakDat == NULL )
							{
								::MessageBox(NULL, "ERROR", "Patch failed!", MB_OK);
								exit(-1);
								return FALSE;
							}
						}
						

						lCurrentPosition += lExtractedFileSize;
						lPackedFileCount++;							

						
						// ���α׷����� repaint

						// ����
						lCurFileProceed++;							
						m_pcCustomProgressRecvBlock->SetCurrentCount(lCounter+1);
						m_pcCustomProgressRecvBlock->SetMaxCount(lMaxFileCount);
						
						// �Ʒ���
						m_iCurrentPatchFiles++;
						m_pcCustomProgressRecvFile->SetCurrentCount( m_iCurrentPatchFiles );
						m_pcCustomProgressRecvFile->SetMaxCount( m_iTotalPatchFiles);
						
						MultiByteToWideChar(CP_ACP, 0, pstrFileName, (int) strlen(pstrFileName) + 1, strFileNameW, TEMP_STRING_LENGTH);
						swprintf( strMsg, strMsgFile, strFileNameW );
						InvalidateStatic( m_pcStaticDetailInfo, strMsg, false );

						swprintf( strMsg, strMsgProgress, m_iCurrentPatchFiles, m_iTotalPatchFiles, strPatchFileNameW );
						InvalidateStatic( m_pcStaticStatus, strMsg, false );

						m_phParentCWnd->InvalidateRect( m_pcCustomProgressRecvBlock->GetRect(), FALSE );
					}
				}
			}

			MultiByteToWideChar(CP_ACP, 0, pstrFileName, (int) strlen(pstrFileName) + 1, strFileNameW, TEMP_STRING_LENGTH);
			swprintf( strMsg, strMsgFile, strFileNameW );
			InvalidateStatic( m_pcStaticDetailInfo, strMsg);

			swprintf( strMsg, strMsgProgress, m_iCurrentPatchFiles, m_iTotalPatchFiles, strPatchFileNameW );
			InvalidateStatic( m_pcStaticStatus, strMsg);
		}	

		if ( g_pLogger )
			g_pLogger->Flush();
		
		// ���� Normal ����Ʈ�� ���������� Reference.Dat �� �����Ѵ�
		if ( bPackingFolder == true && csApAdminBakNormal.GetObjectCount() > 0 )
			WriteNormalPackingRefInfo(fpReference, &csApAdminBakNormal);
		
		if ( fpBakDat )
			fclose(fpBakDat);

		if (fpReference )
			fclose(fpReference);

		if ( bPackingFolder == true )
		{
			m_csAuPackingManager.CompleteCurrentFolderProcess(pstrFolderName, lPackedFileCount + csApAdminBakNormal.GetObjectCount());
		}

		//ApAdminAdd, ApAdminRemove�� ���� �����͸޸𸮸� ������.
		char**	pstrTempData;
		CPackingFile** pstrTempPackingFile;
		int		lIndex;

		lIndex = 0;
		for( pstrTempData = (char **)csApAdminAdd.GetObjectSequence(&lIndex); pstrTempData; pstrTempData = (char **)csApAdminAdd.GetObjectSequence(&lIndex) )
		{
			delete [] (*pstrTempData);
		}

		lIndex = 0;
		for( pstrTempData = (char **)csApAdminRemove.GetObjectSequence(&lIndex); pstrTempData; pstrTempData = (char **)csApAdminRemove.GetObjectSequence(&lIndex) )
		{
			delete [] (*pstrTempData);
		}

		lIndex = 0;
		for( pstrTempPackingFile = (CPackingFile **)csApAdminBakNormal.GetObjectSequence(&lIndex); pstrTempPackingFile; pstrTempPackingFile = (CPackingFile **)csApAdminBakNormal.GetObjectSequence(&lIndex) )
		{				
			if ( pstrTempPackingFile && *pstrTempPackingFile )
				delete (*pstrTempPackingFile);
		}

		lIndex = 0;
		for( pstrTempPackingFile = (CPackingFile **)csApAdminBakEmpty.GetObjectSequence(&lIndex); pstrTempPackingFile; pstrTempPackingFile = (CPackingFile **)csApAdminBakEmpty.GetObjectSequence(&lIndex) )
		{				
			if ( pstrTempPackingFile && *pstrTempPackingFile )
				delete (*pstrTempPackingFile);
		}

		csApAdminAdd.RemoveObjectAll();
		csApAdminRemove.RemoveObjectAll();
		csApAdminBakNormal.RemoveObjectAll();
		csApAdminBakEmpty.RemoveObjectAll();

		if ( pPatchFileInfoArray )
			delete [] pPatchFileInfoArray;
	}

	fclose(fpJZP);

	// �޸�Ǯ ����
	if( pTempMemPool != NULL )
	{
		delete [] pTempMemPool;
		pTempMemPool = NULL;
	}

	return true;
}
//@}

bool CPatchClientLib::DecompressJZP( char *pstrTempFolderName, char *pstrJzpFileName )
{
	FILE			*file;
	bool			bResult;
	bool			bPacking;
	char			strCurrentDir[TEMP_STRING_LENGTH];
	char			strTempDir[TEMP_STRING_LENGTH];
	char			strJZPFileName[TEMP_STRING_LENGTH];
	int				lVersion;
	int				lMaxFolderCount;
	int				lFolderCount;

	int				lFoleCount;
	int				lTotalFileCount;

	int				lFolderNameLen;
	char			pstrFolderName[512];
	//@}
	wchar_t			strMsgFile[TEMP_STRING_LENGTH];
	wchar_t			strMsgProgress[TEMP_STRING_LENGTH];
	wchar_t			strMsg[TEMP_STRING_LENGTH];
	wchar_t			strFileNameW[TEMP_STRING_LENGTH];

	::LoadStringW(g_resInst, IDS_MSG_APPLY_FILE, strMsgFile, TEMP_STRING_LENGTH);
	::LoadStringW(g_resInst, IDS_MSG_APPLY_PROGRESS, strMsgProgress, TEMP_STRING_LENGTH);

	bResult = false;

	GetCurrentDirectory( sizeof(strCurrentDir), strCurrentDir );
    sprintf( strTempDir, "%s\\%s", strCurrentDir, pstrTempFolderName );

	if( pstrJzpFileName )
	{
		sprintf(strJZPFileName , "%s\\%s\\%s", strCurrentDir, pstrTempFolderName, pstrJzpFileName );
		MultiByteToWideChar( CP_ACP, 0, pstrJzpFileName, (int) strlen(pstrJzpFileName) + 1, strFileNameW, TEMP_STRING_LENGTH);

		file = fopen( strJZPFileName, "rb" );

		if( file )
		{
			//����
			fread( &lVersion, 1, sizeof(int), file );

			//��ü ���ϰ����� �о��.
			fread( &lTotalFileCount, 1, sizeof(int), file );

			lFoleCount = 0;
			m_pcCustomProgressRecvBlock->SetMaxCount( lTotalFileCount );
			m_pcCustomProgressRecvBlock->SetCurrentCount( 0 );
			m_phParentCWnd->InvalidateRect( m_pcCustomProgressRecvBlock->GetRect(), FALSE );			
			

			//���� ����
			fread( &lMaxFolderCount, 1, sizeof(int), file );
			
			char*	pstrBuffer = NULL;
			#define nMEM_BLOCK_SIZE		(1024*1024)	// 1M
			pstrBuffer = new char[nMEM_BLOCK_SIZE];
			ASSERT(pstrBuffer);
			long lMemBlockSize = nMEM_BLOCK_SIZE;

			ApAdmin			csApAdminAdd;
			ApAdmin			csApAdminRemove;

			ApAdmin* pCurrentFolderAdmin = m_csAuPackingManager.GetFolderInfo();
			if( pCurrentFolderAdmin )
			{
				int nCurrentFolderCount = pCurrentFolderAdmin->GetObjectCount();
				csApAdminAdd.InitializeObject( sizeof(char *), ( lMaxFolderCount + nCurrentFolderCount ) * 2 );
				csApAdminRemove.InitializeObject( sizeof(char *), ( lMaxFolderCount + nCurrentFolderCount ) * 2 );
			}
			else
			{
				csApAdminAdd.InitializeObject( sizeof(char *), lMaxFolderCount * 2 );
				csApAdminRemove.InitializeObject( sizeof(char *), lMaxFolderCount * 2 );
			}

			for( lFolderCount=0; lFolderCount < lMaxFolderCount; lFolderCount++ )
			{
				int					lMaxFileCount;
				int					lCurrentPosition;
				int					lPackedFileCount;
				bool				bPackingFolder;
				
				lCurrentPosition = 0;
				lPackedFileCount = 0;

				//�����̸� ���̷ε�
				fread( &lFolderNameLen, 1, sizeof(int), file );

				//�����̸� �ε�
				memset( pstrFolderName, 0, lFolderNameLen );
				fread( pstrFolderName, 1, lFolderNameLen, file );

				//���ϰ������
				fread( &lMaxFileCount, 1, sizeof(int), file );

				//�� ������ ��ŷ�� �ϴ� ������ �����ϴ°�?
				fread( &bPackingFolder, 1, sizeof(bool), file );

				if( bPackingFolder == true )
				{
					//���� Data.Dat Refrence.Dat�� �ִٸ� �̸��� �ٲ��ش�.
                    m_csAuPackingManager.ReadyPacking( pstrFolderName );
				}

				FILE* fpDat = NULL;
				FILE* fpReference = NULL;

				char strDataFilePath[255];
				char strReferenceFilePath[255];

				if( !stricmp( pstrFolderName, "root" ) )
				{
					sprintf( strDataFilePath, "%s", Packing_Data_FileName );
					sprintf( strReferenceFilePath, "%s", Packing_Reference_FileName );					
				}
				else
				{
					sprintf( strDataFilePath, "%s\\%s", pstrFolderName, Packing_Data_FileName );
					sprintf( strReferenceFilePath, "%s\\%s", pstrFolderName, Packing_Reference_FileName );
					m_csAuPackingManager.CreateFolder( pstrFolderName );
				}	

				fpDat = fopen( strDataFilePath, "ab" );
				fpReference = fopen( strReferenceFilePath, "ab" );
				
				//���� �� ���������� �̵��Ѵ�.
				fseek( fpDat, 0, SEEK_END );
				fseek( fpReference, 0, SEEK_END );
				
				for( int lCounter=0; lCounter < lMaxFileCount; lCounter++ )
				{
					int				lOperation;
					int				lFileNameLength;
					int				lFileSize;	
					char			pstrFileName[512];

					fread( &bPacking, 1, sizeof(bool), file );
					fread( &lOperation, 1, sizeof(int), file );
					fread( &lFileNameLength, 1, sizeof(int), file );
					memset( pstrFileName, 0, lFileNameLength );
					fread(pstrFileName, 1, lFileNameLength, file );

					if( lOperation == Packing_Operation_Add )
					{
						//FileSize
						fread( &lFileSize, 1, sizeof(int), file );

						if ( lFileSize > lMemBlockSize )
						{
							ASSERT(pstrBuffer);
							delete [] pstrBuffer;

							int lNewBlockCnt = lFileSize / nMEM_BLOCK_SIZE;
							if ( (lFileSize % nMEM_BLOCK_SIZE) > 0 )
								lNewBlockCnt++;

							pstrBuffer = new char[lNewBlockCnt * nMEM_BLOCK_SIZE];
							if( !pstrBuffer )
								return false;

							lMemBlockSize = lNewBlockCnt * nMEM_BLOCK_SIZE;
						}						
	
						//FileData
						fread( pstrBuffer, 1, lFileSize, file );

						//Packing�̸� Data.Dat�� �߰��Ѵ�.
						if( bPacking == true )
						{
							int				lExtractedFileSize;							
							char			*pstrData;
							int				lFileNameSize;

							char* pDecompMem = NULL;
							lExtractedFileSize = m_cCompress.decompressMemory(pstrBuffer, lFileSize, &pDecompMem);
							ASSERT(lExtractedFileSize > 0);
							ASSERT(pDecompMem != NULL);

							//�̸��� �����ͷ� �־��ش�.
							lFileNameSize = (int) strlen(pstrFileName)+1;							
							pstrData = new char[lFileNameSize];
							memset( pstrData, 0, lFileNameSize );
							strcat( pstrData, pstrFileName );
							csApAdminAdd.AddObject( (void **)&pstrData, pstrFileName );
							m_csAuPackingManager.BuildPackingFile( pstrTempFolderName, pstrFolderName, pstrFileName, 
								lCurrentPosition, lExtractedFileSize, fpDat, fpReference, pDecompMem);

							lCurrentPosition += lExtractedFileSize;
							lPackedFileCount++;

						}
						//Packing�� �ƴϸ� �ش� ������ �׳� ������ �����Ѵ�.
						else
						{
							if( !strcmpi( "root", pstrFolderName ) )
							{
								//���� ��ġŬ���̾�Ʈ�� Update�ϴ°���� �ڽ��� �ٸ������� �����Ѵ�.
								//. Ȯ���ڸ� ".exe" -> ".bak"�� �ٲ㼭 �����Ѵ�.
#ifndef _TEST_SERVER_
								if( !stricmp( pstrFileName, m_exeName.c_str() ) )
								{
									m_bPatchClient = true;
									sprintf(pstrFileName, BakName.c_str() );
								}
#else
								if( !stricmp( pstrFileName, "ArchlordTest.exe" ) )
								{
									m_bPatchClient = true;
									sprintf(pstrFileName, "ArchlordTest.bak");

									/*if( MoveFile( "ArchlordTest.exe", "temp\\Archlord.temp" ) )
									{
									m_bPatchClient = true;
									}*/
								}
#endif // _TEST_SERVER_

								m_cCompress.decompressMemory( pstrBuffer, lFileSize, "", pstrFileName );
							}
							else
							{
								char			strDestDir[TEMP_STRING_LENGTH];
								sprintf( strDestDir, "%s\\%s", pstrFolderName, pstrFileName );
								m_cCompress.decompressMemory( pstrBuffer, lFileSize, "", strDestDir );
							}
						}
						//�߰��� �������� ������ �����. Temp������ �ڵ����� �������� �������� �Ƿ���
					}
					else
					{
						if( bPacking == true )
						{							
							char		*pstrData;
							int			lFileNameSize;
							lFileNameSize = (int) strlen(pstrFileName)+1;							
							pstrData = new char[lFileNameSize];
							memset( pstrData, 0, lFileNameSize );
							strcat( pstrData, pstrFileName );

							//�̸��� �����ͷ� �־��ش�.
							csApAdminRemove.AddObject( (void **)&pstrData, pstrFileName );
						}
						else
						{
							char			strDestDir[TEMP_STRING_LENGTH];

							if( !stricmp( "root", pstrFolderName ) )
							{
								sprintf( strDestDir, "%s", pstrFileName );
							}
							else
							{
								sprintf( strDestDir, "%s\\%s", pstrFolderName, pstrFileName );
							}

							DeleteFile( strDestDir );
						}
					}

					// �Ʒ��� ���α׷�����
					m_iCurrentPatchFiles++;								
					m_pcCustomProgressRecvFile->SetMaxCount( 0);
					m_pcCustomProgressRecvFile->SetCurrentCount( 0);
					m_phParentCWnd->InvalidateRect( m_pcCustomProgressRecvFile->GetRect(), FALSE );
								

					// ���� ���α׷�����			
					m_pcCustomProgressRecvBlock->SetCurrentCount(lCounter);
					m_pcCustomProgressRecvBlock->SetMaxCount(lMaxFileCount);					
					m_phParentCWnd->InvalidateRect( m_pcCustomProgressRecvBlock->GetRect(), FALSE );					
					
					swprintf( strMsg, strMsgFile, strFileNameW );
					InvalidateStatic( m_pcStaticDetailInfo, strMsg );

					swprintf( strMsg, strMsgProgress, m_iCurrentPatchFiles, m_iTotalPatchFiles, strFileNameW );
					InvalidateStatic( m_pcStaticStatus, strMsg );

					if( m_iCurrentPatchFiles >= m_iTotalPatchFiles )
					{
						break;
					}
				}
				
				fclose(fpDat);
				fclose(fpReference);

				if( bPackingFolder == true )
				{
					//�ش������� ����� Data.tmp, Refrence.tmp �� ������ Data.Dat, Refrence.Dat�� �߰��Ѵ�.					
					m_csAuPackingManager.CompletePacking( pstrFolderName, lCurrentPosition, lPackedFileCount, &csApAdminAdd, &csApAdminRemove );					
				}

				//ApAdminAdd, ApAdminRemove�� ���� �����͸޸𸮸� ������.
				char		**pstrTempData;
				int			lIndex;

				lIndex =0;
				for( pstrTempData = (char **)csApAdminAdd.GetObjectSequence(&lIndex); pstrTempData; pstrTempData = (char **)csApAdminAdd.GetObjectSequence(&lIndex) )
				{
					delete [] (*pstrTempData);
				}

				lIndex =0;
				for( pstrTempData = (char **)csApAdminRemove.GetObjectSequence(&lIndex); pstrTempData; pstrTempData = (char **)csApAdminRemove.GetObjectSequence(&lIndex) )
				{
					delete [] (*pstrTempData);
				}

				csApAdminAdd.RemoveObjectAll();
				csApAdminRemove.RemoveObjectAll();
			}

			if( pstrBuffer != NULL )
			{
				delete [] pstrBuffer;
				pstrBuffer = NULL;
			}
			fclose( file );
		}
	}

	return bResult;
}

bool CPatchClientLib::GetInternetFile(char *pstrURL )
{
    CInternetSession session;
    CInternetFile *file=NULL;

	bool			bResult;
    BOOL			bNoError = TRUE;

	bResult = true;
    
	try
    {
        file=(CInternetFile *)session.OpenURL(pstrURL); 
    }
    catch(CInternetException * pEx)
    {
        TCHAR    lpszError[TEMP_STRING_LENGTH];
        pEx->GetErrorMessage( lpszError, TEMP_STRING_LENGTH, NULL );
        bNoError = FALSE;

        return false;
    }

    if (file) 
	{
		FILE			*fp;

        CString			data; 
		CString			cFileName;

		char			*pstrData;
		int				iFileSize;
		int				iBufferSize;
		int				iReadByte;
		int				iTotalRecvByte;

		iTotalRecvByte = 0;

		//����ũ�⸦ �����ش�.
		iBufferSize = 4096;

		//������ ����� �˾Ƴ���.
		iFileSize = (int) file->GetLength();
		m_pcCustomProgressRecvFile->SetMaxCount( iFileSize );

		//���� �̸��� ����.
		cFileName = file->GetFileName();

        file->SetReadBufferSize(iBufferSize);

		fp = fopen( cFileName.GetBuffer(0), "wb" );

		if( fp != NULL )
		{
			pstrData = new char[iBufferSize];

			for(;;) 
			{ 
				iReadByte = file->Read( pstrData, iBufferSize );

				iTotalRecvByte+=iReadByte;

				if( iReadByte != 0 )
				{
					int				iWriteBytes;

					iWriteBytes = (int) fwrite( pstrData, 1, iReadByte, fp );

					m_pcCustomProgressRecvFile->SetCurrentCount( iTotalRecvByte );
					m_phParentCWnd->InvalidateRect( m_pcCustomProgressRecvFile->GetRect(), FALSE );
					

					if( iWriteBytes != iReadByte )
					{
//						MessageBox( "�ϵ� ��ũ�� ����?" );
						bResult = false;
						break;
					}
				}
				else
				{
					break;
				}
			} 

			delete [] pstrData;
			fclose( fp );
		}
		else
		{
			//MessageBox( "������ ���� �����ϴ�." );
			bResult = false;
		}

        data.ReleaseBuffer();
		cFileName.ReleaseBuffer();

        file->Close();

		delete file;

		session.Close();
    }
    else 
	{
        bResult = false;
    }

    return bResult;
}

//For Static Display
bool CPatchClientLib::InvalidateStatic( CStatic *pcStatic, wchar_t *pstrText, bool bInvalidateRect )
{	
	if (!pcStatic->m_hWnd)
		return true;

	bool			bResult;
	CRect			cRect;

	bResult = false;

	// �߱� Patch Client���� ���ڿ��� ������ ���� ������ ���� �߰�
	static_cast< CNewStatic* >(pcStatic)->SetText( pstrText );

	if ( bInvalidateRect )
	{
		pcStatic->GetClientRect(cRect);
		pcStatic->ClientToScreen(cRect);
		m_phParentCWnd->ScreenToClient(cRect);
		m_phParentCWnd->InvalidateRect( &cRect, FALSE );
		
	}

	return bResult;
}

char *CPatchClientLib::GetFileName( char *pstrFilePath )
{
	char			*pstrResult;
	int				iLen;
	bool			bFound;

	bFound = false;
	pstrResult = NULL;

	if( pstrFilePath != NULL )
	{
		iLen = (int) strlen( pstrFilePath );

		for( int i=iLen-1; i >= 0; i-- )
		{
			if( '\\' == pstrFilePath[i] )
			{
				pstrResult = &pstrFilePath[i+1];
				bFound = true;
				break;
			}
		}

		if( bFound == false )
		{
			pstrResult = pstrFilePath;
		}
	}

	return pstrResult;
}

bool CPatchClientLib::SetConfigFile()
{

	return true;
}

bool CPatchClientLib::ReadConfig(char *pszFileName)
{
	AuIniManagerA	csIni;
	char *			pszURL;
	char *			pszRegBase;

	csIni.SetPath(pszFileName);

	if (!csIni.ReadFile(0, TRUE))
		return false;

	if (m_szAreaDownloadURL)
	{
		delete [] m_szAreaDownloadURL;
		m_szAreaDownloadURL	= NULL;
	}

	pszURL	= csIni.GetValue(PATCH_CONFIG_AREA_URL_SECTION, PATCH_CONFIG_AREA_URL_KEY);
	if (pszURL)
	{
		m_szAreaDownloadURL = new char [strlen(pszURL) + 1];
		if (!m_szAreaDownloadURL)
			return false;

		strcpy(m_szAreaDownloadURL, pszURL);
	}

	pszRegBase = csIni.GetValue(PATCH_CONFIG_AREA_REG_SECTION, PATCH_CONFIG_AREA_REG_BASE_KEY);
	if (pszRegBase)
	{
		m_cPatchReg.SetRegKeyBase(pszRegBase);
	}

	return true;
}

bool CPatchClientLib::DownloadAreaConfig()
{
	if (!m_szAreaDownloadURL)
		return false;

	if (DownloadFile(m_szAreaDownloadURL, PATCH_CONFIG_AREA_FILE))
	{
		return ReadAreaConfig();
	}

	return false;
}

bool CPatchClientLib::ReadAreaConfig()
{
	AuAutoLock	csLock(m_csCSArea);

	AuIniManagerA	csIni;
	int				lIndex;
	int				lType;
	char *			pszName;

	csIni.SetPath(PATCH_CONFIG_AREA_FILE);
	if (!csIni.ReadFile(0, TRUE))
		return false;

	PulseEvent(m_hEventForNewSocket);

	if (m_pcsAreaInfo)
		delete [] m_pcsAreaInfo;

	if (m_phEvents)
		delete [] m_phEvents;

	m_lArea = csIni.GetNumSection();
	m_pcsAreaInfo = new AgpdPatchAreaInfo [m_lArea];
	if (!m_pcsAreaInfo)
		return false;

	m_phEvents = new HANDLE [m_lArea + 1];
	if (!m_phEvents)
		return false;

	for (lIndex = 0; lIndex < m_lArea; ++lIndex)
	{
		pszName = csIni.GetSectionName(lIndex);

		lType = AgpdPatchAreaInfo::TYPE_NORMAL;

		if (!stricmp(csIni.GetValue(pszName, PATCH_AREA_TYPE), PATCH_AREA_TYPE_NEW))
			lType = AgpdPatchAreaInfo::TYPE_NEW;

		if (m_pcsAreaInfo[lIndex].SetPatchInfo(
												pszName,
												csIni.GetValue(pszName, PATCH_AREA_PATCH_SERVER),
												csIni.GetValueI(pszName, PATCH_AREA_PATCH_PORT),
												csIni.GetValue(pszName, PATCH_AREA_LOGIN_SERVER),
												csIni.GetValueI(pszName, PATCH_AREA_LOGIN_PORT),
												lType,
												csIni.GetValueI(pszName, PATCH_AREA_CATEGORY)
												))
		{

		}
		else
		{
			return false;
		}
	}

	return true;
}

AgpdPatchAreaInfo *	CPatchClientLib::GetArea(char *pszName)
{
	AuAutoLock	csLock(m_csCSArea);

	if (!pszName || !m_pcsAreaInfo)
		return NULL;

	int lIndex;

	for (lIndex = 0; lIndex < m_lArea; ++lIndex)
	{
		if (!strcmp(m_pcsAreaInfo[lIndex].m_pszName, pszName))
		{
			return m_pcsAreaInfo + lIndex;
		}
	}

	return NULL;
}

bool CPatchClientLib::SetArea(char *pszName)
{
	AuAutoLock	csLock(m_csCSArea);

	if (!pszName || !m_pcsAreaInfo)
		return false;

	AgpdPatchAreaInfo*	pcsArea = GetArea( pszName );
	if( !pcsArea )
		return false;

	m_cPatchReg.SetPatchInfo( pcsArea->m_pszPatchServer, pcsArea->m_lPatchPort );
	m_cPatchReg.SetLoginInfo( pcsArea->m_pszLoginServer, pcsArea->m_lLoginPort );
	m_cPatchReg.BuildRegistry( m_cPatchReg.GetRegKeyBase() );

	m_strLoginInfo.Format( "%s%s:%d", PATCHCLIENT_LOGIN_SERVER, pcsArea->m_pszLoginServer, pcsArea->m_lLoginPort );

	return true;
}

int CPatchClientLib::GetAreaCount(int lCategory)
{
	AuAutoLock	csLock(m_csCSArea);

	if (!m_pcsAreaInfo)
		return -1;

	int lIndex;
	int lCount = 0;

	for (lIndex = 0; lIndex < m_lArea; ++lIndex)
	{
		if (m_pcsAreaInfo[lIndex].m_lCategory == lCategory)
			++lCount;
	}

	return lCount;
}

int CPatchClientLib::GetNewAreaCount()
{
	AuAutoLock	csLock(m_csCSArea);

	if (!m_pcsAreaInfo)
		return -1;

	int lIndex;
	int lCount = 0;

	for (lIndex = 0; lIndex < m_lArea; ++lIndex)
	{
		if (m_pcsAreaInfo[lIndex].m_eType == AgpdPatchAreaInfo::TYPE_NEW)
			++lCount;
	}

	return lCount;
}

AgpdPatchAreaInfo *	CPatchClientLib::GetNextArea(int lCategory, int *plIndex)
{
	AuAutoLock	csLock(m_csCSArea);

	if (*plIndex >= m_lArea)
		return NULL;

	for (; *plIndex < m_lArea; ++(*plIndex))
	{
		if (m_pcsAreaInfo[*plIndex].m_lCategory == lCategory)
		{
			++(*plIndex);
			return m_pcsAreaInfo + (*plIndex - 1);
		}
	}

	return NULL;
}

AgpdPatchAreaInfo * CPatchClientLib::GetNextNewArea(int *plIndex)
{
	AuAutoLock	csLock(m_csCSArea);

	if (*plIndex >= m_lArea)
		return NULL;

	for (; *plIndex < m_lArea; ++(*plIndex))
	{
		if (m_pcsAreaInfo[*plIndex].m_eType == AgpdPatchAreaInfo::TYPE_NEW)
		{
			++(*plIndex);
			return m_pcsAreaInfo + (*plIndex - 1);
		}
	}

	return NULL;
}

bool CPatchClientLib::StartGame( char* pPatchCodeString )
{
	char szFilePath[ 512 + 4096 + LENGTH_PATCH_CODE_STRING ] = { 0, };

	CHAR	Seps[]				=	".";
	CHAR	Sep2[]				=	" ";
	LPSTR	lpCommandLine		=	::GetCommandLine();
	CHAR*	Token				=	NULL;
	CHAR*	Token2				=	NULL;
	CHAR	strToken[ GAMSSTRING_LENGTH ];
	ZeroMemory( strToken , GAMSSTRING_LENGTH );
	strcpy_s( strToken , GAMSSTRING_LENGTH , lpCommandLine );


#ifdef _JPN
	_snprintf_s( szFilePath, SIZE_GAMESTRING + MAX_PATH, _TRUNCATE, "\"%s\" %s", m_strAlefClientFullPath, g_jAuth.GetGameString() );
#elif _CHN
	_snprintf_s( szFilePath, 512 + 4096, _TRUNCATE, "\"%s\"  ", "alefclient.exe" );
#elif _ENG

	if( !pPatchCodeString || strlen( pPatchCodeString ) <= 0 ) return false;

	Token = strtok( strToken, Seps ); 
	Token = strtok(NULL, Seps); 

	Token2 = strtok( Token , Sep2 );
	Token2 = strtok( NULL , Sep2 );

	if( Token2 )
		sprintf_s( szFilePath , 512 + 4096 + LENGTH_PATCH_CODE_STRING , "%s 1:%s 2:%s" , "alefclient.exe" , pPatchCodeString, Token2 );
	else
		sprintf_s( szFilePath , 512 + 4096 + LENGTH_PATCH_CODE_STRING , "%s 1:%s" , "alefclient.exe", pPatchCodeString );

#elif _KOR
	// �������� �̰��� �ϴ�.. ������ �۷ι��� �����ϰ� ����..
	if( !pPatchCodeString || strlen( pPatchCodeString ) <= 0 ) return false;

	Token = strtok( strToken, Seps ); 
	Token = strtok(NULL, Seps); 

	Token2 = strtok( Token , Sep2 );
	Token2 = strtok( NULL , Sep2 );

	if( Token2 )
		sprintf_s( szFilePath , 512 + 4096 + LENGTH_PATCH_CODE_STRING , "%s 1:%s 2:%s" , "alefclient.exe" , pPatchCodeString, Token2 );
	else
		sprintf_s( szFilePath , 512 + 4096 + LENGTH_PATCH_CODE_STRING , "%s 1:%s" , "alefclient.exe", pPatchCodeString );
#elif _TIW
	sprintf_s( szFilePath , 512 + 4096 + LENGTH_PATCH_CODE_STRING , "%s %s" , "alefclient.exe", pPatchCodeString );
#endif

	wchar_t strMsg[256] = {0, };
	//if( !CheckDXVersion() )
	//{
	//	::LoadStringW( g_resInst, IDS_ERROR_DIRECTX, strMsg, 256 );
	//	::MessageBoxW( m_phParentCWnd->m_hWnd, strMsg, g_szMessageTitle, MB_OK );

	//	//������ ���ͳ� URL��ũ�� �����ش�.
	//	ShellExecute(NULL, _T("open"), _T("IEXPLORE.EXE"), _T("http://www.archlord.com/data/driver.nhn"), NULL, SW_SHOW);
	//}
	//else
	{
		STARTUPINFO				si;
		ZeroMemory( &si, sizeof(si) );
		si.cb = sizeof(si);

		PROCESS_INFORMATION		pi;
		ZeroMemory( &pi, sizeof(pi) );

		if( !CreateProcess( NULL, szFilePath, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi ) )
		{
 			::LoadStringW( g_resInst, IDS_ERROR_RUN_CLIENT, strMsg, 256 );
			::MessageBoxW( m_phParentCWnd->m_hWnd, strMsg, g_szMessageTitle, MB_OK );
		}
	}

	return true;
}


void CPatchClientLib::ScanCrackedFiles( char *pstrSourceFolder )
{
	if( pstrSourceFolder && strlen( pstrSourceFolder ) )
	{
		BOOL		bPatchFileCheck = FALSE;		
		char		strFolder[255];
		sprintf( strFolder, "%s\\*.*", pstrSourceFolder );

		CFileFind	finder;
		BOOL		bWorking = finder.FindFile(strFolder);
		while (bWorking)
		{
			bWorking = finder.FindNextFile();

			if (finder.IsDots())
				continue;

			if (finder.IsDirectory())
			{
				//@{ 2006/10/17 burumal
				if ( finder.IsHidden() )
					continue;
				//@}

				CString str = finder.GetFilePath();
				//���丮�� �ش���丮�� Ÿ�� ����.

				ScanCrackedFiles( str.GetBuffer() );
			}
			//���丮�� �ƴ϶��?
			else
			{
				CString			strFilePath;
				CString			strFileName;
				char			*pstrCrackedFileName;
				char			strRecoverFileName[255];
				char			strDrv[255], strDir[255];

				strFilePath = finder.GetFilePath();
				strFileName = finder.GetFileName();

				bool bIsRefFileExist = false;
				bool bIsRefBakFileExist = false;
				bool bIsDatFileExist = false;
				bool bIsDatBakFileExist = false;				

				if ( bPatchFileCheck == FALSE )
				{
					bPatchFileCheck = TRUE;

					FILE* fpFile;
					pstrCrackedFileName = strFilePath.GetBuffer();
					_splitpath( pstrCrackedFileName, strDrv, strDir, NULL, NULL );

					sprintf( strRecoverFileName, "%s%s%s", strDrv, strDir, Packing_Data_FileName );
					if ( fpFile = fopen(strRecoverFileName, "rb") )
					{
						bIsDatFileExist = true;						
						fclose(fpFile);
					}

					sprintf( strRecoverFileName, "%s%s%s", strDrv, strDir, Packing_Data_Backup_FileName );
					if ( fpFile = fopen(strRecoverFileName, "rb") )
					{
						bIsDatBakFileExist = true;
						fclose(fpFile);
					}

					sprintf( strRecoverFileName, "%s%s%s", strDrv, strDir, Packing_Reference_FileName);
					if ( fpFile = fopen(strRecoverFileName, "rb") )
					{
						bIsRefFileExist = true;
						fclose(fpFile);
					}

					sprintf( strRecoverFileName, "%s%s%s", strDrv, strDir, Packing_Reference_Backup_FileName);
					if ( fpFile = fopen(strRecoverFileName, "rb") )
					{
						bIsRefBakFileExist = true;
						fclose(fpFile);
					}

					if ( bIsRefBakFileExist == false && 
						bIsRefFileExist == true && 
						bIsDatBakFileExist == true && 
						bIsDatFileExist == false )
					{					
						_splitpath( pstrCrackedFileName, strDrv, strDir, NULL, NULL );
						sprintf( strRecoverFileName, "%s%s%s", strDrv, strDir, Packing_Reference_FileName );
						DeleteFile(strRecoverFileName);
						sprintf( strRecoverFileName, "%s%s%s", strDrv, strDir, Packing_Data_FileName);
						DeleteFile(strRecoverFileName);
						sprintf( strRecoverFileName, "%s%s%s", strDrv, strDir, Packing_Data_Backup_FileName);
						DeleteFile(strRecoverFileName);
					}
				}

				
				if( !strFileName.CompareNoCase( Packing_Data_Backup_FileName ) )
				{
					pstrCrackedFileName = strFilePath.GetBuffer();

					_splitpath( pstrCrackedFileName, strDrv, strDir, NULL, NULL );

					sprintf( strRecoverFileName, "%s%s%s", strDrv, strDir, Packing_Data_FileName );

					DeleteFile( strRecoverFileName );
					MoveFile( pstrCrackedFileName, strRecoverFileName );
				}

				if( !strFileName.CompareNoCase( Packing_Reference_Backup_FileName ) )
				{
					pstrCrackedFileName = strFilePath.GetBuffer();

					_splitpath( pstrCrackedFileName, strDrv, strDir, NULL, NULL );

					sprintf( strRecoverFileName, "%s%s%s", strDrv, strDir, Packing_Reference_FileName );

					DeleteFile( strRecoverFileName );
					MoveFile( pstrCrackedFileName, strRecoverFileName );
				}

				if ( !strFileName.CompareNoCase(Packing_Data_FileName) )
				{	
					pstrCrackedFileName = strFilePath.GetBuffer();

					FILE* fpFile = fopen(pstrCrackedFileName, "rb");					
					if ( fpFile )
					{
						fseek(fpFile, 0, SEEK_END);
						bool bIsZeroFile = ftell(fpFile) <= 0;
						fclose(fpFile);

						if ( bIsZeroFile )
							DeleteFile(pstrCrackedFileName);
					}
				}

				if ( !strFileName.CompareNoCase(Packing_Reference_FileName) )
				{	
					pstrCrackedFileName = strFilePath.GetBuffer();

					FILE* fpFile = fopen(pstrCrackedFileName, "rb");					
					if ( fpFile )
					{
						fseek(fpFile, 0, SEEK_END);
						bool bIsZeroFile = ftell(fpFile) <= 0;
						fclose(fpFile);

						if ( bIsZeroFile )
							DeleteFile(pstrCrackedFileName);
					}
				}
			}
		}
	}
}

//DX Version 9.0c �̻����� Ȯ���Ѵ�.
BOOL CPatchClientLib::CheckDXVersion()
{
	return m_cPatchReg.GetDXVersionFromReg() >= 904 ? TRUE : FALSE;
}

//�׷��� ����̹��� üũ�Ѵ�.
int CPatchClientLib::CheckGraphicDriver()
{
	DriverDownDlg	downdlg;
	downdlg.DoModal();

	if( !m_cPatchReg.GetUseDriverUpdate() )
		return 1;

	LPDIRECT3D9 pD3D9 = Direct3DCreate9( D3D_SDK_VERSION );
	if( !pD3D9 )		return 1;

	D3DADAPTER_IDENTIFIER9	pIdentifier;
	pD3D9->GetAdapterIdentifier( D3DADAPTER_DEFAULT, 0, &pIdentifier );

	int MyVendorID = pIdentifier.VendorId;
	int	MyDeviceID = pIdentifier.DeviceId;

	int MyProduct = HIWORD(pIdentifier.DriverVersion.HighPart);
	int MyVersion = LOWORD(pIdentifier.DriverVersion.HighPart);
	int MySubVersion = HIWORD(pIdentifier.DriverVersion.LowPart);
	int MyBuild = LOWORD(pIdentifier.DriverVersion.LowPart);

	if( MyBuild <= 0 )	return 1;

	pD3D9->Release();

	// ����̹� ���� �˻�
	DriverInfo*		listDriver = NULL;

	
	const CHAR		*szValueName = NULL;
	CHAR			szValue[256];

	INT32			index = 0;

	ApModuleStream	csStream;
	csStream.SetMode( APMODULE_STREAM_MODE_NAME_OVERWRITE );

	// szFile�� �д´�.
	BOOL	bOpenResult = csStream.Open( "Ini\\DriverInfo.Ini", 0 , TRUE );
	if( !bOpenResult )	return 1;

	csStream.ReadSectionName(0);
	csStream.SetValueID(-1);

	// Url ���� �б�
	while( csStream.ReadNextValue() )
	{
		szValueName = csStream.GetValueName();
		if( !strcmp( szValueName, "Url") )
		{
			DriverInfo*	nw_info = new DriverInfo;

			csStream.GetValue(szValue, 256);
			sscanf( szValue, "%d:%s", &index, nw_info->download_url );

			nw_info->iProduct		= 0;
			nw_info->iVersion		= 0;
			nw_info->iSubVersion	= 0;
			nw_info->iBuild			= 0;
			nw_info->index			= index;
			nw_info->next			= listDriver;
			listDriver = nw_info;
		}
	}

	csStream.ReadSectionName(1);
	csStream.SetValueID(-1);

	// ���� üũ�� ���� �б�
	while(csStream.ReadNextValue())
	{
		szValueName = csStream.GetValueName();
		if( !strcmp( szValueName, "Version" ) )
		{
			int i1 = 0,i2 = 0,i3 = 0,i4 = 0;
			csStream.GetValue(szValue, 256);
			sscanf(szValue,"%d:%d.%d.%d.%d",&index,&i1,&i2,&i3,&i4);

			DriverInfo*		cur_info = listDriver;
			BOOL			bFind = FALSE;

			while(cur_info)
			{
				if(cur_info->index == index)
				{
					bFind = TRUE;
					break;
				}

				cur_info = cur_info->next;
			}

			if(bFind)
			{
				cur_info->iProduct		= i1;
				cur_info->iVersion		= i2;
				cur_info->iSubVersion	= i3;
				cur_info->iBuild		= i4;
			}
			else 
				ASSERT(!"Driver Url/Check Version Not Equal .. Check Please");
		}
	}

	csStream.ReadSectionName(2);
	csStream.SetValueID(-1);

	BOOL	bCorrect;
	int		temp;

	// ���� üũ�� ���� �б�
	while(csStream.ReadNextValue())
	{
		szValueName = csStream.GetValueName();
		if( !strcmp( szValueName, "Vendor ID " ) )
		{
			csStream.GetValue(szValue, 256);
			sscanf(szValue,"%d",&temp);
			bCorrect = temp == MyVendorID ? TRUE : FALSE;
		}
		else if(!strcmp(szValueName, "Device ID "))
		{
			csStream.GetValue(szValue, 256);
			sscanf(szValue,"%d",&temp);

			if(bCorrect)
			{
				if(temp == MyDeviceID)
				{
					bCorrect = TRUE;
				}
				else
				{
					bCorrect = FALSE;
				}
			}
		}
		else if(!strcmp(szValueName, "Driver "))
		{
			int		vXP,v2000,v98,vME,vNT;
			csStream.GetValue(szValue, 256);
			sscanf(szValue,"%d:%d:%d:%d:%d",&vXP,&v2000,&v98,&vME,&vNT);

			if(bCorrect)
			{
				int		check_index = -1;
				// ���� �ü�� �Ǻ�
				OSType os_type = GetMyWindowVer();

				if(os_type == WXP)
					check_index = vXP;
				else if(os_type == W2K)
					check_index = v2000;
				else if(os_type == W98 || os_type == W98SP1 || os_type == W98SE)
					check_index = v98;
				else if(os_type == WME)
					check_index = vME;
				else if(os_type == WNT351 || os_type == WNT4)
					check_index = vNT;

				if(check_index == -1)	// -_- ���� ���ϴ� �ü�� �̰ų� url�� ����..
				{
					ClearLists(listDriver);
					listDriver = NULL;
					return 1;
				}

				DriverInfo*	cur_info = listDriver;
				while(cur_info)
				{
					if(cur_info->index == check_index)
					{
						// ����̹� ���� üũ~
						if(cur_info->iProduct == MyProduct &&
							cur_info->iVersion == MyVersion &&
							cur_info->iSubVersion == MySubVersion &&
							cur_info->iBuild > MyBuild)
						{
							DriverDownDlg	downdlg;
							downdlg.m_strDriverURL = cur_info->download_url;

							if(downdlg.DoModal())
							{
								// ����̹� �ٿ�â �߰� �ϱ�~
								//if(IDOK == MessageBoxW("��ũ�ε带 ������ ���� ���ؼ��� �ݵ�� �ֽ� �׷���ī�� ����̹��� �ٿ�ε� �޾ƾ� �մϴ�.\n Ȯ���� �����ø� �ֽ� ����̹��� �ڵ����� �ٿ�ε� ���ݴϴ�\n�ٿ�ε带 �ϼ����� ����̹� ��ġ ���α׷��� �����Ͻð� �ٽ� ���� ������ ���ּž� �˴ϴ�.","",MB_OKCANCEL ))
								//{
								if(downdlg.m_bSelectResult && downdlg.m_bDownloadSuccess)
								{
									STARTUPINFO				si;
									PROCESS_INFORMATION		pi;

									ZeroMemory( &si, sizeof(si) );
									si.cb = sizeof(si);
									ZeroMemory( &pi, sizeof(pi) );

									CreateProcess( NULL, downdlg.m_strFileName.GetBuffer() , NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi );

									ClearLists(listDriver);
									listDriver = NULL;

									//ShellExecute(NULL, _T("open"), _T("IEXPLORE.EXE"),cur_info->download_url, NULL, SW_SHOW);

									return 0;
								}
							}

							ClearLists(listDriver);
							listDriver = NULL;
							return 1;
						}
						else 
						{
							ClearLists(listDriver);
							listDriver = NULL;
							return 1;
						}
					}
					cur_info = cur_info->next;
				}
			}
		}
	}

	ClearLists(listDriver);
	listDriver = NULL;

	return 1;
}


void	CPatchClientLib::ClearLists(DriverInfo*	pList)
{
	DriverInfo*		cur_info = pList;
	DriverInfo*		remove_info;

	while(cur_info)
	{
		remove_info = cur_info;
		cur_info = cur_info->next;

		delete remove_info;
	}
}

OSType CPatchClientLib::GetMyWindowVer()
{
	OSVERSIONINFO osinfo;
	osinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	if (!GetVersionEx(&osinfo))
		return OS_NONE;

	DWORD dwPlatformId   = osinfo.dwPlatformId;
	DWORD dwMinorVersion = osinfo.dwMinorVersion;
	DWORD dwMajorVersion = osinfo.dwMajorVersion;
	DWORD dwBuildNumber  = osinfo.dwBuildNumber & 0xFFFF;	// Win 95 needs this

	if ((dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) && (dwMajorVersion == 4))
	{
		if ((dwMinorVersion < 10) && (dwBuildNumber == 950))
		{
			return W95;
		}
		else if ((dwMinorVersion < 10) && 
			((dwBuildNumber > 950) && (dwBuildNumber <= 1080)))
		{
			return W95SP1;
		}
		else if ((dwMinorVersion < 10) && (dwBuildNumber > 1080))
		{
			return W95OSR2;
		}
		else if ((dwMinorVersion == 10) && (dwBuildNumber == 1998))
		{
			return W98;
		}
		else if ((dwMinorVersion == 10) && 
			((dwBuildNumber > 1998) && (dwBuildNumber < 2183)))
		{
			return W98SP1;
		}
		else if ((dwMinorVersion == 10) && (dwBuildNumber >= 2183))
		{
			return W98SE;
		}
		else if (dwMinorVersion == 90)
		{
			return WME;
		}
	}
	else if (dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		if ((dwMajorVersion == 3) && (dwMinorVersion == 51))
		{
			return WNT351;
		}
		else if ((dwMajorVersion == 4) && (dwMinorVersion == 0))
		{
			return WNT4;
		}
		else if ((dwMajorVersion == 5) && (dwMinorVersion == 0))
		{
			return W2K;
		}
		else if ((dwMajorVersion == 5) && (dwMinorVersion == 1))
		{
			return WXP;
		}
		else if ((dwMajorVersion == 5) && (dwMinorVersion == 2))
		{
			return W2003SERVER;
		}
	}
	//else if (dwPlatformId == VER_PLATFORM_WIN32_CE)
	//{
	//	return WCE;
	//}

	return OS_NONE;
}


//@{ 2006/04/12 burumal
CPackingFile* CPatchClientLib::SearchBakPackingEmptyList(char* pFileName, long lNeeds, ApAdmin* pAdminBakEmpty, long* pOffset)
{	
	CPackingFile* pResult = NULL;	

	if ( lNeeds <= 0 || !pAdminBakEmpty )
		return pResult;	

	CPackingFile** ppCur;

	long lOffsetVal = -1;

	ppCur = (CPackingFile**) pAdminBakEmpty->GetObject(_strlwr(pFileName));
	if ( ppCur && *ppCur ) 
	{
		if ( (*ppCur)->m_pcsDataNode->m_lSize == lNeeds )
			return (*ppCur);

		if ( (*ppCur)->m_pcsDataNode->m_lSize > lNeeds )
		{
			// ���� ������ Empty ���� ����Ʈ�� �߰�
			m_csAuPackingManager.AddEmptyPackingFileInfo((*ppCur)->m_pcsDataNode->m_lStartPos + lNeeds, 
				(*ppCur)->m_pcsDataNode->m_lSize - lNeeds, pAdminBakEmpty);

			(*ppCur)->m_pcsDataNode->m_lSize = lNeeds;

			return (*ppCur);

			/*
			// lNeeds�� 3% �̸��� ���̶�� ���� ������ ������ ����ϵ��� �Ѵ�
			if ( ((*ppCur)->m_pcsDataNode->m_lSize - lNeeds) <= (lNeeds * 0.03f) )
			{
				// 1k �̻� ���� ������ �ٽ� Empty ����Ʈ�� Add
				if ( ((*ppCur)->m_pcsDataNode->m_lSize - lNeeds) >= 1024 * 1 )
				{					
					m_csAuPackingManager.AddEmptyPackingFileInfo((*ppCur)->m_pcsDataNode->m_lStartPos + lNeeds, 
						(*ppCur)->m_pcsDataNode->m_lSize - lNeeds, pAdminBakEmpty);

					(*ppCur)->m_pcsDataNode->m_lSize = lNeeds;
				}
				else
				{
					(*pOffset) += ((*ppCur)->m_pcsDataNode->m_lSize - lNeeds);
				}

				return (*ppCur);
			}

			pResult = (*ppCur);
			lOffsetVal = (*ppCur)->m_pcsDataNode->m_lSize - lNeeds;
			*/
		}
	}

	int nIdx = 0;
	
	for ( ppCur = (CPackingFile**) pAdminBakEmpty->GetObjectSequence(&nIdx); 
		ppCur;
		ppCur = (CPackingFile**) pAdminBakEmpty->GetObjectSequence(&nIdx) )
	{
		ASSERT(ppCur);
		ASSERT(*ppCur);

		if ( (*ppCur)->m_pcsDataNode->m_lSize == lNeeds )
			return (*ppCur);

		if ( (*ppCur)->m_pcsDataNode->m_lSize < lNeeds )
			continue;

		if ( lOffsetVal == -1 )
		{
			lOffsetVal = (*ppCur)->m_pcsDataNode->m_lSize - lNeeds;
			pResult = (*ppCur);
		}
		else
		{
			long lTempOffset = (*ppCur)->m_pcsDataNode->m_lSize - lNeeds;

			if ( lOffsetVal > lTempOffset )
			{
				lOffsetVal = lTempOffset;
				pResult = (*ppCur);
			}
		}

		ASSERT(lOffsetVal > 0);
	}
	
#ifdef _DEBUG
	if ( lOffsetVal < 0 )
		ASSERT(pResult == NULL);
	if ( pResult == NULL )
		ASSERT(lOffsetVal < 0);
#endif

	if ( lOffsetVal > 0 )
	{		
		m_csAuPackingManager.AddEmptyPackingFileInfo(pResult->m_pcsDataNode->m_lStartPos + lNeeds, lOffsetVal, pAdminBakEmpty);

		pResult->m_pcsDataNode->m_lSize = lNeeds;

		/*
		// 1k �̻� ���� ������ �ٽ� Empty ����Ʈ�� Add
		if ( lOffsetVal >= 1024 * 1 )
		{
			m_csAuPackingManager.AddEmptyPackingFileInfo(pResult->m_pcsDataNode->m_lStartPos + lNeeds, 
				lOffsetVal, pAdminBakEmpty);

			pResult->m_pcsDataNode->m_lSize = lNeeds;
		}
		else
		{
			(*pOffset) += lOffsetVal;
		}
		*/
	}

	return pResult;
}
//@}

//@{ 2006/04/3 burumal
bool CPatchClientLib::OverwritePackingData(FILE* fpReference, FILE* fpBakDat, char* pFolderName, CPackingFile* pOverwriteInfo, 
										 char* pFileName, char* pstrBuffer, int lFileSize)
{	
	if ( !fpReference || !fpBakDat )
		return false;

	if ( !pOverwriteInfo || !pstrBuffer || (lFileSize < 0) || !pFileName )
		return false;	
	
	if ( pOverwriteInfo->m_pcsDataNode == NULL )
		return false;
	
	int			lFileNameSize;
	int			lReadBytes;	

	static char	pHeaderBuffer[512];
	
	//Refrence ������ �����Ѵ�.
	lFileNameSize = (int) strlen( pFileName ) + 1;

	char* pCurPtr = pHeaderBuffer;
	
	memcpy(pCurPtr, &lFileNameSize, sizeof(int));
	pCurPtr += sizeof(int);

	memcpy(pCurPtr, pFileName, lFileNameSize);
	pCurPtr += lFileNameSize;

	memcpy(pCurPtr, &(pOverwriteInfo->m_pcsDataNode->m_lStartPos), sizeof(int));
	pCurPtr += sizeof(int);

	memcpy(pCurPtr, &lFileSize, sizeof(int));
	pCurPtr += sizeof(int);

	//fwrite( &lFileNameSize, 1, sizeof(int), fpReference );
	//fwrite( pFileName, 1, lFileNameSize, fpReference );
	//fwrite( &(pOverwriteInfo->m_pcsDataNode->m_lStartPos), 1, sizeof(int), fpReference );
	//fwrite( &lFileSize, 1, sizeof(int), fpReference );

	fwrite(pHeaderBuffer, 1, pCurPtr - pHeaderBuffer, fpReference);


	//Data.Bak ������ �����Ѵ�

	ASSERT(lFileSize <= pOverwriteInfo->m_pcsDataNode->m_lSize);	
	
	// �ش� ��ġ�� �̵��Ѵ�.
	fseek( fpBakDat, pOverwriteInfo->m_pcsDataNode->m_lStartPos, SEEK_SET);

	while( 1 )
	{
		if ( lFileSize >= Packing_lBlockSize )
		{
			lReadBytes = Packing_lBlockSize;
			lFileSize -= Packing_lBlockSize;
		}
		else
		{
			lReadBytes = lFileSize;
			lFileSize = 0;
		}

		if ( 0 < lReadBytes )
		{
			fwrite(pstrBuffer, 1, lReadBytes, fpBakDat);
						
			pstrBuffer += lReadBytes;
		}

		if ( lReadBytes < Packing_lBlockSize )
		{
			break;
		}
	}
	
	return true;
}
//@}

//@{ 2006/04/14 burumal
bool CPatchClientLib::AttachAddedPackingData(FILE* fpReference, char* pstrFolderName, char* pstrFileName, int lFileSize, char* pDataBuffer)
{	
	FILE* fpBakDat;	
	
	if ( !fpReference || !pDataBuffer || !pstrFolderName || !pstrFileName || lFileSize <= 0 )
		return false;

	// Reference.Dat ���ϰ� Data.Bak ������ ����
	static char strDataFilePath[256];
	static char strReferenceFilePath[256];

	if( !stricmp( pstrFolderName, "root" ) )
	{
		sprintf( strDataFilePath, "%s", Packing_Data_Backup_FileName);
		sprintf( strReferenceFilePath, "%s", Packing_Reference_FileName);
	}
	else
	{
		sprintf( strDataFilePath, "%s\\%s", pstrFolderName, Packing_Data_Backup_FileName);
		sprintf( strReferenceFilePath, "%s\\%s", pstrFolderName, Packing_Reference_FileName);

	}	

	fpBakDat = fopen( strDataFilePath, "ab" );	
	if ( !fpBakDat )
	{
		if ( !fpBakDat || !fpReference )
		{
			return false;
		}
	}
	
	//���� �����͸� ���������� �ű��
	fseek(fpBakDat, 0, SEEK_END);
		
	// attach �Ǵ� ����Ÿ �̹Ƿ� ������ġ�� �翬�� ����ũ��� ��ġ
	long lDatStartPos;
	lDatStartPos = ftell(fpBakDat);

	int			lFileNameSize;
	int			lReadBytes;

	static char	pHeaderBuffer[512];

	//Refrence.Dat ������ �����Ѵ�.
	lFileNameSize = (int) strlen( pstrFileName ) + 1;

	char* pCurPtr = pHeaderBuffer;

	memcpy(pCurPtr, &lFileNameSize, sizeof(int));
	pCurPtr += sizeof(int);

	memcpy(pCurPtr, pstrFileName, lFileNameSize);
	pCurPtr += lFileNameSize;

	memcpy(pCurPtr, &lDatStartPos, sizeof(int));
	pCurPtr += sizeof(int);

	memcpy(pCurPtr, &lFileSize, sizeof(int));
	pCurPtr += sizeof(int);

	//fwrite( &lFileNameSize, 1, sizeof(int), fpReference );
	//fwrite( pstrFileName, 1, lFileNameSize, fpReference );
	//fwrite( &lDatStartPos, 1, sizeof(int), fpReference );
	//fwrite( &lFileSize, 1, sizeof(int), fpReference );

	fwrite(pHeaderBuffer, 1, pCurPtr - pHeaderBuffer, fpReference);


	//Data.Bak ������ �����Ѵ�
	while( 1 )
	{
		if ( lFileSize >= Packing_lBlockSize )
		{
			lReadBytes = Packing_lBlockSize;
			lFileSize -= Packing_lBlockSize;
		}
		else
		{
			lReadBytes = lFileSize;
			lFileSize = 0;
		}

		if ( 0 < lReadBytes )
		{
			// ���������� 
			fwrite(pDataBuffer, 1, lReadBytes, fpBakDat);
			
			pDataBuffer += lReadBytes;
		}

		if ( lReadBytes < Packing_lBlockSize )
		{
			break;
		}
	}
	
	if ( fpBakDat )
		fclose(fpBakDat);

	return true;
}
//@}

//@{ 2006/04/16 burumal
bool CPatchClientLib::WriteNormalPackingRefInfo(FILE* fpReference, ApAdmin* pAdminNormal)
{
	if ( !fpReference || !pAdminNormal )
		return false;

	static char pHeaderBuffer[512];

	CPackingFile** ppCur; 

	int nIdx = 0;

	for ( ppCur = (CPackingFile**) pAdminNormal->GetObjectSequence(&nIdx); 
		ppCur; 
		ppCur = (CPackingFile**) pAdminNormal->GetObjectSequence(&nIdx) )
	{
		ASSERT(ppCur);
		ASSERT(*ppCur);

		CPackingFile* pPackingNormal = (*ppCur);
		
		if ( pPackingNormal )
		{
			int lFileNameSize;

			//Refrence.Dat ������ �����Ѵ�.
			lFileNameSize = (int) pPackingNormal->m_lFileNameSize;

			char* pCurPtr = pHeaderBuffer;

			memcpy(pCurPtr, &lFileNameSize, sizeof(int));
			pCurPtr += sizeof(int);

			memcpy(pCurPtr, pPackingNormal->m_pstrFileName, lFileNameSize);
			pCurPtr += lFileNameSize;

			memcpy(pCurPtr, &(pPackingNormal->m_pcsDataNode->m_lStartPos), sizeof(int));
			pCurPtr += sizeof(int);

			memcpy(pCurPtr, &(pPackingNormal->m_pcsDataNode->m_lSize), sizeof(int));
			pCurPtr += sizeof(int);

			//fwrite( &lFileNameSize, 1, sizeof(int), fpReference );
			//fwrite( pPackingNormal->m_pstrFileName, 1, lFileNameSize, fpReference );
			//fwrite( &(pPackingNormal->m_pcsDataNode->m_lStartPos), 1, sizeof(int), fpReference );
			//fwrite( &(pPackingNormal->m_pcsDataNode->m_lSize), 1, sizeof(int), fpReference );

			fwrite(pHeaderBuffer, 1, pCurPtr - pHeaderBuffer, fpReference);
		}
	}

	return true;
}


void	CPatchClientLib::SendPingPacket()
{
	for (int lIndex = 0; lIndex < m_lArea; ++lIndex)
	{
		if (m_pcsAreaInfo[lIndex].m_pszPatchServer && m_pcsAreaInfo[lIndex].m_pszPatchServer[0])
		{
			m_pcsAreaInfo[lIndex].SendPingPacket();
		}
	}
}

INT32	CPatchClientLib::GetPingTime(char *pszArea)
{
	AuAutoLock	csLock(m_csCSArea);

	AgpdPatchAreaInfo *	pcsArea = GetArea(pszArea);

	if (!pcsArea)
		return -1;

	return pcsArea->m_lPingTime;
}

bool CPatchClientLib::CheckPingTime(bool bLoop)
{
	if (!m_phEvents || !m_pcsAreaInfo)
		return false;

	int			lIndex;
	int			lNumEvent = 0;
	DWORD		dwLastSentTime = timeGetTime();

	SendPingPacket();

	while (!m_bExitPing)
	{
		lNumEvent = 0;
		m_phEvents[lNumEvent++] = m_hEventForNewSocket;
		for (lIndex = 0; lIndex < m_lArea; ++lIndex)
		{
			if (m_pcsAreaInfo[lIndex].m_csSocket.m_hSocket && m_pcsAreaInfo[lIndex].m_csSocket.m_hRecvEvent)
			{
				m_phEvents[lNumEvent++] = m_pcsAreaInfo[lIndex].m_csSocket.m_hRecvEvent;
			}
		}

		bool			bResult;

		bResult = true;

		DWORD dwReturn = WSAWaitForMultipleEvents(lNumEvent, m_phEvents, FALSE, PATCHCLIENT_PING_DELAY, FALSE);
		if (m_bExitPing)
			break;

		LockArea();

		for (lIndex = 0; lIndex < m_lArea; ++lIndex)
		{
			if (m_pcsAreaInfo[lIndex].m_csSocket.m_hSocket && m_pcsAreaInfo[lIndex].m_csSocket.m_hRecvEvent)
			{
				dwReturn = WSAEnumNetworkEvents( m_pcsAreaInfo[lIndex].m_csSocket.m_hSocket, m_pcsAreaInfo[lIndex].m_csSocket.m_hRecvEvent, &m_pcsAreaInfo[lIndex].m_csSocket.m_event);
				if( dwReturn == -1 || (m_pcsAreaInfo[lIndex].m_csSocket.m_event.lNetworkEvents & FD_CLOSE) == FD_CLOSE )
				{
				}

				if( (m_pcsAreaInfo[lIndex].m_csSocket.m_event.lNetworkEvents & FD_CONNECT) == FD_CONNECT )
				{
					//m_pcsAreaInfo[lIndex].SendPingPacket();
				}

				if( (m_pcsAreaInfo[lIndex].m_csSocket.m_event.lNetworkEvents & FD_READ) == FD_READ )
				{
					//��Ŷ�� �޾Ƽ� Parsing�Ѵ�.
					int				iTransferedBytes;

					iTransferedBytes = recv( m_pcsAreaInfo[lIndex].m_csSocket.m_hSocket, m_pcsAreaInfo[lIndex].m_cWSABuf.buf, m_pcsAreaInfo[lIndex].m_cWSABuf.len, 0 );

					if( iTransferedBytes > 0 )
					{
						bResult = ExtractPacket( iTransferedBytes, &m_pcsAreaInfo[lIndex] );

						if( bResult == false )
						{
						}
					}
					else
					{
						if( iTransferedBytes == 0 )
						{
						}
						else if( iTransferedBytes == SOCKET_ERROR )
						{
						}
					}
				}
			}
		}

		UnlockArea();

		if (timeGetTime() - dwLastSentTime >= PATCHCLIENT_PING_DELAY)
		{
			dwLastSentTime = timeGetTime();
			SendPingPacket();
		}

		if (!bLoop)
			break;
	}

	ClearAllArea();

	return true;
}

void CPatchClientLib::ClearAllArea()
{
	for (int lIndex = 0; lIndex < m_lArea; ++lIndex)
	{
		if (m_pcsAreaInfo[lIndex].m_csSocket.m_hSocket)
		{
			m_pcsAreaInfo[lIndex].m_csSocket.cleanupWinsock();
		}
	}
}

void CPatchClientLib::StartupSelfPatchProcess()
{	
	int nBackAppVersion[4] = {0, 0, 0, 0};
	
	FILE* fpBackApp = fopen("Archlord.bak", "rb");
	if ( fpBackApp == NULL )
	{
		DeleteFile("SelfPatch.bat");
		return;
	}
	fclose(fpBackApp);

	if ( fpBackApp != NULL )
	{
		ForceExitProgram();
	}
}

BOOL CPatchClientLib::SendRequestClientFileCRC( void )
{
	CPacketTypeCS_0x05  cPacketType0x05;

	UINT32 iPacketSize = cPacketType0x05.m_iSize;
	UINT32 iIndex = 0;

	char* pstrData = new char[iPacketSize];
	memset( pstrData, 0, sizeof( char ) * iPacketSize );	

	memcpy( &pstrData[iIndex], &cPacketType0x05.m_iSize, sizeof(cPacketType0x05.m_iSize) );
	iIndex += sizeof(cPacketType0x05.m_iSize);

	memcpy( &pstrData[iIndex], &cPacketType0x05.m_iCommand, sizeof(cPacketType0x05.m_iCommand) );
	iIndex += sizeof(cPacketType0x05.m_iCommand);

	if (!SendPacket( pstrData, iPacketSize )) return FALSE;
	return TRUE;
}
