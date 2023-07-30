#ifndef		_ACUPARTICLEDRAW_H_
#define		_ACUPARTICLEDRAW_H_

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AcuParticleDrawD" )
#else
#pragma comment ( lib , "AcuParticleDraw" )
#endif
#endif

#include <d3d9.h>
#include <d3dx9.h>
#include <rwcore.h>

typedef struct
{
	D3DXVECTOR3		position;
	D3DCOLOR		color;
	float			u,v;
} VERTEX_PARTICLE;
#define		D3DFVF_PARTICLE (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

// �ϵ���� particle������..���
typedef struct 
{
    D3DXVECTOR3 position;
    float       pointsize;
    D3DCOLOR    color;
} VERTEX_PARTICLE_PSIZE;
#define D3DFVF_PARTICLE_PSIZE (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_PSIZE)

enum	ParticleBillboardType
{
	PARTICLE_BILLBOARD = 0,
	PARTICLE_YBILLBOARD = 1,		// y�� ������
};

// ����: particle�� uv���� �׻� 0 1 �� set�ǹǷ� �ؽ��� 1������ set����
//		 PointSprite�� ó���ÿ��� 64�� �ִ�� ����.. ��κ� 64���� �����Ѵ�..
class	AcuParticleDraw
{
public:
	AcuParticleDraw();
	~AcuParticleDraw();

	static	void	Init();
	static	void	Release();

	static	void	CameraMatrixUpdate();

	static	void	DrawStart(RwTexture*	pTexture,ParticleBillboardType	eBillboardType);
	static	void	AccumulateBuffer(RwV3d*		pPos,DWORD	color,FLOAT	fWidth,FLOAT	fHeight);
	static	void	DrawEnd();

	static	void	AccumulateBufferRotationSelf(RwV3d*		pPos,DWORD	color,FLOAT	fWidth,FLOAT	fHeight,FLOAT	rot_val);
	static	void	AccumulateBufferRotationMatrix(RwV3d*		pPos,DWORD	color,FLOAT	fWidth,FLOAT	fHeight,RwMatrix*	rot_matrix);
	
	// �ѱ� group�� �Ѱ� pointsize�� �ѱ��.. �� ��� 64�� �ѱ�� �ȴ�..
	static	void	DrawStartPoint(RwTexture*	pTexture,INT32	iMaxPointSize);
	static	void	AccumulateBufferPoint(RwV3d*		pPos,DWORD	color,FLOAT	fSize);
	static	void	DrawEndPoint();

	static	void	SetWorldCamera(RwCamera*	pCam);
	
public:
	// Vertex Buffer
	static		VERTEX_PARTICLE*			m_pBuffer;
	static		INT16*						m_pIndexBuffer;
	static		VERTEX_PARTICLE_PSIZE*		m_pBuffer_PSize;

	static		INT32		m_iMaxBufferVerticeIndex;
	static		INT32		m_iCurBufferVerticeIndex;
	static		INT32		m_iCurBuffer;

	static		FLOAT		m_fWeightPSize[64];				// Point �� ���ÿ� billboard ��İ� size��ġ ��Ű�� ���� ���

	static		INT32		m_iMaxBufferPSize;
	static		INT32		m_iCurBufferPSize;
	static		INT32		m_iCountPSize[64];
	static		INT32		m_iMaxPSize;

	static		FLOAT				m_fMaxPointSize;		// H/W �� PSIZE�� ó�� ������ size
	static		RwCamera*			m_pCamera;

	static		IDirect3DDevice9*	m_pCurD3D9Device;
	static		BOOL				m_bEnablePSize;			// MaxPSize�� 8���� �۴ٸ� disable

	//DrawStart�ø��� ����..
	static		RwTexture*				m_pDrawTexture;
	static		ParticleBillboardType	m_eBillboardType;
		
	static		RwMatrix			m_matBillboard;
	static		RwMatrix			m_matBillboardY;

	static		D3DMATRIX			m_matD3DIdentity;
};

#endif