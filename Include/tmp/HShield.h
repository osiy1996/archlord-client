/*++ BUILD Version: 3,3,11,1  

Copyright (c) 2003-2005  AhnLab, Inc.

Module Name:

    HShield.h

Abstract:

    This header file defines the data types and constants, and exposes APIs 
	that are defined by HackShield Library.

Tag Information:
	
	This header file is auto-generated at 2005-03-26 14:10:41.

--*/
           
#ifndef _HSHIELD_H_INC
#define _HSHIELD_H_INC

// HShield Library Version Info
#define	HSVERSION(w,x,y,z)	(w) <<24|(x&0xFF)<<16|(y&0xFF)<<8|(z&0xFF)

#define HSHIELD_PACKAGE_VER	HSVERSION(3,3,11,1)
#define HSHIELD_PACKAGE_CRC	_T("lRkEssr9QE6fJgOfAVgcWA==")


// Init Option
#define AHNHS_CHKOPT_SPEEDHACK					0x2
#define AHNHS_CHKOPT_READWRITEPROCESSMEMORY 	0x4

#define AHNHS_CHKOPT_KDTARCER					0x8
#define AHNHS_CHKOPT_OPENPROCESS				0x10
#define AHNHS_CHKOPT_AUTOMOUSE					0x20
#define AHNHS_CHKOPT_MESSAGEHOOK                0x40

#define AHNHS_CHKOPT_PROCESSSCAN                0x80

#define AHNHS_CHKOPT_ALL AHNHS_CHKOPT_SPEEDHACK \
						| AHNHS_CHKOPT_READWRITEPROCESSMEMORY \
						| AHNHS_CHKOPT_KDTARCER \
						| AHNHS_CHKOPT_OPENPROCESS \
						| AHNHS_CHKOPT_AUTOMOUSE \
						| AHNHS_CHKOPT_MESSAGEHOOK \
						| AHNHS_CHKOPT_PROCESSSCAN \
						| AHNHS_CHKOPT_HSMODULE_CHANGE \
						| AHNHS_USE_LOG_FILE

#define AHNHS_USE_LOG_FILE                      0x100
#define AHNHS_CHECK_UPDATE_STATE                0x200
#define AHNHS_ALLOW_SVCHOST_OPENPROCESS         0x400
#define AHNHS_ALLOW_LSASS_OPENPROCESS           0x800
#define AHNHS_ALLOW_CSRSS_OPENPROCESS           0x1000
#define AHNHS_DONOT_TERMINATE_PROCESS           0x2000
#define AHNHS_DISPLAY_HACKSHIELD_LOGO           0x4000
#define AHNHS_CHKOPT_HSMODULE_CHANGE			0x8000


// SpeedHack Sensing Ratio
#define	AHNHS_SPEEDHACK_SENSING_RATIO_HIGHEST	0x1
#define	AHNHS_SPEEDHACK_SENSING_RATIO_HIGH		0x2
#define	AHNHS_SPEEDHACK_SENSING_RATIO_NORMAL	0x4
#define	AHNHS_SPEEDHACK_SENSING_RATIO_LOW		0x8
#define	AHNHS_SPEEDHACK_SENSING_RATIO_LOWEST	0x10
#define	AHNHS_SPEEDHACK_SENSING_RATIO_GAME		0x20


typedef int (__stdcall* PFN_AhnHS_Callback)(
	long lCode,
	long lParamSize,
	void* pParam 
);


