#pragma once

#include <windows.h>
#include <TCHAR.h>

#ifdef ALEF_DLLEXPORT
#define ALEF_DLL __declspec(dllexport)
#elif ALEF_DLLIMPORT
#define ALEF_DLL __declspec(dllimport)
#else
#error ALEF_DLL define error
#endif

// FrameCallstack�� �����庰�� �����Ǹ� �� �����尡 ����Ǹ鼭 ���İ� �Լ��� ȣ��������
// �����Ѵ�. �̶� �ڵ������� �����Ǵ� ������ �Լ��� ���ۺκа� ����κ��̸�
// �Լ��� �߰��κ��� ������ �ڵ带 �����Ͽ� ������ �����Ѵ�.
// ���� �� 100������ �Լ� ȣ�� ������ �����Ҽ� ������ �̴� ���۰� ���� ������ ������
// ��ġ�̱� ������ ��������� �� 50������ �Լ��� ȣ���Ҽ� �ִ�.

// FrameCallStack�� �ʱ�ȭ �Ѵ�. 
// Callstack������ ������ ���ϴ� �۾��� ������������ �ʱ�ȭ�� �����Ѵ�.
// ���ӷ����� ��� �ѹ��� ���ӷ��� �Ŀ��� �ݵ�� �ʱ�ȭ�� ���־�� �ش� ���ӷ����� Callstack������ 
// ������ �ִ�. ���� �ʱ�ȭ�� ������ ������ Callstack ������ ��� ������Ų��.
ALEF_DLL void FrameProfile_Init();

// FrameCallStack ����� ��뿩�θ� �����Ѵ�. TRUE�� CallStack������ �����ϱ� �����ϰ� FALSE�̸� �����Ѵ�.
// ���� FrameCallStack�� ��� ����Ѵٸ� �ش� �����忡�� �� �ѹ��� ȣ���ϸ� �ȴ�.
// ���ϴ� ������ CallStack ������ ������ ���������� ��� �����ϴ�.
ALEF_DLL void FrameProfile_SetEnable( BOOL bEnable );

ALEF_DLL void FrameProfile_SetThreadName( LPTSTR pThreadName );

// SEH�� �̿��Ͽ� ���� ó���� �Ҷ� ���ȴ�. �ش� �����ڵ�� GetExceptionInformation�� �̿���
// ���� �߻� �ּ������� �����Ѵ�. 
// ���� : GetExceptionInformation�� GetExceptionCode�� ���� ���Ϳ����� ��� �����ϴ�
ALEF_DLL void FrameProfile_AddSEHInfo( DWORD dwErrorCode, DWORD_PTR dwExceptionAddress );

// ������ FrameCallstack ������ ���Ͽ� �����Ѵ�. ���Ͽ� �����Ҷ��� �̸� ��Ģ�� ������ ����.
// �Ϲ����� ���			: FrameCallstack [ThreadID] [Count]
// Exception�� �߻��� ���	: Exception_FrameCallStack [ThreadID] [Count]
ALEF_DLL void FrameProfile_WriteFile();

ALEF_DLL void FrameProfile_WriteFileAll();

// WriteFrameCallStackLog�� �̿��Ͽ� �ۼ��� ������ �о� ���δ�.
// �̶� Callstack ������ ExceptionCode ������ �о�´�.
ALEF_DLL void FrameProfile_ReadFromFile( LPCTSTR pFileName );

// FrameCallstack�� ������ �ִ� ������ �ܼ� ȭ������ ����Ѵ�.
// ���� ������ ������ �̿��ϵ� ReadFrameCallStackLog�� �̿��ϵ� ��밡��
ALEF_DLL void FrameProfile_DisplayToConsole();

// AddSEHInfo�� ���� ���� ���ؼ� �����ϴ� �Լ�
// ��뿹)
// __except( ExceptionFilterFrameCallstack ( GetExceptionCode(), GetExceptionInformation() )
ALEF_DLL int FrameProfile_ExceptionFilter( DWORD dwErrorCode, LPEXCEPTION_POINTERS pException );

// ManualProfile�� FrameCallstack�� �Լ��� ���۰� ���� ������ �������� ����� �ִ� ������
// �����ϴ� ������� �Լ��� �߰��߰��� ���α׷��Ӱ� ���ϴ� ��ġ�� ��������� �α׸� ������ �ִ�.
// ������ naked �Լ��� Ư���� �Լ��� ȣ���� ������ ���� �Լ��� ����� ���⼭ ���ϵǴ�
// FARPROC(ManualProfile �Լ� ������)�� �̿��Ͽ� �Լ��� ȣ���Ѵ�.
//
// FARPROC GetManualProfile()
// {
//		return ::GetProcAddress( ::GetModuleHandle( _T("AuFrameProfile") ), _T("ManualProfile") );
// }
//
//	ex)
// FARPROC fpManual = GetManualProfile();
// fpManual();	// ȣ�� �Ϸ�
//