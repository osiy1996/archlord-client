#include "AcuExportGLTF.h"

#include <AcuRpMTexture/AcuRpMTexture.h>
#include <AcuTexture/AcuTexture.h>

#define _USE_MATH_DEFINES
#include <math.h>

static void debugPrint(const char * fmt, ...)
{
	char buffer[1024];
	va_list list;
	va_start(list, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, list);
	va_end(list);
	OutputDebugStringA(buffer);
	OutputDebugStringA("\n");
}

/*
 * Base64 encoding/decoding (RFC1341)
 * Copyright (c) 2005-2011, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

static const unsigned char base64_table[65] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/**
 * base64_encode - Base64 encode
 * @src: Data to be encoded
 * @len: Length of the data to be encoded
 * @out_len: Pointer to output length variable, or %NULL if not used
 * Returns: Allocated buffer of out_len bytes of encoded data,
 * or %NULL on failure
 *
 * Caller is responsible for freeing the returned buffer. Returned buffer is
 * nul terminated to make it easier to use as a C string. The nul terminator is
 * not included in out_len.
 */
static unsigned char * base64_encode(const unsigned char *src, size_t len, size_t *out_len)
{
	unsigned char *out, *pos;
	const unsigned char *end, *in;
	size_t olen;
	olen = len * 4 / 3 + 4; /* 3-byte blocks to 4-byte */
	olen++; /* nul termination */
	if (olen < len)
		return NULL; /* integer overflow */
	out = (unsigned char *)malloc(olen);
	if (out == NULL)
		return NULL;
	end = src + len;
	in = src;
	pos = out;
	while (end - in >= 3) {
		*pos++ = base64_table[in[0] >> 2];
		*pos++ = base64_table[((in[0] & 0x03) << 4) | (in[1] >> 4)];
		*pos++ = base64_table[((in[1] & 0x0f) << 2) | (in[2] >> 6)];
		*pos++ = base64_table[in[2] & 0x3f];
		in += 3;
	}
	if (end - in) {
		*pos++ = base64_table[in[0] >> 2];
		if (end - in == 1) {
			*pos++ = base64_table[(in[0] & 0x03) << 4];
			*pos++ = '=';
		} else {
			*pos++ = base64_table[((in[0] & 0x03) << 4) |
				(in[1] >> 4)];
			*pos++ = base64_table[(in[1] & 0x0f) << 2];
		}
		*pos++ = '=';
	}
	*pos = '\0';
	if (out_len)
		*out_len = pos - out;
	return out;
}


/**
 * base64_decode - Base64 decode
 * @src: Data to be decoded
 * @len: Length of the data to be decoded
 * @out_len: Pointer to output length variable
 * Returns: Allocated buffer of out_len bytes of decoded data,
 * or %NULL on failure
 *
 * Caller is responsible for freeing the returned buffer.
 */
static unsigned char * base64_decode(const unsigned char *src, size_t len,
	size_t *out_len)
{
	unsigned char dtable[256], *out, *pos, block[4], tmp;
	size_t i, count, olen;
	int pad = 0;

	memset(dtable, 0x80, 256);
	for (i = 0; i < sizeof(base64_table) - 1; i++)
		dtable[base64_table[i]] = (unsigned char) i;
	dtable['='] = 0;

	count = 0;
	for (i = 0; i < len; i++) {
		if (dtable[src[i]] != 0x80)
			count++;
	}

	if (count == 0 || count % 4)
		return NULL;

	olen = count / 4 * 3;
	pos = out = (unsigned char*)malloc(olen);
	if (out == NULL)
		return NULL;

	count = 0;
	for (i = 0; i < len; i++) {
		tmp = dtable[src[i]];
		if (tmp == 0x80)
			continue;

		if (src[i] == '=')
			pad++;
		block[count] = tmp;
		count++;
		if (count == 4) {
			*pos++ = (block[0] << 2) | (block[1] >> 4);
			*pos++ = (block[1] << 4) | (block[2] >> 2);
			*pos++ = (block[2] << 6) | block[3];
			count = 0;
			if (pad) {
				if (pad == 1)
					pos--;
				else if (pad == 2)
					pos -= 2;
				else {
					/* Invalid padding */
					free(out);
					return NULL;
				}
				break;
			}
		}
	}

	*out_len = pos - out;
	return out;
}

AcuExportGLTF::AcuExportGLTF()
	: m_pClump(NULL)
	, m_pVertexView(NULL)
	, m_pNormalView(NULL)
{
	memset(m_szTexDict, 0, sizeof(m_szTexDict));
	memset(&m_stData, 0, sizeof(m_stData));
}

AcuExportGLTF::~AcuExportGLTF()
{
	if (m_stData.buffers) {
		if (m_stData.buffers->data) {
			free(m_stData.buffers->data);
		}
		delete m_stData.buffers;
	}
	for (cgltf_size i = 0; i < m_stData.nodes_count; i++) {
		if (m_stData.nodes[i].children) {
			delete[] m_stData.nodes[i].children;
		}
	}
	if (m_stData.scene) {
		delete m_stData.scene;
	}
	if (m_stData.skins) {
		for (cgltf_size i = 0; i < m_stData.skins->joints_count; i++) {
			delete[] m_stData.skins->joints[i]->name;
		}
		delete[] m_stData.skins->joints;
		delete m_stData.skins;
	}
	for (cgltf_size i = 0; i < m_stData.animations_count; i++) {
		delete[] m_stData.animations[i].name;
	}
}

static void printHierarchy(RwFrame * frame, int indent)
{
	char ibuf[128] = "";
	for (int i = 0; i < indent; i++) {
		strcat(ibuf, "\t");
	}
	RwMatrix* ltm = RwFrameGetLTM(frame);
	RwV3d axis, center;
	RwReal r;
	RwMatrixQueryRotate(ltm, &axis, &r, &center);
	debugPrint("%s a(%g,%g,%g) %g -> c(%g,%g,%g) -> %g", ibuf, axis.x, axis.y, axis.z, RwV3dLength(&axis),
		center.x, center.y, center.z,
		r);
	debugPrint("HAnimID: %d", RpHAnimFrameGetID(frame));
	RpHAnimHierarchy* h = RpHAnimFrameGetHierarchy(frame);
	if (h) {
		debugPrint("Found hierarchy in depth %d -- %p", indent, h);
	}
	if (frame->next) {
		printHierarchy(frame->next, indent);
	}
	if (frame->child) {
		printHierarchy(frame->child, indent + 1);
	}
}

static RwFrame* getChildFrameHierarchy(RwFrame* frame, void* data)
{	
	RpHAnimHierarchy** hierarchy = (RpHAnimHierarchy**)data;
	*hierarchy = RpHAnimFrameGetHierarchy(frame);
	if (!*hierarchy) {
		RwFrameForAllChildren(frame, getChildFrameHierarchy, data);
		return frame;
	}
	return NULL;
}

static RpHAnimHierarchy* getHierarchy(RpClump* clump)
{
	RpHAnimHierarchy* hierarchy = NULL;
	RwFrameForAllChildren(RpClumpGetFrame(clump), getChildFrameHierarchy, (void*)&hierarchy);
	return hierarchy;
}

