#ifndef		_AGCMTUNER_H_
#define		_AGCMTUNER_H_

#include "AgcModule.h"

#include "AgcmLODManager.h"
#include "AgcmFont.h"
#include "AgcmLensFlare.h"
#include "AgcmShadow.h"
#include "AgcmWater.h"
#include "AgcmRender.h"
#include "AgcmMap.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmTunerD" )
#else
#pragma comment ( lib , "AgcmTuner" )
#endif
#endif

#define		TUNER_HANDLE_MODULE_COUNT	10

// 3�ʸ��� check����
#define		TUNER_LOD_CHANGE_TIME		2000

// 0 - font, 1 - lensflare
// �� �ܿ��� ��� �߰��ؼ� ������Ʈ ������ ���̸鼭 �ڿ������� ����� ������ �������� ������!! (Ÿ module ���� �ʿ�)

class AgcmTextBoardMng;

class AgcmTuner : public AgcModule
{
public:
	AgcmTuner();
	~AgcmTuner();

public:
	BOOL OnAddModule();
	BOOL OnInit();
	BOOL OnDestroy();
	BOOL OnIdle(UINT32 ulClockCount);

	bool		m_bStart;
	UINT32		m_uiLastTick;
	UINT32		m_uiCurTickDiff;
	
	int			m_iHighFrame;											// Tuner������ ���� frame
	int			m_iLowFrame;											// Tuner������ ���� frame
	
	int			m_iMapLoadMin;
	int			m_iMapLoadMax;
	int			m_iMapRoughMin;
	int			m_iMapRoughMax;
	int			m_iMapDetailMin;
	int			m_iMapDetailMax;
	
	float		m_fMapLoadRange;
	float		m_fMapRoughRange;
	float		m_fMapDetailRange;
	
	UINT32		m_uiElapsedTime[TUNER_HANDLE_MODULE_COUNT];				// ������ ���� �ð�
	UINT32		m_uiUpdateTime[TUNER_HANDLE_MODULE_COUNT];
	
	// Ÿ module���� ����
	bool		m_bUpdate[TUNER_HANDLE_MODULE_COUNT];
	UINT32		m_uiTickDiff[TUNER_HANDLE_MODULE_COUNT];				// update�ÿ� ���� ����ð�

	// LOD����
	UINT32		m_uiLODElapsedTime;
	UINT32		m_uiLODLastFramePerSec;

	BOOL		m_bAutoTuning;
	UINT32		m_uiLODRangeType;
	UINT32		m_uiMapLoadRangeType;
	UINT32		m_uiShadowRangeType;

	float		m_fLODRangeTable[4];
	float		m_fMapLoadRangeTable[4];
	float		m_fShadowRangeTable[3];

	AgcmRender*			m_pcmRender;
	AgcmMap*			m_pcmMap;
	AgcmLODManager*		m_pcmLODManager;
//@{ kday 20050113
//AgcmCamera*			m_pcmCamera;
//@} kday	
	AgcmFont*			m_pcmFont;
	AgcmWater*			m_pcmWater;
	AgcmLensFlare*		m_pcmLensFlare;
	AgcmShadow*			m_pcmShadow;
	AgcmTextBoardMng*		m_pcmTBoard;

public:
	inline	void		SetHighFrame(int val) { m_iHighFrame = val; }
	inline	int			GetHighFrame()		{return m_iHighFrame;}
	inline	void		SetLowFrame(int val) { m_iLowFrame = val; }
	inline	int			GetLowFrame()		{return m_iLowFrame;}

	void		SetVariableByOption(bool bAuto,UINT32 LODType,UINT32 MapLODType,UINT32 ShadowType);
};

#endif       //_AGCMTUNER_H_	