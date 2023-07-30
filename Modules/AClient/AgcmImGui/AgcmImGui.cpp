
/* ------------------------------------------------------- */

#include "AgcmImGui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx9.h"
#include <AgcmRender/AgcmRender.h>
#include <rwcore.h>

/* ------------------------------------------------------- */

extern IMGUI_IMPL_API LRESULT
ImGui_ImplWin32_WndProcHandler(
	HWND	hWnd, 
	UINT	msg, 
	WPARAM	wParam, 
	LPARAM	lParam);

/* ------------------------------------------------------- */

AgcmImGui::AgcmImGui()
	: m_hwnd(NULL)
{
	SetModuleName("AgcmImGui");
}

AgcmImGui::~AgcmImGui()
{
}

/* ------------------------------------------------------- */

BOOL AgcmImGui::OnAddModule()
{
	return TRUE;
}

BOOL AgcmImGui::OnInit()
{
#ifdef _DEBUG
	ImGuiIO * io;

	ASSERT(m_hwnd != NULL);
	ImGui::CreateContext();
	io = &ImGui::GetIO();
	io->ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
	ImGui::StyleColorsDark();
	
	if (!ImGui_ImplWin32_Init(m_hwnd) || 
		!ImGui_ImplDX9_Init((IDirect3DDevice9 *)RwD3D9GetCurrentD3DDevice())) {
		return FALSE;
	}
	io->Fonts->AddFontDefault();
#endif
	return TRUE;
}

BOOL AgcmImGui::OnDestroy()
{
	return TRUE;
}

BOOL AgcmImGui::OnIdle(UINT32 ulClockCount)
{
	return TRUE;
}

void AgcmImGui::OnLuaInitialize(AuLua * pLua)
{
}

/* ------------------------------------------------------- */

void AgcmImGui::SetHwnd(HWND hwnd)
{
	m_hwnd = hwnd;
}

BOOL AgcmImGui::WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
#ifdef _DEBUG
	if (ImGui_ImplWin32_WndProcHandler(hwnd, message, wparam, lparam))
		return TRUE;
	if (!ImGui::GetCurrentContext())
		return FALSE;
	switch (message) {
    case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
    case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
		if (ImGui::GetIO().WantCaptureMouse)
			return TRUE;
		break;
    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
		if (ImGui::GetIO().WantCaptureKeyboard)
			return TRUE;
		break;
	}
#endif
	return FALSE;
}

void AgcmImGui::Render()
{
#ifdef _DEBUG
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
#endif
}
