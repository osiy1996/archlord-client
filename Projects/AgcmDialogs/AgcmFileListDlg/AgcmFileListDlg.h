#pragma once

class AFX_EXT_CLASS AgcmFileListDlg
{
public:
	AgcmFileListDlg();
	virtual ~AgcmFileListDlg();

protected:
	CHAR m_szSelectedName[256];

public:
	static AgcmFileListDlg *GetInstance();

	CHAR	*OpenFileList(CHAR *szPathName1, CHAR *szPathName2, CHAR *szPathName3);
	CHAR	*OpenFileList(CHAR *szPath, CHAR *szExt1, CHAR *szExt2, CHAR *szExt3);

	// ���������� ����...
	VOID	SetSelectedName(CHAR *szSet)	{
		strcpy(m_szSelectedName, szSet);
	}
};

/* Example

#include "AgcmFileListDlg.h"

VOID Test()
{
	CHAR			szDest[256];
	CHAR			*pszTemp;
	AgcmFileListDlg dlg;

	pszTemp = dlg.OpenFileList(".\\*.dff", ".\\exp\\*.rws", NULL);
	if(pszTemp) // pszTemp�� �����ʹ� Ŭ������ �Ҹ�Ǳ� ������ ��ȿ!
	{
		strcpy(szDest, pszTemp);
	}
} */
