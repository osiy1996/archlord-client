#ifndef		_ACUISKILLTREE_H_
#define		_ACUISKILLTREE_H_

#include <AgcmUIControl/AcUIGrid.h>

// AcUISkillTree�� �ֿ� Ư¡ 
// - Window Size : Width , Height �ڵ� ���� �ȴ�
// - 

// Command Message
enum 
{
	UICM_SKILLTREE_MAX_MESSAGE							= UICM_GRID_MAX_MESSAGE,
};


typedef enum
{
	ACUI_SKILLTREE_TYPE_1_3								= 0, // �տ����� Row �ڿ����� Column (��, 1_3�� 1 Row 3 Column)
	ACUI_SKILLTREE_TYPE_2_3,
	ACUI_SKILLTREE_TYPE_3_3,
	ACUI_SKILLTREE_TYPE_1_10,
	ACUI_SKILLTREE_TYPE_1_2,
	ACUI_SKILLTREE_TYPE_2_2,
	ACUI_SKILLTREE_MAX_TYPE,
} AcUISkillTreeType;


class AcUISkillTree : public AcUIGrid 
{
public:
	INT32												m_lSkillTreeMode;	
	INT32												m_lOneRowItemNum;		// Skill Tree������ �� Row�� �ִ� Item �� - �ʱ�� 3
	INT32												m_alSkillBackImageID[ ACUI_SKILLTREE_MAX_TYPE ];

	AcUISkillTree( void );
	virtual ~AcUISkillTree( void );

public:
	virtual void			MoveGridItemWindow			( void );

	BOOL					SetSkillBackImage			( AcUISkillTreeType eType, INT32 lImageID );
	INT32					GetSkillBackImage			( AcUISkillTreeType eType );

private :
	void					_MoveGridSkill				( void );
	void					_MoveGridSkillOneLineMode	( void );
	void					_UpdateSkillTreeSize		( BOOL bOneLineTreeMode );
};

#endif		// _ACUISKILLTREE_H_