#ifndef _AGPP_GUILD_H
#define _AGPP_GUILD_H

enum eWorldChampionshipPacketType
{
	AGPMGUILD_PACKET_WORLD_CHAMPIONSHIP_REQUEST = 1,
	AGPMGUILD_PACKET_WORLD_CHAMPIONSHIP_ENTER,
	AGPMGUILD_PACKET_WORLD_CHAMPIONSHIP_RESULT,
};

struct PACKET_AGPP_GUILD : public PACKET_HEADER
{
	CHAR	Flag1;
	CHAR	Flag2;
	CHAR	Flag3;
	CHAR	Flag4;

	INT8	pcOperation;

	PACKET_AGPP_GUILD()
		: pcOperation(0), Flag1(1), Flag2(0), Flag3(0), Flag4(0)
	{
		cType			= AGPMGUILD_PACKET_TYPE;
		unPacketLength	= (UINT16)sizeof(PACKET_AGPP_GUILD);
	}
};

struct PACKET_GUILD_WORLDCHAMPIONSHIP : public PACKET_AGPP_GUILD
{
	INT8	pcOperation2;

	PACKET_GUILD_WORLDCHAMPIONSHIP()
	{
		pcOperation		= AGPMGUILD_PACKET_WORLD_CHAMPIONSHIP;
		unPacketLength	= (UINT16)sizeof(PACKET_GUILD_WORLDCHAMPIONSHIP);
	}
};

struct PACKET_GUILD_WORLDCHAMPIONSHIP_REQUEST : public PACKET_GUILD_WORLDCHAMPIONSHIP
{
	INT32 lCID;

	PACKET_GUILD_WORLDCHAMPIONSHIP_REQUEST(INT32 _lCID)
	{
		pcOperation2	= AGPMGUILD_PACKET_WORLD_CHAMPIONSHIP_REQUEST;
		unPacketLength	= (UINT16)sizeof(PACKET_GUILD_WORLDCHAMPIONSHIP_REQUEST);

		lCID = _lCID;
	}
};

struct PACKET_GUILD_WORLDCHAMPIONSHIP_ENTER : public PACKET_GUILD_WORLDCHAMPIONSHIP
{
	INT32 lCID;

	PACKET_GUILD_WORLDCHAMPIONSHIP_ENTER(INT32 _lCID)
	{
		pcOperation2	= AGPMGUILD_PACKET_WORLD_CHAMPIONSHIP_ENTER;
		unPacketLength	= (UINT16)sizeof(PACKET_GUILD_WORLDCHAMPIONSHIP_ENTER);

		lCID = _lCID;
	}
};

struct PACKET_GUILD_WORLDCHAMPIONSHIP_RESULT : public PACKET_GUILD_WORLDCHAMPIONSHIP
{
	CHAR strMessage[512];

	PACKET_GUILD_WORLDCHAMPIONSHIP_RESULT()
	{
		pcOperation2	= AGPMGUILD_PACKET_WORLD_CHAMPIONSHIP_RESULT;
		unPacketLength	= (UINT16)sizeof(PACKET_GUILD_WORLDCHAMPIONSHIP_RESULT);

		memset(strMessage, 0, sizeof(strMessage));
	}
};

//////////////////////////////////////////////////////////////////////////
// 
enum eClassSocietySystemPacketType
{
	AGPMGUILD_CLASS_SOCIETY_SYSTEM_MESSAGE = 1,
	AGPMGUILD_CLASS_SOCIETY_APPOINTMENT_REQUEST,
	AGPMGUILD_CLASS_SOCIETY_APPOINTMENT_ASK,
	AGPMGUILD_CLASS_SOCIETY_APPOINTMENT_ANSWER,
	AGPMGUILD_CLASS_SOCIETY_SUCCESSION_REQUEST,
	AGPMGUILD_CLASS_SOCIETY_SUCCESSION_ASK,
	AGPMGUILD_CLASS_SOCIETY_SUCCESSION_ANSWER,
	AGPMGUILD_CLASS_SOCIETY_UPDATE_GUILD_MEMBER_RANK,
};

struct PACKET_GUILD_CLASS_SOCIETY_SYSTEM : public PACKET_AGPP_GUILD
{
	INT8	pcOperation2;

	PACKET_GUILD_CLASS_SOCIETY_SYSTEM()
	{
		pcOperation		= AGPMGUILD_PACKET_CLASS_SOCIETY_SYSTEM;
		unPacketLength	= (UINT16)sizeof(PACKET_GUILD_CLASS_SOCIETY_SYSTEM);
	}
};

