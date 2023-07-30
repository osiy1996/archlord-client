#ifndef __HanAuthForClient__
#define __HanAuthForClient__

#ifdef HANAUTHFORCLIENT_EXPORTS
	#define HANAUTH_API __declspec(dllexport)
#else
	#define HANAUTH_API __declspec(dllimport)
	#ifdef _DEBUG
		#pragma comment(lib, "HanAuthForClientD.lib") 
	#else
		#pragma comment(lib, "HanAuthForClient.lib") 
	#endif
#endif

#define SIZE_AUTHSTRING			2048
#define SIZE_GAMESTRING			4096

#define SERVICE_NATION			0x000000FF
#define SERVICE_KOR				0x00000001
#define SERVICE_USA				0x00000002
#define SERVICE_JPN				0x00000003
#define SERVICE_CHN				0x00000004
#define SERVICE_TIW				0x00000005

#define SERVICE_TYPE			0x00000F00
#define SERVICE_ALPHA			0x00000100
#define SERVICE_REAL			0x00000200
#define SERVICE_BETA			0x00000300

#define SERVICE_SITE			0x0000F000
#define SERVICE_HANGAME			0x00001000
#define SERVICE_ASOBLOG			0x00002000
#define SERVICE_IJJI			0x00003000
#define SERVICE_GAMETAP			0x00004000

// The number of consecutive failures after which clients will stop to refresh - currently only applies to USA
#define REFRESH_MAXFAIL			3

// The number of variables in the string returned after cookie refresh in USA
#define USA_REFRESH_RET_VARS	4

/*
Return value Info:
0 : Ok
- : fault from  function / this system 
    -1 : general fault.
+ : fault from  auth server
    +1 : parameter of auth is invalid.
	+2 : this ip address is invalid.
	+3 : memeberID is invalid.
	+4 : password incorrect
	+5 : password mismatch ( over 3 times )
	+6 : memberID is not HangameID
	+7 : system error
	+8 : cookie setting error
*/
#define HAN_AUTHCLI_OK								0
#define HAN_AUTHCLI_ARGUMENT_INVALID				-1
#define HAN_AUTHCLI_INITED_ALREADY					-2
#define HAN_AUTHCLI_INITED_NOT						-3
#define HAN_AUTHCLI_INITED_FAIL						-4
#define HAN_AUTHCLI_AUTHHTTP_INITFAIL				-5
#define HAN_AUTHCLI_AUTHHTTP_CONNFAIL				-6
#define HAN_AUTHCLI_REFRESHHTTP_INITFAIL			-7
#define HAN_AUTHCLI_REFRESHHTTP_CONNFAIL			-8
#define HAN_AUTHCLI_NOT_IMPLEMENTED					-9
#define HAN_AUTHCLI_AUTHHTTP_OPENREQFAIL			-10
#define HAN_AUTHCLI_AUTHHTTP_SENDREQFAIL			-11
#define HAN_AUTHCLI_COOKIE_SETFAIL					-13
#define HAN_AUTHCLI_GAMESTRING_IDINVALID			-14
#define HAN_AUTHCLI_GAMESTRING_USNINVALID			-15
#define HAN_AUTHCLI_GAMESTRING_GAMEIDINVALID		-16
#define HAN_AUTHCLI_GAMESTRING_SERVICECODEINVALID	-17
#define HAN_AUTHCLI_CREATEINTANCEFAIL				-18

#define HAN_AUTHCLI_DLL_UNEXPECTED		 -100

#define HAN_AUTHCLI_PARAM_INVALID			1
#define HAN_AUTHCLI_IPADDR_INVALID			2
#define HAN_AUTHCLI_MEMBERID_INVALID		3
#define HAN_AUTHCLI_PASSWORD_INCORRECT		4
#define HAN_AUTHCLI_PASSWORD_MISMATCHOVER	5
#define HAN_AUTHCLI_MEMBERID_NOTEXIST		6
#define HAN_AUTHCLI_SYSTEM_ERROR			7
#define HAN_AUTHCLI_COOKIE_SETERROR			8


HANAUTH_API int __stdcall HanAuthInit(/*IN*/ char* szGameId,
									  /*IN*/ int nServiceCode);

HANAUTH_API int __stdcall HanAuthInitGameString(/*IN*/ char* szGameString);

HANAUTH_API int __stdcall HanAuthForClientGameString(/*IN*/ char* szGameString);

HANAUTH_API int __stdcall HanAuthRefresh(/*IN*/ char* szMemberId);
										   
HANAUTH_API int __stdcall GetAuthString(/*OUT*/ char* szAuthString, 
										/*IN*/ int nSize);

