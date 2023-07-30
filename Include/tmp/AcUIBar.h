#ifndef	_ACUIBAR_H_
#define	_ACUIBAR_H_


#include "AcUIBase.h"


// Command Message
enum 
{
	UICM_BAR_MAX_MESSAGE					= UICM_BASE_MAX_MESSAGE
};

typedef enum
{
	ACUIBAR_TYPE_CUT_HEAD					= 0x01,
	ACUIBAR_TYPE_SMOOTH						= 0x02,
	ACUIBAR_MAX_TYPE,
} AcUIBarType;

// Bar �� ���� �ִϸ��̼� Ÿ��..
// NONE : ����
// DECREASE : ���� �ִϸ��̼�
// INCREASE : ���� �ִϸ��̼�
typedef	enum
{
	ACUIBAR_ANIM_NONE						= 0,
	ACUIBAR_ANIM_DECREASE,
	ACUIBAR_ANIM_INCREASE
} AcUIBarAnimType;


// Bar Ŭ����
class AcUIBar : public AcUIBase
{
public:
	AcUIBar();
	virtual ~AcUIBar();

public:
	INT32									m_lTotalPoint;				// �� Point
	INT32									m_lCurrentPoint;			// ���� Point
	FLOAT									m_fCurrRate;				// ���� �����

	INT32									m_lEdgeImageID;				// �ܰ��� ǥ���� �̹��� ID
	INT32									m_lBodyImageID;				// Bar �߽ɺθ� ǥ���� �̹��� ID

	BOOL									m_bVertical;
	INT32									m_eBarType;

	AcUIBarAnimType							m_eAnim;					// ���� Bar�� �������� �ִϸ��̼�
	FLOAT									m_fAfterImageRate;			// Bar �� �������� �ִϸ��̼��� ��ǥ��ġ
	FLOAT									m_fAfterImageMoveSpeed;		// Bar �� �������� �ִϸ��̼��� ����ӵ�

	UINT32									m_ulLastTick;				// ���������� ������Ʈ�� Tick ��

private :
	void			_CalculateNextRate		( INT32 eBarType, AcUIBarAnimType eAnimType, INT32 lTotalPoint, INT32 lCurrentPoint );
	BOOL			_IsTextureLoaded		( void ) { return m_csTextureList.GetCount() <= 0 ? FALSE : TRUE; }

	RwTexture*		_GetBodyTexture			( void );
	RwTexture*		_GetEdgeTexture			( void );
	BOOL			_IsValidTexture			( RwTexture* pTexture );

	BOOL			_DrawTexture			( RwTexture* pTexture, INT32 lPosX, INT32 lPosY, INT32 lWidth, INT32 lHeight, INT32 lUStart, INT32 lVStart, INT32 lUEnd, INT32 lVEnd, FLOAT fAlphaValue = 255.0f );
	BOOL			_DrawTexture			( RwTexture* pTexture, FLOAT fPosX, FLOAT fPosY, FLOAT fWidth, FLOAT fHeight, FLOAT fUStart, FLOAT fVStart, FLOAT fUEnd, FLOAT fVEnd, FLOAT fAlphaValue = 255.0f );
	BOOL			_DrawBody				( RwTexture* pTexture, INT32 lAbsX, INT32 lAbsY );
	BOOL			_DrawBodyNoAnim			( RwTexture* pTexture, INT32 lAbsX, INT32 lAbsY );
	BOOL			_DrawBodySmooth			( RwTexture* pTexture, INT32 lAbsX, INT32 lAbsY );
	BOOL			_DrawBodySmoothShadow	( RwTexture* pTexture, INT32 lAbsX, INT32 lAbsY, INT32 lOffSet, FLOAT fAlphaValue = 255.0f );
	BOOL			_DrawBodySmoothBody		( RwTexture* pTexture, INT32 lAbsX, INT32 lAbsY, INT32 lOffSet, FLOAT fAlphaValue = 255.0f );
	BOOL			_DrawEdge				( RwTexture* pTexture, INT32 lAbsX, INT32 lAbsY );
	
public:
	void			SetPointInfo			( INT32 lTotalPoint, INT32 lCurrentPoint );	// Point Info �� Setting �Ѵ� - ���⼭ �� Control�� Width�� �����ȴ�
	void			SetEdgeImageID			( INT32 lEdgeImageID ) { m_lEdgeImageID = lEdgeImageID; }
	void			SetBodyImageID			( INT32 lBodyImageID ) { m_lBodyImageID = lBodyImageID; }
	INT32			GetEdgeImageID			( void ) { return m_lEdgeImageID;	}
	INT32			GetBodyImageID			( void ) { return m_lBodyImageID; }

//virtual fuction
public:
	virtual	void	OnWindowRender			( void );
};	

#endif			// _ACUIBAR_H_