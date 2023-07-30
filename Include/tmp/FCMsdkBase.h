#ifndef  _FCMSDK_H_
#define _FCMSDK_H_

typedef int (*funcPtr)(const char* szUserID, const char* szRoleID,int iOnlineMinutes,int iOfflineMinutes);
typedef void (*funcPtrSwitch)(int needFCM); //needFCM = 1需要防沉迷 ，needFCM = 0不需要防沉迷
typedef void (*QueryFuncPtr)(const char* szUserID, const char* szRoleID,int iState ,int iOnlineMinutes,int iOfflineMinutes);

class CFCMSDKBase
{
public:
	static CFCMSDKBase * __stdcall GetInInstance();
	//static void ReleaseInstance();
	virtual bool __stdcall initFCMSDK(unsigned short gametype,unsigned short areacode,unsigned short serverCode,funcPtr func_callbk, funcPtrSwitch func_switch, QueryFuncPtr func_cbquery);
	virtual bool __stdcall initFCMSDKEASY(funcPtr func_callbk, funcPtrSwitch func_switch, QueryFuncPtr func_cbquery);

	virtual int __stdcall FCMQueryState(const char* szUserID, const char* szRoleID,const char *IPAddr,int *OnlineTimeVal,int *OfflineTimeVal);
	virtual int __stdcall FCMQueryStateNoBlock(const char* szUserID, const char* szRoleID,const char *IPAddr);

	virtual int __stdcall FCMIDOnline(const char* szUserID, const char* szRoleID,const char *IPAddr);
	virtual int __stdcall FCMIDOffline(const char* szUserID, const char* szRoleID,const char *IPAddr);
	virtual int __stdcall FCMIDOnlineGS(const char* szUserID, const char* szRoleID,const char *IPAddr);
	virtual int __stdcall FCMIDOfflineGS(const char* szUserID, const char* szRoleID,const char *IPAddr);

private:
	CFCMSDKBase();
	~CFCMSDKBase(void);
};

CFCMSDKBase * __stdcall CreateSDKInstance(void);
#endif