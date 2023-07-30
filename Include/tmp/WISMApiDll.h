// Desc		: WISM API Layer�� Dll�� ����ϴ� Header.
// Warn		: 1.�̰͸� include�ϸ� �ǰ� �ض�.
//			: 1.����ڰ� Dice�� WISMApiDll.h�� ���ÿ� ����ϴ� ��� DiceApiDll.h�� ���� include���Ѿ� �Ѵ�.
// Copyright: Milkji -- All rights reserved.
// Update	: 20100624(milkji)
#pragma once


////////////////////////////////////////////////////////////////////////////////////////////////////
// Desc	: Dll Export-Import
////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(WISMAPI_EXPORTS)
	#define	WISMAPI_XPORT	__declspec(dllexport)
#else
	#define	WISMAPI_XPORT	__declspec(dllimport)
#endif



////////////////////////////////////////////////////////////////////////////////////////////////////
// Desc	: switch&��� ����
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <sal.h>
#include <CodeAnalysis\SourceAnnotations.h>
#ifndef _INC_WINDOWS
#include <windows.h>
#endif /* _INC_WINDOWS */

// VS2008 �̸� ����ϴ� ��� �ʿ� (hjjin)
#if defined(_MSC_VER)
	#if(_MSC_VER < 1500)
		#define	_Check_return_
		#define	_In_
		#define	_In_z_
		#define	_Inout_
		#define	_In_opt_
		#define	_In_opt_z_
		#define	_Out_
		#define	_Out_opt_
		#define	_Printf_format_string_
		#define	_In_count_(x)
		#define	_In_z_count_(x)
		#define	_In_bytecount_(x)
		#define	_In_opt_bytecount_(x)
		#define	_Out_cap_(x)
		#define	_Out_z_cap_(x)
		#define	_Out_bytecap_(x)
		#define	_Inout_bytecount_(x)
	#endif
#endif

namespace WISM
{
	enum
	{
		eNumberQueueSize=512,					/*! ����Queue�� ũ��						*/
		eStringQueueSize=512,					/*! ����Queue�� ũ��						*/
		eMaxLengthOfString=512,
		eHeartbeatIdentification=1,				/*! ���񽺰� ������� �ǹ��ϴ� �ĺ���		*/
		eConcurrentUserCountIdentification=2,	/*! ���� ����� ����� ��(����) �ĺ���	*/
	};

	enum ENotificationNumber
	{
		eNotificationNumberValue=0,				/*!	���簪								*/
		eNotificationNumberMin,					/*! �ּҰ�								*/
		eNotificationNumberMax,					/*! �ִ밪								*/
		eNotificationNumberCount				/*! �뺸�ؾ��� ������ ����				*/
	};

