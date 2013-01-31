//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////
#include "ZProcess.h"
#include "Utilities.h"


#define BUFSIZE 1024

ZProcessBase::ZProcessBase()
    : m_hProcess(NULL)
    , m_hThread(NULL)
{
    
}

ZProcessBase::~ZProcessBase(void)
{
    SAFE_CLOSE_HANDLE(m_hProcess);
    SAFE_CLOSE_HANDLE(m_hThread);
}

DWORD ZProcessBase::waitExit(int time/* = INFINITE*/)
{
    if (isRunning())
    {
        WaitForSingleObject( m_hProcess, time ); 
    }
    return getExitCode();
}

void ZProcessBase::kill(DWORD exitCode/*=0*/)
{
    TerminateProcess(m_hProcess, exitCode);
}

DWORD ZProcessBase::getExitCode()
{
    DWORD code = 0;
    GetExitCodeProcess(m_hProcess, &code);
    return code;
}

bool ZProcessBase::isRunning()
{
    return getExitCode() == STILL_ACTIVE;
}

bool ZProcessBase::createProcess(LPTSTR szCMD,
    BOOL inheriteHandle, 
    DWORD creationFlag,
    STARTUPINFO & si, 
    PROCESS_INFORMATION & pi)
{
    return !!CreateProcess( NULL,   //   No module name (use command line).   
        szCMD,                      //   Command line.   
        NULL,                       //   Process handle not inheritable.   
        NULL,                       //   Thread handle not inheritable.   
        inheriteHandle,             //   Set handle inheritance to FALSE.   
        creationFlag,               //   No creation  flags.   
        NULL,                       //   Use parent 's environment block.   
        NULL,                       //   Use parent 's starting  directory.   
        &si,                        //   Pointer to STARTUPINFO structure. 
        &pi   );                    //   Pointer to PROCESS_INFORMATION structure.
}

//////////////////////////////////////////////////////////////////////////
bool ZProcess::create(const tstring &  cmd, bool start_)
{
    TCHAR szCMD[BUFSIZE];
    lstrcpy(szCMD, cmd.c_str());

    STARTUPINFO   si; 
    PROCESS_INFORMATION   pi; 

    ZeroMemory(&si, sizeof(si)); 
    si.cb   =   sizeof(si); 
    ZeroMemory(&pi, sizeof(pi)); 
  
    if(!createProcess(szCMD, FALSE, 0, si, pi))   
    { 
        return false;
    }

    m_hProcess = pi.hProcess;
    m_hThread = pi.hThread;

    if(start_)
    {
        start();
    }

    return true;
}

bool ZProcess::start()
{
    ResumeThread(m_hThread);
    return true;
}


//////////////////////////////////////////////////////////////////////////
ZProcessInOut::ZProcessInOut(const tstring &  inFile, const tstring &  outFile)
    : m_inputFile(inFile)
    , m_outputFile(outFile)
    , m_hInput(NULL)
    , m_hOutput(NULL)
{
}

ZProcessInOut::~ZProcessInOut()
{
    SAFE_CLOSE_HANDLE(m_hInput);
    SAFE_CLOSE_HANDLE(m_hOutput);
}

