#ifndef ACURPMTEXTURE_H
#define ACURPMTEXTURE_H

#include "rwcore.h"
#include "rpworld.h"
#include "RpUsrdat.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AcuRpMTextureD" )
#else
#pragma comment ( lib , "AcuRpMTexture" )
#endif
#endif


#define rwVENDORID_NHN		(0xfffff0L)
#define rwID_MTEXTURE_NHN	(0x01)

#define rwID_MTEXTURE		MAKECHUNKID(rwVENDORID_NHN, rwID_MTEXTURE_NHN)

typedef enum RpMTextureType
{
	rpMTEXTURE_TYPE_NONE = 0,		// Texture�� ����.
	rpMTEXTURE_TYPE_NORMAL,			// Alpha�� ���� Texture�� ���� ��´�. (�ٷ� ���� rpMTEXTURE_TYPE_ALPHA�� �־��ٸ�, �ش� Alpha�� �Դ´�.)
	rpMTEXTURE_TYPE_NORMALALPHA,	// Alpha�� �ִ� Texture�� ���� ��´�. (�ٷ� ���� rpMTEXTURE_TYPE_ALPHA�� �־��ٸ�, ���õȴ�.)
	rpMTEXTURE_TYPE_ALPHA,			// Alpha�� �����Ѵ�. (rpMTEXTURE_TYPE_NORMAL���� �Դ´�.)
	rpMTEXTURE_TYPE_BUMP,			// ���� ���� �ȵ��ִ�. -_-;
} RpMTextureType;


#ifdef    __cplusplus
extern "C"
{
#endif  /* __cplusplus */

// world camera�� RwMatrix (shadow2�� invcameramatrix)�ʿ� 
extern RwCamera*	RPMTEX_WorldCamera		;
extern RwMatrix*	RPMTEX_invShadowMatrix	;

//extern RwMatrix*	RPMTEX_LightCameraViewMatrix;
//extern RwMatrix*	RPMTEX_LightCameraProjMatrix;

extern RwTexture*	RPMTEX_ShadowTexture	;

/* Attach Plugin */
extern RwInt32 
RpMTexturePluginAttach(void);

/* Set Second Texture (First texture is material's texture)*/
extern RwTexture *
RpMTextureMaterialGetTexture(RpMaterial *material, RwInt16 index, RpMTextureType *type);

/* Get Second Texture */
extern RpMaterial *
RpMTextureMaterialSetTexture(RpMaterial *material, RwInt16 index, RwTexture *texture, RpMTextureType type);

/* Enable Multi-texture at Atomic */
extern RpAtomic *
RpMTextureAtomicEnableEffect(RpAtomic *atomic);

/* Enable Multi-texture at Clump */
extern RpClump *
RpMTextureClumpEnableEffect(RpClump *pClump);

/* Enable Multi-Texture at World Sector */
extern RpWorldSector *
RpMTextureWorldSectorEnableEffect(RpWorldSector *worldSector);

extern void 
RpMTextureSetCamera(RwCamera*	camera);

extern void 
RpMTextureSetInvSMatrix(RwMatrix*	matrix);

extern	void 
RpMTextureSetShadowTexture(RwTexture*	tex);

extern	void 
RpMTextureSetBlendMode(RwUInt32  nBlendMode);
// �ؽ��� ���� ���� ... D3DTOP_MODULATE2X / D3DTOP_MODULATE

extern	RwInt32
RpMTextureSetMaxTexturePerPass( RwInt32 nPass );
// ������ (2004-04-29 ���� 6:26:42) : ���� ����׿� ���..
// ��Ƽ�н� ������ �����Ѵ�.. �뵵�� 1�� �����Ұ�� , ��Ƽ�ؽ��ĸ�
// ������� �ʴµ�.. �̰� �������ؼ� ����Ѵ�.

extern	RwBool
RpMTextureEnableVertexShader( RwBool bEnable );
// ������ (2004-05-??) : 
// ���ý� ���̴� ������� ����~

extern	RwBool
RpMTextureIsEnableVertexShader();
// ������ (2004-07-19 ���� 5:31:11) : ���� ����..

extern	RwBool
RpMTextureEnableLinearFog( RwBool bEnable );
extern	RwBool
RpMTextureIsEnableLinearFog();
// ������ (2004-05-19 ���� 5:09:52) : 
// ���������̶� ���Ͼ� ���� �����ϰ�.

// MTextureOpen ���� �ϴ����� ���� ȣ�����־�� ��(shader �ڵ�� �׷��� ī�� üũ�� ���߿� ����)
extern	void	
RpMTextureCreateShaders();

//extern void 
//RpMTextureSetLightViewMatrix(RwMatrix*	matrix);

//extern void 
//RpMTextureSetLightProjMatrix(RwMatrix*	matrix);

extern	void
RpMTextureDrawStart();
// ���� �׸��� ��ŸƮ(renderstate���� ���� ����)

extern void 
RpMTextureDrawEnd(RwBool	bUseShader);
// render state���� ��Ƴ� data�� �׸���.(bUseShader�� false�̸� shader�Ⱦ���!)

extern void
RpMTextureSetFadeFactor( float fNear , float fFar );

extern void
RpMTextureSetFadeSectorDist ( RwInt32 dist );


//@{ Jaewon 20050512
// Terrain gloss map support
extern RwBool
RpMTextureEnableGlossMap(RwBool enable);
extern RwBool
RpMTextureIsGlossMapEnabled();
//@} Jaewon
#ifdef    __cplusplus
}
#endif  /* __cplusplus */

#endif /* ACURPMTEXTURE_H */