	const	float	gMeaninglessValue=-99999.0f;
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// Desc	: class����
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace WISM
{
	/*
	 *	@brief	WISMApi����ڿ� WISM Agent���� ����Data ���� ����
	 */
	class WISMAPI_XPORT	CNotificationNumber
	{
	public:	explicit CNotificationNumber(_In_ const int pSystemId=0, _In_ const int pActivityId=0, _In_ const float pValue=gMeaninglessValue, _In_ const float pMin=gMeaninglessValue, _In_ const float pMax=gMeaninglessValue) throw();

	public:	SYSTEMTIME	mRegisteredTime;					// ��Ͻð�(local time��)
	public:	int			mSystemId;
	public:	int			mActivityId;
	public:	float		mNumber[eNotificationNumberCount];	// gMeaninglessValue�� �ǹ̾��� ����.
	};

	/*
	 *	@brief	WISMApi����ڿ� WISM Agent���� ����Data ���� ����
	 */
	class WISMAPI_XPORT	CNotificationString
	{
	public:	explicit CNotificationString(_In_ const int pSystemId=0, _In_ const int pActivityId=0, _In_z_ const wchar_t* pString=L"") throw();
				
	public:	SYSTEMTIME	mRegisteredTime;					// ��Ͻð�(local time��)
	public:	int			mSystemId;
	public:	int			mActivityId;
	public:	wchar_t		mString[eMaxLengthOfString+1];
	};


	/*!
	 *	@brief	�ۺ��ſ��� �뺸�ϴ� abstract base class
	 *	@usage	ȣ������� Open()->Notify()�� �ݺ�->Close()��.
	 */
	class WISMAPI_XPORT CNotification
	{
	public:	CNotification() throw();
	public:	virtual ~CNotification() throw()=0;

	public:	_Check_return_	virtual unsigned int	Open(void) throw()=0;
	public:					virtual void			Close(void) throw()=0;
	public:	_Check_return_			bool			IsOpened(void) const throw();

	/*!
	 *	@brief	��(pValue) �Ǵ� ���ڿ�(pString)�� ������.
	 *	@return	�����ϸ� 0, �ܴ̿� �����ڵ��.
	 */
	public:	_Check_return_	virtual unsigned int	NotifyNumber(_In_ const CNotificationNumber& pNumber) throw()=0;
	public:	_Check_return_	virtual unsigned int	NotifyStringA(_In_ const int pSystemId, _In_ const int pActivityId,_In_z_ const char* pString) throw()=0;
	public:	_Check_return_	virtual unsigned int	NotifyStringW(_In_ const int pSystemId, _In_ const int pActivityId,_In_z_ const wchar_t* pString) throw()=0;
	public:	_Check_return_			unsigned int	NotifyHeartbeat(_In_ const int pSystemId) throw();
	public:	_Check_return_			unsigned int	NotifyConcurrentUserCount(_In_ const int pSystemId,  _In_ const float pValue) throw();

	/*!
	 *	@brief	���� �����´�.
	 *	@param	pString	���ڿ��� NULL�� ������ ��ŭ ����� ������ caller�� Ȯ���ؼ� �����ؾ� �Ѵ�.
	 *	@remark	overriding�Ҷ� base�� ȣ������ ���ƶ�.
	 */
	public:	_Check_return_	virtual unsigned int	GetNumber(_Out_ CNotificationNumber& pNumber) throw();
	public:	_Check_return_	virtual unsigned int	GetStringW(_Out_ CNotificationString& pString) throw();

	protected:bool			mIsOpened;
	};


	/*!
	 *	@brief	������ WISM���� �뺸��.
	 */
	class WISMAPI_XPORT CWISMNotification : public CNotification
	{
	private:typedef	CNotification	TSuper;
	public:					CWISMNotification() throw();
	public:	virtual			~CWISMNotification() throw();
	private:				CWISMNotification(_In_ const CWISMNotification&) throw()				{}
	private:_Check_return_	CWISMNotification&	operator=(_In_ const CWISMNotification&) throw()	{return(*this);}
	private:void			Ctor(void) throw();
	private:void			Dtor(void) throw();

	public:	_Check_return_	virtual unsigned int	Open(void) throw();
	public:					virtual void			Close(void) throw();
	

	/*!
	 *	@remark	queue�� ������ ��쿡�� ������ ���� �����ϰ� �ִ´�. ��, ������ ��ȯ�Ѵ�.
	 *			-> Close()�� �ʿ�� ����.
	 *			ȣ����� �� �μ��� ��ȿ���� ������ ���ؼ� �˻����� �ʴ´�.(�Ϲ������� ȣ���ڰ� Ȯ��������)
	 *			MBCS���ٴ� UNICODE�� �ӵ��� ����. �����ϸ� UNICODE�� ����ض�.
	 */
	public:	_Check_return_	virtual unsigned int	NotifyNumber(_In_ const CNotificationNumber& pNumber) throw();
	public:	_Check_return_	virtual unsigned int	NotifyStringA(_In_ const int pSystemId, _In_ const int pActivityId,_In_z_ const char* pString) throw();
	public:	_Check_return_	virtual unsigned int	NotifyStringW(_In_ const int pSystemId, _In_ const int pActivityId,_In_z_ const wchar_t* pString) throw();

	/*!
	 *	@remark	������ �߻��ϸ� Close()->Open()�� �ؾ� �Ѵ�.
	 *	@return	data�� ������ 0, ������ ERROR_EMPTY, �ܴ̿� �����ڵ�.
	 */
	public:	_Check_return_	virtual unsigned int	GetNumber(_Out_ CNotificationNumber& pNumber) throw();
	public:	_Check_return_	virtual unsigned int	GetStringW(_Out_ CNotificationString& pString) throw();
	};
};



namespace WISM
{
	inline	bool	CNotification::IsOpened(void) const
	{
		return(mIsOpened);
	}

	inline	unsigned int	CNotification::NotifyHeartbeat(const int pSystemId)
	{
		return(NotifyNumber(CNotificationNumber(pSystemId, eHeartbeatIdentification, 0)));
	}

	inline	unsigned int	CNotification::NotifyConcurrentUserCount(const int pSystemId, const float pValue)
	{
		return(NotifyNumber(CNotificationNumber(pSystemId, eConcurrentUserCountIdentification, pValue)));
	}
};