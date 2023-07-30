// AgcModule.h:
// Ŭ���̾�Ʈ�� ��� Ŭ����.
// �ۼ� : �����
// ���� : 2002/04/04
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGCMODULE_H__FD36A322_47D9_11D6_9402_000000008510__INCLUDED_)
#define AFX_AGCMODULE_H__FD36A322_47D9_11D6_9402_000000008510__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma comment ( lib , "imm32" )		// IME ���̺귯�� ����
#pragma comment ( lib , "Winmm" )		// ������ (2004-01-06 ���� 6:40:53) : �ֹ� ���̺귯��. timeGettime ���� ����.

#include "ApModule.h"
#include "rwcore.h"
#include "RpWorld.h"
#include "AuLua.h"

#include "skeleton.h"

extern "C" {
	typedef struct 
	{
		const char *name;
		int (*func)(lua_State *);
	} luaDef;
}

// Ŭ���̾�Ʈ������ ApcEngineŬ������ �߽������Ͽ� ���� ����� ������Ѽ� �̷������. 
// ����� 4���� Ÿ��( ����Ÿ,UI,FullUI,Network )�� ������ ���������� �۵��Ѵ�.
enum	CAMERASTATECHANGETYPE
{
	CSC_INIT = 1,
	CSC_RESIZE,
	CSC_NEAR,
	CSC_FAR,
	CSC_ACTIVATE
};

struct RsKeyStatus;
struct RsMouseStatus;
class AgcModule : public ApModule  
{
public:
	// Ŭ���̾�Ʈ ��� Ÿ���� ����.
	enum	MODULETYPE
	{
		DATAMODULE		= 0x01,
		WINDOWUIMODULE	= 0x02,
		FULLUIMODULE	= 0x04,
		NETWORKMODULE	= 0x08
	};

public:
	AgcModule();
	virtual ~AgcModule();

	virtual	void OnCameraStateChange(CAMERASTATECHANGETYPE ChangeType)	{		}
	virtual	void OnLuaInitialize( AuLua* pLua )							{		}
	virtual	void OnTerminate()											{		}	// OnClose�� UI �����̰� , Terminate�� ��Ÿ ��� ��. ���� ó���̴�.

	void		SetType( UINT32 type )		{	m_uType	= type;			}
	BOOL		IsRegistered()				{	return m_bRegistered;	}

	VOID		LockRender();
	VOID		UnlockRender();

	VOID		LockFrame();
	VOID		UnlockFrame();

	BOOL		CheckDelay( UINT32 nCurrentTick );

	VOID		SetDelayTick( UINT32	uDelayTick )	{	m_uDelayTick	=	uDelayTick;	}
	UINT32		GetDelayTick( VOID )					{	return m_uDelayTick;			}

	RpWorld*	GetWorld();
	RwCamera*	GetCamera();
	RpLight*	GetAmbientLight();
	RpLight*	GetDirectionalLight();

public:
	UINT32		m_uType;		// ��� Ÿ���� ���� ��Ʈ �÷��׸� �Ἥ �������� Ÿ���� ���ÿ� ������ ����
	UINT32		m_uPriority;	// âUI��� ���� , �߿䵵������
	BOOL		m_bRegistered;	// ��ϵ� ������� ����.

	UINT32		m_uDelayTick;	// Delay �ð� ����
	UINT32		m_uLastTick;	// ���������� �����ߴ� Tick

};

#endif // !defined(AFX_AGCMODULE_H__FD36A322_47D9_11D6_9402_000000008510__INCLUDED_)