struct PACKET_GUILD_CLASS_SOCIETY_SYSTEM_MESSAGE : public PACKET_GUILD_CLASS_SOCIETY_SYSTEM
{
	CHAR strMessage[512];

	PACKET_GUILD_CLASS_SOCIETY_SYSTEM_MESSAGE(CHAR* _strMessage)
	{
		pcOperation2	= AGPMGUILD_CLASS_SOCIETY_SYSTEM_MESSAGE;
		unPacketLength	= (UINT16)sizeof(PACKET_GUILD_CLASS_SOCIETY_SYSTEM_MESSAGE);

		memset(strMessage, 0, sizeof(strMessage));

		strcpy(strMessage, _strMessage);
	}
};

struct PACKET_GUILD_CLASS_SOCIETY_APPOINTMENT_REQUEST : public PACKET_GUILD_CLASS_SOCIETY_SYSTEM
{
	CHAR strSChar[AGPACHARACTER_MAX_ID_STRING+1];
	CHAR strTChar[AGPACHARACTER_MAX_ID_STRING+1];
	INT32 Rank;

	PACKET_GUILD_CLASS_SOCIETY_APPOINTMENT_REQUEST(CHAR* _strSChar, CHAR* _strTChar, INT32 _Rank)
	{
		pcOperation2	= AGPMGUILD_CLASS_SOCIETY_APPOINTMENT_REQUEST;
		unPacketLength	= (UINT16)sizeof(PACKET_GUILD_CLASS_SOCIETY_APPOINTMENT_REQUEST);

		memset(strSChar, 0, sizeof(strSChar));
		memset(strTChar, 0, sizeof(strTChar));

		strcpy(strSChar, _strSChar);
		strcpy(strTChar, _strTChar);
		Rank = _Rank;
	}
};

struct PACKET_GUILD_CLASS_SOCIETY_APPOINTMENT_ASK : public PACKET_GUILD_CLASS_SOCIETY_SYSTEM
{
	CHAR strMasterID[AGPACHARACTER_MAX_ID_STRING+1];
	CHAR strGuildID[AGPMGUILD_MAX_GUILD_ID_LENGTH+1];
	CHAR strChar[AGPACHARACTER_MAX_ID_STRING+1];
	INT32 Rank;

	PACKET_GUILD_CLASS_SOCIETY_APPOINTMENT_ASK(CHAR* _strMasterID, CHAR* _strGuildID, CHAR* _strChar, INT32 _Rank)
	{
		pcOperation2	= AGPMGUILD_CLASS_SOCIETY_APPOINTMENT_ASK;
		unPacketLength	= (UINT16)sizeof(PACKET_GUILD_CLASS_SOCIETY_APPOINTMENT_ASK);

		memset(strMasterID, 0, sizeof(strMasterID));
		memset(strGuildID, 0, sizeof(strGuildID));
		memset(strChar, 0, sizeof(strChar));

		strcpy(strMasterID, _strMasterID);
		strcpy(strGuildID, _strGuildID);
		strcpy(strChar, _strChar);
		Rank = _Rank;
	}
};

struct PACKET_GUILD_CLASS_SOCIETY_APPOINTMENT_ANSWER : public PACKET_GUILD_CLASS_SOCIETY_SYSTEM
{
	CHAR strMasterID[AGPACHARACTER_MAX_ID_STRING+1];
	CHAR strChar[AGPACHARACTER_MAX_ID_STRING+1];
	INT32 Rank;
	INT32 Answer;

	PACKET_GUILD_CLASS_SOCIETY_APPOINTMENT_ANSWER(CHAR* _strMasterID, CHAR* _strChar, INT32 _Rank, INT32 _Answer)
	{
		pcOperation2	= AGPMGUILD_CLASS_SOCIETY_APPOINTMENT_ANSWER;
		unPacketLength	= (UINT16)sizeof(PACKET_GUILD_CLASS_SOCIETY_APPOINTMENT_ANSWER);

		memset(strMasterID, 0, sizeof(strMasterID));
		memset(strChar, 0, sizeof(strChar));

		strcpy(strMasterID, _strMasterID);
		strcpy(strChar, _strChar);
		Rank = _Rank;
		Answer = _Answer;
	}
};