#if defined(__cplusplus)
extern "C"
{
#endif

int __stdcall _AhnHS_Initialize(
	const char* szFileName,
	PFN_AhnHS_Callback pfn_Callback, //Callback Function Pointer
	int nGameCode,                   //Game Code
	const char* szLicenseKey,        //License Code
	DWORD dwOption,                  //Init Option
	UINT unSHackSensingRatio		 //SpeedHack Sensing Ratio
);


int __stdcall _AhnHS_StartService();

int __stdcall _AhnHS_StopService();

int __stdcall _AhnHS_PauseService( DWORD dwPauseOption );

int __stdcall _AhnHS_ResumeService( DWORD dwResumeOption );

int __stdcall _AhnHS_Uninitialize();

/*
// 3.2.0.1 ��Ƽ���μ��� �����Ǹ鼭 ������ API
BOOL __stdcall _AhnHS_CreateProcess( LPCSTR lpApplicationName,
								    LPSTR lpCommandLine,
									LPSECURITY_ATTRIBUTES lpProcessAttributes,
									LPSECURITY_ATTRIBUTES lpThreadAttributes,
									BOOL bInheritHandles,
									DWORD dwCreationFlags,
									LPVOID lpEnvironment,
									LPCSTR lpCurrentDirectory,
									LPSTARTUPINFOA lpStartupInfo,
									LPPROCESS_INFORMATION lpProcessInformation );
*/


#if defined(__cplusplus)
}
#endif


//ERROR CODE
#define HS_ERR_OK                               0x000				// �Լ� ȣ�� ����
#define HS_ERR_UNKNOWN                          0x001				// �� �� ���� ������ �߻��߽��ϴ�.
#define HS_ERR_INVALID_PARAM                    0x002				// �ùٸ��� ���� �����Դϴ�.
#define HS_ERR_NOT_INITIALIZED                  0x003				// �ٽ��� ����� �ʱ�ȭ���� ���� �����Դϴ�.
#define HS_ERR_COMPATIBILITY_MODE_RUNNING       0x004				// ���� ���μ����� ȣȯ�� ���� ����Ǿ����ϴ�.

#define HS_ERR_INVALID_LICENSE                  0x100				// �ùٸ��� ���� ���̼��� Ű�Դϴ�.
#define HS_ERR_INVALID_FILES                    0x101				// �߸��� ���� ��ġ�Ǿ����ϴ�. ���α׷��� �缳ġ�Ͻñ� �ٶ��ϴ�.
#define HS_ERR_INIT_DRV_FAILED                  0x102
#define HS_ERR_ANOTHER_SERVICE_RUNNING          0x103				// �ٸ� �����̳� ���μ������� �ٽ��带 �̹� ����ϰ� �ֽ��ϴ�.
#define HS_ERR_ALREADY_INITIALIZED              0x104				// �̹� �ٽ��� ����� �ʱ�ȭ�Ǿ� �ֽ��ϴ�.
#define HS_ERR_SOFTICE_DETECT                   0x105				// ��ǻ�Ϳ��� SoftICE ������ �����Ǿ����ϴ�. SoftICE ������ ������Ų �ڿ� �ٽ� ��������ֽñ�ٶ��ϴ�.
#define HS_ERR_EXECUTABLE_FILE_CRACKED			0x106				// ���� ������ �ڵ尡 ũ�� �Ǿ����ϴ�.
#define HS_ERR_NEED_ADMIN_RIGHTS				0x107				// ADMIN ������ �ʿ��մϴ�.

#define HS_ERR_START_ENGINE_FAILED              0x200				// ��ŷ ���α׷� ���� ������ ������ �� �����ϴ�.
#define HS_ERR_ALREADY_SERVICE_RUNNING          0x201				// �̹� �ٽ��� ���񽺰� ���� ���Դϴ�.
#define HS_ERR_DRV_FILE_CREATE_FAILED           0x202				// �ٽ��� ����̹� ������ ������ �� �����ϴ�.
#define HS_ERR_REG_DRV_FILE_FAILED              0x203				// �ٽ��� ����̹��� ����� �� �����ϴ�.
#define HS_ERR_START_DRV_FAILED                 0x204				// �ٽ��� ����̹��� ������ �� �����ϴ�.

#define HS_ERR_SERVICE_NOT_RUNNING              0x301				// �ٽ��� ���񽺰� ����ǰ� ���� ���� �����Դϴ�.
#define HS_ERR_SERVICE_STILL_RUNNING            0x302				// �ٽ��� ���񽺰� ���� �������� �����Դϴ�.

#define HS_ERR_NEED_UPDATE                      0x401				// �ٽ��� ����� ������Ʈ�� �ʿ��մϴ�.

