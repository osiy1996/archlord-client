// AcMapLoader.h: interface for the AcMapLoader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACMAPLOADER_H__DD7AD371_6D11_41D0_A556_FDDDC07F9998__INCLUDED_)
#define AFX_ACMAPLOADER_H__DD7AD371_6D11_41D0_A556_FDDDC07F9998__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <ApBase/ApBase.h>
#include <AuList.h>
#include <ApmMap/MagUnpackManager.h>

#define	ACMAPLOADER_MAX_QUEUE	512
#define	ACMAPLOADER_PACK_QUEUE	12

class AgcmMap		;
class ApWorldSector	;

class AcMapLoader  
{
protected:
	class	LoadBox
	{
	public:
		ApWorldSector	*	pSector	;
		INT32				nDetail	;	// �ε�� ������..

		LoadBox() : pSector( NULL ) , nDetail( -1 ) {}
	};

	unsigned long 	m_hThread	;
	BOOL			m_bEnd		;

	//LoadBox			m_aSectors[ACMAPLOADER_MAX_QUEUE];
	
	//INT16			m_nIn		;
	//INT16			m_nOut		;

	AuList< LoadBox >	m_listSectorQueue;

	ApMutualEx		m_csMutex	;

	BOOL			DoExistSectorInfo( ApWorldSector * pSector, INT32 nDetail );
	BOOL			AddLoadSectorInfo( ApWorldSector * pSector, INT32 nDetail );

	// unpack block ����..
	struct	UnpackBlock
	{
		CMagUnpackManager	*	pUnpackManager	;	// ���� �޴��� ������..
		INT32					nDetail			;	// Detail/Rough
		UINT					uBlockIndex		;	// ���� �ε����� ���� ��..
		UINT					uLastAccessTime	;	// ���������� �＼���ѽð�..

		UnpackBlock():pUnpackManager(NULL),uBlockIndex(0),uLastAccessTime(0),nDetail(0) {}
		~UnpackBlock() { if( pUnpackManager ) delete pUnpackManager; }
	};

	UnpackBlock		m_aUnpackBlock[ ACMAPLOADER_PACK_QUEUE ];
	INT32			m_nUnpackCount							;//� ����.

public:
	CMagUnpackManager	*	GetUnpackManager( INT32 nDetail , UINT nBlockIndex , char * pFilename );
	void					FlushUnpackManager();
	
	AcMapLoader();
	virtual ~AcMapLoader();

	/*
	BOOL		StartThread();
	BOOL		StopThread();

	static VOID	Process(PVOID pvArgs);
	*/

	BOOL		LoadDWSector	( ApWorldSector * pvSector , INT32 nDetail = -1 );
	BOOL		RemoveQueue		( ApWorldSector * pvSector , INT32 nDetail = -1 );	// �ش� ������ �ε� ť�� ��� �����Ѵ�.
																					// nDetail �� -1�̸� �������, �����ϸ�ø� �ش� ������ ����.
	BOOL		OnIdleLoad		();

	BOOL		IsEmpty			();
	void		EmptyQueue		();	// ť�� �ִ� �ε� ������ �����Ѵ�..
	INT32		GetWaitCount	(){ return m_listSectorQueue.GetCount(); };
};

#endif // !defined(AFX_ACMAPLOADER_H__DD7AD371_6D11_41D0_A556_FDDDC07F9998__INCLUDED_)
