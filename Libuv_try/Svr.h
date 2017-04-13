#pragma once

#include <list>
#include <map>
#include <memory>

#include "tcp.hpp"
#include "loop.hpp"

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