// XTThemeManager.h: interface for the CXTThemeManager class.
//
// This file is a part of the XTREME CONTROLS MFC class library.
// (c)1998-2007 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

//{{AFX_CODEJOCK_PRIVATE
#if !defined(__XTTHEMEMANAGER_H__)
#define __XTTHEMEMANAGER_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/XTPWinThemeWrapper.h"
#include "Common/XTPColorManager.h"

// -------------------------------------------------------------
// Summary:
//     Enumeration used to determine active theme.
// Remarks:
//     XTThemeStyle type defines the constants used by the
//     CXTThemeManger to determine the currently selected theme.
// See Also:
//     CXTThemeManagerStyleFactory::CreateTheme
//
// <KEYWORDS xtThemeDefault, xtThemeOfficeXP, xtThemeOffice2003, xtThemeCustom>
// -------------------------------------------------------------
enum XTThemeStyle
{
	xtThemeDefault,     // Windows classic theme.
	xtThemeOfficeXP,    // Microsoft(r) Office XP theme.
	xtThemeOffice2003,  // Microsoft(r) Office 2003 theme.
	xtThemeCustom       // User defined theme.
};

class CXTThemeManagerStyleHost;
class CXTThemeManagerStyleFactory;
class CXTThemeManager;

// ----------------------------------------------------------------------
// Summary:
//     The XTThemeManager function is used for access to the
//     CXTThemeManager class.
// Remarks:
//     Call this function to access CXTThemeManager members. Since
//     this class is designed as a single instance object you can only
//     access its members through this method. You cannot directly
//     instantiate an object of type CXTThemeManager.
// Returns:
//     A pointer to the one and only CXTThemeManager instance.
// Example:
//     The following example demonstrates how to use XTThemeManager.
// <code>
// XTThemeManager()-\>SetTheme(xtThemeOfficeXP);
// </code>
// See Also:
//     CXTThemeManager
// ----------------------------------------------------------------------
_XTP_EXT_CLASS CXTThemeManager* AFX_CDECL XTThemeManager();

//===========================================================================
// Summary:
//     CXTThemeManagerStyle is standalone class used as base for any components
//     theme.
// Remarks:
//     This is base class for any theme class. I collect information about
//     selected theme and factory(CXTThemeManagerStyleFactory) created it.
// See Also: CXTThemeManagerStyleFactory, CXTThemeManager, CXTThemeManagerStyleHost
//===========================================================================
class _XTP_EXT_CLASS CXTThemeManagerStyle
{
public:
	//-------------------------------------------------------------------------
	// Summary:
	//     Construct a CXTThemeManagerStyle object.
	//-------------------------------------------------------------------------
	CXTThemeManagerStyle();

	//-------------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTThemeManagerStyle object, handles cleanup and deallocation.
	//-------------------------------------------------------------------------
	virtual ~CXTThemeManagerStyle();

public:

	//-------------------------------------------------------------------------
	// Summary:
	//     This method is called to refresh the visual metrics of
	//     the theme (fonts, colors, etc.).
	//-------------------------------------------------------------------------
	virtual void RefreshMetrics();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves factory created this theme
	// Returns:
	//     Pointer to CXTThemeManagerStyleFactory class created the theme
	//-----------------------------------------------------------------------
	CXTThemeManagerStyleFactory* GetThemeFactory();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves self theme enumerator.
	// Returns:
	//     Self theme of CXTThemeManagerStyle class. It can be one of the following:
	//     * <b>xtThemeDefault</b>    Enables Windows classic theme.
	//     * <b>xtThemeOfficeXP</b>   Enables Microsoft(r) Office XP theme.
	//     * <b>xtThemeOffice2003</b> Enables Microsoft(r) Office 2003 theme.
	//     * <b>xtThemeCustom</b>     Enables User defined theme.
	//-----------------------------------------------------------------------
	XTThemeStyle GetTheme();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves if default or Microsoft(r) Office theme used.
	// Returns:
	//     TRUE if xtThemeOfficeXP or xtThemeOffice2003 used.
	//-----------------------------------------------------------------------
	BOOL IsOfficeTheme();

	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is called by the Theme Manager to capture the
	//      area on the parent window underneath the specified owner window.
	//      This is then used for rendering the owner transparent.
	// Parameters:
	//      pWndOwner - Pointer the child of the parent window to take a
	//                  snapshot for.
	// Returns:
	//      TRUE if the parent area was successfully captured, otherwise
	//      returns FALSE.
	// See Also:
	//      DrawTransparentBack
	//-----------------------------------------------------------------------
	virtual BOOL TakeSnapShot(CWnd* pWndOwner);

	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is called to render the window specified
	//      by pWndOwner transparent displaying the parent windows background
	//      instead.
	// Parameters:
	//      pDC - Pointer to pWndOwner device context.
	//      pWndOwner - Pointer the child of the parent window to draw
	//                  transparent.
	// Returns:
	//      TRUE if the area was dran transparently, otherwise returns FALSE.
	// See Also:
	//      TakeSnapShot
	//-----------------------------------------------------------------------
	virtual BOOL DrawTransparentBack(CDC* pDC, CWnd* pWndOwner);

protected:

