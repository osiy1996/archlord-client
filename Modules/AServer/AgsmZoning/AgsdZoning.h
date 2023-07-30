#ifndef	__AGSDZONING_H__
#define	__AGSDZONING_H__

#include "ApBase.h"

typedef enum _ZONING_STATUS {
	AGSMZONING_NOT_ZONING					= 0,
	AGSMZONING_ZONING_START_ZONING,					// ������ �����Ѵ�.
	AGSMZONING_ZONING_PROCESS_ZONING,				// ������ ���������� �����ؼ� ó�����̴�.
	AGSMZONING_ZONING_PASSCONTROL,					// character control�� �ѱ��.
	AGSMZONING_ZONING_SUCCESS_PASSCONTROL,			// character control�� �Ѿ��.
	AGSMZONING_ZONING_END_ZONING
} ZONING_STATUS;

typedef struct _stZONING_STATUS {
	INT32		lServerID;
	INT16		fZoningStatus;
	BOOL		bConnectZoneServer;
	BOOL		bTeleport;
} stZoningStatus;


class AgsdZoning {
public:
	BOOL				m_bZoning;				// ���� �ϰ� �ִ��� ����
//	stZoningStatus		m_stZoningStatus[5];	// ���� ����....
	ApSafeArray<stZoningStatus, 5>	m_stZoningStatus;

	BOOL				m_bDisconnectZoning;	// ���� �������� disconnect �� �߻��ϴ� ��� �̰��� TRUE�� �������ش�.
												// �̰��� TRUE�̸� �� �������� character ����Ÿ�� ��� ������Ų��.
};

#endif	//__AGSDZONING_H__