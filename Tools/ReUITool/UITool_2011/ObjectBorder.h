#pragma once


#define BORDER_SIZE 2

class CObjectBorder
{
public:
	CObjectBorder(void);
	~CObjectBorder(void);

	CRect Qurd;
	void InitBorder(CPoint pStart, CPoint pEnd);
	void Render(CBufferDC* pDC);

	BOOL m_bDraw;
};