	CXTThemeManagerStyleFactory*    m_pFactory;     // Self factory class.
	XTThemeStyle                    m_enumTheme;    // Self theme enumerator.
	CBitmap                         m_bmpSnapShot;  // Used for drawing transparent background.

	friend class CXTThemeManagerStyleFactory;
};

//===========================================================================
// Summary:
//     CXTThemeManagerStyleFactory is CObject derived class. It managed themes of corresponded
//     CXTThemeManagerStyleHost class
// Remarks:
//     Each Component supported themes associated with one CXTThemeManagerStyleFactory class
//     which porpoise is to create theme specified for this component.
//     CXTThemeManagerStyleFactory class collect pointer to all CXTThemeManagerStyleHost
//     used it.
//     Use DECLARE_THEME_FACTORY and IMPLEMENT_THEME_FACTORY to create theme factory class.
// See Also: CXTThemeManagerStyle, CXTThemeManager, CXTThemeManagerStyleHost
//===========================================================================
class _XTP_EXT_CLASS CXTThemeManagerStyleFactory : public CObject
{
	DECLARE_DYNCREATE(CXTThemeManagerStyleFactory)
public:

	//-------------------------------------------------------------------------
	// Summary:
	//     Construct a CXTThemeManagerStyleFactory object.
	//-------------------------------------------------------------------------
	CXTThemeManagerStyleFactory();

	//-------------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTThemeManagerStyleFactory object, handles cleanup and deallocation.
	//-------------------------------------------------------------------------
	virtual ~CXTThemeManagerStyleFactory();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     This method created specified theme for corresponded CXTThemeManagerStyleHost component
	// Remarks:
	//     Use IMPLEMENT_THEME_FACTORY macro to create new theme with overridden CreateTheme method
	// Parameters:
	//     theme - Theme need to create
	//-----------------------------------------------------------------------
	virtual CXTThemeManagerStyle* CreateTheme(XTThemeStyle theme);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set safe theme
	// Parameters:
	//     pTheme - Pointer to custom theme
	//     theme  - Theme enumerator.
	// Remarks:
	//     theme can be one of following values:
	//     * <b>xtThemeDefault</b>    Enables Windows classic theme.
	//     * <b>xtThemeOfficeXP</b>   Enables Microsoft(r) Office XP theme.
	//     * <b>xtThemeOffice2003</b> Enables Microsoft(r) Office 2003 theme.
	//-----------------------------------------------------------------------
	void SetTheme(XTThemeStyle theme);
	void SetTheme(CXTThemeManagerStyle* pTheme); // <COMBINE CXTThemeManagerStyleFactory::SetTheme@XTThemeStyle>

	//-------------------------------------------------------------------------
	// Summary:
	//     This method is called to refresh the visual metrics of
	//     the safe theme (fonts, colors, etc.).
	// Remarks:
	//     It routes call to self m_pTheme:
	// Example:
	//     The following example demonstrates how to use RefreshMetrics.
	// <code>
	// if (m_pTheme)
	// {
	//     m_pTheme->RefreshMetrics();
	// }
	// </code>
	//-------------------------------------------------------------------------
	virtual void RefreshMetrics();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves safe theme.
	// Returns:
	//     Pointer to safe CXTThemeManagerStyle class.
	//-----------------------------------------------------------------------
	CXTThemeManagerStyle* GetSafeTheme();

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when new theme created.
	// Parameters:
	//     pTheme - new created theme pointer.
	//     theme - style of theme was created.
	//-----------------------------------------------------------------------
	void OnCreateSafeTheme(CXTThemeManagerStyle* pTheme, XTThemeStyle theme);

private:
	DISABLE_COPY_OPERATOR(CXTThemeManagerStyleFactory)

protected:
	CXTThemeManagerStyle*                       m_pTheme;       // Safe theme pointer
	CTypedSimpleList<CXTThemeManagerStyleHost*> m_hostList;     // Collection of Host objects used this factory.

private:
	CXTThemeManagerStyleFactory*                m_pNextFactory;

	friend class CXTThemeManager;
	friend class CXTThemeManagerStyleHost;
};

