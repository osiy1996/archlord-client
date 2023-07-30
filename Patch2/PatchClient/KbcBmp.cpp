// KbcBmp.cpp : implementation file
//

#include "stdafx.h"
#include "KbcBmp.h"

#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKbcBmp

IMPLEMENT_DYNCREATE(CKbcBmp, CView)

CKbcBmp::CKbcBmp()
{
	m_FontColor = RGB(255, 255, 255);
}

CKbcBmp::~CKbcBmp()
{
}

BEGIN_MESSAGE_MAP(CKbcBmp, CView)
	//{{AFX_MSG_MAP(CKbcBmp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKbcBmp drawing

void CKbcBmp::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CKbcBmp diagnostics

#ifdef _DEBUG
void CKbcBmp::AssertValid() const
{
	CView::AssertValid();
}

void CKbcBmp::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CKbcBmp message handlers

// �����̸����� ���� �ε��� ��..
void CKbcBmp::LoadBitmap(UINT nResourceID)
{
	m_Bitmap.LoadBitmap(nResourceID);
	m_Bitmap.GetBitmap(&m_BmpInfo);	

	m_nWidth		= m_BmpInfo.bmWidth;
	m_nSliceWidth	= (int)(m_BmpInfo.bmWidth/4);
	m_nHeight		= m_BmpInfo.bmHeight;


	m_nMode = 0;
}

// �����̸����� ���� �ε��� ��..
void CKbcBmp::LoadBitmap(CString strFileName)
{
	// �̹����� �ҷ��´�.
	m_hBitmap = (HBITMAP)::LoadImage(
		AfxGetInstanceHandle(),
		strFileName,
		IMAGE_BITMAP,0,0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);

	// �̹����� ũ�⸦ ���ϱ� ����..
	(CBitmap::FromHandle(m_hBitmap))->GetBitmap(&m_BmpInfo);
	
	m_nWidth		= m_BmpInfo.bmWidth;
	m_nSliceWidth	= (int)(m_BmpInfo.bmWidth/4);
	m_nHeight		= m_BmpInfo.bmHeight;


	m_nMode = 1;
}

// ���Ͽ��� ���� �ҷ� �� ��...
void CKbcBmp::Draw(CDC *pDC,int nX,int nY)
{
	CDC					oriDC;
	CBitmap				*poldoriDC;

	UINT BMPWIDTH		= m_nWidth;
	UINT BMPHEIGHT		= m_nHeight;

	// ���� �̹����� oriDC���ٰ� �������ش�.
	// �װ��� srcDC���ٰ� �����ؼ� ��ߵȴ�. �ȱ׷��� ���� �̹����� ���ϴ� ���� �߻��Ѵ�. 
	// �̹��� ���� ������ �ε� �س��� �װ��� ���纻�� ����� ���� �ű⼭ ��������..
	oriDC.CreateCompatibleDC(pDC);
	poldoriDC = oriDC.SelectObject( m_nMode ? CBitmap::FromHandle(m_hBitmap) : &m_Bitmap );

	pDC->BitBlt( nX, nY,BMPWIDTH,BMPHEIGHT,&oriDC,0,0,SRCCOPY);	 

	oriDC.SelectObject(poldoriDC);
}

