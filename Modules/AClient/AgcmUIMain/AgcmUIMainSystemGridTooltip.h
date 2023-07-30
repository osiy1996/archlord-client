#pragma once

#include <AgcmUIControl/AcUIToolTip.h>
#include <AuXmlParser/AuXmlParser.h>
#include <string>
#include <vector>

class AgcmUIManager2;

#define		TooltipLineList			std::list< stTooltipLine >				
#define		TooltipLineListIter		std::list< stTooltipLine >::iterator


struct	stTooltipLine 
{
	string					strLine;
	DWORD					dwColor;
};


struct	stSystemGridTooltip 
{
	string					strSectionName;
	TooltipLineList			listTooltipLine;	
};


class AgcmUIMainSystemGridTooltip
{
private:
	typedef		std::vector< stSystemGridTooltip* >					SystemGridTooltipVec;
	typedef		std::vector< stSystemGridTooltip* >::iterator		SystemGridTooltipVecIter;

public:
	AgcmUIMainSystemGridTooltip						( VOID );
	~AgcmUIMainSystemGridTooltip					( VOID );

#ifdef _AREA_GLOBAL_
	BOOL					Initialize					( AgcmUIManager2* pUIManager, char* p0File, BOOL bEncrypt = FALSE );
#else
	BOOL					Initialize					( BOOL bEncrypt = FALSE );
#endif
	VOID					Destroy						( VOID );

	BOOL					LoadSystemGridTooltipXML	( IN CONST string&	strFullPathName );

#ifdef _AREA_GLOBAL_
	BOOL					OpenSystemGridTooltip		( IN CONST string&	strSection, UINT nExpireTime = 0 );
#else
	BOOL					OpenSystemGridTooltip		( IN CONST string&	strSection );
#endif
	BOOL					CloseSystemGridTooltip		( VOID );


private:
	stTooltipLine*			_GetString					( IN CONST string&	strSection , INT nIndex );
	DWORD					_LineColor					( CONST string&	strLineColor );
#ifdef _AREA_GLOBAL_
	std::string				_MakeExpireTimeString		( UINT32 nExpireTime );
#endif

	AuXmlDocument			m_XMLDocument;
	AuXmlNode*				m_XMLRootNode;

	AcUIToolTip				m_csSystemGridToolTip;		//	System Grid Tooltip Window	

	SystemGridTooltipVec	m_vecSysGridTooltip;		//	System Grid °ü¸® º¤ÅÍ
#ifdef _AREA_GLOBAL_
	AgcmUIManager2*			m_pcsAgcmUIManager2;
#endif
};
