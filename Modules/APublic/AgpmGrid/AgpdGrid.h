/******************************************************************************
Module:  AgpdGrid.h
Notices: Copyright (c) NHN Studio 2003 Ashulam
Purpose: 
Last Update: 2003. 09. 4
******************************************************************************/

#ifndef __AGPDGRID_H__
#define __AGPDGRID_H__

#include <ApModule/ApModule.h>
#include <ApBase/ApBase.h>

#define		AGPDGRIDITEM_BOTTOM_STRING_LENGTH		5

enum AgpdGridItemType
{
	AGPDGRID_ITEM_TYPE_NONE			=	0x00000000,
	AGPDGRID_ITEM_TYPE_ITEM			=	0x00000001,
	AGPDGRID_ITEM_TYPE_SKILL		=	0x00000002,
	AGPDGRID_ITEM_TYPE_SHORCUT		=	0x00000004,
	AGPDGRID_ITEM_TYPE_SPECIALIZE	=	0x00000008,
	AGPDGRID_ITEM_TYPE_QUEST		=	0x00000010,
	AGPDGRID_ITEM_TYPE_GUILDMARK	=	0x00000020,
	AGPDGRID_ITEM_TYPE_SOCIALACTION	=	0x00000040
	/*AGPDGRID_ITEM_MAX_TYPE*/
};

enum EnumGridType
{
	AGPDGRID_TYPE_NONE			= 0,
	AGPDGRID_TYPE_INVENTORY,
	AGPDGRID_TYPE_BANK,
	AGPDGRID_TYPE_EQUIP,
	AGPDGRID_TYPE_TRADEBOX,
	AGPDGRID_TYPE_SALES,
	AGPDGRID_TYPE_NPCTRADEBOX,
	AGPDGRID_TYPE_QUEST,
	AGPDGRID_TYPE_CASH_INVENTORY,
	AGPDGRID_TYPE_GUILD_WAREHOUSE,
	AGPDGRID_TYPE_SOCIALACTION,
	AGPDGRID_TYPE_SUB_INVENTORY,
//	AGPDGRID_TYPE_UNSEEN_INVENTORY,
};

class AgpdGridItem
{
public:
	AgpdGridItem()							;
	~AgpdGridItem()							;

public:

	AgpdGridItemType	m_eType				;
	INT32				m_lItemID			;
	INT32				m_lItemTID			;
	
	BOOL				m_bMoveable			;

	BOOL				m_bForceWriteBottom	;

	CHAR				m_strLeftBottom[AGPDGRIDITEM_BOTTOM_STRING_LENGTH + 1]	;		
	CHAR				m_strRightBottom[AGPDGRIDITEM_BOTTOM_STRING_LENGTH + 1]	;
	BOOL				m_bWriteLeftBottom	;
	BOOL				m_bWriteRightBottom	;
	
	BOOL				m_bTwinkleMode		;	// �����ϰ������� ǥ��
	BOOL				m_bBarMode			;	// Time�� ���ؼ� Charge �ɶ�... Bar Mode���� Clock �������

protected:
	BOOL				m_bIsDurabilityZero	;	// �������� 0�ΰ� ǥ���ؾ� �ϴ��� ����
	BOOL				m_bIsLevelLimited	;	// ���� ������ �ɸ�������.
public:

	BOOL				SetLevelLimited( BOOL bValue ) { return m_bIsLevelLimited = bValue; }
	BOOL				SetDurabilityZero( BOOL bValue ) { return m_bIsDurabilityZero = bValue; }

	BOOL				IsDurabilityZero() const	{ return m_bIsDurabilityZero; }
	BOOL				IsLevelLimited() const		{ return m_bIsLevelLimited; }

	BOOL				IsDisabled() const			{ return IsDurabilityZero() || IsLevelLimited(); }
	
	void				SetLeftBottomString( CHAR* pString );
	void				SetRightBottomString( CHAR* pString );

	void				SetGridDisplayData( char *pstrLeftString, char *pstrRightString );

	UINT32				m_ulUseItemTime		;
	UINT32				m_ulReuseIntervalTime	;
	UINT32				m_ulPauseClockCount	;
	UINT32				m_ulRemainTime;

	void				SetUseItemTime(UINT32 ulUseTime, UINT32 ulReuseIntervalTime, UINT32 ulPauseClockCount = 0);

	AgpdGridItem		*m_pcsTemplateGrid;
	
	ApBase				*m_pcsParentBase;

	inline ApBase*		GetParentBase()							{return m_pcsParentBase;}
	inline void			SetParentBase(ApBase *pcsParentBase)	{m_pcsParentBase = pcsParentBase;}

	CHAR				*m_szTooltip;
	void				SetTooltip(CHAR *szTooltip);
};

class AgpdGrid
{
public:
	EnumGridType	m_eGridType;	// Grid�� ���� ��ǥ Ÿ�� (�κ��丮, Bank ���)
	INT32			m_lItemCount;	//����ִ� �������� ����
	INT32			m_lGridCount;	//Grid�� ��~

	AgpdGridItem	**m_ppcGridData;

	BOOL			*m_pbLockTable;

	INT16			m_nLayer;
	INT16			m_nRow;
	INT16			m_nColumn;
	INT16			m_nMaxRowExistItem;

	INT32			m_lGridType;	// ������ Grid Item�� ������ �ִ� �Ӽ� 

	AgpdGridItem	*m_pcsSelectedGridItem;

	AgpdGrid()
	{
		Clear();
	}

	void Clear()
	{
		m_lItemCount			= 0; //����ִ� �������� ����
		m_lGridCount			= 0; //Grid�� ��~

		m_nLayer				= 0;
		m_nRow					= 0;
		m_nColumn				= 0;
		m_nMaxRowExistItem		= 0;
		m_ppcGridData			= NULL;
		m_pbLockTable			= NULL;
		m_eGridType				= AGPDGRID_TYPE_NONE;
		m_pcsSelectedGridItem	= NULL;
		m_lGridType				= AGPDGRID_ITEM_TYPE_ITEM | AGPDGRID_ITEM_TYPE_SKILL | AGPDGRID_ITEM_TYPE_SHORCUT | AGPDGRID_ITEM_TYPE_SPECIALIZE;
	}
};

#endif