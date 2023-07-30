#ifndef			_AGPASKILLTEMPLATE_H_
#define			_AGPASKILLTEMPLATE_H_

#include <ApBase/ApBase.h>
#include <AgpmSkill/AgpdSkill.h>
#include <ApAdmin/ApAdmin.h>

class AgpaSkillTemplate : public ApAdmin
{
public:
	AgpaSkillTemplate();
	~AgpaSkillTemplate();
	
	//Skill Template Manage Function
	AgpdSkillTemplate*	AddSkillTemplate(AgpdSkillTemplate* pcsSkillTemplate);
	AgpdSkillTemplate*	GetSkillTemplate(INT32 lTID);
	AgpdSkillTemplate*	GetSkillTemplate(CHAR *szName);
	BOOL				RemoveSkillTemplate(INT32 lTID);
	BOOL				AddTemplateStringKey(INT32 lTID, CHAR *szTName);
};

#endif 	