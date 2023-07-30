#pragma once

#include "ApModule.h"
#include "AuPacket.h"
#include "AgpdBuddy.h"

class AgpmCharacter;
class AgpdCharacter;

enum EnumAgpmBuddyDataType
{
	AGPMBUDDY_DATA_BUDDY,
};

enum EnumAgpmBuddyCallback
{
	AGPMBUDDY_CALLBACK_NONE			= 0,
	AGPMBUDDY_CALLBACK_ADD,
	AGPMBUDDY_CALLBACK_ADD_REQUEST,
	AGPMBUDDY_CALLBACK_ADD_REJECT,
	AGPMBUDDY_CALLBACK_REMOVE,
	AGPMBUDDY_CALLBACK_OPTIONS,
	AGPMBUDDY_CALLBACK_ONLINE,
	AGPMBUDDY_CALLBACK_REGION,
	AGPMBUDDY_CALLBACK_MSG_CODE,
	AGPMBUDDY_CALLBACK_INIT_END,
	AGPMBUDDY_CALLBACK_MAX,
	AGPMBUDDY_CALLBACK_MENTOR_REQUEST,
	AGPMBUDDY_CALLBACK_MENTOR_DELETE,
	AGPMBUDDY_CALLBACK_MENTOR_REQUEST_ACCEPT,
	AGPMBUDDY_CALLBACK_MENTOR_REQUEST_REJECT,
	AGPMBUDDY_CALLBACK_MENTOR_UI_OPEN,
};

enum EnumAgpmBuddyOperation
{
	AGPMBUDDY_OPERATION_NONE		= 0,
	AGPMBUDDY_OPERATION_ADD,					// ���� �߰�
	AGPMBUDDY_OPERATION_ADD_REQUEST,			// ���� �߰� ��û
	AGPMBUDDY_OPERATION_ADD_REJECT,				// ���� �߰� ����
	AGPMBUDDY_OPERATION_REMOVE,					// ���� ����
	AGPMBUDDY_OPERATION_OPTIONS,				// �Ӽ� ������Ʈ
	AGPMBUDDY_OPERATION_ONLINE,					// On/Off line
	AGPMBUDDY_OPERATION_REGION,					// ��ġ ����
	AGPMBUDDY_OPERATION_MSG_CODE,				// message code
	AGPMBUDDY_OPERATION_INIT_END,				// �ʱ�ȭ ���� ���� �Ϸ�
	//�����δ� ģ���ʴ� �ý��� A - �ʴ��ϴ� ĳ����(Mentor�� ��Ī) B - �ʴ�޴� ĳ����(Mentee�� ��Ī)
	AGPMBUDDY_OPERATION_MENTOR_REQUEST,			// Mentor�� �Ǳ⸦ ��û
	AGPMBUDDY_OPERATION_MENTOR_DELETE,			// Mentor ����
	AGPMBUDDY_OPRRATION_MENTOR_REQUEST_ACCEPT,	// ��û ����
	AGPMBUDDY_OPRRATION_MENTOR_REQUEST_REJECT,	// ��û �ź�
	AGPMBUDDY_OPRRATION_MENTOR_UI_OPEN,			// ����â ����.
	AGPMBUDDY_OPERATION_MAX,
};

enum EnumAgpmBuddyMsgCode
{
	AGPMBUDDY_MSG_CODE_NONE		= 0,
	AGPMBUDDY_MSG_CODE_ALREADY_EXIST,		// �̹� ��ϵǾ� �ִ� ĳ���͸� ����Ϸ��� �Ҷ�
	AGPMBUDDY_MSG_CODE_OFFLINE_OR_NOTEXIST,	// �������� �̰ų� �������� �ʴ� ĳ����
	AGPMBUDDY_MSG_CODE_INVALID_USER,		// ����� �� ���� ĳ����
	AGPMBUDDY_MSG_CODE_REFUSE,				// ���� ���� �޽���
	// �����δ� ģ���ʴ� �ý��� �޽���
	AGPMBUDDY_MSG_CODE_INVALID_MENTOR_LEVEL,	// ������ Mentor���� �������� ��� ���
	AGPMBUDDY_MSG_CODE_INVALID_MENTEE_LEVEL,	// ������ Mentee���� �������� ��� ���
	AGPMBUDDY_MSG_CODE_FULL_MENTEE_NUMBER,		// Mentee���ڰ� ��ã�� ���
	AGPMBUDDY_MSG_CODE_ALREADY_EXIST_MENTOR,	// �̹� Mentor�� ������ ���
	AGPMBUDDY_MSG_CODE_RECEIVER_IS_MENTOR,		// ���ڰ� �� ĳ���� �̹� Mentor�� ���.
	AGPMBUDDY_MSG_CODE_REQUESTOR_IS_MENTEE,		// ��û�ڰ� �ٸ����� Mentee�� ���
	AGPMBUDDY_MSG_CODE_ALREADY_REQUEST,			// ���ϴ� ���μ����� �̹� ��û�� ���
	AGPMBUDDY_MSG_CODE_INVALID_PROCESS,			// �߸��� ��û.
	AGPMBUDDY_MSG_CODE_NOT_DELETE_BECAUSE_MENTOR,// ģ������ ��û�� Mentor����� ���� ���� �Ұ����ϴ�.
	AGPMBUDDY_MSG_CODE_NOT_INVITE_BUDDY,		// ��û �Ұ����� �������� �˸���.
};

