#pragma once

// ���������� ����Ҽ� �ִ� ��ƿ��Ƽ ��� & Ŭ������ ��Ƶδ� ���.
// �ʿ��Ұ�� ���� Include ���Ѽ� ����ϼ���
//
// ������ (2005-08-10 ���� 2:03:48)


#include "MagDebug.h"

class IgnoreErr{};
template <class T>
class ForcedErrChk
{
	mutable bool read_;
	T			 code_;

public:
	ForcedErrChk(const T& code)
		: read_(false), code_(code)	{};
	ForcedErrChk(const ForcedErrChk& cpy)
		: read_( cpy.read_), code_(cpy.code_)	{
		cpy.read_ = true;
	};

	operator T() {
		read_ = true;
		return code_;
	};

	operator IgnoreErr() {
		read_ = true;
		return IgnoreErr();
	};

	~ForcedErrChk()	{
		ASSERT( "check the errcode, please!" && read_ );
	};
};

//// ex : ForcedErrChk
//////////////////////////////////////////
//	ForcedErrChk<int>	func();
//
//	int res = func();				//good
//	if( func() ){}				//good
//	func();						//assert!
//
//	// there is no assert but don't use like this..
//	!func();
//	(int)func();
//	int dummy = func();
//	// use like this for ignore the errcode...
//	(IgnoreErr)func();

#define	DEFAULT_PARAMETER_SIZE	1024
#include "AuList.h"


// ������ (2002-05-07 ���� 1:44:35) : CGetArg2 ... �ξ� ������. .. �ξ� �����ϴ�...
class CGetArg2  
{
private:
	AuList< char * >	list;
public:
	int		SetParam( char * pa	, char * deli = NULL	);// �Ķ���͸� ���� �����Ѵ�.
	int		GetParam( int num , char * buf				);// �Ķ���͸� ����. num�� 0 based
	char *	GetParam( int num							);

	int		GetArgCount();// �Ķ������ ���� ���� 

	void	Clear();

	CGetArg2( char * pa , char * de = NULL );
	CGetArg2();
	virtual ~CGetArg2();
};


namespace profile
{
	class	Timer
	{
	protected:
		UINT32	uStartTime			;
		UINT32	uLastCheckTime		;
		UINT32	uCurrentCheckTime	;

	public:
		Timer():uStartTime( GetTickCount() ) , uLastCheckTime ( uStartTime ) , uCurrentCheckTime ( uStartTime ) {}

		void	Log( const char * pFile , int nLine )
		{
			char	strMessage[ 1024 ];

			uCurrentCheckTime	= GetTickCount();
			UINT32	uDeltaTime			= uCurrentCheckTime - uLastCheckTime;
			sprintf( strMessage , "%s(%d): %.1f�� ( ��ü %.1f�� )\n" , pFile , nLine , ( FLOAT ) uDeltaTime / 1000.0f , ( FLOAT ) ( uCurrentCheckTime - uStartTime ) / 1000.0f );
			OutputDebugString( strMessage );
			uLastCheckTime	= uCurrentCheckTime;
		}
	};
};

namespace memory
{
	class	InstanceMemoryBlock;

	class	InstanceFrameMemory
	{
	public:
		static InstanceMemoryBlock	* _pCurrentMemoryBlock;

		static void * operator new( size_t size );
		static void operator delete( void *p , size_t size )
		{// do nothing
		}

		InstanceFrameMemory(){}
		virtual ~InstanceFrameMemory(){}
	};

	class	InstanceMemoryBlock
	{
	public:
		InstanceMemoryBlock():_pMemoryBlock( NULL ),_pCurrent(NULL), _uSize( 0 )
		{
			InstanceFrameMemory::_pCurrentMemoryBlock	= this;
		}

		virtual ~InstanceMemoryBlock()
		{
			ReleaseMemoryBlock();
			InstanceFrameMemory::_pCurrentMemoryBlock	= NULL;
		}

		bool	CreateMemoryBlock( size_t	uSize )
		{
			ReleaseMemoryBlock();

			_pMemoryBlock	= new	BYTE[ uSize ];

			if( !_pMemoryBlock )
			{
				// �޸� Ȯ�� ����.
				return false;
			}

			_pCurrent		= _pMemoryBlock;
			_uSize			= uSize;
			return true;
		}

		void	ReleaseMemoryBlock()
		{
			if( _pMemoryBlock )	delete [] _pMemoryBlock;
			_pMemoryBlock	= NULL;
			_pCurrent		= NULL;
			_uSize			= 0;
		}

		bool	IsValid( BYTE * pPointer )
		{
			if( _pMemoryBlock <= pPointer &&
				pPointer < _pMemoryBlock + _uSize ) return true;
			else return false;
		}
		
		void	*	GetMemory( size_t uSize )
		{
			BYTE * pReturn = NULL;

			#ifdef _DEBUG
			if( _pCurrent + uSize >= _pMemoryBlock + _uSize )
				return NULL; // memory over
			#endif

			pReturn	= _pCurrent;
			_pCurrent += uSize;
			return ( void * ) pReturn;
		}
	protected:
		
		BYTE	* _pMemoryBlock;
		BYTE	* _pCurrent;
		size_t	_uSize;
	};
};

// 2008/05/13
// ������ �ӽ� ������ ���� ���� ������.
// ������ Scope���� new�� ����Ÿ�� ������ �ֱ� ���� �༮.
template< typename T>
class AuAutoPtr
{
public:
	AuAutoPtr( T * pPtr = NULL ) : _pPointer( pPtr ) {}

	~AuAutoPtr(){ SafeDestroy(); }

	void	operator=( T * pPtr ){ SafeDestroy(); _pPointer = pPtr; }
	T * operator->() { return _pPointer; }

	bool	operator==( T * pPtr ){ return _pPointer == pPtr ? true : false; }

protected:
	T	*_pPointer;

	void	SafeDestroy() { if( _pPointer ) delete _pPointer; }
};