static void allocChildren(cgltf_node* node, size_t count)
{
	if (count) {
		node->children = new cgltf_node*[count];
	}
	else {
		node->children = NULL;
	}
	node->children_count = count;
}

static char* getClumpName(AgcdCharacterTemplate* pTemp)
{
	UINT32 ulIndex = 0;
	char* pszDefaultClumpName = NULL;
	if (pTemp->m_pcsPreData) {
		if (pTemp->m_pcsPreData->m_pszDFFName) {
			pszDefaultClumpName	= new char[strlen(pTemp->m_pcsPreData->m_pszDFFName) + 1];
			strcpy(pszDefaultClumpName, pTemp->m_pcsPreData->m_pszDFFName);
		}
	}
	else {
		char szTemp[256];
		memset(szTemp, 0, sizeof (char) * 256);
		if (pTemp->GetClumpID() > 0)
		{
			pTemp->GetClumpName(szTemp);
			pszDefaultClumpName	= new char[strlen(szTemp) + 1];
			strcpy(pszDefaultClumpName, szTemp);
		}
	}
	return pszDefaultClumpName;
}

bool AcuExportGLTF::ExportCharacter(AgpmCharacter* pcsAgpmCharacter, int nTemplateID, int nFaceId, int nHairId)
{
	strcpy(m_szTexDict, "character");
	m_pcsAgpmCharacter = pcsAgpmCharacter;
	m_pcsAgcmCharacter = (AgcmCharacter*)pcsAgpmCharacter->GetModule("AgcmCharacter");
	m_pcsAgcmSkill = (AgcmSkill*)pcsAgpmCharacter->GetModule("AgcmSkill");
	AgpdCharacterTemplate* temp = pcsAgpmCharacter->GetCharacterTemplate(nTemplateID);
	if (!temp) {
		return false;
	}
	AgcdCharacterTemplate* tempClient = m_pcsAgcmCharacter->GetTemplateData(temp);
	if (!tempClient) {
		return false;
	}
	tempClient->m_lRefCount++;
	if (!m_pcsAgcmCharacter->LoadTemplateData(temp, tempClient)) {
		tempClient->m_lRefCount--;
		return false;
	}
	if (!tempClient->m_pClump) {
		m_pcsAgcmCharacter->ReleaseTemplateData(tempClient);
		return false;
	}
	char* fileName = getClumpName(tempClient);
	debugPrint("Name: %s", fileName);
	char* str = fileName;
	while (*str) {
		*str = tolower(*str);
		str++;
	}
	char* ext = strrchr(fileName, '.');
	if (ext) {
		strcpy(ext, ".gltf");
	}
	else {
		strcat(fileName, ".gltf");
	}
	char exportPath[MAX_PATH + 1];
	sprintf(exportPath, "export\\character\\%s", fileName);
	ExportCharacter(temp, tempClient, nFaceId, nHairId, exportPath);
	m_pcsAgcmCharacter->ReleaseTemplateData(tempClient);
	return true;
}

bool AcuExportGLTF::ExportObject(ApmObject* pcsApmObject, int nTemplateID)
{
	strcpy(m_szTexDict, "object");
	AgcmObject* pcsAgcmObject = (AgcmObject*)pcsApmObject->GetModule("AgcmObject");
	ApdObjectTemplate* temp = pcsApmObject->GetObjectTemplate(nTemplateID);
	if (!temp) {
		return false;
	}
	AgcdObjectTemplate* tempClient = pcsAgcmObject->GetTemplateData(temp);
	if (!tempClient) {
		return false;
	}
	tempClient->m_lRefCount++;
	if (!pcsAgcmObject->LoadTemplateData(tempClient)) {
		tempClient->m_lRefCount--;
		return false;
	}
	char exportPath[MAX_PATH + 1];
	sprintf(exportPath, "export\\object\\%06d.gltf", nTemplateID);
	ExportObject(temp, tempClient, exportPath);
	tempClient->m_lRefCount--;
	pcsAgcmObject->ReleaseObjectTemplateData(tempClient);
	return true;
}

bool AcuExportGLTF::ExportCharacter(AgpdCharacterTemplate* pTemp, AgcdCharacterTemplate* pTempClient, int nFaceId, int nHairId, const char* pszOutput)
{
	RpClump* pClump = pTempClient->m_pDefaultArmourClump ? pTempClient->m_pDefaultArmourClump : pTempClient->m_pClump;
	RpHAnimHierarchy* h = getHierarchy(pClump);
	if (h) {
		RpHAnimHierarchySetFlags(h, (RpHAnimHierarchyFlag)(
			RpHAnimHierarchyGetFlags(h) | 
			rpHANIMHIERARCHYUPDATELTMS |
			rpHANIMHIERARCHYUPDATEMODELLINGMATRICES ) );
		RpHAnimHierarchyFlag flags = (RpHAnimHierarchyFlag)h->flags;
		RpHAnimHierarchyAttach(h);
	}
	//printHierarchy(RpClumpGetFrame(pClump), 0);
	cgltf_options options = {};
	options.type = cgltf_file_type_gltf;
	m_pClump = pClump;
	memset(&m_stData, 0, sizeof(m_stData));
	m_stData.asset.generator = "DFF2GLTF";
	m_stData.asset.version = "2.0";
	m_stData.buffers_count = 1;
	m_stData.buffers = new cgltf_buffer;
	memset(m_stData.buffers, 0, sizeof(*m_stData.buffers));
	m_stData.buffers->data = malloc(8388608);
	if (!m_stData.buffers->data) {
		return false;
	}
	m_stData.meshes = &m_csArenaMesh.pool[0];
	m_stData.buffer_views = &m_csArenaBufferView.pool[0];
	m_stData.accessors = &m_csArenaAccessor.pool[0];
	m_stData.materials = &m_csArenaMaterial.pool[0];
	m_stData.textures = &m_csArenaTexture.pool[0];
	m_stData.images = &m_csArenaImage.pool[0];
	m_stData.animations = &m_csArenaAnimation.pool[0];
	m_pMesh = m_csArenaMesh.Get(&m_stData.meshes_count);
	m_pMesh->primitives = &m_csArenaPrimitive.pool[m_csArenaPrimitive.cursor];
	RpClumpForAllAtomics(m_pClump, ForAllAtomics, this);
	if (pTempClient->m_pcsDefaultHeadData) {
		if (nFaceId < pTempClient->m_vpFace.size() && pTempClient->m_vpFace[nFaceId]) {
			ForAllAtomics(pTempClient->m_vpFace[nFaceId], this);
		}
		if (nHairId < pTempClient->m_vpHair.size() && pTempClient->m_vpHair[nHairId]) {
			ForAllAtomics(pTempClient->m_vpHair[nHairId], this);
		}
	}
	// Setup scene and nodes.
	m_stData.scene = new cgltf_scene;
	memset(m_stData.scene, 0, sizeof(cgltf_scene));
	m_stData.nodes = &m_csArenaNode.pool[0];
	cgltf_node* meshNode = m_csArenaNode.Get(&m_stData.nodes_count);
	meshNode->mesh = m_pMesh;
	meshNode->name = "Proxy";
	cgltf_node* armatureNode = m_csArenaNode.Get(&m_stData.nodes_count);
	armatureNode->name = "Armature";
	if (h && pClump->atomicList && pClump->atomicList->geometry && RpSkinGeometryGetSkin(pClump->atomicList->geometry)) {
		RpSkin* s = RpSkinGeometryGetSkin(pClump->atomicList->geometry);
		cgltf_skin* skin = new cgltf_skin;
		memset(skin, 0, sizeof(*skin));
		skin->name = "Armature";
		meshNode->skin = skin;
		m_stData.skins = skin;
		m_stData.skins_count = 1;
		RwUInt32 boneCount = RpSkinGetNumBones(s);
		ASSERT(boneCount == h->numNodes);
		m_csBoneNodeMap.resize(boneCount);
		for (RwUInt32 i = 0; i < boneCount; i++) {
			m_csBoneNodeMap[i] = NULL;
		}
		skin->joints = new cgltf_node*[boneCount];
		skin->joints_count = boneCount;
		for (RwUInt32 i = 0; i < boneCount; i++) {
			char* name = new char[32];
			sprintf(name, "Bone_%d", h->pNodeInfo[i].nodeID);
			skin->joints[i] = m_csArenaNode.Get(&m_stData.nodes_count);
			skin->joints[i]->name = name;
		}
		WriteBoneNode(h, s, skin, armatureNode, h->parentFrame);
		WriteInverseMatrices(skin);
		WriteAnimations(pTemp, pTempClient);
	}
	cgltf_node* root = m_csArenaNode.Get(&m_stData.nodes_count);
	root->name = "Root";
	allocChildren(root, 2);
	root->children[0] = meshNode;
	root->children[1] = armatureNode;
	m_stData.scene->nodes = &root;
	m_stData.scene->nodes_count = 1;
	m_stData.scenes = m_stData.scene;
	m_stData.scenes_count = 1;
	size_t encodedlen = 0;
	unsigned char* encoded = base64_encode((unsigned char*)m_stData.buffers->data, m_stData.buffers->size, &encodedlen);
	char* uri = (char*)malloc(++encodedlen + 37);
	strcpy(uri, "data:application/octet-stream;base64,");
	memcpy(uri + 37, encoded, encodedlen);
	m_stData.buffers->uri = uri;
	cgltf_result r = cgltf_write_file(&options, pszOutput, &m_stData);
	free(uri);
	free(encoded);
	if (r != cgltf_result_success) {
		debugPrint("Failed to export as gltf: error_code = %d", r);
		return false;
	}
	return true;
}

