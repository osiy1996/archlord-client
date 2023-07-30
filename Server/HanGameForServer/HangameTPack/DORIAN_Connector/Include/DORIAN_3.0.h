// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the DORIAN_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// DORIAN_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#ifndef __DORIAN_CONNECTOR_H__
#define __DORIAN_CONNECTOR_H__

 #if (defined(_WIN64) || defined(_WIN32)) 
	#ifdef DORIAN_EXPORTS
		#define DORIAN_EXTERN __declspec(dllexport)
	#else
		#define DORIAN_EXTERN __declspec(dllimport)		

		#ifndef DORIAN_NOSTUBLIB
			#ifdef _DEBUG
				#ifdef _WIN64
					#pragma comment(lib, "DORIAN3_Connector_BaseD_x64.lib")
				#else
					#pragma comment(lib, "DORIAN3_Connector_BaseD.lib")
				#endif // _WIN64
			#else
				#ifdef _WIN64
					#pragma comment(lib, "DORIAN3_Connector_Base_x64.lib")
				#else
					#pragma comment(lib, "DORIAN3_Connector_Base.lib")
				#endif // _WIN64
			#endif // _DEBUG
		#endif // DORIAN_NOSTUBLIB
	#endif // DORIAN_EXPORTS	
#elif defined(__linux__)
	#define DORIAN_EXTERN
	#define __stdcall
#else
	#error OS not supported.
#endif // _WINDOWS

#ifndef IN
	#define IN
#endif

#ifndef OUT
	#define OUT
#endif

#ifndef INOUT
	#define INOUT
#endif

#if (defined(_WIN64) || defined(_WIN32))
	typedef __int64 int64;
#else
	#include <stdint.h>
	typedef int64_t int64;
#endif

namespace Dorian
{
	//====================================================================
	// Billing notification info define
	const unsigned int BILLING_EXPIRE					= 0;	// ��ǰ�� ����
	const unsigned int BILLING_REMAINING_TIME_ALRAM		= 1;	// ��ǰ�� �ܿ� �ð��� 5�� ����
	const unsigned int CONFIRM_USER_EXIST				= 3;	// ����� �������� Ȯ��
	const unsigned int BILLING_GAME_SPECIFIC			= 1000; // ���Ӻ� Ư�� ��ǰ��
	//--------------------------------------------------------------------

	//====================================================================
	// üũ��/��ǰ�� ��ȸ API result code define
	const unsigned int RESULT_OK						= 0;	// ����
	const unsigned int RESULT_NO_PRODUCT				= 1;	// ��ǰ�� ����
	const unsigned int RESULT_FAILED_OPERATION			= -1;	// ���� ���� ����
	//--------------------------------------------------------------------

	//====================================================================
	// ���ڿ� �ִ� ���� ����
	enum MAX_LENGTH
	{
		// ���� ���� ���� ����
		eMAX_GAME_ID = 20,
		eMAX_WORLD_ID = 20,
		eMAX_SERVER_ID = 20,

		// ����� ���� ����
		eMAX_USER_IP = 15,
		eMAX_USER_ID = 22,

		// ��ǰ�� �������� Ȯ�� ���� ����
		eMAX_OPTION = 1024,

		// ��ǰ�� ���⳯¥ ���� ����
		eMAX_TIME = 14,

		// ��ǰ�� �Ǿ��� �ڵ� ���� ����
		eMAX_CRM = 15,

		// ���� �޽��� ���� ����
		eMAX_ERROR_MSG = 1024,
	};
	//--------------------------------------------------------------------

	// DORIAN server ���� ����.
	typedef struct _GAMESERVER_INFO
	{
		char	szGameID[eMAX_GAME_ID + 1];
		char	szGameServerID[eMAX_SERVER_ID + 1];
		char	szWorldID[eMAX_WORLD_ID + 1];
	}GAMESERVER_INFO, *LPGAMESERVER_INFO;

	// �������� ��ȸ����� ���� �ڵ�
	typedef struct _DATA_HANDLE_EX
	{
	} *DATA_HANDLE_EX;

	//====================================================================
	// ����/���� ��ǰ�� ���� ���� ����
	// ��ǰ�� ���� ���� ����
	typedef struct _BILLING_NOTI_INFO
	{
		char	szUserID[eMAX_USER_ID + 1];
		char	szUserIP[eMAX_USER_IP + 1];
		int		nEventCode;
		int		nSpecificCode;
		char	szOptions[eMAX_OPTION + 1];
	} BILLING_NOTI_INFO, *LPBILLING_NOTI_INFO;

