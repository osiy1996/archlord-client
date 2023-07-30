// AgcmCamera2.h: interface for the AgcmCamera2 class.
//

// ���콺 ��ǲ�� ���� Ȥ�� ��ĳ���Ͱ� ���������ν� �߻��� ī�޶� ���� ������Ʈ��
// OnIdle ���� �Ѵ�.

// ** camera work base functions
// rotation, zoom �� ���� ����� �����Ͽ����� ���� ������ ������ ���� �� �ִ�.
// �Ʒ� ī�޶� ������ ���� �Լ����� ��κ� ī�޶� ���� �ʿ��� ����(��, ��ġ)�� �����ϰ� ������Ʈ�÷��׸� ���������ν� ���� ��Ʈ���� ������Ʈ�� OnIdle ���� �Ѵ�.
// bRotWY(RwReal deg);				input : mouse L_Button + mouse move : x����
// bRotCXRwReal deg);				input : mouse L_Button + mouse move : y����
// bZoom(RwReal ds);				input : wheel
// bTurnWY_180(void);				input : mouse middlebutton ( 180 ���� ������ �ð����� ȸ���ϸ�. �����Լ��� ���������� �ӵ��� �ǵ��� ���� )
// bOnMoveTarget(void);				desc  : ĳ���� �̵��� ��ŭ ī�޶��̵�.

// ** SetCharacter and Move, Stop callback functions
// CBOnTargetMove( PVOID pData, PVOID pClass, PVOID pCustData );
// CBOnTargetStop( PVOID pData, PVOID pClass, PVOID pCustData );
// CBOnTargetSet ( PVOID pData, PVOID pClass, PVOID pCustData );

// ** util classes
// AgcuCamConstrain		: ī�޶� �������� ����
// AgcuCamDampingForce	: ī�޶� ������ ��������� smoothing
// AgcuCamSpringForce	: ĳ���̵��� ī�޶� �� �ٶ󺸴� ���� ĳ������ ����������(������)
//						  ī�޶� �������ǿ��� ������� ���������� �̿��� ����(ȸ���������� ������ : constrain of pitch)
// AgcuCamPathWork		: ������ ��ũ�ε� ���۽� ���ö����� �̿��� ī�޶������ ����Ͽ����� ���������.
// AgcuCamMode			: 1��Ī, 3��Ī(����, ž, ����)���� ��ȯ, ����庰 ������ �����ڿ���.

// ** �ʿ� ������ ���� �� �׽�Ʈ�� ���� ���� : cam2opt.txt( ������� ������ �߰��� )
// AgcmCamera2::OnIdle ���� [shift + 'R'] Ű�������� AgcmCamera2::OptionFromFile ȣ��
//////////////////////////////////////////////////////////////////////

#ifndef _AGCMCAMERA2_H_
#define _AGCMCAMERA2_H_

#include <AgcModule/AgcModule.h>
#include <stack>

typedef const RwV3d*	LPCRWV3;

class AgcuPathWork;
class AuLua;

class AgcmCamera2 : public AgcModule  
{
public:
	//��ɻ�� �÷���
	enum e_opt_flag	{
		e_flag_use_springforce			= 0x00000001,	//������
		e_flag_use_dampingforce			= 0x00000002,	//ȸ�� �� �ܵ��ۿ��� ����ȿ��
		e_flag_use_constrain_zoom		= 0x00000004,	//�ܱ��� ( ������ �ø��� & ���̴��� ������Ʈ�� ī�޶� �Ÿ� �������� )
		e_flag_use_constrain_pitch		= 0x00000008,	//������ ( ���ǵ� : �������� ī�޶����� ���� ĳ���͸� �����϶� ī�޶�� ĳ�����̿� ������ ���θ��� ��� ī�޶� ȸ������ ���ø� )

		e_flag_use_LimitPitch			= 0x00000010,	//���� ���� �÷���
		e_flag_use_LimitZoom			= 0x00000020,	//������ �÷���

