#pragma once

#include <list>
#include <map>
#include <memory>
#include <iostream>

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
    
    //×èÖ¹¿½±´
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
std::ostream& G_LOG()
{
    return std::cout;
}

class ClientCtx
{
public:
    ClientCtx(uv::Loop& loop)
        :m_tcphandel(loop)
    {}

    ID m_id;
    uv::Tcp m_tcphandel;
};

class CSimpleSvr
{
public:
    CSimpleSvr(uv::Loop& loop)
        :m_rLoop(loop),
        m_SvrHandle(loop)
    {

    }
    virtual ~CSimpleSvr() = default;

    void start(std::string& strIp, int nPort);
    void on_new_connected(uv::Error state);
    int AllocID();

private:
    std::map<ID, std::unique_ptr<ClientCtx> >m_all_Client;
    uv::Loop& m_rLoop;
    uv::Tcp m_SvrHandle;
};

class CSimpleClt
{

};
