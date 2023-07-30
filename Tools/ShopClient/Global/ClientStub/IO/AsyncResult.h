#ifndef  __WBANetwork_AsyncResult_H
#define  __WBANetwork_AsyncResult_H

namespace WBANetwork
{
	enum IOEventType
	{
		Event_None			= 0x000000,
		Event_Accept		= 0x000001,
		Event_Connect		= 0x000002,
		Event_Close			= 0x000004,
		Event_Send			= 0x000008,
		Event_Receive		= 0x000010
	};

	class EventHandler;

	///  �񵿱� I/O �۾��� ����� �޾ƿ� ���� ����ü
	typedef struct tag_AsyncResult : OVERLAPPED
	{
		IOEventType		eventType;
		unsigned long	transBytes;
		unsigned long	error;
		EventHandler*	handler;
		BYTE			szData[10240];
	} AsyncResult;
}

#endif