#pragma once

#include <list>
#include <memory>

#include "tcp.hpp"
#include "loop.hpp"
#include "TcpConn.h"

using namespace uv;
using namespace std;

class Client final : TcpConn
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
    void onConnected(Error error);
    void onRecv(const char* buff, size_t len);
};