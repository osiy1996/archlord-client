// AgcmUIDebugInfo.h: interface for the AgcmUIDebugInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGCMUIDEBUGINFO_H__6D994291_6B9B_4413_8708_76CE782997AD__INCLUDED_)
#define AFX_AGCMUIDEBUGINFO_H__6D994291_6B9B_4413_8708_76CE782997AD__INCLUDED_

#include <AgcModule/AgcModule.h>
#include <AgcmUIDebugInfo/AgcDebugWindow.h>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if _MSC_VER < 1300
#ifdef	_DEBUG
	#pragma comment (lib , "AgcmUIDebugInfoD")
#else
	#pragma comment (lib , "AgcmUIDebugInfo")
#endif
#endif

typedef enum
{
	AGCM_UIDEBUG_MODE_PROFILE	= 0,
	AGCM_UIDEBUG_MODE_MEMORY,
	AGCM_UIDEBUG_MODE_OBJECT,
	AGCM_UIDEBUG_MODE_GRAPHIC,
	AGCM_UIDEBUG_MODE_SOUND,
	AGCM_UIDEBUG_MODE_NETWORK,
	AGCM_UIDEBUG_MAX_MODE
} AgcmUIDebugMode;

class AgcmUIDebugInfo : public AgcModule  
{
private:
	AgcmUIManager2 *	m_pcsAgcmUIManager2;
	AgcDebugWindow		m_csDebugWindow;

	BOOL				m_bEnableDebugInfo;
	AgcmUIDebugMode		m_eDebugMode;

public:
	class ApmMap *				m_pcsApmMap;
	class AgcmMap *				m_pcsAgcmMap;
	class AgpmCharacter *		m_pcsAgpmCharacter;
	class AgcmCharacter *		m_pcsAgcmCharacter;
	class AgpmItem *			m_pcsAgpmItem;
	class AgcmItem *			m_pcsAgcmItem;
	class ApmObject *			m_pcsApmObject;
	class AgcmObject *			m_pcsAgcmObject;
	class AgcmGrass *			m_pcsAgcmGrass;
	class AgcmShadow2 *			m_pcsAgcmShadow2;
	class AgcmRender *			m_pcsAgcmRender;
	class AgcmSound *			m_pcsAgcmSound;
	class AgcmFont *			m_pcsAgcmFont;
	class AgcmResourceLoader *	m_pcsAgcmResourceLoader;
	class AgcmEff2*				m_pcsAgcmEff2;

public:
	AgcmUIDebugInfo();
	virtual ~AgcmUIDebugInfo();

	BOOL	OnAddModule();
	BOOL	OnInit();
	BOOL	OnIdle(UINT32 unClockCount);
	BOOL	OnDestroy();

	BOOL	EnableDebugInfo(BOOL bEnable = TRUE);
	inline	VOID	ToggleDebugInfo()	{ EnableDebugInfo(!m_bEnableDebugInfo);	}

	VOID	SetDebugMode(AgcmUIDebugMode eMode);
	inline	AgcmUIDebugMode		GetCurrentDebugMode()	{ return m_eDebugMode;	}

	VOID	ChangeTextureFilterMode();
};

#endif // !defined(AFX_AGCMUIDEBUGINFO_H__6D994291_6B9B_4413_8708_76CE782997AD__INCLUDED_)
