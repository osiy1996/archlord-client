#ifndef		_ACUITEXTURE_H_
#define		_ACUITEXTURE_H_

#include <AgcmUIControl/AcUIBase.h>

//�ܺο� �ִ� Texture Pointer�� �޾Ƽ� Render�� ���ش�. ( Texture ����, �Ҹ���� �������� �ʴ´�. )

enum 
{
	UICM_TEXTURE_MAX_MESSAGE						= UICM_BASE_MAX_MESSAGE
};

class AcUITexture : public AcUIBase
{
public:
	AcUITexture( void );
	virtual ~AcUITexture( void );

protected:
	RwTexture*										m_pTexture;		// �� ���� NULL�̸� �׸��� �ʴ´�.
	UINT8											m_cAlphaValue;	// Im2D Render�� Alpha Value
	DWORD											m_lColorValue;	// Im2D Render�� Color Value 

public:
	void				SetTexture					( RwTexture* pTexture, UINT32 lValidWidth = 0, UINT32 lValidHeight = 0 );
	void				SetAlphaValue				( UINT8 cAlphaValue );
	void				SetColorValue				( DWORD lColorValue );
	RwTexture*			GetTexturePointer			( void );
		
//virtual fuction
public:
	virtual void		OnWindowRender				( void );
};

#endif //_ACUITEXTURE_H_