bool AcuExportGLTF::ExportObject(ApdObjectTemplate* pTemp, AgcdObjectTemplate* pTempClient, const char* pszOutput)
{
	//printHierarchy(RpClumpGetFrame(pClump), 0);
	cgltf_options options = {};
	options.type = cgltf_file_type_gltf;
	memset(&m_stData, 0, sizeof(m_stData));
	m_stData.asset.generator = "DFF2GLTF";
	m_stData.asset.version = "2.0";
	m_stData.buffers_count = 1;
	m_stData.buffers = new cgltf_buffer;
	memset(m_stData.buffers, 0, sizeof(*m_stData.buffers));
	m_stData.buffers->data = malloc(8388608);
	if (!m_stData.buffers->data) {
		return false;
	}
	m_stData.meshes = &m_csArenaMesh.pool[0];
	m_stData.buffer_views = &m_csArenaBufferView.pool[0];
	m_stData.accessors = &m_csArenaAccessor.pool[0];
	m_stData.materials = &m_csArenaMaterial.pool[0];
	m_stData.textures = &m_csArenaTexture.pool[0];
	m_stData.images = &m_csArenaImage.pool[0];
	m_stData.animations = &m_csArenaAnimation.pool[0];
	m_pMesh = m_csArenaMesh.Get(&m_stData.meshes_count);
	m_pMesh->primitives = &m_csArenaPrimitive.pool[m_csArenaPrimitive.cursor];
	AgcdObjectTemplateGroupList* group = pTempClient->m_stGroup.m_pstList;
	while (group) {
		m_pClump = group->m_csData.m_pstClump;
		if (m_pClump) {
			RpClumpForAllAtomics(m_pClump, ForAllAtomics, this);
		}
		group = group->m_pstNext;
	}
	// Setup scene and nodes.
	m_stData.scene = new cgltf_scene;
	memset(m_stData.scene, 0, sizeof(cgltf_scene));
	m_stData.nodes = &m_csArenaNode.pool[0];
	cgltf_node* meshNode = m_csArenaNode.Get(&m_stData.nodes_count);
	meshNode->mesh = m_pMesh;
	meshNode->name = "Mesh";
	m_stData.scene->nodes = &meshNode;
	m_stData.scene->nodes_count = 1;
	m_stData.scenes = m_stData.scene;
	m_stData.scenes_count = 1;
	size_t encodedlen = 0;
	unsigned char* encoded = base64_encode((unsigned char*)m_stData.buffers->data, m_stData.buffers->size, &encodedlen);
	char* uri = (char*)malloc(++encodedlen + 37);
	strcpy(uri, "data:application/octet-stream;base64,");
	memcpy(uri + 37, encoded, encodedlen);
	m_stData.buffers->uri = uri;
	cgltf_result r = cgltf_write_file(&options, pszOutput, &m_stData);
	free(uri);
	free(encoded);
	if (r != cgltf_result_success) {
		debugPrint("Failed to export as gltf: error_code = %d", r);
		return false;
	}
	return true;
}

static void* expandArray(void** ptr, size_t* size, size_t element_size)
{
	size_t index = (*size)++;
	*ptr = realloc(*ptr, (index + 1) * element_size);
	void* last = (void *)((uintptr_t)*ptr + index * element_size);
	memset(last, 0, element_size);
	return last;
}

static cgltf_attribute* createAttribute(cgltf_primitive * primitive)
{
	return (cgltf_attribute*)expandArray((void**)&primitive->attributes, &primitive->attributes_count, sizeof(cgltf_attribute));
}

