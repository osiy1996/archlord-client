#include "StdAfx.h"
#include "DBProc.h"

CDBProc::CDBProc(void)
{
}

CDBProc::~CDBProc(void)
{
}

bool CDBProc::ConnectDBMS(char* strCon, char* szID, char* szPwd)
{
	m_slqdb.SetConnectInfo(strCon, szID,szPwd);
	return m_slqdb.ConnectDBMS();
}


bool CDBProc::ConnectDBMS(char* strIP, UINT nPortNum, char* szDbName, char* szID, char* szPwd, bool bDBConnect)
{
	char szConn[1024] = {0};
	sprintf(szConn,"%s,%d@%s",strIP,nPortNum,szDbName);
	m_slqdb.SetConnectInfo(szConn, szID,szPwd, SA_SQLServer_Client);

	if(bDBConnect) return m_slqdb.ConnectDBMS();
	return true;
}

bool CDBProc::SetQureyText(const char* szQurey)
{	
	return m_slqdb.SetQureyText(szQurey);	
}

bool CDBProc::ExecSP()
{	
	if(!m_slqdb.Exec())
	{
		if(m_slqdb.ConnectDBMS())
			return m_slqdb.Exec();
		else return false;
	}

	return true;
}


void CDBProc::RunProc()
{		
	
}


bool CDBProc::IsConnected()
{
	if(m_slqdb.IsConnected()) return true;

	return m_slqdb.ConnectDBMS();
}

int CDBProc::GetCountFiled()
{
	return m_slqdb.GetCountFiled();
}

bool CDBProc::FetchNext()
{
	if((m_slqdb.GetCountFiled() > 0) && m_slqdb.isResultSet())
	{
		return m_slqdb.FetchNext();
	}
	else
	{
		return false;
	}	
}

bool CDBProc::ModifyValue(VALUE_TYPE vt, const char* szCol, tm& _time)
{
	return m_slqdb.ModifyValue(vt, szCol, _time);
}

bool CDBProc::ModifyValue(VALUE_TYPE vt, const char* szCol, long& nValue)
{
	return m_slqdb.ModifyValue(vt, szCol, nValue);
}

bool CDBProc::ModifyValue(VALUE_TYPE vt, const char* szCol, short& nValue)
{
	return m_slqdb.ModifyValue(vt, szCol, nValue);
}

bool CDBProc::ModifyValue(VALUE_TYPE vt, const char* szCol, double& nValue)
{
	return m_slqdb.ModifyValue(vt, szCol, nValue);
}

bool CDBProc::ModifyValue(VALUE_TYPE vt, const char* szCol, char& nValue)
{
	return m_slqdb.ModifyValue(vt, szCol, nValue);
}

bool CDBProc::ModifyValue(VALUE_TYPE vt, const char* szCol, BYTE& nValue)
{
	return m_slqdb.ModifyValue(vt, szCol, nValue);
}

bool CDBProc::ModifyValue(VALUE_TYPE vt, const char* szCol, char* nValue, int Size)
{
	return m_slqdb.ModifyValue(vt, szCol, nValue, Size);
}

bool CDBProc::ModifyValue(VALUE_TYPE vt, const char* szCol, WCHAR* nValue, int Size)
{	
	return m_slqdb.ModifyValue(vt, szCol, nValue, Size);
}