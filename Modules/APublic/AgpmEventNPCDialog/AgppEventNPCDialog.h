#ifndef _AGPP_EVENTNPCDIALOG_H
#define _AGPP_EVENTNPCDIALOG_H

#include <ApPacket.h>
#include <AgpmEventNPCDialog/AgpmEventNPCDialog.h>

#define MESSAGE_MAX_SIZE 512

enum eEventNPCDialogGrantPacketType
{
	AGPMEVENTNPCDIALOG_PACKET_GRANT_MESSAGEBOX = 1,
	AGPMEVENTNPCDIALOG_PACKET_GRANT_MENU,
};

enum eEventNPCDialogRequestPacketType
{
	AGPMEVENTNPCDIALOG_PACKET_REQUEST_MESSAGEBOX = 1,
	AGPMEVENTNPCDIALOG_PACKET_REQUEST_MENU,
};

struct PACKET_EVENTNPCDIALOG : public PACKET_HEADER
{
	CHAR	Flag1;

	INT8	pcOperation;

	PACKET_EVENTNPCDIALOG()
		: pcOperation(0), Flag1(1)
	{
		cType			= AGPMEVENT_NPCDIALOG_PACKET_TYPE;
		unPacketLength	= (UINT16)sizeof(PACKET_EVENTNPCDIALOG);
	}
};

struct PACKET_EVENTNPCDIALOG_REQUEST_EX : public PACKET_EVENTNPCDIALOG
{
	INT32	CID, NID, lEvnetIndex, lStep;
	INT8	pcOperation2;

	PACKET_EVENTNPCDIALOG_REQUEST_EX()
		: CID(0), NID(0), lEvnetIndex(0), lStep(0)
	{
		pcOperation = AGPMEVENT_NPCDIALOG_PACKET_OPERATION_REQUEST_EX;
		unPacketLength = (UINT16)sizeof(PACKET_EVENTNPCDIALOG_REQUEST_EX);
	}
};

struct PACKET_EVENTNPCDIALOG_REQUEST_MESSAGEBOX : public PACKET_EVENTNPCDIALOG_REQUEST_EX
{
	BOOL bBtnIndex;
	CHAR strString[ MESSAGE_MAX_SIZE+1 ];

	INT32 nItemID;
	INT32 nItemTID;

	PACKET_EVENTNPCDIALOG_REQUEST_MESSAGEBOX(INT32 _CID, INT32 _NID, INT32 _lEvnetIndex, INT32 _lStep)
	{
		pcOperation2 = AGPMEVENTNPCDIALOG_PACKET_REQUEST_MESSAGEBOX;
		unPacketLength = (UINT16)sizeof(PACKET_EVENTNPCDIALOG_REQUEST_MESSAGEBOX);

		CID = _CID, NID = _NID, lEvnetIndex = _lEvnetIndex, lStep = _lStep;

		bBtnIndex = TRUE;
		memset( strString, 0, sizeof( strString ) );

		nItemID = -1;
		nItemTID = -1;
	}
};

struct PACKET_EVENTNPCDIALOG_REQUEST_MENU : public PACKET_EVENTNPCDIALOG_REQUEST_EX
{
	PACKET_EVENTNPCDIALOG_REQUEST_MENU(INT32 _CID, INT32 _NID, INT32 _lEvnetIndex, INT32 _lStep)
	{
		pcOperation2 = AGPMEVENTNPCDIALOG_PACKET_REQUEST_MENU;
		unPacketLength = (UINT16)sizeof(PACKET_EVENTNPCDIALOG_REQUEST_MENU);

		CID = _CID, NID = _NID, lEvnetIndex = _lEvnetIndex, lStep = _lStep;
	}
};

struct PACKET_EVENTNPCDIALOG_GRANT_EX : public PACKET_EVENTNPCDIALOG
{
	INT32	CID, NID, lEvnetIndex, lStep;
	INT8	pcOperation2;

	PACKET_EVENTNPCDIALOG_GRANT_EX()
		: CID(0), NID(0), lEvnetIndex(0), lStep(0)
	{
		pcOperation = AGPMEVENT_NPCDIALOG_PACKET_OPERATION_GRANT_EX;
		unPacketLength = (UINT16)sizeof(PACKET_EVENTNPCDIALOG_GRANT_EX);
	}
};

struct PACKET_EVENTNPCDIALOG_GRANT_MESSAGEBOX : public PACKET_EVENTNPCDIALOG_GRANT_EX
{
	INT32 lMessageBoxType;
	CHAR strMessage[MESSAGE_MAX_SIZE+1];

	INT32 nItemTID;
	INT32 nSkillTID;

	PACKET_EVENTNPCDIALOG_GRANT_MESSAGEBOX(INT32 _CID, INT32 _NID, INT32 _lEvnetIndex, INT32 _lStep)
	{
		pcOperation2 = AGPMEVENTNPCDIALOG_PACKET_GRANT_MESSAGEBOX;
		unPacketLength = (UINT16)sizeof(PACKET_EVENTNPCDIALOG_GRANT_MESSAGEBOX);

		CID = _CID, NID = _NID, lEvnetIndex = _lEvnetIndex, lStep = _lStep;

		lMessageBoxType = 0;
		memset(strMessage, 0, sizeof(strMessage));

		nItemTID = 0;
		nSkillTID = 0;
	}
};

const int MAX_BUTTON_TEXT_SIZE = 128;
const int MAX_BUTTON_COUNT = 10;

struct PACKET_EVENTNPCDIALOG_GRANT_MENU : public PACKET_EVENTNPCDIALOG_GRANT_EX
{
	struct stButton
	{
		INT32 EID;
		INT32 STEP;
		INT32 ItemTID;
		CHAR strText[MAX_BUTTON_TEXT_SIZE+1];
	};

	INT32 lMenuType;
	CHAR strTitle[MAX_BUTTON_TEXT_SIZE+1];
	CHAR strDialog[MESSAGE_MAX_SIZE+1];

	stButton ButtonArray[MAX_BUTTON_COUNT];

	PACKET_EVENTNPCDIALOG_GRANT_MENU(INT32 _CID, INT32 _NID)
	{
		pcOperation2 = AGPMEVENTNPCDIALOG_PACKET_GRANT_MENU;
		unPacketLength = (UINT16)sizeof(PACKET_EVENTNPCDIALOG_GRANT_MENU);

		CID = _CID, NID = _NID;

		lMenuType = 0;
		memset(strTitle, 0, sizeof(strTitle));
		memset(strDialog, 0, sizeof(strDialog));

		memset(&ButtonArray, 0, sizeof(ButtonArray));
	}
};

#endif // _AGPP_EVENTNPCDIALOG_H