#ifndef __PACKET_TYPE_CS__
#define __PACKET_TYPE_CS__

#include "ApPacket.h"

#pragma pack(1)

//��ġ ������ ��ġ�� ��û�Ѵ�.
struct CPacketTypeCS_0x00 : public PATCH_PACKET_HEADER
{
public:
	int				m_iVersion;
	int				m_iPatchCheckCode;

	void Init()
	{
		m_iSize = sizeof(CPacketTypeCS_0x00);
		m_iCommand = 0x00;
	}

	CPacketTypeCS_0x00()
	{
		Init();
	}

	CPacketTypeCS_0x00( int iVersion, int iPatchCheckCode )
	{
		Init();

		m_iVersion = iVersion;
		m_iPatchCheckCode = iPatchCheckCode;
	}
};

//������ �޾Ҵٰ� �˷���.
struct CPacketTypeCS_0x03 : public PATCH_PACKET_HEADER
{
public:

	int			m_nPatchVersion;

	void Init()
	{
		m_iSize				= sizeof(CPacketTypeCS_0x03);
		m_iCommand			= 0x03;
		m_nPatchVersion		=	0;
	}

	CPacketTypeCS_0x03()
	{
		Init();
	}
};


//������ �޾Ҵٰ� �˷���.
struct CPacketTypeCS_0x04 : public PATCH_PACKET_HEADER
{
public:

	void Init()
	{
		m_iSize = sizeof(CPacketTypeCS_0x04);
		m_iCommand = 0x04;
	}

	CPacketTypeCS_0x04()
	{
		Init();
	}
};

//���� ���� ������ �޶�~
struct CPacketTypeCS_0x10 : public PATCH_PACKET_HEADER
{
public:

	void Init()
	{
		m_iSize = sizeof(CPacketTypeCS_0x10);
		m_iCommand = 0x10;
	}

	CPacketTypeCS_0x10()
	{
		Init();
	}
};

//���� ���� ������ �޶�~
struct CPacketTypeCS_0x11 : public PATCH_PACKET_HEADER
{
public:

	void Init()
	{
		m_iSize = sizeof(CPacketTypeCS_0x11);
		m_iCommand = 0x11;
	}

	CPacketTypeCS_0x11()
	{
		Init();
	}
};

// �ֽ� Ŭ���̾�Ʈ ���������� CRC �� �ּ���
struct CPacketTypeCS_0x05 : public PATCH_PACKET_HEADER
{
public:
	unsigned long	m_ulClientFileCRC;

	void Init()
	{
		m_iSize = sizeof(CPacketTypeCS_0x05);
		m_iCommand = 0x05;
		m_ulClientFileCRC = 0;
	}

	CPacketTypeCS_0x05()
	{
		Init();
	}
};

struct CPacketTypeCS_0xff : public PATCH_PACKET_HEADER
{
public:
	unsigned int	m_iSeq;

	void Init()
	{
		m_iSize = sizeof(CPacketTypeCS_0xff);
		m_iCommand = 0xff;
		m_iSeq = 0;
	}

	CPacketTypeCS_0xff()
	{
		Init();
	}
};

#pragma pack()

#endif