struct PACKET_GUILD_CLASS_SOCIETY_SUCCESSION_REQUEST : public PACKET_GUILD_CLASS_SOCIETY_SYSTEM
{
	CHAR strSChar[AGPACHARACTER_MAX_ID_STRING+1];
	CHAR strTChar[AGPACHARACTER_MAX_ID_STRING+1];
	
	PACKET_GUILD_CLASS_SOCIETY_SUCCESSION_REQUEST(CHAR* _strSChar, CHAR* _strTChar)
	{
		pcOperation2	= AGPMGUILD_CLASS_SOCIETY_SUCCESSION_REQUEST;
		unPacketLength  = (UINT16)sizeof(PACKET_GUILD_CLASS_SOCIETY_SUCCESSION_REQUEST);

		ZeroMemory(strSChar, sizeof(strSChar));
		ZeroMemory(strTChar, sizeof(strTChar));

		strcpy(strSChar, _strSChar);
		strcpy(strTChar, _strTChar);
	}
};

struct PACKET_GUILD_CLASS_SOCIETY_SUCCESSION_ASK : public PACKET_GUILD_CLASS_SOCIETY_SYSTEM
{
	CHAR strMasterID[AGPACHARACTER_MAX_ID_STRING+1];
	CHAR strGuildID[AGPMGUILD_MAX_GUILD_ID_LENGTH+1];
	CHAR strChar[AGPACHARACTER_MAX_ID_STRING+1];

	PACKET_GUILD_CLASS_SOCIETY_SUCCESSION_ASK(CHAR* _strMasterID, CHAR* _strGuildID, CHAR* _strChar)
	{
		pcOperation2	= AGPMGUILD_CLASS_SOCIETY_SUCCESSION_ASK;
		unPacketLength	= (UINT16)sizeof(PACKET_GUILD_CLASS_SOCIETY_SUCCESSION_ASK);

		ZeroMemory(strMasterID, sizeof(strMasterID));
		ZeroMemory(strGuildID, sizeof(strGuildID));
		ZeroMemory(strChar, sizeof(strChar));

		strcpy(strMasterID, _strMasterID);
		strcpy(strGuildID, _strGuildID);
		strcpy(strChar, _strChar);
	}
};

struct PACKET_GUILD_CLASS_SOCIETY_SUCCESSION_ANSWER : public PACKET_GUILD_CLASS_SOCIETY_SYSTEM
{
	CHAR strMasterID[AGPACHARACTER_MAX_ID_STRING+1];
	CHAR strChar[AGPACHARACTER_MAX_ID_STRING+1];
	INT32 Answer;

	PACKET_GUILD_CLASS_SOCIETY_SUCCESSION_ANSWER(CHAR* _strMasterID, CHAR* _strChar, INT32 _Answer)
	{
		pcOperation2	= AGPMGUILD_CLASS_SOCIETY_SUCCESSION_ANSWER;
		unPacketLength	= (UINT16)sizeof(PACKET_GUILD_CLASS_SOCIETY_SUCCESSION_ANSWER);

		ZeroMemory(strMasterID, sizeof(strMasterID));
		ZeroMemory(strChar, sizeof(strChar));

		strcpy(strMasterID, _strMasterID);
		strcpy(strChar, _strChar);
		Answer = _Answer;
	}
};

struct PACKET_GUILD_CLASS_SOCIETY_UPDATE_GUILD_MEMBER_RANK : public PACKET_GUILD_CLASS_SOCIETY_SYSTEM
{
	CHAR strGuildID[AGPMGUILD_MAX_GUILD_ID_LENGTH+1];
	CHAR strCCharID[AGPACHARACTER_MAX_ID_STRING+1];
	INT32 Rank;

	PACKET_GUILD_CLASS_SOCIETY_UPDATE_GUILD_MEMBER_RANK(CHAR* _strGuildID, CHAR* _strCCharID, INT32 _Rank)
	{
		pcOperation2	= AGPMGUILD_CLASS_SOCIETY_UPDATE_GUILD_MEMBER_RANK;
		unPacketLength  = (UINT16)sizeof(PACKET_GUILD_CLASS_SOCIETY_UPDATE_GUILD_MEMBER_RANK);

		ZeroMemory(strGuildID, sizeof(strGuildID));
		ZeroMemory(strCCharID, sizeof(strCCharID));

		strcpy(strGuildID, _strGuildID);
		strcpy(strCCharID, _strCCharID);
		Rank = _Rank;
	}
};

#endif // _AGPP_GUILD_H