#pragma once

/**************************************************************************************************

작성일: 2009-03-12
작성자: 문상현 (youngmoon@webzen.co.kr)

설명: 디비의 프로시져 처리를 담당한다.

**************************************************************************************************/

#include "../../../../LIBRARY/public_src/SqlLibHandler.h"
#include "../../../../LIBRARY/public_src/WorkerThread.h"

class CDBProc : public CWorkerThread
{
public:
	CDBProc(void);
	~CDBProc(void);

	//디비에 연결한다.
	bool ConnectDBMS(char* strCon, char* szID, char* szPwd);
	bool ConnectDBMS(char* strIP, UINT nPortNum, char* szDbName, char* szID, char* szPwd, bool bDBConnect = true);
	//쿼리문을 등록한다.
	bool SetQureyText(const char* szQurey);
	//SP 를 호출한다.
	bool ExecSP();

protected:
	virtual void RunProc();

	//연결이 안되어있으면 연결한다.
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
	//SQL LIB 사용하는 객체
	CSqlLibHandler m_slqdb;

};
