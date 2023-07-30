// RKEntryManager.h
// CRKWriter, CRKParser Wrapper Class
// 2003.07.30 steeple

class CRKParser;
class CRKWriter;

class CRKEntryManager
{
private:
	CRKParser m_csRKEntryParser;
	CRKParser m_csRKHistoryParser;

	CRKWriter m_csRKEntryWriter;
	CRKWriter m_csRKHistoryWriter;

	stRKEntryInfo m_stRKEntryInfo;
	stRKHistoryInfo m_stRKHistoryInfo;

	CString m_szRemoteDrive;	// C, D, E �̷� ����̺갡 �ü��� �ְ�, \\NHN �̷������� �� ���� �ִ�.

public:
	CRKEntryManager();
	~CRKEntryManager();

	void Init();

	void ClearEntryInfo();
	void ClearHistoryInfo();

	void SetRemoteDirve(CString& szDrive) { m_szRemoteDrive = szDrive; }
	void SetEntryParserFileName(CString& szFileName);
	void SetHistoryParserFileName(CString& szFileName);
	void SetEntryWriterFileName(CString& szFileName);
	void SetHistoryWriterFileName(CString& szFileName);

	CString& GetRemoteDrive() { return m_szRemoteDrive; }
	void GetFileNameByFilePath(CString& szFilePath, CString& szFileName);

	BOOL GetEntryFileInfo(CString& szFileName, stRKEntryInfo* pstRKEntryInfo = NULL);
	//BOOL GetHistoryFileInfo(CString& szFileName, stRKHistoryInfo& stRKHistoryInfo); // �̰� �� ���߿� ���� �� �ؼ� ����

	stRKEntryInfo* GetEntryData() { return &m_stRKEntryInfo; }
	stRKHistoryInfo* GetHistoryData() { return &m_stRKHistoryInfo; }



	BOOL AddEntry(CString& szType, CString& szFileName, INT32 lFileSize, CString& szDate, INT32 lVer);	// szFileName ���� PATH ��!!!!! �����̴�.
	BOOL AddEntry(CString& szFileName);	// szFileName ���� PATH �� �����̴�.

	BOOL CheckLatestVersion(CString& szLocalFileName, CString& szFileName);	// szLocalFileName, szFileName ���� PATH �� �����̴�.
	BOOL CheckOut(CString& szLocalFileName, CString& szFileName, CString& szOwner);	// szLocalFileName, szFileName ���� PATH �� �����̴�.
	BOOL CheckIn(CString& szLocalFileName, CString& szFileName, CString& szOwner, BOOL bRealCopy = TRUE);	// szLocalFileName, szFileName ���� PATH �� �����̴�.
	BOOL UndoCheckOut(CString& szLocalFileName, CString& szFileName, CString& szOwner);	// szLocalFileName, szFileName ���� PATH �� �����̴�.

	BOOL GetEntryList(CString& szFileName, CList<stRKEntryInfo, stRKEntryInfo>& csList);	// szFileName ���� PATH �� �����̴�.
	BOOL GetHistoryList(CString& szFileName, CList<stRKHistoryInfo, stRKHistoryInfo>& csList);	// szFileName ���� PATH �� �����̴�.
	CString GetBackupFilePath(CString& szFileName, INT32 lVer);	// szFileName ���� PATH �� �����̴�.

	BOOL RemoveEntry(CString& szFileName, CString& szOwner);	// szFileName ���� PATH �� �����̴�.
	BOOL RemoveDirectoryEntry(CString& szFileName, CString& szOwner);	// szFileName ���� PATH �� �����̴�.

	// ���� ��ƿ�� �Ǿ����.. -0-
	void RemoveFile(CString& szFileName);	// �б� �����̸� Ǯ�� �����.
	void RemoveDirectoryNAllFiles(CString& szDirectoryName);	// ���丮�� ��� ������ �����. ���丮�� �����. ���굵 �����.
	void RemoveDirectoryFinalSlash(CString& szFileName);	// ���丮 �̸��� �������� \ �� ������ �����.
	void CreateSubDirectory(CString& szPath);	// Path �� ���ԵǾ� �ִ� ��� Sub Directory �� �����.
	void GetDirectoryPath(CString& szFileName, CString& szPath);	// szFileName ���� ���丮 Path �� �̴´�.
	void SetReadOnly(CString& szFileName, BOOL bFlag = TRUE);	// �б� �������� �����.
	BOOL SetModifyTime(CString& szOriginFileName, CString& szTargetFileName);	// 2��° ������ ������¥�� 1��° ������ ������¥�� �������ش�.
	BOOL HistoryFile(CString szFileName, CString szDate, CString szOwner, INT32 lVer);	// szFileName ���� PATH �� �����̴�.

protected:
	BOOL BackupFile(CString& szFileName, INT32 lVer);	// szFileName ���� PATH �� �����̴�.

	BOOL RemoveBackupEntry(CString& szFileName);	// ������ �������� ��, ��� �� ó���� ���ش�. szFileName �� ���� PATH
	BOOL RemoveBackupDirectoryEntry(CString& szFileName);	// ���丮�� �������� ��, ��� �� ó���� ���ش�. szFileName �� ���� PATH

	BOOL TemporaryCheckOut(CString& szFileName, CString& szOwner);	// szFileName ���� PATH �� �����̴�.
	BOOL UndoTemporaryCheckOut(CString& szFileName, CString& szOwner);	// szFileName ���� PATH �� �����̴�.

	void GetBackupFilePathByFileName(CString& szFileName, CString& szNewPath);	// ���� PATH ���� Backup File Path �� ��´�.
};