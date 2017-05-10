#pragma once
#include <uv.h>
#include <loop.hpp>
#include <tcp.hpp>
#include <iostream>

class TcpConn
{
public:
    TcpConn(uv::Loop& loop);
    virtual ~TcpConn();

    virtual void OnRecv(const char *buff, ssize_t len);
    virtual void SendTo(std::string& msg);

protected:
    uv::Loop& m_rLoop;
    uv::Tcp m_handle;
};

