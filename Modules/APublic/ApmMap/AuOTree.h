#ifndef _AUOTREE_H_
#define	_AUOTREE_H_

#include <ApBase/ApBase.h>

// Leaf Indexing Order
//

class AuOTree
{
protected:
	int				m_nType		;	// Ÿ�� ����.
	AuOTree *		m_pParent	;

	union
	{
		BOOL		bBlocking	;
		AuOTree *	pLeaf		;	// 8�� �����ͷ� ��� ��������..
	} m_Data;

	void			SetUpLeaf	( float x , float y , float z , float width , float unitsize , BOOL bBlock , AuOTree *pParent );

public:
	// Range..
	float			m_fStartX	;
	float			m_fStartY	;
	float			m_fStartZ	;
	float			m_fWidth	;
	float			m_fUnitSize	;

	enum	TYPE
	{
		LEAF	,
		BRANCH	,
		NOT_READY
	};

	// Creator / Destructor
	AuOTree();
	~AuOTree();

	// Parn �۾�, Destroy�� �ʿ��ϴ�. �� ����� NOT_READY ���·�
	void		Destroy		();
	// Parn �۾�, Init() �Ȱǰ�?
	BOOL		IsReady		() const { return ( GetType() != NOT_READY ); }

	// Get Functions...
	int			GetType		() const { return m_nType		; }

	BOOL		IsRoot		() const { if( m_pParent ) return FALSE; else return TRUE; }
	// ���� �༮�� Root ���� ����..
	AuOTree *	GetRoot		() const ;
	// Root �� ��..


	//////////////////////////////////////////////////////////////
	// ROOT/LEAF Operations...
	//////////////////////////////////////////////////////////////

		// ���� ���� ��ƾ..
		BOOL		CreateLeaf	(	BOOL bBlock1 = TRUE,
									BOOL bBlock2 = TRUE,
									BOOL bBlock3 = TRUE,
									BOOL bBlock4 = TRUE,
									BOOL bBlock5 = TRUE,
									BOOL bBlock6 = TRUE,
									BOOL bBlock7 = TRUE,
									BOOL bBlock8 = TRUE
								);

		// Parn �۾�
		// unitsize ���� Leaf�� ��� �����ع�����.
		BOOL		CreateLeafAll	();

		// �� ������ ����..
		// ����Ʈ������ TRUE ��������.. �˾Ƽ� ���� �ٲپ� �־����.

		BOOL		DeleteLeaf	(	BOOL bBlock = TRUE );
		// �� ��忡 ����ž��ִ� ��� ���� ����.
		// ������ �����ϰ� �� ���� ����� ��ŷ ����.
		// ���� ��带 ���ýú��ϰ� ������..

	//////////////////////////////////////////////////////////////
	// ROOT Operations...
	//////////////////////////////////////////////////////////////
		BOOL		Init		( float x , float y , float z , float width , float unitsize , BOOL bBlock = TRUE );
		// �⺻ ��Ʈ �����ϴ� ���..  x,y,z ��ǥ�� �ְ� , width * width * width ��ŭ�� �Թ�ü..
		void		Optimize	();
		// ���� ������ ���� ������ ���� ������. ������ �ѹ��� �����Ѵ�.;

	//////////////////////////////////////////////////////////////
	// LEAF Operations...
	//////////////////////////////////////////////////////////////
		BOOL		SetBlocking	( BOOL bBlock					);
		// Parn �۾�, Blocking ��Ҹ� �־ ���� LEAF ��忡 ���� ����� �Ѵ�.
		BOOL		AddBlocking	( AuBLOCKING *pstBlocking		);

		// TRUE / FALSE
		// LEAF ������ ���� ����..
		BOOL		GetBlocking	( float x , float y , float z	) const ;
		AuOTree *	GetParent	( void							) const { return m_pParent;}
		AuOTree *	GetLeaf		( int index = 0					) const ;
		// 0~7 ����..
		// ������ �� ����..
		// 0 �̸� ��̸� �����Ѵٰ� ���� ok...

	//////////////////////////////////////////////////////////////
	// Tree traveling functinos... 
	// �浹 üũ ��ǵ�..
	//////////////////////////////////////////////////////////////
		inline BOOL		IsInThisBox	( float x , float y , float z	) const 
		{
			if(	m_fStartX <= x && m_fStartX + m_fWidth > x	&&
				m_fStartY <= y && m_fStartY + m_fWidth > y	&&
				m_fStartX <= z && m_fStartZ + m_fWidth > z	)
					return TRUE		;
			else	return FALSE	;
		}
		// �� ����Ʈ�� ���� �ȿ� �ֳ�?..

};


#endif // #ifndef _AUOTREE_H_
