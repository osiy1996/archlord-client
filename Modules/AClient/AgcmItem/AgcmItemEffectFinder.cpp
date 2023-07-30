#include "AgcmItemEffectFinder.h"
#include <AgcmEventEffect/AgcmEventEffectData.h>
#include <AuExcelTxtLib/AuExcelTxtLib.h>
#include <AuExcelTxtLib/AuExcelBinaryLib.h>



//void stEECommonCharEntryRace::AddEffectTypeID( int nClassType, int nEffectTypeID )
//{
//	stEECommonCharEntryClass* pEntry = m_mapEntry.Get( nClassType );
//	if( pEntry )
//	{
//		pEntry->m_nEffectTypeID = nEffectTypeID;
//	}
//	else
//	{
//		stEECommonCharEntryClass NewEntry;
//
//		NewEntry.m_nClassType = nClassType;
//		NewEntry.m_nEffectTypeID = nEffectTypeID;
//
//		m_mapEntry.Add( nClassType, NewEntry );
//	}
//}
//
//int stEECommonCharEntryRace::GetEffectTypeID( int nClassType, int nEffectTypeID )
//{
//	stEECommonCharEntryClass* pEntry = m_mapEntry.Get( nClassType );
//	if( !pEntry ) return -1;
//
//	return pEntry->m_nEffectTypeID;
//}
//
//bool stEECommonCharEntryRace::IsMyEffectTypeID( int nEffectTypeID )
//{
//	int nEntryCount = m_mapEntry.GetSize();
//	for( int nCount = 0 ; nCount < nEntryCount ; ++nCount )
//	{
//		stEECommonCharEntryClass* pClass = m_mapEntry.Get( nCount );
//		if( !pClass ) continue;
//
//		if( pClass->m_nEffectTypeID == nEffectTypeID )
//		{
//			return true;
//		}
//	}
//
//	return false;
//}
//
//
//void stEECommonCharEntry::AddEffectTypeID( int nRaceType, int nClassType, int nEffectTypeID )
//{
//	stEECommonCharEntryRace* pEntry = m_mapEntry.Get( nRaceType );
//	if( pEntry )
//	{
//		pEntry->AddEffectTypeID( nClassType, nEffectTypeID );
//	}
//	else
//	{
//		stEECommonCharEntryRace NewEntry;
//
//		NewEntry.m_nRaceType = nRaceType;
//		NewEntry.AddEffectTypeID( nClassType, nEffectTypeID );
//
//		m_mapEntry.Add( nRaceType, NewEntry );
//	}
//}
//
//int stEECommonCharEntry::GetEffectTypeID( int nRaceType, int nClassType, int nEffectTypeID )
//{
//	stEECommonCharEntryRace* pEntry = m_mapEntry.Get( nRaceType );
//	if( !pEntry ) return -1;
//
//	return pEntry->GetEffectTypeID( nClassType, nEffectTypeID );
//}
//
//bool stEECommonCharEntry::IsMyEffectTypeID( int nEffectTypeID )
//{
//	int nEntryCount = m_mapEntry.GetSize();
//	for( int nCount = 0 ; nCount < nEntryCount ; ++nCount )
//	{
//		stEECommonCharEntryRace* pRace = m_mapEntry.Get( nCount );
//		if( !pRace ) continue;
//
//		if( pRace->IsMyEffectTypeID( nEffectTypeID ) )
//		{
//			return true;
//		}
//	}
//
//	return false;
//}

AgcmItemEffectFinder::AgcmItemEffectFinder()
	: m_ulTargetCount(0)
	, m_pstTargets(NULL)
{
}

AgcmItemEffectFinder::~AgcmItemEffectFinder()
{
	if (m_pstTargets) {
		free(m_pstTargets);
		m_pstTargets = NULL;
		m_ulTargetCount = 0;
	}
}

int
AgcmItemEffectFinder::GetCharEffectID(
	int nItemExtraType, 
	int nRaceType, 
	int nClassType)
{
	for (UINT32 i = 0; i < m_ulTargetCount; i++) {
		if (m_pstTargets[i].m_nBaseID == nItemExtraType &&
			m_pstTargets[i].m_nRace == nRaceType &&
			m_pstTargets[i].m_nClass == nClassType)
			return (m_pstTargets[i].m_nID - 1);
	}
	return -1;
}