// ���Ͽ��� ���� �ҷ� �� ��...
void CKbcBmp::Draw(CDC *pDC,int nX,int nY,UINT nMask)
{
	UINT BLACK			= RGB(0,0,0);
	UINT WHITE			= RGB(255,255,255);
	UINT BMPWIDTH		= m_nWidth;
	UINT BMPHEIGHT		= m_nHeight;
	UINT MASK			= nMask;

	CDC					oriDC;
	CDC					srcDC;
	CDC					maskDC;
	CDC					memDC;
	CBitmap				srcBmp;
	CBitmap				maskBmp;	
	CBitmap				memBmp;
	CBitmap				*poldoriDC;
	CBitmap				*poldsrcDC;
	CBitmap				*poldmaskDC;
	CBitmap				*poldmemDC;
	
	// ���� �̹����� oriDC���ٰ� �������ش�.
	// �װ��� srcDC���ٰ� �����ؼ� ��ߵȴ�. �ȱ׷��� ���� �̹����� ���ϴ� ���� �߻��Ѵ�. 
	// �̹��� ���� ������ �ε� �س��� �װ��� ���纻�� ����� ���� �ű⼭ ��������..
	oriDC.CreateCompatibleDC(pDC);
	poldoriDC = oriDC.SelectObject( m_nMode ? CBitmap::FromHandle(m_hBitmap) : &m_Bitmap );

	// oriDC �̹����� srcDC ���ٰ� �������ش�..
	srcDC.CreateCompatibleDC(pDC);
	srcBmp.CreateCompatibleBitmap(pDC,BMPWIDTH,BMPHEIGHT);
	poldsrcDC = srcDC.SelectObject(&srcBmp);
	srcDC.BitBlt(0,0,BMPWIDTH,BMPHEIGHT,&oriDC,0,0,SRCCOPY);

	// ����ũ�� ������ DC, 
	// mask�� �׸��� �׷��� �κ��� Black, ����� White�� ǥ�õ� �׸��̴�.
	maskDC.CreateCompatibleDC(pDC);
	maskBmp.CreateBitmap(BMPWIDTH,BMPHEIGHT,1,1,NULL);
	poldmaskDC = maskDC.SelectObject(&maskBmp);

	// ������(source) �׸��� ����� �����س��� BitBlt�ϸ� 
	// ������ ���, �ƴѰ��� ��� ���������� ������ ���ش�.
	// ������ �ؼ� SRCCOPY�ϸ� mask �̹����� �ϼ��ȴ�.
	srcDC.SetBkColor(MASK);
	maskDC.BitBlt(0,0,BMPWIDTH,BMPHEIGHT,&srcDC,0,0,SRCCOPY);


	// ������ ���� DC�� ���� ������ ������ ���� BitBlt �ϸ�
	// �����ϴ� �׸��� White �� ���� ���� DC�� Background�� ������ ��ȯ �ǰ�
	// �����ϴ� �׸��� Black �� ���� ���� DC�� Foreground�� ������ ��ȯ �ȴ�.
	// �̷��� �Ǹ� maskDC �� �׸����� �����Ǵ� �׸��� ������ �ȴ�.
	// �̰��� SRCAND������ �ϸ� ������ srcDC �� �ִ� �׸��� �������鼭
	// ���� �׸��� ����� �������� �ǹ����� �ƴѰ��� ���� �״�� ǥ�ð� �ȴ�.
	srcDC.SetBkColor(BLACK);
	srcDC.SetTextColor(WHITE);
	srcDC.BitBlt(0,0,BMPWIDTH,BMPHEIGHT,&maskDC,0,0,SRCAND);

	// ���� ��� �غ� �Ǿ���. �켱 ���(�̰��� ���� ȭ�鿡 ǥ�õǴ� ������� �츮�� �ø����� �ϴ� �̹����� ����� �ƴϴ�.)
	// ����� ������ ���´�. memDC��..		
	memDC.CreateCompatibleDC(pDC);		
	memBmp.CreateCompatibleBitmap(pDC,BMPWIDTH,BMPHEIGHT);
	poldmemDC = memDC.SelectObject(&memBmp);
	memDC.BitBlt(0,0,BMPWIDTH,BMPHEIGHT,pDC, nX, nY,SRCCOPY);
	
	// �׷����� maskDC�� AND �������� ���� ȭ���� ���� ��ġ�� 
	// �׸��κи� ���������� ǥ�õǰ� ������ ����ְ� �ȴ�. 
	// ���⿡�ٰ� srcDC�� �̹����� OR ������ SRCPATINT �� ������ �ϰ� �Ǹ�
	// ������ �״�� ����ִ� �̹����� ��µȴ�.
	memDC.BitBlt(0,0,BMPWIDTH,BMPHEIGHT,&maskDC,0,0,SRCAND);
	memDC.BitBlt(0,0,BMPWIDTH,BMPHEIGHT,&srcDC,0,0,SRCPAINT);

	pDC->BitBlt( nX, nY,BMPWIDTH,BMPHEIGHT,&memDC,0,0,SRCCOPY);	 

	oriDC.SelectObject(poldoriDC);
	srcDC.SelectObject(poldsrcDC);
	maskDC.SelectObject(poldmaskDC);
	memDC.SelectObject(poldmemDC);
}

