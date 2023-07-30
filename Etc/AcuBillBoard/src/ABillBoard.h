#ifndef _ABILLBOARD_H_
#define _ABILLBOARD_H_

#include "rwcore.h"
#include "rpworld.h"
#include "skeleton.h"

enum E_TFormat
{
	TFORMAT_BMP, 
	TFORMAT_RAS, 
	TFORMAT_PNG, 
	TFORMAT_TIF
};

class AcuBillBoard
{
public:
	
	RwChar		m_szTPath[256];		//Texture�� Path				
	E_TFormat	m_eTFormat;			//Texture�� ����				
	RwChar		m_szTName[256];		//Texture�� �̸�(Ȯ��������)
	RwV3d		m_v3dOriginalPos;	//�������� ���� ��ġ 
	RwReal		m_TexCoords_U;		//u��ǥ
	RwReal		m_TexCoords_V;		//v��ǥ
	

public:
	AcuBillBoard();				//������ 
	~AcuBillBoard();			//�Ҹ���
	RpAtomic*	CreateBBAtomic( );					//�ѹ� ��ĥ������ m_nUseBB�� �ϳ��� �����Ѵ�. 
	RpAtomic*	CreateDoubleSideBoardAtomic();		//��� �÷��� ���鶧 ���� 
	RwBool		RenderIdle( RpAtomic* Atomic, RwCamera* Camera, RwReal fRate1 = 1.0f, RwReal fRate2 = 1.0f );	//ī�޶� ���⿡ �°� �����带 �����ִ� ���� �Ѵ�. 
	
	void		SetTexturePath( RwChar sz_SetPath[30] );
	void		SetTextureName( RwChar sz_SetTName[30] );

};



#endif