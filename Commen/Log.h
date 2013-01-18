//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "Comm.h"
#include "ZLock.h"

/**��־ϵͳ*/
class cLog
{
public:

	cLog(void);

	virtual ~cLog(void);

    bool init(const tstring & fileName);

    void unload(void);

    bool isUsefull(void){ return m_bUsefull; }

    void writeA(const std::string & buffer);

    void writeW(const std::wstring & buffer);

    void xwriteA(LPCSTR format, ...);

    void xwriteW(LPCWSTR format, ...);

protected:

    void generateTimeHead(tstring & header);

    void write(const tstring & buffer);

private:
    tofstream	    m_fLog;		//����ļ���
	bool			m_bUsefull;	//��־ϵͳ�Ƿ����
	SYSTEMTIME		m_systm;

    ZMutex          m_writeMutex;
    ZMutex          m_formatWriteMutex;
};

extern cLog g_globle_log_;

/**��ʼ��־ϵͳ��*/
#define  INIT_LOG(fileName)       g_globle_log_.init(fileName)

/**д��־*/
#define  WRITE_LOGA(buffer)       g_globle_log_.writeA(buffer)

/**д��־*/
#define  WRITE_LOGW(buffer)       g_globle_log_.writeW(buffer)

/**д��־*/
#define  XWRITE_LOGA(format, ...)   g_globle_log_.xwriteA(format, __VA_ARGS__)

/**д��־*/
#define  XWRITE_LOGW(format, ...)   g_globle_log_.xwriteW(format, __VA_ARGS__)

/**ж����־ϵͳ*/
#define  UNLOAD_LOG()               g_globle_log_.unload()

#ifdef _UNICODE
#   define XWRITE_LOG   XWRITE_LOGW
#   define WRITE_LOG    WRITE_LOGW
#else
#   define XWRITE_LOG   XWRITE_LOGA
#   define WRITE_LOG    WRITE_LOGA
#endif