RpAtomic* AcuExportGLTF::ForAllAtomics(RpAtomic* pAtomic, void* pData)
{
	RpGeometry * geometry = RpAtomicGetGeometry(pAtomic);
	if (!geometry || !geometry->mesh || !geometry->mesh->numMeshes) {
		// todo: warn
		return pAtomic;
	}
	if (!geometry->numVertices || !geometry->numTriangles || !geometry->numMorphTargets) {
		// todo: warn
		return pAtomic;
	}
	AcuExportGLTF* exp = static_cast<AcuExportGLTF*>(pData);
	exp->m_nUvCount = 0;
	ASSERT(geometry->numTexCoordSets <= 6);
	cgltf_size size = geometry->numVertices * (sizeof(RwV3d) * 6 + geometry->numTexCoordSets * sizeof(RwTexCoords));
	cgltf_size offset = exp->AppendBuffer(size);
	RpMorphTarget * mt = RpGeometryGetMorphTarget(geometry, 0);
	RwV3d * verts = RpMorphTargetGetVertices(mt);
	RwV3d * norms = RpMorphTargetGetVertexNormals(mt);
	memcpy((void*)((cgltf_size)exp->m_stData.buffers->data + offset), verts, 
		geometry->numVertices * sizeof(RwV3d));
	if (norms) {
		memcpy((void*)((cgltf_size)exp->m_stData.buffers->data + offset + geometry->numVertices * sizeof(RwV3d)), 
			norms, geometry->numVertices * sizeof(RwV3d));
	}
	for (RwInt32 i = 0; i < geometry->numTexCoordSets; i++) {
		memcpy((void*)((cgltf_size)exp->m_stData.buffers->data + offset + geometry->numVertices * sizeof(RwV3d) * 2 + i * geometry->numVertices * sizeof(RwTexCoords)), 
			geometry->texCoords[i], geometry->numVertices * sizeof(RwTexCoords));
	}
	// Min/Max vertex positions are required by GLTF spec, so calculate them.
	RwV3d pmin = verts[0];
	RwV3d pmax = verts[0];
	for (RwInt32 i = 1; i < geometry->numVertices; i++) {
		pmin.x = min(pmin.x, verts[i].x);
		pmax.x = max(pmax.x, verts[i].x);
		pmin.y = min(pmin.y, verts[i].y);
		pmax.y = max(pmax.y, verts[i].y);
		pmin.z = min(pmin.z, verts[i].z);
		pmax.z = max(pmax.z, verts[i].z);
	}
	// Create buffer views and accessors.
	exp->m_pVertexView = exp->m_csArenaBufferView.Get(&exp->m_stData.buffer_views_count);
	exp->m_pVertexView->buffer = exp->m_stData.buffers;
	exp->m_pVertexView->offset = offset;
	exp->m_pVertexView->size = geometry->numVertices * sizeof(RwV3d);
	exp->m_pVertexView->stride = 12;
	exp->m_pVertexView->type = cgltf_buffer_view_type_vertices;
	exp->m_pPositionAccessor = exp->m_csArenaAccessor.Get(&exp->m_stData.accessors_count);
	exp->m_pPositionAccessor->component_type = cgltf_component_type_r_32f;
	exp->m_pPositionAccessor->type = cgltf_type_vec3;
	exp->m_pPositionAccessor->count = geometry->numVertices;
	exp->m_pPositionAccessor->buffer_view = exp->m_pVertexView;
	exp->m_pPositionAccessor->has_min = true;
	exp->m_pPositionAccessor->min[0] = pmin.x;
	exp->m_pPositionAccessor->min[1] = pmin.y;
	exp->m_pPositionAccessor->min[2] = pmin.z;
	exp->m_pPositionAccessor->has_max = true;
	exp->m_pPositionAccessor->max[0] = pmax.x;
	exp->m_pPositionAccessor->max[1] = pmax.y;
	exp->m_pPositionAccessor->max[2] = pmax.z;
	exp->m_pNormalView = exp->m_csArenaBufferView.Get(&exp->m_stData.buffer_views_count);
	exp->m_pNormalView->buffer = exp->m_stData.buffers;
	exp->m_pNormalView->offset = offset + geometry->numVertices * sizeof(RwV3d);
	exp->m_pNormalView->size = geometry->numVertices * sizeof(RwV3d);
	exp->m_pNormalView->stride = 12;
	exp->m_pNormalView->type = cgltf_buffer_view_type_vertices;
	exp->m_pNormalAccessor = exp->m_csArenaAccessor.Get(&exp->m_stData.accessors_count);
	exp->m_pNormalAccessor->component_type = cgltf_component_type_r_32f;
	exp->m_pNormalAccessor->type = cgltf_type_vec3;
	exp->m_pNormalAccessor->count = geometry->numVertices;
	exp->m_pNormalAccessor->buffer_view = exp->m_pNormalView;
	for (RwInt32 i = 0; i < geometry->numTexCoordSets; i++) {
		cgltf_buffer_view* uvView = exp->m_csArenaBufferView.Get(&exp->m_stData.buffer_views_count);
		uvView->buffer = exp->m_stData.buffers;
		uvView->offset = offset + geometry->numVertices * sizeof(RwV3d) * 2 + i * geometry->numVertices * sizeof(RwTexCoords);
		uvView->size = geometry->numVertices * sizeof(RwTexCoords);
		uvView->type = cgltf_buffer_view_type_vertices;
		cgltf_size index = exp->m_nUvCount++;
		exp->m_pUvAccessor[index] = exp->m_csArenaAccessor.Get(&exp->m_stData.accessors_count);
		exp->m_pUvAccessor[index]->component_type = cgltf_component_type_r_32f;
		exp->m_pUvAccessor[index]->type = cgltf_type_vec2;
		exp->m_pUvAccessor[index]->count = geometry->numVertices;
		exp->m_pUvAccessor[index]->buffer_view = uvView;
	}
	exp->WriteWeights(geometry, mt);
	exp->m_pGeometry = geometry;
	RpGeometryForAllMeshes(geometry, ForAllMeshes, pData);
	return pAtomic;
}

