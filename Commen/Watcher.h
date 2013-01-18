//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "ZProcess.h"

//////////////////////////////////////////////////////////////////////////
//���̼�����
//���ã����ӽ���ռ�õ�ʱ����ڴ棬���������Χ����ǿ�ƽ�����
//ԭ������һ���̣߳�ÿ��һ��ʱ����һ�ν���ռ�õ�ʱ����ڴ档
//////////////////////////////////////////////////////////////////////////
class Watcher
{
public:
    Watcher(ZProcessBase* proc, uint64 time, uint64 memory);

    virtual ~Watcher();

    //�ȴ������߳̽�����
    void waitExit() {  threadWatcher = NULL; }

    bool isTimeOut() { return timeLimited; }

    bool isMemoryOut() { return memoryLimited; }

    int64 getRunMemory(){ return runMemory; }

    uint64 getRunTime(){ return runTime; }

    //�̹߳�������
    virtual void _doWatch();

protected:

    virtual bool updateTime();

    virtual bool updateMemory();

private:
    ZProcessBase*   process;    //�����ӵĽ���
    uint64          limitTime;  //���Ƶ�ʱ�� (��λ100ns)
    uint64          runTime;    //����ʱ�� (��λ100ns)
    uint64          limitMemory;//���Ƶ��ڴ� (��λbyte)
    uint64          runMemory;  //����ռ���ڴ� (��λbyte)
    bool            timeLimited;//�Ƿ�ʱ
    bool            memoryLimited;//�Ƿ񳬳��ڴ�
    ZThreadPtr      threadWatcher;//�����߳�
};

//��ý���ռ���ڴ�
uint64 getProcessMemory(HANDLE hprocess);

//��ý��̺ķѵ�ʱ��
uint64 getProcessTime(HANDLE hprocess);