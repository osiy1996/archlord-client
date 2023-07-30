#pragma once

/**************************************************************************************************

�ۼ���: 2009-03-12
�ۼ���: ������ (youngmoon@webzen.co.kr)

����: ����� ���ν��� ó���� ����Ѵ�.

**************************************************************************************************/

#include "../../../../LIBRARY/public_src/SqlLibHandler.h"
#include "../../../../LIBRARY/public_src/WorkerThread.h"

class CDBProc : public CWorkerThread
{
public:
	CDBProc(void);
	~CDBProc(void);

	//��� �����Ѵ�.
	bool ConnectDBMS(char* strCon, char* szID, char* szPwd);
	bool ConnectDBMS(char* strIP, UINT nPortNum, char* szDbName, char* szID, char* szPwd, bool bDBConnect = true);
	//�������� ����Ѵ�.
	bool SetQureyText(const char* szQurey);
	//SP �� ȣ���Ѵ�.
	bool ExecSP();

protected:
	virtual void RunProc();

	//������ �ȵǾ������� �����Ѵ�.
	bool IsConnected();	
	int GetCountFiled();
	bool FetchNext();
	bool ModifyValue(VALUE_TYPE vt, const char* szCol, tm& _time);
	bool ModifyValue(VALUE_TYPE vt, const char* szCol, long& nValue);
	bool ModifyValue(VALUE_TYPE vt, const char* szCol, short& nValue);
	bool ModifyValue(VALUE_TYPE vt, const char* szCol, double& nValue);
	bool ModifyValue(VALUE_TYPE vt, const char* szCol, char& nValue);
	bool ModifyValue(VALUE_TYPE vt, const char* szCol, BYTE& nValue);
	bool ModifyValue(VALUE_TYPE vt, const char* szCol, char* nValue, int Size);
	bool ModifyValue(VALUE_TYPE vt, const char* szCol, WCHAR* nValue, int Size);
	
private:
	//SQL LIB ����ϴ� ��ü
	CSqlLibHandler m_slqdb;

};
