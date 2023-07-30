#include "AcuRenderState.h"

AcuRenderState::AcuRenderState( VOID )
:	m_pd3dDevice(NULL) , m_pd3dPS(NULL) , m_pd3dVS(NULL)
{

}

AcuRenderState::~AcuRenderState( VOID )
{
	Destroy();
}

BOOL	AcuRenderState::Initialize( LPDIRECT3DDEVICE9	pd3dDevice )
{
	if( !pd3dDevice )
		return FALSE;

	m_pd3dDevice		=	pd3dDevice;

	return TRUE;
}

VOID	AcuRenderState::Destroy( VOID )
{
	m_RenderStateList.clear();
	m_TextureStateList.clear();

}

BOOL	AcuRenderState::AddRenderState( UINT uState , UINT uValue )
{
	if( !m_pd3dDevice )
		return FALSE;

	DWORD			dwOriValue	=	0;
	stRenderState	RenderState;

	m_pd3dDevice->GetRenderState( (D3DRENDERSTATETYPE)uState , &dwOriValue );

	RenderState.uOriValue	=	dwOriValue;
	RenderState.uState		=	uState;
	RenderState.uValue		=	uValue;

	m_RenderStateList.push_back( RenderState );

	return TRUE;
}

BOOL	AcuRenderState::AddTextureState( UINT uStage , UINT uType , UINT uValue )
{
	if( !m_pd3dDevice )
		return FALSE;

	DWORD			dwOriValue		=	0;
	stTextureState	TextureState;

	m_pd3dDevice->GetTextureStageState( uStage , (D3DTEXTURESTAGESTATETYPE)uType , &dwOriValue );

	TextureState.uOriValue	=	dwOriValue;
	TextureState.uStage		=	uStage;
	TextureState.uType		=	uType;
	TextureState.uValue		=	uValue;

	m_TextureStateList.push_back( TextureState );

	return TRUE;
}

BOOL	AcuRenderState::DelRenderState( VOID )
{
	return TRUE;
}

BOOL	AcuRenderState::DelTextureState( VOID )
{
	return TRUE;
}

VOID	AcuRenderState::RenderStateApply( VOID )
{
	m_pd3dDevice->SetVertexShader( m_pd3dVS );
	m_pd3dDevice->SetPixelShader( m_pd3dPS );

	RenderStateListIter	RenderIter	=	m_RenderStateList.begin();
	for( ; RenderIter != m_RenderStateList.end() ; ++RenderIter )
	{
		_ApplyRenderState( (*RenderIter) );
	}

	TextureStateListIter	TextureIter	=	m_TextureStateList.begin();
	for( ; TextureIter != m_TextureStateList.end() ; ++TextureIter )
	{
		_ApplyTextureState( (*TextureIter) );
	}
}

VOID	AcuRenderState::RenderStateRestore( VOID )
{
	m_pd3dDevice->SetVertexShader( NULL );
	m_pd3dDevice->SetPixelShader( NULL );

	RenderStateListIter	RenderIter	=	m_RenderStateList.begin();
	for( ; RenderIter != m_RenderStateList.end() ; ++RenderIter )
	{
		_RestoreRenderState( (*RenderIter) );
	}

	TextureStateListIter	TextureIter	=	m_TextureStateList.begin();
	for( ; TextureIter != m_TextureStateList.end() ; ++TextureIter )
	{
		//_RestoreTextureState( (*TextureIter) );
	}
}

VOID	AcuRenderState::SetPixelShader( LPDIRECT3DPIXELSHADER9	pd3dPixelShader )
{
	m_pd3dPS	=	pd3dPixelShader;

	m_pd3dDevice->SetPixelShader( pd3dPixelShader );
}

VOID	AcuRenderState::SetVertexShader( LPDIRECT3DVERTEXSHADER9 pd3dVertexShader )
{
	m_pd3dVS	=	pd3dVertexShader;

	m_pd3dDevice->SetVertexShader( pd3dVertexShader );
}

VOID	AcuRenderState::_ApplyRenderState( stRenderState& RenderState )
{
	m_pd3dDevice->SetRenderState( (D3DRENDERSTATETYPE)RenderState.uState , RenderState.uValue );
}

VOID	AcuRenderState::_RestoreRenderState( stRenderState& RenderState )
{
	m_pd3dDevice->SetRenderState( (D3DRENDERSTATETYPE)RenderState.uState , RenderState.uOriValue );
}

VOID	AcuRenderState::_ApplyTextureState( stTextureState& TextureState )
{
	m_pd3dDevice->SetTextureStageState( TextureState.uStage , (D3DTEXTURESTAGESTATETYPE)TextureState.uType , TextureState.uValue );
}

VOID	AcuRenderState::_RestoreTextureState( stTextureState& TextureState )
{
	m_pd3dDevice->SetTextureStageState( TextureState.uStage , (D3DTEXTURESTAGESTATETYPE)TextureState.uType , TextureState.uOriValue );
}