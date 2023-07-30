#pragma once

#include <AgcmUILogin/AcArchlordInfo.h>
#include <AgcmUIControl/AcUIToolTip.h>

class AgcmUIManager2;
class AgcmLogin;

class AgcmUILoginSelect
{
public:
	AgcmUILoginSelect		( VOID );
	~AgcmUILoginSelect		( VOID );

	BOOL					Initialize				( AgcmUIManager2*	pManager , AgcmLogin* pcmLogin );

	VOID					OpenLoginSelect			( VOID );		//	Login Server ���� â�� ����
	VOID					CloseLoginSelect		( VOID );		//	Login Server ���� â�� �ݴ´�


	BOOL					IsRetryLoginServer		( VOID );		//	������ ������ IP�� ���Ҵ��� �˻�
	VOID					ReConnectLoginServer	( VOID );		//	�α��� ���� �������� �õ��Ѵ�

	stLoginGroup*			GetLoginGroup			( CONST std::string strGroupName );


	static BOOL				CBSelectLoginServer		( PVOID pClass , PVOID pData1 , PVOID pData2 , PVOID pButton );
	static BOOL				CBMouseOnLoginServer	( PVOID pClass , PVOID pData1 , PVOID pData2 , PVOID pButton );

	static BOOL				CBSelectExit			( PVOID pClass , PVOID pData1 , PVOID pData2 , PVOID pButton );

protected:
	BOOL					_LoginSelectWindowInit	( VOID );

	AgcmUIManager2*			m_pUIManager;
	AgcmLogin*				m_pcmLogin;

	AcUIToolTip				m_csLoginSelectUI;	//	Tooltip ��� Button�� �ٿ��� ����Ѵ�

	AcArchlordInfo			m_ArchlordLoginInfo;

	std::string				m_strLastSelectGroup;	//	�α��μ��� ����Ʈâ���� ������ �׷�
};
