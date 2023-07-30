#include "CharacterPropertyCallBack.h"
#include "CUiCharacterProperty.h"
#include <AgpmPvP/AgpmPvP.h>
#include <AgpmCharacter/AgpmCharacter.h>
#include <AgpmFactors/AgpmFactors.h>
#include <AgpmCombat/AgpmCombat.h>
#include <AgpmItem/AgpmItem.h>
#include <AgcmUIManager2/AgcmUIManager2.h>
#include <AgcmUICharacter/AgcmUICharacter.h>
#include <AgcmUIControl/AcUIEdit.h>
#include <AgpmSkill/AgpmSkill.h>
#include <AgpmGrid/AgpmGrid.h>
#include <AgpmEventSkillMaster/AgpmEventSkillMaster.h>
#include <AgcModule/AgcEngine.h>
#include <AgcmResourceLoader/AgcmResourceLoader.h>

BOOL MakeFormattedString( char* pBuffer, char* pFormat, int nValueOriginal, int nValueCurrent, DWORD dwBaseColor, DWORD dwUpperColor, DWORD dwLowerColor )
{
	if( !pBuffer ) return FALSE;
	if( !pFormat || strlen( pFormat ) <= 0 ) return FALSE;

	char strAddValue[ 32 ] = { 0, };

	DWORD dwColor = 0xFFFFFFFF;
	if( nValueOriginal > nValueCurrent )
	{
		dwColor = dwLowerColor;
		sprintf_s( strAddValue, sizeof( char ) * 32, "(%d)", nValueCurrent - nValueOriginal );
	}
	else if( nValueOriginal < nValueCurrent )
	{
		dwColor = dwUpperColor;
		sprintf_s( strAddValue, sizeof( char ) * 32, "(+%d)", nValueCurrent - nValueOriginal );
	}
	else
	{
		dwColor = dwBaseColor;
	}

	// ���İ� �κ��� �߶󳽴�.. �Ľ����� �����ϱ�..
	dwColor = ( dwColor << 8 ) >> 8;

	char strValue[ 128 ] = { 0, };
	sprintf_s( strValue, sizeof( char ) * 128, pFormat, nValueCurrent );
	sprintf_s( pBuffer, sizeof( char ) * 128, "<C%d>%s<C16777215>%s", dwColor, strValue, strAddValue );

	return TRUE;
}

BOOL MakeFormattedStringUINT( char* pBuffer, char* pFormat, int nValueCurrent, DWORD dwBaseColor, DWORD dwUpperColor )
{
	if( !pBuffer ) return FALSE;
	if( !pFormat || strlen( pFormat ) <= 0 ) return FALSE;

	DWORD dwColor = nValueCurrent > 0 ? dwUpperColor : dwBaseColor;

	// ���İ� �κ��� �߶󳽴�.. �Ľ����� �����ϱ�..
	dwColor = ( dwColor << 8 ) >> 8;

	char strValue[ 128 ] = { 0, };
	sprintf_s( strValue, sizeof( char ) * 128, pFormat, nValueCurrent );
	sprintf_s( pBuffer, sizeof( char ) * 128, "<C%d>%s<C16777215>", dwColor, strValue );

	return TRUE;
}




