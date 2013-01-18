//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////
#pragma once

#include <Windows.h>
#include <process.h>
#include <assert.h>

struct ZLockBase
{
    virtual void lock() = 0;
    virtual void unlock() = 0;
};

//�ٽ���
class ZCritical : public ZLockBase
{
public:
    ZCritical(){ InitializeCriticalSection(&m_cs);}

    ~ZCritical(){ DeleteCriticalSection(&m_cs); }

    void lock(){ EnterCriticalSection(&m_cs); }

    void unlock(){ LeaveCriticalSection(&m_cs); }

private:
    CRITICAL_SECTION m_cs;
};

//����
class ZSimpleLock : public ZLockBase
{
public:
    ZSimpleLock() : m_mark(0) {}
    void lock()
    {
        while(InterlockedExchange(&m_mark, 1) == 1)
        {
            Sleep(0);
        }
    }
    void unlock(){ InterlockedExchange(&m_mark, 0); }

private:
    LONG m_mark;
};

//������
class ZMutex : public ZLockBase
{
public:
    ZMutex(void) : m_hMutex(CreateMutex(NULL, FALSE, NULL)) {}

    ~ZMutex(void) {  CloseHandle(m_hMutex);}

    void lock(){ WaitForSingleObject(m_hMutex, INFINITE);}

    void unlock(){ ReleaseMutex(m_hMutex); }
private:
    HANDLE m_hMutex;
};

//�������ߡ��ڶ�����ʱ������������ʱ�������
class ZLockHolder
{
public:
    ZLockHolder(ZLockBase *locker) 
        : m_locker(locker) 
    {
        m_locker->lock();
    }
    ~ZLockHolder(){ m_locker->unlock(); }
private:
    ZLockBase *m_locker;
};

//////////////////////////////////////////////////////////////////////////
//�ź���
class ZSemaphore
{
public:
    ZSemaphore() : m_hSemaphore(CreateSemaphore(NULL, 0, 32767, NULL)) {}
    ~ZSemaphore(){ CloseHandle(m_hSemaphore); }

    void push(){ ReleaseSemaphore(m_hSemaphore, 1, NULL); }

    void pop(){ WaitForSingleObject(m_hSemaphore, INFINITE); }

private:
    HANDLE m_hSemaphore;
};