#pragma once

#include <list>
#include <memory>

#include "tcp.hpp"
#include "loop.hpp"
#include "TcpConn.h"

using namespace uv;
using namespace std;

class Client :public TcpConn
{
public:
    Client(uv::Loop& loop);
    virtual ~Client() = default;

    //×èÖ¹¿½±´
    Client(const Client&) = delete;
    Client& operator = (const Client&) = delete;

public:
    void Start(string strIp, int nPort);
    void Send(string data);

protected:
    virtual void OnConnected(Error error);
    virtual void OnRecv(const char* buff, ssize_t len);
};