	// ��ǰ�� ����
	typedef struct _BILL_INFO
	{
		int		nBillType;
		int		nProductID;
		int64	nRemainMinutes;
		char	szCRM[eMAX_CRM + 1];
		char	szEndDate[eMAX_TIME + 1];
	} BILL_INFO, *LPBILL_INFO;

	// UserMoveTo/UserMoveFrom ���� ��ǰ�� ����
	typedef struct _BILL_INFO_FOR_USER_MOVE
	{
		int		nBillType;
		int		nProductID;
		int		nSeqNo;
		bool	bFirstYN;
		char	szCRM[eMAX_CRM + 1];
		char	szEndDate[eMAX_TIME + 1];
		char	chFreeYN;
	} BILL_INFO_FOR_USER_MOVE, *LPBILL_INFO_FOR_USER_MOVE;

	// üũ��
	typedef struct _REQUEST_CHECKIN
	{
		char	szUserID[eMAX_USER_ID + 1];
		char	szUserIP[eMAX_USER_IP + 1];
	} REQUEST_CHECKIN, *LPREQUEST_CHECKIN;

	typedef struct _RESPONSE_CHECKIN
	{
		int		nResult;
		bool	bAlreadyUsedPCCafeIP;
		char	szErrMsg[eMAX_ERROR_MSG + 1];

		int		nBillType;
		int		nProductID;
		int64	nRemainMinutes;
		char	szCRM[eMAX_CRM + 1];
		char	szEndDate[eMAX_TIME + 1];
	} RESPONSE_CHECKIN, *LPRESPONSE_CHECKIN;

	typedef struct _RESPONSE_CHECKIN_EX
	{
		int				nResult;
		bool			bAlreadyUsedPCCafeIP;
		char			szErrMsg[eMAX_ERROR_MSG + 1];

		int				nBillType;
		int				nProductID;
		int64			nRemainMinutes;
		char			szCRM[eMAX_CRM + 1];
		char			szEndDate[eMAX_TIME + 1];
		DATA_HANDLE_EX	handle;
	} RESPONSE_CHECKIN_EX, *LPRESPONSE_CHECKIN_EX;

	// üũ�ƿ�
	typedef struct _REQUEST_CHECKOUT
	{
		char	szUserID[eMAX_USER_ID + 1];
	} REQUEST_CHECKOUT, *LPREQUEST_CHECKOUT;

	typedef struct _RESPONSE_CHECKOUT
	{
		bool	bIsOK;
		char	szErrMsg[eMAX_ERROR_MSG + 1];
	} RESPONSE_CHECKOUT, *LPRESPONSE_CHECKOUT;

	// ����� �̵�(to/from)
	typedef struct _REQUEST_USERMOVE_TO
	{
		char	szUserID[eMAX_USER_ID + 1];
		char	szUserIP[eMAX_USER_IP + 1];
	} REQUEST_USERMOVE_TO, *LPREQUEST_USERMOVE_TO, REQUEST_USERMOVE_FROM, *LPREQUEST_USERMOVE_FROM;

	typedef struct _RESPONSE_USERMOVE_TO
	{
		bool	bIsOK;
		char	szErrMsg[eMAX_ERROR_MSG + 1];
	} RESPONSE_USERMOVE_TO, *LPRESPONSE_USERMOVE_TO, RESPONSE_USERMOVE_FROM, *LPRESPONSE_USERMOVE_FROM;

	// ��ǰ�� ��ȸ
	typedef struct _REQUEST_BILL_INFO
	{
		char	szUserID[eMAX_USER_ID + 1];
		char	szUserIP[eMAX_USER_IP + 1];
	} REQUEST_BILL_INFO, *LPREQUEST_BILL_INFO;

	typedef struct _RESPONSE_BILL_INFO
	{
		int		nResult;
		bool	bAlreadyUsedPCCafeIP;
		char	szErrMsg[eMAX_ERROR_MSG + 1];

		int		nBillType;
		int		nProductID;
		int64	nRemainMinutes;
		char	szCRM[eMAX_CRM + 1];
		char	szEndDate[eMAX_TIME + 1];
		bool	bPCBang;
	} RESPONSE_BILL_INFO, *LPRESPONSE_BILL_INFO;

