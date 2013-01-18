//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////

#include "Comm.h"
#include "Utilities.h"
#include "ZNet.h"

#pragma comment(lib,"Ws2_32.lib")


//////////////////////////////////////////////////////////////////////////
ZNet::ZNet(void)
{
    WSADATA  wsaData;  
    WSAStartup(MAKEWORD(2,2), &wsaData); //����socket����
}

ZNet::~ZNet(void)
{
    WSACleanup();//���socket����
}
//////////////////////////////////////////////////////////////////////////
ZClientBase::ZClientBase()
    : m_socket(0)
    , m_address("")
    , m_port(0)
    , m_running(false)
{
}

ZClientBase::~ZClientBase()
{
    close();
    m_threadRead = NULL;
    m_threadWrite = NULL;
}

void ZClientBase::close()
{
    if (!m_socket)
    {
        return;
    }
    m_running = false;
    send("$exit");
    ::closesocket(m_socket);
    m_socket = 0;
}

void ZClientBase::send(std::string msg)
{
    m_msgMgr.add(msg);
}

void ZClientBase::onReceivMsg(std::string msg)
{

}

void ZClientBase::onError(DWORD code)
{
    m_running = false;
    close();
}

void ZClientBase::threadReceiv()
{
    char head[32];
    while(isRunning())
    {
        if(::recv(m_socket, head, 5, 0) == SOCKET_ERROR)
        {
            std::cout<<"���ݽ���ʧ��!"<<std::endl;
            onError(GetLastError());
            break;
        }

        if (head[0] != '@')
        {
            std::cout<<"���յ��������ݣ�"<<std::endl;
        }
        else
        {
            head[6] = 0;
            int len = atoi(head + 1);
            std::cout<<"���յ����ݳ��ȣ�"<<len<<std::endl;
            char *buffer = new char[len+1];
            if(::recv(m_socket, buffer, len, 0) == SOCKET_ERROR)
            {
                delete buffer;
                std::cout<<"���ݽ���ʧ��!"<<std::endl;
                onError(GetLastError());
                break;
            }
            else
            {
                buffer[len] = 0;
                onReceivMsg(buffer);
                delete buffer;
            }
        }
    }
}

