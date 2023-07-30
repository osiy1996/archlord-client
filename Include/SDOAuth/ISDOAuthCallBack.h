#ifndef _I_SDO_AUTH_CALLBACK_H_
#define _I_SDO_AUTH_CALLBACK_H_

#if defined(_WIN32) || defined(_WIN64)
#define SNDACALL			__stdcall
#else
#define SNDACALL
#endif

#ifndef interface
#define interface	struct
#endif

#define SNDACALLBACK		SNDACALL
#define SNDAMETHOD(Type)	virtual Type SNDACALL
#define SNDAAPI(Type)		extern "C" Type SNDACALL


typedef struct stKeyValue
{
	int nKey;
	char *pValue;
}SKeyValue;

typedef struct stSDOAuthCallBack
{
	void (SNDACALLBACK *GetUserInfoCallBack)(unsigned int dwRpcId,int nResult, SKeyValue *pKeyValue, int nKeyValueNum);
	
}SSDOAuthCallBack;

#endif