		e_flag_use_reStoreSpring_zoom	= 0x00000040,	//�ܱ������ǿ��� �������.. ������ ����
		e_flag_use_reStoreSpring_pitch	= 0x00000080,	//������
	};

	//�������ӿ� ������Ʈ �Ǿ���� �÷��׵�.
	enum e_upt_mask {
		e_mask_yaw				= 0x00000001,
		e_mask_pitch			= 0x00000002,
		e_mask_zoom				= 0x00000004,
		e_mask_move				= 0x00000008,

		e_mask_wired			= 0x00000010,	//������
		e_mask_compressed		= 0x00000020,	//������

		e_mask_releaseSphiral	= 0x00000040,	//������
		e_mask_releaseLinear	= 0x00000080,	//������

		e_mask_turn180			= 0x00000100,
		e_mask_moveTarget		= 0x00000200,

		e_mask_alphaTarget		= 0x00000400,
		e_mask_transparentTarget= 0x00000800,	//transparent main character
	};


	typedef void (AgcmCamera2::*mfptr_float)(float);
public:
	enum e_idle_type {
		e_idle_default	= 0,	//rot_cx, rot_wy, zoom, follow charac
		e_idle_login	,		//do nothing

		e_idle_chagemode,		//mode change

		e_idle_path		,		//path work

		e_idle_num		,
	};


	//target info
	enum e_targetState {
		e_target_static	= 0,
		e_target_moving	,

		e_target_num	,
	};
	struct stTargetInfo	{
		e_targetState	m_eState;
		RwFrame*		m_pFrm;
		RwV3d			m_vOffset;
		RwV3d			m_vLookat;
		RwV3d			m_vEye;

		stTargetInfo();

		//get matrix axis
		const RwV3d*	GetPtrRight(void);
		const RwV3d*	GetPtrUp(void);
		const RwV3d*	GetPtrAt(void);
		const RwV3d*	GetPtrPos(void);

		void			CalcLookat(const RwV3d* pVEyeSubAt=NULL);

		void			OnMove(const RwV3d* pvEyeSubAt=NULL);
		void			OnStop(void);
		void			OnSet(RwFrame* pFrmTarget, const RwV3d* pvOffset=NULL, const RwV3d* pvEyeSubAt=NULL);
		void			SetYOffset(RwReal fup);
	};

public:
	//camera
	const RwV3d*	bGetPtrCX(void);
	const RwV3d*	bGetPtrCY(void);
	const RwV3d*	bGetPtrCZ(void);
	const RwV3d*	bGetPtrEye(void);
	const RwV3d*	bGetPtrLookat(void);
	
	//. 2006. 5. 23. nonstopdj 
	const float		bGetMaxDistance()
	{
		return m_fMaxLen;
	}

	const float		bGetMinDistance()
	{
		return m_fMinLen;
	}
	
	//. 2006. 4. 17. Nonstopdj
	//. m_v3dEyeSubAt value init.
	void			InitEyeSubAtValue(void)
	{
		m_v3dEyeSubAt.x		= 281.f;
		m_v3dEyeSubAt.y		= 281.f;
		m_v3dEyeSubAt.z		= 450.f;
	};
	//settind
	void		bSetTargetFrame(RwFrame* pFrmTarget, const RwV3d& vOffset);
	void		bSetIdleType(e_idle_type eIdle);

	//call back	
	static	BOOL	CBOnTargetMove( PVOID pData, PVOID pClass, PVOID pCustData );
	static	BOOL	CBOnTargetStop( PVOID pData, PVOID pClass, PVOID pCustData );
	static	BOOL	CBOnTargetSet ( PVOID pData, PVOID pClass, PVOID pCustData );

	static	BOOL	CBIsCameraMoving	( PVOID pData, PVOID pClass, PVOID pCustData );
	static	BOOL	CBTurnCamera	( PVOID pData, PVOID pClass, PVOID pCustData );

