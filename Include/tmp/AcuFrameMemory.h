#ifndef		_ACUFRAMEMEMORY_H_
#define		_ACUFRAMEMEMORY_H_

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AcuFrameMemoryD" )
#else
#pragma comment ( lib , "AcuFrameMemory" )
#endif
#endif

#include "ApBase.h"

#define DEBUG_FRAME_MEMORY_SIZE		2048*1024	//. 2M

class	AcuFrameMemory					// �� frame���� clear
{
public:
	AcuFrameMemory();
	~AcuFrameMemory();

	static	BOOL	Create();
	static	void	Clear();
	static	void	Release();

	static	BOOL	InitManager(INT32 nSize);
	
	static	void*	AllocFrameMemory(INT32 nBytes);
	static	void	DeallocFrameMemory(INT32	nBytes);	// �����Ⱓ�� block�� �ΰ�� block������ �������� ����������

public:
	static	INT32		m_iByteAllocated;					// debug ���������� check
	static	UINT32		m_iCurAllocated;

//#ifdef _DEBUG
//	static	UINT8		m_pMemoryBlock[DEBUG_FRAME_MEMORY_SIZE];
//#else
//	static	UINT8*		m_pMemoryBlock;
//#endif

	static	UINT8		m_pMemoryBlock[DEBUG_FRAME_MEMORY_SIZE];	//. ���̻� heap�� ��������.
	static	UINT8*		m_pFrame;


};

template <class TObject>
class AcuFrameMemoryLocal
{
	INT		size;
	LPVOID	memory;
public:
	AcuFrameMemoryLocal( INT nNum )
		: size(sizeof(TObject)*nNum)
		, memory(AcuFrameMemory::AllocFrameMemory(sizeof(TObject)*nNum))
	{
		ASSERT(nNum);
		ASSERT(memory);
	}

	virtual ~AcuFrameMemoryLocal()
	{
		AcuFrameMemory::DeallocFrameMemory( size );
	}

	operator TObject*()
	{
		return (TObject*)(memory);
	}
};

// ��뿹
//void ex()
//{
//	AcuFrameMemoryLocal<INT>	buff(10);
//	for( int i=0; i<10; ++i )
//		*(buff+i) = i;
//}

#endif