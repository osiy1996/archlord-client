#include "StdAfx.h"
#include "ObjectBorder.h"

CObjectBorder::CObjectBorder(void)
:m_bDraw(FALSE)
{
}

CObjectBorder::~CObjectBorder(void)
{
}

void CObjectBorder::InitBorder(CPoint pStart, CPoint pEnd)
{
	Qurd.left = pStart.x - 6;
	Qurd.top = pStart.y - 6;
	pEnd += pStart;
	Qurd.right = pEnd.x + 6;
	Qurd.bottom = pEnd.y + 6;

	m_bDraw = TRUE;
}
void CObjectBorder::Render(CBufferDC* pDC)
{
	if(!m_bDraw)
	{
		return;
	}
	Graphics gdi(pDC->m_hDC); // gdi °´Ã¼ ¼±¾ð

	Pen BlackPen(Color(255, 0, 0, 255), 5.0f);

	BlackPen.SetDashStyle(DashStyleDot);

	gdi.DrawLine(&BlackPen, Point(Qurd.left, Qurd.top), Point(Qurd.right, Qurd.top));
	gdi.DrawLine(&BlackPen, Point(Qurd.right, Qurd.top), Point(Qurd.right, Qurd.bottom));
	gdi.DrawLine(&BlackPen, Point(Qurd.left, Qurd.bottom), Point(Qurd.right, Qurd.bottom));
	gdi.DrawLine(&BlackPen, Point(Qurd.left, Qurd.top), Point(Qurd.left, Qurd.bottom));

}
