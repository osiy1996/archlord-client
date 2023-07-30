//	AgpdChatting.h
////////////////////////////////////////////////////////////////

#ifndef	__AGPDCHATTING_H__
#define	__AGPDCHATTING_H__

#include <ApBase/ApBase.h>


struct AgpdChatCommand
{
	CHAR *						m_szCommand;
	PVOID						m_pvClass;
	ApModuleDefaultCallBack		m_fnCallback;

	AgpdChatCommand()
	{
		m_szCommand		= NULL;
		m_pvClass		= NULL;
		m_fnCallback	= NULL;
	}
};

class AgpdChattingADChar
{
public:
	INT32	m_lLastWhisperSenderID;			// ���������� �ӼӸ��� �������
	
	BOOL	m_bIsBlockWhisper;				// �ӼӸ� ���� ����
};

#endif	//__AGPDCHATTING_H__