HANDLE ZProcessInOut::createInputFile()
{
    if (m_inputFile.empty())
    {
        return NULL;
    }
    SECURITY_ATTRIBUTES saAttr = {0};
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE; 
    saAttr.lpSecurityDescriptor = NULL;

    HANDLE handle = CreateFile(
        m_inputFile.c_str(),
        GENERIC_READ,          	
        FILE_SHARE_READ,       	
        &saAttr,                  
        OPEN_EXISTING,         	
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (!handle)
    {
        OutputMsg(_T("open file faild:%d"), GetLastError());
    }

    return handle;
}

HANDLE ZProcessInOut::createOutputFile()
{
    if (m_outputFile.empty())
    {
        return NULL;
    }

    SECURITY_ATTRIBUTES saAttr = {0};
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE; 
    saAttr.lpSecurityDescriptor = NULL;

    HANDLE handle = CreateFile(
        m_outputFile.c_str(),
        GENERIC_WRITE, 
        FILE_SHARE_READ,
        &saAttr,                   	
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (!handle)
    {
        OutputMsg(_T("create file faild:%d"), GetLastError());
    }

    return handle;
}

bool ZProcessInOut::create(const tstring & cmd, bool start_ /*= true*/)
{
    if (NULL != m_hProcess)
    {
        OutputMsg(_T("process has been created!"));
        return false;
    }

    TCHAR cmd_[BUFSIZE];
    lstrcpy(cmd_, cmd.c_str());

    m_hInput = createInputFile();
    m_hOutput = createOutputFile();

	/*CreateProcess�ĵ�һ������
	����׼����ʹ�������������ǽ�����ChildOut_Write��
	����׼���붨�����ǽ�����ChildIn_Read��
	�����ӽ��̽���StdIn�Լ�StdOut���ض���
	*/
	STARTUPINFO StartupInfo;
    ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	StartupInfo.cb = sizeof(STARTUPINFO);
	StartupInfo.hStdOutput = m_hOutput;
    StartupInfo.hStdError = m_hOutput;
	StartupInfo.hStdInput = m_hInput;
	StartupInfo.dwFlags = STARTF_USESTDHANDLES;

    PROCESS_INFORMATION ProcessInfo;
    ZeroMemory(&ProcessInfo, sizeof(ProcessInfo));
    
//    int time = GetTickCount();
    if(!createProcess(cmd_, TRUE, CREATE_SUSPENDED, StartupInfo, ProcessInfo))
    { 
        return false;
    }
//    time = GetTickCount() - time;
//    OutputMsg("create process use time:%d", time);

    m_hProcess = ProcessInfo.hProcess;
    m_hThread = ProcessInfo.hThread;

    if (start_)
    {
        start();
    }

    return true;
}

bool ZProcessInOut::start()
{
    ResumeThread(m_hThread);
    return true;
}


ZProcessDebug::ZProcessDebug(const tstring &  inFile, const tstring &  outFile)
    : ZProcessInOut(inFile, outFile)
{

}
ZProcessDebug::~ZProcessDebug()
{

}

bool ZProcessDebug::create(const tstring &  cmd, bool start_/* = true*/)
{
    if (NULL != m_hProcess)
    {
        OutputMsg(_T("process has been created!"));
        return false;
    }

    TCHAR cmd_[BUFSIZE];
    lstrcpy(cmd_, cmd.c_str());

    m_hInput = createInputFile();
    m_hOutput = createOutputFile();

	/*CreateProcess�ĵ�һ������
	����׼����ʹ�������������ǽ�����ChildOut_Write��
	����׼���붨�����ǽ�����ChildIn_Read��
	�����ӽ��̽���StdIn�Լ�StdOut���ض���
	*/
	STARTUPINFO StartupInfo;
    ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	StartupInfo.cb = sizeof(STARTUPINFO);
	StartupInfo.hStdOutput = m_hOutput;
    StartupInfo.hStdError = m_hOutput;
	StartupInfo.hStdInput = m_hInput;
	StartupInfo.dwFlags = STARTF_USESTDHANDLES;

    PROCESS_INFORMATION ProcessInfo;
    ZeroMemory(&ProcessInfo, sizeof(ProcessInfo));
    
//    int time = GetTickCount();
    if(!createProcess(cmd_, TRUE, CREATE_SUSPENDED | DEBUG_ONLY_THIS_PROCESS,
        StartupInfo, ProcessInfo))
    { 
        return false;
    }
//    time = GetTickCount() - time;
//    OutputMsg("create process use time:%d", time);

    m_hProcess = ProcessInfo.hProcess;
    m_hThread = ProcessInfo.hThread;

    if (start_)
    {
        start();
    }

    return true;
}

bool ZProcessDebug::start()
{
//    OutputMsgA("start run");
    ResumeThread(m_hThread);

    SAFE_CLOSE_HANDLE(m_hInput);
    SAFE_CLOSE_HANDLE(m_hOutput);
    SAFE_CLOSE_HANDLE(m_hThread);

    DEBUG_EVENT de;
    ZeroMemory(&de, sizeof(de));
    DWORD dwContinueStatus = DBG_CONTINUE;

    while (WaitForDebugEvent(&de, INFINITE)) 
    {
        if (de.dwDebugEventCode == CREATE_PROCESS_DEBUG_EVENT)
        {
            //ǿ�ƹر�exe�ļ�
            SAFE_CLOSE_HANDLE(de.u.CreateProcessInfo.hFile);
        }
        else if (de.dwDebugEventCode == EXCEPTION_DEBUG_EVENT) 
        {
            if (de.u.Exception.ExceptionRecord.ExceptionCode == EXCEPTION_BREAKPOINT)
            {
            }
            else
            {
                switch(de.u.Exception.ExceptionRecord.ExceptionCode) 
                { 
                case EXCEPTION_INT_DIVIDE_BY_ZERO: //���������ĳ�����0ʱ�������쳣��
                    OutputMsgA("INT_DIVIDE_BY_ZERO");
                    break;

                case EXCEPTION_INT_OVERFLOW://���������Ľ�����ʱ�������쳣��
                    OutputMsgA("INT_OVERFLOW");
                    break;

                case EXCEPTION_ACCESS_VIOLATION: //������ͼ��дһ�����ɷ��ʵĵ�ַʱ�������쳣��������ͼ��ȡ0��ַ�����ڴ档
                    OutputMsgA("ACCESS_VIOLATION");
                    break;

                case EXCEPTION_DATATYPE_MISALIGNMENT://�����ȡһ��δ�����������ʱ�������쳣��
                    OutputMsgA("DATATYPE_MISALIGNMENT");
                    break;

                case EXCEPTION_FLT_STACK_CHECK: //���и���������ʱջ�������������ʱ�������쳣��
                    OutputMsgA("FLT_STACK_CHECK");
                    break;

                case EXCEPTION_INVALID_DISPOSITION: //�쳣����������һ����Ч�Ĵ����ʱ�������쳣��
                    OutputMsgA("INVALID_DISPOSITION");
                    break;

                case EXCEPTION_STACK_OVERFLOW: //ջ���ʱ�������쳣��
                    OutputMsgA("STACK_OVERFLOW");
                    break;

                default:
                    OutputMsgA("UNKNOW_EXCEPTION");
                    break;
                } 

                if (de.u.Exception.dwFirstChance)
                {
                    OutputMsgA("exception at 0x%08x, exception-code: 0x%08x",
                        de.u.Exception.ExceptionRecord.ExceptionAddress,
                        de.u.Exception.ExceptionRecord.ExceptionCode);
                }
                dwContinueStatus = DBG_EXCEPTION_NOT_HANDLED;
            }
        }
        else if(de.dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT) 
        {
            //OutputMsgA("progress exit with code:%u��%u", de.u.ExitProcess.dwExitCode, GetLastError());

            DebugSetProcessKillOnExit(TRUE);
            DebugActiveProcessStop(de.dwProcessId);


            if (de.u.ExitProcess.dwExitCode != 0)
            {
                return false;
            }
            break;
        }

        ContinueDebugEvent(de.dwProcessId, de.dwThreadId, dwContinueStatus); 
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

ZProcessJob::ZProcessJob(const tstring &  inFile, const tstring &  outFile,
                         int timeMS, int memoryKB)
    : ZProcessInOut(inFile, outFile)
    , m_ioCPHandle(NULL)
    , m_exitCode(0)
    , m_runTime(0)
    , m_runMemory(0)
    , m_limitTime(timeMS)
    , m_limitMemory(memoryKB)
{

}

ZProcessJob::~ZProcessJob()
{
    SAFE_CLOSE_HANDLE(m_ioCPHandle);
}

bool ZProcessJob::create(const tstring &  cmd, bool start_/* = true*/)
{
    if (NULL != m_hProcess)
    {
        OutputMsg(_T("process has been created!"));
        return false;
    }

    int64 limitTime = m_limitTime * 10000; //100ns (1s = 10^9ns)
    int limitMemory = m_limitMemory * 1024; //bytes
    if (limitMemory < 0)//����int��Χ��
    {
        limitMemory = 128*1024*1024; //Ĭ��128M
    }

    //////////////////////////////////////////////////////////////////////////
    //������ҵɳ�䣨job��
    //////////////////////////////////////////////////////////////////////////
    
    tstring jobName;
    generateGUID(jobName);
    if(!m_job.create(jobName))
    {
        OutputMsg(_T("create job faild!"));
        return false;
    }

    //////////////////////////////////////////////////////////////////////////
    //����job��Ϣ
    //////////////////////////////////////////////////////////////////////////

    //���û���������Ϣ
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION subProcessLimitRes;
    ZeroMemory(&subProcessLimitRes, sizeof(subProcessLimitRes));

    JOBOBJECT_BASIC_LIMIT_INFORMATION & basicInfo = subProcessLimitRes.BasicLimitInformation;
    basicInfo.LimitFlags = JOB_OBJECT_LIMIT_PROCESS_TIME| \
        JOB_OBJECT_LIMIT_PRIORITY_CLASS| \
        JOB_OBJECT_LIMIT_PROCESS_MEMORY| \
        JOB_OBJECT_LIMIT_DIE_ON_UNHANDLED_EXCEPTION;
    basicInfo.PriorityClass = NORMAL_PRIORITY_CLASS;      //���ȼ�ΪĬ��
    basicInfo.PerProcessUserTimeLimit.QuadPart = limitTime; //�ӽ���ִ��ʱ��ns(1s=10^9ns)
    subProcessLimitRes.ProcessMemoryLimit = limitMemory;    //�ڴ�����

    m_job.setInformation(
        JobObjectExtendedLimitInformation,
        &subProcessLimitRes, 
        sizeof(subProcessLimitRes));


    //����ɶ˿ڷ���ʱ�����Ƶ���Ϣ
    JOBOBJECT_END_OF_JOB_TIME_INFORMATION timeReport;
    ZeroMemory(&timeReport, sizeof(timeReport));

    timeReport.EndOfJobTimeAction = JOB_OBJECT_POST_AT_END_OF_JOB;

    m_job.setInformation(
        JobObjectEndOfJobTimeInformation, 
        &timeReport,
        sizeof(JOBOBJECT_END_OF_JOB_TIME_INFORMATION));


    //UI����
    JOBOBJECT_BASIC_UI_RESTRICTIONS subProcessLimitUi;
    ZeroMemory(&subProcessLimitUi, sizeof(subProcessLimitUi));

    subProcessLimitUi.UIRestrictionsClass = JOB_OBJECT_UILIMIT_NONE| \
        JOB_OBJECT_UILIMIT_DESKTOP| \
        JOB_OBJECT_UILIMIT_SYSTEMPARAMETERS| \
        JOB_OBJECT_UILIMIT_DISPLAYSETTINGS| \
        JOB_OBJECT_UILIMIT_EXITWINDOWS| \
        JOB_OBJECT_UILIMIT_GLOBALATOMS| \
        JOB_OBJECT_UILIMIT_HANDLES| \
        JOB_OBJECT_UILIMIT_READCLIPBOARD;

    m_job.setInformation(
        JobObjectBasicUIRestrictions,
        &subProcessLimitUi,
        sizeof(subProcessLimitUi));


    //����ҵ��������ɶ˿ڣ���ȷ����������������˳���ԭ��
    int id = generateID();
    m_ioCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, id, 0);

    JOBOBJECT_ASSOCIATE_COMPLETION_PORT jobCP;
    ZeroMemory(&jobCP, sizeof(jobCP));

    jobCP.CompletionKey = (PVOID)id;
    jobCP.CompletionPort = m_ioCPHandle;

    m_job.setInformation(
        JobObjectAssociateCompletionPortInformation,
        &jobCP,
        sizeof(jobCP));

    //////////////////////////////////////////////////////////////////////////
    //�����ӽ���
    //////////////////////////////////////////////////////////////////////////

    TCHAR cmd_[BUFSIZE];
    lstrcpy(cmd_, cmd.c_str());

    m_hInput = createInputFile();
    m_hOutput = createOutputFile();

	/*CreateProcess�ĵ�һ������
	����׼����ʹ�������������ǽ�����m_hOutput��
	����׼���붨�����ǽ�����m_hInput��
	�����ӽ��̽���StdIn�Լ�StdOut���ض���
	*/
	STARTUPINFO StartupInfo;
    ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	StartupInfo.cb = sizeof(STARTUPINFO);
	StartupInfo.hStdOutput = m_hOutput;
    StartupInfo.hStdError = m_hOutput;
	StartupInfo.hStdInput = m_hInput;
	StartupInfo.dwFlags = STARTF_USESTDHANDLES;

    PROCESS_INFORMATION ProcessInfo;
    ZeroMemory(&ProcessInfo, sizeof(ProcessInfo));
    
    if(!createProcess(cmd_, TRUE, CREATE_SUSPENDED | CREATE_BREAKAWAY_FROM_JOB,
        StartupInfo, ProcessInfo))
    { 
        return false;
    }

    m_hProcess = ProcessInfo.hProcess;
    m_hThread = ProcessInfo.hThread;

    //////////////////////////////////////////////////////////////////////////
    //���ӽ�����job����
    //////////////////////////////////////////////////////////////////////////

    if (start_)
    {
        start();
    }

    return true;
}

bool ZProcessJob::start()
{
    OutputMsgA("start run.");

    if(!m_job.assinProcess(m_hProcess))
    {
        OutputMsg(_T("Ӧ�ý��̵�jobʧ��!%d"), GetLastError());
        return false;
    }

    //�����ӽ���
    ResumeThread(m_hThread);
    
    //�رձ�׼�����ļ�����ʱ����ļ��ľ��
    SAFE_CLOSE_HANDLE(m_hInput);
    SAFE_CLOSE_HANDLE(m_hOutput);

    //�ر����������߳̾��
    SAFE_CLOSE_HANDLE(m_hThread);

    //�ȴ������ӽ��̴�����ϻ�ľ���Դ�˳�

    DWORD ExecuteResult = -1;
    unsigned long completeKey;
    LPOVERLAPPED processInfo;
    bool done = false;
    while(!done)
    {
        GetQueuedCompletionStatus(
            m_ioCPHandle,
            &ExecuteResult, 
            &completeKey, 
            &processInfo, 
            INFINITE);

        switch (ExecuteResult) 
        {
        case JOB_OBJECT_MSG_NEW_PROCESS: 
            {
                OutputMsg(TEXT("New process (Id=%d) in Job"), processInfo);
            }
            break;

        case JOB_OBJECT_MSG_END_OF_JOB_TIME:
            {
                OutputMsg(TEXT("Job time limit reached"));
                m_exitCode = 1;
                done = true;
            }
            break;

        case JOB_OBJECT_MSG_END_OF_PROCESS_TIME: 
            {
                OutputMsg(TEXT("Job process (Id=%d) time limit reached"), processInfo);
                m_exitCode = 1;
                done = true;
            }
            break;

        case JOB_OBJECT_MSG_PROCESS_MEMORY_LIMIT: 
            {
                OutputMsg(TEXT("Process (Id=%d) exceeded memory limit"), processInfo);
                m_exitCode = 2;
                done = true;
            }
            break;

        case JOB_OBJECT_MSG_JOB_MEMORY_LIMIT: 
            {
                OutputMsg(TEXT("Process (Id=%d) exceeded job memory limit"), processInfo);
                m_exitCode = 2;
                done = true;
            }
            break;

        case JOB_OBJECT_MSG_ACTIVE_PROCESS_LIMIT:
            {
                OutputMsg(TEXT("Too many active processes in job"));
            }
            break;

        case JOB_OBJECT_MSG_ACTIVE_PROCESS_ZERO:
            {
                OutputMsg(TEXT("Job contains no active processes"));
                done = true;
            }
            break;

        case JOB_OBJECT_MSG_EXIT_PROCESS: 
            {
                OutputMsg(TEXT("Process (Id=%d) terminated"), processInfo);
                done = true;
            }
            break;

        case JOB_OBJECT_MSG_ABNORMAL_EXIT_PROCESS: 
            {
                OutputMsg(TEXT("Process (Id=%d) terminated abnormally"), processInfo);
                m_exitCode = 3;
                done = true;
            }
            break;

        default:
            OutputMsg(TEXT("Unknown notification: %d"), ExecuteResult);
            m_exitCode = 99;
            break;
        }
    }

    JOBOBJECT_BASIC_AND_IO_ACCOUNTING_INFORMATION jobai;
    ZeroMemory(&jobai, sizeof(jobai));
    QueryInformationJobObject(m_job.handle(), JobObjectBasicAndIoAccountingInformation, 
        &jobai, sizeof(jobai), NULL);
#if 0
    OutputMsg(_T("total user time : %dms"), jobai.BasicInfo.TotalUserTime.LowPart/10000);
    OutputMsg(_T("total kernel time : %dms"), jobai.BasicInfo.TotalKernelTime.LowPart/10000);
    OutputMsg(_T("Period user time : %dms"), jobai.BasicInfo.ThisPeriodTotalUserTime.LowPart/10000);
    OutputMsg(_T("Period kernel time : %dms"), jobai.BasicInfo.ThisPeriodTotalKernelTime.LowPart/10000);
#endif

    JOBOBJECT_EXTENDED_LIMIT_INFORMATION joeli;
    ZeroMemory(&joeli, sizeof(joeli));
    QueryInformationJobObject(m_job.handle(), JobObjectExtendedLimitInformation, 
        &joeli, sizeof(joeli), NULL);

#if 0
    OutputMsg(_T("peek process memory : %dk"), joeli.PeakProcessMemoryUsed/1024);
    OutputMsg(_T("peek job memory : %dk"), joeli.PeakJobMemoryUsed/1024);
#endif

    m_runTime = jobai.BasicInfo.TotalUserTime.LowPart/10000;
    m_runMemory = joeli.PeakProcessMemoryUsed/1024;

    //�رս��̾��
    SAFE_CLOSE_HANDLE(m_hProcess);

    //�ر���ɶ˿�
    SAFE_CLOSE_HANDLE(m_ioCPHandle);

    //Ϊ�˰�ȫ��ɱ����ҵ�����н���
    while(!m_job.terminate(0))
    {
        OutputMsg(_T("ֹͣjobʧ��!%d"), GetLastError());
        Sleep(1000);
    }
    //�ر���ҵ���
    m_job.close();

    OutputMsgA("end run.");
    return true;
}