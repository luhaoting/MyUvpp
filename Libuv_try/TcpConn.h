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

    virtual void OnRecv(const char *buff, ssize_t len);
    virtual void SendTo(std::string& msg);
protected:
	virtual void OnWriteFinished(uv::Error error);

protected:
    

public:
	uv::Loop& m_rLoop;
	uv::Tcp m_handle;
	ID m_id;
};

