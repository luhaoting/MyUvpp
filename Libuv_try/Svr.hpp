#pragma once

#include <list>
#include <map>
#include <memory>
#include <iostream>
#include <thread>

#include "tcp.hpp"
#include "loop.hpp"

using ID = int64_t;

using namespace uv;
using namespace std;

class Gate final
{
public:
    Gate();
    ~Gate();
    
    //��ֹ����
    Gate(const Gate&) = delete;
    Gate& operator = (const Gate&) = delete;

    bool listen(const int nPort);
    void sendTo(int clientId, const char* buff, size_t len) {};//todo
private:
    void on_new_connection(uv::Error error);
    void on_recv(Tcp client, std::function<void(string msg)> clientRecvCB) {};//todo
private:
    unique_ptr<Loop> m_base;
    unique_ptr<Tcp> m_listener;

    std::map<int, unique_ptr<Tcp> > m_active_client;
};


const std::string g_strIp = "127.0.0.1";
const int g_nPort = 49527;
std::ostream& G_LOG();

//////////////////////////////////////////////////////////////////////////
//�������ʵ���� ���̳�ʵ�ֲ�ͬ�� write �� read
//////////////////////////////////////////////////////////////////////////
class ClientCtx
{
    typedef std::function<void(const char* buf, size_t len)> do_write_t;
    typedef std::function<void(const char* buf, size_t len)> do_read_t;

public:
    ClientCtx(uv::Loop& loop)
        :m_tcphandel(loop)
    {}

    void on_write_finished()
    {
        //TODO
        G_LOG() << "Client[" << m_id << "] write finished!" << std::endl;
    }

    inline void write(const char* buf, size_t len)
    {
        m_write_cb(buf, len);
    }
    inline void read(const char* buf, size_t len)
    {
        m_read_cb(buf, len);
    }
    inline void set_write_cb(do_write_t cb)
    {
        m_write_cb = cb;
    }
    inline void set_read_cb(do_read_t cb)
    {
        m_read_cb = cb;
    }

    ID m_id;
    uv::Tcp m_tcphandel;

private:
    do_write_t m_write_cb;
    do_read_t m_read_cb;
};

#include "TcpConn.h"

class CSimpleSvr : public TcpConn
{
public:
    CSimpleSvr(uv::Loop& loop)
        :TcpConn(loop)
    {

    }

    CSimpleSvr(const CSimpleSvr&) = delete;
    CSimpleSvr& operator = (const CSimpleSvr&) = delete;
    virtual ~CSimpleSvr() = default;

    void start(std::string& strIp, int nPort);
    void on_new_connected(uv::Error state);
    int alloc_client_id();

private:
    std::map<ID, std::unique_ptr<ClientCtx> >m_all_Client;
};

class CSimpleClt
{
public:
    CSimpleClt(uv::Loop& loop)
        :m_rLoop(loop),
        m_MyHandle(m_rLoop)
    {
    }

    bool Connect(const string& strIp, int nProt)
    {
        return m_MyHandle.connect(strIp, nProt, 
            [this](uv::Error state) 
        {
            m_MyHandle.write("i m client", [](uv::Error state) {});
        }
        );
        m_rLoop.run();
    }

private:
    uv::Loop& m_rLoop;
    uv::Tcp m_MyHandle;
};
