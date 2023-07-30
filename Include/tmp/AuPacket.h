/******************************************************************************
Module:  AuPacket.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 08. 22
******************************************************************************/

#if !defined(__AUPACKET_H__)
#define __AUPACKET_H__

#include "ApBase.h"
#include "AuType.h"
#include "AuCircularBuffer.h"
#include "AuMiniLZO.h"

#include "AuCryptManager.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AuPacketD" )
#else
#pragma comment ( lib , "AuPacket" )
#endif
#endif

extern AuCircularBuffer g_AuCircularBuffer;
extern AuCircularBuffer g_AuCircularOutBuffer;

class AuPacket {
private:
	AuMiniLZO	m_MiniLZO;
	UINT16		m_cFieldType[64];		// ��Ŷ�ȿ� �ִ� �ʵ���� Ÿ��
	UINT16		m_cFieldLength[64];		// ��Ŷ�ȿ� �ִ� �ʵ���� ����
	UINT16		m_cNumField;
	BOOL		m_bIsSetField;			// SetFieldType()�� �����ߴ��� ����
	INT16		m_nFlagLength;			// m_dwFlag ����

public:
	AuPacket();
	~AuPacket();

	//Packet�� �����Ѵ�.
	INT16 SetFieldType(UINT16 cFirst, ...);
	BOOL SetFlagLength(INT16 nLength);

	INT16 GetField(BOOL bIsPacket, PVOID pvPacket, INT16 nPacketLength, PVOID pvFirst, ...);

	//���� Packet�� Flag�� return �Ѵ�. (�ִ� ���̴� unsigned long long �� 8byte�̴�.)
	//DWORD GetFlag();

	//Packet�� �����Ѵ�.
	//INT16 MakeField(PVOID pvFirst, ...);
	PVOID MakePacket(BOOL bIsPacket, INT16 *pnPacketLength, UINT8 cType, ...);
	PVOID MakeStaticPacket(BOOL bIsPacket, INT16 *pnPacketLength, UINT8 cType, ...);

	//��Ŷ�� �����´�
	//PVOID GetPacket();
	//INT16 GetPacketLength();
	INT16 GetFlagLength();

	BOOL FreePacket(PVOID pvPacket, BOOL bEmbeddedPacket = TRUE);
	BOOL FreeStaticPacket(PVOID pvPacket, BOOL bEmbeddedPacket = TRUE);

	BOOL SetCID(PVOID pvPacket, INT16 nPacketLength, INT32 lCID);

	static AuCryptManager& GetCryptManager();
	static PVOID EncryptPacket(PVOID pInput, INT16 nSize, INT16* pnOutputSize, AuCryptActor& csCryptActor, eAuCryptType eCryptType = AUCRYPT_TYPE_PUBLIC);
	static PVOID DecryptPacket(PVOID pInput, INT16 nSize, INT16* pnOutputSize, AuCryptActor& csCryptActor, eAuCryptType eCryptType = AUCRYPT_TYPE_PRIVATE);
	static void FreeEncryptBuffer(PVOID pBuffer, INT16 nSize);

	static BOOL SetDummy(PVOID pvPacket, INT16 nSize);
};

#endif //__AUPACKET_H__
