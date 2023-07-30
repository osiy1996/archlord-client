// XTPPropertyGridItemFont.cpp : implementation of the CXTPPropertyGridItemFont class.
//
// This file is a part of the XTREME PROPERTYGRID MFC class library.
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

#include "stdafx.h"
#include "Common/XTPVC80Helpers.h"
#include "Common/XTPDrawHelpers.h"

#include "XTPPropertyGridInplaceEdit.h"
#include "XTPPropertyGridInplaceButton.h"
#include "XTPPropertyGridInplaceList.h"
#include "XTPPropertyGridItem.h"
#include "XTPPropertyGridItemFont.h"
#include "XTPPropertyGrid.h"
#include "XTPPropertyGridDefines.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CXTPPropertyGridItemFont
IMPLEMENT_DYNAMIC(CXTPPropertyGridItemFont, CXTPPropertyGridItem)


CXTPPropertyGridItemFont::CXTPPropertyGridItemFont(LPCTSTR strCaption, LOGFONT& font)
	: CXTPPropertyGridItem(strCaption)
{
	SetFont(font);
	m_nFlags = xtpGridItemHasExpandButton;
	m_clrValue = (COLORREF)-1;
	EnableAutomation();
	m_strDefaultValue = m_strValue;
}

CXTPPropertyGridItemFont::CXTPPropertyGridItemFont(UINT nID, LOGFONT& font)
	: CXTPPropertyGridItem(nID)
{
	SetFont(font);
	m_nFlags = xtpGridItemHasExpandButton;
	m_clrValue = (COLORREF)-1;
	EnableAutomation();
	m_strDefaultValue = m_strValue;
}

CXTPPropertyGridItemFont::~CXTPPropertyGridItemFont()
{
}

/////////////////////////////////////////////////////////////////////////////
//

void CXTPPropertyGridItemFont::SetFont(LOGFONT& font)
{
	MEMCPY_S(&m_lfValue, &font, sizeof(LOGFONT));

	m_strValue = FontToString(m_lfValue);
}

CString CXTPPropertyGridItemFont::FontToString(const LOGFONT& lfValue)
{
	CWindowDC dc(CWnd::GetDesktopWindow());
	int nHeight = -MulDiv(lfValue.lfHeight, 72, dc.GetDeviceCaps(LOGPIXELSY));

	CString strValue;
	strValue.Format(_T("%s; %ipt"), lfValue.lfFaceName, nHeight);

	if (lfValue.lfWeight == FW_BOLD)
		strValue += _T("; bold");

	return strValue;
}

BOOL CXTPPropertyGridItemFont::OnDrawItemValue(CDC& dc, CRect rcValue)
{
	if (m_clrValue == (COLORREF)-1)
		return CXTPPropertyGridItem::OnDrawItemValue(dc, rcValue);

	COLORREF clr = dc.GetTextColor();
	CRect rcSample(rcValue.left - 2, rcValue.top + 1, rcValue.left + 18, rcValue.bottom - 1);
	CXTPPenDC pen(dc, clr);
	CXTPBrushDC brush(dc, m_clrValue);
	dc.Rectangle(rcSample);

	CRect rcText(rcValue);
	rcText.left += 25;

	dc.DrawText(m_strValue, rcText, DT_SINGLELINE | DT_VCENTER);

	return TRUE;
}

CRect CXTPPropertyGridItemFont::GetValueRect()
{
	CRect rcValue(CXTPPropertyGridItem::GetValueRect());
	if (m_clrValue != (COLORREF)-1) rcValue.left += 25;
	return rcValue;
}


UINT_PTR CALLBACK CXTPPropertyGridItemFont::FontDlgProc(HWND hWnd, UINT message, WPARAM, LPARAM)
{
	// special case for WM_INITDIALOG
	if (message == WM_INITDIALOG)
	{
		HWND hWndCombo = GetDlgItem(hWnd, 1139);
		if (hWndCombo)
			EnableWindow(hWndCombo, FALSE);

		CDialog* pDlg = DYNAMIC_DOWNCAST(CDialog, CWnd::FromHandlePermanent(hWnd));
		if (pDlg != NULL)
			return (UINT_PTR)pDlg->OnInitDialog();
		else
			return 1;
	}

	return 0;

}

void CXTPPropertyGridItemFont::OnInplaceButtonDown(CXTPPropertyGridInplaceButton* pButton)
{
	if (m_pGrid->SendNotifyMessage(XTP_PGN_INPLACEBUTTONDOWN, (LPARAM)pButton) == TRUE)
		return;

	if (!OnRequestEdit())
		return;

	InternalAddRef();

	CFontDialog dlg(&m_lfValue, CF_EFFECTS | CF_SCREENFONTS, NULL, m_pGrid);

	if (m_clrValue == (COLORREF)-1)
	{
		dlg.m_cf.lpfnHook = FontDlgProc;
	}
	else
	{
		dlg.m_cf.rgbColors = m_clrValue;
	}

	if (dlg.DoModal() == IDOK)
	{
		LOGFONT lf;
		dlg.GetCurrentFont(&lf);

		CString strValue = FontToString(lf);

		if (OnAfterEdit(strValue))
		{
			SetFont(lf);

			if (m_clrValue != (COLORREF)-1)
				m_clrValue = dlg.GetColor();

			OnValueChanged(strValue);
			SAFE_INVALIDATE(m_pGrid);
		}
	}
	else
	{
		OnCancelEdit();
	}

	InternalRelease();
}

void CXTPPropertyGridItemFont::SetColor(COLORREF clr)
{
	m_clrValue = clr;
	SAFE_INVALIDATE(m_pGrid);
}

COLORREF CXTPPropertyGridItemFont::GetColor()
{
	return m_clrValue;
}

void CXTPPropertyGridItemFont::GetFont(LOGFONT* lf)
{
	ASSERT(lf != NULL);
	MEMCPY_S(lf, &m_lfValue, sizeof(LOGFONT));
}

