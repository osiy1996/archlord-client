#pragma once


// CNewStatic
// �߱� Patch Client�� �����°� ������ ����ƽ ��Ʈ��
// (�����ڵ�)������� ������Ʈ�� �ٲ㵵 ������ ��û�� �ð��� �ʿ��ؼ� �̰ɷ� ��ü�Ѵ� �� ��
// Owner Draw ����

class CNewStatic : public CStatic
{
	DECLARE_DYNAMIC(CNewStatic)

public:
	CNewStatic();
	virtual ~CNewStatic();

	VOID				SetText( wchar_t*	pszText )	{	m_TextData	=	pszText;	}
	const CStringW		GetText( VOID )					{	return m_TextData;			}

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void	OnPaint			( VOID );
	afx_msg BOOL	OnEraseBkgnd	( CDC* pDC );

	CStringW	m_TextData;

	
};


