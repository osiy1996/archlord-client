#include <AuPackingManager/AuPackingManager.h>
#include <windows.h>
#include <commctrl.h>
#include <io.h>
#include <dbghelp.h>

#define MAX_FILECOUNT_PER_FOLDER				20000


AuPackingManager* AuPackingManager::m_pThis = NULL;

void CPackingFile::Reset()
{
/*	CPackingDataNode	*pcsNextNode;
	CPackingDataNode	*pcsCurrentNode;

	pcsCurrentNode = m_pcsDataNode;

	while( 1 )
	{
		if( pcsCurrentNode == NULL )
			break;

		pcsNextNode = pcsCurrentNode->m_pcsNextNode;

		if( m_pcsDataNode != NULL )
            delete m_pcsDataNode;

		m_pcsDataNode = NULL;

		pcsCurrentNode = pcsNextNode;
	}*/

	if ( m_pcsDataNode )
	{
		delete m_pcsDataNode;
		m_pcsDataNode = NULL;
	}

	if( m_pstrFileName != NULL )
	{
		delete [] m_pstrFileName;
		m_pstrFileName = NULL;
	}
}

CPackingFolder::CPackingFolder()
{
	m_csFilesInfo.InitializeObject( sizeof(CPackingFile *), MAX_FILECOUNT_PER_FOLDER );

	m_Mutex.Init();
	
//	m_lEditing  = AuPacking_Status_Build;
	m_bExportToRef = false;

	m_lEmptyBlockCount = 0;
	m_lBlockSize = 0;
	m_pstrBlock = NULL;

	m_lFolderNameSize = 0;
	m_pstrFolderName = NULL;
	m_fpFile = NULL;
}

CPackingFolder::~CPackingFolder()
{
	m_Mutex.Destroy();

	if( m_pstrFolderName )
	{
		delete [] m_pstrFolderName;
		m_pstrFolderName = NULL;
	}
}

void CPatchFolderInfo::SetPacking( bool bPacking )
{
	m_bPacking = bPacking;
}

AuPackingManager::AuPackingManager()
{
	m_lCurrentVersion = 0;

	m_csFolderInfo.InitializeObject( sizeof(CPackingFolder *), MAX_FILECOUNT_PER_FOLDER );
	m_csCrackedFolderInfo.InitializeObject( sizeof(CCrackedFolderInfo *), MAX_FILECOUNT_PER_FOLDER );
	m_csIgnoreFileInfo.InitializeObject( sizeof(CIgnoreFileInfo *), MAX_FILECOUNT_PER_FOLDER );
	//m_csLoadingFolder.InitializeObject( sizeof(void *), 1000 );

	//Read Only Mode
	m_bReadOnlyMode = true;

	//�ɼǼ���.
	m_bBuildJZP = true;
	m_bBuildDat = true;
	m_bCopyRawFiles = true;
//	m_bLoadFromRawFile = true;

//	setLoadFromPackingData( true );

	//@{ 2006/04/11 burumal
	m_cCompress.InitMemPool();
	//@}

	//@{ 2006/04/14 burumal
	m_nEmptyFileSerial = 0;
	//@}

	m_pThis = NULL;
}


AuPackingManager::~AuPackingManager()
{
	//@{ 2006/04/11 burumal
	m_cCompress.RemoveMemPool();
	//@}

	m_pThis = NULL;
}

void AuPackingManager::Destroy()
{
	//Folder�� �ѹ� �޿�~ ���ƺ��ڲٳ�~
	CPackingFolder		**ppcsPackingFolder;
	CPackingFile		**ppcsPackingFile;

	int				lFolderIndex;
	int				lFileIndex;

	lFolderIndex = 0;

	for( ppcsPackingFolder = (CPackingFolder **)m_csFolderInfo.GetObjectSequence( &lFolderIndex ); ppcsPackingFolder; ppcsPackingFolder = (CPackingFolder	**)m_csFolderInfo.GetObjectSequence( &lFolderIndex ) )
	{
		if( ppcsPackingFolder && *ppcsPackingFolder )
		{
			lFileIndex = 0;

			for( ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObjectSequence( &lFileIndex ); ppcsPackingFile; ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObjectSequence( &lFileIndex ) )
			{
				if( ppcsPackingFile && *ppcsPackingFile )
				{
					delete (*ppcsPackingFile);
				}
			}

			(*ppcsPackingFolder)->m_csFilesInfo.RemoveObjectAll();

			delete *ppcsPackingFolder;
		}
	}

	m_csFolderInfo.RemoveObjectAll();
}

ApAdmin	*AuPackingManager::GetFolderInfo()
{
	return &m_csFolderInfo;
}

CDList<CPatchVersionInfo *> *AuPackingManager::GetPatchList()
{
	return &m_cPatchList;
}

bool AuPackingManager::IsCheckedBuildJZP()
{
	return m_bBuildJZP;
}

bool AuPackingManager::IsCheckedBuildDat()
{
	return m_bBuildDat;
}

bool AuPackingManager::IsCheckedCopyRawFiles()
{
	return m_bCopyRawFiles;
}

void AuPackingManager::SetBuildJZP( bool bBuildJZP )
{
	m_bBuildJZP = bBuildJZP;
}

void AuPackingManager::SetBuildDat( bool bBuildDat )
{
	m_bBuildDat = bBuildDat;
}

void AuPackingManager::SetCopyRawFiles( bool bCopyRawFiles )
{
	m_bCopyRawFiles = bCopyRawFiles;
}

AuPackingManager* AuPackingManager::MakeInstance()
{
	if(!m_pThis)
	{
#ifdef new
#undef new
#endif
		m_pThis = new AuPackingManager;
		atexit(AuPackingManager::DestroyInstance);
#ifdef new
#undef new
#define new DEBUG_NEW
#endif
	}
	
	return	m_pThis;
}

void AuPackingManager::DestroyInstance()
{
	if (m_pThis)
	{
		AuPackingManager *	pThis = m_pThis;

		m_pThis = NULL;

		pThis->Destroy();

		delete pThis;
	}
}

AuPackingManager* AuPackingManager::GetSingletonPtr()
{
	if(!m_pThis) MakeInstance();
	return m_pThis;
}

/*void AuPackingManager::setLoadFromPackingData( bool bSet )
{
	m_bLoadFromPackingData = bSet;
}*/

