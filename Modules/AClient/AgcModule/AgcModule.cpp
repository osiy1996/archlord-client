#include <AgcModule/AgcModule.h>
#include <AgcModule/AgcEngine.h>

AgcModule::AgcModule() : m_uType(0), m_uPriority(0), m_bRegistered(FALSE)
{
	SetModuleType(APMODULE_TYPE_CLIENT);

	m_uDelayTick	=	0;
	m_uLastTick		=	0;
}

AgcModule::~AgcModule()
{
}

VOID AgcModule::LockRender()
{
	AgcEngine* pEngine = (AgcEngine *) GetModuleManager();
	if( pEngine )
		g_pEngine->LockRender();
}

VOID AgcModule::UnlockRender()
{
	AgcEngine* pEngine = (AgcEngine *) GetModuleManager();
	if( pEngine )
		g_pEngine->UnlockRender();
}

VOID AgcModule::LockFrame()
{
	AgcEngine* pEngine = (AgcEngine *) GetModuleManager();
	if( pEngine )
		g_pEngine->LockFrame();
}

VOID AgcModule::UnlockFrame()
{
	AgcEngine* pEngine = (AgcEngine *) GetModuleManager();
	if( pEngine )
		g_pEngine->UnlockFrame();
}

RpWorld *	AgcModule::GetWorld()
{
	AgcEngine* pEngine = (AgcEngine *) GetModuleManager();
	return pEngine ? g_pEngine->m_pWorld : NULL;
}

RwCamera *	AgcModule::GetCamera()
{
	AgcEngine* pEngine = (AgcEngine *) GetModuleManager();
	return pEngine ? g_pEngine->m_pCamera : NULL;
}

RpLight *	AgcModule::GetAmbientLight()
{
	AgcEngine* pEngine = (AgcEngine *) GetModuleManager();
	return pEngine ? g_pEngine->m_pLightAmbient : NULL;
}

RpLight *	AgcModule::GetDirectionalLight()
{
	AgcEngine* pEngine = (AgcEngine *) GetModuleManager();
	return pEngine ? g_pEngine->m_pLightDirect : NULL;
}

BOOL		AgcModule::CheckDelay( UINT nCurrentTick )
{
	if( nCurrentTick >= m_uLastTick	+ m_uDelayTick )
	{
		m_uLastTick	= nCurrentTick;
		return TRUE;
	}

	return FALSE;
}