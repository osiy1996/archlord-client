// AuMiniLZO.cpp: implementation of the AuMiniLZO class.
//
//////////////////////////////////////////////////////////////////////

#include "AuMiniLZO.h"
#include "minilzo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define ASSERVER_COMPRESS_ERROR "LOG\\AsServer_Compress_Error.log"

AuMiniLZO::AuMiniLZO()
{
	ASSERT(lzo_init() == LZO_E_OK);

	m_lOutSize = sizeof(m_OutMemory);
	::ZeroMemory(m_WorkMemory, sizeof(m_WorkMemory));
	::ZeroMemory(m_OutMemory, sizeof(m_OutMemory));
}

AuMiniLZO::~AuMiniLZO()
{

}

BOOL AuMiniLZO::Compress(BYTE* pInBuffer, UINT16* pInSize)
{
	UINT InSize = *pInSize;
	INT32 lResult;
	
	try
	{
		lResult = lzo1x_1_compress(pInBuffer, InSize, m_OutMemory, (lzo_uint*)&m_lOutSize, &m_WorkMemory);
	}
	catch(...)
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "Compress Error Insize : %d", *pInSize);
		AuLogFile_s(ASSERVER_COMPRESS_ERROR, strCharBuff);
		return FALSE;
	}
	
	if (LZO_E_OK == lResult)
	{
		ASSERT(USHRT_MAX > m_lOutSize);	// m_lOutSize���� 2Byte ���� ũ�� �ȵȴ�.

		if (*pInSize < m_lOutSize)
			return FALSE;

		*pInSize = (UINT16)m_lOutSize;
		memcpy(pInBuffer, m_OutMemory, m_lOutSize);
		return TRUE;
	}
	
	ASSERT(!"AuMiniLZO::Compress Error");
	return FALSE;
}

BOOL AuMiniLZO::DeCompress(BYTE* pCompressedBuffer, UINT16 lCompressedSize, BYTE* pDeCompressBuffer, UINT16 *pDeCompressSize)
{
	UINT ulCompressedSize = lCompressedSize;
	INT32 lResult;
	
	try
	{
		lResult = lzo1x_decompress(pCompressedBuffer, ulCompressedSize, pDeCompressBuffer, (lzo_uint*)&m_lOutSize, NULL);
	}
	catch(...)
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "DeCompress Error CompressedSize : %d, Decompress progress : %d", lCompressedSize, m_lOutSize);
		AuLogFile_s(ASSERVER_COMPRESS_ERROR, strCharBuff);
		return FALSE;
	}

	//if (LZO_E_OK == lzo1x_decompress_safe(pCompressedBuffer, ulCompressedSize, pDeCompressBuffer, &m_lOutSize, NULL))
	if (LZO_E_OK == lResult)
	{
		ASSERT(m_lOutSize < *pDeCompressSize);	// ���� Ǯ�� ũ�Ⱑ pDeCompressBuffer ũ�⺸�� �۾ƾ� �ȴ�.
		ASSERT(USHRT_MAX > *pDeCompressBuffer);	// m_lOutSize���� 2Byte ���� ũ�� �ȵȴ�.
		*pDeCompressSize = m_lOutSize;
		return TRUE;
	}

	ASSERT(!"AuMiniLZO::DeCompress Error");
	return FALSE;
}
