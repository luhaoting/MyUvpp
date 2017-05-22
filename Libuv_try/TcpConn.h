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
    inline void SetCloseCb(std::function<void()> close_cb)
    {
        mf_close_cb = close_cb;
    }

    virtual void OnRecv(const char *buff, ssize_t len);
    virtual void SendTo(std::string& msg);
    
protected:
	virtual void OnWriteFinished(uv::Error error);

protected:
    

public:
    ID m_id;
    uv::Loop& m_rLoop;
    uv::Tcp m_handle;

private:
	
    std::function<void()> mf_close_cb;
};

