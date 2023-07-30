// RKWriter.h
// .rk , .history file writer
// 2003.07.29 steeple

class CRKWriter
{
private:
	CStdioFile m_csFile;
	BOOL m_bOpened;

	CString m_szFileName;

public:
	CRKWriter();
	~CRKWriter();

	void Init();

	BOOL Open();
	BOOL Close();

	BOOL WriteEntry(CString& szType, CString& szFileName, INT32 lFileSize, CString& szDate, INT32 lVer);	// ����� �� �����.
	BOOL WriteOwner(CString& szFileName, INT32 lSize, CString& szDate, CString& szOwner, INT32 lVer = 1);	// Check-Out, Check-In �ϸ� �ҷ��ش�.
	BOOL WriteHistory(CString szFileName, CString szDate, CString szOwner, INT32 lVer);	// Check-In �ϸ� �ҷ��ش�.
	BOOL RemoveEntry(CString& szType, CString& szFileName);	// ���� ������.. 

	BOOL WriteRepository(CString& szName, CString& szPath, CString& szWorkingFolder, INT8 cDefault);	// Repository ��� �� ���� Info.rk ���� �� �� ���.
	BOOL RemoveRepository(CString& szName);	// szName �� �ִ� ���� �����.
	BOOL WriteRemotePath(CString& szName, CString& szPath);	// szName Repository �� Path �� �����Ѵ�.
	BOOL WriteWorkingFolder(CString& szName, CString& szWorkingFolder);	// szName Repository �� WokingFolder �� �����Ѵ�.
	BOOL WriteRepositoryDefault(CString& szName, BOOL bDefault);	// Default ������ ���ش�.

	BOOL WriteUser(CString& szName, CString& szPassword, BOOL bAdmin, BOOL bAdd = TRUE);	// Repository �� User �� Add / Remove ���ش�.
	BOOL WriteAdmin(CString& szName, BOOL bAdmin);	// User �� Admin �� �����Ѵ�.

	void SetFileName(CString& szFileName) { m_szFileName = szFileName; }
};
