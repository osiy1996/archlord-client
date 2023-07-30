#pragma once


#include <list>
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>


struct stRenderState
{
	UINT		uState;
	UINT		uValue;

	UINT		uOriValue;
};

struct stTextureState
{
	UINT		uStage;
	UINT		uType;
	UINT		uValue;

	UINT		uOriValue;
};

class	AcuRenderState
{
private:
	typedef std::list< stRenderState >					RenderStateList;
	typedef std::list< stRenderState >::iterator		RenderStateListIter;

	typedef std::list< stTextureState >					TextureStateList;
	typedef std::list< stTextureState >::iterator		TextureStateListIter;

public:
	AcuRenderState				( VOID );
	~AcuRenderState				( VOID );

	BOOL	Initialize			( LPDIRECT3DDEVICE9	pd3dDevice	);
	VOID	Destroy				( VOID );

	// Instance Add , Del
	BOOL	AddRenderState		( UINT uState , UINT uValue );
	BOOL	AddTextureState		( UINT uStage , UINT uType , UINT uValue );

	BOOL	DelRenderState		( VOID );
	BOOL	DelTextureState		( VOID );

	// Render State Apply
	VOID	RenderStateApply	( VOID );

	// Render State Restore
	VOID	RenderStateRestore	( VOID );

	// Set Shader
	VOID	SetPixelShader		( LPDIRECT3DPIXELSHADER9	pd3dPixelShader );
	VOID	SetVertexShader		( LPDIRECT3DVERTEXSHADER9 pd3dVertexShader );

	LPDIRECT3DPIXELSHADER9		GetPixelShader( VOID );
	LPDIRECT3DVERTEXSHADER9		GetVertexShader( VOID );

private:
	VOID	_ApplyRenderState		( stRenderState&	RenderState );
	VOID	_RestoreRenderState		( stRenderState&	RenderState );

	VOID	_ApplyTextureState		( stTextureState&	TextureState );
	VOID	_RestoreTextureState	( stTextureState&	TextureState );

	LPDIRECT3DDEVICE9				m_pd3dDevice;

	LPDIRECT3DPIXELSHADER9			m_pd3dPS;
	LPDIRECT3DVERTEXSHADER9			m_pd3dVS;

	RenderStateList					m_RenderStateList;
	TextureStateList				m_TextureStateList;
};
