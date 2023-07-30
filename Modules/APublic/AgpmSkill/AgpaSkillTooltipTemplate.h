#ifndef	_AGPASKILLTOOLTIPTEMPLATE_H_
#define	_AGPASKILLTOOLTIPTEMPLATE_H_

#include <ApAdmin/ApAdmin.h>
#include <AgpmSkill/AgpdSkill.h>

class AgpaSkillTooltipTemplate : public ApAdmin
{
public:
	AgpaSkillTooltipTemplate();
	~AgpaSkillTooltipTemplate();
	
	//Skill Template Manage Function
	AgpdSkillTooltipTemplate*	AddSkillTooltipTemplate(AgpdSkillTooltipTemplate* pcsSkillTooltipTemplate);
	AgpdSkillTooltipTemplate*	GetSkillTooltipTemplate(CHAR *szName);
	BOOL						RemoveSkillTooltipTemplate(CHAR *szSkillName);
};

#endif	//_AGPASKILLTOOLTIPTEMPLATE_H_
