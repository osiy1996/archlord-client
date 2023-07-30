#pragma once

#undef APMEMORY_CHECKER

#define _CRT_ALLOCATION_DEFINED
#define _STD_USING

#include "ApMutualEx.h"
#include <windows.h>

const unsigned int FRONT_GUARD_BYTE	= 0xAABBCCDD;
const unsigned int REAR_GUARD_BYTE	= 0xDDCCBBAA;

enum eAllocationType
{
	EAT_NONE = 0,
	EAT_MALLOC,				// malloc
	EAT_NEW,				// new(size)
	EAT_NEW_DEBUG,			// new(size, sourceFile, sourceLine)
	EAT_NEW_ARRAY,			// new [] (size)
	EAT_NEW_ARRAY_DEBUG,	// new [] (size, sourceFile, sourceLine)
};

enum eDeallocationType
{
	EDT_NONE = 0,
	EDT_FREE,				// free
	EDT_DELETE,				// delete
	EDT_DELETE_ARRAY,		// delete []
};

struct ApMemDebugInfo
{
	size_t				dataSize_;			// ������ �Ҵ� ��û ������
	size_t				allocSize_;			// �޸𸮿� �Ҵ� �� ������
	char*				sourceFile_;		// ���� �̸�
	char*				sourceFunc_;		// �Լ� �̸�
	int					sourceLine_;		// line ��ȣ
	LONG				sequence_;			// �Ҵ�� ����
	eAllocationType		allocType_;			// �Ҵ� Ÿ��
	eDeallocationType	deallocType_;		// �Ҵ� ���� Ÿ��
	ApMemDebugInfo		*next_;				// next node
	ApMemDebugInfo		*prev_;				// previous node
	unsigned int		frontGuardByte_;	// front guard byte

	// followed by:
	// unsigned char rawData[size_]
	// unsigned int rearguardbyte
};

class ApMemoryChecker
{
public:
	ApMemoryChecker(void);
	~ApMemoryChecker(void);

	static ApMemoryChecker& getInstance();

	void dumpAll();
	void dumpCrashedMemory();

	void setBasePtr(const void *pointer);
	bool isValidBasePtr(const void *pointer);

	void setCountLimit(const size_t limitCount);
	bool isValidCountLimit(const size_t count);

	void* debug_alloc(const char *sourceFile, const unsigned int sourceLine, const char *sourceFunc,
			     eAllocationType allocType, const size_t size);

	void debug_free(const char *sourceFile, const unsigned int sourceLine, const char *sourceFunc,
			      eDeallocationType deallocType, const void *pointer);

	// MBCS
	void* memmove_Checker(void *dest, const void *src, size_t count);
	void* memcpy_Checker(void *dest, const void *src, size_t count);
	void* memset_Checker(void *dest, int c, size_t count);

#ifdef _UNICODE
	wchar_t* wmemmove_Checker(wchar_t *dest, const wchar_t *src, size_t count);
	wchar_t* wmemcpy_Checker(wchar_t *dest, const wchar_t *src, size_t count);
	wchar_t* wmemset_Checker(wchar_t *dest, wchar_t c, size_t count);
#endif

// c++ style
void	setNewInfo(const char *file, const unsigned int line, const char *func);

private:
	unsigned int* getRearGuardByte(const ApMemDebugInfo *pDebugInfo);
	ApMemDebugInfo* getDebugInfo(const void *pRawData);
	bool isValidPointer(const void *pointer);
	bool isValidFree(eAllocationType allocType, eDeallocationType deallocType);
	bool isValidGuardByte(const ApMemDebugInfo *pDebugInfo);

private:
	HANDLE heap_;

	void *basePtr_;
	size_t limitCount_;

	LONG sequence_;				// �Ҵ� ��ȣ(malloc, new ����)
	LONG allocCount_;			// �޸� �Ҵ��� ȣ���� Ƚ��
	LONG freeCount_;			// �޸� ������ �̿��� Ƚ�� (pooling ����)
	LONG currentAllocCount_;	// ���� �Ҵ�� �޸� ����

	size_t currentAllocatedSize_;	// ���� �Ҵ�� �޸��� Byte ũ��(debug ���� ���� ������)
	size_t currentDataSize_;		// �Ҵ�� �������� Byte ũ��(���� ������ ������)

