// AcUICheckBox.h: interface for the AcUICheckBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACUICHECKBOX_H__1E6E05AF_CE71_4937_AA9D_998AFED4495C__INCLUDED_)
#define AFX_ACUICHECKBOX_H__1E6E05AF_CE71_4937_AA9D_998AFED4495C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "AcUIBase.h"


#define	ACUICHECKEDBOX_H_MAX_STRING				256

enum 
{
	ACUICHECKBOX_MODE_UNCHECK_NORMAL			= UICM_BASE_MAX_MESSAGE,
	ACUICHECKBOX_MODE_UNCHECK_ACTIVE,
	ACUICHECKBOX_MODE_UNCHECK_DISABLE,
	ACUICHECKBOX_MODE_CHECK_NORMAL,
	ACUICHECKBOX_MODE_CHECK_ACTIVE,
	ACUICHECKBOX_MODE_CHECK_DISABLE		
};

// Command Message
enum 
{
	UICM_CHECKBOX_ON_CHECK						= 0,
	UICM_CHECKBOX_ON_UNCHECK,
	UICM_CHECKBOX_MAX_MESSAGE
};

class AcUICheckBox : public AcUIBase
{
public:
	AcUICheckBox( void );
	virtual ~AcUICheckBox( void );

// Virtual Functions
public:
	void				OnAddImage				( RwTexture* pTexture );
	void				OnPostRender			( RwRaster *raster );			// ���� ������ ��.
	void				OnWindowRender			( void );
	BOOL				OnLButtonDown			( RsMouseStatus *ms	);			// Left Button Down �� 
	void				OnSetFocus				( void );
	void				OnKillFocus				( void );
	BOOL				OnMouseMove				( RsMouseStatus *ms	);
	
// Button Controll
public :
	BOOL				GetCheckedValue			( void ) { return m_bChecked; }	// üũ�Ǿ������� ���θ� �����޴´�. 

private:	
	BOOL										m_bChecked;						//üũ�Ǿ������� ����
	INT8										m_cCheckedBoxMode;		
	INT32										m_lCheckedBoxIndex[ 6 ];		//Mode ���� enum���� 
	char										m_szCheckedBoxText[ ACUICHECKEDBOX_H_MAX_STRING ];	//Button���� Text���� 
	INT32										m_lTextPos_x;					//Text�� X��ǥ 
	INT32										m_lTextPos_y;					//Text�� Y��ǥ 
	BOOL										m_bTextWrite;					//Text�� ����� ������ 

	float										m_fTextureWidth	;				// Check Box Texture �� Width
	float										m_fTextureHeight;				// Check Box Texture �� Height 

/**

	Not Use...
public :
	void				SetCheckedValue			( BOOL bChecked );				// üũ�Ǿ��־�� �ϴ����� ���θ� Set�Ѵ�. 
	void				SetTextUse				( BOOL bTextUse, char* szText, INT32 lTextPosX = 0, INT32 lTextPosY = 0 );
	void				SetCheckedBoxEnable		( BOOL bEnable );				//Ȱ��ȭ ���θ� Set�Ѵ�. 

*/
};

#endif // !defined(AFX_ACUICHECKBOX_H__1E6E05AF_CE71_4937_AA9D_998AFED4495C__INCLUDED_)
