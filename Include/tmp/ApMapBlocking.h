// ApMapBlocking.h: interface for the ApMapBlocking class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APMAPBLOCKING_H__AFE7CD7A_73BF_447F_BECE_FD97DB52D333__INCLUDED_)
#define AFX_APMAPBLOCKING_H__AFE7CD7A_73BF_447F_BECE_FD97DB52D333__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApBase.h"
#include "ApModule.h"

////////////////////////////////////////////////////////////////////
// class	: ApMapBlocking
// Created  : ������ (2002-04-24 ���� 3:24:10)
// Note     : �ʸ�⿡�� ��ŷ ��� �����ϱ� ���� �༮.
// 
// -= Update Log =-
////////////////////////////////////////////////////////////////////

enum	BlockingType
{
	TYPE_NONE		,	// �ʱ�ȭ ���� �ʾ���;.
	TYPE_BOX		,
	TYPE_SPHERE		,
	TYPE_CYLINDER
};


class ApMapBlocking  
{
public:

public:
	// Attribute
	int		type;	// Blocking Type ������. 

	// ���� ��ŷ�� ���� ����.
	union	data
	{
		AuBOX			box			;
		AuSPHERE		sphere		;
		AuCYLINDER		cylinder	;
	};

	// Operations
	ApMapBlocking();
	virtual ~ApMapBlocking();

	BOOL IsPassThis( AuPOS pos1 , AuPOS pos2 , AuPOS *pCollisionPoint  = NULL );
	BOOL IsInTriangle( AuPOS pos );

};

#endif // !defined(AFX_APMAPBLOCKING_H__AFE7CD7A_73BF_447F_BECE_FD97DB52D333__INCLUDED_)
