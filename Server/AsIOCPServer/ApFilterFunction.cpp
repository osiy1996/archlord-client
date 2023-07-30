// ApFilterFunction.cpp: implementation of the ApFilterFunction class.
//
//////////////////////////////////////////////////////////////////////

#include "ApFilterFunction.h"
#include "ApMutualEx.h"

ApCriticalSection ApFilterMutex;

const INT32 SYMBOL_BUFFER_SIZE = 512;

static CHAR *g_szLogFileName = NULL;

// �ɺ����� ���� �ּ�����(�Լ��̸� ��)
static BYTE g_SymbolBuffer[SYMBOL_BUFFER_SIZE] ;

// �ɺ����� ���� �ҽ��ڵ� ����(�����̸�, ���� ��)
static IMAGEHLP_LINE g_stLine ;

// StackWalk�� �̿��Ͽ� Stack�� ������ ������� STACKFRAME
static STACKFRAME g_StackFrame ;

BOOL InitSymbolEngine();
VOID InitSymbolBuffer(PIMAGEHLP_SYMBOL pSym);
BOOL CleanupSymbolEngnie();
VOID InitStackFrame(LPEXCEPTION_POINTERS pException);

BOOL __stdcall Stack_ReadProcessMemory(HANDLE hProcess, LPCVOID lpBaseAddress, LPVOID  lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesRead)
{
    return (::ReadProcessMemory(::GetCurrentProcess(), lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesRead));
}

BOOL InitSymbolEngine()
{
    DWORD dwOpts = ::SymGetOptions();
    ::SymSetOptions(dwOpts | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);
	return ::SymInitialize(ULongToHandle(::GetCurrentProcessId()), NULL, TRUE);
}

BOOL CleanupSymbolEngnie()
{
	return ::SymCleanup(ULongToHandle(::GetCurrentProcessId()));
}

VOID InitSymbolBuffer(PIMAGEHLP_SYMBOL pSym)
{
	// Symbol Buffer �ʱ�ȭ
    ::ZeroMemory(pSym, SYMBOL_BUFFER_SIZE);
    pSym->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
    pSym->MaxNameLength = SYMBOL_BUFFER_SIZE - sizeof(IMAGEHLP_SYMBOL);
}

void InitStackFrame(EXCEPTION_POINTERS *pException)
{
    ZeroMemory(&g_StackFrame , sizeof(STACKFRAME));

    #ifdef _X86_
		g_StackFrame.AddrPC.Offset       = pException->ContextRecord->Eip;
		g_StackFrame.AddrPC.Mode         = AddrModeFlat;
		g_StackFrame.AddrStack.Offset    = pException->ContextRecord->Esp;
		g_StackFrame.AddrStack.Mode      = AddrModeFlat;
		g_StackFrame.AddrFrame.Offset    = pException->ContextRecord->Ebp;
		g_StackFrame.AddrFrame.Mode      = AddrModeFlat;

	#elif defined _M_X64
		// nothing
		//
    #else
		g_StackFrame.AddrPC.Offset       = (DWORD)pException->ContextRecord->Fir;
		g_StackFrame.AddrPC.Mode         = AddrModeFlat;
		g_StackFrame.AddrReturn.Offset   = (DWORD)pException->ContextRecord->IntRa;
		g_StackFrame.AddrReturn.Mode     = AddrModeFlat;
		g_StackFrame.AddrStack.Offset    = (DWORD)pException->ContextRecord->IntSp;
		g_StackFrame.AddrStack.Mode      = AddrModeFlat;
		g_StackFrame.AddrFrame.Offset    = (DWORD)pException->ContextRecord->IntFp;
		g_StackFrame.AddrFrame.Mode      = AddrModeFlat;
	#endif	
}

