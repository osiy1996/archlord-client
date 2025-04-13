#ifndef ACU_EXPORT_GLTF_H
#define ACU_EXPORT_GLTF_H

#include <AgcmResourceLoader/AgcmResourceLoader.h>
#include <AgpmCharacter/AgpmCharacter.h>
#include <AgcmCharacter/AgcmCharacter.h>
#include <AgcmSkill/AgcmSkill.h>
#include <ApmObject/ApmObject.h>
#include <AgcmObject/AgcmObject.h>
#define CGLTF_NO_STDINT
#define CGLTF_NO_STDBOOL
#include <AcuExportDFF/cgltf_write.h>
#include <rwcore.h>
#include <rpworld.h>
#include <rpcollis.h>
#include <rphanim.h>
#include <rpskin.h>
#include <rtcharse.h>
#include <rtanim.h>

class AcuExportGLTF
{
	template<class T,size_t N = 128>
	class Arena
	{
	public:
		T pool[N];
		size_t cursor;

		Arena() : cursor(0)
		{
			memset(pool, 0, sizeof(pool));
		}

		T* Get(cgltf_size * count)
		{
			ASSERT(cursor < N);
			*count += 1;
			return &pool[cursor++];
		}
	};

public:
	AcuExportGLTF();
	~AcuExportGLTF();

	bool ExportCharacter(AgpmCharacter* pcsAgpmCharacter, int nTemplateID, int nFaceId, int nHairId);
	bool ExportObject(ApmObject* pcsApmObject, int nTemplateID);

private:
	bool AcuExportGLTF::ExportCharacter(AgpdCharacterTemplate* pTemp, AgcdCharacterTemplate* pTempClient, int nFaceId, int nHairId, const char* pszOutput);
	bool ExportObject(ApdObjectTemplate* pTemp, AgcdObjectTemplate* pTempClient, const char* pszOutput);

	static RpAtomic* ForAllAtomics(RpAtomic* pAtomic, void* pData);
	static RpMesh* ForAllMeshes(RpMesh* pMesh, RpMeshHeader* pMeshHeader, void* pData);

	cgltf_size AppendBuffer(cgltf_size size);
	cgltf_material* FindMaterial(RpGeometry* pGeometry, RpMaterial* pMaterial);
	void WriteWeights(RpGeometry* pGeometry, RpMorphTarget* pMorphTarget);
	void WriteBoneNode(RpHAnimHierarchy* pHierarchy, RpSkin* pGeometrySkin, cgltf_skin* pSkin, cgltf_node* pParent, RwFrame* pFrame);
	void WriteInverseMatrices(cgltf_skin* pSkin);
	void WriteAnimations(AgpdCharacterTemplate* pTemp, AgcdCharacterTemplate* pTempClient);
	void WriteAnimation(RtAnimAnimation* pAnimation, char* name);

private:
	char m_szTexDict[128];
	AgpmCharacter* m_pcsAgpmCharacter;
	AgcmCharacter* m_pcsAgcmCharacter;
	AgcmSkill* m_pcsAgcmSkill;
	RpClump* m_pClump;
	RpGeometry* m_pGeometry;
	cgltf_data m_stData;
	cgltf_mesh* m_pMesh;
	cgltf_buffer_view* m_pVertexView;
	cgltf_accessor* m_pPositionAccessor;
	cgltf_buffer_view* m_pNormalView;
	cgltf_accessor* m_pNormalAccessor;
	cgltf_size m_nUvCount;
	cgltf_accessor* m_pUvAccessor[8];
	cgltf_accessor* m_pJointsAccessor;
	cgltf_accessor* m_pWeightsAccessor;
	Arena<cgltf_node,1024> m_csArenaNode;
	Arena<cgltf_mesh> m_csArenaMesh;
	Arena<cgltf_primitive> m_csArenaPrimitive;
	Arena<cgltf_material> m_csArenaMaterial;
	Arena<cgltf_texture> m_csArenaTexture;
	Arena<cgltf_image> m_csArenaImage;
	Arena<cgltf_buffer_view, 131072> m_csArenaBufferView;
	Arena<cgltf_accessor, 131072> m_csArenaAccessor;
	Arena<cgltf_animation, 1024> m_csArenaAnimation;
	Arena<cgltf_animation_channel, 131072> m_csArenaAnimationChannel;
	Arena<cgltf_animation_sampler, 131072> m_csArenaAnimationSampler;
	std::vector<cgltf_node*> m_csBoneNodeMap;
	std::vector<RpSkin*> m_csSkins;

};

#endif