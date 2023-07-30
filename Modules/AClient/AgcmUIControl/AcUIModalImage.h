#pragma once

#include <AgcModule/AgcWindow.h>
#include <AuList.h>
#include <rwcore.h>
#include <AgcmUIControl/AcUIBase.h>

struct RsMouseStatus;

enum 
{
	UICM_MODALIMAGE_MAX_MESSAGE						= UICM_BASE_MAX_MESSAGE
};

class AcUIModalImage : public AgcModalWindow
{
public:
	AcUIModalImage( void );
	virtual ~AcUIModalImage( void );

	// AcUIImage ��.
	virtual void			OnAddImage				( RwTexture* pTexture );

	// Parent Override.
	virtual	void 			OnPostRender			( RwRaster *raster );	// ���� ������ ��.
	virtual void 			OnWindowRender			( void );
	virtual	void 			OnClose					( void );				// ������ , �׷��� ����Ÿ ������. WindowUI �� ��� Close�Ŵ� ����,  FullUI�� ��� �ٸ� UI�� Setting�ű� ��.
	virtual	BOOL 			OnLButtonDown			( RsMouseStatus *ms	);
	virtual	BOOL 			OnMouseMove				( RsMouseStatus *ms	);

	INT32					AddImage				( char * filename );
	RwTexture*				GetImage				( INT32 index );
	INT32					AddOnlyThisImage		( char* filename );		// List�� ��� �̹����� ����� �Ѿ�� �̹����� ��´�. 
	void					DestroyTextureList		( void );

public:
	static class AgcmSound*		m_pAgcmSound;
	static class AgcmFont*		m_pAgcmFont;

	BOOL					m_bPassPostMessage;							// Post Message�� Parent���� �ѱ���ΰ�?

protected:
	AuList< RwTexture* >							m_listTexture;
};
