#pragma once
#include "Utility.h"

// CUIToolBaseObject

enum E_State
{
	EOS_DEFAULT = 10,
	EOS_MOUSEON,
	EOS_CLICK,
	EOS_DISABLE,
};

enum E_MousePosition
{
	E_PTPT = 100, // 왼쪽상단
	E_PTST, // 왼쪽하단
	E_STPT, // 오른쪽상단
	E_STST, // 오른쪽하단
	E_NTNT,
};
struct MessageACt
{
	CString MessageState;
	std::list<CString> ActionType;

	MessageACt(CString pMsg, CString pAct)
	{
		MessageState = pMsg;
		ActionType.push_back(pAct);
	}
	~MessageACt()
	{
		for(std::list<CString>::iterator iter = ActionType.begin(); iter != ActionType.end();)
		{
			ActionType.erase(iter++);
		}

		ActionType.clear();
	}

	BOOL SearchItemDelete(CString pAct)
	{
		for(std::list<CString>::iterator iter = ActionType.begin(); iter != ActionType.end(); )
		{
			if(pAct.Compare((*iter)) == 0)
			{
				ActionType.erase(iter++);
				return TRUE;
			}
			else
				iter++;
		}
		return FALSE;
	}
};

struct ObjectStatus
{
	CString StateID;
	CPoint StatePos;
	CPoint StateSize;
	CString StateImgPath;
	COLORREF StateColor;
};

class CUIToolBaseObject
{
public:
	CUIToolBaseObject();
	virtual ~CUIToolBaseObject();
	
	virtual void Create(CString pID, CPoint pStart, CPoint pEnd);
	virtual void Render(CBufferDC* pDC);
	virtual void InitProperty(void){}
	virtual	void RemoveProperties(void){}
	virtual void FindPropertyValueChange(void);
	virtual void PropertyChange(CString pStr, CMFCPropertyGridProperty* pProp);

	void SetID(CString NewID);
	void SetColor(COLORREF pColor);
	void SetParentID(CString ParentID);

	BOOL IsMouseOn(CPoint pPos);

	E_MousePosition IsMouseSideOn(CPoint pPos);

	CUIToolBaseObject* GetItemInTheChildList(CString pItemID);
	CUIToolBaseObject* AllItemCheckPos(CPoint pPoint);	

	void MovePosition(long x, long y);
	CString GetFilePath(CString pStr);

	void SetEnableWindow(bool p_bVisible);
	void SetCaption(CString p_strCaption);
	void SetFontType(CString p_strFontType);

	virtual void SetFontColor(COLORREF p_color){}
	virtual void SetImage(CString p_strimgpath){}

public:
	CString								m_ID;
	CUIToolBaseObject*					m_parent;
	CString								m_PositionType;
	CPoint								m_Position;
	CPoint								m_Absolute_Position;
	CPoint								m_Size;
	COLORREF							m_Color;

	E_State								m_ObjState;
	CString								m_DefaultImg;
	CString								m_MouseOnImg;
	CString								m_ClickImg;
	CString								m_DisableImg;

	E_MousePosition						m_eMousepos;

	CString								m_strCaption;
	CString								m_strFontType;
	LOGFONT								m_logfont;
	int									m_FontSize;
	COLORREF							m_FontColor;

	std::list<CUIToolBaseObject*>		m_ChildList;

	std::list<MessageACt*>				m_MessageAction;
	std::list<ObjectStatus*>			m_StatusEdit;


	bool								m_bVisible;
	bool								m_bUserInput;
	bool								m_bMovalbe;
	bool								m_bModal;

	CString								m_strToolTip;

	CWnd*								m_pCwndObj;
};


