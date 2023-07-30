#ifndef __AGCU_EVENT_EFFECT_H__
#define __AGCU_EVENT_EFFECT_H__

#include <rwcore.h>
#include <rpworld.h>

class AgpdItem;
class AgpmItem;
enum AgpmItemPart;
class AgpdCharacter;
struct AgcdCharacter;
class AgcmCharacter;

#define AGCUEVENTEFFECT_CLUMP_MAX_ATOMIC	10

struct stAgcuEventEffect_SearchPartIDSet
{
	PVOID		m_pClass;
	RpAtomic*	m_pastAtomic[AGCUEVENTEFFECT_CLUMP_MAX_ATOMIC];
	RwInt32		m_lCount;
	RwInt32		m_lPartID;
};

// PartID�� ���� atomic�� ã��...
RwInt32 AgcuEventEffect_SearchAtomicPartID(RpClump *pstBase, RpAtomic **pastDestAtomics, RwInt32 lPartID);
RwInt32 AgcuEventEffect_GetAtomicPartID(RpAtomic *atomic);
RpAtomic *AgcuEventEffect_SearchAtomicPartIDCB(RpAtomic *atomic, void *data);

// Equip-slot�� �ִ� ���� ��������...
AgpdItem *AgcuEventEffect_GetEquipItem(AgpmItem *pModule, AgpdCharacter *pcsAgpdCharacter, AgpmItemPart ePart);

// ĳ���� ���� hierarchy�� ������ anim�� duration�� �����´�...
FLOAT AgcuEventEffect_GetCharacterCurrentAnimDuration(AgcmCharacter *pModule, AgpdCharacter *pcsAgpdCharacter);
FLOAT AgcuEventEffect_GetCharacterCurrentAnimDuration(AgcdCharacter *pcsAgcdCharacter);

#endif // __AGCU_EVENT_EFFECT_H__