//===========================================================================
// Summary:
//     CXTThemeManagerStyleHost is standalone class. It used  as base
//     class for all themed component.
// Remarks:
//     CXTThemeManagerStyleHost is base class for all theme supported
//     components. Use its methods to set specified for individual component
//     theme.
//===========================================================================
class _XTP_EXT_CLASS CXTThemeManagerStyleHost
{
protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTThemeManagerStyleHost object.
	// Parameters:
	//     pThemeFactoryClass - Factory runtime class associated with this class
	//-----------------------------------------------------------------------
	CXTThemeManagerStyleHost(CRuntimeClass* pThemeFactoryClass);

	//-------------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTThemeManagerStyleHost object, handles cleanup and deallocation.
	//-------------------------------------------------------------------------
	virtual ~CXTThemeManagerStyleHost();

	//-------------------------------------------------------------------------
	// Summary:
	//     This member function is called to initialize the theme manager style host.
	// Parameters:
	//     pThemeFactoryClass - Factory runtime class associated with this class
	//-----------------------------------------------------------------------
	void InitStyleHost(CRuntimeClass* pThemeFactoryClass);

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to attach factory to the component.
	// Parameters:
	//     pFactory - Factory need to be attached.
	// Example:
	//     The following example demonstrates how to use AttachThemeFactory.
	// <code>
	// extern CXTCaptionThemeFactory m_buttonFactory;
	// m_buttonFactory.SetTheme(xtpThemeOffice2003);
	// m_btnOK.AttachThemeFactory(&m_buttonFactory)
	// m_btnCancel.AttachThemeFactory(&m_buttonFactory)
	// </code>
	//-----------------------------------------------------------------------
	void AttachThemeFactory(CXTThemeManagerStyleFactory* pFactory);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set theme for the component
	// Parameters:
	//     theme - Theme need to be set
	//     pTheme - Custom theme need to be set
	// Remarks:
	//     Theme can be one of following values:
	//     * <b>xtThemeDefault</b>    Enables Windows classic theme.
	//     * <b>xtThemeOfficeXP</b>   Enables Microsoft(r) Office XP theme.
	//     * <b>xtThemeOffice2003</b> Enables Microsoft(r) Office 2003 theme.
	// Example:
	//     The following example demonstrates how to use SetTheme.
	// <code>
	// m_btnOK.SetTheme(xtThemeOffice2003)
	// m_btnCancel.SetTheme(xtThemeOffice2003)
	// </code>
	//-----------------------------------------------------------------------
	void SetTheme (XTThemeStyle theme);
	void SetTheme (CXTThemeManagerStyle* pTheme); // <combine CXTThemeManagerStyleHost::SetTheme@XTThemeStyle>

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     This method called to create self theme factory class
	// Remarks:
	//     It creates factory class used runtime class specified in constructor.
	// Example:
	//     The following example demonstrates how to use CreateThemeFactory.
	// <code>
	// return (CXTThemeManagerStyleFactory*)m_pThemeFactoryClass->CreateObject();
	// </code>
	// Returns:
	//     Pointer to created CXTThemeManagerStyleFactory class
	//-----------------------------------------------------------------------
	virtual CXTThemeManagerStyleFactory* CreateThemeFactory();

	//-------------------------------------------------------------------------
	// Summary:
	//     This method is called then self theme is changed. Override this method
	//     to redraw or reposition parts of component according of new theme.
	// See Also: GetSafeTheme
	//-------------------------------------------------------------------------
	virtual void OnThemeChanged();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves safe theme of component
	// Returns:
	//     Pointer to self theme.
	//-----------------------------------------------------------------------
	CXTThemeManagerStyle* GetSafeTheme() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves self factory class
	// Returns:
	//     Pointer to CXTThemeManagerStyleFactory class.
	//-----------------------------------------------------------------------
	CXTThemeManagerStyleFactory* GetSafeThemeFactory() const;

protected:
	BOOL                         m_bAutoDeleteCustomFactory; // TRUE to delete custom theme factory.
	CRuntimeClass*               m_pThemeFactoryClass;       // Runtime class of default theme factory.
	CXTThemeManagerStyleFactory* m_pCustomFactory;           // Custom theme factory
	CXTThemeManagerStyleFactory* m_pDefaultFactory;          // Default theme factory

private:
	CXTThemeManagerStyleHost*    m_pNextHost;
	friend class CXTThemeManagerStyleFactory;
};