//CallBack Code
#define AHNHS_ACTAPC_DETECT_ALREADYHOOKED		0x010101			// �Ϻ� API�� �̹� ��ŷ�Ǿ� �ִ� �����Դϴ�. (�׷��� �����δ� �̸� �����ϰ� �ֱ� ������ ��ŷ���α׷��� �������� �ʽ��ϴ�.)
#define AHNHS_ACTAPC_DETECT_AUTOMOUSE			0x010102			// ���丶�콺 ������ �����Ǿ����ϴ�.	
#define AHNHS_ACTAPC_DETECT_HOOKFUNCTION		0x010301			// ��ȣ API�� ���� ��ŷ ������ �����Ǿ����ϴ�.
#define AHNHS_ACTAPC_DETECT_DRIVERFAILED		0x010302			// ��ŷ ���� ����̹��� �ε���� �ʾҽ��ϴ�.
#define AHNHS_ACTAPC_DETECT_SPEEDHACK			0x010303			// ���ǵ��ٷ��� ���α׷��� ���� �ý��� �ð��� ����Ǿ����ϴ�.
#define AHNHS_ACTAPC_DETECT_SPEEDHACK_APP		0x010304			// ���ǵ��ٷ��� ���α׷��� ���� �ý��� �ð��� ����Ǿ����ϴ�.
#define AHNHS_ACTAPC_DETECT_MESSAGEHOOK			0x010306			// �޽��� ��ŷ�� �õ��Ǿ����� �̸� �������� ���߽��ϴ�.
#define AHNHS_ACTAPC_DETECT_KDTRACE				0x010307			// ����� Ʈ���̽��� �߻��ߴ�.(Ŀ�� ����� Ȱ��ȭ, ���� �극��ũ ������ ó��)
#define AHNHS_ACTAPC_DETECT_KDTRACE_CHANGED		0x010308			// ��ġ�� ����� Ʈ���̽��� ����Ǿ���.
#define AHNHS_ACTAPC_DETECT_SPEEDHACK_RATIO		0x01030B			// ���ǵ��� ���� �ɼ��� 'GAME'�� ��� �� �ݹ����� �ֱ� 5�ʵ����� �ð������� ���޵˴ϴ�.
#define AHNHS_ENGINE_DETECT_GAME_HACK           0x010501			// ���� ��ŷ���� ������ �߰ߵǾ����ϴ�.
#define AHNHS_ENGINE_DETECT_GENERAL_HACK        0x010502			// �Ϲ� ��ŷ��(Ʈ���̸� ����)�� �߰ߵǾ����ϴ�.
#define AHNHS_ACTAPC_DETECT_MODULE_CHANGE       0x010701			// �ٽ��� ���ø���� ����Ǿ����ϴ�.

#pragma pack(1)

typedef struct _ACTAPCPARAM_DETECT_HOOKFUNCTION
{	
	char szFunctionName[128];		// file path
	char szModuleName[128];
	
} ACTAPCPARAM_DETECT_HOOKFUNCTION, *PACTAPCPARAM_DETECT_HOOKFUNCTION;


// ���丶�콺 ���� APC Structure
typedef struct
{
	BYTE	byDetectType;			// AutoMouse ���� API ȣ�� 1, AutoMouse ���� API ���� 2
	DWORD	dwPID;					// AutoMouse ���μ��� �Ǵ� API �� ������ ���μ���
	CHAR	szProcessName[16+1];	// ���μ�����
	CHAR	szAPIName[128];			// ȣ��� API �� �Ǵ� ������ API ��
}ACTAPCPARAM_DETECT_AUTOMOUSE, *PACTAPCPARAM_DETECT_AUTOMOUSE;


#define	EAGLE_AUTOMOUSE_APCTYPE_API_CALLED			1
#define	EAGLE_AUTOMOUSE_APCTYPE_API_ALTERATION		2
#define	EAGLE_AUTOMOUSE_APCTYPE_SHAREDMEMORY_ALTERATION	3

#pragma pack()


#endif _HSHIELD_H_INC