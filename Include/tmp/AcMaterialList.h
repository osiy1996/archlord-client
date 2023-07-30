// MaterialList.h: interface for the AcMaterialList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATERIALLIST_H__A54B2F06_7695_41FA_8970_1FDB56636CD4__INCLUDED_)
#define AFX_MATERIALLIST_H__A54B2F06_7695_41FA_8970_1FDB56636CD4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AcTextureList.h"
#include "ApmMap.h"
#include "AuList.h"

#define	ALEF_MATERIAL_LIST_NO_MATERIAL	(0)

#define	ALEF_MATERIAL_DEFAULT_SURFACEPROPERTY_AMBIENT	0.3f
#define	ALEF_MATERIAL_DEFAULT_SURFACEPROPERTY_DIFFUSE	0.7f
#define	ALEF_MATERIAL_DEFAULT_SURFACEPROPERTY_SPECULAR	0.0f

struct RtWorldImport;

class AcMaterialList  
{
public:
	static AcTextureList *	m_pTextureList	;
	RpGeometry *	m_pGeometry		;
	
	struct	MaterialData
	{
		bool			bDeleted			;	// ������� ������ üũ�ϴ� �÷���.
		RpMaterial *	pMateterial			;
		int				matindex			;
		UINT32			pIndex[ TD_DEPTH ]	;	// �͵��� ���� ���� ����.
	};

	AuList< MaterialData >	list;	// ����Ÿ�� ����ٰ� ������.
										// ���� ���� ����Ÿ�� ���带 �����ϸ鼭 Material �� �߰��ϸ鼭 �ε����� �����ϱ⶧����
										// WorldImport�� �����ϴ� MainWindow::CreateMapSectorImport ���� �ۼ��Ѵ�.

	INT32	GetMatIndex		(	UINT32 firsttextureindex								,
								UINT32 secondtextureindex	= ALEF_TEXTURE_NO_TEXTURE	,
								UINT32 thirdtexutreindex	= ALEF_TEXTURE_NO_TEXTURE	,
								UINT32 fourthtextureindex	= ALEF_TEXTURE_NO_TEXTURE	,
								UINT32 fifthtexutreindex	= ALEF_TEXTURE_NO_TEXTURE	,
								UINT32 sixthtexutreindex	= ALEF_TEXTURE_NO_TEXTURE	);

	INT32	GetMatIndex		(	UINT32 * pIndex	);

	void	RemoveTexture	( int matindex		);
	void	RemoveTexture	( RpMaterial * pMat	);
	void	RemoveAll		(					);

	AcMaterialList();
	virtual ~AcMaterialList();

protected:

	MaterialData *	Find			(	int			matindex		);									// ��Ƽ���� �ε����� ã��.
	MaterialData *	Find			(	RpMaterial *	pMat		);									// ��Ƽ���� �����ͷ� ã��.
	MaterialData *	Find			(	UINT32 firsttextureindex		,
										UINT32 secondtextureindex		,
										UINT32 thirdtexutreindex		,
										UINT32 fourthtextureindex		,
										UINT32 fifthtexutreindex		,
										UINT32 sixthtextureindex		);// �ؽ��� �ε����� ã��.
	MaterialData *	Find			(	UINT32 * pIndex		);// �ؽ��� �ε����� ã��.
	MaterialData *	GetEmptyMaterial( void							);									// ����ִ� Material�� ��.
};

#endif // !defined(AFX_MATERIALLIST_H__A54B2F06_7695_41FA_8970_1FDB56636CD4__INCLUDED_)