	ApMemDebugInfo *first_;
	ApMemDebugInfo *last_;

	ApCriticalSection lock_;
};

void* malloc_Checker(const char *sourceFile, const unsigned int sourceLine, const char *sourceFunc,
			     const size_t size);

void free_Checker(const char *sourceFile, const unsigned int sourceLine, const char *sourceFunc,
			      const void *pointer);

void* realloc_Checker(void *memblock, size_t size);
void* calloc_Checker(size_t num, size_t size);

//// memory ���� �Լ�
//void* memmove_Checker(void *dest, const void *src, size_t count);
////wchar_t* wmemmove_Checker(wchar_t *dest, const wchar_t *src, size_t count);
//
//void* memcpy_Checker(void *dest, const void *src, size_t count);
////wchar_t* wmemcpy_Checker(wchar_t *dest, const wchar_t *src, size_t count);
//
//void* memset_Checker(void *dest, int c, size_t count);
//
////wchar_t* wmemset_Checker(wchar_t *dest, wchar_t c, size_t count);
//
//// c++ style
//void	setNewInfo(const char *file, const unsigned int line, const char *func);

// preprocessor definitions ���� APMEMORY_CHECKER�� �������־�� �Ѵ�.
#ifdef APMEMORY_CHECKER

void*	operator new(size_t size);
void*	operator new[](size_t size);
void*	operator new(size_t size, const char *sourceFile, int sourceLine);
void*	operator new[](size_t size, const char *sourceFile, int sourceLine);
void	operator delete(void *pointer);
void	operator delete[](void *pointer);



//#define	new						(setNewInfo  (__FILE__,__LINE__,__FUNCTION__), false) ? NULL : new
//#define	delete					(setNewInfo  (__FILE__,__LINE__,__FUNCTION__),false) ? m_setOwner("",0,"") : delete

#define	malloc(_SIZE)			malloc_Checker(__FILE__,__LINE__,__FUNCTION__, _SIZE)
#define	calloc(_NUM, _SIZE)		calloc_Checker(_NUM, _SIZE)
#define	realloc(_PTR, _SIZE)	realloc_Checker(_PTR, _SIZE)
#define	free(_PTR)				free_Checker(__FILE__,__LINE__,__FUNCTION__, _PTR)

// MBCS
#define memmove(_DEST_PTR, _SRC_PTR, _COUNT)	ApMemoryChecker::getInstance().memmove_Checker(_DEST_PTR, _SRC_PTR, _COUNT)
#define memcpy(_DEST_PTR, _SRC_PTR, _COUNT)		ApMemoryChecker::getInstance().memcpy_Checker(_DEST_PTR, _SRC_PTR, _COUNT)
#define memset(_DEST_PTR, _CHAR, _COUNT)		ApMemoryChecker::getInstance().memset_Checker(_DEST_PTR, _CHAR, _COUNT)

#ifdef _UNICODE
#define wmemmove(_DEST_PTR, _SRC_PTR, _COUNT)	ApMemoryChecker::getInstance().wmemmove_Checker(_DEST_PTR, _SRC_PTR, _COUNT)
#define wmemcpy(_DEST_PTR, _SRC_PTR, _COUNT)	ApMemoryChecker::getInstance().wmemcpy_Checker(_DEST_PTR, _SRC_PTR, _COUNT)
#define wmemset(_DEST_PTR, _CHAR, _COUNT)		ApMemoryChecker::getInstance().wmemset_Checker(_DEST_PTR, _CHAR, _COUNT)
#endif

//#define MoveMemory(_DEST_PTR, _SRC_PTR, _COUNT) memmove_Checker(_DEST_PTR, _SRC_PTR, _COUNT)
//#define CopyMemory(_DEST_PTR, _SRC_PTR, _COUNT) memcpy_Checker(_DEST_PTR, _SRC_PTR, _COUNT)
//#define FillMemory(_DEST_PTR, _COUNT, _CHAR)	memset_Checker(_DEST_PTR, _CHAR, _COUNT)
//#define ZeroMemory(_DEST_PTR, _LENGTH)			memset_Checker(_DEST_PTR, 0, _LENGTH)

#endif