	typedef struct _RESPONSE_BILL_INFO_EX
	{
		int				nResult;
		bool			bAlreadyUsedPCCafeIP;
		char			szErrMsg[eMAX_ERROR_MSG + 1];

		int				nBillType;
		int				nProductID;
		int64			nRemainMinutes;
		char			szCRM[eMAX_CRM + 1];
		char			szEndDate[eMAX_TIME + 1];
		DATA_HANDLE_EX	handle;
	} RESPONSE_BILL_INFO_EX, *LPRESPONSE_BILL_INFO_EX;

	// ����(for debugging)
	typedef struct _RESPONSE_CHARGE
	{
		bool	bIsOK;
		char	szErrMsg[eMAX_ERROR_MSG + 1];
	} RESPONSE_CHARGE, *LPRESPONSE_CHARGE;
	//--------------------------------------------------------------------

	//====================================================================
	// �Լ� ������ Ÿ�� ����
	// ��ǰ�� �̺�Ʈ ����
	typedef int (*FP_NotifyBillingEvent) (const LPBILLING_NOTI_INFO pInfo);

	typedef struct _FUNCTION_POINTER_INFO
	{
		FP_NotifyBillingEvent	fpNotiBillInfo;
	} FUNCTION_POINTER_INFO, *LPFUNCTION_POINTER_INFO;
	//--------------------------------------------------------------------

#if defined(__cplusplus)
	extern "C" {
#endif
	//====================================================================
	// ��� �ʱ�ȭ, ���� ���� API
	DORIAN_EXTERN bool
		__stdcall InitModule(IN GAMESERVER_INFO gameServerInfo, IN FUNCTION_POINTER_INFO fpPointers, OUT char* errorMsg, IN int bufferSize);

	DORIAN_EXTERN void
		__stdcall UninitModule();
	//--------------------------------------------------------------------


	//====================================================================
	// ����/���� ��ǰ�� ���� API
	DORIAN_EXTERN void
		__stdcall Checkin(IN REQUEST_CHECKIN *pRequest, OUT RESPONSE_CHECKIN *pResponse);

	DORIAN_EXTERN void
		__stdcall CheckinEx(IN REQUEST_CHECKIN *pRequest, OUT RESPONSE_CHECKIN_EX *pResponse);

	DORIAN_EXTERN void
		__stdcall Checkout(IN REQUEST_CHECKOUT* pRequest, OUT RESPONSE_CHECKOUT *pResponse);

	DORIAN_EXTERN void
		__stdcall QueryBillingInfo(IN REQUEST_BILL_INFO *pRequest, OUT RESPONSE_BILL_INFO *pResponse);

	DORIAN_EXTERN void
		__stdcall QueryBillingInfoEx(IN REQUEST_BILL_INFO *pRequest, OUT RESPONSE_BILL_INFO_EX *pResponse);

	DORIAN_EXTERN void
		__stdcall UserMoveTo(IN REQUEST_USERMOVE_TO *pRequest, OUT RESPONSE_USERMOVE_TO *pResponse);

	DORIAN_EXTERN void
		__stdcall UserMoveFrom(IN REQUEST_USERMOVE_FROM *pRequest, IN BILL_INFO_FOR_USER_MOVE *pBillInfo, OUT RESPONSE_USERMOVE_FROM *pResponse);

	DORIAN_EXTERN bool
		__stdcall GetBillingInfoForUserMove(IN const char* pszKey, OUT BILL_INFO_FOR_USER_MOVE *pBillInfo);

	DORIAN_EXTERN const char*
		__stdcall GetBillingName(IN const int nPID);

	DORIAN_EXTERN const char*
		__stdcall GetEventName(IN const int nEventCode);

	// for debug
	//////////////////////////////////////////////////////
	DORIAN_EXTERN void
		__stdcall Charge(OUT RESPONSE_CHARGE *pResponse);
	//////////////////////////////////////////////////////
	// for debug
	//--------------------------------------------------------------------

	//====================================================================
	// Data handle ���� API
	DORIAN_EXTERN size_t
		__stdcall GetDataSize(IN DATA_HANDLE_EX handle, OUT char* errorMsg, IN size_t bufferSize);

	DORIAN_EXTERN bool
		__stdcall GetBillingData(IN DATA_HANDLE_EX handle, IN size_t index, OUT BILL_INFO* pBillInfo, OUT char* errorMsg, IN size_t bufferSize);

	DORIAN_EXTERN void
		__stdcall DestroyHandle(IN DATA_HANDLE_EX handle);
	//--------------------------------------------------------------------
#if defined(__cplusplus)
	}
#endif

}//namespace Dorian

#endif // __DORIAN_CONNECTOR_H__