// CKbcBmpButton ���� ȣ���� �� ���� �Լ�.. �׸� ������ ���������� ���� ���� �׷��ش�.nSeq�� ����..
void CKbcBmp::Draw(CDC *pDC,int nSeq,int nX,int nY,UINT nMask, char *pstrText)
{
	UINT BLACK			= RGB(0,0,0);
	UINT WHITE			= RGB(255,255,255);
	UINT BMPWIDTH		= m_nWidth;
	UINT BMPHEIGHT		= m_nHeight;
	UINT MASK			= nMask;
	UINT SLICEWIDTH		= m_nSliceWidth;
	UINT INTERVAL		= SLICEWIDTH * nSeq;

	CDC					oriDC;
	CDC					srcDC;
	CDC					maskDC;
	CDC					memDC;
	CBitmap				srcBmp;
	CBitmap				maskBmp;	
	CBitmap				memBmp;
	CBitmap				*poldoriDC;
	CBitmap				*poldsrcDC;
	CBitmap				*poldmaskDC;
	CBitmap				*poldmemDC;

	// ���� �̹���.. 4�� ��ư�� �׸��� �� ��� �ִ� ���� �ҷ��´�.
	// oriDC���� ��ü �������� �׸��� �� ����ִ�.
	oriDC.CreateCompatibleDC(pDC);
	poldoriDC = oriDC.SelectObject(m_nMode ? CBitmap::FromHandle(m_hBitmap) : &m_Bitmap);

	// oriDC �� 1/4 �̹����� srcDC ���ٰ� �������ش�..
	srcDC.CreateCompatibleDC(pDC);
	srcBmp.CreateCompatibleBitmap(pDC,SLICEWIDTH,BMPHEIGHT);
	poldsrcDC = srcDC.SelectObject(&srcBmp);
	srcDC.BitBlt(0,0,SLICEWIDTH,BMPHEIGHT,&oriDC,INTERVAL,0,SRCCOPY);

	// ����ũ�� ������ DC, 
	// mask�� �׸��� �׷��� �κ��� Black, ����� White�� ǥ�õ� �׸��̴�.
	maskDC.CreateCompatibleDC(pDC);
	maskBmp.CreateBitmap(SLICEWIDTH,BMPHEIGHT,1,1,NULL);
	poldmaskDC = maskDC.SelectObject(&maskBmp);

	// ������(source) �׸��� ����� �����س��� BitBlt�ϸ� 
	// ������ ���, �ƴѰ��� ��� ���������� ������ ���ش�.
	// ������ �ؼ� SRCCOPY�ϸ� mask �̹����� �ϼ��ȴ�.
	srcDC.SetBkColor(MASK);
	maskDC.BitBlt(0,0,SLICEWIDTH,BMPHEIGHT,&srcDC,0,0,SRCCOPY);

	// ������ ���� DC�� ���� ������ ������ ���� BitBlt �ϸ�
	// �����ϴ� �׸��� White �� ���� ���� DC�� Background�� ������ ��ȯ �ǰ�
	// �����ϴ� �׸��� Black �� ���� ���� DC�� Foreground�� ������ ��ȯ �ȴ�.
	// �̷��� �Ǹ� maskDC �� �׸����� �����Ǵ� �׸��� ������ �ȴ�.
	// �̰��� SRCAND������ �ϸ� ������ srcDC �� �ִ� �׸��� �������鼭
	// ���� �׸��� ����� �������� �ǹ����� �ƴѰ��� ���� �״�� ǥ�ð� �ȴ�.
	srcDC.SetBkColor(BLACK);
	srcDC.SetTextColor(WHITE);
	srcDC.BitBlt(0,0,SLICEWIDTH,BMPHEIGHT,&maskDC,0,0,SRCAND);

	// ���� ��� �غ� �Ǿ���. �켱 ���(�̰��� ���� ȭ�鿡 ǥ�õǴ� ������� �츮�� �ø����� �ϴ� �̹����� ����� �ƴϴ�.)
	// ����� ������ ���´�. memDC��..		
	memDC.CreateCompatibleDC(pDC);		
	memBmp.CreateCompatibleBitmap(pDC,SLICEWIDTH,BMPHEIGHT);
	poldmemDC = memDC.SelectObject(&memBmp);
	memDC.BitBlt(0,0,SLICEWIDTH,BMPHEIGHT,pDC, nX, nY,SRCCOPY);
	
	// �׷����� maskDC�� AND �������� ���� ȭ���� ���� ��ġ�� 
	// �׸��κи� ���������� ǥ�õǰ� ������ ����ְ� �ȴ�. 
	// ���⿡�ٰ� srcDC�� �̹����� OR ������ SRCPATINT �� ������ �ϰ� �Ǹ�
	// ������ �״�� ����ִ� �̹����� ��µȴ�.
	memDC.BitBlt(0,0,SLICEWIDTH,BMPHEIGHT,&maskDC,0,0,SRCAND);
	memDC.BitBlt(0,0,SLICEWIDTH,BMPHEIGHT,&srcDC,0,0,SRCPAINT);

	pDC->BitBlt( nX, nY,SLICEWIDTH,BMPHEIGHT,&memDC,0,0,SRCCOPY);	 

	if( pstrText != NULL )
	{
		RECT		cRect;

		cRect.left = nX;
		cRect.top = nY;
		cRect.right = m_nWidth/4;
		cRect.bottom = m_nHeight + nY;

		pDC->SetTextColor( m_FontColor );
		pDC->SetBkMode( TRANSPARENT );
		pDC->DrawText( pstrText, &cRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE );
	}

	oriDC.SelectObject(poldoriDC);
	srcDC.SelectObject(poldsrcDC);
	maskDC.SelectObject(poldmaskDC);
	memDC.SelectObject(poldmemDC);
}