RpMesh* AcuExportGLTF::ForAllMeshes(RpMesh* pMesh, RpMeshHeader* pMeshHeader, void* pData)
{
	AcuExportGLTF* exp = static_cast<AcuExportGLTF*>(pData);
	cgltf_primitive_type type;
	if ((pMeshHeader->flags & rpMESHHEADERPOINTLIST) == rpMESHHEADERPOINTLIST) {
		type = cgltf_primitive_type_points;
	}
	else if ((pMeshHeader->flags & rpMESHHEADERLINELIST) == rpMESHHEADERLINELIST) {
		type = cgltf_primitive_type_lines;
	}
	else if ((pMeshHeader->flags & rpMESHHEADERTRISTRIP) == rpMESHHEADERTRISTRIP) {
		type = cgltf_primitive_type_triangle_strip;
	}
	else if ((pMeshHeader->flags & rpMESHHEADERTRIFAN) == rpMESHHEADERTRIFAN) {
		type = cgltf_primitive_type_triangle_fan;
	}
	else if ((pMeshHeader->flags & rpMESHHEADERPRIMMASK) == 0) {
		type = cgltf_primitive_type_triangles;
	}
	else {
		return pMesh;
	}
	cgltf_primitive * primitive = exp->m_csArenaPrimitive.Get(&exp->m_pMesh->primitives_count);
	primitive->type = type;
	// Create vertex attributes.
	cgltf_attribute * attr = createAttribute(primitive);
	attr->name = "POSITION";
	attr->type = cgltf_attribute_type_position;
	attr->data = exp->m_pPositionAccessor;
	// note: Exported normals appear broken in Blender, let it be calculated in Blender.
	attr = createAttribute(primitive);
	attr->name = "NORMAL";
	attr->type = cgltf_attribute_type_normal;
	attr->data = exp->m_pNormalAccessor;
	for (cgltf_size i = 0; i < exp->m_nUvCount; i++) {
		const char* names[] = { "TEXCOORD_0", "TEXCOORD_1", "TEXCOORD_2", "TEXCOORD_3", "TEXCOORD_4", "TEXCOORD_5", "TEXCOORD_6", "TEXCOORD_7" };
		attr = createAttribute(primitive);
		attr->name = strdup(names[i]);
		attr->type = cgltf_attribute_type_texcoord;
		attr->data = exp->m_pUvAccessor[i];
	}
	if (exp->m_pJointsAccessor && exp->m_pWeightsAccessor) {
		attr = createAttribute(primitive);
		attr->name = "JOINTS_0";
		attr->type = cgltf_attribute_type_joints;
		attr->data = exp->m_pJointsAccessor;
		attr = createAttribute(primitive);
		attr->name = "WEIGHTS_0";
		attr->type = cgltf_attribute_type_weights;
		attr->data = exp->m_pWeightsAccessor;
	}
	// Write index buffer.
	cgltf_size size = pMesh->numIndices * sizeof(RxVertexIndex);
	cgltf_size offset = exp->AppendBuffer(size);
	memcpy((void*)((cgltf_size)exp->m_stData.buffers->data + offset), pMesh->indices, size);
	// Create view for index buffer.
	cgltf_buffer_view * view = exp->m_csArenaBufferView.Get(&exp->m_stData.buffer_views_count);
	view->buffer = exp->m_stData.buffers;
	view->offset = offset;
	view->size = size;
	view->type = cgltf_buffer_view_type_indices;
	// Create accessor for index buffer/view.
	cgltf_accessor * indices = exp->m_csArenaAccessor.Get(&exp->m_stData.accessors_count);
	indices->component_type = cgltf_component_type_r_16u;
	indices->type = cgltf_type_scalar;
	indices->count = pMesh->numIndices;
	indices->buffer_view = view;
	primitive->indices = indices;
	if (pMesh->material) {
		cgltf_material* m = exp->FindMaterial(exp->m_pGeometry, pMesh->material);
		if (m) {
			primitive->material = m;
		}
		else {
			cgltf_material* m = exp->m_csArenaMaterial.Get(&exp->m_stData.materials_count);
			cgltf_texture_view* views[] = { &m->pbr_metallic_roughness.base_color_texture, &m->normal_texture, &m->emissive_texture };
			cgltf_size viewCount = 0;
			RwTexture* tex = pMesh->material->texture;
			m->has_pbr_metallic_roughness = true;
			m->pbr_metallic_roughness.base_color_factor[0] = 1.0f;
			m->pbr_metallic_roughness.base_color_factor[1] = 1.0f;
			m->pbr_metallic_roughness.base_color_factor[2] = 1.0f;
			m->pbr_metallic_roughness.base_color_factor[3] = 1.0f;
			m->pbr_metallic_roughness.metallic_factor = 0.0f;
			m->pbr_metallic_roughness.roughness_factor = 1.0f;
			if (tex) {
				RwTextureSetFilterMode(tex, rwFILTERLINEARMIPLINEAR);
				char szTexturePath[128];
				sprintf(szTexturePath, "export\\%s\\%s.dds", exp->m_szTexDict, RwTextureGetName(tex));
				AcuTexture::RwD3D9DDSTextureWrite(tex, szTexturePath, -1, 1, ACUTEXTURE_COPY_LOAD_LINEAR, NULL);
				sprintf(szTexturePath, "%s.dds", RwTextureGetName(tex));
				cgltf_texture* texture = exp->m_csArenaTexture.Get(&exp->m_stData.textures_count);
				cgltf_image* image = exp->m_csArenaImage.Get(&exp->m_stData.images_count);
				image->uri = strdup(szTexturePath);
				texture->image = image;
				views[0]->texture = texture;
				views[0]->texcoord = 0;
				viewCount++;
			}
			for (RwInt32 i = 1; i < exp->m_pGeometry->numTexCoordSets; i++) {
				RpMTextureType type;
				tex = RpMTextureMaterialGetTexture(pMesh->material, i, &type);
				if (tex) {
					debugPrint("[%d] MTex %d", i, type);
					if (type == rpMTEXTURE_TYPE_NORMAL || type == rpMTEXTURE_TYPE_NORMALALPHA) {
						if (viewCount < 3) {
							RwTextureSetFilterMode(tex, rwFILTERLINEARMIPLINEAR);
							char szTexturePath[128];
							sprintf(szTexturePath, "export\\%s\\%s.dds", exp->m_szTexDict, RwTextureGetName(tex));
							AcuTexture::RwD3D9DDSTextureWrite(tex, szTexturePath, -1, 1, ACUTEXTURE_COPY_LOAD_LINEAR, NULL);
							sprintf(szTexturePath, "%s.dds", RwTextureGetName(tex));
							cgltf_texture* texture = exp->m_csArenaTexture.Get(&exp->m_stData.textures_count);
							cgltf_image* image = exp->m_csArenaImage.Get(&exp->m_stData.images_count);
							image->uri = strdup(szTexturePath);
							texture->image = image;
							views[viewCount]->texture = texture;
							views[viewCount]->texcoord = 0;
							viewCount++;
						}
						else {
							debugPrint("[warning] Too many textures..");
						}
					}
				}
			}
			primitive->material = m;
		}
	}
	return pMesh;
}

static size_t countSiblingNodes(RwFrame* pFrame)
{
	size_t count = 0;
	RwInt32 id = RpHAnimFrameGetID(pFrame);
	if (id != -1) {
		count++;
	}
	RwFrame* next = pFrame->next;
	while (next) {
		id = RpHAnimFrameGetID(next);
		if (id != -1) {
			count++;
		}
		next = next->next;
	}
	return count;
}

cgltf_size AcuExportGLTF::AppendBuffer(cgltf_size size)
{
	cgltf_size aligned = ((m_stData.buffers->size + 15) / 16) * 16;
	m_stData.buffers->size = aligned + size;
	if (m_stData.buffers->size > 8388608) {
		m_stData.buffers->data = realloc(m_stData.buffers->data, m_stData.buffers->size);
	}
	return aligned;
}

cgltf_material* AcuExportGLTF::FindMaterial(RpGeometry* pGeometry, RpMaterial * pMaterial)
{
	for (cgltf_size i = 0; i < m_stData.materials_count; i++) {
		cgltf_material* m = &m_stData.materials[i];
		cgltf_texture_view* views[2] = { NULL, NULL };
		int viewCount = 0;
		cgltf_texture_view* view = &m->pbr_metallic_roughness.base_color_texture;
		RwTexture* tex = pMaterial->texture;
		if (!tex) {
			continue;
		}
		if (!view->texture || view->texcoord != 0) {
			continue;
		}
		char szName[128];
		sprintf(szName, "%s.dds", RwTextureGetName(tex));
		if (strcmp(view->texture->image->uri, szName) != 0) {
			continue;
		}
		if (m->normal_texture.texture) {
			views[viewCount++] = &m->normal_texture;
		}
		if (m->emissive_texture.texture) {
			views[viewCount++] = &m->emissive_texture;
		}
		bool matched = true;
		int index = 0;
		for (RwInt32 j = 1; j < pGeometry->numTexCoordSets; j++) {
			RpMTextureType type;
			tex = RpMTextureMaterialGetTexture(pMaterial, j, &type);
			if (tex) {
				if (type == rpMTEXTURE_TYPE_NORMAL || type == rpMTEXTURE_TYPE_NORMALALPHA) {
					if (index >= 2 || views[index]->texcoord != j) {
						matched = false;
						break;
					}
					sprintf(szName, "%s.dds", RwTextureGetName(tex));
					if (strcmp(views[index]->texture->image->uri, szName) != 0) {
						matched = false;
						break;
					}
					index++;
				}
			}
		}
		if (index != viewCount) {
			continue;
		}
		if (matched) {
			return m;
		}
	}
	return NULL;
}