	static	BOOL	CBRegionChange(PVOID pData, PVOID pClass, PVOID pCustData);

//virtual
	BOOL		OnAddModule();
	BOOL		OnInit();
	BOOL		OnDestroy();
	BOOL		OnIdle(UINT32 ulClockCount);
	void		OnLuaInitialize( AuLua * pLua );

	//basic camera work
	void		bRotWY		(RwReal deg);		//WY : world's y axis
	void		bRotCX		(RwReal deg);		//CX : camera's x axis
	void		bZoom		(RwReal ds);
	void		bMove		(const RwV3d& vdelta);
	void		bTurnWY_180 (void);


	void		bOnMoveTarget(void);

	void		bSetCamFrm(const RwV3d& eye, RwReal fYaw, RwReal fPitch);
	void		bSetCamFrm(const RwV3d& lookat, const RwV3d& eye);
	void		bSetCamFrm(RwMatrix& mat);
	
	void		bSetViewWindow(const RwV2d& window);
	void		bGetViewWindow(RwV2d& window);

	void		bTransparentMainCharac();

	void		bLockAct	(e_upt_mask eMask);
	void		bUnlockAct	(e_upt_mask eMask);
	//option
	void		bOptFlagOn(e_opt_flag eopt);
	void		bOptFlagOff(e_opt_flag eopt);

	bool		bIsMoving()
	{
		// ī�޶� ���� �����̰� �ִ��� ����.
		return m_stTarget.m_eState == e_target_static ? false : true;
	}
	
private:
	//idle
	void		vOnIdle_default(RwReal fElasped);
	void		vOnIdle_Login(RwReal fElasped);
	void		vOnIdle_modechage(RwReal fElasped);

	//for debugging
	void		vShowInfo(void);

	//option_mask
	bool		vLockFlagChk(e_upt_mask eMask);
	bool		vOptFlagChk(e_opt_flag eopt)const;
	//update_mask
	void		vUdtMaskOn(e_upt_mask eudt);
	void		vUdtMaskOff(e_upt_mask eudt);
	bool		vUdtMaskChk(e_upt_mask eudt)const;

	//available check
	void		vAvailablePitch(RwReal& pitchDelta);
	void		vAvailableZoom(RwReal& zoomDelta);

	//update matrix
	void		vUpdateMatrix();
	void		vForcedZoom(RwReal offset);
	void		vForcedRotCX(RwReal offset);
	void		vAdjLookAt(RwReal currlen);
	void		vAdjAlpha(RwReal currlen);

private:
	//flag
	RwUInt32		m_ulOptFlag;		//option flag
	//module
	class AgcmRender*		m_pAgcmRender;
	class AgcmMap*		m_pAgcmMap;
	class ApmObject*		m_pApmObject;
	class AgcmObject*		m_pAgcmObject;
	class AgpmCharacter*	m_pAgpmCharacter;
	class AgcmCharacter*	m_pAgcmCharacter;
	RpClump*		m_pClumpCharacter;
	//base	info
	RwCamera*		m_pRwCam;
	stTargetInfo	m_stTarget;
	RwV3d			m_v3dLookat;
	RwV3d			m_v3dEyeSubAt;

	//basic camera work
	RwUInt32		m_ulLockMask		;	//�ش� �÷��׵��� ��ݻ��·�( e_upt_mask )
	RwUInt32		m_ulUpdateMask		;	//OnIdle���� �����ؾ��� �÷��׵�
	RwReal			m_fWYaw				;
	RwReal			m_fCPitch			;
	RwReal			m_fZoom				;
	RwV3d			m_vMove				;
	FLOAT			m_fCharacterHeight	;

