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

//////////////////////////////////////////////////////////////////////////
//windows ��ҵ����
//////////////////////////////////////////////////////////////////////////
class ZJobObject :  public IBase
{
public:
    ZJobObject(void);

    virtual ~ZJobObject(void);

    HANDLE handle(){ return m_handle; }

    bool valid() const { return m_handle != 0; }

    void close();

    //������ҵ����
    bool create(const tstring & name, LPSECURITY_ATTRIBUTES lpJobAttributes = NULL);

    //�ȴ���ҵ����
    DWORD wait(DWORD time = INFINITE);

    //��ֹ��ҵ
    bool terminate(DWORD exitCode = 4);

    //������ҵ��Ϣ
    bool setInformation(JOBOBJECTINFOCLASS infoClass,
        LPVOID lpInfo,
        DWORD cbInfoLength);

    //������Ӧ�õ�job
    bool assinProcess(HANDLE handel);

protected:
    HANDLE m_handle;
};
