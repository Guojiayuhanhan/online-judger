//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////
#pragma once


#include "ZThread.h"
#include "TaskMgr.h"

//��������״̬
#define ZNET_NONE       0   //��״̬
#define ZNET_CREATE     1   //����socket
#define ZNET_BIND       2   //�󶨶˿�
#define ZNET_LISTEN     3   //����
#define ZNET_ACCEPT     4   //���տͻ�������
#define ZNET_CONNECT    5   //���ӷ�����

typedef TaskMgr<std::string> MessageMgr;
//////////////////////////////////////////////////////////////////////////
//�������
class ZNet : public IBase
{
public:
    ZNet(void);
    virtual ~ZNet(void);
};

//////////////////////////////////////////////////////////////////////////
//�ͻ��˻���
class ZClientBase
{
public:
    ZClientBase();

    virtual ~ZClientBase();

    //�ر�����
    virtual void close();

    //�������ݡ��̰߳�ȫ
    virtual void send(std::string msg);

    //���յ����ݡ�
    virtual void onReceivMsg(std::string msg);

    //��/���̳߳���
    virtual void onError(DWORD code);

    //�����߳�
    virtual void threadReceiv();

    //�����߳�
    virtual void threadSend();

public:

    SOCKET  socket(){ return m_socket; }

    int     port(){ return m_port; }

    std::string address(){ return m_address; }

    MessageMgr& getMsgMgr(){ return m_msgMgr;}

    virtual bool isRunning(){ return m_running; }

protected:
    bool        m_running; //�Ƿ���������
    SOCKET      m_socket;  //�׽���
    int         m_port;     //�˿�
    std::string m_address;  //ip��ַ
    MessageMgr  m_msgMgr;   //��Ϣ������

    ZThreadPtr  m_threadRead;   //�����߳�
    ZThreadPtr  m_threadWrite;  //�����߳�
};

//////////////////////////////////////////////////////////////////////////
//��������
class ZServer : public ZNet
{
public:
    //��Զ�̿ͻ��˶�Ӧ������
    class ZClientNode : public ZClientBase, public IBase
    {
    public:
        ZClientNode(ZServer* server, SOCKET sock, std::string addr, int port_);

        bool isRunning();

        virtual void onReceivMsg(std::string msg);

        int id(){ return m_id ; }
    private:
        int         m_id;
        ZServer*    m_server;
        static int  s_idAllocator;
    };

    typedef RefPtr<ZClientNode>         ZClientNodePtr;
    typedef std::list<ZClientNodePtr>   ClientPool;
    typedef ClientPool::iterator        ClientIterator;

public:
    ZServer() 
        : m_running(false)
        , m_status(ZNET_NONE)
    {}

    ~ZServer() {}

    //������������
    bool start(int port, int acceptMax);

    //�ر���������
    void close();

    //���һ���ͻ���
    void addClient(ZClientNodePtr node);

    //�Ƴ�һ���ͻ���
    void removeClient(ZClientNodePtr node);

    //��ѯ�ͻ���
    ZClientNodePtr getClient(int id);

    //��ͻ��˷�������
    virtual void send(int id, std::string msg);

    //���յ����Կͻ��˵���Ϣ
    //������Ҫ���ش˺�����������ܵ�����Ϣ��
    virtual void onReceivMsg(int id, std::string msg);

    //�������������
    void checkDead();

    //���������̺߳���
    void threadProcAccept();

public:
    SOCKET socket(){ return m_socket; }

    int port(){ return m_port; }

    int status(){ return m_status; }

    virtual bool isRunning(){ return m_running; }

protected:

    //����socket
    bool create();

    //�󶨶˿�
    bool bind(int port_);

    //����
    bool listen(int maxAmount);

    //��������
    bool accept();

protected:
    SOCKET          m_socket;
    int             m_port;
    int             m_status;
    bool            m_running;
    ZThreadPtr      m_threadAccept;
    ZSimpleLock     m_clientLock;
    ClientPool      m_pool;
};

//////////////////////////////////////////////////////////////////////////
//�ͻ���
class ZClient : public ZNet, public ZClientBase
{
public:
    ZClient() : m_status(0) {}

    virtual bool start(std::string addr, int port_);

    int status(){ return m_status; }

    virtual void close();
protected:
    bool create();

    bool connect(std::string addr, int port_);

protected:
    int m_status;
};