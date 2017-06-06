#pragma once
#include <uv.h>
#include <loop.hpp>
#include <tcp.hpp>
#include <iostream>

using ID = int64_t;

class TcpConn
{
public:
    TcpConn(uv::Loop& loop);
    virtual ~TcpConn();

    //must call after accpet
    virtual void SetReadCb();
    inline void SetCloseCb(std::function<void()> close_cb) { mf_close_cb = close_cb; }

    virtual void OnRecv(const char *buff, ssize_t len) = 0;
    virtual void SendTo(std::string& msg);

    virtual void ConnectTo(std::string& strIp, const int nPort);

protected:
    virtual void OnWriteFinished(uv::Error error);

public:
    ID m_id;
    uv::Tcp m_handle;

protected:
    uv::Loop& m_rLoop;
    std::function<void()> mf_close_cb;
};
