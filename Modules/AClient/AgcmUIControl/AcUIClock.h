#ifndef			_ACUICLOCK_H_
#define			_ACUICLOCK_H_

#include <AgcmUIControl/AcUIBase.h>
#include <AgcSkeleton/AcDefine.h>

// Command Message
enum 
{
	UICM_CLOCK_MAX_MESSAGE								= UICM_BASE_MAX_MESSAGE
};

typedef enum
{
	ACUICLOCK_TYPE_CUT_HEAD								= 0x01,
	ACUICLOCK_MAX_TYPE,
} AcUIClockType;

typedef	enum
{
	ACUICLOCK_ANIM_NONE									= 0,
	ACUICLOCK_ANIM_DECREASE,
	ACUICLOCK_ANIM_INCREASE
} AcUIClockAnimType;

class AcUIClock : public AcUIBase
{
public:
	AcUIClock( void );
	virtual ~AcUIClock( void );

public:
	INT32												m_lTotalPoint;			// �� Point
	INT32												m_lCurrentPoint;		// ���� Point

	FLOAT												m_fLengthRate;			
	INT32												m_lAlphaImageID;		

	INT32												m_eClockType;

	AcUIClockAnimType									m_eAnim;	
	FLOAT												m_fAfterImageRate;
	FLOAT												m_fAfterImageMoveSpeed;

	UINT32												m_ulLastTick;
	My2DVertex											m_vAlphaFan[ 7 ];
	
public:
	void				SetPointInfo					( INT32 lTotalPoint, INT32 lCurrentPoint );	// Point Info �� Setting �Ѵ� - ���⼭ �� Control�� Width�� �����ȴ�
	void				SetAlphaImageID					( INT32 lImageID );
	INT32				GetAlphaImageID					( void ) { return m_lAlphaImageID; }

private :
	RwTexture*			_GetAlphaTexture				( void );
	BOOL				_IsVaildTexture					( RwTexture* pTexture );

//virtual fuction
public:
	virtual	void		OnWindowRender					( void );

};	

#endif			// _ACUICLOCK_H_