HANAUTH_API int __stdcall UpdateGameString(/*IN*/ char* szOldGameString, 
										   /*OUT*/ char* szNewGameString,
										   /*IN*/ int nSize);

HANAUTH_API int __stdcall HanAuthGetId(/*IN*/ char* szGameString,
									   /*OUT*/ char* szMemberId,
									   /*IN*/ int nsize_id);

HANAUTH_API int __stdcall HanAuthGetNationCode(/*IN*/ char* szGameString,
											   /*OUT*/ int* pNationCode);

HANAUTH_API int __stdcall HanAuthGetServiceTypeCode(/*IN*/ char* szGameString,
													/*OUT*/ int* pServiceTypeCode);

HANAUTH_API int __stdcall HanAuthGetSiteCode(/*IN*/ char* szGameString,
											 /*OUT*/int* pSiteCode);


HANAUTH_API int __stdcall HanAuthGetValue(/*IN*/ char* szGameString,
										  /*IN*/ char* szValueName,
										  /*OUT*/ char* pBuffer,
										  /*IN*/ int nSize);
// only USA.
HANAUTH_API int __stdcall HanAuthGetUSN(/*IN*/ char* szGameString);

// only JPN.
HANAUTH_API int __stdcall HanAuthGetBillUserNo(/*IN*/ char* szGameString,
											   /*OUT*/ char* szUserNo,
											   /*IN*/ int nSize);

HANAUTH_API int __stdcall HanAuthMiniExplorer(bool bShow);

// only KOR.
HANAUTH_API int __stdcall HanAuthMarketingTag(/*IN*/ char* szGameString);
/*
Return value Info:
0 : Ok
- :fault from  function / this system 
fault from  auth server ( only korea )
	-102	�Ѱ����÷�����ݹ̳����� �α����� �Ұ����մϴ�.
	-1		�������� �ʴ� ���̵��̰ų� �߸��� ���̵��Դϴ�.
	33		ȸ������ ��й�ȣ�� 3ȸ �̻� �ٸ��� �ԷµǾ����ϴ�. 
	5		�Է��Ͻ� �Ѱ��Ӿ��̵�� ��ϵǾ� �ִ� �ֹε�Ϲ�ȣ�� �̸��� ��ġ���� �ʽ��ϴ�.
	1  		ȸ������ ���̵�, ��й�ȣ�� �ٽ� �ѹ� Ȯ���� �ּ���. 

	11		���ϴ� �ҷ��̿��ڷ� �Ǹ�Ǿ� ���̵� �����Ǿ����ϴ�.
	12		���ϴ� �ҷ��̿��ڷ� �Ǹ�Ǿ� ���̵� �Ͻ������Ǿ����ϴ�.
	13		���ϴ� �ҷ��̿��ڷ� �Ǹ�Ǿ� ���̵� 90�ϰ� �̿������� ����ó�� �� �����Դϴ�.

	14		���� �����Ͻ� ���̵�� ������û ó������ ���̵��Դϴ�.
	15		���� �����Ͻ� ���̵�� ����/������ ���̵��Դϴ�.

	16		���� �����Ͻ� ���̵�� ��� ó������ ���̵��Դϴ�.	
			�������� Ȯ�� �� �� �ִ� �ֹε����, ����������, �л��� �纻�� \n
			�ѽ�[ 02-6008-5487 ]�� �����ֽø� Ȯ���� ���ĵ�� ó���� �˴ϴ�.

	17		�θ�� ���� ������ ���� �������� �ʰų� �θ���� ���Ǹ� ���� ���߽��ϴ�.\n
			���� �����Ͻ� ���̵�� 14�� �̸� ���̵��Դϴ�.\n
			14�� �̸� ����� ��� �θ���� �̸��� �ּҷ� Ȯ���� ��������\n
			�θ���� ���Ǹ� �� ��쿡�� �α����� �����մϴ�.
	18		���� �����Ͻ� ���̵�� �������� ���̵� �ƴմϴ�.
	19		14�� �̸� ������ - �θ� ���� ��Ȯ�λ����Դϴ�.\n �Ѱ��Ӹ��ο��� �α��Ͻñ� �ٶ��ϴ�.
	20		14�� �̸� ������ - �θ� ���� Ȯ�λ����Դϴ�.\n �Ѱ��Ӹ��ο��� �α��Ͻñ� �ٶ��ϴ�.
*/										   
HANAUTH_API int __stdcall HanAuthForClientDirect(/*IN*/ char* szMemberId, 
												 /*IN*/ char* szPassword);

#endif // __HanAuthForClient__