class AgpmBuddy : public ApModule
{
private:
	AgpmCharacter	*m_pcsAgpmCharacter;
	INT32			m_lIndexAttachData;
	AgpdBuddyConstrict m_stBuddyConstrict;

public:
	AuPacket		m_csPacket;

private:
	PVOID MakePacketBuddy(INT16 *pnPacketLength, EnumAgpmBuddyOperation eOperation, INT32 lCID, 
							AgpdBuddyElement *pcsBuddyElement, INT32 lRegionIndex, EnumAgpmBuddyMsgCode eMsgCode);

	// ApSafeArray���� operator & �� ������� ���ϰ� �����߱� ������ ���۷����� �Լ�ȣ���� �Ѵ�.
	BOOL _AddBuddy(ApVectorBuddy &rVector, AgpdBuddyElement *pcsBuddyElement);
	BOOL _RemoveBuddy(ApVectorBuddy &rVector, AgpdBuddyElement *pcsBuddyElement);
	BOOL _OptionsBuddy(ApVectorBuddy &rVector, AgpdBuddyElement *pcsBuddyElement);
	BOOL _MentorBuddy(ApVectorBuddy &rVector, AgpdBuddyElement *pcsBuddyElement);

public:
	AgpmBuddy();
	virtual ~AgpmBuddy();

	virtual BOOL OnAddModule();
	virtual BOOL OnInit();
	virtual BOOL OnIdle2(UINT32 ulClockCount);
	virtual BOOL OnDestroy();
	virtual BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	static BOOL AgpdBuddyConstructor(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL AgpdBuddyDestructor(PVOID pData, PVOID pClass, PVOID pCustData);

	AgpdBuddyADChar* GetAttachAgpdBuddyData(AgpdCharacter *pcsCharacter);

	BOOL ReadMentorConstrinctFile(CHAR* szFileName);

	PVOID MakePacketAddBuddy(INT16 *pnPacketLength, AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement);
	PVOID MakePacketAddRequest(INT16 *pnPacketLength, AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement);
	PVOID MakePacketAddReject(INT16 *pnPacketLength, AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement);
	PVOID MakePacketRemoveBuddy(INT16 *pnPacketLength, AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement);
	PVOID MakePacketUpdateOptions(INT16 *pnPacketLength, AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement);
	PVOID MakePacketOnline(INT16 *pnPacketLength, AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement);
	PVOID MakePacketMsgCode(INT16 *pnPacketLength, AgpdCharacter *pcsCharacter, 
							AgpdBuddyElement* pcsBuddyElement, EnumAgpmBuddyMsgCode eMsgCode);
	PVOID MakePacketRegion(INT16 *pnPacketLength, AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement, INT32 lRegionIndex);
	PVOID MakePacketInitEnd(INT16 *pnPacketLength, AgpdCharacter *pcsCharacter);

	//Mentor ��Ŷ
	PVOID MakePacketMentorRequest(INT16 *pnPacketLength, AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement);
	PVOID MakePacketMentorDelete(INT16 *pnPacketLength, AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement);
	PVOID MakePacketMentorRequestAccept(INT16 *pnPacketLength, AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement);
	PVOID MakePacketMentorRequestReject(INT16 *pnPacketLength, AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement);
	PVOID MakePacketMentorUIOpen(INT16 *pnPacketLength, AgpdCharacter *pcsCharacter);

	BOOL SetCallbackAdd(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackAddRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackAddReject(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackRemove(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackOptions(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackOnline(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackMsgCode(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackRegion(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackInitEnd(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackMentorRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackMentorDelete(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackMentorRequestAccept(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackMentorRequestReject(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackMentorUIOpen(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL AddBuddy(AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement);
	BOOL RemoveBuddy(AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement);
	BOOL OptionsBuddy(AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement);
	BOOL MentorBuddy(AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement);
	BOOL SetOnline(AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement);

	BOOL IsExistBuddy(AgpdCharacter *pcsCharacter, CHAR *szName, BOOL bCheckBuddy = TRUE, BOOL bCheckBan = TRUE);
	BOOL IsAlreadyMentor(AgpdCharacter *pcsCharacter);
	BOOL IsAlreadyMentee(AgpdCharacter *pcsCharacter);
	BOOL OverMenteeNumber(AgpdCharacter *pcsCharacter);
	BOOL UnderMentorLevel(AgpdCharacter *pcsCharacter);
	BOOL OverMenteeLevel(AgpdCharacter *pcsCharacter);
	INT32 GetMentorStatus(AgpdCharacter *pcsCharacter, CHAR *szName);
	INT32 GetMentorCID(AgpdCharacter *pcsCharacter);
};