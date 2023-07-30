#pragma once

#include "ApMutualEx.h"

const INT32 MAX_TYPE_SIZE		= 1000;			// MemoryManager���� ���ɼ� �ִ� Type�� �ִ� ����
const INT32 DEFAULT_ARRAY_SIZE	= 1000;			// �ϳ��� Type�� �Ҵ�� �迭 ũ��
const INT32 MAX_TYPE_NAME_SIZE	= 64;

#define GUARD_BYTE_BEGIN	0xBB
#define GUARD_BYTE_END		0xCC
#define ALLOC_BYTE			0xDD
#define FREE_BYTE			0xEE

#if defined (WIN64)
	#define LAST_NODE			0xFFFFFFFFFFFFFFFF
#else
	#define LAST_NODE			0xFFFFFFFF
#endif

class ApMemoryManager 
{
public:
	// Linked List�� ���ؼ� �ʿ��� ������
	PVOID	m_pHeadArray[MAX_TYPE_SIZE];		// linked list�� Head�� ����ɰ�
	PVOID	m_pHeadEmpty[MAX_TYPE_SIZE];		// linked list�� ����ִ� Head�� ����ɰ�
												// (���������� �Ҵ�� �������� �ٷ� ���� ���)

	// ���� �������� ������ ���ؼ� �ʿ��� ������
	INT32	m_lDataSize[MAX_TYPE_SIZE];			// �ش� ������Ʈ�� ������ ũ�� sizeof
	INT32	m_lAllocSize[MAX_TYPE_SIZE];		// ���� �Ҵ�� �������� ũ��
	INT32	m_lArraySize[MAX_TYPE_SIZE];		// �ش� �迭�� ������ ����

	INT32	m_lAllocCount[MAX_TYPE_SIZE];		// ���� �Ҵ�� �������� ����
	INT32	m_lAllocCountPeak[MAX_TYPE_SIZE];	// ���� �ִ� �޸� �Ҵ� ����

	// �ʱ⿡ ������ Ǯ�������� �Ѿ�� ::operator new�� ȣ���ϰ� �Ǵµ� 
	// �׶� alloc�� ������ �����ϱ� ���ؼ� �߰�
	INT32	m_lAddedAllocCount[MAX_TYPE_SIZE];	// �߰��� �Ҵ� ����
	INT32	m_lFlagAssert[MAX_TYPE_SIZE];		// Ǯ�� ������ �Ѿ�� �ѹ��� ASSERT�� ����ֱ� ���� �÷��׿뺯��

	INT32	m_bEnable[MAX_TYPE_SIZE];			// �޸� Ǯ���� Enable/Disable ���θ� ����

#ifdef _CPPRTTI
	CHAR	m_TypeName[MAX_TYPE_SIZE][MAX_TYPE_NAME_SIZE + 1];		// RTTI�� �̿��� Class Name
#endif

	INT32	m_lTypeIndex;

	ApCriticalSection	m_cs;	// CriticalSection�ε� �̸��� mutex�� �ٲ�

	SYSTEMTIME	m_stStartTime;

private:
	ApMemoryManager();

	VOID	Initialize();
	VOID	InitializeNode(INT32 lTypeIndex);
	BOOL	CheckBeginGuardByte(PVOID pObject);
	BOOL	CheckEndGuardByte(PVOID pObject, INT32 lTypeIndex);
	BOOL	GetAllocated(PVOID pObject);
	VOID	SetAllocation(PVOID pObject, BOOL bAllocation);

public:

	~ApMemoryManager();

	static ApMemoryManager& GetInstance();

	VOID DestroyInstance();
	VOID Destroy();

	INT32 AddType(SIZE_T lSize, SIZE_T lArraySize = DEFAULT_ARRAY_SIZE, const CHAR* pTypeName = NULL);

	PVOID NewObject(INT32 lTypeIndex);
	VOID DeleteObject(INT32 lTypeIndex, PVOID pObject);

	INT32 GetAllocatedCount(INT32 lTypeIndex);
	VOID ReportMemoryInfo();

	BOOL SetEnable(INT32 lTypeIndex, BOOL bEnable);
	BOOL GetEnable(INT32 lTypeIndex);
};
