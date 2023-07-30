#pragma once
#include "afxcmn.h"
#include "afxwin.h"


#define		MAX_SLIDER_COUNT		8

class CMaterialEditDlg : public CDialog
{
	DECLARE_DYNAMIC(CMaterialEditDlg)

public:
	CMaterialEditDlg(CWnd* pParent = NULL);  
	virtual ~CMaterialEditDlg();

	enum { IDD = IDD_MATERIAL_EDIT };

	// Clump Setting
	BOOL		SetClump				( RpClump*	pClump );

	// Slider Setting
	BOOL		SetSliderWindow			( INT nIndex , CONST CString& strSliderName , INT nMin , INT nMax , BOOL bShowWindow );

	// Slider Disable
	VOID		DisableSliderWindow		( INT nIndex );
	VOID		AllDisableSliderWindow	( VOID );

	INT			GetEnableSliderCount	( VOID );

	RpAtomic*	GetIndexAtomic			( INT nIndex );
	RpMaterial*	GetIndexMaterial		( INT nindex );


protected:
	virtual void DoDataExchange(CDataExchange* pDX);  

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL	OnInitDialog();
	afx_msg void	OnBnClickedAdd();
	afx_msg void	OnBnClickedDelete();

	////////////////////////////////////
	// FX Material Edit
	CSliderCtrl			m_SliderCtrl[ MAX_SLIDER_COUNT ];
	CStatic				m_StaticText[ MAX_SLIDER_COUNT ];
	////////////////////////////////////

	CListBox			m_listAtomicName;			//	Atomic 이름			
	CListBox			m_listMaterialName;			//	Material 이름
	CListBox			m_ListBoxMaterial;			//	적용된 FX

	CComboBox			m_comboFX;
	
	CStatic				m_staticClumpName;			//	Clump 이름

	RpClump*			m_pClump;				

protected:
	virtual VOID		OnOK();
	virtual VOID		OnCancel();

	static	RpAtomic*	CBAllAtomic		( RpAtomic*	pAtomic	, PVOID pData );
	static	RpMaterial*	CBAllMaterial	( RpMaterial*	pMaterial , PVOID pData );

public:
	afx_msg void OnLbnSelchangeListAtomicName();
	afx_msg void OnLbnSelchangeListMaterialName();
	
};
