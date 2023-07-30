#ifndef	__AGPD_EVENT_NPCDIALOG_H__
#define __AGPD_EVENT_NPCDIALOG_H__

#include <ApBase/ApBase.h>

enum eAgpdEventNPCDialogType
{
	AGPM_NPCDIALOG_TYPE_MOB = 0,
	AGPM_NPCDIALOG_TYPE_NPC
};

enum eAgpdEventNPCDialogNPC
{
	AGPD_NPCDIALOG_NPC_NORMAL = 0, //NPC�� ������ ������ �Ϲ����� ��ȭ. ����� NORMAL�� ����.
	AGPD_NPCDIALOG_NPC_MAX
};

enum eAgpdEventNPCDialogMob
{
	AGPD_NPCDIALOG_MOB_COMMON = 0, //NPC�� ������ ������ �Ϲ����� ��ȭ.
	AGPD_NPCDIALOG_MOB_WANDERING,
	AGPD_NPCDIALOG_MOB_ATTACK,
/*	AGPD_NPCDIALOG_MOB_TARGET_PC,
	AGPD_NPCDIALOG_MOB_DIE,
	AGPD_NPCDIALOG_MOB_SPAWN,
	AGPD_NPCDIALOG_MOB_CHASE,
	AGPD_NPCDIALOG_MOB_PC_MISS,*/ //2004.10.25�� ��� ���յǸ鼭 �������.
	AGPD_NPCDIALOG_MOB_MAX
};

class AgpdEventMobDialogKeyTemplate
{
public:
	INT32			m_lMobTID;	//DialogID�� Ű���� MOB�� TID�̴�.
	INT32			m_lDialogID;
};

class AgpdEventMobDialogData
{
public:
	int				m_iDialogTextNum;
	char			**m_ppstrDialogText;
	float			m_fProbable;
};

class AgpdEventMobDialogTemplate
{
public:
	//AgpdEventMobDialogData m_csEventMobDialog[AGPD_NPCDIALOG_MOB_MAX];
	ApSafeArray<AgpdEventMobDialogData, AGPD_NPCDIALOG_MOB_MAX>	m_csEventMobDialog;

	INT32			m_lMobDialogTID;
	char			m_strTemplateName[80];
};

//����� ��ȭ�� �ؽ�Ʈ�̿��� �����ʹ� ����.
class AgpdEventNPCDialogData
{
public:
	INT32			m_lNPCDialogTextID;
};

#endif