	//turn 180
	RwReal			m_fTurnAccumTime;	//sec ���� ȸ���ð�
	RwReal			m_fTurnAccumAngle;	//deg ���� ȸ����
	RwReal			m_fTurnTime;		//m_fTurnAngle ȸ���ϴµ� �ɸ��� �ð�
	RwReal			m_fTurnAngle;		//m_fTurnTime���� ȸ���� ����
	RwReal			m_fTurnSpeed;		//m_fTurnSpeed = m_fTurnAngle/m_fTurnTime
	RwReal			m_fTurnCoef;		//m_fTurnCoef = 180.f / (DURING*sinf(OMEGA*DURING) + DURING);

	//constrained
	//distance
	RwReal			m_fDesiredLen;
	RwReal			m_fCurrLen;
	RwReal			m_fLimitedLen;
	//pitch(radian)
	RwReal			m_fDesiredPitch;	//������
	RwReal			m_fCurrPitch;		//������
	RwReal			m_fLimitedPitch;	//������

	//limit
	RwReal			m_fMinLen;
	RwReal			m_fMaxLen;
	RwReal			m_fMinPitch;
	RwReal			m_fMaxPitch;
	
	//idle
	e_idle_type		m_eIdle;
	mfptr_float		m_fptrOnIdle;

public:
	// Lua
	struct	PathWorkInfo
	{
		vector< RwMatrix >	vecCtrlMatrix;
		vector< FLOAT >		vecProjection;

		UINT32			uDuration			;	// ��ü����

		INT32			nType				;
		INT32			nSSType				;
		BOOL			bAccel				;
		BOOL			bClosed				;
		BOOL			bLoop				;

		PathWorkInfo():m_pPathWork( NULL ) , m_pAngleWork( NULL ) { Clear(); }

		BOOL	Save( const char * pFilename );
		BOOL	Load( const char * pFilename );

		BOOL	Play();
		BOOL	Stop();
		BOOL	OnIdle( DWORD uDiffTime , RwMatrix * pMatrix , FLOAT * pfProjection );

		void	Clear();

		INT32	Size();
		BOOL	Push( RwMatrix * pMatrix , FLOAT fProjection );
		BOOL	Pop();

		AgcuPathWork *	m_pPathWork			;	// ���ö�������
		AgcuPathWork *	m_pAngleWork		;	// �ޱ�����

		BOOL	PreparePathWork	();
		BOOL	PrepareAngleWork();

		BOOL	IsPlaying() { if( m_pPathWork ) return TRUE; else return FALSE; }
	};

	PathWorkInfo	m_stPathWork;

	struct	CameraStack
	{
		struct	Info
		{
			RwMatrix	matrix;
			FLOAT		fProjection;
		};

		std::stack< Info >	stack;

		void	Push();
		void	Pop	();
	};

	CameraStack	m_stCameraStack;

	AgcmCamera2();
	virtual ~AgcmCamera2();

	// �ֿܼ�.
	VOID	Info();
	VOID	ChangeHeight( float fCharacterHeight , float fFaceHeight );

#ifdef _DEBUG
	//for debug
	void OptionFromFile(void);
	void RenderFrustum(RwUInt32 colr=0xff00ffff);
#endif

	//friend class
	friend class AgcuCamConstrain;
	friend class AgcdCamConstrain_Obj;
	friend class AgcdCamConstrain_Terrain;
	friend class AgcuCamDampingForce;
	friend class AgcuCamSpringForce;
	friend class AgcuCamMode;
};

RwBool	IntersectTriangle(const RwV3d *puvRay	//uv	: unit vector
	, const RwV3d *pvCamPos
	, const RwV3d *pv0
	, const RwV3d *pv1
	, const RwV3d *pv2
	, RwReal* t
	, RwReal* u
	, RwReal* v);

VOID CalcPickedPoint(RwV3d& v3dOut
	, const RwV3d& v0
	, const RwV3d& v1
	, const RwV3d& v2
	, RwReal u
	, RwReal v);
#endif // _AGCMCAMERA2_H_