void AcuExportGLTF::WriteWeights(RpGeometry * pGeometry, RpMorphTarget * pMorphTarget)
{
	RpSkin* skin = RpSkinGeometryGetSkin(pGeometry);
	if (!skin) {
		m_pJointsAccessor = NULL;
		m_pWeightsAccessor = NULL;
		return;
	}
	m_csSkins.push_back(skin);
	size_t skinCount = m_csSkins.size();
	for (size_t i = 0; i < skinCount; i++) {
		RpSkin* s = m_csSkins[i];
		for (size_t j = 1; j < skinCount; j++) {
			RpSkin* s2 = m_csSkins[j];
			RwUInt32 numBones = RpSkinGetNumBones(s);
			if (numBones != RpSkinGetNumBones(s2)) {
				debugPrint("Bone count does not match.");
				break;
			}
			if (memcmp(RpSkinGetSkinToBoneMatrices(s), RpSkinGetSkinToBoneMatrices(s2), numBones * sizeof(RwMatrix)) != 0) {
				debugPrint("Inverse bind matrices do not match.");
				break;
			}
		}
		break;
	}
	cgltf_size size = pGeometry->numVertices * (sizeof(RwUInt32) + sizeof(RwMatrixWeights));
	cgltf_size offset = AppendBuffer(size);
	const RwUInt32* boneIndices = RpSkinGetVertexBoneIndices(skin);
	const RwMatrixWeights* boneWeights = RpSkinGetVertexBoneWeights(skin);
	memcpy((void*)((cgltf_size)m_stData.buffers->data + offset), 
		boneIndices, pGeometry->numVertices * sizeof(RwUInt32));
	memcpy((void*)((cgltf_size)m_stData.buffers->data + offset + pGeometry->numVertices * sizeof(RwUInt32)), 
		boneWeights, pGeometry->numVertices * sizeof(RwMatrixWeights));
	// Create buffer views and accessors.
	cgltf_buffer_view* jointView = m_csArenaBufferView.Get(&m_stData.buffer_views_count);
	jointView->buffer = m_stData.buffers;
	jointView->offset = offset;
	jointView->size = pGeometry->numVertices * sizeof(RwUInt32);
	jointView->stride = 4;
	jointView->type = cgltf_buffer_view_type_vertices;
	m_pJointsAccessor = m_csArenaAccessor.Get(&m_stData.accessors_count);
	m_pJointsAccessor->component_type = cgltf_component_type_r_8u;
	m_pJointsAccessor->type = cgltf_type_vec4;
	m_pJointsAccessor->count = pGeometry->numVertices;
	m_pJointsAccessor->buffer_view = jointView;
	cgltf_buffer_view* weightsView = m_csArenaBufferView.Get(&m_stData.buffer_views_count);
	weightsView->buffer = m_stData.buffers;
	weightsView->offset = offset + pGeometry->numVertices * sizeof(RwUInt32);
	weightsView->size = pGeometry->numVertices * sizeof(RwMatrixWeights);
	weightsView->stride = sizeof(RwMatrixWeights);
	weightsView->type = cgltf_buffer_view_type_vertices;
	m_pWeightsAccessor = m_csArenaAccessor.Get(&m_stData.accessors_count);
	m_pWeightsAccessor->component_type = cgltf_component_type_r_32f;
	m_pWeightsAccessor->type = cgltf_type_vec4;
	m_pWeightsAccessor->count = pGeometry->numVertices;
	m_pWeightsAccessor->buffer_view = weightsView;
}

static cgltf_node* findJoint(cgltf_skin* pSkin,  const char* pszName)
{
	for (cgltf_size i = 0; i < pSkin->joints_count; i++) {
		if (strcmp(pSkin->joints[i]->name, pszName) == 0) {
			return pSkin->joints[i];
		}
	}
	return NULL;
}

void AcuExportGLTF::WriteBoneNode(RpHAnimHierarchy* pHierarchy, RpSkin* pGeometrySkin, cgltf_skin* pSkin, cgltf_node* pParent, RwFrame* pFrame)
{
	allocChildren(pParent, countSiblingNodes(pFrame));
	RwInt32 id = RpHAnimFrameGetID(pFrame);
	size_t i = 0;
	if (id != -1) {
		char name[32];
		sprintf(name, "Bone_%d", id);
		cgltf_node* bone = findJoint(pSkin, name);
		RwUInt32 index = RpHAnimIDGetIndex(pHierarchy, id);
		if (index < m_csBoneNodeMap.size()) {
			m_csBoneNodeMap[index] = bone;
		}
		else {
			debugPrint("[warning] Invalid bone index: bone_id = %d, index = %d", id, index);
		}
		pParent->children[i++] = bone;
		if (pFrame->child) {
			WriteBoneNode(pHierarchy, pGeometrySkin, pSkin, bone, pFrame->child);
		}
	}
	else if (pFrame->child) {
		WriteBoneNode(pHierarchy, pGeometrySkin, pSkin, pParent, pFrame->child);
	}
	RwFrame* sibling = pFrame->next;
	while (sibling) {
		id = RpHAnimFrameGetID(sibling);
		if (id != -1) {
			char name[32];
			sprintf(name, "Bone_%d", id);
			cgltf_node* bone = findJoint(pSkin, name);
			RwUInt32 index = RpHAnimIDGetIndex(pHierarchy, id);
			if (index < m_csBoneNodeMap.size()) {
				m_csBoneNodeMap[index] = bone;
			}
			else {
				debugPrint("[warning] Invalid bone index: bone_id = %d, index = %d", id, index);
			}
			pParent->children[i++] = bone;
			if (sibling->child) {
				WriteBoneNode(pHierarchy, pGeometrySkin, pSkin, bone, sibling->child);
			}
		}
		else if (sibling->child) {
			WriteBoneNode(pHierarchy, pGeometrySkin, pSkin, pParent, sibling->child);
		}
		sibling = sibling->next;
	}
}

