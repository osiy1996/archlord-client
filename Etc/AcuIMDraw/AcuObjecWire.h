// AcuObjecWire.h
// -----------------------------------------------------------------------------
//                       ____  _       _           _   _   _ _               _     
//     /\               / __ \| |     (_)         | | | | | (_)             | |    
//    /  \    ___ _   _| |  | | |__    _  ___  ___| | | | | |_ _ __  ___    | |__  
//   / /\ \  / __| | | | |  | | '_ \  | |/ _ \/ __| | | | | | | '__|/ _ \   | '_ \ 
//  / ____ \| (__| |_| | |__| | |_) | | |  __/ (__|  V _ V  | | |  |  __/ _ | | | |
// /_/    \_\\___|\__,_|\____/|_.__/  | |\___|\___|\__/ \__/|_|_|   \___|(_)|_| |_|
//                                   _/ |                                          
//                                  |__/                                           
//
// post fx effect
//
// -----------------------------------------------------------------------------
// Originally created on 05/13/2005 by Kyeongsam Moon
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#ifndef	_H_ACUOBJECWIRE_20050513
#define _H_ACUOBJECWIRE_20050513

// -----------------------------------------------------------------------------

#include <list>
#include <rwcore.h>
#include <rpworld.h>

class AcuObjecWire
{
	typedef std::list<RpClump*>	CONTAINER;
	typedef CONTAINER::iterator	CLUMP_ITR;

	typedef RwInt32 (AcuObjecWire::*fptrRender)(RpClump*);
public:
	enum	eFlag
	{
		EFLAG_SPHERE		=	0x00000001,
		EFLAG_CLUMP			=	0x00000002,
		EFLAG_COLLISION		=	0x00000004,
		EFLAG_PICKING		=	0x00000008,
	};

	// Construction/Destruction
protected:
	AcuObjecWire();
public:
	virtual	~AcuObjecWire();
	static AcuObjecWire& bGetInst();

	// Operators

	// Accessors

	// Interface methods
	RwInt32	bAddClump(RpClump* pClump);
	RwInt32	bRemClump(RpClump* pClump);
	RwInt32	bClear();

	RwInt32	bRender();
	RwInt32 bRenderAtomic(RpAtomic* atom, RwUInt32 colr=0xFF80FFFF);

	RwInt32 bRenderClump(RpClump* clump);	//�׸��Ÿ� �ٽ� �ȱ׸�(bOnIdle ���� ����Ʈ�� ����Ƿ� bOnIdle�� �������� �ҷ�����)
	void	bOnIdle(void);					//�������Ӹ��� ���ٰ͵�

	RwInt32	bRenderClumpOnce(RpClump* clump);	//�׸��ŵ� �ȱ׸��ŵ� �Ű� �Ⱦ�
	RwInt32 bRenderSphere( const RwSphere& sphere );

	void	bOnOffSphere(bool on);
	void	bOnOffClump(bool on);
	void	bOnOffCollision(bool on);
	void	bOnOffPicking(bool on);

private:

	// Data members
	RwUInt32	m_flag;
	CONTAINER	m_containerClump;			//��Ƽ� ��ť��
	CONTAINER	m_containerClumpImdeate;	//�ϳ��ϳ� �׸���

	fptrRender	m_fptrRenderSphere;
	fptrRender	m_fptrRenderClump;
	fptrRender	m_fptrRenderCollision;
	fptrRender	m_fptrRenderPicking;

	// Implementation methods
	RwInt32	vRenderNothing	(RpClump* clump) { clump; return 0L; };
	RwInt32	vRenderObj		(RpClump* clump);
	RwInt32 vRenderSphere	(RpClump* clump);
	RwInt32 vRenderClump	(RpClump* clump);
	RwInt32	vRenderCollision(RpClump* clump);
	RwInt32	vRenderPicking	(RpClump* clump);
};

#endif // _H_ACUOBJECWIRE_20050513
// -----------------------------------------------------------------------------
// AcuObjecWire.h - End of file
// -----------------------------------------------------------------------------

