
// UITool_2011Doc.h : CUITool_2011Doc Ŭ������ �������̽�
//


#pragma once

#include "ObjectBorder.h"
#include "UIToolBaseObject.h"
#include "MainFrm.h"

class CUITool_2011Doc : public CDocument
{
protected: // serialization������ ��������ϴ�.
	CUITool_2011Doc();
	DECLARE_DYNCREATE(CUITool_2011Doc)

// Ư���Դϴ�.
public:

// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// �����Դϴ�.
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

// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()

public:
	void CurrentPickUpObjUpdate(CUIToolBaseObject* pObj, BOOL bFileViewUpdate = TRUE);
	void AddObject(CPoint pStartpos, CPoint pEndpos);
	void Render(CBufferDC* pDC);

	CUIToolBaseObject* GetPickUpObject(){ return m_CurrentPickObj; }
	CUIToolBaseObject* GetItemInTheList(CString pItemID);
};


