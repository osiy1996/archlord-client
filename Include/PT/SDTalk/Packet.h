#pragma once

class CPacket
{
public:
	CPacket();
	~CPacket();

	int				GetLength();
	unsigned char *	GetBuf(long size = -1);
	
	int				GetWholeLength();
	unsigned char * GetWholeBuf();

	void			CopyBuf(const char * buf);

	static bool		CheckEnough(const char * buf, int length);
	
protected:
	long			m_Length;
	unsigned char * m_pBuf;

	static long ntohl(long a);
	static short ntohs(short a);
};