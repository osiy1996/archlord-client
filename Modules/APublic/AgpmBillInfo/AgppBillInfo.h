#ifndef _APPP_BILLINFO_H
#define _APPP_BILLINFO_H

struct PACKET_BILLINGINFO : public PACKET_HEADER
{
	CHAR	Flag1;

	INT8	pcOperation;
	INT32	lCID;

	PACKET_BILLINGINFO()
		: pcOperation(0), Flag1(1), lCID(AP_INVALID_CID)
	{
		cType			= AGPMBILLINFO_PACKET_TYPE;
		unPacketLength	= (UINT16)sizeof(PACKET_BILLINGINFO);
	}
};

// S -> C
struct PACKET_BILLINGINFO_CASHINFO : public PACKET_BILLINGINFO
{
	INT64 m_pWCoin;
	INT64 m_pPCoin;

	PACKET_BILLINGINFO_CASHINFO(INT32 CID, INT64 WCoin, INT64 PCoin)
	{
		pcOperation		= AGPMBILL_OPERATION_CASHINFO;
		unPacketLength	= (UINT16)sizeof(PACKET_BILLINGINFO_CASHINFO);

		lCID = CID;
		m_pWCoin = WCoin;
		m_pPCoin = PCoin;
	}
};

// LS -> GS
struct PACKET_BILLINGINFO_GUID : public PACKET_BILLINGINFO
{
	DWORD m_dwAccountGUID;

	PACKET_BILLINGINFO_GUID(INT32 CID, DWORD AccountGUID)
	{
		pcOperation		= AGPMBILL_OPERATION_GUID;
		unPacketLength	= (UINT16)sizeof(PACKET_BILLINGINFO_GUID);

		lCID = CID;
		m_dwAccountGUID = AccountGUID;
	}
};

#endif //_APPP_BILLINFO_H