#include "StdAfx.h"
#include ".\bindstatuscallback.h"
#include <shlwapi.h>
#include <mmsystem.h>
#include <iostream>
#include <assert.h>

//////////////////////////////////////////////////////////////////////////////
//. \brief ������
//. \param url �ٿ�ε���� ������ URL
//. \param timeout Ÿ�Ӿƿ�
CBindStatusCallback::CBindStatusCallback(const std::string& url, DWORD timeout)
: m_URL(url), m_Timeout(timeout)
{
    assert(!m_URL.empty());
}

CBindStatusCallback::~CBindStatusCallback()
{	
}

//////////////////////////////////////////////////////////////////////////////
//. \brief ���� ��Ȳ�� �˷��ֱ� ���� ���ͳ� �ͽ��÷η��� ȣ���ϰ� �Ǵ�
//. �ݹ� �Լ�
//.
//. \param ulProgress �ٿ�ε���� ����Ʈ ��
//. \param ulProgressMax �ٿ�ε�޾ƾ��� ��ü ����Ʈ ��
//. \param ulStatusCode ���� �ڵ�
//. \param wszStatusText ���� ���ڿ�
//. \return HRESULT ���ͳ� �ͽ��÷η����� �˷�����ϴ� �����. �ٿ�ε带
//. ����ؾ��ϴ� ��쿡�� S_OK�� ��ȯ�ؾ��ϰ�, �ߴ��Ϸ��� E_ABORT�� ��ȯ�ؾ� 
//. �Ѵ�.
HRESULT STDMETHODCALLTYPE CBindStatusCallback::OnProgress(ULONG ulProgress, 
                                                ULONG ulProgressMax,
                                                ULONG ulStatusCode, 
                                                LPCWSTR wszStatusText)
{
    //. �� �Լ� ���ο��� Ȥ�� ����ڰ� ���� ��ư�� �����ٴ���, 
    //. Ÿ�Ӿƿ��� �Ǿ��ٴ���, ulStatusCode�� �̻��ϴ��� �ϸ�,
    //. E_ABORT�� ��ȯ�ؼ�, �ٿ�ε带 �ߴ��ؾ� �Ѵ�.
    if (m_Timeout < timeGetTime())
    {
		std::cout << STATUS_MSG_WAITOVER << std::endl;
        return E_ABORT;
    }
    
    //. �ٿ�ε带 �����ؾ��ϴ� ��Ȳ�� �ƴ϶��, 
    //. ulProgress ����, ulProgressMax ���� �̿��� 
    //. ���α׷����� ���� ������ ������Ʈ�ϸ� �ȴ�.
    if (0 != ulProgressMax)
    {
        std::cout << STATUS_MSG_DOWNLOADING << m_URL 
            << "(" << int( 100.0 * ulProgress / ulProgressMax ) << "%)" << std::endl;
    }
    else
    {
        std::cout << STATUS_MSG_DOWNLOADING << m_URL << std::endl;
    }

    return S_OK;
}