// AuMiniLZO.h: interface for the AuMiniLZO class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUMINILZO_H__B8CBDB0D_0396_4426_B7A1_885E341DBCD4__INCLUDED_)
#define AFX_AUMINILZO_H__B8CBDB0D_0396_4426_B7A1_885E341DBCD4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <ApBase/ApBase.h>
#include "minilzo.h"

class AuMiniLZO  
{
private:
	// LZO�� ������ �����ϴµ� �ʿ��� �۾� �޸�
	lzo_align_t __LZO_MMODEL m_WorkMemory [ ((LZO1X_1_MEM_COMPRESS) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t) ];
	BYTE m_OutMemory[10 * 1024];	// LZO�� ���ؼ� ����� ������� ����� �޸�
	UINT m_lOutSize;

public:
	AuMiniLZO();
	virtual ~AuMiniLZO();

	BOOL Compress(BYTE* pInBuffer, UINT16* pInSize);
	BOOL DeCompress(BYTE* pCompressedBuffer, UINT16 lCompressedSize, BYTE* pDeCompressBuffer, UINT16 *pDeCompressSize);
};

#endif // !defined(AFX_AUMINILZO_H__B8CBDB0D_0396_4426_B7A1_885E341DBCD4__INCLUDED_)