//===========================================================================
// Summary:
//     CXTThemeManager is standalone class used to manage the visual styles for your
//     application.
// Remarks:
//     CXTThemeManager is used to manage the visual styles for your
//     application.  Add a member of this type to your frame or
//     dialog class to manage your application themes.  See SetTheme
//     for more information on setting themes.
// See Also: XTThemeManager, CXTThemeManagerStyle, CXTThemeManagerStyleHost
//===========================================================================
class _XTP_EXT_CLASS CXTThemeManager : public CNoTrackObject
{
public:

	//-------------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTThemeManager object.
	//-------------------------------------------------------------------------
	CXTThemeManager();

	//-------------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTThemeManager object, handles cleanup and deallocation.
	//-------------------------------------------------------------------------
	virtual ~CXTThemeManager();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieved theme factory class for specified runtime class.
	//     Creates new if no found.
	// Parameters:
	//     pThemeFactoryClass - Runtime class need to find
	// Returns:
	//     CXTThemeManagerStyleFactory pointer associated with its runtime class.
	// Example:
	//     The following example demonstrates how to use GetDefaultThemeFactory.
	// <code>
	// XTThemeManager()->GetDefaultThemeFactory(RUNTIME_CLASS(CXTCaptionThemeFactory));
	// </code>
	//-----------------------------------------------------------------------
	CXTThemeManagerStyleFactory* GetDefaultThemeFactory(CRuntimeClass* pThemeFactoryClass);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set theme for all components.
	// Parameters:
	//     theme - Theme need to set
	// Remarks:
	//     Theme can be one of following values:
	//     * <b>xtThemeDefault</b>    Enables Windows classic theme.
	//     * <b>xtThemeOfficeXP</b>   Enables Microsoft(r) Office XP theme.
	//     * <b>xtThemeOffice2003</b> Enables Microsoft(r) Office 2003 theme.
	// Example:
	//     The following example demonstrates how to use SetTheme.
	// <code>
	// XTThemeManager()->SetTheme(xtThemeOffice2003);
	// </code>
	// See Also: GetTheme, XTThemeManager
	//-----------------------------------------------------------------------
	void SetTheme(XTThemeStyle theme);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieved default theme for all components
	// Returns:
	//     Default theme for all components.
	// Example:
	//     The following example demonstrates how to use GetTheme.
	// <code>
	// XTThemeManager()->GetTheme();
	// </code>
	// See Also: SetTheme, XTThemeManager
	//-----------------------------------------------------------------------
	XTThemeStyle GetTheme();

	//-------------------------------------------------------------------------
	// Summary:
	//     Call this method to refresh visual metrics of all themes.
	// See Also: CXTThemeManagerStyle::RefreshMetrics
	//-------------------------------------------------------------------------
	void RefreshMetrics();

protected:
	CMapPtrToPtr                               m_mapDefaultFactories; // Default factories collection
	XTThemeStyle                               m_enumTheme;           // Default theme for all components
	CTypedSimpleList<CXTThemeManagerStyleFactory*> m_factoryList;     // Collection of all theme factories.

private:
	static CProcessLocal<CXTThemeManager>          m_sThemeManager;

	friend _XTP_EXT_CLASS CXTThemeManager* AFX_CDECL XTThemeManager();
	friend class CXTThemeManagerStyleFactory;
};

