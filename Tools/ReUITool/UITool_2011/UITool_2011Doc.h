
// UITool_2011Doc.h : CUITool_2011Doc 클래스의 인터페이스
//


#pragma once

#include "ObjectBorder.h"
#include "UIToolBaseObject.h"
#include "MainFrm.h"

class CUITool_2011Doc : public CDocument
{
protected: // serialization에서만 만들어집니다.
	CUITool_2011Doc();
	DECLARE_DYNCREATE(CUITool_2011Doc)

// 특성입니다.
public:

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// 구현입니다.
public:
	virtual ~CUITool_2011Doc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CObjectBorder						Border;
	CUIToolBaseObject*					m_CurrentPickObj;

public:
	std::list<CWnd*>				m_UIList;
	std::list<CUIToolBaseObject*>			m_ObjectList;
	bool							m_bTrack;

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()

public:
	void CurrentPickUpObjUpdate(CUIToolBaseObject* pObj, BOOL bFileViewUpdate = TRUE);
	void AddObject(CPoint pStartpos, CPoint pEndpos);
	void Render(CBufferDC* pDC);

	CUIToolBaseObject* GetPickUpObject(){ return m_CurrentPickObj; }
	CUIToolBaseObject* GetItemInTheList(CString pItemID);
};