void AcuExportGLTF::WriteInverseMatrices(cgltf_skin * pSkin)
{
	RpAtomic* atomic = m_pClump->atomicList;
	if (!atomic || !atomic->geometry) {
		return;
	}
	RpSkin* skin = RpSkinGeometryGetSkin(atomic->geometry);
	if (!skin) {
		return;
	}
	RwUInt32 boneCount = RpSkinGetNumBones(skin);
	cgltf_size size = boneCount * 64;
	cgltf_size offset = AppendBuffer(size);
	const RwMatrix* matrices = RpSkinGetSkinToBoneMatrices(skin);
	for (RwUInt32 i = 0; i < boneCount; i++) {
		const RwMatrix* m = &matrices[i];
		float matrix[16] = {
			m->right.x, m->right.y, m->right.z, 0.0f,
			m->up.x, m->up.y, m->up.z, 0.0f,
			m->at.x, m->at.y, m->at.z, 0.0f,
			m->pos.x, m->pos.y, m->pos.z, 1.0f };
		memcpy((void*)((cgltf_size)m_stData.buffers->data + offset + i * 64), matrix, 64);
	}
	// Create buffer views and accessors.
	cgltf_buffer_view* matrixView = m_csArenaBufferView.Get(&m_stData.buffer_views_count);
	matrixView->buffer = m_stData.buffers;
	matrixView->offset = offset;
	matrixView->size = size;
	matrixView->type = cgltf_buffer_view_type_invalid;
	cgltf_accessor* matrixAccessor = m_csArenaAccessor.Get(&m_stData.accessors_count);
	matrixAccessor->component_type = cgltf_component_type_r_32f;
	matrixAccessor->type = cgltf_type_mat4;
	matrixAccessor->count = boneCount;
	matrixAccessor->buffer_view = matrixView;
	pSkin->inverse_bind_matrices = matrixAccessor;
}

static int getAnimType2Num(AgcdCharacterTemplate* pstAgcdCharacterTemplate)
{
	switch (pstAgcdCharacterTemplate->m_lAnimType2) {
	case AGCMCHAR_AT2_BASE:
	case AGCMCHAR_AT2_BOSS_MONSTER:
		return AGCMCHAR_AT2_BASE_NUM;
	case AGCMCHAR_AT2_WARRIOR:
		return AGCMCHAR_AT2_WARRIR_NUM;
	case AGCMCHAR_AT2_ARCHER:
		return AGCMCHAR_AT2_ARCHER_NUM;
	case AGCMCHAR_AT2_WIZARD:
		return AGCMCHAR_AT2_WIZARD_NUM;
	case AGCMCHAR_AT2_ARCHLORD :
		return AGCMCHAR_AT2_ARCHLORD_NUM;
	}
	return -1;
}

static char* allocAnimationName(AgcmCharacterAnimType type, int n)
{
	char* name = new char[128];
	switch (type) {
	case AGCMCHAR_ANIM_TYPE_WAIT:
		sprintf(name, "Idle_%d", n);
		break;
	case AGCMCHAR_ANIM_TYPE_WALK:
		sprintf(name, "Walk_%d", n);
		break;
	case AGCMCHAR_ANIM_TYPE_RUN:
		sprintf(name, "Run_%d", n);
		break;
	case AGCMCHAR_ANIM_TYPE_ATTACK:
		sprintf(name, "Attack_%d", n);
		break;
	case AGCMCHAR_ANIM_TYPE_STRUCK:
		sprintf(name, "Struck_%d", n);
		break;
	case AGCMCHAR_ANIM_TYPE_DEAD:
		sprintf(name, "Dead_%d", n);
		break;
	case AGCMCHAR_ANIM_TYPE_CUSTOMIZE_PREVIEW:
		sprintf(name, "Customize_preview_%d", n);
		break;
	case AGCMCHAR_ANIM_TYPE_LEFTSIDE_STEP:
		sprintf(name, "Leftside_step_%d", n);
		break;
	case AGCMCHAR_ANIM_TYPE_RIGHTSIDE_STEP:
		sprintf(name, "Rightside_step_%d", n);
		break;
	case AGCMCHAR_ANIM_TYPE_BACKWARD_STEP:
		sprintf(name, "Backward_step_%d", n);
		break;
	case AGCMCHAR_ANIM_TYPE_PICKUP:
		sprintf(name, "Pickup_%d", n);
		break;
	case AGCMCHAR_ANIM_TYPE_EAT:
		sprintf(name, "Eat_%d", n);
		break;
	case AGCMCHAR_ANIM_TYPE_SUMMON_CONVEYANCE:
		sprintf(name, "Summon_conveyance_%d", n);
		break;
	case AGCMCHAR_ANIM_TYPE_EMBARK:
		sprintf(name, "Embark_%d", n);
		break;
	case AGCMCHAR_ANIM_TYPE_RIDE:
		sprintf(name, "Ride_%d", n);
		break;
	case AGCMCHAR_ANIM_TYPE_ABNORMAL:
		sprintf(name, "Abnormal_%d", n);
		break;
	default:
		sprintf(name, "Unknown_%d_%d", type, n);
		break;
	}
	return name;
}

static char* allocSkillAnimationName(int i, int j, AgcmSkillVisualInfo* pInfo)
{
	char* name = new char[128];
	if (pInfo->m_paszInfo && pInfo->m_paszInfo[j]) {
		sprintf(name, "%s_%d%d", pInfo->m_paszInfo[j], i, j);
	}
	else {
		sprintf(name, "Cast%d_%d", i, j);
	}
	return name;
}

void AcuExportGLTF::WriteAnimations(AgpdCharacterTemplate* pTemp, AgcdCharacterTemplate* pTempClient)
{
	int lNumAnimType2 = getAnimType2Num(pTempClient);
	for (int i = 0; i < AGCMCHAR_MAX_ANIM_TYPE; i++) {
		for (int j = 0; j < lNumAnimType2; j++) {
			if (!pTempClient->m_pacsAnimationData[i][j]) {
				continue;
			}
			AgcdAnimation2* pAnim2 = pTempClient->m_pacsAnimationData[i][j]->m_pcsAnimation;
			if (!pAnim2 ||
				!pAnim2->m_pcsHead ||
				!pAnim2->m_pcsHead->m_pcsRtAnim ||
				!pAnim2->m_pcsHead->m_pcsRtAnim->m_pstAnimation) {
				continue;
			}
			RtAnimAnimation* pAnim = pAnim2->m_pcsHead->m_pcsRtAnim->m_pstAnimation;
			WriteAnimation(pAnim, allocAnimationName((AgcmCharacterAnimType)i, j));
		}
	}
	AgcdSkillAttachTemplateData* attachment = m_pcsAgcmSkill->GetAttachTemplateData((ApBase*)pTemp);
	if (!attachment || !attachment->m_pacsSkillVisualInfo) {
		return;
	}
	int count = m_pcsAgcmSkill->GetNumVisualInfo(pTempClient);
	for (int i = 0; i < count; i++) {
		if (!attachment->m_pacsSkillVisualInfo[i]) {
			continue;
		}
		for (int j = 0; j < lNumAnimType2; j++) {
			if (!attachment->m_pacsSkillVisualInfo[i]->m_pacsAnimation[j]) {
				continue;
			}
			AgcdAnimation2* pAnim2 = attachment->m_pacsSkillVisualInfo[i]->m_pacsAnimation[j]->m_pcsAnimation;
			if (!pAnim2 ||
				!pAnim2->m_pcsHead ||
				!pAnim2->m_pcsHead->m_pcsRtAnim ||
				!pAnim2->m_pcsHead->m_pcsRtAnim->m_pstAnimation) {
				continue;
			}
			RtAnimAnimation* pAnim = pAnim2->m_pcsHead->m_pcsRtAnim->m_pstAnimation;
			WriteAnimation(pAnim, allocSkillAnimationName(i, j, attachment->m_pacsSkillVisualInfo[i]));
		}
	}
}