BOOL fn_CharacterProperty_PhysicalDamage( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;

	AgpmCombat* ppmCombat = ( AgpmCombat* )g_pEngine->GetModule( "AgpmCombat" );
	if( !ppmCombat ) return FALSE;

	AgpmItem* ppmItem = ( AgpmItem* )g_pEngine->GetModule( "AgpmItem" );
	if( !ppmItem ) return FALSE;

	BOOL bIsEquipWeapon = ppmItem->IsEquipWeapon( ppdCharacter );

	int nValueOriginal = ppmCombat->CalcBasePhysicalAttack( ppdCharacter, FALSE );
	int nValueCurrent = ppmCombat->CalcPhysicalAttack( ppdCharacter, NULL, bIsEquipWeapon, TRUE, FALSE );

	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, nValueOriginal, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_HeroicDamage( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;

	AgpmCombat* ppmCombat = ( AgpmCombat* )g_pEngine->GetModule( "AgpmCombat" );
	if( !ppmCombat ) return FALSE;

	int nValueOriginal = 0;
	int nValueCurrent = ( int )ppmCombat->GetHeroicDamageMax( ppdCharacter );

	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, nValueOriginal, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_RateAttack( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;

	AgpmCombat* ppmCombat = ( AgpmCombat* )g_pEngine->GetModule( "AgpmCombat" );
	if( !ppmCombat ) return FALSE;

	AgpmFactors* ppmFactor = ( AgpmFactors* )g_pEngine->GetModule( "AgpmFactors" );
	if( !ppmFactor ) return FALSE;

	AuCharClassType eClass = ( AuCharClassType )ppmFactor->GetClass( &ppdCharacter->m_pcsCharacterTemplate->m_csFactor );

	int nValueOriginal = ( int )ppmCombat->GetBaseAR( ppdCharacter, eClass );
	int nValueCurrent = ( int )ppmCombat->GetAR( ppdCharacter, eClass );

	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, nValueOriginal, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_RateHit( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;

	AgpmCombat* ppmCombat = ( AgpmCombat* )g_pEngine->GetModule( "AgpmCombat" );
	if( !ppmCombat ) return FALSE;

	AgpmFactors* ppmFactor = ( AgpmFactors* )g_pEngine->GetModule( "AgpmFactors" );
	if( !ppmFactor ) return FALSE;

	int nLevel = ppmFactor->GetLevel( &ppdCharacter->m_csFactor );

	int nValueOriginal = 0;
	ppmFactor->GetValue( &ppdCharacter->m_pcsCharacterTemplate->m_csLevelFactor[ nLevel ], &nValueOriginal, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_HIT_RATE );
	
	int nValueCurrent = 0;
	ppmFactor->GetValue( &ppdCharacter->m_csFactor, &nValueCurrent, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_HIT_RATE );

	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, nValueOriginal, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_DurationCastSkill( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;

	AgpmCombat* ppmCombat = ( AgpmCombat* )g_pEngine->GetModule( "AgpmCombat" );
	if( !ppmCombat ) return FALSE;

	AgpmFactors* ppmFactor = ( AgpmFactors* )g_pEngine->GetModule( "AgpmFactors" );
	if( !ppmFactor ) return FALSE;

	int nLevel = ppmFactor->GetLevel( &ppdCharacter->m_csFactor );

	int nValueOriginal = 0;
	ppmFactor->GetValue( &ppdCharacter->m_pcsCharacterTemplate->m_csLevelFactor[ nLevel ], &nValueOriginal, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SKILL_CAST );
	
	int nValueCurrent = 0;
	ppmFactor->GetValue( &ppdCharacter->m_csFactor, &nValueCurrent, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SKILL_CAST );

	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, nValueOriginal, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_DurationSkillCoolTime( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;

	AgpmCombat* ppmCombat = ( AgpmCombat* )g_pEngine->GetModule( "AgpmCombat" );
	if( !ppmCombat ) return FALSE;

	AgpmFactors* ppmFactor = ( AgpmFactors* )g_pEngine->GetModule( "AgpmFactors" );
	if( !ppmFactor ) return FALSE;

	int nLevel = ppmFactor->GetLevel( &ppdCharacter->m_csFactor );

	int nValueOriginal = 0;
	ppmFactor->GetValue( &ppdCharacter->m_pcsCharacterTemplate->m_csLevelFactor[ nLevel ], &nValueOriginal, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SKILL_DELAY );
	
	int nValueCurrent = 0;
	ppmFactor->GetValue( &ppdCharacter->m_csFactor, &nValueCurrent, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SKILL_DELAY );

	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, nValueOriginal, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_SpeedMove( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;

	AgpmCombat* ppmCombat = ( AgpmCombat* )g_pEngine->GetModule( "AgpmCombat" );
	if( !ppmCombat ) return FALSE;

	AgpmFactors* ppmFactor = ( AgpmFactors* )g_pEngine->GetModule( "AgpmFactors" );
	if( !ppmFactor ) return FALSE;

	int nValueOriginal = 0;
	ppmFactor->GetValue( &ppdCharacter->m_pcsCharacterTemplate->m_csFactor, &nValueOriginal, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST );
	
	int nValueCurrent = 0;
	ppmFactor->GetValue( &ppdCharacter->m_csFactor, &nValueCurrent, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST );

	nValueCurrent = ( int )( ( ( double )nValueCurrent * ( double ) 100 ) / ( ( double )nValueOriginal + FLT_EPSILON ));

	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, 100, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_SpeedAttack( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;

	AgcmUICharacter* pcmUICharacter = ( AgcmUICharacter* )g_pEngine->GetModule( "AgcmUICharacter" );
	if( !pcmUICharacter ) return FALSE;

	AgpmFactors* ppmFactor = ( AgpmFactors* )g_pEngine->GetModule( "AgpmFactors" );
	if( !ppmFactor ) return FALSE;

	float fDefaultAttackSpeed = 100.0f;
	float fValueCurrent = 0.0f;

	float fValueOriginal = fDefaultAttackSpeed;

	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )g_pEngine->GetModule( "AgpmCharacter" );
	if( !ppmCharacter ) return FALSE;

	AgpdCharacterTemplate* ppdOriginalTemplate = ppmCharacter->GetCharacterTemplate( ppdCharacter->m_lOriginalTID );
	AgpdCharacterTemplate* ppdCharacterTemplate = ppdOriginalTemplate ? ppdOriginalTemplate : ppdCharacter->m_pcsCharacterTemplate;
	
	float fCharacterOriginal = 0.0f;
	ppmFactor->GetValue( &ppdCharacterTemplate->m_csFactor, &fCharacterOriginal, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SPEED );

	float fCharacterResult = 0.0f;
	ppmFactor->GetValue( &ppdCharacter->m_csFactor, &fCharacterResult, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SPEED );

	AgpmItem* ppmItem = ( AgpmItem* )g_pEngine->GetModule( "AgpmItem" );
	AgpmGrid* ppmGrid = ( AgpmGrid* )g_pEngine->GetModule( "AgpmGrid" );
	if( ppmItem )
	{
		AgpdItem *		pcsAgpdItem				= NULL;
		AgpdItem *		pcsAgpdItemRight		= ppmItem->GetEquipSlotItem(ppdCharacter, AGPMITEM_PART_HAND_RIGHT);
		AgpdItem *		pcsAgpdItemLeft			= ppmItem->GetEquipSlotItem(ppdCharacter, AGPMITEM_PART_HAND_LEFT);

		INT32			lRace					= ppmFactor->GetRace(&ppdCharacterTemplate->m_csFactor);

		if(lRace == AURACE_TYPE_DRAGONSCION)
		{
			if( pcsAgpdItemRight && pcsAgpdItemRight->m_pcsGridItem && !pcsAgpdItemRight->m_pcsGridItem->IsDurabilityZero())
			{
				pcsAgpdItem = pcsAgpdItemRight;
			}

			if( !pcsAgpdItem )
			{
				if( pcsAgpdItemLeft && pcsAgpdItemLeft->m_pcsGridItem && !pcsAgpdItemLeft->m_pcsGridItem->IsDurabilityZero())
				{
					pcsAgpdItem = pcsAgpdItemLeft;
				}
			}
		}
		else
		{
			pcsAgpdItem = ppmItem->GetEquipWeapon(ppdCharacter);
		}

		if( pcsAgpdItem && pcsAgpdItem->m_pcsGridItem && !pcsAgpdItem->m_pcsGridItem->IsDurabilityZero() )
		{
			float fItemOriginalSpeed = 0.0f;
			float fItemResultSpeed = 0.0f;

			ppmFactor->GetValue(&pcsAgpdItem->m_pcsItemTemplate->m_csFactor, &fItemOriginalSpeed, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SPEED);
			ppmFactor->GetValue(&pcsAgpdItem->m_csFactor, &fItemResultSpeed, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SPEED);
			
			
			fValueCurrent = ((fCharacterResult * 100.0f) / fItemResultSpeed);
		}
		else
		{
			// ���⸦ �������� ���� ���¿��� Ż�Ϳ� Ÿ�� ������ ���ݼӵ��� ������ 100���� ǥ���Ѵ�.
			if( ppmCharacter->IsRideOn( ppdCharacter ) )
			{
				fValueCurrent = 100.0f;
			}
			else
			{
				fValueCurrent = ((fCharacterResult * 100.0f) / fCharacterOriginal);
			}
		}
	}

	// Rounding Off (������) ��¿�� ���� ������ �ڵ�
	/////////////////////////////////////////////////////////////////////////////////////
	fValueCurrent	+= 0.4f;
	float fRoundOff = (fValueCurrent - static_cast<INT32>(fValueCurrent));

	if(fRoundOff >= 0.5) fValueCurrent++;
	//////////////////////////////////////////////////////////////////////////////////////

	//int nValueOriginal = 100;
	//int nValueCurrent = pcmUICharacter->CalcCurrentZeroBaseProperty( ppdCharacter, eZeroBaseProperty_AttackSpeed );

	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, 
		(int)fValueOriginal, (int)fValueCurrent, 
		pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, 
		pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_RateCritical( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;

	AgpmSkill* ppmSkill = ( AgpmSkill* )g_pEngine->GetModule( "AgpmSkill" );
	AgpmEventSkillMaster* ppmEventSkillMaster = ( AgpmEventSkillMaster* )g_pEngine->GetModule( "AgpmEventSkillMaster" );
	AgcmUICharacter* pcmUICharacter = ( AgcmUICharacter* )g_pEngine->GetModule( "AgcmUICharacter" );
	if( !ppmSkill || !ppmEventSkillMaster || !pcmUICharacter ) return 0;

	// ���� ����� ��� ��ų�� ������.
	AgpdSkillAttachData* ppdSkillAttachData = ppmSkill->GetAttachSkillData( ( ApBase* )ppdCharacter );
	if( !ppdSkillAttachData ) return 0;

	int nValueOriginal = 0;
	int nValueCurrent = ppdSkillAttachData->m_stBuffedSkillCombatEffectArg.lMeleeCriticalProbability;

	// ������� ���������� �ö󰡴� ��ġ�� ���� ����Ͽ� �ջ����ش�.
	nValueCurrent += pcmUICharacter->CalcCurrentItemProperty( ppdCharacter, eZeroBaseProperty_RateCritical );

	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, nValueOriginal, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_CriticalDamage( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;

	AgpmSkill* ppmSkill = ( AgpmSkill* )g_pEngine->GetModule( "AgpmSkill" );
	AgpmEventSkillMaster* ppmEventSkillMaster = ( AgpmEventSkillMaster* )g_pEngine->GetModule( "AgpmEventSkillMaster" );
	AgcmUICharacter* pcmUICharacter = ( AgcmUICharacter* )g_pEngine->GetModule( "AgcmUICharacter" );
	if( !ppmSkill || !ppmEventSkillMaster || !pcmUICharacter ) return 0;

	// ���� ����� ��� ��ų�� ������.
	AgpdSkillAttachData* ppdSkillAttachData = ppmSkill->GetAttachSkillData( ( ApBase* )ppdCharacter );
	if( !ppdSkillAttachData ) return 0;

	int nValueOriginal = 0;
	int nValueCurrent = ppdSkillAttachData->m_stBuffedSkillCombatEffectArg.lMeleeDamageAdjustRate;

	// ������� ���������� �ö󰡴� ��ġ�� ���� ����Ͽ� �ջ����ش�.
	nValueCurrent += pcmUICharacter->CalcCurrentItemProperty( ppdCharacter, eZeroBaseProperty_CriticalDamage );

	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, nValueOriginal, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_IgnoreTargetRegistance( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;

	AgpmSkill* ppmSkill = ( AgpmSkill* )g_pEngine->GetModule( "AgpmSkill" );
	AgpmEventSkillMaster* ppmEventSkillMaster = ( AgpmEventSkillMaster* )g_pEngine->GetModule( "AgpmEventSkillMaster" );
	AgcmUICharacter* pcmUICharacter = ( AgcmUICharacter* )g_pEngine->GetModule( "AgcmUICharacter" );
	if( !ppmSkill || !ppmEventSkillMaster || !pcmUICharacter ) return 0;

	// ���� ����� ��� ��ų�� ������.
	AgpdSkillAttachData* ppdSkillAttachData = ppmSkill->GetAttachSkillData( ( ApBase* )ppdCharacter );
	if( !ppdSkillAttachData ) return 0;

	int nValueOriginal = 0;
	int nValueCurrent = ppdSkillAttachData->m_stBuffedSkillCombatEffectArg.lIgnoreAttributeDefence;

	// ������� ���������� �ö󰡴� ��ġ�� ���� ����Ͽ� �ջ����ش�.
	nValueCurrent += pcmUICharacter->CalcCurrentItemProperty( ppdCharacter, eZeroBaseProperty_IgnoreTargetRegistance );

	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, nValueOriginal, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_IgnoreTargetDefense( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;

	AgpmSkill* ppmSkill = ( AgpmSkill* )g_pEngine->GetModule( "AgpmSkill" );
	AgpmEventSkillMaster* ppmEventSkillMaster = ( AgpmEventSkillMaster* )g_pEngine->GetModule( "AgpmEventSkillMaster" );
	AgcmUICharacter* pcmUICharacter = ( AgcmUICharacter* )g_pEngine->GetModule( "AgcmUICharacter" );
	if( !ppmSkill || !ppmEventSkillMaster || !pcmUICharacter ) return 0;

	// ���� ����� ��� ��ų�� ������.
	AgpdSkillAttachData* ppdSkillAttachData = ppmSkill->GetAttachSkillData( ( ApBase* )ppdCharacter );
	if( !ppdSkillAttachData ) return 0;

	int nValueOriginal = 0;
	int nValueCurrent = ppdSkillAttachData->m_stBuffedSkillCombatEffectArg.lIgnorePhysicalDefence;

	// ������� ���������� �ö󰡴� ��ġ�� ���� ����Ͽ� �ջ����ش�.
	nValueCurrent += pcmUICharacter->CalcCurrentItemProperty( ppdCharacter, eZeroBaseProperty_IgnoreTargetDefense );

	//AgcmUICharacter* pcmUICharacter = ( AgcmUICharacter* )g_pEngine->GetModule( "AgcmUICharacter" );
	//if( !pcmUICharacter ) return FALSE;

	//int nValueOriginal = 0;
	//int nValueCurrent = pcmUICharacter->CalcCurrentZeroBaseProperty( ppdCharacter, eZeroBaseProperty_IgnoreTargetDefense );

	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, nValueOriginal, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_AttackRange( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;

	AgpmFactors* ppmFactor = ( AgpmFactors* )g_pEngine->GetModule( "AgpmFactors" );
	if( !ppmFactor ) return FALSE;

	int nValueOriginal = 0;
	ppmFactor->GetValue( &ppdCharacter->m_pcsCharacterTemplate->m_csFactor, &nValueOriginal, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_ATTACKRANGE );
	
	int nValueCurrent = 0;
	ppmFactor->GetValue( &ppdCharacter->m_csFactor, &nValueCurrent, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_ATTACKRANGE );

	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, nValueOriginal, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_PhysicalDefense( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;

	AgpmFactors* ppmFactor = ( AgpmFactors* )g_pEngine->GetModule( "AgpmFactors" );
	if( !ppmFactor ) return FALSE;

	int nLevel = ppmFactor->GetLevel( &ppdCharacter->m_csFactor );

	int nValueOriginal = 0;
	ppmFactor->GetValue( &ppdCharacter->m_pcsCharacterTemplate->m_csLevelFactor[ nLevel ], &nValueOriginal, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL );
	
	int nValueCurrent = 0;
	ppmFactor->GetValue( &ppdCharacter->m_csFactor, &nValueCurrent, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL );

	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, nValueOriginal, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_HeroicDefense( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;

	AgpmCombat* ppmCombat = ( AgpmCombat* )g_pEngine->GetModule( "AgpmCombat" );
	if( !ppmCombat ) return FALSE;

	int nValueOriginal = 0;
	int nValueCurrent = ( int )ppmCombat->GetHeroicDefense( ppdCharacter );
	
	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, nValueOriginal, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_DefenseRate( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;

	AgpmCombat* ppmCombat = ( AgpmCombat* )g_pEngine->GetModule( "AgpmCombat" );
	if( !ppmCombat ) return FALSE;

	AgpmFactors* ppmFactor = ( AgpmFactors* )g_pEngine->GetModule( "AgpmFactors" );
	if( !ppmFactor ) return FALSE;

	AuCharClassType eClass = ( AuCharClassType )ppmFactor->GetClass( &ppdCharacter->m_pcsCharacterTemplate->m_csFactor );

	int nValueOriginal = ( int )ppmCombat->GetBaseDR( ppdCharacter, eClass );
	int nValueCurrent = ( int )ppmCombat->GetDR( ppdCharacter, eClass );

	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, nValueOriginal, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_PhysicalRegistance( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;

	AgpmCombat* ppmCombat = ( AgpmCombat* )g_pEngine->GetModule( "AgpmCombat" );
	if( !ppmCombat ) return FALSE;

	AgpmFactors* ppmFactor = ( AgpmFactors* )g_pEngine->GetModule( "AgpmFactors" );
	if( !ppmFactor ) return FALSE;

	int nLevel = ppmFactor->GetLevel( &ppdCharacter->m_csFactor );

	int nValueOriginal = 0;
	ppmFactor->GetValue( &ppdCharacter->m_pcsCharacterTemplate->m_csLevelFactor[ nLevel ], &nValueOriginal, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL );

	int nValueCurrent = ( int )ppmCombat->GetPhysicalResistance( ppdCharacter );

	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, nValueOriginal, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_BlockRate( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;

	AgpmCombat* ppmCombat = ( AgpmCombat* )g_pEngine->GetModule( "AgpmCombat" );
	if( !ppmCombat ) return FALSE;

	AgpmFactors* ppmFactor = ( AgpmFactors* )g_pEngine->GetModule( "AgpmFactors" );
	if( !ppmFactor ) return FALSE;

	int nLevel = ppmFactor->GetLevel( &ppdCharacter->m_csFactor );

	int nValueOriginal = 0;
	ppmFactor->GetValue( &ppdCharacter->m_pcsCharacterTemplate->m_csLevelFactor[ nLevel ], &nValueOriginal, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL_BLOCK );

	int nValueCurrent = ppmCombat->GetPhysicalBlockRate( ppdCharacter );

	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, nValueOriginal, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_SkillBlockRate( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;

	AgpmCombat* ppmCombat = ( AgpmCombat* )g_pEngine->GetModule( "AgpmCombat" );
	if( !ppmCombat ) return FALSE;

	AgpmFactors* ppmFactor = ( AgpmFactors* )g_pEngine->GetModule( "AgpmFactors" );
	if( !ppmFactor ) return FALSE;

	int nLevel = ppmFactor->GetLevel( &ppdCharacter->m_csFactor );

	int nValueOriginal = 0;
	ppmFactor->GetValue( &ppdCharacter->m_pcsCharacterTemplate->m_csLevelFactor[ nLevel ], &nValueOriginal, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_SKILL_BLOCK );

	int nValueCurrent = ppmCombat->GetSkillBlockRate( ppdCharacter );

	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, nValueOriginal, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_EvadeRate( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;

	AgpmCombat* ppmCombat = ( AgpmCombat* )g_pEngine->GetModule( "AgpmCombat" );
	if( !ppmCombat ) return FALSE;

	AgpmFactors* ppmFactor = ( AgpmFactors* )g_pEngine->GetModule( "AgpmFactors" );
	if( !ppmFactor ) return FALSE;

	int nLevel = ppmFactor->GetLevel( &ppdCharacter->m_csFactor );

	int nValueOriginal = 0;
	ppmFactor->GetValue( &ppdCharacter->m_pcsCharacterTemplate->m_csLevelFactor[ nLevel ], &nValueOriginal, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_EVADE_RATE );

	int nValueCurrent = ppmCombat->GetAdditionalEvadeRate( ppdCharacter );

	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, nValueOriginal, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_DodgeRate( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;

	AgpmCombat* ppmCombat = ( AgpmCombat* )g_pEngine->GetModule( "AgpmCombat" );
	if( !ppmCombat ) return FALSE;

	AgpmFactors* ppmFactor = ( AgpmFactors* )g_pEngine->GetModule( "AgpmFactors" );
	if( !ppmFactor ) return FALSE;

	int nLevel = ppmFactor->GetLevel( &ppdCharacter->m_csFactor );

	int nValueOriginal = 0;
	ppmFactor->GetValue( &ppdCharacter->m_pcsCharacterTemplate->m_csLevelFactor[ nLevel ], &nValueOriginal, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_DODGE_RATE );

	int nValueCurrent = ppmCombat->GetAdditionalDodgeRate( ppdCharacter );

	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, nValueOriginal, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_CriticalRegistance( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;

	AgpmSkill* ppmSkill = ( AgpmSkill* )g_pEngine->GetModule( "AgpmSkill" );
	AgpmEventSkillMaster* ppmEventSkillMaster = ( AgpmEventSkillMaster* )g_pEngine->GetModule( "AgpmEventSkillMaster" );
	AgcmUICharacter* pcmUICharacter = ( AgcmUICharacter* )g_pEngine->GetModule( "AgcmUICharacter" );
	if( !ppmSkill || !ppmEventSkillMaster || !pcmUICharacter ) return 0;

	// ĳ������ ��ų���� ��ġ������ �����ͼ�..
	AgpdSkillAttachData* ppdSkillAttachData = ppmSkill->GetAttachSkillData( ( ApBase* )ppdCharacter );
	if( !ppdSkillAttachData ) return 0;

	// �⺻���� 0�̰�..
	int nValueOriginal = 0;

	// ��ų�� ���� ��������..
	int nValueCurrent = ppdSkillAttachData->m_stBuffedSkillCombatEffectArg.lDefenceCriticalAttack;

	// ������� �����ۿ� ���� ������
	nValueCurrent += pcmUICharacter->CalcCurrentItemProperty( ppdCharacter, eZeroBaseProperty_CriticalRegistance );

	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, nValueOriginal, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_StunRegistance( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;

	AgcmUICharacter* pcmUICharacter = ( AgcmUICharacter* )g_pEngine->GetModule( "AgcmUICharacter" );
	if( !pcmUICharacter ) return FALSE;

	int nValueOriginal = 0;
	int nValueCurrent = pcmUICharacter->CalcCurrentZeroBaseProperty( ppdCharacter, eZeroBaseProperty_StunRegistance );

	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, nValueOriginal, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_ReflectDamageRate( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;

	AgpmSkill* ppmSkill = ( AgpmSkill* )g_pEngine->GetModule( "AgpmSkill" );
	AgpmEventSkillMaster* ppmEventSkillMaster = ( AgpmEventSkillMaster* )g_pEngine->GetModule( "AgpmEventSkillMaster" );
	AgcmUICharacter* pcmUICharacter = ( AgcmUICharacter* )g_pEngine->GetModule( "AgcmUICharacter" );
	if( !ppmSkill || !ppmEventSkillMaster || !pcmUICharacter ) return 0;

	// ĳ������ ��ų���� ��ġ������ �����ͼ�..
	AgpdSkillAttachData* ppdSkillAttachData = ppmSkill->GetAttachSkillData( ( ApBase* )ppdCharacter );
	if( !ppdSkillAttachData ) return 0;

	// �⺻���� 0�̰�..
	int nValueOriginal = 0;

	// ��ų�� ���� ��������..
	int nValueCurrent = ppdSkillAttachData->m_stBuffedSkillCombatEffectArg.lDamage2HPProbability[ 0 ];

	// ������� �����ۿ� ���� ������
	nValueCurrent += pcmUICharacter->CalcCurrentItemProperty( ppdCharacter, eZeroBaseProperty_ReflectDamageRate );

	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, nValueOriginal, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_RefectDamage( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;

	AgpmSkill* ppmSkill = ( AgpmSkill* )g_pEngine->GetModule( "AgpmSkill" );
	AgpmEventSkillMaster* ppmEventSkillMaster = ( AgpmEventSkillMaster* )g_pEngine->GetModule( "AgpmEventSkillMaster" );
	AgcmUICharacter* pcmUICharacter = ( AgcmUICharacter* )g_pEngine->GetModule( "AgcmUICharacter" );
	if( !ppmSkill || !ppmEventSkillMaster || !pcmUICharacter ) return 0;

	// ĳ������ ��ų���� ��ġ������ �����ͼ�..
	AgpdSkillAttachData* ppdSkillAttachData = ppmSkill->GetAttachSkillData( ( ApBase* )ppdCharacter );
	if( !ppdSkillAttachData ) return 0;

	// �⺻���� 0�̰�..
	int nValueOriginal = 0;

	// ��ų�� ���� ��������..
	int nValueCurrent = ppdSkillAttachData->m_stBuffedSkillCombatEffectArg.lDamage2HPAmount[ 0 ];

	// ������� �����ۿ� ���� ������
	nValueCurrent += pcmUICharacter->CalcCurrentItemProperty( ppdCharacter, eZeroBaseProperty_ReflectDamage );

	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, nValueOriginal, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_HeroicMeleeRegistance( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;

	AgpmCombat* ppmCombat = ( AgpmCombat* )g_pEngine->GetModule( "AgpmCombat" );
	if( !ppmCombat ) return FALSE;

	int nValueOriginal = 0;
	int nValueCurrent = ( int )ppmCombat->GetHeroicMeleeResistance( ppdCharacter );
	
	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, nValueOriginal, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_HeroicMissileRegistance( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;

	AgpmCombat* ppmCombat = ( AgpmCombat* )g_pEngine->GetModule( "AgpmCombat" );
	if( !ppmCombat ) return FALSE;

	int nValueOriginal = 0;
	int nValueCurrent = ( int )ppmCombat->GetHeroicRangedResistance( ppdCharacter );
	
	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, nValueOriginal, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_HeroicMagicRegistance( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;

	AgpmCombat* ppmCombat = ( AgpmCombat* )g_pEngine->GetModule( "AgpmCombat" );
	if( !ppmCombat ) return FALSE;

	int nValueOriginal = 0;
	int nValueCurrent = ( int )ppmCombat->GetHeroicMagicResistance( ppdCharacter );
	
	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, nValueOriginal, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_MagicDamage( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;

	AgpmCombat* ppmCombat = ( AgpmCombat* )g_pEngine->GetModule( "AgpmCombat" );
	if( !ppmCombat ) return FALSE;

	int nValueOriginal = 0;
	int nValueCurrent = ( int )ppmCombat->CalcFirstSpiritAttack( ppdCharacter, NULL, AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC, TRUE, FALSE );
	
	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, nValueOriginal, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_MagicRegistance( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;

	AgpmCombat* ppmCombat = ( AgpmCombat* )g_pEngine->GetModule( "AgpmCombat" );
	if( !ppmCombat ) return FALSE;

	int nValueOriginal = 0;
	int nValueCurrent = ( int )ppmCombat->CalcFirstSpiritDefense( ppdCharacter, AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC );
	
	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, nValueOriginal, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_FireDamage( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;

	AgpmCombat* ppmCombat = ( AgpmCombat* )g_pEngine->GetModule( "AgpmCombat" );
	if( !ppmCombat ) return FALSE;

	int nValueOriginal = 0;
	int nValueCurrent = ( int )ppmCombat->CalcFirstSpiritAttack( ppdCharacter, NULL, AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE, TRUE, FALSE );
	
	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, nValueOriginal, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_FireRegistance( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;

	AgpmCombat* ppmCombat = ( AgpmCombat* )g_pEngine->GetModule( "AgpmCombat" );
	if( !ppmCombat ) return FALSE;

	int nValueOriginal = 0;
	int nValueCurrent = ( int )ppmCombat->CalcFirstSpiritDefense( ppdCharacter, AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE );
	
	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, nValueOriginal, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_AirDamage( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;

	AgpmCombat* ppmCombat = ( AgpmCombat* )g_pEngine->GetModule( "AgpmCombat" );
	if( !ppmCombat ) return FALSE;

	int nValueOriginal = 0;
	int nValueCurrent = ( int )ppmCombat->CalcFirstSpiritAttack( ppdCharacter, NULL, AGPD_FACTORS_ATTRIBUTE_TYPE_AIR, TRUE, FALSE );
	
	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, nValueOriginal, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_AirRegistance( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;

	AgpmCombat* ppmCombat = ( AgpmCombat* )g_pEngine->GetModule( "AgpmCombat" );
	if( !ppmCombat ) return FALSE;

	int nValueOriginal = 0;
	int nValueCurrent = ( int )ppmCombat->CalcFirstSpiritDefense( ppdCharacter, AGPD_FACTORS_ATTRIBUTE_TYPE_AIR );
	
	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, nValueOriginal, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_WaterDamage( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;

	AgpmCombat* ppmCombat = ( AgpmCombat* )g_pEngine->GetModule( "AgpmCombat" );
	if( !ppmCombat ) return FALSE;

	int nValueOriginal = 0;
	int nValueCurrent = ( int )ppmCombat->CalcFirstSpiritAttack( ppdCharacter, NULL, AGPD_FACTORS_ATTRIBUTE_TYPE_WATER, TRUE, FALSE );
	
	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, nValueOriginal, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_WaterRegistance( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;

	AgpmCombat* ppmCombat = ( AgpmCombat* )g_pEngine->GetModule( "AgpmCombat" );
	if( !ppmCombat ) return FALSE;

	int nValueOriginal = 0;
	int nValueCurrent = ( int )ppmCombat->CalcFirstSpiritDefense( ppdCharacter, AGPD_FACTORS_ATTRIBUTE_TYPE_WATER );
	
	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, nValueOriginal, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_EarthDamage( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;

	AgpmCombat* ppmCombat = ( AgpmCombat* )g_pEngine->GetModule( "AgpmCombat" );
	if( !ppmCombat ) return FALSE;

	int nValueOriginal = 0;
	int nValueCurrent = ( int )ppmCombat->CalcFirstSpiritAttack( ppdCharacter, NULL, AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH, TRUE, FALSE );
	
	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, nValueOriginal, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_EarthRegistance( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;

	AgpmCombat* ppmCombat = ( AgpmCombat* )g_pEngine->GetModule( "AgpmCombat" );
	if( !ppmCombat ) return FALSE;

	int nValueOriginal = 0;
	int nValueCurrent = ( int )ppmCombat->CalcFirstSpiritDefense( ppdCharacter, AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH );
	
	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, nValueOriginal, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_IceDamage( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;

	AgpmCombat* ppmCombat = ( AgpmCombat* )g_pEngine->GetModule( "AgpmCombat" );
	if( !ppmCombat ) return FALSE;

	int nValueOriginal = 0;
	int nValueCurrent = ( int )ppmCombat->CalcFirstSpiritAttack( ppdCharacter, NULL, AGPD_FACTORS_ATTRIBUTE_TYPE_ICE, TRUE, FALSE );
	
	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, nValueOriginal, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_IceRegistance( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;

	AgpmCombat* ppmCombat = ( AgpmCombat* )g_pEngine->GetModule( "AgpmCombat" );
	if( !ppmCombat ) return FALSE;

	int nValueOriginal = 0;
	int nValueCurrent = ( int )ppmCombat->CalcFirstSpiritDefense( ppdCharacter, AGPD_FACTORS_ATTRIBUTE_TYPE_ICE );
	
	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, nValueOriginal, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_ThunderDamage( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;

	AgpmCombat* ppmCombat = ( AgpmCombat* )g_pEngine->GetModule( "AgpmCombat" );
	if( !ppmCombat ) return FALSE;

	int nValueOriginal = 0;
	int nValueCurrent = ( int )ppmCombat->CalcFirstSpiritAttack( ppdCharacter, NULL, AGPD_FACTORS_ATTRIBUTE_TYPE_LIGHTENING, TRUE, FALSE );
	
	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, nValueOriginal, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_ThunderRegistance( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;

	AgpmCombat* ppmCombat = ( AgpmCombat* )g_pEngine->GetModule( "AgpmCombat" );
	if( !ppmCombat ) return FALSE;

	int nValueOriginal = 0;
	int nValueCurrent = ( int )ppmCombat->CalcFirstSpiritDefense( ppdCharacter, AGPD_FACTORS_ATTRIBUTE_TYPE_LIGHTENING );
	
	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, nValueOriginal, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_PoisonDamage( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;

	AgpmCombat* ppmCombat = ( AgpmCombat* )g_pEngine->GetModule( "AgpmCombat" );
	if( !ppmCombat ) return FALSE;

	int nValueOriginal = 0;
	int nValueCurrent = ( int )ppmCombat->CalcFirstSpiritAttack( ppdCharacter, NULL, AGPD_FACTORS_ATTRIBUTE_TYPE_POISON, TRUE, FALSE );
	
	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, nValueOriginal, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_PoisonRegistance( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;

	AgpmCombat* ppmCombat = ( AgpmCombat* )g_pEngine->GetModule( "AgpmCombat" );
	if( !ppmCombat ) return FALSE;

	int nValueOriginal = 0;
	int nValueCurrent = ( int )ppmCombat->CalcFirstSpiritDefense( ppdCharacter, AGPD_FACTORS_ATTRIBUTE_TYPE_POISON );
	
	char strText[ 128 ] = { 0, };
	MakeFormattedString( strText, pEntry->m_strValueFormat, nValueOriginal, nValueCurrent, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper, pEntry->m_dwValueColorLower );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_RemainTimeAttacker( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )g_pEngine->GetModule( "AgpmCharacter" );
	if( !ppmCharacter ) return FALSE;

	int nRemainTime = ppmCharacter->GetRemainedCriminalTime( ppdCharacter );

	// ������ ���� ƽ���̴� �д��� ������ ����
	nRemainTime = ( int )( ( ( double )nRemainTime / ( double )60 ) + 0.9999 );

	// �ð����̴� 0 ������ �������� �ʵ��� ����
	if( nRemainTime < 0 )
	{
		nRemainTime = 0;
	}

	char strText[ 128 ] = { 0, };
	MakeFormattedStringUINT( strText, pEntry->m_strValueFormat, nRemainTime, pEntry->m_dwValueColor, pEntry->m_dwValueColorUpper );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_MurdererPoint( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )g_pEngine->GetModule( "AgpmCharacter" );
	if( !ppmCharacter ) return FALSE;

	AgcmResourceLoader* pcmResourceLoader = ( AgcmResourceLoader* )g_pEngine->GetModule( "AgcmResourceLoader" );
	if( !pcmResourceLoader ) return FALSE;

	int nMurdererPoint = ppmCharacter->GetMurdererPoint( ppdCharacter );
	int nMurdererLevel = ppmCharacter->GetMurdererLevel( ppdCharacter );

	// ������ �������� ���� ��µ� ������ �����Ѵ�. ( XML�� ����ִ� ������ �⺻���� ����Ѵ�.
	DWORD dwColor = pcmResourceLoader->GetColor( "White" );;
	if( nMurdererLevel >= AGPMCHAR_MURDERER_LEVEL3_POINT )
	{
		dwColor = pcmResourceLoader->GetColor( "Red" );
	}
	else if( nMurdererLevel >= AGPMCHAR_MURDERER_LEVEL2_POINT )
	{
		dwColor = pcmResourceLoader->GetColor( "Yellow" );
	}

	char strText[ 128 ] = { 0, };
	MakeFormattedStringUINT( strText, pEntry->m_strValueFormat, nMurdererPoint, pEntry->m_dwValueColor, dwColor );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_RemainTimeRemission( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )g_pEngine->GetModule( "AgpmCharacter" );
	if( !ppmCharacter ) return FALSE;

	AgcmResourceLoader* pcmResourceLoader = ( AgcmResourceLoader* )g_pEngine->GetModule( "AgcmResourceLoader" );
	if( !pcmResourceLoader ) return FALSE;

	int nRemainTime = ppmCharacter->GetRemainedMurdererTime( ppdCharacter );
	int nMurdererLevel = ppmCharacter->GetMurdererLevel( ppdCharacter );

	// ������ ���� ƽ���̴� �д��� ������ ����
	nRemainTime = ( int )( ( ( double )nRemainTime / ( double )60 ) + 0.9999 );

	// ������ �������� ���� ��µ� ���󰪰� ���� ��µ� �������� �����Ѵ�. ( XML�� ����ִ� ������ �⺻���� ����Ѵ�.
	DWORD dwColor = pcmResourceLoader->GetColor( "White" );;
	if( nMurdererLevel >= AGPMCHAR_MURDERER_LEVEL3_POINT )
	{
		dwColor = pcmResourceLoader->GetColor( "Red" );
	}
	else if( nMurdererLevel >= AGPMCHAR_MURDERER_LEVEL2_POINT )
	{
		dwColor = pcmResourceLoader->GetColor( "Yellow" );
	}

	char strText[ 128 ] = { 0, };
	MakeFormattedStringUINT( strText, pEntry->m_strValueFormat, nRemainTime, pEntry->m_dwValueColor, dwColor );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_MurdererLevel( void* pData, void* pCharacter )
{
	if( !pData || !pCharacter ) return FALSE;

	stCharacterPropertyEntry* pEntry = ( stCharacterPropertyEntry* )pData;
	AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditValue;
	if( !pEdit ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )g_pEngine->GetModule( "AgpmCharacter" );
	if( !ppmCharacter ) return FALSE;

	AgcmResourceLoader* pcmResourceLoader = ( AgcmResourceLoader* )g_pEngine->GetModule( "AgcmResourceLoader" );
	if( !pcmResourceLoader ) return FALSE;

	int nMurdererLevel = ppmCharacter->GetMurdererLevel( ppdCharacter );
	int nLevel = 0;

	// ������ �������� ���� ��µ� ���󰪰� ���� ��µ� �������� �����Ѵ�. ( XML�� ����ִ� ������ �⺻���� ����Ѵ�.
	DWORD dwColor = pcmResourceLoader->GetColor( "White" );;
	if( nMurdererLevel >= AGPMCHAR_MURDERER_LEVEL3_POINT )
	{
		dwColor = pcmResourceLoader->GetColor( "Red" );
		nLevel = 3;
	}
	else if( nMurdererLevel >= AGPMCHAR_MURDERER_LEVEL2_POINT )
	{
		dwColor = pcmResourceLoader->GetColor( "Yellow" );
		nLevel = 2;
	}
	else if( nMurdererLevel >= AGPMCHAR_MURDERER_LEVEL1_POINT )
	{
		nLevel = 1;
	}

	char strText[ 128 ] = { 0, };
	MakeFormattedStringUINT( strText, pEntry->m_strValueFormat, nLevel, pEntry->m_dwValueColor, dwColor );
	pEdit->SetText( strText );
	return TRUE;
}

BOOL fn_CharacterProperty_PVPTotalWin(void * pData, void * pCharacter)
{
	if (!pData || !pCharacter)
		return FALSE;
	stCharacterPropertyEntry * pEntry = (stCharacterPropertyEntry *)pData;
	AcUIEdit * pEdit = (AcUIEdit *)pEntry->m_pEditValue;
	if (!pEdit)
		return FALSE;
	AgpdCharacter * pdCharacter = (AgpdCharacter *)pCharacter;
	AgpmCharacter * pmCharacter = (AgpmCharacter *)g_pEngine->GetModule( "AgpmCharacter" );
	if (!pdCharacter || !pmCharacter)
		return FALSE;
	AgpmPvP * pmPvP = (AgpmPvP *)g_pEngine->GetModule("AgpmPvP");
	if (!pmPvP)
		return FALSE;
	AgpdPvPADChar * pdPvP = pmPvP->GetADCharacter(pdCharacter);
	char strText[ 128 ] = { 0 };
	MakeFormattedStringUINT(strText, pEntry->m_strValueFormat, pdPvP->m_lWin, pEntry->m_dwValueColor, pEntry->m_dwValueColor);
	pEdit->SetText(strText);
	return TRUE;
}

BOOL fn_CharacterProperty_PVPTotalLose(void * pData, void * pCharacter)
{
	if (!pData || !pCharacter)
		return FALSE;
	stCharacterPropertyEntry * pEntry = (stCharacterPropertyEntry *)pData;
	AcUIEdit * pEdit = (AcUIEdit *)pEntry->m_pEditValue;
	if (!pEdit)
		return FALSE;
	AgpdCharacter * pdCharacter = (AgpdCharacter *)pCharacter;
	AgpmCharacter * pmCharacter = (AgpmCharacter *)g_pEngine->GetModule( "AgpmCharacter" );
	if (!pdCharacter || !pmCharacter)
		return FALSE;
	AgpmPvP * pmPvP = (AgpmPvP *)g_pEngine->GetModule("AgpmPvP");
	if (!pmPvP)
		return FALSE;
	AgpdPvPADChar * pdPvP = pmPvP->GetADCharacter(pdCharacter);
	char strText[ 128 ] = { 0 };
	MakeFormattedStringUINT(strText, pEntry->m_strValueFormat, pdPvP->m_lLose, pEntry->m_dwValueColor, pEntry->m_dwValueColor);
	pEdit->SetText(strText);
	return TRUE;
}
