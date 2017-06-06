#pragma once
#include "TcpConn.h"
class EchoSvrConn :
    public TcpConn
{
public:

    EchoSvrConn(uv::Loop& loop) :
        TcpConn(loop)
    {}

    virtual void OnRecv(const char *buff, ssize_t len);
};