BOOL
AgcmItemEffectFinder::ReadFile()
{
	AuExcelLib * pExcel = AuExcel::LoadExcelFile(
		"INI\\ee_item_rune.txt", FALSE);
	int lRowCount;
	int lColCount;

	if (!pExcel) {
		OutputDebugStringA(
			"Failed to open file: INI\\ee_item_rune_txt\n");
		return FALSE;
	}
	lRowCount = pExcel->GetRow();
	lColCount = pExcel->GetColumn();
	if (lColCount != 4 || lRowCount <= 1) {
		delete pExcel;
		return FALSE;
	}
	m_ulTargetCount = (UINT32)lRowCount - 1;
	m_pstTargets = (AgcdItemEffectTarget *)malloc(
		m_ulTargetCount * sizeof(*m_pstTargets));
	memset(m_pstTargets, 0, m_ulTargetCount * sizeof(*m_pstTargets));
	for (int i = 1; i < lRowCount; i++) {
		AgcdItemEffectTarget * pstTarget = &m_pstTargets[i - 1];
		const char * pszData[4];

		for (int j = 0; j < lColCount; j++)
			pszData[j] = pExcel->GetData(j, i);
		pstTarget->m_nID = atoi(pszData[0]);
		pstTarget->m_nBaseID = atoi(pszData[1]);
		pstTarget->m_nRace = atoi(pszData[2]);
		pstTarget->m_nClass = atoi(pszData[3]);
	}
	delete pExcel;
	return TRUE;
}

