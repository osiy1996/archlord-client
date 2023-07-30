#pragma once

#include "MyEngine.h"
#include "UITileList_PluginBase.h"
#include "AxisManager.h"
#include "HSVControlStatic.h"

#include <vector>

// CPlugin_PointLight

class CPlugin_PointLight : public CUITileList_PluginBase
{
	DECLARE_DYNAMIC(CPlugin_PointLight)

public:
	CPlugin_PointLight();
	virtual ~CPlugin_PointLight();

protected:
	DECLARE_MESSAGE_MAP()

public:
	// ������ ����..
	RpAtomic *	m_pSphereAtomic			;
	INT32		m_nSelectedPoint		;

	INT32		m_nSelectedAxis		;
	FLOAT		m_fSelectedValue1	;
	FLOAT		m_fSelectedValue2	;

	RpAtomic *	TranslateSphere( RwV3d * pPos );
	void		DrawSphere( RwV3d	* pPos );
	INT32		GetCollisionIndex( RsMouseStatus *ms );

	CAxisManager	m_cAxis;

	// Point Light Object Info
	struct	PLOInfo
	{
		INT32				nOID	;
		RpLight	*			pLight	;
		AgcdPointLight *	pstInfo	;

		PLOInfo() : nOID( -1 ) , pLight( NULL ) , pstInfo( NULL ) {}
	};

	vector< PLOInfo >	m_listLight;
	void		RemoveAllLights();

	// Utility Functions
	INT32		GetLightOffset( RpLight *	pLight	);
	INT32		GetLightOffset( INT32		nOID	);
	void		RemoveLightInfo( RpLight *	pLight	);

	// Selected Light Info Update
	void		RadiusUpdate();
	void		PositionUpdate();
	void		ColorUpdate();

	static BOOL	CBSaveDivision( DivisionInfo * pDivisionInfo , PVOID pData );
	static BOOL	CBLoadDivision( DivisionInfo * pDivisionInfo , PVOID pData );
	static BOOL CBPointLightFilter(PVOID pData, PVOID pClass, PVOID pCustData);

public:
	CHSVControlStatic	m_ctlRGBSelectStatic;

public:
	virtual BOOL OnWindowRender			();
	virtual	BOOL Window_OnLButtonDown	( RsMouseStatus *ms );
	virtual	BOOL Window_OnLButtonUp		( RsMouseStatus *ms );
	virtual BOOL Window_OnMouseMove		( RsMouseStatus *ms );
	virtual BOOL Window_OnKeyDown		( RsKeyStatus *ks );

	virtual BOOL OnQuerySaveData		( char * pStr );
	virtual BOOL OnSaveData				();
	virtual BOOL OnLoadData				();

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();

	// Add & Remove
	static BOOL	CBOnPointLightAddObject		( PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBOnPointLightRemoveObject	( PVOID pData, PVOID pClass, PVOID pCustData);

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};

/*

// ������ (2005-04-18 ���� 2:51:14) : 
Point Light ȸ��..
1,������Ʈ�� ������ Light ���� ����
  - ���� 2�� ���� �Ű� �ְ� , �Ŀ� ������ ����..
2,�ٴڿ� �̹��� ����Ʈ���� �� ������ �ȉ´�.
  - ���ý� ������ ������ �Ÿ� ������ �ɵ�..
3,����Ʈ�� ���ϴ�.. 
  - ����Ʈ �� ó�������� �Ϻη� ����Ʈ�� ������ ����߸��� �ִ���.
  - ������ ����Ʈ�� ���� Ÿ���� �ξ� ����ؾ��ҵ�
4,���̳ʽ� ����Ʈ�� �����Ѱ�
  - ������ �����.. �׸��� ���̴��� �̿��� �� �ۿ� ����.
5, ī�� Move Undo. .. 
  - �Ф�..
6,�ð��� ���� on/off
  - ���� ����� �������� ��� �����ҵ�
7,����Ʈ�� ī�޶� ���� �ް� �ȹް� �ϴ� ���װ� �ִ�..
  - ����������� �ذ��ߴ���.
*/
