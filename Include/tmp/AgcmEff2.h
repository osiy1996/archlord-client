// AgcmEff2.h: interface for the AgcmEff2 class.
//

// * AgcmEff2 ����
//		m_AgcdEffSetMng		: ���� ������ ����
//		m_AgcdEffCtrlMng	: �� ����Ʈ �ν��Ͻ� ����
//		m_fpEffPack			: ��ŷ ����������

// * �ܺο��� ����ϴ� �Լ�
//		OnAddModule()	: �ʿ� ������ ����
//		OnInit()		: m_AgcdEffSetMng ����
//						: �ʿ� ���� �ʱ�ȭ
//		OnDestroy()		: m_AgcdEffCtrlMng.bClear();
//						: g_agcuEffVBMng.bClear();
//		OnIdle()		: ��� ��ü���� ����� �ð� ����
//						: ���� �����ӿ����� ������ ��Ʈ���� ����
//						: ��� ����Ʈ �ν��Ͻ����� �ð� ����
//		UseEffSet		: ����Ʈ �ν��Ͻ� ����
//		RemoveEffSet	: ����Ʈ �ν��Ͻ� ����

// * �� ����Ʈ���� �帧
//		���� : AgcmEff2::UseEffSet
//		�Ҹ� : AgcmEff2::RemoveEffSet or LIFE CHECK @ AgcdEffCtrlMng::bTimeUpdate
//		������Ʈ : AgcmRender�� Callback ��� ( AgcdEffCtrl_Set::CB_Update )
//			- foreach AgcdEffCtrl_XXX::bUPdate
//				- foreach AgcdEffAnim::bUpdateV
//		���� : AgcmRender�� Callback ��� ( AgcdEffCtrl_Set::CB_Render )
//			- foreach AgcdEffCtrl_XXX::bRender

//////////////////////////////////////////////////////////////////////
#if !defined(AFX_AGCMEFF2_H__9C3E71BF_28C0_41AA_8063_04F5127984DA__INCLUDED_)
#define AFX_AGCMEFF2_H__9C3E71BF_28C0_41AA_8063_04F5127984DA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgcModule.h"
#include "AgcdEffSetMng.h"
#include "AgcdEffCtrlMng.h"
#include "ApRWLock.h"

class AgcmEff2 
	: public AgcModule
{

public:
	AgcmEff2			( VOID );
	virtual ~AgcmEff2	( VOID );

public:
	BOOL					OnAddModule					( VOID );
	BOOL					OnInit						( VOID );
	BOOL					OnDestroy					( VOID );
	BOOL					OnIdle						( UINT32 ulNowTime );

	LPEFFCTRL_SET			CreateEffSet				( LPEFFSET			pEffSet			, LPSTEFFUSEINFO Info	, AgcdEffCtrl_Set* pNewEffCtrl_Set );
	LPEFFCTRL_SET			UseEffSet					( LPSTEFFUSEINFO	pstEffUseInfo	, BOOL bForceImmediate = TRUE	);
	BOOL					RemoveEffSet				( LPEFFCTRL_SET		pEffCtrlSet		, BOOL bCallCB = TRUE			);
 
	VOID					PushToEffSetPool			( LPEFFSET pEffSet );
	VOID					PopFromEffSetPool			( LPEFFSET pEffSet );

	RwInt32					GetSoundLength				( RwUInt32 dwEffSetID	);
	FILE*					GetFPEffSet					( VOID					)	{	return m_fpEffPack;			}
	AgcdEffCtrlMng *		GetCtrlMng					( VOID					)	{	return &m_AgcdEffCtrlMng;	}

	BOOL					SetGlobalVariable			( RpClump *pClump , RwFrame* pFrmMainCharac, RpWorld* pWorld, RwCamera* pCamera );
	BOOL					SetMainCharacterFrame		( RpClump *pClump , RwFrame* pFrmMainCharac );

	VOID					ToneDown					( RwBool bSwitch );

	VOID					EffOn						( VOID );
	VOID					EffOff						( VOID );

	static BOOL				CBLoadEffBase				( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL				CBRemovecharacter			( PVOID pData, PVOID pClass, PVOID pCustData );

	BOOL					FindEffectSet				( AgcdEffCtrl_Set* pcdEffCtrl_Set ) { return m_AgcdEffCtrlMng.FindEffectSet( pcdEffCtrl_Set ); }
	BOOL					RemoveAllLoadingEffsetList	( VOID );

	BOOL					SetCallbackRemoveEffect		( ApModuleDefaultCallBack pfCallback, PVOID pClass );

	///////////////////// Tool ////////////////////////
	const LPEffectSetMap&	ForTool_GetEffSetMng		( VOID ) 					{ 	return m_AgcdEffSetMng.ForTool_GetEffSetMap();									}
	LPEFFSET				ForTool_FindEffSet			( UINT32	ulEffID		)	{	return m_AgcdEffSetMng.FindEffSet( ulEffID );									}
	RwInt32					ForTool_DeleteEffCtrl		( UINT32	ulEffSetID	)	{	return m_AgcdEffCtrlMng.ForTool_DeleteEff( ulEffSetID );						}
	RwInt32					ForTool_InsEffSet			( LPEFFSET	lpNewEffSet	)	{	return m_AgcdEffSetMng.ForTool_Ins(lpNewEffSet);								}
	RwInt32					ForTool_DeleteEffSet		( UINT32	ulEffID		)	{	ForTool_DeleteEffCtrl(ulEffID); return m_AgcdEffSetMng.ForTool_Del(ulEffID);	}
	BOOL					ForTool_ReloadAllEffect	( VOID )					{	return _BuildEffSetContainer();		}

	RwInt32					ForTool_MakeEffFile		( LPCSTR exepath = NULL , LPCSTR packingpath = NULL , AcCallbackData1 pfOutputCallback = NULL )
	{
		AuAutoSetDirectory setcurrentdirectory( exepath );
		ASSERT( packingpath && ( (packingpath[ strlen(packingpath) - 1 ]	== '\\') || (packingpath[ strlen(packingpath) - 1 ] == '/') ) );

		return m_AgcdEffSetMng.MakeEffFile(packingpath, pfOutputCallback);
	};
	///////////////////// Tool ////////////////////////


	VOID					DBG_ShowWire				( VOID );
	VOID					DBG_ShowSphere				( VOID );
	VOID					DBG_ShowEffFrm				( VOID );
	VOID					DBG_ShowEffMemoryLog		( VOID );
	VOID					DBG_ShowEffInfo				( RwUInt32 effID );

#ifdef _DEBUG
	RwUInt32			m_chkEffSetID;
	RwUInt32			m_skipEffSetID;
	RwInt32				m_axisDbgType;//0,1,2,3,4,5
	RwInt32				m_dbgBaseIndex;

	VOID					DBG_ClearDbgAxis			( VOID );
#endif



private:
	BOOL					_BuildEffSetContainer		( VOID );

public:
	INT32					m_lLoaderLoadID;
	ApRWLock				m_RWLock;

private:
	AgcdEffSetMng			m_AgcdEffSetMng;	//���� ������ ����
	AgcdEffCtrlMng			m_AgcdEffCtrlMng;	//���� �ν��Ͻ� ����
	FILE*					m_fpEffPack;		//��ŷ ����������

public :
	LPEFFSET				GetEffectSet				( int nEffectID );
};	

#endif