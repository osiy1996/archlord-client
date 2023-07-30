#ifndef PTSDK_DLL
#	ifndef PTSDK_STUFF
#		define PTSDK_STUFF
#	endif
#else
#	ifndef PTSDK_STUFF
#		ifdef PTSDK_EXPORT
#			define PTSDK_STUFF	__declspec(dllexport)
#		else
#			define PTSDK_STUFF	__declspec(dllimport)
#		endif
#	endif
#endif
