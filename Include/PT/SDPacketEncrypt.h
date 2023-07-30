#pragma once
#include "sdkconfig.h"
#include "packetprocessor.h"
#include "d3wrapper.h"

class PTSDK_STUFF CSDPacketEncrypt :
	public CPacketProcessor, public CD3Wrapper
{
public:
	CSDPacketEncrypt(void);
	~CSDPacketEncrypt(void);

	virtual void DoPacket(CSDPacket & packet);
};