static CHAR *GetExpectionCodeText(DWORD dwExceptionCode) 
{
	switch(dwExceptionCode) 
	{
	case EXCEPTION_ACCESS_VIOLATION:			return "ACCESS VIOLATION";
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:		return "ARRAY BOUNDS EXCEEDED";
	case EXCEPTION_BREAKPOINT:					return "BREAKPOINT";
	case EXCEPTION_DATATYPE_MISALIGNMENT:		return "DATATYPE MISALIGNMENT";
	case EXCEPTION_FLT_DENORMAL_OPERAND:		return "FLT DENORMAL OPERAND";
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:			return "FLT DIVIDE BY ZERO";
	case EXCEPTION_FLT_INEXACT_RESULT:			return "FLT INEXACT RESULT";
	case EXCEPTION_FLT_INVALID_OPERATION:		return "FLT INVALID OPERATION";
	case EXCEPTION_FLT_OVERFLOW:				return "FLT OVERFLOW";
	case EXCEPTION_FLT_STACK_CHECK:				return "FLT STACK CHECK";
	case EXCEPTION_FLT_UNDERFLOW:				return "FLT UNDERFLOW";
	case EXCEPTION_ILLEGAL_INSTRUCTION:			return "ILLEGAL INSTRUCTION";
	case EXCEPTION_IN_PAGE_ERROR:				return "IN PAGE ERROR";
	case EXCEPTION_INT_DIVIDE_BY_ZERO:			return "INT DIVIDE BY ZERO";
	case EXCEPTION_INT_OVERFLOW:				return "INT OVERFLOW";
	case EXCEPTION_INVALID_DISPOSITION:			return "INVALID DISPOSITION";
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:	return "NONCONTINUABLE EXCEPTION";
	case EXCEPTION_PRIV_INSTRUCTION:			return "PRIV INSTRUCTION";
	case EXCEPTION_SINGLE_STEP:					return "SINGLE STEP";
	case EXCEPTION_STACK_OVERFLOW:				return "STACK OVERFLOW";
	case DBG_CONTROL_C :						return "DBG CONTROL C ";
	default:									return "<unkown exception>";
	}
}

BOOL ApFilterFunction(LPEXCEPTION_POINTERS pException, BOOL bDebugBreak)
{
	AuAutoLock Lock(ApFilterMutex);
	if (!Lock.Result()) return FALSE;
	
	// ���� �̸� ����
	FILE*	fp;
	CHAR szFileName[_MAX_PATH];
	::ZeroMemory(szFileName, _MAX_PATH);
	::GetModuleFileName(NULL, szFileName, _MAX_PATH);

	SYSTEMTIME	timefile;
	::GetLocalTime(&timefile);
	sprintf(&szFileName[strlen(szFileName)], "_%d%02d%02d_%2d%2d%2d", timefile.wYear, timefile.wMonth, timefile.wDay,
											timefile.wHour, timefile.wMinute, timefile.wSecond);

	strcat(szFileName, FILEEXT);

	fp = fopen(szFileName, "a");
	if (NULL == fp) return FALSE;

	// �ɺ� ���� �ʱ�ȭ
	InitSymbolEngine();

	// �ɺ� �������� ���� �����͸� ���� ���ۼ���
	PIMAGEHLP_SYMBOL pSym = (PIMAGEHLP_SYMBOL)&g_SymbolBuffer;
	InitSymbolBuffer(pSym);

	IMAGEHLP_LINE Line;
	::ZeroMemory(&Line, sizeof(IMAGEHLP_LINE));

	// Ư�� �ּҸ� �̿��Ͽ� �ɺ� ������ ���´�.
	DWORD_PTR	dwDiaplacementSym;
    DWORD		dwDiaplacementLine;
	DWORD		dwHandle = GetCurrentProcessId();

	InitStackFrame(pException);

	// ���� ���� ���
	fprintf(fp,
      "[[[ ApFilterFunction ]]]\n"
      "   ExpCode\t: 0x%8.8X [%s]\n"
      "   ExpFlags\t: %d\n"
      "   ExpAddress\t: 0x%016p\n"
	  "   GetLastError\t: %d\n"
      "   Please report!",
      pException->ExceptionRecord->ExceptionCode,
	  GetExpectionCodeText(pException->ExceptionRecord->ExceptionCode),	// Exceptin Code�� �ش��ϴ� �������� 
      pException->ExceptionRecord->ExceptionFlags,
      pException->ExceptionRecord->ExceptionAddress,
	  GetLastError());

	// �ݽ��� ������ ����
	while (StackWalk(IMAGE_FILE_MACHINE_I386, ULongToHandle(::GetCurrentProcessId()), ::GetCurrentThread(), 
					&g_StackFrame, pException->ContextRecord, (PREAD_PROCESS_MEMORY_ROUTINE)Stack_ReadProcessMemory, 
					SymFunctionTableAccess, SymGetModuleBase, NULL))
	{
		if (TRUE == ::SymGetSymFromAddr(ULongToHandle(dwHandle), (DWORD)g_StackFrame.AddrPC.Offset, &dwDiaplacementSym, pSym))
			if (TRUE == ::SymGetLineFromAddr(ULongToHandle(dwHandle), (DWORD_PTR)g_StackFrame.AddrPC.Offset, &dwDiaplacementLine, &Line))
			{
				fprintf(fp, "\n%s (%d) [%s] ---> Exception Address : 0x%8.8X", Line.FileName, Line.LineNumber, 
									pSym->Name, g_StackFrame.AddrPC.Offset);
			}
	}
	
	//Cleanup
	CleanupSymbolEngnie();
	fclose(fp);

	// DebugBreak�� ����� �Ŷ�� ���⼭ Breakpoint�۵�
	if (bDebugBreak)
		DebugBreak();

	return TRUE;
}