#ifndef _AGCM_IMGUI_H_
#define _AGCM_IMGUI_H_

#include <AgcModule/AgcModule.h>

#if _MSC_VER < 1300
	#ifdef	_DEBUG
		#pragma comment ( lib , "AgcmImGuiD" )
	#else
		#pragma comment ( lib , "AgcmImGui" )
	#endif
#endif

class AuLua;

class AgcmImGui : public AgcModule
{
private:
	HWND m_hwnd;

public:
	AgcmImGui();
	~AgcmImGui();

	/* Module function overloads. */
	
	BOOL	OnAddModule();
	BOOL	OnInit();
	BOOL	OnDestroy();
	BOOL	OnIdle(UINT32 ulClockCount);
	void	OnLuaInitialize( AuLua * pLua );

	/* ImGui related functions */

	void SetHwnd(HWND hwnd);
	static BOOL WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
	void Render();
};

#endif /* _AGCM_IMGUI_H_ */