static RpHAnimKeyFrame* getKeyFrame(RtAnimAnimation* pAnimation, RwUInt32 index)
{
	return (RpHAnimKeyFrame*)((RwInt8 *)pAnimation->pFrames + index * pAnimation->interpInfo->animKeyFrameSize);
}

void AcuExportGLTF::WriteAnimation(RtAnimAnimation* pAnimation, char* name)
{
	RwUInt32 numNodes = RtAnimAnimationGetNumNodes(pAnimation);
	if (numNodes != m_csBoneNodeMap.size()) {
		debugPrint("Animation node count doesn't match that of skeleton.");
		return;
	}
	for (std::vector<cgltf_node*>::iterator it = m_csBoneNodeMap.begin(); it != m_csBoneNodeMap.end(); it++) {
		if (!*it) {
			debugPrint("Bone mapping is incomplete.");
			return;
		}
	}
	cgltf_animation* anim = m_csArenaAnimation.Get(&m_stData.animations_count);
	anim->name = name;
	anim->channels = &m_csArenaAnimationChannel.pool[m_csArenaAnimationChannel.cursor];
	anim->samplers = &m_csArenaAnimationSampler.pool[m_csArenaAnimationSampler.cursor];
	RpHAnimKeyFrame* frame= (RpHAnimKeyFrame*)pAnimation->pFrames;
	RpHAnimKeyFrame* head[256];
	void* frames = pAnimation->pFrames;
	memset(head, 0, sizeof(head));
	for (RwUInt32 i = 0; i < numNodes; i++) {
		head[i] = getKeyFrame(pAnimation, i);
	}
	for (RwUInt32 i = 0; i < numNodes; i++) {
		RpHAnimKeyFrame* kf = getKeyFrame(pAnimation, i);
		RwUInt32 index = numNodes;
		RwUInt32 count = 0;
		while (kf) {
			count++;
			// Find next key frame;
			RpHAnimKeyFrame* next = NULL;
			for (RwUInt32 j = index; j < (RwUInt32)pAnimation->numFrames; j++) {
				RpHAnimKeyFrame* f = getKeyFrame(pAnimation, j);
				if (f->prevFrame == kf) {
					index = j + 1;
					next = f;
					break;
				}
			}
			if (!next) {
				break;
			}
			kf = next;
		}
		kf = getKeyFrame(pAnimation, i);
		index = numNodes;
		cgltf_size stride = (sizeof(float) + sizeof(RtQuat) + sizeof(RwV3d));
		cgltf_size size = count * stride;
		cgltf_size offset = AppendBuffer(size);
		cgltf_size frameIndex = 0;
		float timeMin = kf->time;
		float timeMax = kf->time;
		while (kf) {
			memcpy((void*)((cgltf_size)m_stData.buffers->data + offset + frameIndex * sizeof(float)), &kf->time, sizeof(float));
			memcpy((void*)((cgltf_size)m_stData.buffers->data + offset + count * sizeof(float) + frameIndex * sizeof(RtQuat)), &kf->q, sizeof(RtQuat));
			memcpy((void*)((cgltf_size)m_stData.buffers->data + offset + count * (sizeof(float) + sizeof(RtQuat)) + frameIndex * sizeof(RwV3d)), &kf->t, sizeof(RwV3d));
			frameIndex++;
			timeMin = min(timeMin, kf->time);
			timeMax = max(timeMax, kf->time);
			// Find next key frame;
			RpHAnimKeyFrame* next = NULL;
			for (RwUInt32 j = index; j < (RwUInt32)pAnimation->numFrames; j++) {
				RpHAnimKeyFrame* f = getKeyFrame(pAnimation, j);
				if (f->prevFrame == kf) {
					index = j + 1;
					next = f;
					break;
				}
			}
			if (!next) {
				break;
			}
			kf = next;
		}
		// Create buffer views and accessors.
		cgltf_buffer_view* timeView = m_csArenaBufferView.Get(&m_stData.buffer_views_count);
		timeView->buffer = m_stData.buffers;
		timeView->offset = offset;
		timeView->size = count * sizeof(float);
		timeView->type = cgltf_buffer_view_type_invalid;
		cgltf_accessor* timeAccessor = m_csArenaAccessor.Get(&m_stData.accessors_count);
		timeAccessor->component_type = cgltf_component_type_r_32f;
		timeAccessor->type = cgltf_type_scalar;
		timeAccessor->count = count;
		timeAccessor->buffer_view = timeView;
		timeAccessor->has_min = timeAccessor->has_max = true;
		timeAccessor->min[0] = timeMin;
		timeAccessor->max[0] = timeMax;
		cgltf_buffer_view* rotationView = m_csArenaBufferView.Get(&m_stData.buffer_views_count);
		rotationView->buffer = m_stData.buffers;
		rotationView->offset = offset + count * sizeof(float);
		rotationView->size = count * sizeof(RtQuat);
		rotationView->type = cgltf_buffer_view_type_invalid;
		cgltf_accessor* rotationAccessor = m_csArenaAccessor.Get(&m_stData.accessors_count);
		rotationAccessor->component_type = cgltf_component_type_r_32f;
		rotationAccessor->type = cgltf_type_vec4;
		rotationAccessor->count = count;
		rotationAccessor->buffer_view = rotationView;
		cgltf_buffer_view* translationView = m_csArenaBufferView.Get(&m_stData.buffer_views_count);
		translationView->buffer = m_stData.buffers;
		translationView->offset = offset + count * sizeof(float) + count * sizeof(RtQuat);
		translationView->size = count * sizeof(RwV3d);
		translationView->type = cgltf_buffer_view_type_invalid;
		cgltf_accessor* translationAccessor = m_csArenaAccessor.Get(&m_stData.accessors_count);
		translationAccessor->component_type = cgltf_component_type_r_32f;
		translationAccessor->type = cgltf_type_vec3;
		translationAccessor->count = count;
		translationAccessor->buffer_view = translationView;
		// Create channels and samplers.
		cgltf_animation_sampler* rotationSampler = m_csArenaAnimationSampler.Get(&anim->samplers_count);
		rotationSampler->input = timeAccessor;
		rotationSampler->output = rotationAccessor;
		rotationSampler->interpolation = cgltf_interpolation_type_linear;
		cgltf_animation_channel* rotationChannel = m_csArenaAnimationChannel.Get(&anim->channels_count);
		rotationChannel->sampler = rotationSampler;
		rotationChannel->target_node = m_csBoneNodeMap[i];
		rotationChannel->target_path = cgltf_animation_path_type_rotation;
		cgltf_animation_sampler* translationSampler = m_csArenaAnimationSampler.Get(&anim->samplers_count);
		translationSampler->input = timeAccessor;
		translationSampler->output = translationAccessor;
		translationSampler->interpolation = cgltf_interpolation_type_linear;
		cgltf_animation_channel* translationChannel = m_csArenaAnimationChannel.Get(&anim->channels_count);
		translationChannel->sampler = translationSampler;
		translationChannel->target_node = m_csBoneNodeMap[i];
		translationChannel->target_path = cgltf_animation_path_type_translation;
	}
}