bool AuPackingManager::LoadIgnoreFileInfo( char *pstrRootFolder, char *pstrFileName )
{
	bool			bResult;

	bResult = false;

	if( pstrFileName )
	{
		FILE			*file;
		char			strOriginDir[255];

		GetCurrentDirectory( sizeof(strOriginDir), strOriginDir );

		file = fopen( pstrFileName, "rb" );
		if( file )
		{
			char			strSearchCommand[255];

			while( 1 )
			{
				if( EOF == fscanf( file, "%s", strSearchCommand ) )
				{
					break;
				}
				else
				{
					//�ش������� �����̸��� �Ҿ�~~~ �о�鿩����~ �ƽ�����~~~
					HANDLE				hHandle;
					WIN32_FIND_DATA		csFindData;
					char				strSearchFolder[255];
					char				strFolderName[255];
					char				strFileFullPath[255];

					bool				bResult;

					bResult = false;

					_splitpath( strSearchCommand, NULL, strFolderName, NULL, NULL );

					SetCurrentDirectory( pstrRootFolder );
					sprintf( strSearchFolder, "%s\\%s", pstrRootFolder,strSearchCommand );

					hHandle = FindFirstFile( strSearchFolder, &csFindData );

					if( INVALID_HANDLE_VALUE != hHandle )
					{
						if( csFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
						{
							;
						}
						else
						{
							if( !strcmp( strFolderName, "\\" ) )
							{
								sprintf( strFileFullPath, "%s\\%s", pstrRootFolder, csFindData.cFileName );
							}
							else
							{
								sprintf( strFileFullPath, "%s\\%s%s", pstrRootFolder, strFolderName, csFindData.cFileName );
							}

							AddIgnoreFileName( strFileFullPath );
						}
					}

					while( 1 )
					{
						if( FindNextFile( hHandle, &csFindData ) == FALSE )
							break;

						//������� ����~
						if( csFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
						{
							continue;
						}
						//�����̶�� ���� ���ϸ���Ʈ�� �߰��Ѵ�~ �ƽ�����~
						else
						{
							if( !strcmp( strFolderName, "\\" ) )
							{
								sprintf( strFileFullPath, "%s\\%s", pstrRootFolder, csFindData.cFileName );
							}
							else
							{
								sprintf( strFileFullPath, "%s\\%s%s", pstrRootFolder, strFolderName, csFindData.cFileName );
							}

							AddIgnoreFileName( strFileFullPath );
						}
					}

					FindClose( hHandle );
				}
			}

			fclose( file );
		}

		SetCurrentDirectory( strOriginDir );
	}

	return bResult;
}

bool AuPackingManager::AddIgnoreFileName( char *pstrIgnoreFileName )
{
	CIgnoreFileInfo		*pcsIgnoreFileInfo;

	pcsIgnoreFileInfo = new CIgnoreFileInfo;

	pcsIgnoreFileInfo->m_lFileNameSize = strlen( pstrIgnoreFileName ) + 1;
	pcsIgnoreFileInfo->m_pstrFileName = new char[pcsIgnoreFileInfo->m_lFileNameSize];
	memset( pcsIgnoreFileInfo->m_pstrFileName, 0, pcsIgnoreFileInfo->m_lFileNameSize );
	strcat( pcsIgnoreFileInfo->m_pstrFileName, pstrIgnoreFileName );

	if( m_csIgnoreFileInfo.AddObject( (void **)&pcsIgnoreFileInfo, _strlwr(pstrIgnoreFileName) ) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool AuPackingManager::RemoveIgnoreFileName( char *pstrIgnoreFileName )
{
	CIgnoreFileInfo		**ppcsIgnoreFileInfo;

	ppcsIgnoreFileInfo = (CIgnoreFileInfo **)m_csIgnoreFileInfo.GetObject( _strlwr(pstrIgnoreFileName) );

	if( ppcsIgnoreFileInfo && (*ppcsIgnoreFileInfo) )
	{
		delete [] (*ppcsIgnoreFileInfo)->m_pstrFileName;

		delete (*ppcsIgnoreFileInfo);
	}

	if( m_csIgnoreFileInfo.RemoveObject( _strlwr(pstrIgnoreFileName) ) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool AuPackingManager::IsIgnoreFileName( char *pstrIgnoreFileName )
{
	if( m_csIgnoreFileInfo.GetObject( _strlwr(pstrIgnoreFileName) ) )
		return true;
	else
		return false;
}

ApAdmin *AuPackingManager::GetIgnoreFileInfo()
{
	return &m_csIgnoreFileInfo;
}

bool AuPackingManager::AddCrackedFolderName( char *pstrCrackedFolderName )
{
	CCrackedFolderInfo		*pcsCrackedFolderInfo;

	pcsCrackedFolderInfo = new CCrackedFolderInfo;

	pcsCrackedFolderInfo->m_lFolderNameSize = strlen( pstrCrackedFolderName ) + 1;
	pcsCrackedFolderInfo->m_pstrFolderName = new char[pcsCrackedFolderInfo->m_lFolderNameSize];
	memset( pcsCrackedFolderInfo->m_pstrFolderName, 0, pcsCrackedFolderInfo->m_lFolderNameSize );
	strcat( pcsCrackedFolderInfo->m_pstrFolderName, pstrCrackedFolderName );

	if( m_csCrackedFolderInfo.AddObject( (void **)&pcsCrackedFolderInfo, _strlwr(pstrCrackedFolderName) ) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool AuPackingManager::RemoveCrackedFolderName( char *pstrCrackedFolderName )
{
	CCrackedFolderInfo		**ppcsCrackedFolderInfo;

	ppcsCrackedFolderInfo = (CCrackedFolderInfo **)m_csCrackedFolderInfo.GetObject( _strlwr(pstrCrackedFolderName) );

	if( ppcsCrackedFolderInfo && (*ppcsCrackedFolderInfo) )
	{
		delete [] (*ppcsCrackedFolderInfo)->m_pstrFolderName;

		delete (*ppcsCrackedFolderInfo);
	}

	if( m_csCrackedFolderInfo.RemoveObject( _strlwr(pstrCrackedFolderName) ) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

ApAdmin *AuPackingManager::GetCrackedFolderInfo()
{
	return &m_csCrackedFolderInfo;
}

int AuPackingManager::GetLastVersion()
{
	int				lVersion;

	if( m_cPatchList.getCount() == 0 )
	{
		lVersion = 0;
	}
	else
	{
		CListNode<CPatchVersionInfo *>	*pcsNode;

		pcsNode = m_cPatchList.GetEndNode();

		if( pcsNode )
		{
			lVersion = pcsNode->m_tData->m_lVersion;
		}
		else
		{
			lVersion = 0;
		}
	}

	return lVersion;
}

CPatchVersionInfo *AuPackingManager::GetPatchInfo( int lVersion )
{
	CListNode<CPatchVersionInfo *>		*pcsPatchVersionNode;
	CPatchVersionInfo *pcsResult;

	pcsResult = NULL;

	for( pcsPatchVersionNode=m_cPatchList.GetStartNode(); pcsPatchVersionNode; pcsPatchVersionNode=pcsPatchVersionNode->m_pcNextNode )
	{
		if( pcsPatchVersionNode->m_tData->m_lVersion == lVersion )
		{
			pcsResult = pcsPatchVersionNode->m_tData;
		}
	}

	return pcsResult;
}

bool AuPackingManager::AddFolder( char *pstrFolderName )
{
	bool			bResult;

	bResult = false;

	CPackingFolder		*pcsPackingFolder;

	pcsPackingFolder = new CPackingFolder;

	if( pcsPackingFolder )
	{
		if( pstrFolderName )
		{
			size_t			lFolderNameLen;

			lFolderNameLen = strlen( pstrFolderName ) + 1;

			pcsPackingFolder->m_lFolderNameSize = lFolderNameLen;

			pcsPackingFolder->m_pstrFolderName = new char[lFolderNameLen];
			memset( pcsPackingFolder->m_pstrFolderName, 0, lFolderNameLen );
			strcat( pcsPackingFolder->m_pstrFolderName, pstrFolderName );

			if( m_csFolderInfo.AddObject( (void **)&pcsPackingFolder, _strlwr(pstrFolderName )) )
			{
				bResult = true;
			}
		}
	}

	return bResult;
}

bool AuPackingManager::AddFile( char *pstrFolderName, CPackingFile *pcsTempPackingFile, int lFileSize, unsigned long lCheckCode )
{
	bool			bResult;

	bResult = false;

	CPackingFolder		**ppcsPackingFolder;
	CPackingFile		**ppcsPackingFile;
	//CPackingDataNode	*pcsPackingDataNode;

	ppcsPackingFolder = (CPackingFolder	**)m_csFolderInfo.GetObject( _strlwr(pstrFolderName) );

	if( ppcsPackingFolder && *ppcsPackingFolder && pstrFolderName )
	{
		//���� �̸��� �����ϴ��� ����.
		ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObject( _strlwr(pcsTempPackingFile->m_pstrFileName) );

		if( ppcsPackingFile )
		{
			delete (*ppcsPackingFile);
			(*ppcsPackingFolder)->m_csFilesInfo.RemoveObject( _strlwr(pcsTempPackingFile->m_pstrFileName) );
		}

		CPackingFile		*pcsPackingFile;

		pcsPackingFile = new CPackingFile;

		//@{ 2006/11/10 burumal
		//pcsPackingDataNode = new CPackingDataNode;
		//@}

		pcsPackingFile->m_bPacking = pcsTempPackingFile->m_bPacking;
		pcsPackingFile->m_lOperation = pcsTempPackingFile->m_lOperation;
		pcsPackingFile->m_lFileNameSize = pcsTempPackingFile->m_lFileNameSize;
		pcsPackingFile->m_pstrFileName = new char[pcsPackingFile->m_lFileNameSize];
		memset( pcsPackingFile->m_pstrFileName, 0, pcsPackingFile->m_lFileNameSize );
		strcat( pcsPackingFile->m_pstrFileName, pcsTempPackingFile->m_pstrFileName );
		pcsPackingFile->m_lFileSize = lFileSize;
		pcsPackingFile->m_lFileCheckSum = lCheckCode;

		if( (*ppcsPackingFolder)->m_csFilesInfo.AddObject( (void **)&pcsPackingFile, _strlwr(pcsPackingFile->m_pstrFileName) ) )
		{
			bResult = true;
		}
	}

	return bResult;
}

/*
bool AuPackingManager::AddPackingFile( char *pstrTempFolderName, char *pstrFolderName, char *pstrFileName )
{
	char			strTempFile[255];
	char			strDataFile[255];
	bool			bResult;

	bResult = false;

	CPackingFolder		**ppcsPackingFolder;

	ppcsPackingFolder = (CPackingFolder	**)m_csFolderInfo.GetObject( _strlwr(pstrFolderName) );

	//������ ���ٸ�! ����� �ٽ� ��´�. <- ���� �߰��Ǵ� ������ ��� Ŭ���̾�Ʈ���� �����Ƿ� �߰��Ѵ�.
	if( ppcsPackingFolder == NULL )
	{
        AddFolder( pstrFolderName );

		//��ũ�� �ش� ������ �����.
		CreateFolder( pstrFolderName );

		ppcsPackingFolder = (CPackingFolder	**)m_csFolderInfo.GetObject( _strlwr(pstrFolderName) );
	}

	//�켱 ����� ����.
	RemovePackingFile( pstrFolderName, pstrFileName );

	//���� Ȯ��~
	if( ppcsPackingFolder && *ppcsPackingFolder && pstrFolderName && pstrFileName )
	{
		CPackingFile			*pcsPackingFile;

		FILE			*fpTempFile;
		FILE			*fpDataDat;
		char			strBuffer[Packing_lBlockSize];
		int				lFileSize;
		int				lBlockCount;

		sprintf( strTempFile, "%s\\%s", pstrTempFolderName, pstrFileName );
		fpTempFile = fopen( strTempFile, "rb" );

		if( fpTempFile )
		{
			fseek( fpTempFile, 0, SEEK_END );
			lFileSize = ftell( fpTempFile );

			lBlockCount = lFileSize/Packing_lBlockSize;

			if( lFileSize%Packing_lBlockSize )
			{
				lBlockCount++;
			}

			//���� ũ�⸦ �������� ó������ ������.
			fseek( fpTempFile, 0, SEEK_SET );

			pcsPackingFile = new CPackingFile;

			//�� ���� ������ ������ �߰���!
			if( (*ppcsPackingFolder)->m_lEmptyBlockCount <= 0 )
			{
				CPackingDataNode		*pcsPackingDataNode;

				pcsPackingDataNode = new CPackingDataNode;

				sprintf( strDataFile, "%s\\%s", (*ppcsPackingFolder)->m_pstrFolderName, Packing_Data_FileName );

				if( (*ppcsPackingFolder)->m_pstrBlock == NULL )
				{
					//File�� ��ġ������ �����Ѵ�.
					pcsPackingDataNode->m_lStartPos = 0;
					pcsPackingDataNode->m_lSize = lFileSize;

					//���ο� ���� �Ҵ��Ѵ�.
					(*ppcsPackingFolder)->m_pstrBlock = new char[lBlockCount];

					memset( (*ppcsPackingFolder)->m_pstrBlock, 1, lBlockCount );
					(*ppcsPackingFolder)->m_lBlockSize = lBlockCount;

					//���� ������ �����.
					fpDataDat = fopen( strDataFile, "wb" );

					if( fpDataDat )
					{
						for( int lBlockCounter=0; lBlockCounter<lBlockCount; lBlockCounter++ )
						{
							memset( strBuffer, 0, Packing_lBlockSize );
							fread( strBuffer, 1, Packing_lBlockSize, fpTempFile );

							fwrite( strBuffer, 1, Packing_lBlockSize, fpDataDat );
						}

						pcsPackingFile->m_lChildDataNodeCount = 1;
						pcsPackingFile->m_pcsDataNode = pcsPackingDataNode;

						fclose( fpDataDat );
					}
				}
				else
				{
					char		*pstrData;

					//File�� ��ġ�� ���� ������
					pcsPackingDataNode->m_lStartPos = ((*ppcsPackingFolder)->m_lBlockSize)*Packing_lBlockSize;
					pcsPackingDataNode->m_lSize = lFileSize;

					//���� ���� �����.
					delete [] (*ppcsPackingFolder)->m_pstrBlock;

					//�� ũ�⸦ ������Ų��.
					(*ppcsPackingFolder)->m_lBlockSize += lBlockCount;

					//���ο� ���� �Ҵ��Ѵ�.
					pstrData = new char[(*ppcsPackingFolder)->m_lBlockSize];
					memset( pstrData, 1, (*ppcsPackingFolder)->m_lBlockSize );

					//���� �Ҵ�� ���� �������ش�.
					(*ppcsPackingFolder)->m_pstrBlock = pstrData;

					fpDataDat = fopen( strDataFile, "ab" );

					if( fpDataDat )
					{
						for( int lBlockCounter=0; lBlockCounter<lBlockCount; lBlockCounter++ )
						{
							memset( strBuffer, 0, Packing_lBlockSize );
							fread( strBuffer, 1, Packing_lBlockSize, fpTempFile );

							fwrite( strBuffer, 1, Packing_lBlockSize, fpDataDat );
						}

						pcsPackingFile->m_lChildDataNodeCount = 1;
						pcsPackingFile->m_pcsDataNode = pcsPackingDataNode;

						fclose( fpDataDat );
					}
				}
			}
			//�� �� �ִٸ�?
			else
			{
				int				lExtendBlockCount;
				int				lExportBlockCount;

				sprintf( strDataFile, "%s\\%s", (*ppcsPackingFolder)->m_pstrFolderName, Packing_Data_FileName );

				fpDataDat = fopen( strDataFile, "rb+" );

				lExtendBlockCount = 0;
				lExportBlockCount = 0;

				if( fpDataDat )
				{
					//�켱 ������ �� ���� �� ���� �ִ����� ����.
					if( lBlockCount > (*ppcsPackingFolder)->m_lEmptyBlockCount )
					{
						char		*pstrData;

						lExtendBlockCount = lBlockCount - (*ppcsPackingFolder)->m_lEmptyBlockCount;

						pstrData = new char[(*ppcsPackingFolder)->m_lBlockSize+lExtendBlockCount];
						memset( pstrData, 0, (*ppcsPackingFolder)->m_lBlockSize+lExtendBlockCount );

						//���� ���� �����ϰ�.
						memcpy( pstrData, (*ppcsPackingFolder)->m_pstrBlock, (*ppcsPackingFolder)->m_lBlockSize );

						//�޸� �Ҵ��� Ǯ��
						delete [] (*ppcsPackingFolder)->m_pstrBlock;

						//�ٿ��ش�.
						(*ppcsPackingFolder)->m_pstrBlock = pstrData;

						//�� ��ũ��, ��ũ�⸦ ���� ������Ų��.
						(*ppcsPackingFolder)->m_lEmptyBlockCount += lExtendBlockCount;
						(*ppcsPackingFolder)->m_lBlockSize += lExtendBlockCount;
					}
					
					//�� ���� ���� ������ �Ҵ��Ѵ�.
					CPackingDataNode	*pcsPrevPackingDataNode;

					int				lIndex;
					int				lStart, lSize;
					int				lAllocedBlockCount = 0;
					int				lWriteFileSize;					//���� ������ ũ��

					lStart = -1;
					lSize = 0;
					lWriteFileSize = 0;

					pcsPrevPackingDataNode = pcsPackingFile->m_pcsDataNode;

					for( lIndex=0; lIndex<(*ppcsPackingFolder)->m_lBlockSize; lIndex++)
					{
						if( (*ppcsPackingFolder)->m_pstrBlock[lIndex] == 0 )
						{
							lStart = lIndex;
							lSize = 1;

							lIndex++;

							for( ; lIndex<(*ppcsPackingFolder)->m_lBlockSize; lIndex++ )
							{
								if( (*ppcsPackingFolder)->m_pstrBlock[lIndex] == 0 )
								{
									lSize++;
								}
								else
								{
									//lStart, lSize����.
									CPackingDataNode		*pcsTempPackingDataNode;

									pcsTempPackingDataNode = new CPackingDataNode;

									//���� �� ��尡 ���ٸ�?
									if( pcsPrevPackingDataNode == NULL )
									{
										pcsPackingFile->m_pcsDataNode = pcsTempPackingDataNode;
										pcsPrevPackingDataNode = pcsTempPackingDataNode;
									}
									else
									{
										pcsPrevPackingDataNode->m_pcsNextNode = pcsTempPackingDataNode;
										pcsPrevPackingDataNode = pcsTempPackingDataNode;
									}

									pcsPackingFile->m_lChildDataNodeCount++;

									//�޸� ���� ä���ش�.
									memset( &(*ppcsPackingFolder)->m_pstrBlock[lStart], 1, lSize );

									//��ġ �̵�~
									fseek( fpDataDat, lStart*Packing_lBlockSize, SEEK_SET );

									//���� ������ �����Ѵ�.
									for( int lCounter=0; lCounter<lSize; lCounter++ )
									{
										memset( strBuffer, 0, Packing_lBlockSize );
										fread( strBuffer, 1, Packing_lBlockSize, fpTempFile );
										lWriteFileSize += fwrite( strBuffer, 1,Packing_lBlockSize, fpDataDat );
									}

									//���� ������ġ�� ����ũ�⸦ ����Ѵ�.
									pcsTempPackingDataNode->m_lStartPos = lStart*Packing_lBlockSize;
									pcsTempPackingDataNode->m_lSize = lWriteFileSize;

									//���� ��ŭ �� ���� �ٿ��ش�.
									(*ppcsPackingFolder)->m_lEmptyBlockCount -= lSize;

									lStart = -1;
									lSize = 0;
									lWriteFileSize = 0;
									
									break;
								}
							}
						}
					}

					if( lSize != -1 )
					{
						//lStart, lSize����.
						CPackingDataNode		*pcsTempPackingDataNode;

						pcsTempPackingDataNode = new CPackingDataNode;

						//���� �� ��尡 ���ٸ�?
						if( pcsPrevPackingDataNode == NULL )
						{
							pcsPackingFile->m_pcsDataNode = pcsTempPackingDataNode;
							pcsPrevPackingDataNode = pcsTempPackingDataNode;
						}
						else
						{
							pcsPrevPackingDataNode->m_pcsNextNode = pcsTempPackingDataNode;
							pcsPrevPackingDataNode = pcsTempPackingDataNode;
						}

						pcsPackingFile->m_lChildDataNodeCount++;

						//�޸� ���� ä���ش�.
						memset( &(*ppcsPackingFolder)->m_pstrBlock[lStart], 1, lSize );

						//��ġ �̵�~
						fseek( fpDataDat, lStart*Packing_lBlockSize, SEEK_SET );

						//���� ������ �����Ѵ�.
						for( int lCounter=0; lCounter<lSize; lCounter++ )
						{
							memset( strBuffer, 0, Packing_lBlockSize );
							fread( strBuffer, 1, Packing_lBlockSize, fpTempFile );
							lWriteFileSize += fwrite( strBuffer, 1,Packing_lBlockSize, fpDataDat );
						}

						//���� ������ġ�� ����ũ�⸦ ����Ѵ�.
						pcsTempPackingDataNode->m_lStartPos = lStart*Packing_lBlockSize;
						pcsTempPackingDataNode->m_lSize = lWriteFileSize;

						//���� ��ŭ �� ���� �ٿ��ش�.
						(*ppcsPackingFolder)->m_lEmptyBlockCount -= lSize;
					}

					fclose( fpDataDat );
				}
			}

			//������ ������ ����
			//1.���۷��̼�
			pcsPackingFile->m_lOperation = Packing_Operation_Add;
			//2.����ũ��
			pcsPackingFile->m_lFileNameSize = strlen( pstrFileName ) + 1;
			//3.�����̸�
			pcsPackingFile->m_pstrFileName = new char[pcsPackingFile->m_lFileNameSize];
			memset( pcsPackingFile->m_pstrFileName, 0, sizeof(pcsPackingFile->m_lFileNameSize) );
			strcat( pcsPackingFile->m_pstrFileName, pstrFileName );
			//4.����ũ��
			pcsPackingFile->m_lFileSize = lFileSize;
			//5.üũ���� �ʿ�����Ƿ� �߰����� �ʴ´�.

			if( (*ppcsPackingFolder)->m_csFilesInfo.AddObject( (void **)&pcsPackingFile, _strlwr(pstrFileName) ) )
			{
				bResult = true;
			}

			fclose( fpTempFile );
		}
	}

	return bResult;
}*/

/*bool AuPackingManager::RemovePackingFile( char *pstrFolderName, char *pstrFileName )
{
	bool			bResult;

	bResult = false;

	CPackingFolder		**ppcsPackingFolder;

	ppcsPackingFolder = (CPackingFolder	**)m_csFolderInfo.GetObject( _strlwr(pstrFolderName) );

	//�����ϴ� �����ΰ�?
	if( ppcsPackingFolder && *ppcsPackingFolder && pstrFolderName && pstrFileName )
	{
		CPackingFile			**ppcsPackingFile;

		ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObject( _strlwr(pstrFileName) );

		//�ִٸ�!! �����~
		if( ppcsPackingFile && (*ppcsPackingFile) )
		{
			CPackingDataNode			*pstrTempNode;
			int					lStartPos;
			int					lBlockCount;

			for( pstrTempNode = (*ppcsPackingFile)->m_pcsDataNode; pstrTempNode; pstrTempNode=pstrTempNode->m_pcsNextNode )
			{
				lStartPos = pstrTempNode->m_lStartPos/Packing_lBlockSize;

				lBlockCount = pstrTempNode->m_lSize/Packing_lBlockSize;
				if( (pstrTempNode->m_lSize%Packing_lBlockSize) != 0 )
				{
					lBlockCount++;
				}

				//���� ������ �����.
				memset( &(*ppcsPackingFolder)->m_pstrBlock[lStartPos], 0, lBlockCount );
				//���ŭ �߰��Ѵ�.
                (*ppcsPackingFolder)->m_lEmptyBlockCount += lBlockCount;
			}

			//���۷������� �����Ѵ�.
			(*ppcsPackingFile)->Reset();

			(*ppcsPackingFolder)->m_csFilesInfo.RemoveObject( _strlwr(pstrFileName) );
		}
	}

	return bResult;	
}*/


//@{ 2006/05/05 burumal
//CPackingFolder *AuPackingManager::GetFolder( char *pstrFolderName )
CPackingFolder *AuPackingManager::GetFolder( char *pstrFolderName, bool bAlreadlyLowercase )
//@}
{
	CPackingFolder** ppcsPackingFolder;

	if ( bAlreadlyLowercase )
		ppcsPackingFolder = (CPackingFolder**) m_csFolderInfo.GetObject( pstrFolderName );
	else
		ppcsPackingFolder = (CPackingFolder**) m_csFolderInfo.GetObject( _strlwr(pstrFolderName) );

	if( (ppcsPackingFolder != NULL) && (*ppcsPackingFolder) != NULL )
	{
		return (*ppcsPackingFolder);
	}
	else 
	{
		return NULL;
	}
}

//@{ 2006/05/05 burumal
//CPackingFile *AuPackingManager::GetFile( char *pstrFolderName, char *pstrFileName )
CPackingFile *AuPackingManager::GetFile( char *pstrFolderName, char *pstrFileName, bool bAlreadlyLowercase  )
//@}
{
	CPackingFolder		**ppcsPackingFolder;

	if ( bAlreadlyLowercase )
		ppcsPackingFolder = (CPackingFolder	**)m_csFolderInfo.GetObject( pstrFolderName );
	else
		ppcsPackingFolder = (CPackingFolder	**)m_csFolderInfo.GetObject( _strlwr(pstrFolderName) );

	if( ppcsPackingFolder && (*ppcsPackingFolder) )
	{
		CPackingFile		**ppcsPackingFile;

		if ( bAlreadlyLowercase )
			ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObject( pstrFileName );
		else
			ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObject( _strlwr(pstrFileName) );

		if( ppcsPackingFile && (*ppcsPackingFile) )
		{
			return (*ppcsPackingFile);
		}
		else
		{
			return NULL;
		}
	}
	else 
	{
		return NULL;
	}
}

bool AuPackingManager::AddPatch( CPatchVersionInfo *pcsPatchVersionInfo )
{
	bool				bResult;

	bResult = false;

	if( m_cPatchList.Add( pcsPatchVersionInfo ) )
	{
		bResult = true;
	}

	return bResult;
}

bool AuPackingManager::BuildJZPFile( int lVersion, char *pstrExportFolder, CPatchVersionInfo *pcsPatchVersionInfo, HWND hDisplayWnd, HWND hProgressWnd )
{
	CListNode<CPatchFolderInfo *>		*pcsPatchFolderNode;
	CListNode<CPackingFile *>			*pcsPatchFileNode;

	bool			bResult;
	char			strFullFileName[256];

	bResult = false;
	
	if( pcsPatchVersionInfo )
	{
		FILE			*fpJZPFile;
		FILE			*fpResFile;

		bool			bPackingFolder;
		char			strJZPFileName[256];
		char			strResFileName[256];
		char			strBuffer[256];
		int				iFileHandle;
		int				iFileSize;
		size_t			iFileNameSize;
		int				iFolderCount;
		int				iFileCount;
		size_t			iFolderNameLen;
		int				iTotalFileCount;
		int				iProgressCurrentPostion;

		//���α׷��� �� ������ ���� ī��Ʈ�� ����.
		if( hProgressWnd )
		{
			iTotalFileCount = 0;
			iProgressCurrentPostion = 0;

			for( pcsPatchFolderNode=pcsPatchVersionInfo->m_clFolderList.GetStartNode(); pcsPatchFolderNode; pcsPatchFolderNode=pcsPatchFolderNode->m_pcNextNode )
			{
				iTotalFileCount += pcsPatchFolderNode->m_tData->m_clFileList.getCount();
			}

			//��������
			//SendMessage( hProgressWnd, PBM_SETRANGE, 0, MAKELPARAM( 0, iTotalFileCount) );
			//��ġ�ʱ�ȭ
			//SendMessage( hProgressWnd, PBM_SETPOS, 0, 0 );

			::SendMessage( hProgressWnd , WM_USER+84 , 0 , MAKELPARAM( 0 , iTotalFileCount ) );
		}

		sprintf( strResFileName, "%s\\Patch%04d.JZP", pstrExportFolder, lVersion );

		fpResFile = fopen( strResFileName, "wb" );

		//���� ���
		fwrite( &lVersion, 1, sizeof(int), fpResFile );

		//��ü���ϰ����� ���ؼ� ��ΰ�?
		fwrite( &iTotalFileCount, 1, sizeof(int), fpResFile );

		//���� ���� ���
		iFolderCount = pcsPatchVersionInfo->m_clFolderList.getCount();
		fwrite( &iFolderCount, 1, sizeof(int), fpResFile );

		for( pcsPatchFolderNode=pcsPatchVersionInfo->m_clFolderList.GetStartNode(); pcsPatchFolderNode; pcsPatchFolderNode=pcsPatchFolderNode->m_pcNextNode )
		{
			//�����̸� ��������
			iFolderNameLen = strlen( pcsPatchFolderNode->m_tData->m_pstrFolderName ) + 1;
			fwrite( &iFolderNameLen, 1, sizeof(int), fpResFile );

			//�����̸� ����
			fwrite( pcsPatchFolderNode->m_tData->m_pstrFolderName, 1, iFolderNameLen, fpResFile );

			//���ϰ������
			iFileCount = pcsPatchFolderNode->m_tData->m_clFileList.getCount();
			fwrite( &iFileCount, 1, sizeof(int), fpResFile );

			//��ŷ�� ���õ� ������ �ö󰡴��� �ùķ��̼��غ���.
			bPackingFolder = false;

			for( pcsPatchFileNode = pcsPatchFolderNode->m_tData->m_clFileList.GetStartNode(); pcsPatchFileNode; pcsPatchFileNode=pcsPatchFileNode->m_pcNextNode )
			{
				if( pcsPatchFileNode->m_tData->m_bPacking == true )
				{
					bPackingFolder = true;
					break;
				}
			}

			//�� ������ ���ؼ� ��ŷ������ �ִ����� �����Ѵ�.
			fwrite( &bPackingFolder, 1, sizeof(bool), fpResFile );

			for( pcsPatchFileNode = pcsPatchFolderNode->m_tData->m_clFileList.GetStartNode(); pcsPatchFileNode; pcsPatchFileNode=pcsPatchFileNode->m_pcNextNode )
			{
				if( !strcmpi( "root", pcsPatchFolderNode->m_tData->m_pstrFolderName ) )
				{
					sprintf( strFullFileName, "%s", pcsPatchFileNode->m_tData->m_pstrFileName );
				}
				else
				{
					sprintf( strFullFileName, "%s\\%s", pcsPatchFolderNode->m_tData->m_pstrFolderName, pcsPatchFileNode->m_tData->m_pstrFileName );
				}

				if( hDisplayWnd )
				{
					//���÷���
					sprintf( strBuffer, "[Building JZP] %s ó����", strFullFileName );
					//SetWindowText( hDisplayWnd, strBuffer );
					//::SendMessage( hDisplayWnd , LB_ADDSTRING , NULL , (LPARAM)strBuffer );
					//::SendMessage( hDisplayWnd , LB_SETCARETINDEX , (WPARAM)::SendMessage( hDisplayWnd , LB_GETCOUNT , NULL , NULL ) , TRUE );					
					iProgressCurrentPostion++;
					//::SendMessage( hProgressWnd, PBM_SETPOS, iProgressCurrentPostion, 0 );

					::SendMessage( hDisplayWnd , WM_USER+83 , (WPARAM)strBuffer , iProgressCurrentPostion );
				}

				if( pcsPatchFileNode->m_tData->m_lOperation == Packing_Operation_Add )
				{
					//�Ʒ��� �����̸��� ������ �����Ͽ� Ư���� ������ �����.
					sprintf( strJZPFileName, "ZipDirs\\%d\\%s", lVersion, strFullFileName );

					iFileNameSize = pcsPatchFileNode->m_tData->m_lFileNameSize; //Null��������.

					fpJZPFile = fopen( strJZPFileName, "rb" );

					if( fpJZPFile != NULL )
					{
						//������ �о�� ����.
						char			*pstrJZPFile;

						//���� ���̸� ����.
						iFileHandle = fileno( fpJZPFile );
						iFileSize = filelength( iFileHandle );

						pstrJZPFile = new char[iFileSize];

						fread( pstrJZPFile, 1, iFileSize, fpJZPFile );

						fclose( fpJZPFile );

						//��ŷ���� �����Ѵ�.
						fwrite( &pcsPatchFileNode->m_tData->m_bPacking, 1, sizeof(bool), fpResFile );

						//Res���Ͽ�����.
						//0.File Operation�� ����.
						fwrite( &pcsPatchFileNode->m_tData->m_lOperation, 1, sizeof( pcsPatchFileNode->m_tData->m_lOperation ), fpResFile );
						//1.FileNameSize
						fwrite( &iFileNameSize, 1, sizeof(iFileNameSize), fpResFile );
						//2.FileName�� ����.
						fwrite( pcsPatchFileNode->m_tData->m_pstrFileName, 1, iFileNameSize, fpResFile );
						//3.FileSize
						fwrite( &iFileSize, 1, sizeof(iFileSize), fpResFile );
						//4.FileData;
						fwrite( pstrJZPFile, 1, iFileSize, fpResFile );

						delete [] pstrJZPFile;
					}
				}
				else if( pcsPatchFileNode->m_tData->m_lOperation == Packing_Operation_Remove )
				{
					iFileNameSize = pcsPatchFileNode->m_tData->m_lFileNameSize; //Null��������.

					//��ŷ���� �����Ѵ�.
					fwrite( &pcsPatchFileNode->m_tData->m_bPacking, 1, sizeof(bool), fpResFile );

					//Res���Ͽ�����.
					//0.File Operation�� ����.
					fwrite( &pcsPatchFileNode->m_tData->m_lOperation, 1, sizeof( pcsPatchFileNode->m_tData->m_lOperation ), fpResFile );
					//1.FileNameSize
					fwrite( &iFileNameSize, 1, sizeof(iFileNameSize), fpResFile );
					//2.FileName�� ����.
					fwrite( pcsPatchFileNode->m_tData->m_pstrFileName, 1, iFileNameSize, fpResFile );
				}
			}
		}

		fclose( fpResFile );

		bResult = true;
	}
	else
	{
		bResult = false;
	}

	return bResult;
}

bool AuPackingManager::LoadResFile( char *pstrResFileName )
{
	FILE			*file;

	bool			bResult;

	bResult = false;

	if( pstrResFileName )
	{
		file = fopen( pstrResFileName, "rb" );

		if( file )
		{
			CPatchVersionInfo	*pcsPatchVersionInfo;

			int					lMaxPatchCount;
			int					lPatchCount;
			int					lMaxPatchFolderCount;
			int					lPatchFolderCount;
			int					lMaxPatchFileCount;
			int					lPatchFileCount;

			fread( &m_bBuildJZP		, 1 , sizeof(bool) , file );
			fread( &m_bBuildDat		, 1 , sizeof(bool) , file );
			fread( &m_bCopyRawFiles	, 1 , sizeof(bool) , file );

			//�� ������~
			fread( &lMaxPatchCount	, 1 , sizeof( int ) , file );

			for( lPatchCount=0; lPatchCount<lMaxPatchCount; ++lPatchCount )
			{
				pcsPatchVersionInfo = new CPatchVersionInfo;
				m_cPatchList.Add( pcsPatchVersionInfo );

				//���� ������ �д´�.
				fread( &pcsPatchVersionInfo->m_lVersion, 1, sizeof(int), file );

				//���� ����.
				fread( &lMaxPatchFolderCount, 1, sizeof(int), file );

				for( lPatchFolderCount=0; lPatchFolderCount<lMaxPatchFolderCount; ++lPatchFolderCount )
				{
					CPatchFolderInfo	*pcsPatchFolderInfo;

					int				lFolderNameLen;

					pcsPatchFolderInfo = new CPatchFolderInfo;
					pcsPatchVersionInfo->m_clFolderList.Add( pcsPatchFolderInfo );

					//Folder Name Length
					fread( &lFolderNameLen, 1, sizeof(int), file );

					pcsPatchFolderInfo->m_pstrFolderName = new char[lFolderNameLen];
					memset( pcsPatchFolderInfo->m_pstrFolderName, 0, lFolderNameLen );

					//Folder Name
					fread( pcsPatchFolderInfo->m_pstrFolderName, 1, lFolderNameLen, file );

					fread( &lMaxPatchFileCount, 1, sizeof(int), file );

					for(lPatchFileCount = 0; lPatchFileCount<lMaxPatchFileCount; ++lPatchFileCount )
					{
						CPackingFile			*pcsPackingFile;
						pcsPackingFile = new CPackingFile;

						pcsPatchFolderInfo->m_clFileList.Add( pcsPackingFile );

						//Operation
						fread( &pcsPackingFile->m_lOperation, 1, sizeof(int), file );
						//FileNameSize
						fread( &pcsPackingFile->m_lFileNameSize, 1, sizeof(int), file );

						pcsPackingFile->m_pstrFileName = new char[pcsPackingFile->m_lFileNameSize];
						memset( pcsPackingFile->m_pstrFileName, 0, pcsPackingFile->m_lFileNameSize );

						//FileName
						fread( pcsPackingFile->m_pstrFileName, 1, pcsPackingFile->m_lFileNameSize, file );
						//FileSize;
						fread( &pcsPackingFile->m_lFileSize, 1, sizeof(int), file );
						//CheckSum
						fread( &pcsPackingFile->m_lFileCheckSum, 1, sizeof(int), file );
					}
				}
			}

			//JZP ���� ������ ����.
			int				iJZPFiles;

			fread( &iJZPFiles, 1, sizeof( iJZPFiles ), file );

			m_csJZPFileInfo.InitializeObject( sizeof(CJZPFileInfo *), iJZPFiles );

			for( int lCounter=0; lCounter<iJZPFiles; lCounter++ )
			{
				CJZPFileInfo	*pcJZPFileInfo;

				int				iVersion;
				int				iFileNameSize;
				char			*pstrFileName;
				int				iFileSize;
				int				iRawFileSize;
				unsigned long	iCRCCheckSum;

				if( feof( file ) )
					break;

				fread( &iVersion, 1, sizeof( iVersion ), file );
				fread( &iFileNameSize, 1, sizeof( iFileNameSize ), file );

				if( feof( file ) )
					break;

				pstrFileName = new char [iFileNameSize];
				memset( pstrFileName, 0, iFileNameSize );

				fread( pstrFileName, 1, iFileNameSize, file );
				fread( &iFileSize, 1, sizeof( iFileSize ), file );
				fread( &iRawFileSize, 1, sizeof( iRawFileSize ), file );
				fread( &iCRCCheckSum, 1, sizeof( unsigned long ), file );

				if( feof( file ) )
					break;

				pcJZPFileInfo = new CJZPFileInfo( iVersion, iFileNameSize, pstrFileName, iFileSize, iRawFileSize, iCRCCheckSum );

				m_csJZPFileInfo.AddObject( (void **)&pcJZPFileInfo, iVersion );
			}

			//���� ����.
			m_lCurrentVersion = GetLastVersion();

			//Ʈ������
			CListNode<CPatchVersionInfo *>	*pcsVersionInfo;
			CListNode<CPatchFolderInfo *>	*pcsFolderInfo;
			CListNode<CPackingFile *>		*pcsFileInfo;
			CPackingFile					*pcsPackingFile;

			//�� �������� �����Ѵ�.
			for( pcsVersionInfo = m_cPatchList.GetEndNode(); pcsVersionInfo; pcsVersionInfo=pcsVersionInfo->m_pcPrevNode )
			{
				//������ ���������� ������~
				for( pcsFolderInfo = pcsVersionInfo->m_tData->m_clFolderList.GetStartNode(); pcsFolderInfo; pcsFolderInfo=pcsFolderInfo->m_pcNextNode )
				{
					AddFolder( pcsFolderInfo->m_tData->m_pstrFolderName );

					for( pcsFileInfo = pcsFolderInfo->m_tData->m_clFileList.GetStartNode(); pcsFileInfo; pcsFileInfo=pcsFileInfo->m_pcNextNode )
					{
						//�̹� ����ֳ�����. ������ ����~
						pcsPackingFile = GetFile( pcsFolderInfo->m_tData->m_pstrFolderName, pcsFileInfo->m_tData->m_pstrFileName );

						//������ �ִ´�.
						if( pcsPackingFile == NULL )
						{
							AddFile( pcsFolderInfo->m_tData->m_pstrFolderName, pcsFileInfo->m_tData, pcsFileInfo->m_tData->m_lFileSize, pcsFileInfo->m_tData->m_lFileCheckSum );
						}
					}
				}
			}

			bResult = true;

			fclose( file );
		}
	}

	return bResult;
}

/*
bool AuPackingManager::BuildReferenceFile( char *pstrPackingFolder, char *pstrSourceFolder, bool bResort, HWND hDisplayWnd, HWND hProgressWnd )
{
	FILE			*file;
	bool			bResult;

	char			strCurrentDir[255];
	char			strPackingFolder[255];
	char			strDisplayBuffer[255];
	int				lFolderIndex;
	int				lFileIndex;
	int				lFileCount;
	int				lStartPos;

	lFolderIndex = 0;
	bResult = false;

	CPackingFolder		**ppcsPackingFolder;
	CPackingFile		**ppcsPackingFile;
	CPackingDataNode	*pcsPackingDataNode;

	GetCurrentDirectory( sizeof(strCurrentDir), strCurrentDir );
	sprintf( strPackingFolder, "%s\\%s", strCurrentDir, pstrPackingFolder );

	for( ppcsPackingFolder = (CPackingFolder **)m_csFolderInfo.GetObjectSequence( &lFolderIndex ); ppcsPackingFolder; ppcsPackingFolder = (CPackingFolder	**)m_csFolderInfo.GetObjectSequence( &lFolderIndex ) )
	{
		if( (*ppcsPackingFolder) == NULL )
			continue;

		//Ref������ Export���� �����Ѵ�.
		if( (*ppcsPackingFolder)->m_bExportToRef == false )
			continue;

		SetCurrentDirectory( strPackingFolder );

		//������ �ȵǸ� ������ ����� �ٽ� �����Ѵ�.
		if( !strcmpi( "root", (*ppcsPackingFolder)->m_pstrFolderName ) )
		{
			; //��Ʈ �̸��� Ư���� ���𰡴� ����. ;;
		}
		else
		{
			if( SetCurrentDirectory( (*ppcsPackingFolder)->m_pstrFolderName ) == FALSE )
			{
				CreateFolder( (*ppcsPackingFolder)->m_pstrFolderName );
				SetCurrentDirectory( (*ppcsPackingFolder)->m_pstrFolderName );
			}
		}

		//��ŷ�� ������ ����� ����. ���� 0���� �׳� �����Ѵ�.
		lFileIndex = 0;
		lFileCount = 0;
		for( ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObjectSequence( &lFileIndex ); ppcsPackingFile; ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObjectSequence( &lFileIndex ) )
		{
			if( (*ppcsPackingFile)->m_lOperation == Packing_Operation_Remove )
				continue;

			if( (*ppcsPackingFile)->m_bPacking == true )
				lFileCount++;
		}

		if( lFileCount == 0 )
			continue;

		file = fopen( Packing_Reference_FileName, "wb" );

		if( (*ppcsPackingFolder) && file )
		{
			char				lEditing;

			lEditing = AuPacking_Status_Build;
			lFileIndex = 0;
			lStartPos = 0;

			//���� �����ϴ°��̴� ������ �������̴�~
			fwrite( &lEditing, 1, sizeof(bool), file );

			//���� �̸�ũ��
			fwrite( &(*ppcsPackingFolder)->m_lFolderNameSize, 1, sizeof(int), file );

			//���� �̸�
			fwrite( (*ppcsPackingFolder)->m_pstrFolderName, 1, (*ppcsPackingFolder)->m_lFolderNameSize, file );

			lFileCount = (*ppcsPackingFolder)->m_csFilesInfo.GetObjectCount();

			fwrite( &lFileCount, 1, sizeof(int), file );

			if( bResort == false )
			{
				//��ü������ ���� ��ΰ�?
				fwrite( &(*ppcsPackingFolder)->m_lBlockSize, 1, sizeof(int), file );

				//���� ũ�⸸ŭ �����Ѵ�.
				fwrite( (*ppcsPackingFolder)->m_pstrBlock, 1, (*ppcsPackingFolder)->m_lBlockSize, file );

				//����� ��ΰ�?
				fwrite( &(*ppcsPackingFolder)->m_lEmptyBlockCount, 1, sizeof(int), file );
			}
			//�������϶�
			else //if( bResort == true )
			{
				int				lFileTotalSize;
				int				lEmptyBlockCount;
				char			*pstrTempBlock;
				int				lFileSize;

				lFileTotalSize = 0;
				lEmptyBlockCount = 0; //�������� ��� ����� ������~ 0�̴�.
				lFileIndex = 0;

				//���� ũ�⸦ ����Ѵ�.
				for( ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObjectSequence( &lFileIndex ); ppcsPackingFile; ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObjectSequence( &lFileIndex ) )
				{
					if( (*ppcsPackingFile)->m_lOperation == Packing_Operation_Remove )
						continue;

					if( (*ppcsPackingFile)->m_bPacking == false )
						continue;

					lFileSize = (*ppcsPackingFile)->m_lFileSize;

					lFileTotalSize += lFileSize/Packing_lBlockSize;

					if( (lFileSize%Packing_lBlockSize) != 0 )
					{
						lFileTotalSize++;
					}
				}

				//�����Ľÿ��� ������ 1(��, ���� ���� ����ִ�.)�� �����Ѵ�.
				pstrTempBlock = new char[lFileTotalSize];
				memset( pstrTempBlock, 1, lFileTotalSize );

				//��ü������ ���� ��ΰ�?
				fwrite( &lFileTotalSize, 1, sizeof(int), file );

				//���� ũ�⸸ŭ �����Ѵ�.
				fwrite( pstrTempBlock, 1, lFileTotalSize, file );

				//����� ��ΰ�?
				fwrite( &lEmptyBlockCount, 1, sizeof(int), file );
			}

			lFileIndex = 0;

			for( ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObjectSequence( &lFileIndex ); ppcsPackingFile; ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObjectSequence( &lFileIndex ) )
			{
				if( (*ppcsPackingFile)->m_lOperation == Packing_Operation_Remove )
					continue;

				if( (*ppcsPackingFile)->m_bPacking == false )
					continue;

				if( hDisplayWnd )
				{
					//���� ǥ��
					sprintf( strDisplayBuffer, "[Build Ref] %s\\%s ó����", (*ppcsPackingFolder)->m_pstrFolderName, (*ppcsPackingFile)->m_pstrFileName );
					SetWindowText( hDisplayWnd, strDisplayBuffer );
				}

				//�����̸�ũ��
				fwrite( &(*ppcsPackingFile)->m_lFileNameSize, 1, sizeof(int), file );

				//�����̸�
				fwrite( (*ppcsPackingFile)->m_pstrFileName, 1, (*ppcsPackingFile)->m_lFileNameSize, file );

				if( bResort == false )
				{
					//������ ��ġ��������
					fwrite( &(*ppcsPackingFile)->m_lChildDataNodeCount, 1, sizeof(int), file );

					pcsPackingDataNode = (*ppcsPackingFile)->m_pcsDataNode;

					for( int lChildCount=0; lChildCount<(*ppcsPackingFile)->m_lChildDataNodeCount; lChildCount++ )
					{
						if( pcsPackingDataNode == NULL )
							break;

						//������
						fwrite( &pcsPackingDataNode->m_lStartPos, 1, sizeof(int), file );

						//ũ��
						fwrite( &pcsPackingDataNode->m_lSize, 1, sizeof(int), file );

						pcsPackingDataNode = pcsPackingDataNode->m_pcsNextNode;
					}
				}
				//�������϶�
				else //if( bResort == true )
				{
					int				lChildDataNodeCount;
					int				lFileSize;

					//�������϶��� ChildDataNodeCount�� ������ 0�̴�.
					lChildDataNodeCount = 1;
					
					fwrite( &lChildDataNodeCount, 1, sizeof(int), file );

					//������
					fwrite( &lStartPos, 1, sizeof(int), file );

					lFileSize = (*ppcsPackingFile)->m_lFileSize;
					//ũ��
					fwrite( &lFileSize, 1, sizeof(int), file );

					lStartPos += ((lFileSize/Packing_lBlockSize)*Packing_lBlockSize);

					if( (lFileSize%Packing_lBlockSize) != 0 )
					{
						lStartPos += Packing_lBlockSize;
					}
				}
			}

			fclose( file );
		}
	}

	//Ref���� ��ũ��Ʈ����
	lFolderIndex = 0;

	for( ppcsPackingFolder = (CPackingFolder **)m_csFolderInfo.GetObjectSequence( &lFolderIndex ); ppcsPackingFolder; ppcsPackingFolder = (CPackingFolder	**)m_csFolderInfo.GetObjectSequence( &lFolderIndex ) )
	{
		if( (*ppcsPackingFolder) == NULL )
			continue;

		//Ref������ Export���� �����Ѵ�.
		if( (*ppcsPackingFolder)->m_bExportToRef == false )
			continue;

		//��ŷ�� ������ ����� ����. ���� 0���� �׳� �����Ѵ�.
		lFileIndex = 0;
		lFileCount = 0;
		for( ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObjectSequence( &lFileIndex ); ppcsPackingFile; ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObjectSequence( &lFileIndex ) )
		{
			if( (*ppcsPackingFile)->m_lOperation == Packing_Operation_Remove )
				continue;

			if( (*ppcsPackingFile)->m_bPacking == true )
				lFileCount++;
		}

		if( lFileCount == 0 )
			continue;

		SetCurrentDirectory( strPackingFolder );

		//������ �ȵǸ� ������ ����� �ٽ� �����Ѵ�.
		if( !strcmpi( "root", (*ppcsPackingFolder)->m_pstrFolderName ) )
		{
			; //��Ʈ �̸��� Ư���� ���𰡴� ����. ;;
		}
		else
		{
			if( SetCurrentDirectory( (*ppcsPackingFolder)->m_pstrFolderName ) == FALSE )
			{
				CreateFolder( (*ppcsPackingFolder)->m_pstrFolderName );
				SetCurrentDirectory( (*ppcsPackingFolder)->m_pstrFolderName );
			}
		}

		file = fopen( Packing_Reference_FileName, "rb" );

		if( file )
		{
			long				lFileSize;
			char				*pstrEncryptBuffer;
			char				lEditing;

			lEditing = AuPacking_Status_Build;

			//���� ũ�⸦ ��´�.
			fseek( file, 0, SEEK_END );
			lFileSize = ftell(file) - 1; //����տ� lEditing(char)���� ���ڵ����� �ʴ´�.
			//����ũ�⸦ ������� �ٽ� ������~
			fseek( file, 1, SEEK_SET );  //����տ� lEditing(char)���� �ǳʶڴ�.

			pstrEncryptBuffer = new char[lFileSize];

			fread( pstrEncryptBuffer, 1, lFileSize, file );

			fclose( file );

			//Key�� "archlord"�̴�.
			m_cMD5Encrypt.EncryptString( "archlord", pstrEncryptBuffer, lFileSize );

			file = fopen( Packing_Reference_FileName, "wb" );
			if( file )
			{
				//���� �տ� �������� �ڵ带 �����Ѵ�.
				fwrite( &lEditing, 1, sizeof(char), file );
				fwrite( pstrEncryptBuffer, 1, lFileSize, file );

				fclose( file );
			}

			delete [] pstrEncryptBuffer;
		}
	}

	//�� �������� ���� ���丮�� ������.
	SetCurrentDirectory( strCurrentDir );

	return bResult;
}
*/

bool AuPackingManager::MarkPacking( char *pstrPackingFolder )
{
	if( pstrPackingFolder )
	{
		FILE			*file;
		char			strFilePath[255];

		sprintf( strFilePath, "%s\\%s", pstrPackingFolder, Packing_Reference_FileName );

		file = fopen( strFilePath, "rb+" );

		if( file )
		{
			char			lEditing;

			lEditing = AuPacking_Status_Complete;

			fwrite( &lEditing, 1, sizeof(char), file );

			fclose( file );
		}
	}

	return true;
}

bool AuPackingManager::MarkPackingComplete( char *pstrPackingFolder )
{
	CPackingFolder		**ppcsPackingFolder;
	FILE			*file;

	char			lEditing;
	char			strCurrentDir[255];
	char			strPackingFolder[255];
	int				lFolderIndex;

	lFolderIndex = 0;
	lEditing = AuPacking_Status_Complete;

	GetCurrentDirectory( sizeof(strCurrentDir), strCurrentDir );
	sprintf( strPackingFolder, "%s\\%s", strCurrentDir, pstrPackingFolder );

	for( ppcsPackingFolder = (CPackingFolder **)m_csFolderInfo.GetObjectSequence( &lFolderIndex ); ppcsPackingFolder; ppcsPackingFolder = (CPackingFolder	**)m_csFolderInfo.GetObjectSequence( &lFolderIndex ) )
	{
		if( (*ppcsPackingFolder) == NULL )
			continue;

		//Ref������ Export���� �����Ѵ�.
		if( (*ppcsPackingFolder)->m_bExportToRef == false )
			continue;

		SetCurrentDirectory( strPackingFolder );
		SetCurrentDirectory( (*ppcsPackingFolder)->m_pstrFolderName );

		//���������� ��ŷ �Ϸ� �ڵ带 �־��ش�.
		file = fopen( Packing_Reference_FileName, "rb+" );
		if( file )
		{
			//���� �տ� �������� �ڵ带 �����Ѵ�.
			fwrite( &lEditing, 1, sizeof(char), file );
			fclose( file );
		}
	}

	//�� �������� ���� ���丮�� ������.
	SetCurrentDirectory( strCurrentDir );

	return true;
}

/*
bool AuPackingManager::LoadReferenceFile( LPCTSTR pstrCurrentPath, bool bReadOnly, bool bRecursiveSubDir )
{
	HANDLE				hHandle;
	WIN32_FIND_DATA		csFindData;
	char				strSearchFolder[255];
	char				strCurrentFolder[255];

	bool				bResult;

	bResult = false;

	if( pstrCurrentPath == NULL )
	{
		GetCurrentDirectory( sizeof(strCurrentFolder), strCurrentFolder );
        sprintf( strSearchFolder, "%s\\*.*", strCurrentFolder );
	}
	else
	{
		sprintf( strCurrentFolder, "%s", pstrCurrentPath );
        sprintf( strSearchFolder, "%s\\*.*", pstrCurrentPath );
	}

	hHandle = FindFirstFile( strSearchFolder, &csFindData );

	while( 1 )
	{
		if( FindNextFile( hHandle, &csFindData ) == FALSE )
			break;

		//�����ΰ�?
		if( csFindData.dwFileAttributes == 16 )
		{
			if( !strcmpi( ".", csFindData.cFileName ) || !strcmpi( "..", csFindData.cFileName ) )
			{
				continue;
			}
			else
			{
				//���� ���丮�� Ÿ�� ���� �˻��Ұ��ΰ�?
				if( bRecursiveSubDir == true )
				{
					char			strBuffer[255];

					sprintf( strBuffer, "%s\\%s", strCurrentFolder, csFindData.cFileName );

					LoadReferenceFile( strBuffer );
				}
			}
		}
		else
		{
			int			lFileCount;

			//Reference.Dat������ �ִٸ� �д´�.
			if( !strcmpi( Packing_Reference_FileName, csFindData.cFileName) )
			{
				FILE				*file;
				char				lEditing;
				char				strFullPath[255];
				char				*pstrEncryptBuffer;
				char				*pstrDeletePointer;
				int					lEncryptSize;

				sprintf( strFullPath, "%s\\%s", pstrCurrentPath, csFindData.cFileName );

				file = fopen( strFullPath, "rb" );
				
				if( file )
				{
					//����ũ�⸦ ��´�.
					fseek( file, 0, SEEK_END );
					lEncryptSize = ftell(file) - 1;		//�� �տ� Editing(bool)���� �н�~ 1Byte�� ���� �ʴ´�.
					//���� ũ�⸦ ������� �ٽ� ������~
					fseek( file, 0, SEEK_SET );

					pstrEncryptBuffer = new char[lEncryptSize];
					pstrDeletePointer = pstrEncryptBuffer; //���������� �����͸� �����صд�.

					fread( &lEditing, 1, sizeof(char), file );
					fread( pstrEncryptBuffer, 1, lEncryptSize, file );

					fclose( file );

					//������ �� �о����� ��ȣȭǮ��~
					if( m_cMD5Encrypt.EncryptString( "archlord", pstrEncryptBuffer, lEncryptSize ) )
					{
						CPackingFolder			*pcsPackingFolder;
						CPackingFile			*pcsPackingFile;
						CPackingDataNode		*pcsPrevPackingDataNode;

						pcsPackingFolder = new CPackingFolder;

						pcsPackingFolder->m_lEditing = lEditing;

						//���� �̸�ũ��
						//fread( &pcsPackingFolder->m_lFolderNameSize, 1, sizeof(int), file );
						memcpy( &pcsPackingFolder->m_lFolderNameSize, pstrEncryptBuffer, sizeof(int) );
						pstrEncryptBuffer+=sizeof(int);

						//���� �̸�
						pcsPackingFolder->m_pstrFolderName = new char[pcsPackingFolder->m_lFolderNameSize];
						memset( pcsPackingFolder->m_pstrFolderName, 0, pcsPackingFolder->m_lFolderNameSize );
						//fread( pcsPackingFolder->m_pstrFolderName, 1, pcsPackingFolder->m_lFolderNameSize, file );
						memcpy( pcsPackingFolder->m_pstrFolderName, pstrEncryptBuffer, pcsPackingFolder->m_lFolderNameSize );
						pstrEncryptBuffer+=pcsPackingFolder->m_lFolderNameSize;

						//���� �������̾��� �����̾��ٸ�?
						if( pcsPackingFolder->m_lEditing == AuPacking_Status_Editing )
						{
							//�������� ����Ʈ�� �߰��Ѵ�.
							AddCrackedFolderName( pcsPackingFolder->m_pstrFolderName );
						}
						else
						{
							//��� ������ �ִ��� �д´�.
							//fread( &lFileCount, 1, sizeof(int), file );
							memcpy( &lFileCount, pstrEncryptBuffer, sizeof(int) );
							pstrEncryptBuffer+=sizeof(int);

							//��ü������ ���� ��ΰ�?
							//fread( &pcsPackingFolder->m_lBlockSize, 1, sizeof(int), file );
							memcpy(&pcsPackingFolder->m_lBlockSize, pstrEncryptBuffer, sizeof(int) );
							pstrEncryptBuffer+=sizeof(int);

							if( pcsPackingFolder->m_lBlockSize )
							{
								//���� ũ�⸸ŭ �����Ѵ�.
								pcsPackingFolder->m_pstrBlock = new char[pcsPackingFolder->m_lBlockSize];
								memset( pcsPackingFolder->m_pstrBlock, 0, pcsPackingFolder->m_lBlockSize );
								//fread( pcsPackingFolder->m_pstrBlock, 1, pcsPackingFolder->m_lBlockSize, file );
								memcpy( pcsPackingFolder->m_pstrBlock, pstrEncryptBuffer, pcsPackingFolder->m_lBlockSize );
								pstrEncryptBuffer+=pcsPackingFolder->m_lBlockSize;
							}

							//����� ��ΰ�?
							//fread( &pcsPackingFolder->m_lEmptyBlockCount, 1, sizeof(int), file );
							memcpy( &pcsPackingFolder->m_lEmptyBlockCount, pstrEncryptBuffer, sizeof(int) );
							pstrEncryptBuffer+=sizeof(int);

							//������ �߰��Ѵ�.
							m_csFolderInfo.AddObject( (void **)&pcsPackingFolder, _strlwr(pcsPackingFolder->m_pstrFolderName) );

							//���ϰ��� �б�
							for( int lCounter=0; lCounter<lFileCount; lCounter++ )
							{
								pcsPackingFile = new CPackingFile;

								//�����̸�ũ��
								//fread( &pcsPackingFile->m_lFileNameSize, 1, sizeof(int), file );
								memcpy( &pcsPackingFile->m_lFileNameSize, pstrEncryptBuffer, sizeof(int) );
								pstrEncryptBuffer+=sizeof(int);

								pcsPackingFile->m_pstrFileName = new char[pcsPackingFile->m_lFileNameSize];
								memset( pcsPackingFile->m_pstrFileName, 0, pcsPackingFile->m_lFileNameSize );

								//�����̸�
								//fread( pcsPackingFile->m_pstrFileName, 1, pcsPackingFile->m_lFileNameSize, file );
								memcpy( pcsPackingFile->m_pstrFileName, pstrEncryptBuffer, pcsPackingFile->m_lFileNameSize );
								pstrEncryptBuffer+=pcsPackingFile->m_lFileNameSize;

								//������ ��ġ��������
								//fread( &pcsPackingFile->m_lChildDataNodeCount, 1, sizeof(int), file );
								memcpy( &pcsPackingFile->m_lChildDataNodeCount, pstrEncryptBuffer, sizeof(int) );
								pstrEncryptBuffer+=sizeof(int);

								pcsPrevPackingDataNode = pcsPackingFile->m_pcsDataNode;

								for( int lChildNodeCounter=0; lChildNodeCounter<pcsPackingFile->m_lChildDataNodeCount; lChildNodeCounter++ )
								{
									//������ġ ���� ����
									CPackingDataNode		*pcsTempPackingDataNode;

									pcsTempPackingDataNode =  new CPackingDataNode;

									//fread( &pcsTempPackingDataNode->m_lStartPos, 1, sizeof(int), file );
									memcpy( &pcsTempPackingDataNode->m_lStartPos, pstrEncryptBuffer, sizeof(int) );
									pstrEncryptBuffer+=sizeof(int);
									//fread( &pcsTempPackingDataNode->m_lSize, 1, sizeof(int), file );
									memcpy( &pcsTempPackingDataNode->m_lSize, pstrEncryptBuffer, sizeof(int) );
									pstrEncryptBuffer+=sizeof(int);

									if( pcsPrevPackingDataNode == NULL )
									{
										pcsPackingFile->m_pcsDataNode = pcsTempPackingDataNode;
										pcsPrevPackingDataNode = pcsTempPackingDataNode;
									}
									else
									{
										pcsPrevPackingDataNode->m_pcsNextNode = pcsTempPackingDataNode;
										pcsPrevPackingDataNode = pcsTempPackingDataNode;
									}
								}

								//������ �߰��Ѵ�.
								pcsPackingFolder->m_csFilesInfo.AddObject( (void **)&pcsPackingFile, _strlwr(pcsPackingFile->m_pstrFileName) );
							}
						}

						bResult = true;
					}

					delete [] pstrDeletePointer;
				}
			}
		}
	}

	FindClose( hHandle );

	return bResult;
}
*/

//@{ 2006/04/16 burumal
//bool AuPackingManager::LoadReferenceFile( LPCTSTR pstrCurrentPath, bool bReadOnly, bool bRecursiveSubDir )
bool AuPackingManager::LoadReferenceFile( LPCTSTR pstrCurrentPath, bool bReadOnly, bool bRecursiveSubDir, bool bResizing )
//@}
{
	HANDLE				hHandle;
	WIN32_FIND_DATA		csFindData;
	char				strSearchFolder[255];
	char				strCurrentFolder[255];

	bool				bResult;

	bResult = false;

	if( pstrCurrentPath == NULL )
	{
		GetCurrentDirectory( sizeof(strCurrentFolder), strCurrentFolder );
        sprintf( strSearchFolder, "%s\\*.*", strCurrentFolder );
	}
	else
	{
		sprintf( strCurrentFolder, "%s", pstrCurrentPath );
        sprintf( strSearchFolder, "%s\\*.*", pstrCurrentPath );
	}

	//Reference.Dat������ �ִٸ� �д´�.
	{
		char				strFullPath[255];
		sprintf( strFullPath, "%s\\%s", strCurrentFolder, Packing_Reference_FileName );
		
		//@{ 2006/04/16 burumal
		//if( ReadReferenceFile( strFullPath ) )
		if( ReadReferenceFile( strFullPath, bResizing ) )
		//@}
		{
			bResult = true;
		}
	}

	if( bRecursiveSubDir == true )
	{
		hHandle = FindFirstFile( strSearchFolder, &csFindData );
		while( 1 )
		{
			if( FindNextFile( hHandle, &csFindData ) == FALSE )
				break;

			//�����ΰ�?
			if( csFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				if( !strcmpi( ".", csFindData.cFileName		) ||
					!strcmpi( "..", csFindData.cFileName	) ||
					!strcmpi( ".svn", csFindData.cFileName	) )
				{
					continue;
				}
				else
				{
					//���� ���丮�� Ÿ�� ���� �˻��Ұ��ΰ�?
					{
						char			strBuffer[255];

						sprintf( strBuffer, "%s\\%s", strCurrentFolder, csFindData.cFileName );

						//@{ 2006/04/16 burumal
						//LoadReferenceFile( strBuffer, bReadOnly, bRecursiveSubDir );
						LoadReferenceFile( strBuffer, bReadOnly, bRecursiveSubDir, bResizing );
						//@}
					}
				}
			}
		}
		FindClose( hHandle );
	}

	return bResult;
}

//@{ 2006/04/16 burumal
//bool AuPackingManager::ReadReferenceFile( char * pstFileName )
bool AuPackingManager::ReadReferenceFile( char * pstFileName, bool bResizing  )
//@}
{
	// Implementation of reading reference file
	// called in LoadReferenceFile function internally

	FILE				*file;
	char				*pstrEncryptBuffer;
	char				*pstrDeletePointer;
	int					lEncryptSize;
	int					lFileCount;

	bool				bResult = false;
	
	file = fopen( pstFileName, "rb" );
	
	if( file )
	{
		//����ũ�⸦ ��´�.
		fseek( file, 0, SEEK_END );
		lEncryptSize = ftell(file);

		//���� ũ�⸦ ������� �ٽ� ������~
		fseek( file, 0, SEEK_SET );

		pstrEncryptBuffer = new char[lEncryptSize];
		pstrDeletePointer = pstrEncryptBuffer; //���������� �����͸� �����صд�.
		fread( pstrEncryptBuffer, 1, lEncryptSize, file );

		fclose( file );
		
		//������ �� �о����� ��ȣȭǮ��~
#ifndef _AREA_CHINA_
		if( m_cMD5Encrypt.DecryptString( "1111", pstrEncryptBuffer, lEncryptSize ) )
#else
		if( m_cMD5Encrypt.DecryptString( MD5_HASH_KEY_STRING, pstrEncryptBuffer, lEncryptSize ) )
#endif
		{			
			CPackingFolder*			pcsPackingFolder;
			CPackingFile*			pcsPackingFile;

			char szTempMsg[256];
			sprintf(szTempMsg, "%s %d\n", pstFileName, lEncryptSize);
			OutputDebugStr(szTempMsg);

			pcsPackingFolder = new CPackingFolder;
			
			//��� ������ �ִ��� �д´�.
			//fread( &lFileCount, 1, sizeof(int), file );
			memcpy( &lFileCount, pstrEncryptBuffer, sizeof(int) );
			pstrEncryptBuffer+=sizeof(int);

			//@{ 2006/08/03 burumal
			if ( lFileCount < 0 )
			{
				delete pcsPackingFolder;
				delete [] pstrDeletePointer;
				return false;
			}

			// ����ε� Reference.Dat �������� �ּ����� ������� ���� ũ�⸦ �˻��غ���
			// ����Ȳ���� ������ ������ �̸��� �˼��� �����Ƿ� �ּ� ������ 2�� ����ߴ�
			if ( lEncryptSize < 
				(int) (sizeof(int) + 2 + lFileCount * (sizeof(int) + 2 + sizeof(int) + sizeof(int))) )
			{
				delete pcsPackingFolder;
				delete [] pstrDeletePointer;
				return false;
			}
			//@}

			//@{ 2006/10/19 burumal
			if ( lFileCount == 0 )
			{
				delete pcsPackingFolder;
				delete [] pstrDeletePointer;
				return true;
			}
			//@}

			if( lFileCount != 0 )
			{
				//���� �̸�ũ��
				//fread( &pcsPackingFolder->m_lFolderNameSize, 1, sizeof(int), file );
				memcpy( &pcsPackingFolder->m_lFolderNameSize, pstrEncryptBuffer, sizeof(int) );
				pstrEncryptBuffer+=sizeof(int);

				//���� �̸�
				//fread( pcsPackingFolder->m_pstrFolderName, 1, pcsPackingFolder->m_lFolderNameSize, file );
				pcsPackingFolder->m_pstrFolderName = new char[pcsPackingFolder->m_lFolderNameSize];
				memset( pcsPackingFolder->m_pstrFolderName, 0, pcsPackingFolder->m_lFolderNameSize );
				memcpy( pcsPackingFolder->m_pstrFolderName, pstrEncryptBuffer, pcsPackingFolder->m_lFolderNameSize );
				pstrEncryptBuffer+=pcsPackingFolder->m_lFolderNameSize;

				//@{ 2006/04/16 burumal
				long nCurFolderDataFileSize = 0;
				pcsPackingFolder->m_lDataFileSize = 0;
				//@}
				
				//������ �߰��Ѵ�.
				m_csFolderInfo.AddObject( (void **)&pcsPackingFolder, _strlwr(pcsPackingFolder->m_pstrFolderName) );
				
				//���ϰ��� �б�
				for( int lCounter=0; lCounter<lFileCount; lCounter++ )
				{
					pcsPackingFile = new CPackingFile;

					//�����̸�ũ��
					//fread( &pcsPackingFile->m_lFileNameSize, 1, sizeof(int), file );
					memcpy( &pcsPackingFile->m_lFileNameSize, pstrEncryptBuffer, sizeof(int) );
					pstrEncryptBuffer+=sizeof(int);

					pcsPackingFile->m_pstrFileName = new char[pcsPackingFile->m_lFileNameSize];
					memset( pcsPackingFile->m_pstrFileName, 0, pcsPackingFile->m_lFileNameSize );

					//�����̸�
					//fread( pcsPackingFile->m_pstrFileName, 1, pcsPackingFile->m_lFileNameSize, file );
					memcpy( pcsPackingFile->m_pstrFileName, pstrEncryptBuffer, pcsPackingFile->m_lFileNameSize );
					pstrEncryptBuffer+=pcsPackingFile->m_lFileNameSize;

					//������ġ ���� ����
					CPackingDataNode		*pcsPackingDataNode;

					pcsPackingDataNode =  new CPackingDataNode;

					//fread( &pcsPackingDataNode->m_lStartPos, 1, sizeof(int), file );
					memcpy( &pcsPackingDataNode->m_lStartPos, pstrEncryptBuffer, sizeof(int) );
					pstrEncryptBuffer+=sizeof(int);
					//fread( &pcsPackingDataNode->m_lSize, 1, sizeof(int), file );
					memcpy( &pcsPackingDataNode->m_lSize, pstrEncryptBuffer, sizeof(int) );
					pstrEncryptBuffer+=sizeof(int);

					//@{ 2006/04/16 burumal
					if ( (pcsPackingDataNode->m_lStartPos + pcsPackingDataNode->m_lSize) > nCurFolderDataFileSize )
						nCurFolderDataFileSize = (pcsPackingDataNode->m_lStartPos + pcsPackingDataNode->m_lSize);
					//@}

					pcsPackingFile->m_pcsDataNode = pcsPackingDataNode;

					//������ �߰��Ѵ�.
					pcsPackingFolder->m_csFilesInfo.AddObject( (void **)&pcsPackingFile, _strlwr(pcsPackingFile->m_pstrFileName) );
				}

				//@{ 2006/04/16 burumal
				pcsPackingFolder->m_lDataFileSize = nCurFolderDataFileSize;

				if ( bResizing )
					ResizeDatFile(pcsPackingFolder);
				//@}				
			}

			bResult = true;
		}

		delete [] pstrDeletePointer;
	}

	return bResult;
}


void AuPackingManager::SetFilePointer(bool bLoadSubDir)
{
	//Ref������ �о����� Data.Dat������ ���� �����͸� ��������!!
	CPackingFolder			**ppcsPackingFolder;

	char				strDataFilePath[255];
	int					lIndex;

	lIndex = 0;

	char	strDir[255];
	GetCurrentDirectory( 255, strDir );

	for( ppcsPackingFolder = (CPackingFolder **)m_csFolderInfo.GetObjectSequence( &lIndex ); ppcsPackingFolder; ppcsPackingFolder = (CPackingFolder **)m_csFolderInfo.GetObjectSequence( &lIndex ) )
	{
		if( (*ppcsPackingFolder) )
		{
			if( !stricmp( "root", (*ppcsPackingFolder)->m_pstrFolderName ) )
			{
				sprintf( strDataFilePath, "%s", Packing_Data_FileName );
			}
			else
			{
				if( bLoadSubDir )
				{
                    sprintf( strDataFilePath, "%s\\%s", (*ppcsPackingFolder)->m_pstrFolderName, Packing_Data_FileName );
				}
				else
				{
                    sprintf( strDataFilePath, "%s", Packing_Data_FileName );
				}
			}

			//Read, Write, Append ��� �����ϰ� ����.
			if( m_bReadOnlyMode )
			{
				(*ppcsPackingFolder)->m_fpFile = fopen( strDataFilePath, "rb" );
			}
			else
			{
				(*ppcsPackingFolder)->m_fpFile = fopen( strDataFilePath, "rb+" );
			}
			//������ ������ �ȵǸ� NULL�����̱⿡ �׳� ���� ���̴�. Ư���� ó���� ����. 
		}
	}
}

void AuPackingManager::CloseAllFileHandle()
{
	CPackingFolder			**ppcsPackingFolder;

	int					lIndex;

	lIndex = 0;

	for( ppcsPackingFolder = (CPackingFolder **)m_csFolderInfo.GetObjectSequence( &lIndex ); ppcsPackingFolder; ppcsPackingFolder = (CPackingFolder **)m_csFolderInfo.GetObjectSequence( &lIndex ) )
	{
		if( (*ppcsPackingFolder) )
		{
			if( (*ppcsPackingFolder)->m_fpFile != NULL )
			{
				fclose( (*ppcsPackingFolder)->m_fpFile );
				(*ppcsPackingFolder)->m_fpFile = NULL;
			}			
		}
	}
}

bool AuPackingManager::BuildDatFile( char *pstrPackingDir, char *pstrRawDir, int lVersion, HWND hDisplayhWnd, HWND hProgressWnd )
{
	FILE			*datfile;
	FILE			*readfile;
	CPackingFolder	**ppcsPackingFolder;
	CPackingFile	**ppcsPackingFile;

	bool			bResult;
	char			strCurrentDir[255];
	char			strDataFileName[255];
	char			strSourceFileName[255];
	int				lFolderIndex;
	int				lFileIndex;
	int				lRealFileCount;
	int				lTotalFileCount;
	int				lProgressCurrentPostion;

	bResult = false;

	GetCurrentDirectory( sizeof(strCurrentDir), strCurrentDir ); 

	lFolderIndex = 0;

	if( hProgressWnd )
	{
		lTotalFileCount = 0;
		lProgressCurrentPostion = 0;

		for( ppcsPackingFolder = (CPackingFolder **)m_csFolderInfo.GetObjectSequence( &lFolderIndex ); ppcsPackingFolder; ppcsPackingFolder = (CPackingFolder	**)m_csFolderInfo.GetObjectSequence( &lFolderIndex ) )
		{
			if( ppcsPackingFolder && (*ppcsPackingFolder) )
			{
				//�ٲ��츸 ���� �����Ѵ�.
				if( (*ppcsPackingFolder)->m_bExportToRef == true )
                    lTotalFileCount += (*ppcsPackingFolder)->m_csFilesInfo.GetObjectCount();
			}
		}

		//��������
		SendMessage( hProgressWnd, PBM_SETRANGE, 0, MAKELPARAM( 0, lTotalFileCount) );
		//��ġ�ʱ�ȭ
		SendMessage( hProgressWnd, PBM_SETPOS, 0, 0 );
	}

	lFolderIndex = 0;

	for( ppcsPackingFolder = (CPackingFolder **)m_csFolderInfo.GetObjectSequence( &lFolderIndex ); ppcsPackingFolder; ppcsPackingFolder = (CPackingFolder	**)m_csFolderInfo.GetObjectSequence( &lFolderIndex ) )
	{
		if( (*ppcsPackingFolder) == NULL )
			continue;

		if( (*ppcsPackingFolder)->m_bExportToRef == false )
			continue;

		//���� �̵��� ������ ����.
		if( !strcmpi( "root", (*ppcsPackingFolder)->m_pstrFolderName) )
		{
            sprintf( strDataFileName, "%s\\%s\\%s", strCurrentDir, pstrPackingDir, Packing_Data_FileName );
		}
		else
		{
            sprintf( strDataFileName, "%s\\%s\\%s\\%s", strCurrentDir, pstrPackingDir, (*ppcsPackingFolder)->m_pstrFolderName, Packing_Data_FileName );
		}

		//������ ������ ������ ����� ����.
		lFileIndex = 0;
		lRealFileCount = 0;
		for( ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObjectSequence( &lFileIndex ); ppcsPackingFile; ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObjectSequence( &lFileIndex ) )
		{
			if( (*ppcsPackingFile)->m_lOperation == Packing_Operation_Remove )
				continue;

			if( (*ppcsPackingFile)->m_bPacking == true )
				lRealFileCount++;
		}

		if( lRealFileCount == 0 )
			continue;

		datfile = fopen( strDataFileName, "wb" );

		if( datfile )
		{			
			char			strBuffer[Packing_lBlockSize];
			char			strDisplayBuffer[256];
			size_t			lReadBytes;

			lFileIndex = 0;

			for( ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObjectSequence( &lFileIndex ); ppcsPackingFile; ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObjectSequence( &lFileIndex ) )
			{
				if( (*ppcsPackingFile)->m_lOperation == Packing_Operation_Remove )
					continue;

				if( (*ppcsPackingFile)->m_bPacking == false )
					continue;

				if( !strcmpi( "root", (*ppcsPackingFolder)->m_pstrFolderName) )
				{
					sprintf( strSourceFileName, "%s\\%s", pstrRawDir, (*ppcsPackingFile)->m_pstrFileName );
				}
				else
				{
					sprintf( strSourceFileName, "%s\\%s\\%s", pstrRawDir, (*ppcsPackingFolder)->m_pstrFolderName, (*ppcsPackingFile)->m_pstrFileName );
				}

				if( hDisplayhWnd != NULL )
				{
					//���÷���.
					sprintf( strDisplayBuffer, "[Build Dat] %s ó����...", strSourceFileName );
					SetWindowText( hDisplayhWnd, strDisplayBuffer );

					lProgressCurrentPostion++;
					SendMessage( hProgressWnd, PBM_SETPOS, lProgressCurrentPostion, 0 );
				}

				readfile = fopen( strSourceFileName, "rb" );

				if( readfile )
				{
					while( 1 )
					{
						//���� �ʱ�ȭ
						memset( strBuffer, 0, Packing_lBlockSize );

						//������ �����Ѵ�.
						lReadBytes = fread( strBuffer, 1, Packing_lBlockSize, readfile );

						if( 0 < lReadBytes )
						{
							//4k�� �о Data.Dat�� �����Ѵ�. 
							fwrite( strBuffer, 1, Packing_lBlockSize, datfile );
						}

						//4k���ϸ� �о��ٸ� ������.
						if( lReadBytes < Packing_lBlockSize )
						{
							fclose( readfile );
							break;
						}
					}
				}
			}

			fclose( datfile ); 
		}
	}

	return bResult;
}

/*
bool AuPackingManager::UpdateDatAndRefFileFromJZP( char *pstrTempFolderName, char *pstrJzpFileName )
{
	FILE			*file;
	bool			bResult;
	bool			bPacking;
	char			strCurrentDir[255];
	char			strTempDir[255];
	char			strJZPFileName[255];
	int				lVersion;
	int				lMaxFolderCount;
	int				lFolderCount;
	int				lMaxFileCount;
	int				lTotalFileCount;
	int				lFileCount;

	int				lFolderNameLen;
	char			*pstrFolderName;

	bResult = false;

	GetCurrentDirectory( sizeof(strCurrentDir), strCurrentDir );
    sprintf( strTempDir, "%s\\%s", strCurrentDir, pstrTempFolderName );
	sprintf( strJZPFileName, "%s\\%s\\%s", strCurrentDir, pstrTempFolderName, pstrJzpFileName );

	if( pstrJzpFileName )
	{
		file = fopen( strJZPFileName, "rb" );

		if( file )
		{
			//����
			fread( &lVersion, 1, sizeof(int), file );

			fread( &lTotalFileCount, 1, sizeof(int), file );

			//���� ����
			fread( &lMaxFolderCount, 1, sizeof(int), file );

			for( lFolderCount=0; lFolderCount<lMaxFolderCount; lFolderCount++ )
			{
				//�����̸� ���̷ε�
				fread( &lFolderNameLen, 1, sizeof(int), file );

				//�����̸� �ε�
				pstrFolderName = new char[lFolderNameLen];
				memset( pstrFolderName, 0, lFolderNameLen );
				fread( pstrFolderName, 1, lFolderNameLen, file );

				//�������̶�� �����Ѵ�.
				MarkPacking( pstrFolderName );

				//���ϰ������
				fread( &lMaxFileCount, 1, sizeof(int), file );

				for( lFileCount=0; lFileCount<lMaxFileCount ;lFileCount++ )
				{
					int				lOperation;
					int				lFileNameLength;
					int				lFileSize;
					char			*pstrFileName;
					char			*pstrBuffer;

					pstrFileName = NULL;
					pstrBuffer = NULL;

					//Packing
					fread( &bPacking, 1, sizeof(bool), file );

					//Operation
					fread( &lOperation, 1, sizeof(int), file );

					//FileNameLength
					fread( &lFileNameLength, 1, sizeof(int), file );

					//FileName
					pstrFileName = new char[lFileNameLength];
					memset( pstrFileName, 0, lFileNameLength );
					fread(pstrFileName, 1, lFileNameLength, file );

					if( lOperation == Packing_Operation_Add )
					{
						//FileSize
						fread( &lFileSize, 1, sizeof(int), file );

						pstrBuffer = new char[lFileSize];

						//FileData
						fread( pstrBuffer, 1, lFileSize, file );

						//Packing�̸� Data.Dat�� �߰��Ѵ�.
						if( bPacking == true )
						{
							//�켱 �о����� Ǯ���.
							m_cCompress.decompressMemory( pstrBuffer, lFileSize, strTempDir, pstrFileName );

							//������ Ǯ������~ Data.Dat���Ͽ� �߰�����~
							AddPackingFile( pstrTempFolderName, pstrFolderName, pstrFileName );
						}
						//Packing�� �ƴϸ� �ش� ������ �׳� ������ �����Ѵ�.
						else
						{
							if( !strcmpi( "root", pstrFolderName ) )
							{
								m_cCompress.decompressMemory( pstrBuffer, lFileSize, "", pstrFileName );
							}
							else
							{
								m_cCompress.decompressMemory( pstrBuffer, lFileSize, pstrFolderName, pstrFileName );
							}
						}

						//�߰��� �������� ������ �����. Temp������ �ڵ����� �������� �������� �Ƿ���? ��.
					}
					else
					{
						//Data.Dat ���Ͽ��� ������ �����.
						RemovePackingFile( pstrFolderName, pstrFileName );
					}

					if( pstrFileName != NULL )
						delete [] pstrFileName;
					if( pstrBuffer != NULL )
						delete [] pstrBuffer;
				}

				//�ش������� Refrence ������ �ٽ� �����ϵ��� �����Ѵ�.
				CPackingFolder			*pcsTempPackingFolder;
				pcsTempPackingFolder = GetFolder( pstrFolderName );

				if( pcsTempPackingFolder )
				{
					pcsTempPackingFolder->m_bExportToRef = true;
				}
			}
		}
	}

	return bResult;
}
*/

bool AuPackingManager::GetFolderName( const char *pstrFullPath, const char *pstrSourceFolder, char *pstrBuffer, int lBufferSize )
{
	bool			bResult;

	bResult = false;

	if( pstrFullPath && pstrSourceFolder && pstrBuffer )
	{
		size_t		lRootSize;
		size_t		lFullPathSize;

		memset( pstrBuffer, 0, lBufferSize );
		lRootSize = strlen( pstrSourceFolder ) + 1; // "\\", �� ���������� �����.
		lFullPathSize = strlen( pstrFullPath );
		
		if( lRootSize >= lFullPathSize )
		{
			; //�׳� �Ѿ��.
		}
		else
		{
			memcpy( pstrBuffer, &pstrFullPath[lRootSize], lFullPathSize - lRootSize );
		}

		bResult = true;
	}

	return bResult;
}

bool AuPackingManager::CreateFolder( char *pstrFolderName )
{
	bool			bResult;
	char			strFolderName[255];

	bResult = false;

	if( pstrFolderName )
	{
		int			lFolderNameLen;

		lFolderNameLen = 0;

		memset( strFolderName, 0, 255 );

		for( int lCounter=0; lCounter<255; lCounter++ )
		{
			if( pstrFolderName[lCounter] == 0 )
				break;

			if( pstrFolderName[lCounter] == '\\' )
			{
				CreateDirectory( strFolderName, NULL );
			}

			memcpy( &strFolderName[lFolderNameLen], &pstrFolderName[lCounter], 1 );
			lFolderNameLen++;
		}

		if( lFolderNameLen != 0 )
		{
			CreateDirectory( strFolderName, NULL );
		}
	}

	return bResult;
}

CJZPFileInfo *AuPackingManager::GetJZPInfo( int iVersion )
{
	CJZPFileInfo		**ppcsJZPFileInfo;

	ppcsJZPFileInfo = (CJZPFileInfo **)m_csJZPFileInfo.GetObject( iVersion );

	if( ppcsJZPFileInfo && (*ppcsJZPFileInfo) )
	{
		return (*ppcsJZPFileInfo);
	}

	return NULL;
}

int AuPackingManager::GetFileSize( CPackingFile *pcsPackingFile )
{
	int				lFileSize;

	lFileSize = 0;

	CPackingDataNode		*pcsPackingDataNode;

	if( pcsPackingFile )
	{
		pcsPackingDataNode = pcsPackingFile->m_pcsDataNode;

		if( pcsPackingDataNode )
		{
			lFileSize = pcsPackingDataNode->m_lSize;
		}
	}

	return lFileSize;
}

int AuPackingManager::GetFileSize( ApdFile *pcsApdFile )
{
	CPackingFile			*pcsPackingFile;
	int						lFileSize;

	lFileSize = 0;

	if( pcsApdFile )
	{
		//��ŷ���� ������ �ִٸ� ��ŷ���� ã�ƺ���.
		if( pcsApdFile->m_pcsPackingFolder && pcsApdFile->m_pcsPackingFile )
		{
			pcsPackingFile = GetFile( pcsApdFile->m_strFolderName, pcsApdFile->m_strFileName );

			if( pcsPackingFile )
			{
                lFileSize = GetFileSize( pcsPackingFile );
			}
		}
		//�ƴ϶�� ���� �о��.
		else
		{
			if( pcsApdFile->m_bLoadFromPackingFile == false )
			{
				FILE			*file;
				char			strFileName[255];

				sprintf( strFileName, "%s\\%s", pcsApdFile->m_strFolderName, pcsApdFile->m_strFileName );

				file = fopen( strFileName, "rb" );

				if( file )
				{
					fseek(file, 0, SEEK_END );
					lFileSize = ftell( file );
					fclose(file);
				}
			}
		}
	}

	return lFileSize;
}

bool AuPackingManager::OpenFile( char *pstrFilePath, ApdFile *pcsApdFile )
{
    if( pstrFilePath && pcsApdFile )
	{
		char			strFolderName[255];
		char			strFileName[255];
		char			strFileExt[255];
		char			strFileFullName[255];
		size_t			lFolderNameLen;

		_splitpath( pstrFilePath, NULL, strFolderName, strFileName, strFileExt );
		sprintf( strFileFullName, "%s%s", strFileName, strFileExt );
		
		//���� ���ӵڿ� ���� "\"�� �����.
		lFolderNameLen = strlen(strFolderName);

		if( lFolderNameLen == 0 )
		{
			sprintf( strFolderName, "%s", "root" );
		}
		else
		{
			//@{ 2006/05/05 burumal
			//memset( &strFolderName[lFolderNameLen-1], 0, 1 );
			strFolderName[lFolderNameLen-1] = NULL;
			//@}
		}
		
		//@{ 2006/05/05 burumal
		//return OpenFile( strFolderName, strFileFullName, pcsApdFile );
		return OpenFile( strFolderName, strFileFullName, pcsApdFile, pstrFilePath );
		//@}
	}
	else
	{
		return 0;
	}
}

//@{ 2006/05/05 burumal
//bool AuPackingManager::OpenFile( char *pstrFolderName, char *pstrFileName, ApdFile *pcsApdFile )
bool AuPackingManager::OpenFile( char *pstrFolderName, char *pstrFileName, ApdFile *pcsApdFile, char* pstrFullFilePath )
//@}
{
	CPackingFolder		*pcsPackingFolder;
	CPackingFile		*pcsPackingFile;

	bool				bResult;

	bResult = false;

	if( pstrFolderName && pstrFileName && pcsApdFile )
	{
		memset( pcsApdFile->m_strFolderName, 0, sizeof( CHAR ) * 255 );
		memset( pcsApdFile->m_strFileName, 0, sizeof( CHAR ) * 255 );

		strncpy( pcsApdFile->m_strFolderName	, pstrFolderName	, 255	);
		strncpy( pcsApdFile->m_strFileName		, pstrFileName		, 255	);

		//�о���� ����, �����̸��� �ҹ��ڷ� �ٲ۴�.
		_strlwr(pcsApdFile->m_strFolderName);
		_strlwr(pcsApdFile->m_strFileName);

		//@{ 2006/05/05 burumal
		//pcsPackingFolder = GetFolder( pcsApdFile->m_strFolderName );
		pcsPackingFolder = GetFolder( pcsApdFile->m_strFolderName, true );
		//@}

		//�켱 ��ŷ���� �о��.
		if( pcsPackingFolder )
		{
			//@{ 2006/05/05 burumal
			//pcsPackingFile = GetFile( pcsApdFile->m_strFolderName, pcsApdFile->m_strFileName );
			pcsPackingFile = GetFile( pcsApdFile->m_strFolderName, pcsApdFile->m_strFileName, true );
			//@}

			if( pcsPackingFile )
			{
				if( pcsPackingFile->m_pcsDataNode )
				{
					int				lStartPoint;

					//�� �۾� ���ϴ�~ ���ϴ�~ ��~ �������ô�~
					Lock( pcsApdFile );

					lStartPoint = pcsPackingFile->m_pcsDataNode->m_lStartPos;
					pcsApdFile->m_lFilePosition = lStartPoint;

					pcsApdFile->m_pcsPackingFolder = pcsPackingFolder;
					pcsApdFile->m_pcsPackingFile = pcsPackingFile;

					//���� ��ġ�� �̵����ѳ��´�.
					if( pcsPackingFolder->m_fpFile )
					{
						fseek( pcsPackingFolder->m_fpFile, lStartPoint, SEEK_SET );

						bResult = true;
					}
				}
			}
		}
		//��ŷ������ �ƴ϶�� ���� ã�ƺ���.
		else
		{
			//@{ 2006/05/05 burumal
			/*
			FILE			*file;
			char			strFilePath[255];

			sprintf( strFilePath, "%s\\%s", pstrFolderName, pstrFileName );

			file = fopen( strFilePath, "rb" );

			if( file )
			{
				pcsApdFile->m_phDirectAccess = file;
				pcsApdFile->m_bLoadFromPackingFile = false;
				bResult = true;
			}
			*/

			FILE			*file;

			file = fopen( pstrFullFilePath, "rb" );

			if( file )
			{
				pcsApdFile->m_phDirectAccess = file;
				pcsApdFile->m_bLoadFromPackingFile = false;
				bResult = true;
			}
			//@}
		}
	}	

	return bResult;
}

int AuPackingManager::ReadFile( void *pstrBuffer, int iBufferLen, ApdFile *pcsApdFile )
{
	size_t			lReadBytes;

	lReadBytes = 0;

	if( pstrBuffer && iBufferLen > 0 && pcsApdFile )
	{
		CPackingFolder		*pcsPackingFolder;

		pcsPackingFolder = GetFolder( pcsApdFile->m_strFolderName );

		//�켱 ��ŷ���� �о��.
		if( pcsPackingFolder )
		{
			int				lStartIndex;
			int				lCompiledPos;
			size_t			lCurrentPosition;
			size_t			lRealReadBytes;

			lStartIndex = 0;
			lCompiledPos = 0;
			lCurrentPosition = pcsApdFile->m_lCurrentPosition;			
			
			lRealReadBytes = fread( &(((char *)pstrBuffer)[lReadBytes]), 1, iBufferLen, pcsPackingFolder->m_fpFile );

			lReadBytes += lRealReadBytes;
			pcsApdFile->m_lCurrentPosition += lRealReadBytes;			
		}
		//��ŷ������ �ƴ϶�� ���� ã�ƺ���.
		else
		{
			if( pcsApdFile->m_bLoadFromPackingFile == false )
			{
				lReadBytes = fread( pstrBuffer, 1, iBufferLen, pcsApdFile->m_phDirectAccess );
			}
		}
	}

	return (int)lReadBytes;
}

bool AuPackingManager::SetPos( int lPos, ApdFile *pcsApdFile )
{
	bool			bResult;

	bResult = false;

	if( lPos >= 0 && pcsApdFile )
	{
		//�켱 ��ŷ ������ �ִ°�?
		if( pcsApdFile->m_pcsPackingFolder && pcsApdFile->m_pcsPackingFile )
		{
			if( pcsApdFile->m_pcsPackingFile->m_pcsDataNode )
			{
				CPackingDataNode	*pcsPackingDataNode;

				int					lCompiledCount;

				lCompiledCount = 0;
				pcsPackingDataNode = pcsApdFile->m_pcsPackingFile->m_pcsDataNode;

				//fseek�� �����ϸ� 0�� �����Ѵ�.
				if( fseek(	pcsApdFile->m_pcsPackingFolder->m_fpFile,
							pcsApdFile->m_pcsPackingFile->m_pcsDataNode->m_lStartPos + lPos,
							SEEK_SET )  == 0 )
				{
					pcsApdFile->m_lCurrentPosition = lPos;
					bResult = true;
				}
			}
		}
		//���ٸ� �׳� �о��.
		else
		{
			if( pcsApdFile->m_bLoadFromPackingFile == false )
			{
				if( fseek( pcsApdFile->m_phDirectAccess, lPos, SEEK_SET ) == 0 )
				{
					bResult = true;                
				}
			}
		}
	}

	return bResult;
}

bool AuPackingManager::CloseFile( ApdFile *pcsApdFile )
{
	bool			bResult;

	bResult = false;

	if( pcsApdFile )
	{
		//�켱 ��ŷ ������ �ִ°�?
		if( pcsApdFile->m_pcsPackingFolder && pcsApdFile->m_pcsPackingFile )
		{
			bResult = Unlock( pcsApdFile );
		}
		else
		{
			if( pcsApdFile->m_bLoadFromPackingFile == false )
			{
				if( fclose( pcsApdFile->m_phDirectAccess ) == 0 )
				{
					bResult = true;
				}
			}
		}
	}

	return bResult;
}

bool AuPackingManager::Lock( ApdFile *pcsApdFile )
{
	bool			bResult;

	bResult = false;

	if( pcsApdFile )
	{
		if( pcsApdFile->m_pcsPackingFolder )
		{
			pcsApdFile->m_pcsPackingFolder->m_Mutex.WLock();
		}
	}

	return bResult;
}

bool AuPackingManager::Unlock( ApdFile *pcsApdFile )
{
	bool			bResult;

	bResult = false;

	if( pcsApdFile )
	{
		if( pcsApdFile->m_pcsPackingFolder )
		{
			pcsApdFile->m_pcsPackingFolder->m_Mutex.Release();
		}
	}

	return bResult;
}

//. 2006. 4. 25. nonstopdj
void AuPackingManager::CreateEmptyFile( const char* pstrFullPath )
{
	FILE*			fpTemp = NULL;
	HANDLE			handle;

	//. check file exist.
	fpTemp = fopen(pstrFullPath, "wb");
	if(!fpTemp)
	{
		//. if original file is not exist then create file.
		handle = CreateFile(pstrFullPath, FILE_ALL_ACCESS, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		CloseHandle(handle);
	}
	else
		fclose(fpTemp);
}


bool AuPackingManager::ReadyPacking( char *pstrFolder )
{
	bool			bResult;

	bResult = false;

	char			strDataFilePath[255];
	char			strBackupDataFilePath[255];
	char			strRefrenceFilePath[255];
	char			strBackupRefrenceFilePath[255];

	if( !stricmp( pstrFolder, "root" ) )
	{
		sprintf( strDataFilePath, "%s", Packing_Data_FileName );		
		sprintf( strBackupDataFilePath, "%s", Packing_Data_Backup_FileName );

		sprintf( strRefrenceFilePath, "%s", Packing_Reference_FileName );		
		sprintf( strBackupRefrenceFilePath, "%s", Packing_Reference_Backup_FileName );
	}
	else
	{
		sprintf( strDataFilePath, "%s\\%s", pstrFolder, Packing_Data_FileName );		
		sprintf( strBackupDataFilePath, "%s\\%s", pstrFolder, Packing_Data_Backup_FileName );

		sprintf( strRefrenceFilePath, "%s\\%s", pstrFolder, Packing_Reference_FileName );		
		sprintf( strBackupRefrenceFilePath, "%s\\%s", pstrFolder, Packing_Reference_Backup_FileName );

		//������ ������ش�.
		CreateFolder( pstrFolder );
	}

	//Data, Ref ������ ����Ѵ�.
	int nTry;
	for ( nTry = 0; nTry < 3; nTry++ )
		if ( MoveFile( strDataFilePath, strBackupDataFilePath ) != FALSE )
			break;

	for ( nTry = 0; nTry < 3; nTry++ )
		if ( MoveFile( strRefrenceFilePath, strBackupRefrenceFilePath ) != FALSE )
			break;

	//Ref������ �����Ѵ�. ������ 0����.
	FILE				*file;

	file = fopen( strRefrenceFilePath, "wb" );

	if( file )
	{
		int				lFileCount;
		size_t			lFolderNameLen;

		lFileCount = 0;

		fwrite( &lFileCount, 1, sizeof(int), file );

		lFolderNameLen = strlen( pstrFolder ) + 1;

		if( lFolderNameLen == 1 )
		{
			char			strBuffer[5] = "root";

			lFolderNameLen = 5;

			fwrite( &lFolderNameLen, 1, sizeof(int), file );
			fwrite( strBuffer, 1, lFolderNameLen, file );
		}
		else
		{
			fwrite( &lFolderNameLen, 1, sizeof(int), file );
			fwrite( pstrFolder, 1, lFolderNameLen, file );
		}

		fclose( file );
		bResult = true;
	}

	return bResult;
}

/*
bool AuPackingManager::CompletePacking( char *pstrFolder, int lCurrentPosition, int lPackedFileCount, ApAdmin *pcsAdminAdd, ApAdmin *pcsAdminRemove )
{
	FILE			*fpDat, *fpRef;
	FILE			*fpBackupDat, *fpBackupRef;

	bool			bResult;
	bool			bEmptyDat, bEmptyRef;
	char			strDataFilePath[255];
	char			strBackupDataFilePath[255];
	char			strReferenceFilePath[255];
	char			strBackupReferenceFilePath[255];

	bEmptyDat = false;
	bEmptyRef = false;

	if(  !stricmp( pstrFolder, "root" ) )
	{
		sprintf( strDataFilePath, "%s", Packing_Data_FileName );
		sprintf( strBackupDataFilePath, "%s", Packing_Data_Backup_FileName );

		sprintf( strReferenceFilePath, "%s", Packing_Reference_FileName );
		sprintf( strBackupReferenceFilePath, "%s", Packing_Reference_Backup_FileName );
	}
	else
	{
		sprintf( strDataFilePath, "%s\\%s", pstrFolder, Packing_Data_FileName );
		sprintf( strBackupDataFilePath, "%s\\%s", pstrFolder, Packing_Data_Backup_FileName );

		sprintf( strReferenceFilePath, "%s\\%s", pstrFolder, Packing_Reference_FileName );
		sprintf( strBackupReferenceFilePath, "%s\\%s", pstrFolder, Packing_Reference_Backup_FileName );
	}

	bResult = false;

	fpDat = fopen( strDataFilePath, "ab" );
	fpBackupDat = fopen( strBackupDataFilePath, "rb" );

	fpRef = fopen( strReferenceFilePath, "ab" );
	fpBackupRef = fopen( strBackupReferenceFilePath, "rb" );

	if( (fpDat!=NULL) && (fpRef!=NULL) )
	{
		if( (fpBackupDat!=NULL) && (fpBackupRef!=NULL) )
		{
			char				*pstrEncryptBuffer;
			char				*pstrDeletePointer;
			int					lEncryptSize;

			//����ũ�⸦ ��´�.
			fseek( fpBackupRef, 0, SEEK_END );
			lEncryptSize = ftell(fpBackupRef);

			//���� ũ�⸦ ������� �ٽ� ������~
			fseek( fpBackupRef, 0, SEEK_SET );

			pstrEncryptBuffer = new char[lEncryptSize];
			pstrDeletePointer = pstrEncryptBuffer; //���������� �����͸� �����صд�.

			fread( pstrEncryptBuffer, 1, lEncryptSize, fpBackupRef );
			fclose( fpBackupRef );

			if( m_cMD5Encrypt.DecryptString( MD5_HASH_KEY_STRING, pstrEncryptBuffer, lEncryptSize ) )
			{
				//����� RefrenceFile�� ������ �����͸� �߰��Ѵ�.
				int					lFolderNameSize;
				int					lRefFileCount;
				char				*pstrFolderName;

				//������ ���?
				memcpy( &lRefFileCount, pstrEncryptBuffer, sizeof(int) );
				pstrEncryptBuffer+=sizeof(int);
				//fread( &lRefFileCount, 1, sizeof(int), fpBackupRef );

				//���� �̸�ũ��
				memcpy( &lFolderNameSize, pstrEncryptBuffer, sizeof(int) );
				pstrEncryptBuffer+=sizeof(int);
				//fread( &lFolderNameSize, 1, sizeof(int), fpBackupRef );

				//���� �̸�
				pstrFolderName = new char[lFolderNameSize];
				memset( pstrFolderName, 0, lFolderNameSize );
				memcpy( pstrFolderName, pstrEncryptBuffer, lFolderNameSize );
				pstrEncryptBuffer+=lFolderNameSize;
				//fread( pstrFolderName, 1, lFolderNameSize, fpBackupRef );

				for( int lCounter=0; lCounter<lRefFileCount; lCounter++ )
				{
					size_t				lFileNameSize;
					int				lStartPos, lFileSize;
					char			*pstrFileName;

					//�����̸� ũ�� ����.
					memcpy( &lFileNameSize, pstrEncryptBuffer, sizeof(int) );
					pstrEncryptBuffer+=sizeof(int);
					//fread( &lFileNameSize, 1, sizeof(int), fpBackupRef );

					pstrFileName = new char[lFileNameSize];
					memset( pstrFileName, 0, lFileNameSize );

					//�����̸� ����
					memcpy( pstrFileName, pstrEncryptBuffer, lFileNameSize );
					pstrEncryptBuffer+=lFileNameSize;
					//fread( pstrFileName, 1, lFileNameSize, fpBackupRef );

					//������ġ ����
					memcpy( &lStartPos, pstrEncryptBuffer, sizeof(int) );
					pstrEncryptBuffer+=sizeof(int);
					//fread( &lStartPos, 1, sizeof(int), fpBackupRef );

					//����ũ�� ����
					memcpy( &lFileSize, pstrEncryptBuffer, sizeof(int) );
					pstrEncryptBuffer+=sizeof(int);
					//fread( &lFileSize, 1, sizeof(int), fpBackupRef );

					if( pstrFileName )
					{
						bool			bSkip;

						bSkip = false;

						//���� ��ŷ�� �����̶�� ���������� �ʿ�����Ƿ� ��ŵ~
						if( pcsAdminAdd->GetObject( pstrFileName ) )
						{
							bSkip = true;
						}

						//���� ����Ե� �����̸� �ʿ�����Ƿ� ��ŵ~
						if( pcsAdminRemove->GetObject( pstrFileName ) )
						{
							bSkip = true;
						}

						//��ŵ�� ������ �ƴϸ� �̾ ����.
						if( bSkip == false )
						{
							size_t			lRestSize;
							size_t			lReadBytes;
							char			strBuffer[Packing_lBlockSize];

							//Refrence������ �����Ѵ�.
							lFileNameSize = strlen( pstrFileName ) + 1;

							fwrite( &lFileNameSize, 1, sizeof(int), fpRef );
							fwrite( pstrFileName, 1, lFileNameSize, fpRef );
							fwrite( &lCurrentPosition, 1, sizeof(int), fpRef );
							fwrite( &lFileSize, 1, sizeof(int), fpRef );

							//������ ������ ��ġ�� �̵��Ѵ�.
							fseek( fpBackupDat, lStartPos, SEEK_SET );

							lRestSize = lFileSize;

							//Data.Dat�� ������ �����Ѵ�.
							while( 1 )
							{
								//���� �ʱ�ȭ
								memset( strBuffer, 0, Packing_lBlockSize );

								if( lRestSize < Packing_lBlockSize )
								{
									lReadBytes = fread( strBuffer, 1, lRestSize, fpBackupDat );
									fwrite( strBuffer, 1, lReadBytes, fpDat );
									break;
								}
								else
								{
									lReadBytes = fread( strBuffer, 1, Packing_lBlockSize, fpBackupDat );

									if( 0 < lReadBytes )
									{
										//4k�� �о Data.Dat�� �����Ѵ�. 
										fwrite( strBuffer, 1, Packing_lBlockSize, fpDat );
									}

									//4k���ϸ� �о��ٸ� ������.
									if( lReadBytes < Packing_lBlockSize )
									{
										break;
									}

									lRestSize -= lReadBytes;

									if( lRestSize <= 0 )
									{
										break;
									}
								}
							}

							lCurrentPosition += lFileSize;
							lPackedFileCount++;
						}
					}

					delete [] pstrFileName;
				}

				delete [] pstrFolderName;
			}

			delete [] pstrDeletePointer;

			fclose( fpBackupDat );
			fclose( fpBackupRef );
		}

		//����ũ�⸦ ����.
		fseek( fpDat, 0, SEEK_END );
		fseek( fpRef, 0, SEEK_END );

		if( ftell( fpDat ) == 0 )
			bEmptyDat = true;
		if( ftell( fpRef ) == 0  )
			bEmptyRef = true;

		fclose( fpDat );
		fclose( fpRef );

		//��� ������ �����ϰ� Refrence������ ��ȣȭ�Ѵ�.
		DeleteFile( strBackupDataFilePath );
		DeleteFile( strBackupReferenceFilePath );

		if( (bEmptyDat == true) || (bEmptyRef == true) )
		{
			DeleteFile( strDataFilePath );
			DeleteFile( strReferenceFilePath );
		}
		else
		{
			//���������� fpRef�� lPackedFileCount�� �������ش�.
			fpRef = fopen( strReferenceFilePath, "rb+" );
			if( fpRef )
			{
				fwrite( &lPackedFileCount, 1, sizeof(int), fpRef );
				fclose( fpRef );
			}

			//��ȣȭ�Ѵ�.
			fpRef = fopen( strReferenceFilePath, "rb" );
			if( fpRef )
			{
				long				lFileSize;
				char				*pstrEncryptBuffer;

				//���� ũ�⸦ ��´�.
				fseek( fpRef, 0, SEEK_END );
				lFileSize = ftell(fpRef);
				//����ũ�⸦ ������� �ٽ� ������~
				fseek( fpRef, 0, SEEK_SET ); 

				pstrEncryptBuffer = new char[lFileSize];

				fread( pstrEncryptBuffer, 1, lFileSize, fpRef );

				fclose( fpRef );

				//Key�� "archlord"�̴�.
				if( m_cMD5Encrypt.EncryptString( MD5_HASH_KEY_STRING, pstrEncryptBuffer, lFileSize ) )
				{
					fpRef = fopen( strReferenceFilePath, "wb" );

					if( fpRef )
					{
						fwrite( pstrEncryptBuffer, 1, lFileSize, fpRef );

						fclose( fpRef );
					}
				}

				delete [] pstrEncryptBuffer;
			}
		}
	}
	else
	{
		if( fpDat )
		{
			fclose( fpDat );
		}
		if( fpRef )
		{
			fclose( fpRef );
		}
		if( fpBackupDat )
		{
			fclose( fpBackupDat );
		}
		if( fpBackupRef )
		{
			fclose( fpBackupRef );
		}
	}

	return bResult;
}
*/

bool AuPackingManager::CompletePacking( char *pstrFolder, int lCurrentPosition, int lPackedFileCount, ApAdmin *pcsAdminAdd, ApAdmin *pcsAdminRemove )
{
	FILE			*fpDat, *fpRef;
	FILE			*fpBackupDat, *fpBackupRef;

	bool			bResult;
	bool			bEmptyDat, bEmptyRef;
	char			strDataFilePath[255];
	char			strBackupDataFilePath[255];
	char			strReferenceFilePath[255];
	char			strBackupReferenceFilePath[255];

	bEmptyDat = false;
	bEmptyRef = false;

	if(  !stricmp( pstrFolder, "root" ) )
	{
		sprintf( strDataFilePath, "%s", Packing_Data_FileName );
		sprintf( strBackupDataFilePath, "%s", Packing_Data_Backup_FileName );

		sprintf( strReferenceFilePath, "%s", Packing_Reference_FileName );
		sprintf( strBackupReferenceFilePath, "%s", Packing_Reference_Backup_FileName );
	}
	else
	{
		sprintf( strDataFilePath, "%s\\%s", pstrFolder, Packing_Data_FileName );
		sprintf( strBackupDataFilePath, "%s\\%s", pstrFolder, Packing_Data_Backup_FileName );

		sprintf( strReferenceFilePath, "%s\\%s", pstrFolder, Packing_Reference_FileName );
		sprintf( strBackupReferenceFilePath, "%s\\%s", pstrFolder, Packing_Reference_Backup_FileName );
	}

	bResult = false;

	fpDat = fopen( strDataFilePath, "ab" );
	fpBackupDat = fopen( strBackupDataFilePath, "rb" );

	fpRef = fopen( strReferenceFilePath, "ab" );
	fpBackupRef = fopen( strBackupReferenceFilePath, "rb" );
	
	if( (fpDat!=NULL) && (fpRef!=NULL) )
	{
		if( (fpBackupDat!=NULL) && (fpBackupRef!=NULL) )
		{
			char				*pstrEncryptBuffer;			
			char				*pstrDeletePointer;
			int					lEncryptSize;
			
			//����ũ�⸦ ��´�.
			fseek( fpBackupRef, 0, SEEK_END );
			lEncryptSize = ftell(fpBackupRef);

			//���� ũ�⸦ ������� �ٽ� ������~
			fseek( fpBackupRef, 0, SEEK_SET );

			//@{ 2006/04/11 burumal
			//pstrEncryptBuffer = new char[lEncryptSize];
			//pstrDeletePointer = pstrEncryptBuffer; //���������� �����͸� �����صд�.

			bool bUseMemPool = false;
			if ( m_cCompress.GetMemPool() )
			{
				if ( lEncryptSize > m_cCompress.GetMemPoolSize() )					
					m_cCompress.AllocMemPool(lEncryptSize);

				if ( lEncryptSize <= m_cCompress.GetMemPoolSize() )
				{
					pstrEncryptBuffer = m_cCompress.GetMemPool();
					pstrDeletePointer = NULL;
					bUseMemPool = true;
				}
			}			
			
			if ( bUseMemPool == false )
			{
				pstrEncryptBuffer = new char[lEncryptSize];
				pstrDeletePointer = pstrEncryptBuffer; //���������� �����͸� �����صд�.
			}
			//@}

			fread( pstrEncryptBuffer, 1, lEncryptSize, fpBackupRef );
			fclose( fpBackupRef );			
#ifndef _AREA_CHINA_
			if( m_cMD5Encrypt.DecryptString( "1111", pstrEncryptBuffer, lEncryptSize ) )
#else
			if( m_cMD5Encrypt.DecryptString( MD5_HASH_KEY_STRING, pstrEncryptBuffer, lEncryptSize ) )
#endif
			{
				//����� RefrenceFile�� ������ �����͸� �߰��Ѵ�.
				int					lFolderNameSize;
				int					lRefFileCount;
				
				//@{ 2006/04/11 burumal
				//char				*pstrFolderName;
				char				pstrFolderName[512];
				//@}

				//������ ���?
				memcpy( &lRefFileCount, pstrEncryptBuffer, sizeof(int) );
				pstrEncryptBuffer+=sizeof(int);
				//fread( &lRefFileCount, 1, sizeof(int), fpBackupRef );

				//���� �̸�ũ��
				memcpy( &lFolderNameSize, pstrEncryptBuffer, sizeof(int) );
				pstrEncryptBuffer+=sizeof(int);
				//fread( &lFolderNameSize, 1, sizeof(int), fpBackupRef );

				//���� �̸�
				//@{ 2006/04/11 burumal
				//pstrFolderName = new char[lFolderNameSize];
				//@}
				memset( pstrFolderName, 0, lFolderNameSize );
				memcpy( pstrFolderName, pstrEncryptBuffer, lFolderNameSize );
				pstrEncryptBuffer+=lFolderNameSize;
				//fread( pstrFolderName, 1, lFolderNameSize, fpBackupRef );				

				for( int lCounter=0; lCounter < lRefFileCount; lCounter++ )
				{
					int				lFileNameSize;
					int				lStartPos, lFileSize;					
					char			pstrFileName[512];

					//�����̸� ũ�� ����.
					memcpy( &lFileNameSize, pstrEncryptBuffer, sizeof(int) );
					pstrEncryptBuffer+=sizeof(int);
					//fread( &lFileNameSize, 1, sizeof(int), fpBackupRef );

					//@{ 2006/04/11 burumal
					//pstrFileName = new char[lFileNameSize];
					//@}
					memset( pstrFileName, 0, lFileNameSize );

					//�����̸� ����
					memcpy( pstrFileName, pstrEncryptBuffer, lFileNameSize );
					pstrEncryptBuffer+=lFileNameSize;
					//fread( pstrFileName, 1, lFileNameSize, fpBackupRef );

					//������ġ ����
					memcpy( &lStartPos, pstrEncryptBuffer, sizeof(int) );
					pstrEncryptBuffer+=sizeof(int);
					//fread( &lStartPos, 1, sizeof(int), fpBackupRef );

					//����ũ�� ����
					memcpy( &lFileSize, pstrEncryptBuffer, sizeof(int) );
					pstrEncryptBuffer+=sizeof(int);
					//fread( &lFileSize, 1, sizeof(int), fpBackupRef );

					if( pstrFileName )
					{
						bool			bSkip;

						bSkip = false;

						//���� ��ŷ�� �����̶�� ���������� �ʿ�����Ƿ� ��ŵ~
						if( pcsAdminAdd->GetObject( pstrFileName ) )
						{
							bSkip = true;
						}

						//���� ����Ե� �����̸� �ʿ�����Ƿ� ��ŵ~
						if( pcsAdminRemove->GetObject( pstrFileName ) )
						{
							bSkip = true;
						}

						//��ŵ�� ������ �ƴϸ� �̾ ����.
						if( bSkip == false )
						{
							int				lRestSize;
							int				lReadBytes;
							char			strBuffer[Packing_lBlockSize];
							
							//Refrence������ �����Ѵ�.
							lFileNameSize = (int)strlen( pstrFileName ) + 1;

							fwrite( &lFileNameSize, 1, sizeof(int), fpRef );
							fwrite( pstrFileName, 1, lFileNameSize, fpRef );
							fwrite( &lCurrentPosition, 1, sizeof(int), fpRef );
							fwrite( &lFileSize, 1, sizeof(int), fpRef );

							//������ ������ ��ġ�� �̵��Ѵ�.
							fseek( fpBackupDat, lStartPos, SEEK_SET );

							lRestSize = lFileSize;
							
							//Data.Dat�� ������ �����Ѵ�.
							while( 1 )
							{								
								//���� �ʱ�ȭ
								//memset( strBuffer, 0, Packing_lBlockSize );

								if( lRestSize < Packing_lBlockSize )
								{
									lReadBytes = (int)fread( strBuffer, 1, lRestSize, fpBackupDat );
									fwrite( strBuffer, 1, lReadBytes, fpDat );
									break;
								}
								else
								{
									lReadBytes = (int)fread( strBuffer, 1, Packing_lBlockSize, fpBackupDat );

									if( 0 < lReadBytes )
									{
										//@{ 2006/04/10 burumal								

										//4k�� �о Data.Dat�� �����Ѵ�. 										
										//fwrite( strBuffer, 1, Packing_lBlockSize, fpDat );

										//�о� ���� ��ŭ Data.Dat�� �����Ѵ�.
										fwrite( strBuffer, 1, lReadBytes, fpDat );
										//@}
									}

									//4k���ϸ� �о��ٸ� ������.
									if( lReadBytes < Packing_lBlockSize )
									{
										break;
									}

									lRestSize -= lReadBytes;

									if( lRestSize <= 0 )
									{
										break;
									}
								}
							}

							lCurrentPosition += lFileSize;
							lPackedFileCount++;
						}
					}

					//@{ 2006/04/11 burumal
					//delete [] pstrFileName;
					//@}
				}

				//@{ 2006/04/11 burumal
				//delete [] pstrFolderName;
				//@}
			}

			//@{ 2006/04/11 burumal
			//delete [] pstrDeletePointer;
			if ( pstrDeletePointer )
				delete [] pstrDeletePointer;
			//@}

			fclose( fpBackupDat );
			fclose( fpBackupRef );
		}

		//����ũ�⸦ ����.
		fseek( fpDat, 0, SEEK_END );
		fseek( fpRef, 0, SEEK_END );

		if( ftell( fpDat ) == 0 )
			bEmptyDat = true;
		if( ftell( fpRef ) == 0  )
			bEmptyRef = true;

		fclose( fpDat );
		fclose( fpRef );

		//@{ 2006/04/11 burumal
		/*
		//��� ������ �����ϰ� Refrence������ ��ȣȭ�Ѵ�.
		DeleteFile( strBackupDataFilePath );
		DeleteFile( strBackupReferenceFilePath );

		if( (bEmptyDat == true) || (bEmptyRef == true) )
		{
			DeleteFile( strDataFilePath );
			DeleteFile( strReferenceFilePath );
		}
		else
		{
			//���������� fpRef�� lPackedFileCount�� �������ش�.
			fpRef = fopen( strReferenceFilePath, "rb+" );
			if( fpRef )
			{
				fwrite( &lPackedFileCount, 1, sizeof(int), fpRef );
				fclose( fpRef );
			}

			//��ȣȭ�Ѵ�.
			fpRef = fopen( strReferenceFilePath, "rb" );
			if( fpRef )
			{
				long				lFileSize;
				char				*pstrEncryptBuffer;

				//���� ũ�⸦ ��´�.
				fseek( fpRef, 0, SEEK_END );
				lFileSize = ftell(fpRef);
				//����ũ�⸦ ������� �ٽ� ������~
				fseek( fpRef, 0, SEEK_SET ); 

				pstrEncryptBuffer = new char[lFileSize];

				fread( pstrEncryptBuffer, 1, lFileSize, fpRef );

				fclose( fpRef );

				//Key�� "archlord"�̴�.
				if( m_cMD5Encrypt.EncryptString( MD5_HASH_KEY_STRING, pstrEncryptBuffer, lFileSize ) )
				{
					fpRef = fopen( strReferenceFilePath, "wb" );

					if( fpRef )
					{
						fwrite( pstrEncryptBuffer, 1, lFileSize, fpRef );

						fclose( fpRef );
					}
				}

				delete [] pstrEncryptBuffer;
			}
		}
		*/
		// reference ������ ��ȣȭ���� ��� ������ �����Ѵ�.
		//@}

		if( (bEmptyDat == true) || (bEmptyRef == true) )
		{
			DeleteFile( strDataFilePath );
			DeleteFile( strReferenceFilePath );
		}
		else
		{
			//���������� fpRef�� lPackedFileCount�� �������ش�.
			fpRef = fopen( strReferenceFilePath, "rb+" );
			if( fpRef )
			{
				fwrite( &lPackedFileCount, 1, sizeof(int), fpRef );
				fclose( fpRef );
			}

			//��ȣȭ�Ѵ�.
			fpRef = fopen( strReferenceFilePath, "rb" );
			if( fpRef )
			{
				long				lFileSize;
				char				*pstrEncryptBuffer;

				//���� ũ�⸦ ��´�.
				fseek( fpRef, 0, SEEK_END );
				lFileSize = ftell(fpRef);

				//����ũ�⸦ ������� �ٽ� ������~
				fseek( fpRef, 0, SEEK_SET ); 

				pstrEncryptBuffer = new char[lFileSize];

				fread( pstrEncryptBuffer, 1, lFileSize, fpRef );

				fclose( fpRef );

				//Key�� MD5_HASH_KEY_STRING�̴�.
				if( m_cMD5Encrypt.EncryptString( "1111", pstrEncryptBuffer, lFileSize ) )
				{
					fpRef = fopen( strReferenceFilePath, "wb" );

					if( fpRef )
					{
						fwrite( pstrEncryptBuffer, 1, lFileSize, fpRef );

						fclose( fpRef );
					}
				}

				delete [] pstrEncryptBuffer;
			}
		}

		//��� ������ �����Ѵ�.
		DeleteFile( strBackupDataFilePath );
		DeleteFile( strBackupReferenceFilePath );
	}
	else
	{
		if( fpDat )
		{
			fclose( fpDat );
		}
		if( fpRef )
		{
			fclose( fpRef );
		}
		if( fpBackupDat )
		{
			fclose( fpBackupDat );
		}
		if( fpBackupRef )
		{
			fclose( fpBackupRef );
		}
	}

	return bResult;
}

/*
bool AuPackingManager::BuildPackingFile( char *pstrTempFolderName, char *pstrFolderName, char *pstrFileName, int lCurrentPos, int lFileSize )
{
	FILE			*fpData, *fpReference;
	FILE			*fpOriginFile;
	bool			bResult;

	char			strDataFilePath[255];
	char			strReferenceFilePath[255];
	char			strOriginFilePath[255];

	bResult = false;

	if( !stricmp( pstrFolderName, "root" ) )
	{
		sprintf( strDataFilePath, "%s", Packing_Data_FileName );
		sprintf( strReferenceFilePath, "%s", Packing_Reference_FileName );
		sprintf( strOriginFilePath, "%s", pstrFileName );
	}
	else
	{
		sprintf( strDataFilePath, "%s\\%s", pstrFolderName, Packing_Data_FileName );
		sprintf( strReferenceFilePath, "%s\\%s", pstrFolderName, Packing_Reference_FileName );
		sprintf( strOriginFilePath, "%s\\%s", pstrTempFolderName, pstrFileName );

		//@{ 2006/04/07 burumal		
		//CreateFolder( pstrFolderName );
		//@}
	}

	fpData = fopen( strDataFilePath, "ab" );
	fpReference = fopen( strReferenceFilePath, "ab" );
	fpOriginFile = fopen( strOriginFilePath, "rb" );

	if( fpData && fpReference && fpOriginFile )
	{
		size_t			lFileNameSize;
		size_t			lReadBytes;

		//@{ 2006/04/07 burumal
		//char			strBuffer[Packing_lBlockSize];
		static char		strBuffer[Packing_lBlockSize];
		//@}

		//���� �� ���������� �̵��Ѵ�.
		fseek( fpData, 0, SEEK_END );
		fseek( fpReference, 0, SEEK_END );

		//Refrence������ �����Ѵ�.
		lFileNameSize = strlen( pstrFileName ) + 1;

		fwrite( &lFileNameSize, 1, sizeof(int), fpReference );
		fwrite( pstrFileName, 1, lFileNameSize, fpReference );
		fwrite( &lCurrentPos, 1, sizeof(int), fpReference );
        fwrite( &lFileSize, 1, sizeof(int), fpReference );

		//Data.Dat�� ������ �����Ѵ�.
		while( 1 )
		{
			//@{ 2006/04/07 burumal 
			// ���� ���۸� �ʱ�ȭ�� �ʿ�� ����
			//���� �ʱ�ȭ
			//memset( strBuffer, 0, Packing_lBlockSize );
			//@}

			//������ �����Ѵ�.
			lReadBytes = fread( strBuffer, 1, Packing_lBlockSize, fpOriginFile );

			if( 0 < lReadBytes )
			{
				//4k�� �о Data.Dat�� �����Ѵ�. 
				fwrite( strBuffer, 1, lReadBytes, fpData );
			}

			//4k���ϸ� �о��ٸ� ������.
			if( lReadBytes < Packing_lBlockSize )
			{
				break;
			}
		}

		fclose( fpData );
		fclose( fpReference );
		fclose( fpOriginFile );
	}
	//����� �������� ������ Ȥ�� �𸣴�....;;
	else
	{
		if( fpData )
		{
			fclose( fpData );
		}
		if( fpReference )
		{
			fclose( fpReference );
		}
		if( fpOriginFile )
		{
			fclose( fpOriginFile );
		}
	}

	return bResult;
}
*/

bool AuPackingManager::BuildPackingFile( char* pstrTempFolderName, char* pstrFolderName, 
										char* pstrFileName, int lCurrentPos, int lFileSize,
										FILE* fpData, FILE* fpReference, char* pDecompMem)
{	
	//FILE			*fpOriginFile;
	bool			bResult;
	
	char			strOriginFilePath[255];

	bResult = false;

	if ( !fpData || !fpReference || !pDecompMem || (lFileSize <= 0) )
		return bResult;

	if( !stricmp( pstrFolderName, "root" ) )
	{		
		sprintf( strOriginFilePath, "%s", pstrFileName );
	}
	else
	{		
		sprintf( strOriginFilePath, "%s\\%s", pstrTempFolderName, pstrFileName );
	}
	
	//fpOriginFile = fopen( strOriginFilePath, "rb" );

	//if( fpData && fpReference && fpOriginFile )
	if( fpData && fpReference )
	{
		int			lFileNameSize;
		int			lReadBytes;
		
		//char		strBuffer[Packing_lBlockSize];
		
		//Refrence������ �����Ѵ�.
		lFileNameSize = (int)strlen( pstrFileName ) + 1;

		fwrite( &lFileNameSize, 1, sizeof(int), fpReference );
		fwrite( pstrFileName, 1, lFileNameSize, fpReference );
		fwrite( &lCurrentPos, 1, sizeof(int), fpReference );
		fwrite( &lFileSize, 1, sizeof(int), fpReference );

		//Data.Dat�� ������ �����Ѵ�.
		while( 1 )
		{			
			/*
			//������ �����Ѵ�.
			lReadBytes = fread( strBuffer, 1, Packing_lBlockSize, fpOriginFile );		

			if( 0 < lReadBytes )
			{
				//4k�� �о Data.Dat�� �����Ѵ�. 
				fwrite( strBuffer, 1, lReadBytes, fpData );
			}

			//4k���ϸ� �о��ٸ� ������.
			if( lReadBytes < Packing_lBlockSize )
			{
				break;
			}
			*/
		
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
				fwrite(pDecompMem, 1, lReadBytes, fpData);
				pDecompMem += lReadBytes;
			}

			if ( lReadBytes < Packing_lBlockSize )
			{
				break;
			}
		}
		
		//fclose( fpOriginFile );
	}
	//����� �������� ������ Ȥ�� �𸣴�....;;
	else
	{
		//if( fpOriginFile )
		{
			//fclose( fpOriginFile );
		}
	}

	return bResult;
}

//@{ 2006/04/12 burumal
//PackingFileSpaceCompFunc
struct PACKING_FILE_SPACE
{
	int nStart;
	int nEnd;
	int nType; // 0: normal , 1: empty
	CPackingFile* pPackingFile;
};

int PackingFileSpaceCompFunc(const void* pAlpha, const void* pBeta)
{
	return ((*(PACKING_FILE_SPACE**) pAlpha))->nStart - (*((PACKING_FILE_SPACE**) pBeta))->nStart;
}

bool AuPackingManager::ReadBakReference(char* pstrFolder, ApAdmin* pAdminAdd, ApAdmin* pAdminRemove, 
										ApAdmin* pAdminPackingNormal, ApAdmin* pAdminPackingEmpty)
{
	FILE*	fpBackupRef;
	char	strBackupReferenceFilePath[255];

	int		lFolderNameSize;
	int		lRefFileCount;

	char	pstrFolderName[256];
	
	PACKING_FILE_SPACE** ppPackingFileSpaceInfo = NULL;
	
	if( !stricmp( pstrFolder, "root" ) )
	{		
		sprintf( strBackupReferenceFilePath, "%s", Packing_Reference_Backup_FileName );
	}
	else
	{
		sprintf( strBackupReferenceFilePath, "%s\\%s", pstrFolder, Packing_Reference_Backup_FileName );
	}

	fpBackupRef = fopen( strBackupReferenceFilePath, "rb" );
	
	if ( fpBackupRef )
	{
		char*	pstrEncryptBuffer;			
		char*	pstrDeletePointer;
		int		lEncryptSize;

		//����ũ�⸦ ��´�.
		fseek( fpBackupRef, 0, SEEK_END );
		lEncryptSize = ftell(fpBackupRef);

		//���� ũ�⸦ ������� �ٽ� ������~
		fseek( fpBackupRef, 0, SEEK_SET );

		// �޸�Ǯ ó��
		bool bUseMemPool = false;
		if ( m_cCompress.GetMemPool() )
		{
			if ( lEncryptSize > m_cCompress.GetMemPoolSize() )					
				m_cCompress.AllocMemPool(lEncryptSize);

			if ( lEncryptSize <= m_cCompress.GetMemPoolSize() )
			{
				pstrEncryptBuffer = m_cCompress.GetMemPool();
				pstrDeletePointer = NULL;
				bUseMemPool = true;
			}
		}			

		if ( bUseMemPool == false )
		{
			pstrEncryptBuffer = new char[lEncryptSize];
			pstrDeletePointer = pstrEncryptBuffer; //���������� �����͸� �����صд�.
		}

		fread( pstrEncryptBuffer, 1, lEncryptSize, fpBackupRef );
		fclose( fpBackupRef );
#ifndef _AREA_CHINA_
		if( m_cMD5Encrypt.DecryptString( "1111", pstrEncryptBuffer, lEncryptSize ) )
#else
		if( m_cMD5Encrypt.DecryptString( MD5_HASH_KEY_STRING, pstrEncryptBuffer, lEncryptSize ) )
#endif
		{
			//����� RefrenceFile�� ������ �����͸� �߰��Ѵ�.

			//������ ���?
			memcpy( &lRefFileCount, pstrEncryptBuffer, sizeof(int) );
			pstrEncryptBuffer+=sizeof(int);
			
			//���� �̸�ũ��
			memcpy( &lFolderNameSize, pstrEncryptBuffer, sizeof(int) );
			pstrEncryptBuffer+=sizeof(int);
			
			//���� �̸�
			memset( pstrFolderName, 0, lFolderNameSize );
			memcpy( pstrFolderName, pstrEncryptBuffer, lFolderNameSize );
			pstrEncryptBuffer+=lFolderNameSize;

			// ���� �������� �޸� �Ҵ�
			ppPackingFileSpaceInfo = new PACKING_FILE_SPACE*[lRefFileCount];
			ASSERT(ppPackingFileSpaceInfo);
			memset(ppPackingFileSpaceInfo, 0, sizeof(PACKING_FILE_SPACE*) * lRefFileCount);
			
			for( int lCounter = 0; lCounter < lRefFileCount; lCounter++ )
			{				
				int				lFileNameSize;
				int				lStartPos, lFileSize;					
				char			pstrFileName[256];

				//�����̸� ũ�� ����.
				memcpy( &lFileNameSize, pstrEncryptBuffer, sizeof(int) );
				pstrEncryptBuffer+=sizeof(int);
				
				memset( pstrFileName, 0, lFileNameSize );

				//�����̸� ����
				memcpy( pstrFileName, pstrEncryptBuffer, lFileNameSize );
				pstrEncryptBuffer+=lFileNameSize;
				
				//������ġ ����
				memcpy( &lStartPos, pstrEncryptBuffer, sizeof(int) );
				pstrEncryptBuffer+=sizeof(int);
				
				//����ũ�� ����
				memcpy( &lFileSize, pstrEncryptBuffer, sizeof(int) );
				pstrEncryptBuffer+=sizeof(int);

				if ( pstrFileName )
				{
					bool bSkip = false;

					//���� ��ŷ�� �����̶�� ���������� �ʿ�����Ƿ� ��ŵ~
					if( !bSkip && pAdminAdd->GetObject( _strlwr(pstrFileName) ) )
					{
						bSkip = true;
					}

					//���� ����Ե� �����̸� �ʿ�����Ƿ� ��ŵ~
					if( !bSkip && pAdminRemove->GetObject( _strlwr(pstrFileName) ) )
					{
						bSkip = true;
					}
					
					// ����Ÿ �غ�
					CPackingFile* pPackingFile;
					CPackingDataNode* pPackingDataNode;

					pPackingFile = new CPackingFile;
					pPackingDataNode = new CPackingDataNode;

					ASSERT(pPackingFile);
					ASSERT(pPackingDataNode);

					// �������� ����
					pPackingFile->m_lFileNameSize = lFileNameSize;
					pPackingFile->m_pstrFileName = new char[lFileNameSize];
					memset(pPackingFile->m_pstrFileName, 0, lFileNameSize);
					memcpy(pPackingFile->m_pstrFileName, pstrFileName, lFileNameSize);

					// ������ġ ��������
					pPackingDataNode->m_lStartPos = lStartPos;
					pPackingDataNode->m_lSize = lFileSize;

					pPackingFile->m_pcsDataNode = pPackingDataNode;

					if ( bSkip )
					{
						// Empty �迭�� �߰��Ѵ�
						pAdminPackingEmpty->AddObject( (void **)&pPackingFile, _strlwr(pstrFileName) );
					}
					else
					{
						// Normal �迭�� �߰��Ѵ�
						pAdminPackingNormal->AddObject( (void **)&pPackingFile, _strlwr(pstrFileName) );
					}

					//���� �������� ����
					ppPackingFileSpaceInfo[lCounter] = new PACKING_FILE_SPACE;
					ppPackingFileSpaceInfo[lCounter]->nStart = lStartPos;
					ppPackingFileSpaceInfo[lCounter]->nEnd = lStartPos + lFileSize - 1;
					ppPackingFileSpaceInfo[lCounter]->nType = bSkip ? 1 : 0;
					ppPackingFileSpaceInfo[lCounter]->pPackingFile = pPackingFile;
				}
			}
		}

		if ( pstrDeletePointer )
			delete [] pstrDeletePointer;

		fclose(fpBackupRef);
	}
	
	if ( ppPackingFileSpaceInfo )
	{
		// ���� �������� Sorting
		qsort(ppPackingFileSpaceInfo, lRefFileCount, sizeof(PACKING_FILE_SPACE**), ::PackingFileSpaceCompFunc);
		
		
		// ��� ��ƴ�� ã�Ƽ� empty �������� �����
		int nIdx;

		for ( nIdx = 0; nIdx < lRefFileCount - 1; nIdx++ )
		{
			ASSERT(ppPackingFileSpaceInfo[nIdx + 1]->nStart > ppPackingFileSpaceInfo[nIdx]->nEnd);

			if ( (ppPackingFileSpaceInfo[nIdx]->nEnd + 1) != ppPackingFileSpaceInfo[nIdx + 1]->nStart )
			{
				// ���ʰ� ������ ���̿� ��ƴ�� ������

				if ( ppPackingFileSpaceInfo[nIdx]->nType == 1 ) 
				{
					// ������ �̹� empty �����̹Ƿ� ������ Ȯ�����ش�
					ASSERT(ppPackingFileSpaceInfo[nIdx]->pPackingFile);

					ppPackingFileSpaceInfo[nIdx]->pPackingFile->m_pcsDataNode->m_lSize =
						ppPackingFileSpaceInfo[nIdx + 1]->nStart - ppPackingFileSpaceInfo[nIdx]->nStart;
				}
				else
				{
					// ������ normal �̰�

					if ( ppPackingFileSpaceInfo[nIdx + 1]->nType == 1 )
					{
						// �������� empty �̸� ������ empty ������ Ȯ�����ش�

						ppPackingFileSpaceInfo[nIdx + 1]->nStart = ppPackingFileSpaceInfo[nIdx]->nEnd + 1;

						ppPackingFileSpaceInfo[nIdx + 1]->pPackingFile->m_pcsDataNode->m_lStartPos =
							ppPackingFileSpaceInfo[nIdx + 1]->nStart;

						ppPackingFileSpaceInfo[nIdx + 1]->pPackingFile->m_pcsDataNode->m_lSize =
							ppPackingFileSpaceInfo[nIdx + 1]->nEnd - ppPackingFileSpaceInfo[nIdx + 1]->nStart + 1;
					}
					else
					{
						// �����ʵ� normal �̸� ���ο� empty ���� ������Ʈ�� �߰����ش�

						AddEmptyPackingFileInfo(ppPackingFileSpaceInfo[nIdx]->nEnd + 1,
							ppPackingFileSpaceInfo[nIdx + 1]->nStart - ppPackingFileSpaceInfo[nIdx]->nEnd - 1, pAdminPackingEmpty);
					}
				}
			}
		}

		// �پ��ִ� empty ������ ���� merge ��Ų��	
		if ( pAdminPackingEmpty->GetObjectCount() > 0 )
		{
			for ( nIdx = 0; nIdx < lRefFileCount - 1; nIdx++ )
			{
				if ( ppPackingFileSpaceInfo[nIdx]->nType == 1 && ppPackingFileSpaceInfo[nIdx + 1]->nType == 1 )
				{
					// ������ ��� empty�� merge ��Ų��

					ppPackingFileSpaceInfo[nIdx]->pPackingFile->m_pcsDataNode->m_lSize +=
						ppPackingFileSpaceInfo[nIdx + 1]->pPackingFile->m_pcsDataNode->m_lSize;

					// �ʿ���� empty ���� ����
					pAdminPackingEmpty->RemoveObject(ppPackingFileSpaceInfo[nIdx + 1]->pPackingFile->m_pstrFileName);
					delete ppPackingFileSpaceInfo[nIdx + 1]->pPackingFile;

					ppPackingFileSpaceInfo[nIdx]->nEnd = ppPackingFileSpaceInfo[nIdx + 1]->nEnd;
					ppPackingFileSpaceInfo[nIdx + 1]->nStart = ppPackingFileSpaceInfo[nIdx]->nStart;
					ppPackingFileSpaceInfo[nIdx + 1]->pPackingFile = ppPackingFileSpaceInfo[nIdx]->pPackingFile;
				}
			}
		}
	}

	/*
	// �پ��ִ� empty ������ ���� merge ��Ų��	
	if ( pAdminPackingEmpty->GetObjectCount() > 0 )
	{
		int nEmptyIdx = 0;
		CPackingFile** ppCurEmpty;

		// ��� empty ������ �Ĺ� ������ ������ ���� ó��
		for ( ppCurEmpty = (CPackingFile**) pAdminPackingEmpty->GetObjectSequence(&nEmptyIdx);
			ppCurEmpty;
			ppCurEmpty = (CPackingFile**) pAdminPackingEmpty->GetObjectSequence(&nEmptyIdx) )
		{
			ASSERT(ppCurEmpty);
			ASSERT(*ppCurEmpty);
			ASSERT((*ppCurEmpty)->m_pcsDataNode);

			long lSrcStartPos = (*ppCurEmpty)->m_pcsDataNode->m_lStartPos;
			long lSrcEndPos = lSrcStartPos + (*ppCurEmpty)->m_pcsDataNode->m_lSize;

			if ( lSrcStartPos < 0 && lSrcEndPos < 0 )
				continue;

			// �ٽ� ��� Empty ����Ʈ�� ���ؼ�
			int nRefIdx = 0;
			CPackingFile** ppRef;

			for ( ppRef = (CPackingFile**) pAdminPackingEmpty->GetObjectSequence(&nRefIdx);
				ppRef;
				ppRef= (CPackingFile**) pAdminPackingEmpty->GetObjectSequence(&nRefIdx) )
			{					
				ASSERT(ppRef);
				ASSERT(*ppRef);
				ASSERT((*ppRef)->m_pcsDataNode);

				long lDestStartPos = (*ppRef)->m_pcsDataNode->m_lStartPos;
				long lDestEndPos = lDestStartPos + (*ppRef)->m_pcsDataNode->m_lSize;

				if ( lDestStartPos < 0 && lDestEndPos < 0 )
					continue;

				if ( *ppRef == *ppCurEmpty )
					continue;

				if ( lSrcEndPos == lDestStartPos )
				{					
				}
			}
		}
	}
	*/

	/*
	// Empty ���ϵ��� ������� ���������(space)�� ����� ũ�⸦ �����Ѵ�
	{
		if ( pAdminPackingEmpty->GetObjectCount() <= 0 )
			return true;

		int nEmptyIdx = 0;
		CPackingFile** ppCurEmpty;

		// lFirstSpaceSize ���� 0 �� �ƴ� ���� 
		// Data.Bak ������ ù��° ������ ������ ������ ����	
		long lFirstSpaceSize = -1;
		CPackingFile** ppFirstSpaceNeighbor = NULL;

		// ��� empty ������ �Ĺ� ������ ������ ���� ó��
		for ( ppCurEmpty = (CPackingFile**) pAdminPackingEmpty->GetObjectSequence(&nEmptyIdx);
			ppCurEmpty;
			ppCurEmpty = (CPackingFile**) pAdminPackingEmpty->GetObjectSequence(&nEmptyIdx) )
		{
			ASSERT(ppCurEmpty);
			ASSERT(*ppCurEmpty);

			ApAdmin* pRefAdmin;

			int nAdminIdx;
			long lResizedPos = -1;

			bool bNoSpace = false;

			ASSERT((*ppCurEmpty)->m_pcsDataNode);
			
			long lSrcStartPos = (*ppCurEmpty)->m_pcsDataNode->m_lStartPos;
			long lSrcEndPos = lSrcStartPos + (*ppCurEmpty)->m_pcsDataNode->m_lSize;

			if ( lSrcStartPos < 0 && lSrcEndPos < 0 )
				continue;

			for ( nAdminIdx = 0; nAdminIdx < 2; nAdminIdx++ )
			{
				switch ( nAdminIdx )
				{
				case 0 :
					// Normal ����Ʈ�� ���ؼ� search
					pRefAdmin = pAdminPackingNormal;
					break;

				case 1 :
					// Empty ����Ʈ�� ���ؼ� search
					pRefAdmin = pAdminPackingEmpty;
					break;
				}

				CPackingFile** ppRef;
				int nRefIdx = 0;				
				
				for ( ppRef = (CPackingFile**) pRefAdmin->GetObjectSequence(&nRefIdx);
					ppRef;
					ppRef= (CPackingFile**) pRefAdmin->GetObjectSequence(&nRefIdx) )
				{					
					ASSERT(ppRef);
					ASSERT(*ppRef);
					ASSERT((*ppRef)->m_pcsDataNode);

					long lDestStartPos = (*ppRef)->m_pcsDataNode->m_lStartPos;
					long lDestEndPos = lDestStartPos + (*ppRef)->m_pcsDataNode->m_lSize;

					if ( lDestStartPos < 0 && lDestEndPos < 0 )
						continue;

					if ( lFirstSpaceSize == -1 )
					{
						lFirstSpaceSize = lDestStartPos;

						if ( nAdminIdx == 1 ) 
							ppFirstSpaceNeighbor = ppRef; // empty �϶��� ���
						else
							ppFirstSpaceNeighbor = NULL;
					}	
					else
					if ( lFirstSpaceSize > lDestStartPos )
					{
						lFirstSpaceSize = lDestStartPos;

						if ( nAdminIdx == 1 ) 
							ppFirstSpaceNeighbor = ppRef; // empty �϶��� ���
						else
							ppFirstSpaceNeighbor = NULL;
					}

					if ( *ppRef == *ppCurEmpty )
						continue;

					// ��ƴ�� ���� ���
					if ( lDestStartPos == lSrcEndPos )
					{	
						if ( nAdminIdx == 1 ) // ���� Empty �����̸� merge ��Ų��
						{
							ASSERT(*ppRef != *ppCurEmpty);

							(*ppCurEmpty)->m_pcsDataNode->m_lSize = 
								(*ppCurEmpty)->m_pcsDataNode->m_lSize + (*ppRef)->m_pcsDataNode->m_lSize;

							// DataNode ���� �Ѵ� -1�� ��ũ�صе� �� ������ ������ �� ������Ʈ�� ã�� �����Ѵ�
							(*ppRef)->m_pcsDataNode->m_lStartPos = -1;
							(*ppRef)->m_pcsDataNode->m_lSize = -1;
							
							continue;
						}

						bNoSpace = true;
						break;
					}

					if ( lDestStartPos > lSrcEndPos )
					{
						if ( lResizedPos == -1 )
							lResizedPos = lDestStartPos;
						else
						if ( lResizedPos > lDestStartPos )
							lResizedPos = lDestStartPos;
					}
				}

				if ( bNoSpace )
					break;
			}

			if ( !bNoSpace )
			{
				//ASSERT(lResizedPos > 0);

				// ����� �÷��ش�
				if ( lResizedPos > 0 )
					(*ppCurEmpty)->m_pcsDataNode->m_lSize = (lResizedPos - (*ppCurEmpty)->m_pcsDataNode->m_lStartPos);

				// lResizedPos �� -1�� ���� ppCurEmpty�� ������ �����̱� ����
			}
		}		
		
		// ������ Empty ���� ������Ʈ�� �����Ѵ�
		int nDelIdx = 0;
		CPackingFile* pDelEmpty = NULL;
		CPackingFile** ppDelEmpty = (CPackingFile**) pAdminPackingEmpty->GetObjectSequence(&nDelIdx);

		while ( pDelEmpty && *ppDelEmpty )
		{
			pDelEmpty = *ppDelEmpty;

			if ( (pDelEmpty->m_pcsDataNode->m_lStartPos == -1) && (pDelEmpty->m_pcsDataNode->m_lSize == -1) )
			{
				pAdminPackingEmpty->RemoveObject(pDelEmpty->m_pstrFileName);
				delete pDelEmpty;
			}

			ppDelEmpty = (CPackingFile**) pAdminPackingEmpty->GetObjectSequence(&nDelIdx);
		}		
		
		// First Space pos
		ASSERT(lFirstSpaceSize >= 0);

		if ( lFirstSpaceSize > 0 )
		{
			// First Space ������
			// CPackingFile ������Ʈ�� ���� AdminEmpty�� �߰��Ѵ�
			if ( ppFirstSpaceNeighbor && (*ppFirstSpaceNeighbor) )
			{
				(*ppFirstSpaceNeighbor)->m_pcsDataNode->m_lStartPos = 0;
				(*ppFirstSpaceNeighbor)->m_pcsDataNode->m_lSize = (*ppFirstSpaceNeighbor)->m_pcsDataNode->m_lSize + lFirstSpaceSize;
			}
			else
			{
				AddEmptyPackingFileInfo(0, lFirstSpaceSize, pAdminPackingEmpty);
			}
		}		
	}
	*/

	
	/*
	// Normal ���ϵ��� ������� ���������(space)�� ����� Empty ����Ʈ�� �߰��Ѵ�
	{
		if ( pAdminPackingNormal->GetObjectCount() <= 0 )
			return true;

		int nNormalIdx = 0;
		CPackingFile** ppCurNormal;

		// ��� normal ������ �Ĺ� ������ ������ ���� ó��
		for ( ppCurNormal = (CPackingFile**) pAdminPackingNormal->GetObjectSequence(&nNormalIdx);
			ppCurNormal;
			ppCurNormal = (CPackingFile**) pAdminPackingNormal->GetObjectSequence(&nNormalIdx) )
		{
			ASSERT(ppCurNormal);
			ASSERT(*ppCurNormal);

			ApAdmin* pRefAdmin;

			int nAdminIdx;
			long lResizedPos = -1;		

			bool bNoSpace = false;

			ASSERT((*ppCurNormal)->m_pcsDataNode);
			long lSrcStartPos = (*ppCurNormal)->m_pcsDataNode->m_lStartPos;
			long lSrcEndPos = lSrcStartPos + (*ppCurNormal)->m_pcsDataNode->m_lSize;

			for ( nAdminIdx = 0; nAdminIdx < 2; nAdminIdx++ )
			{
				switch ( nAdminIdx )
				{
				case 0 :				
					// Normal ����Ʈ�� ���ؼ� search
					pRefAdmin = pAdminPackingNormal;					
					break;

				case 1 :
					// Empty ����Ʈ�� ���ؼ� search
					pRefAdmin = pAdminPackingEmpty;
					break;
				}

				CPackingFile** ppRef;
				int nRefIdx = 0;

				for ( ppRef = (CPackingFile**) pRefAdmin->GetObjectSequence(&nRefIdx);
					ppRef;
					ppRef= (CPackingFile**) pRefAdmin->GetObjectSequence(&nRefIdx) )
				{
					ASSERT(ppRef);
					ASSERT(*ppRef);
					ASSERT((*ppRef)->m_pcsDataNode);

					long lDestStartPos = (*ppRef)->m_pcsDataNode->m_lStartPos;

					if ( *ppRef == *ppCurNormal )
						continue;

					// ��ƴ�� ���� ���
					if ( lDestStartPos == lSrcEndPos )
					{
						bNoSpace = true;
						break;
					}

					if ( lDestStartPos > lSrcEndPos )
					{
						if ( lResizedPos == -1 )
							lResizedPos = lDestStartPos;
						else
							if ( lResizedPos > lDestStartPos )
								lResizedPos = lDestStartPos;
					}
				}

				if ( bNoSpace )
					break;
			}

			if ( !bNoSpace )
			{
				// Normal ���ϰ� Normal ���� ���̿� ���Ӱ� ���� �� ������
				// CPackingFile ������Ʈ�� ���� AdminEmpty�� �߰��Ѵ�

				// lResizedPos �� -1�� ���� ppCurNormal�� ������ �����̱� ����

				if ( lResizedPos > 0 )
					AddEmptyPackingFileInfo(lSrcEndPos, lResizedPos - lSrcEndPos, pAdminPackingEmpty);
			}
		}	
	}
	*/

	if ( ppPackingFileSpaceInfo )
	{
		int lCounter;
		for ( lCounter = 0; lCounter < lRefFileCount; lCounter++ )
			if ( ppPackingFileSpaceInfo[lCounter] )
				delete ppPackingFileSpaceInfo[lCounter];

		delete [] ppPackingFileSpaceInfo;
	}

	return true;
}
//@}

//@{ 2006/04/13 burumal
CPackingFile* AuPackingManager::AddEmptyPackingFileInfo(long lStartPos, long lSize, ApAdmin* pAdminPackingEmpty)
{
	if ( !pAdminPackingEmpty || (lStartPos < 0) || (lSize <= 0) )
		return NULL;
	
	CPackingFile* pNewEmpty = new CPackingFile;
	ASSERT(pNewEmpty);

	if ( pNewEmpty )
	{
		// �̸��� �ӽ÷� �߱��Ѵ�
		char pTempFileName[256];
		sprintf(pTempFileName, "__e_%05d__.__", m_nEmptyFileSerial++);
		int nFileNameLen = (int)strlen(pTempFileName) + 1;

		char* pNewFileName = new char[nFileNameLen];
		CPackingDataNode* pNewDataNode = new CPackingDataNode;

		if ( !pNewFileName || !pNewDataNode )
		{
			if ( pNewFileName )
				delete pNewFileName;

			if ( pNewDataNode )
				delete pNewDataNode;

			delete pNewEmpty;
		}
		else
		{
			memset(pNewFileName, 0, nFileNameLen);
			memcpy(pNewFileName, pTempFileName, nFileNameLen);

			pNewEmpty->m_pstrFileName = pNewFileName;
			pNewEmpty->m_lFileNameSize = nFileNameLen;
			
			pNewDataNode->m_lStartPos = lStartPos;
			pNewDataNode->m_lSize = lSize;

			pNewEmpty->m_pcsDataNode = pNewDataNode;

			pAdminPackingEmpty->AddObject((void**) &pNewEmpty, pNewFileName);
		}
	}

	return pNewEmpty;
}
//@}

//@{ 2006/04/16 burumal
bool AuPackingManager::ResizeFileSize(char* pFilePath, int nFinalSize)
{	
	if ( !pFilePath || nFinalSize <= 0 )
		return false;

	FILE* fpData = fopen(pFilePath, "rb");
	if ( !fpData )
		return false;
	fseek(fpData, 0, SEEK_END);
	long lDataFileSize = ftell(fpData);
	fclose(fpData);

	if ( lDataFileSize > nFinalSize )
	{
		HANDLE hDataFile = ::CreateFile(
			pFilePath, 
			//FILE_ALL_ACCESS, 
			GENERIC_READ | GENERIC_WRITE,
			0, 
			NULL, 
			OPEN_EXISTING, 
			FILE_ATTRIBUTE_NORMAL, 
			NULL);

		if ( hDataFile != INVALID_HANDLE_VALUE )
		{	
			LARGE_INTEGER llMove;

			llMove.HighPart = 0;
			llMove.LowPart = (DWORD) nFinalSize;

			//SetFilePointerEx(hDataFile, llMove, NULL, FILE_BEGIN); // win98�� �� �Լ��� �������� �ʴ´� -_-			

			DWORD dwPtrLow, dwError;
			dwPtrLow = ::SetFilePointer(hDataFile, llMove.LowPart, &llMove.HighPart, FILE_BEGIN);

			if ( dwPtrLow == INVALID_SET_FILE_POINTER && (dwError = GetLastError()) != NO_ERROR )
			{
				// try once again
				dwPtrLow = ::SetFilePointer(hDataFile, llMove.LowPart, &llMove.HighPart, FILE_BEGIN);

				if ( dwPtrLow == INVALID_SET_FILE_POINTER && (dwError = GetLastError()) != NO_ERROR )
				{
					ASSERT(0);
					CloseHandle(hDataFile);
					return false;
				}
			}

			SetEndOfFile(hDataFile);
			CloseHandle(hDataFile);

			return true;
		}
	}

	return false;
}

bool AuPackingManager::ResizeDatFile(CPackingFolder* pFolder)
{
	if ( !pFolder || !pFolder->m_pstrFolderName )
		return false;

	char strDataFilePath[256];	

	if( !stricmp( pFolder->m_pstrFolderName, "root" ) )
	{
		sprintf( strDataFilePath, "%s", Packing_Data_FileName );
	}
	else
	{
		sprintf( strDataFilePath, "%s\\%s", pFolder->m_pstrFolderName, Packing_Data_FileName );
	}

	FILE* fpData = fopen(strDataFilePath, "rb");
	if ( !fpData )
		return false;
	fseek(fpData, 0, SEEK_END);
	long lDataFileSize = ftell(fpData);
	fclose(fpData);

	if ( lDataFileSize != pFolder->m_lDataFileSize && (lDataFileSize > pFolder->m_lDataFileSize) )
	{
		HANDLE hDataFile = CreateFile(strDataFilePath, 
			//FILE_ALL_ACCESS, 
			GENERIC_READ | GENERIC_WRITE,
			0, 
			NULL, 
			OPEN_EXISTING, 
			FILE_ATTRIBUTE_NORMAL, 
			NULL);

		if ( hDataFile != INVALID_HANDLE_VALUE )
		{	
			LARGE_INTEGER llMove;
			llMove.HighPart = 0;
			llMove.LowPart = pFolder->m_lDataFileSize;
			
			//SetFilePointerEx(hDataFile, llMove, NULL, FILE_BEGIN); // win98�� �� �Լ��� �������� �ʴ´� -_-
			
			DWORD dwPtrLow, dwError;
			dwPtrLow = ::SetFilePointer(hDataFile, llMove.LowPart, &llMove.HighPart, FILE_BEGIN);			
			if ( dwPtrLow == INVALID_SET_FILE_POINTER &&
				(dwError = GetLastError()) != NO_ERROR )
			{
				// try once again
				dwPtrLow = ::SetFilePointer(hDataFile, llMove.LowPart, &llMove.HighPart, FILE_BEGIN);				
				if ( dwPtrLow == INVALID_SET_FILE_POINTER &&
					(dwError = GetLastError()) != NO_ERROR )
				{
					ASSERT(0);
				}
			}

			SetEndOfFile(hDataFile);
			CloseHandle(hDataFile);

			return true;
		}
	}

	return false;
}
//@}

//@{ 2006/04/16 burumal
bool AuPackingManager::CompleteCurrentFolderProcess(char* pstrFolder, int lPackedFileCount)
{
	if ( !pstrFolder )
		return false;

	//bool	bEmptyDat;
	bool	bEmptyRef;	

	char	strReferenceFilePath[256];	
	char	strBackupReferenceFilePath[256];

	char	strDataFilePath[255];
	char	strBackupDataFilePath[255];

	int		nFinalValidDataSize = 0;

	bEmptyRef = false;

	if(  !stricmp( pstrFolder, "root" ) )
	{	
		sprintf( strReferenceFilePath, "%s", Packing_Reference_FileName );
		sprintf( strBackupReferenceFilePath, "%s", Packing_Reference_Backup_FileName );

		sprintf( strDataFilePath, "%s", Packing_Data_FileName );
		sprintf( strBackupDataFilePath, "%s", Packing_Data_Backup_FileName );
	}
	else
	{	
		sprintf( strReferenceFilePath, "%s\\%s", pstrFolder, Packing_Reference_FileName );		
		sprintf( strBackupReferenceFilePath, "%s\\%s", pstrFolder, Packing_Reference_Backup_FileName );

		sprintf( strDataFilePath, "%s\\%s", pstrFolder, Packing_Data_FileName );
		sprintf( strBackupDataFilePath, "%s\\%s", pstrFolder, Packing_Data_Backup_FileName );
	}	

	FILE* fpRef = fopen( strReferenceFilePath, "rb" );	
	if ( !fpRef )
		return false;

	//����ũ�⸦ ����.	
	fseek( fpRef, 0, SEEK_END );

	if( ftell( fpRef ) <= 0  )
	{
		fseek(fpRef, 0, SEEK_END);

		if ( ftell(fpRef) <= 0 )
			bEmptyRef = true;
	}

	fclose( fpRef );

	if( bEmptyRef == true )
	{		
		DeleteFile( strReferenceFilePath );
	}
	else
	{
		//���������� fpRef�� lPackedFileCount�� �������ش�.
		fpRef = fopen( strReferenceFilePath, "rb+" );

		if( fpRef )
		{
			fwrite( &lPackedFileCount, 1, sizeof(int), fpRef );

			fseek(fpRef, 0, SEEK_END);
			int nRefFileSize = ftell(fpRef);
			fseek(fpRef, 0, SEEK_SET);

			char* pRefBuffer = new char[nRefFileSize];
			ASSERT(pRefBuffer);
			char* pDelBuffer = pRefBuffer;

			fread(pRefBuffer, nRefFileSize, 1, fpRef);
			fclose( fpRef );

			//���� ��ȿ�������� size�� �˾Ƴ���			

			int lRefFileCount;
			int lFolderNameSize;

			//������ ���?
			memcpy( &lRefFileCount, pRefBuffer, sizeof(int) );
			pRefBuffer += sizeof(int);

			//���� �̸�ũ��
			memcpy( &lFolderNameSize, pRefBuffer, sizeof(int) );
			pRefBuffer += sizeof(int);

			//���� �̸�			
			pRefBuffer += lFolderNameSize;

			for( int lCounter = 0; lCounter < lRefFileCount; lCounter++ )
			{				
				long				lFileNameSize;
				long				lStartPos, lFileSize;

				//�����̸� ũ�� ����
				memcpy( &lFileNameSize, pRefBuffer, sizeof(int) );
				pRefBuffer += sizeof(int);

				//�����̸�
				pRefBuffer += lFileNameSize;

				//������ġ ����
				memcpy( &lStartPos, pRefBuffer, sizeof(int) );
				pRefBuffer += sizeof(int);

				//����ũ�� ����
				memcpy( &lFileSize, pRefBuffer, sizeof(int) );
				pRefBuffer += sizeof(int);

				if ( (lStartPos + lFileSize) > nFinalValidDataSize )
					nFinalValidDataSize = lStartPos + lFileSize;
			}

			if ( pDelBuffer )
				delete [] pDelBuffer;

			//ASSERT(nFinalValidDataSize > 0);
		}


		//��ȣȭ�Ѵ�.
		fpRef = fopen( strReferenceFilePath, "rb" );
		if( fpRef )
		{
			long				lFileSize;
			char				*pstrEncryptBuffer;

			//���� ũ�⸦ ��´�.
			fseek( fpRef, 0, SEEK_END );
			lFileSize = ftell(fpRef);

			//����ũ�⸦ ������� �ٽ� ������~
			fseek( fpRef, 0, SEEK_SET ); 

			pstrEncryptBuffer = new char[lFileSize];

			fread( pstrEncryptBuffer, 1, lFileSize, fpRef );

			fclose( fpRef );

			//Key�� MD5_HASH_KEY_STRING�̴�.
			if( m_cMD5Encrypt.EncryptString( "1111", pstrEncryptBuffer, lFileSize ) )
			{
				fpRef = fopen( strReferenceFilePath, "wb" );

				if( fpRef )
				{
					fwrite( pstrEncryptBuffer, 1, lFileSize, fpRef );

					fclose( fpRef );
				}
			}

			delete [] pstrEncryptBuffer;
		}
	}

	//��� ������ �����Ѵ�.
	DeleteFile( strBackupReferenceFilePath );


	//@{ 2006/06/13 burumal
	// ��ġ���� Reference.dat , Data.dat ������ �����Ǵ� ���װ� ���� �߻�
	// �� ������ ������ �� �κ����� ���� resizing �۾��� ���Ƶ�.
	/*
	// Data.Bak resize
	if ( nFinalValidDataSize > 0 )
		ResizeFileSize(strBackupDataFilePath, nFinalValidDataSize);
	*/


	// Data.Bak �� Data.Dat �� �̸��� �����Ѵ�
	int nTry;
	for ( nTry = 0; nTry < 3; nTry++ )
		if ( MoveFile( strBackupDataFilePath, strDataFilePath ) != FALSE )
			break;

	return true;
}
//@}