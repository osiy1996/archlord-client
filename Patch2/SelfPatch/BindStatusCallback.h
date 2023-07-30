
#include <windows.h>
#include "urlmon.h"
#include <string>

#define _KOR
//#define _CHN
//#define _ENG

#ifdef _KOR
#define STATUS_MSG_WAITOVER				"��� �ð��� �ʰ��Ǿ����ϴ�."
#define STATUS_MSG_DOWNLOADING			"�ٿ� �ε� ���Դϴ�."
#elif defined _CHN
#define STATUS_MSG_WAITOVER				"��� �ð��� �ʰ��Ǿ����ϴ�."
#define STATUS_MSG_DOWNLOADING			"�ٿ� �ε� ���Դϴ�."
#elif defined _ENG
#define STATUS_MSG_WAITOVER				"��� �ð��� �ʰ��Ǿ����ϴ�."
#define STATUS_MSG_DOWNLOADING			"�ٿ� �ε� ���Դϴ�."
#endif


///////////////////////////////////////////////////////////////////////
//. 2006. 4. 27. Nonstopdj
//. desc. ���ͳ� �ͽ��÷η��� �̿��� HTTP�� ������ �ٿ�ε�ޱ� ���� 
//.		 COM �������̽� Ŭ����. from devpia.com ^^
class CBindStatusCallback : public IBindStatusCallback
{
private:
    std::string m_URL;     ///< �ٿ�ε���� ������ URL
    DWORD       m_Timeout; ///< Ÿ�Ӿƿ�

public:
    /// \brief ������
    CBindStatusCallback(const std::string& url, DWORD timeout=INFINITE);

    /// \brief �Ҹ���
    ~CBindStatusCallback();

public:
    STDMETHOD(OnStartBinding)(
        /* [in] */ DWORD dwReserved,
        /* [in] */ IBinding __RPC_FAR *pib)
        { return E_NOTIMPL; }

    STDMETHOD(GetPriority)(
        /* [out] */ LONG __RPC_FAR *pnPriority)
        { return E_NOTIMPL; }

    STDMETHOD(OnLowResource)(
        /* [in] */ DWORD reserved)
        { return E_NOTIMPL; }

    STDMETHOD(OnProgress)(
        /* [in] */ ULONG ulProgress,
        /* [in] */ ULONG ulProgressMax,
        /* [in] */ ULONG ulStatusCode,
        /* [in] */ LPCWSTR wszStatusText);

    STDMETHOD(OnStopBinding)(
        /* [in] */ HRESULT hresult,
        /* [unique][in] */ LPCWSTR szError)
        { return E_NOTIMPL; }

    STDMETHOD(GetBindInfo)(
        /* [out] */ DWORD __RPC_FAR *grfBINDF,
        /* [unique][out][in] */ BINDINFO __RPC_FAR *pbindinfo)
        { return E_NOTIMPL; }

    STDMETHOD(OnDataAvailable)(
        /* [in] */ DWORD grfBSCF,
        /* [in] */ DWORD dwSize,
        /* [in] */ FORMATETC __RPC_FAR *pformatetc,
        /* [in] */ STGMEDIUM __RPC_FAR *pstgmed)
        { return E_NOTIMPL; }

    STDMETHOD(OnObjectAvailable)(
        /* [in] */ REFIID riid,
        /* [iid_is][in] */ IUnknown __RPC_FAR *punk)
        { return E_NOTIMPL; }

    STDMETHOD_(ULONG, AddRef)() { return 0; }
    STDMETHOD_(ULONG, Release)() { return 0; }

    STDMETHOD(QueryInterface)(
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject)
        { return E_NOTIMPL; }
};