void ZClientBase::threadSend()
{
    char head[32];
    while(isRunning())
    {
        std::string msg = m_msgMgr.get();
        if (msg.empty())
        {
            continue;
        }
        if (msg == "$exit")
        {
            break;
        }
        int len = msg.length();
        sprintf_s(head, 32, "@%4.4d", len);
        msg = head + msg;
        std::cout<<"��������:"<<msg<<std::endl;
        if(::send(m_socket, msg.c_str(), len+5, 0) == SOCKET_ERROR)
        {
            std::cout<<"���ݷ���ʧ��!"<<std::endl;
            onError(GetLastError());
            break;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
/*static*/ int ZServer::ZClientNode::s_idAllocator = 100;

ZServer::ZClientNode::ZClientNode(ZServer* server, SOCKET sock, std::string addr, int port_)
    : m_server(server)
   
    , m_id(++s_idAllocator)
{ 
    m_socket = sock;
    m_address = addr;
    m_port = port_;
    m_running = true;
    m_threadRead = new ZThreadEx<ZClientNode>(this, &ZClientNode::threadReceiv);
    m_threadWrite = new ZThreadEx<ZClientNode>(this, &ZClientNode::threadSend);
}

void ZServer::ZClientNode::onReceivMsg(std::string msg)
{
    m_server->onReceivMsg(m_id, msg);
}

bool ZServer::ZClientNode::isRunning()
{ 
    return m_running && m_server->isRunning(); 
}
//////////////////////////////////////////////////////////////////////////

void ZServer::threadProcAccept()
{
    sockaddr_in  address;
    int len = 0;
    while(isRunning())
    {
        memset(&address, 0, sizeof(address));
        len = sizeof(address);

        SOCKET socket = ::accept(m_socket, (sockaddr*)&address, &len);    
        if(socket == SOCKET_ERROR)
        {
            printf("����������ʧ�ܣ�%d\n", GetLastError());
            continue;
        }

        printf("���յ�����[%s:%d]������-socket[%d]\n",
            inet_ntoa(address.sin_addr),
            htons(address.sin_port),
            socket);

        ZClientNodePtr ptr = new ZClientNode(this, socket, inet_ntoa(address.sin_addr), address.sin_port);
        addClient(ptr);       
        std::cout<<"�������ӣ�"<<ptr->id()<<std::endl;

        checkDead();
    }

}

bool removeDead(ZServer::ZClientNodePtr ptr)
{
    return !ptr->isRunning();
}

void ZServer::checkDead()
{
    m_pool.remove_if(removeDead);
}

bool ZServer::start(int port, int acceptMax)
{
    if(!create())
    {
        return false;
    }
    if(!bind(port))
    {
        return false;
    }
    if(!listen(acceptMax))
    {
        return false;
    }
    if(!accept())
    {
        return false;
    }
    return true;
}

bool ZServer::create()
{
    m_status = ZNET_CREATE;
    m_socket = ::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if (NULL == m_socket)
    {
        m_status = -ZNET_CREATE;
    }
    return m_status == ZNET_CREATE;
}

bool ZServer::bind(int port_)
{
    if (m_status != ZNET_CREATE)
    {
        return false;
    }
    m_status = ZNET_BIND;
    m_port = port_;
    sockaddr_in  service;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = INADDR_ANY;//inet_addr(szIP); 
    service.sin_port = htons(m_port);
    if(::bind(m_socket,(SOCKADDR*)&service, sizeof(service))==SOCKET_ERROR)
    {
        printf("�󶨶˿�ʧ�ܣ�%d\n", GetLastError());
        m_status = -ZNET_BIND;
    }
    return m_status == ZNET_BIND;
}

bool ZServer::listen(int maxAmount)
{
    if (m_status != ZNET_BIND)
    {
        return false;
    }
    m_status = ZNET_LISTEN;
    if(::listen(m_socket,20)==SOCKET_ERROR)
    {
        m_status = -ZNET_LISTEN;
    }
    return m_status == ZNET_LISTEN;
}

bool ZServer::accept()
{
    if (m_status != ZNET_LISTEN)
    {
        return false;
    }
    m_running = true;
    m_status = ZNET_ACCEPT;
    m_threadAccept = new ZThreadEx<ZServer>(this, &ZServer::threadProcAccept);
    return m_status == ZNET_ACCEPT;
}

void ZServer::close()
{
    m_running = false;
    m_status = ZNET_NONE;
    if (m_socket)
    {
        ::closesocket(m_socket);
    }
    for(ClientIterator it=m_pool.begin(); it!=m_pool.end(); ++it)
    {
        (*it)->close();
    }
    m_threadAccept = NULL;
}

void ZServer::addClient(ZClientNodePtr node)
{
    ZLockHolder hoder(&m_clientLock);
    m_pool.push_back(node);
}

void ZServer::removeClient(ZClientNodePtr node)
{
    ZLockHolder hoder(&m_clientLock);
    m_pool.remove(node);
}

ZServer::ZClientNodePtr ZServer::getClient(int id)
{
    ZLockHolder hoder(&m_clientLock);
    for (ClientIterator it = m_pool.begin(); it!=m_pool.end(); ++it)
    {
        if ((*it)->id() == id)
        {
            return (*it);
        }
    }
    return NULL;
}

void ZServer::send(int id, std::string msg)
{
    ZClientNodePtr ptr = getClient(id);
    if (ptr)
    {
        ptr->send(msg);
    }
    else
    {
        std::cout<<"error:"<<id<<" not found"<<std::endl;
    }
}

void ZServer::onReceivMsg(int id, std::string msg)
{
    std::cout<<"���ܵ����ݣ�"<<id <<" - "<< msg << std::endl;
}

//////////////////////////////////////////////////////////////////////////
bool ZClient::start(std::string addr, int port_)
{
    if (!create())
    {
        return false;
    }
    if (!connect(addr, port_))
    {
        return false;
    }
    return true;
}

bool ZClient::create()
{
    m_status = ZNET_CREATE;
    m_socket = ::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if (NULL == m_socket)
    {
        m_status = -ZNET_CREATE;
    }
    return m_status == ZNET_CREATE;
}

bool ZClient::connect(std::string addr, int port_)
{
    m_address = addr;
    m_port = port_;
    if(m_status < 0)
    {
        return false;
    }
    m_status = ZNET_CONNECT;
    sockaddr_in  service;  
    service.sin_family = AF_INET;  
    service.sin_addr.s_addr = inet_addr(m_address.c_str());  
    service.sin_port = htons(m_port);
    if(::connect(m_socket, (SOCKADDR*)&service, sizeof(service))==SOCKET_ERROR)
    {
        m_status = -ZNET_CONNECT;
    }
    else
    {
        m_running = true;
        m_threadRead = new ZThreadEx<ZClient>(this, &ZClient::threadReceiv);
        m_threadWrite = new ZThreadEx<ZClient>(this, &ZClient::threadSend);
    }
    return m_status == ZNET_CONNECT;
}

void ZClient::close()
{
    m_status = ZNET_NONE;
    __super::close();
}