//-----------------------------------------------------------------------
// Summary:
//     Use this macro to declare the factory class for specified theme
// Parameters:
//     theClass - Base theme class.
// Example:
//     The following example demonstrates how to use DECLARE_THEME_FACTORY.
// <code>
// DECLARE_THEME_FACTORY(CXTCaptionTheme)
//
// class _XTP_EXT_CLASS CXTCaptionTheme : public CXTThemeManagerStyle
// {
// public:
//     CXTCaptionTheme();
// };
// </code>
// See Also: IMPLEMENT_THEME_FACTORY, CXTThemeManagerStyleFactory
//-----------------------------------------------------------------------
#define DECLARE_THEME_FACTORY(theClass)
//{{AFX_CODEJOCK_PRIVATE
#undef DECLARE_THEME_FACTORY
#define DECLARE_THEME_FACTORY(theClass)\
class _XTP_EXT_CLASS theClass##Factory : public CXTThemeManagerStyleFactory\
{\
	DECLARE_DYNCREATE(theClass##Factory)\
public:\
	CXTThemeManagerStyle* CreateTheme(XTThemeStyle theme);\
};
//}}AFX_CODEJOCK_PRIVATE

//-----------------------------------------------------------------------
// Summary:
//     Use this macro to implement factory class for specified theme.
// Parameters:
//     theClass - Base theme class.
//     theClassOfficeXP - Theme used for xtThemeOfficeXP style
//     theClassOffice2003 - Theme used for xtThemeOffice2003 style
// Example:
//     The following example demonstrates how to use IMPLEMENT_THEME_FACTORY.
// <code>
// // Implement CXTCaptionThemeFactory class
// IMPLEMENT_THEME_FACTORY(CXTCaptionTheme, CXTCaptionThemeOfficeXP, CXTCaptionThemeOfficeXP)
//
// CXTCaptionTheme::CXTCaptionTheme()
// {
//     RefreshMetrics();
// }
//
// void CXTCaptionTheme::RefreshMetrics()
// {
//     m_clrEdgeShadow.SetStandardValue(
//         GetXtremeColor(COLOR_3DSHADOW));
//     m_clrEdgeLight.SetStandardValue(
//         GetXtremeColor(COLOR_3DHILIGHT));
// }
// </code>
// See Also: DECLARE_THEME_FACTORY, CXTThemeManagerStyleFactory
//-----------------------------------------------------------------------
#define IMPLEMENT_THEME_FACTORY(theClass)
//{{AFX_CODEJOCK_PRIVATE
#undef IMPLEMENT_THEME_FACTORY

#define IMPLEMENT_THEME_FACTORY(theClass)\
IMPLEMENT_DYNCREATE(theClass##Factory, CXTThemeManagerStyleFactory)\
CXTThemeManagerStyle* theClass##Factory::CreateTheme(XTThemeStyle theme)\
{\
	CXTThemeManagerStyle* pTheme = 0;\
	switch (theme)\
	{\
		case xtThemeOfficeXP:\
			pTheme = new theClass##OfficeXP();\
			break;\
		case xtThemeOffice2003:\
			pTheme = new theClass##Office2003();\
			break;\
		default:\
			pTheme = new theClass##();\
			break;\
	}\
	OnCreateSafeTheme(pTheme, theme);\
	return pTheme;\
}

#define DECLARE_THEME_HOST(theClass)\
public:\
theClass##Theme* GetTheme() const;\
static theClass##Theme* AFX_CDECL GetDefaultTheme();\
static void AFX_CDECL SetDefaultTheme(XTThemeStyle theme);\
static void AFX_CDECL SetDefaultTheme(CXTThemeManagerStyle* pTheme);\
static CRuntimeClass* AFX_CDECL GetThemeFactoryClass();

#define IMPLEMENT_THEME_HOST(theClass)\
theClass##Theme* theClass::GetTheme() const {\
	return (theClass##Theme*)GetSafeTheme();\
}\
theClass##Theme* theClass::GetDefaultTheme() {\
	return (theClass##Theme*)XTThemeManager()->GetDefaultThemeFactory(GetThemeFactoryClass())->GetSafeTheme();\
}\
void theClass::SetDefaultTheme(XTThemeStyle theme) {\
	XTThemeManager()->GetDefaultThemeFactory(GetThemeFactoryClass())->SetTheme(theme);\
}\
void theClass::SetDefaultTheme(CXTThemeManagerStyle* pTheme) {\
	XTThemeManager()->GetDefaultThemeFactory(GetThemeFactoryClass())->SetTheme(pTheme);\
}\
CRuntimeClass* theClass::GetThemeFactoryClass() {\
	return RUNTIME_CLASS(theClass##ThemeFactory);\
}

#define DECLARE_THEME_REFRESH(theClass)\
protected:\
virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);

#define IMPLEMENT_THEME_REFRESH(theClass, theBase)\
BOOL theClass::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult) {\
	if (message == WM_SETTINGCHANGE || message == WM_SYSCOLORCHANGE)\
	{\
		GetTheme()->RefreshMetrics();\
		Invalidate(FALSE);\
	}\
	return theBase::OnWndMsg(message, wParam, lParam, pResult);\
}

#define CXTThemeManagerStyleHostBase CXTThemeManagerStyleHost

//}}AFX_CODEJOCK_PRIVATE

//////////////////////////////////////////////////////////////////////////

AFX_INLINE XTThemeStyle CXTThemeManagerStyle::GetTheme() {
	return m_enumTheme;
}
AFX_INLINE CXTThemeManagerStyleFactory* CXTThemeManagerStyle::GetThemeFactory() {
	return m_pFactory;
}
AFX_INLINE XTThemeStyle CXTThemeManager::GetTheme() {
	return m_enumTheme;
}
AFX_INLINE void CXTThemeManagerStyleFactory::OnCreateSafeTheme(CXTThemeManagerStyle* pTheme, XTThemeStyle theme) {
	pTheme->m_enumTheme = theme;
}

//////////////////////////////////////////////////////////////////////

#endif // !defined(__XTTHEMEMANAGER_H__)