//void AgcmItemEffectFinder::AddCloakEffectID( AuEECommonEffectTypeCloak eType, int nRaceType, int nClassType, int nEffectTypeID )
//{
//	stEECommonCharEntry* pEntry = m_mapEECommonCharCloak.Get( ( int )eType );
//	if( pEntry )
//	{
//		pEntry->AddEffectTypeID( nRaceType, nClassType, nEffectTypeID );
//	}
//	else
//	{
//		stEECommonCharEntry NewEntry;
//
//		NewEntry.m_nEffectBaseType = ( int )eType;
//		NewEntry.AddEffectTypeID( nRaceType, nClassType, nEffectTypeID );
//
//		m_mapEECommonCharCloak.Add( ( int )eType, NewEntry );
//	}
//}
//
//void AgcmItemEffectFinder::AddWingEffectID( AuEECommonEffectTypeWing eType, int nRaceType, int nClassType, int nEffectTypeID )
//{
//	stEECommonCharEntry* pEntry = m_mapEECommonCharWing.Get( ( int )eType );
//	if( pEntry )
//	{
//		pEntry->AddEffectTypeID( nRaceType, nClassType, nEffectTypeID );
//	}
//	else
//	{
//		stEECommonCharEntry NewEntry;
//
//		NewEntry.m_nEffectBaseType = ( int )eType;
//		NewEntry.AddEffectTypeID( nRaceType, nClassType, nEffectTypeID );
//
//		m_mapEECommonCharWing.Add( ( int )eType, NewEntry );
//	}
//}
//
//int AgcmItemEffectFinder::FindEffectIDCloak( AuEECommonEffectTypeCloak eType, int nRaceType, int nClassType, int nEffectTypeID )
//{
//	stEECommonCharEntry* pEntry = m_mapEECommonCharCloak.Get( ( int )eType );
//	if( !pEntry ) return -1;
//
//	return pEntry->GetEffectTypeID( nRaceType, nClassType, nEffectTypeID );
//}
//
//int AgcmItemEffectFinder::FindEffectIDWing( AuEECommonEffectTypeWing eType, int nRaceType, int nClassType, int nEffectTypeID )
//{
//	stEECommonCharEntry* pEntry = m_mapEECommonCharWing.Get( ( int )eType );
//	if( !pEntry ) return -1;
//
//	return pEntry->GetEffectTypeID( nRaceType, nClassType, nEffectTypeID );
//}
//
//int AgcmItemEffectFinder::GetEffectIDCloak( int nEffectTypeID, int nRaceType, int nClassType )
//{
//	AuEECommonEffectTypeCloak eCloakType = ( AuEECommonEffectTypeCloak )FindEffectTypeCloak( nEffectTypeID );
//	int nEffectID = FindEffectIDCloak( eCloakType, nRaceType, nClassType, nEffectTypeID );
//	return nEffectID;
//}
//
//int AgcmItemEffectFinder::GetEffectIDWing( int nEffectTypeID, int nRaceType, int nClassType )
//{
//	AuEECommonEffectTypeWing eWingType = ( AuEECommonEffectTypeWing )FindEffectTypeWing( nEffectTypeID );
//	int nEffectID = FindEffectIDWing( eWingType, nRaceType, nClassType, nEffectTypeID );
//	return nEffectID;
//}
//
//int AgcmItemEffectFinder::FindEffectTypeCloak( int nEffectTypeID )
//{
//	for( int nCount = 0 ; nCount < ( int )EndOfEnumCloak ; nCount++ )
//	{
//		stEECommonCharEntry* pEntry = m_mapEECommonCharCloak.Get( nCount );
//		if( !pEntry ) continue;
//
//		if( pEntry->IsMyEffectTypeID( nEffectTypeID ) )
//		{
//			return pEntry->m_nEffectBaseType;
//		}
//	}
//
//	return -1;
//}
//
//int AgcmItemEffectFinder::FindEffectTypeWing( int nEffectTypeID )
//{
//	for( int nCount = 0 ; nCount < ( int )EndOfEnumWing ; nCount++ )
//	{
//		stEECommonCharEntry* pEntry = m_mapEECommonCharWing.Get( nCount );
//		if( !pEntry ) continue;
//
//		if( pEntry->IsMyEffectTypeID( nEffectTypeID ) )
//		{
//			return pEntry->m_nEffectBaseType;
//		}
//	}
//
//	return -1;
//}
//
//void AgcmItemEffectFinder::LoadCloakEffectID( void )
//{
//	// Air Type
//	AddCloakEffectID( Cloak_Air, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_AIR_CLOAK_HK );
//	AddCloakEffectID( Cloak_Air, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_AIR_CLOAK_HA );
//	AddCloakEffectID( Cloak_Air, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_AIR_CLOAK_HM );
//
//	AddCloakEffectID( Cloak_Air, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_AIR_CLOAK_OB );
//	AddCloakEffectID( Cloak_Air, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_AIR_CLOAK_OH );
//	AddCloakEffectID( Cloak_Air, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_AIR_CLOAK_OS );
//
//	AddCloakEffectID( Cloak_Air, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_AIR_CLOAK_MS );
//	AddCloakEffectID( Cloak_Air, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_AIR_CLOAK_MR );
//	AddCloakEffectID( Cloak_Air, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_AIR_CLOAK_ME );
//
//	AddCloakEffectID( Cloak_Air, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_AIR_CLOAK_SLA );
//	AddCloakEffectID( Cloak_Air, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_SCION,		E_COMMON_CHAR_TYPE_AIR_CLOAK_SCI );
//	AddCloakEffectID( Cloak_Air, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_AIR_CLOAK_OBI );
//
//	// Fire Type
//	AddCloakEffectID( Cloak_Fire, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_FIRE_CLOAK_HK );
//	AddCloakEffectID( Cloak_Fire, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_FIRE_CLOAK_HA );
//	AddCloakEffectID( Cloak_Fire, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_FIRE_CLOAK_HM );
//
//	AddCloakEffectID( Cloak_Fire, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_FIRE_CLOAK_OB );
//	AddCloakEffectID( Cloak_Fire, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_FIRE_CLOAK_OH );
//	AddCloakEffectID( Cloak_Fire, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_FIRE_CLOAK_OS );
//
//	AddCloakEffectID( Cloak_Fire, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_FIRE_CLOAK_MS );
//	AddCloakEffectID( Cloak_Fire, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_FIRE_CLOAK_MR );
//	AddCloakEffectID( Cloak_Fire, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_FIRE_CLOAK_ME );
//
//	AddCloakEffectID( Cloak_Fire, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_FIRE_CLOAK_SLA );
//	AddCloakEffectID( Cloak_Fire, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_SCION,		E_COMMON_CHAR_TYPE_FIRE_CLOAK_SCI );
//	AddCloakEffectID( Cloak_Fire, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_FIRE_CLOAK_OBI );
//
//	// Magic Type
//	AddCloakEffectID( Cloak_Magic, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_MAGIC_CLOAK_HK );
//	AddCloakEffectID( Cloak_Magic, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_MAGIC_CLOAK_HA );
//	AddCloakEffectID( Cloak_Magic, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_MAGIC_CLOAK_HM );
//
//	AddCloakEffectID( Cloak_Magic, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_MAGIC_CLOAK_OB );
//	AddCloakEffectID( Cloak_Magic, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_MAGIC_CLOAK_OH );
//	AddCloakEffectID( Cloak_Magic, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_MAGIC_CLOAK_OS );
//
//	AddCloakEffectID( Cloak_Magic, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_MAGIC_CLOAK_MS );
//	AddCloakEffectID( Cloak_Magic, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_MAGIC_CLOAK_MR );
//	AddCloakEffectID( Cloak_Magic, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_MAGIC_CLOAK_ME );
//
//	AddCloakEffectID( Cloak_Magic, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_MAGIC_CLOAK_SLA );
//	AddCloakEffectID( Cloak_Magic, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_SCION,		E_COMMON_CHAR_TYPE_MAGIC_CLOAK_SCI );
//	AddCloakEffectID( Cloak_Magic, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_MAGIC_CLOAK_OBI );
//
//	// Ground Type
//	AddCloakEffectID( Cloak_Ground, AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_GROUND_CLOAK_HK );
//	AddCloakEffectID( Cloak_Ground, AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_GROUND_CLOAK_HA );
//	AddCloakEffectID( Cloak_Ground, AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_GROUND_CLOAK_HM );
//
//	AddCloakEffectID( Cloak_Ground, AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_GROUND_CLOAK_OB );
//	AddCloakEffectID( Cloak_Ground, AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_GROUND_CLOAK_OH );
//	AddCloakEffectID( Cloak_Ground, AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_GROUND_CLOAK_OS );
//
//	AddCloakEffectID( Cloak_Ground, AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_GROUND_CLOAK_MS );
//	AddCloakEffectID( Cloak_Ground, AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_GROUND_CLOAK_MR );
//	AddCloakEffectID( Cloak_Ground, AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_GROUND_CLOAK_ME );
//
//	AddCloakEffectID( Cloak_Ground, AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_GROUND_CLOAK_SLA );
//	AddCloakEffectID( Cloak_Ground, AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_SCION,		E_COMMON_CHAR_TYPE_GROUND_CLOAK_SCI );
//	AddCloakEffectID( Cloak_Ground, AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_GROUND_CLOAK_OBI );
//
//	// Water Type
//	AddCloakEffectID( Cloak_Water, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WATER_CLOAK_HK );
//	AddCloakEffectID( Cloak_Water, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WATER_CLOAK_HA );
//	AddCloakEffectID( Cloak_Water, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WATER_CLOAK_HM );
//
//	AddCloakEffectID( Cloak_Water, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WATER_CLOAK_OB );
//	AddCloakEffectID( Cloak_Water, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WATER_CLOAK_OH );
//	AddCloakEffectID( Cloak_Water, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WATER_CLOAK_OS );
//
//	AddCloakEffectID( Cloak_Water, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WATER_CLOAK_MS );
//	AddCloakEffectID( Cloak_Water, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WATER_CLOAK_MR );
//	AddCloakEffectID( Cloak_Water, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WATER_CLOAK_ME );
//
//	AddCloakEffectID( Cloak_Water, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WATER_CLOAK_SLA );
//	AddCloakEffectID( Cloak_Water, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_SCION,		E_COMMON_CHAR_TYPE_WATER_CLOAK_SCI );
//	AddCloakEffectID( Cloak_Water, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WATER_CLOAK_OBI );
//
//	// Soul Type
//	AddCloakEffectID( Cloak_Soul, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_SOUL_CLOAK_HK );
//	AddCloakEffectID( Cloak_Soul, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_SOUL_CLOAK_HA );
//	AddCloakEffectID( Cloak_Soul, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_SOUL_CLOAK_HM );
//
//	AddCloakEffectID( Cloak_Soul, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_SOUL_CLOAK_OB );
//	AddCloakEffectID( Cloak_Soul, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_SOUL_CLOAK_OH );
//	AddCloakEffectID( Cloak_Soul, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_SOUL_CLOAK_OS );
//
//	AddCloakEffectID( Cloak_Soul, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_SOUL_CLOAK_MS );
//	AddCloakEffectID( Cloak_Soul, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_SOUL_CLOAK_MR );
//	AddCloakEffectID( Cloak_Soul, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_SOUL_CLOAK_ME );
//
//	AddCloakEffectID( Cloak_Soul, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_SOUL_CLOAK_SLA );
//	AddCloakEffectID( Cloak_Soul, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_SCION,		E_COMMON_CHAR_TYPE_SOUL_CLOAK_SCI );
//	AddCloakEffectID( Cloak_Soul, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_SOUL_CLOAK_OBI );
//
//	// Poison Type
//	AddCloakEffectID( Cloak_Poison, AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_POISON_CLOAK_HK );
//	AddCloakEffectID( Cloak_Poison, AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_POISON_CLOAK_HA );
//	AddCloakEffectID( Cloak_Poison, AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_POISON_CLOAK_HM );
//
//	AddCloakEffectID( Cloak_Poison, AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_POISON_CLOAK_OB );
//	AddCloakEffectID( Cloak_Poison, AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_POISON_CLOAK_OH );
//	AddCloakEffectID( Cloak_Poison, AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_POISON_CLOAK_OS );
//
//	AddCloakEffectID( Cloak_Poison, AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_POISON_CLOAK_SLA );
//	AddCloakEffectID( Cloak_Poison, AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_POISON_CLOAK_SCI );
//	AddCloakEffectID( Cloak_Poison, AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_POISON_CLOAK_OBI );
//
//	AddCloakEffectID( Cloak_Poison, AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WATER_CLOAK_SLA );
//	AddCloakEffectID( Cloak_Poison, AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_SCION,		E_COMMON_CHAR_TYPE_WATER_CLOAK_SCI );
//	AddCloakEffectID( Cloak_Poison, AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WATER_CLOAK_OBI );
//
//	// Soul2 Type
//	AddCloakEffectID( Cloak_Soul2, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_SOUL2_CLOAK_HK );
//	AddCloakEffectID( Cloak_Soul2, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_SOUL2_CLOAK_HA );
//	AddCloakEffectID( Cloak_Soul2, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_SOUL2_CLOAK_HM );
//
//	AddCloakEffectID( Cloak_Soul2, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_SOUL2_CLOAK_OB );
//	AddCloakEffectID( Cloak_Soul2, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_SOUL2_CLOAK_OH );
//	AddCloakEffectID( Cloak_Soul2, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_SOUL2_CLOAK_OS );
//
//	AddCloakEffectID( Cloak_Soul2, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_SOUL2_CLOAK_MS );
//	AddCloakEffectID( Cloak_Soul2, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_SOUL2_CLOAK_MR );
//	AddCloakEffectID( Cloak_Soul2, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_SOUL2_CLOAK_ME );
//
//	AddCloakEffectID( Cloak_Soul2, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_SOUL2_CLOAK_SLA );
//	AddCloakEffectID( Cloak_Soul2, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_SCION,		E_COMMON_CHAR_TYPE_SOUL2_CLOAK_SCI );
//	AddCloakEffectID( Cloak_Soul2, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_SOUL2_CLOAK_OBI );
//}
//
//void AgcmItemEffectFinder::LoadWingEffectID( void )
//{
//	// King Type
//	AddWingEffectID( Wing_King, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_KING );
//	AddWingEffectID( Wing_King, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_KING );
//	AddWingEffectID( Wing_King, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WINGS_OF_KING );
//
//	AddWingEffectID( Wing_King, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_KING_O_W );
//	AddWingEffectID( Wing_King, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_KING );
//	AddWingEffectID( Wing_King, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WINGS_OF_KING_O_S );
//
//	AddWingEffectID( Wing_King, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_KING );
//	AddWingEffectID( Wing_King, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_KING );
//	AddWingEffectID( Wing_King, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WINGS_OF_KING );
//
//	AddWingEffectID( Wing_King, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WING_KING_SLA );
//	AddWingEffectID( Wing_King, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_SCION,		E_COMMON_CHAR_TYPE_WING_KING_SCI );
//	AddWingEffectID( Wing_King, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WING_KING_OBI );
//
//	// Angel Type
//	AddWingEffectID( Wing_Angel, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_ANGEL );
//	AddWingEffectID( Wing_Angel, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_ANGEL );
//	AddWingEffectID( Wing_Angel, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WINGS_OF_ANGEL );
//
//	AddWingEffectID( Wing_Angel, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_ANGEL_O_W );
//	AddWingEffectID( Wing_Angel, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_ANGEL );
//	AddWingEffectID( Wing_Angel, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WINGS_OF_ANGEL_O_S );
//
//	AddWingEffectID( Wing_Angel, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_ANGEL );
//	AddWingEffectID( Wing_Angel, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_ANGEL );
//	AddWingEffectID( Wing_Angel, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WINGS_OF_ANGEL );
//
//	AddWingEffectID( Wing_Angel, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WING_ANGEL_SLA );
//	AddWingEffectID( Wing_Angel, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_SCION,		E_COMMON_CHAR_TYPE_WING_ANGEL_SCI );
//	AddWingEffectID( Wing_Angel, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WING_ANGEL_OBI );
//
//	// Bat Type
//	AddWingEffectID( Wing_Bat, 		AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_BAT );
//	AddWingEffectID( Wing_Bat, 		AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_BAT );
//	AddWingEffectID( Wing_Bat, 		AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WINGS_OF_BAT );
//
//	AddWingEffectID( Wing_Bat, 		AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_BAT_O_W );
//	AddWingEffectID( Wing_Bat, 		AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_BAT );
//	AddWingEffectID( Wing_Bat, 		AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WINGS_OF_BAT_O_S );
//
//	AddWingEffectID( Wing_Bat, 		AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_BAT );
//	AddWingEffectID( Wing_Bat, 		AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_BAT );
//	AddWingEffectID( Wing_Bat, 		AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WINGS_OF_BAT );
//
//	AddWingEffectID( Wing_Bat, 		AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WING_BAT_SLA );
//	AddWingEffectID( Wing_Bat, 		AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_SCION,		E_COMMON_CHAR_TYPE_WING_BAT_SCI );
//	AddWingEffectID( Wing_Bat, 		AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WING_BAT_OBI );
//
//	// Devil Type
//	AddWingEffectID( Wing_Devil, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_DEVIL );
//	AddWingEffectID( Wing_Devil, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_DEVIL );
//	AddWingEffectID( Wing_Devil, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WINGS_OF_DEVIL );
//
//	AddWingEffectID( Wing_Devil, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_DEVIL_O_W );
//	AddWingEffectID( Wing_Devil, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_DEVIL );
//	AddWingEffectID( Wing_Devil, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WINGS_OF_DEVIL_O_S );
//
//	AddWingEffectID( Wing_Devil, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_DEVIL );
//	AddWingEffectID( Wing_Devil, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_DEVIL );
//	AddWingEffectID( Wing_Devil, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WINGS_OF_DEVIL );
//
//	AddWingEffectID( Wing_Devil, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WING_DEVIL_SLA );
//	AddWingEffectID( Wing_Devil, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_SCION,		E_COMMON_CHAR_TYPE_WING_DEVIL_SCI );
//	AddWingEffectID( Wing_Devil, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WING_DEVIL_OBI );
//
//	// Wing_15 Type
//	AddWingEffectID( Wing_15, 		AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_15 );
//	AddWingEffectID( Wing_15, 		AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_15 );
//	AddWingEffectID( Wing_15, 		AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WINGS_OF_15 );
//
//	AddWingEffectID( Wing_15, 		AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_15_O_W );
//	AddWingEffectID( Wing_15, 		AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_15 );
//	AddWingEffectID( Wing_15, 		AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WINGS_OF_15_O_S );
//
//	AddWingEffectID( Wing_15, 		AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_15 );
//	AddWingEffectID( Wing_15, 		AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_15 );
//	AddWingEffectID( Wing_15, 		AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WINGS_OF_15 );
//
//	AddWingEffectID( Wing_15, 		AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WING_15_SLA );
//	AddWingEffectID( Wing_15, 		AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_SCION,		E_COMMON_CHAR_TYPE_WING_15_SCI );
//	AddWingEffectID( Wing_15, 		AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WING_15_OBI );
//
//	// Judge Type
//	AddWingEffectID( Wing_Judge, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_JUDGE );
//	AddWingEffectID( Wing_Judge, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_JUDGE );
//	AddWingEffectID( Wing_Judge, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WINGS_OF_JUDGE );
//
//	AddWingEffectID( Wing_Judge, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_JUDGE_O_W );
//	AddWingEffectID( Wing_Judge, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_JUDGE );
//	AddWingEffectID( Wing_Judge, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WINGS_OF_JUDGE_O_S );
//
//	AddWingEffectID( Wing_Judge, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_JUDGE );
//	AddWingEffectID( Wing_Judge, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_JUDGE );
//	AddWingEffectID( Wing_Judge, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WINGS_OF_JUDGE );
//
//	AddWingEffectID( Wing_Judge, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WING_JUDGE_SLA );
//	AddWingEffectID( Wing_Judge, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_SCION,		E_COMMON_CHAR_TYPE_WING_JUDGE_SCI );
//	AddWingEffectID( Wing_Judge, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WING_JUDGE_OBI );
//
//	// Messiah Type
//	AddWingEffectID( Wing_Messiah, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_MESSIAH );
//	AddWingEffectID( Wing_Messiah, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_MESSIAH );
//	AddWingEffectID( Wing_Messiah, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WINGS_OF_MESSIAH );
//
//	AddWingEffectID( Wing_Messiah, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_MESSIAH_O_W );
//	AddWingEffectID( Wing_Messiah, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_MESSIAH );
//	AddWingEffectID( Wing_Messiah, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WINGS_OF_MESSIAH_O_S );
//
//	AddWingEffectID( Wing_Messiah, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_MESSIAH );
//	AddWingEffectID( Wing_Messiah, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_MESSIAH );
//	AddWingEffectID( Wing_Messiah, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WINGS_OF_MESSIAH );
//
//	AddWingEffectID( Wing_Messiah, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WING_MESSIAH_SLA );
//	AddWingEffectID( Wing_Messiah, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_SCION,		E_COMMON_CHAR_TYPE_WING_MESSIAH_SCI );
//	AddWingEffectID( Wing_Messiah, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WING_MESSIAH_OBI );
//
//	// �Ϻ� ������Ʈ�� �׽�Ʈ
//
//	//P15
//	AddWingEffectID( Wing_15_Premium, 		AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_15_P );
//	AddWingEffectID( Wing_15_Premium, 		AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_15_P );
//	AddWingEffectID( Wing_15_Premium, 		AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WINGS_OF_15_P );
//
//	AddWingEffectID( Wing_15_Premium, 		AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_15_O_W_P );
//	AddWingEffectID( Wing_15_Premium, 		AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_15_P );
//	AddWingEffectID( Wing_15_Premium, 		AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WINGS_OF_15_O_S_P );
//
//	AddWingEffectID( Wing_15_Premium, 		AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_15_P );
//	AddWingEffectID( Wing_15_Premium, 		AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_15_P );
//	AddWingEffectID( Wing_15_Premium, 		AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WINGS_OF_15_P );
//
//	AddWingEffectID( Wing_15_Premium, 		AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_15_SLA_P );
//	AddWingEffectID( Wing_15_Premium, 		AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_SCION,		E_COMMON_CHAR_TYPE_WINGS_OF_15_SCI_P );
//	AddWingEffectID( Wing_15_Premium, 		AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_15_OBI_P );
//
//	// Messiah Type
//	AddWingEffectID( Wing_Messiah_Premium, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_MESSIAH_P );
//	AddWingEffectID( Wing_Messiah_Premium, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_MESSIAH_P );
//	AddWingEffectID( Wing_Messiah_Premium, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WINGS_OF_MESSIAH_P );
//
//	AddWingEffectID( Wing_Messiah_Premium, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_MESSIAH_O_W_P );
//	AddWingEffectID( Wing_Messiah_Premium, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_MESSIAH_P );
//	AddWingEffectID( Wing_Messiah_Premium, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WINGS_OF_MESSIAH_O_S_P );
//
//	AddWingEffectID( Wing_Messiah_Premium, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_MESSIAH_P );
//	AddWingEffectID( Wing_Messiah_Premium, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_MESSIAH_P );
//	AddWingEffectID( Wing_Messiah_Premium, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WINGS_OF_MESSIAH_P );
//
//	AddWingEffectID( Wing_Messiah_Premium, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_MESSIAH_SLA_P );
//	AddWingEffectID( Wing_Messiah_Premium, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_SCION,		E_COMMON_CHAR_TYPE_WINGS_OF_MESSIAH_SCI_P );
//	AddWingEffectID( Wing_Messiah_Premium, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_MESSIAH_OBI_P );
//
//	// King Type
//	AddWingEffectID( Wing_King_Premium, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_KING_P );
//	AddWingEffectID( Wing_King_Premium, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_KING_P );
//	AddWingEffectID( Wing_King_Premium, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WINGS_OF_KING_P );
//
//	AddWingEffectID( Wing_King_Premium, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_KING_O_W_P );
//	AddWingEffectID( Wing_King_Premium, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_KING_P );
//	AddWingEffectID( Wing_King_Premium, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WINGS_OF_KING_O_S_P );
//
//	AddWingEffectID( Wing_King_Premium, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_KING_P );
//	AddWingEffectID( Wing_King_Premium, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_KING_P );
//	AddWingEffectID( Wing_King_Premium, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WINGS_OF_KING_P );
//
//	AddWingEffectID( Wing_King_Premium, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_KING_SLA_P );
//	AddWingEffectID( Wing_King_Premium, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_SCION,		E_COMMON_CHAR_TYPE_WINGS_OF_KING_SCI_P );
//	AddWingEffectID( Wing_King_Premium, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_KING_OBI_P );
//
//	// Bat Type
//	AddWingEffectID( Wing_Bat_Premium, 		AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_BAT_P );
//	AddWingEffectID( Wing_Bat_Premium, 		AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_BAT_P );
//	AddWingEffectID( Wing_Bat_Premium, 		AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WINGS_OF_BAT_P );
//
//	AddWingEffectID( Wing_Bat_Premium, 		AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_BAT_O_W_P );
//	AddWingEffectID( Wing_Bat_Premium, 		AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_BAT_P );
//	AddWingEffectID( Wing_Bat_Premium, 		AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WINGS_OF_BAT_O_S_P );
//
//	AddWingEffectID( Wing_Bat_Premium, 		AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_BAT_P );
//	AddWingEffectID( Wing_Bat_Premium, 		AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_BAT_P );
//	AddWingEffectID( Wing_Bat_Premium, 		AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WINGS_OF_BAT_P );
//
//	AddWingEffectID( Wing_Bat_Premium, 		AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_BAT_SLA_P );
//	AddWingEffectID( Wing_Bat_Premium, 		AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_SCION,		E_COMMON_CHAR_TYPE_WINGS_OF_BAT_SCI_P );
//	AddWingEffectID( Wing_Bat_Premium, 		AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_BAT_OBI_P );
//
//	// Judge Type
//	AddWingEffectID( Wing_Judge_Premium, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_JUDGE_P );
//	AddWingEffectID( Wing_Judge_Premium, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_JUDGE_P );
//	AddWingEffectID( Wing_Judge_Premium, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WINGS_OF_JUDGE_P );
//
//	AddWingEffectID( Wing_Judge_Premium, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_JUDGE_O_W_P );
//	AddWingEffectID( Wing_Judge_Premium, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_JUDGE_P );
//	AddWingEffectID( Wing_Judge_Premium, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WINGS_OF_JUDGE_O_S_P );
//
//	AddWingEffectID( Wing_Judge_Premium, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_JUDGE_P );
//	AddWingEffectID( Wing_Judge_Premium, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_JUDGE_P );
//	AddWingEffectID( Wing_Judge_Premium, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WINGS_OF_JUDGE_P );
//
//	AddWingEffectID( Wing_Judge_Premium, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_JUDGE_SLA_P );
//	AddWingEffectID( Wing_Judge_Premium, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_SCION,		E_COMMON_CHAR_TYPE_WINGS_OF_JUDGE_SCI_P );
//	AddWingEffectID( Wing_Judge_Premium, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_JUDGE_OBI_P );
//
//	// Angel Type
//	AddWingEffectID( Wing_Angel_Premium, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_ANGEL_P );
//	AddWingEffectID( Wing_Angel_Premium, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_ANGEL_P );
//	AddWingEffectID( Wing_Angel_Premium, 	AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WINGS_OF_ANGEL_P );
//
//	AddWingEffectID( Wing_Angel_Premium, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_ANGEL_O_W_P );
//	AddWingEffectID( Wing_Angel_Premium, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_ANGEL_P );
//	AddWingEffectID( Wing_Angel_Premium, 	AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WINGS_OF_ANGEL_O_S_P );
//
//	AddWingEffectID( Wing_Angel_Premium, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_ANGEL_P );
//	AddWingEffectID( Wing_Angel_Premium, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_ANGEL_P );
//	AddWingEffectID( Wing_Angel_Premium, 	AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WINGS_OF_ANGEL_P );
//
//	AddWingEffectID( Wing_Angel_Premium, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_ANGEL_SLA_P );
//	AddWingEffectID( Wing_Angel_Premium, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_SCION,		E_COMMON_CHAR_TYPE_WINGS_OF_ANGEL_SCI_P );
//	AddWingEffectID( Wing_Angel_Premium, 	AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_ANGEL_OBI_P );
//
//	// 15p2
//	AddWingEffectID( Wing_15_2_Premium, 		AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_15_P2 );
//	AddWingEffectID( Wing_15_2_Premium, 		AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_15_P2 );
//	AddWingEffectID( Wing_15_2_Premium, 		AURACE_TYPE_HUMAN, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WINGS_OF_15_P2 );
//
//	AddWingEffectID( Wing_15_2_Premium, 		AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_15_O_W_P2 );
//	AddWingEffectID( Wing_15_2_Premium, 		AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_15_P2 );
//	AddWingEffectID( Wing_15_2_Premium, 		AURACE_TYPE_ORC, 			AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WINGS_OF_15_O_S_P2 );
//
//	AddWingEffectID( Wing_15_2_Premium, 		AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_15_P2 );
//	AddWingEffectID( Wing_15_2_Premium, 		AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_15_P2 );
//	AddWingEffectID( Wing_15_2_Premium, 		AURACE_TYPE_MOONELF, 		AUCHARCLASS_TYPE_MAGE,		E_COMMON_CHAR_TYPE_WINGS_OF_15_P2 );
//
//	AddWingEffectID( Wing_15_2_Premium, 		AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_KNIGHT,	E_COMMON_CHAR_TYPE_WINGS_OF_15_SLA_P2 );
//	AddWingEffectID( Wing_15_2_Premium, 		AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_SCION,		E_COMMON_CHAR_TYPE_WINGS_OF_15_SCI_P2 );
//	AddWingEffectID( Wing_15_2_Premium, 		AURACE_TYPE_DRAGONSCION, 	AUCHARCLASS_TYPE_RANGER,	E_COMMON_CHAR_TYPE_WINGS_OF_15_OBI_P2 );
//}
