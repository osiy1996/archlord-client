#ifndef __AGCM_ANIMATION_H__
#define __AGCM_ANIMATION_H__

//#include "ApModule.h"
//#include "AgcaAnimation.h"
//#include "AgcmIntervalAnimAdmin.h"

/*class AgcmAnimation
{
public:
	AgcmAnimation();
	virtual ~AgcmAnimation();

protected:
	AgcdAnimationCallbackList	*m_pcsList;

	INT32						m_lNumCurCB;
	INT32						m_lNumMaxCB;

public:
	FLOAT						m_fCurrentTime;
	FLOAT						m_fDuration;

protected:
	BOOL						AddAnimList(AgcdAnimationCallbackList *pstPreList, AgcdAnimationCallbackList **ppstCurList, AgcdAnimationCallbackList *pstNextList, AgcdAnimationCallbackData *pcsData);
	AgcdAnimationCallbackList	*HeadMoveToTail(AgcdAnimationCallbackList *pstList);
	AgcdAnimationCallbackList	*RemoveAnimList(AgcdAnimationCallbackList *pstCurList);

public:
	///////////////////////////////////
	// Inializations
	// �������� �ʴ� ��� �������� Callback�� �����Ѵ�!
	VOID	SetMaxCallback(INT32 lMax);

	///////////////////////////////////
	// Operations
	// ���������� ResetAnimCB�� �θ���.
	VOID	InitializeAnimation(FLOAT fDuration);

	///////////////////////////////////
	// Attributes
	BOOL	AddAnimCB(AgcdAnimationCallbackData *pcsData);
	VOID	ResetAnimCB(BOOL InfluenceNextAnimation = TRUE);

	INT32	ParseAnimPoint(CHAR *szAnimPoint, INT32 *plDest);

	INT32	GetNumCallback()	{return m_lNumCurCB;}
	FLOAT	GetDuration()		{return m_fDuration;}

	///////////////////////////////////
	// Update
	BOOL	AddTime(FLOAT fTime);
};*/

#endif // __AGCM_ANIMATION_H__