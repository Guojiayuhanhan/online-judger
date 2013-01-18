//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "RefPtr.h"
#include "ZLock.h"

//////////////////////////////////////////////////////////////////////////
//�̻߳���
class ZThreadBase : public IBase
{
public:
    ZThreadBase()
    {
        unsigned int threadID = 0;
        m_hThread = (HANDLE)_beginthreadex(NULL, 0, _threadCall, this, 0, &threadID);
    }

    ~ZThreadBase(void)
    {
        WaitForSingleObject(m_hThread, INFINITE);
        CloseHandle(m_hThread);
    }

    virtual void callFunction() = 0;

private:
    HANDLE          m_hThread;

    static unsigned int CALLBACK _threadCall(void* param)
    {
        ZThreadBase *pCall = (ZThreadBase*)param;
        pCall->callFunction();
        return 0;
    }
};

typedef RefPtr<ZThreadBase> ZThreadPtr;

typedef void(*ZThreadFunc)(void*);

//��ͨ�߳���
class ZThread : public ZThreadBase
{
public:
    ZThread(ZThreadFunc pf, void *param)
        : m_pFunc(pf)
        , m_pParam(param)
    {
        assert(pf && "����ָ�벻��Ϊ�գ�");
    }

    void callFunction()
    {
        m_pFunc(m_pParam);
    }

private:
    ZThreadFunc m_pFunc;
    void*       m_pParam;
};

//��ǿ�߳���.���ڵ������Ա������
template<typename T>
class ZThreadEx : public ZThreadBase
{
public:
    typedef void(T::*ZFuncPtr)(void);

    ZThreadEx(T *pobj, ZFuncPtr func) 
        : m_obj(pobj)
        , m_func(func) 
    {
        assert(pobj && func && "thisָ��ͺ�����ַ������Ϊ�գ�");
    }

    virtual void callFunction()
    {
        (m_obj->*m_func)();
    }

private:
    T           *m_obj;
    ZFuncPtr     m_func;
};