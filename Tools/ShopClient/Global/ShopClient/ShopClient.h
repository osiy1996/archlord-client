#ifndef _SHOPCLIENT_H
#define _SHOPCLIENT_H

#include "WebzenBilling.h"

#ifdef _WIN64
	#ifdef _DEBUG
		#pragma comment(lib, "WebzenBillingD_x64.lib") 
	#else
		#pragma comment(lib, "WebzenBilling_x64.lib") 
	#endif
#else
	#ifdef _DEBUG
		#pragma comment(lib, "WebzenBillingD.lib") 
	#else
		#pragma comment(lib, "WebzenBilling.lib") 
	#endif
#endif

#endif //_SHOPCLIENT_H