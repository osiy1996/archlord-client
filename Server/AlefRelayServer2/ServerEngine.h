// ServerEngine.h: interface for the ServerEngine class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _ALEF_RELAY_SERVER_ENGINE_H_
	#define _ALEF_RELAY_SERVER_ENGINE_H_

#include "ApBase.h"
#include "AsCommonLib.h"

#include "ApmMap.h"
#include "AgpmFactors.h"
#include "AgpmCharacter.h"
#include "AgpmBillInfo.h"
#include "AgpmGrid.h"
#include "AgpmItem.h"
//#include "AgpmScript.h"
#include "AgpmConfig.h"

#include "AgsmAOIFilter.h"
//#include "AgsmDBStream.h"
#include "AgsmServerManager2.h"
#include "AgsmAdmin.h"
#include "AgsmInterServerLink.h"


#include "AgsmDatabaseConfig.h"
#include "AgsmDatabasePool.h"
#include "AgsmRelay2.h"
#include "AgsmAuctionRelay.h"

#include "AgsmConfig.h"
#include "AsEchoServer.h"
#include "AgsmServerStatus.h"


/********************************************************/
/*		The Definition of Dummy test worker class		*/
/********************************************************/
//
class TestWorker : public zzThread
	{
	public:
		AgsmRelay2			*m_pRelay;
		BOOL				m_bStop;

	public:
		TestWorker();
		virtual ~TestWorker();

		void	Stop();

	protected:
		// ... Working Method(zzThread inherited)
		DWORD	Do();
	};

class ServerEngine : public AgsEngine
	{
	private:
		BOOL	SetMaxCount();
		BOOL	LoadTemplateData();

	public:
		ServerEngine();
		virtual ~ServerEngine();
		
		BOOL	OnRegisterModule();
		BOOL	OnTerminate();

		BOOL	InitServerManager();
		BOOL	CreateDBPool();
		
		void	Test(BOOL bStop);
		
		TestWorker *m_pWorker;
	};

#endif // _ALEF_RELAY_SERVER_ENGINE_H_
